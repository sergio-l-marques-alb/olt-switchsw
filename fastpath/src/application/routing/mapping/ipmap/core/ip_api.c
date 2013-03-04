
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ip_api.c
*
* @purpose   IP API functions
*
* @component IP Mapping Layer
*
* @comments  Most APIs take either a read or write lock. The ARP APIs are
*            an exception because ARP has its own locks. It's questionable
*            whether the ARP APIs should even be in this file.
*
* @create    03/13/2001
*
* @author    asuthan
*
*            Major revision January 2006, rrice.
*
* @end
*
**********************************************************************/


#define L7_MAC_ENET_BCAST                    /* For broadcast address */

#include "l7_ipinclude.h"
#include "ip_util.h"
#include "dot1q_api.h"
#include "rto_api.h"
#include "l7_mcast_api.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "osapi.h"
#include "osapi_support.h"
#include "ipstk_api.h"
#include "dhcp_client_api.h"
#include "simapi.h"

#ifdef L7_NSF_PACKAGE
#include "ipmap_ckpt.h"
#include "ipmap_dhcp_ckpt.h"
#endif

/* Time to wait for an NSF protocol to finish its route updates after a
 * warm restart. 2 minutes. */
static const L7_uint32 IPMAP_STALE_ROUTE_TIME = 120000;

extern pIpRoutingEventNotifyList_t   pIpIntfStateNotifyList;
extern ipMapInfo_t                *pIpMapInfo;
extern L7_rtrIntfMap_t rtrIntfMap[L7_RTR_MAX_RTR_INTERFACES+1];
extern void *ipMapForwardQueue[L7_L3_FORWARD_QUEUE_COUNT];
extern L7_uint32 ipRxStats[256];
extern void *ipMapMsgQSema;


/*---------------------------------------*/
/* Global Configuration Data Structures  */
/*---------------------------------------*/
extern L7_ipMapCfg_t            *ipMapCfg;
extern L7_rtrStaticRouteCfg_t   *route;
extern L7_rtrStaticArpCfg_t     *arp;

extern L7_uint32 ipMapTraceFlags;

/* forwarding path counters */
extern L7_uint32 ipForwardArpPktsIn;
extern L7_uint32 ipForwardPktsIn0;
extern L7_uint32 ipForwardQueueFull;



/*********************************************************************
* @purpose  Returns ip spoofing mode
*
* @param    void
*
* @returns  L7_ENABLE   If Ip spoofing is enabled
* @returns  L7_DISABLE  If Ip spoofing is disabled
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpSpoofingGet()
{
  L7_uint32 spoofCheck;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  spoofCheck = ipMapCfg->ip.ipSpoofingCheck;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return spoofCheck;
}

/*********************************************************************
* @purpose  Sets Ip Spoofing mode
*
* @param    L7_uint32 mode - enables or disables Ip spoofing
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpSpoofingSet(L7_uint32 mode)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->ip.ipSpoofingCheck = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapIpSpoofingModeApply(mode);
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
}

/*********************************************************************
* @purpose  Returns ip default ttl
*
* @param    void
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpDefaultTTLGet()
{
  L7_uint32 defTtl;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return 0;
  defTtl = ipMapCfg->ip.ipDefaultTTL;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return defTtl;
}

/*********************************************************************
* @purpose  Sets Ip default ttl
*
* @param    ttl  time to live
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpDefaultTTLSet(L7_uint32 ttl)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->ip.ipDefaultTTL = ttl;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapIpDefaultTTLApply(ttl);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
}

/*********************************************************************
* @purpose  Returns contents of the specified ARP entry
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry
* @param    intIfNum   @b{(input)} Internal interface number for entry
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes
* The intIfNum field in pArp will be L7_INVALID_INTF if no interface
* explicitly configured for the entry.
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
                           L7_arpEntry_t *pArp)
{
  return(ipMapArpEntryByAddrGet(ipAddr, intIfNum, pArp));
}

/*********************************************************************
* @purpose  Returns contents of the next ARP entry following the one
*           specified
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry to
*                                    start the search
* @param    intIfNum   @b{(input)} Internal interface number for entry
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes
* The intIfNum field in pArp will be L7_INVALID_INTF if no interface
* explicitly configured for the entry.
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
                            L7_arpEntry_t *pArp)
{
  return(ipMapArpEntryNext(ipAddr, intIfNum, pArp));
}

/*********************************************************************
* @purpose  Purges a specific dynamic/gateway entry from the ARP cache
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry to purge
* @param    intIfNum   @b{(input)} Internal interface number for entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not support deleting local/static ARP entries, as there
*           are other APIs for handling those.
*
*           If intIfNum is L7_INVALID_INTF, ARP entries for IP Address on all
*           interfaces will be purged.
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryPurge(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_BOOL   found = L7_FALSE;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if ( intIfNum != L7_INVALID_INTF )
  {
    return(ipMapArpEntryPurge((L7_uint32)ipAddr, intIfNum));
  }

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
    {
      intIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
      {
        if (ipMapArpEntryPurge((L7_uint32)ipAddr, intIfNum) != L7_FAILURE)
        {
          found = L7_TRUE;
        }
      }
    }
  }

  return( found == L7_TRUE ? L7_SUCCESS : L7_FAILURE );
}

/*********************************************************************
* @purpose  Clears the ARP cache of all dynamic/gateway entries
*
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
L7_RC_t ipMapIpArpCacheClear(L7_BOOL gateway)
{
  return(ipMapArpCacheClear(IP_MAP_ARP_INTF_ALL, gateway));
}

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to ARP cache stats structure to be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpCacheStatsGet(L7_arpCacheStats_t *pStats)
{
  return ipMapArpCacheStatsGet(pStats);
}

/*********************************************************************
* @purpose  Returns Arp entry ageout time
*
* @param    void
*
* @returns  L7_uint32  Arp entry ageout time
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpArpAgeTimeGet()
{
  L7_uint32 ageTime;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return 0;
  ageTime = ipMapCfg->ip.ipArpAgeTime;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return ageTime;
}

/*********************************************************************
* @purpose  Sets the ARP entry ageout time
*
* @param    L7_uint32  ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpAgeTimeSet(L7_uint32 arpAgeTime)
{
  L7_RC_t rc;

  if ((arpAgeTime < L7_IP_ARP_AGE_TIME_MIN) ||
      (arpAgeTime > L7_IP_ARP_AGE_TIME_MAX))
    return L7_FAILURE;

  rc = ipMapArpAgeTimeSet(arpAgeTime);

  if (rc == L7_SUCCESS)
  {
    ipMapCfg->ip.ipArpAgeTime = arpAgeTime;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Returns Arp request response timeout value
*
* @param    void
*
* @returns  L7_uint32  Arp request response timeout value
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpArpRespTimeGet()
{
  L7_uint32 respTime;
  respTime = ipMapCfg->ip.ipArpRespTime;
  return respTime;
}

/*********************************************************************
* @purpose  Sets the ARP request response timeout
*
* @param    L7_uint32  ARP request response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpRespTimeSet(L7_uint32 arpRespTime)
{
  L7_RC_t rc;

  if ((arpRespTime < L7_IP_ARP_RESP_TIME_MIN) ||
      (arpRespTime > L7_IP_ARP_RESP_TIME_MAX))
    return L7_FAILURE;

  rc = ipMapArpRespTimeSet(arpRespTime);

  if (rc == L7_SUCCESS)
  {
    ipMapCfg->ip.ipArpRespTime = arpRespTime;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Returns Arp request max retries count
*
* @param    void
*
* @returns  L7_uint32  Arp request max retries count value
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpArpRetriesGet()
{
  L7_uint32 retries;
  retries = ipMapCfg->ip.ipArpRetries;
  return retries;
}

/*********************************************************************
* @purpose  Sets the ARP request max retries count
*
* @param    L7_uint32  ARP request max retries count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpRetriesSet(L7_uint32 arpRetries)
{
  L7_RC_t rc;

  if ((arpRetries < L7_IP_ARP_RETRIES_MIN) ||
      (arpRetries > L7_IP_ARP_RETRIES_MAX))
    return L7_FAILURE;

  rc = ipMapArpRetriesSet(arpRetries);

  if (rc == L7_SUCCESS)
  {
    ipMapCfg->ip.ipArpRetries = arpRetries;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Returns maximum number of entries allowed in ARP cache
*
* @param    void
*
* @returns  L7_uint32  Arp cache entries max value
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpArpCacheSizeGet()
{
  L7_uint32 cacheSize;
  cacheSize = ipMapCfg->ip.ipArpCacheSize;
  return cacheSize;
}

/*********************************************************************
* @purpose  Sets the maximum number of entries allowed in ARP cache
*
* @param    L7_uint32  ARP cache entries max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpCacheSizeSet(L7_uint32 arpCacheSize)
{
  L7_RC_t rc;

  if ((arpCacheSize < L7_IP_ARP_CACHE_SIZE_MIN) ||
      (arpCacheSize > platRtrArpMaxEntriesGet()))
    return L7_FAILURE;

  rc = ipMapArpCacheSizeSet(arpCacheSize);

  if (rc == L7_SUCCESS)
  {
    ipMapCfg->ip.ipArpCacheSize = arpCacheSize;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Returns ARP dynamic entry renew mode
*
* @param    void
*
* @returns  L7_uint32  Arp dynamic renew mode value (L7_ENABLE, L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpArpDynamicRenewGet()
{
  L7_uint32 renew;
  renew = ipMapCfg->ip.ipArpDynamicRenew;
  return renew;
}

/*********************************************************************
* @purpose  Sets the ARP dynamic entry renew mode
*
* @param    L7_uint32  Arp dynamic renew mode value (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpDynamicRenewSet(L7_uint32 arpDynamicRenew)
{
  L7_RC_t rc;

  if ((arpDynamicRenew != L7_ENABLE) &&
      (arpDynamicRenew != L7_DISABLE))
    return L7_FAILURE;

  rc = ipMapArpDynamicRenewSet(arpDynamicRenew);

  if (rc == L7_SUCCESS)
  {
    ipMapCfg->ip.ipArpDynamicRenew = arpDynamicRenew;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Determines if Ip Source Checking is enabled or disabled
*
* @param    void
*
* @returns  L7_ENABLE   If Ip Source Checking is enabled
* @returns  L7_DISABLE  If Ip Source Checking is disabled
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapIpSourceCheckingGet()
{
  L7_uint32 srcAddrCheck;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  srcAddrCheck = ipMapCfg->ip.ipSourceAddrCheck;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return srcAddrCheck;
}

/*********************************************************************
* @purpose  Sets the IP Source Checking mode
*
* @param    L7_uint32 mode - enables or disables Ip Source Checking mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpSourceCheckingSet(L7_uint32 mode)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->ip.ipSourceAddrCheck = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapIpSourceCheckingApply(mode);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
}

/*********************************************************************
* @purpose  Sets the IP MTU in bytes.
*
* @param    @b{(input)} L7_uint32 ipMtu
* @param    @b{(input)} L7_uint32 intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR if out of range
*
* @notes    A value of 0 means "not configured." When the IP MTU is not
*           configured, the IP MTU is derived from the link max frame size.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_uint32 maxSize;

  if (ipMapIntfMaxIpMtu(intIfNum, &maxSize) != L7_SUCCESS)
    return L7_FAILURE;

  if ((ipMtu != FD_IP_DEFAULT_IP_MTU) &&
      ((ipMtu < L7_L3_MIN_IP_MTU) || (ipMtu > maxSize)))
    return L7_ERROR;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if (ipMtu == pCfg->ipMtu)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  pCfg->ipMtu = ipMtu;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  /* Ignore return code from apply. Return success if stored in config. */
  ipMapIntfIpMtuApply(intIfNum, ipMtu);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the configured IP MTU value
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ipMtu @b{(output)}   configured IP MTU
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfIpMtuGet(L7_uint32 intIfNum, L7_uint32 *ipMtu)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_RC_t rc = L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *ipMtu = pCfg->ipMtu;
    rc = L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
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
L7_RC_t ipMapIntfMaxIpMtuGet( L7_uint32 intIfNum, L7_uint32 *maxIpMtu)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ipMapIntfMaxIpMtu(intIfNum, maxIpMtu);

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Get the IP MTU being enforced on an interface. If the IP MTU
*           is not configured, it is derived from the link MTU.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    pval     @b{(output)} ptr to value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfEffectiveIpMtuGet( L7_uint32 intIfNum, L7_uint32 *pval)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if (pCfg->ipMtu != FD_IP_DEFAULT_IP_MTU)
  {
    *pval = pCfg->ipMtu;
  }
  else if (nimGetIntfMtuSize(intIfNum, pval) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to get link MTU from NIM for interface %s", ifName);
    rc = L7_FAILURE;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Return Router Preference
*
* @param    index   router protocol type
*
* @returns  The router preference associated with the specified
* @reutrns  protocol
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipMapRouterPreferenceGet(L7_uint32 index)
{
  L7_uint32 pref;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_RTO_PREFERENCE_MAX;

  pref = _ipMapRouterPreferenceGet(index);

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return pref;
}

/*********************************************************************
* @purpose  Return the preference type for a route type
*
* @param    protocol @b{(input)} router protocol type
*
* @returns  The preference type associated with the specified
* @returns  protocol
*
* @notes    No need to take semaphore here.
*
* @end
*********************************************************************/
L7_uint32 ipMapRouterPrefTypeGet(L7_uint32 protocol)
{
  L7_uint32 prefType = ROUTE_PREF_LOCAL;

  switch (protocol)
  {
    case RTO_LOCAL:
      prefType = ROUTE_PREF_LOCAL;
      break;
    case RTO_STATIC:
    case RTO_DEFAULT:
      prefType = ROUTE_PREF_STATIC;
      break;
    case RTO_MPLS:
      prefType = ROUTE_PREF_MPLS;
      break;
    case RTO_OSPF_INTRA_AREA:
      prefType = ROUTE_PREF_OSPF_INTRA_AREA;
      break;
    case RTO_OSPF_INTER_AREA:
      prefType = ROUTE_PREF_OSPF_INTER_AREA;
      break;
    case RTO_OSPF_TYPE1_EXT:
    case RTO_OSPF_TYPE2_EXT:
    case RTO_OSPF_NSSA_TYPE1_EXT:
    case RTO_OSPF_NSSA_TYPE2_EXT:
      prefType = ROUTE_PREF_OSPF_EXTERNAL;
      break;
    case RTO_RIP:
      prefType = ROUTE_PREF_RIP;
      break;
    case RTO_IBGP:
      prefType = ROUTE_PREF_IBGP;
      break;
    case RTO_EBGP:
      prefType = ROUTE_PREF_EBGP;
      break;
    default:
      break;
  }
  return prefType;
}

/*********************************************************************
* @purpose  Change the default route preference for routes from a
*           specific protocol.
*
* @param    index   protocol (one of L7_RTO_PROTOCOL_INDICES_t)
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
*           The preference value shall be the same for all the OSPF
*           external route types like type1/type2/nssa1/nssa2.
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterPreferenceSet(L7_uint32 index, L7_uint32 pref)
{
  L7_uint32 prevPref, rtoIndex = RTO_LOCAL;

  /* Check for range */
  if ((pref < L7_RTO_PREFERENCE_MIN) || (pref > L7_RTO_PREFERENCE_MAX))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /*
  ** Save the current preference value because it may need to be restored
  ** if this is indeed an invalid preference change
  */
  prevPref = ipMapCfg->ip.route_preferences[index];

  if (prevPref == pref)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  ipMapCfg->ip.route_preferences[index] = pref;

  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  /* don't change preference of existing static/default routes */
  if (index == ROUTE_PREF_STATIC)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  if (index == ROUTE_PREF_OSPF_EXTERNAL)
  {
    /* Preference value is the same for all the external ospf routes
     * Applying the same external route preference value in RTO for
     * all the external type routes */
    if (! ((rtoRouterPreferenceApply(RTO_OSPF_TYPE1_EXT, pref) == L7_SUCCESS) &&
           (rtoRouterPreferenceApply(RTO_OSPF_TYPE2_EXT, pref) == L7_SUCCESS) &&
           (rtoRouterPreferenceApply(RTO_OSPF_NSSA_TYPE1_EXT, pref) == L7_SUCCESS) &&
           (rtoRouterPreferenceApply(RTO_OSPF_NSSA_TYPE2_EXT, pref) == L7_SUCCESS)) )
    {
      ipMapCfg->ip.route_preferences[index] = prevPref;
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
  }
  else
  {
    switch (index)
    {
      /* STATIC and OSPF_EXTERNAL cases handled above. If included in this switch 
       * statement, coverity barks that the statements are DEADCODE. */
      case ROUTE_PREF_LOCAL:
        rtoIndex = RTO_LOCAL;
        break;
      case ROUTE_PREF_MPLS:
        rtoIndex = RTO_MPLS;
        break;
      case ROUTE_PREF_OSPF_INTRA_AREA:
        rtoIndex = RTO_OSPF_INTRA_AREA;
        break;
      case ROUTE_PREF_OSPF_INTER_AREA:
        rtoIndex = RTO_OSPF_INTER_AREA;
        break;
      case ROUTE_PREF_RIP:
        rtoIndex = RTO_RIP;
        break;
      case ROUTE_PREF_IBGP:
        rtoIndex = RTO_IBGP;
        break;
      case ROUTE_PREF_EBGP:
        rtoIndex = RTO_EBGP;
        break;
      default:
        ipMapCfg->ip.route_preferences[index] = prevPref;
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
        break;
    }
    if (rtoRouterPreferenceApply(rtoIndex, pref) != L7_SUCCESS)
    {
      ipMapCfg->ip.route_preferences[index] = prevPref;
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
  }
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/

/*********************************************************************
* @purpose  Get the administrative mode of the router
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrAdminModeGet(void)
{
  L7_uint32 adminMode;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  adminMode = ipMapCfg->rtr.rtrAdminMode;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return adminMode;
}

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
L7_RC_t ipMapRtrAdminModeSet(L7_uint32 mode)
{
  L7_RC_t rc= L7_SUCCESS;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* Only invoke the Apply function if there is a true change */
  if (ipMapCfg->rtr.rtrAdminMode == mode)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
  {
    ipMapCfg->rtr.rtrAdminMode = mode;
    ipMapAdminModeProcess(mode);
  }
  else if (mode == L7_DISABLE)
  {
    ipMapCfg->rtr.rtrAdminMode = mode;
    ipMapAdminModeProcess(mode);
  }
  else
    rc = L7_FAILURE;

  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Get the administrative mode of TOS forwarding
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   none
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrTosForwardingModeGet(void)
{
  L7_uint32 fwdingMode;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  fwdingMode = ipMapCfg->rtr.rtrTOSForwarding;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return fwdingMode;
}

/*********************************************************************
* @purpose  Enable or disable TOS forwarding
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
L7_RC_t ipMapRtrTosForwardingModeSet(L7_uint32 mode)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->rtr.rtrTOSForwarding = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapRtrTosForwardingModeApply(mode);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
}

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   none
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrICMPRedirectModeGet(void)
{
  L7_uint32 redirectMode;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  redirectMode = ipMapCfg->rtr.rtrICMPRedirectMode;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return redirectMode;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
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
L7_RC_t ipMapICMPRedirectModeSet(L7_uint32 mode)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->rtr.rtrICMPRedirectMode = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapRtrICMPRedirectModeApply(mode);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
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
L7_RC_t  ipMapRtrICMPEchoReplyModeGet(L7_uint32 *mode)
{
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    *mode = L7_DISABLE;
  *mode = ipMapCfg->rtr.rtrICMPEchoReplyMode;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);

  return L7_SUCCESS;
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
L7_RC_t ipMapRtrICMPEchoReplyModeSet(L7_uint32 mode)
{

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  ipMapCfg->rtr.rtrICMPEchoReplyMode = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  (void)ipMapRtrICMPEchoReplyModeApply(mode);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/

/*********************************************************************
* @purpose  Get the gratuitous ARP administrative status for an interface.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    gratArpStatus @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t ipMapGratArpGet(L7_uint32 intIfNum, L7_uint32 *gratArpStatus)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_RC_t rc = L7_FAILURE;

  if (gratArpStatus == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if ((pCfg->flags & L7_RTR_INTF_GRAT_ARP) != 0)
    {
      *gratArpStatus = L7_ENABLE;
    }
    else
    {
      *gratArpStatus = L7_DISABLE;
    }
    rc = L7_SUCCESS;
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable gratuitous ARP on an interface.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    gratArpState  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ipMapGratArpSet(L7_uint32 intIfNum, L7_uint32 gratArpState)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_uint32 curGratArpState = L7_DISABLE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (pCfg->flags & L7_RTR_INTF_GRAT_ARP)
      curGratArpState = L7_ENABLE;
    if (curGratArpState == gratArpState)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
    if (gratArpState == L7_ENABLE)
    {
      pCfg->flags |= L7_RTR_INTF_GRAT_ARP;
    }
    else if (gratArpState == L7_DISABLE)
    {
      pCfg->flags &= (~L7_RTR_INTF_GRAT_ARP);
    }
    else
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
    /* There is no "apply" function. The grat ARP configuration is
     * checked whenever a routing interface is enabled.
     */
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine whether proxy ARP is enabled on an interface
*
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
L7_RC_t ipMapProxyArpGet(L7_uint32 intIfNum, L7_uint32 *proxyArpMode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_RC_t rc = L7_FAILURE;

  if (proxyArpMode == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if ((pCfg->flags & L7_RTR_INTF_PROXY_ARP) != 0)
    {
      *proxyArpMode = L7_ENABLE;
    }
    else
    {
      *proxyArpMode = L7_DISABLE;
    }
    rc = L7_SUCCESS;
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable proxy ARP on an interface
*
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
L7_RC_t ipMapProxyArpSet(L7_uint32 intIfNum, L7_uint32 proxyArpMode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_uint32 curProxyArpMode = L7_DISABLE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (pCfg->flags & L7_RTR_INTF_PROXY_ARP)
      curProxyArpMode = L7_ENABLE;
    if (curProxyArpMode == proxyArpMode)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
    /* If we need to enable the interface */
    if (proxyArpMode == L7_ENABLE)
    {
      pCfg->flags |= L7_RTR_INTF_PROXY_ARP;
    }
    else if (proxyArpMode == L7_DISABLE)
    {
      pCfg->flags &= (~L7_RTR_INTF_PROXY_ARP);
    }
    else
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
    /* There is no "apply" function. The proxy ARP configuration is
     * checked whenever an ARP request is received.
     */
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    ipMapArpProxyArpSet(intIfNum, proxyArpMode);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine whether local proxy ARP is enabled on an interface
*
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
L7_RC_t ipMapLocalProxyArpGet(L7_uint32 intIfNum, L7_uint32 *localProxyArpMode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_RC_t rc = L7_FAILURE;

  if (localProxyArpMode == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if ((pCfg->flags & L7_RTR_INTF_LOCAL_PROXY_ARP) != 0)
    {
      *localProxyArpMode = L7_ENABLE;
    }
    else
    {
      *localProxyArpMode = L7_DISABLE;
    }
    rc = L7_SUCCESS;
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable local proxy ARP on an interface
*
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
L7_RC_t ipMapLocalProxyArpSet(L7_uint32 intIfNum, L7_uint32 localProxyArpMode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_uint32 curLocalProxyArpMode = L7_DISABLE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (pCfg->flags & L7_RTR_INTF_LOCAL_PROXY_ARP)
      curLocalProxyArpMode = L7_ENABLE;
    if (curLocalProxyArpMode == localProxyArpMode)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
    /* If we need to enable the interface */
    if (localProxyArpMode == L7_ENABLE)
    {
      pCfg->flags |= L7_RTR_INTF_LOCAL_PROXY_ARP;
    }
    else if (localProxyArpMode == L7_DISABLE)
    {
      pCfg->flags &= (~L7_RTR_INTF_LOCAL_PROXY_ARP);
    }
    else
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
    /* There is no "apply" function. The local proxy ARP configuration is
     * checked whenever an ARP request is received.
     */
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    ipMapArpLocalProxyArpSet(intIfNum, localProxyArpMode);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           IPv4 network directed broadcast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE
* @returns  L7_DISABLE
*
* @notes    Returns L7_DISABLE if interface is not configurable.
*
* @end
*********************************************************************/
L7_uint32 ipMapNetDirectBcastsCfgGet(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULL;
  L7_uint32 status = L7_DISABLE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->flags & L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE)
    {
      status = L7_ENABLE;
    }
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return status;
}

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           IPv4 network directed broadcast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE
* @returns  L7_DISABLE
*
* @notes    Returns L7_DISABLE if interface is not configurable.
*
* @end
*********************************************************************/
L7_uint32 ipMapRtrIntfNetDirectBcastsGet(L7_uint32 intIfNum)
{
  return ipMapNetDirectBcastsCfgGet(intIfNum);
}

/*********************************************************************
* @purpose  Enable or disable forwarding of net directed broadcasts
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfNetDirectBcastsSet( L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_rtrCfgCkt_t *pCfg = L7_NULL;
  L7_uint32 curState = L7_DISABLE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum,&pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* See if this represents a change */
  if (pCfg->flags & L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE)
    curState = L7_ENABLE;
  if (curState == mode)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
    pCfg->flags |= L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
  else if (mode == L7_DISABLE)
    pCfg->flags &= (~L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE);
  else
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  if (ipMapIntfIsUp(intIfNum))
  {
    rc = ipMapRtrIntfNetDirectBcastsApply(intIfNum, mode);
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
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
L7_RC_t ipMapUnnumberedGet(L7_uint32 intIfNum, L7_BOOL *isUnnumbered,
                           L7_uint32 *numberedIfc)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if (isUnnumbered == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
  {
    *isUnnumbered = L7_TRUE;
    if (numberedIfc)
    {
      /* Get the corresponding numbered interface */
      *numberedIfc = ipMapNumberedIfc(intIfNum);
    }
  }
  else
  {
    *isUnnumbered = L7_FALSE;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_SUCCESS;
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
L7_BOOL ipMapIntfIsUnnumbered(L7_uint32 intIfNum)
{
  L7_BOOL isUnnumbered;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;
  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return isUnnumbered;
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
* @returns  L7_ERROR if attempt to point unnumbered interface at itself.
* @returns  L7_FAILURE
*
* @notes    Fails if an address is already configured on the interface.
*
* @end
*********************************************************************/
L7_RC_t ipMapUnnumberedSet(L7_uint32 intIfNum, L7_BOOL isUnnumbered,
                           L7_uint32 numberedIfc)
{
  L7_rtrCfgCkt_t *pCfg;                /* interface config for intIfNum */
  L7_BOOL currentStatus = L7_FALSE;    /* whether interface is currently unnumbered */
  L7_uint32 currentNumberedIfc;        /* if unnumbered, interface currently referenced */
  L7_RC_t rc = L7_SUCCESS;

  if (mcastIntfIsConfigured(intIfNum) == L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* See if this is a change */
  if (pCfg->flags & L7_RTR_INTF_UNNUMBERED)
    currentStatus = L7_TRUE;
  currentNumberedIfc = ipMapNumberedIfc(intIfNum);
  if ((currentStatus == isUnnumbered) &&
      (currentNumberedIfc == numberedIfc))
  {
    /* no change */
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  /* There is some change */
  if (isUnnumbered)
  {
    /* Make interface unnumbered */
    /* Verify that no IP address is configured */
    if (pCfg->addrs[0].ipAddr)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_ALREADY_CONFIGURED;
    }

    /* Don't allow user to point unnumbered interface at itself. */
    if (intIfNum == numberedIfc)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_ERROR;
    }

    if (currentStatus == isUnnumbered)
    {
      /* Changing the numbered interface. Bounce interface. */
      pCfg->flags &= (~L7_RTR_INTF_UNNUMBERED);
      ipMapNumberedIfcBind(intIfNum, 0);
      ipMapIntfUpdate(intIfNum, NULL);
    }

    pCfg->flags |= L7_RTR_INTF_UNNUMBERED;
    ipMapNumberedIfcBind(intIfNum, numberedIfc);
  }
  else
  {
    /* Remove unnumbered status */
    pCfg->flags &= (~L7_RTR_INTF_UNNUMBERED);
    ipMapNumberedIfcBind(intIfNum, 0);
  }

  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
  rc = ipMapIntfUpdate(intIfNum, NULL);
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Get the borrowed address and mask for an unnnumbered interface
*
* @param    intIfNum   @b{(input)} internal interface number of unnumbered interface
*
* @returns  borrowed IPv4 address. 0 if an error occurs.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapBorrowedAddr(L7_uint32 intIfNum)
{
  L7_uint32 bAddr;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return 0;

  bAddr = _ipMapBorrowedAddr(intIfNum);
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);

  return bAddr;
}

/*********************************************************************
* @purpose  Determine if routing is configured on an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *mode    @b{(output)} L7_ENABLE, if enabled
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE)
    *mode = L7_ENABLE;
  else
    *mode = L7_DISABLE;

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable IPv4 on an interface
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
L7_RC_t ipMapRtrIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 currentMode;
  L7_uint32 oldFlags;

  /*    Validity Checking     */
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum,&pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* If the mode is the same as the current admin mode, return success */
  if ((pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE) == L7_RTR_INTF_ADMIN_MODE_ENABLE)
    currentMode = L7_ENABLE;
  else
    currentMode = L7_DISABLE;
  if (mode == currentMode)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  oldFlags = pCfg->flags;
  if (mode == L7_ENABLE)
  {
    pCfg->flags |= L7_RTR_INTF_ADMIN_MODE_ENABLE;
  }
  else if (mode == L7_DISABLE)
  {
    pCfg->flags &= (~L7_RTR_INTF_ADMIN_MODE_ENABLE);
  }
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  if ((rc = ipMapIntfUpdate(intIfNum, NULL)) != L7_SUCCESS)
  {
    /* Restore flags if call failed */
    pCfg->flags = oldFlags;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           multicast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE, if enabled
* @returns  L7_DISABLE, if disabled
*
* @notes    Returns the configuration even if the interface is not
*           currently a routing interface.
*
*           If the interface is not configurable, returns L7_DISABLE
*
* @end
*********************************************************************/
L7_uint32 ipMapMcastFwdModeCfgGet(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULL;
  L7_uint32 fwdMode = L7_DISABLE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_DISABLE;
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE)
    {
      fwdMode = L7_ENABLE;
    }
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return fwdMode;
}

/*********************************************************************
* @purpose  Obtain multicast forwarding admin mode of router interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE, if enabled
* @returns  L7_DISABLE, if disabled
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipMapRtrIntfMcastFwdModeGet(L7_uint32 intIfNum)
{
  return ipMapMcastFwdModeCfgGet(intIfNum);
}

/*********************************************************************
* @purpose  Enable or disable multicast forwarding on router interface
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
L7_RC_t ipMapRtrIntfMcastFwdModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 fwdMode = L7_DISABLE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if (pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE)
  {
    fwdMode = L7_ENABLE;
  }
  if (fwdMode == mode)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
  {
    pCfg->flags |= L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
  }
  else if (mode == L7_DISABLE)
  {
    pCfg->flags &= (~L7_RTR_INTF_MCAST_FWD_MODE_ENABLE);
  }
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  if (ipMapIntfIsUp(intIfNum))
  {
    rc = ipMapRtrIntfMcastFwdModeApply(intIfNum, mode);
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}


/*********************************************************************
* @purpose  Returns the primary IP Address associated with the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *ipAddr @b{(input)} Pointer to IP address of router interface
* @param    *mask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS  if the interface is configured as a router interface
* @returns  L7_FAILURE  if the interface is not configured as a router interface
*
* @notes    This routine returns a valid IP address only if the interface
* @notes    is configured as a router interface.
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfIpAddressGet(L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr,
                                  L7_IP_MASK_t *mask)
{
  return ipMapRtrIntfCfgIpAddressGet(intIfNum, ipAddr, mask);
}

/*********************************************************************
* @purpose  Find the network mask associated with a specific IP address
*           on a given interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    ipAddr   @b{(input)} Pointer to IP address of router interface
* @param    mask     @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS    if router interface address found
* @returns  L7_NOT_EXIST  if router interface address not found
* @returns  L7_FAILURE    if interface not a valid IPv4 interface
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSpecificIpAddressGet(L7_uint32 intIfNum,
                                          L7_IP_ADDR_t *ipAddr,
                                          L7_IP_MASK_t *mask)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 j;

  if (!ipAddr || !mask || (*ipAddr == 0))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr && (pCfg->addrs[j].ipAddr == *ipAddr))
      {
        *mask = pCfg->addrs[j].ipMask;
        ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
        return L7_SUCCESS;
      }
    }

    /* Matching address not found on the interface */
    *mask = 0;
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_NOT_EXIST;
  }
  else
  {
    *mask = 0;
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Returns the primary IP Address associated with the routing
*           interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *pIpAddr @b{(input)} Pointer to IP address of router interface
* @param    *pMask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine returns the IP address and mask stored in the
* @notes    configuration file.
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddressGet(L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr,
                                     L7_IP_MASK_t *mask)
{
  L7_rtrCfgCkt_t *pCfg;

  if (!ipAddr || !mask)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *ipAddr = pCfg->addrs[0].ipAddr;
    *mask   = pCfg->addrs[0].ipMask;
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  *ipAddr = 0;
  *mask = 0;

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Returns the matching subnet mask on
*           a routing interfaces given the ip address
*
* @param    intIfNum @b{(input)}  Internal Interface Number
* @param    ipAddr   @b{(input)}  IP address
* @param    ipMask   @b{(output)} IP mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSubnetMaskGet(L7_uint32 intIfNum,
                                   L7_IP_ADDR_t ipAddr, 
                                   L7_IP_MASK_t *ipMask)
{
  L7_uint32      j;
  L7_rtrCfgCkt_t *pCfg;

  if (!ipAddr || !ipMask)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    for (j = 0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if ((pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) ==
          (ipAddr & pCfg->addrs[j].ipMask))
      {
        *ipMask = pCfg->addrs[j].ipMask;
        ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
        return L7_SUCCESS;
      }
    }
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;    /* address not found */
  }

  *ipMask = 0;

  /* interface is invalid */
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Returns the IP Address Configuration Method of the Router
*           Interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *method  @b{(output)} Pointer to IP address method of router
*                                 interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine returns the IP address configuration method
*           stored in the configuration file.
*
* @end
*********************************************************************/
L7_RC_t
ipMapRtrIntfCfgIpAddressMethodGet (L7_uint32 intIfNum,
                                   L7_INTF_IP_ADDR_METHOD_t *method)
{
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (((pCfg->flags) & L7_RTR_INTF_ADDR_METHOD_DHCP)
                      == L7_RTR_INTF_ADDR_METHOD_DHCP)
    {
      *method = L7_INTF_IP_ADDR_METHOD_DHCP;
    }
    else if ((pCfg->addrs[0].ipAddr != 0) && (pCfg->addrs[0].ipMask != 0))
    {
      *method = L7_INTF_IP_ADDR_METHOD_CONFIG;
    }
    else
    {
      *method = L7_INTF_IP_ADDR_METHOD_NONE;
    }

    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  *method = L7_INTF_IP_ADDR_METHOD_NONE;

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  Get IP subnet broadcast address for the primary IP address
*           on the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    bcast      (output) Interface IP broadcast address.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE Interface is not valid.
*
* @notes    Outputs the IP subnet broadcast address, not the layer 2
*           MAC broadcast address.
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterIfIPBroadcastGet (L7_uint32 intIfNum, L7_uint32 *bcast)
{
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *bcast = (pCfg->addrs[0].ipAddr & pCfg->addrs[0].ipMask) | ~(pCfg->addrs[0].ipMask);
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get MAC address for the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    mac_addr   (output) MAC address for the specified interface.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR   NIM says interface not active
* @returns  L7_FAILURE
*
* @notes    Doesn't touch any IP MAP data. No need to take IP MAP lock.
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterIfMacAddressGet(L7_uint32 intIfNum, L7_uchar8 *mac_addr)
{
  L7_uint32 adminState, activeState;

  if ( (nimGetIntfAdminState(intIfNum, &adminState) != L7_SUCCESS) ||
       (adminState != L7_ENABLE) ||
       (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) ||
       (activeState != L7_ACTIVE) )
  {
    return(L7_ERROR);
  }

  if (nimGetIntfL3MacAddress(intIfNum, 0, mac_addr) != L7_SUCCESS)
  {
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Determine whether IPv4 routing is configured on a given interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number to check
*
* @returns  L7_TRUE if routing is configured
*           L7_FALSE if routing is not configured
*
* @notes    Routing is considered configured under the following conditions:
*              - routing is enabled at the box level
*              - routing is enabled at the interface level
*              - the interface either is configured with a primary IP address, or
*                the interface is configured to be unnumbered
*
* @end
*********************************************************************/
L7_BOOL ipMapRtrIntfConfigured(L7_uint32 intIfNum)
{
  L7_BOOL isConfigured = L7_FALSE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;

  isConfigured = _ipMapRtrIntfConfigured(intIfNum);

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return isConfigured;
}

/*********************************************************************
* @purpose  Determine if an IP address is a primary or secondary address
*           on a given interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number to check
* @param    *pIpAddr @b{(input)} IP address to check. May not be 0.
*
* @returns  L7_SUCCESS  if the address is configured on the interface
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if the address is not configured on the interface
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddressCheck(L7_uint32 intIfNum,
                                       L7_IP_ADDR_t ipAddr)
{
  L7_uint32   j;
  L7_rtrCfgCkt_t  *pCfg;

  if (ipAddr == 0)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr == ipAddr)
      {
        ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
        return L7_SUCCESS;
      }
    }
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;    /* address not found */
  }
  else
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_ERROR;      /* invalid interface */
  }
}

/*********************************************************************
* @purpose  Sets the primary IP address on an interface
*
* @param    intIfNum          Internal interface number out of which address
*                             is reachable
* @param    ipAddress         IP Address of the interface
* @param    subnetMask        Subnet Mask of the interface
* @param    method            IP Address Configuration Method
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified IP address and
*                             and address on a network or service port
* @returns  L7_REQUEST_DENIED Return this error if the ipAddress is same as
*                             next hop address of a static route
* @returns  L7_ALREADY_CONFIGURED  if one or more secondary addresses are configured
* @returns  L7_NOT_EXIST      if the interface is configured to be unnumbered
* @returns  L7_TABLE_IS_FULL  if the routing table is full and the corresponding
*                             local route can't be added
* @returns  L7_FAILURE        other errors or failures
*
* @notes    If a primary address is already configured on the interface, removes
*           it first. This will temporarily bring the IP interface down.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressSet(L7_uint32 intIfNum,
                                 L7_IP_ADDR_t ipAddress,
                                 L7_IP_MASK_t subnetMask,
                                 L7_INTF_IP_ADDR_METHOD_t method)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 j;
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Fail if interface is configured to be unnumbered */
  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_NOT_EXIST;        /* stretching to find an unnused code! */
  }

  /* Check if the IP address is valid */
  if (ipMapRtrIntfIpAddressValidityCheck(ipAddress, subnetMask) != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Check if the same address is already configured on this interface. */
  if ((ipAddress == pCfg->addrs[0].ipAddr) && (subnetMask == pCfg->addrs[0].ipMask))
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  /* Check if address conflicts with network or service port. */
  if (ipMapMgmtPortConflict(intIfNum, ipAddress, subnetMask) ||
      ipMapRtrIntfAddressConflictFind(intIfNum, ipAddress, subnetMask))
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_ERROR;
  }

  /* Check if the ipAddress is same as the next hop address of a static route */
  if (ipMapSrNextHopIpAddressConflictCheck(ipAddress) == L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_REQUEST_DENIED;
  }

  /* Cannot configure an IP address that is already configured as a
   * static ARP entry. */
  if (ipMapStaticArpIpAddressConflictCheck(ipAddress) == L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_REQUEST_DENIED;
  }


  /*
  Don't allow a primary IP address to be changed if any secondaries
are configured. See defect 48936.
*/
  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr != L7_NULL_IP_ADDR)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_ALREADY_CONFIGURED;
    }
  }

  /* If a primary IP address is already configured, remove it. */
  if (pCfg->addrs[0].ipAddr != 0)
  {
    if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
    {
      /* Invoke DHCP Client to Release the IP address on this interface.
       * This has to be done immediately in order to get the the DHCP Release
       * out on the wire.
       */
      if (dhcpClientIPAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_METHOD_NONE,
                                        L7_MGMT_IPPORT, L7_FALSE)
                                     != L7_SUCCESS)
      {
        if (ipMapTraceFlags & IPMAP_TRACE_DHCP_UPDATE)
        {
          L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
          osapiSnprintf (traceBuf, sizeof(traceBuf),
                         "[%s-%d]: DHCP Address Release Failed on intIfNum %d\n",
                         __FUNCTION__, __LINE__, intIfNum);
          ipMapTraceWrite (traceBuf);
        }
      }
    }
    rc = ipMapRtrIntfIpAddressRemoveProcess(intIfNum);
    if (rc != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed removing IP address from interface %s. Error %d.",
              ifName, rc);
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
  }

  /* Reserve space in RTO for this address */
  if (rtoRouteReserve() != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_TABLE_IS_FULL;
  }

  /* Store the new IP address in the configuration. */
  pCfg->addrs[0].ipAddr = ipAddress;
  pCfg->addrs[0].ipMask = subnetMask;
  if ((method == L7_INTF_IP_ADDR_METHOD_CONFIG) &&
      ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0))
  {
    pCfg->flags &= ~L7_RTR_INTF_ADDR_METHOD_DHCP;
  }
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapRtrIntfIpAddressAddProcess(intIfNum, ipAddress, subnetMask);

  /* If address add failed, undo the config change */
  if (rc != L7_SUCCESS)
  {
    pCfg->addrs[0].ipAddr = 0;
    pCfg->addrs[0].ipMask = 0;
    rtoRouteUnReserve();
  }
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Sets/Resets the DHCP IP address method on an interface
*
* @param    intIfNum          Internal interface number out of which 
*                             address is reachable
* @param    method            IP Address Configuration Method
* @param    actImmediate      Immediate action flag
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_FAILURE        other errors or failures
*
* @notes    If DHCP is configured, this API invokes the DHCP Client to
*           acquire an IP address for this interface.
*           If DHCP is unconfigured, this API invokes the DHCP Client to
*           release the IP address on this interface.
*
* @end
*********************************************************************/
L7_RC_t
ipMapRtrIntfIpAddressMethodSet (L7_uint32 intIfNum,
                                L7_INTF_IP_ADDR_METHOD_t method,
                                L7_BOOL actImmediate)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 secAddrIndex = 0;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Fail if interface is configured to be unnumbered */
  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_NOT_EXIST;        /* stretching to find an unnused code! */
  }

  if (method == L7_INTF_IP_ADDR_METHOD_DHCP) /* Acquire an address */
  {
    /* Check if DHCP Mode is already configured on this interface */
    if (((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0) &&
        (pCfg->addrs[0].ipAddr != 0))
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    /* If a primary IP address is already configured, remove it. */
    if (pCfg->addrs[0].ipAddr != 0)
    {
      /* Remove Secondary IP addresses first before removing the primary address.
       */
      for (secAddrIndex = 1; secAddrIndex < L7_L3_NUM_IP_ADDRS; secAddrIndex++)
      {
        L7_IP_ADDR_t ipAddr = 0;
        L7_IP_MASK_t ipMask = 0;

        ipAddr = pCfg->addrs[secAddrIndex].ipAddr;
        ipMask = pCfg->addrs[secAddrIndex].ipMask;
        if ((ipAddr != L7_NULL_IP_ADDR) || (ipMask != L7_NULL_IP_ADDR))
        {
          if (ipMapRtrIntfSecondaryIpAddrRemove (intIfNum, ipAddr, ipMask)
                                              != L7_SUCCESS)
          {
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
            return L7_FAILURE;
          }
        }
      }

      if (ipMapRtrIntfIpAddressRemoveProcess (intIfNum) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];     
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Failed removing IP address from interface %s.", ifName);
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
      }
    }

    pCfg->flags |= L7_RTR_INTF_ADDR_METHOD_DHCP;
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

    if (ipMapIntfUpdate (intIfNum, L7_NULL) != L7_SUCCESS)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_FAILURE;
    }
  }
  else if (method == L7_INTF_IP_ADDR_RENEW) /* Renew the address */
  {
    IPMAP_RTR_INTF_ENABLE_MODE_t enableMode;

    /* Check if DHCP Mode is already configured on this interface */
    if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) == 0)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    /* Try to Renew only if a primary IP address is present */
    if (pCfg->addrs[0].ipAddr != 0)
    {
      /* Invoke DHCP Client to Renew the IP address on this interface. */
      if (dhcpClientIPAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_RENEW,
                                        L7_MGMT_IPPORT, L7_FALSE)
                                     != L7_SUCCESS)
      {
        if (ipMapTraceFlags & IPMAP_TRACE_DHCP_UPDATE)
        {
          L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
          osapiSnprintf (traceBuf, sizeof(traceBuf),
                         "[%s-%d]: DHCP Address Renew Failed on intIfNum %d\n",
                         __FUNCTION__, __LINE__, intIfNum);
          ipMapTraceWrite (traceBuf);
        }
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
      }
    }
    else
    {
      enableMode = ipMapMayEnableInterface(intIfNum);
      if (ipMapIntfEnable (intIfNum, enableMode, method) != L7_SUCCESS)
      {
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
      }
    }
  }
  /* Release the Leased IP address and Set the Method to None */
  else if ((method == L7_INTF_IP_ADDR_METHOD_NONE) ||
           (method == L7_INTF_IP_ADDR_RELEASE))
  {
    L7_uint32 interfaceNum = intIfNum;

    /* Check if DHCP Mode is already configured on this interface */
    if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) == 0) 
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    /* Invoke DHCP Client to Release the IP address on this interface.
     * After releasing the address, DHCP client invokes the respective IPMAP
     * API to remove the IP address.
     */
    if (actImmediate == L7_TRUE)
    {
      if (_ipMapIntIfNumToRtrIntf (intIfNum, &interfaceNum) != L7_SUCCESS)
      {
        if (method == L7_INTF_IP_ADDR_METHOD_NONE)
        {
          pCfg->flags &= ~L7_RTR_INTF_ADDR_METHOD_DHCP;
          ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
        }
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_SUCCESS;
      }
    }
    if (dhcpClientIPAddressMethodSet (interfaceNum, L7_INTF_IP_ADDR_METHOD_NONE,
                                      L7_MGMT_IPPORT, actImmediate)
                                   != L7_SUCCESS)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_DHCP_UPDATE)
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        osapiSnprintf (traceBuf, sizeof(traceBuf),
                       "[%s-%d]: DHCP Address Release Failed on intIfNum %d\n",
                       __FUNCTION__, __LINE__, intIfNum);
        ipMapTraceWrite(traceBuf);
      }
    }
    if (actImmediate == L7_TRUE)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__); /* Minor hack to get rid of IPMAP locks */
      if (ipMapRtrIntfIpAddressRemove (intIfNum, pCfg->addrs[0].ipAddr,
                                       pCfg->addrs[0].ipMask)
                                    != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
        return L7_FAILURE;
    }
    if (method == L7_INTF_IP_ADDR_METHOD_NONE)
    {
      pCfg->flags &= ~L7_RTR_INTF_ADDR_METHOD_DHCP;
      ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Do Nothing */
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds a secondary IP address to an interface
*
* @param    intIfNum    Internal interface number where the address is configured
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified ip
*                             address & an already configured ip
*                             address
* @returns  L7_TABLE_IS_FULL  exceeded maximum number of addresses allowable
* @returns  L7_FAILURE        other errors or failures
*
* @notes    Error codes have to match those for ipMapRtrIntfIpAddressSet().
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrAdd(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                       L7_IP_MASK_t subnetMask)
{
  L7_RC_t            rc = L7_FAILURE;
  L7_uint32          j;
  L7_rtrCfgCkt_t     *pCfg;

  if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: adding secondary addr to intf %d\n",
            intIfNum);
    ipMapTraceWrite(traceBuf);
  }

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Secondary addresses may only be configured once a primary IP address has been
   * assigned to the interface */
  if (pCfg->addrs[0].ipAddr == 0)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: intf %d no primary address\n",
              intIfNum);
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_NOT_EXIST;
  }

  /* Don't allow the same secondary IP address as the primary */
  if (pCfg->addrs[0].ipAddr == ipAddress)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_ALREADY_CONFIGURED;
  }

  /* Check if the IP address network number is valid */
  if (ipMapRtrIntfIpAddressValidityCheck(ipAddress, subnetMask) != L7_SUCCESS)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: intf %d secondary addr invalid\n",
              intIfNum);
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Do nothing if the specified IP address and mask are already configured
   * as a secondary */
  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if ((ipAddress == pCfg->addrs[j].ipAddr) && (subnetMask == pCfg->addrs[j].ipMask))
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
  }

  /* Verify that there is no subnet conflict with the existing IP configuration */
  if (ipMapMgmtPortConflict(intIfNum, ipAddress, subnetMask) ||
      ipMapRtrIntfAddressConflictFind(intIfNum, ipAddress, subnetMask))
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: intf %d subnet conflict\n",
              intIfNum);
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_ERROR;
  }

  /* Check if the ipAddress is same as the next hop address of a static route */
  if (ipMapSrNextHopIpAddressConflictCheck(ipAddress) == L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_REQUEST_DENIED;
  }

  /* Cannot configure an IP address that is already configured as a
   * static ARP entry. */
  if (ipMapStaticArpIpAddressConflictCheck(ipAddress) == L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_REQUEST_DENIED;
  }

  /* Find a vacant index position to add the new secondary address */
  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
    {
      break;
    }
  }

  /*
  If the maximum number of secondary addresses have already been configured return with
  a failure
  */

  if (j >= L7_L3_NUM_IP_ADDRS)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: Secondary address table full!\n");
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_TABLE_IS_FULL;
  }

  if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrAdd: adding secondary addr to slot %d\n", j);
    ipMapTraceWrite(traceBuf);
  }

  /* Reserve space in RTO for a local route */
  if (rtoRouteReserve() != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_TABLE_IS_FULL;
  }

  pCfg->addrs[j].ipAddr = ipAddress;
  pCfg->addrs[j].ipMask = subnetMask;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rc = ipMapRtrIntfSecondaryIpAddrAddProcess(intIfNum, ipAddress, subnetMask, j);
  if (rc != L7_SUCCESS)
  {
    pCfg->addrs[j].ipAddr = 0;
    pCfg->addrs[j].ipMask = 0;
    rtoRouteUnReserve();
  }
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  return rc;
}

/*********************************************************************
* @purpose  Removes the primary IP address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR        Attempt to remove primary ip address prior
*                           to removing secondary address configuration
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressRemove(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                    L7_IP_MASK_t subnetMask)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 j;
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Check if the IP address and mask belong to this interface */
  if ((ipAddress != pCfg->addrs[0].ipAddr) || (subnetMask != pCfg->addrs[0].ipMask))
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* The primary ip address can be removed if and only if there are no configured
  ** secondary addresses
  */
  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr != L7_NULL_IP_ADDR)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_ERROR;
    }
  }

  rc = ipMapRtrIntfIpAddressRemoveProcess(intIfNum);
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Removes a secondary IP address configured on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrRemove(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                          L7_IP_MASK_t subnetMask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 j = 0;
  L7_rtrCfgCkt_t *pCfg;

  if ((ipAddress == 0) || (subnetMask == 0))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrRemove: intf %d not configurable\n",
              intIfNum);
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Check if the IP address and mask belong to this interface */
  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if ((ipAddress == pCfg->addrs[j].ipAddr) &&
        (subnetMask == pCfg->addrs[j].ipMask))
    {
      break;
    }
  }

  /* If we could not find the specified IP address return with an error */

  if (j >= L7_L3_NUM_IP_ADDRS)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddrRemove: could not find 0x%08x/0x%08x on %d\n",
              ipAddress, subnetMask, intIfNum);
      ipMapTraceWrite(traceBuf);
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_ERROR;
  }

  pCfg->addrs[j].ipAddr = L7_NULL_IP_ADDR;
  pCfg->addrs[j].ipMask = L7_NULL_IP_MASK;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  rtoRouteUnReserve();

  rc = ipMapRtrIntfSecondaryIpAddrRemoveProcess(intIfNum, ipAddress, subnetMask, j);

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Removes all IP addresses configured on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_FAILURE      The interface is not a valid IP interface
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressesRemove(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t mask;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32 i;
  L7_BOOL isDhcpIntf = L7_FALSE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
  {
    isDhcpIntf = L7_TRUE;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  if (isDhcpIntf == L7_TRUE)
  {
    if (ipMapRtrIntfIpAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_METHOD_NONE,
                                        L7_TRUE)
                                     != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  rc = ipMapRtrIntfIpAddrListGet( intIfNum, ipAddrList);

  if (rc == L7_SUCCESS)
  {
    ipAddr = ipAddrList[0].ipAddr;
    mask = ipAddrList[0].ipMask;
    if ((ipAddr== L7_NULL_IP_ADDR) || (mask== L7_NULL_IP_ADDR) )
    {
      return L7_SUCCESS;
    }
    /* Removing Secondary Addresses on the interface */
    for (i = 1; i <= L7_L3_NUM_IP_ADDRS - 1; i++)
    {
      ipAddr = ipAddrList[i].ipAddr;
      mask = ipAddrList[i].ipMask;

      if(ipAddr != L7_NULL_IP_ADDR && mask != L7_NULL_IP_MASK)
      {
        rc = ipMapRtrIntfSecondaryIpAddrRemove(intIfNum,ipAddr,mask);
        if (rc != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
    memset (&ipAddr,0,sizeof(L7_IP_ADDR_t));
    memset (&mask,0,sizeof(L7_IP_MASK_t));

    /* Removing Primary Address on the interface */
    ipAddr = ipAddrList[0].ipAddr;
    mask = ipAddrList[0].ipMask;
    rc = ipMapRtrIntfIpAddressRemove(intIfNum,ipAddr,mask);
  }

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Check to see if the IP address conflicts with that of
*           any other routing interface
*
* @param    intIfNum        Internal interface where address is configured
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_TRUE      conflict
* @returns  L7_FALSE     no conflict
*
* @notes    Two IP addresses conflict if they are in the same IP subnet or
*           are in overlapping subnets.
*           On Linux, we allow two IP addresses in the same subnet on the
*           same interface.
*
* @end
*********************************************************************/
L7_BOOL ipMapRtrIntfAddressConflict(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                    L7_IP_MASK_t subnetMask)
{
  L7_BOOL conflictFound;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;

  conflictFound = ipMapRtrIntfAddressConflictFind(intIfNum, ipAddress, subnetMask);
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return conflictFound;
}

/*********************************************************************
* @purpose  Sets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    encapType  @b{(output)} Encapsulation Type (L7_ENCAPSULATION_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfEncapsTypeSet(L7_uint32 intIfNum, L7_uint32 encapType)
{
  L7_RC_t rc = L7_SUCCESS;
  dtlRtrIntfDesc_t ipCircDesc;

  if ((encapType != L7_ENCAP_ETHERNET) && (encapType != L7_ENCAP_802))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapIntfIsUp(intIfNum))
  {
    if (ipMapRtrIntfDataGet(intIfNum, &ipCircDesc) == L7_SUCCESS)
    {
      ipCircDesc.llEncapsType = (L7_ENCAPSULATION_t) encapType;
      rc = dtlIpv4RtrIntfModify(&ipCircDesc);
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return(rc);
}

/*********************************************************************
* @purpose  Add a static ARP entry
*
* @param    ipAddress       IP Address
* @param    pLLAddr         Pointer to link layer address information
*                               in L7_linkLayerAddr_t
* @param    intIfNum        Interface for the entry. Set to L7_INVALID_INTF if
*                           interface not explicitly configured.
*
* @returns  L7_SUCCESS     static ARP entry is added to the configuration and
*                          the ARP cache
*           L7_NOT_EXIST   static ARP entry is added to the configuration, but
*                          not to the ARP cache because the IP address is not in
*                          an IP subnet on a local interface
*           L7_ADDR_INUSE  rejected. ipAddress is assigned to a local interface
*           L7_TABLE_IS_FULL  rejected. the max number of static entries is already configured
*           L7_FAILURE     rejected. any other failure
*
* @notes    We do allow a user to configure a static ARP entry mapping a unicast
*           IP address to a multicast MAC address. This is required to support
*           Network Load Balancing (NLB), a microsoft protocol which requires the
*           router to forward packets to a virtual IP address to all servers in
*           a cluster.
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpAdd(L7_IP_ADDR_t ipAddress, L7_linkLayerAddr_t *pLLAddr,
                          L7_uint32 intIfNum)
{
  L7_enetMacAddr_t  nullMac = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  L7_enetMacAddr_t  bcastMac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
  L7_uint32 i;
  L7_uint32 freeEntry;
  nimConfigID_t intfConfigId;
  L7_uint32 localIntf = 0;
  L7_RC_t rc = L7_SUCCESS;

  /* Don't allow a null MAC or a broadcast MAC address to be used here */
  if ((memcmp(pLLAddr->addr.enetAddr.addr, nullMac.addr, L7_ENET_MAC_ADDR_LEN) == 0) ||
      (memcmp(pLLAddr->addr.enetAddr.addr, bcastMac.addr, L7_ENET_MAC_ADDR_LEN) == 0))
    return L7_FAILURE;

  if ((ipAddress == L7_NULL_IP_ADDR) || (ipAddress == INADDR_BROADCAST) ||
      (ipAddress >= 0xe0000000))
    return L7_FAILURE;

  /* reject the loopback address */
  if (((ipAddress & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET)
    return L7_FAILURE;

  memset(&intfConfigId, 0, sizeof(intfConfigId));
  /* Get nimConfigId_t structure for interface configured */
  if ( intIfNum != L7_INVALID_INTF &&
       nimConfigIdGet(intIfNum, &intfConfigId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (_ipMapIpAddressToIntf(ipAddress, &localIntf) == L7_SUCCESS)
  {
    /* address is assigned to a local interface. Don't accept static ARP entry. */
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_ADDR_INUSE;
  }

  /*Put it in static arp config structure*/
  freeEntry = L7_RTR_MAX_STATIC_ARP_ENTRIES;
  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr == L7_NULL_IP_ADDR)
      if (freeEntry > i)
        freeEntry = i;
    if (arp->rtrStaticArpCfgData[i].ipAddr == ipAddress)
    {
      /* Check if new entry is a duplicate of existing entry. */

      /* case where interface is not specified explicitly */
      if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId,
                                  &intfConfigId) == L7_TRUE)
      {
        if (memcmp(arp->rtrStaticArpCfgData[i].macAddr.addr.enetAddr.addr,
                   pLLAddr->addr.enetAddr.addr, L7_ENET_MAC_ADDR_LEN) == 0)
        {
          ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
          return L7_SUCCESS;
        }
        else
        {
          /* same IP address, different MAC. Replace the MAC address. */
          break;
        }
      }
    }
  }

  if (i < L7_RTR_MAX_STATIC_ARP_ENTRIES)
    freeEntry = i;

  /* check for no more room in config */
  if (freeEntry == L7_RTR_MAX_STATIC_ARP_ENTRIES)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_TABLE_IS_FULL;
  }

  arp->rtrStaticArpCfgData[freeEntry].ipAddr = ipAddress;
  memcpy(arp->rtrStaticArpCfgData[freeEntry].macAddr.addr.enetAddr.addr,
         pLLAddr->addr.enetAddr.addr, L7_ENET_MAC_ADDR_LEN);

  /* Store the interface configuration */
  NIM_CONFIG_ID_COPY(&arp->rtrStaticArpCfgData[freeEntry].intfConfigId, &intfConfigId);

  arp->cfgHdr.dataChanged = L7_TRUE;

  /* If interface not expliclity configured, or configured interface is up,
   * try to apply the static ARP entry. */
  if ((intIfNum == L7_INVALID_INTF) || ipMapIntfIsUp(intIfNum))
  {
    /* If an error occurs within the apply function, the apply function
     * should note the error. This function considers the operation
     * a success if the static ARP entry is added to the configuration (it has
     * been at this point). If no matching interface is up, the apply may
     * happen later (ipMapRtrIntfStaticConfigApply()). In this case, we get
     * back L7_NOT_EXIST, and use that as a clue to the caller. */
    rc = ipMapStaticArpAddApply(ipAddress, intIfNum, pLLAddr);
    if (rc != L7_NOT_EXIST)
    {
      rc = L7_SUCCESS;
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Delete a static ARP entry
*
* @param    ipAddress       IP Address
* @param    intIfNum        Interface configured for the entry. Set to
*                           L7_INVALID_INTF if interface not explicitly given
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      Invalid address or could not delete entry
* @returns  L7_NOT_FOUND    If entry is not found.
*
* @notes    Does not take the IP MAP lock. Probably should take the ARP lock.
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpDelete(L7_IP_ADDR_t ipAddress, L7_uint32 intIfNum)
{
  L7_uint32 i, j;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 index = L7_RTR_MAX_STATIC_ARP_ENTRIES;
  L7_uint32 count = 0;
  nimConfigID_t intfConfigId;
  L7_uint32 cfgIntIfNum = L7_INVALID_INTF;
  L7_BOOL duplicateFound = L7_FALSE;

  if ((ipAddress == L7_NULL_IP_ADDR) || (ipAddress == INADDR_BROADCAST) ||
      (ipAddress >= 0xe0000000))
    return L7_FAILURE;

  /* reject the loopback address */
  if (((ipAddress & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET)
    return L7_FAILURE;

  memset(&intfConfigId, 0, sizeof(intfConfigId));
  /* Get nimConfigId_t structure for interface configured */
  if ( intIfNum != L7_INVALID_INTF &&
       nimConfigIdGet(intIfNum, &intfConfigId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Lookup the static arp in the configuration to find number of entries
   * matching IP address. Also, store index to first matching entry */
  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr != ipAddress)
    {
      continue;
    }
    count++;

    /* Check if interface specification matches */
    if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId,
                                &intfConfigId) == L7_TRUE)
    {
      index = i;
    }
  }

  /* If it doesn't exist, return failure*/
  if (index >= L7_RTR_MAX_STATIC_ARP_ENTRIES)
    return L7_NOT_EXIST;

  /* Check if there is a static entry that can replace entry being deleted.
   * Its possible in case given below,
   * a. Address 1.1.1.2 on interface 1
   * b. Address 1.1.1.2 without any interface.
   * c. IP address on interface 1 is 1.1.1.1/24
   * Assume, entry a. is added into ARP cache. If entry a. is deleted, need
   * to add back entry b. into ARP cache.
   *
   * First find if there is any duplicate entry with same IP Address on
   * interface given
   */

  /* Find interface if not specified explicitly */
  if (intIfNum == L7_INVALID_INTF)
  {
    if ( ipMapRouterIfResolve(ipAddress, &intIfNum) != L7_SUCCESS )
    {
      intIfNum = L7_INVALID_INTF;
    }
  }

  /* Interface found, now find a duplicate static ARP entry if any */
  if ( intIfNum != L7_INVALID_INTF )
  {
    if ( count > 1 )
    {
      memset(&intfConfigId, 0, sizeof(intfConfigId));
      /* Find entry with same IP address and interface */
      for (j = 0; j < L7_RTR_MAX_STATIC_ARP_ENTRIES; j++)
      {
        /* Ignore if its entry being deleted or different IP address */
        if (index == j || arp->rtrStaticArpCfgData[j].ipAddr != ipAddress)
        {
          continue;
        }

        /* Compute interface number for configured entry */
        cfgIntIfNum = L7_INVALID_INTF;
        if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[j].intfConfigId,
                                    &intfConfigId) == L7_TRUE )
        {
          if ( ipMapRouterIfResolve(ipAddress, &cfgIntIfNum) != L7_SUCCESS )
          {
            cfgIntIfNum = L7_INVALID_INTF;
          }
        }
        else
        {
          if (nimIntIfFromConfigIDGet(&arp->rtrStaticArpCfgData[j].intfConfigId,
                                      &cfgIntIfNum) != L7_SUCCESS)
          {
            cfgIntIfNum = L7_INVALID_INTF;
          }
        }

        /* Its duplicate entry if interface numbers match */
        if (cfgIntIfNum == intIfNum)
        {
          /* There is a duplicate entry. Update the existing entry */
          rc = ipMapStaticArpAddApply(ipAddress, intIfNum,
                                      &arp->rtrStaticArpCfgData[j].macAddr);
          duplicateFound = L7_TRUE;
          break;
        }
      }
    }

    if (duplicateFound == L7_FALSE)
    {
      /* Ignore this return code. The static entry may not have been in the ARP
       * cache, for example, if the IP address is not on a local subnet.
       */
      ipMapStaticArpDeleteApply(ipAddress, intIfNum);
    }
  }

  /*Delete the static arp entry from config file*/
  memset(( void * )&arp->rtrStaticArpCfgData[index], 0,
         sizeof(L7_rtrStaticArpCfgData_t));
  arp->cfgHdr.dataChanged = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Count the number of static ARP entries currently configured.
*
* @param    void
*
* @returns  The number of static ARP entries currently configured.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapStaticArpCount(void)
{
  L7_uint32 count = 0;
  L7_uint32 i;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return 0;

  /* The array can contain holes, so have to traverse the whole array */
  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr != L7_NULL_IP_ADDR)
    {
      count++;
    }
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return count;
}

/*********************************************************************
* @purpose  Get a list of all static ARP entries currently configured.
*
* @param    staticEntries @b{(output)} An array of L7_IP_ARP_CACHE_STATIC_MAX
*                      ARP entries. On return, the array is filled with the
*                      static ARP entries. The first entry with an IP address
*                      of 0 indicates the end of the list.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if API is called with firstEntry NULL
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpGetAll(L7_arpEntry_t *staticEntries)
{
  L7_uint32 i;
  L7_uint32 outIndex = 0;   /* next index to write to in output array */
  nimConfigID_t intfConfigId;

  if (staticEntries == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* The configuration array can contain holes, so have to traverse
     the whole array */
  memset(staticEntries, 0, L7_RTR_MAX_STATIC_ARP_ENTRIES * sizeof(L7_arpEntry_t));
  memset(&intfConfigId, 0, sizeof(intfConfigId));
  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr != L7_NULL_IP_ADDR)
    {
      staticEntries[outIndex].ipAddr = arp->rtrStaticArpCfgData[i].ipAddr;
      memcpy(&staticEntries[outIndex].macAddr,
             &(arp->rtrStaticArpCfgData[i].macAddr),
             sizeof(L7_linkLayerAddr_t));

      if (NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId, &intfConfigId) == L7_TRUE)
      {
        staticEntries[outIndex].intIfNum = L7_INVALID_INTF;
      }
      else
      {
        if (nimIntIfFromConfigIDGet(&arp->rtrStaticArpCfgData[i].intfConfigId,
                                    &staticEntries[outIndex].intIfNum) != L7_SUCCESS)
        {
          staticEntries[outIndex].intIfNum = L7_INVALID_INTF;
        }
      }
      outIndex++;
    }
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies if the specified static route is valid
*
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
*
* @returns  L7_TRUE         Specified route is valid
* @returns  L7_FALSE        Specified route is invalid
*
* @notes    No need to take IP MAP lock.
*
* @end
*********************************************************************/
L7_BOOL ipMapStaticRouteAddrIsValid(L7_uint32 ipAddr, L7_uint32 subnetMask)
{
  return rtoValidDestAddr(ipAddr, subnetMask);
}

/*********************************************************************
* @purpose  Add a static route entry
*
* @param    *pStaticRoute @b{(input)}  Pointer to L7_rtrStaticRoute_t structure
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
L7_RC_t ipMapStaticRouteAdd(L7_uint32 unit,L7_rtrStaticRoute_t *pStaticRoute)
{
  L7_uint32 tempIntIfNum;
  L7_rtrStaticRouteCfgData_t *staticRoutePref;
  L7_rtrStaticRouteCfgData_t *staticRouteHop, *staticRoute;
  L7_RC_t rc = L7_FAILURE;
  nimConfigID_t  intfConfigId;
  L7_IP_ADDR_t networkPortIpAddr,servPortIpAddr;
  L7_IP_MASK_t networkPortMask,servPortMask;
  L7_IP_ADDR_t ipAddr = pStaticRoute->ipAddr & pStaticRoute->ipMask;
  L7_IP_MASK_t subnetMask = pStaticRoute->ipMask;
  L7_IP_ADDR_t nextHopRtr = pStaticRoute->nextHopRtr;
  L7_uint32 pref = pStaticRoute->preference;

  /* Check that the specified destination address is valid */
  if (rtoValidDestAddr(ipAddr, subnetMask) != L7_TRUE)
    return L7_FAILURE;

  memset(&intfConfigId, 0, sizeof(intfConfigId));

  if (pStaticRoute->flags & L7_RTF_REJECT)
  {
    /* Handle the "static reject route" creation */
    if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
      return L7_FAILURE;

    /* If static reject route is being added/modified */
    staticRoutePref = ipMapSrPrefFind(ipAddr, subnetMask, pref);
    rc = L7_SUCCESS;
    if(staticRoutePref != L7_NULLPTR)
    {
      if(!(staticRoutePref->flags & L7_RTF_REJECT))
      {
        /* we have a static route with same pref already in the RTO.
         * Disallow adding static reject route to the same destination */
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
      }
    }
    else
    {
      staticRoute = ipMapSrRejectFind(ipAddr, subnetMask);
      if((staticRoute != L7_NULLPTR) && (staticRoute->preference != pref))
      {
        rc = ipMapSrNextHopPrefSet(staticRoute, nextHopRtr, &intfConfigId, pref);
      }
      else if(staticRoute == L7_NULLPTR)
      {
        rc = ipMapSrAdd(ipAddr, subnetMask, nextHopRtr, &intfConfigId, pref,
                        pStaticRoute->flags);
      }
    }
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return rc;
  }

  /* Get nimConfigId_t structure for interface configured */
  if(pStaticRoute->intIfNum != L7_INVALID_INTF &&
     nimConfigIdGet(pStaticRoute->intIfNum, &intfConfigId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Do not allow a static route whose next hop IP address is 0 */
  if (nextHopRtr == 0)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* Reject this route if any existing local routing interface address
   * is used as the next hop
   */
  if (_ipMapIpAddressToIntf(nextHopRtr, &tempIntIfNum) == L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  staticRoutePref = ipMapSrPrefFind(ipAddr, subnetMask, pref);
  if((staticRoutePref != L7_NULLPTR) &&
     (staticRoutePref->flags & L7_RTF_REJECT))
  {
    /* we have a static reject route with same pref already in the RTO.
     * Disallow adding static route to the same destination */
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Check to see if a static route with the same destination and next hop
   * already exists.  */
  staticRouteHop = ipMapSrNextHopFind(ipAddr, subnetMask, nextHopRtr, &intfConfigId);

  /* add the next hop to route at new pref level. */
  if (staticRoutePref != L7_NULLPTR)
  {
    rc = ipMapSrNextHopAdd(staticRoutePref, nextHopRtr, &intfConfigId);

    /* delete existing route at old pref level (if any) */
    if ((rc == L7_SUCCESS) &&
        (staticRouteHop != L7_NULLPTR) &&
        (staticRouteHop != staticRoutePref))
      ipMapSrNextHopRemove(staticRouteHop, nextHopRtr, &intfConfigId);

    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return rc;
  }

  /* update the preference of the existing route. */
  if (staticRouteHop != L7_NULLPTR)
  {
    rc = ipMapSrNextHopPrefSet(staticRouteHop, nextHopRtr, &intfConfigId, pref);
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return rc;
  }
  /* Check if the specified ip address & subnet mask conflicts with the network
     or service port configuration */
  networkPortIpAddr = simGetSystemIPAddr();
  networkPortMask = simGetSystemIPNetMask();
  if ((networkPortIpAddr != 0) && (networkPortMask != 0))
  {
    if ((nextHopRtr & networkPortMask) == (networkPortIpAddr & networkPortMask))
    {
      /* subnet conflict with network */
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_REQUEST_DENIED;
    }
  }

  servPortIpAddr = simGetServPortIPAddr();
  servPortMask = simGetServPortIPNetMask();
  if ( servPortIpAddr!= 0 && servPortMask != 0)
  {
    if ((nextHopRtr & servPortMask) == (servPortIpAddr & servPortMask))
    {
      /* subnet conflict with service port */
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_REQUEST_DENIED;
    }
  }

  /* Add new static route with unique next hop and preference */
  rc = ipMapSrAdd(ipAddr, subnetMask, nextHopRtr, &intfConfigId, pref,
                  pStaticRoute->flags);
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Delete a static route entry
*
* @param    *pStaticRoute   Pointer to L7_rtrStaticRoute_t structure
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
L7_RC_t ipMapStaticRouteDelete(L7_rtrStaticRoute_t *pStaticRoute)
{
  L7_rtrStaticRouteCfgData_t *staticRoute;
  L7_IP_ADDR_t ipAddr = pStaticRoute->ipAddr;
  L7_IP_MASK_t subnetMask = pStaticRoute->ipMask;
  L7_IP_ADDR_t nextHopRtr = pStaticRoute->nextHopRtr;
  L7_RC_t rc = L7_FAILURE;
  nimConfigID_t intfConfigId;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  memset(&intfConfigId, 0, sizeof(intfConfigId));
  /* Get nimConfigId_t structure for interface configured */
  if ( pStaticRoute->intIfNum != L7_INVALID_INTF &&
       nimConfigIdGet(pStaticRoute->intIfNum, &intfConfigId) != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  if (pStaticRoute->flags & L7_RTF_REJECT)
  {
    rc = ipMapSrRejectDelete(ipAddr, subnetMask);
  }
  else
  {
    if (nextHopRtr != 0)
    {
      staticRoute = ipMapSrNextHopFind(ipAddr, subnetMask, nextHopRtr, &intfConfigId);
      if (staticRoute != L7_NULLPTR)
      {
        rc = ipMapSrNextHopRemove(staticRoute, nextHopRtr, &intfConfigId);
      }
    }
    else
    {
      rc = ipMapSrDelete(ipAddr, subnetMask);
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

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
L7_RC_t ipMapStaticRoutePrefRevert(L7_IP_ADDR_t ipAddr,
                                   L7_IP_MASK_t subnetMask, L7_uint32 oldPref)
{
  L7_RC_t rc;

  /* static route whose preference is being reverted to the default */
  L7_rtrStaticRouteCfgData_t *sr1;

  /* existing static route to same dest as sr1, but with default preference. */
  L7_rtrStaticRouteCfgData_t *sr2;

  L7_uint32 defaultPref;

   if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  defaultPref = ipMapCfg->ip.route_preferences[ROUTE_PREF_STATIC];
  if (oldPref == defaultPref)
  {
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  sr1 = ipMapSrPrefFind(ipAddr, subnetMask, oldPref);
  if (sr1 != L7_NULLPTR)
  {
    /* if there is no static route at the default preference, we can
     * simply change the preference. */
    sr2 = ipMapSrPrefFind(ipAddr, subnetMask, defaultPref);
    if (sr2 == L7_NULLPTR)
    {
      rc = ipMapSrPrefSet(sr1, defaultPref);
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return rc;
    }
    else
    {
      if (((sr1->flags & L7_RTF_REJECT) && !(sr2->flags & L7_RTF_REJECT)) ||
          (!(sr1->flags & L7_RTF_REJECT) && (sr2->flags & L7_RTF_REJECT)))
      {
        /* We don't allow ECMP routes with one of the nexthops as a reject route
         * Hence we disallow the reverting of pref of a normal route or reject route
         * if that makes reject route as part of ECMP route */
        ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
        return L7_FAILURE;
      }
      /* Need to merge next hops in sr1 to sr2 */
      rc = ipMapSrNextHopsMerge(sr1, sr2);
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return rc;
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get a list of all static routes.
*
* @param    staticEntries @b{(input/output)} An array of L7_RTR_MAX_STATIC_ROUTES
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
L7_RC_t ipMapStaticRouteGetAll(L7_routeEntry_t *staticEntries)
{
  L7_uint32 nh;
  L7_uint32 i;
  L7_uint32 outIndex = 0;   /* next index to write to in output array */
  L7_uint32 intIfNum;
  nimConfigID_t nullIntfConfigId;

  if (staticEntries == L7_NULLPTR)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* The configuration array is a sparce array, so we have to traverse the entire length */
  memset(staticEntries, 0, L7_RTR_MAX_STATIC_ROUTES * sizeof(L7_routeEntry_t));
  memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));
  for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {
    if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
    {
      staticEntries[outIndex].ipAddr = route->rtrStaticRouteCfgData[i].ipAddr;
      staticEntries[outIndex].subnetMask = route->rtrStaticRouteCfgData[i].ipMask;
      staticEntries[outIndex].protocol = RTO_STATIC;
      staticEntries[outIndex].metric = FD_RTR_STATIC_ROUTE_COST;
      staticEntries[outIndex].pref = route->rtrStaticRouteCfgData[i].preference;
      staticEntries[outIndex].flags = route->rtrStaticRouteCfgData[i].flags;

      for (nh = 0;
          (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
          (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0);
          nh++)
      {
        staticEntries[outIndex].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr =
        route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr;
        staticEntries[outIndex].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum = L7_INVALID_INTF;

        if ( NIM_CONFIG_ID_IS_EQUAL(&route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                                    &nullIntfConfigId) != L7_TRUE )
        {
          if (nimIntIfFromConfigIDGet(&route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                                      &intIfNum) == L7_SUCCESS)
          {
            staticEntries[outIndex].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum = intIfNum;
          }
        }
      }
      staticEntries[outIndex].ecmpRoutes.numOfRoutes = nh;

      outIndex++;
    }
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the comparision result between 2 static route entries
*
* @param    ip1        @b{(input)} route network
* @param    mask1      @b{(input)} network mask
* @param    pref1      @b{(input)} route preference
* @param    nh1        @b{(input)} next hop
* @param    nhIfNum1   @b{(input)} next hop interface number
* @param    ip2        @b{(input)} route network
* @param    mask2      @b{(input)} network mask
* @param    pref2      @b{(input)} route preference
* @param    nh2        @b{(input)} next hop
* @param    nhIfNum2   @b{(input)} next hop interface number
*
* @returns  -1     If first route < second route
* @returns  0      If first route == second route
* @returns  1      If first route > second route
*
* @notes
*
* @end
*********************************************************************/
L7_int32 ipStaticRouteCompare(L7_uint32 ip1, L7_uint32 mask1, L7_uint32 pref1, L7_uint32 nh1, L7_uint32 nhIfNum1,
                              L7_uint32 ip2, L7_uint32 mask2, L7_uint32 pref2, L7_uint32 nh2, L7_uint32 nhIfNum2)
{
  L7_int32 rc  = memcmp(&ip1, &ip2, sizeof(L7_uint32));

  if(rc < 0)
  {
    return -1;
  }
  if(rc > 0)
  {
    return 1;
  }
  rc = memcmp(&mask1, &mask2, sizeof(L7_uint32));
  if(rc < 0)
  {
    return -1;
  }
  if(rc > 0)
  {
    return 1;
  }
  if(pref1 < pref2)
  {
    return -1;
  }
  if(pref1 > pref2)
  {
    return 1;
  }
  rc = memcmp(&nh1, &nh2, sizeof(L7_uint32));
  if(rc < 0)
  {
    return -1;
  }
  if(rc > 0)
  {
    return 1;
  }
  if(nhIfNum1 < nhIfNum2)
  {
    return -1;
  }
  if(nhIfNum2 < nhIfNum1)
  {
    return 1;
  }
  return 0;
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
L7_RC_t ipMapStaticRouteGetNext(L7_uint32 *ipaddr, L7_uint32 *ipmask, L7_uint32 *preference,
                                L7_uint32 *nexthopaddr, L7_uint32 *nhIfNum)
{
  L7_RC_t rtRc;
  L7_int32 rc, nextIndex = -1, nhIndex = -1, nh, i;
  L7_uint32 cpuIntf = 0;
  L7_routeEntry_t * staticEntries;

  /* Check the given pointers */

  staticEntries = (L7_routeEntry_t *) osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                   FD_RTR_MAX_STATIC_ROUTES * sizeof(L7_routeEntry_t));

  rtRc = ipMapStaticRouteGetAll(staticEntries);

  if(rtRc != L7_SUCCESS)
  {
     osapiFree(L7_IP_MAP_COMPONENT_ID, staticEntries);
     return rtRc;
  }

  for (i = 0; (i < FD_RTR_MAX_STATIC_ROUTES); i++)
  {
    if(staticEntries[i].protocol == RTO_STATIC)
    {
      if(staticEntries[i].flags & L7_RTF_REJECT)
      {
        if(nimFirstValidIntfNumberByType(L7_CPU_INTF, &cpuIntf) != L7_SUCCESS)
        {
          /* If nimFirstValidIntfNumberByType() fails, we default to 1.
           * This shouldn't generally happen */
           cpuIntf = 1;
        }
        staticEntries[i].ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = cpuIntf;
      }
      for (nh = 0;
           (nh < platRtrRouteMaxEqualCostEntriesGet());
           nh++)
      {
        rc = ipStaticRouteCompare(staticEntries[i].ipAddr, staticEntries[i].subnetMask, staticEntries[i].pref,
                                  staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr,
                                  staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum,
                                  *ipaddr, *ipmask, *preference, *nexthopaddr, *nhIfNum);
        if(rc < 0)
        {
          continue;
        }
        else if(rc > 0)
        {
          if(-1 == nextIndex)
          {
            nextIndex = i;
            nhIndex = nh;
          }
          else
          {
            rc = ipStaticRouteCompare(staticEntries[i].ipAddr, staticEntries[i].subnetMask, staticEntries[i].pref,
                                      staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr,
                                      staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum,
                                      staticEntries[nextIndex].ipAddr, staticEntries[nextIndex].subnetMask,
                                      staticEntries[nextIndex].pref,
                                      staticEntries[nextIndex].ecmpRoutes.equalCostPath[nhIndex].arpEntry.ipAddr,
                                      staticEntries[nextIndex].ecmpRoutes.equalCostPath[nhIndex].arpEntry.intIfNum);
            if(rc < 0)
            {
              nextIndex = i;
              nhIndex = nh;
            }
            else if(rc > 0)
            {
              continue;
            }
          }
        }
        if(staticEntries[i].flags & L7_RTF_REJECT)
        {
          break;
        }
      }
    }
  }
  if((nextIndex != -1) && (nhIndex != -1))
  {
    *ipaddr = staticEntries[nextIndex].ipAddr;
    *ipmask = staticEntries[nextIndex].subnetMask;
    *preference = staticEntries[nextIndex].pref;
    *nexthopaddr = staticEntries[nextIndex].ecmpRoutes.equalCostPath[nhIndex].arpEntry.ipAddr;
    *nhIfNum = staticEntries[nextIndex].ecmpRoutes.equalCostPath[nhIndex].arpEntry.intIfNum;

    osapiFree(L7_IP_MAP_COMPONENT_ID, staticEntries);
    return L7_SUCCESS;
  }
  else
  {
    osapiFree(L7_IP_MAP_COMPONENT_ID, staticEntries);
    return L7_FAILURE;
  }
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
L7_RC_t ipMapStaticRouteIfindexGet(L7_uint32 ipaddr, L7_uint32 ipmask,
                                   L7_uint32 pref, L7_uint32 nexthop, L7_uint32 *index)
{
  L7_uint32 flag, i, nh;
  L7_RC_t rc;
  L7_routeEntry_t * staticEntries;

  flag = L7_FALSE;

  staticEntries = (L7_routeEntry_t *) osapiMalloc(L7_IP_MAP_COMPONENT_ID, FD_RTR_MAX_STATIC_ROUTES * sizeof(L7_routeEntry_t));

  rc = ipMapStaticRouteGetAll(staticEntries);

  if(rc != L7_SUCCESS)
  {
     osapiFree(L7_IP_MAP_COMPONENT_ID, staticEntries);
     return rc;
  }

  for (i = 0; (i < FD_RTR_MAX_STATIC_ROUTES); i++)
  {
     if(staticEntries[i].protocol == RTO_STATIC)
     {
        if((ipaddr == staticEntries[i].ipAddr) && (ipmask == staticEntries[i].subnetMask) &&
           (pref == staticEntries[i].pref))
        {
           if(staticEntries[i].flags & L7_RTF_REJECT)
           {
             /* This will be the case of a REJECT ROUTE since reject route
              * doesn't have any next hops.
              *
              * But for the SNMP query to proceed with the other valid routes following
              * this route, we shall give the interface number as CPU interface's
              * internal interface number, as the packets with DA best match as
              * a reject route are directed to CPU and get discarded there.
              */
             if(nimFirstValidIntfNumberByType(L7_CPU_INTF, index) != L7_SUCCESS)
             {
               /* If nimFirstValidIntfNumberByType() fails, we default to 1.
                * This shouldn't generally happen */
               *index = 1;
             }
             flag = L7_TRUE;
             break;
           }
           else
           {
             for (nh = 0; nh < staticEntries[i].ecmpRoutes.numOfRoutes; nh++)
             {
                if( (staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr == nexthop))
                {
                    *index = staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum;
                    flag = L7_TRUE;
                    break;
                }
             }
           }
         }
     }
  }

  if(flag == L7_TRUE)
  {
    if(*index == L7_INVALID_INTF)
    {
      /* Find nexthop interface if not specified explicitly */
      if(ipMapRouterIfResolve(nexthop, index) != L7_SUCCESS)
      {
         *index = L7_INVALID_INTF;
      }
    }
  }

  osapiFree(L7_IP_MAP_COMPONENT_ID, staticEntries);

  if(flag == L7_TRUE)
  {
     return L7_SUCCESS;
  }
  else
  {
     return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
 * @purpose  Set interface bandwidth
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
L7_RC_t ipMapIntfBandwidthSet(L7_uint32 intIfNum, L7_uint32 bandwidth)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_RC_t rc = L7_FAILURE;

  if ((bandwidth < L7_L3_MIN_BW || bandwidth > L7_L3_MAX_BW) && bandwidth != FD_IP_DEFAULT_BW)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (bandwidth == pCfg->bandwidth)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    pCfg->bandwidth = bandwidth;

    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

    rc=ipMapIntfBandwidthApply(intIfNum, bandwidth);

    if (rc == L7_SUCCESS)
      rc = ipMapv6IntfBandwidthApply(intIfNum, bandwidth);
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

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
L7_RC_t ipMapIntfBandwidthGet(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_uint32 ifDataRate;
  L7_RC_t rc = L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->bandwidth != 0)
    {
      *bandwidth = pCfg->bandwidth;
    }
    else
    {
      rc = nimGetIntfSpeedSimplexDataRate(intIfNum,&ifDataRate);

      if (rc != L7_SUCCESS)
      {
        /* At this time this function may be invoked for port-based routing interfaces
        ** when a link for the interface is down. In that scenario nimGetIntfSpeedDataRate
        ** returns an error. For now we will assume that the link is 100Full.
        */
        ifDataRate = L7_PORTCTRL_PORTSPEED_DEFAULT/2;
      }
      *bandwidth = ifDataRate*1000;
    }
    rc = L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
 * @purpose  Get interface bandwidth without modifying the SET value.
 *
 * @param    @b{(input)} L7_uint32 intIfNum
 * @param    @b{(output)} L7_uint32 bandwidth
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapIntfBWGet(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_RC_t rc = L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *bandwidth = pCfg->bandwidth;
    rc = L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
*
* @param    intIfNum      internal interface number
* @param    *rtrIntf    pointer to router interface number
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
L7_RC_t ipMapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32 *rtrIfNum)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  rc = _ipMapIntIfNumToRtrIntf(intIfNum, rtrIfNum);

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Determine if IPv4 routing is configured on an interface.
*
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This does not necessarily imply that the interface is up and
*           usable.
*
* @end
*********************************************************************/
L7_BOOL ipMapIpIntfExists(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_TRUE;
  }
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FALSE;
}

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
L7_RC_t ipMapRtrIntfToIntIfNum(L7_uint32 rtrIfNum, L7_uint32* intIfNum)
{
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;
  if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
  {
    *intIfNum = rtrIntfMap[rtrIfNum].intIfNum;

    if ((*intIfNum) > 0)
    {
      ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FAILURE;
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
L7_RC_t ipMapRouterIfResolve(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum)
{
  L7_RC_t rc;
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  rc = _ipMapRouterIfResolve(ipAddr, intIfNum);
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Determine whether a given IPv4 address is configured on a
*           routing interface.
*
* @param    ipAddr    ({input})  IP address to look for
* @param    intIfNum  ({output}) internal interface number of interface with
*                                the specified address
*
* @returns  L7_SUCCESS  if an interface is found with given address
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpAddressToIntf(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum)
{
  L7_RC_t rc;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  rc = _ipMapIpAddressToIntf(ipAddr, intIfNum);

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Register a routine to be called for routing events
*
* @param    routerProtocol_ID     routine router protocol id
*                                 (See L7_IPMAP_REGISTRANTS_t)
* @param    *name                 pointer to the name of the registered routine,
*                                 up to IPMAP_NOTIFY_FUNC_NAME_SIZE characters will be stored.
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
L7_RC_t ipMapRegisterRoutingEventChange(L7_uint32 routerProtocol_ID, L7_char8 *name,
                                        L7_RC_t (*notify)(L7_uint32 intIfNum,
                                                          L7_uint32 event,
                                                          void *pData,
                                                          ASYNC_EVENT_NOTIFY_INFO_t *response))
{
  if (routerProtocol_ID >= L7_LAST_ROUTER_PROTOCOL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Attempt to register invalid protocol %u for IPv4 router events.",
            routerProtocol_ID);
    return L7_FAILURE;
  }

  (void) osapiSemaTake(pIpMapInfo->eventHandler.registered_rwlock, L7_WAIT_FOREVER);
  if (pIpIntfStateNotifyList[routerProtocol_ID].notify_routing_event != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Duplicate registration for IPv4 routing events by protocol %u.",
            routerProtocol_ID);
    osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);
    return L7_FAILURE;
  }

  osapiStrncpySafe((L7_char8 *)&(pIpIntfStateNotifyList[routerProtocol_ID].name), name,
          IPMAP_NOTIFY_FUNC_NAME_SIZE);
  pIpIntfStateNotifyList[routerProtocol_ID].notify_routing_event = notify;
  pIpIntfStateNotifyList[routerProtocol_ID].rtrProtoId = routerProtocol_ID;

  /* The COMPONENT_MASK_t is purposely overloaded with L7_IPMAP_REGISTRANTS_t type to
  allow for general usage of the COMPONENT_MASK_t type.  It is not expected that
  the number of IP mapping layer registrants will exceed the number of component ids. */
  COMPONENT_SETMASKBIT(pIpMapInfo->registeredComponents,routerProtocol_ID);

  osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID     routine router protocol id
*                                 (See L7_IPMAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t ipMapDeregisterRoutingEventChange(L7_uint32 routerProtocol_ID)
{
  if (routerProtocol_ID >= L7_LAST_ROUTER_PROTOCOL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Attempt to deregister invalid protocol %u for IPv4 router events.",
            routerProtocol_ID);
    return L7_FAILURE;
  }

  (void) osapiSemaTake(pIpMapInfo->eventHandler.registered_rwlock, L7_WAIT_FOREVER);
  if (pIpIntfStateNotifyList[routerProtocol_ID].notify_routing_event == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Attempt to deregister protocol %u for IPv4 router events, but protocol is not registered.",
            routerProtocol_ID);
    osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);
    return L7_FAILURE;
  }

  pIpIntfStateNotifyList[routerProtocol_ID].notify_routing_event = L7_NULL;
  pIpIntfStateNotifyList[routerProtocol_ID].rtrProtoId = L7_NULL;

  COMPONENT_CLRMASKBIT(pIpMapInfo->registeredComponents, routerProtocol_ID);

  osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes    Don't take semaphore. Generally called w/in IP MAP code.
*
* @end
*********************************************************************/
L7_uint32 ipMapTraceModeGet(void)
{
  L7_uint32 traceMode;

  if (ipMapCfg == L7_NULL)
  {
    return(L7_DISABLE);
  }

  traceMode = ipMapCfg->rtr.rtrTraceMode;
  return traceMode;
}

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Unreachables
*
* @param    void
*
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrICMPUnreachablesModeGet(L7_uint32 intIfNum, L7_uint32 * mode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if ((mode != L7_NULLPTR) &&
      (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE))
  {
    *mode = pCfg->icmpUnreachablesMode;
    rc = L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Unreachables
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
L7_RC_t ipMapRtrICMPUnreachablesModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if ( mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (mode == pCfg->icmpUnreachablesMode)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    pCfg->icmpUnreachablesMode = mode;

    /* Setting in the statck */
    ipMapIntfIpUnreachablesModeApply (intIfNum, mode);

    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t ipMapIfICMPRedirectsModeGet(L7_uint32 intIfNum, L7_uint32 * mode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if ((mode != L7_NULLPTR) &&
      (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE))
  {
    *mode = pCfg->icmpRedirectsMode;
    rc = L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
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
L7_RC_t ipMapIfICMPRedirectsModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if ( mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* See if this is a change */
    if (mode == pCfg->icmpRedirectsMode)
    {
      ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }

    pCfg->icmpRedirectsMode = mode;
    /* Informing to Statck */
    ipMapIntfIpRedirectsModeApply (intIfNum, mode);

    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  setting ICMP Rate Limiting parameters like Burst size and Burst interval.
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
L7_RC_t ipMapRtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval)
{

  if ( burstSize < L7_L3_ICMP_RATE_LIMIT_MIN_BURST_SIZE  ||
       burstSize > L7_L3_ICMP_RATE_LIMIT_MAX_BURST_SIZE)
     return L7_FAILURE;

  if (interval  < L7_L3_ICMP_RATE_LIMIT_MIN_INTERVAL  ||
      interval > L7_L3_ICMP_RATE_LIMIT_MAX_INTERVAL )
    return L7_FAILURE;


  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapCfg->rtr.rtrICMPRatelimitBurstSize != burstSize ||
      ipMapCfg->rtr.rtrICMPRatelimitInterval != interval)
  {
    ipMapCfg->rtr.rtrICMPRatelimitBurstSize = burstSize;
    ipMapCfg->rtr.rtrICMPRatelimitInterval =  interval;

    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

    /* Applying the change */
    (void) ipMapRtrICMPRatelimitApply (burstSize, interval);

    ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

    return L7_SUCCESS;
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get ICMP Rate Limiting parameters like Burst size and interval.
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
L7_RC_t  ipMapRtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval)
{
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *burstSize = ipMapCfg->rtr.rtrICMPRatelimitBurstSize;
  *interval = ipMapCfg->rtr.rtrICMPRatelimitInterval;

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Receives IP packets from sysnet
*
* @param    netBufHandle    Handle to buffer
* @param    pduInfo         pointer to pdu info structure
* @param                    which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRecvIP(L7_netBufHandle netBufHandle, sysnet_pdu_info_t *pduInfo)
{
  ipForwardPktsIn0++;

  /* Call interceptors who are interested in all IP packets that have come through
  ** sysnet.  This hook will notify registrants using the incoming internal interface
  ** number, before the potential conversion to VLAN routing internal interface number
  ** below.  If L7_TRUE is returned, the frame was either discarded or consumed,
  ** which means that the network buffer has been freed by the intercept call, or
  ** will be freed by the consumer.
  */
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_RECV_IN, netBufHandle,
                           pduInfo, L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  return ipMapRecvQueue(netBufHandle, pduInfo->vlanId, pduInfo->intIfNum);
}

/*********************************************************************
* @purpose  Queue an incoming IP packet to the IP forwarding thread.
*
* @param    netBufHandle    Handle to buffer
* @param    vlanId          ingress VLAN ID
* @param    intIfNum        ingress interface
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRecvQueue(L7_netBufHandle netBufHandle,
                       L7_uint32 vlanId, L7_uint32 intIfNum)
{
  ipMapMsg_t        ipMapMsg;
  L7_RC_t           rc;
  L7_char8          *data;
  L7_uint32         offset;
  L7_ipHeader_t     *ip_header;
  L7_uint32          priority = L7_L3_FWD_QUEUE;
  L7_udp_header_t   *udp_header;
  L7_uint32         vlanIntf;    /* intIfNum of ingress vlan routing interface */
  L7_rtrCfgCkt_t *pCfg;
  L7_uchar8 protocol;          /* protocol ID in IP header */
  L7_ushort16 sourcePort = 0;  /* if TCP or UDP, source port number */
  L7_ushort16 destPort = 0;    /* if TCP or UDP, destination port number */

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (!ipMapForwardQueue[L7_L3_FWD_QUEUE] || !ipMapForwardQueue[L7_L3_FWDPRI_QUEUE] ||
      !ipMapForwardQueue[L7_L3_FWDHIGHPRI_QUEUE])
  {
    /* In case of failure the buffer is freed by DTL. */
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  memset(&ipMapMsg,0x00,sizeof(ipMapMsg_t));
  ipMapMsg.msgId = IPMAP_PDU;
  ipMapMsg.type.pdu.bufHandle = netBufHandle;
  ipMapMsg.type.pdu.intf = intIfNum;
  ipMapMsg.type.pdu.dtlIntf = intIfNum;  /* save original intIfNum */
  ipMapMsg.type.pdu.vlanId = vlanId;

  if (ipMapVlanRoutingIntfGet(vlanId, &vlanIntf) == L7_SUCCESS)
  {
    /* If the VLAN routing interface is up, consider the packet to
     * have arrived on the VLAN interface rather than the physical
     * interface. */
    if (ipMapMapIntfIsConfigurable(vlanIntf, &pCfg) &&
        (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE))
    {
      ipMapMsg.type.pdu.intf = vlanIntf;
    }
  }

  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  offset = sysNetDataOffsetGet(data);
  ip_header = (L7_ipHeader_t *)(data + offset);
  protocol = ip_header->iph_prot;
  if ((protocol == IP_PROT_TCP) || (protocol == IP_PROT_UDP))
  {
    udp_header = (L7_udp_header_t *)(data + offset + L7_IP_HDR_LEN);
    sourcePort = osapiNtohs(udp_header->sourcePort);
    destPort = osapiNtohs(udp_header->destPort);
  }

  ipRxStats[protocol]++;

  priority = ipMapQueuePriority(protocol, sourcePort, destPort);

  rc = osapiMessageSend(ipMapForwardQueue[priority], &ipMapMsg, sizeof(ipMapMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc == L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_SET_LOC(netBufHandle, MBUF_LOC_IP4_FWD_Q);
    osapiSemaGive(ipMapMsgQSema);
  }
  else
  {
     ipForwardQueueFull++;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "Failed to queue IP packet to IP MAP forwarding thread queue %u.",
            priority);
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}


/*********************************************************************
* @purpose  Receives ARP packets from sysnet
*
* @param    netBufHandle    Handle to buffer
* @param    pduInfo         pointer to pdu info structure
*                           which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpRecvIP(L7_netBufHandle netBufHandle, sysnet_pdu_info_t *pduInfo)
{
  ipForwardArpPktsIn++;
  SYSAPI_NET_MBUF_SET_LOC(netBufHandle, MBUF_LOC_ARP_RX);

  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_RECV_ARP_IN, netBufHandle, pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* Queue the packet to the IP MAP processing thread. */
  return ipMapArpRecvQueue(netBufHandle, pduInfo->vlanId, pduInfo->intIfNum);
}

/*********************************************************************
* @purpose  Queue an incoming ARP packet to the IP MAP forwarding thread.
*
* @param    netBufHandle    Handle to buffer
* @param    vlanId          Ingress VLAN ID
* @param    intIfNum        Ingress interface
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR    - Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpRecvQueue(L7_netBufHandle netBufHandle,
                          L7_uint32 vlanId, L7_uint32 intIfNum)
{
  ipMapMsg_t ipMapMsg;
  L7_RC_t rc;
  L7_uint32 vlanIntf;    /* internal interface number for vlanId */
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (!ipMapForwardQueue[L7_L3_ARP_QUEUE])
  {
    /* In case of failure the buffer is freed by DTL. */
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  memset(&ipMapMsg,0x00,sizeof(ipMapMsg_t));
  ipMapMsg.msgId = IPMAP_PDU;
  ipMapMsg.type.pdu.bufHandle = netBufHandle;
  ipMapMsg.type.pdu.intf = intIfNum;
  ipMapMsg.type.pdu.dtlIntf = intIfNum;  /* save original intIfNum */
  ipMapMsg.type.pdu.vlanId = vlanId;

  if (ipMapVlanRoutingIntfGet(vlanId, &vlanIntf) == L7_SUCCESS)
  {
    /* If the VLAN routing interface is up, consider the packet to
     * have arrived on the VLAN interface rather than the physical
     * interface. */
    if (ipMapMapIntfIsConfigurable(vlanIntf, &pCfg) &&
        (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE))
    {
      ipMapMsg.type.pdu.intf = vlanIntf;
    }
  }

  rc = osapiMessageSend(ipMapForwardQueue[L7_L3_ARP_QUEUE], &ipMapMsg, sizeof(ipMapMsg_t),
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
      osapiSemaGive(ipMapMsgQSema);
  }
  else
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Failed to queue ARP packet to IP MAP forwarding thread.");
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return rc;
}




/***************VLAN ROUTER INTERFACE APIS *****************************/
/***************VLAN ROUTER INTERFACE APIS *****************************/
/***************VLAN ROUTER INTERFACE APIS *****************************/
/***************VLAN ROUTER INTERFACE APIS *****************************/
/***************VLAN ROUTER INTERFACE APIS *****************************/
/***************VLAN ROUTER INTERFACE APIS *****************************/


/*********************************************************************
* @purpose  Get the vlan corresponding to this interface number
*
* @param    intIfNum        Internal interface number of this vlan
* @param    vlanid              pointer to vlan id output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    No need to take the IP MAP read/write lock.
*
* @end
*********************************************************************/
L7_RC_t ipMapVlanRtrIntIfNumToVlanId(L7_uint32 intIfNum, L7_uint32 *vlanId)
{
  nimMacroPort_t    macroPortIntf;
  L7_uint32 sysIntfType;

  *vlanId = 0;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    if (sysIntfType == L7_LOGICAL_VLAN_INTF)
    {
      if ( nimGetMacroPortAssignment(intIfNum, &macroPortIntf) == L7_SUCCESS)
      {
        *vlanId    = (L7_uint32)macroPortIntf.macroInfo;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the internal interface number of the VLAN routing
*           interface with a given VLAN ID.
*
* @param    vlanid    @b{(input)}  VLAN ID
* @param    intIfNum  @b{(output)} internal interface number of VLAN routing interface
*
* @returns  L7_SUCCESS if VLAN is a VLAN routing interface
*           L7_FAILURE if VLAN is not a VLAN routing interface
*
* @notes    No need to take the IP MAP read/write lock.
*
* @end
*********************************************************************/
L7_RC_t ipMapVlanRoutingIntfGet(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
  return dot1qVlanIntfVlanIdToIntIfNum(vlanId, intIfNum);
}

/*********************************************************************
* @purpose  Get the first ip address of any routing interface
*
* @param    ipAddress       IP Address of the interface
*
* @returns  L7_SUCCESS      An address was found
* @returns  L7_FAILURE      No addresses were configured
*
* @notes    This gets called (indirectly) from logmsg(). That means that
*           whenever IP MAP logs a message, the code may circle back around
*           to this API. If IP MAP already holds a write lock, we'll deadlock.
*           For now, don't take lock here. We're no worse off than we've
*           been in past releases, but there is still a chance address data can
*           change out from under a component writing a log msg.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfFirstAddress(L7_uint32 *ipAddress)
{
  L7_uint32 i, intIfNum, ipAddr;
  L7_rtrCfgCkt_t *pCfg;

  /*  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
      return L7_FAILURE; */

  /* Iterate through all existing interfaces */
  for (i = 1; i <=  L7_RTR_MAX_RTR_INTERFACES; i++)
  {
    if (ipMapCheckIfNumber(i) == L7_SUCCESS)
    {
      intIfNum = rtrIntfMap[i].intIfNum;
      if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
      {
        ipAddr = pCfg->addrs[0].ipAddr;
        if (ipAddr)
        {
          *ipAddress = ipAddr;
          /* ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__); */
          return L7_SUCCESS;
        }
      }
    }
  }

  /* ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__); */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the list of IP addresses associated with an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       addresses associated with the specified interface.
*
* @returns  L7_SUCCESS  The interface is configured as a router interface
*                       and the address information could be successfully
*                       retrieved
* @returns  L7_FAILURE  The interface is not a valid IP interface
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS. Caller must provide a buffer of
*           this size to store all IP addresses associated with this interface.
*
*           Equivalent to ipMapRtrIntfCfgIpAddrListGet().
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfIpAddrListGet(L7_uint32 intIfNum, L7_rtrIntfIpAddr_t *ipAddrList)
{
  return ipMapRtrIntfCfgIpAddrListGet(intIfNum, ipAddrList);
}

/*********************************************************************
* @purpose  Returns the list of IP addresses associated with an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       addresses associated with the specified interface.
*
* @returns  L7_SUCCESS  The interface is configured as a router interface
*                       and the address information could be successfully
*                       retrieved
* @returns  L7_FAILURE  The interface is not a valid IP interface
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS. Caller must provide a buffer of
*           this size to store all IP addresses associated with this interface.
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddrListGet(L7_uint32 intIfNum, L7_rtrIntfIpAddr_t *ipAddrList)
{
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memcpy((L7_char8 *)ipAddrList, (L7_char8 *)(pCfg->addrs),
           L7_L3_NUM_IP_ADDRS*(sizeof(L7_rtrIntfIpAddr_t)));
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_SUCCESS;
  }

  memset((L7_char8 *)ipAddrList, 0,
         L7_L3_NUM_IP_ADDRS*(sizeof(L7_rtrIntfIpAddr_t)));

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get the index associated with the secondary IP address
*           for the specified routing interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number associated
*                       with this router port.
* @param    ipAddr      @b{(input)} Secondary IP Address. Must be non-zero.
* @param    ipMask      @b{(input)} Subnet mask associated with the IP Address
* @param    index       @b{(output)} Router interface index.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSecondaryAddrIndexGet(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                           L7_IP_MASK_t ipMask, L7_uint32 *index)
{
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;

  if ((ipAddr == 0) || (ipMask == 0))
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if ((ipAddr == pCfg->addrs[j].ipAddr) && (ipMask == pCfg->addrs[j].ipMask))
    {
      *index = j;
      ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Checks if the specified IP address matches the net directed
*           broadcast address of one of the IP addresses configured
*           on the given interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(input)} IP address to check
* @param    pMacAddr    @b{(output)} Pointer to a buffer to hold the Net
*                       Directed Broadcast MAC Addresses if a match is
*                       found
*
* @returns  L7_SUCCESS  Match found
* @returns  L7_FAILURE  Could not find a match
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  ipMapNetDirBcastMatchCheck(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                    L7_uchar8 *pMacAddr)
{
  L7_IP_ADDR_t    netDirBcastAddr = L7_NULL_IP_ADDR;
  L7_uint32     j;
  L7_rtrCfgCkt_t *pCfg;

  if (pMacAddr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }

    netDirBcastAddr = (pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) |
                      ~(pCfg->addrs[j].ipMask);

    if (ipAddr == netDirBcastAddr)
    {
      memcpy(pMacAddr, &(L7_ENET_BCAST_MAC_ADDR), (size_t)L7_MAC_ADDR_LEN);
      ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

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
L7_BOOL ipMapIpAddrIsLocal(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr)
{
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;
  L7_BOOL isLocal = L7_FALSE;

  if (ipAddr == 0)
    return L7_FALSE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
        continue;

      if ((ipAddr & pCfg->addrs[j].ipMask) ==
          (pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask))
      {
        isLocal = L7_TRUE;
        break;
      }
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return isLocal;
}

/*********************************************************************
* @purpose  Determine if specified IP Address exactly matches a configued
*           IP Address for the given interface
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
L7_BOOL ipMapIpAddrMatchesConfigured(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr)
{
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;
  L7_BOOL isMatch = L7_FALSE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FALSE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
        continue;

      if (ipAddr == pCfg->addrs[j].ipAddr)
      {
        isMatch = L7_TRUE;
        break;
      }
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return isMatch;
}

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
* @notes    does not take the IP MAP lock since it doesn't touch
*           any IP MAP data.
*
* @end
*********************************************************************/
L7_BOOL ipMapIntfSupports(L7_uint32 intIfNum, ipIntfCapability_t capability)
{
  L7_INTF_TYPES_t itype;

  if (nimGetIntfType(intIfNum, &itype) != L7_SUCCESS)
  {
    IPMAP_ERROR("invalid capability specified (%d)\n", capability);
    return L7_FALSE;
  }

  switch (capability)
  {
    case IPMAP_INTFCAP_MULTICAST:
      return(itype == L7_PHYSICAL_INTF || itype == L7_LOGICAL_VLAN_INTF ||
             itype == L7_TUNNEL_INTF);

    case IPMAP_INTFCAP_ARP:
      return(itype == L7_PHYSICAL_INTF || itype == L7_LOGICAL_VLAN_INTF);

    case IPMAP_INTFCAP_ROUTING_CONFIGURABLE:
      return(itype == L7_PHYSICAL_INTF || itype == L7_LOGICAL_VLAN_INTF);

    default:
      IPMAP_ERROR("invalid capability specified (%d)\n", capability);
      break;
  }

  return L7_FALSE;
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
L7_RC_t ipMapRtrIntfOperModeGet(L7_uint32 intIfNum, L7_uint32 *ifState)
{
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;
  if (ipMapIntfIsUp(intIfNum))
    *ifState = L7_ENABLE;
  else
    *ifState = L7_DISABLE;
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a neighbor's IPv4 address, get the local IPv4 address
*           on a given interface in the same subnet.
*
* @param    nbrAddr     @b{(input)}  neighbor's IP address.
* @param    intIfNum    @b{(input)}  interface to neighbor
* @param    localAddr   @b{(output)} local address in same subnet as neighbor
*
* @returns  L7_SUCCESS if local address found
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapLocalAddrToNeighbor(L7_uint32 nbrAddr, L7_uint32 intIfNum,
                                 L7_uint32 *localAddr)
{
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;

  if (nbrAddr == 0)
    return L7_FAILURE;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return L7_FAILURE;
  }

  /* walk through router interfaces, looking for an IP address match */
  for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
      continue;

    if ((pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) ==
        (nbrAddr & pCfg->addrs[j].ipMask))
    {
      *localAddr = pCfg->addrs[j].ipAddr;
      ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
      return L7_SUCCESS;
    }
  }

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Put an IP packet on the software forwarding path.
*
* @param    bufHandle   @b{(input)}  mbuf containing packet, starting with L2 header
*                                    Forwarding task takes ownership of mbuf if packet
*                                    is successfully queued.
* @param    rxPort      @b{(input)}  ingress physical port
* @param    rxIntf      @b{(output)} ingress routing interface
* @param    vlanId      @b{(output)} ingress VLAN ID
*
* @returns  L7_SUCCESS if packet is enqueued to the IP MAP forwarding task.
* @returns  L7_FAILURE
*
* @notes    All packets go on the data packet queue
*
* @end
*********************************************************************/
L7_RC_t ipMapPacketForward(L7_netBufHandle bufHandle, L7_uint32 rxPort,
                           L7_uint32 rxIntf, L7_uint32 vlanId)
{
  ipMapMsg_t ipMapMsg;
  L7_RC_t rc;

  if (!ipMapForwardQueue[L7_L3_FWD_QUEUE])
  {
    /* In case of failure the buffer is freed by caller. */
    return L7_FAILURE;
  }

  memset(&ipMapMsg, 0x00, sizeof(ipMapMsg_t));
  ipMapMsg.msgId = IPMAP_PDU;
  ipMapMsg.type.pdu.bufHandle = bufHandle;
  ipMapMsg.type.pdu.intf = rxIntf;
  ipMapMsg.type.pdu.dtlIntf = rxPort;
  ipMapMsg.type.pdu.vlanId = vlanId;

  rc = osapiMessageSend(ipMapForwardQueue[L7_L3_FWD_QUEUE], &ipMapMsg, sizeof(ipMapMsg_t),
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc == L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_IP4_FWD_Q);
    osapiSemaGive(ipMapMsgQSema);
  }
  else
  {
    ipForwardQueueFull++;
  }

  return rc;
}

/*********************************************************************
* @purpose  Indicate that a protocol intends to add routes to RTO following
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
L7_RC_t ipMapNsfRouteSource(L7_NSF_PROTOCOL_t protocol)
{
  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapTraceFlags & IPMAP_TRACE_NSF)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    sprintf(traceBuf, "%s to provide NSF routes.",
            ipMapNsfProtocolName(protocol));
    ipMapTraceWrite(traceBuf);
  }

  if (pIpMapInfo->nsfRoutesPending == 0)
  {
    /* first protocol to register. Start timer in case someone
     * never gives complete indication. */
    osapiTimerAdd((void*)ipMapStaleRouteTimerExp, 0, 0, IPMAP_STALE_ROUTE_TIME,
                         &pIpMapInfo->staleRouteTimer);
  }

  pIpMapInfo->nsfRoutesPending |= protocol;

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  A protocol that controls NSF routes calls this function to
*           indicate it has updated RTO with all its initial routes after
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
L7_RC_t ipMapInitialRoutesDone(L7_NSF_PROTOCOL_t protocol)
{
#ifdef L7_NSF_PACKAGE
  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  if (pIpMapInfo->warmRestart)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_NSF)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "%s has completed NSF route update.",
              ipMapNsfProtocolName(protocol));
      ipMapTraceWrite(traceBuf);
    }

    pIpMapInfo->nsfRoutesPending &= ~protocol;

    if (pIpMapInfo->nsfRoutesPending == 0)
    {
      osapiTimerFree(pIpMapInfo->staleRouteTimer);
      ipMapStaleRoutesFlush();
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

#endif
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the internal interface number associated with the
*           port-based routing interface corresponding to this
*          internal VLAN
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of port-based routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a port-based routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a VLAN routing interface.
*
* @end
*********************************************************************/
L7_RC_t ipMapInternalVlanIdToRtrIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
    return ipstkInternalVlanIdToRtrIntIfNum(vlanId,intIfNum);
}


/*********************************************************************
* @purpose  Get the port-based routing interface for the given assigned VLANID
*
* @param    vlanId VLAN ID
*
* @returns  non-zero if Routing interface is found.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipMapInternalVlanRoutingInterfaceGet(L7_uint32 vlanId)
{
  return ipstkInternalVlanRoutingInterfaceGet(vlanId);
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
L7_RC_t ipMapInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
    return ipstkInternalVlanFindFirst(vlanId,intIfNum);
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
L7_RC_t ipMapInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
    return ipstkInternalVlanFindNext(prevVid, vlanId, intIfNum);
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
* @returns  L7_SUCCESS if address is found
* @returns  L7_FAILURE if for any reason address not found
*
* @notes    To get the default gateway route in the RTO, set the 
*           argument 'activeFlag' to TRUE.
*           To get the configured default gateway , set the 
*           argument 'activeFlag' to FALSE.
*
* @end
*********************************************************************/
L7_RC_t
ipMapGlobalDefaultGatewayAddressGet (L7_IP_ADDR_t *globalDefGwAddr,
                                     L7_BOOL isActive)
{
  L7_uint32 i;
  L7_IP_ADDR_t gwAddr = 0;

  if (route != L7_NULLPTR)
  {
    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
      if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
      {
        if ((route->rtrStaticRouteCfgData[i].ipAddr == 0) &&
            (route->rtrStaticRouteCfgData[i].ipMask == 0) &&
            (route->rtrStaticRouteCfgData[i].preference ==
                 ipMapRouterPreferenceGet(ROUTE_PREF_GLOBAL_DEFAULT_GATEWAY)) &&
            ((route->rtrStaticRouteCfgData[i].flags &
                              L7_RT_GLOBAL_DEFAULT_GATEWAY) != 0))
        {
            gwAddr = route->rtrStaticRouteCfgData[i].nextHops[0].nextHopRtr;
            if (isActive != L7_TRUE)
            {
              *globalDefGwAddr = gwAddr;
              return L7_SUCCESS;
            }
            break;
        }
      }
    }
  }

  if ((gwAddr != 0) && (isActive == L7_TRUE))
  {
    L7_IP_ADDR_t globalDefGateway = 0;
    if (rtoIsGlobalDefGatewayRoutePresent (&globalDefGateway) == L7_TRUE)
    {
      if (globalDefGateway == gwAddr)
      {
        *globalDefGwAddr = globalDefGateway;
        return L7_SUCCESS;
      }
    }
  }

  *globalDefGwAddr = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get warm start dhcp addr
*
* @param   intIfNum  @b{(input)}  Internal Interface Number
* @param   ipAddr    @b{(output)} Checkpointed DHCP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
ipMapIntfCheckpointDhcpAddrGet (L7_uint32 intIfNum,
                                L7_uint32 *ipAddr)
{
  if (ipAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  ipMapCheckpointDhcpGet (intIfNum, ipAddr);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To create Default Routing Vlan
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapDefaultRoutingVlanCreate (void)
{
  L7_uint32 unit = 1;
  L7_uint32 defVlanId = FD_IP_DEFAULT_ROUTING_VLAN_ID;
  L7_uint32 intIfNum = 0;

  if ((simStartupConfigIsExists() != L7_TRUE) && (pIpMapInfo->warmRestart != L7_TRUE))
  {
    if (usmDbIpVlanRoutingIntfCreate (unit, defVlanId, intIfNum) == L7_SUCCESS)
    {
      IPMAP_TRACE("[%s-%d]:  Creation of a Routing interface on VLAN-1 (by default) successful.\n",
                  __FUNCTION__, __LINE__);
    }
    else
    {
      IPMAP_TRACE("[%s-%d]:  Creation of a Routing interface on VLAN-1 (by default) failed.\n",
                  __FUNCTION__, __LINE__);
    }
  }
  return;
}

