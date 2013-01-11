/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_helper.c
* @purpose     Helper module for spanning tree component
* @component   dot1s
* @comments    none
* @create      01/7/09
* @author      akulkarn
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "dot1s_helper.h"
#include "dot1s_debug.h"
#include "sim_pts_api.h"
#include "dtl_helper.h"

dot1s_helper_msg_t *dot1sHelperLocalDb = L7_NULLPTR;
void *dot1s_helper_queue; /* reference to the helper message queue */
static L7_uint32 dot1s_helper_task_id = 0;
L7_uint32 dot1sUserHandle = ~0;

L7_BOOL dot1sIsPortLag(nimUSP_t *usp)
{
  return (usp->slot == L7_LAG_SLOT_NUM);
}

L7_RC_t dot1sHelperGetPortIndexFromUsp(nimUSP_t *usp, L7_uint32 *portIndex)
{

  if (dot1sIsPortLag(usp) != L7_TRUE)
  {
    if (usp->port > L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      return L7_FAILURE;
    }
    *portIndex = usp->port;
  }
  else
  {
    if (usp->port > L7_MAX_NUM_LAG_INTF)
    {
      return L7_FAILURE;
    }
    *portIndex = L7_MAX_PHYSICAL_PORTS_PER_UNIT + usp->port;
  }
  return L7_SUCCESS;
}

L7_RC_t dot1sHelperGetUspFromPortIndex(L7_uint32 portIndex, nimUSP_t *usp)
{
  if (portIndex < 1 || portIndex > DOT1S_HELPER_MAX_PORTS)
  {
    return L7_FAILURE;
  }



  memcpy(usp, &dot1sHelperLocalDb[portIndex].usp, sizeof(nimUSP_t));


  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Allocate resources required by the dot1s helper subsystem
*
* @param    None
*
* @returns  L7_SUCCESS if succesful
*           L7_NO_MEMORY if cannot allocate memory
*           L7_FAILURE   for any other error.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sHelperInit()
{
  L7_uint32 dbSize = sizeof(dot1s_helper_msg_t)*(DOT1S_HELPER_MAX_PORTS);

  /* create dot1s_timer_task - to service queue*/

  dot1sHelperLocalDb = osapiMalloc(L7_DOT1S_COMPONENT_ID, dbSize);
  if (dot1sHelperLocalDb == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Cannot allocate memory for local helper database");
    return L7_NO_MEMORY;
  }

  /* message queue*/
  dot1s_helper_queue = (void*)osapiMsgQueueCreate("dot1s_helper_queue",
                                                  DEFAULT_HELPER_MSG_COUNT,
                                            (L7_uint32)sizeof(dot1s_helper_msg_t));
  if (dot1s_helper_queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: msgQueue creation error.\n");
    return L7_FAILURE;
  }

  /* task create*/
  dot1s_helper_task_id = (L7_uint32)osapiTaskCreate("dot1s_helper_task",
                                                    (void *)dot1sHelperTask, 0, 0,
                                                    L7_DEFAULT_STACK_SIZE,
                                                    L7_DOT1S_DEFAULT_DOT1S_TIMER_TASK_PRI,
                                                    L7_DEFAULT_TASK_SLICE);

  if (dot1s_helper_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: timer task creation error.\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_DOT1S_HELPER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Unable to initialize dot1s_timer_task()\n");
    return(L7_FAILURE);
  }

  memset(dot1sHelperLocalDb, 0, dbSize);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return resources allocated by the dot1s helper subsystem
*
* @param    None
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sHelperFini()
{
  osapiFree(L7_DOT1S_COMPONENT_ID, dot1sHelperLocalDb);

  osapiMsgQueueDelete(dot1s_helper_queue);

  osapiTaskDelete(dot1s_helper_task_id);

  return L7_SUCCESS;
}

L7_RC_t dot1sHelperCallbacksRegister()
{
  L7_RC_t rc;
  if ((rc = simPtsRegister(L7_DOT1S_COMPONENT_ID, 0,
                           dot1sHelperTimerCallback,
                           &dot1sUserHandle)) != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
            "Failed to register with SIM Periodic Timer Service\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  dot1s helper task which serves the local unit
*
* @param
*
* @returns  void
*
* @notes    This task upon receiving notification from core tx task,
*           stores the pdu locally and transmits them periodically
*           until told otherwise
*
* @end
*********************************************************************/
void dot1sHelperTask()
{
  dot1s_helper_msg_t msg;
  L7_uint32 status;
  L7_RC_t rc;
  L7_uint32 portIndex = 0;

  rc = osapiTaskInitDone(L7_DOT1S_HELPER_TASK_SYNC);
  for(;;)
  {
     status = (L7_uint32)osapiMessageReceive(dot1s_helper_queue,
                                            (void*)&msg,
                                            (L7_uint32)sizeof(dot1s_helper_msg_t),
                                            L7_WAIT_FOREVER);

    rc = dot1sHelperGetPortIndexFromUsp(&msg.usp, &portIndex);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
              "Cannot obtain port index from usp %d/%d/%d ",
               msg.usp.unit, msg.usp.slot, msg.usp.port);
      continue;
    }

    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
            "Rx message (%d) on port %d/%d/%d Port Index %d interval %d \n",
            msg.msgType, msg.usp.unit, msg.usp.slot, msg.usp.port, portIndex, msg.interval);
    switch (msg.msgType)
    {

      case DOT1S_HELPER_PDU_START:

        /* Store the PDU for future use*/
        memcpy(&dot1sHelperLocalDb[portIndex], &msg, sizeof(dot1sHelperLocalDb[portIndex]));
        dot1sHelperTimerCallback(portIndex);
        simPtsTimerStart(dot1sUserHandle, portIndex, msg.interval);
        break;

      case DOT1S_HELPER_PDU_MODIFY:
        /* Store the PDU for future use*/
        simPtsTimerModify(dot1sUserHandle, portIndex, msg.interval);

        /* Transmit the PDU */
        /* set the timer to zero so timer task can transmit */
        break;

      case DOT1S_HELPER_PDU_STOP:
        /* Stop Timer */
          simPtsTimerStop(dot1sUserHandle, portIndex);
          /* Clean out local DB for port*/
          memset(&dot1sHelperLocalDb[portIndex], 0, sizeof(dot1sHelperLocalDb[0]));
        break;
      case DOT1S_HELPER_TIMER_EXPIRE:
        /* Transmit PDU */
        dot1sHelperTimerCallback(portIndex);
        break;
      default:
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
        "Incorrect message type(%d) received by the helper ", msg.msgType);
        break;
    } /* switch msg.flag*/
  } /* End - For */

}

/*********************************************************************
* @purpose  Put a message on the helper queue
*
* @param    msg @b{(input)} the msg to be put on the queue
*
* @returns  L7_FAILURE if cannot put message on the queue
*           L7_SUCCESS otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sHelperIssueCmd(void *msg, L7_BOOL controlMsg)
{
  L7_RC_t rc;
  dot1s_helper_msg_t *txMsg = (dot1s_helper_msg_t *)msg;
  dot1s_helper_msg_t tempMsg;

  if (controlMsg == L7_TRUE)
  {
    memset(&tempMsg, 0, sizeof(tempMsg));
    tempMsg.msgType = ((dot1s_helper_ctrl_data_t *)msg)->ctrlMsgType;
    txMsg = &tempMsg;
  }

  rc = osapiMessageSend(dot1s_helper_queue, txMsg, sizeof(dot1s_helper_msg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Cannnot put message on the helper message queue rc %d ", rc);
  }

  return rc;
}

/*********************************************************************
* @purpose  Callback registered with the helper timer utility
*
* @param    intIfNum @b{(input)} the interface num on which the timer has expired
*
* @returns  void
*
* @notes   This callback is processed on the timer thread. So cannot take semaphore
*          and generally want to minimize the processing done on this thread.
*
* @end
*********************************************************************/
void dot1sHelperTimerCallback(L7_uint32 portIndex)
{
  DOT1S_BYTE_MSTP_ENCAPS_t *pdu = L7_NULLPTR;
  L7_RC_t rc;
  nimUSP_t usp;

  pdu = &dot1sHelperLocalDb[portIndex].pdu;
  if (dot1sHelperGetUspFromPortIndex(portIndex, &usp) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Cannot get USP for port Index %d", portIndex);
    return;
  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
                  "Callback on index %d Tx on %d/%d/%d",
                  portIndex, usp.unit, usp.slot, usp.port);

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
                  "Desig port %.2x.%.2x \n", (pdu->cistPortId[0] ),
                  (pdu->cistPortId[1] ));

  rc = dtlHelperSend((L7_uchar8 *)pdu, dot1sHelperLocalDb[portIndex].pduLen,
                      DOT1S_HELPER_PRIO, usp.slot, usp.port, L7_TRUE);
  if (rc==L7_SUCCESS)
  {
    /* Send pdu to debug routine */

  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Helper %d failed to transmit PDU on port %d return code %d ",
             usp.unit, portIndex, rc);
  }

}
