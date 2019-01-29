
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_ip6_api.h
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



#ifndef INCLUDE_IP6_API_H
#define INCLUDE_IP6_API_H


#include "osapi_sockdefs.h"
#include "async_event_api.h"
#include "ipv6_commdefs.h"
#include "l7_rto6_api.h"


/* async event types */
#define  RTR6_EVENT_ENABLE                1
#define  RTR6_EVENT_DISABLE_PENDING       2
#define  RTR6_EVENT_DISABLE               3
#define  RTR6_EVENT_INTF_ENABLE           4
#define  RTR6_EVENT_INTF_DISABLE_PENDING  5
#define  RTR6_EVENT_INTF_DISABLE          6
#define  RTR6_EVENT_INTF_ADDR_ADD         7
#define  RTR6_EVENT_INTF_ADDR_DEL         8
#define  RTR6_EVENT_INTF_MTU              9
#define  RTR6_EVENT_INTF_BW_CHANGE       10
#define  RTR6_EVENT_INTF_CREATE          11
#define  RTR6_EVENT_INTF_DELETE          12
#define  RTR6_EVENT_INTF_SPEED_CHANGE    13
#define  RTR6_EVENT_STARTUP_DONE         14
#define  RTR6_EVENT_INTF_ENABLE_HOST     15

/* Interface Capability Query Values */
typedef enum {
  IP6MAP_INTFCAP_RADV = 1,
  IP6MAP_INTFCAP_NDP,
  IP6MAP_INTFCAP_ROUTING_CONFIGURABLE,
} ip6IntfCapability_t;


/*-------------------------------------------------------------*/
/*                   ip6map_api.c                          */
/*-------------------------------------------------------------*/

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
L7_uint32  ip6MapRtrAdminModeGet(void);

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
L7_RC_t ip6MapRtrAdminModeSet(L7_uint32 mode);


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
L7_RC_t  ip6MapRtrDefaultTTLGet(L7_uint32 *valp);


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
L7_RC_t ip6MapRtrDefaultTTLSet( L7_uint32 val);

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
L7_RC_t ip6MapRtrIntfModeGet( L7_uint32 intIfNum, L7_uint32 *mode);
 
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
L7_RC_t ip6MapRtrIntfOperModeGet( L7_uint32 intIfNum, L7_uint32 *mode);
 

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
L7_RC_t ip6MapRtrIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode);


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
L7_RC_t ip6MapRtrIntfIpAddressAdd(L7_uint32 intIfNum, L7_in6_addr_t *ip6Address, 
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
L7_RC_t ip6MapRtrIntfIpAddressRemove(L7_uint32 intIfNum, L7_in6_addr_t *ip6Address, 
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
L7_uint32  ip6MapRtrForwardingModeGet(void);

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
L7_RC_t ip6MapRtrForwardingModeSet(L7_uint32 mode);


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
L7_RC_t ip6MapRegisterRoutingEventChange(L7_uint32 routerProtocol_ID, L7_char8 *name,
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
L7_RC_t ip6MapDeregisterRoutingEventChange(L7_uint32 routerProtocol_ID);


/*********************************************************************
* @purpose  Get addresses of interface
*
* @param    rtrIntf      router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_ipv6IntfAddr_t *pAddr);

/*********************************************************************
* @purpose  Determine if specified IP Address lies in the local subnet
*           for the given interface
*
* @param    intIfNum      Internal interface number
* @param    ipAddr        IP Address to be checked
*
* @returns  L7_TRUE    The IP Address lies in the local subnet
* @returns  L7_FALSE   The IP Address does not lie in the local subnet
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ip6MapIsLocalAddr(L7_uint32 intIfNum, L7_in6_addr_t *ipAddr);

/*********************************************************************
* @purpose  Get active addresses of interface
*
* @param    rtrIntf      router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*           pLen         ptr to array of prefix lens
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    called by registered applications after address events
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfActiveAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, 
                                        L7_in6_addr_t *pAddr, L7_uint32 *pPlen);


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
L7_RC_t ip6MapRtrCfgIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_in6_addr_t *pAddr,L7_uint32 *pPlen,
                                     L7_uint32 *llena_flag, L7_uint32 *pEui_flag );

/*********************************************************************
* @purpose  Get prefixlen of a particular interface address.
*
* @param    intIfNum     router interface number
*           prefixLen    ptr to prefixLen (to be filled in by function)
*           pAddr        ptr to array of addresses
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfAddrPrefixLenGet(L7_uint32 intIfNum, L7_uint32 *prefixLen, L7_in6_addr_t *pAddr);

/*********************************************************************
* @purpose  Get link local address of interface
*
* @param    rtrIntf      router interface
*           pAddr        ptr to array of addresses
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfActiveLinkLocalAddrGet(L7_uint32 intIfNum, L7_in6_addr_t *pAddr);

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
L7_RC_t ip6MapRtrRadvMinAdvIntSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrRadvMinAdvIntGet( L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t ip6MapRtrRadvMaxAdvIntSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrRadvMaxAdvIntGet( L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t ip6MapRtrRadvAdvLifetimeSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrRadvAdvLifetimeGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set router preference
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} low|medium|high
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrRadvRtrPrefSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get router preference
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
L7_RC_t ip6MapRtrRadvRtrPrefGet( L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ip6MapRtrIntfIpv6MtuGet( L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get interface mtu for the ospfv3 application.
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
L7_RC_t ip6MapRtrIntfIpv6EffectiveMtuGet( L7_uint32 intIfNum, L7_uint32 *pval);

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
L7_RC_t ip6MapRtrIntfIpv6MtuSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Gets the interface bandwidth
*
* @param    intIfNum        Internal Interface Number
* @param    bandwidth       bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  ip6MapIntfBandwidthGet(L7_uint32 intIfNum, L7_uint32 *intfBandwidth);

/*********************************************************************
* @purpose  Determine if IP6 interface exists
*
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE   
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ip6MapIpIntfExists(L7_uint32 intIfNum);

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
L7_RC_t ip6MapRtrRadvManagedFlagSet( L7_uint32 intIfNum, L7_BOOL val);

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
L7_RC_t ip6MapRtrRadvManagedFlagGet( L7_uint32 intIfNum, L7_BOOL *val);

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
L7_RC_t ip6MapRtrRadvOtherCfgFlagSet( L7_uint32 intIfNum, L7_BOOL val);

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
L7_RC_t ip6MapRtrRadvOtherCfgFlagGet( L7_uint32 intIfNum, L7_BOOL *val);

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
L7_RC_t ip6MapRtrRadvPrefixGet(L7_uint32 intIfNum, 
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
* @param    devr_life_flags   flags for life decrement mode
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
L7_RC_t ip6MapRtrRadvPrefixSet(L7_uint32 intIfNum, 
                               L7_in6_addr_t *prefix, L7_uint32 plen,
                               L7_uint32 prefLife, L7_uint32 validLife,
                               L7_ushort16 decr_life_flags, L7_BOOL onLink, L7_BOOL autonomous);

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
L7_RC_t ip6MapStaticRouteAdd(L7_rtr6StaticRoute_t *pStaticRoute);

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
L7_RC_t ip6MapStaticRouteDelete(L7_rtr6StaticRoute_t *pStaticRoute);

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
L7_RC_t ip6MapStaticRoutePrefRevert(L7_in6_addr_t *ipAddr,
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
L7_RC_t ip6MapStaticRouteGetAll(L7_route6Entry_t *staticEntries);

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
void ip6MapTraceModeSet(L7_uint32 mode);

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
L7_uint32 ip6MapTraceModeGet(void);

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
L7_RC_t ip6MapNbrTblEntryGetNext(L7_ip6NbrTblEntry_t *nbr);

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
L7_RC_t ip6MapMaskApply(L7_in6_addr_t *ip6Addr, L7_uint32 prefixLen);

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
void ip6MapPrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len);

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
L7_uint32 ip6MapRouterPreferenceGet(L7_uint32 index);


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
L7_RC_t ip6MapRouterPreferenceSet(L7_uint32 index, L7_uint32 pref);


/*********************************************************************
* @purpose  Convert Router Interface Number to Internal Interface Number
*
* @param    rtrIntf     router interface number
* @param    *intIfNum   pointer to internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfToIntIfNum(L7_uint32 rtrIfNum, L7_uint32* intIfNum);

/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number 
*
* @param    intIfNum   internal interface number
* @param    *rtrIntf   pointer to router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
*       
* @end
*********************************************************************/
L7_RC_t ip6MapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum);

/*********************************************************************
* @purpose  Determine if the interface type is valid for participation
*           in the component
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL ip6MapIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid to participate in the component
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
L7_BOOL ip6MapIntfIsValid(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Get SNMP general group
*
*           grpEnt      in/out ptr to snmp group entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpGeneralGroupGet(ipstkIpv6GeneralGroup_t *gEnt);

/*********************************************************************
* @purpose  Get SNMP interface entry
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp interface entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpIfEntryGet(L7_uint32 type, ipstkIpv6IfEntry_t *ifEnt);

/*********************************************************************
* @purpose  Get SNMP interface entry: private extensions
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp interface entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpIfEntryExtGet(L7_uint32 type, ipstkIpv6IfEntryExt_t *ifEnt);

/*********************************************************************
* @purpose  Get SNMP addresses
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp address entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpAddrEntryGet(L7_uint32 type, ipstkIpv6AddrEntry_t *addrEnt);

/*********************************************************************
* @purpose  Get SNMP addresses
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp address entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpAddrEntryGetNext(L7_uint32 type, ipstkIpv6AddrEntry_t *addrEnt);

/*********************************************************************
* @purpose  Get SNMP address prefixes
*
* @param    type         NEXT or EXACT
*           pfxEnt      in/out ptr to snmp address prefix entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpAddrPrefixEntryGet(L7_uint32 type, ipstkIpv6AddrPrefixEntry_t *pfxEnt);

/*********************************************************************
* @purpose  Return next best static route in the config table.
*
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpStaticRouteGetNext (ipstkIpv6RouteEntry_t *nextRt);

/*********************************************************************
* @purpose  Return next best static route in the config table. 
*
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found. 
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. 
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapSnmpStaticRouteGet (ipstkIpv6RouteEntry_t *nextRt);

/*********************************************************************
* @purpose  Determine whether the specified interface supports the
*           specified capability.
*
* @param    intIfNum      Internal interface number
* @param    capability    Capability to be checked
*
* @returns  L7_TRUE    The interface supports the capability
* @returns  L7_FALSE   The interfaces does not support the capability
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ip6MapIntfSupports(L7_uint32 intIfNum, ip6IntfCapability_t capability);

/*********************************************************************
* @purpose  convert RTR6 events to ascii string
*
* @param    event    event to convert
*
* @returns  void
*
* @end
*********************************************************************/
L7_char8 *ip6MapEventStringGet(L7_uint32 event);


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
L7_RC_t ip6MapRtrNdNsIntervalSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrNdNsIntervalGet( L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t ip6MapRtrNdReachableTimeSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrNdReachableTimeGet( L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t ip6MapRtrDadTransmitsSet( L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ip6MapRtrDadTransmitsGet( L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ip6MapRtrIntfICMPUnreachablesModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the mode of  sending ICMPV6 Unreachables
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mode     @b{(output)} L7_ENABLE or L7_DISABLE
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
L7_RC_t ip6MapRtrIntfICMPUnreachablesModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMPv6 Echo Replies
*
* @param    mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrICMPEchoReplyModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  setting ICMPv6 Rate Limiting parameters like burst size and interval.
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
L7_RC_t ip6MapRtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval);

/*********************************************************************
* @purpose  Get ICMPv6 Rate Limiting parameters like Burst size and interval.
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
L7_RC_t  ip6MapRtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval);

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
void ip6MapNdpFlush( L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr);


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
L7_RC_t ip6MapRtrRadvSuppressSet( L7_uint32 intIfNum, L7_BOOL val);

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
L7_RC_t ip6MapRtrRadvSuppressGet( L7_uint32 intIfNum, L7_BOOL *val);


/*********************************************************************
* @purpose  Insert an entry into the gateway table
*
* @param    ip6Addr      gateway IP address to add to table
* @param    intIfnum    gateway interface num
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
*       
* @end
*********************************************************************/
L7_RC_t ip6MapNdpGwTableInsert(L7_in6_addr_t *ip6Addr, L7_uint32 intIfnum);


/*********************************************************************
* @purpose  Remove an entry from the gateway table
*
* @param    ip6Addr      gateway IP address to remove from table
* @param    intIfNum    internal interface number associated with
*                       NDP entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapNdpGwTableRemove(L7_in6_addr_t *ip6Addr, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  get interface stats from stack and hardware
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapIfStatsGet( L7_uint32 searchType, ipstkIpv6IfStats_t *vars);

/*********************************************************************
* @purpose  get the count of ipv6 neighbors
*
* @param    nbrCount @b{(output)} number of ipv6 neighbors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapNbrCountGet(L7_uint32 *nbrCount);

/*********************************************************************
* @purpose  get interface stats from stack and hardware
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapIfIcmpStatsGet( L7_uint32 searchType, ipstkIpv6IfIcmpStats_t *vars);

/*********************************************************************
* @purpose  clear interface stats from stack and hardware
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    saves a copy of current stats values for later subtraction
*
* @end
*********************************************************************/
L7_RC_t ip6MapIfStatsClear( L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Check to see if the ip6 address conflicts with that of
*           any other routing interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    gpHandle        general prefix handle
* @param    ip6Address       IP6 Address of the interface
* @param    prefixLen       Prefix Length
* @param    flags           address flags
* @param    prefixCheckOnly Flag to indicate check the prefix part only
*                           or the complete address for conflict
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with any of the above mentioned ip6
*                           addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfAddressConflict(L7_uint32 intIfNum, L7_uint32 gpHandle,
                                     L7_in6_addr_t *ip6Address, L7_uint32 prefixLen,
                                     L7_uint32 flags, L7_BOOL prefixCheckOnly);

/*********************************************************************
* @purpose  Add a general prefix
*
* @param    name @b{(input)}  prefix name
* @param    ip6Addr @b{(input)} destination prefix
* @param    prefixlen @b{(input)}   prefix len
* @param    isPD @b{(input)} boolean true if set by prefix delegation
*
* @returns  L7_SUCCESS or L7_FAILURE, also returns handle
*
* @end
*********************************************************************/
L7_RC_t ip6MapGenPrefixAdd(L7_uchar8 *name, L7_in6_addr_t *ip6Addr, 
                           L7_uint32 prefixLen, L7_BOOL isPD);

/*********************************************************************
* @purpose  Determine if IP Address is a configured Router Interface
*
* @param    ip6Addr      IP address of desired router interface
* @param    *intIfNum   pointer to internal interface number of routing intf
*
* @returns  L7_SUCCESS  router interface found for specified IP address
* @returns  L7_FAILURE  router interface not found for specified IP address
*
* @notes    This works
*           from the configuration data, and does not depend on the
*           current admin mode setting or whether the configuration
*           is applied or not.
*
* @end
*********************************************************************/
L7_RC_t ip6MapIpAddrIsCfgRtrIntf(L7_in6_addr_t *ip6Addr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Find the nexthop interface for a global address
*
* @param    ip6Address      IP6 Address to resolve
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with any of the above mentioned ip6
*                           addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRouterIfResolve( L7_in6_addr_t *ip6Address, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Indicate that a protocol intends to add routes to RTO6 following 
*           a warm restart.
*
* @param    protocol   @b{(input)}  protocol to add NSF routes
*
* @returns  L7_SUCCESS 
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t ip6MapNsfRouteSource(L7_NSF_PROTOCOL_t protocol);

/*********************************************************************
* @purpose  A protocol that controls NSF routes calls this function to 
*           indicate it has updated RTO6 with all its initial routes after
*           a warm restart.
*
* @param    protocol   @b{(input)}  protocol to add NSF routes
*
* @returns  L7_SUCCESS 
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t ip6MapInitialRoutesDone(L7_NSF_PROTOCOL_t protocol);

/*********************************************************************
* @purpose  Add the IPv6 address of a neighbor to the checkpoint table.
*
* @param    ip6Addr     IPv6 address of neighbor
* @param    intIfnum    interface where neighbor is attached. Non-zero if link local.
* @param    isAdded     L7_TRUE if neighbor has been added. L7_FALSE if neighbor has
*                       been deleted.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL if entry not added because checkpoint table is full
*
* @notes    
*
* @end
*********************************************************************/
void ip6MapCheckpointNeighbor(L7_in6_addr_t *nbrAddr, L7_uint32 intIfNum, L7_BOOL isAdded);

/*********************************************************************
* @purpose  Add the IPv6 address of a neighbor to the checkpoint table.
*
* @param    ip6Addr     IPv6 address of neighbor
* @param    intIfnum    interface where neighbor is attached. Non-zero if link local.
* @param    isAdded     L7_TRUE if neighbor has been added. L7_FALSE if neighbor has
*                       been deleted.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL if entry not added because checkpoint table is full
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapNdpCkptEntryAdd(L7_in6_addr_t *nbrAddr, L7_uint32 intIfNum, L7_BOOL isAdded);

/*********************************************************************
* @purpose  Add the IPv6 address learned via dhcp6c to ip6map
*
* @param    ifName      intreface name
* @param    ip6Addr     IPv6 address of neighbor
* @param    plen        prefix len
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapIntfDhcpAddrAdd(L7_uchar8 *ifName, L7_in6_addr_t *ip6Addr, L7_uint32 plen);

/*********************************************************************
* @purpose  Delete the IPv6 address learned via dhcp6c to ip6map
*
* @param    ifName      intreface name
* @param    ip6Addr     IPv6 address of neighbor
* @param    plen        prefix len
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapIntfDhcpAddrDel(L7_uchar8 *ifName, L7_in6_addr_t *ip6Addr, L7_uint32 plen);

/*********************************************************************
* @purpose  Get intifnum of single dhcp client
*
* @param    intIfNum    (out) ptr to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    used by cli to enforce single dhcp client intf
*
* @end
*********************************************************************/
L7_RC_t ip6MapDhcpClientIntIfNumGet(L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get previous dhcp addr after warm restart
*
* @param    intIfNum    (in) interface id
* @param    ip6Addr     (out) ptr to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapIntfDhcpAddrGet(L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr);

/*********************************************************************
* @purpose  Get the interface ipv6 address configuration mode
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *automode   @b{(output)} Autoconfig mode
* @param    *dhcpmode   @b{(output)} DHCP Client mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfConfigModeGet(L7_uint32 intIfNum, L7_uint32 *automode,
                                   L7_uint32 *dhcpmode);


#endif /* INCLUDE_IP6_API_H */

