/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   simapi.h
*
* @purpose    System Interface Manager API Functions, Constants and Data Structures
*
* @component  sim
*
* @comments
*
* @create     08/21/2000
*
* @author     bmutz
*
* @end
*
**********************************************************************/

#ifndef _SIMAPI_H_
#define _SIMAPI_H_

#include "l7_cnfgr_api.h"
#include "l3_addrdefs.h"
#include "ipstk_mib_api.h"
#include "usmdb_timezone_api.h"
#include "timezone_exports.h"
#include "sim_exports.h"

/*********************************************************************
* @purpose  Get the Unit's Ipc IP Address
*
*
* @returns  ipAddr
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetIpcIpAddr(void);

/*********************************************************************
* @purpose  Get the Unit's system name
*
* @param    *name  @b{(output)} system name, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemName(L7_char8 *name);

/*********************************************************************
* @purpose  Sets the Unit's System Name
*
* @param    *name  @b{(input)} system name, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemName(L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the time zone minutes parameter
*
* @param    minutes       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneOffsetSet(L7_int32 minutes);

/*********************************************************************
*
* @purpose  Set the time zone acronym parameter
*
* @param    acronym       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneAcronymSet(L7_char8 *acronym);

/*********************************************************************
*
* @purpose  Get the time zone minutes parameter
*
* @param    minutes       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneOffsetGet(L7_int32 *minutes);

/*********************************************************************
*
* @purpose  Get the time zone acronym parameter
*
* @param    acronym       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneAcronymGet(L7_char8 *acronym);

/*********************************************************************
*
* @purpose  Get the summer-time mode
*
* @param  ct(output)summer-time mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStModeGet(L7_int32 *mode);

/*********************************************************************
*
* @purpose  Get the summer-time mode
*
* @param  ct(output)summer-time mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStModeGet(L7_int32 *mode);

/*********************************************************************
*
* @purpose  Set the summer-time mode
*
* @param  ct(input)summer-time mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStModeSet(L7_int32 mode);


/*********************************************************************
*
* @purpose  Get non recurring summertime parameters
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t simStNonRecurringGet(L7_int32 parameter,void *data);


/*********************************************************************
*
* @purpose  Get summertime recurring parameters
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t simStRecurringGet(L7_int32 parameter,void *data);


/*********************************************************************
* @purpose  Set the  recurring summer-time parameters
*
*
* @returns   L7_SUCCESS
*            L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t simStRecurringSet(L7_int32 parameter, void *ptr);

/*********************************************************************
* @purpose  Set the  non recurring summer-time parameters
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t simStNonRecurringSet(L7_int32 parameter, void *ptr);

/**********************************************************************
* @purpose  know whether the summer-time is in effect or not.
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL simSummerTimeIsInEffect();

/**********************************************************************
* @purpose  compare two split-apart dates
*
* @returns  >0 if b is greater
*           <0 if a is greater
*            0 if both are equal
*
* @end
*********************************************************************/
L7_int32 simCompareDateAndTime(L7_uint32 year_a, L7_uint32 month_a, L7_uint32 day_a, L7_uint32 minute_a,
                               L7_uint32 year_b, L7_uint32 month_b, L7_uint32 day_b, L7_uint32 minute_b);

/**********************************************************************
* @purpose  calculates the ordinal day of the month
*
* @returns  
*
* @end
*********************************************************************/
L7_uint32 simFindDayOfWeek(L7_uint32 mday, L7_uint32 wday, L7_uint32 ordinal, L7_uint32 day);

/**********************************************************************
* @purpose  know whether the summer-time is in effect or not.
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @end
*********************************************************************/
L7_uint32 simAdjustedTimeGet();

/*********************************************************************
* @purpose  Get the Unit's System Location
*
* @param    *location  @b{(output)} system location, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemLocation(L7_char8 *location);

/*********************************************************************
* @purpose  Sets the Unit's System Location
*
* @param    *location  @b{(input)} system location, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemLocation(L7_char8 *location);

/*********************************************************************
* @purpose  Get the Unit's System Contact
*
* @param    *contact  @b{(output)} System Contact, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemContact(L7_char8 *contact);

/*********************************************************************
* @purpose  Sets the Unit's system Contact
*
* @param    *contact  @b{(input)} System Contact, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemContact(L7_char8 *contact);

/*********************************************************************
* @purpose  Get the Unit's System Object ID
*
* @param    *sysOID   @b{(output)} System Object
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemOID(L7_uchar8 *sysOID);

/*********************************************************************
* @purpose  Get the value of the number of services primarily
*           offered by this entity.
*
* @param    *val  @b{(output)} Number of services
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemServices(L7_int32 *val);

/*********************************************************************
* @purpose  Get the configured IPv4 address on the network port
*
* @param    none
*
* @returns  network port IPv4 address
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simConfiguredSystemIPAddrGet(void);

/*********************************************************************
* @purpose  Get the Unit's System IP Address
*
* @param    none
*
* @returns  ipAddr   System's IP Address
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemIPAddr(void);

/*********************************************************************
* @purpose  Configure an IPv4 address on the network port
*
* @param    ipAddr   @b{(input)} System IPv4 Address
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemIPAddrConfigure(L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Configure an IPv4 address with its network mask on the network port
*
* @param    ipAddr   @b{(input)} System IPv4 Address
* @param    netMask  @b{(input)} network mask
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments  Better to set the address and mask together when possible
*
* @end
*********************************************************************/
L7_RC_t simConfigureSystemIPAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask);

/*********************************************************************
* @purpose  Determine whether the network port IP address is a checkpointed
*           address, not yet confirmed as a valid address following failover.
*
* @param    none
*
* @returns  L7_TRUE if the network port address is a checkpointed address
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL simIsSystemAddrCheckpointed(void);

/*********************************************************************
* @purpose  Set DHCP or BOOTP address and network mask on the network port.
*
* @param    ipAddr   @b{(input)} Network port IPv4 address
* @param    netMask  @b{(input)} network mask
*
* @returns  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSetSystemAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask);


/*********************************************************************
* @purpose  Get the Unit's System NetMask
*
* @param    none
*
* @returns  netMask   System's NetMask
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemIPNetMask(void);

/*********************************************************************
* @purpose  Configure the network mask on the network port
*
* @param    netMask   @b{(input)} System NetMask
*
* @returns  none
*
* @comments  
*
* @end
*********************************************************************/
void simSystemIPNetMaskConfigure(L7_uint32 netMask);

/*********************************************************************
* @purpose  Get the network mask configured on the network port
*
* @param    void
*
* @returns  network mask on the network port
*
* @comments   
*
* @end
*********************************************************************/
L7_uint32 simConfiguredSystemIPNetMaskGet(void);

/*********************************************************************
* @purpose  API for DHCP or BOOTP to set the network mask for the IPv4 
*           address on the network port.
*
* @param    netMask   @b{(input)} System NetMask
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemIPNetMask(L7_uint32 netMask);

/*********************************************************************
* @purpose  Get the Unit's System Gateway
*
* @param    none
*
* @returns  gateway  System's Gateway
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemIPGateway(void);

/*********************************************************************
* @purpose  Get the configured IPv4 default gateway on the network port
*
* @param    void
*
* @returns  IPv4 address of the default gateway  
*
* @comments  This is the configured value, not the operational value.
*
* @end
*********************************************************************/
extern L7_uint32 simConfiguredSystemIPGatewayGet(void);

/*********************************************************************
* @purpose  Sets the default gateway associated with the network port 
*           as learned from DHCP or BOOTP.
*
* @param    newGateway  @b{(input)} IPv4 address of new default gateway on network port
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemIPGateway(L7_uint32 newGateway);

/*********************************************************************
* @purpose  Configure the IPv4 default gateway for the network port.
*
* @param    newGateway  @b{(input)} new System Gateway
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSystemIPGatewayConfigure(L7_uint32 newGateway);

/*********************************************************************
 * @purpose  Get the Unit's System (Network Port) IPV6 Admin maode
 *
 * @param    none
 *
 * @returns  adminMode    L7_ENABLE or L7_DISABLE
 *
 * @comments
 *
 * @end
 *********************************************************************/
extern L7_uint32 simGetSystemIPV6AdminMode(void);                                                                                                                                     
/*********************************************************************
 * @purpose  Sets the Unit's System (Network Port) IPV6 Admin Mode
 *
 * @param    adminMode     @b{(input)} L7_ENABLE or L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
extern L7_RC_t simSetSystemIPV6AdminMode(L7_uint32 adminMode);

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on network port interface
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
extern L7_RC_t simGetSystemIPV6Addrs( L7_in6_prefix_t *addrs,
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
extern L7_RC_t simGetNextSystemIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                           L7_uint32 *ip6PrefixLen,
                                           L7_uint32 *eui_flag);

/**************************************************************************
 * @purpose  Get the auto-configured link-local address of the management port
 *
 * @param    ip6Addr        @b{(input)}   IPv6 address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    This function updates the ip6Addr argument with the
 *              autoconfigured eui-64 format link local address of
 *              the management port (service port or network port),
 *              irrespective of the ipv6 state of the mgmt interface.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetSystemLinkLocalIPV6Addr (L7_in6_addr_t *ip6Addr);

/**************************************************************************
 * @purpose  Sets one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t simSetSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, 
                                       L7_uint32 ip6PrefixLen,
                                       L7_uint32 eui_flag,
                                       L7_uint32 force);

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
extern L7_RC_t simDeleteSystemIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                          L7_uint32 ip6PrefixLen,
                                          L7_uint32 eui_flag);

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on network port interface
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
extern L7_RC_t simGetSystemIPV6DefaultRouters( L7_in6_addr_t *addrs,
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
extern L7_RC_t simGetSystemIPV6Gateway( L7_in6_addr_t *gateway);

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on network port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t simSetSystemIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force);

/*********************************************************************
* @purpose  Set the DHCPv6 operational address on the service port.
*
* @param    ip6Addr  @b{(input)} IPv6 address on service port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simServPortDhcpv6AddrSet(L7_in6_addr_t* ip6Addr);

/*********************************************************************
* @purpose  Clear the DHCPv6 operational address on the service port.
*           This is called when we disable DHCPv6 Client protocol on
*           the service port once we released the address.
*
* @param    None
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simServPortDhcpv6AddrClear(void);

/*********************************************************************
* @purpose  Get the DHCPv6 operational address on the service port
*
* @param    ip6Addr  @b{(output)} IPv6 address on service port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simServPortDhcpv6AddrGet(L7_in6_addr_t* ip6Addr);

/*********************************************************************
* @purpose  Set the DHCPv6 operational address on the network port.
*
* @param    ip6Addr  @b{(input)} IPv6 address on network port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSystemDhcpv6AddrSet(L7_in6_addr_t* ip6Addr);

/*********************************************************************
* @purpose  Clear the DHCPv6 operational address on the network port.
*           This is called when we disable DHCPv6 Client protocol on
*           the network port once we released the address.
*
* @param    None
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSystemDhcpv6AddrClear(void);

/*********************************************************************
* @purpose  Get the DHCPv6 operational address on the network port
*
* @param    ip6Addr  @b{(output)} IPv6 address on network port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSystemDhcpv6AddrGet(L7_in6_addr_t* ip6Addr);

/*********************************************************************
* @purpose  Get the System Mac Address
*
* @param    *sysMacAddr  @b{(output)} pointer to system mac address
*                              Length L7_MAC_ADDR_LEN
*
* @returns  none
*
* @comments This API will retrieve the correct system MAC address and
*           should be used by most applications so there is no need to
*           check the Mac address type before calling either
*           simGetSystemIPBurnedInMac or simGetSystemIPLocalAdminMac.
*
* @end
*********************************************************************/
extern void simMacAddrGet(L7_uchar8 *sysMacAddr);

/*********************************************************************
* @purpose  Get the Unit's System Burned in Mac Address
*
* @param    *sysBIA   @b{(output)} pointer to system burned in mac address
*                              Length L7_MAC_ADDR_LEN
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemIPBurnedInMac(L7_uchar8 *sysBIA);


/*********************************************************************
* @purpose  Set the system mac-address to the local burned-in mac address
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
void simRestoreSystemMac(void);

/*********************************************************************
* @purpose  Get the Unit's Service Port's Burned in Mac Address
*
* @param    *servPortBIA   @b{(output)} pointer to service port burned in mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetServicePortBurnedInMac(L7_uchar8 *servPortBIA);

/*********************************************************************
* @purpose  Get the Unit's System Locally Administered Mac Address
*
* @param    *sysLAA   @b{(output)} pointer to system local admin mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simGetSystemIPLocalAdminMac(L7_uchar8 *sysLAA);

/*********************************************************************
* @purpose  Sets the Unit's System Locally Administered Mac Address
*
* @param    *sysLAA   @b{(input)} pointer to system local admin mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemIPLocalAdminMac(L7_uchar8 *sysLAA);

/*********************************************************************
* @purpose  Get the Unit's System Mac Address Type
*
* @param    none
*
* @returns  sysMacType   System Mac Address Type
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemIPMacType(void);

/*********************************************************************
* @purpose  Sets the Unit's System Mac Address Type
*
* @param    type   @b{(input)} System Mac Address Type
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemIPMacType(L7_uint32 type);

/*********************************************************************
* @purpose  Get the Unit's service port IP Address
*
* @param    none
*
* @returns  ipAddr   service port IP Address
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortIPAddr(void);

/*********************************************************************
* @purpose  Get configured value for service port IPv4 address
*
* @param    void
*
* @returns  configured service port IPv4 address
*
* @comments  Not necessarily the value being used operationally on 
*            the service port.
*
* @end
*********************************************************************/
extern L7_uint32 simConfiguredServPortIPAddrGet(void);

/*********************************************************************
* @purpose  Configure an IPv4 address on the service port
*
* @param    ipAddr   @b{(input)} Service port IPv4 address
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortIPAddrConfigure(L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Configure an IPv4 address with its network mask on the service port
*
* @param    ipAddr   @b{(input)} Service port IPv4 address
* @param    netMask  @b{(input)} network mask
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments  Better to set the address and mask together when possible
*
* @end
*********************************************************************/
L7_RC_t simConfigureServPortIPAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask);

/*********************************************************************
* @purpose  Set DHCP or BOOTP address and network mask on the service port.
*
* @param    ipAddr   @b{(input)} Service port IPv4 address
* @param    netMask  @b{(input)} network mask
*
* @returns  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSetServPortAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask);


/*********************************************************************
* @purpose  Determine whether the service port address is a checkpointed 
*           address, not yet confirmed to be valid following a failover.
*
* @param    none
*
* @returns  L7_TRUE if service port address is a checkpointed address
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL simIsServPortAddrCheckpointed(void);

/*********************************************************************
 * @purpose  Get the Unit's Service Port IPV6 Admin maode
 *
 * @param    none
 *
 * @returns  adminMode    L7_ENABLE or L7_DISABLE
 *
 * @comments
 *
 * @end
 *********************************************************************/
extern L7_uint32 simGetServPortIPV6AdminMode(void);           

/*********************************************************************
 * @purpose  Sets the Unit's Service Port IPV6 Admin Mode
 *
 * @param    adminMode     @b{(input)} L7_ENABLE or L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
extern L7_RC_t simSetServPortIPV6AdminMode(L7_uint32 adminMode);

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on service port interface
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
extern L7_RC_t simGetServPortIPV6Addrs( L7_in6_prefix_t *addrs,
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
extern L7_RC_t simGetNextServPortIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                             L7_uint32 *ip6PrefixLen,
                                             L7_uint32 *eui_flag);

/**************************************************************************
 * @purpose  Sets one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
extern L7_RC_t simSetServPortIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                         L7_uint32 ip6PrefixLen,
                                         L7_uint32 eui_flag,
                                         L7_uint32 force);


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
extern L7_RC_t simDeleteServPortIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                            L7_uint32 ip6PrefixLen,
                                            L7_uint32 eui_flag);


/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on service port interface
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
extern L7_RC_t simGetServPortIPV6DefaultRouters( L7_in6_addr_t *addrs,
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
extern L7_RC_t simGetServPortIPV6Gateway( L7_in6_addr_t *gateway);

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on service port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
extern L7_RC_t simSetServPortIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force);


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
extern L7_RC_t simGetServPortIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated, 
                               L7_BOOL         *ipv6IsRtr);

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
extern L7_RC_t simGetSystemIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated, 
                               L7_BOOL         *ipv6IsRtr);


/*********************************************************************
* @purpose  Get the Unit's service port NetMask
*
* @param    none
*
* @returns  netMask   service port NetMask
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortIPNetMask(void);

/*********************************************************************
* @purpose  Configure the network mask on the network port
*
* @param    netMask   @b{(input)} System NetMask
*
* @returns  none
*
* @comments  
*
* @end
*********************************************************************/
void simServPortIPNetMaskConfigure(L7_uint32 netMask);

/*********************************************************************
* @purpose  Get the network mask configured on the network port
*
* @param    void
*
* @returns  network mask on the network port
*
* @comments   
*
* @end
*********************************************************************/
L7_uint32 simConfiguredServPortIPNetMaskGet(void);

/*********************************************************************
* @purpose  Get the configured IPv4 default gateway on the service port
*
* @param    void
*
* @returns  IPv4 address of the default gateway  
*
* @comments  This is the configured value, not the operational value.
*
* @end
*********************************************************************/
extern L7_uint32 simConfiguredServPortIPGatewayGet(void);

/*********************************************************************
* @purpose  Configure the IPv4 default gateway for the service port.
*
* @param    newGateway  @b{(input)} IPv4 address of the default gw
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simServPortIPGatewayConfigure(L7_uint32 newGateway);

/*********************************************************************
* @purpose  Get the Unit's service port Gateway
*
* @param    none
*
* @returns  gateway  service port Gateway
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortIPGateway(void);

/*********************************************************************
* @purpose  Sets the IPv4 address of the default gateway on the service port
*
* @param    newGateway  @b{(input)} service port default gw
*
* @returns  none
*
* @comments  used by DHCP client and BOOTP
*
* @end
*********************************************************************/
extern void simSetServPortIPGateway(L7_uint32 newGateway);

/*********************************************************************
* @purpose  Get the Unit's System Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemConfigMode(void);

/*********************************************************************
* @purpose  Get the Unit's System IPv6 Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemIPv6ConfigMode(void);

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortIPv6ConfigMode(void);

/*********************************************************************
* @purpose  Get the Unit's Service Port Admin state
*
* @param    none
*
* @returns  mode  Service Port admin state
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortAdminState(void);

/*********************************************************************
* @purpose  Sets the Unit's Service Port Admin State
*
* @param    state  @b{(input)} Service Port Admin State
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortAdminState(L7_uint32 state);

/*********************************************************************
* @purpose  Get the Unit's Service Port Link state
*
* @param    none
*
* @returns  mode  Service Port link state
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortLinkState(void);

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t simServPortIPv6AddrAutoConfigGet(L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the ipv6 address autoconfiguration is enabled on the network port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t simServPortIPv6AddrAutoConfigSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the Unit's System Current Config Mode
*
* @param    none
*
* @returns  mode  System Current Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSystemCurrentConfigMode(void);

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t simSystemIPv6AddrAutoConfigGet(L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the ipv6 address autoconfiguration is enabled on the service port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t simSystemIPv6AddrAutoConfigSet(L7_uint32 val);

/*********************************************************************
* @purpose  Sets the Unit's System Config Mode
*
* @param    mode  @b{(input)} System Config Mode
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSystemConfigMode(L7_SYSCFG_MODE_t mode);

/*********************************************************************
* @purpose  Sets the Unit's System IPv6 Config Mode
*
* @param    mode  @b{(input)} System Config Mode
*
* @returns L7_FAILURE    When any of this is true
*                        - the ipv6 address autoconfiguration is enabled on the network port
*                        - the ipv6 address autoconfiguration is enabled on the service port
*                        - the dhcpv6 protocol is enabled on the service port
*          L7_SUCCESS    otherwise
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSetSystemIPv6ConfigMode(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortConfigMode(void);

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Current Config Mode
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetServPortCurrentConfigMode(void);

/*********************************************************************
* @purpose  Sets the Unit's Service Port Config Mode
*
* @param    mode  @b{(input)} Service Port Config Mode
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetServPortConfigMode(L7_SYSCFG_MODE_t mode);

/*********************************************************************
* @purpose  Sets the Unit's Service Port Config Mode
*
* @param    mode  @b{(input)} Service Port Config Mode
*
* @returns L7_FAILURE    When any of this is true
*                        - the ipv6 address autoconfiguration is enabled on the service port
*                        - the ipv6 address autoconfiguration is enabled on the network port
*                        - the dhcpv6 protocol is enabled on the network port
*          L7_SUCCESS    otherwise
*
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSetServPortIPv6ConfigMode(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the Unit's Serial Port Baudrate
*
* @param    none
*
* @returns  baudRate Unit's Serial Port Baudrate
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSerialBaudRate(void);

/*********************************************************************
* @purpose  Sets the Unit's Serial Port Baudrate
*
* @param    baudRate   @b{(input)} Unit's Serial Port Baudrate
*
* @returns  L7_SUCCES
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t simSetSerialBaudRate(L7_uint32 baudRate);

/*********************************************************************
* @purpose  Get the Unit's Serial Port TimeOut
*
* @param    none
*
* @returns  timeOut  Unit's Serial Port TimeOut
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetSerialTimeOut(void);

/*********************************************************************
* @purpose  Sets the Unit's Serial Port TimeOut
*
* @param    timeOut    @b{(input)} Unit's Serial Port TimeOut
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSerialTimeOut(L7_uint32 timeOut);

/*********************************************************************
* @purpose  Sets the Serial Port Parameter
*
* @param    parm        @b{(input)} serial port parameter to be set
* @param    newValue    @b{(input)} new value of port parameter
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
extern void simSetSerialPortParm(L7_uint32 parm, L7_uint32 newValue);

/*********************************************************************
* @purpose  Get this unit number
*
* @param    none
*
* @returns  unitID      The stack ID for this unit
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 simGetThisUnit();

/*********************************************************************
*
* @purpose  Get the transfer mode
*
* @param    none
*
* @returns  System transfer mode of type enum L7_TRANSFER_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simGetTransferMode(void);

/*********************************************************************
*
* @purpose  Set the transfer mode
*
* @param    val   @b{(input)} Transfer mode of type enum L7_TRANSFER_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferMode(L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the Ip Address type of the Tftp Server
*
* @param    val  @b{(output)} pointer to tftp server Ip Address type
*
* @returns  Ip Address of tftp server
*
* @comments none
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferServerAddressType(L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the Ip Address of the Tftp Server
*
* @param    val  @b{(input)} tftp server Ip Address type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferServerAddressType(L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the Ip Address of the Tftp Server
*
* @param    val  @b{(output)} pointer to tftp server Ip Address data
*
* @returns  Ip Address of tftp server
*
* @comments none
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferServerIp(L7_uchar8 *val);

/*********************************************************************
*
* @purpose  Set the Ip Address of the Tftp Server
*
* @param    val  @b{(input)} tftp server Ip Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferServerIp(L7_uchar8 *val);

/*********************************************************************
*
* @purpose  Get the transfer file path for the local file
*
* @param    *buf  @b{(output)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFilePathLocal(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the local file name for the transfer
*
* @param    *buf  @b{(output)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferLocalFileName(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Set the local file name for the transfer
*
* @param    *buf  @b{(input)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferLocalFileName(L7_char8 *buf);


/*********************************************************************
*
* @purpose  Set the transfer file path for the local file
*
* @param    *buf  @b{(input)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFilePathLocal(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the transfer file name for the local file
*
* @param    *buf  @b{(output)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFileNameLocal(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Set the transfer file name for the local file
*
* @param    *buf  @b{(input)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFileNameLocal(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the transfer file path for the remote file
*
* @param    *buf  @b{(output)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFilePathRemote(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Set the transfer file path for the remote file
*
* @param    *buf  @b{(input)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFilePathRemote(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the transfer file name for the remote file
*
* @param    *buf  @b{(output)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFileNameRemote(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Set the transfer file name for the remote file
*
* @param    *buf  @b{(input)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFileNameRemote(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the transfer upload file type
*
* @param    none
*
* @returns  Upload file type of enum L7_FILE_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simGetTransferUploadFileType(void);

/*********************************************************************
*
* @purpose  Set the transfer upload file type
*
* @param    val  @b{(input)} Transfer upload file of type enum L7_FILE_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferUploadFileType(L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the transfer download file type
*
* @param    none
*
* @returns  Download file type of enum L7_FILE_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simGetTransferDownloadFileType(void);

/*********************************************************************
*
* @purpose  Set the transfer download file type
*
* @param    val  @b{(input)} transfer download file of type enum L7_FILE_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferDownloadFileType(L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the status of the transfer
*
* @param    none
*          
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_BOOL simTransferInProgressGet();

/*********************************************************************
*
* @purpose  Set the status of the transfer
*          
* @param    val   @b{(input)} The context for the transfer of type L7_BOOL
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simTransferInProgressSet(L7_BOOL val);

/*********************************************************************
*
* @purpose  Gets the context of the transfer
*
* @param    none
*          
* @returns  pointer of the context for this transfer  
*
* @comments
*
* @end
*
*********************************************************************/
extern void* simTransferContextGet();

/*********************************************************************
*
* @purpose  Set the trasfer context
*          
* @param    *context @b{(input)} The context of the transfer
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
extern void simTransferContextSet(void *context);

/*********************************************************************
*
* @purpose  Get the management VLAN ID
*          
* @param    none
*
* @returns  management VLAN ID
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simMgmtVlanIdGet();

/*********************************************************************
*
* @purpose  Set the management VLAN ID
*          
* @param    mgmtVlanId   @b{(input)} management vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments Assumes valid VLAN ID
*
* @end
*
*********************************************************************/
extern L7_RC_t simMgmtVlanIdSet(L7_uint32 mgmtVlanId);

/*********************************************************************
*
* @purpose  Get the management Port
*
* @param    none
*
* @returns  management Port
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simMgmtPortGet();

/*********************************************************************
*
* @purpose  Set the management Port
*
* @param    @b{(input)} management port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments Assumes valid 
*
* @end
*
*********************************************************************/
extern L7_RC_t simMgmtPortSet(L7_uint32 interface);

/*********************************************************************
* @purpose  Get MIB capability description
*
* @param    Index   @b{(input)}  Index
* @param    buf     @b{(output)} description
*
* @return   L7_SUCCESS
*
* @note     none 
*
* @end
*********************************************************************/
extern L7_RC_t simMibDescription(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
* @purpose  Tell SIM to override serial port rate with the setting
*       passed to this function.
*
* @param    baud    Baud rate.
*
* @returns  none
*
* @notes    This function may be used to coordinate the baud rate
*       between boot code and the operational code.
*       
* @end
*********************************************************************/
void simSerialBaudOverride (L7_BAUDRATES_t baud_rate);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by invoquin the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*
* @notes    This function runs in the configurator. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
void simApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );


/*********************************************************************
* @purpose  To check if the route formed with the arguments is the same
*           as the local route formed by the serv port or network port
*           ip address and mask.
*
* @param    ipAddr    The network of the route
* @param    netmask   Netmask of the route
* @param    gateway   Gateway of the route
* @param    routeCmd  Route command of the route which is add, delete or modify
*
* @returns  L7_SUCCESS  The route is not the same
*           L7_FAILURE  The route is the same
*
* @notes    If the route is the same, the arguements are stored
*       
* @end
*********************************************************************/
extern L7_RC_t simRouteModifyCheck(L7_uint32 ipAddr, L7_uint32 netmask,
                                   L7_uint32 gateway, L7_uint32 routeCmd);

/*********************************************************************
*
* @purpose  Get the unit number for the transfer
*
* @param    *unit  @b{(output)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferUnitNumber(L7_uint32 *unit);

/*********************************************************************
*
* @purpose  Set the unit number for the transfer
* 
* @param    unit  @b{(input)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferUnitNumber(L7_uint32 unit);
/*********************************************************************
* @purpose  Register a routine to be called when a system address
*           changes
*
* @param    registrar_ID  Routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
* @param    *notify       pointer to a routine to be invoked for address
*                         changes.  Each routine has the following parameters:
*                         registrar_ID, addrType, addrFamily
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simAddrChangeEventRegister(L7_COMPONENT_IDS_t      registrar_ID,
                                   SIM_ADDR_CHANGE_TYPE_t  addrType,
                                   L7_uchar8               addrFamily,
                                   SIM_ADDR_CHANGE_CB      addrChangeCB);
/*********************************************************************
* @purpose  Deregister a routine to be called when system address
*           changes
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simAddrChangeEventDeRegister(L7_COMPONENT_IDS_t      registrar_ID,
                                     SIM_ADDR_CHANGE_TYPE_t  addrType,
                                     L7_uchar8               addrFamily);

/*********************************************************************
* @purpose  Get the Unit's system mac address which is currently in use.
*
* @param    *name  @b{(output)} system mac, length L7_ENET_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simGetSystemMac(L7_uchar8 *mac);


/*********************************************************************
 *
 * @purpose Gets bytes transferred for the file 
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferBytesCompletedGet(L7_uint32 *bytes);

/*********************************************************************
 *
 * @purpose Sets bytes transferred for the file 
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferBytesCompletedSet(L7_uint32 bytes);

/*********************************************************************
 *
 * @purpose Gets completion status for transfer
 *
 * @param completed
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferCompletionStatusGet(L7_BOOL *completed);

/*********************************************************************
 *
 * @purpose Sets completion status for transfer
 *
 * @param completed
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferCompletionStatusSet(L7_BOOL completed);

/*********************************************************************
 *
 * @purpose Gets the direction of the transfer
 *
 * @param direction
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferDirectionGet(L7_uint32 *direction);

/*********************************************************************
 *
 * @purpose Sets the direction of the transfer
 *
 * @param direction
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void simTransferDirectionSet(L7_uint32 direction);

/*********************************************************************
*
* @purpose Set the history buffer size for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
extern L7_RC_t simSerialHistoryBufferSizeSet(L7_uint32 historyBufferSize);

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSerialHistoryBufferSizeGet(L7_uint32 *bufferSize);
/*********************************************************************
*
* @purpose  serial history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSerialHistoryBufferEnableSet(L7_uint32 enable);

/*********************************************************************
*
* @purpose  serial history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSerialHistoryBufferEnableGet(L7_uint32 *enable);

/*********************************************************************
*
* @purpose Set the history buffer size for telnet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
extern L7_RC_t simTelnetHistoryBufferSizeSet(L7_uint32 historyBufferSize);

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simTelnetHistoryBufferSizeGet(L7_uint32 *bufferSize);
/*********************************************************************
*
* @purpose  Telnet history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simTelnetHistoryBufferEnableSet(L7_uint32 enable);

/*********************************************************************
*
* @purpose  Telnet history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simTelnetHistoryBufferEnableGet(L7_uint32 *enable);

/*********************************************************************
*
* @purpose Set the Timeout for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
extern L7_RC_t simSshHistoryBufferSizeSet(L7_uint32 historyBufferSize);
/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSshHistoryBufferSizeGet(L7_uint32 *bufferSize);
/*********************************************************************
*
* @purpose  Ssh history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSshHistoryBufferEnableSet(L7_uint32 enable);

/*********************************************************************
*
* @purpose  Ssh history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t simSshHistoryBufferEnableGet(L7_uint32 *enable);

/*********************************************************************
*
* @purpose Set the username for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FTP Password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFTPUserName(L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the username for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FTP Username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFTPUserName(L7_char8 *buf);


/*********************************************************************
*
* @purpose Set the password for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FPAssword for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferFTPPassword(L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the password for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) Password for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t simGetTransferFTPPassword(L7_char8 *buf);

/*********************************************************************
*
* @purpose  Set the Notify function to be called upon Transfer complete
*
* @param    void  (*notify)(L7_TRANSFER_STATUS_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
extern void simTransferCompleteNotification(L7_TRANSFER_STATUS_t status);

/*********************************************************************
*
* @purpose  Set the Notify function to be called upon Transfer complete
*
* @param    void  (*notify)(L7_TRANSFER_STATUS_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSetTransferCompleteNotification(void (*notifyFuncPtr)(L7_TRANSFER_STATUS_t status));

/*********************************************************************
*
* @purpose  Get the status of Mgmt Access
*
* @param    none
*          
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_BOOL simTransferMgmtAccessGet();

/*********************************************************************
*
* @purpose  Set the status of Mgmt Access
*          
* @param    val   @b{(input)} The context for the transfer of type L7_BOOL
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
extern void simTransferMgmtAccessSet(L7_BOOL val);

/*********************************************************************
*
* @purpose  Get the stack up time
*
* @param    none
*          
* @returns  stack up time in seconds
*
* @comments  Stack up time is the time since the stack performed a cold
*            restart. Stack up time does not reset on a warm restart.
*
* @end
*
*********************************************************************/
extern L7_uint32 simSystemUpTimeGet(void);

/*********************************************************************
*
* @purpose  Get the stack up time in milliseconds
*
* @param    none
*          
* @returns  stack up time (milliseconds)
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_uint32 simSystemUpTimeMsecGet(void);

/*********************************************************************
*
* @purpose  Set the System Up Time
*          
* @param    sysUpTime   @b{(input)} The system up time
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t simSystemUpTimeSet(L7_uint32 systemUpTime);

/*********************************************************************
*
* @purpose  Adjust the System Up Time
*          
* @param    resetType   @b{(input)} Event causing the systemUpTime adjustment
*
* @returns  L7_SUCCESS
*
* @comments Upon election of a new manager, the system up time is bumped
*           by 5 seconds to ensure the clock doesn't drift backwards.
*           On cold restart, it is set to zero to reflect the time since
*           the last cold restart.
*
* @end
*
*********************************************************************/
L7_RC_t simSystemUpTimeAdjust(L7_RESET_TYPE resetType);

/*********************************************************************
* @purpose  Clears the IP stack's ARP cache entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All the IP stack's ARP cache entries are cleared.
*
* @end
*********************************************************************/
L7_RC_t simArpSwitchClear(void);

/*********************************************************************
*
* @purpose  Trigger the Active Address Conflict Detection
*          
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectRun(void);

/*********************************************************************
*
* @purpose  Clear the Address Conflict Detection Status
*          
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectStatusClear(void);

/*********************************************************************
*
* @purpose  Get the Address Conflict Detection Status
*          
* @param    conflictDetectStatus   @b{(output)} conflict detection Status
*
* @returns  L7_SUCCESS
*
* @comments status would be returned as L7_TRUE if there was at least
*           one conflict detected since last reset of the status.
*           Else status is returned as L7_FALSE.
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectStatusGet(L7_BOOL *conflictDetectStatus);

/*********************************************************************
*
* @purpose  Get the IP Address of the last detected address Conflict
*          
* @param    conflictIP   @b{(output)} last detected conflicting IP
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectIPGet(L7_uint32 *conflictIP);

/*********************************************************************
*
* @purpose  Set the IP Address of the last detected address Conflict
*          
* @param    conflictIP   @b{(output)} last detected conflicting IP
* @param    conflictMAC  @b{(output)} MAC of last detected
*                                     conflicting host
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectIPMacSet(L7_uint32 conflictIP,
                                        L7_uchar8 *conflictMAC);

/*********************************************************************
*
* @purpose  Get the MAC Address of the last detected address conflict
*          
* @param    conflictMAC   @b{(output)} MAC of last detected
*                                      conflicting host
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectMACGet(L7_uchar8 *conflictMAC);

/*********************************************************************
*
* @purpose  Get the time in seconds since the last address conflict
*           was detected
*          
* @param    conflictDetectTime   @b{(output)} time since the last
*                                             detected conflict
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectTimeGet(L7_uint32 *conflictDetectTime);

/*********************************************************************
*
* @purpose  Register for the address conflict on mgmt interface(s)
*
* @param    cbFunc   @b{(input)} callback function
*
* @returns  L7_SUCCESS  If function was registered
* @returns  L7_FAILURE  If function was not registered
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictRegister(simIPAddrConflictCB_t cbFunc);

/*********************************************************************
* @purpose  Unregisters for address conflict on mgmt interface(s)
*
* @param    cbFunc   @b{(input)} Callback function
*
* @returns  L7_SUCCESS  If function was unregistered
* @returns  L7_FAILURE  If function was not unregistered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simIPAddrConflictUnregister(simIPAddrConflictCB_t cbFunc);


/*********************************************************************
* @purpose  Set the CPU Free memory threshold for monitoring
*
* @param    threshold {(input)} Free memory threshold in KB. A value of 0 
*                               indicates that threshold monitoring should be
*                               stopped.
*
* @returns L7_SUCCESS If threshold was successfully set
*          L7_REQUEST_DENIED If the threshold is configured to be more than the
*                            total available memory
*          L7_FAILURE Other errors
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t simCpuFreeMemoryThresholdSet(L7_uint32 threshold);


/*********************************************************************
* @purpose  Get the configured CPU Free memory threshold
*
* @param    threshold {(output)} Free memory threshold in KB. A value of 0 
*                                indicates that threshold monitoring should be
*                                stopped.
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t simCpuFreeMemoryThresholdGet(L7_uint32 *threshold);


/*********************************************************************
* @purpose  Monitor free memory and generate traps as required.
*
* @param    none
*
* @returns  void
*
* @notes    Called periodically by the osapiMonitorTask. Generates trap
*           if free memory falls below the user configured threshold. Also,
*           a snapshot of the total memory allocated by each component and
*           details of last x allocations are recorded in persistent memory.
*           Another trap is generated when the free memory has recovered
*           SIM_FREE_MEMORY_RECOVERED_THRESHOLD % over the configured threshold.
*
* @end
*********************************************************************/
void simMemoryMonitor(void);


/*********************************************************************
* @purpose  Set a CPU Util monitor parameter
*
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(input)} Value of the parameter
*
* @returns L7_SUCCESS If parameter was successfully set
*          L7_FAILURE Other errors
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t simCpuUtilMonitorParamSet(simCpuUtilMonitorParam_t paramType,
                                  L7_uint32 paramVal);


/*********************************************************************
* @purpose  Get a CPU Util monitor parameter
*
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(output)} Value of the parameter
*
* @returns L7_SUCCESS 
*          L7_FAILURE 
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t simCpuUtilMonitorParamGet(simCpuUtilMonitorParam_t paramType,
                                  L7_uint32 *paramVal);

/****************************************
*
*  SIM Tasks                    
*
*****************************************/
/*need to fix */
extern int    L7_transfer_task();
extern void   simAddrConflictTask();

/*********************************************************************
*
* @purpose  Get the Image semaphore
*
* @param    wait         wait time
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t simImageSemaTake(L7_uint32 wait);

/*********************************************************************
*
* @purpose  Release the Image semaphore
*
* @param    None
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t simImageSemaGive();

/*********************************************************************
* @purpose  Checks if Startup-config exists or not
*
* @param    none
*
* @returns  L7_TRUE if exists,
*           L7_FALSE otherwise.
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL simStartupConfigIsExists (void);

/*********************************************************************
* @purpose  Check all conditions that must be met to consider the 
*           network port up.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    There must be a non-zero management VLAN ID and at least 
*           one port in the management VLAN must be active.
*
* @end
*********************************************************************/
L7_BOOL simMayEnableNetworkPort(void);

#endif  /* _SIMAPI_H_ */

