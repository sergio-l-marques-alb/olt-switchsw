/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_cnfgr.c
*
* @purpose   SFlow component interface to configurator
*
* @component sflow
*
* @comments  none
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#include <string.h>
#include "commdefs.h"
#include "l7_product.h"
#include "datatypes.h"
#include "nvstoreapi.h"
#include "osapi_support.h"
#include "osapi_sockdefs.h"
#include "buff_api.h"
#include "l3_addrdefs.h"

#include "sflow_cfg.h"
#include "sflow_cnfgr.h"
#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_debug.h"

/* Data objects being used */
SFLOW_agent_t   agent;

extern PORTEVENT_MASK_t   sFlowEventMask_g;

/* Local function definitions */
static void    sFlowCnfgrStateSet(sFlowCnfgrState_t newState);
static L7_RC_t sFlowAgentCreate(void);
static L7_RC_t sFlowAgentSocketInit();
static L7_RC_t sFlowCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                           L7_CNFGR_RESPONSE_t *response,
                                           L7_CNFGR_ERR_RC_t *reason);
/*********************************************************************
*
* @purpose  Set the sFlow configuration state
*
* @param    newState - @b{(input)}  The configuration state that sFlow
*                                   is transitioning to.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void sFlowCnfgrStateSet(sFlowCnfgrState_t newState)
{
  L7_char8 *sFlowInitStateNames[] = {"INIT 0", "INIT 1", "INIT 2",
                                            "WMU", "INIT 3", "EXECUTE",
                                            "UNCONFIG 1", "UNCONFIG 2"};
  SFLOW_TRACE(SFLOW_DEBUG_INTERNAL,"sFlow transitioning from %s to %s\n",
                  sFlowInitStateNames[agent.sFlowCnfgrState],
                  sFlowInitStateNames[newState]);
  agent.sFlowCnfgrState = newState;
}

/*********************************************************************
* @purpose  This function process the configurator control command/request
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
L7_RC_t sFlowCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t sFlowRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return(sFlowRC);
}

/*********************************************************************
* @purpose  CNFGR System Initialization for sFlow component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the sFlow comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void sFlowApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  sFlowEventMsg_t msg;
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
   * sFlow thread. */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the sFlow thread, then pass
     * a message via the queue to complete phase 1 initialization.
     */

   /*  Instantiate sFlowAgent runtime instance */
    if (sFlowAgentCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.u.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = SFLOW_CNFGR_MSG;
  if (osapiMessageSend(agent.sFlowEventQueue, &msg, sizeof(sFlowEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(agent.sFlowMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "Failure sending sFlow configurator init message.");
  }
  return;
}

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator on
*           the sFlow thread.
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
void sFlowCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
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
      rc = sFlowCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      if (request == L7_CNFGR_RQST_E_START)
      {
        sFlowCnfgrStateSet(SFLOW_PHASE_EXECUTE);
        rc = L7_SUCCESS;
        response = L7_CNFGR_CMD_COMPLETE;
        reason = 0;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = sFlowCnfgrNoopProcess(&response, &reason);
          sFlowCnfgrStateSet(SFLOW_PHASE_UNCONFIG_1);

          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = sFlowCnfgrUconfigPhase2(&response, &reason);
          sFlowCnfgrStateSet(SFLOW_PHASE_UNCONFIG_2);
          break;

        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      rc = sFlowCnfgrNoopProcess(&response, &reason);
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
static L7_RC_t sFlowCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                           L7_CNFGR_RESPONSE_t *response,
                                           L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch (request)
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = sFlowCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        sFlowCnfgrFiniPhase1Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = sFlowCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        sFlowCnfgrFiniPhase2Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = sFlowCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        sFlowCnfgrFiniPhase3Process();
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = sFlowCnfgrNoopProcess(response, reason);
      sFlowCnfgrStateSet(SFLOW_PHASE_WMU);
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
* @purpose  This function process the configurator control command/request
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
L7_RC_t sFlowCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t           sFlowRC;
  L7_uint32         idx;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  sFlowRC = L7_SUCCESS;

  /* Allocate and initialized memory for configuration data */
  if ((agent.sFlowCfg = (SFLOW_cfgData_t *) osapiMalloc(L7_SFLOW_COMPONENT_ID,
                                            (L7_uint32) sizeof(SFLOW_cfgData_t)))
                      == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
     sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  memset((void *) agent.sFlowCfg, 0, (L7_uint32) sizeof(SFLOW_cfgData_t));

  /* Debug config structure */
  agent.sFlowDebugCfg = osapiMalloc(L7_SFLOW_COMPONENT_ID, sizeof(sFlowDebugCfg_t));
  if (agent.sFlowDebugCfg == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
     sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  memset((void *) agent.sFlowDebugCfg, 0, (L7_uint32) sizeof(sFlowDebugCfg_t));
  agent.sFlowDebugPacketTraceTxFlag = L7_FALSE;

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  sFlowDebugCfgRead();
  sFlowApplyDebugConfigData();

  /* Create Receiver runtime instances */
  if ((agent.receiver = (SFLOW_receiver_t*) osapiMalloc(L7_SFLOW_COMPONENT_ID,
                                                   sizeof(SFLOW_receiver_t) *
                                                   SFLOW_MAX_RECEIVERS))
                      == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }
  memset(agent.receiver, 0x00, sizeof(SFLOW_receiver_t) * SFLOW_MAX_RECEIVERS);

  /* Initialize receivers instances */
  for (idx = 0; idx < SFLOW_MAX_RECEIVERS; idx++)
  {
    agent.receiver[idx].rcvrCfg = &(agent.sFlowCfg->receiverGroup[idx]);
  }

  /* Create Poller runtime instances */
  agent.pollerTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_SFLOW_COMPONENT_ID,
                          SFLOW_MAX_POLLERS * sizeof(avlTreeTables_t));
  agent.pollerDataHeap  = (SFLOW_poller_t *)osapiMalloc(L7_SFLOW_COMPONENT_ID,
                              SFLOW_MAX_POLLERS * sizeof(SFLOW_poller_t));

  if ((agent.pollerTreeHeap == L7_NULLPTR) ||
      (agent.pollerDataHeap == L7_NULLPTR)
     )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: Error allocating data for poller AVL Tree \n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* AVL Tree creations - pollerAvlTree*/
  avlCreateAvlTree(&(agent.pollerAvlTree),  agent.pollerTreeHeap,
                   agent.pollerDataHeap, SFLOW_MAX_POLLERS,
                   sizeof(SFLOW_poller_t), 0x10,
                   sizeof(SFLOW_poller_DataKey_t));



  /* Create Sampler runtime instances */
  agent.samplerTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_SFLOW_COMPONENT_ID,
                          SFLOW_MAX_SAMPLERS * sizeof(avlTreeTables_t));
  agent.samplerDataHeap  = (SFLOW_sampler_t *)osapiMalloc(L7_SFLOW_COMPONENT_ID,
                              SFLOW_MAX_SAMPLERS * sizeof(SFLOW_sampler_t));

  if ((agent.samplerTreeHeap == L7_NULLPTR) ||
      (agent.samplerDataHeap == L7_NULLPTR)
     )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: Error allocating data for sampler AVL Tree \n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* AVL Tree creations - samplerAvlTree*/
  avlCreateAvlTree(&(agent.samplerAvlTree),  agent.samplerTreeHeap,
                   agent.samplerDataHeap, SFLOW_MAX_SAMPLERS,
                   sizeof(SFLOW_sampler_t), 0x10,
                   sizeof(SFLOW_sampler_DataKey_t));

  /* Packet queue */
  agent.sFlowPacketQueue = (void *)osapiMsgQueueCreate(SFLOW_PACKET_QUEUE,
                                    SFLOW_PACKETQ_MSG_COUNT, SFLOW_PDU_MSG_SIZE);
  if (agent.sFlowPacketQueue == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* Buffer pool */
  if (bufferPoolInit(SFLOW_PACKETQ_MSG_COUNT, SFLOW_MAX_HEADER_SIZE, SFLOW_PACKET_BUFF_NAME,
                     &(agent.sflowBufferPool)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "Error allocating buffers\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* App Timer creation */

  /* Timer Initializations */
   if(bufferPoolInit(SFLOW_MAX_POLLERS ,
                     L7_APP_TMR_NODE_SIZE, "sFlow Timer Bufs", &agent.appTimerBufferPoolId)
                     != L7_SUCCESS)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
             "sFlowCnfgrInitPhase1Process: Falied to allocate memory for Sflow timer buffers\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
   }

  /* Register for time ticks with appTimer */
  agent.timerCB =  appTimerInit(L7_SFLOW_COMPONENT_ID, sflowTimerExpiryHdlr,
                      (void *) &agent, SFLOW_TIMER_INTERVAL, agent.appTimerBufferPoolId);

  if (agent.timerCB == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: App Timer Initialization Failed.\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* Create timer handles */
  /* Allocate memory for the Handle List */
  if ((agent.handleListMemHndl =
        (handle_member_t*) osapiMalloc (L7_SFLOW_COMPONENT_ID,
                                        (SFLOW_MAX_POLLERS * sizeof (handle_member_t))))
                        == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: Error allocating Handle List Buffers\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }
  if(handleListInit (L7_SFLOW_COMPONENT_ID, SFLOW_MAX_POLLERS,
                     &(agent.handle_list), agent.handleListMemHndl)
                  != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: Unable to create timer handle list\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* Initialize communication sockets */
  if (sFlowAgentSocketInit() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowCnfgrInitPhase1Process: Unable to create sockets\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sFlowRC = L7_ERROR;
    return sFlowRC;
  }

  /* move to next cnfgr state */
  sFlowCnfgrStateSet(SFLOW_PHASE_INIT_1);
  return sFlowRC;
}

/*********************************************************************
* @purpose  This function process the configurator control command/request
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
L7_RC_t sFlowCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t sFlowRC;
  nvStoreFunctionList_t sFlowNotifyFunctionList;
  sysnetNotifyEntry_t   snEntry;


  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  sFlowRC     = L7_SUCCESS;

  /* Registrations
           1. nvstore
           2. NIM
           3. sysNet
   */

  /* nv Store */
  memset((void *) &sFlowNotifyFunctionList, 0, sizeof(sFlowNotifyFunctionList));
  sFlowNotifyFunctionList.registrar_ID   = L7_SFLOW_COMPONENT_ID;
  sFlowNotifyFunctionList.notifySave     = sFlowSave;
  sFlowNotifyFunctionList.hasDataChanged = sFlowHasDataChanged;
  sFlowNotifyFunctionList.notifyConfigDump     = sFlowCfgDump;
  sFlowNotifyFunctionList.resetDataChanged = sFlowResetDataChanged;

  if (nvStoreRegister(sFlowNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sFlowRC     = L7_ERROR;

    return sFlowRC;
  }

  /* NIM Inteface changes */
  if (nimRegisterIntfChange(L7_SFLOW_COMPONENT_ID,
                            sFlowIntfChangeCallback,
                            sFlowNimStartupNotifyCallback,
                            NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sflowCnfgrInitPhase2Process: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sFlowRC     = L7_ERROR;
    return sFlowRC;
  }

  LOG_INFO(LOG_CTX_STARTUP,"Going to register sFlowPduReceive related to type=%u, rx_reason=%u: 0x%08x",
           SYSNET_PKT_RX_REASON, SFLOW_SAMPLE_INGRESS_REASON_CODE, (L7_uint32) sFlowPduReceive);

  /* register with sysnet for sampled packets */
  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "sFlowPduReceive", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = sFlowPduReceive;
  snEntry.type = SYSNET_PKT_RX_REASON;
  snEntry.u.rxReason = SFLOW_SAMPLE_INGRESS_REASON_CODE;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
     sysapiPrintf("\nsFlowCnfgrInitPhase2Process: sysnet registration for reason 0x20 failed");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sFlowRC     = L7_ERROR;
    return sFlowRC;
 }

  /* register for debug */
  sFlowDebugRegister();

  sFlowCnfgrStateSet(SFLOW_PHASE_INIT_2);
  return sFlowRC;
}

/*********************************************************************
* @purpose  This function processes the configurator control command/request
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
L7_RC_t sFlowCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&sFlowEventMask_g, 0, sizeof(sFlowEventMask_g));
  nimRegisterIntfEvents(L7_SFLOW_COMPONENT_ID, sFlowEventMask_g);

  rc = sysapiCfgFileGet(L7_SFLOW_COMPONENT_ID, SFLOW_CFG_FILENAME,
                       (L7_char8 *)agent.sFlowCfg, sizeof(SFLOW_cfgData_t),
                       &agent.sFlowCfg->checkSum, SFLOW_CFG_VER_CURRENT,
                       sFlowBuildDefaultConfigData, L7_NULL);
  if (rc != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return( rc );
  }

  sFlowBuildDefaultConfigData(SFLOW_CFG_VER_CURRENT);

  if (sFlowApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return( rc );
  }

  agent.sFlowCfg->cfgHdr.dataChanged = L7_FALSE;
  sFlowCnfgrStateSet(SFLOW_PHASE_INIT_3);
  /* Start SFLOW periodic timer */
  return( rc );
}

/*********************************************************************
* @purpose  This function undoes sFLowCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sFlowCnfgrFiniPhase1Process( void )
{
  if (agent.sFlowCfg != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.sFlowCfg);
    agent.sFlowCfg = L7_NULLPTR;
  }

  if (agent.sFlowDebugCfg != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.sFlowDebugCfg);
    agent.sFlowDebugCfg = L7_NULLPTR;
  }

  /* Destroy the poller AVL Tree */
  if (agent.pollerTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.pollerTreeHeap);
    agent.pollerTreeHeap = L7_NULLPTR;
  }

  if (agent.pollerDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.pollerDataHeap);
    agent.pollerDataHeap = L7_NULLPTR;
  }

  /* Destroy the sampler AVL Tree */
  if (agent.samplerTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.samplerTreeHeap);
    agent.samplerTreeHeap = L7_NULLPTR;
  }

  if (agent.samplerDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.samplerDataHeap);
    agent.samplerDataHeap = L7_NULLPTR;
  }

  if (agent.receiver != L7_NULLPTR)
  {
    osapiFree(L7_SFLOW_COMPONENT_ID, agent.receiver);
    agent.receiver = L7_NULLPTR;
  }

  if (agent.sFlowPacketQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(agent.sFlowPacketQueue);
  }

  /* Destroy the timer data */
  if (agent.handle_list != L7_NULLPTR)
  {
    osapiFree (L7_SFLOW_COMPONENT_ID, agent.handleListMemHndl);
    (void)handleListDeinit(L7_SFLOW_COMPONENT_ID, agent.handle_list);
    agent.handle_list = L7_NULLPTR;
  }

  if (agent.timerCB != L7_NULLPTR)
  {
    (void)appTimerDeInit(agent.timerCB);
    agent.timerCB = L7_NULLPTR;
  }

  /* cleanup buffer pools */
  if (agent.sflowBufferPool != 0)
  {
    bufferPoolDelete(agent.sflowBufferPool);
    agent.sflowBufferPool = 0;
  }

  if (agent.appTimerBufferPoolId != 0)
  {
    bufferPoolDelete(agent.appTimerBufferPoolId);
    agent.appTimerBufferPoolId = 0;
  }

  osapiSocketClose(agent.sFlowAgentv4Socket);
  osapiSocketClose(agent.sFlowAgentv6Socket);

  osapiRWLockDelete(agent.sFlowCfgRWLock);
  sFlowCnfgrStateSet(SFLOW_PHASE_INIT_0);
}

/*********************************************************************
* @purpose  This function undoes sFlowCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sFlowCnfgrFiniPhase2Process(void)
{
  sysnetNotifyEntry_t snEntry;

  LOG_NOTICE(LOG_CTX_PTIN_CNFGR, "ATTENTION: sFlow callback will be deregistered!");

  /* deregistrations with
             NIM, NVStore, sysNet
   */
  (void)nimDeRegisterIntfChange(L7_SFLOW_COMPONENT_ID);
  (void)nvStoreDeregister(L7_SFLOW_COMPONENT_ID);

  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "sFlowPduReceive", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = sFlowPduReceive;
  snEntry.type = SYSNET_PKT_RX_REASON;
  snEntry.u.rxReason = SFLOW_SAMPLE_INGRESS_REASON_CODE;
  (void)sysNetDeregisterPduReceive(&snEntry);

  sFlowCnfgrStateSet(SFLOW_PHASE_INIT_1);
}

/*********************************************************************
* @purpose  This function undoes sFlowCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sFlowCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place agent.sFlowCnfgrState to WMU */
  sFlowCnfgrUconfigPhase2(&response, &reason);
}


/*********************************************************************
* @purpose  This function process the configurator control command/request
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
L7_RC_t sFlowCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return( rc );
}

/*********************************************************************
* @purpose  This function process the configurator control command/request
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
L7_RC_t sFlowCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  if (sFlowRestoreProcess() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return rc;
  }

  sFlowCnfgrStateSet(SFLOW_PHASE_WMU);

  return rc;
}
/*********************************************************************
* @purpose  Create and bind sockets for sending datagrams
*
* @param    void
*
* @returns  L7_SUCCESS - The sockets are initialized properly
* @returns  L7_FAILURE - otherwise
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sFlowAgentSocketInit(void)
{
  L7_uint32         val;
  L7_sockaddr_in_t  addr;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_sockaddr_in6_t addr6;
#endif
  /* Socket creations */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0,
                        &(agent.sFlowAgentv4Socket)) != L7_SUCCESS)
  {
    sysapiPrintf("\nsFlowAgentSocketInit: Unable to create ipv4 socket for sFlow");
    return L7_FAILURE;;
  }

  /* Set options to reuse the Address and Port*/
  val = 1;
  if ( osapiSetsockopt(agent.sFlowAgentv4Socket, L7_SOL_SOCKET,
                       L7_SO_REUSEADDR,(L7_char8 *) &val, sizeof(val)) == L7_FAILURE)
  {
     sysapiPrintf("\nsFlowAgentSocketInit: Unable to set socket options for sFlow v4 socket errorno %d",
                   osapiErrnoGet());
    return L7_FAILURE;
  }

  /* We never read from this socket, so set maximum RX size to zero.
   *   ** A DOS may fill up the socket with garbage and cause the IP stack
   *     ** to run out of memory.
   *       */
  val = 0;
  if (osapiSetsockopt(agent.sFlowAgentv4Socket, L7_SOL_SOCKET, L7_SO_RCVBUF,(L7_char8 *)
                      &val, sizeof(val))== L7_FAILURE)
  {
    sysapiPrintf("\nsFlowAgentSocketInit: Can't set SO_RCVBUF to 0 - errno %d\n", osapiErrnoGet());
    return L7_FAILURE;
  }

  addr.sin_family = L7_AF_INET;
  addr.sin_port = osapiHtons(FD_SFLOW_RCVR_PORT);
  addr.sin_addr.s_addr = L7_INADDR_ANY;

  if (osapiSocketBind(agent.sFlowAgentv4Socket, (L7_sockaddr_t *)&addr, sizeof(addr))
                      != L7_SUCCESS)
  {
    sysapiPrintf("sFlowAgentSocketInit: Can't bind socket to local address %lx port %d - errno %d\n",
            L7_INADDR_ANY, FD_SFLOW_RCVR_PORT, osapiErrnoGet());
    return L7_FAILURE;
  }

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  if (osapiSocketCreate(L7_AF_INET6, L7_SOCK_DGRAM, 0,
                        &(agent.sFlowAgentv6Socket)) != L7_SUCCESS)
  {
    sysapiPrintf("sFlowAgentSocketInit: Unable to create ipv6 socket for sFlow errno %d\n",
                  osapiErrnoGet());
    return L7_FAILURE;
  }

  /* Set options to reuse the Address and Port*/
  val = 1;
  if (osapiSetsockopt(agent.sFlowAgentv6Socket, L7_SOL_SOCKET,
                      L7_SO_REUSEADDR,(L7_char8 *) &val, sizeof(val)) == L7_FAILURE)
  {
    sysapiPrintf("\nsFlowAgentSocketInit: Unable to set socket options for sFlow v6 socket errorno %d",
                  osapiErrnoGet());
    return L7_FAILURE;
  }

  memset(&addr6, 0x00, sizeof(L7_sockaddr_in6_t));
  addr6.sin6_family = L7_AF_INET6;
  addr6.sin6_port    = osapiHtons(FD_SFLOW_RCVR_PORT);

  if (osapiSocketBind(agent.sFlowAgentv6Socket, (L7_sockaddr_t *)&addr6, sizeof(addr6))
                      != L7_SUCCESS)
  {
    sysapiPrintf("sFlowAgentSocketInit: Can't bind v6 socket port %d - errno %d\n",
                  FD_SFLOW_RCVR_PORT, osapiErrnoGet());
    return L7_FAILURE;
  }

  /* We never read from this socket, so set maximum RX size to zero.
   *   ** A DOS may fill up the socket with garbage and cause the IP stack
   *     ** to run out of memory.
   *       */
  val = 0;
  if (osapiSetsockopt(agent.sFlowAgentv6Socket, L7_SOL_SOCKET, L7_SO_RCVBUF,(L7_char8 *)
                      &val, sizeof(val))== L7_FAILURE)
  {
    sysapiPrintf("\nsFlowAgentSocketInit: Can't set SO_RCVBUF to 0 for v6 socket - errno %d\n", osapiErrnoGet());
    return L7_FAILURE;
  }
#endif
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create the sFlow thread and the message queue used to
*           send work to the thread.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    The sFlow thread serves two message queues. One queue
*           contains events (configurator events, timer events).
*           The second queue holds incoming sFlow packets.
*
*           This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/
static L7_RC_t sFlowAgentCreate(void)
{

  /* Initialize sFlowAgent instance */
  memset(&agent, 0x00, sizeof(SFLOW_agent_t));
  agent.sFlowCnfgrState = SFLOW_PHASE_INIT_0;

  /* Read/write lock to protect component data since processing occurs
   * on multiple threads (sFlow thread, UI thread, dot1q, etc) */
  if (osapiRWLockCreate(&(agent.sFlowCfgRWLock), OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "Failure creating sFlow rw lock.");
    return L7_FAILURE;
  }

  /* Counting semaphore. Given whenever a message is added to any message queue
   * for the sFlow thread. Taken when a message is read. */
  if ((agent.sFlowMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0)) == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "Failure creating sFlow queue semaphore.");
    return L7_FAILURE;
  }

  /* create queue for cnfgr, timer, configuration and interface events */
  if ((agent.sFlowEventQueue = osapiMsgQueueCreate(SFLOW_EVENT_QUEUE,
                               SFLOW_EVENTQ_MSG_COUNT, SFLOW_MSG_SIZE))
                             == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "sFlow unable to create event queue.");
    return L7_FAILURE;
  }

  /* create sFlow packet queue */
  if ((agent.sFlowPacketQueue = osapiMsgQueueCreate(SFLOW_PACKET_QUEUE,
                                                    SFLOW_PACKETQ_MSG_COUNT,
                                                    SFLOW_PDU_MSG_SIZE))
                              == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "sFlow unable to create packet queue.");
    return L7_FAILURE;
  }

  /* create sFlow task */
  if ((agent.sFlowTaskHandle = osapiTaskCreate(SFLOW_TASK,
                                          sFlowTask,
                                          L7_NULL, L7_NULLPTR,
                                          FD_CNFGR_SFLOW_DEFAULT_STACK_SIZE,
                                          FD_CNFGR_SFLOW_DEFAULT_TASK_PRIORITY,
                                          FD_CNFGR_SFLOW_DEFAULT_TASK_SLICE))
                               == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID,
           "ERROR: Unable to create sFlow thread.");
    return L7_FAILURE;
  }
   if (osapiWaitForTaskInit (L7_SFLOW_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SFLOW_COMPONENT_ID, "Unable to initialize sFlowTask()\n");
    return(L7_FAILURE);
  }

  return L7_SUCCESS;
}

/************************************************************************
* @purpose  Validate whether sFlow component is ready to accept external
*           events
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
************************************************************************/
L7_BOOL sFlowIsReady(void)
{

  return (((agent.sFlowCnfgrState == SFLOW_PHASE_INIT_3) || \
                         (agent.sFlowCnfgrState == SFLOW_PHASE_EXECUTE) || \
                          agent.sFlowCnfgrState == SFLOW_PHASE_UNCONFIG_1)) \
                          ? (L7_TRUE) : (L7_FALSE);
}

