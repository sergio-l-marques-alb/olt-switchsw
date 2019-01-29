/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename  usmdb_dhcps.c
 *
 * @purpose   Provide interface to API's for DHCP Server UI components
 *
 * @component unitmgr
 *
 * @comments
 *
 * @create    09/12/2003
 *
 * @author    athakur
 *
 * @end
 *
 **********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "l7_dhcps_api.h"
#include "defaultconfig.h"
#include "usmdb_dhcps_api.h"

/*********************************************************************
 * @purpose  Set DHCP lease time
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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

L7_RC_t usmDbDhcpsLeaseTimeSet(L7_uint32 UnitIndex, L7_uint32  leaseTime,L7_char8 *pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsMapLeaseTimeSet(pPoolName,leaseTime);
  return(rc);
}


/*********************************************************************
 * @purpose  Get DHCP lease time
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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


L7_RC_t usmDbDhcpsLeaseTimeGet(L7_uint32 UnitIndex, L7_uint32 *pLeaseTime, L7_char8 *pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsMapLeaseTimeGet(pPoolName,pLeaseTime);
  return(rc);
}


/*********************************************************************
 * @purpose  Reset DHCP host Default Lease Time
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Pool Name
 *
 * @returns  L7_SUCCESS  Number of ping packets set
 * @returns  L7_FAILURE  Failed to set number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/


L7_RC_t usmDbDhcpsLeaseTimeReset(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;
  L7_uint32 leaseTime=0;

  leaseTime=FD_DHCPS_DEFAULT_POOLCFG_LEASETIME;

  rc=dhcpsMapLeaseTimeSet(pPoolName,leaseTime);
  return(rc);
}


/*********************************************************************
 * @purpose  Set DHCP host network  for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
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
L7_RC_t usmDbDhcpsNetworkSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  ipAddr, L7_uint32 ipMask)
{
  L7_RC_t rc;

  rc=dhcpsMapNetworkSet(pPoolName,ipAddr,ipMask);
  return(rc);
}

/*********************************************************************
 * @purpose  Get DHCP host network  for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
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
L7_RC_t usmDbDhcpsNetworkGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  *pIpAddr, L7_uint32 *pIpMask)
{
  L7_RC_t rc;

  rc=dhcpsMapNetworkGet(pPoolName, pIpAddr,pIpMask);
  return(rc);
}

/*********************************************************************
 * @purpose  Set DHCP host DNS Servers  for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    dnsSvrs     @b{(input)} Array of ipAddress of DNS Servers
 *
 * @returns  L7_SUCCESS  DNS servers set
 * @returns  L7_FAILURE  Failed to set DNS Servers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsDnsServersSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  dnsSvrs[L7_DHCPS_DNS_SERVER_MAX])
{
  L7_RC_t rc;
  rc=dhcpsMapDnsServersSet(pPoolName, dnsSvrs);
  return(rc);
}

/*********************************************************************
 * @purpose  Get DHCP host DNS Servers  for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    dnsSvrs     @b{(output)} Array of ipAddress of DNS Servers
 *
 * @returns  L7_SUCCESS  DNS servers Got
 * @returns  L7_FAILURE  Failed to get DNS Servers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsDnsServersGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  dnsSvrs[L7_DHCPS_DNS_SERVER_MAX])
{
  L7_RC_t rc;
  rc=dhcpsMapDnsServersGet(pPoolName,dnsSvrs);
  return(rc);
}

/*********************************************************************
 * @purpose  Reset DHCP host DNS Servers  for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    dnsSvrs     @b{(input)} Array of ipAddress of DNS Servers
 *
 * @returns  L7_SUCCESS  DNS servers set
 * @returns  L7_FAILURE  Failed to set DNS Servers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsDnsServersReset(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;
  L7_uint32 dnsSvrs[L7_DHCPS_DNS_SERVER_MAX];
  memset(dnsSvrs,0,sizeof(dnsSvrs));
  rc=dhcpsMapDnsServersSet(pPoolName, dnsSvrs);
  return(rc);
}


/*********************************************************************
 * @purpose  Set DHCP host Routers for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    routers     @b{(input)} Array of ipAddress of routers
 *
 * @returns  L7_SUCCESS  Routers set
 * @returns  L7_FAILURE  Failed to set Routers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsRoutersSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  routers[L7_DHCPS_DEFAULT_ROUTER_MAX])
{
  L7_RC_t rc;
  rc=dhcpsMapDefaultRoutersIpSet(pPoolName,routers);
  return(rc);
}

/*********************************************************************
 * @purpose  Get DHCP host Routers for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    routers     @b{(output)} Array of ipAddress of routers
 *
 * @returns  L7_SUCCESS  Routers Got
 * @returns  L7_FAILURE  Failed to Get Routers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsRoutersGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  routers[L7_DHCPS_DEFAULT_ROUTER_MAX])
{
  L7_RC_t rc;

  rc=dhcpsMapDefaultRoutersIpGet(pPoolName,routers);
  return(rc);
}

/*********************************************************************
 * @purpose  Set DHCP host Default Routers for a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    routers     @b{(input)} Array of ipAddress of routers
 *
 * @returns  L7_SUCCESS  Routers set
 * @returns  L7_FAILURE  Failed to set Routers
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsDefaultRoutersIpReset(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;
  L7_uint32 routers[L7_DHCPS_DEFAULT_ROUTER_MAX];
  memset(routers,0,sizeof(routers));
  rc=dhcpsMapDefaultRoutersIpSet(pPoolName,routers);
  return(rc);
}

/*********************************************************************
 * @purpose  Set DHCP host number of ping packets
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    noOfPingPkt @b{(input)} number of ping packets
 *
 * @returns  L7_SUCCESS  Number of ping packets set
 * @returns  L7_FAILURE  Failed to set number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfPingPktSet(L7_uint32 UnitIndex, L7_uint32  noOfPingPkt)
{
  L7_RC_t rc;

  rc=dhcpsMapNoOfPingPktSet(noOfPingPkt);
  return(rc);
}

/*********************************************************************
 * @purpose  Get DHCP host number of ping packets
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoOfPingPkt @b{(output)} number of ping packets
 *
 * @returns  L7_SUCCESS  Number of ping packets Got
 * @returns  L7_FAILURE  Failed to get number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsNoOfPingPktGet(L7_uint32 UnitIndex, L7_uint32  *pNoOfPingPkt)
{
  L7_RC_t rc;

  rc=dhcpsMapNoOfPingPktGet(pNoOfPingPkt);
  return(rc);
}

/*********************************************************************
 * @purpose  Sets the default no of DHCP host number of ping packets
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * 
 *
 * @returns  L7_SUCCESS  Number of ping packets set
 * @returns  L7_FAILURE  Failed to set number of ping packets
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsNoOfPingPktReset(L7_uint32 UnitIndex)
{
  L7_RC_t rc;
  L7_uint32 noOfPingPkt = 0;

  rc=dhcpsMapNoOfPingPktSet(noOfPingPkt);

  return(rc);
}

/*********************************************************************
 * @purpose  Set DHCP Server admin mode
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    mode        @b{(input)} admin mode
 *
 * @returns  L7_SUCCESS  admin mode set
 * @returns  L7_FAILURE  Failed to set admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsAdminModeSet(L7_uint32 UnitIndex, L7_uint32  mode)
{
  L7_RC_t rc;
  rc=dhcpsMapAdminModeSet(mode);
  return(rc);
}

/*********************************************************************
 * @purpose  Get DHCP Server admin mode
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pMode        @b{(output)} admin mode
 *
 * @returns  L7_SUCCESS  admin mode Get
 * @returns  L7_FAILURE  Failed to Get admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/


L7_RC_t usmDbDhcpsAdminModeGet(L7_uint32 UnitIndex, L7_uint32  *pMode)
{
  L7_RC_t rc;

  rc=dhcpsMapAdminModeGet(pMode);
  return(rc);
}

/*********************************************************************
 * @purpose  Create a DHCP address pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool created
 * @returns  L7_FAILURE  Failed to create Dhcp address pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsPoolCreate(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsMapPoolCreate(pPoolName);
  return(rc);
}

/*********************************************************************
 * @purpose  Get the DHCP Server pool type
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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

L7_RC_t usmDbDhcpsPoolTypeGet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 *pPoolType)
{
  L7_RC_t rc;

  rc=dhcpsMapPoolTypeGet(pPoolName,pPoolType);
  return(rc);
}

/*************************************************************
 * @purpose  Get the first entry in  DHCP address pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName     @b{(output)} Name of address pool
 * @param    pPoolIndex   @b{(output)} pool index to fetch next
 *
 * @returns  L7_SUCCESS  Dhcp address pool first entry found
 * @returns  L7_FAILURE  Dhcp address pool first entry not found
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsPoolEntryFirst(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32 *pPoolIndex)
{
  L7_RC_t rc;

  rc=dhcpsMapPoolEntryFirst(pPoolName, pPoolIndex );
  return(rc);
}

/******************************************************************
 * @purpose  Get the next entry in  DHCP address pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName     @b{(output)} Name of address pool
 * @param    pPoolIndex   @b{(input)}  pool index to get next entry
 * @param                 @b{(output)} pool index to fetch next entry
 *
 *
 * @returns  L7_SUCCESS  Dhcp address pool first entry found
 * @returns  L7_FAILURE  Dhcp address pool first entry not found
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsPoolEntryNext(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32 *pPoolIndex)
{
  L7_RC_t rc;

  rc=dhcpsMapPoolEntryNext(pPoolName, pPoolIndex );
  return(rc);
}

/*********************************************************************
 * @purpose  Get the number of active leases statistics for a DHCP Server
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoActiveLeases @b{(output)} number of active leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfActiveLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoActiveLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfActiveLeasesGet(pNoActiveLeases);
  return(rc);
}

/*********************************************************************
 * @purpose  Get the number of active dynamic leases statistics 
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoActiveDynamicLeases @b{(output)} number of active dynamic leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfActiveDynamicLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoActiveDynamicLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfActiveDynamicLeasesGet(pNoActiveDynamicLeases);
  return(rc);
}
/*********************************************************************
 * @purpose  Get the number of active manual leases statistics
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoActiveManualLeases @b{(output)} number of active manual leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfActiveManualLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoActiveManualLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfActiveManualLeasesGet(pNoActiveManualLeases);
  return(rc);
}
/*********************************************************************
 * @purpose  Get the number of active bootp leases statistics
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoActiveBootpLeases @b{(output)} number of active bootp leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfActiveBootpLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoActiveBootpLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfActiveBootpLeasesGet(pNoActiveBootpLeases);
  return(rc);
}
/*********************************************************************
 * @purpose  Get the number of free leases statistics for a DHCP Server
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoFreeLeases @b{(output)} number of free leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfFreeLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoFreeLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfFreeLeasesGet(pNoFreeLeases);
  return(rc);
}

/*********************************************************************
 * @purpose  Get the number of expired leases statistics for a DHCP Server
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoExpiredLeases @b{(output)} number of expired leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsNoOfExpiredLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoExpiredLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfExpiredLeasesGet(pNoExpiredLeases);
  return(rc);
}


/*********************************************************************
 * @purpose  Get the number of abandoned leases statistics for a DHCP Server
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoAbandonedLeases @b{(output)} number of abandoned leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfAbandonedLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoAbandonedLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfAbandonedLeasesGet(pNoAbandonedLeases);
  return(rc);
}

/*********************************************************************
 * @purpose  Get the number of offered leases statistics for a DHCP Server
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pNoOfferedLeases @b{(output)} number of offered leases
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbDhcpsNoOfpNoOfferedLeasesGet(L7_uint32 UnitIndex,  L7_uint32  *pNoOfferedLeases)
{
  L7_RC_t rc;

  rc=dhcpsNoOfpNoOfferedLeasesGet(pNoOfferedLeases);
  return(rc);
}

/*********************************************************************
 * @purpose  Get first lease Entry for a pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsLeaseEntryFirst(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 *pLeaseIndex, L7_uint32 *pIpAddr, L7_uint32 *pState )
{
  L7_RC_t rc;

  rc=dhcpsMapLeaseEntryFirst(pPoolName,pLeaseIndex,pIpAddr,pState);
  return(rc);
}

/*********************************************************************
 * @purpose  Get next lease Entry for a pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsLeaseEntryNext(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 *pLeaseIndex
    , L7_uint32 *pIpAddr, L7_uint32 *pState )
{
  L7_RC_t rc;

  rc=dhcpsMapLeaseEntryNext(pPoolName,pLeaseIndex,pIpAddr,pState);
  return(rc);
}


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
L7_RC_t usmDbDhcpsLeaseDataGet(L7_uint32 UnitIndex,  L7_uint32 ipAddr, L7_uint32 *pIpMask, L7_uchar8 * hwAddr, 
    L7_uchar8 * clientIdentifier, L7_uchar8 * pPoolName, L7_uint32 *pRemainingTime)
{
  L7_RC_t rc;

  rc=dhcpsMapLeaseDataGet( ipAddr, pIpMask,hwAddr,clientIdentifier,pPoolName,pRemainingTime);
  return(rc);
}

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
L7_RC_t usmDbDhcpsLeaseHWAddrGet(L7_uint32 UnitIndex, L7_uint32 ipAddr, L7_uchar8 *hwAddr)
{
  L7_RC_t rc;

  rc = dhcpsMapLeaseHWAddrGet(ipAddr, hwAddr);
  return(rc);
}

/*:ignore*/
/*---------------------------------------------------------------------
 *                    USMDB FUNCTIONS  - ADMINISTRATIVE 
 *---------------------------------------------------------------------
 */
/*:end ignore*/


/*********************************************************************
 * @purpose  Delete a DHCP address pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Dhcp address pool deleted
 * @returns  L7_FAILURE  Failed to delete Dhcp address pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsPoolDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsMapPoolDelete(pPoolName);
  return(rc);
}

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


L7_RC_t usmDbDhcpsInitMapLeaseConfigData()
{
  return dhcpsMapInitMappingLeaseConfigData();
}

/*********************************************************************
 * @purpose  Delete DHCP binding
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsBindingClear(L7_uint32 UnitIndex, L7_uint32  ipAddr,L7_uint32 state)
{
  L7_RC_t rc;

  rc=dhcpsMapBindingClear(ipAddr,state );
  return(rc);
}

/*********************************************************************
 * @purpose  Delete All DHCP binding
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    state      @b{(input)} state of lease
 *
 * @returns  L7_SUCCESS  admin mode set
 * @returns  L7_FAILURE  Failed to set admin mode
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsBindingClearAll(L7_uint32 UnitIndex,L7_uint32 state )
{
  L7_RC_t rc;

  rc=dhcpsMapBindingClearAll(state);
  return(rc);
}

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

L7_RC_t usmDbDhcpsStatisticsClear(L7_uint32 UnitIndex )
{
  L7_RC_t rc;

  rc=dhcpsStatisticsClear();
  return(rc);
}

/*:ignore*/
/*---------------------------------------------------------------------
 *                    USMDB FUNCTIONS  - MANUAL POOL 
 *---------------------------------------------------------------------
 */
/*:end ignore*/


/*********************************************************************
 * @purpose  Set DHCP host for a manual pool
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
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
L7_RC_t usmDbDhcpsHostSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  ipAddr, L7_uint32 ipMask)
{
  L7_RC_t rc;

  rc=dhcpsHostSet(pPoolName, ipAddr, ipMask);
  return rc;
}

/*********************************************************************
 * @purpose  Get DHCP host from a manual  pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
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
L7_RC_t usmDbDhcpsHostGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uint32  *pIpAddr, L7_uint32 *pIpMask)
{
  L7_RC_t rc;

  rc=dhcpsHostGet(pPoolName, pIpAddr, pIpMask);
  return rc;
}

/*********************************************************************
 * @purpose  Delete DHCP host ipaddress and mask from a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask deleted
 * @returns  L7_FAILURE  Failed to deleted ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsHostDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsHostDelete(pPoolName);
  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsHostIpAddrSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipAddr)
{
  L7_RC_t rc;

  rc=dhcpsHostIpAddrSet(pPoolName, ipAddr);
  return rc;
}
/*********************************************************************
 * @purpose  Set the DHCP Server host Mask
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsHostMaskSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipMask)
{
  L7_RC_t rc;

  rc=dhcpsHostMaskSet(pPoolName, ipMask);        
  return rc;
}

/*********************************************************************
 * @purpose  Add exluded IP address range
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsExcludedAddressRangeAdd(L7_uint32 UnitIndex, L7_uint32 fromIpAddr, L7_uint32 toIpAddr)
{
  L7_RC_t rc;

  rc=dhcpsExcludedAddressRangeAdd(fromIpAddr, toIpAddr);
  return rc;
}

/*********************************************************************
 * @purpose  Delete exluded IP address range
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsExcludedAddressRangeDelete(L7_uint32 UnitIndex, L7_uint32 fromIpAddr, L7_uint32 toIpAddr)
{
  L7_RC_t rc;

  rc=dhcpsExcludedAddressRangeDelete(fromIpAddr, toIpAddr);  
  return rc;
}
/*********************************************************************
 * @purpose  To get exluded IP address range first entry
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsExcludedAddressRangeEntryFirst(L7_uint32 UnitIndex,
    L7_uint32 * pFromIpAddr,
    L7_uint32  * pToIpAddr,
    L7_uint32  * pRangeIndex)
{

  L7_RC_t rc;
  rc=  dhcpsExcludedAddressRangeEntryFirst(pFromIpAddr,pToIpAddr,pRangeIndex);
  return rc;
}

/*********************************************************************
 * @purpose  To get exluded IP address range next entry
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pFromIpAddr  @b{(input)} from IP address of range
 * @param    pFromIpAddr  @b{(output)} next from IP address of range
 * @param    pToIpAddr    @b{(input)} to IP address of range
 * @param    pToIpAddr    @b{(output)} next to IP address of range
 * @param    pRangeIndex @b{(output)} index to get next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsExcludedAddressRangeEntryNext(L7_uint32 UnitIndex,
    L7_uint32 * pFromIpAddr,
    L7_uint32  * pToIpAddr,
    L7_uint32  * pRangeIndex)
{

  L7_RC_t rc;
  rc=  dhcpsExcludedAddressRangeEntryNext(pFromIpAddr,pToIpAddr,pRangeIndex);
  return rc;
}
/*********************************************************************
 * @purpose  Set DHCP host hardware address for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
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
L7_RC_t usmDbDhcpsHwAddressSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8  * hwAddr, L7_uint32 hwType)
{
  L7_RC_t rc;

  rc=dhcpsHwAddressSet(pPoolName, hwAddr, hwType);
  return rc;
}

/*********************************************************************
 * @purpose  Get DHCP host hardware address for a manual pool
 *
 * @param    UnitIndex   @b{(input)}  Unit for this operation
 * @param    pPoolName   @b{(input)}  Name of address pool
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
L7_RC_t usmDbDhcpsHwAddressGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8 * pHwAddr, L7_uint32 * pHwType)
{
  L7_RC_t rc;

  rc=dhcpsHwAddressGet(pPoolName, pHwAddr, pHwType);
  return rc;
}


/*********************************************************************
 * @purpose  Delete DHCP host hardware address for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Hardware address deleted
 * @returns  L7_FAILURE  Failed to delete Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsHwAddressDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsHwAddressDelete(pPoolName);
  return rc;
}

/*********************************************************************
 * @purpose  Set DHCP client identifier for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    clientId    @b{(input)} Hardware address
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientIdSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 clientIdLen)
{
  L7_RC_t rc;

  rc=dhcpsClientIdSet(pPoolName, clientId, clientIdLen);
  return rc;
}
/*********************************************************************
 * @purpose  Get DHCP client identifier for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    clientId    @b{(output)} Client Id
 * @param    clientIdLen    @b{(output)} Client Id Length
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientIdGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 * clientIdLen)
{
  L7_RC_t rc;

  rc=dhcpsClientIdGet(pPoolName, clientId, clientIdLen);
  return rc;

}

/*********************************************************************
 * @purpose  Delete DHCP client identifier for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Client identifier deleted
 * @returns  L7_FAILURE  Failed to delete client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientIdDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName )
{
  L7_RC_t rc;

  rc=dhcpsClientIdDelete(pPoolName );
  return rc;
}

/*********************************************************************
 * @purpose  Set DHCP client name for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    clientName  @b{(input)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientNameSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8 * clientName)
{
  L7_RC_t rc;

  rc=dhcpsClientNameSet(pPoolName, clientName);
  return rc;
}

/*********************************************************************
 * @purpose  Get DHCP client name for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    clientName  @b{(output)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientNameGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_char8 * clientName)
{
  L7_RC_t rc;

  rc=dhcpsClientNameGet(pPoolName, clientName);
  return rc;
}

/*********************************************************************
 * @purpose  Delete DHCP client name for a manual pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    clientName  @b{(input)} Name of client
 *
 * @returns  L7_SUCCESS  Client name delete
 * @returns  L7_FAILURE  Failed to delete client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsClientNameDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName )
{
  L7_RC_t rc;

  rc=dhcpsClientNameDelete(pPoolName );
  return rc;
}

/*********************************************************************
 * @purpose  Delete DHCP host ipaddress and mask from a automatic pool
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask deleted
 * @returns  L7_FAILURE  Failed to deleted ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNetworkDelete(L7_uint32 UnitIndex, L7_char8 * pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsMapNetworkDelete(pPoolName );
  return(rc);
}


/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  EXTRA(FOR SNMP) CONFIG ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
 * @purpose  Check if a DHCP pool corresponding to number is valid or not
 *
 * @param    poolNumber   @b{(input)} pool number
 * @param    UnitIndex    @b{(input)} Unit for this operation
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

L7_RC_t usmDbDhcpsPoolValidate(L7_uint32 UnitIndex, L7_uint32 poolNumber)
{

  L7_RC_t rc;

  rc=dhcpsMapPoolValidate( poolNumber);
  return rc;
}

/*********************************************************************
 * @purpose  Get the next valid DHCP pool number
 * 
 * @param    UnitIndex        @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsPoolGetNext(L7_uint32 UnitIndex, L7_uint32 poolNumber, L7_uint32 *pNextPoolNumber)
{

  L7_RC_t rc;

  rc=dhcpsMapPoolGetNext(poolNumber,pNextPoolNumber);
  return rc;
}

/*********************************************************************
 * @purpose  Get the pool name corresponding to a valid pool number
 * 
 * @param    UnitIndex        @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsNumberToPoolNameGet(L7_uint32 UnitIndex, L7_uint32 poolNumber, L7_uchar8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsMapNumberToPoolNameGet(poolNumber,pPoolName);
  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP Server pool name by index
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsMapPoolNameByIndexGet(L7_uint32 UnitIndex, L7_uint32 poolindex, L7_char8 *pPoolName )
{

  L7_RC_t rc;
  rc=dhcpsMapPoolNameByIndexGet(poolindex,pPoolName );
  return rc;

}

/*********************************************************************
 * @purpose  Set the DHCP Server network ipAddress
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsNetworkIpAddrSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipAddr )
{
  L7_RC_t rc;

  rc=dhcpsMapNetworkIpAddrSet(pPoolName,ipAddr );
  return rc;
}


/*********************************************************************
 * @purpose  Set the DHCP Server network mask for a pool
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsNetworkMaskSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipMask)
{

  L7_RC_t rc;
  rc = dhcpsMapNetworkMaskSet(pPoolName,ipMask);
  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsSNMPHostIpAddrSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipAddr )
{
  L7_RC_t rc;

  rc=dhcpsMapSNMPHostIpAddrSet(pPoolName,ipAddr );
  return rc;
}


/*********************************************************************
 * @purpose  Set the DHCP Server host ip mask for a pool
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
L7_RC_t usmDbDhcpsSNMPHostMaskSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 ipMask)
{

  L7_RC_t rc;
  rc = dhcpsMapSNMPHostMaskSet(pPoolName,ipMask);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of Discover Received statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pDiscoverReceived @b{(output)} number of discover received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsDiscoverReceivedGet( L7_uint32 UnitIndex, L7_uint32  *pDiscoverReceived)
{

  L7_RC_t rc;

  rc=dhcpsDiscoverReceivedGet( pDiscoverReceived);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of Request Received statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pRequestReceived @b{(output)} number of requests  received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsRequestReceivedGet( L7_uint32 UnitIndex, L7_uint32  *pRequestReceived)
{

  L7_RC_t rc;

  rc=dhcpsRequestReceivedGet(pRequestReceived);
  return rc;
}

/***************************************************************************
 * @purpose  Get the number of Decline Received statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pDeclineReceived @b{(output)} number of decline messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ****************************************************************************/
L7_RC_t usmDbDhcpsDeclineReceivedGet(L7_uint32 UnitIndex, L7_uint32  *pDeclineReceived)
{

  L7_RC_t rc;

  rc=dhcpsDeclineReceivedGet(pDeclineReceived);
  return rc;
}

/*****************************************************************************
 * @purpose  Get the number of Release Received statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pReleaseReceived @b{(output)} number of release messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ******************************************************************************/
L7_RC_t usmDbDhcpsReleaseReceivedGet( L7_uint32 UnitIndex, L7_uint32  *pReleaseReceived)
{
  L7_RC_t rc;

  rc=dhcpsReleaseReceivedGet(pReleaseReceived);
  return rc;

}

/****************************************************************************
 * @purpose  Get the number of Inform Received statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pInformReceived @b{(output)} number of inform messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 ****************************************************************************/
L7_RC_t usmDbDhcpsInformReceivedGet( L7_uint32 UnitIndex, L7_uint32  *pInformReceived)
{
  L7_RC_t rc;

  rc=dhcpsInformReceivedGet(pInformReceived);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of Offer Sent statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pOfferSent        @b{(output)} number of offers sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsOfferSentGet( L7_uint32 UnitIndex, L7_uint32  *pOfferSent)
{
  L7_RC_t rc; 

  rc=dhcpsOfferSentGet(pOfferSent);
  return rc;
}
/*********************************************************************
 * @purpose  Get the number of Ack Sent statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pAckSent          @b{(output)} number of Acknowledges sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsAckSentGet( L7_uint32 UnitIndex, L7_uint32  *pAckSent)  
{
  L7_RC_t rc;

  rc=dhcpsAckSentGet(pAckSent);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of Nack Sent statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pNackSent         @b{(output)} number of Nack Messages sent
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNackSentGet( L7_uint32 UnitIndex, L7_uint32  *pNackSent)
{
  L7_RC_t rc;

  rc=dhcpsNackSentGet(pNackSent);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of malformed statistics
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
 * @param    pMalformed        @b{(output)} number of malformed messages
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsMalformedGet( L7_uint32 UnitIndex, L7_uint32  *pMalformed)  
{
  L7_RC_t rc;

  rc=dhcpsMalformedGet(pMalformed);
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of pool for a DHCP Server
 *
 *@param     UnitIndex         @b{(input)} Unit for this operation
 * @param    pNoOfPool         @b{(output)} number of pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsNoOfPool(L7_uint32 UnitIndex, L7_uint32  *pNoOfPool)
{
  L7_RC_t rc;

  rc=dhcpsNoOfPool(pNoOfPool);
  return rc;
}

/*********************************************************************
 * @purpose  Create the DHCP Server pool from SNMP Manager
 *
 * @param     UnitIndex          @b{(input)} Unit for this operation
 * @param    *pPoolName          @b{(input)} pool name
 *
 * @returns  L7_SUCCESS          New pool created
 * @returns  L7_TABLE_IS_FULL    No space left for a new pool
 * @returns  L7_FAILURE          Function failed
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsSNMPMapPoolCreate(L7_uint32 UnitIndex, L7_char8 *pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsSNMPMapPoolCreate(pPoolName);
  return rc;
}

/*********************************************************************
 * @purpose  Sets the  DHCP Server pool name
 *
 * @param    UnitIndex          @b{(input)} Unit for this operation
 * @param    poolNumber         @b{(input)}  poolNumber
 *           pPoolName           @b{(input)}  pPoolName
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsSNMPPoolNameSet( L7_uint32 UnitIndex, L7_uint32 poolNumber, 
    L7_uchar8* pPoolName)
{
  L7_RC_t rc;

  rc=dhcpsSNMPPoolNameSet( poolNumber, pPoolName);
  return rc;
}

/*********************************************************************
 * @purpose  Get the valid exclude address range
 * 
 * @param    UnitIndex        @b{(input)} Unit for this operation
 * @param    rangeNumber      @b{(input)} valid entry number
 * @param    pStartIpAddress  @b{(output)} from Ip Address of the next valid  range
 * @param    pEndIpAddress    @b{(output)} to Ip  Address of the next valid range
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
L7_RC_t usmDbDhcpsSNMPExcludeAddressRangeGet(L7_uint32 UnitIndex, 
    L7_uint32 rangeNumber,
    L7_uint32 *pStartIpAddress,
    L7_uint32 *pEndIpAddress)
{

  L7_RC_t rc;

  rc=dhcpsSNMPExcludeAddressRangeGet(rangeNumber,pStartIpAddress,pEndIpAddress);
  return rc;
}


/*********************************************************************
 * @purpose  Get the valid exclude address index 
 * 
 * @param    UnitIndex        @b{(input)} Unit for this operation
 * @param    rangeNumber      @b{(input)} valid entry number
 * @param    pIndex           @b{(output)} valid index
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
L7_RC_t usmDbDhcpsSNMPExcludeAddressRangeGetNext(L7_uint32 UnitIndex, 
    L7_uint32 rangeNumber,
    L7_uint32 *pIndex)
{

  L7_RC_t rc;

  rc=dhcpsSNMPExcludeAddressRangeGetNext(rangeNumber,pIndex);
  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP Server Hardware type
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    *pPoolName   @b{(input)} pool name
 * @param    hwType       @b{(input)} hardware type of the DHCP client
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDhcpsSNMPHwTypeSet(L7_uint32 UnitIndex, L7_char8 *pPoolName, L7_uint32 hwType)
{
  L7_RC_t rc;

  rc=dhcpsSNMPHwTypeSet(pPoolName,hwType);
  return rc;
}

/*********************************************************************
 * @purpose  Get Next Lease IP address
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    leaseIpAddr @b{(input)} Lease IP address
 * @param    pNextAddr   @b{(output)} Next Lease IP address
 *
 * @returns  L7_SUCCESS  Got the next IP
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsLeaseGetNext(L7_uint32 UnitIndex, L7_uint32 leaseIpAddr
    , L7_uint32 * pNextAddr)
{
  L7_RC_t rc;

  rc=dhcpsLeaseGetNext(leaseIpAddr, pNextAddr);

  return rc;
}

/* DHCP Phase 2 USMDB Functions */
/*********************************************************************
 * @purpose  Create the option row
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    code        @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS  Row for storing the TLV created successfully
 * @returns  L7_FAILURE 
 * @returns  L7_ALREADY_CONFIGURED  Row already exist.
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsOptionCreate(L7_uint32 UnitIndex, L7_char8 * pPoolName, L7_uchar8 code )
{

  L7_RC_t rc;

  rc=dhcpsOptionCreate(pPoolName, code);

  return rc;
}


/*********************************************************************
 * @purpose  Set the option field value
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
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
extern L7_RC_t usmDbDhcpsOptionSet(L7_uint32 UnitIndex, L7_char8 * pPoolName, 
    L7_uchar8 code, L7_uchar8* pData, L7_uint32 length, 
    L7_uchar8 dataFormat )
{

  L7_RC_t rc;

  rc=dhcpsOptionSet(pPoolName, code, pData, length, dataFormat);

  return rc;
}


/*********************************************************************
 * @purpose  Get the first option code
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pPoolName   @b{(input)} Name of address pool
 * @param    pCode       @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsOptionGetFirst(L7_uint32 UnitIndex, 
    L7_char8 * pPoolName, L7_uchar8* pCode)
{

  L7_RC_t rc;

  rc=dhcpsOptionGetFirst(pPoolName, pCode);

  return rc;
}


/*********************************************************************
 * @purpose  Set the next option code
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    prevCode   @b{(input)} TLV Field code
 * @param    pCode      @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsOptionGetNext(L7_uint32 UnitIndex, 
    L7_char8 * pPoolName, L7_uchar8 prevCode, L7_uchar8* pCode)
{

  L7_RC_t rc;

  rc=dhcpsOptionGetNext(pPoolName, prevCode, pCode);

  return rc;
}

/*********************************************************************
 * @purpose  Get the option field value
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 * @param    pData        @b{(output)} TLV code data
 * @param    pLength      @b{(output)} TLV code field length
 * @param    pDataFormat  @b{(output)} TLV data format
 * @param    pStatus      @b{(output)} TLV row status
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsOptionGet(L7_uint32 UnitIndex, L7_char8 * pPoolName, 
    L7_uchar8 code, L7_uchar8* pData, L7_uint32* pLength, 
    L7_uchar8* pDataFormat, char* pStatus )
{

  L7_RC_t rc;

  rc=dhcpsOptionGet(pPoolName, code, pData, pLength, pDataFormat, pStatus);

  return rc;
}

/*********************************************************************
 * @purpose  delete the option field value
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsOptionDelete(L7_uint32 UnitIndex, L7_char8* pPoolName, L7_uchar8 code)
{

  L7_RC_t rc = L7_FAILURE;

  rc = dhcpsOptionDelete(pPoolName, code);

  return rc;
}

/*********************************************************************
 * @purpose  Set the ip dhcp conflict mode
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    conflict    @b{(input)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsConflictLoggingSet(L7_uint32 UnitIndex, L7_BOOL  conflict)
{

  L7_RC_t rc;

  rc=dhcpsConflictLoggingSet(conflict);

  return rc;
}

/*********************************************************************
 * @purpose  Get the ip dhcp conflict logging mode
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    pConflict   @b{(output)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsConflictLoggingGet(L7_uint32 UnitIndex, L7_BOOL*  pConflict)
{

  L7_RC_t rc;

  rc=dhcpsConflictLoggingGet(pConflict);

  return rc;
}

/*********************************************************************
 * @purpose  Clear the dhcp conflict address
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    poolAddress  @b{(input)} DHCP conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsClearConflict(L7_uint32 UnitIndex, L7_uint32 poolAddress)
{

  L7_RC_t rc;

  rc=dhcpsClearConflict(poolAddress);

  return rc;
}

/*********************************************************************
 * @purpose  Clear all the dhcp conflict address
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsClearAllConflict(L7_uint32 UnitIndex)
{

  L7_RC_t rc;

  rc=dhcpsClearAllConflict();

  return rc;
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    UnitIndex           @b{(input)} Unit for this operation
 * @param    pConflictAddress    @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsConflictGetFirst(L7_uint32 UnitIndex, 
    L7_uint32* pConflictAddress)
{

  L7_RC_t rc;

  rc=dhcpsConflictGetFirst(pConflictAddress);

  return rc;
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    UnitIndex              @b{(input)} Unit for this operation
 * @param    prevConflictAddress    @b{(input)} Conflict ip address
 * @param    pConflictAddress       @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsConflictGetNext(L7_uint32 UnitIndex, 
    L7_uint32 prevConflictAddress, L7_uint32* pConflictAddress)
{

  L7_RC_t rc;

  rc=dhcpsConflictGetNext(prevConflictAddress,  pConflictAddress);

  return rc;
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    UnitIndex         @b{(input)} Unit for this operation
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
extern L7_RC_t usmDbDhcpsConflictGet(L7_uint32 UnitIndex, 
    L7_uint32 conflictAddress , L7_uint32* pDetectionTime,
    L7_uint32* pDetectionMethod)
{

  L7_RC_t rc;

  rc=dhcpsConflictGet(conflictAddress, pDetectionTime, pDetectionMethod);

  return rc;
}


/*********************************************************************
 * @purpose  Set the netbios name server addresses
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(input)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNameServerAddressSet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName , L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX])
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNameServerAddressSet(pPoolName, addresses);

  return rc;
}


/*********************************************************************
 * @purpose  Get the netbios name server addresses
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(output)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNameServerAddressGet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName , L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX])
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNameServerAddressGet(pPoolName, addresses);

  return rc;
}

/*********************************************************************
 * @purpose  Delete the netbios name server addresses
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNameServerAddressDelete(L7_uint32 UnitIndex, 
    L7_char8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNameServerAddressDelete(pPoolName);

  return rc;
}

/*********************************************************************
 * @purpose  To set the netbios node type
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    nodeType   @b{(input)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNodeTypeSet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName,
    L7_char8 nodeType)
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNodeTypeSet(pPoolName, nodeType);

  return rc;
}

/*********************************************************************
 * @purpose  To get the netbios node type
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pNodeType  @b{(output)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNodeTypeGet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName,
    L7_char8* pNodeType)
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNodeTypeGet(pPoolName, pNodeType);

  return rc;
}

/*********************************************************************
 * @purpose  To delete the netbios node type
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNetbiosNodeTypeDelete(L7_uint32 UnitIndex, 
    L7_char8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsNetbiosNodeTypeDelete(pPoolName);

  return rc;
}

/*********************************************************************
 * @purpose  To set the next server address
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    nextAddress  @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNextServerSet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName, L7_uint32 nextAddress)
{

  L7_RC_t rc;

  rc=dhcpsNextServerSet(pPoolName, nextAddress);

  return rc;
}

/*********************************************************************
 * @purpose  To get the next server address
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pNextAddress @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNextServerGet(L7_uint32 UnitIndex, 
    L7_char8* pPoolName, L7_uint32* pNextAddress)
{

  L7_RC_t rc;

  rc=dhcpsNextServerGet(pPoolName, pNextAddress );

  return rc;
}

/*********************************************************************
 * @purpose  To delete the next server address
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsNextServerDelete(L7_uint32 UnitIndex, 
    L7_char8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsNextServerDelete(pPoolName);

  return rc;
}

/*********************************************************************
 * @purpose  To set the domain name of a dhcp client
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pDomainName  @b{(input)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsDomainNameSet(L7_uint32 UnitIndex, L7_char8* pPoolName,
    L7_char8* pDomainName)
{

  L7_RC_t rc;

  rc=dhcpsDomainNameSet(pPoolName, pDomainName);

  return rc;
}

/*********************************************************************
 * @purpose  To get the domain name of a dhcp client
 *
 * @param    UnitIndex    @b{(input)} Unit for this operation
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pDomainName  @b{(output)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsDomainNameGet(L7_uint32 UnitIndex, L7_char8* pPoolName,
    L7_char8* pDomainName)
{

  L7_RC_t rc;

  rc=dhcpsDomainNameGet(pPoolName, pDomainName);

  return rc;
}

/*********************************************************************
 * @purpose  To delete the domain name of a dhcp client
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsDomainNameDelete(L7_uint32 UnitIndex, L7_char8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsDomainNameDelete(pPoolName);

  return rc;
}

/*********************************************************************
 * @purpose  To set the bootfile name
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(input)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsBootfileSet(L7_uint32 UnitIndex, L7_char8* pPoolName,
    L7_char8* pBootFile)
{

  L7_RC_t rc;

  rc=dhcpsBootfileSet(pPoolName, pBootFile);

  return rc;
}

/*********************************************************************
 * @purpose  To get the bootfile name
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(input)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsBootfileGet(L7_uint32 UnitIndex, L7_char8* pPoolName,
    L7_char8* pBootFile)
{

  L7_RC_t rc;

  rc=dhcpsBootfileGet(pPoolName, pBootFile);

  return rc;
}

/*********************************************************************
 * @purpose  To delete the bootfile name
 *
 * @param    UnitIndex  @b{(input)} Unit for this operation
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsBootfileDelete(L7_uint32 UnitIndex, L7_char8* pPoolName)
{

  L7_RC_t rc;

  rc=dhcpsBootfileDelete(pPoolName);

  return rc;
}

/*********************************************************************
 * @purpose  To set the bootp client allocation mode
 *
 * @param    UnitIndex @b{(input)} Unit for this operation
 * @param    mode      @b{(input)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsBootpAutomaticSet(L7_uint32 UnitIndex, L7_BOOL mode)
{

  L7_RC_t rc;

  rc=dhcpsBootpAutomaticSet(mode);

  return rc;
}

/*********************************************************************
 * @purpose  To get the bootp client allocation mode
 *
 * @param    UnitIndex @b{(input)} Unit for this operation
 * @param    pMode     @b{(input)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbDhcpsBootpAutomaticGet(L7_uint32 UnitIndex, L7_BOOL* pMode)
{

  L7_RC_t rc;

  rc=dhcpsBootpAutomaticGet(pMode);

  return rc;
}

