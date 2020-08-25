/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlim.c
*
* @purpose Contains definitions to support routing logical interfaces
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "l7_ip_api.h"
#include "l7_ip6_api.h"
#include "rto_api.h"
#include "rlim.h"
#include "platform_config.h"
#include "l3_default_cnfgr.h"

L7_uint32 rlimDebugTraceFlags ;
extern rlimCnfgrState_t rlimCnfgrState;

rlimCfgData_t  *rlimCfgData = L7_NULLPTR;
rlimOpData_t *rlimOpData = L7_NULLPTR;
rlimLoopbackOpData_t *rlimLoopbackOpData = L7_NULLPTR;
rlimTunnelOpData_t *rlimTunnelOpData = L7_NULLPTR;
rlimDeregister_t rlimDeregister = {L7_FALSE, L7_FALSE, L7_FALSE};
osapiRWLock_t rlimRWLock;
void *rlimCreateDeleteMutex;
void *rlimNhopChangeEventMutex;
L7_BOOL rlimNhopChangeMessageAllow = L7_TRUE;
L7_BOOL rlimDebug = L7_FALSE;
L7_uint32 rlimIntIfNumToTunnelId[L7_MAX_INTERFACE_COUNT+1];


/*********************************************************************
* @purpose  Build default rlim config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Resets to default values.  Also inits the config file header.
*
* @end
*********************************************************************/
void rlimBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  memset((char*)rlimCfgData, 0, sizeof(rlimCfgData_t));

  /* Build header */
  strcpy(rlimCfgData->cfgHdr.filename, RLIM_CFG_FILENAME);
  rlimCfgData->cfgHdr.version = ver;
  rlimCfgData->cfgHdr.componentID = L7_RLIM_COMPONENT_ID;
  rlimCfgData->cfgHdr.type = L7_CFG_DATA;
  rlimCfgData->cfgHdr.length = sizeof(rlimCfgData_t);
  rlimCfgData->cfgHdr.dataChanged = L7_FALSE;

  for (i = 0; i < RLIM_MAX_TUNNEL_COUNT; i++)
  {
    rlimTunnelDefaultConfigSet(&rlimCfgData->tunnelCfgData[i]);
  }
}

/*********************************************************************
* @purpose  Set a tunnel interface configuration to the default
*
* @param    pTunnelAttrs  tunnel interface config data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelDefaultConfigSet(rlimTunnelCfgData_t *tcp)
{
  memset(tcp, 0, sizeof(*tcp));
  tcp->tunnelMode = L7_TUNNEL_MODE_UNDEFINED;
  tcp->localAddr.addrType = RLIM_ADDRTYPE_UNDEFINED;
  tcp->remoteAddr.addrType = RLIM_ADDRTYPE_UNDEFINED;
}

/*********************************************************************
* @purpose  Applies rlim config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    Fully resets the operational state of the component
*           according to the current config, including deleting
*           any existing state that is not specified by the config.
*
* @end
*********************************************************************/
L7_RC_t rlimApplyConfigData(void)
{
  L7_uint32 loopbackId;
  L7_uint32 tunnelId;
  L7_uint32 intIfNum;

  if (RLIM_IS_READY)
  {
    /* apply configuration */
    for (loopbackId = 0; loopbackId <= RLIM_MAX_LOOPBACK_ID; loopbackId++)
    {
      intIfNum = rlimLoopbackOpData[loopbackId].loopbackIntIfNum;

      if (RLIM_MASK_ISBITSET(rlimCfgData->loopbackMask, loopbackId))
      {
        rlimLoopbackOpCreate(loopbackId);
      }
      else if (intIfNum != 0)
      {
        rlimLoopbackOpDelete(intIfNum, loopbackId);
        RLIM_WRITE_LOCK_GIVE(rlimRWLock);
        rlimLoopbackOpDeleteWait(loopbackId);
        RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
      }
    }

    for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
    {
      intIfNum = rlimTunnelOpData[tunnelId].tunnelIntIfNum;

      if (RLIM_MASK_ISBITSET(rlimCfgData->tunnelMask, tunnelId))
      {
        rlimTunnelOpCreate(tunnelId);
        rlimTunnelOpLocalAddrUpdate(tunnelId);
        rlimTunnelOpRemoteAddrUpdate(tunnelId, 0);
        rlimTunnelOpUpdate(tunnelId);
      }
      else if (intIfNum != 0)
      {
        rlimTunnelOpDelete(intIfNum, tunnelId);
        RLIM_WRITE_LOCK_GIVE(rlimRWLock);
        rlimTunnelOpDeleteWait(tunnelId);
        RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
      }
    }
  }

  /* As of this writing, the error returns are logged in the called routines.
     To allow for future change, and to be consistent with similar routines
      in other application, return L7_RC_t */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if rlim user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL rlimHasDataChanged(void)
{
  L7_BOOL dataChanged = L7_FALSE;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if (rlimDeregister.rlimHasDataChanged != L7_TRUE)
  {
    dataChanged = rlimCfgData->cfgHdr.dataChanged;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return dataChanged;
}
void rlimResetDataChanged(void)
{
  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  rlimCfgData->cfgHdr.dataChanged = L7_FALSE;
  RLIM_WRITE_LOCK_GIVE(rlimRWLock);
  return;
}
/*********************************************************************
* @purpose  Saves rlim user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 rlimSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if (rlimDeregister.rlimSave != L7_TRUE &&
      rlimCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    rlimCfgData->cfgHdr.dataChanged = L7_FALSE;
    rlimCfgData->checkSum =
        nvStoreCrc32((L7_char8 *)rlimCfgData,
                     sizeof(rlimCfgData_t) - sizeof(rlimCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_RLIM_COMPONENT_ID,
                           RLIM_CFG_FILENAME,
                           (L7_char8 *)rlimCfgData,
                           sizeof(rlimCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
              "Error writing to config file %s\n", RLIM_CFG_FILENAME);
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return(rc);
}

/*********************************************************************
* @purpose  Restores rlim user config file to factory defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimRestoreProcess(void)
{
  RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rlimBuildDefaultConfigData(rlimCfgData->cfgHdr.version);
  (void)rlimApplyConfigData();

  rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  RLIM_SEMA_GIVE(rlimCreateDeleteMutex);
}

/*********************************************************************
* @purpose  Restores rlim user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 rlimRestore(void)
{
  L7_RC_t rc;
  rlimMsg_t msg;

  if (rlimDeregister.rlimRestore == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset((void *)&msg, 0, sizeof(msg));
  msg.msgId = RLIM_RESTORE_EVENT;

  rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "osapiMessageSend failed\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Initialize loopback operational data
*
* @param    lop - loopback operational data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimLoopbackOpDataInit(rlimLoopbackOpData_t *lop)
{
  void *saveSema;

  saveSema = lop->deleteSema;
  memset(lop, 0, sizeof(*lop));
  lop->deleteSema = saveSema;
}

/*********************************************************************
* @purpose  Create operational state for a loopback interface
*
* @param    loopbackId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackOpCreate(L7_uint32 loopbackId)
{
  rlimLoopbackOpData_t *lop = &rlimLoopbackOpData[loopbackId];
  L7_RC_t rc;
  L7_uint32 intIfNum;

  if (lop->loopbackIntIfNum != 0)
  {
    return L7_ALREADY_CONFIGURED;
  }

  rlimLoopbackOpDataInit(lop);

  rc = rlimIntfCreateInNim(L7_LOOPBACK_INTF, loopbackId, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "failed to create loopback%u in NIM (rc=%u)\n", loopbackId, rc);
    return rc;
  }

  lop->loopbackIntIfNum = intIfNum;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the operational state of a loopback interface
*
* @param    intIfNum
* @param    loopbackId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimLoopbackOpDelete(L7_uint32 intIfNum, L7_uint32 loopbackId)
{
  L7_RC_t rc = L7_FAILURE;

  /*
   * Turn off routing.  The subsequent reception of
   * an L7_PORT_ROUTING_DISABLED event will complete the processing.
   */
  rc = ipMapRtrIntfModeSet(intIfNum, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "failed to disable routing on loopback%u\n", loopbackId);
  }
#ifdef L7_IPV6_PACKAGE
  rc = ip6MapRtrIntfModeSet(intIfNum, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "failed to disable v6 routing on loopback%u\n", loopbackId);
  }
#endif
}

/*********************************************************************
* @purpose  Wait for the completion of a loopback deletion
*
* @param    loopbackId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimLoopbackOpDeleteWait(L7_uint32 loopbackId)
{
  rlimLoopbackOpData_t *lop = &rlimLoopbackOpData[loopbackId];
  RLIM_SEMA_TAKE(lop->deleteSema, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  Signal the completion of the deletion of a loopback from NIM
*
* @param    intIfNum
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimLoopbackNimDeleteDone(NIM_NOTIFY_CB_INFO_t retVal)
{
  L7_uint32 loopbackId;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (loopbackId = 0; loopbackId <= RLIM_MAX_LOOPBACK_ID; loopbackId++)
  {
    rlimLoopbackOpData_t *lop = &rlimLoopbackOpData[loopbackId];
    if (lop->loopbackIntIfNum == retVal.intIfNum)
    {
      rlimLoopbackOpDataInit(lop);
      RLIM_SEMA_GIVE(lop->deleteSema);
      break;
    }
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);
}

/*********************************************************************
* @purpose  Process the disabling of routing on a loopback
*
* @param    intIfNum      internal interface ID of loopback
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimLoopbackPortRoutingDisableProcess(L7_uint32 intIfNum)
{
  L7_uint32 rc;

  rc = rlimIntfDeleteInNim(intIfNum);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "NIM delete of loopback intf %s failed (rc=%u)\n",
            ifName, rc);
  }
}

/*********************************************************************
* @purpose  Process NIM interface events
*
* @param    intIfnum    port changing state
* @param    event       event type
* @param    correlator
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimNimIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 event,
                                  NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc;
  rlimMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_INTF_TYPES_t ifType;
  L7_BOOL returnFlag = L7_FALSE;

  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* If this is an event we are not interested in, perform early return*/
  if (event != L7_PORT_ROUTING_DISABLED &&
      event != L7_DELETE &&
      event != L7_PORT_DISABLE &&
      event != L7_PORT_ENABLE)
  {
    returnFlag = L7_TRUE;
  }

  if(nimGetIntfType(intIfNum, &ifType) != L7_SUCCESS)
  {
    returnFlag = L7_TRUE;
  }

  /* If it is a tunnel or loopback interface, then only the events
   * L7_PORT_ROUTING_DISABLED, L7_PORT_DISABLE, L7_PORT_ENABLE are of interest to us */
  if( ((event == L7_PORT_ROUTING_DISABLED) || (event == L7_PORT_DISABLE) || (event == L7_PORT_ENABLE)) &&
      ((ifType != L7_TUNNEL_INTF) && (ifType != L7_LOOPBACK_INTF)) )
  {
    returnFlag = L7_TRUE;
  }

  /* Return early to avoid unnecessary queueing */
  if(returnFlag == L7_TRUE)
  {
    status.intIfNum = intIfNum;
    status.component = L7_RLIM_COMPONENT_ID;
    status.event = event;
    status.correlator = correlator;
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  memset((void *)&msg, 0, sizeof(msg));
  msg.msgId = RLIM_NIM_EVENT;
  msg.type.nimEvent.intIfNum = intIfNum;
  msg.type.nimEvent.event = event;
  msg.type.nimEvent.correlator = correlator;

  rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "osapiMessageSend failed\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlimNimIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  rlimMsg_t   msg;
  L7_RC_t rc;

  if (RLIM_TRACE_EVENTS & rlimDebugTraceFlags)
  {
    L7_uchar8 traceBuf[RLIM_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                  "RLIM received NIM %s startup event",
                  startupPhase == NIM_INTERFACE_CREATE_STARTUP ? "CREATE" : "ACTIVATE");
    rlimDebugTraceWrite(traceBuf);
  }

  if (rlimCnfgrState != RLIM_PHASE_EXECUTE)
  {
    nimStartupEventDone(L7_RLIM_COMPONENT_ID);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
            "Received NIM startup %d callback while not in execute state", startupPhase);
    return;
  }

  memset(&msg, 0, sizeof(rlimMsg_t));
  msg.msgId = RLIM_NIM_STARTUP;
  msg.type.nimStartupEvent.startupPhase = startupPhase;

  rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if(rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
            "Failed to send NIM startup phase %d to RLIM thread",
            startupPhase);
  }
}

/*********************************************************************
* @purpose  Process IP4 routing events
*
* @param    intIfnum    port changing state
* @param    event       event type
* @param    *pData      pointer to event specific data
* @param    *pResponse  pointer to event specific data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimIp4RoutingEventChangeCallBack(L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse)
{
  L7_RC_t rc;
  rlimMsg_t msg;

  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset((void *)&msg, 0, sizeof(msg));
  msg.msgId = RLIM_IP4_RTR_EVENT;
  msg.type.ip4RtrEvent.intIfNum = intIfNum;
  msg.type.ip4RtrEvent.event = event;
  msg.type.ip4RtrEvent.pData = pData;
  if (pResponse != L7_NULL)
  {
    memcpy(&(msg.type.ip4RtrEvent.response), pResponse,
           sizeof(ASYNC_EVENT_NOTIFY_INFO_t) );
  }

  rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "osapiMessageSend failed\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Create the RLIM threads and the message queues used to send
*           work to the threads.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    RLIM has two threads. One thread handles configurator events.
*           The other thread handles all other work (router events, NIM events,
*           RTO best route changes). The thread for configurator events is
*           necessary because a) we don't want to process configurator
*           events on the configurator thread, and b) RLIM applies its config
*           data when it goes to EXECUTE state. If logical interfaces are being
*           deleted (e.g., clear config or move management) the thread that runs
*           rlimApplyConfigData() blocks on each interface delete until the RLIM
*           thread gets a NIM event saying the interface has been deleted.
*           The blocking is done so the UI won't return until the logical
*           interface is deleted. But if the RLIM thread were to do the
*           interface deletes and then block, it can't then process the NIM
*           events to unblock itself. So we introduce a second RLIM thread to
*           absorb the blocking on the interface delete.
*
* @end
*********************************************************************/
L7_RC_t rlimThreadCreate(void)
{
  rlimOpData = osapiMalloc(L7_RLIM_COMPONENT_ID, sizeof(*rlimOpData));
  if (rlimOpData == L7_NULL)
  {
    return L7_ERROR;
  }

  memset(rlimOpData, 0, sizeof(*rlimOpData));

  /* Create main RLIM thread */
  rlimOpData->processQueue = osapiMsgQueueCreate(RLIM_PROC_QUEUE,
                                                 RLIM_PROC_MSG_COUNT,
                                                 sizeof(rlimMsg_t));
  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
           "Failed to create message queue.");
    return L7_FAILURE;
  }

  rlimOpData->procTaskHandle = osapiTaskCreate(RLIM_PROC_TASK, rlimProcTask,
                                               L7_NULL, L7_NULLPTR,
                                               L7_DEFAULT_STACK_SIZE,
                                               L7_DEFAULT_TASK_PRIORITY,
                                               L7_DEFAULT_TASK_SLICE);
  if (rlimOpData->procTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
           "Failed to create thread.");
    return L7_FAILURE;
  }

  /* Create second thread to process configurator events */
  rlimOpData->cnfgrQueue = osapiMsgQueueCreate(RLIM_CNFGR_QUEUE,
                                               RLIM_CNFGR_MSG_COUNT,
                                               sizeof(rlimMsg_t));
  if (rlimOpData->cnfgrQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
           "Failed to create RLIM configurator queue.");
    return L7_FAILURE;
  }

  rlimOpData->cnfgrTaskHandle = osapiTaskCreate(RLIM_CNFGR_TASK, rlimCnfgrTask,
                                                L7_NULL, L7_NULLPTR,
                                                L7_DEFAULT_STACK_SIZE,
                                                L7_DEFAULT_TASK_PRIORITY,
                                                L7_DEFAULT_TASK_SLICE);
  if (rlimOpData->cnfgrTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
           "Failed to create RLIM configurator thread.");
    return L7_FAILURE;
  }

  rlimOpData->ip4NhopChangeTimer = L7_NULL;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Main function for the RLIM configurator thread. Processes
*           configurator events.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlimCnfgrTask(void)
{
  L7_uint32 status;
  rlimMsg_t Message;

  /* Loop forever, processing incoming messages */
  for (;;)
  {
    status = osapiMessageReceive(rlimOpData->cnfgrQueue, &Message,
                                 sizeof(Message), L7_WAIT_FOREVER);
    if (status == L7_SUCCESS)
    {
      switch (Message.msgId)
      {
        case RLIM_CNFGR_EVENT:
          rlimCnfgrHandle(&Message.type.cmdData);
          break;

        default:
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
                 "Unknown message type received in the RLIM configurator task.");
          break;
      }
    }
    else /* status != L7_SUCCESS */
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "Bad status from RLIM cnfgr message queue read: %d.", status);
    }
  } /* end for (;;) */
}

/*********************************************************************
* @purpose  The RLIM thread uses this function to process messages
*           from the message queue, performing the tasks specified
*           in each message. The initial motivation for this task
*           is to process router events.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlimProcTask(void)
{
  L7_uint32 status;
  rlimMsg_t Message;

  /* Loop forever, processing incoming messages */
  for (;;)
  {
    status = osapiMessageReceive(rlimOpData->processQueue, &Message,
                                 sizeof(Message), L7_WAIT_FOREVER);
    if (status == L7_SUCCESS)
    {
      switch (Message.msgId)
      {
        case RLIM_NIM_EVENT:
          rlimNimEventProcess(Message.type.nimEvent.intIfNum,
                              Message.type.nimEvent.event,
                              Message.type.nimEvent.correlator);
          break;

        case RLIM_RESTORE_EVENT:
          rlimRestoreProcess();
          break;

        case RLIM_IP4_RTR_EVENT:
          rlimIp4RoutingEventProcess(Message.type.ip4RtrEvent.intIfNum,
                                     Message.type.ip4RtrEvent.event,
                                     Message.type.ip4RtrEvent.pData,
                                     &Message.type.ip4RtrEvent.response);
          break;

        case RLIM_IP4_NHOP_CHANGE_EVENT:
          rlimTunnelIp4NhopChangeEventProcess();
          break;

        case RLIM_TUNNEL_DEST_AGE_EVENT:
#ifdef L7_IPV6_PACKAGE
          /* 6to4 tunnels shall be available only if IPv6 package exists */
          rlim6to4TunnelDestAgeEventProcess();
#endif
          break;

        case RLIM_NIM_STARTUP:

          if (Message.type.nimStartupEvent.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
          {
            rlimNimCreateStartup();
          }
          else if (Message.type.nimStartupEvent.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
          {
            rlimNimActivateStartup();
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
                    "Unknown NIM startup event %d", Message.type.nimStartupEvent.startupPhase);
          }
          break;

        default:
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
                 "Unknown message type received in the RLIM processing task.");
          break;
      }
    }
    else /* status != L7_SUCCESS */
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "Bad status from RLIM message queue read: %d.", status);
    }
  } /* end for (;;) */
}

/*********************************************************************
* @purpose  Enable Tunnel or Loopback interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ifType   @b{(input)} interface type
*
* @returns  none
*
* @end
*********************************************************************/
void rlimIntfEnable(L7_uint32 intIfNum, L7_INTF_TYPES_t ifType)
{
  rlimTunnelOpData_t *top =L7_NULLPTR;
  L7_uint32 tunnelId;

  if(ifType == L7_TUNNEL_INTF)
  {
    if(rlimTunnelIdGetNoLock(intIfNum, &tunnelId) == L7_SUCCESS)
    {
      top = &rlimTunnelOpData[tunnelId];
      top->flags |= RLIM_TNNLOP_FLAG_PORT_ENABLED;
      rlimTunnelOpUpdate(tunnelId);
    }
  }
  else if(ifType == L7_LOOPBACK_INTF)
  {
    rlimNimEventSend(intIfNum, L7_UP);
  }
}

/*********************************************************************
* @purpose  Disable Tunnel or Loopback interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ifType   @b{(input)} interface type
*
* @returns  none
*
* @end
*********************************************************************/
void rlimIntfDisable(L7_uint32 intIfNum, L7_INTF_TYPES_t ifType)
{
  rlimTunnelOpData_t *top =L7_NULLPTR;
  L7_uint32 tunnelId;

  if(ifType == L7_TUNNEL_INTF)
  {
    if(rlimTunnelIdGetNoLock(intIfNum, &tunnelId) == L7_SUCCESS)
    {
      top = &rlimTunnelOpData[tunnelId];
      top->flags &= ~RLIM_TNNLOP_FLAG_PORT_ENABLED;

      rlimTunnelOpUpdate(tunnelId);
    }
  }
  else if(ifType == L7_LOOPBACK_INTF)
  {
    rlimNimEventSend(intIfNum, L7_DOWN);
  }
}

/*********************************************************************
* @purpose  Process NIM interface events
*
* @param    intIfNum    internal Interface number
* @param    event       port event
* @param    correlator  correlator for the event
*
* @returns  L7_SUCCESS if event is recognized and processed
*           L7_FAILURE if event is an unexpected type and is not processed.
*
* @notes    Most interface event handling is done in the router event
*           handling function.  We have one purpose here, detect the
*           deletion of a referenced interface in our config.  This is
*           currently limited to the specification of a tunnel source
*           address by interface rather than an explicit address.
*
* @end
*********************************************************************/
L7_RC_t rlimNimEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                            NIM_CORRELATOR_t correlator)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_INTF_TYPES_t ifType;
  L7_uchar8 ifname[L7_NIM_IFNAME_SIZE];
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum = intIfNum;
  status.component = L7_RLIM_COMPONENT_ID;
  status.event = event;
  status.correlator = correlator;

  if((rc = nimGetIntfType(intIfNum, &ifType)) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "failed to get intf type for intf %s\n", ifName);
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (!(RLIM_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
            "RLIM received an interface change event in invalid phase");
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if( (RLIM_TRACE_EVENTS & rlimDebugTraceFlags))
  {
    if(ifType == L7_LOOPBACK_INTF || ifType == L7_TUNNEL_INTF)
    {
      L7_uchar8 traceBuf[RLIM_TRACE_LEN_MAX];
      memset(ifname, 0, sizeof(ifname));

      if(nimGetIntfName(intIfNum,L7_SYSNAME,ifname)== L7_SUCCESS)
      {
        if(ifType == L7_LOOPBACK_INTF)
        {
          osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                        " RLIM Loopback interface %s (%u) received NIM event  %s ",
                        ifname, intIfNum, nimGetIntfEvent(event));
          rlimDebugTraceWrite(traceBuf);
        }
        else
        {
          if(ifType == L7_TUNNEL_INTF)
          {
            osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                          "RLIM tunnel interface %s (%u) received NIM event %s ",
                          ifname, intIfNum, nimGetIntfEvent(event));
            rlimDebugTraceWrite(traceBuf);
          }
        }
      }
      else
      {
        osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                      "RLIM interface Err (%u) received NIM event %s",
                      intIfNum, nimGetIntfEvent(event));
        rlimDebugTraceWrite(traceBuf);
      }
    }
  }


  switch (event)
  {
    case L7_PORT_ROUTING_DISABLED:
      switch (ifType)
      {
        case L7_LOOPBACK_INTF:
          rlimLoopbackPortRoutingDisableProcess(intIfNum);
          break;

        case L7_TUNNEL_INTF:
          rlimTunnelPortRoutingDisableProcess(intIfNum);
          break;

        default:
          break;
      }

      break;

    case L7_DELETE:
      rlimTunnelIntfDeleteProcess(intIfNum);
      break;

    case L7_PORT_DISABLE:
      if(ifType == L7_LOOPBACK_INTF || ifType == L7_TUNNEL_INTF)
      {
        rlimIntfDisable(intIfNum, ifType);
      }
      break;

    case L7_PORT_ENABLE:
      if(ifType == L7_LOOPBACK_INTF || ifType == L7_TUNNEL_INTF)
      {
        rlimIntfEnable(intIfNum, ifType);
      }
      break;

    default:
      /* no action, return success */
      rc = L7_SUCCESS;
      break;
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  status.response.rc  = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rlimNimCreateStartup(void)
{
  nimStartupEventDone(L7_RLIM_COMPONENT_ID);

  if (RLIM_TRACE_EVENTS & rlimDebugTraceFlags)
  {
    L7_uchar8 traceBuf[RLIM_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                  "RLIM has processed NIM create startup callback");
    rlimDebugTraceWrite(traceBuf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rlimNimActivateStartup(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_INTF_TYPES_t ifType;

  if (RLIM_TRACE_EVENTS & rlimDebugTraceFlags)
  {
    L7_uchar8 traceBuf[RLIM_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                  "RLIM beginning NIM activate startup");
    rlimDebugTraceWrite(traceBuf);
  }

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if((nimGetIntfType(intIfNum, &ifType) == L7_SUCCESS) &&
       ((L7_LOOPBACK_INTF == ifType) || (ifType == L7_TUNNEL_INTF)))
    {
      rlimIntfEnable(intIfNum, ifType);
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register for future NIM events */
  PORTEVENT_SETMASKBIT(rlimOpData->nimEventMask, L7_PORT_ENABLE);
  PORTEVENT_SETMASKBIT(rlimOpData->nimEventMask, L7_PORT_DISABLE);
  PORTEVENT_SETMASKBIT(rlimOpData->nimEventMask, L7_PORT_ROUTING_DISABLED);
  PORTEVENT_SETMASKBIT(rlimOpData->nimEventMask, L7_DELETE);

  nimRegisterIntfEvents(L7_RLIM_COMPONENT_ID, rlimOpData->nimEventMask);

  nimStartupEventDone(L7_RLIM_COMPONENT_ID);

  if (RLIM_TRACE_EVENTS & rlimDebugTraceFlags)
  {
    L7_uchar8 traceBuf[RLIM_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RLIM_TRACE_LEN_MAX,
                  "RLIM completed processing NIM activate startup");
    rlimDebugTraceWrite(traceBuf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process IP4 routing events.
*
* @param    intIfnum @b{(input)}  internal interface number
* @param    event    @b{(input)}  an event in L7_RTR_EVENT_CHANGE_t
* @param    pData    @b{(input)}  unused
* @param    response @b{(input)}  provides parameters used to acknowledge
*                                 processing of the event
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    This function is called on the RLIM thread.
*           We are required to send to IP MAP an asynchronous response
*           saying that RLIM has completely processed the event. We
*           always send that response from this function.
*
* @end
*********************************************************************/
L7_RC_t rlimIp4RoutingEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                                   void *pData,
                                   ASYNC_EVENT_NOTIFY_INFO_t *response)
{
  L7_uint32 i;
  L7_RC_t rc;
  /* used to send acknowledgement data back to IP MAP */
  ASYNC_EVENT_COMPLETE_INFO_t event_completion;
  memset(&event_completion, 0, sizeof(event_completion));

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  switch (event)
  {
    case L7_RTR_DISABLE_PENDING:
    /* implies IPv4 disabled on all routing interfaces */
      for (rc = nimFirstValidIntfNumber(&i); rc == L7_SUCCESS;
        rc = nimNextValidIntfNumber(i, &i))
      {
        rlimTunnelIp4IntfDownProcess(i);
      }
      break;

    case L7_RTR_INTF_ENABLE:
      rlimTunnelIp4IntfUpProcess(intIfNum);
      break;

    case L7_RTR_INTF_IPADDR_DELETE_PENDING:
      /* deprecated */
      break;

    case L7_RTR_INTF_DISABLE_PENDING:
      rlimTunnelIp4IntfDownProcess(intIfNum);
      break;

    case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
      rlimTunnelIp4SecondaryAddrAddProcess(intIfNum);
      break;

    case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
      rlimTunnelIp4SecondaryAddrDeleteProcess(intIfNum);
      break;

    case L7_RTR_STARTUP_DONE:
      /* When we get this event the configuration apply is complete and the source endpoints of tunnels would have come up.
       * As a result the configured tunnels would come up by now. Inform configurator that h/w reconciliation is complete */

      cnfgrApiComponentHwUpdateDone(L7_RLIM_COMPONENT_ID, L7_CNFGR_HW_APPLY_L3);

      break;

    default:
      break;
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  if ((response != L7_NULLPTR) &&
      (response->handlerId != 0) &&
      (response->correlator != 0))
  {
    /* acknowledge RLIM's processing of the event */
    event_completion.componentId = L7_IPRT_RLIM;
    event_completion.handlerId = response->handlerId;
    event_completion.correlator = response->correlator;
    event_completion.async_rc.rc = L7_SUCCESS;
    event_completion.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    asyncEventCompleteTally(&event_completion);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Service nexthop change callback
*
* @param    void
*
* @returns  void
*
* @notes    Set a timer to delay processing these until they have
*           some more time to accumulate.
*
* @end
*********************************************************************/
void rlimTunnelIp4NhopChangeCallback(L7_uint32 addr, void *tid)
{
  if (rlimOpData->ip4NhopChangeTimer  == L7_NULL)
  {
    /* schedule an event to process best route changes */
    osapiTimerAdd((void *)rlimTunnelIp4NhopChangeTimerCallback, L7_NULL, L7_NULL,
                  RLIM_NHOP_CHANGE_DELAY, &rlimOpData->ip4NhopChangeTimer);
  }
}

/*********************************************************************
* @purpose Timer callback to trigger IP MAP to get best routes from RTO.
*
* @param   void
*
* @returns void
*
* @comments  This callback tells RLIM that best route changes are pending
*            in RTO. RLIM has to go back to RTO and ask for the changes.
*
* @end
*
*********************************************************************/
void rlimTunnelIp4NhopChangeTimerCallback(void)
{
  L7_RC_t rc;
  rlimMsg_t msg;
  L7_BOOL sendMsg;

  /*
   * Mark timer as "fired"
   */
  rlimOpData->ip4NhopChangeTimer = L7_NULL;

  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
            "RLIM Process_Queue is NULL");
    return;
  }

  /*
   * Only allow one outstanding route event message as that's all we need
   */
  RLIM_SEMA_TAKE(rlimNhopChangeEventMutex, L7_WAIT_FOREVER);
  sendMsg = rlimNhopChangeMessageAllow;
  rlimNhopChangeMessageAllow = L7_FALSE;
  RLIM_SEMA_GIVE(rlimNhopChangeEventMutex);

  if (sendMsg == L7_TRUE)
  {
    memset((void *)&msg, 0, sizeof(msg));
    msg.msgId = RLIM_IP4_NHOP_CHANGE_EVENT;

    rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "osapiMessageSend() failed with rc = %u", rc);
    }
  }
}

/*********************************************************************
* @purpose  Process RTO route events.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIp4NhopChangeEventProcess(void)
{
  RLIM_SEMA_TAKE(rlimNhopChangeEventMutex, L7_WAIT_FOREVER);
  rlimNhopChangeMessageAllow = L7_TRUE;
  RLIM_SEMA_GIVE(rlimNhopChangeEventMutex);

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  rlimTunnelIp4NhopChangeProcess();
  RLIM_WRITE_LOCK_GIVE(rlimRWLock);
}
