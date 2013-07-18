/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_cnfgr.c
*
* @purpose    Contains definitions to support the configurator framework
*             for snooping component
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include "commdefs.h"
#include "l7_product.h"
#include "datatypes.h"
#include "nvstoreapi.h"
#include "dot1q_api.h"
#include "dot1s_api.h"
#include "buff_api.h"
#include "simapi.h"

#include "snooping_defs.h"
#include "snooping_sid.h"
#include "snooping_util.h"
#include "snooping_migrate.h"
#include "snooping_debug.h"
#include "snooping_ctrl.h"
#include "snooping_proto.h"

/* PTin Add: IGMPv3 */
#if SNOOP_PTIN_IGMPv3_GLOBAL

#include "ptin_globaldefs.h"

#if SNOOP_PTIN_IGMPv3_ROUTER
#include "snooping_ptin_grouptimer.h"
#include "snooping_ptin_sourcetimer.h"
#include "snooping_ptin_querytimer.h"
#endif

#if SNOOP_PTIN_IGMPv3_PROXY
#include "snooping_ptin_proxytimer.h"


#endif
#endif

#ifdef L7_NSF_PACKAGE
#include "snooping_ckpt.h"
#endif /* L7_NSF_PACKAGE */

#ifdef L7_MCAST_PACKAGE
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#endif /* L7_MCAST_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif /* L7_ROUTING_PACKAGE */
static snoop_eb_t  snoopEB;   /* Snoop execution block holder */
static snoop_cb_t *snoopCB;   /* Snoop Control blocks holder */

#define L7_MIN_GROUP_REGISTRATION_ENTRIES L7_MAX_GROUP_REGISTRATION_ENTRIES/4
/*********************************************************************
* @purpose  CNFGR System Initialization for Snooping component
*
* @param    pCmdData    Data structure for this CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the Snooping comp. This function is re-entrant.
*
* @end
*********************************************************************/
void snoopApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  snoopMgmtMsg_t msg;

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
    if (snoopInit(pCmdData) != L7_SUCCESS)
    {
      snoopInitUndo();
    }
  }
  else
  {
    memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = snoopCnfgr;
    if (osapiMessageSend(snoopEB.snoopQueue, &msg, SNOOP_MSG_SIZE,
                         L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
       if (osapiSemaGive(snoopEB.snoopMsgQSema) != L7_SUCCESS)
       {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopApiCnfgrCommand: Failed to give msgQueue semaphore\n");
       }
    }
  }
}

/*********************************************************************
* @purpose  System Initialization for Snooping component
*
* @param    pCmdData    Data structure for this CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  snoopMgmtMsg_t       msg;
  L7_CNFGR_CB_DATA_t  cbData;
  snoop_eb_t         *pSnoopEB = L7_NULLPTR;
  L7_uint32           i;

  pSnoopEB = &snoopEB;

  /* Initialize Execution block */
  memset(pSnoopEB , 0x00, sizeof(snoop_eb_t));

  pSnoopEB->snoopCnfgrState = SNOOP_PHASE_INIT_0;
  /* Create the Message queue */
  pSnoopEB->snoopQueue = (void *)osapiMsgQueueCreate(SNOOP_QUEUE,
                                SNOOP_MSG_COUNT, SNOOP_MSG_SIZE);
  if (pSnoopEB->snoopQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  pSnoopEB->snoopTimerQueue = (void *)osapiMsgQueueCreate(SNOOP_TIMER_QUEUE,
                                (L7_MAX_GROUP_REGISTRATION_ENTRIES * 3)/2, SNOOP_TIMER_MSG_SIZE);
  if (pSnoopEB->snoopTimerQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopInit: Timer msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* Find out how many snooping instances are supported*/
  /* Find out how many snooping instances are supported*/
  pSnoopEB->maxSnoopInstances = 0;

  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID,
                            L7_IGMP_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* the queue depth will be same as the number of pdu buffer pools */
    /* Create the Message queue */
    pSnoopEB->snoopIGMPQueue = (void *)osapiMsgQueueCreate(SNOOP_IGMP_PDU_QUEUE,
                                (SNOOP_NUM_SMALL_BUFFERS +
                                 SNOOP_NUM_LARGE_BUFFERS), SNOOP_PDU_MSG_SIZE);

    if (pSnoopEB->snoopIGMPQueue == L7_NULLPTR)
    {
      sysapiPrintf("snoopInit: IGMP PDU msgQueue creation error.\n");
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return L7_FAILURE;
    }
    pSnoopEB->maxSnoopInstances++;
  }

  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID,
                            L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* the queue depth will be same as the number of pdu buffer pools */
    /* Create the Message queue */
    pSnoopEB->snoopMLDQueue = (void *)osapiMsgQueueCreate(SNOOP_MLD_PDU_QUEUE,
                                (SNOOP_NUM_SMALL_BUFFERS +
                                 SNOOP_NUM_LARGE_BUFFERS), SNOOP_PDU_MSG_SIZE);
    if (pSnoopEB->snoopMLDQueue == L7_NULLPTR)
    {
      sysapiPrintf("snoopInit: MLD PDU msgQueue creation error.\n");
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return L7_FAILURE;
    }
    pSnoopEB->maxSnoopInstances++;
  }

  /* create the semaphore */
  pSnoopEB->snoopMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (pSnoopEB->snoopMsgQSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopInit: Unable to create snoop message queue semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

/* PTin added: IGMPv3 snooping */
#if 1
  /* create the semaphore */
  pSnoopEB->snoopPTinQueryQSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pSnoopEB->snoopPTinQueryQSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID, "snoopInit: Unable to create snoop query queue semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* Initialize snoopPTinQueryQueueFreePos */
  for(i=0; i<PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE ;++i)
  {
    pSnoopEB->snoopPTinQueryQueueFreeList[i] = i;
  }

  /* Initialize push and pop indexes */
  pSnoopEB->snoopPTinQueryQueueFreeListPopIdx = 0;
  pSnoopEB->snoopPTinQueryQueueFreeListPushIdx = 0;
  pSnoopEB->snoopPTinQueryQueueFreeListFull = L7_FALSE;
  pSnoopEB->snoopPTinQueryQueueFreeListEmpty = L7_TRUE;
#endif

  /* Create Snoop Task - Errors are logged inside the called func */
  if (snoopStartTask() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = snoopCnfgr;
  /* Send cnfgr phase 1 init event to snoopTask */
  if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE, L7_WAIT_FOREVER,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopInit: Failed to post message. msgqueue is full\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_FATAL;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
  else
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopApiCnfgrCommand: Failed to give msgQueue semaphore\n");
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for Snooping component
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void snoopInitUndo(void)
{
  /* Delete the message queue if it was created */
  if (snoopEB.snoopQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(snoopEB.snoopQueue);
  }

  if (snoopEB.snoopTimerQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(snoopEB.snoopTimerQueue);
  }

  if (snoopEB.snoopIGMPQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(snoopEB.snoopIGMPQueue);
  }

  if (snoopEB.snoopMLDQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(snoopEB.snoopMLDQueue);
  }

  /* Delete the snoopTask if it was created */
  if (snoopEB.snoopTaskId != L7_ERROR)
  {
    osapiTaskDelete(snoopEB.snoopTaskId);
  }

  /* Delete the snoop message queue Sema if it was created */
  if (snoopEB.snoopMsgQSema != L7_NULL)
  {
    (void)osapiSemaDelete(snoopEB.snoopMsgQSema);
  }

  /* Set snoop component to be in phase 1 */
  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_0;
}

/****************************************************************************
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
****************************************************************************/
L7_RC_t snoopCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t   snoopRC;
  L7_uint32 cbIndex = 0;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
   snoopRC     = L7_SUCCESS;

   /*
   snoopDebugTraceEnable();
   snoopDebugTraceFlagsSetAF(4);
   */

  /* Memory Allocations */

  /* Initialize execution block */
  if (snoopEBInit() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    snoopRC   = L7_ERROR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCnfgrInitPhase1Process: snoopEBInit failed\n");
    return snoopRC;
  }

  /* Instantiate CBs */
  snoopCB = L7_NULLPTR;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopEB.maxSnoopInstances: Allocating %u",sizeof(snoop_cb_t) *snoopEB.maxSnoopInstances);
  snoopCB = (snoop_cb_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                          sizeof(snoop_cb_t) * snoopEB.maxSnoopInstances);
  if (snoopCB == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    snoopRC   = L7_ERROR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCnfgrInitPhase1Process: snoopCB Allocation failed\n");
    return snoopRC;
  }

  /* Initialize each snoopInstances based on cnfgrfeature present check
   */
  cbIndex = 0;
  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID,
                            L7_IGMP_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    if (snoopCBInit(cbIndex, L7_AF_INET) != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      snoopRC   = L7_ERROR;
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
             "snoopCnfgrInitPhase1Process: snoopCB[%d] Allocation failed\n",
               cbIndex);
      return snoopRC;
    }
    cbIndex++;
  }

  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID,
                            L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    if (snoopCBInit(cbIndex, L7_AF_INET6) != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
       snoopRC   = L7_ERROR;
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
              "snoopCnfgrInitPhase1Process: snoopCB[%d] Allocation failed\n",
                cbIndex);
       return snoopRC;
    }
  }

  /* Support for MLD packets with IPv6 options header is not supported
    by all hardwares. Add to the list during integration of new hardware.
  */
  switch (cnfgrBaseTechnologySubtypeGet())
  {
    case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x:
    case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x:
    case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
    case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5682x:
    case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
         snoopEB.ipv6OptionsSupport = L7_TRUE;
         break;
    default:
         snoopEB.ipv6OptionsSupport = L7_FALSE;
          break;
  }
  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_1;

  return snoopRC;
}

/***************************************************************************
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
***************************************************************************/
L7_RC_t snoopCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t snoopRC;
  nvStoreFunctionList_t snoopNotifyFunctionList;
  L7_uint32 eventMask = 0;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  snoopRC     = L7_SUCCESS;

  /* Registrations
           1. nvstore
           2. NIM
           3. DOT1Q
           4. DOT1S
           5. Debug
   */


  /* nv Store */
  memset((void *) &snoopNotifyFunctionList, 0, sizeof(snoopNotifyFunctionList));
  snoopNotifyFunctionList.registrar_ID   = L7_SNOOPING_COMPONENT_ID;
  snoopNotifyFunctionList.notifySave     = snoopSave;
  snoopNotifyFunctionList.hasDataChanged = snoopHasDataChanged;
  snoopNotifyFunctionList.notifyConfigDump     = snoopCfgDump;
  snoopNotifyFunctionList.resetDataChanged = snoopResetDataChanged;

  if (nvStoreRegister(snoopNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;

  }

  /* NIM Inteface changes */
  if (nimRegisterIntfChange(L7_SNOOPING_COMPONENT_ID, snoopIntfChangeCallback,
                            snoopIntfStartupCallback, NIM_STARTUP_PRIO_IGMP_SNOOPING) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCnfgrInitPhase2Process: IGMP Snooping: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;
  }
  eventMask = VLAN_ADD_NOTIFY | VLAN_DELETE_PENDING_NOTIFY | VLAN_ADD_PORT_NOTIFY |
              VLAN_DELETE_PORT_NOTIFY | VLAN_INITIALIZED_NOTIFY | VLAN_RESTORE_NOTIFY;
  /* DOT1Q Vlan Changes */
  if (vlanRegisterForChange(snoopVlanChangeCallback, L7_SNOOPING_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCnfgrInitPhase2Process: Unable to register for VLAN change callback\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;
  }

  if (cnfgrIsFeaturePresent(L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_TRUE)
  {
    /* DOT1S Registration */
    if (dot1sRegisterIntfChange(L7_SNOOPING_COMPONENT_ID, snoopDot1sIntfChangeCallback) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
             "snoopCnfgrInitPhase2Process: Unable to register for dot1s interface change callback\n");

      *pResponse  = 0;
      *pReason    = L7_CNFGR_ERR_RC_FATAL;
      snoopRC     = L7_ERROR;

      return snoopRC;
    }
  }

  if (snoopCBGet(L7_AF_INET) != L7_NULLPTR)
  {
    /* Register for address change events with SIM */
    if (simAddrChangeEventRegister(L7_SNOOPING_COMPONENT_ID, SIM_ADDR_CHANGE_TYPE_NETWORK,
                                   L7_AF_INET, snoopNetworkAddrChanged) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopCnfgrInitPhase2Process: Unable to register with sim for address change events\n");
    }
  }

  /* register for debug */
  snoopDebugRegister();

#ifdef L7_NSF_PACKAGE
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Registering for NSF callbacks");
  if (snoopCheckpointCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SNOOPING_COMPONENT_ID,
           "snooping failed to register for checkpoint service callbacks.");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;
  }
#endif /* L7_NSF_PACKAGE */

#ifdef L7_MCAST_PACKAGE
  if (mcastMapRegisterAdminModeEventChange(L7_MRP_SNOOPING, snoopMcastEventCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SNOOPING_COMPONENT_ID,
           "snooping failed to register for mcast admin mode change callbacks.");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;
  }
#endif /* L7_MCAST_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
  /*------------------------------------------*/
  /* register callbacks with IP Mapping layer */
  /*------------------------------------------*/
  /* register snooping to receive a callback when an ip address is added
   * or deleted for routing vlan
   */
  if (ipMapRegisterRoutingEventChange(L7_IPRT_SNOOPING,
                                   "snoopRoutingEventCallback",
                                   snoopRoutingEventCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SNOOPING_COMPONENT_ID,
           "snooping failed to register for ip map routing change callbacks.");
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    *pResponse = 0;
    snoopRC     = L7_ERROR;
    return snoopRC;
  }
#endif
  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_2;

  return snoopRC;
}

/****************************************************************************
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
****************************************************************************/
L7_RC_t snoopCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t      snoopRC;
  snoop_cb_t  *pSnoopCB;
  L7_uint32    cbIndex;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
   snoopRC     = L7_SUCCESS;

  snoopEB.snoopWarmRestart = warmRestart;

  /*
  Clear the NIM port event mask until ready to receive events
  This is necessary because Phase3 can be re-entered without
  re-initializing the memory containing nimEventMask
  */

  memset(&snoopEB.nimEventMask, 0, sizeof(snoopEB.nimEventMask));
  nimRegisterIntfEvents(L7_SNOOPING_COMPONENT_ID, snoopEB.nimEventMask);

  /*
  If not a warm restart, clear snooping Checkpoint Data
  */

  if (!warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    /* Clear any checkpointed data */
    SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Not warm restart");
    snoopCheckpointFlushAll();
#endif /* L7_NSF_PACKAGE */
  }

  /* Read configuration from file */

  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Applying Config Data");
  for (cbIndex = L7_NULL, pSnoopCB = snoopCB;
       cbIndex < snoopEB.maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    snoopRC = snoopCnfgrConfigFileReadApply(pSnoopCB, warmRestart, pResponse, pReason);
    if (snoopRC != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopCnfgrInitPhase3Process: Failed to read configuration file\n");
      return snoopRC;
    }
  }
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Apply of Config Data done");

  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_3;
  return snoopRC;
}

/*********************************************************************
* @purpose  This function undoes snoopCnfgrInitPhase1Process
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopCnfgrFiniPhase1Process(void)
{
  L7_uint32 idx;

  /* Delete snoop instances */
  for (idx = 0; idx < snoopEB.maxSnoopInstances; idx++)
  {
    snoopCBInitUndo(idx);
  }

  if (snoopCB != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, snoopCB);
  }

  snoopEBInitUndo();
  /* Delete message queue, snoop task */
  snoopInitUndo();

  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes snoopCnfgrInitPhase2Process
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopCnfgrFiniPhase2Process(void)
{

  /* deregistrations with
             DOT1S, VLAN, NIM, NVStore
   */
  (void)nimDeRegisterIntfChange(L7_SNOOPING_COMPONENT_ID);
  (void)nvStoreDeregister(L7_SNOOPING_COMPONENT_ID);
#ifdef L7_MCAST_PACKAGE
  (void) mcastMapDeregisterAdminModeEventChange(L7_MRP_SNOOPING);
#endif /* L7_MCAST_PACKAGE */
#ifdef L7_ROUTING_PACKAGE
  (void) ipMapDeregisterRoutingEventChange(L7_IPRT_SNOOPING);
#endif /* L7_ROUTING_PACKAGE */

  snoopEB.snoopCnfgrState = SNOOP_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes snoopCnfgrInitPhase2Process
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place snoopCnfgrState to WMU */
  snoopCnfgrUconfigPhase2(&response, &reason);
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
* @notes    User has initiated a failover to the backup unit. If snooping is waiting
*           to checkpoint data to the backup unit, wait until that completes. In this
*           case, wait up to 20 seconds and then blunder on. This semaphore will
*           never be given. Assumption is that if we get here, the management unit
*           will definitely be rebooted.
*
*           Deprecated: checkpoint pending sema is now obsolete
*
* @end
***************************************************************************/
L7_RC_t snoopCnfgrTerminateProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t       snoopRC = L7_SUCCESS;
  snoop_eb_t    *pSnoopEB = L7_NULLPTR;

  pSnoopEB = &snoopEB;

  if (pSnoopEB != L7_NULL)
  {
      /*
      osapiSemaTake(pSnoopEB->snoopCkptPendingSema, 20000);
      */
  }

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(snoopRC);
}

/***************************************************************************
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
***************************************************************************/
L7_RC_t snoopCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t snoopRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(snoopRC);
}

/****************************************************************************
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
****************************************************************************/
L7_RC_t snoopCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t     snoopRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  snoopRC     = L7_SUCCESS;
  /* Restore snoop instances */
  if (snoopRestore() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    snoopRC     = L7_ERROR;

    return snoopRC;
  }
  memset(snoopEB.snoopIntfInfo, 0x00, sizeof(snoopIntfAcqInfo_t));
  return snoopRC;
}

/*********************************************************************
* @purpose  To parse the configurator commands send to snoopTask
*
* @param    pCmdData    Data structure for this CNFGR request
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{

  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL               warmRestart = L7_FALSE;

  L7_RC_t             snoopRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((snoopRC = snoopCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  snoopCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((snoopRC = snoopCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  snoopCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                warmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
                if ((snoopRC = snoopCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  snoopCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                snoopRC = snoopCnfgrNoopProccess( &response, &reason );
                snoopEB.snoopCnfgrState = SNOOP_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                snoopEB.snoopCnfgrState = SNOOP_PHASE_EXECUTE;

                snoopRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                snoopRC = snoopCnfgrNoopProccess( &response, &reason );
                snoopEB.snoopCnfgrState = SNOOP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                snoopRC = snoopCnfgrUconfigPhase2( &response, &reason );
                snoopEB.snoopCnfgrState = SNOOP_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
            snoopRC = snoopCnfgrTerminateProcess(&response, &reason);
            break;

          case L7_CNFGR_CMD_SUSPEND:
            snoopRC = snoopCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = snoopRC;
  if (snoopRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}

#if SNOOP_PTIN_IGMPv3_GLOBAL
/*PTIN Added MGDM Initializations*/
#if SNOOP_PTIN_IGMPv3_ROUTER
/*********************************************************************
* @purpose  MGMD Router Execution block initializations
*
* @param    None
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopPtinRouterAVLTreeInit(void)
{
  snoop_eb_t *pSnoopEB;
  pSnoopEB = &snoopEB;

  pSnoopEB->snoopPTinL3TreeHeap = (avlTreeTables_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  
  pSnoopEB->snoopPTinL3DataHeap = (snoopPTinL3InfoData_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinL3InfoData_t));

  if ((pSnoopEB->snoopPTinL3TreeHeap == L7_NULLPTR) || (pSnoopEB->snoopPTinL3DataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Error allocating data for snoop PTin Router AVL Tree \n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinL3AvlTree, 0x00, sizeof(avlTree_t));
  memset(pSnoopEB->snoopPTinL3TreeHeap, 0x00, sizeof(avlTreeTables_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);
  memset(pSnoopEB->snoopPTinL3DataHeap, 0x00, sizeof(snoopPTinL3InfoData_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);

  /* AVL Tree creations - snoopAvlTree*/
  avlCreateAvlTree(&(pSnoopEB->snoopPTinL3AvlTree), pSnoopEB->snoopPTinL3TreeHeap, pSnoopEB->snoopPTinL3DataHeap,
                   L7_MAX_GROUP_REGISTRATION_ENTRIES, sizeof(snoopPTinL3InfoData_t), 0x10, sizeof(snoopPTinL3InfoDataKey_t));
  return L7_SUCCESS;
}
#endif
#if SNOOP_PTIN_IGMPv3_PROXY


/*********************************************************************
* @purpose  Proxy Source Execution block initializations
*
* @param    None
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopPtinProxySourceAVLTreeInit(void)
{
  snoop_eb_t *pSnoopEB;
  pSnoopEB = &snoopEB;

  pSnoopEB->snoopPTinProxySourceTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  
  pSnoopEB->snoopPTinProxySourceDataHeap = (snoopPTinProxySource_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxySource_t));

  if ((pSnoopEB->snoopPTinProxySourceTreeHeap == L7_NULLPTR) || (pSnoopEB->snoopPTinProxySourceDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Error allocating data for snoop PTin Proxy Source AVL Tree \n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxySourceAvlTree, 0x00, sizeof(avlTree_t));
  memset(pSnoopEB->snoopPTinProxySourceTreeHeap, 0x00, sizeof(avlTreeTables_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);
  memset(pSnoopEB->snoopPTinProxySourceDataHeap, 0x00, sizeof(snoopPTinProxySource_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);

  /* AVL Tree creations - snoopAvlTree*/
  avlCreateAvlTree(&(pSnoopEB->snoopPTinProxySourceAvlTree), pSnoopEB->snoopPTinProxySourceTreeHeap, pSnoopEB->snoopPTinProxySourceDataHeap,
                   L7_MAX_GROUP_REGISTRATION_ENTRIES, sizeof(snoopPTinProxySource_t), 0x10, sizeof(snoopPTinProxySourceKey_t));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Proxy Group Execution block initializations
*
* @param    None
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopPtinProxyGroupAVLTreeInit(void)
{
  snoop_eb_t *pSnoopEB;
  pSnoopEB = &snoopEB;

  pSnoopEB->snoopPTinProxyGroupTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  
  pSnoopEB->snoopPTinProxyGroupDataHeap = (snoopPTinProxyGroup_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxyGroup_t));

  if ((pSnoopEB->snoopPTinProxyGroupTreeHeap == L7_NULLPTR) || (pSnoopEB->snoopPTinProxyGroupDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Error allocating data for snoop PTin Proxy Group AVL Tree \n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxyGroupAvlTree, 0x00, sizeof(avlTree_t));
  memset(pSnoopEB->snoopPTinProxyGroupTreeHeap, 0x00, sizeof(avlTreeTables_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);
  memset(pSnoopEB->snoopPTinProxyGroupDataHeap, 0x00, sizeof(snoopPTinProxyGroup_t) * L7_MAX_GROUP_REGISTRATION_ENTRIES);

  /* AVL Tree creations - snoopAvlTree*/
  avlCreateAvlTree(&(pSnoopEB->snoopPTinProxyGroupAvlTree), pSnoopEB->snoopPTinProxyGroupTreeHeap, pSnoopEB->snoopPTinProxyGroupDataHeap,
                   L7_MAX_GROUP_REGISTRATION_ENTRIES, sizeof(snoopPTinProxyGroup_t), 0x10, sizeof(snoopPTinProxyGroupKey_t));
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Proxy Interface Execution block initializations
*
* @param    None
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopPtinProxyInterfaceAVLTreeInit(void)
{
  snoop_eb_t *pSnoopEB;
  pSnoopEB = &snoopEB;

  pSnoopEB->snoopPTinProxyInterfaceTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      PTIN_SYSTEM_N_EVCS*sizeof(avlTreeTables_t));
  
  pSnoopEB->snoopPTinProxyInterfaceDataHeap = (snoopPTinProxyInterface_t *) osapiMalloc(L7_SNOOPING_COMPONENT_ID,
      PTIN_SYSTEM_N_EVCS*sizeof(snoopPTinProxyInterface_t));

  if ((pSnoopEB->snoopPTinProxyInterfaceTreeHeap == L7_NULLPTR) || (pSnoopEB->snoopPTinProxyInterfaceDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Error allocating data for snoop PTin Proxy Interface AVL Tree \n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, 0x00, sizeof(avlTree_t));
  memset(pSnoopEB->snoopPTinProxyInterfaceTreeHeap, 0x00, sizeof(avlTreeTables_t) * PTIN_SYSTEM_N_EVCS);
  memset(pSnoopEB->snoopPTinProxyInterfaceDataHeap, 0x00, sizeof(snoopPTinProxyInterface_t) * PTIN_SYSTEM_N_EVCS);

  /* AVL Tree creations - snoopAvlTree*/
  avlCreateAvlTree(&(pSnoopEB->snoopPTinProxyInterfaceAvlTree), pSnoopEB->snoopPTinProxyInterfaceTreeHeap, pSnoopEB->snoopPTinProxyInterfaceDataHeap,
                   PTIN_SYSTEM_N_EVCS, sizeof(snoopPTinProxyInterface_t), 0x10, sizeof(snoopPTinProxyInterfaceKey_t));
  return L7_SUCCESS;
}

#endif

void  checkIGMPv3Size(void)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_uint8: Allocating %u",sizeof(L7_uint8));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_inet_addr_t: Allocating %u",sizeof(L7_inet_addr_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"PTIN_SYSTEM_N_EVCS=%u L7_MAX_GROUP_REGISTRATION_ENTRIES=%u PTIN_SYSTEM_MAXINTERFACES_PER_GROUP=%u PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE=%u PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE=%u",PTIN_SYSTEM_N_EVCS,L7_MAX_GROUP_REGISTRATION_ENTRIES,PTIN_SYSTEM_MAXINTERFACES_PER_GROUP,PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE,PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE);
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinL3TreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinL3DataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinL3InfoData_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxySource_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxySource_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxyGroup_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceTreeHeap: Allocating %u",PTIN_SYSTEM_N_IGMP_INSTANCES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceDataHeap: Allocating %u",PTIN_SYSTEM_N_IGMP_INSTANCES*sizeof(snoopPTinProxyInterface_t));
}

#endif

/*********************************************************************
* @purpose  Snooping Execultion block initializations
*
* @param    None
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopEBInit(void)
{
  snoop_eb_t *pSnoopEB;
  L7_uint32   numBufs;
  L7_uint32 maxTimers = 0, idx;

  /* Create Heap for AVL tree */
  pSnoopEB = &snoopEB;
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  pSnoopEB->snoopTreeHeap =
                          (avlTreeTables_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                          L7_MAX_GROUP_REGISTRATION_ENTRIES *
                          sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopInfoData_t));
  pSnoopEB->snoopDataHeap  = (snoopInfoData_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                              L7_MAX_GROUP_REGISTRATION_ENTRIES *
                              sizeof(snoopInfoData_t));

  if ((pSnoopEB->snoopTreeHeap == L7_NULLPTR) ||
      (pSnoopEB->snoopDataHeap == L7_NULLPTR)
     )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Error allocating data for snoop AVL Tree \n");
    return L7_FAILURE;
  }

  /* AVL Tree creations - snoopAvlTree*/
  avlCreateAvlTree(&(pSnoopEB->snoopAvlTree),  pSnoopEB->snoopTreeHeap,
                   pSnoopEB->snoopDataHeap, L7_MAX_GROUP_REGISTRATION_ENTRIES,
                   sizeof(snoopInfoData_t), 0x10,
                   sizeof(snoopInfoDataKey_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE=%u PTIN_SYSTEM_MAXINTERFACES_PER_GROUP=%u",PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE,PTIN_SYSTEM_MAXINTERFACES_PER_GROUP);
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinL3TreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinL3DataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinL3InfoData_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxySource_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxySourceDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxySource_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupTreeHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyGroupDataHeap: Allocating %u",L7_MAX_GROUP_REGISTRATION_ENTRIES*sizeof(snoopPTinProxyGroup_t));

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceAvlTree: Allocating %u",sizeof(avlTree_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceTreeHeap: Allocating %u",PTIN_SYSTEM_N_IGMP_INSTANCES*sizeof(avlTreeTables_t));
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopPTinProxyInterfaceDataHeap: Allocating %u",PTIN_SYSTEM_N_IGMP_INSTANCES*sizeof(snoopPTinProxyInterface_t));
  


/* DFF - PTin added: IGMPv3 snooping */
#if SNOOP_PTIN_IGMPv3_GLOBAL

#if SNOOP_PTIN_IGMPv3_ROUTER
  if ( snoop_ptin_grouptimer_init()!=L7_SUCCESS) // IGMPv3 grouptimer
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopEBInit: snoop_ptin_grouptimer_init() failed");
    return L7_FAILURE;
  }
  if ( snoop_ptin_sourcetimer_init()!=L7_SUCCESS) // IGMPv3 sourcetimer
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopEBInit: snoop_ptin_sourcetimer_init() failed");
    return L7_FAILURE;
  }
  if ( snoop_ptin_querytimer_init()!=L7_SUCCESS) // IGMPv3 querytimer
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopEBInit: snoop_ptin_querytimer_init() failed");
    return L7_FAILURE;
  }

  if ( snoopPtinRouterAVLTreeInit()!=L7_SUCCESS)//Initialization of Memory for the Component of Router (Downstream Interfaces)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopEBInit: snoopRouterInit() failed");
    return L7_FAILURE;
  }
#endif
  
#if SNOOP_PTIN_IGMPv3_PROXY

  if ( snoopPtinProxySourceAVLTreeInit()!=L7_SUCCESS)//Initialization of Memory for the Component of Proxy Source
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopPtinProxySourceAVLTreeInit failed");
    return L7_FAILURE;
  }

  if ( snoopPtinProxyGroupAVLTreeInit()!=L7_SUCCESS)//Initialization of Memory for the Component of Proxy Group
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopPtinProxyGroupAVLTreeInit failed");
    return L7_FAILURE;
  }

  if ( snoopPtinProxyInterfaceAVLTreeInit()!=L7_SUCCESS)//Initialization of Memory for the Component of Proxy Interface
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopPtinProxyInterfaceAVLTreeInit failed");
    return L7_FAILURE;
  }

  if ( snoop_ptin_proxytimer_init()!=L7_SUCCESS)//Initialization of Proxy Timer
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"snoop_ptin_proxy_timer_init() failed ");
    return L7_FAILURE;
  } 


#endif
    
#endif

  /* L3 AVL Tree */
#ifdef L7_MCAST_PACKAGE
//pSnoopEB->snoopL3TreeHeap =
//                        (avlTreeTables_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
//                        L7_MULTICAST_FIB_MAX_ENTRIES *
//                        sizeof(avlTreeTables_t));
//pSnoopEB->snoopL3DataHeap  = (snoopL3InfoData_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
//                            L7_MULTICAST_FIB_MAX_ENTRIES *
//                            sizeof(snoopL3InfoData_t));
//
//if ((pSnoopEB->snoopL3TreeHeap == L7_NULLPTR) ||
//    (pSnoopEB->snoopL3DataHeap == L7_NULLPTR)
//   )
//{
//  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
//         "snoopEBInit: Error allocating data for snoop L3 AVL Tree \n");
//  return L7_FAILURE;
//}
//
///* AVL Tree creations - snoopAvlTree*/
//avlCreateAvlTree(&(pSnoopEB->snoopL3AvlTree),  pSnoopEB->snoopL3TreeHeap,
//                 pSnoopEB->snoopL3DataHeap, L7_MULTICAST_FIB_MAX_ENTRIES,
//                 sizeof(snoopL3InfoData_t), 0x10,
//                 sizeof(snoopL3InfoDataKey_t));
#endif

#ifdef L7_NSF_PACKAGE
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Allocating EB checkpoint info");
  if (snoopCheckpointEBInfoAlloc(pSnoopEB) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
            "snoopEBInit: Error allocating Checkpoint data structures\n");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "EB checkpoint info allocated");
#endif /* L7_NSF_PACKAGE */

  /* Buffer pool creations
      1. Timer Pool
  */

  if (snoopEB.maxSnoopInstances > 1)
  {
    numBufs = (SNOOP_NUM_SMALL_BUFFERS * 2);
  }
  else
  {
    numBufs = SNOOP_NUM_SMALL_BUFFERS;
  }

  /* Small buffer pool */
  if (bufferPoolInit(numBufs, SNOOP_SMALL_BUFFER_SIZE, "Snoop Sm Bufs",
                     &(pSnoopEB->snoopSmallBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopEBInit: Error allocating small buffers."
        " Could not allocate buffers for small IGMP packets");
    return L7_FAILURE;
  }

  /* Medium buffer pool */
  if (bufferPoolInit(SNOOP_NUM_MED_BUFFERS, SNOOP_MED_BUFFER_SIZE, "Snoop Md Bufs",
                     &(pSnoopEB->snoopMedBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopEBInit: Error allocating Med buffers."
        " Could not allocate buffers for Med IGMP packets");
    return L7_FAILURE;
  }

  /* Large buffer pool */
  if (bufferPoolInit(SNOOP_NUM_LARGE_BUFFERS, L7_MAX_FRAME_SIZE, "Snoop Lg Bufs",
                     &(pSnoopEB->snoopLargeBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopEBInit: Error allocating large buffers."
        " Could not allocate buffers for large IGMP packets");
    return L7_FAILURE;
  }

  /* Timer buffer pool */
  if (bufferPoolInit(SNOOP_NUM_GRP_TIMERS, sizeof(snoopGrpTimerData_t), "Snoop Tm Bufs",
                     &(pSnoopEB->snoopTimerBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Error allocating buffers for timers\n");
    return L7_FAILURE;
  }

  /* Component Interface Acquired list */
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopIntfInfo: Allocating %u",sizeof(snoopIntfAcqInfo_t)*platIntfMaxCountGet());
  pSnoopEB->snoopIntfInfo   = osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                             sizeof(snoopIntfAcqInfo_t) *
                             platIntfMaxCountGet());

  if (pSnoopEB->snoopIntfInfo == L7_NULLPTR)
  {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "snoopEBInit: Failed to allocate memory for snoopIntfInfo\n");
     return L7_FAILURE;
  }

  /* Interface Map table */
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopIntfMapTbl: Allocating %u",sizeof(L7_uint32)*platIntfMaxCountGet());
  pSnoopEB->snoopIntfMapTbl = osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                             sizeof(L7_uint32) *
                             platIntfMaxCountGet());
  if (pSnoopEB->snoopIntfMapTbl == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Failed to allocate memory for snoopIntfMapTbl\n");
    return L7_FAILURE;
  }

  /* Timer Initializations */
    if(bufferPoolInit(SNOOP_NUM_GRP_TIMERS + (maxSnoopInstancesGet()*SNOOP_NUM_TIMERS_PER_INSTANCE),
                                        L7_APP_TMR_NODE_SIZE, "Snooping Timer Bufs", &pSnoopEB->appTimerBufferPoolId) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
               "snoopEBInit: Failed to allocate memory for Snooping timer buffers\n");
        return L7_FAILURE;
    }

  /* Register for time ticks with appTimer */
  pSnoopEB->timerCB =  appTimerInit(L7_SNOOPING_COMPONENT_ID, snoopTimerExpiryHdlr,
                                    L7_NULLPTR, SNOOP_TIMER_INTERVAL, pSnoopEB->appTimerBufferPoolId);

  if (pSnoopEB->timerCB == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: App Timer Initialization Failed.\n");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopEntryTimerFreeList: Allocating %u",sizeof(L7_int32)*SNOOP_ENTRY_TIMER_BLOCKS_COUNT);
  pSnoopEB->snoopEntryTimerFreeList =  osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                                 sizeof(L7_int32) *
                                                 SNOOP_ENTRY_TIMER_BLOCKS_COUNT);
  if (pSnoopEB->snoopEntryTimerFreeList == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Entry App Timer free list alloc Failed.\n");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopEntryTimerCBList: Allocating %u",sizeof(snoopAppTimer_t)*SNOOP_ENTRY_TIMER_BLOCKS_COUNT);
  pSnoopEB->snoopEntryTimerCBList =  osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                                 sizeof(snoopAppTimer_t) *
                                                 SNOOP_ENTRY_TIMER_BLOCKS_COUNT);
  if (pSnoopEB->snoopEntryTimerCBList == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Entry App Timer ctrl blk list alloc Failed.\n");
    return L7_FAILURE;
  }

  for (idx = 0; idx < SNOOP_ENTRY_TIMER_BLOCKS_COUNT; idx++)
  {
    /* Use 1 to SNOOP_ENTRY_TIMER_BLOCKS_COUNT for snoopEntryCBs and 0 for snoopEB->timerCB */
    pSnoopEB->snoopEntryTimerCBList[idx].timerCB = appTimerInit(L7_SNOOPING_COMPONENT_ID, snoopTimerExpiryHdlr,
                                                       (void *)(idx + 1), SNOOP_TIMER_INTERVAL,
                                                        pSnoopEB->appTimerBufferPoolId);
    if (pSnoopEB->snoopEntryTimerCBList[idx].timerCB == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
             "snoopEBInit: snoopEntry App Timer[%d] Initialization Failed.\n", idx);
      return L7_FAILURE;
    }
    pSnoopEB->snoopEntryTimerCBList[idx].pSnoopEntry = L7_NULLPTR;
    pSnoopEB->snoopEntryTimerFreeList[idx] = idx;
  }
  pSnoopEB->snoopEntryTimerFreeIdx = 0; /* point to the first entry in free list */

  /* Create timer handles */
  /* Allocate memory for the Handle List */
  maxTimers = SNOOP_NUM_GRP_TIMERS +
              (maxSnoopInstancesGet() * SNOOP_NUM_TIMERS_PER_INSTANCE);
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"handleListMemHndl: Allocating %u",(maxTimers * sizeof (handle_member_t)));
  if ((pSnoopEB->handleListMemHndl =
        (handle_member_t*) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                        (maxTimers * sizeof (handle_member_t))))
                        == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Error allocating Handle List Buffers\n");
    return L7_FAILURE;
  }
  if(handleListInit (L7_SNOOPING_COMPONENT_ID, maxTimers,
                     &(pSnoopEB->handle_list), pSnoopEB->handleListMemHndl)
                  != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopEBInit: Unable to create timer handle list\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Snooping Execution block De-initializations
*
* @param    none
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopEBInitUndo(void)
{
  snoop_eb_t *pSnoopEB;
  L7_uint32   idx;

  pSnoopEB = &snoopEB;

  /* cleanup buffer pools */
  if (pSnoopEB->snoopSmallBufferPoolId != 0)
  {
    bufferPoolDelete(pSnoopEB->snoopSmallBufferPoolId);
    pSnoopEB->snoopSmallBufferPoolId = 0;
  }

  if (pSnoopEB->snoopMedBufferPoolId != 0)
  {
    bufferPoolDelete(pSnoopEB->snoopMedBufferPoolId);
    pSnoopEB->snoopMedBufferPoolId = 0;
  }

  if (pSnoopEB->snoopLargeBufferPoolId != 0)
  {
    bufferPoolDelete(pSnoopEB->snoopLargeBufferPoolId);
    pSnoopEB->snoopLargeBufferPoolId = 0;
  }

  if (pSnoopEB->snoopTimerBufferPoolId != 0)
  {
    bufferPoolDelete(pSnoopEB->snoopTimerBufferPoolId);
    pSnoopEB->snoopTimerBufferPoolId = 0;
  }

  if (pSnoopEB->appTimerBufferPoolId != 0)
  {
    bufferPoolDelete(pSnoopEB->appTimerBufferPoolId);
    pSnoopEB->appTimerBufferPoolId = 0;
  }

  /* Destroy the AVL Tree */
  if (pSnoopEB->snoopTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopTreeHeap);
    pSnoopEB->snoopTreeHeap = L7_NULLPTR;
  }

  if (pSnoopEB->snoopDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopDataHeap);
    pSnoopEB->snoopDataHeap = L7_NULLPTR;
  }

#ifdef L7_MCAST_PACKAGE
  /* Destroy the L3 AVL Tree */
  if (pSnoopEB->snoopL3TreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopL3TreeHeap);
    pSnoopEB->snoopL3TreeHeap = L7_NULLPTR;
  }

  if (pSnoopEB->snoopL3DataHeap != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopL3DataHeap);
    pSnoopEB->snoopL3DataHeap = L7_NULLPTR;
  }
#endif /* L7_MCAST_PACKAGE  */
  /* Destroy the timer data */
  if (pSnoopEB->handle_list != L7_NULLPTR)
  {
    osapiFree (L7_SNOOPING_COMPONENT_ID, pSnoopEB->handleListMemHndl);
    (void)handleListDeinit(L7_SNOOPING_COMPONENT_ID, pSnoopEB->handle_list);
    pSnoopEB->handle_list = L7_NULLPTR;
  }

  if (pSnoopEB->timerCB != L7_NULLPTR)
  {
    (void)appTimerDeInit(pSnoopEB->timerCB);
    pSnoopEB->timerCB = L7_NULLPTR;
  }

  for (idx = 0; idx < SNOOP_ENTRY_TIMER_BLOCKS_COUNT; idx++)
  {
    if (pSnoopEB->snoopEntryTimerCBList[idx].timerCB != L7_NULLPTR)
    {
      (void)appTimerDeInit(pSnoopEB->snoopEntryTimerCBList[idx].timerCB);
    }
  }

  if (pSnoopEB->snoopEntryTimerCBList != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopEntryTimerCBList);
    pSnoopEB->snoopEntryTimerCBList = L7_NULLPTR;
  }

  if (pSnoopEB->snoopEntryTimerFreeList != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopEntryTimerFreeList);
    pSnoopEB->snoopEntryTimerFreeList = L7_NULLPTR;
  }

  /* Free up alloced memory */
  if (pSnoopEB->snoopIntfInfo != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopIntfInfo);
    pSnoopEB->snoopIntfInfo = L7_NULLPTR;
  }

  if (pSnoopEB->snoopIntfMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopEB->snoopIntfMapTbl);
    pSnoopEB->snoopIntfMapTbl = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose  Snooping Control block initializations
*
* @param    cbIndex  - @b{(input)}  Instance index whose Cb is
*                                   to be initalized.
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS - Initialization complete
*           L7_FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopCBInit(L7_uint32 cbIndex, L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* validate the cbIndex */
  if (cbIndex >= snoopEB.maxSnoopInstances)
  {
    return L7_ERROR;
  }

   pSnoopCB = (snoopCB + cbIndex);
  /* Control Block initialization */
   pSnoopCB->family  = family;
   pSnoopCB->cbIndex = cbIndex;

   /* Create Heap for Timer AVL tree */
   LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMrtrTimerTreeHeap: Allocating %u",SNOOP_NUM_MRTR_TIMERS*sizeof(avlTreeTables_t));
   pSnoopCB->snoopMrtrTimerTreeHeap =
                           (avlTreeTables_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                           SNOOP_NUM_MRTR_TIMERS *
                           sizeof(avlTreeTables_t));
   LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMrtrTimerDataHeap: Allocating %u",SNOOP_NUM_MRTR_TIMERS*sizeof(snoopMrtrTimerData_t));
   pSnoopCB->snoopMrtrTimerDataHeap =
             (snoopMrtrTimerData_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                    SNOOP_NUM_MRTR_TIMERS *
                                    sizeof(snoopMrtrTimerData_t));

   if ((pSnoopCB->snoopMrtrTimerTreeHeap == L7_NULLPTR) ||
       (pSnoopCB->snoopMrtrTimerDataHeap == L7_NULLPTR)
      )
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "snoopCBInit: Error allocating data for mrouter timer AVL Tree \n");
     return L7_FAILURE;
   }

   /* AVL Tree creations - snoopAvlTree*/
   avlCreateAvlTree(&(pSnoopCB->snoopMrtrTimerAvlTree),
                    pSnoopCB->snoopMrtrTimerTreeHeap,
                    pSnoopCB->snoopMrtrTimerDataHeap, SNOOP_NUM_MRTR_TIMERS,
                    sizeof(snoopMrtrTimerData_t), 0x10,
                    sizeof(snoopMrtrTimerDataKey_t));

   /* Create Heap for VlanInfo AVL tree */
   LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopOperDataTreeHeap: Allocating %u",platVlanTotalMaxEntriesGet()*sizeof(avlTreeTables_t));
   pSnoopCB->snoopOperDataTreeHeap =
                           (avlTreeTables_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                           platVlanTotalMaxEntriesGet() *
                           sizeof(avlTreeTables_t));
   LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopOperDataDataHeap: Allocating %u",platVlanTotalMaxEntriesGet()*sizeof(snoopOperData_t));
   pSnoopCB->snoopOperDataDataHeap =
             (snoopOperData_t *)osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                    platVlanTotalMaxEntriesGet() *
                                    sizeof(snoopOperData_t));

   if ((pSnoopCB->snoopOperDataDataHeap == L7_NULLPTR) ||
       (pSnoopCB->snoopOperDataTreeHeap == L7_NULLPTR)
      )
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "snoopCBInit: Error allocating data for oper data AVL Tree \n");
     return L7_FAILURE;
   }

   /* AVL Tree creations - snoopOperDataAVLTree*/
   avlCreateAvlTree(&(pSnoopCB->snoopOperDataAvlTree),
                    pSnoopCB->snoopOperDataTreeHeap,
                    pSnoopCB->snoopOperDataDataHeap,
                    platVlanTotalMaxEntriesGet(),
                    sizeof(snoopOperData_t), 0x10,
                    sizeof(L7_ushort16));

  (void)avlSetAvlTreeComparator(&(pSnoopCB->snoopOperDataAvlTree), avlCompareShort16);

#ifdef L7_NSF_PACKAGE
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, pSnoopCB->family, "Allocating CB checkpoint info");
  if (snoopCheckpointCBInfoAlloc(pSnoopCB) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
              "snoopCBInit: Error allocating data for NSF Checkpointing");
      return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, pSnoopCB->family, "CB checkpoint info allocated");
#endif /* L7_NSF_PACKAGE */

  /* Config structures */
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopCfgData: Allocating %u",sizeof(snoopCfgData_t));
  pSnoopCB->snoopCfgData = osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                       sizeof(snoopCfgData_t));

  if (pSnoopCB->snoopCfgData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCBInit: Memory insufficient for snoop configuration structure");
    return L7_FAILURE; /* Memory Allocation failed! */
  }

   /* Debug config structure */
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopDebugCfg: Allocating %u",sizeof(snoopDebugCfg_t));
  pSnoopCB->snoopDebugCfg = osapiMalloc(L7_SNOOPING_COMPONENT_ID,
                                              sizeof(snoopDebugCfg_t));
  if (pSnoopCB->snoopDebugCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopCBInit: Memory insufficient for snoop debug configuration structure");
    return L7_FAILURE; /* Memory Allocation failed! */
  }

  /* Initialize CB with default values */
  memset(pSnoopCB->snoopCfgData, 0x00,
         sizeof(snoopCfgData_t));

  pSnoopCB->snoopExec = &snoopEB; /* For quick reference */

  memset(&(pSnoopCB->counters), 0x00, sizeof(snoopStats_t));

  pSnoopCB->snoopDebugPacketTraceTxFlag = L7_FALSE;
  pSnoopCB->snoopDebugPacketTraceRxFlag = L7_FALSE;

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  snoopDebugCfgRead(pSnoopCB);
  snoopApplyDebugConfigData(pSnoopCB);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Snooping Control block De-initializations
*
* @param    cbIndex  - @b{(input)}  Instance index whose Cb is
*                                   to be de-initalized.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopCBInitUndo(L7_uint32 cbIndex)
{
  snoop_cb_t *pSnoopCB;

  /* validate the cbIndex */
  if (cbIndex >= snoopEB.maxSnoopInstances)
  {
    return;
  }


  if (snoopCB == L7_NULLPTR)
  {
    return;
  }

  if ((snoopCB+cbIndex) != L7_NULLPTR)
  {
    pSnoopCB = (snoopCB + cbIndex);

    if (pSnoopCB->snoopCfgData != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopCfgData);
    }

    if (pSnoopCB->snoopDebugCfg != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopDebugCfg);
    }

    /* Destroy the Operational data AVL Tree */
    if (pSnoopCB->snoopOperDataTreeHeap != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopOperDataTreeHeap);
      pSnoopCB->snoopOperDataTreeHeap = L7_NULLPTR;
    }

    if (pSnoopCB->snoopOperDataDataHeap != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopOperDataDataHeap);
      pSnoopCB->snoopOperDataDataHeap = L7_NULLPTR;
    }

    /* Destroy the MRTR timer data AVL Tree */
    if (pSnoopCB->snoopMrtrTimerTreeHeap != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopMrtrTimerTreeHeap);
      pSnoopCB->snoopMrtrTimerTreeHeap = L7_NULLPTR;
    }

    if (pSnoopCB->snoopMrtrTimerDataHeap != L7_NULLPTR)
    {
      osapiFree(L7_SNOOPING_COMPONENT_ID, pSnoopCB->snoopMrtrTimerDataHeap);
      pSnoopCB->snoopMrtrTimerDataHeap = L7_NULLPTR;
    }

    memset(&(pSnoopCB->counters), 0x00, sizeof(snoopStats_t));
    pSnoopCB->family = L7_NULL;
    pSnoopCB->snoopExec = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  Start Snooping task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopStartTask(void)
{
  snoopEB.snoopTaskId = osapiTaskCreate("snoopTask", snoopTask, 0, 0,
                                  snoopSidDefaultStackSize(),//PTIN
                                  snoopSidDefaultTaskPriority(),
                                  snoopSidDefaultTaskSlice());

  if (snoopEB.snoopTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopStartTask: Could not create task snoopTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_IGMP_SNOOPING_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
           "snoopStartTask: Unable to initialize snoopTask()\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the Snoop Control block if it is supported
*
* @param    family  @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                 L7_AF_INET6 => MLD Snooping
*
* @returns  pointer to the snoop control block
* @returns  L7_NULLPTR  -  If invalid snoop instance
*
* @notes    none
*
* @end
*********************************************************************/
snoop_cb_t *snoopCBGet(L7_uchar8 family)
{
  L7_uint32   cbIndex;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  for (cbIndex = 0; cbIndex < snoopEB.maxSnoopInstances; cbIndex++)
  {
     pSnoopCB = (snoopCB + cbIndex);
     if (family == pSnoopCB->family)
     {
       break;
     }
  }
  if (cbIndex == snoopEB.maxSnoopInstances)
  {
    pSnoopCB = L7_NULLPTR;
  }
  return pSnoopCB;
}
/*********************************************************************
* @purpose  Get the Snoop Execution block
*
* @param    none
*
* @returns  pointer to snoop execution block
*
* @notes    none
*
* @end
*********************************************************************/
snoop_eb_t *snoopEBGet(void)
{
  return &snoopEB;
}
/*********************************************************************
* @purpose  Get the First Snoop Control block
*
* @param    none
*
* @returns  pSnoopCB pointer to first supported snoop instance
*
* @notes    none
*
* @end
*********************************************************************/
snoop_cb_t *snoopCBFirstGet(void)
{
  return snoopCB;
}
/*********************************************************************
* @purpose  This function reads configuration file.
*
* @param    pSnoopCB        @b{(input)}   Snooping Control Block.
* @param    pResponse       @b{(output)}  Response if L7_SUCCESS.
* @param    pReason         @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopCnfgrConfigFileReadApply(snoop_cb_t *pSnoopCB, L7_BOOL warmRestart,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc = L7_FAILURE;

  if (pSnoopCB->family == L7_AF_INET)
  {
     rc = sysapiCfgFileGet(L7_SNOOPING_COMPONENT_ID,
          SNOOP_IGMP_CFG_FILENAME,
         (L7_char8 *)pSnoopCB->snoopCfgData,
          sizeof(snoopCfgData_t),
          &pSnoopCB->snoopCfgData->checkSum,
          SNOOP_IGMP_CFG_VER_CURRENT, snoopIGMPBuildDefaultConfigData,
          snoopMigrateConfigData);
  }
  else
  {
     rc = sysapiCfgFileGet(L7_SNOOPING_COMPONENT_ID,
          SNOOP_MLD_CFG_FILENAME,
          (L7_char8 *)pSnoopCB->snoopCfgData,
          sizeof(snoopCfgData_t),
          &pSnoopCB->snoopCfgData->checkSum,
          SNOOP_MLD_CFG_VER_CURRENT, snoopMLDBuildDefaultConfigData,
          L7_NULL);
  }

  if (rc != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopCnfgrConfigFileReadApply: Error retrieving config sysapiCfgFileGet().\n");
    return L7_FAILURE;
  }

  /*
  If this is a warm restart, then defer applying the config data until
  after the checkpointed data has been restored
  */
  if (warmRestart == L7_FALSE)
  {
    if (snoopApplyConfigData(pSnoopCB) != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopCnfgrConfigFileReadApply: Error applying config snoopApplyConfigData().\n");
      return L7_FAILURE;
    }
  }
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the number of snoop instances supported
*
* @param    none
*
* @returns  number of snoop instances
*
* @comments none
*
* @end
*********************************************************************/
L7_uint32 maxSnoopInstancesGet(void)
{
  return snoopEB.maxSnoopInstances;
}
/************************************************************************
* @purpose  Validate whether snoop component is ready to accept external
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
L7_BOOL snoopIsReady(void)
{

  return (((snoopEB.snoopCnfgrState == SNOOP_PHASE_INIT_3) || \
                         (snoopEB.snoopCnfgrState == SNOOP_PHASE_EXECUTE) || \
                          snoopEB.snoopCnfgrState == SNOOP_PHASE_UNCONFIG_1)) \
                          ? (L7_TRUE) : (L7_FALSE);
}
