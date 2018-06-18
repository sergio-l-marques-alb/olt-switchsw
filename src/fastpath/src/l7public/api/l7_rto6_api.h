/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_rto6_api.h
*
* @purpose  rto6 pubic api
*          
*
* @component rto6
*
* @comments none
*
* @create 12/20/2004
*
* @author jpickering
* @end
*
**********************************************************************/

/**************************@null{*************************************
                     
 *******************************}*************************************}
**********************************************************************/

#ifndef INCLUDE_L7_RTO6_API
#define INCLUDE_L7_RTO6_API

#include "ipv6_commdefs.h"
#include "ipstk_mib_api.h"


/* Route Event Types
*/
typedef enum
{
    /* RTO6 uses these 3 enumerators to indicate the addition, deletion, or 
     * change to the best route for a given destination. */
  RTO6_ADD_ROUTE = 1,
  RTO6_DELETE_ROUTE,
  RTO6_CHANGE_ROUTE,

    /* RTO6 uses these 2 enumerators to tell ARP whether a neighbor's IP address
     * is being used as the next hop address of a route. ARP then marks such
     * routes as "gateway" routes. */
  RTO6_ADD_ECMP_ROUTE,
  RTO6_DELETE_ECMP_ROUTE

} RTO6_ROUTE_EVENT_t;


/* enumeration defines the type of best route change to be reported to
 * a best route client for a specific route. */
typedef enum {

  RTO6_NOOP = 0,
  RTO6_RT_ADD,
  RTO6_RT_DEL,
  RTO6_RT_MOD

} e_RTO6_RT_CHANGE_TYPE;

typedef struct rto6RouteChange_s
{
	/* RTO6_ADD_ROUTE, RTO6_DELETE_ROUTE, RTO6_CHANGE_ROUTE */
	RTO6_ROUTE_EVENT_t changeType;

	L7_route6Entry_t routeEntry;

} rto6RouteChange_t;


/*********************************************************************
* @purpose  Change the preference of a specific route.
*
* @param   destPrefix @b{(input)}  route's destination address
* @param   destPrefixLen @b{(input)}  route's destination masklen
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
L7_RC_t rto6RoutePrefChange(L7_in6_addr_t *destPrefix, L7_uint32 destPrefixLen, 
                           L7_RTO6_ROUTE_TYPE_INDICES_t protocol, 
                           L7_uint32 oldPref, L7_uint32 newPref);

/*********************************************************************
* @purpose  Change the metric of a specific static route.
*
* @param   destPrefix @b{(input)}  route's destination address
* @param   destPrefixLen @b{(input)}  route's destination masklen
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
* @end
*********************************************************************/
L7_RC_t rto6RouteMetricChange(L7_in6_addr_t *destPrefix, L7_uint32 destPrefixLen, 
                             L7_uint32 metric);

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
* @end
*********************************************************************/
L7_RC_t rto6RouterPreferenceApply(L7_uint32 protocol, L7_uint32 pref);

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
L7_RC_t rto6RouteDelete (L7_route6Entry_t *routeEntry);

/*********************************************************************
* @purpose  Adds a network entry into the database.
*
* @param    routeEntry @b{(input)} Route information about the network 
*                                  to be added
*
* @returns  L7_SUCCESS  If the route is added to the routing table
* @returns  L7_TABLE_IS_FULL if the routing table is already full
* @returns  L7_ALREADY_CONFIGURED if the route is already in the table
* @returns  L7_FAILURE  any other failure
*
* @notes    In routeEntry->ecmpRoutes.equalCostPath[i].arpEntry, the 
*           caller need only set ipAddr. RTO will determine the 
*           internal interface number of the outgoing interface to 
*           the network to the next hop.
*
* @end
*********************************************************************/
L7_RC_t rto6RouteAdd (L7_route6Entry_t *routeEntry);

/*********************************************************************
* @purpose  Finds a network entry in the database.
*
* @param    routeEntry @b{(input)} Route information about the network 
*                                  to be found
*
* @returns  L7_SUCCESS  If the route is in the routing table
* @returns  L7_FAILURE  any other failure
*
* @notes    protocol/pref must match. 
*           
*
* @end
*********************************************************************/
L7_RC_t rto6RouteFind (L7_route6Entry_t *routeEntry);


/*********************************************************************
* @purpose  Returns the number of routes in the routing table. 
*
* @param    bestRoutesOnly  @b{(input)} If set to L7_TRUE, only best 
*                                       routes are counted.
*
* @returns  the number of routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 rto6RouteCount(L7_BOOL bestRoutesOnly);

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
L7_RC_t rto6LocalRouteReserve ();

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
L7_RC_t rto6LocalRouteUnReserve ();


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
L7_RC_t rto6BestRouteLookup (L7_in6_addr_t *dest_ip, L7_route6Entry_t *route,
                             L7_BOOL acceptRejectRoute);

/*********************************************************************
* @purpose  Find the best route to a specific prefix.
*
* @param    destPrefix  @b{(input)}  Destination IP prefix.
* @param    destPfxLen  @b{(input)}  Destination mask.
* @param    route       @b{(output)} best route to the prefix
*
* @returns  L7_SUCCESS  Route found
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_ERROR    If RTO is not initialized or bad param
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t rto6PrefixFind(L7_in6_addr_t *destPrefix, L7_uint32 destPfxLen, 
                       L7_route6Entry_t *route);

/*********************************************************************
* @purpose  Return next best route in the routing table. 
*
* @param    type         (input)  EXACT or NEXT
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found. 
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. Nexthop instance identified by ipv6RouteIndex.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t rto6SnmpNextBestRouteGet (L7_uint32 type, ipstkIpv6RouteEntry_t *nextRoute);

/*********************************************************************
* @purpose  Return next route (best or non-best) in the routing table.
*
* @param    type         (input)  EXACT or NEXT
* @param    nextRoute    (input/output) Next route in the database.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. Nexthop instance identified by ipv6RouteIndex.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t rto6AllNextRouteGet (L7_uint32 type, ipstkIpv6RouteEntry_t *nextRoute);

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
rto6CnfgrInitPhase1Process(L7_uint32 max_routes, L7_uint32 max_nhres);

/*********************************************************************
* @purpose  Reset startup done flag.
*
* @param    void
*
* @returns  L7_SUCCESS  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t rto6CnfgrInitPhase3Process(void);

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
rto6CnfgrFiniPhase1Process(L7_uint32 max_routes);

/*********************************************************************
*
* @purpose  Register a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr @b{(input)}     Pointer to the callback function.
*
* @returns  L7_SUCCESS  if the callback function is registered.
* @returns  L7_FAILURE  if the callback function is not registered, 
*                       either because of an error in the input 
*                       parameters or because the maximum number of
*                       callbacks are already registered.
*
* @notes    If a caller attempts to register a callback function 
*           that is already registered, the function returns 
*           L7_SUCCESS without registering the function a second 
*           time.
* @notes    
*
* @end
*******************************************************************/
L7_RC_t rto6BestRouteClientRegister (L7_uchar8 *clientName, void (*funcPtr)(void)); 

/*********************************************************************
*
* @purpose  Deregister a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr @b{(input)}     Pointer to the callback function.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*******************************************************************/
L7_RC_t rto6BestRouteClientDeregister (L7_uchar8 *clientName, void (*funcPtr)(void)); 

/****************************************************************
* @purpose  Request a set of the next N best route changes  
*           for a specific client.
*
* @param  callback @b{(input)} callback function pointer uniquely 
*                              identifying client
* @param  maxChanges @b {(input)} maximum number of changes 
*                                 client is willing to receive
* @param  numChanges @b {(output)} number of changes returned
* @param  moreChanges @b {(output)} L7_TRUE if RTO6 has more 
*                                   changes to report to this 
*                                   client
* @param  routes @b {(output)} RTO6 copies the changed routes to 
*               this buffer. The client allocates the buffer 
*               large enough to hold maxChanges.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    
*
* @end
***************************************************************/
L7_RC_t rto6RouteChangeRequest (L7_VOIDFUNCPTR_t callback, 
                                L7_uint32 maxChanges, 
                                L7_uint32 *numChanges, 
                                L7_BOOL *moreChanges, 
                                rto6RouteChange_t *routes);


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
L7_RC_t rto6NHResCallbackUnregister (L7_in6_addr_t *ip6Addr, 
                         void (*funcPtr)(L7_in6_addr_t *ip6Addr,void *passthru),
                         void * passthru);

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
L7_RC_t rto6NHResCallbackRegister (L7_in6_addr_t *ip6Addr, 
                                   void (*funcPtr)(L7_in6_addr_t *ip6Addr,void *passthru),
                                   void * passthru);


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
* @end
*********************************************************************/
L7_RC_t rto6NextRouteGet(L7_route6Entry_t *inOutRoute, L7_BOOL bestRouteOnly);


/*********************************************************************
* @purpose  Returns the number of routes in the routing table. 
*
* @param    bestRoutesOnly  @b{(input)} If set to L7_TRUE, only best 
*                                       routes are counted.
*
* @returns  the number of routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 rto6RouteCount(L7_BOOL bestRoutesOnly);

/*********************************************************************
* @purpose  Tell DTL/hw and IP stack about all best routes.
*           
* @param    void
*
* @returns  L7_SUCCESS 
*
*
* @end
*********************************************************************/
L7_RC_t rto6AllBestRoutesNotify(void);


/*********************************************************************
* @purpose  get rto6 default gateway
*
* @param    gwaddr:      ptr to next hop addr
* @param    intifnum:    ptr to next hop intifnum
*
* @returns  L7_SUCCESS if gw valid, else L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t rto6DefaultGatewayGet(L7_in6_addr_t *gw_addr, L7_uint32 *gw_intIfNum);

#endif
