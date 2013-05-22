/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot3ad_transport.c
* @purpose     802.3ad link aggregation, Helpers
* @component   dot3ad
* @comments    none
* @create      10/24/08
* @author      akulkarn
* @end
*
**********************************************************************/

#include "dot3ad_helper.h"
#include "dot3ad_sid.h"
#include "sim_pts_api.h"
#include "log.h"
#include "dtlapi.h"
#include "dot3ad_lacp.h"
#include "dot3ad_debug.h"
#include "dtl_helper.h"

/*#include "sysnet_api.h"*/

dot3adHelperMsg_t *dot3adHelperLocalDb;

static dot3ad_helper_states_t dot3adHelperStates[L7_MAX_PHYSICAL_PORTS_PER_UNIT + 1];
static L7_uint32 dot3ad_helper_task_id = L7_ERROR;

void *dot3ad_helper_queue; /* reference to the helper message queue */
L7_uint32 dot3adHelperUsrHandle = ~0;

L7_RC_t dot3adHelperGetPortIndexFromUsp(nimUSP_t *usp, L7_uint32 *portIndex)
{
  *portIndex = usp->port;
  return L7_SUCCESS;
}

L7_RC_t dot3adHelperGetUspFromPortIndex(L7_uint32 portIndex, nimUSP_t *usp)
{
  if (portIndex < 1 || portIndex > L7_MAX_PHYSICAL_PORTS_PER_UNIT)
  {
    return L7_FAILURE;
  }

  memcpy(usp, &dot3adHelperLocalDb[portIndex].usp, sizeof(nimUSP_t));


  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Initiate the Helper subsystem for dot3ad.
*
* @param
*
* @returns  L7_SUCCESS if succesful
*           L7_NO_MEMORY if out of memory
*           L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adHelperInit()
{

  dot3ad_helper_queue = (void*)osapiMsgQueueCreate("dot3ad_helper_queue", DEFAULT_HELPER_MSG_COUNT,
                                                   (L7_uint32)sizeof(dot3adHelperMsg_t));

  if (dot3ad_helper_queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT3AD_COMPONENT_ID,
            "LAG: Unable to create msg queue for dot3ad_helper_task\n");
    return L7_FAILURE;
  }

  dot3adHelperLocalDb = osapiMalloc(L7_DOT3AD_COMPONENT_ID,
                                    sizeof(dot3adHelperMsg_t)*(L7_MAX_PHYSICAL_PORTS_PER_UNIT + 1 ));
  if (dot3adHelperLocalDb == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Cannot allocate memory for dot3adHelperLocalDb");
    return L7_NO_MEMORY;
  }

  memset(dot3adHelperLocalDb, 0,
         sizeof(dot3adHelperMsg_t)*(L7_MAX_PHYSICAL_PORTS_PER_UNIT + 1 ));
  memset(dot3adHelperStates, 0, sizeof(dot3adHelperStates));

  dot3ad_helper_task_id = (L7_uint32)osapiTaskCreate( "dot3ad_helper_task",
                                                      (void *)dot3ad_helper_task, 0, 0,
                                                      dot3adSidTimerTaskStackSizeGet(),
                                                      dot3adSidTimerTaskPriorityGet(),
                                                      dot3adSidTimerTaskSliceGet());

  if (osapiWaitForTaskInit (L7_DOT3AD_HELPER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Unable to initialize dot3ad_helper_task");
    return L7_FAILURE;
  }

  if (dot3ad_helper_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Failed to create dot3ad helper task.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the resources allocated by the helper subsystem
*
* @param
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot3adHelperFini()
{
  osapiFree(L7_DOT3AD_COMPONENT_ID, dot3adHelperLocalDb);
  osapiTaskDelete(dot3ad_helper_task_id);
  osapiMsgQueueDelete(dot3ad_helper_queue);
}

/*********************************************************************
* @purpose  dot3ad helper task which serves the local unit
*
* @param
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adHelperIssueCmd(void *msg, L7_BOOL controlMsg)
{

  L7_RC_t rc;
  dot3adHelperMsg_t *txMsg = (dot3adHelperMsg_t *)msg;
  dot3adHelperMsg_t tempMsg;

  memset(&tempMsg, 0, sizeof(tempMsg));

  if (controlMsg == L7_TRUE)
  {
    tempMsg.msgType = ((dot3ad_helper_ctrl_data_t *)msg)->ctrlMsgType;
    txMsg = &tempMsg;
  }

  rc = osapiMessageSend(dot3ad_helper_queue, txMsg, sizeof(dot3adHelperMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT3AD_COMPONENT_ID,
            "Cannnot put message on the helper message queue rc %d ", rc);
  }

  return rc;
}

/*********************************************************************
* @purpose  Handle the notification of the switchover for core.
*
* @param
*
* @returns  void
*
* @notes    The helper is notified that the core has gone away.
*           For now maintain this info for the helper to check
           Mark all the helper ports we have for resync
*
* @end
*********************************************************************/
void dot3adHelperSwitchoverHasBegun()
{
  L7_uint32 port;

  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER, "%s ", __FUNCTION__);

  for (port= 1; port <= L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    if (dot3adHelperStates[port] == DOT3AD_HELPER_PORT_CURRENT)
    {
      dot3adHelperStates[port] = DOT3AD_HELPER_PORT_STALE;
    }
  }

  return;
}
/*********************************************************************
* @purpose  Handle the notification of the switchover completion for core.
*
* @param
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void dot3adHelperSwitchoverIsDone(L7_BOOL purge)
{
  L7_uint32 port;


  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                               "%s Purge(%d)\n", __FUNCTION__, purge);

  for (port= 1; port <= L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    if ((purge == L7_TRUE &&
         (dot3adHelperStates[port] == DOT3AD_HELPER_PORT_CURRENT)) ||
        (dot3adHelperStates[port] == DOT3AD_HELPER_PORT_STALE))
    {
      DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                                    "Cleaning data for port %d\n", port);
      simPtsTimerStop(dot3adHelperUsrHandle, port);
      dot3adHelperStates[port] = DOT3AD_HELPER_PORT_NOT_CONFIG;
      /* Clean out local DB for port*/
      memset(&dot3adHelperLocalDb[port], 0, sizeof(dot3ad_pdu_t));
    }
  }

  return;
}
/*********************************************************************
* @purpose  Handle the expiration of the helper timer.
*
* @param
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void dot3adHelperTimerCallback(L7_uint32 portIndex)
{
  dot3ad_pdu_t *pdu = L7_NULLPTR;
  L7_RC_t rc;
  nimUSP_t usp;

  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                               "Timer callback (%d) at %d \n",
                               portIndex, osapiUpTimeRaw());

  pdu = &dot3adHelperLocalDb[portIndex].pdu;
  if (dot3adHelperGetUspFromPortIndex(portIndex, &usp) != L7_SUCCESS)
  {
    printf("Cannot get intIfNum \n");
    return;
  }

  rc = dtlHelperSend((L7_uchar8 *)pdu, sizeof(* pdu), DOT3AD_HELPER_PRIO,
                      usp.slot, usp.port, L7_TRUE);
  if (rc==L7_SUCCESS)
  {
    /* Send pdu to debug routine */
    if (DOT3AD_DEBUG_NSF(DOT3AD_DEBUG_NSF_HELPER_DETAIL) )
    {
      DOT3AD_DEBUG_NSF_PRINT("DOT3AD_DEBUG_NSF_HELPER:Transmission of PDU on helper "
                             "successful for port  \n", portIndex);
      dot3adNsfDebugPktDump((L7_uchar8 *)pdu, sizeof(dot3ad_pdu_t));
    }
    /*dot3adDebugPacketTxTrace(intIfNum, pdu);*/

  }
  else
  {
    DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                                 "failed to transmit PDU on port %d\n",
                                 portIndex);
  }

}

/*********************************************************************
* @purpose  dot3ad helper task which serves the local unit
*
* @param
*
* @returns  void
*
* @notes    This task upon receiving notification from core tx task stores the PDU in the
*
*
* @end
*********************************************************************/
void dot3ad_helper_task()
{
  dot3adHelperMsg_t msg;
  L7_uint32 status;
  L7_RC_t rc;
  L7_uint32 port = 0;
  L7_BOOL purge = L7_FALSE;

  rc = osapiTaskInitDone(L7_DOT3AD_HELPER_TASK_SYNC);
  for (;;)
  {
    status = (L7_uint32)osapiMessageReceive(dot3ad_helper_queue,
                                            (void*)&msg,
                                            (L7_uint32)sizeof(dot3adHelperMsg_t),
                                            L7_WAIT_FOREVER);

    DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                                 "Rx message (%d)\n", msg.msgType);

    /* rc = osapiSemaTake(dot3adTaskSyncSema, L7_WAIT_FOREVER);*/
    if (msg.msgType < DOT3AD_HELPER_CONTROL_START)
    {
      dot3adHelperGetPortIndexFromUsp(&msg.usp, &port);

      if (port > L7_MAX_PHYSICAL_PORTS_PER_UNIT )
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT3AD_COMPONENT_ID,
                "Invalid port (%d) in helper received message  ", port );
        continue;
      }
      DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_HELPER,
                                   "Rx port (%d/%d/%d)\n",
                                   msg.usp.unit,msg.usp.slot, msg.usp.port);
    }


    switch (msg.msgType)
    {

      case DOT3AD_HELPER_PDU_START:

        /* Store the PDU for future use*/
        memcpy(&dot3adHelperLocalDb[port], &msg, sizeof(dot3adHelperLocalDb[port]));
        dot3adHelperStates[port] = DOT3AD_HELPER_PORT_CURRENT;
        dot3adHelperTimerCallback(port);
        simPtsTimerStart(dot3adHelperUsrHandle, port, msg.interval);
        break;

      case DOT3AD_HELPER_PDU_MODIFY:
        /* Store the PDU for future use*/
        /*memcpy(&dot3adHelperLocalDb[port], &msg.pdu, sizeof(dot3ad_pdu_t));*/
        simPtsTimerModify(dot3adHelperUsrHandle, port, msg.interval);

        /* Transmit the PDU */
        /* set the timer to zero so timer task can transmit */
        break;

      case DOT3AD_HELPER_PDU_STOP:
        /* Stop Timer */
        simPtsTimerStop(dot3adHelperUsrHandle, port);
        dot3adHelperStates[port] = DOT3AD_HELPER_PORT_NOT_CONFIG;
        /* Clean out local DB for port*/
        memset(&dot3adHelperLocalDb[port], 0, sizeof(dot3ad_pdu_t));
        /*rc = osapiSemaGive(dot3adTaskSyncSema); */
        break;
      case DOT3AD_HELPER_TIMER_EXPIRE:
        /* Transmit PDU */
        dot3adHelperTimerCallback(port);
        break;
      case DOT3AD_HELPER_SWITCHOVER_START:
        /* Transmit PDU */
        dot3adHelperSwitchoverHasBegun();
        break;
      case DOT3AD_HELPER_PURGE:
        purge = L7_TRUE;
        /* Fall Thru */
      case DOT3AD_HELPER_SWITCHOVER_DONE:
        /* Transmit PDU */
        dot3adHelperSwitchoverIsDone(purge);
        break;
      default:
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT3AD_COMPONENT_ID,
                "Incorrect message type(%d) received by the helper ", msg.msgType);
        break;
    } /* switch msg.flag*/
  } /* End - For */
}
