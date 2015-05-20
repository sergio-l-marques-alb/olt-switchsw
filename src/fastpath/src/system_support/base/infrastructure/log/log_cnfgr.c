/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  log_cnfgr.c
*
* @purpose   Implements the stacking initialization sequence
*
* @component LOG
*
* @comments   The main configurator dictates what sequence of state
*             changes by using the apt command/request pair. The
*             current initialization sequence for non-management units
*             is: INIT_0, INIT_1, INIT_2 and WMU.
*             Management units have the sequence: INIT_0, INIT_1, INIT_2,
*             WMU and INIT_3. After the management unit (MU) has
*             transitioned into INIT_3 the configurator issues a
*             command to all units (MU and non-MU) to move into the
*             EXECUTE phase.
*
*             When a "clear config" command is issued, the sequence
*             of state changes is: EXECUTE,UNCONFIG_1, UNCONFIG_2, WMU.
*             The MU then goes into INIT_3 and subsequently all
*             units go into EXECUTE phase.
*
*             None the state transitions are enforced by the LOG
*             component; the main configurator ensures the order
*             of execution.
*
*             Logging implements the configuration state changes
*             somewhat differently than other system components.
*             Logging is started from the BSP startup code so
*             that it is available to all components during
*             the startup phases. During this time,
*             logging is performed locally (since all stackables
*             are the "master"). When the master of the stack
*             is established, log messages are forwarded to
*             the master in addition to being logged locally.
*             The advantage of this approach is that the
*             eventual stack master will have all local messages
*             logged and messages on stack units subsequent to
*             entering EXECUTE are logged on the stack master.
*
*             In order to avoid issues with logging, errors
*             in logging are written to the console instead
*             of being "logged."
*
* @create    2/3/2004
*
* @author    McLendon
*
* @end
*
**********************************************************************/
#if defined _L7_OS_VXWORKS_
#include <taskLib.h>
#endif
#include "l7_common.h"
#include "l7utils_api.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "l7_ip_api.h"
#include "sysapi_hpc.h"
#include "sysapi.h"
#include "flex.h"
#include "simapi.h"
#if defined (L7_STACKING_PACKAGE)
#include "fftp_api.h"
#endif

#include "log_api.h"
#include "log.h"
#include "log_ext.h"
#include "log_cfg.h"
#include "log_debug.h"
#include "log_server.h"

#if L7_FEAT_DNI8541_BLADESERVER
#include "bspapi_blade.h"
#endif

#define LOG_CNFGR_QUEUE    "LogCfgQ"
#define LOG_CNFGR_TASK     "LOGC"
#define LOG_CNFGR_MSG_CNT  16

L7_RC_t emailAlertTimerStop(void);
extern void add_flash_log_entry(struct logformat_event *log_entry,
                                L7_ulong32 int_context);

#ifdef _L7_OS_VXWORKS_
#if defined(CPU_FAMILY) && (CPU_FAMILY == PPC)
extern void osapiLogStackTraceFlash(L7_char8 *fileName, L7_char8 * format, ...);
extern void osapiSoftwareWatchdogStart(L7_uint32 timePeriod);
#endif
#endif

#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
extern L7_RC_t dot3ahDyingGaspCallBack(L7_uint32 eventNum, L7_uint32 component, L7_BOOL isComponent);
#endif
#endif
void (*logNvStoreFuncPtr)(L7_uchar8 *buf) = L7_NULLPTR;
static void (*logNvRetrieveFuncPtr)(L7_uchar8 *buf) = L7_NULLPTR;

/* The log config task id. */
static L7_int32  logCfgTaskId = 0;

/* The log config queue. */
static void *      logCfgQueue = L7_NULL;

/* Different phases of the log component. */
typedef enum
{
  LOG_PHASE_INIT_0 = 0,
  LOG_PHASE_INIT_1,
  LOG_PHASE_INIT_2,
  LOG_PHASE_WMU,
  LOG_PHASE_INIT_3,
  LOG_PHASE_EXECUTE,
  LOG_PHASE_UNCONFIG_1,
  LOG_PHASE_UNCONFIG_2,
  LOG_PHASE_LAST_ELEMENT
} logCnfgrState_t;

/* Types of message passed to the LOG cfg task
   via the LOG processing queue */
typedef enum
{
    LOG_CNFGR_CMD = 0,
    LOG_ADMIN_MODE,
    LOG_RESTORE_EVENT,
    LOG_SAVE_EVENT
} logCfgMessage_t;


/* Message received by Log config processing task through Log
* processing queue
*/
typedef struct
{
    logCfgMessage_t msgType;
    union
    {
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;

} logCfgMsg_t;


/* This is a pointer to the log configuration data structure. */
static struct logCfg_s  * logCfg = L7_NULL;

/* Names of the phases. */

static L7_char8 *logCnfgrPhaseNames[] = {"INIT 0",
                                          "INIT 1",
                                          "INIT 2",
                                          "WMU",
                                          "INIT 3",
                                          "EXECUTE",
                                          "UNCONFIG 1",
                                          "UNCONFIG 2"};

/* Cfg changes traced? */
static L7_BOOL traceLogConfigState = L7_FALSE;

/* To be init'ed to INIT_0 */
static logCnfgrState_t logCnfgrState = LOG_PHASE_INIT_0;

/* Forward declarations. */

static L7_RC_t logCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                          L7_CNFGR_RESPONSE_t *response,
                                          L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t logUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason);

static L7_RC_t logCfgTaskCreate(void);


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
void logCnfgrStateTraceEnable()
{
  traceLogConfigState = L7_TRUE;
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
void logCnfgrStateTraceDisable()
{
  traceLogConfigState = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Set the LOG configuration state
*
* @param    newState - @b{(input)}  The configuration state that LOG
*                                   is transitioning to.
*
* @returns  void
*
* @notes    Set traceLOGConfigState to L7_TRUE to trace configuration
*           state transitions.
*
* @end
*********************************************************************/
static void logCnfgrStateSet(logCnfgrState_t newState)
{
  if (traceLogConfigState == L7_TRUE)
  {
    sysapiPrintf("logCnfgrStateSet: LOG configuration state set to %s\n",
                 logCnfgrPhaseNames[newState]);
  }
  logCnfgrState = newState;
}

/*********************************************************************
* @purpose  Check whether logging is ready.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL logCnfgrStateCheck (void)
{
  if ((logCnfgrState == LOG_PHASE_EXECUTE) ||
      (logCnfgrState == LOG_PHASE_INIT_3) ||
      (logCnfgrState == LOG_PHASE_UNCONFIG_1))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  This function processes the configurator control
*           commands/request pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.
*
* @returns  None
*
* @notes    This function completes synchronously. The return value
*           is presented to the configurator by calling the
*           cnfgrApiCallback().
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
* @notes    Handles only Phase 1 initialization on the configurator
*           thread. Enqueues all other command/request pairs for
*           processing task to handle.
*
* @end
*********************************************************************/
void logApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  logCfgMsg_t msg;  /* message to pass request to LOG processing task */
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
   * to LOG cfg task.
   */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the LOG processing task, then pass
     * a message via the queue to the processing task to complete
     * phase 1 initialization.
     */

    if (logCfgTaskCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }
  /* Default is to fall thru to here and pass a message to the cfg task. */
  memcpy(&msg.type.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = LOG_CNFGR_CMD;
  (void)osapiMessageSend(logCfgQueue, &msg, sizeof(logCfgMsg_t),
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
}

/*********************************************************************
* @purpose  This function processes the configurator control
*           commands/request pair Init Phase 1. Init phase 1 mainly
*           involves allocation of memory, creation of semaphores,
*           message queues, tasks and other resources
*           used by the protocol. In this component, these items
*           already exist.
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
static L7_RC_t logCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason)
{
  /* logInit is called from the BSP - not from here. */
  logCnfgrStateSet(LOG_PHASE_INIT_1);
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
static L7_RC_t logCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason)
{
#if defined (L7_STACKING_PACKAGE)
  register L7_uint32 ndx;
  L7_char8 fileName[L7_LOG_MAX_FILENAME_LEN];
#endif
  nvStoreFunctionList_t   logNotifyFunctionList;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  memset((void *) &logNotifyFunctionList, '\0',
         sizeof(nvStoreFunctionList_t));
  logNotifyFunctionList.registrar_ID   = L7_LOG_COMPONENT_ID;
  logNotifyFunctionList.notifySave     = logSave;
  logNotifyFunctionList.hasDataChanged = logCfgHasDataChanged;
  logNotifyFunctionList.resetDataChanged = logCfgResetDataChanged;

  if (nvStoreRegister(logNotifyFunctionList) != L7_SUCCESS)
  {
    sysapiPrintf("logCnfgrInitPhase2Process: Could not register with nvStore\n");
    return L7_FAILURE;
  }
#if defined (L7_STACKING_PACKAGE)
  /* register persistent logs with FFTP */
  for (ndx = 0; ndx < L7_LOG_MAX_PERSISTENT_LOGS; ndx++)
  {
    sprintf(fileName, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, ndx);
    fftpFileRegister(fileName, "");
    sprintf(fileName, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, ndx);
    fftpFileRegister(fileName, "");
  }
  /* Always register to receive. Sender "knows" to send to top of stack. */

  sysapiHpcReceiveCallbackRegister(logMsgBuffer, L7_LOG_COMPONENT_ID);
#endif

  logCnfgrStateSet(LOG_PHASE_INIT_2);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Init Phase 3. Phase 3 involves applying building and
*           applying the configuration onto the hardware.
*
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
static L7_RC_t logCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason )
{
  logCnfgrStateSet(LOG_PHASE_INIT_3);

  logConfigRead();

  /* reset the datachanged flag for clear config case, where phase 2 is not executed
     before phase 3*/
  if (logCfg != L7_NULL)
    logCfg->hdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
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
*           Failure of any of the configurator commands does not
*           cause a change in the log tasks. We jes' keep on loggin'.
*
*
* @end
*********************************************************************/
static L7_RC_t logCnfgrUnconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason)
{
  logRestoreDefaults();
  logCnfgrStateSet(LOG_PHASE_WMU);
  emailAlertTimerStop();	
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Serves as a no-op for configurator stages that require
*           no action for LOG. Simply sets pResponse to L7_CNFGR_CMD_COMPLETE
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
static L7_RC_t logCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
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
* @param    pCmdData - @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
static void logCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response = L7_CNFGR_INVALID_RESPONSE;
  L7_RC_t logProcRC = L7_ERROR;
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
      logProcRC = logCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      switch ( request )
      {
        case L7_CNFGR_RQST_E_START:
          logCnfgrStateSet(LOG_PHASE_EXECUTE);
          logProcRC  = L7_SUCCESS;
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
      logProcRC = logUnconfigRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      logProcRC = logCnfgrNoopProcess( &response, &reason );
      break;

    default:
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
  } /* endswitch command/event pair */

  /* respond to configurator */
  cbData.asyncResponse.rc = logProcRC;
  if (logProcRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason = reason;

  cnfgrApiCallback(&cbData);
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
static L7_RC_t logCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch ( request )
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = logCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        /* Configurator will reset router, so no need to take
         * any action.
         */
        /* logCnfgrFiniPhase1Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = logCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        /* Configurator will reset router, so no need to take
         * any action.
         */
        /*  logCnfgrFiniPhase2Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = logCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        /* Configurator will reset router, so no need to take
         * any action.
         */
        /*  logCnfgrFiniPhase3Process(); */
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = logCnfgrNoopProcess(response, reason);
      logCnfgrStateSet(LOG_PHASE_WMU);
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
* @param @b{(input)} request - indicates the level to which LOG should
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
static L7_RC_t logUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch (request)
  {
    case L7_CNFGR_RQST_U_PHASE1_START:
      rc = logCnfgrNoopProcess(response, reason);
      logCnfgrStateSet(LOG_PHASE_UNCONFIG_1);
      break;

    case L7_CNFGR_RQST_U_PHASE2_START:
      rc = logCnfgrUnconfigPhase2(response, reason);
      logCnfgrStateSet(LOG_PHASE_UNCONFIG_2);
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
* @purpose  The LOG cfg task uses this function to
*           process messages from the message queue, performing the tasks
*           specified in each message. The motivation for this
*           task is to offload configuration processing from the
*           LOG client.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void logCfgTask()
{
  L7_uint32 status;
  logCfgMsg_t message;

  osapiTaskInitDone(L7_LOG_CFG_TASK_SYNC);

  /* Loop forever, processing incoming messages */
  while (L7_TRUE)
  {
    memset(&message, '\0', sizeof(logCfgMsg_t));
    status = osapiMessageReceive(logCfgQueue, &message,
                                 sizeof(logCfgMsg_t), L7_WAIT_FOREVER);

    if (status == L7_SUCCESS)
    {
      switch (message.msgType)
      {
        case LOG_CNFGR_CMD:
          logCnfgrHandle(&message.type.cmdData);
          break;

        case LOG_RESTORE_EVENT:
          logRestoreDefaults();
          break;

        case LOG_SAVE_EVENT:
          logSave();
          break;

        default:
          break;
      }
    }
    else
    {
      sysapiPrintf("logCfgTask: Bad status on receive message "
                   "from cfg queue %08x\n", status);
    }
  } /* endwhile */
}

/*********************************************************************
* @purpose  Create the LOG cfg task and cfg message queue.
*           The cfg task performs initialization tasks
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
static L7_RC_t logCfgTaskCreate(void)
{
  /* create LOG processing message queue */
  logCfgQueue = osapiMsgQueueCreate(LOG_CNFGR_QUEUE,
                                    LOG_CNFGR_MSG_CNT,
                                    sizeof(logCfgMsg_t));
  if (logCfgQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  logCnfgrStateTraceDisable();
  /* Create configurator layer thread */
  logCfgTaskId = osapiTaskCreate(LOG_CNFGR_TASK,
                                 logCfgTask,
                                 L7_NULL, L7_NULLPTR,
                                 FD_CNFGR_LOG_DEFAULT_STACK_SIZE,
                                 FD_CNFGR_LOG_DEFAULT_TASK_PRI,
                                 FD_CNFGR_LOG_DEFAULT_TASK_SLICE);

  if (logCfgTaskId == L7_ERROR)
  {
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_LOG_CFG_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    sysapiPrintf("logCfgTaskCreate: Unable to syncronize with LOG Cfg \
                processing task\n");
    return L7_FALSE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Intialize the logging subsystem.
*           Allocate config data here.
*           Start the log server task.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    This function runs in the system startup thread and should
*           avoid blocking. Note that no functions may be called from here
*           that invoke logging. This sets up the logging system with the
*           default configuration. We have no other option because the
*           configurator is not running at this time.
* @end
*********************************************************************/
void loggingInit()
{
  L7_uchar8 log_buf [LOG_CALLOUT_BUF_SIZE];
  L7_uchar8 file_name [LOG_CALLOUT_BUF_SIZE];
  L7_uint32 line_num = 0, err_code;
  L7_uint32 error_type, file_name_size, i;
  L7_uint32 max_file_name;
  L7_char8  buf[L7_LOG_FORMAT_BUF_SIZE];
  L7_clocktime  ct;
  L7_uint32 arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
  struct logformat_event error_record;
  struct logformat_event fault_event_record;
  struct logformat_fault fault_record;

  /* Allocate configuration data */
  logCfg = osapiMalloc(L7_LOG_COMPONENT_ID, sizeof (struct logCfg_s));
  if (logCfg == L7_NULL)
  {
    /* Bad hair day for this box! */
    printf("loggingInit: %s(%d) unable to allocate log configuration\n",
           __FILE__, __LINE__);
    /* We should dump the persistent logs here. */
    return;
  }
  /* Initialize logServer (passing in pointer to configuration data) */
  logServerInit(logCfg);

  logTaskLock();
  logCfgDefaultsSet(L7_LOG_CFG_VER_1);
  logTaskUnlock();

  /* Retrieve log entry from previous reset. If entry exists then
  ** print it on the screen and log it in the event log.
  */
  if (logNvRetrieveFuncPtr != L7_NULLPTR)
  {
    memset (log_buf, 0, LOG_CALLOUT_BUF_SIZE);
    logNvRetrieveFuncPtr (log_buf);

    error_type = *(L7_uint32 *) &log_buf[0];

    if (error_type == L7_LOG_ERROR_FORMAT)
    {
      memcpy (&err_code, &log_buf[4], sizeof (err_code));
      memcpy (&line_num, &log_buf[8], sizeof (line_num));
      max_file_name = LOG_CALLOUT_BUF_SIZE - 13;
      strncpy (file_name, &log_buf[12], max_file_name);

      printf("File: %s, Line: %d, Error %d (0x%x)\n",
             file_name,
             line_num,
             err_code, err_code);

      (void)sprintf(buf, "Error %d (0x%x)", err_code, err_code);

      ct.seconds = simAdjustedTimeGet();

      logmsg(logDefaultFacilityGet(),
             L7_LOG_SEVERITY_ALERT,
             L7_LOG_COMPONENT_DEFAULT,
             ct,
             0,
             (L7_uint32)osapiTaskIdSelf(),
             file_name,
             line_num,
             buf);

      /* Log the previous error in event log */
      error_record.entry_format = LOGFMT_ERROR;
      error_record.line_number = line_num;
      error_record.error_code = err_code;
      error_record.time_stamp = osapiUpTimeRaw();
      error_record.task_id =  osapiTaskIdSelf ();
      file_name_size = strlen(file_name);
      for ( i = 0; i < LOG_FILE_NAME_SIZE; i++ )
      {
        if ( i < (L7_long32)file_name_size )
        {
          error_record.file_name[i] = (L7_uchar8)file_name[i];
        }
        else
        {
          error_record.file_name[i]  = (L7_uchar8)' ';
        }
      }

      add_flash_log_entry (&error_record, L7_FALSE);
    }
    else if (error_type == LOG_FAULT_FORMAT)
    {
      err_code = *(L7_uint32 *) &log_buf[4];
      arg1 = *(L7_uint32 *) &log_buf[8];
      arg2 = *(L7_uint32 *) &log_buf[12];
      arg3 = *(L7_uint32 *) &log_buf[16];
      arg4 = *(L7_uint32 *) &log_buf[20];
      arg5 = *(L7_uint32 *) &log_buf[24];
      arg6 = *(L7_uint32 *) &log_buf[28];
      arg7 = *(L7_uint32 *) &log_buf[32];
      arg8 = *(L7_uint32 *) &log_buf[36];
#if defined(CPU_FAMILY) && defined(PPC) && (CPU_FAMILY == PPC)
      sprintf(buf, "FAULT: vector: 0x%x, srr0: 0x%x, msr: 0x%x, srr1: 0x%x, dmiss: 0x%x, imiss: 0x%x",
              err_code,
              arg1,
              arg2,
              arg3,
              arg4,
              arg5);

      printf("%s\n", buf);
#else
      sprintf(buf, "FAULT: 0x%x, 1: 0x%x, 2: 0x%x, 3: 0x%x, 4: 0x%x, 5: 0x%x, 6: 0x%x, 7: 0x%x 8: 0x%x",
              err_code,
              arg1,
              arg2,
              arg3,
              arg4,
              arg5,
              arg6,
              arg7,
              arg8);

      printf("%s\n", buf);
#endif

      ct.seconds = simAdjustedTimeGet();

      logmsg(logDefaultFacilityGet(),
             L7_LOG_SEVERITY_ALERT,
             L7_LOG_COMPONENT_DEFAULT,
             ct,
             0,
             (L7_uint32)osapiTaskIdSelf(),
             file_name,
             line_num,
             buf);
      /* Log the previous fault in event log */
      fault_record.entry_format = err_code;
      fault_record.time_stamp   = osapiUpTimeRaw();

      fault_record.srr0  = arg1;
      fault_record.srr1  = arg3;
      fault_record.msr   = arg2;
      fault_record.dsisr = arg4;
      fault_record.dar   = arg5;
      /* The fault records "reuses" the event records, but are not structured well for this.
         This is a workaround to account for the differences in sizes of the structures.*/
      memset (&fault_event_record, 0, sizeof(fault_event_record));
      memcpy(&fault_event_record, &fault_record, sizeof(fault_event_record));

      add_flash_log_entry (&fault_event_record,L7_FALSE);
    }
    else
    {
#if 0
      printf("Unknown error type 0x%x\n", error_type);
#endif
      return;
    }

    /* Erase log record.
     */
    memset (log_buf, 0, LOG_CALLOUT_BUF_SIZE);
    if (logNvStoreFuncPtr != L7_NULLPTR)
    {
      logNvStoreFuncPtr (log_buf);
    }
  }
}

/*********************************************************************
* @purpose  BSP uses this routine to register functions that can
*           be used bu the log facility to store and retrieve
*           information for fatal errors.
*
*           The "store" function must be callable from interrupt and
*           fault handlers. Under VxWorks the function shouldn't use
*           any OS services because the OS may be corrupted.
*
* @param    logNvStoreFuncPtrLocal(L7_uchar8 * buf)
* @param    logNvRetrieveFuncPtrLocal (L7_uchar8 * buf)
*
* @returns  none
*
* @notes    The buffer size is defined by LOG_CALLOUT_BUF_SIZE
* @notes    If callouts are not registered then the information
* @notes    about fatal errors is not saved accross resets.
*
* @end
*********************************************************************/
void loggingCalloutsInit(void (*logNvStoreFuncPtrLocal)(L7_uchar8 *buf),
                         void (*logNvRetrieveFuncPtrLocal)(L7_uchar8 *buf))
{
  logNvStoreFuncPtr = logNvStoreFuncPtrLocal;
  logNvRetrieveFuncPtr = logNvRetrieveFuncPtrLocal;
}


#ifdef _L7_OS_VXWORKS_
#if defined(CPU_FAMILY) && (CPU_FAMILY == PPC)

/* Name of the file in which crashdump will be written */
#define L7_LOG_ERROR_CRASHDUMP_FILENAME "log_error_crashdump"

/* Software watchdog will reset the system after this time in case crashdump
 * generation hangs the box
 */
#define L7_LOG_ERROR_SOFTWARE_WATCHDOG_PERIOD (120)  /* secs */


/*********************************************************************
* @purpose  Generate crashdump info for log_error
*
* @param    error_code - 32-bit error code.
* @param    file_name  - File where the error ocurred.
* @param    line_num - Line number where the error occurred.
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void log_error_crashdump_generate(L7_uint32 err_code,
                                  L7_char8 *file_name,
                                  L7_uint32 line_num)
{
  /* Start a sw watchdog that will reset the box incase crashdum generation
   * never finishes.
   */
  osapiSoftwareWatchdogStart(LOG_ERROR_SOFTWARE_WATCHDOG_PERIOD);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Generating log error crashdump file...\n");
  osapiLogStackTraceFlash(LOG_ERROR_CRASHDUMP_FILENAME,
                          "Log error in file %s, line %d, error code %d\n",
                          file_name, line_num, err_code);
}


/*********************************************************************
* @purpose  Print the contents of log error crashdump file
*
* @param    deleteFile {(input)} 1: Delete the file from flash after 
*                                   displaying the contents
*                                0: Keep the file
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void log_error_crashdump_print(L7_BOOL deleteFile)
{
  L7_int32    fd, numBytes;
  L7_uint32   bufSize;
  L7_RC_t     rc;
  L7_uchar8   buffer[513];
  L7_char8   *ptr, *startPtr, *endPtr;


  rc = osapiFsOpen(LOG_ERROR_CRASHDUMP_FILENAME, &fd);
  if (rc != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Log error crashdump file does not exists\n");
    return;
  }


  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Printing the contents of %s\n\n",
                L7_LOG_ERROR_CRASHDUMP_FILENAME);

  bufSize = sizeof(buffer) - 1;

  memset(buffer, 0, sizeof(buffer));
  numBytes = bufSize;
  rc = osapiFileReadWithLen(fd, buffer, &numBytes);
  if (rc != L7_SUCCESS )
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "ERROR reading from file.\r\n");
  }
  else
  {
    while ((0 < numBytes) && (rc == L7_SUCCESS))
    {
      ptr = startPtr = buffer;
      endPtr = &buffer[numBytes];
      buffer[bufSize] = '\0'; /* To make sure the final string is null terminated */
      do
      {
        while ( ( '\0' != *ptr ) && ( ptr < endPtr ) )
        {
          ptr++;
        }

        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s", startPtr );
        ptr++;
        startPtr = ptr;
      } while ( ptr < endPtr );

      memset(buffer, 0, sizeof(buffer));
      numBytes = bufSize;
      rc = osapiFileReadWithLen(fd, buffer, &numBytes);
    }
  }
  
  osapiFsClose(fd);

  if (deleteFile == L7_TRUE)
  {
    (void) osapiFsDeleteFile(LOG_ERROR_CRASHDUMP_FILENAME);    
  }

  return;
}

#endif
#endif

extern void log_error_func_exec(void);
/* When this flag is true, log_error will not reset the box. Useful for
 * debugging purposes.
 */
L7_BOOL log_error_debug_skip = L7_FALSE;

void log_error_debug_skip_flag_set(L7_BOOL flag)
{
  log_error_debug_skip = flag;
}
/*********************************************************************
* @purpose  Log error and reset the box.
*
* @param    error_code - 32-bit error code.
* @param    file_name  - File where the error ocurred.
* @param    line_num - Line number where the error occurred.
*
* @returns  none
*
* @notes    This function may be called from an interrupt handler.
*
* @end
*********************************************************************/
void log_error_code (L7_uint32 err_code,
                     L7_char8 *file_name,
                     L7_uint32 line_num)
{
  L7_uchar8 log_buf [LOG_CALLOUT_BUF_SIZE];
  L7_uint32 max_file_name;

#ifdef L7_DOT3AH_PACKAGE
  if (err_code == L7_LOG_SEVERITY_EMERGENCY)
  {
    dot3ahDyingGaspCallBack(L7_LOG_SEVERITY_EMERGENCY, line_num, L7_FALSE);
    osapiSleep(2);
  }
#endif /* L7_DOT3AH_PACKAGE */

  if (logNvStoreFuncPtr != L7_NULLPTR)
  {
    memset(log_buf, 0, LOG_CALLOUT_BUF_SIZE);
    *(L7_uint32 *) &log_buf[0] = L7_LOG_ERROR_FORMAT;
    memcpy(&log_buf[4], &err_code, sizeof(err_code));
    memcpy(&log_buf[8], &line_num, sizeof(line_num));
    l7utilsFilenameStrip((L7_char8 **)&file_name);
    max_file_name = LOG_CALLOUT_BUF_SIZE - 13;
    strncpy(&log_buf[12], file_name, max_file_name);

    logNvStoreFuncPtr(log_buf);
  }
  log_error_func_exec();

  /* Don't reset the box on log_error, useful for debugging */
  if (log_error_debug_skip == L7_TRUE)
  {
    printf("Ignoring log_error_code %s  %d \n", file_name, line_num);
    return;
  }

#ifdef _L7_OS_VXWORKS_
#if defined(CPU_FAMILY) && (CPU_FAMILY == PPC)
  /* Generate crashdump for log_error. */
  log_error_crashdump_generate(err_code, file_name, line_num);
#endif
#endif


#if L7_FEAT_DNI8541_BLADESERVER
  bspapiIomFaultSet();
#endif

  /* Cause SIGSEGV handler to run if applicable */
  printf("log_error_code %s  %d \n", file_name, line_num);

#ifdef _L7_OS_LINUX_
  *(L7_uint32 *)0 = 0;
#elif defined _L7_OS_VXWORKS_
extern int logErrorCalls;
  logErrorCalls++;
  taskSuspend(0);
#endif

  bspapiSwitchReset();
}
