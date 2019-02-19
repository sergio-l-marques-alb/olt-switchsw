#ifndef RTO_API_H
#define RTO_API_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto_api.h
*
* @purpose API interface for route table object.
*
* @component
*
* @create 03/13/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "l7_common_l3.h"
#include "sysnet_api.h"

#define RTO_ALL_ROUTES  1
#define RTO_BEST_ROUTE  2

/* Route Event Types
*/
typedef enum
{
    /* RTO uses these 3 enumerators to indicate the addtion, deletion, or 
     * change to the best route for a given destination. */
  RTO_ADD_ROUTE = 1,
  RTO_DELETE_ROUTE,
  RTO_CHANGE_ROUTE

} RTO_ROUTE_EVENT_t;



/* enumeration defines the type of best route change to be reported to
 * a best route client for a specific route. */
typedef enum {

  RTO_NOOP = 0,
  RTO_RT_ADD,
  RTO_RT_DEL,
  RTO_RT_MOD

} e_RTO_RT_CHANGE_TYPE;

typedef struct rtoRouteChange_s
{
	/* RTO_ADD_ROUTE, RTO_DELETE_ROUTE, RTO_CHANGE_ROUTE */
	RTO_ROUTE_EVENT_t changeType;

	L7_routeEntry_t routeEntry;

} rtoRouteChange_t;


/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Initialize RTO object
*
* @param    max_routes    Maximum number of routes to store in the table.
* @param    max_nhres     Maximum number of nexthops resolved
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
rtoCnfgrInitPhase1Process(L7_uint32 max_routes, L7_uint32 max_nhres);


/*********************************************************************
* @purpose  Fini RTO object
*
* @param    max_routes    Maximum number of routes to store in the table.
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
rtoCnfgrFiniPhase1Process(L7_uint32 max_routes);

/*********************************************************************
* @purpose  Change the preference of a specific route.
*
* @param   destAddr @b{(input)}  route's destination address
* @param   destMask @b{(input)}  route's destination mask
* @param   nextHop  @b{(input)}  previous preference 
* @param   protocol @b{(input)}  protocol that supplied the route
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
                           L7_uint32 oldPref, L7_uint32 newPref);

/*********************************************************************
* @purpose  Config Router Preference
*
* @param    index   router protocol type
* @param    pref    preference associated with the protocol
*
* @returns  L7_SUCCESS  If the new preferences were successfully applied
* @returns  L7_FAILURE  If the new preferences could not be applied
*
* @notes    Ip state is always ENABLED.  This function walks the
*           tree of network entries and applies the new preference.
*           It re-elects a new best route as a result of changes
*           to the route preferences.
*
* @end
*********************************************************************/
L7_RC_t rtoRouterPreferenceApply(L7_uint32 index, L7_uint32 pref);

/*********************************************************************
* @purpose  Inform routing object that an IP interface became active.
*
* @param    intIfNum    Internal Interface Number
*
* @notes    No longer does anything
*
* @end
*********************************************************************/
void
rtoIfUp (L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Inform routing object that an IP interface went down
*
* @param    intIfNum    Internal Interface Number
*
* @notes    No longer does anything
*
* @end
*********************************************************************/
void
rtoIfDown (L7_uint32 intIfNum);

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
L7_RC_t rtoBestRouteClientRegister (L7_uchar8 *clientName, void (*funcPtr)(void)); 

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
L7_RC_t rtoBestRouteClientDeregister (L7_uchar8 *clientName, void (*funcPtr)(void)); 

/****************************************************************
* @purpose  Request a set of the next N best route changes  
*           for a specific client.
*
* @param  callback @b{(input)} callback function pointer uniquely 
*                              identifying client
* @param  maxChanges @b {(input)} maximum number of changes 
*                                 client is willing to receive
* @param  numChanges @b {(output)} number of changes returned
* @param  moreChanges @b {(output)} L7_TRUE if RTO has more 
*                                   changes to report to this 
*                                   client
* @param  routes @b {(output)} RTO copies the changed routes to 
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
L7_RC_t rtoRouteChangeRequest (L7_VOIDFUNCPTR_t callback, 
                               L7_uint32 maxChanges, 
                               L7_uint32 *numChanges, 
                               L7_BOOL *moreChanges, 
                               rtoRouteChange_t *routes);

/*********************************************************************
* @purpose  Adds a network entry into the database.
*
* @param    routeEntry  Route information about the network to be added
*
* @returns  L7_SUCCESS  If the route is added to the routing table
* @returns  L7_TABLE_IS_FULL if the routing table is already full
* @returns  L7_ALREADY_CONFIGURED if the route is already in the table
* @returns  L7_NOT_EXIST if none of the route's next hops is in an 
*                        attached subnet
* @returns  L7_FAILURE  any other failure
*
* @notes    none
*
* @end
*********************************************************************/ 
L7_RC_t rtoRouteAdd (L7_routeEntry_t *routeEntry);

/*********************************************************************
* @purpose  Delete all routes to the specified network, learnt
*           via the specified protocol.
*
* @param    routeEntry  Route information about the network to delete
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes    Called by a routing protocol when it loses all routes
            to a particular destination network.
*
* @end
*********************************************************************/
L7_RC_t rtoRouteDelete (L7_routeEntry_t *routeEntry);

/*********************************************************************
* @purpose  Modifies the route to the specified network, learnt
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
*           (NOTE)
*           Note that the best route change notification does not send 
*           out the old set of next hops. However since the modify 
*           operation translates to an add/delete operation for hardware, 
*           we can get away with this
*
* @end
*********************************************************************/
L7_RC_t rtoRouteModify (L7_routeEntry_t *routeEntry);

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
L7_RC_t rtoNextRouteGet (L7_routeEntry_t *inOutRoute, L7_BOOL bestRouteOnly);

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
L7_RC_t rtoRouteGet (L7_routeEntry_t *inOutRoute);

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
                            L7_BOOL acceptRejectRoute);

/*********************************************************************
* @purpose  Find the best route to a specific prefix.
*
* @param    destPrefix  @b{(input)}  Destination IP prefix.
* @param    destMask    @b{(input)}  Destination mask.
* @param    route    @b{(output)} best route to the prefix
*
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_SUCCESS  Router found
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t rtoPrefixFind(L7_uint32 destPrefix, L7_uint32 destMask, 
                      L7_routeEntry_t *route);

/*********************************************************************
* @purpose  Delete all routes for the specified interface. Routes
*           are deleted whenever an IP interface goes down.
*
* @param    intIfNum    Internal Interface Number associated with this router port.
*
* @notes    none
*
* @end
*********************************************************************/
void rtoIfRouteFlush (L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Route IP frames and process ARPs.
*
* @param    bufHandle Network Buffer containg incoming IP or ARP packet.
* @param    *pduInfo    pointer to PDU info structure which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  Frame has been consumed by the routing process.
* @returns  L7_FAILURE  Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtoIPv4Handle (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

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
* @notes    
*
* @end
*********************************************************************/
L7_RC_t rtoBestRouteLookupExact(L7_uint32 ipAddr, L7_uint32 netMask, L7_uint32 *gateway);

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
L7_BOOL rtoValidDestAddr(L7_uint32 destAddr, L7_uint32 mask);

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
L7_uint32 rtoMaskLength(L7_uint32 mask);

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
L7_RC_t rtoRouteReserve (void);

/*********************************************************************
* @purpose  Un-reserves a route in the route tables database.
*
* @returns  L7_SUCCESS        If successful
* @returns  L7_FAILURE        If the rto semaphore could not be taken
* @returns  L7_TABLE_IS_FULL  If there are no free entries in the table
*
* @notes    Usage: This function must be called if and only if you have
*           previously reserved a route in the route table using
*           rtoRouteReserve
*
*           Remember to unreserve an entry that you have previously   
*           reserved if you do not intend to use it.
*         
* @end
*********************************************************************/
L7_RC_t rtoRouteUnReserve (void);

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
L7_uint32 rtoRouteCount(L7_BOOL bestRoutesOnly);

/*********************************************************************
* @purpose  Delete a nh resolution route. 
*
* @param    ipAddr @b{(input)} Identifies the destination prefix
* @param    funcPtr @b{(input)} callback function pointer 
* @param    passthru @b{(input)} passthrough used in callback
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rtoNHResCallbackUnregister (L7_uint32 ipAddr, 
                                    void (*funcPtr)(L7_uint32 ipAddr,void *passthru),
                                    void * passthru);

/*********************************************************************
* @purpose  Adds a nh res entry into the database.
*
* @param    ipAddr @b{(input)} Identifies the destination prefix
* @param    funcPtr @b{(input)} callback function pointer 
* @param    passthru @b{(input)} passthrough used in callback
*
* @returns  L7_SUCCESS  If registration successfull
* @returns  L7_FAILURE  If not
*
* @notes:   if the next hop resolution for ipAddr changes,
*           the protocol gets a callback.
*
*
* @end
*********************************************************************/
L7_RC_t rtoNHResCallbackRegister (L7_uint32 ipAddr, 
                                  void (*funcPtr)(L7_uint32 ipAddr,void *passthru),
                                  void * passthru);

#endif /* RTO_API_H */



