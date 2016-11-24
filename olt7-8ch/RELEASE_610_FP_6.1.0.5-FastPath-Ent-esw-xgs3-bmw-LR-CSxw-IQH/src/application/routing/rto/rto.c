/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto.c
*
* @purpose The rto component maintains a database of all routes in the
*          system.
*
* @component Routing Table Object for IP Mapping Layer
*
* @comments none
*
* @create 03/22/2001
*
* @author alt
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "stdlib.h"
#include "string.h"
#include "log.h"
#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l3end_api.h"
#include "rto_api.h"
#include "rto.h"
#include "radix_api.h"
#include "rtiprecv.h"
#include "l7_ip_api.h"
#include "l7_ipmap_arp_api.h"

/* Mapping from L7_RTO_PROTOCOL_INDICES_t to L7_REDIST_RT_INDICES_t */
L7_REDIST_RT_INDICES_t protocolMapping[RTO_LAST_PROTOCOL] = 
    {REDIST_RT_FIRST, 
     REDIST_RT_LOCAL, 
     REDIST_RT_STATIC, 
     REDIST_RT_FIRST,
     REDIST_RT_OSPF, REDIST_RT_OSPF, REDIST_RT_OSPF, REDIST_RT_OSPF, 
     REDIST_RT_OSPF, REDIST_RT_OSPF,
     REDIST_RT_FIRST, REDIST_RT_FIRST, REDIST_RT_FIRST, REDIST_RT_FIRST,
     REDIST_RT_FIRST, REDIST_RT_FIRST,
     REDIST_RT_RIP,
     REDIST_RT_STATIC, 
     REDIST_RT_BGP, REDIST_RT_BGP,
     REDIST_RT_FIRST, REDIST_RT_FIRST, REDIST_RT_FIRST};

L7_uchar8 *rtoProtoNames[RTO_LAST_PROTOCOL] = 
{
    "Reserved", "Local", "Static", "MPLS", "OSPF INTRA", "OSPF INTER", 
    "OSPF ET1", "OSPF ET2", "OSPF NSSA ET1", "OSPF NSSA ET2", 
    "OSPFV3 INTRA", "OSPFV3 INTER", 
    "OSPFV3 ET1", "OSPFV3 ET2", "OSPFV3 NSSA ET1", "OSPFV3 NSSA ET2", "RIP",
    "Default", "IBGP", "EBGP", "Reserved", "Reserved", "Reserved"
};

/* Table of registered callback functions for ARP gateway notification. */
static RtoCallbackElement CallbackList[RTO_MAX_CALLBACKS];

/* Maximum number of routes in the database.*/
static L7_uint32 RtoMaxRoutes;

static L7_uchar8         *rtoRouteTreeHeap;
static rtoRouteData_t    *rtoRouteDataHeap;
static rtoRouteInfo_t    *rtoRouteInfo;
radixTree_t       rtoRouteTreeData;
rto_stats_t       rtoStats;
static L7_BOOL           rtoInitialized = L7_FALSE;
L7_uint32                rtoEcmpRouteCount;
L7_uint32                rtoEcmpRouteMax;

/* Free route info descriptors are stored in a list.*/
static rtoRouteInfo_t *rtoFreeRouteInfoList = 0;

/* next hop registration */
static L7_uint32         RtoMaxNHRes;
static L7_uchar8         *rtoNHResTreeHeap;
static rtoNhopData_t     *rtoNHResDataHeap;
static rtoProtoInfo_t    *rtoProtoInfo;
radixTree_t              rtoNHResTreeData;
static rtoProtoInfo_t    *rtoFreeProtoInfoList = 0;

/* read-write lock protects all RTO data. */
osapiRWLock_t    rtoRwLock;

L7_uint32 rtoTraceFlags = 0;

rtoRouteData_t *rtoFirstRouteGet(L7_uint32 lookupType);

static L7_RC_t rtoRouteCopy(rtoRouteInfo_t *routeInfo, 
                            L7_routeEntry_t *routeEntry);

static rtoRouteData_t *rtoRouteDataFind(L7_uint32 destPrefix, L7_uint32 destMask);

static rtoRouteInfo_t *rtoRouteInfoPrefFind(L7_uint32 destPrefix, L7_uint32 destMask,
                                            L7_uint32 pref, 
                                            L7_RTO_PROTOCOL_INDICES_t protocol);

static L7_BOOL rtoRouteInfoMatch(rtoRouteInfo_t *routeInfo, 
                                 L7_routeEntry_t *routeEntry);

static void rtoEcmpRetry(void);
static L7_RC_t rtoRouterIfResolve(L7_arpEntry_t *arpEntry, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Initialize RTO object
*
* @param    max_routes    Maximum number of best routes to store in the table.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    The max number of best routes is normally limited by hardware.
*           RTO will also maintain non-best routes. 
*
* @end
*********************************************************************/
L7_RC_t
rtoCnfgrInitPhase1Process(L7_uint32 max_routes, L7_uint32 max_nhres)
{
  L7_uint32 i;
  L7_uint32 treeHeapSize; /* memory for radix tree internal data structures
                           * (mask nodes and mask lists).
                           * This is a function of the maximum number of
                           * active routes that the RTO supports (RtoMaxRoutes).
                           */

  L7_uint32 dataHeapSize; /* memory for rtoRouteData structures that define a
                           * radix node+leaf pair to attach to the tree, the 
                           * key (addr, mask) and a pointer to a routeInfo 
                           * that describes the route.
                           * This is a function of the maximum number of
                           * active routes that the RTO supports (RtoMaxRoutes).
                           */

  L7_uint32 routeInfoSize;/* memory for rtoRouteInfo structures that descibe 
                           * routes. At least one routeInfo hangs off every 
                           * tree leaf (via the ptr in rtoRouteData). This 
                           * describes the primary (best) route to the subnet.
                           * Other (alternative) routes to the same destination 
                           * are described by items in the routeInfo linked-list. 
                           * This is a function of the total routes in RTO best+
                           * non-best (maxAllRoutes).
                           */

  L7_uint32 maxAllRoutes;  /* max routes (best + non-best) */


  /* needed simply to get stuff in rto_debug.c to be linked in */
  rtoDebugInit();

  /* Cleanup data structures before proceeding */
  bzero ((void *) CallbackList, sizeof (CallbackList));


  /* max number of destinations */
  RtoMaxRoutes = max_routes;

  /* max number of routes in RTO, both best and non-best */
  /* Allow space for some non-best routes */
  maxAllRoutes = 2 * max_routes;

  /* calculate buffer sizes. Because deleted routes can hang around until 
   * best route clients are notified, allocate space for twice the number
   * of routes we want to support. */
  treeHeapSize = RADIX_TREE_HEAP_SIZE(2 * RtoMaxRoutes, sizeof(rtoTreeKey_t));
  dataHeapSize = RADIX_DATA_HEAP_SIZE(2 * RtoMaxRoutes, sizeof (rtoRouteData_t));
  routeInfoSize= maxAllRoutes * sizeof (rtoRouteInfo_t);

  if (rtoTraceFlags & RTO_TRACE_ALLOC) 
  {
    L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO allocating memory for radix tree of %u nodes. \ntree heap: %u\ndata heap: %u\nroute info: %u", 
            max_routes, treeHeapSize, dataHeapSize, routeInfoSize);               
    rtoTraceWrite(traceBuf);
  } 

  /* only need one for each best route */
  rtoRouteTreeHeap = osapiMalloc (L7_IP_MAP_COMPONENT_ID, treeHeapSize);
  if (rtoRouteTreeHeap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* only need one for each best route */
  rtoRouteDataHeap =  osapiMalloc (L7_IP_MAP_COMPONENT_ID, dataHeapSize);
  if (rtoRouteDataHeap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  rtoRouteInfo =  osapiMalloc (L7_IP_MAP_COMPONENT_ID, routeInfoSize);
  if (rtoRouteInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Cleanup data structures before proceeding */
  bzero ((char *)rtoRouteTreeHeap, treeHeapSize);
  bzero ((char *)rtoRouteDataHeap, dataHeapSize);
  bzero ((char *)rtoRouteInfo, routeInfoSize);
  bzero((char *) &rtoStats, sizeof(rto_stats_t));

  rtoEcmpRouteCount = 0;
  rtoEcmpRouteMax = L7_IPV4_ECMP_ROUTES_MAX;

  /* The destination networks are stored in a radix tree.
   * NOTE: Its important to ensure that the correct max_routes is
   * passed in here! Radix uses the same RADIX_..SIZE macros while
   * initializing the structs. A mismatch will cause a crash!
   */
  radixCreateTree(&rtoRouteTreeData, rtoRouteDataHeap, rtoRouteTreeHeap, 
                  2 * RtoMaxRoutes, sizeof(rtoRouteData_t), sizeof(rtoTreeKey_t));

  /* Multiple routes to the same destination are stored in a linked list
  ** that starts on the radix node for the target network.
  ** Here we create the free list.  
  */
  for (i = 0; i < maxAllRoutes; i++)
  {
    rtoRouteInfo[i].next = rtoFreeRouteInfoList;
    rtoFreeRouteInfoList = &rtoRouteInfo[i];
  }

  RtoMaxNHRes  = max_nhres;
  /* space for next hop resolution tree */
  rtoNHResTreeHeap = osapiMalloc (L7_IP_MAP_COMPONENT_ID, RADIX_TREE_HEAP_SIZE(RtoMaxNHRes, sizeof(rtoTreeKey_t)));
  if (rtoNHResTreeHeap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  rtoNHResDataHeap =  osapiMalloc (L7_IP_MAP_COMPONENT_ID, RtoMaxNHRes * sizeof (rtoRouteData_t));
  if (rtoNHResDataHeap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  rtoProtoInfo =  osapiMalloc (L7_IP_MAP_COMPONENT_ID, 2*RtoMaxNHRes * sizeof (rtoProtoInfo_t));
  if (rtoProtoInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  bzero ((char *)rtoNHResTreeHeap, RADIX_TREE_HEAP_SIZE(RtoMaxNHRes, sizeof(rtoTreeKey_t)));
  bzero ((char *)rtoNHResDataHeap, RtoMaxNHRes * sizeof (rtoRouteData_t));
  bzero ((char *)rtoProtoInfo, 2* RtoMaxNHRes * sizeof (rtoProtoInfo_t));


  /* The set of next hops that require resolution notifies are stored in a radix tree.
  */
  radixCreateTree(&rtoNHResTreeData, rtoNHResDataHeap, rtoNHResTreeHeap, 
                  RtoMaxNHRes, (L7_uint32)(sizeof(rtoRouteData_t)), sizeof(rtoTreeKey_t));
  for (i = 0; i < 2*RtoMaxNHRes; i++)
  {
    rtoProtoInfo[i].next = rtoFreeProtoInfoList;
    rtoFreeProtoInfoList = &rtoProtoInfo[i];
  }

  rtoClientListAllocate();

  if (osapiRWLockCreate(&rtoRwLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
           "Failed to create RTO read-write lock.");
        
    return L7_FAILURE;
  } 

  rtoInitialized = L7_TRUE;

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Fini RTO object
*
* @param    max_routes    Maximum number of routes to store in the table.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    none
*
* @end
*********************************************************************/
void
rtoCnfgrFiniPhase1Process(L7_uint32 max_routes)
{


  (void)radixDeleteTree(&rtoRouteTreeData);

  /* Cleanup data structures before proceeding */
  bzero ((void *) CallbackList, sizeof (CallbackList));

  if (rtoRouteTreeHeap != L7_NULLPTR)
    OSAPI_FREE_MEMORY(rtoRouteTreeHeap); 

  if (rtoRouteDataHeap != L7_NULLPTR)
    OSAPI_FREE_MEMORY(rtoRouteDataHeap); 

  if (rtoRouteInfo != L7_NULLPTR )
    OSAPI_FREE_MEMORY(rtoRouteInfo); 

  (void)radixDeleteTree(&rtoNHResTreeData);

  if (rtoNHResTreeHeap != L7_NULLPTR)
    OSAPI_FREE_MEMORY(rtoNHResTreeHeap); 

  if (rtoNHResDataHeap != L7_NULLPTR)
    OSAPI_FREE_MEMORY(rtoNHResDataHeap); 

  if (rtoProtoInfo != L7_NULLPTR )
    OSAPI_FREE_MEMORY(rtoProtoInfo); 

  rtoClientListDeallocate();

  osapiRWLockDelete(rtoRwLock);

  return;
}


/*********************************************************************
* @purpose  Get a route info structure from the free list.
*
* @param    none
*
* @returns  pointer to an available route info structure. All fields
*           in the structure are initialized to 0.
*
* @notes    
*
* @end
*********************************************************************/
static rtoRouteInfo_t *rtoNewRouteInfoGet(void)
{
    rtoRouteInfo_t *routeInfo = rtoFreeRouteInfoList;
    if (routeInfo == L7_NULLPTR)
        LOG_ERROR (rtoStats.route_entries);
   
    rtoFreeRouteInfoList = rtoFreeRouteInfoList->next;
    memset ((char *) routeInfo, 0, sizeof (rtoRouteInfo_t));
    return routeInfo;
}

/*********************************************************************
* @purpose  Change the preference of a specific route.
*
* @param   destAddr @b{(input)}  route's destination address
* @param   destMask @b{(input)}  route's destination mask
* @param   protocol @b{(input)}  protocol that supplied the route
* @param   oldPref  @b{(input)}  previous preference 
* @param   newPref  @b{(input)}  new preference for the route
*
* @returns  L7_SUCCESS  If the preference was successfully changed
* @returns  L7_FAILURE  If the preference could not be changed
*
* @notes    RTO may select a new best route to the route's destination.
*
* @end
*********************************************************************/
L7_RC_t rtoRoutePrefChange(L7_uint32 destAddr, L7_uint32 destMask, 
                           L7_RTO_PROTOCOL_INDICES_t protocol, 
                           L7_uint32 oldPref, L7_uint32 newPref)
{
    rtoRouteData_t *pData;
    rtoRouteInfo_t *routeInfo;
    rtoRouteInfo_t *curBestRoute = L7_NULLPTR;   /* current best route to this dest */
    L7_RC_t rc = L7_FAILURE;

    if (rtoTraceFlags & RTO_TRACE_PREF)
    {
        L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
        L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(destAddr, destAddrStr);
        osapiInetNtoa(destMask, destMaskStr);
        osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO changing preference of %s route to %s %s from %u to %u", 
                rtoProtoNames[protocol], destAddrStr, destMaskStr, oldPref, newPref);
        rtoTraceWrite(traceBuf);
    }

    if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        return L7_FAILURE;
    
    routeInfo = rtoRouteInfoPrefFind(destAddr, destMask, oldPref, protocol);
    if (routeInfo)
    {
      routeInfo->preference1 = newPref;
      pData = rtoRouteDataFind(destAddr, destMask);
      if (pData)
      {
        if (pData->nextRouteInfo && 
            (pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE))
        {
          curBestRoute = pData->nextRouteInfo;
        }
        rtoSelectNewBestRoute(pData, curBestRoute, L7_FALSE);
        rc = L7_SUCCESS;
      }
    }

    osapiWriteLockGive(rtoRwLock);
    return rc;
}

/*********************************************************************
* @purpose  Change the metric of a specific static route.
*
* @param   destAddr @b{(input)}  route's destination address
* @param   destMask @b{(input)}  route's destination mask
* @param   metric   @b{(input)}  new metric for the route
*
* @returns  L7_SUCCESS  If the metric was successfully changed
* @returns  L7_FAILURE  If the metric could not be changed
*
* @notes    This function is being added to enable us to configure a
*           router so that it will pass three ANVL tests. These ANVL
*           tests (ANVL PIMSM 7.1 and ANVL PIMDM 5.3 and 5.15) expect 
*           the router to set the metric in an Assert message to a 
*           specific value. The routes are added to RTO as static routes.
*           But we do not allow the user to specify the metric of a
*           static route normally (there normally is no value in 
*           configuring the metric of a static route). Adding a metric
*           option to the "ip route" command could cause confusion, esp.
*           since cisco's version of "ip route" doesn't allow configuration
*           of a metric. So we decided to create a hidden CLI command
*           allowing the metric of a static route to be changed
*           directly in RTO. 
*
*           Changing a route's metric has no effect on best route selection.
*           So we don't do best route selection. Note that this means any
*           protocol redistributing static routes won't be notified of the
*           metric change. Again, this function is only intended to be used
*           to configured these specific ANVL tests.
*
*           Note that there can be multiple static routes to a given 
*           destination, each with a different preference. This function
*           changes the metric of the first static route and doesn't 
*           consider preference. Since to pass the ANVL tests we're trying
*           change the metric of the best route, this should be ok.
*
* @end
*********************************************************************/
L7_RC_t rtoRouteMetricChange(L7_uint32 destAddr, L7_uint32 destMask, 
                             L7_uint32 metric)
{
    rtoRouteData_t *pData;
    rtoRouteInfo_t *routeInfo;
    L7_RC_t rc = L7_FAILURE;

    if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        return L7_FAILURE;
        
    pData = rtoRouteDataFind(destAddr, destMask);
    if (pData)
    {
        /* iterate through routes to this destination looking for the static route. */
        routeInfo = pData->nextRouteInfo;
        while (routeInfo)
        {
            if ((routeInfo->protocol == RTO_STATIC) || 
                (routeInfo->protocol == RTO_DEFAULT))
            {
                routeInfo->metric = metric;
                rtoSelectNewBestRoute(pData, pData->nextRouteInfo, L7_TRUE);                
                rc = L7_SUCCESS;
            }
            routeInfo = routeInfo->next;
        }
    }

    osapiWriteLockGive(rtoRwLock);
    return rc;
}

/*********************************************************************
* @purpose  In response to a configuration change to the preference
*           for routes from a specific source protocol, update existing
*           routes in the routing table with the new preference.
*
* @param    index @b{(input)}  router protocol type
* @param    pref  @b{(input)}  preference associated with the protocol
*
* @returns  L7_SUCCESS  If the new preference was successfully applied
* @returns  L7_FAILURE  If the new preference could not be applied
*
* @notes    This function walks the
*           tree of network entries and applies the new preference.
*           For each destination, RTO re-elects a new best route as 
*           a result of changes to the route preferences.
*
*           This function is not used for static routes. Thus we can
*           use the assumption that the source protocol provides at most
*           a single route to a given destination.
*
*           RADIX_USAGE_NOTE:
*           This function iterates routes in the tree using 
*           a combination of L7_RN_GETNEXT and radixGetNextEntry.
*           This is the most efficient way to iterate provided
*           the tree stays locked (no changes allowed) and you're
*           not expecting a monotonically increasing key (ipaddr, mask).
*
* @end
*********************************************************************/
L7_RC_t rtoRouterPreferenceApply(L7_uint32 index, L7_uint32 pref)
{
    rtoRouteData_t  *pData = L7_NULLPTR;      /* Current network node */
    rtoRouteInfo_t  *routeInfo = L7_NULLPTR;  /* Current routeInfo node
                                                 for the current network */

    if (rtoTraceFlags & RTO_TRACE_PREF)
    {
        L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
        osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO changing preference of %s routes to %u", 
                rtoProtoNames[index], pref);
        rtoTraceWrite(traceBuf);
    }

    if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        return L7_FAILURE;

    pData = rtoFirstRouteGet(L7_RN_GETNEXT);
    if (pData == L7_NULLPTR)
    {
        /* If network is not found then we are done.*/
        if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");

        return L7_SUCCESS;
    }

    /* Iterate through all destinations */
    while (pData != L7_NULLPTR)
    {
        routeInfo = pData->nextRouteInfo;

        while (routeInfo != L7_NULLPTR)
        {
            if (routeInfo->protocol == index)
            {
                routeInfo->preference1 = pref;
                rtoSelectNewBestRoute(pData, pData->nextRouteInfo, L7_FALSE);
                /* A protocol can only supply a single route to a given destination.
                 * So no need to look at the rest of the routes to this dest. */
                break;
            }
            routeInfo = routeInfo->next;
        }

        /* Next destination. */
        pData = radixGetNextEntry(&rtoRouteTreeData, pData);
    }

    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Inform routing table manager that an IP interface became active.
*
* @param    intIfNum  @b{(input)}  Internal Interface Number
*
* @notes    No longer does anything
*
* @end
*********************************************************************/
void rtoIfUp (L7_uint32 intIfNum)
{
  return;
}

/*********************************************************************
* @purpose  Inform routing table manager that an IP interface went down
*
* @param    intIfNum  @b{(input)}  Internal Interface Number
*
* @notes    none
*
* @end
*********************************************************************/
void rtoIfDown (L7_uint32 intIfNum)
{
  return;
}

/*********************************************************************
* @purpose  Delete all routes whose next hop is out a given interface. 
*
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @notes    IP MAP now triggers delete of static and local routes
*           associated with an interface that's gone down. Dynamic
*           routing protocols handle their own routes.
*
* @end
*********************************************************************/
void rtoIfRouteFlush (L7_uint32 intIfNum)
{
  return;
}

/*********************************************************************
* @purpose  Given a routeInfo object whose destination matches a given
*           routeEntry object, determine if the routeInfo and routeEntry
*           represent the same route.
*
* @param    routeInfo  @b{(input)} Potential match for routeEntry
* @param    routeEntry @b{(input)} Identifies the route to be deleted.
*
* @returns  L7_TRUE if routeInfo is a match
* @returns  L7_FALSE if not a match
*
* @notes    This is an implementation function for rtoRouteAdd() and
*           rtoRouteDelete(). When adding a route, RTO needs to determine 
*           whether the new route is really the same as an existing route, or
*           if it differs enough to be considered a different route.  
*           When deleting a route, there are a couple complications when 
*           trying to find the route to delete. When BGP deletes a route, it
*           does not indicate whether the original route was an internal or
*           an external BGP route. We consider either to be a match, since 
*           BGP should never give RTO two routes to the same destination. 
*           The user can configure static and default routes to a given 
*           destination with different preferences. So if the source protocol
*           is static or default, the routes are only considered the same
*           if they have the same route preference.
*
*           We do not require that the next hops match. We cannot have two 
*           routes from the same source with the same preference with a 
*           different set of next hops.
*
* @end
*********************************************************************/
static L7_BOOL rtoRouteInfoMatch(rtoRouteInfo_t *routeInfo, 
                                 L7_routeEntry_t *routeEntry)
{
    if (routeInfo->protocol == routeEntry->protocol)
    {
        if ((routeInfo->protocol == RTO_STATIC) || 
            (routeInfo->protocol == RTO_DEFAULT))
        {
            /* must match preference, too */
            if (routeInfo->preference1 == routeEntry->pref)
            {
                return L7_TRUE;
            }
        }
        else
        {
            return L7_TRUE;
        }
    }
         
    /* Since BGP should never give RTO two routes to the same destination, we
     * can safely consider iBGP and eBGP routes to be a match here. 
     */
    if (((routeInfo->protocol == RTO_IBGP) || (routeInfo->protocol == RTO_EBGP)) &&
         ((routeEntry->protocol == RTO_IBGP) || (routeEntry->protocol == RTO_EBGP)))
    {
        return L7_TRUE;
    }
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Delete a route. 
*
* @param    routeEntry @b{(input)} Identifies the destination prefix and 
*              the source of the route to be deleted.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    Called by a routing protocol when it no longer has a route
            to a particular destination network.
*
* @end
*********************************************************************/
L7_RC_t rtoRouteDelete (L7_routeEntry_t *routeEntry)
{
  rtoRouteData_t *pData;
  rtoRouteInfo_t *routeInfo;
  rtoRouteInfo_t *prevRouteInfo = L7_NULLPTR;
  L7_BOOL        route_found = L7_FALSE;
  rtoRouteData_t tempData;
  L7_BOOL rtoFull = rtoIsFull();
  L7_routeEntry_t delRouteEntry; /* For notifying ARP of next hops gone */
  L7_uint32 i;

  /* Number of active next hops in the route being deleted */
  L7_uint32 activeNextHops = 0;


  if (rtoTraceFlags & RTO_TRACE_ROUTE_DEL)
  {
      L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
      L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(routeEntry->ipAddr, destAddrStr);
      osapiInetNtoa(routeEntry->subnetMask, destMaskStr);
      osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO deleting %s route to %s %s", 
              rtoProtoNames[routeEntry->protocol], destAddrStr, destMaskStr);
      rtoTraceWrite(traceBuf);
  }

  rtoStats.tot_dels++;  /* Count how many times this function is called */

  /* Cleanup data structures before proceeding */
  memset(&tempData, 0, sizeof(rtoRouteData_t));
  rtoSetKey(&tempData.network, (routeEntry->ipAddr & routeEntry->subnetMask));
  rtoSetKey(&tempData.netmask, routeEntry->subnetMask);

  if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return L7_FAILURE;

  /* Try to find the destination network.
  */
  pData = radixLookupNode(&rtoRouteTreeData, &tempData.network, &tempData.netmask, L7_RN_EXACT);
  if (pData == L7_NULLPTR)
  {
    /* If we can't find the network, increment bad deletes and exit */
    rtoStats.bad_dels++;
    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiWriteLockGive\n");
    return L7_FAILURE;
  }

  routeInfo = pData->nextRouteInfo;
  
  /* Find the route to this destination provided by the protocol deleting
   * the route. */
  while ((routeInfo != L7_NULLPTR) && (route_found == L7_FALSE))
  {
      if (rtoRouteInfoMatch(routeInfo, routeEntry) == L7_TRUE) 
      {
          if(pData->nextRouteInfo == routeInfo)
              pData->nextRouteInfo = routeInfo->next;
          else if(prevRouteInfo != L7_NULLPTR)
              prevRouteInfo->next = routeInfo->next;

          route_found = L7_TRUE;
      }
      else
      {
          prevRouteInfo = routeInfo;
          routeInfo = routeInfo->next;
      }
  } 

  /* If route is not found then update an error counter and return failure indication.
  */
  if (route_found == L7_FALSE || routeInfo == L7_NULLPTR)
  {
    rtoStats.bad_dels++;

    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return L7_FAILURE;
  }

  bzero((char *)&delRouteEntry, sizeof(L7_routeEntry_t));
  delRouteEntry.ipAddr = osapiNtohl(pData->network.addr);
  delRouteEntry.subnetMask = osapiNtohl(pData->netmask.addr);
  delRouteEntry.protocol = routeInfo->protocol;
  delRouteEntry.metric = routeInfo->metric;
  for (i = 0; i < routeInfo->numNextHops; i++)
  {
    /* Populate the callback structure with only the active routes */
    if (routeInfo->nextHops[i].ifUp == L7_TRUE)
    {
      /* Tell ARP about each next hop no longer used as a next hop */
      delRouteEntry.ecmpRoutes.numOfRoutes++;
      delRouteEntry.ecmpRoutes.equalCostPath[activeNextHops].arpEntry.intIfNum =
        routeInfo->nextHops[i].intIfNum;
      delRouteEntry.ecmpRoutes.equalCostPath[activeNextHops].arpEntry.ipAddr =
        routeInfo->nextHops[i].nextHopIP;
      activeNextHops++;
    }
  }

  /* Delete the protocol node */
  rtoCleanupAfterDelete(pData, routeInfo);

  if (rtoFull && (rtoIsFull() == L7_FALSE))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID, 
            "RTO no longer full. Routing table contains %u best routes, "
            "%u total routes, %u reserved local routes."
            " When the number of best routes drops below full capacity, RTO logs this notice.",
            rtoStats.best_entries, rtoStats.route_entries, 
            rtoStats.reserved_locals - rtoStats.active_locals); 
  }

  if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiWriteLockGive\n");

  /* Must do this after giving the RTO lock */
  if (activeNextHops > 0)
  {
    ipMapArpGwTableUpdate(&delRouteEntry, RTO_DELETE_ROUTE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds a network entry into the database.
*
* @param    routeEntry @b{(input)} Route information about the network 
*                                  to be added
*
* @returns  L7_SUCCESS  If the route is added to the routing table
* @returns  L7_TABLE_IS_FULL if the routing table is already full
* @returns  L7_ALREADY_CONFIGURED if the route is already in the table
* @returns  L7_NOT_EXIST if none of the route's next hops is in an 
*                        attached subnet
* @returns  L7_FAILURE  any other failure
*
* @notes    With introduction on un-numbered interface, the caller is expected
*           to set intf in routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.
*           However intIfNum is still optional param. When not known, the 
*           caller must set it to L7_INVALID_INTF and RTO will find outgoing 
*           interface from IP address in arpEntry. If not L7_INVALID_INTF RTO 
*           will take intf configured in arpEntry without further validation.
*
* @end
*********************************************************************/
L7_RC_t rtoRouteAdd (L7_routeEntry_t *routeEntry)
{
  L7_uint32 intIfNum;       
  rtoRouteData_t routeData, *pData;
  rtoRouteInfo_t *routeInfo, *bestRouteInfo = L7_NULLPTR;
  L7_uint32 protocol = routeEntry->protocol;       
  L7_BOOL   isRejectRoute = L7_FALSE;
  L7_uint32 i;

  L7_uint32 numNextHops;  /* Number of next hops in the new route. */
  
  /* The number of next hops in routeEntry on an UP interface */
  L7_uint32 activeNextHops = 0;

  L7_routeEntry_t allRouteEntry; 

  /* L7_TRUE if routeEntry contains at least one valid next hop */
  L7_BOOL   isValid = L7_FALSE;

  if (rtoTraceFlags & RTO_TRACE_ROUTE_ADD)
  {
      #define RTOROUTEADD_TRACE_BUFF_LEN (50 + (OSAPI_INET_NTOA_BUF_SIZE + 2) * platRtrRouteMaxEqualCostEntriesGet())
      L7_uchar8 traceBuf[RTOROUTEADD_TRACE_BUFF_LEN];
      L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(routeEntry->ipAddr, destAddrStr);
      osapiInetNtoa(routeEntry->subnetMask, destMaskStr);
      osapiSnprintf(traceBuf, RTOROUTEADD_TRACE_BUFF_LEN, 
          "RTO adding %s route to %s %s via ", 
              rtoProtoNames[routeEntry->protocol], destAddrStr, destMaskStr);
      for (i = 0; i < routeEntry->ecmpRoutes.numOfRoutes; i++)
      {
          osapiInetNtoa(routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr, nhStr);
          strcat(traceBuf, nhStr);
          osapiSnprintf(nhStr, OSAPI_INET_NTOA_BUF_SIZE, " intf %d", routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum);
          strcat(traceBuf, nhStr);
          if (i != routeEntry->ecmpRoutes.numOfRoutes - 1)
              strcat(traceBuf, ", ");
      }
      rtoTraceWrite(traceBuf);
  }

  if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return L7_FAILURE;

  rtoStats.tot_adds++;  /* Count how many times this function is called */

  /* Verify that the destination address is valid */
  if (rtoValidDestAddr(routeEntry->ipAddr, routeEntry->subnetMask) != L7_TRUE)
  {
      L7_uchar8 logbuf[255];
      L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(routeEntry->ipAddr, destAddrStr);
      osapiInetNtoa(routeEntry->subnetMask, destMaskStr);
      osapiSnprintf(logbuf, 255, "RTO rejected %s route with invalid destination address %s/%s",
              rtoProtoNames[routeEntry->protocol], destAddrStr, destMaskStr);
      L7_LOG(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID, logbuf);
      L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                    "The following destination addresses are considered invalid:"
                    "- the prefix has non-zero host bits"
                    "- the network mask is not contiguous"
                    "- either the prefix or mask is 0.0.0.0, but the other is not"
                    "- The prefix is 0.x.x.x"
                    "- class D and class E addresses"
                    "- the prefix is in 127.x.x.x");
      rtoStats.bad_adds++;
      osapiWriteLockGive(rtoRwLock);
      return L7_FAILURE;
  }

  /* If interface not specified, check if the route has at least one next hop 
   * in a local subnet. If not, bump up the bad adds couter and return */
  if (routeEntry->protocol == RTO_LOCAL)
  { 
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    isValid = L7_TRUE;
  }
  else
  {
    for(i = 0; i < routeEntry->ecmpRoutes.numOfRoutes; i++)
    {
      if(rtoRouterIfResolve(&routeEntry->ecmpRoutes.equalCostPath[i].arpEntry, 
                            &intIfNum) == L7_SUCCESS)
      {
        isValid = L7_TRUE;
        break;
      }
    }
  }

  isRejectRoute = ((routeEntry->flags & L7_RTF_REJECT) ? L7_TRUE : L7_FALSE);

  if((isValid != L7_TRUE) && (isRejectRoute != L7_TRUE))
  {
    rtoStats.bad_adds++; 
    osapiWriteLockGive(rtoRwLock);
    return L7_NOT_EXIST;
  }

  bzero ((char *)&routeData, sizeof (routeData));

  /* Always zero host bits in destination prefix. */
  rtoSetKey(&routeData.network, (routeEntry->ipAddr & routeEntry->subnetMask));
  rtoSetKey(&routeData.netmask, routeEntry->subnetMask);

  /* See if we already have a route to this destination. */
  pData = radixLookupNode(&rtoRouteTreeData, &routeData.network, 
                          &routeData.netmask, L7_RN_EXACT);

  /* If this is a new network then check that we have not exceeded
  ** the maximum route capacity.
  */
  if ((!pData || !pData->nextRouteInfo) && (rtoIsFull() == L7_TRUE))
  {
    /* if this is a local route and space is reserved for the local route, 
     * continue with add. */
    if ((routeEntry->protocol != RTO_LOCAL) ||
        (rtoStats.reserved_locals - rtoStats.active_locals) == 0)
    {
      rtoStats.bad_adds++;
      if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");
      
      return L7_TABLE_IS_FULL;
    }
  }

  if (pData == L7_NULLPTR)
  {
      /* Insert new destination */
    pData = radixInsertEntry(&rtoRouteTreeData, &routeData);

    /* NULL indicates successful insertion */
    if (pData == L7_NULLPTR)
    {
      rtoStats.radix_entries++;
    }
    else
      LOG_ERROR ((L7_uint32) pData);

    /* Find the new entry so that we can attach route info. */
    pData = radixLookupNode(&rtoRouteTreeData, &routeData.network, &routeData.netmask, L7_RN_EXACT);
    if (!pData)
    {
      LOG_ERROR ("Error: rto.c radix corruption detected!\n"); /* Database corruption. */
      return L7_FAILURE;
    }          
  }
  else
  {
    /* We already have a route to this destination. Note current best route. */
    if (pData->nextRouteInfo && 
        (pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE))
      bestRouteInfo = pData->nextRouteInfo;
  }

  routeInfo = pData->nextRouteInfo;

  /* See if this route is already in the routing table. */
  while (routeInfo != L7_NULLPTR)
  {
    if (rtoRouteInfoMatch(routeInfo, routeEntry) == L7_TRUE)
    {
       rtoStats.dup_adds++;

       if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
          LOG_MSG("Error: rto.c osapiWriteLockGive\n");

       return L7_ALREADY_CONFIGURED;
    }
    routeInfo = routeInfo->next;
  }

  routeInfo = rtoNewRouteInfoGet();
  routeInfo->protocol = protocol;
  /*Specifies the last time the route was updated (in hours:minutes:seconds)*/
  routeInfo->updateTime = osapiUpTimeRaw();
  routeInfo->preference1 = routeEntry->pref;
  if(routeEntry->flags & L7_RTF_REJECT)
     routeInfo->flags |= RTO_REJECT_ROUTE;

  /* To break the tie-up when the user configured preference1 values are equal
   * for the route entries, we use the preference2 values.
   * We assume the order in the enumeration L7_RTO_PROTOCOL_INDICES_t as the
   * order of preference2 values.
   *
   * i.e, local < static < mpls < intra < inter < type1 < type2 < nssa1 < nssa2
   *      < rip < default < ibgp < ebgp
   *
   * NOTE: low preference value means the most preferred route entry
   */
  routeInfo->preference2 = (int)routeEntry->protocol;

  routeInfo->metric = routeEntry->metric;

  numNextHops = (routeEntry->ecmpRoutes.numOfRoutes < platRtrRouteMaxEqualCostEntriesGet()) ?
      routeEntry->ecmpRoutes.numOfRoutes : platRtrRouteMaxEqualCostEntriesGet();
 
  bzero((char *)&allRouteEntry, sizeof(L7_routeEntry_t));
  allRouteEntry.ipAddr = osapiNtohl(pData->network.addr);
  allRouteEntry.subnetMask = osapiNtohl(pData->netmask.addr);
  allRouteEntry.protocol = routeInfo->protocol;
  allRouteEntry.metric = routeInfo->metric;
  allRouteEntry.updateTime = routeInfo->updateTime; 
  for(i = 0; i < numNextHops; i++)
  {
    /* resolve next hop IP address to outgoing interface */
    intIfNum = 0;
    if (routeEntry->protocol == RTO_LOCAL)
    { 
      intIfNum = routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum;
    }
    else
    {
      rtoRouterIfResolve(&routeEntry->ecmpRoutes.equalCostPath[i].arpEntry,
                            &intIfNum); 
    }
    if (intIfNum)
    {
      routeInfo->nextHops[activeNextHops].intIfNum = intIfNum;
      routeInfo->nextHops[activeNextHops].nextHopIP =
        routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
      routeInfo->nextHops[activeNextHops].ifUp = L7_TRUE;
      /* tell ARP the next hop address is the next hop of a route */
      allRouteEntry.ecmpRoutes.numOfRoutes++;
      allRouteEntry.ecmpRoutes.equalCostPath[activeNextHops].arpEntry.intIfNum = intIfNum;
      allRouteEntry.ecmpRoutes.equalCostPath[activeNextHops].arpEntry.ipAddr =
        routeInfo->nextHops[i].nextHopIP;
      activeNextHops++;
    }
  }
  routeInfo->numNextHops = activeNextHops;

  /* Insert the new route at the head of the list of routes to this dest. */
  routeInfo->next = pData->nextRouteInfo;
  pData->nextRouteInfo = routeInfo;

  rtoStats.route_entries++;
  if (routeInfo->protocol == RTO_LOCAL)
    rtoStats.active_locals++;

  /* Organize the next hop entries in ascending order of ip addresses */
  rtoNextHopsSort(routeInfo->nextHops, routeInfo->numNextHops);

  if (activeNextHops == 0)
  {
    /* Maybe this is just here for debugging */
    L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uint32 maskLen;
    osapiInetNtoa(osapiNtohl(pData->network.addr), destAddrStr);
    maskLen = rtoMaskLength(osapiNtohl(pData->netmask.addr));
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_IP_MAP_COMPONENT_ID,
            "Added route to %s/%u with no active next hops",
            destAddrStr, maskLen);
  }
    
  /* Determine if a new best route has been created. */
  rtoSelectNewBestRoute(pData, bestRouteInfo, L7_FALSE);

  /* If RTO has become full, log it. */
  if (rtoIsFull() == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
            "RTO is full. Routing table contains %u best routes, %u total routes. "
            " %u reserved local routes."
            " The routing table manager, also called RTO, stores a limited number "
            "of best routes, based on hardware capacity. When the routing table becomes "
            "full, RTO logs this alert. The count of total routes includes alternate "
            "routes, which are not installed in hardware.",
            rtoStats.best_entries, rtoStats.route_entries,
            rtoStats.reserved_locals - rtoStats.active_locals); 
  }

  if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiWriteLockGive\n");

  /* Need to do this after we give the RTO lock to avoid deadlock between
   * RTO lock and ARP lock. See defect 52171. */
  if (activeNextHops > 0)
  {
    /* Update ARP gw table with all active next hops. */
    ipMapArpGwTableUpdate(&allRouteEntry, RTO_ADD_ROUTE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modifies the route to the specified network, learned
*           via the specified protocol.
*
* @param    routeEntry  Route information about the network to modify
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    Called by a routing protocol when it wants to modify a 
*           to route's next hops. This function sends out notifications
*           for next hops that have been added/deleted and updates 
*           the route's route info. If none of the updated set of next
*           hops are active, the route is deleted. 
*
*           Other than the delete case, this function does not change
*           a route's "best route" status. However a changed notification
*           is sent out for best routes to allow other components to 
*           update the new set of next hops.
*
*
*           NOTE: no notifications for metric change
*
*           If only the metric changes we don’t send out any notifications. 
*           This is more efficient. However changes to BGP route 
*           redistribution could require this notification in the future.
*
*
*           NOTE:
*           Note that the best route change notification does not send 
*           out the old set of next hops. However since the modify 
*           operation translates to an add/delete operation for hardware, 
*           we can get away with this
*
* @end
*********************************************************************/
L7_RC_t rtoRouteModify (L7_routeEntry_t *routeEntry)
{
  rtoRouteData_t *pData;
  rtoRouteInfo_t *routeInfo;
  rtoRouteInfo_t updRouteInfo;

  /* These two route entries used to tell ARP gateway table about 
   * gateway addresses that have been added and removed. */
  L7_routeEntry_t addRouteEntry; 
  L7_routeEntry_t delRouteEntry; 

  /* Used to delete a route entry if left with no active next hops */
  L7_routeEntry_t inactiveRouteEntry;

  /* Number of gateways added and deleted */
  L7_uint32      addedNextHops = 0;
  L7_uint32      deletedNextHops = 0;

  L7_BOOL        route_found = L7_FALSE;
  L7_BOOL        isChanged = L7_FALSE;
  rtoRouteData_t tempData;
  L7_uint32      i, j;
  L7_uint32      intIfNum;
  L7_uint32      activeNextHops = 0;
  L7_uint32      updNextHops = 0;
  L7_BOOL bestWasEcmp, bestWasTrunc;


  if (rtoTraceFlags & RTO_TRACE_ROUTE_CHANGE)
  {
      L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
      L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(routeEntry->ipAddr, destAddrStr);
      osapiInetNtoa(routeEntry->subnetMask, destMaskStr);
      osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO changing %s route to %s %s", 
              rtoProtoNames[routeEntry->protocol], destAddrStr, destMaskStr);
      rtoTraceWrite(traceBuf);
  }

  /* Cleanup data structures before proceeding */
  memset(&updRouteInfo, 0, sizeof(updRouteInfo));
  memset(&tempData, 0, sizeof(rtoRouteData_t));
  rtoSetKey(&tempData.network, (routeEntry->ipAddr & routeEntry->subnetMask));
  rtoSetKey(&tempData.netmask, routeEntry->subnetMask);

  if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return L7_FAILURE;

  /* Try to find the destination network.
  */
  pData = radixLookupNode(&rtoRouteTreeData, &tempData.network, &tempData.netmask, L7_RN_EXACT);
  if (!pData || !pData->nextRouteInfo)
  {
    /* If we can't find the network, exit */
    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiWriteLockGive\n");
    return L7_FAILURE;
  }

  routeInfo = pData->nextRouteInfo;

  /* Find the route to this destination provided by the protocol deleting
   * the route. */
  while ((routeInfo != L7_NULLPTR) && (route_found == L7_FALSE))
  {
      if (rtoRouteInfoMatch(routeInfo, routeEntry) == L7_TRUE) 
        route_found = L7_TRUE;
      else
        routeInfo = routeInfo->next;
  } 

  /* If route is not found then return failure indication.
  */
  if (route_found == L7_FALSE || routeInfo == L7_NULLPTR)
  {
    rtoStats.bad_mods++;
    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return L7_FAILURE;
  }

  if(routeEntry->flags & L7_RTF_REJECT)
     routeInfo->flags |= RTO_REJECT_ROUTE;
  else
     routeInfo->flags &= ~RTO_REJECT_ROUTE;

  /* update the metric value. notifications do not go out 
   * if only the metric has changed.
   */
  routeInfo->metric = routeEntry->metric;
  /*Specifies the last time the route was updated (in hours:minutes:seconds)*/ 
  routeInfo->updateTime = osapiUpTimeRaw();
  
  if(routeInfo->flags & RTO_BEST_ROUTE_NODE)
  {
    if(routeInfo->numNextHops > 1) 
    {
       bestWasEcmp = L7_TRUE;
       if((routeInfo->flags & RTO_ECMP_TRUNC) != 0)
          bestWasTrunc = L7_TRUE;
       else
          bestWasTrunc = L7_FALSE;
    }
    else
    {
       bestWasEcmp = L7_FALSE;
       bestWasTrunc = L7_FALSE;
    }
  }
  else{
       bestWasEcmp = L7_FALSE;
       bestWasTrunc = L7_FALSE;
  }
  /********************************************************************
   * Process Next Hops
   * This is done in 3 stages.
   ********************************************************************/

  /********************************************************************
   * STAGE 1: check for new and unchanged next hops 
   *
   * note: we don't handle cases where the next hop ipaddr is the same
   * but the interface has changed as this can't happen without an
   * interface configuration change.
   ********************************************************************/
  bzero((char *)&addRouteEntry, sizeof(L7_routeEntry_t));
  addRouteEntry.ipAddr      = osapiNtohl(pData->network.addr);
  addRouteEntry.subnetMask  = osapiNtohl(pData->netmask.addr);
  addRouteEntry.protocol    = routeInfo->protocol;
  addRouteEntry.metric      = routeInfo->metric;

  if(routeEntry->ecmpRoutes.numOfRoutes > platRtrRouteMaxEqualCostEntriesGet())
      routeEntry->ecmpRoutes.numOfRoutes = platRtrRouteMaxEqualCostEntriesGet();

  /* check each next hop int the changed route against installed next hops */
  for(i = 0; i < routeEntry->ecmpRoutes.numOfRoutes; i++)
  {
    for(j = 0; j < routeInfo->numNextHops; j++)
    {
      if(routeInfo->nextHops[j].nextHopIP == 
         routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr &&
         routeInfo->nextHops[j].intIfNum ==
         routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum)
        break;
    }

    /* if found, add to updated route info to ensure we retain it */
    if(j < routeInfo->numNextHops)
    {
      /* update routeEntry as its used for the route changed notification */
      routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum = 
        routeInfo->nextHops[j].intIfNum;

      /* copy next hop information to upd route info */
      memcpy(&updRouteInfo.nextHops[updNextHops], 
             &routeInfo->nextHops[j], 
             sizeof(rtoNextHop_t));

      updNextHops++;

      if (routeInfo->nextHops[j].ifUp == L7_TRUE)
        activeNextHops++;

      continue;
    }

    /* never call rtoRouteModify() for a local route, so don't have 
     * to worry about local case here. */
    if (rtoRouterIfResolve (&routeEntry->ecmpRoutes.equalCostPath[i].arpEntry,
                            &intIfNum) == L7_SUCCESS)
    {
      /* add to updated routeInfo */
      updRouteInfo.nextHops[updNextHops].intIfNum = intIfNum;
      updRouteInfo.nextHops[updNextHops].nextHopIP = 
        routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
      updRouteInfo.nextHops[updNextHops].ifUp = L7_FALSE;

      /* update routeEntry becoz its used for the route changed notification */
      routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum = intIfNum;

        updRouteInfo.nextHops[updNextHops].ifUp = L7_TRUE;
        activeNextHops++;

        /* tell ARP the next hop address is the next hop of a route */
        addRouteEntry.ecmpRoutes.equalCostPath[addedNextHops].arpEntry.intIfNum = intIfNum;
        addRouteEntry.ecmpRoutes.equalCostPath[addedNextHops].arpEntry.ipAddr =
          routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
        addRouteEntry.ecmpRoutes.numOfRoutes++;
        addedNextHops++;

      updNextHops++;
    }
  }

  if (addedNextHops > 0)
  {
    isChanged = L7_TRUE;
  }

  /********************************************************************
  * STAGE 2: check for next hops that have been removed
  * 
  * compare each currently installed next hop with the modified set.
  * notify ARP of next hops that are not included anymore.
  *
  *********************************************************************/
  bzero((char *)&delRouteEntry, sizeof(L7_routeEntry_t));
  delRouteEntry.ipAddr      = osapiNtohl(pData->network.addr);
  delRouteEntry.subnetMask  = osapiNtohl(pData->netmask.addr);
  delRouteEntry.protocol    = routeInfo->protocol;
  delRouteEntry.metric      = routeInfo->metric;

  /* check each installed next hop against the changed route */
  for(i = 0; i < routeInfo->numNextHops; i++)
  {
    for(j = 0; j < routeEntry->ecmpRoutes.numOfRoutes; j++)
    {
      if(routeEntry->ecmpRoutes.equalCostPath[j].arpEntry.ipAddr == 
         routeInfo->nextHops[i].nextHopIP &&
         routeEntry->ecmpRoutes.equalCostPath[j].arpEntry.intIfNum == 
         routeInfo->nextHops[i].intIfNum)
        break;
    }

    /* if found, skip */
    if(j < routeEntry->ecmpRoutes.numOfRoutes)
      continue;

    /* Populate the callback structure with only the active next hops */
    if (routeInfo->nextHops[i].ifUp == L7_TRUE)
    {
      /* Tell ARP about each next hop no longer used as a next hop */
      delRouteEntry.ecmpRoutes.equalCostPath[deletedNextHops].arpEntry.intIfNum =
        routeInfo->nextHops[i].intIfNum;
      delRouteEntry.ecmpRoutes.equalCostPath[deletedNextHops].arpEntry.ipAddr =
        routeInfo->nextHops[i].nextHopIP;
      delRouteEntry.ecmpRoutes.numOfRoutes++;
      deletedNextHops++;
    }
  }
  /* notify if next hops have gone away but route is still valid 
   * i.e. updated route has some valid next hops left. If the route
   * is no longer valid (activeNextHops == 0 or the modified route
   * is a reject route with no next hops) then we will delete the
   * route and let that take care of notifications.
   */
  if ((deletedNextHops > 0) && ((activeNextHops > 0) ||
                       (routeEntry->flags & L7_RTF_REJECT)))
  {
    isChanged = L7_TRUE;
  }

  /********************************************************************
  * STAGE 3: notify if next hops on a best route have changed
  *********************************************************************/
  if(isChanged == L7_TRUE)
  {
    /* If the update is going to leave us with a route that has no
     * active next hops(and is not a reject route), give up and 
     * delete it now. Delete will take care of notifications as well.
     * We don't need to worry about what to do with newly added next hops,
     * because if there were any we would need to do this (delete).
     */
    if((activeNextHops == 0) && !(routeEntry->flags & L7_RTF_REJECT))
    {
      bzero((char *)&inactiveRouteEntry, sizeof(L7_routeEntry_t));
      inactiveRouteEntry.ipAddr      = osapiNtohl(pData->network.addr);
      inactiveRouteEntry.subnetMask  = osapiNtohl(pData->netmask.addr);
      inactiveRouteEntry.protocol    = routeInfo->protocol;
      inactiveRouteEntry.metric      = routeInfo->metric;

      /* release semaphore */
      if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");

      if(rtoRouteDelete(&inactiveRouteEntry) != L7_SUCCESS)
        LOG_MSG("Error: rto.c unable to delete route on modify\n");

      return L7_FAILURE;
    }

    /* update route info with the new set of next hops */
    routeInfo->numNextHops = updNextHops;
    memcpy(routeInfo->nextHops, updRouteInfo.nextHops, 
           sizeof(rtoNextHop_t) * L7_RT_MAX_EQUAL_COST_ROUTES);

    /* Organize the next hop entries in ascending order of ip addresses */
    rtoNextHopsSort(routeInfo->nextHops, routeInfo->numNextHops);

    /* We don't need a 
     * best route selection because modify can only change the metric 
     * and/or next hops - these don't affect the routes "best route" status
     * (unless none of the new set of hops is active in which case we
     * got delete to do what's required).
    */
    if(routeInfo->flags & RTO_BEST_ROUTE_NODE)
    {
      if(bestWasEcmp)
      {
            /* ecmp tracking */
            if(bestWasTrunc == L7_FALSE)
            {
               rtoEcmpRouteCount--;
            }
            else{
               routeInfo->flags &= ~RTO_ECMP_TRUNC;
                
            }
      }
      if(routeInfo->numNextHops > 1)
      {
            if(rtoEcmpRouteCount < rtoEcmpRouteMax)
            {
                rtoEcmpRouteCount++;
            }
            else{
                routeInfo->flags |= RTO_ECMP_TRUNC;
            }
      }
      if(rtoEcmpRouteCount == rtoEcmpRouteMax -1)
            rtoEcmpRetry();

      /* The best route has been modified, update the change list. */
      rtoChangeListMod(pData); 
    }
  }

  if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiWriteLockGive\n");

  /* Must update ARP gateway table after giving up the RTO lock. */

  /* Update ARP gw table with new active next hops. */
  if (addedNextHops > 0)
  {
    ipMapArpGwTableUpdate(&addRouteEntry, RTO_ADD_ROUTE);
  }

  /* notify ARP if next hops have gone away but route is still valid 
   * i.e. updated route has some valid next hops left. If the route
   * is no longer valid (activeNextHops == 0) then we will delete the
   * route and let that take care of notifications.
   */
  if ((deletedNextHops > 0 ) && ((activeNextHops > 0) ||
                                                (routeEntry->flags & L7_RTF_REJECT)))
  {
    ipMapArpGwTableUpdate(&delRouteEntry, RTO_DELETE_ROUTE);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Select new best route for a specific destination. 
*
* @param    pData    @b{(input)} head of the list of routes to a given destination
* @param    oldBestRouteInfo @b{(input)}  Previous best route. This routeInfo
*                                         object may or may not still be in 
*                                         the list of routes for the destination
* @param    forceNotify @b{(input)}  Notify registrants of the new best route, even
*                                    if the new best route is the same as the old
*                                    best route (because the attributes of the best
*                                    route changed).
*
* @notes    This function must be called after any change is made to the list of 
*           routes for a given destination if the change could affect which route
*           is the best route or if the change alters the current best route.
*
* @end
*********************************************************************/
void rtoSelectNewBestRoute(rtoRouteData_t *pData, rtoRouteInfo_t *oldBestRouteInfo, 
                           L7_BOOL forceNotify)
{
  rtoRouteInfo_t *newBestRouteInfo = L7_NULLPTR;  /* New Best route node
                                                     after re-election */

  if (pData == L7_NULLPTR)
    LOG_ERROR (0);

  /* Sort the existing routes to the network to elect the best route */
  pData->nextRouteInfo = rtoRouteInfoListSort(pData->nextRouteInfo);
  newBestRouteInfo = pData->nextRouteInfo;

  if ((newBestRouteInfo == L7_NULLPTR) || 
      (newBestRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0)
  {
      if (oldBestRouteInfo)
      {
	if(oldBestRouteInfo->numNextHops > 1)
        {
             /* ecmp tracking */
	     if((oldBestRouteInfo->flags & RTO_ECMP_TRUNC) == 0)
             {
                 rtoEcmpRouteCount--;
             }
             else{
                 oldBestRouteInfo->flags &= ~RTO_ECMP_TRUNC;
             }
        }
        if(rtoEcmpRouteCount == rtoEcmpRouteMax -1)
             rtoEcmpRetry();

        /* We had a best route but don't now. Notify clients of change. */
        rtoChangeListDel(pData);
        rtoStats.best_entries--;
        return;
      }
      /* Don't have a best route now and didn't have one before; no notification */
      return;
  }

  /* If the new best route is the same as the old best route then we are done. */
  if (!forceNotify && (newBestRouteInfo == oldBestRouteInfo) && 
      (newBestRouteInfo->flags & RTO_BEST_ROUTE_NODE))
    return;

  if (rtoTraceFlags & RTO_TRACE_BEST)
  {
      L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
      L7_uint32 maskLen = rtoMaskLength(osapiNtohl(pData->netmask.addr));
      L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(osapiNtohl(pData->network.addr), destAddrStr);    
      osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO selected %s route as best route to %s/%u", 
              rtoProtoNames[newBestRouteInfo->protocol], destAddrStr, maskLen);               
      rtoTraceWrite(traceBuf);
  }

  if (oldBestRouteInfo)
  {
    if(oldBestRouteInfo->numNextHops > 1)
    {
        /* ecmp tracking */
        if((oldBestRouteInfo->flags & RTO_ECMP_TRUNC) == 0)
        {
           rtoEcmpRouteCount--;
        }
        else{
           oldBestRouteInfo->flags &= ~RTO_ECMP_TRUNC;
        }
    }
    if(newBestRouteInfo->numNextHops > 1)
    {
        if(rtoEcmpRouteCount < rtoEcmpRouteMax)
        {
            rtoEcmpRouteCount++;
        }
        else{
            newBestRouteInfo->flags |= RTO_ECMP_TRUNC;
        }
    }
    if(rtoEcmpRouteCount == rtoEcmpRouteMax -1)
        rtoEcmpRetry();

    rtoChangeListMod(pData);
  }
  else
  {
    if(newBestRouteInfo->numNextHops > 1)
    {
        if(rtoEcmpRouteCount < rtoEcmpRouteMax)
        {
            rtoEcmpRouteCount++;
        }
        else{
            newBestRouteInfo->flags |= RTO_ECMP_TRUNC;
        }
    }
    rtoChangeListAdd(pData);
    rtoStats.best_entries++;
  }
}

/*********************************************************************
* @purpose  Sorts the linked list of routes for a network
*           on the basis of preference level.
*
* @param    head  @b{(input)} The first route in the linked list.
*
* @returns  The first route, after sorting the list. This is the new 
*           best route to the destination.
*
* @notes    This routine also flags the best route when done.
*
* @end
*********************************************************************/
rtoRouteInfo_t *rtoRouteInfoListSort(rtoRouteInfo_t *head)
{
  rtoRouteInfo_t *prev, *curr, *beforePrev, *tail = head;

  /* Set the best route flag to false before starting, the flag will
     later be set to true once the new best route has been elected */
  for (curr = head; curr != L7_NULLPTR; curr = curr->next)
      curr->flags &= ~(RTO_BEST_ROUTE_NODE);

  if(head)
  {
    while(tail->next)
    {
      curr = tail->next;

      if(rtoCompareRoutes(curr, head) >= 0)
      {
        tail->next = curr->next;
        curr->next = head;
        head = curr;
      }
      else
      {
        beforePrev = head;
        prev = head->next;

        while(rtoCompareRoutes(curr, prev) < 0)
        {
          beforePrev = prev;
          prev = prev->next;
        }

        if(prev == curr)
          tail = curr;
        else
        {
          tail->next = curr->next;
          curr->next = prev;
          beforePrev->next = curr;
        }
      }
    }
  }

  /* Only mark the head as a best route if its preference is less than 255 */
  if (head)
  {
    if (head->preference1 < L7_RTO_PREFERENCE_MAX)
      head->flags |= RTO_BEST_ROUTE_NODE;
  }

  return head;
}

/*********************************************************************
* @purpose  Sorts a route's next hops by next hop ip address.
*
* @param  @b{(input)}  array of next hops
* @param  @b{(input)}  number of next hops in the array
*
* @returns  none
*
* @notes    none.
*
* @end
*********************************************************************/
void rtoNextHopsSort(rtoNextHop_t A[], L7_uint32 N)
{
  L7_uint32 i, j;
  rtoNextHop_t tmp;

  for(i = 1; i < N; i++ )
  {
    memcpy(&tmp, &A[i], sizeof(rtoNextHop_t));

    for(j = i; (j>0 && rtoCompareNextHops(&tmp, &A[j-1]) < 0); j--)
      memcpy(&A[j], &A[j-1], sizeof(rtoNextHop_t));

    memcpy(&A[j], &tmp, sizeof(rtoNextHop_t));
  }
}

/*********************************************************************
* @purpose  Shifts elements in a route's array of next hops to the left 
*           starting at the index location, to file a hole left by a 
*           deleted element.
*
* @param    @b{(input)} index
* @param    @b{(input)} routeInfo node containing the array to shift
*
* @returns  L7_TRUE   The route is a duplicate
* @returns  L7_FALSE  This route is not a duplicate
*
* @notes
*
* @end
*********************************************************************/
void rtoNextHopsLeftShift(L7_uint32 index, rtoRouteInfo_t *routeInfo)
{
  L7_uint32 i;

  for(i = index; i < platRtrRouteMaxEqualCostEntriesGet(); i++)
  {
    memcpy((char *)(&routeInfo->nextHops[i]), (char *)(&routeInfo->nextHops[i+1]),
           sizeof(rtoNextHop_t));
  }

  /* zero the index at the end that has been vacated */
  bzero((char *)(&routeInfo->nextHops[routeInfo->numNextHops]), 
        sizeof(rtoNextHop_t));
}

/*********************************************************************
* @purpose  Checks if the specified nexthopIP already exists in
*           the array of next hops
*
* @param    A  @b{(input)} array of next hops
* @param    N  @b{(input)} number of next hops in the array
* @param    nextHopIP @b{(input)} next hop ip for comparison
*
* @returns  L7_TRUE   The route is a duplicate
* @returns  L7_FALSE  This route is not a duplicate
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL rtoIsDuplicateNextHop(rtoNextHop_t A[], L7_uint32 N,
                              L7_uint32 nextHopIP)
{
  L7_uint32 i;

  for(i = 0; i < N; i++)
  {
    if(A[i].nextHopIP == nextHopIP)
      return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Compare the preference of two routes.
*
* @param  firstRouteInfo @b{(input)}  Pointer to a rtoRouteInfo_t structure.
* @param  secondRouteInfo @b{(input)}  Pointer to a rtoRouteInfo_t structure.
*
* @returns 1  - First route is preferred over the second route.
* @returns 0  - Routes are equal.
* @returns -1  - Second route is preferred over the first route.
*
* @notes     
*********************************************************************/
L7_int32 rtoCompareRoutes(rtoRouteInfo_t *firstRouteInfo,
                          rtoRouteInfo_t *secondRouteInfo)
{
  return rtoComparePreferences(firstRouteInfo, secondRouteInfo);
}

/*********************************************************************
* @purpose  Compare the preference value of two routes.
*
* @param  firstRouteInfo @b{(input)}  Pointer to a rtoRouteInfo_t structure.
* @param  secondRouteInfo @b{(input)}  Pointer to a rtoRouteInfo_t structure.
*
* @returns 1  - First route preferred to the second route.
* @returns 0  - Routes are equal.
* @returns -1  - Second route is preferred to the first route.
*
* @notes  The route with the lower preference value is considered the
*         more preferred route.
*
* @notes  We do not currently allow the user to configure preference2.
*         preference2 is set to a low value for static and default routes
*         and to a high value for other routes so that configured routes
*         are preferred to dynamic routes if they have the same preference1.
*
*********************************************************************/
L7_int32 rtoComparePreferences(rtoRouteInfo_t *firstRouteInfo,
                               rtoRouteInfo_t *secondRouteInfo)
{
  if(firstRouteInfo->preference1 < secondRouteInfo->preference1)
    return 1;

  if(firstRouteInfo->preference1 > secondRouteInfo->preference1)
    return -1;

  if(firstRouteInfo->preference2 < secondRouteInfo->preference2)
    return 1;

  if(firstRouteInfo->preference2 > secondRouteInfo->preference2)
    return -1;

  /* Routes are equal! */
  return 0;
}

/*********************************************************************
* @purpose  Compare two next hops.
*
* @param  firstRouteInfo @b{(input)}  first next hop
* @param  secondRouteInfo @b{(input)}  second next hop
*
* @returns 1  - First next hop is higher than second route.
* @returns 0  - Routes are equal.
* @returns -1  - Second next hop is higher than first route.
*
* @notes  A next hop whose interface is up is preferred to a next hop
*         whose interface is down. Among up interfaces, sort numerically
*         on the next hop IP address.
*
*********************************************************************/
L7_int32 rtoCompareNextHops (rtoNextHop_t * firstNextHop,
                             rtoNextHop_t * secondNextHop)
{
  if(firstNextHop->ifUp == L7_FALSE && secondNextHop->ifUp == L7_FALSE)
    return 0;

  if(firstNextHop->ifUp == L7_FALSE)
    return -1;

  if(secondNextHop->ifUp == L7_FALSE)
    return 1;

  if(firstNextHop->nextHopIP > secondNextHop->nextHopIP)
    return 1;


  if(firstNextHop->nextHopIP < secondNextHop->nextHopIP)
    return -1;

  /* nextHopIP are same. Compare interfaces */
  if(firstNextHop->intIfNum > secondNextHop->intIfNum)
    return 1;


  if(firstNextHop->intIfNum < secondNextHop->intIfNum)
    return -1;

  /* Routes are equal!
  */
  return 0;
}

/*********************************************************************
* @purpose  Delete a specific entry from the routing table
*
* @param    pData - network node which contains the route to be deleted
* @param    routeInfo - route to be deleted. 
*
* @notes    routeInfo must already be removed from pData's list of routes.
*
*           This function returns the routeInfo structure to the free list.
*
*           If the route being deleted was a best route, notifies registrants
*           of the deletion and selects a new best route. If the deleted route
*           was the only route to the destination but was not a best route,
*           the radix node is deleted immediately.
*
* @end
*********************************************************************/
void rtoCleanupAfterDelete (rtoRouteData_t *pData, rtoRouteInfo_t *routeInfo)
{
  L7_BOOL bestRouteDeleted = (routeInfo->flags & RTO_BEST_ROUTE_NODE);

  if (bestRouteDeleted == L7_TRUE)
  {
    /* If we ended up deleting the best route then we need to elect a
     * a new best route and inform all registered users.  */
    rtoSelectNewBestRoute(pData, routeInfo, L7_FALSE);
  }

  if (routeInfo->protocol == RTO_LOCAL)
    rtoStats.active_locals--;
  /* Return freed route info entry to the FREE list. */
  routeInfo->next = rtoFreeRouteInfoList;
  rtoFreeRouteInfoList = routeInfo;
  rtoStats.route_entries--;

  /* If the deleted route was the only route to the destination, but 
   * it was not a best route (pref was 255), then remove the node from
   * the radix tree. */
  if ((pData->nextRouteInfo == NULL) && !bestRouteDeleted && 
      (pData->changeType != RTO_RT_DEL))
  {
    /* This node should not be linked into the change list. If it is, we're 
     * in trouble. */
    if (pData->nextChange || pData->prevChange)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
              "Deleting radix entry still linked into the change list.");
    }
    pData = radixDeleteEntry(&rtoRouteTreeData, pData);
    if (pData == L7_NULLPTR)
      LOG_ERROR (0);
    rtoStats.radix_entries--;
  }
}

/*********************************************************************
* @purpose  Copy the attributes of a route from a rtoRouteInfo_t structure to a 
*           L7_routeEntry_t structure.
*
* @param    routeInfo    (input) source route
* @param    routeEntry   (output) target route
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Since rtoRouteInfo_t does not specify the destination address and
*           mask (they are on the rtoRouteData_t at the head of the list), 
*           this does not set the ipAddr and subnetMask fields on routeEntry.
*
*           Only copies the next hops whose outgoing interface is up.
*
* @end
*********************************************************************/
static L7_RC_t rtoRouteCopy(rtoRouteInfo_t *routeInfo, 
                            L7_routeEntry_t *routeEntry)
{
    L7_uint32 i, index;

    if (!routeInfo || !routeEntry)
        return L7_FAILURE;

    if (routeInfo->numNextHops > L7_RT_MAX_EQUAL_COST_ROUTES)
    {
      /* This would indicate a serious bug */
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
              "rtoRouteCopy() asked to copy bad routeInfo object with numNextHops %u.",
              routeInfo->numNextHops);
      return L7_FAILURE;
    }

    routeEntry->protocol = routeInfo->protocol;
    routeEntry->pref = routeInfo->preference1;   
    routeEntry->metric = routeInfo->metric;
    routeEntry->flags = 0;
    if(routeInfo->flags & RTO_REJECT_ROUTE)
       routeEntry->flags |= L7_RTF_REJECT;
    /*Specifies the last time the route was updated (in hours:minutes:seconds)*/ 
    routeEntry->updateTime=routeInfo->updateTime;
    routeEntry->ecmpRoutes.numOfRoutes = routeInfo->numNextHops;
    if(routeInfo->flags & RTO_ECMP_TRUNC)
        routeEntry->ecmpRoutes.numOfRoutes = 1;

    /* copy each next hop */
    for (i = 0, index = 0; i < routeInfo->numNextHops; i++)
    {
        if (routeInfo->nextHops[i].ifUp == L7_TRUE)
        {
            routeEntry->ecmpRoutes.equalCostPath[index].arpEntry.ipAddr = 
                routeInfo->nextHops[i].nextHopIP;

            routeEntry->ecmpRoutes.equalCostPath[index].arpEntry.intIfNum = 
                routeInfo->nextHops[i].intIfNum;

            index++;
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determines if the route table is full.
*
* @returns  L7_TRUE   If the table is full
* @returns  L7_FALSE  If the table is not full
*
* @notes    Internal utility function.  The calling function is responsible
*           for acquiring the route table semaphore.
*
* @end
*********************************************************************/
L7_BOOL rtoIsFull (void)
{
  /* Number of IPv4 address RTO needs to reserve space for. */
  L7_uint32 open_locals = rtoStats.reserved_locals - rtoStats.active_locals;

  if ((rtoStats.best_entries + open_locals) >= RtoMaxRoutes)
      return L7_TRUE;

  /* ROBRICE - verify that route info object available */

  if (rtoFreeRouteInfoList == L7_NULLPTR)
      /* exceeded max for all routes */
      return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Returns the number of routes in the routing table. 
*
* @param    bestRoutesOnly  @b{(input)} If set to L7_TRUE, only best 
*                                       routes are counted.
*
* @returns  the number of routes
*
* @notes    does not include inactive routes or deleted routes pending
*           client notification
*
* @end
*********************************************************************/
L7_uint32 rtoRouteCount(L7_BOOL bestRoutesOnly)
{
  L7_uint32 cnt;

  if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

  if (bestRoutesOnly)
    cnt = rtoStats.best_entries;
  else
    cnt = rtoStats.route_entries;

  osapiReadLockGive(rtoRwLock);
  return cnt;
}

/*********************************************************************
* @purpose  Reserves a route in the route tables database.
*
* @returns  L7_SUCCESS        If successful
* @returns  L7_FAILURE        If the rto semaphore could not be taken
* @returns  L7_TABLE_IS_FULL  If there are no free entries in the table
*
* @notes    This function was added to account for a pending add to the  
*           route table.  Following is an example of when to use this:
*           Consider a situation where the route table is almost at max 
*           capacity, and a user is trying to create a new routing 
*           interface while new routes are simultaneously being learned
*           from the routing protocols.  Before the interface creation 
*           is actually committed the code checks if there is space in 
*           the route table for adding the new local route generated as 
*           a result of the interface creation.  If there exists one, 
*           this is reserved.  This guarantees that once the interface 
*           has been created there will definitely be space available 
*           in the route table for the local route.
*
*           Remember to unreserve an entry that you have previously 
*           reserved if you do not intend to use it.                
* @end
**********************************************************************/
L7_RC_t rtoRouteReserve (void)
{
  if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return L7_FAILURE;

  if(rtoIsFull() == L7_TRUE)
  {
    if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return L7_TABLE_IS_FULL;
  }

  rtoStats.reserved_locals++;

  if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiWriteLockGive\n");

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Un-reserves a route in the route tables database.
*
* @returns  L7_SUCCESS        If successful
* @returns  L7_FAILURE        If the rto semaphore could not be taken
* @returns  L7_TABLE_IS_FULL  If there are no free entries in the table
*
* @notes    Remember to unreserve an entry that you have previously   
*           reserved if you do not intend to use it.
*
*           This mechanism isn't quite perfect. rtoRouteReserve() gets
*           called when the IP circuit is created. This is done on 
*           the interface create event normally. Certain configuration
*           actions or interface events can prevent the creation of a 
*           local route or cause the deletion of a local route, even
*           though the IP circuit remains. When a change happens to 
*           allow the local route to be recreated, there is no need to
*           recreate the corresponding IP circuit, which still exists.
*           Thus, rtoRouteReserve() may not always be called for every
*           call to rtoRouteUnReserve(). Therefore, we only decrement
*           pending_adds if pending_adds is > 0 and don't log an error
*           if pending_adds is <= 0. The result is that in some cases
*           we add a local route without first reserving it.  
*         
* @end
*********************************************************************/
L7_RC_t rtoRouteUnReserve (void)
{
  if (osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return L7_FAILURE;

  if(rtoStats.reserved_locals > 0)
  {
      rtoStats.reserved_locals--;
  }

  if (osapiWriteLockGive(rtoRwLock) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiWriteLockGive\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the given destination prefix/mask is a valid
*           destination for a unicast route. 
*
* @param    destAddr - a network prefix
* @param    mask - associated network mask
*
* @returns  L7_TRUE if valid
* @returns  L7_FALSE otherwise
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL rtoValidDestAddr(L7_uint32 destAddr, L7_uint32 mask) 
{
    /* make sure host bits are zeroed */
    L7_uint32 prefix = destAddr & mask;

    L7_uint32 inverseMask = ~mask;
    
    /* verify that destAddr does not have any host bits set. */ 
    if ((destAddr & inverseMask) != 0)
        return L7_FALSE;
         
    /* verify that mask is contiguous */
    if ((inverseMask & (inverseMask + 1)) != 0)
        return L7_FALSE;    

    /* If either destAddr or mask is 0, require them both to be 0 */ 
    if (((destAddr == 0) && (mask != 0)) ||
        ((destAddr != 0) && (mask == 0)))
        return L7_FALSE;

    /* First valid prefix is 1.0.0.0 */
    if ((prefix != 0) && (prefix < 0x01000000UL))
        return L7_FALSE;
    if ((prefix & 0xE0000000) == 0xE0000000) {
        /* don't allow class D or class E addresses */
        return L7_FALSE;
    }
    if ((prefix & 0xFF000000) == 0x7F000000) {
        /* suppress 127.0.0.0 network */
        return L7_FALSE;
    }
    return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine the length of a network mask. A mask's length
*           is the number of 1s bits set. 
*
* @param    mask - network mask
*
* @returns  Length of the mask.
*
* @notes    Function assumes the mask is contiguous.
*       
* @end
*********************************************************************/
L7_uint32 rtoMaskLength(L7_uint32 mask)
{
    L7_uint32 length = 0;
    L7_uint32 b = 0x80000000;
    while (mask & b) {
        length++;
        b = b >> 1;
    }
    return length;
}

/*********************************************************************
* @purpose  Find the route with the longest matching prefix to a 
*           given destination. 
*
* @param    dest_ip  @b{(input)}  Destination IP address.
* @param    route    @b{(output)} best route to the destination
* @param    acceptRejectRoute @b{(input)} whether to accept reject route
*                                         as a best match
*
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_SUCCESS  Router found
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t rtoBestRouteLookup (L7_uint32 dest_ip, L7_routeEntry_t *route,
                            L7_BOOL acceptRejectRoute)
{ 
    rtoRouteData_t *pData = L7_NULLPTR;
    L7_uint32 skipentries = 0;
    rtoTreeKey_t key;
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32 logRate = 200;     /* log every 200th bad lookup. count them all. */

    /* Bad things happen if we do a radix lookup for an address matches
     * a mask node. So avoid 224 and above. */
    if (dest_ip >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    {
      if ((rtoStats.bad_lookups % logRate) == 0)
      {
        L7_uchar8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(dest_ip, destAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                "Route lookup to invalid destination IP address %s.",
                destAddrStr);
      }
      rtoStats.bad_lookups++;
      return L7_FAILURE;
    }

    memset(route, 0, sizeof(L7_routeEntry_t));

    if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    {
        return L7_FAILURE;
    }

    rtoSetKey(&key, dest_ip);

    /* find the longest match */
    do
    {
        pData = radixMatchNode(&rtoRouteTreeData, &key, skipentries);

        /* deleted routes pending notification have no nextRouteInfo. Skip them. 
         * Also skip destinations with only inactive routes. */
        if (pData && (!pData->nextRouteInfo || 
                      ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0)))
            skipentries++;   
        else
            break; /* no match, or best match found! */
    } 
    while(L7_NULLPTR != pData);
      
    if (pData == L7_NULLPTR)
    {
        if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");
        return(L7_FAILURE);
    }

    if (!pData->nextRouteInfo)
        LOG_ERROR (dest_ip); /* radix entry must always have a list of IP addresses */

    if((acceptRejectRoute == L7_FALSE) &&
       (pData->nextRouteInfo->flags & RTO_REJECT_ROUTE))
    {
        /* caller of this function doesn't want the best matching reject route */
        rc = L7_FAILURE;
    }
    else
    {
        route->ipAddr = osapiNtohl(pData->network.addr);
        route->subnetMask = osapiNtohl(pData->netmask.addr);
        if (rtoRouteCopy(pData->nextRouteInfo, route) != L7_SUCCESS)
        {
            rc = L7_FAILURE;
        }
    }

    if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return rc;
}

/*********************************************************************
* @purpose  Find the list of routes to a specific prefix.
*
* @param    destPrefix  @b{(input)}  Destination IP prefix.
* @param    destMask    @b{(input)}  Destination mask.
*
* @returns  pointer to the head of the list, if found
*
* @notes    This is an internal utility function and therefore does not
*           take the RTO semaphore.
*
* @end
*********************************************************************/
static rtoRouteData_t *
rtoRouteDataFind(L7_uint32 destPrefix, L7_uint32 destMask)
{
    rtoTreeKey_t network;
    rtoTreeKey_t netmask;

    /* Bad things happen if we do a radix lookup for an address matches
     * a mask node. So avoid 224 and above. */
    if (destPrefix >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    {
      /* don't increment bad_lookups. This is generally a configuration action. */
      return NULL;
    }
    
    rtoSetKey(&network, (destPrefix & destMask)); /* just in case host bits aren't zeroed */
    rtoSetKey(&netmask, destMask);

    return (rtoRouteData_t*) radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);
}

/*********************************************************************
* @purpose  Find the route to a specific prefix with a specific preference
*           from a specific protocol.
*
* @param    destPrefix  @b{(input)}  Destination IP prefix.
* @param    destMask    @b{(input)}  Destination mask.
* @param    pref        @b{(input)}  route preference
* @param    protocol    @b{(input)}  protocol that provided the route
*
* @returns  pointer to the route if found
*
* @notes    This is an internal utility function and therefore does not
*           take the RTO semaphore.
*
*           Two routes to the same destination from different protocols
*           can have the same preference. The user can specify the 
*           preference of individual static routes. The preference of
*           a static route can be the same as the preference of routes
*           learned by a dynamic routing protocol. Thus, we have to 
*           check both preference and protocol here.
*
* @end
*********************************************************************/
static rtoRouteInfo_t *
rtoRouteInfoPrefFind(L7_uint32 destPrefix, L7_uint32 destMask,
                     L7_uint32 pref, L7_RTO_PROTOCOL_INDICES_t protocol)
{
    rtoRouteInfo_t *routeInfo;
    rtoRouteData_t *pData = rtoRouteDataFind(destPrefix, destMask);
    if (pData == L7_NULLPTR)
    {
        return L7_NULLPTR;
    }
    /* iterate through routes to this destination looking for a matching
     * preference and protocol. */
    routeInfo = pData->nextRouteInfo;
    while (routeInfo)
    {
        if ((routeInfo->protocol == protocol) &&
            (routeInfo->preference1 == pref))
        {
            return routeInfo;
        }
        routeInfo = routeInfo->next;
    }
    return L7_NULLPTR;
}


/*********************************************************************
* @purpose  Find the best route to a specific prefix.
*
* @param    destPrefix  @b{(input)}  Destination IP prefix.
* @param    destMask    @b{(input)}  Destination mask.
* @param    route    @b{(output)} best route to the prefix
*
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_SUCCESS  Router found
* @returns  L7_ERROR    If RTO is not initialized
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t rtoPrefixFind(L7_uint32 destPrefix, L7_uint32 destMask, 
                      L7_routeEntry_t *route)
{
    rtoRouteData_t *pData = L7_NULLPTR;
    rtoTreeKey_t network;
    rtoTreeKey_t netmask;

    L7_RC_t rc = L7_SUCCESS;

    /* Bad things happen if we do a radix lookup for an address matches
     * a mask node. So avoid 224 and above. */
    if (destPrefix >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    {
      /* don't increment bad_lookups */
      return L7_FAILURE;
    }

    if (route == L7_NULLPTR)
        return L7_FAILURE;

    if (rtoInitialized != L7_TRUE)
        return L7_ERROR;

    if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    {
        return L7_FAILURE;
    }
    
    rtoSetKey(&network, (destPrefix & destMask));  /* just in case host bits aren't zeroed */
    rtoSetKey(&netmask, destMask);

    pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);
    if ((pData == L7_NULLPTR) || !pData->nextRouteInfo || 
        ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0))
    {
        if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");
        return L7_FAILURE;
    }

    route->ipAddr = osapiNtohl(pData->network.addr);
    route->subnetMask = osapiNtohl(pData->netmask.addr);
    if (rtoRouteCopy(pData->nextRouteInfo, route) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }

    if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return rc;
}

/*********************************************************************
* @purpose  Search for the route that is same as the route formed by 
*           the parameters.
*
* @param    ipAddr      Destination IP address.
* @param    netMask    	subnet mask of the outgoing interface.
* @param    gateway    	(output) Next hop ip address
*
*
* @returns  L7_FAILURE 	Route not found
* @returns  L7_ERROR    RTO not initialized
* @returns  L7_SUCCESS  Route found
*
* @notes    This function only returns the first next hop in the matching
*           route. Callers should be aware that there may be other next
*           hops in the route. Use of this function is discouraged.
*
* @end
*********************************************************************/
L7_RC_t rtoBestRouteLookupExact(L7_uint32 ipAddr, L7_uint32 netMask, L7_uint32 *gateway)
{
  rtoRouteData_t *pData = 0;
  rtoTreeKey_t network;
  rtoTreeKey_t netmask;

  if (rtoInitialized != L7_TRUE)
    return L7_ERROR;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
  {
    /* don't increment bad_lookups */
    return L7_FAILURE;
  }

  rtoSetKey(&network, (ipAddr & netMask));  /* just in case host bits aren't zeroed */
  rtoSetKey(&netmask, netMask);

  /* Find the next network given the input network.
  */
  if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);

  /* Don't return a deleted route or an inactive route or a reject route */
  if ((pData == L7_NULLPTR) || !pData->nextRouteInfo || 
      ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0) ||
      (pData->nextRouteInfo->flags & RTO_REJECT_ROUTE))
  {
    osapiReadLockGive(rtoRwLock);
    return L7_FAILURE;
  }

  if (gateway != L7_NULLPTR)
    *gateway = pData->nextRouteInfo->nextHops[0].nextHopIP;

  osapiReadLockGive(rtoRwLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the first route in the routing table.
*
* @param    lookupType    specifies the lookup type can be L7_RN_GETNEXT
*                         or L7_RN_GETNEXT_ASCEND
*
* @returns  pointer to the set of routes to the first destination
*           in the routing table.
*
* @notes    Typically used to initiate a walk of the routing table.
*           
*           L7_RN_GETNEXT returns the longest prefix for the first route
*
*           1.0.0.0/32 <-- first item returned
*           1.0.0.0/24
*           1.0.0.0/8
*           
*           L7_RN_GETNEXT_ASCEND shortest prefix for the first route
*
*           1.0.0.0/8  <-- first item returned
*           1.0.0.0/24
*           1.0.0.0/32
*
*           The former is more efficient (radix is optimized for LPM), but 
*           the latter makes sense for clients that expect a monotonically
*           increasing key (ipaddr,mask) viz. SNMP.
*
* @end
*********************************************************************/
rtoRouteData_t *rtoFirstRouteGet(L7_uint32 lookupType)
{
    rtoRouteData_t *pData;
    rtoTreeKey_t network, netmask;

    /* must pass a valid lookup type */
    if(lookupType != L7_RN_GETNEXT && lookupType != L7_RN_GETNEXT_ASCEND)
    {
      LOG_ERROR(lookupType);
      return L7_NULLPTR;
    }

    /* Get default route, if one exists. */
    rtoSetKey(&network, 0);
    rtoSetKey(&netmask, 0);
    pData = radixLookupNode(&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);

    /* No exact match. Lookup next */
    if (pData == L7_NULLPTR)
    {
      pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, lookupType);
    }

    return pData;
}

/*********************************************************************
* @purpose  Return next best route in the routing table. 
*
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    In order to look up the first route all fields of the next_route 
*           structure must be set to zero. In order to look up subsequent 
*           routes use previous output as input.
*
*           To accommodate the default route (next_route->network == 0), use
*           the protocol field to differentiate between the first and subsequent
*           reference to network number 0 such that the appropriate 'next'
*           route is returned.  Assume that for the first invocation, both
*           network and protocol are 0.  If a default route exists, it is returned
*           along with a non-zero protocol value, such as RTO_DEFAULT.  Since the
*           usage rule for finding subsequent routes is to provide "previous
*           output as input", a subsequent access for network 0 will have a
*           non-zero protocol field value.
*
*           If the default route does not exist when looking up the first route,
*           the first non-default best route is returned.
*
*           The route returned includes all active next hops in the best route
*           to the destination.
*
*           RADIX_USAGE_NOTE:
*           Since this function iterates routes that could potentially go to SNMP,
*           it uses GET_NEXT_ASCENDING lookups to ensure that routes are returned
*           in the correct order i.e. monotonically ascending key, mask values 
*           (Instead of GET_NEXT/radixGetNextEntry which would be more efficient).
*
* @end
*********************************************************************/
L7_RC_t rtoNextBestRouteGet (L7_routeEntry_t *nextRoute)
{
  rtoRouteData_t *pData = L7_NULLPTR;
  rtoTreeKey_t network, netmask;

  rtoSetKey(&network, 0);
  rtoSetKey(&netmask, 0);

  if ((nextRoute->ipAddr == 0) && (nextRoute->protocol == 0))
      /* Get default route, if one exists. */
    pData = radixLookupNode(&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);

  if ((pData == L7_NULLPTR) || !pData->nextRouteInfo || 
      ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0))
  {
    /* No exact match. Lookup next (must lookup as current ip may not be in tree). */
    rtoSetKey(&network, nextRoute->ipAddr);
    rtoSetKey(&netmask, nextRoute->subnetMask);
    /* see radix usage note above */
    pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_GETNEXT_ASCEND);
    while ((pData != L7_NULLPTR) && 
           (!pData->nextRouteInfo || 
            ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0)))
    {
        /* skip destinations which have been deleted or for which there 
         * is no best route. */
        memcpy(&network, &pData->network, sizeof(rtoTreeKey_t));
        memcpy(&netmask, &pData->netmask, sizeof(rtoTreeKey_t));
        pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_GETNEXT_ASCEND);
    }   
  }

  if (!pData || !pData->nextRouteInfo || 
      ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0))
  {
      /* No more best routes */
      return L7_ERROR;
  }

  /* The next best route is the first ecmp route in the first route node
     in the network entry */
  nextRoute->ipAddr = osapiNtohl(pData->network.addr);
  nextRoute->subnetMask = osapiNtohl(pData->netmask.addr);
  return rtoRouteCopy(pData->nextRouteInfo, nextRoute);
}

/*********************************************************************
* @purpose  Return next route in the routing table. 
*
* @param    inOutRoute @b{(input/output)} Set to the previous route on 
*            input and set to the next route on output.
* @param    bestRouteOnly @b{(input)}  set to L7_TRUE if the user is 
*                                        interested only in best routes.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    This API can be used to walk the routing table, getting either
*           all routes or only best routes. In order to look up the
*           first route all fields of the inOutRoute structure must be
*           set to zero. In order to look up subsequent routes use previous
*           output as input.
*
*           The route returned includes all active next hops in the next route.
*
*           When searching the routing table for the input route, a route 
*           is considered a match if it has the same destination address and
*           mask, the same protocol, and the same preference. 
*
*           Assumes the host bits have been zeroed in inOutRoute->ipAddr.
*
*           Skips inactive routes and deleted routes that are hanging around
*           pending client notification.
*
*
*           RADIX_USAGE_NOTE:
*           Since this function iterates routes that could potentially go to SNMP,
*           it uses GET_NEXT_ASCENDING lookups to ensure that routes are returned
*           in the correct order i.e. monotonically ascending key, mask values 
*           (Instead of GET_NEXT/radixGetNextEntry which would be more efficient).
*
* @end
*********************************************************************/
L7_RC_t rtoNextRouteGet(L7_routeEntry_t *inOutRoute, L7_BOOL bestRouteOnly)
{
    /* tree key */
    rtoTreeKey_t network, netmask;

    /* Head of set of routes to the next destination */
    rtoRouteData_t *pData = L7_NULLPTR;

    /* Indicates whether we found the input route */
    L7_BOOL inputRouteFound = L7_FALSE;

    /* The route following the input route for in the routing table. */
    rtoRouteInfo_t *nextRouteInfo = L7_NULLPTR;

    /* A route in the routing table. */
    rtoRouteInfo_t *routeInfo;

    /* the protcol that supplied the input route */
    L7_RTO_PROTOCOL_INDICES_t inputProtocol = inOutRoute->protocol; 

    /* the preference of the input route */
    L7_uchar8 inputPref = inOutRoute->pref;

    L7_RC_t rc = L7_SUCCESS;

    /* Bad things happen if we do a radix lookup for an address matches
     * a mask node. So avoid 224 and above. */
    if (inOutRoute->ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    {
      /* don't increment bad lookups */
      return L7_FAILURE;
    }

    /* setup tree key */
    rtoSetKey(&network, inOutRoute->ipAddr);
    rtoSetKey(&netmask, inOutRoute->subnetMask);

    /* Grab a semaphore while manipulating the route table. We need to do this
     * because the code is reentrant. */
    if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        return L7_FAILURE;

    if (bestRouteOnly == L7_TRUE)
    {
        rc = rtoNextBestRouteGet(inOutRoute);

        if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");

        return rc;
    }

    /* We are walking all the routes, not just best routes. The input route may 
     * be one of a set of routes to a single destination. If so, we have to find 
     * that set of routes and then find the input route within that set. So ask 
     * for an exact match to the input route. Could also be looking for the very
     * first route (inOutRoute is all zeros). */
    pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_EXACT);
    if ((pData == L7_NULLPTR) || !pData->nextRouteInfo)
    {
        /* If there is no exact match for the network then this is either the first
         * lookup or the routing table no longer contains any routes to the given
         * destination. Try to find the next network.
         *
         * Also see radix usage note above for L7_RN_GETNEXT_ASCEND.
         */
        pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_GETNEXT_ASCEND);
        while (pData && !pData->nextRouteInfo)
        {
          /* skip destinations whose only routes are delete pending */
          memcpy(&network, &pData->network, sizeof(rtoTreeKey_t));
          memcpy(&netmask, &pData->netmask, sizeof(rtoTreeKey_t));
          pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_GETNEXT_ASCEND);
        }   
    }
    else
    {
        inputRouteFound = L7_TRUE;
    }

    if (!pData || !pData->nextRouteInfo)
    {
        /* Couldn't find input route or any route following it. */
        if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");

        return L7_ERROR;
    }

    /* inputProtocol of 0 means caller wants the first route in the table */
    if ((inputProtocol == 0) || (inputRouteFound == L7_FALSE))
    {
        /* We don't know the previous route, so use the first (best) route to 
         * the next destination. */
        nextRouteInfo = pData->nextRouteInfo;
    }
    else
    {
        /* We have one or more routes to the destination of the input route. 
         * See if one of them matches the input route. It's a match if it
         * has the same source protocol and preference. */
        routeInfo = pData->nextRouteInfo;
        while ((routeInfo != L7_NULLPTR) && (nextRouteInfo == L7_NULLPTR))
        {
            if ((routeInfo->protocol == inputProtocol) && 
                (routeInfo->preference1 == inputPref))
            {
                /* found the input route */
                nextRouteInfo = routeInfo->next;
            }
            routeInfo = routeInfo->next;
        }
        if (nextRouteInfo == L7_NULLPTR)
        {
            /* Either we found the input route and it was the last route to that
             * destination, or we didn't find the input route. In either case, go
             * to the next destination and report the best route to that dest. */
            memcpy(&network, &pData->network, sizeof(rtoTreeKey_t));
            memcpy(&netmask, &pData->netmask, sizeof(rtoTreeKey_t));
            pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, L7_RN_GETNEXT_ASCEND);
            if (pData != L7_NULLPTR)
            {
                nextRouteInfo = pData->nextRouteInfo;
            }
        }
    }

    if ((pData == L7_NULLPTR) || (nextRouteInfo == L7_NULLPTR))
    {
        /* Did not find next route */
        if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiWriteLockGive\n");
        return L7_ERROR;
    }

    inOutRoute->ipAddr = osapiNtohl(pData->network.addr);
    inOutRoute->subnetMask = osapiNtohl(pData->netmask.addr);
    if (rtoRouteCopy(nextRouteInfo, inOutRoute) != L7_SUCCESS)
        rc = L7_ERROR;

    if (osapiReadLockGive(rtoRwLock) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiWriteLockGive\n");

    return rc;
}

/*********************************************************************
* @purpose  Return route from the routing table. 
*
* @param    inOutRoute @b{(input/output)} 
*
* @returns  L7_SUCCESS  Route found.
* @returns  L7_FAILURE  Route not found.
*
* @notes    This API can be used to find a route in the routing table.
*
*           The route returned includes all active next hops in the next route.
*
*           When searching the routing table for the input route, a route 
*           is considered a match if it has the same destination address and
*           mask, the same protocol, and the same preference. 
*
*           Assumes the host bits have been zeroed in inOutRoute->ipAddr.
*
* @end
*********************************************************************/
L7_RC_t rtoRouteGet (L7_routeEntry_t *inOutRoute)
{
  rtoRouteInfo_t *routeInfo;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == inOutRoute)
    return L7_FAILURE;

  if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      return L7_FAILURE;

  routeInfo = rtoRouteInfoPrefFind(inOutRoute->ipAddr, inOutRoute->subnetMask,
                     inOutRoute->pref, inOutRoute->protocol);

  if(L7_NULLPTR != routeInfo)
  {
    rc = rtoRouteCopy(routeInfo, inOutRoute);
  }

  osapiReadLockGive(rtoRwLock);
  return rc;
}

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg @b{(input)} A string to be displayed as a trace message.
*
* @notes    The input string must be no longer than RTO_TRACE_LEN_MAX
*           characters. This function will prepend the message with the 
*           current time and will insert a new line character at the 
*           beginning of the message.
*
* @end
*********************************************************************/
void rtoTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint32 msecSinceBoot;
    L7_uint32 msecs;
    L7_uint32 secSinceBoot;
    L7_uchar8 debugMsg[512];

    if (traceMsg == NULL)
        return;

    msecSinceBoot = osapiTimeMillisecondsGet();
    secSinceBoot = msecSinceBoot / 1000;
    msecs = msecSinceBoot % 1000;

    /* For now, just print the message with a timestamp. */
    osapiSnprintf(debugMsg, 512, "\n%u.%03u:  ", secSinceBoot, msecs);
    strncat(debugMsg, traceMsg, RTO_TRACE_LEN_MAX);
    printf(debugMsg);
}

/*********************************************************************
* @purpose  sets up a key for the RTO radix tree
*
* @param    rtoTreeKey_t @b{(output)} A pointer to the key
* @param    addr         @b{(input)} Key value.
*
* @notes    Sets the 1st byte to the length of the key, initializes
*           pad bytes and sets the key bytes to addr.
*
* @end
*********************************************************************/
void rtoSetKey(rtoTreeKey_t * p_key, L7_uint32 addr)
{
  if(addr)
  {
    *((L7_int32 *)((p_key)->reserved)) = -1;  /* ffff out pad bytes */
    (p_key)->reserved[0] = sizeof(rtoTreeKey_t);  /* set 1st byte as length */
    (p_key)->addr = osapiHtonl(addr); /* set address: must be net byte order */
  }
  else
  {
    /* zero out */
    memset(p_key, 0, sizeof(rtoTreeKey_t));
  }
}

/*********************************************************************
* @purpose  look for an ecmp route that is not active and activate it.
*
* @param    none
*
*
* @end
*********************************************************************/
static void rtoEcmpRetry(void)
{
    rtoRouteData_t *pData, *pDataNext;
    rtoRouteInfo_t *routeInfo = L7_NULL;

    if(rtoEcmpRouteCount >= rtoEcmpRouteMax)
        return;

    pData = rtoFirstRouteGet(L7_RN_GETNEXT);

    /* iterate through all destinations */
    while (pData)
    {
        routeInfo = pData->nextRouteInfo;

        /* only best routes */
        if((routeInfo != L7_NULL) && (routeInfo->numNextHops > 1)
            && (routeInfo->flags & RTO_ECMP_TRUNC))
            break;

        /* cache next item */
        pDataNext = radixGetNextEntry(&rtoRouteTreeData, pData);
        pData = pDataNext;
    }
    if(pData && routeInfo)
    {
      /* install full ecmp route */
      routeInfo->flags &= ~RTO_ECMP_TRUNC;
      rtoEcmpRouteCount++;
      rtoChangeListMod(pData); 
    }
}

/*********************************************************************
* @purpose  Determine whether a given next hop IP address is on a local 
*           subnet. If so, return the internal interface number of the
*           interface to reach the next hop.
*
* @param    arpEntry    @b{(input)}  ARP entry for next hop
* @param    intIfNum    @b{(output)} internal interface number of outgoing interface
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t rtoRouterIfResolve(L7_arpEntry_t *arpEntry, L7_uint32 *intIfNum)
{
    rtoRouteData_t *pData = L7_NULLPTR;
    L7_uint32 skipentries = 0;
    rtoTreeKey_t key;

    if (arpEntry->intIfNum != L7_INVALID_INTF)
    {
      *intIfNum = arpEntry->intIfNum;
      return L7_SUCCESS;
    }

    /* Bad things happen if we do a radix lookup for an address matches
     * a mask node. So avoid 224 and above. */
    if (arpEntry->ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    {
      /* don't increment bad_lookups */
      return L7_FAILURE;
    }

    rtoSetKey(&key, arpEntry->ipAddr);

    /* find the longest match */
    do
    {
        pData = radixMatchNode(&rtoRouteTreeData, &key, skipentries);

        /* deleted routes pending notification have no nextRouteInfo. Skip them. 
         * Also skip destinations with only inactive routes. Only use local
         * routes to resolve next hops. */
        if (pData && (!pData->nextRouteInfo || 
                      ((pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE) == 0) ||
                      (pData->nextRouteInfo->protocol != RTO_LOCAL)))
        {
          skipentries++;   
        }
        else
            break; /* no match, or best match found! */
    } 
    while(L7_NULLPTR != pData);
      
    if (pData == L7_NULLPTR)
    {
        return(L7_FAILURE);
    }

    if (!pData->nextRouteInfo)
        LOG_ERROR (arpEntry->ipAddr); /* radix entry must always have a list of IP addresses */

    /* Local routes have a single outgoing interface */
    *intIfNum = pData->nextRouteInfo->nextHops[0].intIfNum;

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Determines if the nhres table is full.
*
* @returns  L7_TRUE   If the table is full
* @returns  L7_FALSE  If the table is not full
*
* @notes    Internal utility function.  The calling function is responsible
*           for acquiring the route table semaphore.
*
* @end
*********************************************************************/
L7_BOOL rtoNHResIsFull ()
{
  if ((rtoStats.nhres_radix_entries ) >= RtoMaxNHRes)
      /* exceeded max best routes */
      return L7_TRUE;

  if (rtoFreeProtoInfoList == L7_NULLPTR)
      /* exceeded max for all routes */
      return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get a nhres protocol info structure from the free list.
*
* @param    none
*
* @returns  pointer to an available route info structure. All fields
*           in the structure are initialized to 0.
*
* @notes    
*
* @end
*********************************************************************/
static rtoProtoInfo_t *rtoNewProtoInfoGet(void)
{
    rtoProtoInfo_t *protoInfo = rtoFreeProtoInfoList;
    if (protoInfo == L7_NULLPTR){
        LOG_ERROR (0);
        return L7_NULL;
    }
   
    rtoFreeProtoInfoList = rtoFreeProtoInfoList->next;
    memset ((char *) protoInfo, 0, sizeof (rtoProtoInfo_t));
    return protoInfo;
}


/*********************************************************************
* @purpose  Delete a nh resolution route. 
*
* @param    routeEntry @b{(input)} Identifies the destination prefix and 
*              the source of the route to be deleted.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    Called by a routing protocol when it no longer has a route
            to a particular destination network.
*
* @end
*********************************************************************/
L7_RC_t rtoNHResCallbackUnregister (L7_uint32 ipAddr, 
                         void (*funcPtr)(L7_uint32 ipAddr,void *passthru),
                         void * passthru)
{
  rtoNhopData_t *pData;
  rtoProtoInfo_t *protoInfo;
  rtoProtoInfo_t *prevProtoInfo = L7_NULLPTR;
  L7_BOOL        route_found = L7_FALSE;
  rtoNhopData_t tempData;


  if (rtoTraceFlags & RTO_TRACE_NHRES_DEL)
  {
      L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
      L7_char8 destAddrStr[40];
      osapiInetNtoa(ipAddr, destAddrStr);
      sprintf(traceBuf, "RTO deleting NHres to %s", destAddrStr);
      rtoTraceWrite(traceBuf);
  }


  /* Cleanup data structures before proceeding */
  memset(&tempData, 0, sizeof(rtoNhopData_t));
  rtoSetKey(&tempData.network, ipAddr );
  rtoSetKey(&tempData.netmask, 0xffffffff);


  if (osapiSemaTake(rtoNHResTreeData.semId, 5) == L7_FAILURE)
    return L7_FAILURE;

  /* Try to find the destination network.
  */
  pData = radixLookupNode(&rtoNHResTreeData, &tempData.network, &tempData.netmask, L7_RN_EXACT);
  if (pData == L7_NULLPTR)
  {
    /* If we can't find the network, increment bad deletes and exit */
    /* note: this is expected behavior when a protocol deletes a route
       that has already been deleted by an ifdown flush 
    */
    if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiSemaGive\n");
    return L7_FAILURE;
  }

  protoInfo = pData->nextProtoInfo;

  /* Find the route to this destination provided by the protocol deleting
   * the route. */
  while ((protoInfo != L7_NULLPTR) && (route_found == L7_FALSE))
  {
      if ((protoInfo->funcPtr == funcPtr) &&
          (protoInfo->passthru == passthru))
      {
          if(prevProtoInfo == L7_NULL)
              pData->nextProtoInfo = protoInfo->next;
          else
              prevProtoInfo->next = protoInfo->next;

          route_found = L7_TRUE;
      }
      else
      {
          prevProtoInfo = protoInfo;
          protoInfo = protoInfo->next;
      }
  } 

  /* If route is not found then update an error counter and return failure indication.
  */
  if (route_found == L7_FALSE || protoInfo == L7_NULLPTR)
  {

    if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiSemaGive\n");

    return L7_FAILURE;
  }

  /* Return freed route info entry to the FREE list.
  */
  protoInfo->next = rtoFreeProtoInfoList;
  rtoFreeProtoInfoList = protoInfo;

  /* If we freed all route entries for the selected network then remove
  ** the network from the radix tree.
  */
  if (pData->nextProtoInfo == L7_NULLPTR)
  {
    pData = radixDeleteEntry(&rtoNHResTreeData, pData);

    if (pData == L7_NULLPTR)
      LOG_ERROR (0);

    rtoStats.nhres_radix_entries--;
  }

  if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiSemaGive\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds a nh res entry into the database.
*
* @param    routeEntry @b{(input)} Route information about the network 
*                                  to be added
*
* @returns  L7_SUCCESS  If the route is added to the routing table
* @returns  L7_TABLE_IS_FULL if the routing table is already full
* @returns  L7_ALREADY_CONFIGURED if the route is already in the table
* @returns  L7_FAILURE  any other failure
*
* @notes:   if the resolution for ip6Addr changes, the protocol gets
*           a callback.
*
*
* @end
*********************************************************************/
L7_RC_t rtoNHResCallbackRegister (L7_uint32 ipAddr, 
                                   void (*funcPtr)(L7_uint32 ipAddr,void *passthru),
                                   void * passthru)
{
  rtoNhopData_t routeData, *pData;
  rtoProtoInfo_t *protoInfo;
  L7_routeEntry_t routeEntry;

  if (rtoTraceFlags & RTO_TRACE_NHRES_ADD)
  {
      L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
      L7_char8 destAddrStr[40];
      osapiInetNtoa(ipAddr, destAddrStr);
      sprintf(traceBuf, "RTO adding NHRes to %s \n ", destAddrStr);
      rtoTraceWrite(traceBuf);

  }

  if(rtoValidDestAddr(ipAddr, 0xffffffff) != L7_TRUE)
  {
    return L7_FAILURE;
  }


  bzero ((char *)&routeData, sizeof (routeData));
  rtoSetKey(&routeData.network, ipAddr );
  rtoSetKey(&routeData.netmask, 0xffffffff);

  if (osapiSemaTake(rtoNHResTreeData.semId, 5) == L7_FAILURE)
    return L7_FAILURE;

  /* See if we already have a route to this destination. */
  pData = radixLookupNode(&rtoNHResTreeData, &routeData.network, &routeData.netmask, L7_RN_EXACT);

  /* If this is a new network then check that we have not exceeded
  ** the maximum route capacity.
  */
  if ((pData == L7_NULLPTR) && (rtoNHResIsFull() == L7_TRUE))
  {
      L7_char8 destAddrStr[40];
      osapiInetNtoa(ipAddr, destAddrStr);
      LOG_MSG("Failed to add NHRes to %s to the routing table. \n", destAddrStr);

      if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
          LOG_MSG("Error: rto.c osapiSemaGive\n");

      return L7_TABLE_IS_FULL;
  }

  if (pData == L7_NULLPTR)
  {
      /* Insert new destination */
    pData = radixInsertEntry(&rtoNHResTreeData, &routeData);

    /* NULL indicates successful insertion */
    if (pData == L7_NULLPTR)
    {
      rtoStats.nhres_radix_entries++;
    }
    else
      LOG_ERROR (0);

    /* Find the new entry so that we can attach route info. */
    pData = radixLookupNode(&rtoNHResTreeData, &routeData.network, &routeData.netmask, L7_RN_EXACT);
    if (!pData)
    {
      LOG_ERROR (0); /* Database corruption. */
      /* happy tools */
      if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiSemaGive\n");
      return L7_FAILURE;
    }

    /* find best route to dest */
    if(rtoBestRouteLookup (ipAddr, &routeEntry, L7_FALSE) == L7_SUCCESS)
    {
          pData->resolvingPrefixLen = rtoMaskLength(routeEntry.subnetMask);
    }
    else
    {
          /* no route */
          pData->resolvingPrefixLen = -1;
    }
  }
  else
  {
    /* We already have a route to this destination. */
  }

  protoInfo = pData->nextProtoInfo;

  /* See if this route is already in the routing table. */
  while (protoInfo != L7_NULLPTR)
  {
    if ((protoInfo->funcPtr == funcPtr) &&
        (protoInfo->passthru == passthru))
    {

      if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiSemaGive\n");

      return L7_ALREADY_CONFIGURED;
    }
    protoInfo = protoInfo->next;
  }

  protoInfo = rtoNewProtoInfoGet();
  /* cant be null, but keep tools happy */
  if(protoInfo == L7_NULL){
    if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
      LOG_MSG("Error: rto.c osapiSemaGive\n");
    return L7_FAILURE;
  }
  protoInfo->funcPtr = funcPtr;
  protoInfo->passthru = passthru;

  /* Insert the new route at the head of the list of routes to this dest. */
  protoInfo->next = pData->nextProtoInfo;
  pData->nextProtoInfo = protoInfo;

  if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
    LOG_MSG("Error: rto.c osapiSemaGive\n");

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Notify all relevant protocols of any registered next hop
*           resolutions that have changed.
*           
*
* @param    routeEntry  @b{(input)}  bestroute that has changed 
* @param    type        @b{(input)}  type of change (add/delete)
*
*
* @returns  SUCCESS or FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t rtoNHResNotify (rtoRouteData_t *routeEntry, L7_uint32 command )
{ 
    rtoNhopData_t tempData,*pData;
    rtoProtoInfo_t *protoInfo;
    L7_uint32   maxAddr;
    L7_int32   mlen;
    L7_uint32 network = osapiNtohl(routeEntry->network.addr);
    L7_uint32 netmask = osapiNtohl(routeEntry->netmask.addr);


    /* Cleanup data structures before proceeding */
    bzero ((char *)&tempData, sizeof (tempData));
    rtoSetKey(&tempData.network, network);
    rtoSetKey(&tempData.netmask, netmask);

    if (osapiSemaTake(rtoNHResTreeData.semId, 5) == L7_FAILURE)
      return L7_FAILURE;

    /* this returns the lexical lowest host address in the registered
       NH resolution radix tree that is resolved by the passed in routeEntry
       (+32 in key_bits is because key contains 32 bits of length)
       Note: this is fast.
    */
    mlen = (L7_int32)rtoMaskLength(netmask);
    if((pData = (rtoNhopData_t *)radixFirstResolved(&rtoNHResTreeData,
                                     &tempData.network, mlen+32)) == L7_NULL)
    {
         /* no hosts resolved by this route */
         if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
            LOG_MSG("Error: rto.c osapiSemaGive\n");
         return L7_SUCCESS;
    }

    /* make max resolved host address */
    maxAddr = network;
    maxAddr |= ~netmask;

    do{

        /* note that just because routeEntry resolves pData, it is not 
           certain that routeEntry is the best route to pData. Compare this route
           with last resolution to see if we really need to notify.
        */
        if(pData->resolvingPrefixLen <= mlen)
        {
              /* the changed route has a longer or equal  prefix than previously resolving route */
              if(command != RTO_RT_DEL)
              {
                   /* add/mod, set prefixLen of resolving route */
                   pData->resolvingPrefixLen = mlen;
              }
              else{
                   /* note that the prefixLen is only kept as a way to filter potentially
                      unnecessary notifies. To optimally filter, we would need to re-resolve
                      here in case a shorter resolution is available. however, to do so would
                      impose an undesirable overhead on the original caller, so we just clear
                      the filter. 
                   */
                   pData->resolvingPrefixLen = -1;
                   
              }


              /* send notify to registered protocols */
              protoInfo = pData->nextProtoInfo;

              while (protoInfo != L7_NULLPTR)
              {
                   (*protoInfo->funcPtr)(osapiNtohl(pData->network.addr),protoInfo->passthru);
                   protoInfo = protoInfo->next;
              }
        }


     /* get lexical next registered host addr and compare with max resolved address */
    }while( ((pData = radixGetNextEntry(&rtoNHResTreeData, pData)) != L7_NULL) &&
             (maxAddr >= osapiNtohl(pData->network.addr)));
      
    if (osapiSemaGive(rtoNHResTreeData.semId) != L7_SUCCESS)
        LOG_MSG("Error: rto.c osapiSemaGive\n");

    return L7_SUCCESS;
}

