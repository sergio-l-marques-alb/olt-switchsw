/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename   l7_dhcps_api.h
 *
 * @purpose    DHCP Server Mapping Layer APIs
 *
 * @component  DHCP Server Mapping Layer
 *
 * @comments   none
 *
 * @create     09/15/2003
 *
 * @author     athakur
 * @end
 *
 **********************************************************************/

#ifndef _DHCPS_MAP_API_H_
#define _DHCPS_MAP_API_H_

#include "dhcps_exports.h"
#include "dhcps_exports.h"

/* Begin Function Prototypes */
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
 * @purpose  Get DHCP Server admin mode
 *
 * @param    mode        @b{(output)} admin mode
 *
 * @returns  L7_SUCCESS  admin mode Get
 * @returns  L7_FAILURE  Failed to Get admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapAdminModeGet(L7_uint32 *mode);

/*********************************************************************
 * @purpose  Set DHCP Server admin mode
 *
 * @param    mode        @b{(input)} admin mode
 *

 * @returns  L7_FAILURE  Failed to set admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapAdminModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Get DHCP host number of ping packets
 *
 * @param    pNoOfPingPkt @b{(output)} number of ping packets
 *
 * @returns  L7_SUCCESS  Number of ping packets Got
 * @returns  L7_FAILURE  Failed to get number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNoOfPingPktGet(L7_uint32 *pNoOfPingPkt);

/*********************************************************************
 * @purpose  Set DHCP host number of ping packets
 *
 * @param    noOfPingPkt @b{(input)} number of ping packets
 *
 * @returns  L7_SUCCESS  Number of ping packets set
 * @returns  L7_FAILURE  Failed to set number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNoOfPingPktSet(L7_uint32 noOfPingPkt);

/*********************************************************************
 * @purpose  Get the number of active leases statistics for a DHCP Server
 *
 * @param    pNoActiveLeases @b{(output)} number of active leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfActiveLeasesGet( L7_uint32  *pNoActiveLeases);

/*********************************************************************
 * @purpose  Get the number of active dynamic leases statistics
 *
 * @param    pNoActiveDynamicLeases @b{(output)} number of active dynamic leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveDynamicLeasesGet( L7_uint32  *pNoActiveDynamicLeases);

/*********************************************************************
 * @purpose  Get the number of active manual leases statistics
 *
 * @param    pNoActiveManualLeases @b{(output)} number of active manual leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveManualLeasesGet( L7_uint32  *pNoActiveManualLeases);

/*********************************************************************
 * @purpose  Get the number of active bootp leases statistics
 *
 * @param    pNoActiveBootpLeases @b{(output)} number of active bootp leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveBootpLeasesGet( L7_uint32  *pNoActiveBootpLeases);

/*********************************************************************
 * @purpose  Get the number of free leases statistics for a DHCP Server
 *
 * @param    pNoFreeLeases @b{(output)} number of free leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfFreeLeasesGet( L7_uint32  *pNoFreeLeases);

/*********************************************************************
 * @purpose  Get the number of expired leases statistics for a DHCP Server
 *
 * @param    pNoExpiredLeases @b{(output)} number of expired leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfExpiredLeasesGet( L7_uint32  *pNoExpiredLeases);

/*********************************************************************
 * @purpose  Get the number of abandoned leases statistics for a DHCP Server
 *
 * @param    pNoAbandonedLeases @b{(output)} number of abandoned leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfAbandonedLeasesGet( L7_uint32  *pNoAbandonedLeases);

/*********************************************************************
 * @purpose  Get the number of offered leases statistics for a DHCP Server
 *
 * @param    pNoOfferedLeases @b{(output)} number of offered leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfpNoOfferedLeasesGet( L7_uint32  *pNoOfferedLeases);

/*********************************************************************
 * @purpose  Clear the  statistics for a DHCP Server
 *
 * @param
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Resets packet counters only
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsStatisticsClear();

/*********************************************************************
 * @purpose  Set DHCP lease time
 *
 * @param    leaseTime   @b{(input)} lease time (in minutes)
 * @param    pPoolName   @b{(input)} Pool Name
 *
 * @returns  L7_SUCCESS  lease time set
 * @returns  L7_FAILURE  Failed to set lease time
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapLeaseTimeSet(L7_char8 *pPoolName, L7_uint32 leaseTime);

/*********************************************************************
 * @purpose  Get DHCP lease time
 *
 * @param    leaseTime   @b{(output)} lease time (in minutes)
 * @param    pPoolName   @b{(input)} Pool Name
 *
 * @returns  L7_SUCCESS  lease time received
 * @returns  L7_FAILURE  Failed to get lease time
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapLeaseTimeGet(L7_char8 *pPoolName, L7_uint32 *pLeaseTime);

/*********************************************************************
 * @purpose  Create a DHCP address pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool created
 * @returns  L7_FAILURE  Failed to create Dhcp address pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolCreate(L7_char8 *pPoolName);

/*********************************************************************
 * @purpose  Delete a DHCP address pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool deleted
 * @returns  L7_FAILURE  Failed to delete Dhcp address pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolDelete(L7_char8 *pPoolName);

/*********************************************************************
 * @purpose  Map the Lease data structures to existing pools
 *
 *
 * @returns  L7_TRUE       If Success
 * @returns  L7_FALSE      Otherwise
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapInitMappingLeaseConfigData();

/*************************************************************
 * @purpose  Get the first entry in  DHCP address pool
 *
 * @param    pPoolIndex   @b{(input)} Unit for this operation
 * @param    pPoolName     @b{(output)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool first entry found
 * @returns  L7_FAILURE  Dhcp address pool first entry not found
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolEntryFirst(L7_char8 *pPoolName, L7_uint32 *pPoolIndex );

/******************************************************************
 * @purpose  Get the next entry in  DHCP address pool
 *
 * @param    pPoolIndex   @b{(input)} Unit for this operation
 * @param    pPoolName     @b{(output)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool first entry found
 * @returns  L7_FAILURE  Dhcp address pool first entry not found
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolEntryNext(L7_char8 *pPoolName, L7_uint32 *pPoolIndex );

/*********************************************************************
 * @purpose  Set DHCP host network  for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    ipAddr      @b{(input)} ipAddress for a pool
 * @param    ipMask      @b{(input)} netmask for a pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask set
 * @returns  L7_FAILURE  Failed to set ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNetworkSet(L7_char8 *pPoolName, L7_uint32 ipAddr, L7_uint32 ipMask);

/*********************************************************************
 * @purpose  Get DHCP host network  for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    ipAddr      @b{(output)} ipAddress for a pool
 * @param    ipMask      @b{(output)} netmask for a pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask set
 * @returns  L7_FAILURE  Failed to set ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNetworkGet(L7_char8 *pPoolName, L7_uint32 *pIpAddr, L7_uint32 *pIpMask);

/*********************************************************************
 * @purpose  Delete the DHCP Server network ip and mask for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkDelete(L7_char8 *pPoolName );


/*********************************************************************
 * @purpose  Set DHCP host DNS Servers  for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    DnsServers     @b{(input)} Array of ipAddress of DNS Servers
 *
 * @returns  L7_SUCCESS  DNS servers set
 * @returns  L7_FAILURE  Failed to set DNS Servers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapDnsServersSet(L7_char8 *pPoolName, L7_uint32  DnsServers[L7_DHCPS_DNS_SERVER_MAX]);

/*********************************************************************
 * @purpose  Get DHCP host DNS Servers  for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    DnsServers  @b{(output)} Array of ipAddress of DNS Servers
 *
 * @returns  L7_SUCCESS  DNS servers Got
 * @returns  L7_FAILURE  Failed to get DNS Servers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapDnsServersGet(L7_char8 *pPoolName, L7_uint32  DnsServers[L7_DHCPS_DNS_SERVER_MAX]);

/*********************************************************************
 * @purpose  Set DHCP host Routers for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    defaultRouters     @b{(input)} Array of ipAddress of routers
 *
 * @returns  L7_SUCCESS  Routers set
 * @returns  L7_FAILURE  Failed to set Routers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapDefaultRoutersIpSet(L7_char8 *pPoolName,
    L7_uint32  defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX]);

/*********************************************************************
 * @purpose  Get DHCP host Routers for a automatic pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    defaultRouters     @b{(output)} Array of ipAddress of routers
 *
 * @returns  L7_SUCCESS  Routers Got
 * @returns  L7_FAILURE  Failed to Get Routers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapDefaultRoutersIpGet(L7_char8 *pPoolName,
    L7_uint32  defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX]);

/*********************************************************************
 * @purpose  Get first lease Entry for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pIpAddr     @b{(output)} IP address assigned to lease
 * @param    *pState      @b{(output)} state of the lease
 * @param    *pLeaseIndex @b{(output)} lease index to fetch next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapLeaseEntryFirst(L7_char8 *pPoolName, L7_uint32 *pLeaseIndex, 
    L7_uint32 *pIpAddr, L7_uint32 *pState );

/*********************************************************************
 * @purpose  Get next lease Entry for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pLeaseIndex @b{(input)} lease index to fetch next entry
 *                        @b{(output)} lease index to fetch further next entry
 * @param    *pIpAddr     @b{(output)} IP address assigned to lease
 * @param    *pState      @b{(output)} state of the lease
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE   function failed
 * @returns  L7_ERROR     No more lease entry available
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapLeaseEntryNext(L7_char8 *pPoolName, L7_uint32 *pLeaseIndex, 
    L7_uint32 *pIpAddr, L7_uint32 *pState );

/*********************************************************************
 * @purpose  Get the DHCP Server pool type
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pPoolType   @b{(output)} pool type
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolTypeGet(L7_char8 *pPoolName, L7_uint32 *pPoolType);

/*********************************************************************
 * @purpose  Delete DHCP binding
 *
 * @param    ipAddr      @b{(input)} admin mode
 * @param    state      @b{(input)} state of lease
 *
 * @returns  L7_SUCCESS  admin mode set
 * @returns  L7_FAILURE  Failed to set admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapBindingClear( L7_uint32 ipAddr, L7_uint32 state );

/*********************************************************************
 * @purpose  Delete All DHCP binding
 *
 * @param    ipAddr      @b{(input)} admin mode
 * @param    state      @b{(input)} state of lease
 *
 * @returns  L7_SUCCESS  admin mode set
 * @returns  L7_FAILURE  Failed to set admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapBindingClearAll(L7_uint32 state);

/*********************************************************************
 * @purpose  Get lease data for an given ipAddress
 *
 * @param    ipAddr           @b{(input)} IP address assigned to lease
 * @param    piPMask          @b{(output)} IP Mask
 * @param    hwaddress        @b{(output)} hardware address
 * @param    clientIdentifier @b{(output)} client Identifier
 * @param    pPoolName        @b{(output)} pool name
 * @param    pRemainingTime   @b{(output)} time remaining
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Hardware address is passed out as a formatted string xx:xx:xx...
 *           The max length is therefore (L7_DHCPS_HARDWARE_ADDR_MAXLEN*3)
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseDataGet( L7_uint32 ipAddr, L7_uint32 *pIpMask,
    L7_uchar8 * hwAddr,
    L7_uchar8 * clientIdentifier,
    L7_uchar8 * pPoolName,
    L7_uint32 * pRemainingTime);

/*********************************************************************
 * @purpose  Get lease hardware-address for an given ipAddress
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    ipAddr      @b{(input)} IP address assigned to lease
 * @param    hwAddr      @b{(output)) Hardware address of assigned lease
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseHWAddrGet(L7_uint32 ipAddr, L7_uchar8 *hwAddr);

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  EXTRA(FOR SNMP)
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
 * @purpose  Check if a DHCP pool corresponding to number is valid or not
 *
 * @param    poolNumber   @b{(input)} pool number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolValidate( L7_uint32 poolNumber);

/*********************************************************************
 * @purpose  Get the next valid DHCP pool number
 *
 * @param    poolNumber       @b{(input)} pool number
 * @param    pNextPoolNumber  @b{(output)} next pool number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolGetNext( L7_uint32 poolNumber, L7_uint32 *pNextPoolNumber);

/*********************************************************************
 * @purpose  Get the pool name corresponding to a valid pool number
 *
 * @param    poolNumber       @b{(input)} pool Number
 * @param    pPoolName        @b{(output)} pool Name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP GET / SET operation, where it is required
 * @notes  to convert pool number to pool name for calling the USMDB apis
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNumberToPoolNameGet(L7_uint32 poolNumber, L7_uchar8* pPoolName);

/*********************************************************************
 * @purpose  Get the DHCP Server pool name by index
 *
 * @param    poolIndex    @b{(input)} pool Index
 * @param    *pPoolName   @b{(output)} pool name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapPoolNameByIndexGet(L7_uint32 poolindex, L7_char8 *pPoolName );

/*********************************************************************
 * @purpose  Set the DHCP Server network mask for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipMask       @b{(input)} network mask for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask);

/*********************************************************************
 * @purpose  Set the DHCP Server network ipAddress
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipAddr       @b{(input)} network address for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapNetworkIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr );


/*********************************************************************
 * @purpose  Get the number of Discover Received statistics
 *
 * @param    pDiscoverReceived @b{(output)} number of discover received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsDiscoverReceivedGet( L7_uint32  *pDiscoverReceived);

/*********************************************************************
 * @purpose  Get the number of Request Received statistics
 *
 * @param    pRequestReceived @b{(output)} number of requests  received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsRequestReceivedGet( L7_uint32  *pRequestReceived);

/***************************************************************************
 * @purpose  Get the number of Decline Received statistics
 *
 * @param    pDeclineReceived @b{(output)} number of decline messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ****************************************************************************/

L7_RC_t dhcpsDeclineReceivedGet( L7_uint32  *pDeclineReceived);

/*****************************************************************************
 * @purpose  Get the number of Release Received statistics
 *
 * @param    pReleaseReceived @b{(output)} number of release messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ******************************************************************************/

L7_RC_t dhcpsReleaseReceivedGet( L7_uint32  *pReleaseReceived);

/****************************************************************************
 * @purpose  Get the number of Inform Received statistics
 *
 * @param    pInformReceived @b{(output)} number of inform messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ****************************************************************************/

L7_RC_t dhcpsInformReceivedGet( L7_uint32  *pInformReceived);

/*********************************************************************
 * @purpose  Get the number of Offer Sent statistics
 *
 * @param    pOfferSent @b{(output)} number of offers sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsOfferSentGet( L7_uint32  *pOfferSent);

/*********************************************************************
 * @purpose  Get the number of Ack Sent statistics
 *
 * @param    pAckSent @b{(output)} number of Acknowledges sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsAckSentGet( L7_uint32  *pAckSent);


/*********************************************************************
 * @purpose  Get the number of Nack Sent statistics
 *
 * @param    pNackSent @b{(output)} number of Nack Messages sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNackSentGet( L7_uint32  *pNackSent);

/*********************************************************************
 * @purpose  Get the number of malformed statistics
 *
 * @param    pMalformed @b{(output)} number of malformed messages
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMalformedGet( L7_uint32  *pMalformed);

/*********************************************************************
 * @purpose  Get the number of pool for a DHCP Server
 *
 * @param    pNoOfPool @b{(output)} number of pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsNoOfPool( L7_uint32  *pNoOfPool);


/*********************************************************************
 * @purpose  Create the DHCP Server pool from SNMP Manager
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS          New pool created
 * @returns  L7_TABLE_IS_FULL    No space left for a new pool
 * @returns  L7_FAILURE          Function failed
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPMapPoolCreate(L7_char8 *pPoolName);

/*********************************************************************
 * @purpose  Sets the  DHCP Server pool name
 *
 * @param    poolNumber @b{(input)}  poolNumber
 *           pPoolName @b{(input)}  pPoolName
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPPoolNameSet( L7_uint32 poolNumber, L7_uchar8* pPoolName);
/*********************************************************************
 * @purpose  Set DHCP host for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    ipAddr     @b{(input)}  IP Address
 * @param    ipMask     @b{(input)}  IP Mask
 *
 * @returns  L7_SUCCESS  Set host completed successfully
 * @returns  L7_FAILURE  Failed to set host for a pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostSet(L7_char8 * pPoolName, L7_uint32  ipAddr, L7_uint32 ipMask);
/*********************************************************************
 * @purpose  Get DHCP host from a manual  pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pIpAddr     @b{(output)} Pointer to IP Address
 * @param    pIpMask     @b{(output)} Pointer to IP Mask
 *
 * @returns  L7_SUCCESS  Dhcp host deleted from a pool
 * @returns  L7_FAILURE  Failed to delete Dhcp host pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostGet(L7_char8 * pPoolName, L7_uint32  *pIpAddr, L7_uint32 *pIpMask);
/*********************************************************************
 * @purpose  Delete DHCP host ipaddress and mask from a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask deleted
 * @returns  L7_FAILURE  Failed to deleted ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostDelete( L7_char8 * pPoolName);
/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress
 *
 * @param    pPoolName     @b{(input)} pool name
 * @param    ipAddr       @b{(input)} IP Address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr );
/*********************************************************************
 * @purpose  Set the DHCP Server host Mask
 *
 * @param    pPoolName     @b{(input)} pool name
 * @param    ipMask       @b{(input)} IP Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask);
/*********************************************************************
 * @purpose  Set DHCP host hardware address for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    hwAddr      @b{(input)} Hardware address
 * @param    hwType      @b{(input)} Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware address set
 * @returns  L7_FAILURE  Failed to set Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressSet(L7_char8 * pPoolName, L7_char8 * hwAddr, L7_uint32 hwType);
/*********************************************************************
 * @purpose  Get DHCP host hardware address for a manual pool
 *
 * @param    pPoolName    @b{(input)}  Name of address pool
 * @param    pHwAddr     @b{(output)} Pointer to Hardware address
 * @param    pHwType     @b{(output)} Pointer to Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware address set
 * @returns  L7_FAILURE  Failed to set Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressGet(L7_char8 * pPoolName, L7_char8  *pHwAddr, L7_uint32 *pHwType);
/*********************************************************************
 * @purpose  Delete DHCP host hardware address for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Hardware address deleted
 * @returns  L7_FAILURE  Failed to delete Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressDelete( L7_char8 * pPoolName);
/*********************************************************************
 * @purpose  Set DHCP client identifier for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientId    @b{(input)} Client id 
 * @param    clientIdLen @b{(input)} Client id Length
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdSet(L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 clientIdLen);
/*********************************************************************
 * @purpose  Get DHCP client identifier for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientId    @b{(output)} Client Id
 * @param    clientIdLen @b{(output)} Client Id Length
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdGet(L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 * clientIdLen);
/*********************************************************************
 * @purpose  Delete DHCP client identifier for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientId    @b{(input)} Hardware address
 *
 * @returns  L7_SUCCESS  Client identifier deleted
 * @returns  L7_FAILURE  Failed to delete client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdDelete(L7_char8 * pPoolName );
/*********************************************************************
 * @purpose  Set DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientName  @b{(input)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameSet(L7_char8 * pPoolName, L7_char8 *  clientName);
/*********************************************************************
 * @purpose  Get DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientName  @b{(output)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameGet(L7_char8 * pPoolName, L7_char8 *  clientName);
/*********************************************************************
 * @purpose  Delete DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Client name delete
 * @returns  L7_FAILURE  Failed to delete client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameDelete(L7_char8 * pPoolName );
/*********************************************************************
 * @purpose  Add exluded IP address range
 *
 * @param    fromIpAddr  @b{(input)} from IP address of range
 * @param    toIpAddr    @b{(input)} to IP address of range
 *
 * @returns  L7_SUCCESS  IP address range added for exclusion
 * @returns  L7_FAILURE  Failed to add IP address range
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeAdd(L7_uint32 fromIpAddr, L7_uint32 toIpAddr);
/*********************************************************************
 * @purpose  Delete exluded IP address range
 *
 * @param    fromIpAddr  @b{(input)} from IP address of range
 * @param    toIpAddr    @b{(input)} to IP address of range
 *
 * @returns  L7_SUCCESS  IP address range deleted for exclusion
 * @returns  L7_FAILURE  Failed to delete IP address range
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeDelete(L7_uint32 fromIpAddr, L7_uint32 toIpAddr);
/*********************************************************************
 * @purpose  To get exluded IP address range first entry
 *
 * @param    pFromIpAddr  @b{(output)} from IP address of range
 * @param    pToIpAddr    @b{(output)} to IP address of range
 * @param    pRangeIndex @b{(output)} index to get next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeEntryFirst(L7_uint32 * pFromIpAddr,
    L7_uint32  * pToIpAddr,
    L7_uint32  * pRangeIndex);

/*********************************************************************
 * @purpose  To get exluded IP address range Next entry
 *
 * @param    pFromIpAddr  @b{(output)} from IP address of range
 * @param    pToIpAddr    @b{(output)} to IP address of range
 * @param    pRangeIndex  @b{(input)} index to get this entry
 * @param    pRangeIndex  @b{(output)} index to get next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeEntryNext(L7_uint32 * pFromIpAddr,
    L7_uint32  * pToIpAddr,
    L7_uint32  * pRangeIndex);


/*********************************************************************
 * @purpose  Set the DHCP Server host ip mask for a pool for manual binding
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipMask       @b{(input)} ip mask for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapSNMPHostMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask);

/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress for manual binding
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipAddr       @b{(input)} ip address for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapSNMPHostIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr );

/*********************************************************************
 * @purpose  Get the next valid exclude address range
 *
 * @param    rangeNumber     @b{(input)} starting Ip Address of the range
 * @param    pStartIpAddress @b{(output)} from Ip  Address of the range
 * @param    pEndIpAddress   @b{(output)} to Ip Address of the range
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPExcludeAddressRangeGet(L7_uint32 rangeNumber,
    L7_uint32 *pStartIpAddress,
    L7_uint32 *pEndIpAddress);

/*********************************************************************
 * @purpose  Get the next valid exclude address index
 *
 * @param    rangeNumber     @b{(input)} valid range number
 * @param    pIndex          @b{(output)} valid index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPExcludeAddressRangeGetNext(L7_uint32 rangeNumber,
    L7_uint32 *pIndex);

/*********************************************************************
 * @purpose  Set DHCP host hardware type for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    hwType      @b{(input)} Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware type set
 * @returns  L7_FAILURE  Failed to set Hardware type
 *
 * @comments  for setting HW type from SNMP manager
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSNMPHwTypeSet(L7_char8 * pPoolName,  L7_uint32 hwType);

/*********************************************************************
 * @purpose  Get Next Lease IP address
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    currIpAddr  @b{(input)} Current Lease IP address
 * @param    pNextAddr   @b{(output)} Next Lease IP address
 *
 * @returns  L7_SUCCESS  Got the next IP
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsLeaseGetNext(L7_uint32 currIpAddr, L7_uint32 * pNextAddr);


/*********************************************************************
 * @purpose  Create the option row
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS  Row for storing the TLV created successfully
 * @returns  L7_FAILURE 
 * @returns  L7_ALREADY_CONFIGURED  Row already exist.
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionCreate(L7_char8* pPoolName, L7_uchar8 code );


/*********************************************************************
 * @purpose  Set the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 * @param    pData        @b{(input)} TLV code data
 * @param    length       @b{(input)} TLV code field length
 * @param    dataFormat   @b{(input)} TLV data format
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionSet(L7_char8* pPoolName, L7_uchar8 code, 
    L7_uchar8* pData, L7_uint32 length, 
    L7_uchar8 dataFormat );


/*********************************************************************
 * @purpose  Get the first option code
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pCode        @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGetFirst( L7_char8* pPoolName, L7_uchar8* pCode);


/*********************************************************************
 * @purpose  Set the next option code
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    prevCode     @b{(input)} TLV Field code
 * @param    pCode        @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGetNext( L7_char8* pPoolName, L7_uchar8 prevCode, L7_uchar8* pCode);

/*********************************************************************
 * @purpose  Get the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 * @param    pData        @b{(output)} TLV code data
 * @param    pLength      @b{(output)} TLV code field length
 * @param    pDataFormat  @b{(output)} TLV data format
 * @param    pStatus      @b{(output)} Row Status
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGet(L7_char8* pPoolName, L7_uchar8 code, 
    L7_uchar8* pData, L7_uint32* pLength, 
    L7_uchar8* pDataFormat, char* pStatus );

/*********************************************************************
 * @purpose  delete the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionDelete(L7_char8* pPoolName, L7_uchar8 code);

/*********************************************************************
 * @purpose  Set the ip dhcp conflict mode
 *
 * @param    conflict @b{(input)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictLoggingSet(L7_BOOL  conflict);

/*********************************************************************
 * @purpose  Get the ip dhcp conflict logging mode
 *
 * @param    pConflict @b{(output)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictLoggingGet(L7_BOOL*  pConflict);

/*********************************************************************
 * @purpose  Clear the dhcp conflict address
 *
 * @param    poolAddress @b{(input)} DHCP conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsClearConflict(L7_uint32 poolAddress);

/*********************************************************************
 * @purpose  Clear all the dhcp conflict address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsClearAllConflict();


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    pConflictAddress    @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGetFirst(L7_uint32* pConflictAddress);


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    prevConflictAddress    @b{(input)} Conflict ip address
 * @param    pConflictAddress       @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGetNext( L7_uint32 prevConflictAddress, 
    L7_uint32* pConflictAddress);


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    conflictAddress   @b{(output)} Conflict ip address
 * @param    pDetectionTime    @b{(input)} Conflict detection time
 * @param    pDetectionMethod  @b{(input)} Conflict detection Method
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGet( L7_uint32 conflictAddress , L7_uint32* pDetectionTime,
    L7_uint32* pDetectionMethod);


/*********************************************************************
 * @purpose  Set the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(input)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressSet( L7_char8* pPoolName , 
    L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX]);


/*********************************************************************
 * @purpose  Get the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(output)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressGet( L7_char8* pPoolName , 
    L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX]);

/*********************************************************************
 * @purpose  Delete the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressDelete(L7_char8* pPoolName);

/*********************************************************************
 * @purpose  To set the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    nodeType   @b{(input)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeSet(L7_char8* pPoolName,
    L7_char8 nodeType);

/*********************************************************************
 * @purpose  To get the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pNodeType  @b{(output)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeGet( L7_char8* pPoolName, L7_char8* pNodeType);

/*********************************************************************
 * @purpose  To delete the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeDelete( L7_char8* pPoolName);

/*********************************************************************
 * @purpose  To set the next server address
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    nextAddress  @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerSet( L7_char8* pPoolName, 
    L7_uint32 nextAddress);

/*********************************************************************
 * @purpose  To get the next server address
 *
 * @param    pPoolName     @b{(input)} Name of address pool
 * @param    pNextAddress  @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerGet( L7_char8* pPoolName, 
    L7_uint32* pNextAddress);

/*********************************************************************
 * @purpose  To delete the next server address
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerDelete( L7_char8* pPoolName);

/*********************************************************************
 * @purpose  To set the domain name of a dhcp client
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pDomainName  @b{(input)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameSet(L7_char8* pPoolName, L7_char8* pDomainName);

/*********************************************************************
 * @purpose  To get the domain name of a dhcp client
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pDomainName  @b{(output)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameGet(L7_char8* pPoolName, L7_char8* pDomainName);

/*********************************************************************
 * @purpose  To delete the domain name of a dhcp client
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameDelete(L7_char8* pPoolName);

/*********************************************************************
 * @purpose  To set the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(input)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileSet(L7_char8* pPoolName, L7_char8* pBootFile);

/*********************************************************************
 * @purpose  To get the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(input)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileGet(L7_char8* pPoolName, L7_char8* pBootFile);

/*********************************************************************
 * @purpose  To delete the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileDelete(L7_char8* pPoolName);

/*********************************************************************
 * @purpose  To set the bootp client allocation mode
 *
 * @param    mode  @b{(input)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootpAutomaticSet(L7_BOOL mode);

/*********************************************************************
 * @purpose  To get the bootp client allocation mode
 *
 * @param    pMode  @b{(input)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootpAutomaticGet(L7_BOOL* pMode);

/*********************************************************************
 * @purpose  Inject a DHCP IP Packet into the DHCP Server
 *
 * @param    ipData    @b{(output)} Pointer to an DHCP IP packet
 * @param    ipLen     @b{(output)} Length of ipData
 * @param    intIfNum  @b{(output)} Interface on which ipData was received
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsPacketInject(L7_uchar8 *ipData, L7_int32 ipLen,  
    L7_uint32 intIfNum);

#endif /* _DHCPS_MAP_API_H_*/
