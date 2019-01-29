/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 * @filename  dhcps_cnfgr.c
 *
 * @purpose   Implements the new stacking initialization sequence
 *
 * @component
 *
 * @comments  The main configurator dictates what sequence of state changes
 *            by using the apt command/request pair. The current initialization
 *            sequence for non-management units is: INIT_0, INIT_1, INIT_2 and WMU.
 *            Management units have the sequence: INIT_0, INIT_1, INIT_2, WMU and INIT_3.
 *            After the management unit (MU) has transitioned into INIT_3 the configurator
 *            issues a command to all units (MU and non-MU) to move into the EXECUTE phase.
 *
 *            When a "clear config" command is issued, the sequence of state changes
 *            is: EXECUTE,UNCONFIG_1, UNCONFIG_2, WMU.
 *            The MU then goes into INIT_3 and subsequently all units go into EXECUTE phase.
 *
 *            None the state changes are enforced by the DHCP relay component; the main
 *            configurator ensures this.
 *
 * @create    11/18/2003
 *
 * @author    ksriniv
 *
 * @end
 *
 **********************************************************************/

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "l7_common_l3.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "l7_dhcpsinclude.h"

/*************************************************************

 *************************************************************/

extern L7_dhcpsMapCfg_t *pDhcpsMapCfgData;
extern L7_dhcpsMapLeaseCfg_t *pDhcpsMapLeaseCfgData;
extern dhcpsInfo_t      *pDhcpsInfo;
extern dhcpsAsyncMsg_t  *pDhcpsAsyncInfo;
extern dhcpsMapCtrl_t   dhcpsMapCtrl_g;

extern void dhcpsProcTask(void);

void *      dhcpsProcQueue;             /* DHCPS Processing queue */
L7_int32    dhcpsProcTaskHandle;    /* DHCPS Processing task handle */

L7_char8 *dhcpsCnfgrPhaseNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
  "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};

L7_BOOL traceDHCPSConfigState = L7_FALSE;
dhcpsCnfgrState_t dhcpsCnfgrState = DHCPS_PHASE_INIT_0;/* To be init'ed to INIT_0 */

static L7_RC_t dhcpsProcessingTaskCreate(void);
static L7_RC_t dhcpsCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
    L7_CNFGR_RESPONSE_t *response,
    L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t dhcpsUnconfigRequestHandle(L7_CNFGR_RQST_t request,
    L7_CNFGR_RESPONSE_t *response,
    L7_CNFGR_ERR_RC_t *reason);


/*********************************************************************
 *
 * @purpose  Enabling tracing of configuration state changes.
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void dhcpsCnfgrStateTraceEnable(void)
{
  traceDHCPSConfigState = L7_TRUE;
}

/*********************************************************************
 *
 * @purpose  Disabling tracing of configuration state changes.
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void dhcpsCnfgrStateTraceDisable(void)
{
  traceDHCPSConfigState = L7_FALSE;
}

/*********************************************************************
 *
 * @purpose  Set the DHCPS configuration state
 *
 * @param    newState - @b{(input)}  The configuration state that DHCPS
 *                                   is transitioning to.
 *
 * @returns  void
 *
 * @notes    Set traceDHCPSConfigState to L7_TRUE to trace configuration state
 *           transitions.
 *
 * @end
 *********************************************************************/
static void dhcpsCnfgrStateSet(dhcpsCnfgrState_t newState)
{
  /* ksriniv: Check if the correct logging flag has been used */
  if (traceDHCPSConfigState == L7_TRUE) {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
    "DHCPS configuration state set to %s\n",
    dhcpsCnfgrPhaseNames[newState]);
  }
  dhcpsCnfgrState = newState;
}


/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair.
 *
 * @param    pCmdData  - @b{(input)}  command to be processed.
 *
 * @returns  None
 *
 * @notes    This function completes synchronously. The return value
 *           is presented to the configurator by calling the cnfgrApiCallback().
 *           The following are the possible return codes:
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_CB_ERR_RC_INVALID_RQST
 *           L7_CNFGR_CB_ERR_RC_INVALID_CMD
 *           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
 *           L7_CNFGR_ERR_RC_INVALID_PAIR
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread.
 *
 * @notes    Handles only Phase 1 initialization on the configurator thread.
 *           Enqueues all other command/request pairs for processing task to handle.
 *
 * @end
 *********************************************************************/
void dhcpsApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  dhcpsProcMsg_t msg;           /* message to pass request to DHCPS processing task */
  L7_CNFGR_CB_DATA_t cbData;  /* indicates response to correlator */

  /* validate command type */
  if ( pCmdData == L7_NULL )
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  /* Do minimum amount of work on configurator's thread. Pass other work
   * to DHCPS processing task.
   */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the DHCPS processing task, then pass
     * a message via the queue to the processing task to complete
     * phase 1 initialization.
     */

    if (dhcpsProcessingTaskCreate() != L7_SUCCESS) {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.type.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = DHCPS_CNFGR_CMD;
  (void)osapiMessageSend(dhcpsProcQueue, &msg, sizeof(dhcpsProcMsg_t),
             L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return;
}

/*********************************************************************
 * @purpose  Create the DHCPS processing task and processing message
 *           queue. The processing task performs initialization tasks
 *           at the request of the configurator and handles other events.
 *
 * @param    void
 *
 * @returns  L7_SUCCESS - The thread and queue were successfully created
 * @returns  L7_FAILURE - otherwise
 *
 * @notes    This function runs in the configurator's thread and should
 *           avoid blocking.
 *
 * @end
 *********************************************************************/
static L7_RC_t dhcpsProcessingTaskCreate(void)
{
  /* create DHCPS processing message queue */
  dhcpsProcQueue = osapiMsgQueueCreate(DHCPS_PROC_QUEUE,
      DHCPS_PROC_MSG_COUNT,
      sizeof(dhcpsProcMsg_t));
  if (dhcpsProcQueue == L7_NULLPTR) {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create DHCPS processing message queue");
    return L7_FAILURE;
  }

  dhcpsCnfgrStateTraceDisable();
  /* Create mapping layer thread */
  dhcpsProcTaskHandle = osapiTaskCreate(DHCPS_PROC_TASK,
      dhcpsProcTask,
      L7_NULL, L7_NULLPTR,
      L7_DEFAULT_STACK_SIZE,
      L7_DEFAULT_TASK_PRIORITY,
      L7_DEFAULT_TASK_SLICE);

  if (dhcpsProcTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create DHCPS processing task");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_DHCPS_PROC_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to syncronize with DHCP Server processing task");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  This function processes the configurator control commands/request
 *           pair Init Phase 1. Init phase 1 mainly invloves allocation of memory,
 *           creation of semaphores, message queues, tasks and other resources
 *           used by the protocol
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
 * @notes    This function runs in the configurator's thread.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  /*-----------------------------------------------------------*/
  /* malloc space for the dhcpsMap configuration data structure  */
  /*-----------------------------------------------------------*/
  pDhcpsMapCfgData = (L7_dhcpsMapCfg_t *)osapiMalloc(L7_DHCPS_MAP_COMPONENT_ID, sizeof(L7_dhcpsMapCfg_t));
  if (pDhcpsMapCfgData == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to allocate memory for DHCP Server configuration structure");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;

  }
  pDhcpsMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  /*-----------------------------------------------------------*/
  /* malloc space for the dhcpsMap lease configuration data structure  */
  /*-----------------------------------------------------------*/
  pDhcpsMapLeaseCfgData = (L7_dhcpsMapLeaseCfg_t *)osapiMalloc(L7_DHCPS_MAP_COMPONENT_ID, sizeof(L7_dhcpsMapLeaseCfg_t));
  if (pDhcpsMapLeaseCfgData == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to allocate memory for DHCP Server"
           " lease configuration structure");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;

  }
  pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_FALSE;

  /*----------------------------------------------*/
  /* malloc space for various DHCP Server info structures */
  /*----------------------------------------------*/
  pDhcpsInfo = (dhcpsInfo_t *)osapiMalloc(L7_DHCPS_MAP_COMPONENT_ID, sizeof(dhcpsInfo_t));
  if (pDhcpsInfo == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to allocate memory for DHCP Server info structure");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
  memset(pDhcpsInfo, 0, sizeof(dhcpsInfo_t));

  /*---------------------------------------------------------------*/
  /* Create async message list to store the DHCP DISCOVER messages */
  /* for asynchronous processing                                   */
  /*---------------------------------------------------------------*/
  pDhcpsAsyncInfo = (dhcpsAsyncMsg_t *)osapiMalloc(L7_DHCPS_MAP_COMPONENT_ID,
                                       L7_DHCPS_MAX_ASYNC_MSGS * sizeof(dhcpsAsyncMsg_t));
  if (pDhcpsAsyncInfo == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to allocate memory for DHCP Async message list");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
  memset(pDhcpsAsyncInfo, 0, L7_DHCPS_MAX_ASYNC_MSGS * sizeof(dhcpsAsyncMsg_t));

  if(dhcpsMapAppsInit() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  dhcpsCnfgrStateSet(DHCPS_PHASE_INIT_1);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Init Phase 2. Phase 2 mainly involves callback registration.
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
 * @notes    This function runs on the processing task.
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  nvStoreFunctionList_t   dhcpsNotifyFunctionList;
  sysnetPduIntercept_t    sysnetPduIntercept;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  memset((void *) &dhcpsNotifyFunctionList, 0, sizeof(nvStoreFunctionList_t));
  dhcpsNotifyFunctionList.registrar_ID   = L7_DHCPS_MAP_COMPONENT_ID;
  dhcpsNotifyFunctionList.notifySave     = dhcpsSave;
  dhcpsNotifyFunctionList.hasDataChanged = dhcpsHasDataChanged;
  dhcpsNotifyFunctionList.resetDataChanged = dhcpsResetDataChanged;

  if(nvStoreRegister(dhcpsNotifyFunctionList) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to register with nvStore");
    return L7_FAILURE;
  }

  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_LOCAL_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCPS_FILTER_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = dhcpsMapFrameFilter;
  strcpy(sysnetPduIntercept.interceptFuncName, "dhcpsMapFrameFilter");
  /* Register with sysnet */
  if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to register with sysnet for intercept on PDU receive");
    return L7_FAILURE;
  }

  dhcpsCnfgrStateSet(DHCPS_PHASE_INIT_2);
  return L7_SUCCESS;

}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Init Phase 3. Phase 3 involves applying building and
 *           applying the configuration onto the hardware.
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
L7_RC_t dhcpsCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
    if (sysapiCfgFileGet(L7_DHCPS_MAP_COMPONENT_ID, L7_DHCPS_CFG_FILENAME,
                         (L7_char8 *)pDhcpsMapCfgData, sizeof(L7_dhcpsMapCfg_t),
                         &pDhcpsMapCfgData->checkSum, L7_DHCPS_CFG_VER_CURRENT,
                         dhcpsBuildDefaultConfigData, L7_NULL) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
               "Failed to read saved configuration file");
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        return L7_ERROR;
    }

    /* get lease data */
    if (sysapiCfgFileGet(L7_DHCPS_MAP_COMPONENT_ID, L7_DHCPS_LEASE_CFG_FILENAME,
                  (L7_char8 *)pDhcpsMapLeaseCfgData, sizeof(L7_dhcpsMapLeaseCfg_t),
                  &pDhcpsMapLeaseCfgData->checkSum, L7_DHCPS_LEASE_CFG_VER_CURRENT,
                  dhcpsBuildDefaultLeaseConfigData, dhcpsMigrateLeaseConfigData ) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
               "Failed to read lease configuration file");
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        return L7_ERROR;
    }

    dhcpsInitializeOptionValidationData(pDhcpsInfo->dhcpsOptionInfo);

    if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
    {
          /* apply config data */
      if(dhcpsApplyConfigData() != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                 "Failed to apply configuration");
         *pResponse = 0;
         *pReason   = L7_CNFGR_ERR_RC_FATAL;
         return L7_ERROR;
      }
    }
    else
    {
          /* check validity of admin mode */
       if (! (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_ENABLE ||
            pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_DISABLE) )
       {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                 "Invalid admin mode is specified");
          return L7_FAILURE;
       }
       dhcpsInitMappingLeaseConfigData();
    }

    dhcpsCnfgrStateSet(DHCPS_PHASE_INIT_3);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes dhcpsCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsCnfgrFiniPhase1Process()
{

}

/*********************************************************************
 * @purpose  This function undoes dhcpsCnfgrInitPhase2Process, i.e.
 *           deregister from all callbacks.
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dhcpsCnfgrFiniPhase2Process()
{

}

/*********************************************************************
 * @purpose  This function undoes dhcpsCnfgrInitPhase3Process
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    dhcpsCnfgrUnconfigPhase2 unapplies the configuration.
 *           Hence, is reused to undo INIT_3
 *
 * @end
 *********************************************************************/
void dhcpsCnfgrFiniPhase3Process()
{

}

/*********************************************************************
 * @purpose  This function processes the configurator control commands/request
 *           pair Unconfigure Phase 2. Mainly unapplies the current
 *           configuration
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
 *           dhcpsRestoreProcess only builds default configuration and
 *           applies it, while UNCONFIG_2 requires configuration information
 *           to be reset. Hence, dhcpsCfgData is reset to 0.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  rc = dhcpsRestoreProcess();

  dhcpsCnfgrStateSet(DHCPS_PHASE_WMU);
  return rc;

}

/*********************************************************************
 *
 * @purpose  Handles initialization messages from the configurator.
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
void dhcpsCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response;
  L7_RC_t dhcpsProcRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  if (pCmdData->type != L7_CNFGR_RQST)
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

  switch ( command )
  {
    case L7_CNFGR_CMD_INITIALIZE:
      dhcpsProcRC = dhcpsCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      switch ( request )
      {
    case L7_CNFGR_RQST_E_START:
      dhcpsCnfgrStateSet(DHCPS_PHASE_EXECUTE);
      dhcpsProcRC  = L7_SUCCESS;
      response  = L7_CNFGR_CMD_COMPLETE;
      reason    = 0;
      break;

    default:
      /* invalid command/request pair */
      reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      dhcpsProcRC = dhcpsUnconfigRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      dhcpsProcRC = dhcpsCnfgrNoopProcess( &response, &reason );
      break;

    default:
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
  } /* endswitch command/event pair */

  /* respond to configurator */
  cbData.asyncResponse.rc = dhcpsProcRC;
  if (dhcpsProcRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason = reason;

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
static L7_RC_t dhcpsCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
    L7_CNFGR_RESPONSE_t *response,
    L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch ( request )
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = dhcpsCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
    /* Configurator will reset router, so no need to take
     * any action.
     */
    /* dhcpsCnfgrFiniPhase1Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = dhcpsCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
    /* Configurator will reset router, so no need to take
     * any action.
     */
    /*  dhcpsCnfgrFiniPhase2Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = dhcpsCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
    /* Configurator will reset router, so no need to take
     * any action.
     */
    /*  dhcpsCnfgrFiniPhase3Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = dhcpsCnfgrNoopProcess(response, reason);
      dhcpsCnfgrStateSet(DHCPS_PHASE_WMU);
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
 *
 * @purpose  Handles an unconfigure request from the configurator.
 *
 * @param @b{(input)} request - indicates the level to which DHCPS should
 *                              be unconfigured
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
static L7_RC_t dhcpsUnconfigRequestHandle(L7_CNFGR_RQST_t request,
    L7_CNFGR_RESPONSE_t *response,
    L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch ( request )
  {
    case L7_CNFGR_RQST_U_PHASE1_START:
      rc = dhcpsCnfgrNoopProcess(response, reason);
      dhcpsCnfgrStateSet(DHCPS_PHASE_UNCONFIG_1);
      break;

    case L7_CNFGR_RQST_U_PHASE2_START:
      rc = dhcpsCnfgrUnconfigPhase2(response, reason);
      dhcpsCnfgrStateSet(DHCPS_PHASE_UNCONFIG_2);
      /* we need to do something with the stats in the future */
      break;

    default:
      /* invalid command/request pair */
      rc = L7_ERROR;
      *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;
  }
  return rc;
}

/*********************************************************************
 * @purpose  Serves as a no-op for configurator stages that require
 *           no action for DHCPS. Simply sets pResponse to L7_CNFGR_CMD_COMPLETE
 *           and returns L7_SUCCESS;
 *
 * @param    pResponse - @b{(output)}  Always set to L7_CNFGR_CMD_COMPLETE
 *
 * @param    pReason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           None.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
}

