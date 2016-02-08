/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmMain.c
*
* @purpose   Main PIMSM Task and Initialization Functionality.
*
* @component 
*
* @comments  none
*
* @create   01/01/2006
*
* @author   dsatyanarayana
*
* @end
*
**********************************************************************/


#include "pimsmdefs.h"
#include "pimsmmain.h"
#include "osapi_support.h"
#include "rto_api.h"
#include "rtmbuf.h"
#include "rtiprecv.h"
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "l7_cnfgr_api.h"
#include "l7apptimer_api.h"
#include "l7_mcast_api.h"
#include "l7_rto6_api.h"
#include "buff_api.h"
#include "heap_api.h"
#include "mcast_map.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmtimer.h"
#include "pimsmbsr.h"
#include "pimsmcontrol.h"
#include "pimsmintf.h"
#include "pimsmwrap.h"
#include "pimsmv6util.h"
#include "pimsmtimer.h"
#include "pimsm_map_cfg.h"
#include "pimsm_map_util.h"
#include "pimsm_map.h"
#include "pimsm_vend_ctrl.h"


/*********************************************************************
* @purpose  cleanup control block
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmCtrlBlockCleanUp(pimsmCB_t *pimsmCb)
{

    pimsmTimersDeInit(pimsmCb);

    pimsmUtilAppTimerCancel (pimsmCb, &(pimsmCb->pimsmSPTTimer));
    pimsmUtilAppTimerCancel (pimsmCb, &(pimsmCb->pimsmJPBundleTimer));

    if(pimsmCb->pimsmRouteChangeBuf != L7_NULLPTR)
    {
      PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmRouteChangeBuf);
    }
    pimsmIntfDeInit(pimsmCb);
    pimsmBsrDeInit(pimsmCb);
    pimsmRpDeInit(pimsmCb);
    pimsmStarStarRpDeInit(pimsmCb);
    pimsmStarGDeInit(pimsmCb);
    pimsmSGRptDeInit(pimsmCb); 
    pimsmSGDeInit(pimsmCb);
    if(pimsmCb->sockFd >= 0)
    {
      if(pimsmCb->family == L7_AF_INET6)
      {
        if(mcastMapPktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_PIMSM) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "De-Register with Pkt Rcvr Failed ");
        }
      }
      osapiSocketClose(pimsmCb->sockFd);
    }   
    inetAddressZeroSet(pimsmCb ->family, &(pimsmCb->allPimRoutersGrpAddr));

    /* De-Register with MFC  */
    if (mfcCacheEventDeregister (L7_MRP_PIMSM) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                  "MFC De-Registration Failed");
    }

    if (mgmdMapDeRegisterMembershipChangeEvent(pimsmCb->family,
                                                L7_MRP_PIMSM ) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                  "MGMD De-Registration Failed");
    }

    /* De-Register for Static MRoutes */
    if (mcastMapStaticMRouteEventDeRegister (pimsmCb->family, L7_MRP_PIMSM)
                                          != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "Static MRoutes De-Registration Failed");
    }

    if(pimsmCb->pimsmNbrLock != L7_NULLPTR)
    {    
       osapiSemaDelete(pimsmCb->pimsmNbrLock);
       pimsmCb->pimsmNbrLock = L7_NULLPTR;
    }

    /* Delete the PIMSM RTO Timer Semaphore */
    if (pimsmCb->rtoTimerSemId != L7_NULLPTR)
    {
      osapiSemaDelete(pimsmCb->rtoTimerSemId);
      pimsmCb->rtoTimerSemId = L7_NULLPTR;
    }

    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb);
}

/*********************************************************************
*
* @purpose  To Initialize the Memory for PIM-SM Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimsmMemoryInit (L7_uchar8 addrFamily)
{
  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Entry");

  if (pimsmUtilMemoryInit (addrFamily) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                 "PIM-SM Memory Initialization Failed for addrFamily - %d",
                 addrFamily);
    return L7_FAILURE;
  }

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,
               "PIM-SM Memory Initialization Successful for addrFamily - %d",
               addrFamily);

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for PIM-SM Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimsmMemoryDeInit (L7_uchar8 addrFamily)
{
  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Entry");

  /* Validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"Bad Address Family");
    return L7_FAILURE;
  }

  /* De-Initialize the Heap memory */
  if (heapDeInit (mcastMapHeapIdGet (addrFamily), L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                 "Heap De-Init Failed for Family - %d", addrFamily);
    return L7_FAILURE;
  }

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,
               "PIM-SM Memory De-Initialization Successful");

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  PIMSM Protocol initialization.
*
* @param    family           @b{(input)}  address family
* @param    pimsmEbHandle    @b{(input)} exec-block handle
*
* @returns  CB handle on success
* @returns  L7_NULLPTR on failure
*
* @comments     
*     
* @end
******************************************************************************/
MCAST_CB_HNDL_t pimsmCtrlBlockInit(L7_uchar8 family)
{
  L7_uint32    sockfd =0;
  pimsmCB_t *pimsmCb = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"Entry");

  /* Initialize the Control Block Members for PIM-SM */
  pimsmCb = PIMSM_ALLOC (family, sizeof(pimsmCB_t));

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for pimsmCb\n");
    return L7_NULLPTR;
  }

  memset(pimsmCb, 0, sizeof(pimsmCB_t));

  pimsmCb->family = family;

  /* Create the event scheduler for IPC and also to get packets 
   * from socket(V6 onlu).
   */
  if(pimsmTimersInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: Timer init failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"family is %d",family);
  /* open socket */

  if((osapiSocketCreate((L7_uint32 )family,L7_SOCK_RAW,
                        PIM_IP_PROTO_NUM, &sockfd)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "PIMSM: Socket Create failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"Sock Fd is %d",sockfd);
  if(sockfd >= 0)
  {
    pimsmCb->sockFd = sockfd;
    pimsmSocketOptionsSet(family,sockfd);
    if(family == L7_AF_INET6)
    {
      /* Register the SockFD with Event Scheduler for IPv6 Packet Reception */
      if(mcastMapPktRcvrSocketFdRegister (sockfd, MCAST_EVENT_IPv6_CONTROL_PKT_RECV, 
                                          mcastCtrlPktBufferPoolIdGet(L7_AF_INET6),
                                          L7_IPV6_PKT_RCVR_ID_PIMSM) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "Socket FD Registration Failure");
        pimsmCtrlBlockCleanUp(pimsmCb);
        return L7_NULLPTR;
      }
    }
  }
  inetAllPimRouterAddressInit(family,&(pimsmCb->allPimRoutersGrpAddr));

  /*initialize the trees */ 
  if(pimsmSGInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: (S,G) AVL Tree initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  if(pimsmSGRptInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: (S,G,rpt) AVL Tree initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }

  if(pimsmStarGInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: (*,G) AVL Tree initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  if(pimsmStarStarRpInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: (*,*,RP) SLL initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  /* initialize BSR and RP */
  if(pimsmRpInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: RP initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  if(pimsmBsrInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: BSR initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }

  /* initialize per interface stuff */
  if(pimsmIntfInit(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: Interface initialization failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  pimsmCb->pimsmSPTTimerHandle = 
  handleListNodeStore(pimsmCb->handleList,
                      (void*)&pimsmCb->pimsmSPTTimerParam); 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "pimsmCb->pimsmSPTTimerHandle= %d",
              pimsmCb->pimsmSPTTimerHandle);   
  /*Initialize the timer */
  pimsmCb->pimsmRateCheckInterval = PIMSM_DEFAULT_CHECK_INTERVAL;
  pimsmCb->pimsmDataRateBytes = (L7_PIMSM_DATA_THRESHOLD_RATE * 1000 *
                                  PIMSM_DEFAULT_CHECK_INTERVAL)/10;
  pimsmCb->pimsmRegRateBytes = (L7_PIMSM_REG_THRESHOLD_RATE * 1000 *
                                 PIMSM_DEFAULT_CHECK_INTERVAL)/10;

  pimsmCb->pimsmJPBundleTimerHandle= 
  handleListNodeStore(pimsmCb->handleList,
                      (void*)&pimsmCb->pimsmJPBundleTimerParam); 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "pimsmCb->pimsmJPBundleTimerHandle= %d",
              pimsmCb->pimsmJPBundleTimerHandle);   
  if(pimsmRouteChangeBufMemAlloc(pimsmCb) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: RTO change buffer allocation failed\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;
  }
  pimsmCb->pimsmNbrLock = (void *)
                            osapiSemaBCreate (OSAPI_SEM_Q_FIFO, 
                            OSAPI_SEM_FULL);
  if(pimsmCb->pimsmNbrLock == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: Failed to create Semaphore for NbrLists\n");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;  
  }

  /* Create the Semaphore for RTO Timer */
  pimsmCb->rtoTimerSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, 
                                             OSAPI_SEM_FULL);
  if(pimsmCb->rtoTimerSemId == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: Failed to create Semaphore for RTO Timer");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;  
  }

  /* Register with MFC */
  if (mfcCacheEventRegister (L7_MRP_PIMSM, pimsmMapCommonCallback, L7_TRUE,
                             PIMSM_MFC_WRONG_IF_RATE_LIMIT_THRESHOLD)
                          != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"MFC Registration Failed");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;  
  }

  if (mgmdMapRegisterMembershipChangeEvent(pimsmCb->family, 
                                 L7_MRP_PIMSM,
                                 pimsmMapCommonCallback) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM Registration Failed");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;  
  }

  /* Register for Static MRoutes */
  if (mcastMapStaticMRouteEventRegister (pimsmCb->family,
                                         L7_MRP_PIMSM,
                                         pimsmMapCommonCallback)
                                      != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"Static MRoutes Registration Failed");
    pimsmCtrlBlockCleanUp(pimsmCb);
    return L7_NULLPTR;  
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"CB init successful");
  return((void *)pimsmCb);
}

/******************************************************************************
* @purpose  PIMSM control block de-initialization.
*
* @param    pimsmCbHandle     @b{(input)}  control block  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmCtrlBlockDeInit(MCAST_CB_HNDL_t pimsmCbHandle)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_uint32 addrFamily;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Entry");

  if(L7_NULLPTR == pimsmCbHandle)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameter");
    return L7_FAILURE;
  }

  pimsmCb = (pimsmCB_t *)pimsmCbHandle;
  addrFamily = pimsmCb->family;

  pimsmCtrlBlockCleanUp(pimsmCb);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"De Init successful");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  This function process PIMDM Events.
*
* @param    pimsmCbHandle  @b{ (input) } pimsm CB Handle.
* @param    eventType      @b{ (input) } Event.
* @param    pMsg           @b{ (input) } Message.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function recieves all the messages and events and
*            call the appropriate handler.
*
* @end
*********************************************************************/
L7_RC_t pimsmEventProcess(L7_uchar8 familyType, 
                          mcastEventTypes_t eventType,
                          void *pMsg)
{
  L7_RC_t               rc = L7_FAILURE;
  L7_inet_addr_t        *pSrcAddr = L7_NULLPTR;
  L7_uint32             rtrIfNum = L7_NULL;
  struct rtmbuf         *mbuf = L7_NULLPTR;
  L7_inet_addr_t        *pGrpAddr = L7_NULLPTR;
  pimsmCB_t             *pimsmCb = L7_NULLPTR;
  L7_BOOL               moreChanges = L7_FALSE;
  mcastControlPkt_t     *pMcastPkt = L7_NULLPTR;
  L7_uchar8             *pData = L7_NULLPTR;
  mfcEntry_t            *pMfcEntry = L7_NULLPTR;
  mcastAdminMsgInfo_t    adminInfo;
  pimsmMapCB_t           *pimsmMapCb =L7_NULLPTR;
  mgmdMrpEventInfo_t     *mgmdMrpGrpInfo = L7_NULLPTR;
  mcastRPFInfo_t         rpfInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_MAX,"Entry");

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(familyType);

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"PIMSM: Invalid Ctrl Block");
    return L7_FAILURE;
  }

  switch(eventType)
  {
    case MCAST_EVENT_PIMSM_TIMER_EXPIRY:
      /* this (appTimer) event should be handle irrespective of pimsm being enabled */
      appTimerProcess(pimsmCb->timerCb);
      rc = L7_SUCCESS;
      break;

    case MCAST_MFC_NOCACHE_EVENT:
      pMfcEntry = (mfcEntry_t*) pMsg;
      pSrcAddr= &(pMfcEntry->source);
      pGrpAddr= &(pMfcEntry->group);
      rtrIfNum = pMfcEntry->iif;
      rc = pimsmMfcNoCacheProcess(pimsmCb, pSrcAddr, pGrpAddr, rtrIfNum);
      break;

    case MCAST_MFC_WRONGIF_EVENT:
      pMfcEntry = (mfcEntry_t*) pMsg;
      pSrcAddr= &(pMfcEntry->source);
      pGrpAddr= &(pMfcEntry->group);
      rtrIfNum = pMfcEntry->iif;
      rc = pimsmMfcWrongIfProcess(pimsmCb, pSrcAddr, pGrpAddr, rtrIfNum);
      break;

    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
      pMfcEntry = (mfcEntry_t*) pMsg;
      pSrcAddr= &(pMfcEntry->source);
      pGrpAddr= &(pMfcEntry->group);
      rc = pimsmMfcEntryExpireProcess(pimsmCb, pSrcAddr, pGrpAddr);
      break;

    case MCAST_MFC_WHOLEPKT_EVENT:
      if(pimsmMapCtrlBlockGet(pimsmCb->family,&pimsmMapCb) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"Failed to get pimsm Map CB for address family %d",pimsmCb->family);
        return L7_FAILURE;
      }
      pMfcEntry = (mfcEntry_t*) pMsg;
      mbuf = (struct rtmbuf *)pMfcEntry->m;
      pData = rtmtod(mbuf, L7_uchar8 *);
      rc = pimsmMfcWholePktProcess(pimsmCb, pData);
      if ((mbuf->rtm_bufhandle) != L7_NULLPTR)
      {
        SYSAPI_NET_MBUF_FREE((L7_netBufHandle)mbuf->rtm_bufhandle);
      }
      rtm_freem(mbuf);
      break;

    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      mgmdMrpGrpInfo = (mgmdMrpEventInfo_t *) pMsg;
      rc = pimsmIgmpMemberUpdateProcess(pimsmCb, mgmdMrpGrpInfo);

      /* Free the MCAST MGMD Events Source List Buffers */
      if (rc == L7_SUCCESS)
      {
        mcastMgmdEventsBufferPoolFree (pimsmCb->family, mgmdMrpGrpInfo);
      }
      break;

    case MCAST_EVENT_RTO_BEST_ROUTE_CHANGE:
      /* this (RTO) event should be handle irrespective of pimsm being enabled */
      rc = pimsmRtoBestRoutesGet(pimsmCb, &moreChanges);
      if(rc == L7_SUCCESS)
      {
        if(moreChanges == L7_TRUE)
        {
          /* post another event to the PIMSM message queue to request more 
           * changes from RTO */
          if (pimsmMapMessageQueueSend(pimsmCb->family, 
                                       MCAST_EVENT_RTO_BEST_ROUTE_CHANGE,
                                       0,
                                       L7_NULLPTR) != L7_SUCCESS)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                         "PIMSM: RTO Event Post Failed.\n");
          }
        }
        else 
        {
        /* RP's RPF Interface has changed .
          * Post an event to MGMD to get the Group membership details.
          */
         for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
         {
           pimsmInterfaceEntry_t *intfEntry = L7_NULLPTR;
       
           if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry) != L7_SUCCESS)
           {
             continue;
           }
           if (mgmdMapMRPGroupInfoGet (pimsmCb->family, rtrIfNum) != L7_SUCCESS)
           {
             PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                          "Failure in Posting the MGMD Info Get Event for rtrIfNum - %d",
                          rtrIfNum);
             continue;
           }
         }
        }
      }
      break;

    case MCAST_EVENT_PIMSM_CONTROL_PKT_RECV:
    case MCAST_EVENT_IPv6_CONTROL_PKT_RECV:
      pMcastPkt = (mcastControlPkt_t *) pMsg;
      rc = pimsmMcastPktRecvProcess(pimsmCb, pMcastPkt);
      break;

    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:
      memcpy(&adminInfo, pMsg, sizeof(mcastAdminMsgInfo_t));
      pimsmAdminScopeEventProcess(pimsmCb, &adminInfo);
      break;

    case MCAST_EVENT_STATIC_MROUTE_CHANGE:
      memcpy(&rpfInfo, pMsg, sizeof(mcastRPFInfo_t));
      pimsmRtoBestRouteChangeProcess(pimsmCb, &rpfInfo);

      /* Post an event to MGMD to get the Group membership details. */
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        pimsmInterfaceEntry_t *intfEntry = L7_NULLPTR;
        if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry) != L7_SUCCESS)
        {
          continue;
        }
        if (mgmdMapMRPGroupInfoGet (pimsmCb->family, rtrIfNum) != L7_SUCCESS)
        {
          PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                       "Event MGMDInfoGetEvent Post Failed for rtrIfNum - %d", rtrIfNum);
          continue;
        }
      }
      break;

   default:
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"invalid eventType: %d", 
                  eventType);
      return L7_FAILURE;
      /* passthru */
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_MAX,"Exit Successful");
  return rc;
}
