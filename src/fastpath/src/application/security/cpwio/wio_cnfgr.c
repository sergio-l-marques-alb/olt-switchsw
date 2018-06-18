/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_cnfgr.c
*
* @purpose   Handle configurator events for this component
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/27/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include <stdio.h>
#include "l7_common.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "osapi.h"
#include "log.h"
#include "default_cnfgr.h"
#include "cpwio_api.h"     /* so we can set fcn pointers */

#include "wio_util.h"
#include "wio_cnfgr.h"

#ifdef L7_CLI_PACKAGE
#include "cli_txt_cfg_api.h"
#endif

extern wioInfo_t *wioInfo;
extern L7_uchar8 *pseudo_pkt;


L7_char8 *wioInitStateNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
  "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};

static void wioCnfgrStateSet(wioCnfgrState_t newState);
static L7_RC_t wioThreadCreate(void);
static L7_RC_t wioCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason);

/*********************************************************************
*
* @purpose  Set the configuration state
*
* @param    newState - @b{(input)}  The configuration state that this component
*                                   is transitioning to.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void wioCnfgrStateSet(wioCnfgrState_t newState)
{
  if (wioInfo->wioTraceFlags & WIO_TRACE_INIT)
  {
    L7_uchar8 traceMsg[WIO_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, WIO_MAX_TRACE_LEN,
                  "CP wired interface owner transitioning from %s to %s",
                  wioInitStateNames[wioInfo->cnfgrState],
                  wioInitStateNames[newState]);
    wioTraceWrite(traceMsg);
  }
  wioInfo->cnfgrState = newState;
}

/*********************************************************************
* @purpose  Do nothing, but do it successfully.
*
* @param    pResponse - @b{(output)}  Response always command complete
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t wioCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  CNFGR System Initialization for this component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to this comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void wioApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  wioEventMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  if (pCmdData == L7_NULL)
  {
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  /* Do minimum amount of work on configurator thread. Pass other work to
   * our own thread. */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the component thread, then pass
     * a message via the queue to complete phase 1 initialization.
     */
    if (wioThreadCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.wioMsgData.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = WIO_CNFGR_INIT;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
           "Failure sending CP wired interface owner configurator init message.");
  }
}

/*********************************************************************
* @purpose  Handles initialization messages from the configurator on
*           our own thread.
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
void wioCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t reason = 0;
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
      rc = wioCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      if (request == L7_CNFGR_RQST_E_START)
      {
        wioCnfgrStateSet(WIO_PHASE_EXECUTE);
        rc = L7_SUCCESS;
        response = L7_CNFGR_CMD_COMPLETE;
        reason = 0;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = wioCnfgrNoopProcess(&response, &reason);
          wioCnfgrStateSet(WIO_PHASE_UNCONFIG_1);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = wioCnfgrUconfigPhase2(&response, &reason);
          wioCnfgrStateSet(WIO_PHASE_UNCONFIG_2);
          break;

        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      rc = wioCnfgrNoopProcess(&response, &reason);
      break;

    default:
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
  }

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
static L7_RC_t wioCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch (request)
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = wioCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        wioCnfgrFiniPhase1Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = wioCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        wioCnfgrFiniPhase2Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = wioCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        wioCnfgrFiniPhase3Process();
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = wioCnfgrNoopProcess(response, reason);
      wioCnfgrStateSet(WIO_PHASE_WMU);
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
* @purpose  Do Init Phase 1.
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
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;

  wioInfo->wioClientList.maxClients = WIO_MAX_CLIENTS;

  wioInfo->wioTraceFlags = 0x0;

  if (wioClientListCreate() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  if (wioClientConnListCreate() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  pseudo_pkt = (L7_uchar8*) osapiMalloc(L7_CP_WIO_COMPONENT_ID, WIO_TCP_PSEUDO_PKT_LEN);

  /* Set callback function pointers for later registration with CP */
  wioInfo->wioCallbacks = osapiMalloc(L7_CP_WIO_COMPONENT_ID, sizeof(intfCpCallbacks_t));
  if (wioInfo->wioCallbacks == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /* register a structure of callback function pointers */
  wioInfo->wioCallbacks->cpCapabilitySet         = wioCpCapabilitySet;
  wioInfo->wioCallbacks->cpCapabilityGet         = wioCpCapabilityGet;
  wioInfo->wioCallbacks->cpAuthenUnauthenClient  = wioCpAuthenUnauthenClient;
  wioInfo->wioCallbacks->cpBlockUnblockAccess    = wioCpBlockUnblockAccess;
  wioInfo->wioCallbacks->cpDeauthenClient        = wioCpDeauthenClient;
  wioInfo->wioCallbacks->cpEnableDisableFeature  = wioCpEnableDisableFeature;
  wioInfo->wioCallbacks->cpClientStatisticsGet   = wioCpClientStatisticsGet;
  wioInfo->wioCallbacks->cpSetAuthPort           = wioCpSetAuthPortNum;
  wioInfo->wioCallbacks->cpSetAuthSecurePort1  = wioCpAuthSecurePort1Num;
  wioInfo->wioCallbacks->cpSetAuthSecurePort2  = wioCpAuthSecurePort2Num;

  wioCnfgrStateSet(WIO_PHASE_INIT_1);
  return rc;
}

/*********************************************************************
* @purpose  Do Init Phase 2.
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
* @notes    This component has no persistent data
*
* @end
*********************************************************************/
L7_RC_t wioCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  #ifdef L7_CLI_PACKAGE
  /* register with Text Based Configuration Manager for
   * configuration apply completion notification */
  if (txtCfgApplyCompletionNotifyRegister(L7_CP_WIO_COMPONENT_ID,
                                          wioTxtCfgApplyCompleteCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
            "Failed to register for cfg apply completion.");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    *pResponse = 0;
    return L7_ERROR;
  }
  #endif

  wioCnfgrStateSet(WIO_PHASE_INIT_2);
  return rc;
}

/*********************************************************************
* @purpose  Do Init Phase 3.
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
L7_RC_t wioCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  wioDefaultCapabilitiesSet();

  wioCnfgrStateSet(WIO_PHASE_INIT_3);

  return rc;
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
L7_RC_t wioCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  /* No need for a xxxRestoreProcess() since we have no persistent config */

  wioCnfgrStateSet(WIO_PHASE_WMU);

  return rc;
}

/*********************************************************************
* @purpose  Create the component thread and the message queue used to
*           send work to the thread.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t wioThreadCreate(void)
{
  L7_int32 wioTaskHandle;

  wioInfo = (wioInfo_t*) osapiMalloc(L7_CP_WIO_COMPONENT_ID, sizeof(wioInfo_t));
  if (!wioInfo)
  {
    return L7_FAILURE;
  }
  memset(wioInfo, 0, sizeof(wioInfo));

  /* Binary semaphore to synchronize access to component data */
  wioInfo->wioLock = (void *)osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (!wioInfo->wioLock)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
            "Unable to create semaphore for CP wired interface owner.");
    return L7_FAILURE;
  }

  /* Counting semaphore. Given whenever a message is added to any message queue
   * for the WIO thread. Taken when a message is read. */
  wioInfo->msgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (wioInfo->msgQSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
           "Failure creating captive portal wired interface queue semaphore.");
    return L7_FAILURE;
  }

  /* create queue for VLAN and initialization events */
  wioInfo->wioMsgQ = osapiMsgQueueCreate(WIO_EVENT_QUEUE,
                                         WIO_QUEUE_MSG_COUNT,
                                         sizeof(wioEventMsg_t));

  if (wioInfo->wioMsgQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
           "CP wired interface owner unable to create event queue.");
    return L7_FAILURE;
  }

  /* Create message queue for received ARP frames to be forwarded */
  wioInfo->wioFrameMsgQ = osapiMsgQueueCreate(WIO_FRAME_QUEUE,
                                              WIO_FRAME_QUEUE_MSG_COUNT,
                                              sizeof(wioFrameMsg_t));
  if (wioInfo->wioFrameMsgQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
           "CP wired interface owner unable to create ARP message queue.");
    return L7_FAILURE;
  }

  wioTaskHandle = osapiTaskCreate(WIO_TASK,
                                  wioTask,
                                  L7_NULL, L7_NULLPTR,
                                  L7_DEFAULT_STACK_SIZE,
                                  L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                  L7_DEFAULT_TASK_SLICE);

  if (wioTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID,
           "ERROR: Unable to create CP wired interface thread.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase1Process( void )
{

  wioClientListDelete();
  wioClientConnListDelete();

  if (wioInfo != L7_NULLPTR)
  {
    if (wioInfo->wioCallbacks)
    {
      osapiFree(L7_CP_WIO_COMPONENT_ID, wioInfo->wioCallbacks);
    }

    osapiFree(L7_CP_WIO_COMPONENT_ID, wioInfo);
    wioInfo = L7_NULLPTR;
  }

  wioCnfgrStateSet(WIO_PHASE_INIT_0);
}

/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase2Process(void)
{
  wioCnfgrStateSet(WIO_PHASE_INIT_1);
}

/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place cnfgrState to WMU */
  wioCnfgrUconfigPhase2(&response, &reason);
}


