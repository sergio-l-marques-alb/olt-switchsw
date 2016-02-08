
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_ip6_api.h
*
* @purpose    Ip6 Mapping layer external function prototypes
*
* @component  Ip6 Mapping Layer
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/



#ifndef USMDB_IP6_API_H
#define USMDB_IP6_API_H


#include "ipv6_commdefs.h"
#include "async_event_api.h"

/*********************************************************************
* @purpose  Get the administrative mode of the router
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_RC_t  usmDbIp6RtrAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Enable or disable the router.
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    This command is a global switch for enabling all routing
* @notes    functions, including forwarding and various routing protocols.
* @notes    Compare this command with ipMapIpForwardingSet(), which
* @notes    simply enables or disables IP forwarding.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrAdminModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Get the default TTL of the router
*
* @param    void
*
* @returns  TTL, if 0 means unconfigured
*
* @notes   
*
*
* @end
*********************************************************************/
L7_RC_t  usmDbIp6RtrDefaultTTLGet(L7_uint32 *valp);


/*********************************************************************
* @purpose  Set router TTL
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    this value is sent in router advertisements
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrDefaultTTLSet( L7_uint32 val);

/*********************************************************************
* @purpose  Get the administrative state of an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfModeGet( L7_uint32 intIfNum, L7_uint32 *mode);
 
/*********************************************************************
* @purpose  Get the operational state of an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfOperModeGet( L7_uint32 intIfNum, L7_uint32 *mode);
 

/*********************************************************************
* @purpose  Enable or disable interface particip6ation in the router
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode);


/*********************************************************************
* @purpose  Determine if IP6 interface exists
*
* @param    unitIndex       Unit Index
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE   
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL usmDbIp6IntfExists(L7_uint32 unitIndex, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Determine if the interface is valid for participation
*           in the component
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL usmDbIp6MapIntfIsValid(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Get the first valid interface for participation
*           in the component
*
* @param    intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIp6MapIntfValidFirstGet(L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  Get the next valid interface for participation
*           in the component
*
* @param     prevIfNum  @b{(input)} internal interface number
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIp6MapIntfValidNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  ADD an IP6 address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ip6Address       IP6 Address of the interface
* @param    prefixLen       length of prefix
* @param    flags           L7_IP6V6_ADDR_LINK_LOCAL_ONLY, L7_IP6V6_ADDR_EUI64
*
* @returns  L7_SUCCESS      ip6 address could be successfully configured
* @returns  L7_ERROR        subnet conflict between specified ip6
*                           address & an already configured ip6
*                           address
* @returns  L7_FAILURE      other errors or failures
*
* @notes    There is no such concept in ip6map of primary global addr. eg no set addr.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfIpAddressAdd(L7_uint32 intIfNum, L7_in6_addr_t *ip6Address, 
                                 L7_uint32 prefixLen, L7_uint32 flags);

/*********************************************************************
* @purpose  remove an IP6 address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ip6Address       IP6 Address of the interface
* @param    prefixLen       length of prefix
* @param    flags           L7_IP6V6_ADDR_LINK_LOCAL_ONLY, L7_IP6V6_ADDR_EUI64
*
* @returns  L7_SUCCESS      ip6 address could be successfully configured
* @returns  L7_ERROR        subnet conflict between specified ip6
*                           address & an already configured ip6
*                           address
* @returns  L7_FAILURE      other errors or failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfIpAddressRemove(L7_uint32 intIfNum, L7_in6_addr_t *ip6Address, 
                                 L7_uint32 prefixLen, L7_uint32 flags);



/*********************************************************************
* @purpose  Get the forwarding mode of the router
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_RC_t  usmDbIp6RtrForwardingModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Enable or disable forwarding on the router
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrForwardingModeSet(L7_uint32 mode);



/*********************************************************************
* @purpose  Get interface mtu
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfIpv6MtuGet( L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Get effective interface mtu. Returns link mtu if ipv6 mtu
*           not configured. 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    pval      @b{(output)} ptr to value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfIpv6EffectiveMtuGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set interface mtu
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfIpv6MtuSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get interface bandwidth
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{output)} value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfBandwidthGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Register a routine to be called for routing events
*
* @param    routerProtocol_ID     routine router protocol id
*                                 (See L7_IP6MAP_REGISTRANTS_t)
* @param    *name                 pointer to the name of the registered routine,
*                                 up to IP6MAP_NOTIFY_FUNC_NAME_SIZE characters will be stored.
* @param    *notify               pointer to a routine to be invoked for
*                                 routing event changes as listed in
*                                 L7_RTR_EVENT_CHANGE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RegisterRoutingEventChange(L7_uint32 routerProtocol_ID, L7_char8 *name,
                                        L7_RC_t (*notify)(L7_uint32 intIfNum,
                                                          L7_uint32 event,
                                                          void *pData,
                                                          ASYNC_EVENT_NOTIFY_INFO_t *response));


/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID     routine router protocol id
*                                 (See L7_IP6MAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6DeregisterRoutingEventChange(L7_uint32 routerProtocol_ID);


/*********************************************************************
* @purpose  Get addresses of interface
*
* @param    intIfNum     router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_ipv6IntfAddr_t *pAddr);



/*********************************************************************
* @purpose  Get configured addresses of interface
*
* @param    rtrIntf      router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*           pPlen        ptr to array of prefix lens
*           llena_flag   ptr to link_local_enable flag
*           pEui_flag    ptr to eui flags
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrCfgIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_in6_addr_t *pAddr,L7_uint32 *pPlen,
                                     L7_uint32 *llena_flag, L7_uint32 *pEui_flag );

/*********************************************************************
* @purpose  Set router advertise Min adv interval
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvMinAdvIntSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router advertise Min adv interval
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvMinAdvIntGet( L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Set router advertise Max adv interval
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvMaxAdvIntSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router advertise Max adv interval
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvMaxAdvIntGet( L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Set router advertise lifetime
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvAdvLifetimeSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router advertise lifetime
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvAdvLifetimeGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set router advertise managed flag
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvManagedFlagSet( L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get router advertise Max adv interval
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvManagedFlagGet( L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set router advertise other config flag
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvOtherCfgFlagSet( L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get router other config flag
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvOtherCfgFlagGet( L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose Get rtr adv per prefix attributes
*         
*
* @param    intIfNum          internal interface
* @param    prefix            ptr to prefix
* @param    plen              prefix len
* @param    prefLife          preferred life
* @param    validLife         valid life
* @param    onLink            onlink flag
* @param    autonomous        autonomous flag
*
*
*
* @returns  L7_SUCCESS  
*           L7_FAILURE
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvPrefixGet(L7_uint32 intIfNum, 
                               L7_in6_addr_t *prefix, L7_uint32 plen,
                               L7_uint32 *prefLife, L7_uint32 *validLife,
                               L7_BOOL *onLink, L7_BOOL *autonomous);

/*********************************************************************
* @purpose Set rtr adv per prefix attributes
*         
*
* @param    intIfNum          internal interface
* @param    prefix            ptr to prefix
* @param    plen              prefix len
* @param    prefLife          preferred life
* @param    validLife         valid life
* @param    life_decr_flags   flags for lifetime decrement mode
* @param    onLink            onlink flag
* @param    autonomous        autonomous flag
*
*
*
* @returns  L7_SUCCESS  
*           L7_FAILURE
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvPrefixSet(L7_uint32 intIfNum, 
                               L7_in6_addr_t *prefix, L7_uint32 plen,
                               L7_uint32 prefLife, L7_uint32 validLife,
                               L7_ushort16 life_decr_flags, L7_BOOL onLink, L7_BOOL autonomous);

/*********************************************************************
* @purpose  Add a static route entry
*
* @param    *pStaticRoute @b{(input)}  Pointer to L7_rtr6StaticRoute_t structure
*
* @returns  L7_SUCCESS          If the route was successfully added to the
*                               configuration
* @returns  L7_FAILURE          If the specified IP address is invalid
* @returns  L7_ERROR            If the maximum number of next hops for the specified 
*                               network and route preference has been exceeded
* @returns  L7_TABLE_IS_FULL    If the maximum number of static routes has been 
*                               exceeded
*
*
* @notes    pStaticRoute contains a single next hop IP address. If a static
*           route already exists to the destination, the next hop is added to
*           the route, if the route does not already contain the maximum number 
*           of next hops.
*
*           If a static route already exists with the same destination and 
*           next hop, then the preference of that static route is updated.
*
*           The next hop is accepted even if it is not on a local subnet. The
*           next hop will be added to RTO, which determines whether to announce
*           it to the forwarding table, to ARP, and other RTO registrants.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6StaticRouteAdd(L7_rtr6StaticRoute_t *pStaticRoute);

/*********************************************************************
* @purpose  Delete a static route entry
*
* @param    *pStaticRoute   Pointer to L7_rtr6StaticRoute_t structure
*
*
* @returns  L7_SUCCESS if static route deleted
* @returns  L7_FAILURE if static route not found
*
* @notes    If pStaticRoute->nextHopRtr is 0, all next hops to the destination
*           are deleted.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6StaticRouteDelete(L7_rtr6StaticRoute_t *pStaticRoute);



/*********************************************************************
* @purpose  Return the preference of a static route to the default preference.
*
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    oldPref         Existing (non-default) preference of the route.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*                                                                                              
* @end
*********************************************************************/
L7_RC_t usmDbIp6StaticRoutePrefRevert(L7_in6_addr_t *ipAddr,
                                   L7_uint32 prefixLen, L7_uint32 oldPref);

/*********************************************************************
* @purpose  Get a list of all static routes.
*
* @param    staticEntries @b{(input/output)} An array of L7_RTR6_MAX_STATIC_ROUTES
*                      static route entries. On return, the array is filled with the
*                      static route entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This fuction populates an array of structure of type L7_routeEntry_t
            with all the static routes configured.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6StaticRouteGetAll(L7_route6Entry_t *staticEntries);

/*********************************************************************
* @purpose  Set IP6 Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6TraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get IP6 Mapping Layer tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6TraceModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Get IP6 Neighbor Entry
*
* @param    void
*
* @param    nbr        @b{(input/output)} pointer to table entry
*
* @notes    set nbr->intIfNum to 0 to get first, else pass previously
*           returned nbr to get next.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6NbrTblEntryGet(L7_ip6NbrTblEntry_t *nbr);

/*********************************************************************
* @purpose  mask interface ID portions of an address, leaving net num
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6MaskApply(L7_in6_addr_t *ip6Addr, L7_uint32 prefixLen);

/*********************************************************************
* @purpose  creates mask from prefixlen
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6PrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len);



/*********************************************************************
* @purpose  Return next route in the routing table. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
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
L7_RC_t usmDbNextRoute6EntryGet(L7_uint32 UnitIndex, 
                               L7_route6Entry_t *inOutRoute, L7_uint32 bestRouteOnly);

/*********************************************************************
* @purpose  Find the route with the longest matching prefix to a 
*           given destination. 
*
* @param    dest_ip     @b{(input)}  Destination IPv6 address.
* @param    routeEntry  @b{(output)} best route to the destination
*
*
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_SUCCESS  Router found
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t usmDbBestMatchRoute6Lookup(L7_uint32 UnitIndex,
                                   L7_in6_addr_t *dest_ip, L7_route6Entry_t *routeEntry);

/*********************************************************************
* @purpose  Returns the number of routes in the routing table. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    bestRoutesOnly  @b{(input)} If set to L7_TRUE, only best 
*                                       routes are counted.
*
* @returns  the number of routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 usmDbRoute6Count(L7_uint32 UnitIndex, L7_BOOL bestRoutesOnly);



/*********************************************************************
* @purpose  Set router advertise nd ns interval (ie retransmit)
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrNdNsIntervalSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router advertise nd ns interval (ie retransmit)
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrNdNsIntervalGet( L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Set router advertise nd reachable time
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrNdReachableTimeSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router advertise nd reachable time
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrNdReachableTimeGet( L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Set Dad transmits 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrDadTransmitsSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get dad transmits
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrDadTransmitsGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Enable or disable sending ICMPV6 Unreachables
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*                      or other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfICMPUnreachablesModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the mode of  sending ICMPV6 Unreachables
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mode     @b{(ouput)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*                      or other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrIntfICMPUnreachablesModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  setting ICMPv6 Rate Limiting parameters like Burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMP messages the IPMAP is allowed to per
                                   interval
* @param    interval   @b{(input)} The time interval between tokens being placed
                                   in the bucket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  To disable ICMPv6 rate limiting, set the interval to zero.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval);

/*********************************************************************
* @purpose  Get ICMPv6 Rate Limiting parameters like burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMPv6 messages the IPMAP is allowed to per
                                   interval
* @param    interval   @b{(input)} The time interval between tokens being placed
                                   in the bucket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval);

/**************************************************************************
*
* @purpose  Delete all NDP entries from the operating system NDP table.
*           Or single entry if ip6Addr not null.
*
* @param       intIfNum   internal interface number
* @param       ip6Addr    point to NDP address
*
* @returns     none
*
* @comments    
*
* @end
*
*************************************************************************/
void usmDbIp6NdpFlush( L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr);

/*********************************************************************
* @purpose  Set router advertise suppression
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvSuppressSet( L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get router advertise suppression
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} ptr to value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RtrRadvSuppressGet( L7_uint32 intIfNum, L7_BOOL *val);



/*********************************************************************
* @purpose  Return Router Preference
*
* @param    index   router protocol type
*
* @returns  The router preference associated with the specified
* @reutrns  protocol
*
* @notes    Ip state is always ENABLED
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RouterPreferenceGet(L7_uint32 index, L7_uint32 *pref);



/*********************************************************************
* @purpose  Change the default route preference for routes from a 
*           specific protocol.
*
* @param    index   protocol (one of L7_RTO6_PROTOCOL_INDICES_t)
* @param    pref    default preference for the protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE Preference value out of range
* @returns  L7_ERROR   Invalid preference change
* @returns  L7_ALREADY_CONFIGURED Preference value is in use by another
*                                 protocol
*
* @notes    All routes from the dynamic routing protocols use the 
*           default preference for that protocol. The user may configure
*           the preference of individual static and default routes. 
*           A change to the default preference for static or default 
*           routes will not change the preference of existing static 
*           or default routes. Static and default routes configured 
*           after the change will use the new default preference.
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6RouterPreferenceSet(L7_uint32 index, L7_uint32 pref);



/*********************************************************************
* @purpose  Clear interface stats from stack
*
* @param    intIfNum       internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6IfStatsClear( L7_uint32 intIfNum );

/*********************************************************************
* @purpose  mask interface ID portions of an address, leaving net num
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6MapMaskApply(L7_in6_addr_t *ip6Addr, L7_uint32 prefix_len);

/*********************************************************************
*
* @purpose Special Web formatting display for Loopback and tunnel interfaces
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFormatLoopbackTunnelInterfaceDisplay( L7_uint32 interface, L7_uchar8 *displayBuf);

/*********************************************************************
* @purpose  Find the interface number associated with a specific IPv6 address
*
*
* @param    ipv6Addr   @b{(input)} IPv6 address of router interface
* @param    intIfNum @b{(output)} Internal Interface Number
*
* @returns  L7_SUCCESS    if router interface found
* @returns  L7_FAILURE    otherwise
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6MapRtrAddressSpecificIntfGet (L7_in6_addr_t *ip6Address, L7_uint32 *intIfNum);

#endif /* USMDB_IP6_API_H */

