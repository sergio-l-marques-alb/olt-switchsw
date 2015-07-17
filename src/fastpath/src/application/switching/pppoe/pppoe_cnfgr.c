/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_cnfgr.c
*
* @purpose   PPPoE Intermediate Agent
*
* @component PPPoE
*
* @comments none
*
* @create 14/05/2013
*
* @author Daniel Figueira
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
#include "logger.h"

#include "ptin_globaldefs.h"
#include "ptin_include.h"

#include "pppoe_cnfgr.h"
#include "pppoe_util.h"


osapiRWLock_t pppoeCfgRWLock;
pppoeCnfgrState_t pppoeCnfgrState = PPPOE_PHASE_INIT_0;


/* The PPPoE thread reads from two queues. pppoe_Event_Queue
 * holds events to be processed. pppoe_Packet_Queue holds incoming
 * PPPoE packets. */
void *pppoeMsgQSema = L7_NULLPTR;
void *pppoe_Event_Queue = L7_NULLPTR;
void *pppoe_Packet_Queue = L7_NULLPTR;

L7_char8 *pppoeInitStateNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
  "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};


static void pppoeCnfgrStateSet(pppoeCnfgrState_t newState);
static L7_RC_t pppoeThreadCreate(void);
static L7_RC_t pppoeCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                           L7_CNFGR_RESPONSE_t *response,
                                           L7_CNFGR_ERR_RC_t *reason);

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
static void pppoeCnfgrStateSet(pppoeCnfgrState_t newState)
{
  L7_LOGF(L7_LOG_SEVERITY_INFO, PTIN_PPPOE_COMPONENT_ID,
          "PPPoE transitioning from %s to %s",
          pppoeInitStateNames[pppoeCnfgrState],
          pppoeInitStateNames[newState]);
  pppoeCnfgrState = newState;
}

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
L7_RC_t pppoeCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                           L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t pppoeRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return(pppoeRC);
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
void pppoeApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  pppoeEventMsg_t msg;
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
    if (pppoeThreadCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.pppoeMsgData.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = PPPOE_CNFGR_INIT;
  if (osapiMessageSend(pppoe_Event_Queue, &msg, sizeof(pppoeEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(pppoeMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, PTIN_PPPOE_COMPONENT_ID,
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
void pppoeCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
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
      rc = pppoeCnfgrInitRequestHandle(request, &response, &reason);
      break;

    case L7_CNFGR_CMD_EXECUTE:
      if (request == L7_CNFGR_RQST_E_START)
      {
        pppoeCnfgrStateSet(PPPOE_PHASE_EXECUTE);
        rc = L7_SUCCESS;
        response = L7_CNFGR_CMD_COMPLETE;
        reason = 0;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = pppoeCnfgrNoopProcess(&response, &reason);
          pppoeCnfgrStateSet(PPPOE_PHASE_UNCONFIG_1);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = pppoeCnfgrUconfigPhase2(&response, &reason);
          pppoeCnfgrStateSet(PPPOE_PHASE_UNCONFIG_2);
          break;

        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
      rc = pppoeCnfgrNoopProcess(&response, &reason);
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
static L7_RC_t pppoeCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                        L7_CNFGR_RESPONSE_t *response,
                                        L7_CNFGR_ERR_RC_t *reason)
{
  L7_RC_t rc;

  switch (request)
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      rc = pppoeCnfgrInitPhase1Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        pppoeCnfgrFiniPhase1Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      rc = pppoeCnfgrInitPhase2Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        pppoeCnfgrFiniPhase2Process();
      }
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      rc = pppoeCnfgrInitPhase3Process(response, reason);
      if (rc != L7_SUCCESS)
      {
        pppoeCnfgrFiniPhase3Process();
      }
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      rc = pppoeCnfgrNoopProcess(response, reason);
      pppoeCnfgrStateSet(PPPOE_PHASE_WMU);
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
L7_RC_t pppoeCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t pppoeRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  pppoeRC = L7_SUCCESS;

  /* Allocate memory for PPPoE Binding AvlTree*/
  pppoeBindingTable.treeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(avlTreeTables_t)); 
  pppoeBindingTable.dataHeap = (ptinPppoeBindingInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(ptinPppoeBindingInfoData_t)); 
  if ((pppoeBindingTable.treeHeap == L7_NULLPTR) || (pppoeBindingTable.dataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error allocating data for PPPoE AVL Trees\n");
    return L7_FAILURE;
  }

  memset(pppoeBindingTable.treeHeap, 0x00, sizeof(PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(avlTreeTables_t)));
  memset(pppoeBindingTable.dataHeap, 0x00, sizeof(PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(ptinPppoeBindingInfoData_t)));
  memset(&pppoeBindingTable.avlTree, 0x00, sizeof(pppoeBindingTable.avlTree));

  avlCreateAvlTree(&pppoeBindingTable.avlTree, pppoeBindingTable.treeHeap, pppoeBindingTable.dataHeap,  
                   PTIN_SYSTEM_PPPOE_MAXCLIENTS, sizeof(ptinPppoeBindingInfoData_t), 0x10, sizeof(ptinPppoeClientDataKey_t));

  pppoeCnfgrStateSet(PPPOE_PHASE_INIT_1);
  return pppoeRC;
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
L7_RC_t pppoeCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;
  sysnetNotifyEntry_t snEntry;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  LOG_INFO(LOG_CTX_STARTUP,"Going to register pppoePduReceive related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_PPPOE, (L7_uint32) pppoePduReceive);

  memset(&snEntry, 0x00, sizeof(snEntry));
  strcpy(snEntry.funcName, "pppoePduReceive");
  snEntry.notify_pdu_receive = pppoePduReceive;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_PPPOE;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Failure registering sysNetRegisterPduReceive() for PPPoE packets");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc         = L7_ERROR;

    return rc;
  }

  pppoeCnfgrStateSet(PPPOE_PHASE_INIT_2);
  return rc;
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
L7_RC_t pppoeCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

///* Activate PPPoE by default */
//if (pppoeAdminModeEnable() != L7_SUCCESS)
//{
//  return L7_FAILURE;
//}

  pppoeCnfgrStateSet(PPPOE_PHASE_INIT_3);

  return( rc );
}

/*********************************************************************
* @purpose  This function undoes pppoeCnfgrFiniPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pppoeCnfgrFiniPhase1Process( void )
{
  pppoeCnfgrStateSet(PPPOE_PHASE_INIT_0);
}

/*********************************************************************
* @purpose  This function undoes pppoeCnfgrFiniPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pppoeCnfgrFiniPhase2Process(void)
{
  pppoeCnfgrStateSet(PPPOE_PHASE_INIT_1);
}

/*********************************************************************
* @purpose  This function undoes pppoeCnfgrFiniPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pppoeCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place dsCnfgrState to WMU */
  pppoeCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t pppoeCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc = L7_SUCCESS;

  pppoeCnfgrStateSet(PPPOE_PHASE_WMU);

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
static L7_RC_t pppoeThreadCreate(void)
{
  L7_int32 pppoeTaskHandle;

  /* Read/write lock to protect component data since processing occurs
   * on multiple threads (DS thread, UI thread, dot1q, etc) */
  if (osapiRWLockCreate(&pppoeCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Unable to create read/write lock for PPPoE");
    return L7_FAILURE;
  }

  /* Counting semaphore. Given whenever a message is added to any message queue
   * for the DHCP snooping thread. Taken when a message is read. */
  pppoeMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (pppoeMsgQSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, PTIN_PPPOE_COMPONENT_ID,
           "Failure creating DHCP snooping message queue semaphore.");
    return L7_FAILURE;
  }

  /* create queue for VLAN and initialization events */
  pppoe_Event_Queue = osapiMsgQueueCreate(DHCP_SNOOP_EVENT_QUEUE,
                                       DHCP_SNOOP_EVENTQ_MSG_COUNT,
                                       sizeof(pppoeEventMsg_t));

  if (pppoe_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, PTIN_PPPOE_COMPONENT_ID,
           "DHCP snooping unable to create event queue.");
    return L7_FAILURE;
  }

  /* create DHCP snooping packet queue */
  pppoe_Packet_Queue = osapiMsgQueueCreate(DHCP_SNOOP_PACKET_QUEUE,
                                           DHCP_SNOOP_PACKETQ_MSG_COUNT,
                                           sizeof(pppoeFrameMsg_t));

  if (pppoe_Packet_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, PTIN_PPPOE_COMPONENT_ID,
           "DHCP snooping unable to create packet queue.");
    return L7_FAILURE;
  }

  pppoeTaskHandle = osapiTaskCreate(DHCP_SNOOP_TASK,
                                    pppoeTask,
                                    L7_NULL, L7_NULLPTR,
                                    L7_DEFAULT_STACK_SIZE,
                                    L7_DEFAULT_TASK_PRIORITY,
                                    L7_DEFAULT_TASK_SLICE);

  if (pppoeTaskHandle == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, PTIN_PPPOE_COMPONENT_ID,
           "ERROR: Unable to create DHCP snooping thread.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


