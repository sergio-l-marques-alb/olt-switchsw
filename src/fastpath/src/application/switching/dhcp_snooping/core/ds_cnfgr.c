/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_cnfgr.c
*
* @purpose   DHCP Snooping configurator file
*
* @component DHCP Snooping
*
* @comments none
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "log.h"
#include "nimapi.h"

#include "ds_util.h"
#include "ds_cnfgr.h"
#include "ds_cfg.h"

#ifdef L7_IPSG_PACKAGE
#include "ds_ipsg.h"
#endif

#ifdef L7_NSF_PACKAGE
#include "ds_ckpt.h"
#endif

#include "dhcp_snooping_api.h"

osapiRWLock_t dsCfgRWLock;
dsCfgData_t *dsCfgData = L7_NULLPTR;
dsDbCfgData_t dsDbCfgData;
dsCnfgrState_t dsCnfgrState = DHCP_SNOOP_PHASE_INIT_0;
extern dsInfo_t *dsInfo;
extern dsIntfInfo_t *dsIntfInfo;

#ifdef L7_IPSG_PACKAGE
extern ipsgInfo_t *ipsgInfo;
#endif


/* The DHCP snooping thread reads from two queues. Ds_Event_Queue
 * holds events to be processed. Ds_Packet_Queue holds incoming
 * DHCP packets. */
void *dsMsgQSema = L7_NULLPTR;
void *Ds_Event_Queue = L7_NULLPTR;
void *Ds_Packet_Queue = L7_NULLPTR;

L7_char8 *dsInitStateNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
  "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};


static void dsCnfgrStateSet(dsCnfgrState_t newState);
static L7_RC_t dsThreadCreate(void);
static L7_RC_t dsCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason);
#ifdef L7_DHCPS_PACKAGE


extern L7_RC_t dhcpsSendTypeNotifyRegister(L7_uint32 registrar_ID,
      L7_uint32 (*notify_func)(L7_uint32 sendMsgType,
                               L7_enetMacAddr_t chAddr,
                               L7_uint32 ipAddr,
                               L7_uint32 leaseTime));

extern L7_RC_t dhcpsSendTypeNotifyDeRegister(L7_uint32 registrar_ID);
#endif

extern dsFuncTable_t dsFuncTable;

static void dsFuncTableInit(void);

/*********************************************************************
*
* @purpose  Set the DHCP snooping configuration state
*
* @param    newState - @b{(input)}  The configuration state that DHCP snooping
*                                   is transitioning to.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsCnfgrStateSet(dsCnfgrState_t newState)
{
  if (dsCfgData->dsTraceFlags & DS_TRACE_INIT)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "DHCP snooping transitioning from %s to %s",
                  dsInitStateNames[dsCnfgrState],
                  dsInitStateNames[newState]);
    dsTraceWrite(traceMsg);
  }
  dsCnfgrState = newState;
}

/*********************************************************************
*
* @purpose  Set initial defaults for DHCP global variables
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsInfoInit(void)
{
  dsInfo->cfgDataChanged = L7_FALSE;
  dsInfo->dsDbDataChanged = L7_FALSE;
  dsInfo->bindingsTable.maxBindings = L7_DHCP_SNOOP_MAX_BINDINGS;
  return;
}

#ifdef L7_IPSG_PACKAGE
/*********************************************************************
*
* @purpose  Set initial defaults for IPSG global variables
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipsgInfoInit(void)
{
  ipsgInfo->cfgDataChanged = L7_FALSE;
  ipsgInfo->ipsgEntryTable.maxBindings =(L7_DHCP_SNOOP_MAX_BINDINGS) +
                                         (L7_IPSG_MAX_STATIC_BINDINGS);
  ipsgInfo->ipsgEntryTable.maxStaticBindings = L7_IPSG_MAX_STATIC_BINDINGS;
  ipsgInfo->ipsgEntryTable.currentStaticBindings = 0;
  return ;
}
#endif



/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t dsCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                           L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t dsRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return(dsRC);
}

/*********************************************************************
* @purpose  CNFGR System Initialization for DHCP Snooping component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the DHCP Snooping comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  dsEventMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  if (pCmdData == L7_NULL)
  {
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  /* Do minimum amound of work on configurator thread. Pass other work to
   * DHCP snooping thread. */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the DHCP snooping thread, then pass
     * a message via the queue to complete phase 1 initialization.
     */
    if (dsThreadCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.dsMsgData.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = DS_CNFGR_INIT;
  if (osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Failure sending DHCP snooping configurator init message.");
  }

  return;
}

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator on
*           the DHCP snooping thread.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void dsCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;
  L7_RC_t rc = L7_ERROR;

  if ((pCmdData == L7_NULLPTR) || (pCmdData->type != L7_CNFGR_RQST))
  {
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  command = pCmdData->command;
  request = pCmdData->u.rqstData.request;
  cbData.correlator = pCmdData->correlator;
  if ((request <= L7_CNFGR_RQST_FIRST) || (request >= L7_CNFGR_RQST_LAST))
  {
    /* invalid request */
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
    cnfgrApiCallback(&cbData);
    return;
  }

  switch (command)
  {
    case L7_CNFGR_CMD_INITIALIZE:
      if (L7_CNFGR_RQST_I_PHASE3_START == request)
      {
        dsInfo->warmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
      }
      rc = dsCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      if (request == L7_CNFGR_RQST_E_START)
      {
        dsCnfgrStateSet(DHCP_SNOOP_PHASE_EXECUTE);
        rc = L7_SUCCESS;
        response = L7_CNFGR_CMD_COMPLETE;
        reason = 0;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = dsCnfgrNoopProcess(&response, &reason);
          dsCnfgrStateSet(DHCP_SNOOP_PHASE_UNCONFIG_1);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = dsCnfgrUconfigPhase2(&response, &reason);
          dsCnfgrStateSet(DHCP_SNOOP_PHASE_UNCONFIG_2);
          break;

        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      rc = dsCnfgrNoopProcess(&response, &reason);
      break;

    default:
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
  }               /* end switch command */

  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
*
* @purpose  Handles an initialization request from the configurator.
*
* @param @b{(input)} request - indicates the initialization phase to
*                              be executed
* @param @b{(output)} response - response if request successfully handled
* @param @b{(input)} reason - if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t dsCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;

  switch (request)
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = dsCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        dsCnfgrFiniPhase1Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = dsCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        dsCnfgrFiniPhase2Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = dsCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        dsCnfgrFiniPhase3Process();
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = dsCnfgrNoopProcess(response, reason);
      dsCnfgrStateSet(DHCP_SNOOP_PHASE_WMU);
      break;
    default:
      /* invalid request */
      rc = L7_ERROR;
      *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;
  }
  return rc;
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t dsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t dsRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  dsRC = L7_SUCCESS;

  /* Allocate and initialized memory for global data */
  dsCfgData = (dsCfgData_t *) osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID,
                                          (L7_uint32) sizeof(dsCfgData_t));

  if (dsCfgData == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dsRC = L7_ERROR;
    return dsRC;
  }

  memset((void *) dsCfgData, 0, (L7_uint32) sizeof(dsCfgData_t));

  dsInfo = (dsInfo_t*) osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, sizeof(dsInfo_t));
  if (!dsInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dsRC = L7_ERROR;
    return dsRC;
  }
  memset(dsInfo, 0, sizeof(dsInfo));
  dsInfoInit();
  dsBindingsTableCreate();
#ifdef L7_IPSG_PACKAGE
  ipsgInfo = (ipsgInfo_t*) osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, sizeof(ipsgInfo_t));
  if (!ipsgInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dsRC = L7_ERROR;
    return dsRC;
  }
  memset(ipsgInfo, 0, sizeof(ipsgInfo));
  ipsgInfoInit();
  ipsgEntryTableCreate();
#endif
  dsIntfInfo = (dsIntfInfo_t*) osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID,
                                           (DS_MAX_INTF_COUNT+1) * sizeof(dsIntfInfo_t));
  if (!dsIntfInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dsRC = L7_ERROR;
    return dsRC;
  }
  memset(dsIntfInfo, 0, DS_MAX_INTF_COUNT * sizeof(dsIntfInfo_t));

#ifdef L7_NSF_PACKAGE
  dsInfo->ckptPendingSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(!dsInfo->ckptPendingSema)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Failed to create DHCP Snooping ckptPendingSema");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dsRC = L7_ERROR;
    return dsRC;
  }
#endif

  dsCnfgrStateSet(DHCP_SNOOP_PHASE_INIT_1);
  return dsRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
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
L7_RC_t dsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t dsNotifyFunctionList;
  L7_uint32 eventMask = 0;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *)&dsNotifyFunctionList, 0, sizeof(dsNotifyFunctionList));
  dsNotifyFunctionList.registrar_ID     = L7_DHCP_SNOOPING_COMPONENT_ID;
  dsNotifyFunctionList.notifySave       = dsSave;
  dsNotifyFunctionList.hasDataChanged   = dsHasDataChanged;
  dsNotifyFunctionList.resetDataChanged = dsResetDataChanged;

  if (nvStoreRegister(dsNotifyFunctionList) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_SNOOPING_COMPONENT_ID,
            "dsCnfgrInitPhase2Process: Failed to register with nvStore." );
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;

    return rc;
  }

  if (nimRegisterIntfChange(L7_DHCP_SNOOPING_COMPONENT_ID, dsIntfChangeCallback,
                            dsIntfStartupCallback, NIM_STARTUP_PRIO_DS) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to register with interface change callback with NIM");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc     = L7_ERROR;

    return rc;
  }

  /* register callback with dot1q to get notifications for vlan delete
   * and port membership changes events */
  eventMask = VLAN_DELETE_PENDING_NOTIFY | VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY;
  if (vlanRegisterForChange(dsVlanChangeCallback,
                            L7_DHCP_SNOOPING_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping failed to register for VLAN changes.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc  = L7_ERROR;
    return rc;
  }

  /* Register with cli text config componet to get notifications after
    apply config completion */
#ifdef L7_CLI_PACKAGE
  if (txtCfgApplyCompletionNotifyRegister(L7_DHCP_SNOOPING_COMPONENT_ID,
                            dsApplyConfigCompleteCb) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping failed to register for apply config completions.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc  = L7_ERROR;
    return rc;
  }
#endif

#ifdef L7_DHCPS_PACKAGE
  /* Register with the DHCPS to get notified on ACK and NACK messages
     to complete the tentative bindings */
  if (dhcpsSendTypeNotifyRegister(L7_DHCP_SNOOPING_COMPONENT_ID,
                              dsLocalServerCb))
  {

    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping failed to register with DHCPS.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc  = L7_ERROR;
    return rc;
  }
#endif

#ifdef L7_NSF_PACKAGE
  if (dsCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP Snooping failed to register for checkpoint service callbacks.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc  = L7_ERROR;
    return rc;
  }
#endif

  dsCnfgrStateSet(DHCP_SNOOP_PHASE_INIT_2);
  return rc;
}

/*********************************************************************
*
* @purpose  Initialize the function pointers in the function table
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void dsFuncTableInit(void)
{
    dsFuncTable.dsBindingGet      = dsBindingGet;
    dsFuncTable.dsAdminModeGet    = dsAdminModeGet;
    dsFuncTable.dsVlanConfigGet   = dsVlanConfigGet;
    dsFuncTable.dsIntfTrustGet    = dsIntfTrustGet;
    dsFuncTable.dsPortEnabledGet  = dsPortEnabledGet;
}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t dsCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&dsInfo->nimEventMask, 0, sizeof(dsInfo->nimEventMask));
  nimRegisterIntfEvents(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->nimEventMask);

  dsBuildDefaultConfigData(0);

  if (! dsInfo->warmRestart)
  {
    /* Apply the default config data only if this is not warm restart,
     * else we would end up clearing the bindings database */

    if (dsApplyConfigData() != L7_SUCCESS)
    {
      *pResponse  = 0;
      *pReason    = L7_CNFGR_ERR_RC_FATAL;
      rc = L7_ERROR;
      return( rc );
    }
  }
  else
  {
    if(L7_ENABLE == dsCfgData->dsGlobalAdminMode)
    {
      dsAdminModeEnable();
    }
    else
    {
      /* In case of warm restart, disable global snooping but with
       * a flag to indicate not to clear the bindings */
      dsAdminModeDisable(L7_FALSE);
    }
  }

  dsCfgDataNotChanged();

  /* Start DS periodic timer */
  dsTimerStart();

  dsCnfgrStateSet(DHCP_SNOOP_PHASE_INIT_3);

  dsFuncTableInit();

  return( rc );
}

/*********************************************************************
* @purpose  This function undoes dsCnfgrFiniPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsCnfgrFiniPhase1Process( void )
{
  if (dsCfgData != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, dsCfgData);
    dsCfgData = L7_NULLPTR;
  }
  dsCnfgrStateSet(DHCP_SNOOP_PHASE_INIT_0);
}

/*********************************************************************
* @purpose  This function undoes dsCnfgrFiniPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsCnfgrFiniPhase2Process(void)
{
  (void)nimDeRegisterIntfChange(L7_DHCP_SNOOPING_COMPONENT_ID);
  (void)nvStoreDeregister(L7_DHCP_SNOOPING_COMPONENT_ID);
  #ifdef L7_DHCPS_PACKAGE
   (void)dhcpsSendTypeNotifyDeRegister(L7_DHCP_SNOOPING_COMPONENT_ID);
  #endif

  dsCnfgrStateSet(DHCP_SNOOP_PHASE_INIT_1);
}

/*********************************************************************
* @purpose  This function undoes dsCnfgrFiniPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place dsCnfgrState to WMU */
  dsCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t dsCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  if (dsRestoreProcess() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return rc;
  }

  dsCnfgrStateSet(DHCP_SNOOP_PHASE_WMU);

  return rc;
}

/*********************************************************************
* @purpose  Create the DHCP snooping thread and the message queue used to
*           send work to the thread.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    The DHCP snooping thread serves two message queues. One queue
*           contains events (VLAN membership changes, configurator events).
*           The second queue holds incoming DHCP packets.
*
*           This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/
static L7_RC_t dsThreadCreate(void)
{
  L7_int32 dsTaskHandle;

  /* Read/write lock to protect component data since processing occurs
   * on multiple threads (DS thread, UI thread, dot1q, etc) */
  if (osapiRWLockCreate(&dsCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Unable to create read/write lock for DHCP Snooping");
    return L7_FAILURE;
  }

  /* Counting semaphore. Given whenever a message is added to any message queue
   * for the DHCP snooping thread. Taken when a message is read. */
  dsMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (dsMsgQSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Failure creating DHCP snooping message queue semaphore.");
    return L7_FAILURE;
  }

  /* create queue for VLAN and initialization events */
  Ds_Event_Queue = osapiMsgQueueCreate(DHCP_SNOOP_EVENT_QUEUE,
                                       DHCP_SNOOP_EVENTQ_MSG_COUNT,
                                       sizeof(dsEventMsg_t));

  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping unable to create event queue.");
    return L7_FAILURE;
  }

  /* create DHCP snooping packet queue */
  Ds_Packet_Queue = osapiMsgQueueCreate(DHCP_SNOOP_PACKET_QUEUE,
                                        DHCP_SNOOP_PACKETQ_MSG_COUNT,
                                        sizeof(dsFrameMsg_t));

  if (Ds_Packet_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping unable to create packet queue.");
    return L7_FAILURE;
  }

  dsTaskHandle = osapiTaskCreate(DHCP_SNOOP_TASK,
                                 dhcpSnoopTask,
                                 L7_NULL, L7_NULLPTR,
                                 L7_DEFAULT_STACK_SIZE,
                                 L7_DEFAULT_TASK_PRIORITY,
                                 L7_DEFAULT_TASK_SLICE);

  if (dsTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
           "ERROR: Unable to create DHCP snooping thread.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


