/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  sntp_cnfgr.c
*
* @purpose   Implements the stacking initialization sequence
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
*            None the state changes are enforced by the SNTP component; the main
*            configurator ensures this.
*
* @create    1/16/2004
*
* @author    McLendon
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "l7_ip_api.h"
#include "log.h"
#include "sysapi_hpc.h"

#include "sntp_cfg.h"
#include "sntp_api.h"
#include "sntp_client.h"

/** Configuration task parameters. */

#define L7_SNTP_CFG_TASK                "SNTPC"
#define L7_SNTP_CFG_QUEUE               "SNTPQ"

/** The SntpCfg structure contains the system level persistent data definition. */

static struct sntpCfg_s * sntpCfg = L7_NULL;


/* Types of message passed to the SNTP cfg task
   via the SNTP processing queue */
typedef enum
{
    SNTP_CNFGR_CMD = 0,
    SNTP_ADMIN_MODE,
    SNTP_RESTORE_EVENT,
    SNTP_SAVE_EVENT
} sntpCfgMessage_t;

/* Message received by SNTP processing task through SNTP processing queue  */

typedef struct
{
    sntpCfgMessage_t msgType;
    union
    {
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;
} sntpCfgMsg_t;


/* SNTP Cfg task */
static L7_int32  sntpCfgTaskId = 0;

/* SNTP Cfg queue */
static void *      sntpCfgQueue = L7_NULL;

/* Different phases of the unit */
typedef enum {
  SNTP_PHASE_INIT_0 = 0,
  SNTP_PHASE_INIT_1,
  SNTP_PHASE_INIT_2,
  SNTP_PHASE_WMU,
  SNTP_PHASE_INIT_3,
  SNTP_PHASE_EXECUTE,
  SNTP_PHASE_UNCONFIG_1,
  SNTP_PHASE_UNCONFIG_2,
  SNTP_PHASE_LAST_ELEMENT
} sntpCnfgrState_t;

static L7_char8 *sntpCnfgrPhaseNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
  "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};

static L7_BOOL traceSNTPConfigState = L7_FALSE;
static sntpCnfgrState_t sntpCnfgrState = SNTP_PHASE_INIT_0;/* To be init'ed to INIT_0 */


static L7_RC_t sntpCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                          L7_CNFGR_RESPONSE_t *response,
                                          L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t sntpUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t sntpCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason);
/**
* SECTION: Configuration support utilities
*
*
*/
/**********************************************************************
* @purpose  Fills out the Config data header with defaults
*
* @param    ver @b{(input)} version of the config data
*
* @returns  None.
*
* @notes    none
*
* @end
*********************************************************************/

void sntpCfgHeaderDefaultsSet(L7_uint32 ver)
{
  if (sntpCfg == L7_NULL)
    return;
    /** setup file header */
  sntpCfg->hdr.version = ver;
  sntpCfg->hdr.componentID = L7_SNTP_COMPONENT_ID;
  sntpCfg->hdr.type = L7_CFG_DATA;
  sntpCfg->hdr.length = (L7_uint32)sizeof(struct sntpCfg_s);
  strcpy((L7_char8 *)sntpCfg->hdr.filename, L7_SNTP_CFG_FILENAME);
  sntpCfg->hdr.dataChanged = L7_FALSE;
}


/**********************************************************************
* @purpose  Marks cfg data as changed
*
* @notes
*
* @end
*********************************************************************/

void sntpCfgMarkDataChanged()
{
  sntpCfg->hdr.dataChanged = L7_TRUE;
}

/**********************************************************************
* @purpose Terminate all SNMP processing, free all allocated data,
*           and terminate all threads.
*
* @returns  Pointer to the cfg data
*
* @notes
*
* @end
*********************************************************************/

void sntpTerminate()
{
  sntpClientTerminate();
}
/**********************************************************************
* @purpose  Checks if SNTP config data has changed
*
* @param    void
*
* @returns  L7_TRUE if changed or L7_FALSE if not
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL sntpCfgHasDataChanged(void)
{
  return sntpCfg->hdr.dataChanged;
}
void sntpCfgResetDataChanged(void)
{
  sntpCfg->hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Saves sntp configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    sntpCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t sntpSave(void)
{
  L7_RC_t rc= L7_SUCCESS;

  if (sntpCfg->hdr.dataChanged == L7_TRUE)
  {
    sntpTaskLock();
    sntpCfg->checkSum = nvStoreCrc32((L7_uchar8 *)sntpCfg,
            sizeof(struct sntpCfg_s) - sizeof(sntpCfg->checkSum));
    /* call save NVStore routine */
    if ((rc = sysapiCfgFileWrite(L7_SNTP_COMPONENT_ID, L7_SNTP_CFG_FILENAME ,
                        (L7_char8 *)sntpCfg, sizeof(struct sntpCfg_s))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n",
                      L7_SNTP_CFG_FILENAME);
    }
    sntpCfg->hdr.dataChanged = L7_FALSE;
    sntpTaskUnlock();
  }
  return rc;
}
/*********************************************************************
* @purpose  Build default SNTP config data
*
* @param    ver  @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void sntpBuildDefaultConfigData(L7_uint32 ver)
{
  sntpCfgHeaderDefaultsSet(ver);
  sntpClientDefaultsSet(ver);
}

/*********************************************************************
* @purpose  Restores sntp configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    sntpCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t sntpRestore(void)
{
  sntpBuildDefaultConfigData(L7_SNTP_CFG_VER_1);
  sntpCfg->hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

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
void sntpCnfgrStateTraceEnable(void)
{
  traceSNTPConfigState = L7_TRUE;
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
static void sntpCnfgrStateTraceDisable(void)
{
  traceSNTPConfigState = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Set the SNTP configuration state
*
* @param    newState @b{(input)}  The configuration state that SNTP
*                                   is transitioning to.
*
* @returns  void
*
* @notes    Set traceSNTPConfigState to L7_TRUE to trace configuration state
*           transitions.
*
* @end
*********************************************************************/
static void sntpCnfgrStateSet(sntpCnfgrState_t newState)
{
  if (traceSNTPConfigState == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "SNTP configuration state set to %s\n",
                  sntpCnfgrPhaseNames[newState]);
  }
  sntpCnfgrState = newState;
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
static L7_RC_t sntpCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  sntpBuildDefaultConfigData(L7_SNTP_CFG_VER_CURRENT);
  sntpCnfgrStateSet(SNTP_PHASE_INIT_1);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2. Phase 2 mainly involves callback registration.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS  There were no errors. Response is available.
* @returns  L7_ERROR    There were errors. Reason code is available.
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
static L7_RC_t sntpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason)
{
  nvStoreFunctionList_t   sntpNotifyFunctionList;
  memset((void *) &sntpNotifyFunctionList, '\0', sizeof(nvStoreFunctionList_t));
  sntpNotifyFunctionList.registrar_ID   = L7_SNTP_COMPONENT_ID;
  sntpNotifyFunctionList.notifySave     = sntpSave;
  sntpNotifyFunctionList.hasDataChanged = sntpCfgHasDataChanged;
  sntpNotifyFunctionList.resetDataChanged = sntpCfgResetDataChanged;

  if (nvStoreRegister(sntpNotifyFunctionList))
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "sntpCnfgrInitPhase2Process: Could not register with nvStore\n");
    return L7_ERROR;
  }

#if defined(L7_IPV6_PACKAGE)
  /* Register for IPv6 routing interface events */
  if (ip6MapRegisterRoutingEventChange(L7_IPRT_SNTP,
                                       "sntpIPv6RoutingEventChangeCallBack",
                                       sntpIPv6RoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    L7_LOGF(L7_SNTP_COMPONENT_ID, L7_LOG_SEVERITY_ERROR,
            "Error registering with IPv6 for routing events.");
    return L7_ERROR;
  }
#endif

  sntpCnfgrStateSet(SNTP_PHASE_INIT_2);
#if defined (L7_STACKING_PACKAGE)
  sysapiHpcReceiveCallbackRegister((void *)sntpHandleStackBroadcast, L7_SNTP_COMPONENT_ID);
#endif

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3. Phase 3 involves applying building and
*           applying the configuration onto the hardware.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_ERROR   There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
static L7_RC_t sntpCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason )
{
    if (sysapiCfgFileGet(L7_SNTP_COMPONENT_ID, L7_SNTP_CFG_FILENAME,
                         (L7_char8 *)sntpCfg, sizeof (struct sntpCfg_s),
                         &(sntpCfg->checkSum), L7_SNTP_CFG_VER_CURRENT,
                         sntpBuildDefaultConfigData, sntpMigrateConfigData) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "%s: %d: sntpCnfgrInitPhase3Process: SNTP Cfg unable to read \
              config file", __FILE__, __LINE__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  sntpCnfgrStateSet(SNTP_PHASE_INIT_3);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes sntpCnfgrInitPhase1Process
*
* @notes    Disable the sntp client and close the local socket.
*           We want to force the main client loop back to
*           the top of the loop.
*           We also reset the configuration back to the defaults.
*
* @end
*********************************************************************/
static void sntpCnfgrFiniPhase1Process()
{
  /* Nothing to do. */
}

/*********************************************************************
* @purpose  This function undoes sntpCnfgrInitPhase2Process, i.e.
*           deregister from all callbacks.
*
*
* @end
*********************************************************************/
static void sntpCnfgrFiniPhase2Process()
{
  nvStoreDeregister(L7_SNTP_COMPONENT_ID);
}
/*********************************************************************
* @purpose  This function undoes sntpCnfgrInitPhase3Process, i.e.
*           deregister from all callbacks.
*
*
* @end
*********************************************************************/
static void sntpCnfgrFiniPhase3Process()
{
  sntpBuildDefaultConfigData(L7_SNTP_CFG_VER_CURRENT);
  sntpClientModeSet(L7_SNTP_CLIENT_DISABLED);
  sntpLocalSocketClose();
}
/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current
*           configuration
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_ERROR   There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
*           sntpRestore builds default configuration and
*           applies it, while UNCONFIG_2 requires configuration information
*           to be reset. Hence, sntpCfgData is reset to defaults.
*
* @end
*********************************************************************/
static L7_RC_t sntpCnfgrUnconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;

  rc = sntpRestore();
  sntpClientModeSet(L7_SNTP_CLIENT_DISABLED);
  sntpCnfgrStateSet(SNTP_PHASE_WMU);
  return rc;

}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Unconfigure Phase 1. Mainly unapplies the current
*           configuration
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_ERROR   There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
*           sntpRestore builds default configuration and
*           applies it, while UNCONFIG_2 requires configuration information
*           to be reset. Hence, sntpCfgData is reset to defaults.
*
* @end
*********************************************************************/
static L7_RC_t sntpCnfgrUnconfigPhase1(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason)
{
    return sntpCnfgrNoopProcess(pResponse, pReason);
}


/*********************************************************************
* @purpose  Serves as a no-op for configurator stages that require
*           no action for SNTP. Simply sets pResponse to L7_CNFGR_CMD_COMPLETE
*           and returns L7_SUCCESS;
*
* @param    pResponse @b{(output)}  Always set to L7_CNFGR_CMD_COMPLETE
*
* @param    pReason   @b{(output)}  Always 0
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
static L7_RC_t sntpCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t *pReason)
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Handles configuration messages from the configurator.
*
* @param    pCmdData @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
static void sntpCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response = L7_CNFGR_INVALID_RESPONSE;
  L7_RC_t sntpProcRC = L7_ERROR;
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
    sntpProcRC = sntpCnfgrInitRequestHandle(request, &response, &reason);
    break;

  case L7_CNFGR_CMD_EXECUTE:
    switch ( request )
    {
    case L7_CNFGR_RQST_E_START:
      sntpCnfgrStateSet(SNTP_PHASE_EXECUTE);
      sntpProcRC  = L7_SUCCESS;
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
    sntpProcRC = sntpUnconfigRequestHandle(request, &response, &reason);
    break;

  case L7_CNFGR_CMD_TERMINATE:
  case L7_CNFGR_CMD_SUSPEND:
    sntpProcRC = sntpCnfgrNoopProcess( &response, &reason );
    break;

  default:
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    break;
  } /* endswitch command/event pair */

  /* respond to configurator */
  cbData.asyncResponse.rc = sntpProcRC;
  if (sntpProcRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason = reason;

  cnfgrApiCallback(&cbData);
}

/*********************************************************************
*
* @purpose  Handles an initialization request from the configurator.
*
* @param request @b{(input)} indicates the initialization phase to
*                              be executed
* @param response @b{(output)} response if request successfully handled
* @param reason @b{(output)} if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t sntpCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                          L7_CNFGR_RESPONSE_t *response,
                                          L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch ( request )
  {
  case L7_CNFGR_RQST_I_PHASE1_START:
    rc = sntpCnfgrInitPhase1Process(response, reason);
    if (rc != L7_SUCCESS)
    {
      sntpCnfgrFiniPhase1Process();
    }
    break;
  case L7_CNFGR_RQST_I_PHASE2_START:
    rc = sntpCnfgrInitPhase2Process(response, reason);
    if (rc != L7_SUCCESS)
    {
      sntpCnfgrFiniPhase2Process();
    }
    break;
  case L7_CNFGR_RQST_I_PHASE3_START:
    rc = sntpCnfgrInitPhase3Process(response, reason);
    if (rc != L7_SUCCESS)
    {
      sntpCnfgrFiniPhase3Process();
    }
    break;
  case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
    rc = sntpCnfgrNoopProcess(response, reason);
    sntpCnfgrStateSet(SNTP_PHASE_WMU);
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
* @param request @b{(input)} indicates the level to which SNTP should
*                              be unconfigured
* @param response @b{(output)} response if request successfully handled
* @param reason @b{(output)} if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t sntpUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch (request)
  {
  case L7_CNFGR_RQST_U_PHASE1_START:
    rc = sntpCnfgrUnconfigPhase1(response, reason);
    sntpCnfgrStateSet(SNTP_PHASE_UNCONFIG_1);
    break;

  case L7_CNFGR_RQST_U_PHASE2_START:
    rc = sntpCnfgrUnconfigPhase2(response, reason);
    sntpCnfgrStateSet(SNTP_PHASE_UNCONFIG_2);
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
* @purpose  The SNTP cfg task uses this function to
*           process messages from the message queue, performing the tasks
*           specified in each message. The motivation for this
*           task is to offload configuration processing from the
*           SNTP client.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void sntpCfgTask()
{
  L7_uint32 status;
  sntpCfgMsg_t message;


  osapiTaskInitDone(L7_SNTP_CFG_TASK_SYNC);

  /* Loop forever, processing incoming messages */
  while (L7_TRUE)
  {
    memset(&message, '\0', sizeof(sntpCfgMsg_t));
    status = osapiMessageReceive(sntpCfgQueue, &message,
                                 sizeof(sntpCfgMsg_t), L7_WAIT_FOREVER);

    if (status == L7_SUCCESS)
    {
      switch (message.msgType)
      {
      case SNTP_CNFGR_CMD:
        sntpCnfgrHandle(&message.type.cmdData);
        break;

      case SNTP_RESTORE_EVENT:
        sntpRestore();
        break;

      case SNTP_SAVE_EVENT:
        sntpSave();
        break;

      default:
        break;
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
              "sntpCfgTask: Bad status on receive message from cfg queue %08x", status);
    }
  } /* end while */
}


/*********************************************************************
* @purpose  Create the SNTP cfg task and cfg message queue.
*           The cfg task performs initialization tasks
*           at the request of the configurator and handles other events.
*
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/
static L7_RC_t sntpCfgTaskCreate()
{
  L7_CNFGR_CB_DATA_t    cbData;
  if (sntpCfg == L7_NULL)
  {
    sntpCfg = osapiMalloc(L7_SNTP_COMPONENT_ID, sizeof(struct sntpCfg_s));
  }
  if (sntpCfg == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "%s: %d: sntpCfgTaskCreate: Error allocating CFG data\n",
            __FILE__, __LINE__);
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (sntpClientInit(&(sntpCfg->cfg)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "sntpCfgTaskCreate: Error starting SNTP Client");
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_ERROR;
  }

  /* create SNTP processing message queue */
  sntpCfgQueue = osapiMsgQueueCreate(L7_SNTP_CFG_QUEUE,
                                     L7_SNTP_MSG_COUNT,
                                     sizeof(sntpCfgMsg_t));
  if (sntpCfgQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "sntpCfgTaskCreate: SNTP unable to create message queue");
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  sntpCnfgrStateTraceDisable();
  /* Create configurator layer thread */
  sntpCfgTaskId = osapiTaskCreate(L7_SNTP_CFG_TASK,
                                  sntpCfgTask,
                                  L7_NULL, L7_NULLPTR,
                                  L7_SNTP_DEFAULT_STACK_SIZE,
                                  L7_SNTP_DEFAULT_TASK_PRI,
                                  L7_SNTP_DEFAULT_TASK_SLICE);

  if (sntpCfgTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "sntpCfgTaskCreate: Unable to create SNTP processing task\n");
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_SNTP_CFG_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNTP_COMPONENT_ID,
            "sntpCfgTaskCreate: Unable to syncronize with SNTP Cfg processing task");
    return L7_FALSE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Deallocate the SNTP cfg task and cfg message queue.
*
* @notes    This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/

void sntpCfgTerminate()
{
  if (sntpCfgTaskId != 0)
  {
    osapiTaskDelete(sntpCfgTaskId);
    sntpCfgTaskId = 0;
  }
  if (sntpCfgQueue != L7_NULL)
  {
    osapiMsgQueueDelete(sntpCfgQueue);
    sntpCfgQueue = L7_NULL;
  }
  /* TODO - Handle deallocation of the cfg data */
}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
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
void sntpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  sntpCfgMsg_t msg;           /* message to pass request to SNTP processing task */
  L7_CNFGR_CB_DATA_t cbData;  /* indicates response to correlator */

  /* validate command type */
  if (pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  /* Do minimum amount of work on configurator's thread. Pass other work
   * to SNTP cfg task.
   */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the SNTP processing task, then pass
     * a message via the queue to the processing task to complete
     * phase 1 initialization.
     */

    if (sntpCfgTaskCreate() != L7_SUCCESS)
    {
      sntpTerminate();
      return;
    }
  }
  /* Default is to fall thru to here and pass a message to the cfg task. */
  memcpy(&msg.type.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = SNTP_CNFGR_CMD;
  (void)osapiMessageSend(sntpCfgQueue, &msg, sizeof(sntpCfgMsg_t),
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
}

