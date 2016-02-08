
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_cnfgr.c
*
* @purpose   Dynamic ARP Inspection configurator file
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create 09/06/2007
*
* @author Kiran Kumar Kella
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
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"

#include "dai_cfg.h"
#include "dai_util.h"
#include "dai_cnfgr.h"

osapiRWLock_t         daiCfgRWLock;
daiCfgData_t          *daiCfgData   = L7_NULLPTR;
daiCnfgrState_t       daiCnfgrState = DAI_PHASE_INIT_0;
extern daiInfo_t      *daiInfo;
extern daiIntfInfo_t  *daiIntfInfo;
extern daiVlanInfo_t  *daiVlanInfo;

/* The DAI thread reads from two queues. dai_Event_Queue 
 * holds events to be processed. dai_Packet_Queue holds incoming 
 * ARP packets. */
void *daiMsgQSema      = L7_NULLPTR;
void *dai_Event_Queue  = L7_NULLPTR;
void *dai_Packet_Queue = L7_NULLPTR;

L7_char8 *daiInitStateNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
                                 "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};

static void daiCnfgrStateSet(daiCnfgrState_t newState);
static L7_RC_t daiThreadCreate(void);
static L7_RC_t daiCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason);


/*********************************************************************
*
* @purpose  Set the DAI configuration state
*
* @param    newState - @b{(input)}  The configuration state that DAI
*                                   is transitioning to.
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
static void daiCnfgrStateSet(daiCnfgrState_t newState)
{
  if (daiCfgData->daiTraceFlags & DAI_TRACE_INIT)
  {
    L7_uchar8 traceMsg[DAI_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DAI_MAX_TRACE_LEN,
                  "DAI transitioning from %s to %s", 
                  daiInitStateNames[daiCnfgrState],
                  daiInitStateNames[newState]);
    daiTraceWrite(traceMsg);
  }
  daiCnfgrState = newState;
}

/*********************************************************************
*
* @purpose  Set initial defaults for global variables
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void daiInfoInit(void)
{
  daiInfo->cfgDataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  This function process the configurator control commandai/request
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
L7_RC_t daiCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                           L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t daiRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return(daiRC);
}

/*********************************************************************
* @purpose  CNFGR System Initialization for DAI component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the DAI comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void daiApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  daiEventMsg_t msg;           
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
   * DAI thread. */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the DAI thread, then pass
     * a message via the queue to complete phase 1 initialization.
     */
    if (daiThreadCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.daiMsgData.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = DAI_CNFGR_INIT;
  if (osapiMessageSend(dai_Event_Queue, &msg, sizeof(daiEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(daiMsgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID, 
           "Failure sending DAI configurator init message.");
  }

  return;
}

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator on
*           the DAI thread.
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
void daiCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
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
      rc = daiCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      if (request == L7_CNFGR_RQST_E_START)
      {
        daiCnfgrStateSet(DAI_PHASE_EXECUTE);
        rc = L7_SUCCESS;
        response = L7_CNFGR_CMD_COMPLETE;
        reason = 0;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = daiCnfgrNoopProcess(&response, &reason);
          daiCnfgrStateSet(DAI_PHASE_UNCONFIG_1);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = daiCnfgrUconfigPhase2(&response, &reason);
          daiCnfgrStateSet(DAI_PHASE_UNCONFIG_2);
          break;

        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      rc = daiCnfgrNoopProcess(&response, &reason);
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
static L7_RC_t daiCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;
  switch (request)
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = daiCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        daiCnfgrFiniPhase1Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = daiCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        daiCnfgrFiniPhase2Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = daiCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        daiCnfgrFiniPhase3Process();
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = daiCnfgrNoopProcess(response, reason);
      daiCnfgrStateSet(DAI_PHASE_WMU);
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
* @purpose  This function process the configurator control commandai/request
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
L7_RC_t daiCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t i, daiRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  daiRC = L7_SUCCESS;

  /* Allocate and initialized memory for global data */
  daiCfgData = (daiCfgData_t *) osapiMalloc(L7_DAI_COMPONENT_ID,
                                          (L7_uint32) sizeof(daiCfgData_t));

  if (daiCfgData == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    daiRC = L7_ERROR;
    return daiRC;
  }

  memset((void *) daiCfgData, 0, (L7_uint32) sizeof(daiCfgData_t));

  daiInfo = (daiInfo_t*) osapiMalloc(L7_DAI_COMPONENT_ID, sizeof(daiInfo_t));
  if (!daiInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    daiRC = L7_ERROR;
    return daiRC;
  }
  memset(daiInfo, 0, sizeof(daiInfo));
  daiInfoInit();

  daiIntfInfo = (daiIntfInfo_t*) osapiMalloc(L7_DAI_COMPONENT_ID, 
                                             DAI_MAX_INTF_COUNT * sizeof(daiIntfInfo_t));
  if (!daiIntfInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    daiRC = L7_ERROR;
    return daiRC;
  }
  memset(daiIntfInfo, 0, DAI_MAX_INTF_COUNT * sizeof(daiIntfInfo_t));
  for(i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    daiIntfInfo[i].consecutiveInterval = 1;
  }

  daiVlanInfo = (daiVlanInfo_t*) osapiMalloc(L7_DAI_COMPONENT_ID, 
                                             (DAI_MAX_VLAN_COUNT+1) * sizeof(daiVlanInfo_t));
  if (!daiVlanInfo)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    daiRC = L7_ERROR;
    return daiRC;
  }
  memset(daiVlanInfo, 0, (DAI_MAX_VLAN_COUNT+1) * sizeof(daiVlanInfo_t));

  daiCnfgrStateSet(DAI_PHASE_INIT_1);
  return daiRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commandai/request
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
L7_RC_t daiCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t daiNotifyFunctionList;
  sysnetPduIntercept_t sysnetPduInterceptIn;
  L7_uint32 eventMask = 0;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *)&daiNotifyFunctionList, 0, sizeof(daiNotifyFunctionList));
  daiNotifyFunctionList.registrar_ID     = L7_DAI_COMPONENT_ID;
  daiNotifyFunctionList.notifySave       = daiSave;
  daiNotifyFunctionList.hasDataChanged   = daiHasDataChanged;
  daiNotifyFunctionList.resetDataChanged = daiResetDataChanged;

  if (nvStoreRegister(daiNotifyFunctionList) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DAI_COMPONENT_ID,
            "daiCnfgrInitPhase2Process: Failed to register with nvStore." );
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;

    return rc;
  }

  /* register with NIM for interface events like create and delete */
  if (nimRegisterIntfChange(L7_DAI_COMPONENT_ID, daiIntfChangeCallback)
            != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DAI_COMPONENT_ID,
            "daiCnfgrInitPhase2Process: Failed to register with NIM." );
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;

    return rc;
  }


  /* register callback with dot1q to get notifications for vlan delete 
   * and port membership changes events */
  eventMask = VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY | VLAN_DELETE_PENDING_NOTIFY;
  if (vlanRegisterForChange(daiVlanChangeCallback, 
                            L7_DAI_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID,
           "DAI failed to register for VLAN changes.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc          = L7_ERROR;
    return rc;
  }

  /* Intercept incoming ARP packets */
  sysnetPduInterceptIn.addressFamily = L7_AF_INET;
  sysnetPduInterceptIn.hookId = SYSNET_INET_RECV_ARP_IN;
  sysnetPduInterceptIn.hookPrecedence = FD_SYSNET_HOOK_DAI_ARP_IN_PRECEDENCE;
  sysnetPduInterceptIn.interceptFunc = daiArpRecv;
  strcpy(sysnetPduInterceptIn.interceptFuncName, "daiArpRecv");

  if (sysNetPduInterceptRegister(&sysnetPduInterceptIn) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  daiCnfgrStateSet(DAI_PHASE_INIT_2);
  return rc;
}

/*********************************************************************
* @purpose  This function processes the configurator control commandai/request
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
L7_RC_t daiCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  daiBuildDefaultConfigData(0);

  if (daiApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return( rc );
  }

  daiCfgDataNotChanged();
  daiCnfgrStateSet(DAI_PHASE_INIT_3);

  /* Start DAI periodic timer */
  daiTimerStart();

  return( rc );
}

/*********************************************************************
* @purpose  This function undoes daiCnfgrFiniPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiCnfgrFiniPhase1Process( void )
{
  if (daiCfgData != L7_NULLPTR)
  {
    osapiFree(L7_DAI_COMPONENT_ID, daiCfgData);
    daiCfgData = L7_NULLPTR;
  }
  daiCnfgrStateSet(DAI_PHASE_INIT_0);
}

/*********************************************************************
* @purpose  This function undoes daiCnfgrFiniPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiCnfgrFiniPhase2Process(void)
{
  (void)nvStoreDeregister(L7_DAI_COMPONENT_ID);
  (void)vlanRegisterForChange(L7_NULLPTR, L7_DAI_COMPONENT_ID,L7_NULL);

  daiCnfgrStateSet(DAI_PHASE_INIT_1);
}

/*********************************************************************
* @purpose  This function undoes daiCnfgrFiniPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place daiCnfgrState to WMU */
  daiCnfgrUconfigPhase2(&response, &reason);
}


/*********************************************************************
* @purpose  This function process the configurator control commandai/request
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
L7_RC_t daiCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return( rc );
}

/*********************************************************************
* @purpose  This function process the configurator control commandai/request
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
L7_RC_t daiCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  if (daiRestoreProcess() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
    return rc;
  }

  daiCnfgrStateSet(DAI_PHASE_WMU);

  return rc;
}

/*********************************************************************
* @purpose  Create the DAI thread and the message queue used to
*           send work to the thread. 
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    The DAI thread serves two message queues. One queue 
*           contains events (VLAN membership changes, configurator events).
*           The second queue holds incoming ARP packets. 
*    
*           This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/
static L7_RC_t daiThreadCreate(void)
{
  L7_int32 daiTaskHandle;

  /* Read/write lock to protect component data since processing occurs 
   * on multiple threads (DAI thread, UI thread, dot1q, etc) */
  if (osapiRWLockCreate(&daiCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DAI_COMPONENT_ID,
            "Unable to create read/write lock for DAI");
    return L7_FAILURE;
  }

  /* Counting semaphore. Given whenever a message is added to any message queue
   * for the DAI thread. Taken when a message is read. */
  daiMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (daiMsgQSema == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID, 
           "Failure creating DAI queue semaphore.");
    return L7_FAILURE;
  }

  /* create queue for VLAN and initialization events */
  dai_Event_Queue = osapiMsgQueueCreate(DAI_EVENT_QUEUE,
                                        DAI_EVENTQ_MSG_COUNT,
                                        sizeof(daiEventMsg_t));

  if (dai_Event_Queue == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID,
           "DAI unable to create event queue.");
    return L7_FAILURE;
  }

  /* create DAI packet queue */
  dai_Packet_Queue = osapiMsgQueueCreate(DAI_PACKET_QUEUE,
                                         DAI_PACKETQ_MSG_COUNT,
                                         sizeof(daiFrameMsg_t));

  if (dai_Packet_Queue == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID,
           "DAI unable to create packet queue.");
    return L7_FAILURE;
  }

  daiTaskHandle = osapiTaskCreate(DAI_TASK,
                                  daiTask,
                                  L7_NULL, L7_NULLPTR,
                                  FD_CNFGR_DAI_DEFAULT_STACK_SIZE,
                                  FD_CNFGR_DAI_DEFAULT_TASK_PRIORITY,
                                  FD_CNFGR_DAI_DEFAULT_TASK_SLICE);

  if (daiTaskHandle == L7_ERROR)
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID,
           "ERROR: Unable to create DAI thread.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

