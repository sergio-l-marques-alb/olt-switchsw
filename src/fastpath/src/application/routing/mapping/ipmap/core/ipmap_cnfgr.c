/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipmap_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 07/09/2003
*
* @author markl
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "l7_ipinclude.h"
#include "dot1q_api.h"
#include "rtiprecv.h"

#ifdef L7_NSF_PACKAGE
#include "ipmap_ckpt.h"
#include "ipstk_ckpt_api.h"
#include "ipmap_dhcp_ckpt.h"
#endif

#include "cli_txt_cfg_api.h"

ipMapCnfgrState_t ipMapCnfgrState = IPMAP_PHASE_INIT_0;
extern void *ipMapForwardQueue[L7_L3_FORWARD_QUEUE_COUNT];
extern L7_uint32 ipMapForwardQueueHWMark[L7_L3_FORWARD_QUEUE_COUNT];
extern void *ipMapProcess_Queue;

extern L7_int32 ipMapForwardingTaskId;
extern L7_int32 ipMapProcessingTaskId;
extern L7_ipMapCfg_t           *ipMapCfg;
extern L7_rtrStaticRouteCfg_t  *route;
extern L7_rtrStaticArpCfg_t    *arp;
extern ipMapInfo_t             *pIpMapInfo;
extern L7_uint32               *intIfToCfgIndex;
extern pIpRoutingEventNotifyList_t  pIpIntfStateNotifyList;
extern ipMapIntfInfo_t     *ipMapIntfInfo;
extern ipMapLockInfo_t *ipMapLockInfo;
extern L7_uint32 ipMapTraceFlags;

extern osapiRWLock_t  ipRwLock;

/* array of rtoRouteChange_t objects for best route notification. */
extern rtoRouteChange_t *ipMapRouteChangeBuf;  

extern void IpRxStatsReset(void);

extern void ipMapDebugRegister(void);
#ifdef L7_QOS_FLEX_PACKAGE_ACL
extern void ipMapDebugCfgRead(void);
#endif

extern void ipMapDebugRoutingInfoDumpRegister(void);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for IpMap component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the ipMap comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ipMapApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  ipMapMsg_t   msg;
  L7_CNFGR_CB_DATA_t    cbData;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ( pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR; 
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;

    cnfgrApiCallback(&cbData);
    return;
  }

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (ipMapInit(pCmdData) != L7_SUCCESS)
      ipMapInitUndo();
  } else
  {
    memcpy(&msg.type.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = IPMAP_CNFGR;
    osapiMessageSend(ipMapProcess_Queue, &msg, sizeof(ipMapMsg_t), L7_WAIT_FOREVER, 
                     L7_MSG_PRIORITY_NORM); 
  }
}

/*********************************************************************
*
* @purpose  System Initialization for IGMP Snooping component
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
L7_RC_t ipMapInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  ipMapMsg_t   msg;
  L7_CNFGR_CB_DATA_t    cbData;
  static const char *routine_name = "ipMapInit()";

  /* create the task queues  */
  ipMapForwardQueue[L7_L3_FWD_QUEUE] = (void *)osapiMsgQueueCreate(IPMAP_FORWARD_QUEUE,
                                                    IPMAP_FWD_MSG_COUNT, 
                                                    sizeof(ipMapMsg_t) );
  ipMapForwardQueueHWMark[L7_L3_FWD_QUEUE] = 0;

  ipMapForwardQueue[L7_L3_FWDPRI_QUEUE] = (void *)osapiMsgQueueCreate(IPMAP_FWD_PRIORITY_QUEUE,
                                                    IPMAP_FWD_PRIORITY_MSG_COUNT, 
                                                    sizeof(ipMapMsg_t) );
  ipMapForwardQueueHWMark[L7_L3_FWDPRI_QUEUE] = 0;

  ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE] = (void *)osapiMsgQueueCreate(IPMAP_FWD_HIGHPRIORITY_QUEUE,
                                                    IPMAP_FWD_HIGHPRIORITY_MSG_COUNT, 
                                                    sizeof(ipMapMsg_t) );
  ipMapForwardQueueHWMark[L7_L3_FWDHIGHPRI_QUEUE] = 0;

  ipMapForwardQueue[L7_L3_ARP_QUEUE] = (void *)osapiMsgQueueCreate(IPMAP_ARP_QUEUE,
                                                IPMAP_ARP_MSG_COUNT, 
                                                sizeof(ipMapMsg_t) );
  ipMapForwardQueueHWMark[L7_L3_ARP_QUEUE] = 0;

  ipMapProcess_Queue = (void *)osapiMsgQueueCreate( IPMAP_PROCESS_QUEUE,
                                                    IPMAP_PROC_MSG_COUNT, 
                                                    sizeof(ipMapMsg_t) );

  if ((ipMapForwardQueue[L7_L3_FWD_QUEUE] == L7_NULLPTR) ||
      (ipMapForwardQueue[L7_L3_FWDPRI_QUEUE] == L7_NULLPTR) ||
      (ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE] == L7_NULLPTR) ||
      (ipMapForwardQueue[L7_L3_ARP_QUEUE] == L7_NULLPTR) ||
      (ipMapProcess_Queue == L7_NULLPTR))
  {
    IPMAP_ERROR_SEVERE("%s %d: %s: Unable to create task Queues\n",
                       __FILE__, __LINE__, routine_name);
    /* L7_assert(1); */
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    return L7_FAILURE;
  }

  if (ipStartTasks() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.type.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = IPMAP_CNFGR;

  osapiMessageSend(ipMapProcess_Queue, &msg, sizeof(ipMapMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM); 

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for IGMP Snooping component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
void ipMapInitUndo()
{
  if (ipMapProcessingTaskId != L7_NULL)
  {
      osapiTaskDelete(ipMapProcessingTaskId);
      ipMapProcessingTaskId = L7_NULL;
  }
               
  if (ipMapForwardingTaskId != L7_NULL)
  {
      osapiTaskDelete(ipMapForwardingTaskId);
      ipMapForwardingTaskId = L7_NULL;
  }

  if (ipMapProcess_Queue != L7_NULLPTR)
  {
      (void) osapiMsgQueueDelete(ipMapProcess_Queue);
      ipMapProcess_Queue = L7_NULLPTR;
  }

  if (ipMapForwardQueue[L7_L3_FWD_QUEUE] != L7_NULLPTR)
  {
      (void) osapiMsgQueueDelete(ipMapForwardQueue[L7_L3_FWD_QUEUE]);
      ipMapForwardQueue[L7_L3_FWD_QUEUE] = L7_NULLPTR;
  }

  if (ipMapForwardQueue[L7_L3_FWDPRI_QUEUE] != L7_NULLPTR)
  {
      (void) osapiMsgQueueDelete(ipMapForwardQueue[L7_L3_FWDPRI_QUEUE]);
      ipMapForwardQueue[L7_L3_FWDPRI_QUEUE] = L7_NULLPTR;
  }

  if (ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE] != L7_NULLPTR)
  {
      (void) osapiMsgQueueDelete(ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE]);
      ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE] = L7_NULLPTR;
  }

  if (ipMapForwardQueue[L7_L3_ARP_QUEUE] != L7_NULLPTR)
  {
      (void) osapiMsgQueueDelete(ipMapForwardQueue[L7_L3_ARP_QUEUE]);
      ipMapForwardQueue[L7_L3_ARP_QUEUE] = L7_NULLPTR;
  }


  ipMapCnfgrState = IPMAP_PHASE_INIT_0;
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
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipMapRC;
  static const char *routine_name = "ipMapCnfgrInitPhase1Process()";

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipMapRC   = L7_SUCCESS;

  /* Create read write lock to protect IP MAP configuration and status data */
  if (osapiRWLockCreate(&ipRwLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
           "Failed to create IP MAP read-write lock.");
        
    return L7_FAILURE;
  } 

  /* Allocate Memory */
  ipMapCfg = (L7_ipMapCfg_t *)osapiMalloc(L7_IP_MAP_COMPONENT_ID, 
                                          sizeof(L7_ipMapCfg_t));
  route = (L7_rtrStaticRouteCfg_t *)osapiMalloc(L7_IP_MAP_COMPONENT_ID, 
                                                sizeof(L7_rtrStaticRouteCfg_t));
  arp = (L7_rtrStaticArpCfg_t *)osapiMalloc(L7_IP_MAP_COMPONENT_ID, 
                                            sizeof(L7_rtrStaticArpCfg_t));
  pIpMapInfo              = (ipMapInfo_t *)osapiMalloc(L7_IP_MAP_COMPONENT_ID, sizeof(ipMapInfo_t));  
  ipMapLockInfo = (ipMapLockInfo_t*) osapiMalloc(L7_IP_MAP_COMPONENT_ID, 
                                                 sizeof(ipMapLockInfo_t));
  intIfToCfgIndex = osapiMalloc(L7_IP_MAP_COMPONENT_ID, 
                                sizeof(L7_uint32) * platIntfMaxCountGet());
  /* Allocate space for ip registration arrays to allow for callback to
     registered protocol for routing interface state changes from enable
     to disable & ip address changes */
  pIpIntfStateNotifyList  = 
  (pIpRoutingEventNotifyList_t)osapiMalloc(L7_IP_MAP_COMPONENT_ID, sizeof(ipRoutingEventNotifyList_t) *
                                           L7_LAST_ROUTER_PROTOCOL);

  ipMapIntfInfo = (ipMapIntfInfo_t *) osapiMalloc(L7_IP_MAP_COMPONENT_ID, sizeof(ipMapIntfInfo_t) * 
                                                  (L7_MAX_INTERFACE_COUNT + 1));  

  /* validate allocated memory */
  if ((ipMapCfg                 == L7_NULL)     ||
      (route                    == L7_NULL)     || 
      (arp                      == L7_NULL)     ||
      (pIpMapInfo               == L7_NULLPTR)  ||
      (ipMapLockInfo            == L7_NULLPTR)  ||
      (pIpIntfStateNotifyList   == L7_NULLPTR)  ||
      (ipMapIntfInfo            == L7_NULLPTR))
  {
    IPMAP_ERROR_SEVERE("%s %d: %s: Error allocating memory\n",
                       __FILE__, __LINE__, routine_name);
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  /* initialize memory */
  memset(ipMapCfg,                0,  sizeof(L7_ipMapCfg_t));
  memset(route,                   0,  sizeof(L7_rtrStaticRouteCfg_t));
  memset(arp,                     0,  sizeof(L7_rtrStaticArpCfg_t));
  memset(pIpMapInfo,              0,  sizeof(ipMapInfo_t));
  memset(ipMapLockInfo,           0,  sizeof(ipMapLockInfo_t));
  memset(pIpIntfStateNotifyList,  0,  sizeof(ipRoutingEventNotifyList_t)*L7_LAST_ROUTER_PROTOCOL);
  memset(( void * )ipMapIntfInfo, 0, 
           sizeof(ipMapIntfInfo_t) * (L7_MAX_INTERFACE_COUNT + 1));  


  /* Create semaphore that IP MAP holds while a forwarding table update is pending */
  pIpMapInfo->ckptPendingSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (!pIpMapInfo->ckptPendingSema)
  {
    IPMAP_ERROR_SEVERE("%s %d: %s: Error allocating semaphores\n",
                       __FILE__, __LINE__, routine_name);
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  /* Create buffer for best route notification */
  ipMapRouteChangeBuf = (rtoRouteChange_t*) osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                                                        L7_IPMAP_MAX_BEST_ROUTE_CHANGES * 
                                                        sizeof(rtoRouteChange_t));
  if (ipMapRouteChangeBuf == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
            "Unable to allocate IP MAP route change buffer.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC      = L7_ERROR;
    return ipMapRC;
  }
  memset((void*) ipMapRouteChangeBuf, 0, 
         L7_IPMAP_MAX_BEST_ROUTE_CHANGES * sizeof(rtoRouteChange_t));

  IpRxStatsReset();

  /* Initialize the event handler */
  memset( (void *)&(pIpMapInfo->eventHandler), 0, sizeof(asyncEventHandler_t) );
  pIpMapInfo->eventHandler.componentId          = L7_IP_MAP_COMPONENT_ID;
  pIpMapInfo->eventHandler.maxCorrelators       = L7_IPMAP_MAX_ASYNC_EVENTS;
  pIpMapInfo->eventHandler.registeredMask       = &(pIpMapInfo->registeredComponents);
  pIpMapInfo->eventHandler.timeout              = L7_IPMAP_MAX_ASYNC_EVENT_TIMEOUT;
  pIpMapInfo->eventHandler.complete_notify      = (void *)ipMapAsyncEventCompleteNotify;
  strncpy((L7_uchar8 *)&(pIpMapInfo->eventHandler.name), "IP MAP", ASYNC_MAX_HANDLER_NAME);
  if (asyncEventHandlerCreate(&(pIpMapInfo->eventHandler), 
                              &(pIpMapInfo->eventHandlerId) ) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;
    
    return ipMapRC;
  }
  
  if (ipMapBufferPoolCreate() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;
    
    return ipMapRC;
  }

  /* Initialize the IP forwarding code. */
  if (rtIpRecvCnfgrInitPhase1Process() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  if (ipMapArpInitPhase1Process() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  if (rtoCnfgrInitPhase1Process(platRtrRouteMaxEntriesGet(), L7_IP_NHRES_MAX) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  /* Initialize the router interfaces */
  /* There is no way to Fini this function therefore it will only be executed once
   * If it fails for any reason a LOG_ERROR will be issued
   */
  ipmRouterIfInitPhase1Process(L7_RTR_MAX_RTR_INTERFACES);

  if (ipMapIpForwardingStackInit() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }


#ifdef L7_NSF_PACKAGE
  if (ipstkCheckpointInit() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /* Create the DHCP Check point table */
  if (ipMapDhcpCkptTableCreate() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
#endif

  ipMapCnfgrState = IPMAP_PHASE_INIT_1;

  return ipMapRC;
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
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipMapRC;
  nvStoreFunctionList_t notifyFunctionList;
  sysnetNotifyEntry_t snEntry;
  static const char *routine_name = "ipMapCnfgrInitPhase2Process()";

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipMapRC    = L7_SUCCESS;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_IP_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = ipSave;
  notifyFunctionList.hasDataChanged = ipMapHasDataChanged;
  notifyFunctionList.resetDataChanged = ipMapResetDataChanged;

  if ( nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  LOG_INFO(LOG_CTX_STARTUP,"Going to register ipMapArpRecvIP related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY,  L7_ETYPE_ARP,  (L7_uint32) ipMapArpRecvIP);

  /* Register with SYSNET to receive ARP packets */
  memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t));
  strcpy(snEntry.funcName, "ipMapArpRecvIP");
  snEntry.notify_pdu_receive = ipMapArpRecvIP;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_ARP;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  LOG_INFO(LOG_CTX_STARTUP,"Going to register ipMapRecvIP related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY,  L7_ETYPE_IP,  (L7_uint32) ipMapRecvIP);

  /* Register with sysNET to receive IP packets */
  memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t));
  strcpy(snEntry.funcName, "ipMapRecvIP");
  snEntry.notify_pdu_receive = ipMapRecvIP;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_IP;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }


  /* register NIM callback to support interface changes */

  if (nimRegisterIntfChange(L7_IP_MAP_COMPONENT_ID, ipMapIntfChangeCallback,
                            ipMapIntfStartupCallback, NIM_STARTUP_PRIO_IPMAP) != L7_SUCCESS)
  {
    IPMAP_ERROR_SEVERE("%s %d: %s: Unable to register IP Map callback\n",
                       __FILE__, __LINE__, routine_name);
/*    L7_assert(1); */
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC     = L7_ERROR;

    return ipMapRC;
  }

  if (ipMapArpInitPhase2Process() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC    = L7_ERROR;

    return ipMapRC;
  }

  if (ipmRouterIfInitPhase2Process (L7_RTR_MAX_RTR_INTERFACES) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC    = L7_ERROR;

    return ipMapRC;
  }

#ifdef L7_NSF_PACKAGE
  if (ipMapCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
           "IP MAP failed to register for checkpoint service callbacks.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
#endif

  /* register for debug */
  ipMapDebugRoutingInfoDumpRegister();

  ipMapDebugRegister();

  ipMapCnfgrState = IPMAP_PHASE_INIT_2;

  return ipMapRC;
}

/* PTin added: callbacks */
#if 1
/**
 * Register callback again
 * 
 * @author mruas (2/2/2015)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ipMapCnfgrRegisterCallbacks(L7_int which)
{
  sysnetNotifyEntry_t snEntry;

  /* Register with SYSNET to receive ARP packets */
  if (which == 1)
  {
    LOG_INFO(LOG_CTX_STARTUP,"Going to register ipMapArpRecvIP related to type=%u, protocol_type=%u: 0x%08x",
             SYSNET_ETHERTYPE_ENTRY,  L7_ETYPE_ARP,  (L7_uint32) ipMapArpRecvIP);

    memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t)); 
    strcpy(snEntry.funcName, "ipMapArpRecvIP");
    snEntry.notify_pdu_receive = ipMapArpRecvIP;
    snEntry.type = SYSNET_ETHERTYPE_ENTRY;
    snEntry.u.protocol_type = L7_ETYPE_ARP;
    if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  /* Register with sysNET to receive IP packets */
  if (which == 2)
  {
    LOG_INFO(LOG_CTX_STARTUP,"Going to register ipMapRecvIP related to type=%u, protocol_type=%u: 0x%08x",
             SYSNET_ETHERTYPE_ENTRY,  L7_ETYPE_IP,  (L7_uint32) ipMapRecvIP);

    memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t));
    strcpy(snEntry.funcName, "ipMapRecvIP");
    snEntry.notify_pdu_receive = ipMapRecvIP;
    snEntry.type = SYSNET_ETHERTYPE_ENTRY;
    snEntry.u.protocol_type = L7_ETYPE_IP;
    if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}
#endif

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
* 
* @param    warmRestart @b{(input)}   L7_TRUE if restart is warm
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
L7_RC_t ipMapCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t ipMapRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipMapRC   = L7_SUCCESS;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&pIpMapInfo->nimEventMask, 0, sizeof(pIpMapInfo->nimEventMask));
  nimRegisterIntfEvents(L7_IP_MAP_COMPONENT_ID, pIpMapInfo->nimEventMask);

  pIpMapInfo->nsfRoutesPending = 0;
  pIpMapInfo->l3HwUpdateComplete = L7_FALSE;
  pIpMapInfo->createStartupDone = L7_FALSE;
  pIpMapInfo->backupElected = L7_FALSE;

  if (ipMapTraceFlags & IPMAP_TRACE_NSF) 
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "Restart is %s", warmRestart ? "WARM" : "COLD");
    ipMapTraceWrite(traceBuf);
  }
  pIpMapInfo->warmRestart = warmRestart;

  ipBuildDefaultConfigData(L7_IP_CFG_VER_CURRENT);
 
  ipStaticRtsBuildDefaultConfigData(L7_IP_STATIC_ROUTES_CFG_VER_CURRENT);
  ipStaticArpBuildDefaultConfigData(L7_IP_STATIC_ARP_CFG_VER_CURRENT);

  if (ipMapApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipMapRC    = L7_ERROR;

    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return ipMapRC;
  }

  /* If a warm restart, defer best route registration until we have cleaned up 
   * the set of checkpointed routes and the driver is ready to accept route 
   * change events. */
  if (!warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    /* Clear any checkpointed data */
    ipMapCheckpointDataClear(); 
#endif

    /* register for best route changes */
    if ((rtoBestRouteClientRegister("IP MAP", ipMapRouteCallback)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
              "Unable to register best route callback with RTO");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      return L7_ERROR;
    }
  }

#ifdef L7_QOS_FLEX_PACKAGE_ACL 
  ipMapDebugCfgRead();
  ipMapApplyDebugConfigData();
#endif

  ipMapCfg->cfgHdr.dataChanged = L7_FALSE;

  ipMapCnfgrState = IPMAP_PHASE_INIT_3;

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  return ipMapRC;
}


/*********************************************************************
* @purpose  This function undoes ipMapCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCnfgrFiniPhase1Process()
{

  ipmRouterIfFiniPhase1Process(L7_RTR_MAX_RTR_INTERFACES);

  rtoCnfgrFiniPhase1Process(platRtrRouteMaxEntriesGet());

  ipMapArpFiniPhase1Process();
  
  rtIpRecvCnfgrFiniPhase1Process();

  asyncEventHandlerDelete(pIpMapInfo->eventHandlerId);

  ipMapBufferPoolDelete();

#ifdef L7_NSF_PACKAGE
  /* Delete the DHCP Check point table */
  ipMapDhcpCkptTableDelete();
#endif

  if (ipMapCfg != L7_NULLPTR)
    OSAPI_FREE_MEMORY(ipMapCfg);

  if (route != L7_NULLPTR)
    OSAPI_FREE_MEMORY(route);

  if (arp != L7_NULLPTR)
    OSAPI_FREE_MEMORY(arp);

  if (pIpMapInfo != L7_NULLPTR)
    OSAPI_FREE_MEMORY(pIpMapInfo);
  
  if (ipMapLockInfo)
    OSAPI_FREE_MEMORY(ipMapLockInfo);
  
  if (intIfToCfgIndex != L7_NULLPTR)
    OSAPI_FREE_MEMORY(intIfToCfgIndex);

  if (pIpIntfStateNotifyList != L7_NULLPTR )
    OSAPI_FREE_MEMORY(pIpIntfStateNotifyList);

  if (ipMapRouteChangeBuf)
  {
    osapiFree(L7_IP_MAP_COMPONENT_ID, ipMapRouteChangeBuf);
    ipMapRouteChangeBuf = NULL;
  }

  ipMapCnfgrState = IPMAP_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes ipMapCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCnfgrFiniPhase2Process()
{

  sysnetNotifyEntry_t snEntry;
  
  (void)nvStoreDeregister(L7_IP_MAP_COMPONENT_ID);

  /* Deregister with sysnet to receive ARP and IP packets */
  memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t));
  strcpy(snEntry.funcName, "ipMapArpRecvIP");
  snEntry.notify_pdu_receive = ipMapArpRecvIP;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_ARP;
  (void)sysNetDeregisterPduReceive(&snEntry);


  memset((void *)&snEntry, 0, sizeof(sysnetNotifyEntry_t));
  strcpy(snEntry.funcName, "ipMapRecvIP");
  snEntry.notify_pdu_receive = ipMapRecvIP;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_IP;
  (void)sysNetDeregisterPduReceive(&snEntry);

  (void)nimDeRegisterIntfChange(L7_IP_MAP_COMPONENT_ID);
           
  ipmRouterIfFiniPhase2Process();
  ipMapArpFiniPhase2Process();

  
  ipMapCnfgrState = IPMAP_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes ipMapCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place ipMapCnfgrState to WMU */
  ipMapCnfgrUconfigPhase2(&response, &reason);
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t ipMapCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipMapRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(ipMapRC);
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
* @notes    
*           
*       
* @end
*********************************************************************/
L7_RC_t ipMapCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  rtoBestRouteClientDeregister("IP MAP", ipMapRouteCallback);

  ipMapRestoreProcess();

  ipMapCnfgrState = IPMAP_PHASE_WMU;

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a terminate event  
*
* @param    pResponse - @b{(output)}  Response always command complete.
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    User has initiated a failover to the backup unit. If IP MAP is waiting to
*           checkpoint data to the backup unit, wait until that completes. Wait up to 
*           20 seconds and then blunder on. This semaphore will
*           never be given. Assumption is that if we get here, the management unit
*           will definitely be rebooted.
*       
* @end
*********************************************************************/
L7_RC_t ipMapCnfgrTerminate(L7_CNFGR_RESPONSE_t *pResponse,
                            L7_CNFGR_ERR_RC_t   *pReason)
{
  osapiSemaTake(pIpMapInfo->ckptPendingSema, 20000);    

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to l7_proccess task
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL               warmRestart = L7_FALSE;

  L7_RC_t             ipMapRC = L7_ERROR;
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
                if ((ipMapRC = ipMapCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  ipMapCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((ipMapRC = ipMapCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  ipMapCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                /* Determine if this is a cold or warm restart */
                warmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
                if ((ipMapRC = ipMapCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  ipMapCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                ipMapRC = ipMapCnfgrNoopProccess( &response, &reason );
                ipMapCnfgrState = IPMAP_PHASE_WMU;
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
                ipMapCnfgrState = IPMAP_PHASE_EXECUTE;

                ipMapRC   = L7_SUCCESS;
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
                ipMapRC = ipMapCnfgrNoopProccess( &response, &reason );
                ipMapCnfgrState = IPMAP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                ipMapRC = ipMapCnfgrUconfigPhase2( &response, &reason );
                ipMapCnfgrState = IPMAP_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
            ipMapRC = ipMapCnfgrTerminate(&response, &reason);
            break;
          case L7_CNFGR_CMD_SUSPEND:
            ipMapRC = ipMapCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = ipMapRC;
  if (ipMapRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

