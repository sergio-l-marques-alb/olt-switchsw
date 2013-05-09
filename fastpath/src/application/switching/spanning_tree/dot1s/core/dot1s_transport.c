/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_transport.c
* @purpose     Spanning tree: conduit between core and the helpers
* @component   dot1s
* @comments    none
* @create      1/23/09
* @author      akulkarn
* @end
*
**********************************************************************/


#include <string.h>
#include "l7_common.h"
#include "l7_resources.h"
#include "comm_mask.h"
#include "dot1s_helper.h"
#include "dot1s_debug.h"
#include "dot1s_transport.h"
#include "dot1s_nsf.h"
#include "osapi.h"
#include "log.h"
#include "ms_api.h"
#include "unitmgr_api.h"

void *dot1sTransSema = L7_NULLPTR;
void *dot1sTransSyncSema = L7_NULLPTR;

/* Transport sub systems copy of the PDUs
   This is shared by the application and the transport
*/
dot1s_helper_msg_t *dot1sTransportPduDb = L7_NULLPTR;

L7_INTF_MASK_t dot1sPduChangedMask;


/* Global copy for the message . At any time we are going to use only one*/
static dot1s_helper_data_msg_t txMsg;
static L7_uint32 dot1s_transport_task_id = 0;

/*********************************************************************
* @purpose  Initializes the dot1s transport subsystem.
*
* @param    None
*
* @returns  L7_SUCCESS on success
*           L7_NO_MEM if it cannot allocate the memory
*           L7_FAILURE if it cannot create semaphore
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1sTransportInit()
{

  dot1sTransSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1sTransSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Cannot create semaphore for dot1s transport task");
    return L7_FAILURE;
  }

  dot1sTransSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (dot1sTransSyncSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Cannot create empty sync semaphore for dot1s transport task");
    return L7_FAILURE;
  }

  dot1sTransportPduDb = osapiMalloc(L7_DOT1S_COMPONENT_ID,
                                    sizeof(dot1s_helper_msg_t)*(L7_DOT1S_MAX_INTERFACE_COUNT + 1));
  if (dot1sTransportPduDb == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID, "Cannot allocate memory for dot1s transport buffer");
    return L7_NO_MEMORY;
  }


  memset(dot1sTransportPduDb, 0, sizeof(dot1s_helper_msg_t)*(L7_DOT1S_MAX_INTERFACE_COUNT + 1));
  memset(&dot1sPduChangedMask,0,sizeof(L7_INTF_MASK_t));
  memset(&txMsg ,0,sizeof(txMsg));

  dot1sHelperFuncTable.dot1sNotifyHelperPduStart = dot1sNotifyHelperPduStart;
  dot1sHelperFuncTable.dot1sNotifyHelperPduModify = dot1sNotifyHelperPduModify;
  dot1sHelperFuncTable.dot1sNotifyHelperPduStop = dot1sNotifyHelperPduStop;


  dot1s_transport_task_id = osapiTaskCreate("Dot1s transport task", dot1s_transport_task, 0, 0,
                                            L7_DEFAULT_STACK_SIZE,
                                            L7_DOT1S_DEFAULT_DOT1S_TIMER_TASK_PRI,
                                            L7_DEFAULT_TASK_SLICE);

  if (dot1s_transport_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: timer task creation error.\n");
    return L7_FAILURE;
  }
  if (osapiWaitForTaskInit (L7_DOT1S_TRANSPORT_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Unable to initialize dot1s_transport_task()\n");
    return(L7_FAILURE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  releases resources allocated byt the dot1s transport subsystem.
*
* @param    None
*
* @returns  None
*
* @notes
*
*
* @end
*********************************************************************/
void dot1sTransportFini()
{
  osapiSemaDelete(dot1sTransSema);

  osapiSemaDelete(dot1sTransSyncSema);

  osapiFree(L7_DOT1S_COMPONENT_ID, dot1sTransportPduDb);

  osapiTaskDelete(dot1s_transport_task_id);



}

L7_RC_t dot1sTransportCallbacksRegister()
{
  L7_RC_t rc;
  if ((rc = msMsgRegister(L7_MS_DOT1S_REGISTRAR_ID, dot1sTransportCoreCallback,
                          MS_TARGET_CORE)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to register with message service for core callback\n");
  }

  else if ((rc = msMsgRegister(L7_MS_DOT1S_REGISTRAR_ID,
                               dot1sTransportHelperCallback,
                               MS_TARGET_HELPER)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to ergister with message service for helper callback\n");
  }

  return rc;

}

/*********************************************************************
* @purpose  dot1s core transmit task.
*
* @param
*
* @returns  void
*
* @notes   This task serves as a conduit between the core and the helper(s).
*          It distributes the information from the core to the appropriate
*          helper w/o blocking the core.
*
*
* @end
*********************************************************************/
void dot1s_transport_task()
{
  L7_RC_t rc;
  L7_uint32 intIfIndex = 0;
  L7_uint32 unit = 0;
  L7_BOOL done = L7_TRUE;
  L7_uint32 portIndex = 0;
  dot1s_helper_msg_t *localMsg = L7_NULLPTR;
  unitMgrStatus_t unitStatus;
  /*dot1s_transportQueue = (void*)osapiMsgQueueCreate("dot1s_transportQueue", DEFAULT_MSG_COUNT_HIGH,
                                                  (L7_uint32)sizeof(dot1s_trans_msg_t));
  */
  rc = osapiTaskInitDone(L7_DOT1S_TRANSPORT_TASK_SYNC);

  for (;;)
  {

    /* This ifindex ideally should be preserved by making it static
       This will ensure that the message transport will loop through all the ports fairly.
       Otherwise A group of hyper-active ports at the begining of the mask could starve
       those at the end of the mask. TBD.
     */

    osapiSemaTake(dot1sTransSyncSema,L7_WAIT_FOREVER);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_TRANSPORT, "Transport task woken up ....");
    osapiSemaTake(dot1sTransSema,L7_WAIT_FOREVER);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_TRANSPORT,"Obtains the sempahore  \n");

    L7_INTF_FHMASKBIT(dot1sPduChangedMask,intIfIndex);
    while (intIfIndex != 0)
    {
      portIndex = dot1sPortIndexFromIntfNumGet(intIfIndex);
      if (portIndex != L7_NULL)
      {
        memset(&txMsg,0, sizeof(txMsg));
        localMsg = &dot1sTransportPduDb[portIndex];
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_TRANSPORT,
                        "Intf(%d) has data ready %d \n", intIfIndex);
        memcpy(&txMsg.helperData, localMsg, sizeof(dot1s_helper_msg_t));

        txMsg.msHeader.appRegistar = L7_MS_DOT1S_REGISTRAR_ID;
        txMsg.msHeader.target = MS_TARGET_HELPER;

        /* For the intIfNum for which the bit is set
           get the usp and send the message to the appropriate
           helper to transmit the PDU
        */

        unit = L7_NULL;
        do
        {
          if (dot1sIsPortLag(&localMsg->usp) != L7_TRUE)
          {
            unit = localMsg->usp.unit;
            done = L7_TRUE;
          }
          else
          {
            rc = unitMgrStackMemberGetNext(unit, &unit);
            done = (rc == L7_SUCCESS) ? L7_FALSE : L7_TRUE;
          }


          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_TRANSPORT,
                      "sending PDU(%d) to helper(%d) for interface %d at %u \n",
                      txMsg.helperData.msgType, unit, intIfIndex,
                      osapiUpTimeMillisecondsGet());
#ifdef L7_STACKING_PACKAGE
          unitMgrUnitStatusGet(unit, &unitStatus);
          if ((unit != L7_NULL) && (unitStatus == L7_UNITMGR_UNIT_OK))
          {
            rc = msMsgSend((msSyncMsgHdr_t *)&txMsg,
                      sizeof(txMsg), unit, MS_BEST_EFFORT);
            if (rc != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
                      "Msg Send failed for port %d \n", portIndex);
            }
          }
#endif

        }
        while (done != L7_TRUE);
      }

      L7_INTF_CLRMASKBIT(dot1sPduChangedMask,intIfIndex);
      /* Get the next valid interface */
      L7_INTF_FHMASKBIT(dot1sPduChangedMask, intIfIndex);
    }
    osapiSemaGive(dot1sTransSema);

  }

}


/*********************************************************************
* @purpose  Callback from the message service for the core
*
* @param
*
* @returns  void
*
* @notes   Currently no communication from Helper to core.
*
*
* @end
*********************************************************************/
void dot1sTransportCoreCallback(msSyncMsgHdr_t         *msg,
                                L7_uint32               msg_len)
{

}

/*********************************************************************
* @purpose  Callback from the message service for the helper
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
void dot1sTransportHelperCallback(msSyncMsgHdr_t         *msg,
                                  L7_uint32               msg_len)
{
  void *helperMsg;
  L7_BOOL controlMsg = L7_FALSE;

  if (msg == L7_NULLPTR)
  {
    return;
  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_TRANSPORT,
                  "%s :received callback from message service %d \n", __FUNCTION__, msg_len);

  helperMsg = msg->appData;
  if (msg_len == DOT1S_HELPER_CTL_MSG_LEN)
  {
    controlMsg = L7_TRUE;
  }

  dot1sHelperIssueCmd(helperMsg, controlMsg);
}


/*********************************************************************
* @purpose  Notification to the transport to tell the helper to begin
*           transmission of pdus.
*
* @param   intIfNum internal interface number.
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1sNotifyHelperPduStart(DOT1S_BYTE_MSTP_ENCAPS_t *pdu,
                                  L7_uint32 intIfNum, L7_uint32 length)
{
  dot1s_helper_msg_t *localMsg = L7_NULLPTR;
  L7_int32 pduComp = 1; /* non zero def value for memcomp check*/
  L7_uint32 interval = dot1sGetPortTransmitIntervalInSec(intIfNum);
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  if (interval == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (dot1sCnfgrAppIsReady() != L7_TRUE)
  {
    if (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
        dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }

  osapiSemaTake(dot1sTransSema,L7_WAIT_FOREVER);
  localMsg = &(dot1sTransportPduDb[portIndex]);

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER_DETAIL,
                  "%s: Opportunity to transmit %d index %d\n",
                  __FUNCTION__, intIfNum, portIndex);

  if (localMsg->pduLen == length)
  {
    pduComp = memcmp(&(localMsg->pdu), pdu, length);
  }

  if (pduComp != 0)
  {

    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
                    "%s: New data available intIfNum %d index %d\n",
                    __FUNCTION__, intIfNum, portIndex);
    localMsg->msgType = DOT1S_HELPER_PDU_START;
    /* need to memset ??*/
    memcpy(&(localMsg->pdu), pdu, length);
    localMsg->interval = interval * 1000;
    localMsg->intIfNum = intIfNum;
    localMsg->pduLen = length;
    if (nimGetUnitSlotPort(intIfNum, &localMsg->usp) == L7_SUCCESS)
    {
      L7_INTF_SETMASKBIT(dot1sPduChangedMask, intIfNum);
      osapiSemaGive(dot1sTransSyncSema);
    }
  }
  osapiSemaGive(dot1sTransSema);

  return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Notification to the transport to tell the helper to modify
*           the interval
*
* @param   intIfNum internal interface number.
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1sNotifyHelperPduModify(L7_uint32 intIfNum,
                                   L7_uint32 interval)
{
  dot1s_helper_msg_t *localMsg = L7_NULLPTR;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  if (dot1sCnfgrAppIsReady() != L7_TRUE)
  {
    if (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
        dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }

  osapiSemaTake(dot1sTransSema,L7_WAIT_FOREVER);
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
                  "%s: Enter intIfNum %d index %d\n", __FUNCTION__, intIfNum, portIndex);
  localMsg = &(dot1sTransportPduDb[portIndex]);

  localMsg->msgType = DOT1S_HELPER_PDU_MODIFY;

  /*memcpy(&(localMsg->pdu), pdu, sizeof(localMsg->pdu));*/
  localMsg->interval = interval;
  localMsg->intIfNum = intIfNum;
  if ( nimGetUnitSlotPort(intIfNum, &localMsg->usp) == L7_SUCCESS)
  {
    L7_INTF_SETMASKBIT(dot1sPduChangedMask, intIfNum);
    osapiSemaGive(dot1sTransSyncSema);
  }

  osapiSemaGive(dot1sTransSema);

  return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Notification to the transport to tell the helper to stop
*           pdu transmission
*
* @param   intIfNum internal interface number.
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1sNotifyHelperPduStop(L7_uint32 intIfNum)
{
  dot1s_helper_msg_t *localMsg = L7_NULLPTR;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);


  if (dot1sCnfgrAppIsReady() != L7_TRUE)
  {
    if (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
        dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }

  osapiSemaTake(dot1sTransSema,L7_WAIT_FOREVER);
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER,
                  "%s: Enter intIfNum %d index %d\n", __FUNCTION__, intIfNum, portIndex);
  localMsg = &(dot1sTransportPduDb[portIndex]);

  localMsg->msgType = DOT1S_HELPER_PDU_STOP;

  memset(&(localMsg->pdu), 0, sizeof(localMsg->pdu));
  localMsg->interval = 0;
  localMsg->intIfNum = intIfNum;
  if (nimGetUnitSlotPort(intIfNum, &localMsg->usp) == L7_SUCCESS)
  {
    L7_INTF_SETMASKBIT(dot1sPduChangedMask, intIfNum);
    osapiSemaGive(dot1sTransSyncSema);
  }

  osapiSemaGive(dot1sTransSema);

  return L7_SUCCESS;


}
