/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   src\application\unitmgr\usmdb\usmdb_ipmap.c
*
* @purpose    Provide interface to ip API's for unitmgr components
*
* @component  unitmgr
*
* @comments   none
*
* @create     03/13/2001
*
* @author     asuthan
* @end
*
**********************************************************************/

#include <string.h>
#include <l7_common.h>
#include <l7_ip_api.h>
#include <rto_api.h>

#include <usmdb_util_api.h>
#include <usmdb_nim_api.h>
#include <nimapi.h>
#include <dot1q_api.h>
#include "usmdb_ip_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"

/*======================== IP  Configuration Data================================*/
/*======================== IP  Configuration Data================================*/
/*======================== IP  Configuration Data================================*/
/*======================== IP  Configuration Data================================*/
/*======================== IP  Configuration Data================================*/

/*********************************************************************
* @purpose  Determines if Ip Source Checking is enabled or disabled
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLE, L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpSourceCheckingGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpSourceCheckingGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ip Source Checking mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLE, L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpSourceCheckingSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpSourceCheckingSet(val);
  return rc;
}




/*********************************************************************
* @purpose  Returns Ip Spoofing Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpSpoofingGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpSpoofingGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Ip Spoofing mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpSpoofingSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpSpoofingSet(val);
  return rc;
}

/*********************************************************************
* @purpose  Gets the configured IP MTU value
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(output)} the intIfNum value
* @param ipMtu      @b{(output)} the ip mtu value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfIpMtuGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * ipMtu)
{
  L7_RC_t rc;
  rc = ipMapIntfIpMtuGet(intIfNum, ipMtu);
  return rc;
}

/*********************************************************************
* @purpose  Get the maximum IP MTU that may be set on an interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    maxIpMtu @b{(output)} maximum IP MTU in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    On port based routing interfaces, the IP MTU may be set
*           as large as the link MTU. On VLAN routing interfaces,
*           the IP MTU may be set as large as the largest jumbo
*           frame will allow.
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfMaxIpMtuGet(L7_uint32 intIfNum, L7_uint32 *maxIpMtu)
{
  return ipMapIntfMaxIpMtuGet(intIfNum, maxIpMtu);
}

/*********************************************************************
* @purpose  Gets the IP MTU value being enforced on a given interface
*
* @param intIfNum   @b{(output)} the intIfNum value
* @param ipMtu      @b{(output)} the ip mtu value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If the IP MTU is not configured, it is derived from the link MTU.
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfEffectiveIpMtuGet(L7_uint32 intIfNum, L7_uint32 *ipMtu)
{
  return ipMapIntfEffectiveIpMtuGet(intIfNum, ipMtu);
}


/*********************************************************************
* @purpose  Sets the Ip Mtu value
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the ip mtu value
* @param ipMtu      @b{(input)} the ip mtu value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfIpMtuSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipMtu)
{
  L7_RC_t rc;
  rc = ipMapIntfIpMtuSet(intIfNum, ipMtu);
  return rc;
}


/*********************************************************************
* @purpose  Returns contents of the specified ARP entry
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} IP address of the ARP entry
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*                                  L7_INVALID_INTF if interface not configured
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpEntryGet(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
    L7_uint32 intIfNum, L7_arpEntry_t *pArp)
{
  return(ipMapIpArpEntryGet(ipAddr, intIfNum, pArp));
}

/*********************************************************************
* @purpose  Returns contents of the next ARP entry following the one
*           specified
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} IP address of the ARP entry to
*                                    start the search
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*                                  L7_INVALID_INTF if interface not configured
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpEntryNext(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
    L7_uint32 intIfNum, L7_arpEntry_t *pArp)
{
  return(ipMapIpArpEntryNext(ipAddr, intIfNum, pArp));
}

/*********************************************************************
* @purpose  Purges a specific dynamic/gateway entry from the ARP cache
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} IP address of the ARP entry to purge
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*                                  L7_INVALID_INTF if interface not configured
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not support deleting local/static ARP entries, as there
*           are other APIs for handling those.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpEntryPurge(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
    L7_uint32 intIfNum )
{
  return(ipMapIpArpEntryPurge(ipAddr, intIfNum));
}

/*********************************************************************
* @purpose  Clears the ARP cache of all dynamic/gateway entries
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    gateway    @b{(input)} flag to clear gateway entries as well
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All dynamic entries currently in the ARP cache are cleared.
*           The gateway entries are conditionally cleared based on the
*           value of the gateway parm.  No local/static entries are
*           affected.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpCacheClear(L7_uint32 UnitIndex, L7_BOOL gateway)
{
  return(ipMapIpArpCacheClear(gateway));
}

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param UnitIndex  @b{(input)}  the unit for this operation
* @param *pStats    @b{(output)} pointer to ARP cache stats structure to
*                                  be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpCacheStatsGet(L7_uint32 UnitIndex, L7_arpCacheStats_t *pStats)
{
  return ipMapIpArpCacheStatsGet(pStats);
}

/*********************************************************************
* @purpose  Returns Arp entry ageout time
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp entry ageout time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpAgeTimeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpArpAgeTimeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Arp entry ageout time
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpAgeTimeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpArpAgeTimeSet(val);
  return rc;
}


/*********************************************************************
* @purpose  Add a static ARP entry to the ARP table
*
* @param    UnitIndex       Unit Index
* @param    ipAddress       IP Address
* @param    intIfNum        Internal interface number of the entry
*                           L7_INVALID_INTF if interface not configured
* @param    *macAddr        Pointer to a buffer containing the mac address
*                           of type L7_uchar8
*
* @returns  L7_SUCCESS  entry added to the ARP table
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticArpAdd(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
    L7_uint32 intIfNum, L7_uchar8 *macAddr)
{
  L7_linkLayerAddr_t LLAddr;

  bzero((L7_char8 *)&LLAddr, sizeof(L7_linkLayerAddr_t));
  memcpy(LLAddr.addr.enetAddr.addr, macAddr, L7_MAC_ADDR_LEN );

  return ipMapStaticArpAdd(ipAddr, &LLAddr, intIfNum);
}


/*********************************************************************
* @purpose  Delete a static ARP entry
*
* @param    unitIndex       Unit Index
* @param    ipAddress       IP Address
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*                                  L7_INVALID_INTF if interface not configured
*
* @returns  L7_SUCCESS  entry deleted from the ARP table
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapStaticArpDelete(L7_uint32 unitIndex, L7_IP_ADDR_t ipAddress,
    L7_uint32 intIfNum)
{
  return ipMapStaticArpDelete(ipAddress, intIfNum);
}

/*********************************************************************
* @purpose  Count the number of static ARP entries currently configured.
*
* @param    unitIndex       Unit Index
*
* @returns  The number of static ARP entries currently configured.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usmDbIpMapStaticArpCount(L7_uint32 unitIndex)
{
    return ipMapStaticArpCount();
}

/*********************************************************************
* @purpose  Get a list of all static ARP entries currently configured.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    staticEntries @b{(input/output)} An array of L7_IP_ARP_CACHE_STATIC_MAX
*                      ARP entries. On return, the array is filled with the
*                      static ARP entries. The first entry with an IP address
*                      of 0 indicates the end of the list.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if API is called with firstEntry NULL
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapStaticArpGetAll(L7_uint32 unitIndex,
                                  L7_arpEntry_t *staticEntries)
{
    if (staticEntries == L7_NULLPTR)
        return L7_FAILURE;

    return ipMapStaticArpGetAll(staticEntries);
}

/*********************************************************************
* @purpose  Returns Arp request response timeout value
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp response timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpRespTimeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpArpRespTimeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Arp request response timeout
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp request response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpRespTimeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpArpRespTimeSet(val);
  return rc;
}

/*********************************************************************
* @purpose  Returns Arp request max retries count
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp request max retries count value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpRetriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpArpRetriesGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Arp request max retries count
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp request max retries count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpRetriesSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpArpRetriesSet(val);
  return rc;
}

/*********************************************************************
* @purpose  Returns maximum number of entries allowed in ARP cache
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp cache max entries value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpCacheSizeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpArpCacheSizeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the maximum number of entries in the ARP cache
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp cache entries max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpCacheSizeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpArpCacheSizeSet(val);
  return rc;
}

/*********************************************************************
* @purpose  Returns ARP dynamic entry renew mode
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} IP ARP dynamic entry renew mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpDynamicRenewGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = ipMapIpArpDynamicRenewGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the ARP dynamic entry renew mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} IP ARP dynamic entry renew mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpArpDynamicRenewSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  rc = ipMapIpArpDynamicRenewSet(val);
  return rc;
}


/*======================== Router Configuration Data================================*/
/*======================== Router Configuration Data================================*/
/*======================== Router Configuration Data================================*/
/*======================== Router Configuration Data================================*/
/*======================== Router Configuration Data================================*/


/*********************************************************************
* @purpose  Get the gratuitous ARP administrative state for an interface.
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} internal interface number
* @param gratArpState @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_uint32 usmDbGratArpGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                          L7_uint32 *gratArpState)
{
    return ipMapGratArpGet(intIfNum, gratArpState);
}

/*********************************************************************
* @purpose  Enable or disable gratuitous ARP on an interface.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param intIfNum  @b{(input)} internal interface number
* @param gratArpState @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbGratArpSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                        L7_uint32 gratArpState)
{
    return ipMapGratArpSet(intIfNum, gratArpState);
}

/*********************************************************************
* @purpose  Determine whether proxy ARP is enabled on an interface
*
* @param    UnitIndex @b{(input)} unit number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *proxyArpMode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbProxyArpGet(L7_uint32 UnitIndex,
                         L7_uint32 intIfNum,
                         L7_uint32 *proxyArpMode)
{
    return ipMapProxyArpGet(intIfNum, proxyArpMode);
}

/*********************************************************************
* @purpose  Enable or disable proxy ARP on an interface
*
* @param    UnitIndex @b{(input)} unit number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    proxyArpMode    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbProxyArpSet(L7_uint32 UnitIndex,
                         L7_uint32 intIfNum,
                         L7_uint32 proxyArpMode)
{
    return ipMapProxyArpSet(intIfNum, proxyArpMode);
}

/*********************************************************************
* @purpose  Determine whether local proxy ARP is enabled on an interface
*
* @param    UnitIndex @b{(input)} unit number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *localProxyArpMode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLocalProxyArpGet(L7_uint32 UnitIndex,
                         L7_uint32 intIfNum,
                         L7_uint32 *localProxyArpMode)
{
    return ipMapLocalProxyArpGet(intIfNum, localProxyArpMode);
}

/*********************************************************************
* @purpose  Enable or disable local proxy ARP on an interface
*
* @param    UnitIndex @b{(input)} unit number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    localProxyArpMode    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLocalProxyArpSet(L7_uint32 UnitIndex,
                         L7_uint32 intIfNum,
                         L7_uint32 localProxyArpMode)
{
    return ipMapLocalProxyArpSet(intIfNum, localProxyArpMode);
}

/*********************************************************************
* @purpose  Returns L7_ENABLE if Ip forwards net-directed broadcasts
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNetDirectBcastsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val)
{
    *val = ipMapNetDirectBcastsCfgGet(intIfNum);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures Ip regarding forwarding net directed broadcasts
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(input)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNetDirectBcastsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 val)
{
    L7_RC_t rc;
    rc = ipMapRtrIntfNetDirectBcastsSet(intIfNum, val);
    return rc;
}

/*********************************************************************
* @purpose  Determine whether a given interface is unnumbered and if it
*           is, determine the interface whose address the unnumbered
*           interface borrows.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    isUnnumbered @b{(output)} L7_TRUE or L7_FALSE
* @param    numberedIfc @b{(output)} internal interface number of the
*                                    numbered interface. Optional.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface not configurable or isUnnumbered NULL
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpUnnumberedGet(L7_uint32 intIfNum, L7_BOOL *isUnnumbered,
                             L7_uint32 *numberedIfc)
{
    return ipMapUnnumberedGet(intIfNum, isUnnumbered, numberedIfc);
}

/*********************************************************************
* @purpose  Returns L7_TRUE if a given interface is configured to be
*           unnumbered.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbIntfIsUnnumbered(L7_uint32 intIfNum)
{
  return ipMapIntfIsUnnumbered(intIfNum);
}

/*********************************************************************
* @purpose  Set the unnumbered status of an interface
*
* @param    intIfNum  @b {(input)}  Internal Interface Number
* @param    isUnnumbered  @b{(input)}  L7_TRUE or L7_FALSE
* @param    numberedIfc  @b{(input)}  internal interface number of the
*                                     numbered interface whose IP address the
*                                     unnumbered interface is to borrow
*
* @returns  L7_SUCCESS
* @returns  L7_ALREADY_CONFIGURED if setting interface unnumbered but an
*                                 IP address is already configured on the
*                                 interface
* @returns  L7_FAILURE
*
* @notes    Fails if an address is already configured on the interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpUnnumberedSet(L7_uint32 intIfNum, L7_BOOL isUnnumbered,
                             L7_uint32 numberedIfc)
{
    return ipMapUnnumberedSet(intIfNum, isUnnumbered, numberedIfc);
}


/*********************************************************************
* @purpose  Returns L7_ENABLE if Ip forwards net-directed broadcasts
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMcastsFwdModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val)
{
    *val = ipMapMcastFwdModeCfgGet(intIfNum);
    return L7_SUCCESS;
}





/*======================== Router Interface Data================================*/
/*======================== Router Interface Data================================*/
/*======================== Router Interface Data================================*/
/*======================== Router Interface Data================================*/
/*======================== Router Interface Data================================*/





/*********************************************************************
* @purpose  Gets the router interface mode
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    *mode        pointer to mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode)
{
  return ipMapRtrIntfModeGet(intIfNum, mode);
}


/*********************************************************************
* @purpose  Sets the router interface mode
*
* @param    UnitIndex   Unit Number
* @param    intIfNum    Internal Interface Number
* @param    mode        Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  if (usmDbIpRtrIntfModeConfigurable(UnitIndex, intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  return ipMapRtrIntfModeSet(intIfNum, mode);
}


/*********************************************************************
* @purpose  Check if routing is configurable on a given interface
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_TRUE     if "routing" is configurable
* @returns  L7_FALSE    otherwise
*
* @notes    Routing cannot be configured on some interfaces because
*           those interfaces do not support routing.  Others do
*           support routing, but it cannot be turned off (tunnels
*           and loopbacks are examples of the latter).
*
* @end
*********************************************************************/
L7_BOOL usmDbIpRtrIntfModeConfigurable(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ROUTING_CONFIGURABLE);
}

/*********************************************************************
* @purpose  Determine whether a given IP interface is up for IPv4.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ifState  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    An interface is considered up if IP MAP has issued an
*           L7_RTR_INTF_ENABLE for it. The interface is operational and
*           ready for use.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfOperModeGet(L7_uint32 intIfNum, L7_uint32 *ifState)
{
  return ipMapRtrIntfOperModeGet(intIfNum, ifState);
}

/*********************************************************************
* @purpose  Obtain the subnet configuration of the router interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *pIpAddr @b{(input)} Pointer to IP address of router interface
* @param    *pMask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfIpAddressGet(L7_uint32 UnitIndex,L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr,
                                    L7_IP_MASK_t *mask)
{

  return(ipMapRtrIntfCfgIpAddressGet( intIfNum, ipAddr, mask));
}


/*********************************************************************
* @purpose  Set an IP address on an interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
* @param    method          IP Address Configuration Method
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified ip
*                             address & an already configured ip
*                             address
* @returns  L7_REQUEST_DENIED Return this error if the ipAddress is same as
*                             next hop address of a static route
* @returns  L7_ALREADY_CONFIGURED  if the address is already configured on
*                                  the same interface
* @returns  L7_NOT_EXIST      if the interface is configured to be unnumbered
* @returns  L7_FAILURE        other errors or failures
*
* @notes    The calling function must not allow the user to set a null
*           IP address value (0.0.0.0) for a router interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfIPAddressSet(L7_uint32 UnitIndex,
                                   L7_uint32 intIfNum,
                                   L7_IP_ADDR_t ipAddress,
                                   L7_IP_MASK_t subnetMask,
                                   L7_INTF_IP_ADDR_METHOD_t method)
{
  L7_uint32 intfType;

  if (usmDbIntfTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  else if( (intfType == L7_PHYSICAL_INTF)
         && (usmDbComponentPresentCheck(UnitIndex, L7_IP_MAP_COMPONENT_ID) == L7_FALSE))
  {
      /* Block configuration of  an IP address to a physical port if VLAN
         routing is enabled.  */
      return L7_NOT_SUPPORTED;
  }
  else
  {
      return(ipMapRtrIntfIpAddressSet(intIfNum,  ipAddress, subnetMask, method));
  }
}

/*********************************************************************
* @purpose  Get the IP Address Configuration Method
*
* @param    intIfNum @b{(input)}  Internal Interface Number
* @param    *method  @b{(output)} Pointer to IP Address Method of router
*                                 interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbIpRtrIntfIpAddressMethodGet (L7_uint32 intIfNum,
                                  L7_INTF_IP_ADDR_METHOD_t *method)
{
  return ipMapRtrIntfCfgIpAddressMethodGet (intIfNum, method);
}

/*********************************************************************
* @purpose  Adds a secondary IP address to an interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_NOT_SUPPORTED  this is not a routing build
* @returns  L7_ERROR          subnet conflict between specified ip
*                             address & an already configured ip
*                             address
* @returns  L7_FAILURE        other errors or failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfSecondaryIpAddrAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                         L7_IP_ADDR_t ipAddress, L7_IP_MASK_t subnetMask)
{
  L7_uint32 intfType;

  if (usmDbIntfTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*
    If this is not a routing build then we need to return back as the requested
    feature is not supported
  */

  if ((intfType == L7_PHYSICAL_INTF) &&
      (usmDbComponentPresentCheck(UnitIndex, L7_IP_MAP_COMPONENT_ID) == L7_FALSE))
  {
    return L7_NOT_SUPPORTED;
  }
  else
  {
    return(ipMapRtrIntfSecondaryIpAddrAdd(intIfNum, ipAddress, subnetMask));
  }
}

/*********************************************************************
* @purpose  Removes an IP address on an interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfIPAddressRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_IP_ADDR_t ipAddress, L7_IP_MASK_t subnetMask)
{
  return(ipMapRtrIntfIpAddressRemove(intIfNum,  ipAddress, subnetMask));
}

/*
This define will allow the following function to loop for up to
60 seconds waiting for the state of the vlan interface to change.
After that time, it will return an error.
*/

#define USMDB_VLAN_ROUTING_MAX_LOOPS 6000

/*********************************************************************
* @purpose  Removes a secondary IP address configured on an interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       Secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with the secondary IP
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfSecondaryIpAddrRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                            L7_IP_ADDR_t ipAddress, L7_IP_MASK_t subnetMask)
{
  return ipMapRtrIntfSecondaryIpAddrRemove(intIfNum, ipAddress, subnetMask);
}

/*********************************************************************
* @purpose  Removes all IP addresses configured on an interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfIpAddressesRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return ipMapRtrIntfIpAddressesRemove(intIfNum);
}

/*********************************************************************
* @purpose  Returns the list of IP Addresses associated with the interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       Addresses associated with the specified interface.
*
* @returns  L7_SUCCESS  The interface is configured as a router interface
*                       and the address information could be successfully
*                       retrieved
* @returns  L7_FAILURE  The interface is not configured as a router interface
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS, please provide a buffer of
*           this size to store all IP Addresses associated with this inter-
*           face.
*
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfIpAddrListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_rtrIntfIpAddr_t *ipAddrList)
{
  return ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList);
}

/*********************************************************************
* @purpose  Returns the list of IP Addresses associated with the interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       Addresses associated with the specified interface.
*
* @returns  L7_SUCCESS  The configured address information was successfully
*                       retrieved
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS, please provide a buffer of
*           this size to store all IP Addresses associated with this inter-
*           face.  This routine differs from usmDbIpRtrIntfIpAddrListGet
*           because it retrieves the configured information rather than
*           the currently running information
*
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfCfgIpAddrListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_rtrIntfIpAddr_t *ipAddrList)
{
  return ipMapRtrIntfCfgIpAddrListGet(intIfNum, ipAddrList);
}

/*********************************************************************
* @purpose  Create a VLAN interface and enable it for routing
*
* @param    UnitIndex   Unit Number
* @param    vlanId      ID of the VLAN
* @param    intfId      (Optional) interface ID. If non-zero, dot1q uses
*                       this value as the index to its VLAN interface array,
*                       dot1qVlanIntf[]. If 0, dot1q uses the first available
*                       index.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_REQUEST_DENIED - request timed out
*           L7_ADDR_INUSE - VLAN ID already in use for port based routing
*           L7_ALREADY_CONFIGURED - Interface ID already in use
*           L7_DEPENDENCY_NOT_MET - VLAN ID is the management VLAN. Cannot enable
*                                   routing on the management VLAN.
*
* @notes    This routine causes two actions to occur:
*               1. the creation of a NIM interface associated with the
*                  specified VLAN.
*               2. the automatic enabling of the resultant interface for routing.
*
*           The user must use the intIfNum to refer to the routed vlan in
*           any future correspondence.
*
*           Once a VLAN has been created as a routing interface, it can be
*           enabled or disabled for routing via usmDbIpRtrIntfModeSet().
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfCreate(L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uint32 intfId)
{
  L7_uint32 intIfNum;
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_uint32 type;
  L7_INTF_STATES_t waitEvent;
  L7_RC_t   rc;
  L7_uint32 count = 0;
  L7_uint32 mgmtVlanID = 0;

  /* Do not allow enable routing on a Management VLAN used by the
   * Network port.
   */
  if (usmDbMgmtVlanIdGet (UnitIndex, &mgmtVlanID) == L7_SUCCESS)
  {
    if (vlanId == mgmtVlanID)
    {
      return L7_DEPENDENCY_NOT_MET;
    }
  }

  /* Ultimately, we want the creation/deletion of a VLAN interface to be distinct
     from the enabling/disabling of the VLAN interface for routing.  In order to facilitate
     moving to that direction in the future, this routine is divided into two parts:
       1. vlan interface creation.
       2. routing interface configuration */

  /*---------------------------*/
  /* Create the vlan interface */
  /*---------------------------*/
  rc = dot1qVlanIntfCreate(vlanId, intfId, &intIfNum);

  if (rc == L7_SUCCESS)
  {
     /* Wait for vlan interface to be created before proceeding.
        If the vlan is configured at the time that it is created,
        wait for it to be attached */

     if ( dot1qVlanStatusGet(vlanId,  &type) == L7_NOT_EXISTS)
         waitEvent = L7_INTF_CREATED;
     else
         waitEvent = L7_INTF_ATTACHED;


     memset((void *)&nimQueryData,0,sizeof(nimQueryData));
     nimQueryData.intIfNum = intIfNum;
     nimQueryData.request = L7_NIM_QRY_RQST_STATE;

     /*
       This loop is used instead of a semaphore because of the
       way NIM state is changed, and what it is changed to.
       Because the state can be changed in multiple ways, not
       all of which will be followed by this function, a simple
       synchronization semaphore cannot be used here.
     */

     while (1)
     {
         if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
         {
           /* should never get here */
           return L7_FAILURE;
         }
         if (waitEvent == L7_INTF_CREATED)
         {
             if ((nimQueryData.data.state == L7_INTF_CREATED) ||
                 (nimQueryData.data.state == L7_INTF_ATTACHING) ||
                 (nimQueryData.data.state == L7_INTF_ATTACHED))
             {
                 break;
             }

         }
         else
             if (nimQueryData.data.state == L7_INTF_ATTACHED)
         {
                 break;
         }

         osapiSleepMSec(10);
         count++;
         if (count > USMDB_VLAN_ROUTING_MAX_LOOPS)
         {
             L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
             nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

             /*
             If the state hasn't changed after 60 seconds (with
             the current define), then return "request denied" rather
             than looping forever
             */
                         dot1qVlanIntfDelete(vlanId);
             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USMDB_COMPONENT_ID,
                     "NIM creation of VLAN routing intf %s not completed, timeout\n", ifName);

             return L7_REQUEST_DENIED;
         }
     }

    /* Vlan Interface Fix*/
    /* applying admin mode for the newly created Vlan Interface                  */
    /* this is done to make sure that the nim data changed flag is set           */
    /* when a  new vlan interface is created                                     */
    /* if this is not done then the internal interface number and vlan interface */
    /* mapping is not maintained when configuratin is saved and restored         */
     rc = nimSetIntfMgmtAdminState(intIfNum,FD_DOT1Q_DEFAULT_VLAN_INTF_ADMIN_STATE);
     rc = nimSetIntfAdminState(intIfNum,FD_DOT1Q_DEFAULT_VLAN_INTF_ADMIN_STATE);
  }
  else
  {
      return rc;
  }



  /*----------------------------------*/
  /* Enable the interface for routing */
  /*----------------------------------*/
  /* WPJ_TBD:  If NIM's sleep is removed, then need semaphore protection before
     continuing with routing */
#ifdef L7_IPV6_PACKAGE
  ip6MapRtrIntfModeSet(intIfNum, L7_ENABLE);
#endif

  if (L7_SUCCESS != (rc = ipMapRtrIntfModeSet(intIfNum, L7_ENABLE)))
  {
       dot1qVlanIntfDelete(vlanId);
  }
  return rc;
}


/*********************************************************************
* @purpose  Delete a VLAN routing interface
*
* @param    UnitIndex   Unit Number
* @param    vlanid          id of the vlan
*
* @returns  L7_SUCCESS
*           L7_NOT_EXIST, if not a VLAN routing interface
* @returns  L7_FAILURE
*
* @notes    This routine causes the deletion of a VLAN routing interface,
* @notes    of the NIM interface (intIfNum) and  of the routing interface
* @notes    (i.e. deallocates the router interface number corresponding to
* @notes    the intIfNum assigned to the VLAN interface).  The user will then
* @notes    lose any IP configuration associated with that VLAN interface.
*
*
* @notes    Once a VLAN has been created as a routing interface, it can be
* @notes    enabled or disabled for routing via usmDbIpRtrIntfModeSet().
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfDelete(L7_uint32 UnitIndex, L7_uint32 vlanId)
{
    L7_uint32 intIfNum;
    L7_NIM_QUERY_DATA_t nimQueryData;
    L7_RC_t   rc;
    L7_uint32 mgmtVlanID = 0;


  /* Do not allow enable routing on a Management VLAN used by the
   * Network port.
   */
  if (usmDbMgmtVlanIdGet (UnitIndex, &mgmtVlanID) == L7_SUCCESS)
  {
    if (vlanId == mgmtVlanID)
    {
      return L7_ADDR_INUSE;
    }
  }

  /* Purposeful architectual deviation:

     Ultimately, we want the creation/deletion of a VLAN interface to be distinct
     from the enabling/disabling of the VLAN interface for routing.

     Note that the routing configuration will automatically be removed when the interface
     is deleted.

  */

   rc = dot1qVlanIntfVlanIdToIntIfNum(vlanId, &intIfNum);
   if (rc == L7_SUCCESS)
   {
      rc = dot1qVlanIntfDelete(vlanId);
   }

   if (rc == L7_SUCCESS)
   {
       memset((void *)&nimQueryData,0,sizeof(nimQueryData));
       nimQueryData.intIfNum = intIfNum;
       nimQueryData.request = L7_NIM_QRY_RQST_STATE;
       while (1)
       {
           if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
           {
             /* should never get here */
             return L7_FAILURE;
           }
           if (nimQueryData.data.state == L7_INTF_UNINITIALIZED)
           {
             break;
           }

           osapiSleepMSec(10);
       }
   }

   return rc;
}

/*********************************************************************
* @purpose  Check if routing is enabled on this vlan
*
* @param    UnitIndex   Unit Number
* @param    vlanid          id of the vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRtrVlanIdGet(L7_uint32 UnitIndex, L7_uint32 vlanId)
{

    L7_uint32 intIfNum;
    L7_uint32 mode;

    if ( dot1qVlanIntfVlanIdToIntIfNum(vlanId, &intIfNum) == L7_SUCCESS)
    {
        if (ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS)
        {
            if (mode == L7_ENABLE)
            {
                return L7_SUCCESS;
            }
        }
    }
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the next vlan after this vlan on which routing is enabled
*
* @param    UnitIndex   Unit Number
* @param    vlanid          id of the vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRtrVlanIdGetNext(L7_uint32 UnitIndex, L7_uint32 *vlanId)
{

  L7_uint32 searchVlan;
  L7_uint32 nextVlanId;
  L7_BOOL   entry_found;

  entry_found = L7_FALSE;
  searchVlan = *vlanId;

  while (searchVlan < L7_DOT1Q_MAX_VLAN_ID)
  {
    if (dot1qVlanIntfNextVlanIntfGetNext(searchVlan, &nextVlanId) == L7_SUCCESS)
    {
        if (usmDbIpVlanRtrVlanIdGet(UnitIndex, nextVlanId) == L7_SUCCESS)
        {
            *vlanId = nextVlanId;
            entry_found = L7_TRUE;
            break;
        }
        else
            searchVlan = nextVlanId;
    }
    else
        break;
  }

  if (entry_found == L7_TRUE)
      return L7_SUCCESS;

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the interface number corresponding to this vlan
*
* @param    UnitIndex   Unit Number
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of this vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRtrVlanIdToIntIfNum(L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uint32 *intIfNum)
{

  return ( dot1qVlanIntfVlanIdToIntIfNum(vlanId, intIfNum) );
}

/*********************************************************************
* @purpose  Get the index number corresponding to this vlan interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum        Internal interface number of this vlan
* @param    vlanId          id of the vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRtrIntIfNumToVlanId(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *vlanId)
{
  return ( dot1qVlanIntfIntIfNumToVlanId(intIfNum, vlanId) );
}

/*********************************************************************
* @purpose  Get the interface ID corresponding to this port-based routing interface
*
* @param    vlanId          id of the vlan
* @param    intfId          interface ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The interface ID is an internally assigned integer in the
*           range [1, L7_MAX_NUM_VLAN_INTF]. The user may optionally
*           specify the interface ID. The interface ID influences the
*           u/s/p and internal interface number assigned to the VLAN
*           interface. It is listed in the text configuration in order
*           to retain the internal interface number and u/s/p across
*           reboots.
*
* @end
*********************************************************************/
L7_RC_t usmDbVlanIntfIdGet(L7_uint32 vlanId, L7_uint32 *intfId)
{
  return dot1qVlanIntfIdGet(vlanId, intfId);
}

/*********************************************************************
* @purpose  Get the interface number corresponding to this internally-assigned
*           VLAN ID
*
* @param    vlanid      id of the vlan
* @param    intIfNum    Internal interface number of port-based routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a port-based routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a VLAN routing interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrInternalVlanIdToIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
  return ipMapInternalVlanIdToRtrIntIfNum(vlanId, intIfNum);
}





/*********************************************************************
* @purpose  Get the port-based routing interface associated with given Internal VLAN ID.
*
* @param    vlanId VLAN ID
*
* @returns  L7_SUCCESS Routing Interface
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 usmDbIpRtrInternalVlanRoutingInterfaceGet(L7_uint32 vlanId)
{
  return ipMapInternalVlanRoutingInterfaceGet(vlanId);
}

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*           for a port-based routing interface
*
* @param    vlanId          vlanId
* @param    intIfNum        corresponding interface
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
    return ipMapInternalVlanFindFirst(vlanId,intIfNum);
}

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*           for a port-based routing interface
*
* @param    prevVid      previous VLAN ID
* @param    vlanId       next vlanId
* @param    intIfNum     interface corresponding to next VLAN ID
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
    return ipMapInternalVlanFindNext(prevVid, vlanId, intIfNum);
}


/*======================== Miscellaneous ================================*/
/*======================== Miscellaneous ================================*/
/*======================== Miscellaneous ================================*/
/*======================== Miscellaneous ================================*/
/*======================== Miscellaneous ================================*/



/*********************************************************************
* @purpose  Returns Number of Active Routes
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} Number of Active Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfActiveRoutesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Number of Active Routes
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Number of Active Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfActiveRoutesSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns Number of Interfaces
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} Number of Interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfInterfacesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Number of Interfaces
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Number of Interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfInterfacesSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determines if Ip is enabled or disabled
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ip state is always ENABLED
*
* @end
*********************************************************************/
L7_RC_t usmDbIpStateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = L7_ENABLE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return Router Preference
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param    origin  @b{(input)} router protocol type
* @param    pref    @b{(output)} preference associated with the protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ip state is always ENABLED
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRouterPreferenceGet(L7_uint32 UnitIndex, L7_uint32 origin, L7_uint32 *pref)
{
  *pref = ipMapRouterPreferenceGet(origin);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Config Router Preference
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    origin     @b{(input)} router protocol type
* @param    pref       @b{(input)} preference associated with the protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE Preference value out of range
* @returns  L7_ERROR   Invalid preference change
* @returns  L7_ALREADY_CONFIGURED Preference value is in use by another
*                                 protocol
*
* @notes    Ip state is always ENABLED
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRouterPreferenceSet(L7_uint32 UnitIndex, L7_uint32 origin, L7_uint32 pref)
{
  L7_RC_t rc;
  rc = ipMapRouterPreferenceSet(origin, pref);
  return rc;
}


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
L7_RC_t usmDbNextRouteEntryGet(L7_uint32 UnitIndex,
                               L7_routeEntry_t *inOutRoute, L7_uint32 bestRouteOnly)
{
  L7_RC_t rc = rtoNextRouteGet(inOutRoute, bestRouteOnly);
  return rc;
}

/*******************************************************************************
* @purpose  It is an utility function to return the next ecmp route from the routing
*           table
*
* @param    UnitIndex   Unit for this operation
* @param    *routeIp    @{(input/output)} Destination network of the route
* @param    *routeMask  @{(input/output)} Destination networkmask of the route
* @param    *protocol   @{(input/output)} Source protocol of the route
* @param    *pref       @{(input/output)} Preference value of the route
* @param    *ipaddr     @{(input/output)} Ip address of the route's next hop
* @param    *IfindexValue @{(input/output)} Interface index of the route's next hop
* @param    routeType   @{(input)}        Route Type (Best or Not)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*******************************************************************************/
L7_RC_t usmDbGetNextEcmpRoute(L7_uint32 unit, L7_uint32 *routeIp, L7_uint32 *routeMask, L7_uint32 *protocol,
                              L7_uint32 *pref, L7_uint32 *ipaddr, L7_uint32 *IfindexValue, L7_BOOL routeType)
{

  L7_uint32 networkip, networkmask, addr, ifIndex;
  L7_uint32 flag, nh, routeFound;
  L7_RC_t rc;
  L7_routeEntry_t  routeEntry;

  flag = L7_FALSE;
  routeFound = L7_FALSE;
  memset(&routeEntry, 0x0, sizeof(routeEntry));

   /* Check the given pointers */
  if ((routeIp != NULL) && (routeMask != NULL) && (ipaddr!= NULL) && (IfindexValue!= NULL) )
  {
     addr     = *ipaddr;
     ifIndex  = *IfindexValue;

     networkip = *routeIp;
     networkmask = *routeMask;
  }
  else
  {
     return L7_FAILURE;
  }

  while ((rc = usmDbNextRouteEntryGet(unit, &routeEntry, routeType)) == L7_SUCCESS)
  {
     if((routeEntry.ipAddr == networkip) && (routeEntry.subnetMask == networkmask)&& (routeEntry.protocol == *protocol)
        && (routeEntry.pref == *pref))
     {
        routeFound = L7_TRUE;
        break;
     }
  }

  if(routeFound == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* If there are no ecmp routes */
  if(routeEntry.ecmpRoutes.numOfRoutes == 0)
  {
    return L7_FAILURE;
  }

  /* Return the first entry */
  if( (addr == 0) && (ifIndex == 0) )
  {
    *ipaddr = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
    *IfindexValue = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    return L7_SUCCESS;
  }
  else if ((addr != 0) && (ifIndex == 0))
  {
     flag = L7_FALSE;

     for (nh = 0; nh < routeEntry.ecmpRoutes.numOfRoutes; nh++)
     {
         if(routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr == addr)
         {
            flag = L7_TRUE;
            *IfindexValue = routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum;
            break;
         }
     }

     if(flag != L7_TRUE)
     {
        return L7_FAILURE;
     }
     else
     {
        return L7_SUCCESS;
     }
  }
  else if ((addr != 0) && (ifIndex != 0))
  {
     flag = L7_FALSE;

     for (nh = 0; nh < routeEntry.ecmpRoutes.numOfRoutes; nh++)
     {
        if( (routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr == addr) &&
            (routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum == ifIndex) )
        {
           /*Since we are looking for the next entry, we have to copy nex entry */
           if ((nh + 1) < routeEntry.ecmpRoutes.numOfRoutes)
           {
              flag = L7_TRUE;
              *ipaddr = routeEntry.ecmpRoutes.equalCostPath[nh + 1].arpEntry.ipAddr;
              *IfindexValue = routeEntry.ecmpRoutes.equalCostPath[nh + 1].arpEntry.intIfNum;
               break;
            }
        }
     }

     if(flag != L7_TRUE)
     {
        return L7_FAILURE;
     }
     else
     {
        return L7_SUCCESS;
     }
  }
  else
  {
     return L7_FAILURE;
  }
  /*Control should not comehere. If so return failure */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the route with the longest matching prefix to a
*           given destination.
*
* @param    dest_ip     @b{(input)}  Destination IP address.
* @param    routeEntry  @b{(output)} best route to the destination
*
*
* @returns  L7_FAILURE  Route not found.
* @returns  L7_SUCCESS  Router found
*
* @notes    If there are multiple routes with the same prefix, always
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t usmDbBestMatchRouteLookup (L7_uint32 UnitIndex,
                                   L7_uint32 dest_ip, L7_routeEntry_t *routeEntry)
{
  L7_RC_t rc = rtoBestRouteLookup(dest_ip, routeEntry,
                                  L7_TRUE /* get reject routes as well */);
  return rc;
}

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
L7_uint32 usmDbRouteCount(L7_uint32 UnitIndex, L7_BOOL bestRoutesOnly)
{
    return rtoRouteCount(bestRoutesOnly);
}

/*********************************************************************
* @purpose  Verifies if the specified static route is valid
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
*
* @returns  L7_TRUE         Specified route is valid
* @returns  L7_FALSE        Specified route is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbIpStaticRouteAddrIsValid(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                      L7_uint32 subnetMask)
{
  return ipMapStaticRouteAddrIsValid(ipAddr, subnetMask);
}

/*********************************************************************
* @purpose  Add a static route entry
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    nextHopRtr      Next Hop Router IP Address
* @param    preference      Preference of this route compared to routes to
*                           the same destination learned from other sources.
*                           If not configured, preference must be set to the
*                           default preference for static routes.
* @param    intIfNum        outgoing interface for the route. L7_INVALID_INTF if
*                           interface not specified
* @param    routeFlags      flags for the route entry type
*
* @returns  L7_SUCCESS          If the route was successfully added to the
*                               configuration
* @returns  L7_FAILURE          If the specified IP address is invalid
* @returns  L7_ERROR            If the maximum number of next hops for the specified
*                               network and route preference has been exceeded
* @returns  L7_TABLE_IS_FULL    If the maximum number of static routes has been
*                               exceeded
*
* @notes    If a static route already exists to ipAddr/subnetMask with next
*           hop nextHopRtr, the preference of the existing static route is
*           changed to the preference value passed in the last argument.
*
*           The next hop is accepted even if it is not on a local subnet. The
*           next hop will be added to RTO, which determines whether to announce
*           it to the forwarding table, to ARP, and other RTO registrants.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRouteAdd(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                              L7_uint32 subnetMask, L7_uint32 nextHopRtr,
                              L7_uint32 preference, L7_uint32 intIfNum,
                              L7_RT_ENTRY_FLAGS_t routeFlags)
{
  L7_rtrStaticRoute_t staticRoute;
  L7_RC_t rc = L7_FAILURE;

  bzero((L7_char8 *)&staticRoute, sizeof(L7_rtrStaticRoute_t));
  staticRoute.ipAddr      = ipAddr;
  staticRoute.ipMask      = subnetMask;
  staticRoute.nextHopRtr  = nextHopRtr;
  staticRoute.preference  = preference;
  staticRoute.intIfNum    = intIfNum;
  staticRoute.flags       = routeFlags;

  rc = ipMapStaticRouteAdd(UnitIndex,&staticRoute);

  return rc;
}


/*********************************************************************
* @purpose  Delete a static route entry.
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    nextHopRtr      Next Hop Router IP Address. If 0, delete all
*                           next hops to this destination.
* @param    intIfNum        outgoing interface for the route. L7_INVALID_INTF if
*                           interface not specified
* @param    routeFlags      flags for the route entry type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If nextHopRtr is 0, all static routes to the destination are deleted.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRouteDelete(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
                                 L7_IP_MASK_t subnetMask, L7_IP_ADDR_t nextHopRtr,
                                 L7_uint32 intIfNum, L7_RT_ENTRY_FLAGS_t routeFlags)
{
    L7_rtrStaticRoute_t staticRoute;


    bzero((L7_char8 *)&staticRoute, sizeof(L7_rtrStaticRoute_t));
    staticRoute.ipAddr      = ipAddr;
    staticRoute.ipMask      = subnetMask;
    staticRoute.nextHopRtr  = nextHopRtr;
    staticRoute.intIfNum    = intIfNum;
    staticRoute.flags       = routeFlags;

    return ipMapStaticRouteDelete(&staticRoute);
}

/*********************************************************************
* @purpose  List all static routes
*
* @param    UnitIndex       The unit for this operation
* @param    staticEntries   An array of L7_RTR_MAX_STATIC_ROUTES
*                           static route entries.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function populates the array staticEntries of type L7_routeEntry_t
*           with all the configured static routes.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapStaticRouteGetAll(L7_uint32 UnitIndex, L7_routeEntry_t *staticEntries)
{
    L7_RC_t rc = ipMapStaticRouteGetAll(staticEntries);
    return rc;
}
/*********************************************************************
* @purpose  Revert the preference of a static route to the default preference.
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    pref            Existing (non-default) preference of the route.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRoutePrefRevert(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
                                     L7_IP_MASK_t subnetMask, L7_uint32 pref )
{
    return ipMapStaticRoutePrefRevert(ipAddr, subnetMask, pref);
}

/*********************************************************************
* @purpose  Get the nexthop interface number corresponding to the
*           static route entry
*
* @param    ipaddr          @b{(input)} route network
* @param    ipmask          @b{(input)} network mask
* @param    pref            @b{(input)} route preference
* @param    nexthop         @b{(input)} next hop
* @param    *index          @b{(output)} interface index
*
* @returns  L7_SUCCESS      If matching route is found
* @returns  L7_FAILURE      If matching route is not found
*
* @notes    For reject routes, we return cpu internal interface number as the nexthop
*           interface, since for such routes the packets are destined to CPU.
*
* @end
*********************************************************************/
L7_RC_t usmDbStaticRouteIfindexGet(L7_uint32 ipaddr, L7_uint32 ipmask,
                                   L7_uint32 pref, L7_uint32 nexthop, L7_uint32 *index)
{
  return ipMapStaticRouteIfindexGet(ipaddr, ipmask, pref, nexthop, index);
}

/*********************************************************************
* @purpose  Get the next static route entry expected in snmp order
*
* @param    *ipaddr          @b{(input/output)} route network
* @param    *ipmask          @b{(input/output)} network mask
* @param    *preference      @b{(input/output)} route preference
* @param    *nexthopaddr     @b{(input/output)} next hop
* @param    *nhIfNum         @b{(input/output)} next hop interface number
*
* @returns  L7_SUCCESS      If next static route is found
* @returns  L7_FAILURE      If next static route is not found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbGetNextStaticRoute(L7_uint32 *ipaddr, L7_uint32 *ipmask, L7_uint32 *preference,
                                L7_uint32 *nexthopaddr, L7_uint32 *nhIfNum)
{
  return ipMapStaticRouteGetNext(ipaddr, ipmask, preference, nexthopaddr, nhIfNum);
}

/*********************************************************************
* @purpose  Determine if IP interface exists
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
L7_BOOL usmDbIpIntfExists(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  return(ipMapIpIntfExists(intIfNum));
}

/*********************************************************************
* @purpose  Find the interface attached to a subnet containing a given
*           IP address.
*
* @param    ipAddr    ({input})  IP address to look for
* @param    intIfNum  ({output}) internal interface number of interface
*                                containing the specified address
*
* @returns  L7_SUCCESS  if an interface is found containing given address
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpRouterIfResolve(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum)
{
  return (ipMapRouterIfResolve(ipAddr, intIfNum));
}
/*********************************************************************
*
* @purpose check to see if intIfNum is a valid ipmap Interface
*
* @param    unitIndex         Unit Index
* @param    intIfNum          Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIpMapIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  return ipMapIsValidIntf(intIfNum);
}


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
L7_RC_t usmDbIpMapValidIntfFirstGet(L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumFirstGet(&interface);
  while (rc == L7_SUCCESS)
  {
    if (ipMapIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface);
    }
  }
  return L7_FAILURE;
}


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
L7_RC_t usmDbIpMapValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumNext(prevIfNum, &interface);
  while (rc == L7_SUCCESS)
  {
    if (ipMapIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface);
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Unreachables
*
* @param    void
*
* @param    UnitIndex   Unit Index
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapICMPUnreachablesModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * mode)
{
  return ipMapRtrICMPUnreachablesModeGet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Unreachables
*
* @param    UnitIndex   Unit Index
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
L7_RC_t usmDbIpMapICMPUnreachablesModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  return ipMapRtrICMPUnreachablesModeSet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Get the interfcae mode of sending ICMP Redirects
*
* @param    void
*
* @param    UnitIndex   Unit Index
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapIfICMPRedirectsModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * mode)
{
  return ipMapIfICMPRedirectsModeGet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    UnitIndex   Unit Index
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
L7_RC_t usmDbIpMapIfICMPRedirectsModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  return ipMapIfICMPRedirectsModeSet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPRedirectsModeGet(L7_uint32 * mode)
{
  *mode = ipMapRtrICMPRedirectModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPRedirectsModeSet( L7_uint32 mode)
{
  return ipMapICMPRedirectModeSet(mode);
}


/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Echo Replies
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t  usmDbIpMapRtrICMPEchoReplyModeGet(L7_uint32 *mode)
{
  return ipMapRtrICMPEchoReplyModeGet(mode);
}

/*********************************************************************
* @purpose  Set Ignore mode for ICMP ECHO RQUESTS
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPEchoReplyModeSet(L7_uint32 mode)
{
  return ipMapRtrICMPEchoReplyModeSet(mode);
}

/*********************************************************************
* @purpose  setting ICMP Rate Limiting parameters like burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMP messages the IPMAP is allowed to per
                                   interval
* @param    interval   @b{(input)} The time interval between tokens being placed
                                   in the bucket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  To disable ICMP rate limiting, set the interval to zero.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval)
{
  return ipMapRtrICMPRatelimitSet (burstSize, interval);
}

/*********************************************************************
* @purpose  Get ICMP Rate Limiting parameters like burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMP messages the IPMAP is allowed to per
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
L7_RC_t  usmDbIpMapRtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval)
{
  return ipMapRtrICMPRatelimitGet (burstSize, interval);
}

/*********************************************************************
 * @purpose  Sets the bandwidth of the specified interface.
 *
 * @param    intIfNum @b{(input)} Internal Interface Number
 * @param    bandwidth @b{(input)} bandwidth
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR    if interface does not exist
 * @returns  L7_FAILURE  if other failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbIfBandwidthSet(L7_uint32 intIfNum, L7_uint32 bandwidth)
{
  return ipMapIntfBandwidthSet(intIfNum, bandwidth);
}

/*********************************************************************
* @purpose  Get the bandwidth of the specified interface.
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    bandwidth @b{(output)} bandwidth
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIfBandwidthGet(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  return ipMapIntfBandwidthGet(intIfNum, bandwidth);
}

/*********************************************************************
* @purpose  Get the bandwidth of the specified interface without
*           making any modification to the SET bandwidth.
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    bandwidth @b{(output)} bandwidth
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIfBWGet(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  return ipMapIntfBWGet(intIfNum, bandwidth);
}
/*********************************************************************
* @purpose  Get the routing max equal cost entries
*
* @param    UnitIndex @b{(input)} Unit number
* @param    maxhops   @b{(output)} maxhops
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbRtrRouteMaxEqualCostEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxhops)
{
  *maxhops = platRtrRouteMaxEqualCostEntriesGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the routing max routes entries
*
* @param    UnitIndex @b{(input)} Unit number
* @param    maxhops   @b{(output)} maxroutes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbRtrRouteMaxRouteEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxroutes)
{
  *maxroutes=platRtrRouteMaxEntriesGet();
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the Global Default Gateway Address
*
* @param    globalDefGwAddr @b{(input)}  Address of the global default
*                                        gateway
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbIpGlobalDefaultGatewayAddressSet (L7_IP_ADDR_t globalDefGwAddr)
{
  L7_rtrStaticRoute_t globalDefGwRoute;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unitIndex = 0;
  L7_IP_ADDR_t existingDefGw = 0;

  memset (&globalDefGwRoute, 0, sizeof(L7_rtrStaticRoute_t));
  globalDefGwRoute.ipAddr = 0;
  globalDefGwRoute.ipMask = 0;
  globalDefGwRoute.preference =
                    ipMapRouterPreferenceGet(ROUTE_PREF_GLOBAL_DEFAULT_GATEWAY);
  globalDefGwRoute.intIfNum = 0;
  globalDefGwRoute.flags |= L7_RT_GLOBAL_DEFAULT_GATEWAY;

  if (usmDbNetworkAddressValidate(globalDefGwAddr) != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (ipMapGlobalDefaultGatewayAddressGet (&existingDefGw, L7_FALSE)
                                        == L7_SUCCESS)
  {
    globalDefGwRoute.nextHopRtr = existingDefGw;
    rc = ipMapStaticRouteDelete (&globalDefGwRoute);
  }

  globalDefGwRoute.nextHopRtr = globalDefGwAddr;

  rc = ipMapStaticRouteAdd (unitIndex, &globalDefGwRoute);

  return rc;
}

/*********************************************************************
* @purpose  Get the Global Default Gateway Address
*
* @param    globalDefGwAddr @b{(input)}  Pointer to the default global
*                                        gateway address
* @param    isActive        @b{(input)}  Flag to indicate if the
*                                        default gateway is active or
*                                        configured
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    To get the default gateway route in the RTO, set the 
*           argument 'activeFlag' to TRUE.
*           To get the configured default gateway , set the 
*           argument 'activeFlag' to FALSE.
*
* @end
*********************************************************************/
L7_RC_t
usmDbIpGlobalDefaultGatewayAddressGet (L7_IP_ADDR_t *globalDefGwAddr,
                                       L7_BOOL isActive)
{
  return ipMapGlobalDefaultGatewayAddressGet (globalDefGwAddr, isActive);
}

/*********************************************************************
* @purpose  Remove the Global Default Gateway Address
*
* @param none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbIpGlobalDefaultGatewayAddressReset (void)
{
  L7_rtrStaticRoute_t globalDefGwRoute;
  L7_IP_ADDR_t globalDefGwAddr;
  L7_RC_t rc = L7_FAILURE;

  memset (&globalDefGwRoute, 0, sizeof(L7_rtrStaticRoute_t));
  if (ipMapGlobalDefaultGatewayAddressGet (&globalDefGwAddr, L7_FALSE) == L7_SUCCESS)
  {
    globalDefGwRoute.ipAddr = 0;
    globalDefGwRoute.ipMask = 0;
    globalDefGwRoute.nextHopRtr = globalDefGwAddr;
    globalDefGwRoute.preference =
                      ipMapRouterPreferenceGet(ROUTE_PREF_GLOBAL_DEFAULT_GATEWAY);
    globalDefGwRoute.intIfNum = 0;
    globalDefGwRoute.flags |= L7_RT_GLOBAL_DEFAULT_GATEWAY;
  
    rc = ipMapStaticRouteDelete (&globalDefGwRoute);
  }
  return rc;
}

