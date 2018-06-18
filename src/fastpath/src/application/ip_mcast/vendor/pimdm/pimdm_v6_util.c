/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   pimdm_v6_util.c
*
* @purpose    To maintain the PIM-DM V6 specific functionality
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_rto6_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_ip6_api.h"
#include "l7_socket.h"
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "pimdm_mrt.h"
#include "pimdm_util.h"
#include "pimdm_debug.h"
#include "pimdm_mgmd.h"
#include "pim_defs.h"
#include "mcast_util.h"

/*******************************************************************************
**                    Externs & Global declarations                           **
*******************************************************************************/

/* Number of elements in IPv6 heap pool list, including dummy end-of-list element */
#define PIMDM_IPV6_POOL_LIST_SIZE 11
heapBuffPool_t pimdmV6HeapPoolList[PIMDM_IPV6_POOL_LIST_SIZE];


/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Build the PIM-DM IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment PIMDM_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *pimdmV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(pimdmV6HeapPoolList, 0, sizeof(heapBuffPool_t) * PIMDM_IPV6_POOL_LIST_SIZE);

  pimdmV6HeapPoolList[i].buffSize = sizeof(pimdmCB_t);
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM CONTROL BLOCK", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = PIMDM_MAX_JP_MSG_SIZE;
  pimdmV6HeapPoolList[i].buffCount = PIMDM_MAX_JP_PACKETS;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM_JPG_MSG_BUFFER", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = PIM_IPV6_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST * sizeof(pim_encod_src_addr_t);
  pimdmV6HeapPoolList[i].buffCount = PIMDM_MAX_PROTOCOL_PACKETS;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM_JP_ADDR_LIST_BUFFER", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(avlTreeTables_t) * platIpv6McastRoutesMaxGet();
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM MRT TREE HEAP", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(pimdmMrtEntry_t) * platIpv6McastRoutesMaxGet();
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM MRT DATA HEAP", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(pimdmDownstrmIntfInfo_t);
  pimdmV6HeapPoolList[i].buffCount = (PIMDM_NUM_OPTIMAL_OUTGOING_INTERFACES * platIpv6McastRoutesMaxGet());
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM MRT OPT OIFs", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(avlTreeTables_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES;
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM MGMD TREE HEAP", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(pimdmLocalMemb_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES;
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM MGMD DATA HEAP", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(pimdmIntfInfo_t);
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM INTERFACES", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  pimdmV6HeapPoolList[i].buffSize = sizeof(rto6RouteChange_t) * PIMDM_MAX_BEST_ROUTE_CHANGES;
  pimdmV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "PIMDM RTO CHANGES", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Include end indicator whether or not IPv6 package */
  pimdmV6HeapPoolList[i].buffSize = 0;
  pimdmV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = 0;
  i++;

  if (i != PIMDM_IPV6_POOL_LIST_SIZE)
  {
    L7_LOG_ERROR(i);
  }

  return pimdmV6HeapPoolList;
}

/*********************************************************************
* @purpose  Function to get IPv6 best route changes
*           
*
* @param    pimdmCb          @b{(input)} pimdm control block
* @param    routeEntry       @b{(input)} Route information 
* @param    route_status     @b{(input)} Route Status
      Information regarding the reason the callback was
*        issued.  Route_status events are enumerated in 
*        RTO_ROUTE_EVENT_t.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments   
*
* @end
*********************************************************************/
static L7_RC_t pimdmV6BestRouteChanges(pimdmCB_t *pimdmCB,
                                       L7_route6Entry_t *routeEntry, 
                                       L7_uint32 route_status)
{
  L7_uint32 intIfNum =0;
  mcastRPFInfo_t routeInfo;
  L7_inet_addr_t tempSrcAddr;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if((routeEntry == L7_NULLPTR) || (pimdmCB == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_FAILURE;
  }

  /* Check to see if a Static MRoute already exists for the same network.
   * If so, ignore this notification.
   */
  inetAddressSet (pimdmCB->addrFamily, &(routeEntry->ip6Addr), &tempSrcAddr);
  if (mcastMapStaticMRouteBestRouteLookup (pimdmCB->addrFamily, &tempSrcAddr,
                                           &routeInfo)
                                        == L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_RTO, "Static MRoute already exists for Src - %s "
                 "Ignoring this RTO Notification", inetAddrPrint(&tempSrcAddr,src));
    return L7_FAILURE;
  }

  if (route_status == RTO_ADD_ROUTE  ||
      route_status == RTO_DELETE_ROUTE)
  {  
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Recvd Reject Route - Ignore");
      return L7_SUCCESS;
    }
  } 
  else if (route_status == RTO_CHANGE_ROUTE )
  {
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Recvd Reject Route - Delete");
      route_status = RTO_DELETE_ROUTE;
    }
  }

  memset(&routeInfo,0,sizeof(mcastRPFInfo_t));
  inetAddressZeroSet(pimdmCB->addrFamily, &routeInfo.rpfNextHop);
  inetAddressZeroSet(pimdmCB->addrFamily, &routeInfo.rpfRouteAddress);

  /* select only one path, let it be the first one index by zero*/
  inetAddressSet(pimdmCB->addrFamily, 
                 &(routeEntry->ecmpRoutes.equalCostPath[0].ip6Addr),
                 &routeInfo.rpfNextHop);

  /* RTO doesn't provide the intIfNum for the Deleted Routes.
   */
  if (route_status != RTO_DELETE_ROUTE)
  {
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].intIfNum;
    if(ip6MapIntIfNumToRtrIntf(intIfNum, &routeInfo.rpfIfIndex) != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to convert intfIfNum to rtrIfNum "
                  "for intIfNum - %d", intIfNum);
      return L7_FAILURE;
    }
  }

  routeInfo.rpfRouteMetricPref = routeEntry->pref;
  routeInfo.rpfRouteMetric = routeEntry->metric;
  routeInfo.rpfRouteProtocol =routeEntry->protocol;

  routeInfo.status = route_status;
  routeInfo.prefixLength = routeEntry->ip6PrefixLen; 
  inetAddressSet(pimdmCB->addrFamily, &routeEntry->ip6Addr, 
                 &(routeInfo.rpfRouteAddress));

  if (pimdmMrtRTOBestRouteChangeProcess (pimdmCB,&routeInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to process Best Route Changes");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose RTO best-route change event handler
*
* @param  pimdmCb     @b{ (input) } Pointer PIMDM Control Block.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimdmV6BestRoutesGet(pimdmCB_t *pimdmCB)
{
  L7_uint32          idx=0,numChanges =0;
  rto6RouteChange_t  *routeChangeBuf;
  L7_BOOL            moreChanges=L7_FALSE;

  PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"PIMDM Control Block is NULL");
    return L7_FAILURE;
  }
  routeChangeBuf = (rto6RouteChange_t*)pimdmCB->routeChangeBuffer;
  if(rto6RouteChangeRequest(pimdmCB->rtoCallback,
                            PIMDM_MAX_BEST_ROUTE_CHANGES,
                            &numChanges,&moreChanges,
                            routeChangeBuf) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to process RTO6 Route Change Request");
    return L7_FAILURE;
  }

  for(idx=0;idx < numChanges;idx++)
  {
    pimdmV6BestRouteChanges(pimdmCB,
                            &routeChangeBuf->routeEntry,
                            routeChangeBuf->changeType);
    routeChangeBuf++;
  }   
  if(moreChanges == L7_TRUE)
  {
    pimdmCB->rtoCallback();
  }

  PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");
  return L7_SUCCESS;  
}

/******************************************************************************
* @purpose Allocate memory for the RTO buffer 
*
* @param  pimdmCB     @b{ (input) } Pointer PIMDM Control Block.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimdmV6RouteChangeBufMemAlloc(pimdmCB_t *pimdmCB)
{
  rto6RouteChange_t  *routeChangeBuf = L7_NULLPTR;

  PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");
  if(L7_NULLPTR == pimdmCB)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"PIMDM Control Block is NULL");
    return L7_FAILURE;
  }

  if ((routeChangeBuf = PIMDM_ALLOC (pimdmCB->addrFamily,
                    (sizeof (rto6RouteChange_t) * PIMDM_MAX_BEST_ROUTE_CHANGES)))
                                  == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM v6 RTO Route Change Info Heap Allocation Failed");
    return L7_FAILURE;
  }

  pimdmCB->routeChangeBuffer =(void *)routeChangeBuf;
  
  PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Set Ipv6 Socket Options
*
* @param    pimdmCB @b{ (input) } pointer to Control Block
* @param    sockFD  @b{ (input) } socket fd
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmV6SocketOptionsSet (pimdmCB_t *pimdmCB,L7_int32 sockFd)
{
  L7_int32 sockRcvPktInfo = 1;
  L7_int32 sockMcastLoop = 0;
  L7_int32 sockMcastHops = 64;
  L7_int32 sockIpv6CkSum = 2;
  L7_sockaddr_in6_t in6addrAny;

  /* Set the Socket Receive Packet Info Option */
  if (osapiSetsockopt (sockFd, IPPROTO_IPV6, L7_IPV6_RECVPKTINFO,
                       (L7_uchar8*) &sockRcvPktInfo, sizeof (sockRcvPktInfo))
                       != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Recv Pkt Info v6 Sock Opt Set Failure");
    return L7_FAILURE;
  }

  /* Set the Socket Multicast Loop Option */
  if (osapiSetsockopt (sockFd, IPPROTO_IPV6, L7_IPV6_MULTICAST_LOOP,
                       (L7_uchar8*) &sockMcastLoop, sizeof (sockMcastLoop))
                       != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Mcast Loop v6 Sock Opt Set Failure");
    return L7_FAILURE;
  }

  /* Set the Socket Multicast Hops Option */
  if (osapiSetsockopt (sockFd, IPPROTO_IPV6, L7_IPV6_MULTICAST_HOPS,
                       (L7_uchar8*) &sockMcastHops, sizeof (sockMcastHops))
                       != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Multicast Hops v6 Sock Opt Set Failure");
    return L7_FAILURE;
  }

  /* Set the Socket IPv6 Checksum Option */
  if (osapiSetsockopt (sockFd, IPPROTO_IPV6,L7_IPV6_CHECKSUM,
                       (L7_uchar8*) &sockIpv6CkSum, sizeof (sockIpv6CkSum))
                       != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Checksum v6 Sock Opt Set Failure");
    return L7_FAILURE;
  }

  /* Bind client-side socket to v6 addr any */
  memset (&in6addrAny, 0, sizeof(in6addrAny));
  in6addrAny.sin6_family = L7_AF_INET6;
    
  if(osapiSocketBind (sockFd, (L7_sockaddr_t *)&in6addrAny, 
                      sizeof (in6addrAny))
                      != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "v6 Socket Bind Failure");
    return L7_FAILURE;
  }

  /* Register the SockFD with Event Scheduler for IPv6 Packet Reception */
  if(mcastMapPktRcvrSocketFdRegister (sockFd, 
                                      MCAST_EVENT_IPv6_CONTROL_PKT_RECV, 
                                      mcastCtrlPktBufferPoolIdGet(L7_AF_INET6),
                                      L7_IPV6_PKT_RCVR_ID_PIMDM) 
                                      != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Socket FD Regis'tion Failure");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
