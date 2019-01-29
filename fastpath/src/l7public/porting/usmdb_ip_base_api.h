/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_ip_base_api.h
*
* @purpose externs for USMDB layer 
*
* @component unitmgr
*
* @comments ip common functions between esa201(base) and esa300
*
* @create 04/29/2001
*
* @author Diogenes De Los Santos
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

  

#ifndef USMDB_IP_BASE_API_H
#define USMDB_IP_BASE_API_H
#include "l7_common.h"
#include "l3_addrdefs.h"

/*********************************************************************
*
* @purpose Retrieve the active IPv4 address on the network port 
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) IPv4 Address
*
* @returns L7_SUCCESS
*
* @notes 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfAddressGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 * @purpose Get the configured IPv4 address on the network port
 *
 * @param   networkPortIpAddr   @b((output)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentConfiguredIpIfAddressGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Retrieve the network mask for the active IPv4 address on
*          the network port.
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Returns the System NetMask
*
* @returns L7_SUCCESS
*
* @notes 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfNetMaskGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the configured IPv4 network mask for the network port
 *
 * @param   netmask  @b((output)) Returns the network port NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentConfiguredIpIfNetMaskGet(L7_uint32 *netmask);

/*********************************************************************
*
* @purpose Retrieve the Unit's System Gateway
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) System Gateway
*
* @returns L7_SUCCESS
*
* @notes 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfDefaultRouterGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 * @purpose Retrieve the configured IPv4 default gateway for the network port
 *
 * @param   defGw  @b((output)) the IPv4 default gateway on the network port
 *
 * @returns L7_SUCCESS
 *
 * @notes   
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentConfiguredIpIfDefaultRouterGet(L7_uint32 *defGw);

/* @p1261 start */
/*********************************************************************
*
* @purpose Set the Unit's System IP Address and NetMask
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 ipAddr    @b((input))  IP Address
* @param L7_uint32 NetMask   @b((input))  NetMask
* @param L7_uint32 errorNum  @b((output)) Localized error message number to display if L7_FAILURE is returned
*
* @returns L7_SUCCESS
*
* @notes IP Address is 4 bytes long
* @notes if netmask set fails, will attempt to set IP address to old value
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbSystemIPAndNetMaskSet(L7_uint32 UnitIndex,
                                          L7_uint32 ipAddr,
                                          L7_uint32 netMask,
                                          L7_uint32 *errorNum);

/* @p1261 end */

/*********************************************************************
*
* @purpose Set the Unit's System IP Address 
*          
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 ipAddr1   @b((input)) IP Address
*
* @returns L7_SUCCESS
*
* @notes IP Address is 4 bytes long
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfAddressSet(L7_uint32 UnitIndex, L7_uint32 ipAddr1);

/*********************************************************************
*
* @purpose Set the Unit's System NetMask
*          
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) System NetMask 
*
* @returns L7_SUCCESS
*
* @notes System NetMask is 4 bytes long
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfNetMaskSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Set the Unit's System Gateway
*          
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) System Gateway 
*
* @returns L7_SUCCESS
*
* @notes System Gateway is 4 bytes long
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentIpIfDefaultRouterSet(L7_uint32 UnitIndex, L7_uint32 val);

/**************************************************************************
 *
 * @purpose  Get IPv6 Admin Mode for the network port interface
 *
 * @param      none
 *
 * @returns    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
extern L7_uint32 usmDbAgentIpIfIPV6AdminModeGet( void );
                                                                                                                                     
/**************************************************************************
 *
 * @purpose  Set IPv6 Admin Mode for the network port interface
 *
 * @param    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
extern L7_uint32 usmDbAgentIpIfIPV6AdminModeSet( L7_uint32 adminMode );


/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on network port
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6AddrsGet(L7_in6_prefix_t *addrs,
                                          L7_uint32 *acount);


/**************************************************************************
 * @purpose  Get next of the Unit's network port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6PrefixGetNext (L7_in6_addr_t *ip6Addr,
                                                L7_uint32 *ip6PrefixLen,
                                                L7_uint32 *eui_flag);


/**************************************************************************
 * @purpose  Sets one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6PrefixAdd (L7_in6_addr_t *ip6Addr,
                                            L7_uint32 ip6PrefixLen,
                                            L7_uint32 eui_flag);


/**************************************************************************
 * @purpose  Removes one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6PrefixRemove (L7_in6_addr_t *ip6Addr,
                                               L7_uint32 ip6PrefixLen,
                                               L7_uint32 eui_flag);


/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on network port
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6DefaultRoutersGet(L7_in6_addr_t *addrs,
                                                   L7_uint32 *acount);

/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on network port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6GatewayGet( L7_in6_addr_t *gateway);


/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on network port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbAgentIpIfIPV6GatewaySet( L7_in6_addr_t *gateway);


/*********************************************************************
* @purpose  get network port NDisc info from stack
*
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state 
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbAgentIpIfIPV6NdpGet( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr);


/*********************************************************************
* @purpose  ping operation
*          
* @param    L7_char8  *ipHost @b((input)) Pointer to the hosts IP address
* @param    L7_uint32 count   @b((input)) Number of times to issue a ping request  
*	
* @returns 	Number of packets echoed 
*
* @notes    none
*
* @end
*
*********************************************************************/
extern L7_uint32 usmDBPing(L7_uint32 UnitIndex, L7_char8 *ipHost, L7_uint32 count);

/*********************************************************************
*
* @purpose Set the service port gateway
*
* @param   L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param   L7_uint32 val        @b((input)) Service port gateway
*
* @returns L7_SUCCESS
*
* @notes gateway is 4 bytes long
*
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortGatewaySet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the service port Gateway
*          
* @param   L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param   L7_uint32 *val       @b((output)) Service port Gateway
*
* @returns L7_SUCCESS
*
* @notes service port Gateway is 4 bytes long
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortGatewayGet(L7_uint32 UnitIndex, L7_uint32 *val);


/* @p1261 start */
/*********************************************************************
*
* @purpose Set the Unit's Service Port IP Address and NetMask
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 ipAddr    @b((input))  IP Address
* @param L7_uint32 netMask   @b((input))  NetMask
* @param L7_uint32 errorNum  @b((output)) Localized error message number to display
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes IP Address is 4 bytes long
* @notes if netmask set fails, will attempt to set IP address to old value
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortIPAndNetMaskSet(L7_uint32 UnitIndex,
                                               L7_uint32 ipAddr,
                                               L7_uint32 netMask,
                                               L7_uint32 *errorNum);


/* @p1261 end */

/*********************************************************************
*
* @purpose Set the service port IP Address
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) IP Address
*
* @returns L7_SUCCESS
*
* @notes 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortIPAddrSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the service port IP Addr
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Service port IP Addr
*
* @returns L7_SUCCESS
*
* @notes 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortIPAddrGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 * @purpose Get the configured IPv4 address on the service port
 *
 * @param   networkPortIpAddr   @b((output)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbServicePortConfiguredIpAddrGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the service port NetMask
*          
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Service port NetMask 
*
* @returns L7_SUCCESS
*
* @notes  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortNetMaskSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the service port NetMask
*          
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Service port NetMask 
*
* @returns L7_SUCCESS
*
* @notes  NetMask is 4 bytes long
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbServicePortNetMaskGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the configured IPv4 network mask for the service port
 *
 * @param   netmask  @b((output)) Returns the service port network mask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbServicePortConfiguredNetMaskGet(L7_uint32 *netmask);

/*********************************************************************
 * @purpose Retrieve the configured IPv4 default gateway for the service port
 *
 * @param   netmask  @b((output)) Returns the service port default gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbServicePortConfiguredGatewayGet(L7_uint32 *defGw);


/**************************************************************************
 *
 * @purpose  Get IPv6 Admin Mode for the service port interface
 *
 * @param      none
 *
 * @returns    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
extern L7_uint32 usmDbServicePortIPV6AdminModeGet( void );
                                                                                                                                     
/**************************************************************************
 *
 * @purpose  Set IPv6 Admin Mode for the service port interface
 *
 * @param    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
extern L7_uint32 usmDbServicePortIPV6AdminModeSet( L7_uint32 adminMode );


/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on serviceport
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbServicePortIPV6AddrsGet(L7_in6_prefix_t *addrs, 
                                            L7_uint32 *acount);


/**************************************************************************
 * @purpose  Get next of the Unit's service port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbServicePortIPV6PrefixGetNext (L7_in6_addr_t *ip6Addr,
                                                  L7_uint32 *ip6PrefixLen,
                                                  L7_uint32 *eui_flag);


/**************************************************************************
 * @purpose  Sets one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbServicePortIPV6PrefixAdd (L7_in6_addr_t *ip6Addr,
                                              L7_uint32 ip6PrefixLen,
                                              L7_uint32 eui_flag);


/**************************************************************************
 * @purpose  Removes one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t usmDbServicePortIPV6PrefixRemove (L7_in6_addr_t *ip6Addr,
                                                 L7_uint32 ip6PrefixLen,
                                                 L7_uint32 eui_flag);


/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on serviceport
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbServicePortIPV6DefaultRoutersGet(L7_in6_addr_t *addrs,
                                                     L7_uint32 *acount);


/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on service port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbServicePortIPV6GatewayGet( L7_in6_addr_t *gateway);


/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on service port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t usmDbServicePortIPV6GatewaySet( L7_in6_addr_t *gateway);


/*********************************************************************
* @purpose  get service port NDisc info from stack
*
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state 
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbServicePortIPV6NdpGet( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr);



#endif
