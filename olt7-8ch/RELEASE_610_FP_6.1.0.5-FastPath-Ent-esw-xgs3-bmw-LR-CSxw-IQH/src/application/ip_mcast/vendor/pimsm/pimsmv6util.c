/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsm_v6_util.c
*
* @purpose   PIM-SM IPv6 API's.
*
* @component 
*
* @comments  none
*
* @create   11/04/2006
*
* @author   nramu
*
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmain.h"
#include "osapi_support.h"
#include "l7_rto6_api.h"
#include "rtmbuf.h"
#include "l3_mcast_commdefs.h"
#include "l7_ip6_api.h"
#include "l7apptimer_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
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
#include "pimsm_vend_ctrl.h"

#define PIMSM_IPV6_VER    6

/* PIM-SM V6 Heap Pool Items */
heapBuffPool_t pimsmV6HeapPoolList[] ={

  /* PIMDM Protocol Control Block Buffer */
  {sizeof (pimsmCB_t), 1,"PIMSM_CB", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Message Buffers */
  {PIMSM_MAX_JP_MSG_SIZE,PIMSM_MAX_JP_PACKETS,"PIMSM_JPG_MSG_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Address List Buffers for IPv6 */
  {PIM_IPV6_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST * sizeof(pim_encod_src_addr_t), PIMSM_MAX_PROTOCOL_PACKETS,"PIMSM_JP_ADDR_LIST_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Cache Entry Buffers */
  {sizeof(pimsmCache_t), PIMSM_MAX_IPV6_CACHE_ENTRIES,"PIMSM_CACHE_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Tree Heap Buffers for (S,G), (*,G) and (S,G,Rpt) Databases */
  {PIMSM_S_G_IPV6_TBL_SIZE_TOTAL * sizeof (avlTreeTables_t), 3,"PIMSM_SG_TREE_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (S,G) Database */
  {PIMSM_S_G_IPV6_TBL_SIZE_TOTAL * sizeof (pimsmSGNode_t), 1,"PIMSM_SG_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (S,G) and (*,G) Databases */
  {sizeof (pimsmSGIEntry_t), ((PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_S_G_IPV6_TBL_SIZE_TOTAL) + (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL)),"PIMSM_SGI_*GI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (S,G,Rpt) Database */
  {(PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL * sizeof (pimsmSGRptNode_t)), 1,"PIMSM_SG_RPT_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (S,G,Rpt) Database */
  {sizeof (pimsmSGRptIEntry_t), (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL),"PIMSM_SGRptI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (*,G) Database */
  {PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL * sizeof (pimsmStarGNode_t), 1,"PIMSM_*G_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* (*,*,RP) Buffers */
  {sizeof (pimsmStarStarRpNode_t), PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL,"PIMSM_**RP_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (*,*,RP) Database */
  {sizeof (pimsmStarStarRpIEntry_t),PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL,"PIMSM_**RPI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Block Buffer */
  {sizeof (pimsmRpBlock_t), 1,"PIMSM_RP_BLOCK", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RADIX Tree Heap Buffers for RP Set Database */
  {(RADIX_TREE_HEAP_SIZE(PIMSM_RP_GRP_ENTRIES_MAX,sizeof(rpSetTreeKey_t))), 1,"PIMSM_RP_SET_TREE_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RADIX Data Heap Buffers for RP Set Database */
  {PIMSM_RP_GRP_ENTRIES_MAX * sizeof (pimsmRpSetNode_t), 1,"PIMSM_RP_SET_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Group List Buffers */
  {sizeof (pimsmRpGrpNode_t), PIMSM_RP_GRP_ENTRIES_MAX,"PIMSM_RP_GRP_LIST", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Group Address List Buffers */
  {sizeof (pimsmAddrList_t), PIMSM_RP_GRP_ADDR_LIST_NODES_MAX,"PIMSM_MRT_RP_GRP_ADDR_LIST", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Block Buffer */
  {sizeof (pimsmBSRBlock_t), 1,"PIMSM_BSR_BLOCK", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Per Scope Zone Buffer */
  {sizeof (pimsmBsrPerScopeZone_t), PIMSM_BSR_SCOPE_NODES_MAX,"PIMSM_BSR_PER_SCOPE_ZONE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Fragment Group-RP Node Buffers */
  {sizeof (pimsmFragGrpRpNode_t), PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX,"PIMSM_FRAG_GRP_RP_NODE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Fragment RP Node Buffers */
  {sizeof (pimsmFragRpNode_t), PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX,"PIMSM_FRAG_RP_NODE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Neighbor Database Buffers */
  {sizeof (pimsmNeighborEntry_t), PIMSM_MAX_NBR,"PIMSM_NBR_DATABASE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RTO-IPv6 Route Change Buffers */
  {(PIMSM_BEST_ROUTE_CHANGES_MAX * sizeof(rto6RouteChange_t)), 1,"PIMSM_RTO6_RtBuf", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  {0,0,"LAST POOL INDICATOR",0}
};

/*********************************************************************
* @purpose  Function to get the best route changes
*           
*
* @param    pimsmCb        @b{(input)}  control block
* @param    routeEntry     @b{(input)}  Route information about the 
                              network that was added/ deleted/ modified
* @param    routeStatus    @b{(input)}  Information regarding the reason
                              the callback was issued.  Route_status 
                              events are enumerated in RTO_ROUTE_EVENT_t.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    
*
* @end
*********************************************************************/
static L7_RC_t pimsmV6BestRouteChanges(pimsmCB_t *pimsmCb,
                                       L7_route6Entry_t *routeEntry, 
                                       L7_uint32 routeStatus)
{
  L7_uint32 intIfNum =0;
  mcastRPFInfo_t routeInfo;

  if(L7_NULLPTR == routeEntry)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return L7_FAILURE;
  }
  if (routeStatus == RTO_ADD_ROUTE  ||
      routeStatus == RTO_DELETE_ROUTE)
  {  
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, 
         PIMSM_TRACE_INFO, "Recvd Reject Route - Ignore");
      return L7_SUCCESS;
    }
  } 
  else if (routeStatus == RTO_CHANGE_ROUTE )
  {
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, 
         PIMSM_TRACE_INFO, "Recvd Reject Route - Delete");
      routeStatus = RTO_DELETE_ROUTE;
    }
  }
  memset(&routeInfo,0,sizeof(mcastRPFInfo_t));
  inetAddressZeroSet(pimsmCb->family, &routeInfo.rpfNextHop);
  inetAddressZeroSet(pimsmCb->family, &routeInfo.rpfRouteAddress);
  if (routeStatus == RTO_ADD_ROUTE  ||
      routeStatus == RTO_CHANGE_ROUTE)
  {  
    /* select only one path, let it be the first one index by zero*/
    inetAddressSet(pimsmCb->family, 
                   &(routeEntry->ecmpRoutes.equalCostPath[0].ip6Addr),
                   &routeInfo.rpfNextHop);
  
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].intIfNum;
    if(ip6MapIntIfNumToRtrIntf(intIfNum, &routeInfo.rpfIfIndex) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "ip6MapIntIfNumToRtrIntf() failed");
      return L7_FAILURE;
    }
  
    routeInfo.rpfRouteMetricPref =routeEntry->pref;
    routeInfo.rpfRouteMetric = routeEntry->metric;
    routeInfo.rpfRouteProtocol =routeEntry->protocol;
  } 
  else if (routeStatus == RTO_DELETE_ROUTE)
  {
      routeInfo.rpfIfIndex = MCAST_MAX_INTERFACES;
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
      "Wrong routeStatus = %d", routeStatus);
    return L7_FAILURE;
  }
  routeInfo.status = routeStatus;
  routeInfo.prefixLength = routeEntry->ip6PrefixLen; 
  inetAddressSet(pimsmCb->family, &routeEntry->ip6Addr, 
                 &(routeInfo.rpfRouteAddress));

  if(pimsmRtoBestRouteChangeProcess(pimsmCb,&routeInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "pimsmRtoBestRouteChangeProcess() failed");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Function to get  best route 
*
* @param    pimsmCb        @b{(input)}  control block
* @param    pMoreChanges   @b{(output)} any more changes avaiable
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmV6BestRoutesGet(pimsmCB_t *pimsmCb,
                             L7_BOOL *pMoreChanges)
{
  L7_uint32 ii=0,numChanges =0;
  rto6RouteChange_t *routeChange = L7_NULLPTR,*routeChangeBuf = L7_NULLPTR;


  if(L7_NULLPTR == pMoreChanges)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return L7_FAILURE;
  }
  routeChangeBuf = (rto6RouteChange_t *)pimsmCb->pimsmRouteChangeBuf;
  if(rto6RouteChangeRequest(pimsmMapRto6BestRouteClientCallback,
                            PIMSM_BEST_ROUTE_CHANGES_MAX,
                            &numChanges,pMoreChanges,
                            routeChangeBuf) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "rto6RouteChangeRequest() failed");
    return L7_FAILURE;
  }

  for(ii =0;ii < numChanges;ii++)
  {
    routeChange = &routeChangeBuf[ii];
    pimsmV6BestRouteChanges(pimsmCb,
                            &routeChange->routeEntry,
                            routeChange->changeType);
  }
  return L7_SUCCESS;  
}

/******************************************************************************
* @purpose  Wrapper to get Source, Group address &  pkt length
            given the packet buffer 
*
* @param        family    @b{(input)}
* @param        pData     @b{(input)}   ip pkt start
* @param        pSrcAddr  @b{(output)}  ip source address
* @param        pGrpAddr  @b{(output)}  ip destination address
                pPktLen   @b{(output)}  payload length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmV6SourceAndGroupAddressGet(L7_uchar8 *pData,L7_inet_addr_t *pSrcAddr,
                          L7_inet_addr_t *pGrpAddr,L7_uint32 *pPktLen)
{
  L7_ip6Header_t *ip6Hdr=L7_NULLPTR;
  L7_uint32 version =0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "Enter");

  if((L7_NULLPTR == pData ) || (L7_NULLPTR == pSrcAddr) || 
     (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"invlaid input parameters");
    return L7_FAILURE;
  }
  ip6Hdr = (L7_ip6Header_t *)pData;
  version = L7_IP6_GET_VER(ip6Hdr);
  if(version != PIMSM_IPV6_VER)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Version is not IPv6");
    return(L7_FAILURE);
  }

  inetAddressSet(L7_AF_INET6,(void *)ip6Hdr->src,pSrcAddr);
  inetAddressSet(L7_AF_INET6,(void *)ip6Hdr->dst,pGrpAddr);
  *pPktLen = (L7_uint32 )ip6Hdr->paylen + L7_IP6_HEADER_LEN;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS; 
}
/******************************************************************************
* @purpose  This function is called to lookup for  the best route for 
            the given address.
*
* @param     pIpAddr       @b{(input)}    ip address
* @param     pRtoRoute     @b{(output)}   RTO Best route entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmV6BestRouteLookup(L7_in6_addr_t *pIpAddr, 
                               mcastRPFInfo_t *pRtoRoute)
{
  L7_route6Entry_t bestV6Route;
  L7_inet_addr_t ipAddr;
  L7_uint32 rtrIfNum =0,routeIntIntf;
  L7_RC_t rc;

  if((L7_NULLPTR == pIpAddr) || (L7_NULLPTR == pRtoRoute))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"invlaid input parameters");
    return L7_FAILURE;
  }

  inetAddressZeroSet(L7_AF_INET6, &ipAddr);
  memset(&bestV6Route,0,sizeof(L7_route6Entry_t));
  if(rto6BestRouteLookup(pIpAddr,&bestV6Route, L7_FALSE) == L7_SUCCESS)
  {
    routeIntIntf = bestV6Route.ecmpRoutes.equalCostPath[0].intIfNum;
    pRtoRoute->rpfRouteMetric = bestV6Route.metric;
    pRtoRoute->rpfRouteMetricPref = bestV6Route.pref;
    rc = mcastIpMapRtrIntfIpAddressGet(L7_AF_INET6, routeIntIntf, &ipAddr);
    if(rc !=L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                  "mcastIpMapRtrIntfIpAddressGet() failed! IfNum = %d",routeIntIntf);
      return L7_FAILURE;
    }
    rc = mcastIpMapIntIfNumToRtrIntf(L7_AF_INET6, routeIntIntf, &rtrIfNum);
    if(rc !=L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                  "mcastIpMapIntIfNumToRtrIntf() failed! IfNum = %d",routeIntIntf);
      return L7_FAILURE;
    }

    if(rtrIfNum == 0)
      inetAddressZeroSet(L7_AF_INET6, &pRtoRoute->rpfNextHop);

    inetAddressSet(L7_AF_INET6,
                   (void *)&bestV6Route.ecmpRoutes.equalCostPath[0].ip6Addr,
                   &pRtoRoute->rpfNextHop);

    inetCopy(&pRtoRoute->rpfRouteAddress,&ipAddr);
    pRtoRoute->rpfIfIndex = rtrIfNum;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Wrapper to build IP Hdr 
*
* @param        pData      @b{(input)}  ip pkt start
* @param        pSrcAddr   @b{(input)}  ip source address
* @param        pGrpAddr   @b{(input)}  ip destination address
* @param        pPktLen    @b{(output)} ip pkt length
*  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmV6IpHdrFrame(L7_uchar8 *pData,L7_uint32 *pPktLen,
                          L7_inet_addr_t *pSrcAddr,L7_inet_addr_t *pGrpAddr)
{
  L7_ip6Header_t ipv6Header, *ip6Hdr = &ipv6Header;
  L7_uchar8 pimsmHdr[PIM_HEARDER_SIZE];
  L7_uchar8 pseudoIpv6Hdr[L7_IP6_HEADER_LEN + PIM_HEARDER_SIZE];
  L7_uchar8 *cp;
  L7_int32 checksum =0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "Enter");
  if((L7_NULLPTR == pData) || (L7_NULLPTR == pPktLen))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"invlaid input parameters");
    return L7_FAILURE;
  }
  L7_IP6_SET_VER(ip6Hdr, PIMSM_IPV6_VER);
  ip6Hdr->paylen = osapiHtons(PIM_HEARDER_SIZE);
  ip6Hdr->hoplim = 0;
  ip6Hdr->next = osapiHtons(IP_PROT_PIM);
  memcpy(ip6Hdr->src,pSrcAddr->addr.ipv6.in6.addr8,16);
  memcpy(ip6Hdr->dst,pGrpAddr->addr.ipv6.in6.addr8,16);



  /* prepare the pseudo header */
  cp = pseudoIpv6Hdr;
  MCAST_PUT_DATA(pSrcAddr->addr.ipv6.in6.addr8,16, cp);
  MCAST_PUT_DATA(pGrpAddr->addr.ipv6.in6.addr8,16,cp);
  MCAST_PUT_LONG(4, cp);
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_BYTE(IP_PROT_PIM, cp);

    /* append the PIM pData */
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_SHORT(0, cp);
  checksum = inetChecksum((void*)pseudoIpv6Hdr, cp - pseudoIpv6Hdr);

  /* Prepare the PIM pData */
  cp = pimsmHdr;
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_BYTE(0, cp);
  MCAST_PUT_SHORT(checksum, cp);
  
  cp = pData;
  memcpy(cp , ip6Hdr, L7_IP6_HEADER_LEN);
  cp  += L7_IP6_HEADER_LEN;
  memcpy(cp , pimsmHdr, PIM_HEARDER_SIZE);

  *pPktLen =L7_IP6_HEADER_LEN + PIM_HEARDER_SIZE;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose   This function is called to set the socket options for 
               the Ipv6 socket descriptor passed as input.
*
* @param     sockFd      @b{(input)}  socket fd
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments      none
*
* @end
******************************************************************************/
L7_RC_t pimsmV6SocketOptionsSet(L7_uint32 sockfd)
{
  L7_int32 sockOpt = -1;
  L7_sockaddr_in6_t in6addrAny;

  sockOpt = 1;
  if(osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_RECVPKTINFO,
                     (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: L7_IPV6_RECVPKTINFO socket option failed\n");
    return L7_FAILURE;
  }

  sockOpt = 0;
  if(osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_MULTICAST_LOOP,
                     (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: L7_IPV6_MULTICAST_LOOP socket option failed\n");
    return L7_FAILURE;
  }

  sockOpt = 1;
  if(osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_MULTICAST_HOPS,
                     (L7_uchar8 *) &sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: L7_IPV6_MULTICAST_HOPS socket option failed\n");
    return L7_FAILURE;
  }

  #if 0
   /* Set the offset of the checksum field in the PIM message.
      By setting this,the checksum will be calculated by the IP layer 
      if the checksum is filled as 0 */
  sockOpt = 2;
  if(osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_CHECKSUM,
                     (L7_uchar8 *) &sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    LOG_MSG("PIMSM: L7_IPV6_CHECKSUM socket option failed\n");
    return L7_FAILURE;
  }
  #endif

  /* bind IN6ADDR_ANY */
  memset(&in6addrAny, 0, sizeof(in6addrAny));
  in6addrAny.sin6_family = L7_AF_INET6;

  if(osapiSocketBind(sockfd, (L7_sockaddr_t *) & in6addrAny,
                     sizeof(in6addrAny)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: osapiSocketBind() failed\n");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "IPv6 Socket Options Set Success");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Intialize memory to receive RTO route changes 
*
* @param    pimsmCb     @b{(input)}  control block 
* @param    
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmV6RouteChangeBufMemAlloc(pimsmCB_t *pimsmCb)
{
  rto6RouteChange_t  *routeChangeBuf = L7_NULLPTR;

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"Invalid input parameter");
    return L7_FAILURE;
  }
  routeChangeBuf = (rto6RouteChange_t *)
                   PIMSM_ALLOC (pimsmCb->family,
                               (PIMSM_BEST_ROUTE_CHANGES_MAX *
                                sizeof(rto6RouteChange_t)));

  if(routeChangeBuf == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for RTO Buffer\n");
    return L7_FAILURE;
  }

  pimsmCb->pimsmRouteChangeBuf =(void *)routeChangeBuf;
  return L7_SUCCESS;

}
