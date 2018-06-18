/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2010
 *
 * *********************************************************************
 *
 * @filename  sprdx.c
 *
 * @purpose   APIs and implementation wrapping use of radix tree for 
 *            OSPF routing table.
 *
 * @component  Routing OSPF Component
 *
 * @create     5/10/2010
 *
 * @author     Rob Rice
 *
 *             OSPF maintains two routing tables:  the "networks" table and
 *             the "routers" table. The networks table includes the routes 
 *             that are given to RTO. The routers table includes routes to all
 *             routers in the area, all ABRs, and all ASBRs. Since each route
 *             includes a bitmask that indicates the type of route, it might
 *             be possible to have a single routing table, although it would 
 *             be necessary to distinguish network routes with a 32-bit netmask
 *             from router routes to the same IP address. (All router routes 
 *             have a 32-bit netmask). 
 *
 *             OSPF does a longest prefix match lookup on the networks table 
 *             for three reasons:
 * 
 *             a) To find the best route to a non-zero forwarding address when 
 *                computing a route for a T5 LSA
 *             b) When looking for the route to a virtual neighbor's IP address. 
 *             c) When determining the forwarding address to put in an external LSA.
 *
 *             Because of this, the networks table must be a data structure that
 *             allows efficient longest-prefix match lookups. OSPF only does 
 *             exact match lookups on the routers table. It could be a different
 *             data structure, such as AVL; however, in several places OSPF treats
 *             the networks table and routers table interchangeably. For this 
 *             reason, the routers table is also implemented as a radix tree. 
 *
 *             OSPF uses the same radix tree library as RTO. t_RoutingTableEntry 
 *             follows the guidelines described in radixCreateTree(). Unlike RTO,
 *             OSPF manages the memory for data entries in the tree. OSPF does 
 *             not provide a data heap, but instead allocates routing table 
 *             entries from the routing heap. This allows OSPF to directly 
 *             manage the items in the tree.  
 *             t_RoutingTableEntry includes a linked list of next hops. OSPF 
 *             manages the memory for the next hops, allocating each 
 *             individually from the routing heap. 
 * 
 *             This code only runs on the OSPFv2 protocol thread. So there is 
 *             no need for any additional semaphore protection. 
 *
 *             The routing table gets created when OSPF is first enabled. The 
 *             routing table gets deleted when the system goes from EXECUTE
 *             back to P3 INIT (i.e., on clear config, move management).
 *
 * @end
 *
 * ********************************************************************/

#include "std.h"
#include "local.h"
#include "l7_common.h"
#include "l3_commdefs.h"
#include "log.h"
#include "spobj.h"
#include "osapi.h"
#include "osapi_support.h"
#include "radix_api.h"


/* Set these to limit OSPF add and delete route tracing to certain prefixes. */
#define O2_TRACE_PREFIX_NONE 0xFFFFFFFF
static L7_uint32 o2TracePrefix = O2_TRACE_PREFIX_NONE;
static L7_uint32 o2TraceMask = O2_TRACE_PREFIX_NONE;



typedef Bool (*BROWSEFUNC)(byte *value, ulng param);

/* the radix key structure. Use o2SetKey to set 
 * the key properly. The first byte of the key 
 * gets set to the key length. The other three
 * bytes are not used, and are zeroed out. They 
 * ensure proper byte alignment is maintained.
 */
typedef struct o2TreeKey_s
{
  L7_uchar8 reserved[4];
  L7_uint32 value;             /* Always network byte order. Required by radix. */
} o2TreeKey_t;

/* Structure that wraps t_RoutingTableEntry objects when they are 
 * inserted in a radix tree. This might not be the best approach if one were 
 * starting from scratch, but originally routing tables were AVL trees and 
 * have been changed to radix trees. The radix tree requires certain fields
 * in the data structures inserted. Rather than change all of OSPF's 
 * references to this structure, we wrap the routing table entry in 
 * a structure containing the goop that the radix tree needs. */
typedef struct o2RadixWrapper_s
{
  /* Routing table entries are stored in a radix tree. Radix nodes must be the 
   * first element in this structure. */
  struct l7_radix_node nodes[2]; 

  /* Prefix and mask must be the next two members of the structure. 
   * Prefix must be AND'd with netmask (i.e. host bits are zero here). */
  o2TreeKey_t      prefix;  

  /* Destination network mask. Always all ones for router routes. */ 
  o2TreeKey_t      netmask;       

  /* The route itself */
  t_RoutingTableEntry *route;

  void *next;   /* Need this for radix */

} o2RadixWrapper_t;

/*********************************************************************
* @purpose  sets up a key for the OSPF radix tree
*
* @param    o2TreeKey_t  @b{(output)} A pointer to the key
* @param    addr         @b{(input)}  Key value.
*
* @notes    Sets the 1st byte to the length of the key, initializes
*           pad bytes and sets the key bytes to addr.
*
* @end
*********************************************************************/
static void o2SetKey(o2TreeKey_t *p_key, L7_uint32 addr)
{
  *((L7_int32 *)((p_key)->reserved)) = -1;     /* ffff out pad bytes */
  (p_key)->reserved[0] = sizeof(o2TreeKey_t);  /* set 1st byte as length */
  (p_key)->value = osapiHtonl(addr);           /* set address: must be net byte order */
}

/*********************************************************************
 * @purpose  Create OSPFv2 routing table.
 *
 * @param    p_RTB          @b{(input)}  routing table instance
 * @param    maxOspfRoutes  @b{(input)}  Maximum number of OSPF routes
 *
 * @returns  E_OK
 *
 * @notes    This creates four radix trees, two for network routes and 
 *           two for router routes. For network and router routers, there 
 *           is a tree of routes from the previous SPF and a tree of routes
 *           from the current SPF. The old tree is empty except when an SPF
 *           is in progress.  A radix tree is
 *           used to allow efficient longest prefix match lookups.    
 *
 *           OSPF does not provide radix with a data heap. Instead, OSPF
 *           manages the allocation and deallocation of data nodes added
 *           to the radix tree. This is done to allow OSPF to maintain
 *           references to routes in the tree across SPF runs. 
 *
 *           This code wraps each routing table entry in a wrapper structure
 *           that provides radix with the data fields it needs. 
 *
 * @end
 * ********************************************************************/
e_Err o2RoutingTableCreate(t_RTB *p_RTB, L7_uint32 maxOspfRoutes)
{
  /* Memory for radix tree internal data structures (mask nodes and mask lists).
   * Memory allocated is a function of the maximum number of OSPFv2 routes. */
  L7_uint32 treeHeapSize = RADIX_TREE_HEAP_SIZE(maxOspfRoutes, sizeof(o2TreeKey_t));
  

  /* Create new network routes radix tree. */
  p_RTB->RtbNtBt = XX_Malloc(sizeof(radixTree_t));
  if (p_RTB->RtbNtBt == NULL)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->RtbNtBt, 0, sizeof(radixTree_t));

  p_RTB->o2NtRouteTreeHeap = XX_Malloc(treeHeapSize);
  if (p_RTB->o2NtRouteTreeHeap == L7_NULLPTR)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->o2NtRouteTreeHeap, 0, treeHeapSize);

  radixCreateTree((radixTree_t*) p_RTB->RtbNtBt, NULL, p_RTB->o2NtRouteTreeHeap, 
                  maxOspfRoutes, sizeof(o2RadixWrapper_t), sizeof(o2TreeKey_t));

  /* Create old network routes radix tree. */
  p_RTB->RtbNtBtOld = XX_Malloc(sizeof(radixTree_t));
  if (p_RTB->RtbNtBtOld == NULL)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->RtbNtBtOld, 0, sizeof(radixTree_t));

  p_RTB->o2OldNtRouteTreeHeap = XX_Malloc(treeHeapSize);
  if (p_RTB->o2OldNtRouteTreeHeap == L7_NULLPTR)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->o2OldNtRouteTreeHeap, 0, treeHeapSize);

  radixCreateTree((radixTree_t*) p_RTB->RtbNtBtOld, NULL, p_RTB->o2OldNtRouteTreeHeap, 
                  maxOspfRoutes, sizeof(o2RadixWrapper_t), sizeof(o2TreeKey_t));

  /* Create new routers routes radix tree. */
  p_RTB->RtbRtBt = XX_Malloc(sizeof(radixTree_t));
  if (p_RTB->RtbRtBt == NULL)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->RtbRtBt, 0, sizeof(radixTree_t));

  p_RTB->o2RtRouteTreeHeap = XX_Malloc(treeHeapSize);
  if (p_RTB->o2RtRouteTreeHeap == L7_NULLPTR)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->o2RtRouteTreeHeap, 0, treeHeapSize);

  radixCreateTree((radixTree_t*) p_RTB->RtbRtBt, NULL, p_RTB->o2RtRouteTreeHeap, 
                  maxOspfRoutes, sizeof(o2RadixWrapper_t), sizeof(o2TreeKey_t));

  /* Create old routers routes radix tree. */
  p_RTB->RtbRtBtOld = XX_Malloc(sizeof(radixTree_t));
  if (p_RTB->RtbRtBtOld == NULL)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->RtbRtBtOld, 0, sizeof(radixTree_t));

  p_RTB->o2OldRtRouteTreeHeap = XX_Malloc(treeHeapSize);
  if (p_RTB->o2OldRtRouteTreeHeap == L7_NULLPTR)
  {
    return E_NOMEMORY;
  }
  memset(p_RTB->o2OldRtRouteTreeHeap, 0, treeHeapSize);

  radixCreateTree((radixTree_t*) p_RTB->RtbRtBtOld, NULL, p_RTB->o2OldRtRouteTreeHeap, 
                  maxOspfRoutes, sizeof(o2RadixWrapper_t), sizeof(o2TreeKey_t));


  return E_OK;
}

/*********************************************************************
 * @purpose  Delete OSPFv2 routing table.
 *
 * @param    p_RTB          @b{(input)}  routing table instance
 *
 * @returns  E_OK
 *
 * @notes       
 *
 * @end
 * ********************************************************************/
void o2RoutingTableDelete(t_RTB *p_RTB)
{
  /* New networks tree */
  if (p_RTB->o2NtRouteTreeHeap != L7_NULLPTR)
  {
    XX_Free(p_RTB->o2NtRouteTreeHeap); 
    p_RTB->o2NtRouteTreeHeap = NULL;
  }

  (void)radixDeleteTree((radixTree_t*) p_RTB->RtbNtBt);
  XX_Free(p_RTB->RtbNtBt);
  p_RTB->RtbNtBt = NULL;

  /* Old networks tree */
  if (p_RTB->o2OldNtRouteTreeHeap != L7_NULLPTR)
  {
    XX_Free(p_RTB->o2OldNtRouteTreeHeap); 
    p_RTB->o2OldNtRouteTreeHeap = NULL;
  }

  (void)radixDeleteTree((radixTree_t*) p_RTB->RtbNtBtOld);
  XX_Free(p_RTB->RtbNtBtOld);
  p_RTB->RtbNtBtOld = NULL;

  /* New routers tree */
  if (p_RTB->o2RtRouteTreeHeap != L7_NULLPTR)
  {
    XX_Free(p_RTB->o2RtRouteTreeHeap); 
    p_RTB->o2RtRouteTreeHeap = NULL;
  }

  (void)radixDeleteTree((radixTree_t*) p_RTB->RtbRtBt);
  XX_Free(p_RTB->RtbRtBt);
  p_RTB->RtbRtBt = NULL;

  /* Old routers tree */
  if (p_RTB->o2OldRtRouteTreeHeap != L7_NULLPTR)
  {
    XX_Free(p_RTB->o2OldRtRouteTreeHeap); 
    p_RTB->o2OldRtRouteTreeHeap = NULL;
  }

  (void)radixDeleteTree((radixTree_t*) p_RTB->RtbRtBtOld);
  XX_Free(p_RTB->RtbRtBtOld);
  p_RTB->RtbRtBtOld = NULL;
}

/*********************************************************************
* @purpose  Delete all routes in a routing table.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  routing table
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2ClearRoutes(t_RTB *p_RTB, t_Handle routeTable)
{
  e_Err e;
  e_Err ed;
  e_Err ret = E_OK;
  t_RoutingTableEntry *p_Rte = NULL;
  t_RoutingTableEntry *p_RteNext = NULL;

  e = o2RouteGetFirst(p_RTB, routeTable, (void *)&p_RteNext);
  while (e == E_OK)
  { 
    p_Rte = p_RteNext;
    e = o2RouteGetNext(p_RTB, routeTable, &p_RteNext);
    ed = o2RouteDelete(p_RTB, routeTable, p_Rte->DestinationId, p_Rte->IpMask, TRUE);
    if (ed != E_OK)
    {
      p_RTB->rtDbgStats.o2RdxBadClear++;
      ret = ed;
    }
  }
  return ret;
}

/*********************************************************************
* @purpose  Get the number of routes in a routing table.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  routing table
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 o2RoutingTableCount(t_RTB *p_RTB, t_Handle routeTable)
{
  return radixTreeCount((radixTree_t*) routeTable);
}

/*********************************************************************
* @purpose  Add a route
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    route       @b{(input)}  Route to be added
*
* @returns  E_OK if route added to radix tree
*           E_IN_MATCH is a matching route is already in the tree
*           E_NOMEMORY if memory allocation failed
*           E_FAILED if prefix is not a valid unicast prefix
*           E_BADPARM if destination prefix is invalid
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2RouteAdd(t_RTB *p_RTB, t_Handle routeTable, t_RoutingTableEntry *route)
{
  t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
  o2RadixWrapper_t *dup = NULL;
  o2RadixWrapper_t *rdxWrapper; 

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (route->DestinationId >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return E_BADPARM;
  }

  rdxWrapper = XX_Malloc(sizeof(o2RadixWrapper_t));
  if (!rdxWrapper)
  {
    p_RTB->rtDbgStats.o2RdxMallocFail++;
    return E_NOMEMORY;
  }
  memset(rdxWrapper, 0, sizeof(o2RadixWrapper_t));

  o2SetKey(&rdxWrapper->prefix, (route->DestinationId & route->IpMask));
  o2SetKey(&rdxWrapper->netmask, route->IpMask);

  /* See if we already have a route to this destination. */
  if (radixLookupNode((radixTree_t*) routeTable, &rdxWrapper->prefix, 
                      &rdxWrapper->netmask, L7_RN_EXACT) != NULL)
  {
    XX_Free(rdxWrapper);
    return E_IN_MATCH;
  }
  
  /* Insert new route */
  rdxWrapper->route = route;
  dup = (o2RadixWrapper_t*) radixInsertEntry((radixTree_t*) routeTable, rdxWrapper);
  if (dup != NULL)
  {
    /* Failed to insert route. Figure out why and report failure. */
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(route->DestinationId, destStr);
    osapiInetNtoa(route->IpMask, maskStr);
    if (dup != rdxWrapper)
    {
      /* Matching entry already exists */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failed to add OSPF route to %s %s to OSPF routing table. "
              "Route to this destination already in table.",
              destStr, maskStr);
    }
    else if (radixTreeFull((radixTree_t*) routeTable))
    {
      p_RTB->rtDbgStats.o2RdxTreeFull++;
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failed to add OSPF route to %s %s to OSPF routing table. "
              "Radix tree is full.",
              destStr, maskStr);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failed to add OSPF route to %s %s to OSPF routing table.",
              destStr, maskStr);
    }
    XX_Free(rdxWrapper);
    p_RTB->rtDbgStats.o2RdxAddFail++;
    return E_FAILED;
  }

  if ((p_RTO->ospfTraceFlags & OSPF_TRACE_ROUTE) &&
      ((o2TracePrefix == O2_TRACE_PREFIX_NONE) ||
       ((o2TracePrefix & o2TraceMask) == (route->DestinationId & o2TraceMask))))
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(route->DestinationId, destStr);
    osapiInetNtoa(route->IpMask, maskStr);
    osapiInetNtoa(route->AreaId, areaStr);
    osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                  "OSPFv2 adding %s route to %s %s in area %s. Path type %s. "
                  "Cost %u. Hops %u.",
                  (route->DestinationType < DEST_TYPE_IS_NETWORK) ? "router" : "network",
                  destStr, maskStr, areaStr, o2PathTypeString(route->PathType), 
                  route->Cost, route->PathNum);
    RTO_TraceWrite(traceBuf);    
  }
  
  return E_OK;
}

/*********************************************************************
* @purpose  Delete a route and free it.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    destPrefix  @b{(input)}  Destination IP prefix
* @param    destMask    @b{(input)}  Destination mask
* @param    freeRoute   @b{(input)}  If TRUE, free the route and its next hops
*
* @returns  E_OK if route removed from radix tree
*           E_FAILED otherwise
*           E_BADPARM if destPrefix is invalid
*
* @notes    Frees all memory used by the route.
*
* @end
*********************************************************************/
e_Err o2RouteDelete(t_RTB *p_RTB, t_Handle routeTable, L7_uint32 destPrefix,
                    L7_uint32 destMask, Bool freeRoute)
{
  t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
  o2RadixWrapper_t *rdxWrapper;
  o2TreeKey_t network;
  o2TreeKey_t netmask;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (destPrefix >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return E_BADPARM;
  }
  
  o2SetKey(&network, (destPrefix & destMask));
  o2SetKey(&netmask, destMask);

  rdxWrapper = radixLookupNode((radixTree_t*) routeTable, &network, &netmask, L7_RN_EXACT);
  if (rdxWrapper == NULL)
  {
    p_RTB->rtDbgStats.o2RdxBadDel++;
    return E_FAILED;
  }

  rdxWrapper = radixDeleteEntry((radixTree_t*) routeTable, rdxWrapper);
  if (rdxWrapper == L7_NULLPTR)
  {
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(destPrefix, destStr);
    osapiInetNtoa(destMask, maskStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to delete OSPFv2 route to %s %s from OSPF routing table.",
            destStr, maskStr);
    p_RTB->rtDbgStats.o2RdxBadDel++;
    return E_FAILED;
  }

  if ((routeTable != p_RTB->RtbNtBtOld) &&
      (routeTable != p_RTB->RtbRtBtOld) &&
      (p_RTO->ospfTraceFlags & OSPF_TRACE_ROUTE) &&
      ((o2TracePrefix == O2_TRACE_PREFIX_NONE) ||
       ((o2TracePrefix & o2TraceMask) == (destPrefix & o2TraceMask))))
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(rdxWrapper->route->DestinationId, destStr);
    osapiInetNtoa(rdxWrapper->route->IpMask, maskStr);
    osapiInetNtoa(rdxWrapper->route->AreaId, areaStr);
    osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                  "OSPFv2 deleting %s route to %s %s in area %s. Path type %s.",
                  (rdxWrapper->route->DestinationType < DEST_TYPE_IS_NETWORK) ? "router" : "network",
                  destStr, maskStr, areaStr, o2PathTypeString(rdxWrapper->route->PathType));
    RTO_TraceWrite(traceBuf);    
  }

  if (freeRoute)
  {
    RteRelease(rdxWrapper->route, TRUE);
  }
  XX_Free(rdxWrapper);

  return E_OK;
}

/*********************************************************************
* @purpose  Update a radix tree node to point to a new route object.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    new Route   @b{(input)}  new route
*
* @returns  E_OK if route replaced in radix tree
*           E_FAILED otherwise
*           E_BADPARM if destination address of new route is invalid
*
* @notes    Frees the memory for the replaced route
*
* @end
*********************************************************************/
e_Err o2RadixChange(t_RTB *p_RTB, t_Handle routeTable, t_RoutingTableEntry *newRoute)
{
  o2RadixWrapper_t *rdxWrapper;
  o2TreeKey_t network;
  o2TreeKey_t netmask;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (newRoute->DestinationId >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return E_BADPARM;
  }

  o2SetKey(&network, (newRoute->DestinationId & newRoute->IpMask));  
  o2SetKey(&netmask, newRoute->IpMask);

  rdxWrapper = (o2RadixWrapper_t*) radixLookupNode((radixTree_t*) routeTable, 
                                                   &network, &netmask, L7_RN_EXACT);
  if (!rdxWrapper)
  {
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(newRoute->DestinationId, destStr);
    osapiInetNtoa(newRoute->IpMask, maskStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to change OSPFv2 route to %s %s in OSPF routing table.",
            destStr, maskStr);

    p_RTB->rtDbgStats.o2RdxBadChange++;
    return E_FAILED;
  }

  RteRelease(rdxWrapper->route, TRUE);
  rdxWrapper->route = newRoute;
  return E_OK;
}

/*********************************************************************
* @purpose  Get the first route in a route table.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    route       @b{(output)} Handle to returned route
*
* @returns  pointer to route object in the radix tree, if a route exists
*
* @notes    E_OK if a route is returned
*           E_NOT_FOUND if routing table is empty
*           E_BADPARM if destination address invalid
*
* @end
*********************************************************************/
e_Err o2RouteGetFirst(t_RTB *p_RTB, t_Handle routeTable, t_RoutingTableEntry **route)
{
  o2TreeKey_t network, netmask;
  o2RadixWrapper_t *rdxWrapper;

  /* Get default route, if one exists. */
  o2SetKey(&network, 0);
  o2SetKey(&netmask, 0);
  rdxWrapper = (o2RadixWrapper_t*) radixLookupNode((radixTree_t*) routeTable, &network, 
                                                   &netmask, L7_RN_EXACT);

  if (rdxWrapper == L7_NULLPTR)
  {
    /* No exact match. Lookup next */
    rdxWrapper = (o2RadixWrapper_t*) radixLookupNode((radixTree_t*) routeTable, &network, 
                                                   &netmask, L7_RN_GETNEXT);
    if (rdxWrapper == NULL)
    {
      return E_NOT_FOUND;
    }
  }

  *route = rdxWrapper->route;
  return E_OK;
}

/*********************************************************************
* @purpose  Get the next route in a route table.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    route       @b{(input/output)} Handle to previous/next route
*
* @returns  pointer to route object in the radix tree, if another route exists
*
* @notes    E_OK if a route is returned
*           E_BADPARM if input route is invalid
*           E_NOT_FOUND if no more routes
*
* @end
*********************************************************************/
e_Err o2RouteGetNext(t_RTB *p_RTB, t_Handle routeTable, t_RoutingTableEntry **route)
{
  t_RoutingTableEntry *inOutRoute = (t_RoutingTableEntry*)(*route);
  o2RadixWrapper_t *rdxWrapper;

  /* tree key */
  o2TreeKey_t network, netmask;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (inOutRoute->DestinationId >= (L7_uint32) L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return E_BADPARM;
  }

  /* setup tree key */
  o2SetKey(&network, inOutRoute->DestinationId);
  o2SetKey(&netmask, inOutRoute->IpMask);

  rdxWrapper = (o2RadixWrapper_t*) radixLookupNode((radixTree_t*) routeTable, 
                                                   &network, &netmask, L7_RN_GETNEXT);
  if (rdxWrapper == NULL)
  {
    return E_NOT_FOUND;
  }

  *route = rdxWrapper->route;
  return E_OK;
}

/*********************************************************************
* @purpose  Find the current OSPF route to a specific network. This is an
*           exact match for prefix/mask, not an LPM match.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    destPrefix  @b{(input)}  Destination IP prefix
* @param    destMask    @b{(input)}  Destination mask
*
* @returns  pointer to route object in the radix tree, if matching route
*           found. NULL if route not found.
*
* @notes    
*
* @end
*********************************************************************/
t_RoutingTableEntry *o2RouteFind(t_RTB *p_RTB, t_Handle routeTable, 
                                 L7_uint32 destPrefix, L7_uint32 destMask)
{
  o2RadixWrapper_t *rdxWrapper;
  o2TreeKey_t network;
  o2TreeKey_t netmask;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (destPrefix >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return NULL;
  }

  o2SetKey(&network, (destPrefix & destMask));  /* just in case host bits aren't zeroed */
  o2SetKey(&netmask, destMask);

  rdxWrapper = (o2RadixWrapper_t*) radixLookupNode((radixTree_t*) routeTable, 
                                                   &network, &netmask, L7_RN_EXACT);
  if (rdxWrapper)
    return rdxWrapper->route;
  else
    return NULL;
}

/*********************************************************************
* @purpose  Find the longest prefix match for a given IP address.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    destPrefix  @b{(input)}  IP address
*
* @returns  pointer to route object in the radix tree, if matching route
*           found. NULL if route not found.
*
* @notes    
*
* @end
*********************************************************************/
t_RoutingTableEntry *o2LongestPrefixMatch(t_RTB *p_RTB, t_Handle routeTable, 
                                          L7_uint32 ipAddr)
{
  o2TreeKey_t key;
  o2RadixWrapper_t *rdxWrapper;

  /* Bad things happen if we do a radix lookup for an address matches
   * a mask node. So avoid 224 and above. */
  if (ipAddr >= (L7_uint32) L7_CLASS_D_ADDR_NETWORK)
  {
    p_RTB->rtDbgStats.o2RdxBadAddr++;
    return NULL;
  }

  o2SetKey(&key, ipAddr);

  rdxWrapper = (o2RadixWrapper_t*) radixMatchNode(routeTable, &key, 0);
  if (rdxWrapper)
    return rdxWrapper->route;
  else
    return NULL;
}

/*********************************************************************
* @purpose  Call a given function for each route in a routing table.
*
* @param    p_RTB       @b{(input)}  routing table instance
* @param    routeTable  @b{(input)}  radix tree
* @param    fcn         @b{(input)}  function to be called for each route
* @param    param       @b{(input)}  function argument
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2RoutesBrowse(t_RTB *p_RTB, t_Handle routeTable, 
                     BROWSEFUNC userBrowser, ulng param)
{
  e_Err     e, er;
  t_RoutingTableEntry *route;

  e = o2RouteGetFirst(p_RTB, routeTable, &route);
  while (e == E_OK)
  {
    if (!userBrowser((byte *) route, param))
    {
      /* If browse function returns FALSE, remove the route from the tree. */
      er = o2RouteDelete(p_RTB, routeTable, route->DestinationId, route->IpMask, TRUE);
      ASSERT (er == E_OK);
    }
    e = o2RouteGetNext(p_RTB, routeTable, &route);
  }

  return E_OK;
}


void o2TracePrefixSet(L7_uint32 prefix, L7_uint32 mask)
{
  o2TracePrefix = prefix;
  o2TraceMask = mask;
}

void o2TracePrefixClear(void)
{
  o2TracePrefix = O2_TRACE_PREFIX_NONE;
  o2TraceMask = O2_TRACE_PREFIX_NONE;
}

void o2RadixDebugStats(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  t_RTB *p_RTB;
  e_Err e;

  if (p_RTO == NULL)
    return;

  for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
        e == E_OK;
        e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
  {
    printf("\nBad clear:                   %u", p_RTB->rtDbgStats.o2RdxBadClear);
    printf("\nInvalid unicast IP address:  %u", p_RTB->rtDbgStats.o2RdxBadAddr);
    printf("\nMemory allocation failure:   %u", p_RTB->rtDbgStats.o2RdxMallocFail);
    printf("\nRadix tree full:             %u", p_RTB->rtDbgStats.o2RdxTreeFull);
    printf("\nAdd failure:                 %u", p_RTB->rtDbgStats.o2RdxAddFail);
    printf("\nDelete failure:              %u", p_RTB->rtDbgStats.o2RdxBadDel);
    printf("\nChange failure:              %u", p_RTB->rtDbgStats.o2RdxBadChange);
  }
}

void o2RoutePrint(t_RoutingTableEntry *route)
{
  L7_uchar8 pfxStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];
  t_OspfPathPrm *path;
  L7_uint32 i;

  osapiInetNtoa(route->DestinationId, pfxStr);
  osapiInetNtoa(route->IpMask, maskStr);
  osapiInetNtoa(route->AreaId, areaStr);

  path = route->PathPrm;
  if (path)
  {
    osapiInetNtoa(path->IpAdr, nhStr);
  }
  else
  {
    osapiStrncpySafe(nhStr, "None", OSAPI_INET_NTOA_BUF_SIZE);
  }
  sysapiPrintf("\n%16s %16s %16s %8s %8u %8u %16s",
                 pfxStr, maskStr, areaStr, o2PathTypeString(route->PathType), route->Cost,
                 (route->PathType == OSPF_TYPE_2_EXT) ? route->Type2Cost : 0,
                 nhStr);
  for (i = 1; i < route->PathNum; i++)
  {
    path = path->next;
    if (path)
    {
      osapiInetNtoa(path->IpAdr, nhStr);
    }
    else
    {
      osapiStrncpySafe(nhStr, "None", OSAPI_INET_NTOA_BUF_SIZE);
    }
    sysapiPrintf("\n%75s %18s", "", nhStr);
  }
}

/* routeTypes mask - 1 - print network routes 
 *                   2 - print router routes 
 *                   3 - print both */
void o2RouteTablePrint(L7_uint32 routeTypes)
{
  e_Err e;
  t_RoutingTableEntry *route;
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  t_RTB *p_RTB;

  if (p_RTO == NULL)
    return;

  e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
  if (e != E_OK)
  {
    return;
  }

  sysapiPrintf("\nNumber of network routes:  %u", 
               o2RoutingTableCount(p_RTB, p_RTB->RtbNtBt));
  sysapiPrintf("\nNumber of router routes:  %u", 
               o2RoutingTableCount(p_RTB, p_RTB->RtbRtBt));


  if (routeTypes & 1)
  {
    /* Print network routes */
    sysapiPrintf("\nNetwork routes...\n");
    sysapiPrintf("\n%16s %16s %16s %8s %8s %8s %16s",
                 "Prefix", "Mask", "Area", "Type", "Metric", "T2 Cost", "Next Hops");
    e = o2RouteGetFirst(p_RTB, p_RTB->RtbNtBt, &route);
    while (e == E_OK)
    {
      o2RoutePrint(route);
      e = o2RouteGetNext(p_RTB, p_RTB->RtbNtBt, &route);
    }
  }

  if (routeTypes & 2)
  {
    /* Print router routes */
    sysapiPrintf("\n\nRouter routes...\n");
    sysapiPrintf("\n%16s %16s %16s %8s %8s %8s %16s",
                 "Prefix", "Mask", "Area", "Type", "Metric", "T2 Cost", "Next Hops");
    e = o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &route);
    while (e == E_OK)
    {
      o2RoutePrint(route);
      e = o2RouteGetNext(p_RTB, p_RTB->RtbRtBt, &route);
    }
  }
}

void o2DumpTrees(void)
{
  e_Err e;
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  t_RTB *p_RTB;

  if (p_RTO == NULL)
    return;

  e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);

  sysapiPrintf("\nNetworks Tree\n");
  radixDumpTree((radixTree_t*) p_RTB->RtbNtBt);

  sysapiPrintf("\nOld Networks Tree\n");
  radixDumpTree((radixTree_t*) p_RTB->RtbNtBtOld);

  sysapiPrintf("\nRouters Tree\n");
  radixDumpTree((radixTree_t*) p_RTB->RtbRtBt);

  sysapiPrintf("\nOld Routers Tree\n");
  radixDumpTree((radixTree_t*) p_RTB->RtbRtBtOld); 
}



