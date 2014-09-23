/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename lldp_cnfgr.c
*
* @purpose Contains definitions to support the configurator API
*
* @component
*
* @comments
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "buff_api.h"
#include "lldp.h"
#include "lldp_util.h"
#include "lldp_tlv.h"
#include "lldp_migrate.h"
#include "lldp_debug.h"

#ifdef L7_NSF_PACKAGE
#include "lldp_ckpt.h"
#endif /* L7_NSF_PACKAGE */

lldpCnfgrState_t            lldpCnfgrState = LLDP_PHASE_INIT_0;
extern void                *lldpQueue;
extern L7_uint32           *lldpMapTbl;
extern L7_int32             lldpTaskId;
extern lldpCfgData_t       *lldpCfgData;
extern lldpStats_t         *lldpStats;
extern lldpIntfOprData_t   *lldpIntfTbl;
extern void                *lldpSemaphore;
extern L7_uchar8           *lldpPDUBuf;
extern L7_uint32            lldpMgmtAddrPoolId;
extern L7_uint32            lldpUnknownTLVPoolId;
extern L7_uint32            lldpOrgDefInfoPoolId;
extern lldpXMedNotifyList_t *lldpXMedNotifyList;

extern L7_BOOL              lldpWarmRestart;
extern void                *lldpCkptReqQ;
extern avlTree_t            lldpCkptInfoAvlTree;
extern avlTreeTables_t     *lldpCkptInfoTreeHeap;
extern lldpCkptInfo_t      *lldpCkptInfoDataHeap;

extern PORTEVENT_MASK_t     lldpNimEventMask;

/* This defines the maximum number of remote nodes supported by LLDP */
#define L7_LLDP_MAX_REM_ENTRIES_SUPPORTED  L7_MAX_PORT_COUNT * L7_LLDP_MAX_ENTRIES_PER_PORT

extern void lldpTimerAdd(void);
extern void lldpTimerDelete(void);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for 802.1AB component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the component.  This function is re-entrant.
*
* @end
*********************************************************************/
void lldpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  lldpMgmtMsg_t msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (lldpInit(pCmdData) != L7_SUCCESS)
      lldpInitUndo();
  } else
  {
    memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = lldpMsgCnfgr;
    if (osapiMessageSend(lldpQueue,
                         &msg,
                         LLDP_MSG_SIZE,
                         L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
              "lldpApiCnfgrCommand(): message send failed\n");
    }
  }
}


/*********************************************************************
*
* @purpose  System Initialization for 802.1AB component
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  lldpMgmtMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  lldpQueue = (void *)osapiMsgQueueCreate(LLDP_QUEUE, LLDP_MSG_COUNT, LLDP_MSG_SIZE);
  if (lldpQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "lldpInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (lldpTaskStart() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = lldpMsgCnfgr;
  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "lldpInit(): message send failed\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for 802.1AB component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpInitUndo()
{
  if (lldpQueue != L7_NULLPTR)
    osapiMsgQueueDelete(lldpQueue);


  if (lldpTaskId != L7_ERROR)
    osapiTaskDelete(lldpTaskId);

  lldpCnfgrState = LLDP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t lldpCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t   rc;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;


  lldpCfgData   = osapiMalloc(L7_LLDP_COMPONENT_ID, sizeof(lldpCfgData_t));
  lldpMapTbl    = osapiMalloc(L7_LLDP_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
  lldpStats     = osapiMalloc(L7_LLDP_COMPONENT_ID, sizeof(lldpStats_t));

  rc = lldpRemoteDBInit(L7_LLDP_MAX_REM_ENTRIES_SUPPORTED);

  lldpIntfTbl   = osapiMalloc(L7_LLDP_COMPONENT_ID, sizeof(lldpIntfOprData_t) * L7_LLDP_INTF_MAX_COUNT);
  lldpPDUBuf    = osapiMalloc(L7_LLDP_COMPONENT_ID, sizeof(L7_uchar8) * LLDP_PDU_SIZE_MAX);
  lldpXMedNotifyList = (lldpXMedNotifyList_t *)osapiMalloc(L7_LLDP_COMPONENT_ID,
                                                           (L7_uint32)(sizeof(lldpXMedNotifyList_t) * L7_LAST_COMPONENT_ID));

  if ((lldpCfgData   == L7_NULLPTR) ||
      (lldpMapTbl    == L7_NULLPTR) ||
      (lldpStats     == L7_NULLPTR) ||
      (rc            == L7_FAILURE) ||
      (lldpIntfTbl   == L7_NULLPTR) ||
      (lldpPDUBuf    == L7_NULLPTR) ||
      (lldpXMedNotifyList == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /* Allocate buffer pools for optional remote TLVs */
  if (bufferPoolInit(L7_LLDP_REM_MGMT_ADDR_BUFFERS,
                     sizeof(lldpMgmtAddrEntry_t) + (sizeof(lldpMgmtAddrEntry_t) % 4),
                     LLDP_MGMT_ADDR_BUF_DESC,
                     &lldpMgmtAddrPoolId) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
  if (bufferPoolInit(L7_LLDP_REM_UNKNOWN_TLV_BUFFERS,
                     sizeof(lldpUnknownTLVEntry_t) + (sizeof(lldpUnknownTLVEntry_t) % 4),
                     LLDP_UNKNOWN_TLV_BUF_DESC,
                     &lldpUnknownTLVPoolId) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
  if (bufferPoolInit(L7_LLDP_REM_ORG_DEF_INFO_BUFFERS,
                     sizeof(lldpOrgDefInfoEntry_t) + (sizeof(lldpOrgDefInfoEntry_t) % 4),
                     LLDP_ORG_DEF_INFO_BUF_DESC,
                     &lldpOrgDefInfoPoolId) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  lldpSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (lldpSemaphore == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  memset((void *)lldpCfgData, 0, sizeof(lldpCfgData_t));
  memset((void *)lldpMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
  memset((void *)lldpStats, 0, sizeof(lldpStats_t));
  memset((void *)lldpIntfTbl, 0, sizeof(lldpIntfOprData_t) * L7_LLDP_INTF_MAX_COUNT);
  memset((void *)lldpPDUBuf, 0, sizeof(L7_uchar8) * LLDP_PDU_SIZE_MAX);
  memset((void *)lldpXMedNotifyList, 0, (sizeof(lldpXMedNotifyList_t) * L7_LAST_COMPONENT_ID));

#ifdef L7_NSF_PACKAGE
  /*
  lldpDebugTraceEnable();
  lldpDebugTraceFlagsSet(LLDP_DEBUG_CHECKPOINT);
  */

  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Allocating checkpoint info");
  if (lldpCheckpointInfoAlloc() != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID,
              "lldpCnfgrInitPhase1Process: Error allocating data for NSF Checkpointing");
      *pResponse  = 0;
      *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      return L7_ERROR;
  }
  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Checkpoint info allocated");
#endif /* L7_NSF_PACKAGE */

  lldpCnfgrState = LLDP_PHASE_INIT_1;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t lldpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  nvStoreFunctionList_t lldpNotifyFunctionList;
  sysnetNotifyEntry_t   snEntry;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *) &lldpNotifyFunctionList, 0, sizeof(lldpNotifyFunctionList));
  lldpNotifyFunctionList.registrar_ID     = L7_LLDP_COMPONENT_ID;
  lldpNotifyFunctionList.notifySave       = lldpSave;
  lldpNotifyFunctionList.hasDataChanged   = lldpHasDataChanged;
  lldpNotifyFunctionList.notifyConfigDump = lldpCfgDump;
  lldpNotifyFunctionList.resetDataChanged = lldpResetDataChanged;

  if (nvStoreRegister(lldpNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  if (nimRegisterIntfChange(L7_LLDP_COMPONENT_ID, lldpIntfChangeCallback,
                            lldpStartupCallback, NIM_STARTUP_PRIO_LLDP) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "802.1AB: Failed to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  /* register with sysnet for LLDP packets, no packets
     will actually be sent to the application until
     we configure the driver to accept them on the
     physical port, we do that when the receive
     function is enabled and port is up */
  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "lldpPduReceiveCallback", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = lldpPduReceiveCallback;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_LLDP;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

#ifdef L7_NSF_PACKAGE
  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Registering for NSF callbacks");
  if (lldpCheckpointCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_LLDP_COMPONENT_ID,
           "LLDP failed to register for checkpoint service callbacks.");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
#endif /* L7_NSF_PACKAGE */

  lldpCnfgrState = LLDP_PHASE_INIT_2;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t lldpCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                   L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /*
  Clear the NIM port event mask until ready to receive events
  This is necessary because Phase3 can be re-entered without
  re-initializing lldpNimEventMask
  */

  memset(&lldpNimEventMask, 0, sizeof(lldpNimEventMask));
  nimRegisterIntfEvents(L7_LLDP_COMPONENT_ID, lldpNimEventMask);

  if (sysapiCfgFileGet(L7_LLDP_COMPONENT_ID, LLDP_CFG_FILENAME,
                       (L7_char8 *)lldpCfgData, sizeof(lldpCfgData_t),
                       &lldpCfgData->checkSum, LLDP_CFG_VER_CURRENT,
                       lldpBuildDefaultConfigData, lldpMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  lldpWarmRestart = warmRestart;

  /*
  If not a warm restart, clear LLDP Checkpoint Data
  */

  if (!warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    /* Clear any checkpointed data */
    LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Not warm restart");
    lldpCheckpointFlushAll();
#endif /* L7_NSF_PACKAGE */
  }

  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Applying Config Data");
  if (lldpApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "Apply of Config Data done");

 lldpTimerAdd(); /* start the lldp timer process */
  lldpCnfgrState = LLDP_PHASE_INIT_3;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase1Process()
{

  if (lldpCfgData != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpCfgData);
    lldpCfgData = L7_NULLPTR;
  }

  if (lldpMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpMapTbl);
    lldpMapTbl = L7_NULLPTR;
  }

  if (lldpStats != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpStats);
    lldpStats = L7_NULLPTR;
  }

  lldpRemoteDBDeInit();

  if (lldpIntfTbl != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpIntfTbl);
    lldpIntfTbl = L7_NULLPTR;
  }

  if (lldpPDUBuf != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpPDUBuf);
    lldpPDUBuf = L7_NULLPTR;
  }

  if (lldpMgmtAddrPoolId != 0)
  {
    bufferPoolTerminate(lldpMgmtAddrPoolId);
    lldpMgmtAddrPoolId = 0;
  }

  if (lldpUnknownTLVPoolId != 0)
  {
    bufferPoolTerminate(lldpUnknownTLVPoolId);
    lldpUnknownTLVPoolId = 0;
  }

  if (lldpOrgDefInfoPoolId != 0)
  {
    bufferPoolTerminate(lldpOrgDefInfoPoolId);
    lldpOrgDefInfoPoolId = 0;
  }

  if (lldpXMedNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpXMedNotifyList);
    lldpXMedNotifyList = L7_NULLPTR;
  }

  if (lldpSemaphore != L7_NULLPTR)
  {
    osapiSemaDelete(lldpSemaphore);
    lldpSemaphore = L7_NULLPTR;
  }

  lldpInitUndo();

  lldpCnfgrState = LLDP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase2Process()
{
  sysnetNotifyEntry_t snEntry;

  (void)nimDeRegisterIntfChange(L7_LLDP_COMPONENT_ID);
  (void)nvStoreDeregister(L7_LLDP_COMPONENT_ID);

  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "lldpPduReceiveCallback", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = lldpPduReceiveCallback;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_LLDP;
  (void)sysNetDeregisterPduReceive(&snEntry);

  lldpCnfgrState = LLDP_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  lldpTimerDelete(); /* Stop the lldp timer */
  /* this func will place lldpCnfgrState to WMU */
  lldpCnfgrUconfigPhase2(&response, &reason);
}

/***************************************************************************
* @purpose  This function process the configurator control commands/request
*           pair TERMINATE.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    User has initiated a failover to the backup unit. If LLDP is waiting to
*           checkpoint data to the backup unit, wait until that completes. In this
*           case, wait up to 20 seconds and then blunder on. This semaphore will
*           never be given. Assumption is that if we get here, the management unit
*           will definitely be rebooted.
*
*           Deprecated: checkpoint pending sema is now obsolete
*
* @end
***************************************************************************/
L7_RC_t lldpCnfgrTerminateProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  /*
  osapiSemaTake(lldpCkptPendingSema, 20000);
  */

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t lldpCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
L7_RC_t lldpCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* TBD - disable Tx/Rx on all interfaces?? */

  memset(lldpCfgData, 0, sizeof(lldpCfgData));

  lldpCnfgrState = LLDP_PHASE_WMU;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to lldpTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL               warmRestart = L7_FALSE;

  L7_RC_t             lldpRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if (request > L7_CNFGR_RQST_FIRST &&
          request < L7_CNFGR_RQST_LAST)
      {
        /* validate command/event pair */
        switch (command)
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch (request)
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((lldpRC = lldpCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  lldpCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((lldpRC = lldpCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  lldpCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                warmRestart = ((pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM) ? L7_TRUE : L7_FALSE);
                if ((lldpRC = lldpCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  lldpCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                lldpRC = lldpCnfgrNoopProccess(&response, &reason);
                lldpCnfgrState = LLDP_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch (request)
            {
              case L7_CNFGR_RQST_E_START:
                lldpCnfgrState = LLDP_PHASE_EXECUTE;

                lldpRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                lldpRC = lldpCnfgrNoopProccess(&response, &reason);
                lldpCnfgrState = LLDP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                lldpRC = lldpCnfgrUconfigPhase2(&response, &reason);
                lldpCnfgrState = LLDP_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
            lldpRC = lldpCnfgrTerminateProcess(&response, &reason);
            break;

          case L7_CNFGR_CMD_SUSPEND:
            lldpRC = lldpCnfgrNoopProccess(&response, &reason);
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */


      } else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      } /* endif validate request */


    } else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    } /* endif validate command type */


  } else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = lldpRC;
  if (lldpRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}
