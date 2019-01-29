/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot3ad_transport.c
* @purpose     802.3ad link aggregation, conduit between core and the helpers
* @component   dot3ad
* @comments    none
* @create      10/24/08
* @author      akulkarn
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "dot3ad_helper.h"
#include "comm_mask.h"
#include "osapi.h"
#include "log.h"
#include "ms_api.h"
#include "dot3ad_transport.h"
#include "dot3ad_debug.h"
#include "dot3ad_sid.h"
#include "unitmgr_api.h"
#include "l7_resources.h"
#include "dot3ad_helper.h"

void *dot3adTransSema = L7_NULLPTR;
void *dot3adTransSyncSema = L7_NULLPTR;
void *dot3ad_transportQueue = L7_NULLPTR;
dot3adHelperMsg_t *dot3adTransportPduDb = L7_NULLPTR;
L7_INTF_MASK_t pduChangedMask;

static L7_uint32 dot3ad_core_tx_task_id = L7_ERROR;
/* Global copy for the message . At any time we are going to use only one*/
static dot3ad_helper_data_msg_t txMsg;

/*********************************************************************
* @purpose  Initializes the dot3ad transport subsystem.
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
L7_RC_t dot3adTransportInit()
{
  dot3adTransSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot3adTransSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Cannot create semaphore for dot3ad transport task");
    return L7_FAILURE;
  }

  dot3adTransSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (dot3adTransSyncSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Cannot create empty sync semaphore for dot3ad transport task");
    return L7_FAILURE;
  }

  dot3adTransportPduDb = osapiMalloc(L7_DOT3AD_COMPONENT_ID,
                                     sizeof(dot3adHelperMsg_t)*(L7_MAX_PORT_COUNT + 1));
  if (dot3adTransportPduDb == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Cannot allocate memory for dot3ad transport buffer");
    return L7_NO_MEMORY;
  }

  /*txMsg.compData = (L7_uchar8 *)osapiMalloc(L7_DOT3AD_COMPONENT_ID, sizeof(dot3adHelperMsg_t));*/

  memset(dot3adTransportPduDb, 0, sizeof(dot3adHelperMsg_t)*(L7_MAX_PORT_COUNT + 1));
  memset(&pduChangedMask,0,sizeof(L7_INTF_MASK_t));
  memset(&txMsg,0,sizeof(txMsg));


  dot3ad_core_tx_task_id = (L7_uint32)osapiTaskCreate( "dot3ad_core_tx_task",
                                                       (void *)dot3ad_transport_task, 0, 0,
                                                       dot3adSidTimerTaskStackSizeGet(),
                                                       dot3adSidTimerTaskPriorityGet(),
                                                       dot3adSidTimerTaskSliceGet());

  if (osapiWaitForTaskInit (L7_DOT3AD_CORE_TX_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Failed to initialize dot3ad core task");
    return L7_FAILURE;
  }

  if (dot3ad_core_tx_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT3AD_COMPONENT_ID,
           "Cannot create dot3ad core task");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the resources allocated by the transport subsystem
*
* @param
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot3adTransportFini()
{

  osapiSemaDelete(dot3adTransSema);
  osapiSemaDelete(dot3adTransSyncSema);
  osapiFree(L7_DOT3AD_COMPONENT_ID, dot3adTransportPduDb);
  osapiTaskDelete(dot3ad_core_tx_task_id);

}

/*********************************************************************
* @purpose  dot3ad core transmit task.
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
void dot3ad_transport_task()
{
  L7_RC_t rc;
  L7_uint32 intIfIndex = 0;
  unitMgrStatus_t unitStatus;
  L7_uint32 unit;

  rc = osapiTaskInitDone(L7_DOT3AD_CORE_TX_TASK_SYNC);

  for (;;)
  {

    /* This ifindex ideally should be preserved by making it static
       This will ensure that the message transport will loop through all the ports fairly.
       Otherwise A group of hyper-active ports at the begining of the mask could starve
       those at the end of the mask. TBD.
     */

    osapiSemaTake(dot3adTransSyncSema,L7_WAIT_FOREVER);
    osapiSemaTake(dot3adTransSema,L7_WAIT_FOREVER);


    L7_INTF_FHMASKBIT(pduChangedMask,intIfIndex);
    while (intIfIndex != 0)
    {

      memset(&txMsg,0, sizeof(txMsg));
      memcpy(&txMsg.helperData ,&dot3adTransportPduDb[intIfIndex], sizeof(dot3adHelperMsg_t));

      txMsg.msHeader.appRegistar = L7_MS_DOT3AD_REGISTRAR_ID;
      txMsg.msHeader.target = MS_TARGET_HELPER;
      /* For the intIfNum for which the bit is set
         get the usp and send the message to the appropriate helper to transmit the PDU
      */
      if (DOT3AD_DEBUG_NSF(DOT3AD_DEBUG_NSF_TRANSPORT) )
      {
        DOT3AD_DEBUG_NSF_PRINT("DOT3AD_DEBUG_NSF_TRANSPORT:Sending PDU to msg service"
                               " on interface (%d)  \n", intIfIndex);
        dot3adNsfDebugPktDump((L7_uchar8 *)&txMsg,0);
      }

      unit = dot3adTransportPduDb[intIfIndex].usp.unit;
      unitMgrUnitStatusGet(unit, &unitStatus);
#ifdef L7_STACKING_PACKAGE
      if (unitStatus == L7_UNITMGR_UNIT_OK)
      {
        rc = msMsgSend((msSyncMsgHdr_t *)&txMsg, sizeof(txMsg),
                  unit, MS_BEST_EFFORT);
        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT3AD_COMPONENT_ID,
                  "%s: Msg Send failed for port %d \n", 
                  __FUNCTION__, intIfIndex);
        }
      }
#endif
      L7_INTF_CLRMASKBIT(pduChangedMask,intIfIndex);
      /* Get the next valid interface */
      L7_INTF_FHMASKBIT(pduChangedMask, intIfIndex);
    }

    osapiSemaGive(dot3adTransSema);


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
void dot3adTransportCoreCallback(msSyncMsgHdr_t         *msg,
                                 L7_uint32               msg_len)
{

  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_TRANSPORT,"%s  msg len %d ",
                               __FUNCTION__, msg_len);
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
void dot3adTransportHelperCallback(msSyncMsgHdr_t         *msg,
                                   L7_uint32               msg_len)
{
  void *helperMsg;
  L7_BOOL controlMsg = L7_FALSE;

  if (msg == L7_NULLPTR)
  {
    printf("rx Empty message \n");
    return;
  }

  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_TRANSPORT,"%s  msg len %d ",
                               __FUNCTION__, msg_len);

  helperMsg = msg->appData;
  if (msg_len == DOT3AD_HELPER_CTL_MSG_LEN)
  {
    controlMsg = L7_TRUE;
  }

  dot3adHelperIssueCmd(helperMsg, controlMsg);




}

/*********************************************************************
* @purpose  Send a control message to the helper
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
L7_RC_t dot3adTransportHelperCtlMsg(dot3ad_helper_msgType_t msgType, L7_uint32 unit)
{
  dot3ad_helper_ctrl_msg_t helperMsg;

  if (DOT3AD_HELPER_IS_CTL_MSG(msgType) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT3AD_COMPONENT_ID,
            "Cannot call helper incorrect message type %d", msgType);
    return L7_FAILURE;
  }

  memset(&helperMsg, 0, sizeof(helperMsg));

  helperMsg.ctrlData.ctrlMsgType = msgType;

  helperMsg.msHeader.appRegistar = L7_MS_DOT3AD_REGISTRAR_ID;
  helperMsg.msHeader.target = MS_TARGET_HELPER;

#ifdef L7_STACKING_PACKAGE
  msMsgSend((msSyncMsgHdr_t *)&helperMsg, DOT3AD_HELPER_CTL_MSG_LEN, unit,MS_BEST_EFFORT);
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send a control message to all the helpers
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
L7_RC_t dot3adTransportBcastHelperCtlMsg(dot3ad_helper_msgType_t helperMsgType)
{
  L7_uint32 unit;
  L7_RC_t rc;
  unitMgrStatus_t unitStatus;

  /* Notify all the helpers that switchover has begun*/
  rc = unitMgrStackMemberGetFirst(&unit);
  while (rc == L7_SUCCESS)
  {
    /* ignore preconfigured units */
    unitMgrUnitStatusGet(unit, &unitStatus);
    if (unitStatus == L7_UNITMGR_UNIT_OK)
    {
      dot3adTransportHelperCtlMsg(helperMsgType, unit);
    }
    rc = unitMgrStackMemberGetNext(unit,&unit);
  }

  return L7_SUCCESS;
}

