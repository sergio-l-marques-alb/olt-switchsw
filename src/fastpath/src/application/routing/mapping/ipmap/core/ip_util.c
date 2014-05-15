/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ip_util.c
*
* @purpose   Ip Utility functions
*
* @component Ip Mapping Layer
*
* @comments  none
*
* @create    03/13/2001
*
* @author    asuthan
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/




#include "l7_ipinclude.h"
#include "rtip_icmp.h"
#include "rticmp_var.h"
#include "ip_util.h"
#include "l7freebsd_api.h"
#include "l3_defaultconfig.h"
#include "rtiprecv.h"
#include "ipstk_api.h"
#include "osapi_support.h"
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "ipmap_arp.h"
#include "dhcp_client_api.h"
#ifdef L7_NSF_PACKAGE
#include "ipmap_dhcp_ckpt.h"
#endif

extern L7_rtrIntfMap_t          rtrIntfMap[L7_RTR_MAX_RTR_INTERFACES+1];
extern L7_uint32                intIfNumToRtrIntf[L7_MAX_INTERFACE_COUNT+1];
extern ipMapInfo_t              *pIpMapInfo;
extern pIpRoutingEventNotifyList_t   pIpIntfStateNotifyList;
extern void *ipMapProcess_Queue;
extern ipMapIntfInfo_t     *ipMapIntfInfo;
extern L7_uint32 ipMapTraceFlags;
extern L7_uint32 ipMapTraceIntf;
extern L7_uchar8 *ipMapRouterEventNames[L7_LAST_RTR_STATE_CHANGE];
extern ipMapCnfgrState_t    ipMapCnfgrState;

extern void arpMapCheckpointCallback(ARP_EVENT_TYPE_t arpEvent,
                                     L7_uint32 ipAddr,
                                     L7_uint32 intIfNum,
                                     L7_uchar8 *macAddr);

/* For easier debugging, name the router event clients.
 * NOTE: Make sure that this reflects exactly the same as
 * L7_IPMAP_REGISTRANTS_t.
 */
L7_uchar8 *ipMapEventClientNames[L7_LAST_ROUTER_PROTOCOL] =
{
    "Reserved", "RIP", "VRRP", "OSPFv2",
    "MGMD", "PIMDM", "DVMRP",
    "STATIC", "LOCAL", "BGP",
    "DEFAULT", "RADV", "ERROR",
    "MCAST", "PIMSM", "OSPFv3", "RLIM", "DHCP6S", "WIRELESS",
    "RADIUS", "IGMPSnooping" "DHCPv4Client"
};

L7_uchar8 *ipMapCnfgrStateNames[IPMAP_PHASE_UNCONFIG_2 + 1] =
{
  "P0", "P1", "P2", "WMU", "P3", "EXE", "U1", "U2"
};

/* Read write lock. Taken in IP MAP API functions and other entry points
 * to synchronize access to IP MAP configuration and status data. The
 * lock does not protect ARP configuration data and is not taken for calls to
 * the ARP MAP code. The RTIP forwarding code assumes the IP MAP lock is
 * not taken and accesses IP MAP data through IP MAP APIs. The ipm code
 * in os/<linux | vxworks>/routing assumes the IP MAP lock is taken. */
osapiRWLock_t  ipRwLock;

/*---------------------------------------*/
/* Global Configuration Data Structures  */
/*---------------------------------------*/
extern L7_ipMapCfg_t            *ipMapCfg;
extern L7_rtrStaticRouteCfg_t   *route;
extern L7_rtrStaticArpCfg_t     *arp;

ipMapLockInfo_t *ipMapLockInfo;

/* Number of packets received by IP MAP. Indexed by the 8-bit protocol field
 * in the IP header. */
L7_uint32 ipRxStats[256];

static L7_RC_t ipMapIntfDisable(L7_uint32 intIfNum);
static L7_RC_t ipMapNumberedIntfStateChange(L7_uint32 intIfNum);

void IpRxStatsReset(void)
{
  memset(ipRxStats, 0, sizeof(ipRxStats));
}

void IpRxStatsShow(L7_uchar8 protocol)
{
  L7_uint32 i;
  if (protocol != 0)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\nPkts received in IP MAP for protocol %d:  %u",
                   protocol, ipRxStats[protocol]);
  }
  else
  {
    for (i = 0; i < 256; i++)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                     "\nPkts received in IP MAP for protocol %d:  %u",
                     i, ipRxStats[i]);
    }
  }
}

void IpRxStatsShowOspf(void)
{
  IpRxStatsShow(IP_PROT_OSPFIGP);
}

void IpRxStatsShowUdp(void)
{
  IpRxStatsShow(IP_PROT_UDP);
}

void IpRxStatsShowTcp(void)
{
  IpRxStatsShow(IP_PROT_TCP);
}

void IpRxStatsShowIcmp(void)
{
  IpRxStatsShow(IP_PROT_ICMP);
}

void IpRxStatsShowPim(void)
{
  IpRxStatsShow(IP_PROT_PIM);
}

void IpRxStatsShowVrrp(void)
{
  IpRxStatsShow(IP_PROT_VRRP);
}

void IpRxStatsShowIgmp(void)
{
  IpRxStatsShow(IP_PROT_IGMP);
}

/*********************************************************************
* @purpose  Checks if ipMap user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ipMapHasDataChanged(void)
{

  return (ipMapCfg->cfgHdr.dataChanged ||
          route->cfgHdr.dataChanged ||
          arp->cfgHdr.dataChanged);
}

void ipMapResetDataChanged(void)
{
  ipMapCfg->cfgHdr.dataChanged = L7_FALSE;
  route->cfgHdr.dataChanged = L7_FALSE;
  arp->cfgHdr.dataChanged = L7_FALSE;
  return;
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
L7_RC_t ipMapIpSpoofingModeApply(L7_uint32 mode)
{
  L7_RC_t rc;

  rc = dtlIpv4SpoofingModeSet(mode);
  return(rc);
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
L7_RC_t ipMapIpArpAgeTimeApply(L7_uint32 arpAgeTime)
{
  if (ipMapArpAgeTimeSet(arpAgeTime) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
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
L7_RC_t ipMapIpArpRespTimeApply(L7_uint32 arpRespTime)
{
  if (ipMapArpRespTimeSet(arpRespTime) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
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
L7_RC_t ipMapIpArpRetriesApply(L7_uint32 arpRetries)
{
  if (ipMapArpRetriesSet(arpRetries) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
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
L7_RC_t ipMapIpArpCacheSizeApply(L7_uint32 arpCacheSize)
{
  if (ipMapArpCacheSizeSet(arpCacheSize) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
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
L7_RC_t ipMapIpArpDynamicRenewApply(L7_uint32 arpDynamicRenew)
{
  if (ipMapArpDynamicRenewSet(arpDynamicRenew) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the maximum allowed IP MTU for an interface.
*
* @param    intIfNum
* @param    maxIpMtu
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The link MTU is the payload length of the max L2 frame.
*           For physical interfaces, the max IP MTU is the link MTU.
*           VLAN interfaces do not have a configurable max
*           frame length. So we allow up to the largest jumbo frame
*           for VLAN interfaces. It's the user's responsibility to
*           avoid configuring an IP MTU on a VLAN interface that's
*           larger than the max frame length for any of the physical
*           ports in the VLAN.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfMaxIpMtu(L7_uint32 intIfNum, L7_uint32 *maxIpMtu)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  if (sysIntfType == L7_PHYSICAL_INTF)
  {
    return nimGetIntfMtuSize(intIfNum, maxIpMtu);
  }

  /* Loopback or VLAN interface */
  *maxIpMtu = L7_MAX_FRAME_SIZE - (L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + L7_ETHERNET_CHECKSUM_LEN);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Communicates the IPv4 MTU to other interested parties.
*
* @param    L7_uint32 cfgIpMtu    configured value for IP MTU. 0 means "not configured"
* @param    L7_uint32 intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If IP MTU is not configured, the IP MTU is derived from the link MTU.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfIpMtuApply(L7_uint32 intIfNum, L7_uint32 cfgIpMtu)
{
  L7_uint32 rtrIfNum;
  L7_uint32 ipMtu = cfgIpMtu;
  dtlRtrIntfDesc_t ipCircDesc;


  if (ipMtu == FD_IP_DEFAULT_IP_MTU)
  {
    /* Default the IP MTU to the max payload length of a max length frame. */
    if (nimGetIntfMtuSize(intIfNum, &ipMtu) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (ipMapIntfIsUp(intIfNum))
    {
      /* Set value in IP stack */
      if (osapiIfIpv4MtuSet(intIfNum, ipMtu) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "IP stack failed to accept IPv4 MTU of %u on interface %s.",
                ipMtu, ifName);
        return L7_FAILURE;
      }

     /*
      * NOTE: The IP MTU is associated with a routing interface. Some devices can
      * only apply the IP MTU to a physical port. A physical port can be associated
      * with many (vlan) routing interfaces. Also, devices have limitations w.r.t
      * setting different IPv4 and IPv6 MTU values.
      */
      if (ipMapRtrIntfDataGet(intIfNum, &ipCircDesc) == L7_SUCCESS)
      {
        ipCircDesc.mtu = ipMtu;
        if (dtlIpv4RtrIntfModify(&ipCircDesc) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                  "Hardware failed to accept IPv4 MTU of %u on interface %s.",
                  ipMtu, ifName);
          return L7_FAILURE;
        }
      }
    }
  }

  /* Tell OSPFv2 MTU has changed. OSPF has to come back and ask for MTU. */
  ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_MTU, L7_FALSE, L7_NULL);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Unreachables
*
* @param    intIfNum @b{(input)} internal interface number
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
L7_RC_t ipMapIntfIpUnreachablesModeApply (L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (ipMapIntfIsUp(intIfNum))
    {
      /* Set value in IP stack */
      if (osapiIfIpv4IcmpSendDestinationUnreachableSet(intIfNum, mode) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 *action;
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        if (mode == L7_ENABLE)
            action = "enable";
        else
            action = "disable";
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "IP stack failed to %s sending ICMP Destination Unreachable messages on interface %s.",
                 action, ifName);
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    intIfNum @b{(input)} internal interface number
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
L7_RC_t ipMapIntfIpRedirectsModeApply (L7_uint32 intIfNum, L7_uint32 mode)
{
   L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (ipMapIntfIsUp(intIfNum))
    {
      /* Set value in IP stack */
      if (osapiIfIpv4IcmpRedirectSendSet(intIfNum, mode) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 *action;
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        if (mode == L7_ENABLE)
            action = "enable";
        else
            action = "disable";
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "IP stack failed to %s sending IP Redirects on interface %s.",
                action, ifName);
        return L7_FAILURE;
      }
    }
   }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the Ip Source Checking mode
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
L7_RC_t ipMapIpSourceCheckingApply(L7_uint32 mode)
{
  L7_RC_t rc;

  rc = dtlIpv4SourceAddressCheckingModeSet(mode);

  return(rc);
}

/********************* ROUTER GLOBAL UTIL ROUTINES *******************/
/********************* ROUTER GLOBAL UTIL ROUTINES *******************/
/********************* ROUTER GLOBAL UTIL ROUTINES *******************/
/********************* ROUTER GLOBAL UTIL ROUTINES *******************/


/*********************************************************************
* @purpose  Enable the router.
*
* @param    void
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
L7_RC_t ipMapRtrAdminModeEnable(void)
{
  L7_uint32 intIfNum;

  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, __FUNCTION__);

  /* If previous event enabled router, nothing to be done. */
  if (pIpMapInfo->operRtr.lastNotify == L7_RTR_ENABLE)
    return L7_SUCCESS;

  /* If a global asynchronous event is pending, process change after
   * that event completes. */
  if (pIpMapInfo->operRtr.asyncPending)
    return L7_SUCCESS;

  /* If enabling routing globally converts one or more host interfaces to
   * routing interfaces, then best route change from RTO can report 
   * a route modify in place of delete and add. Driver won't accept modify 
   * if if doesn't already have route. To prevent this, deregister and
   * reregister with RTO. */
  if (rtoBestRouteClientDeregister("IP MAP", ipMapRouteCallback) == L7_SUCCESS)
  {
    /* IP MAP was registered; so re-register */
    if (rtoBestRouteClientRegister("IP MAP", ipMapRouteCallback) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Unable to register best route callback with RTO");
      return L7_ERROR;
    }
  }

  /*Enable the forwarding in freebsd code*/
    ipMapRtrForwardingModeApply(L7_ENABLE);

  /* Notify router event clients that routing is enabled. Non-async. */
  if (ipMapRoutingEventChangeNotify(0, L7_RTR_ENABLE, L7_FALSE, L7_NULLPTR) == L7_SUCCESS)
  {
    pIpMapInfo->operRtr.lastNotify = L7_RTR_ENABLE;
  }
  else
  {
    IPMAP_ERROR("%s %d: %s: Failed to notify router event clients that routing is enabled.\n",
                __FILE__, __LINE__, __FUNCTION__);
  }

  /* See if any routing interfaces are ready to be enabled. */
    if (nimPhaseStatusCheck() == L7_TRUE)
    {
        L7_RC_t rc = nimFirstValidIntfNumber(&intIfNum);
        while (rc == L7_SUCCESS)
        {
          ipMapIntfUpdate(intIfNum, NULL);
          /* Had to call this to re-enable to enable Routing */
          if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_DISABLE)
          {
            ipMapIntfUpdate(intIfNum, NULL);
          }
          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
        }
    }

  ipMapMartianAddrsInstall();

  /* Add the static reject routes into RTO */
  ipMapStaticRejectRoutesAdd();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable the router.
*
* @param    void
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
L7_RC_t ipMapRtrAdminModeDisable(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0;
#ifdef L7_NSF_PACKAGE
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
#endif

  IPMAP_TRACE("%s %d: %s\n", __FILE__, __LINE__, __FUNCTION__);

  if (pIpMapInfo->operRtr.lastNotify != L7_RTR_ENABLE)
    return L7_SUCCESS;

  if (dtlIpv4RouteEntryDeleteAll () != L7_SUCCESS)
  {
    IPMAP_ERROR("%s %d: %s: Failed to Delete all the IPv4 route entries from "
                "the hardware\n", __FILE__, __LINE__, __FUNCTION__);
  }
  
  if (dtlIpv4ArpEntryDeleteAll () != L7_SUCCESS)
  {
    IPMAP_ERROR("%s %d: %s: Failed to Delete all the IPv4 ARP entries from "
                "the hardware\n", __FILE__, __LINE__, __FUNCTION__);
  }

  /* Notify router event clients that routing is disabled. Requires async response. */
  if (ipMapRoutingEventChangeNotify(0, L7_RTR_DISABLE_PENDING,
                                    L7_TRUE, L7_NULLPTR) == L7_SUCCESS)
  {
    pIpMapInfo->operRtr.lastNotify = L7_RTR_DISABLE_PENDING;
    pIpMapInfo->operRtr.asyncPending = L7_TRUE;
  }
  else
  {
    IPMAP_ERROR("%s %d: %s: Failed to notify router event clients that routing is disabled\n",
                __FILE__, __LINE__, __FUNCTION__);
  }
  for (rc = nimFirstValidIntfNumber(&intIfNum); rc == L7_SUCCESS;
        rc = nimNextValidIntfNumber(intIfNum, &intIfNum))
  {
    if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_ENABLE)
    {
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE_PENDING;
    }
  }

  ipMapRtrForwardingModeApply(L7_DISABLE);

  ipMapMartianAddrsRemove();

  /* Remove the static reject routes from RTO */
  ipMapStaticRejectRoutesRemove();

#ifdef L7_NSF_PACKAGE
  memset(macAddr, 0, sizeof(macAddr));
  /* Calling the checkpointing code for NSF to flush all ARP entries */
  arpMapCheckpointCallback(ARP_ENTRY_FLUSH_ALL, 0, 0,
                           macAddr);
  ipMapDhcpFullCheckpoint();
#endif

  return L7_SUCCESS;
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
L7_RC_t ipMapRtrForwardingModeApply(L7_uint32 mode)
{
  if (mode == L7_DISABLE)
  {
    if(dtlIpv4ForwardingModeSet(mode) != L7_SUCCESS)
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
               "Could not set forwarding mode in driver\n");

    freeBSDIPForwardingSet(L7_DISABLE);
    return L7_SUCCESS;
  }
  else if (mode == L7_ENABLE)
  {
    if (ipMapCfg->rtr.rtrAdminMode == L7_ENABLE)
    {
      if (dtlIpv4ForwardingModeSet(mode) != L7_SUCCESS)
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                  "Could not set forwarding mode in driver\n");

      freeBSDIPForwardingSet(L7_ENABLE);
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Enable or disable TOS forwarding
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    Deprecated. TOS forwarding not supported.
*
*********************************************************************/
L7_RC_t ipMapRtrTosForwardingModeApply(L7_uint32 mode)
{
  L7_RC_t rc;

    rc = dtlIpv4TOSForwardingModeSet(mode);

  return(rc);
}

/*********************************************************************
* @purpose  Enable or disable sending of ICMP Redirects
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
*********************************************************************/
L7_RC_t ipMapRtrICMPRedirectModeApply(L7_uint32 mode)
{
  L7_RC_t rc;

  rc = dtlIpv4ICMPRedirectModeSet(mode);

  return(rc);
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
L7_RC_t ipMapRtrICMPRatelimitApply (L7_uint32 burstSize, L7_uint32 interval)
{
   /* Informing to Stack */
   return osapiIpv4IcmpRatelimitSet(burstSize, interval);

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
L7_RC_t ipMapRtrICMPEchoReplyModeApply (L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;

  /* Informing to Stack */
  if (mode == L7_DISABLE)
  {
    rc = osapiIpv4IcmpIgnoreEchoRequestSet(L7_ENABLE);
  }
  else
  {
     rc = osapiIpv4IcmpIgnoreEchoRequestSet(L7_DISABLE);
  }

  if (rc != L7_SUCCESS)
  {
      L7_uchar8 *action;
      if (mode == L7_ENABLE)
          action = "enable";
      else
          action = "disable";
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Failed to %s sending ICMP Echo Replies in IP Stack.", action);
       return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable BOOTP/DHCP Relay
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
L7_RC_t ipMapRtrBootpDhcpRelayModeApply(L7_uint32 mode, L7_IP_ADDR_t ipAddr)
{
  L7_RC_t rc;

  rc = dtlIpv4BootpDhcpRelaySet(mode, ipAddr);

  return(rc);
}

/*********************************************************************
* @purpose  Set the BOOTP/DHCP Relay Server
*
* @param    ipAddress @b{(input)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrBootpDhcpRelayServerApply(L7_uint32 mode,
                                          L7_IP_ADDR_t ipAddress)
{
  L7_RC_t rc;


  rc = dtlIpv4BootpDhcpRelaySet(mode, ipAddress);

  return(rc);
}

/*********************************************************************/
/********************* ROUTER INTERFACE UTIL ROUTINES ****************/
/********************* ROUTER INTERFACE UTIL ROUTINES ****************/
/********************* ROUTER INTERFACE UTIL ROUTINES ****************/
/********************* ROUTER INTERFACE UTIL ROUTINES ****************/
/********************* ROUTER INTERFACE UTIL ROUTINES ****************/


/*********************************************************************
* @purpose  Enable or disable forwarding of net directed broadcasts
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
L7_RC_t ipMapRtrIntfNetDirectBcastsApply( L7_uint32 intIfNum, L7_uint32 mode)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a routing interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assigns a routing interface number for the interface, informs
*           the IPv4 software forwarding code, and generates
*           L7_PORT_ROUTING_ENABLED NIM event.
*
*           The only condition for creating a routing interface is that routing
*           is configured on the interface. It may not be possible to enable
*           the interface (if the interface is down, address not configured, etc).
*
* @end
*********************************************************************/
static L7_RC_t ipMapRoutingIntfCreate(L7_uint32 intIfNum)
{
  L7_uint32 rtrIntf = 0;
  NIM_HANDLE_t nimHandle = 0;


  /* Assign router interface number */
  if (ipstkRtrIfNumGet(intIfNum, L7_IP_MAP_COMPONENT_ID, &rtrIntf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  intIfNumToRtrIntf[intIfNum] = rtrIntf;
  rtrIntfMap[rtrIntf].intIfNum = intIfNum;
  rtrIntfMap[rtrIntf].present = L7_TRUE;
  pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;

  /* Tell IPv4 software forwarding code that interface exists */
  rtIfNetCreate(intIfNum);

  if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
      (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
  {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                    "IP MAP sending L7_PORT_ROUTING_ENABLED for interface %s",
                    ifName);
      ipMapTraceWrite(traceBuf);
  }
  ipstkIntfChangeNimNotify(intIfNum, L7_PORT_ROUTING_ENABLED,
                           L7_NULLPTR, &nimHandle, L7_IP_MAP_COMPONENT_ID);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  When a numbered interface comes up or goes down, check if
*           the change affects the state of any unnumbered interfaces.
*
* @param    intIfNum @b{(input)} Internal Interface Number. Must be numbered.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t ipMapNumberedIntfStateChange(L7_uint32 intIfNum)
{
  L7_uint32 i;   /* an internal interface number */
  L7_RC_t rc;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_rtrCfgCkt_t *otherCfg = L7_NULLPTR;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

  for (rc = nimFirstValidIntfNumber(&i);
        rc == L7_SUCCESS;
        rc = nimNextValidIntfNumber(i, &i))
  {
    if (ipMapMapIntfIsConfigurable(i, &otherCfg))
    {
      if ((otherCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
      {
        if (ipMapNumberedIfc(i) == intIfNum)
        {
          ipMapIntfUpdate(i, NULL);
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable a routing interface
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    enableMode @b{(input)} Routing mode (or) Host mode
*                                  (or) none
* @param    method     @b{(input)} IP Address Configuration Method
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipMapIntfEnable (L7_uint32 intIfNum,
                 IPMAP_RTR_INTF_ENABLE_MODE_t enableMode,
                 L7_INTF_IP_ADDR_METHOD_t method)
{
  L7_uint32 rtrIntf;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_uint32 j;
  ipMapArpIntfRegParms_t arp_reg;
  L7_BOOL isUnnumbered = L7_FALSE;
  L7_routeEntry_t routeEntry;
  L7_routeEntry_t routeEntryDefault;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_RC_t rc;
  L7_uint32 gwAddr = 0;

  /* If interface is unnumbered, these identify the numbered interface */
  L7_uint32 numbIntIfNum;
  L7_rtrCfgCkt_t *numberedCfg = L7_NULLPTR;

  /* IP address and mask used operationally by the interface. */
  L7_uint32 operAddr;
  L7_uint32 operMask;

  if (ipMapTraceFlags & IPMAP_TRACE_INTF_STATE_CHANGE)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf (traceBuf, sizeof(traceBuf),
                   "[%s-%d]: Enabling intIfNum - %d; Mode - %d; Method - %d\n",
                   __FUNCTION__, __LINE__, intIfNum, enableMode, method);
    ipMapTraceWrite (traceBuf);
  }

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIntf) != L7_SUCCESS)
    return L7_FAILURE;

  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
  {
    isUnnumbered = L7_TRUE;
    numbIntIfNum = ipMapNumberedIfc(intIfNum);
    if (!ipMapMapIntfIsConfigurable(numbIntIfNum, &numberedCfg))
      return L7_FAILURE;
    operAddr = numberedCfg->addrs[0].ipAddr;
    /* Netmask is not valid for un-numbered interface. Set it to 0xFFFFFFFF */
    operMask = 0xFFFFFFFF;
  }
  else
  {
    /* numbered interface. Use configured primary address. */
    operAddr = pCfg->addrs[0].ipAddr;
    operMask = pCfg->addrs[0].ipMask;
  }

  /* Get MAC address of the routing interface */
  if (nimGetIntfL3MacAddress(intIfNum, 0, mac) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to get MAC address for interface %s.", ifName);
    return L7_FAILURE;
  }

  printf("Got intfnum:%u mac address to %02X:%02X:%02X:%02X:%02X:%02X\n", intIfNum,
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  if (enableMode == IPMAP_RTR_INTF_ENABLE_HOST)
  {
    pCfg->flags |= L7_RTR_INTF_HOST;
  }

  /* dtl event to create routing interface in hardware */
    if (ipMapDtlIpIntfSet(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* Create routing interface in IP stack */
  if (ipmRouterIfCreate(rtrIntf, intIfNum, operAddr, operMask,
                        isUnnumbered, mac) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to create router interface %s in IP stack.", ifName);

    /* undo what's already been done */
    dtlIpv4IntfIPAddrSet(intIfNum, 0, 0);
    ipstkIntfChangeDtlNotify(intIfNum, L7_DISABLE, L7_IP_MAP_COMPONENT_ID);
    return L7_FAILURE;
  }

  pIpMapInfo->operIntf[intIfNum].stackEnabled = L7_TRUE;

  /* Check if DHCP Mode is already configured on this interface.
   * If so, Invoke DHCP Client to lease an IP address for this interface.
   * After acquiring the address, DHCP client invokes the respective IPMAP
   * API to update the leased IP address.
   */
  if (((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0) &&
      ((pCfg->addrs[0].ipAddr == 0) || (pCfg->addrs[0].ipMask == 0)))
  {
    if (dhcpClientIPAddressMethodSet (intIfNum, method, L7_MGMT_IPPORT, L7_FALSE)
                                   != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  if ((!isUnnumbered) && (operAddr != 0) && (operMask != 0))
  {
    /* Add local route for primary address */
    memset(&routeEntry, 0x00, sizeof(L7_routeEntry_t));
    routeEntry.ipAddr      = operAddr & operMask;
    routeEntry.subnetMask  = operMask;
    routeEntry.protocol    = RTO_LOCAL;
    routeEntry.metric      = FD_RTR_ROUTE_DEFAULT_COST;
    routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = operAddr;
    routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = intIfNum;
    routeEntry.ecmpRoutes.numOfRoutes = 1;

    /* Create a local route for the primary IP address on this interface. */
    /* Local routes are checkpointed. So it could be already configured if we are
     * doing a warm restart. */
    rc = rtoRouteAdd (&routeEntry);
    if ((rc != L7_SUCCESS) && ((rc != L7_ALREADY_CONFIGURED) || !pIpMapInfo->warmRestart))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_char8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
      memset((L7_char8 *)destStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
      memset((L7_char8 *)maskStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
      osapiInetNtoa(routeEntry.ipAddr, destStr);
      osapiInetNtoa(routeEntry.subnetMask, maskStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
                "Failed to add local route for network %s/%s on interface %s.",
                destStr, maskStr, ifName);

      /* undo what's already been done */
      ipmRouterIfRemove(intIfNum);
      pIpMapInfo->operIntf[intIfNum].stackEnabled = L7_FALSE;
      dtlIpv4IntfIPAddrSet(intIfNum, 0, 0);
      ipstkIntfChangeDtlNotify(intIfNum, L7_DISABLE, L7_IP_MAP_COMPONENT_ID);
      return L7_FAILURE;
    }
    /* Update the default gateway if one is not already present.
     * Useful in scenario's where an interface link goes down and comes up.
     */
    if (((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0) &&
        (rtoIsDhcpDefGatewayPresent (&gwAddr) != L7_TRUE))
    {
      L7_uint32 defGwAddr = 0;
      if (dhcpClientRtrIntfDefaultGatewayGet (rtrIntf, L7_MGMT_IPPORT,
                                              &defGwAddr)
                                           == L7_SUCCESS)
      {
        memset (&routeEntryDefault, 0, sizeof(L7_routeEntry_t));
        routeEntryDefault.ipAddr = 0;
        routeEntryDefault.subnetMask = 0;
        routeEntryDefault.protocol = RTO_DEFAULT;
        routeEntryDefault.pref = _ipMapRouterPreferenceGet(ROUTE_PREF_DEFAULT_ROUTE_DHCP);
        routeEntryDefault.metric = FD_RTR_ROUTE_DEFAULT_COST;
        routeEntryDefault.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = defGwAddr;
        routeEntryDefault.ecmpRoutes.numOfRoutes = 1;
        if (rtoRouteAdd (&routeEntryDefault) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
                     "Failed to add the Default Gateway "
                     "Route to RTO on intIfNum - %d", intIfNum);

          /* undo what's already been done */
          rtoRouteDelete(&routeEntry);
          ipmRouterIfRemove(intIfNum);
          pIpMapInfo->operIntf[intIfNum].stackEnabled = L7_FALSE;
          dtlIpv4IntfIPAddrSet(intIfNum, 0, 0);
          ipstkIntfChangeDtlNotify(intIfNum, L7_DISABLE, L7_IP_MAP_COMPONENT_ID);
          return L7_FAILURE;
        }

        /* Update in the IP Stack */
        ipstkDefGwUpdate();
      }
    }
  }

  /* Apply the IPv4 MTU */
  ipMapIntfIpMtuApply(intIfNum, pCfg->ipMtu);

  /* Apply the IPv4 Unreachables Mode */
  ipMapIntfIpUnreachablesModeApply (intIfNum, pCfg->icmpUnreachablesMode);

  /*Apply the IPv4 Redirects Mode */
  ipMapIntfIpRedirectsModeApply (intIfNum, pCfg->icmpRedirectsMode);

  /* Tell IPv4 forwarding code that interface is up. */
  (void)rtIfNetUp(intIfNum);

  /* If the interface is capable of running ARP, register the interface with ARP. */
  if ((ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE) &&
      (operAddr != 0) && (operMask != 0))
  {
    /* Register with ARP code. */
    arp_reg.ipAddr = operAddr;
    arp_reg.netMask = operMask;
    arp_reg.unnumbered = isUnnumbered;

    if ((pCfg->flags & L7_RTR_INTF_PROXY_ARP) == L7_RTR_INTF_PROXY_ARP)
      arp_reg.proxyArp = L7_TRUE;
    else
      arp_reg.proxyArp = L7_FALSE;

    if ((pCfg->flags & L7_RTR_INTF_LOCAL_PROXY_ARP) == L7_RTR_INTF_LOCAL_PROXY_ARP)
      arp_reg.localProxyArp = L7_TRUE;
    else
      arp_reg.localProxyArp = L7_FALSE;

    arp_reg.pMacAddr = mac;
    arp_reg.rawSendFn = ipmRouterIfBufSend;
    arp_reg.sendBufSize = 2048;
    arp_reg.intfHandle = rtrIntf;
    if (ipMapArpIntfRegister (intIfNum, &arp_reg) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to register router interface %s with ARP.", ifName);

      /* undo what's been done. */
      if (!isUnnumbered)
      {
        rtoRouteDelete(&routeEntry);
        rtoRouteDelete(&routeEntryDefault);
      }
      ipmRouterIfRemove(intIfNum);
      pIpMapInfo->operIntf[intIfNum].stackEnabled = L7_FALSE;
      rtIfNetDown(intIfNum);
      dtlIpv4IntfIPAddrSet(intIfNum, 0, 0);
      ipstkIntfChangeDtlNotify(intIfNum, L7_DISABLE, L7_IP_MAP_COMPONENT_ID);

      return L7_FAILURE;
    }
  }

  if (enableMode == IPMAP_RTR_INTF_ENABLE_HOST)
  {
    /* For now, do not notify a Host Interface to Clients.
     * If required, this can be notified in future, if need arises.
     */
    pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_HOST_MODE;
  }
  else if (enableMode == IPMAP_RTR_INTF_ENABLE_ROUTING)
  {
#ifdef L7_IP_MCAST_PACKAGE
    /* Apply mcast forwarding mode */
    if ((pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE) != 0)
    {
      ipMapRtrIntfMcastFwdModeApply (intIfNum, L7_ENABLE);
    }
    else
    {
      ipMapRtrIntfMcastFwdModeApply (intIfNum, L7_DISABLE);
    }
#endif
    /* Notify Clients that a Host interface has now become a Routing
     * Interface.
     */
    ipMapRoutingEventChangeNotify (intIfNum, L7_RTR_INTF_ENABLE, L7_FALSE,
                                   L7_NULL);
    pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_ENABLE;
  }
  else
  {
    /* Do Nothing */
  }

  /* See if any static routes or static ARP entries are associated with
   * the primary subnet on this interface */
  ipMapRtrIntfStaticConfigApply(intIfNum, operAddr, operMask);

  if ((pCfg->flags & L7_RTR_INTF_GRAT_ARP) != 0 && ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
  {
    /* Send a sequence of gratuitous ARPs */
    ipMapGratArpSend(intIfNum, operAddr);
  }

  /* Process secondary addresses. Can't have secondaries on an unnumbered ifc. */
  if (!isUnnumbered)
  {
    for(j = 1; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr)
      {
      ipMapRtrIntfSecondaryIpAddressApply(intIfNum, pCfg->addrs[j].ipAddr,
                                          pCfg->addrs[j].ipMask, j);

    }
  }
  }

  /* timestamp */
  /* ROBRICE - need this?
  usmDb1213SysUpTimeRawGet(USMDB_UNIT_CURRENT,&ip6Map->operIntf[intIfNum].operLastChanged);
  ip6Map->ifLastChange = ip6Map->operIntf[intIfNum].operLastChanged;
  */

  /* If a numbered interface came up, see if that allows any unnumbered
   * interfaces to come up. */
  if (!isUnnumbered)
    ipMapNumberedIntfStateChange(intIfNum);

#ifdef L7_NSF_PACKAGE
  if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
  {
    L7_uint32 defGwAddr = 0;
    dhcpClientRtrIntfDefaultGatewayGet (rtrIntf, L7_MGMT_IPPORT, &defGwAddr);
    ipMapCheckpointDhcp (intIfNum, pCfg->addrs[0].ipAddr, pCfg->addrs[0].ipMask,
                         defGwAddr, L7_TRUE);
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable a routing interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Even if there is some failure along the way, continue to try
*           to clean up the interface as much as possible. Caller must
*           hold IP MAP write lock.
*
* @end
*********************************************************************/
static L7_RC_t ipMapIntfDisable(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_BOOL isUnnumbered = L7_FALSE;   /* L7_TRUE if interface is unnumbered */
  L7_uint32 rtrIntf = 0;
  L7_uint32 j;
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL wasEnabledInStack = pIpMapInfo->operIntf[intIfNum].stackEnabled;

  if (ipMapTraceFlags & IPMAP_TRACE_INTF_STATE_CHANGE)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf (traceBuf, sizeof(traceBuf),
                   "[%s-%d]: Disabling intIfNum - %d\n", __FUNCTION__, __LINE__,
                   intIfNum);
    ipMapTraceWrite (traceBuf);
  }

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Attempt to disable IPv4 on interface %s which cannot be configured for IPv4.",
            ifName);
    rc = L7_FAILURE;
  }

  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
    isUnnumbered = L7_TRUE;

  /* Unnumbered interfaces check if numbered interface is up to update
   * their own state in ipMapNumberedIntfStateChange(). They look at
   * stackEnabled. So need to set that here, before we actually remove
   * numbered interface from IP stack. */
  pIpMapInfo->operIntf[intIfNum].stackEnabled = L7_FALSE;

  /* First remove all secondary IP addresses from the interface. This takes all
   * actions to remove the secondary from the IP stack, route table, ARP cache, etc. */
  if (!isUnnumbered)
  {
    for (j=1; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr)
      {
        if (ipMapRtrIntfSecondaryIpAddressRemoveApply(intIfNum,
                                                      pCfg->addrs[j].ipAddr,
                                                      pCfg->addrs[j].ipMask,
                                                      j) != L7_SUCCESS)
        {
          rc = L7_FAILURE;
        }
      }
    }
  }

  /* If a numbered interface is going down, see if that forces any unnumbered
   * interfaces down. */
  if (!isUnnumbered)
    ipMapNumberedIntfStateChange(intIfNum);

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
  {
    /* Clear the ARP cache of entries associated with this interface */
    if (ipMapArpCacheClear(intIfNum, L7_TRUE) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to clear ARP cache of entries learned on interface %s when "
              "interface disabled.", ifName);
      rc = L7_FAILURE;
    }

    /* Delete all static ARP entries whose target IP is on the same subnet as
     * the primary IP address on this interface. */
    ipMapIntfStaticArpDelete(intIfNum);

    /* Tell ARP that we are no longer using this interface. */
    if (ipMapArpIntfUnregister(intIfNum) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to unregister router interface %s with ARP.", ifName);
      rc = L7_FAILURE;
    }
  }

  /* Tell IPv4 software forwarding code that interface is down. */
  (void)rtIfNetDown(intIfNum);

  /* Tell IP stack that IPv4 is down for this interface. */
  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIntf) == L7_SUCCESS)
    ipstkRtrIfDown(rtrIntf, L7_IP_MAP_COMPONENT_ID);

  /* remove static routes whose next hop is on the primary's subnet */
  ipMapSubnetStaticRoutesRemove(intIfNum, pCfg->addrs[0].ipAddr, pCfg->addrs[0].ipMask);

  /* remove local route for primary address */
  if (!isUnnumbered && pCfg->addrs[0].ipAddr && pCfg->addrs[0].ipMask)
    ipMapSubnetLocalRouteRemove(pCfg->addrs[0].ipAddr, pCfg->addrs[0].ipMask);

  if (wasEnabledInStack)
  {
    if (ipmRouterIfRemove(intIfNum) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to remove router interface %s from IP stack.", ifName);
      rc = L7_FAILURE;
    }
  }

  /* This is a bit of a hack. On a stack, deleting the routing interface in hardware
   * can block for a long time (on the order of a minute). Avoid hanging the system
   * for that long by releasing the IP MAP lock here and retaking after interface is
   * deleted in hardware. DTL calls don't access IP MAP data and don't need to
   * hold the lock. */
  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_MULTICAST) == L7_TRUE)
  {
    /*------------------------------------------------------------------------------*/
    /* Remove static IP local multicast addresses from the intf local multicast table.  */
    /*------------------------------------------------------------------------------*/
    if (dtlIpv4LocalMulticastAddrDelete(intIfNum, L7_IP_ALL_HOSTS_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to remove IP All Hosts Address from interface %s.", ifName);
    }

    if ((pCfg->flags & L7_RTR_INTF_HOST) == 0)
    {
      if (dtlIpv4LocalMulticastAddrDelete(intIfNum, L7_IP_ALL_RTRS_ADDR) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Failed to remote IP All Routers Address from interface %s.", ifName);
      }
    }

    if (dtlIpv4LocalMulticastAddrDelete(intIfNum, L7_IP_LTD_BCAST_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to remove Limited Broadcast Address from interface %s.", ifName);
    }
  }

    /* if v6 still there, need to clear addr */
    dtlIpv4IntfIPAddrSet(intIfNum, 0, 0);

  /* dtl event to delete interface in hardware */
  if (ipstkIntfChangeDtlNotify(intIfNum, L7_DISABLE,
                               L7_IP_MAP_COMPONENT_ID) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to remove router interface %s from hardware.", ifName);
    rc = L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
  {
    L7_uint32 defGwAddr = 0;
    dhcpClientRtrIntfDefaultGatewayGet (rtrIntf, L7_MGMT_IPPORT, &defGwAddr);
    ipMapCheckpointDhcp (intIfNum, pCfg->addrs[0].ipAddr, pCfg->addrs[0].ipMask,
                         defGwAddr, L7_FALSE);
  }
#endif

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return L7_FAILURE;

  /* ROBRICE - keep? timestamp
  usmDb1213SysUpTimeRawGet(USMDB_UNIT_CURRENT,&ip6Map->operIntf[intIfNum].operLastChanged);
  ip6Map->ifLastChange = ip6Map->operIntf[intIfNum].operLastChanged;
  */

  if ((pCfg->flags & L7_RTR_INTF_HOST) != 0)
  {
    pCfg->flags &= ~(L7_RTR_INTF_HOST);
  }

  return rc;
}

/*********************************************************************
* @purpose  Check the state of an interface and determine whether the
*           state needs to be updated in response to configuration changes,
*           recently completed router events, or NIM events.
*
* @param    intIfNum         Internal Interface Number
* @param    eventCompleted   ({output}) Set to L7_FALSE if an asynchronous
*                                       router event has been initiated
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    This function allows IP MAP to defer acting on certain events
*           when other events are still in progress.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfUpdate(L7_uint32 intIfNum, L7_uint32 *eventCompleted)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_uint32 rtrIntf;
  NIM_HANDLE_t nimHandle = 0;
  L7_NIM_QUERY_DATA_t QueryData;
  IPMAP_RTR_INTF_ENABLE_MODE_t enableMode;
  L7_INTF_IP_ADDR_METHOD_t method;

  printf("%s(%u): intfNum:%u\n", __FUNCTION__, __LINE__, intIfNum);

  /* Assume no asych event generated unless we determine otherwise. */
  if (eventCompleted)
    *eventCompleted = L7_TRUE;

  /* validate we have ifnum to cfgid mapping */
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
    return(L7_SUCCESS);
  }

  rtrIntf = intIfNumToRtrIntf[intIfNum];

  /* if routing enabled on interface, get router interface number. */
  if ((pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE) ==
      L7_RTR_INTF_ADMIN_MODE_ENABLE)
  {
    if (rtrIntf == 0)
    {
      if (ipMapRoutingIntfCreate(intIfNum) != L7_SUCCESS)
      {
        printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
    }
  }

  if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
  {
    method = L7_INTF_IP_ADDR_METHOD_DHCP;
  }
  else if (pCfg->addrs[0].ipAddr != 0)
  {
    method = L7_INTF_IP_ADDR_METHOD_CONFIG;
  }
  else
  {
    method = L7_INTF_IP_ADDR_METHOD_NONE;
  }

  /* Check if interface is down and should be up or if interface is up and
   * should be down. */
  enableMode = ipMapMayEnableInterface(intIfNum);
  printf("%s(%u): ENABLE MODE IS: %u\n", __FUNCTION__, __LINE__, enableMode);
  if (enableMode == IPMAP_RTR_INTF_ENABLE_ROUTING)
  {
    printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
    if (ipMapTraceFlags & IPMAP_TRACE_INTF_STATE_CHANGE)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf (traceBuf, sizeof(traceBuf),
                     "[%s-%d]: intIfNum - %d is a Routing Intf  with lastNotify - %s\n",
                     __FUNCTION__, __LINE__, intIfNum,
                     ipMapRouterEventNames[pIpMapInfo->operIntf[intIfNum].lastNotify]);
      ipMapTraceWrite (traceBuf);
    }
    /* Translate a Host interface to a Routing interface */
    if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_HOST_MODE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      ipMapIntfDisable(intIfNum);
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
    }
    /* Interface should be enabled */
    else if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_DISABLE)
    {
      /* Interface is not enabled. If no async event is pending for this
       * interface or for the router, bring up the interface. */
      if ((!pIpMapInfo->operIntf[intIfNum].asyncPending) &&
          (!pIpMapInfo->operRtr.asyncPending))
      {
        printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
        return ipMapIntfEnable(intIfNum, IPMAP_RTR_INTF_ENABLE_ROUTING, method);
      }
    }
    else if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_ENABLE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE_PENDING;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;
      ipMapRoutingEventChangeNotify (intIfNum, L7_RTR_INTF_DISABLE_PENDING,
                                     L7_TRUE, L7_NULL);
      /* tell caller not to respond to NIM until this event completes. */
      if (eventCompleted)
      {
        *eventCompleted = L7_FALSE;
      }
    }
    else
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Do Nothing */
    }
  }
  else if (enableMode == IPMAP_RTR_INTF_ENABLE_HOST)
  {
    printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
    if (ipMapTraceFlags & IPMAP_TRACE_INTF_STATE_CHANGE)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf (traceBuf, sizeof(traceBuf),
                     "[%s-%d]: intIfNum - %d is a Host Intf  with lastNotify - %s\n",
                     __FUNCTION__, __LINE__, intIfNum,
                     ipMapRouterEventNames[pIpMapInfo->operIntf[intIfNum].lastNotify]);
      ipMapTraceWrite (traceBuf);
    }
    /* Interface should be enabled */
    if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_DISABLE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Interface is not enabled. If no async event is pending for this
       * interface or for the router, bring up the interface. */
      if (!pIpMapInfo->operIntf[intIfNum].asyncPending &&
          !pIpMapInfo->operRtr.asyncPending)
      {
        if ((pIpMapInfo->operIntf[intIfNum].stackEnabled != L7_TRUE) ||
            ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0))
        {
          printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
          return ipMapIntfEnable(intIfNum, IPMAP_RTR_INTF_ENABLE_HOST, method);
        }
      }
    }
    else if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_ENABLE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE_PENDING;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;
      ipMapRoutingEventChangeNotify (intIfNum, L7_RTR_INTF_DISABLE_PENDING,
                                     L7_TRUE, L7_NULL);
      /* tell caller not to respond to NIM until this event completes. */
      if (eventCompleted)
      {
        *eventCompleted = L7_FALSE;
      }
    }
    else if (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_HOST_MODE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Interface is in Host Mode. Need to bring it down.
       * Assuming that no other protocols are interested in receiving Host Mode
       * notifications, directly disabling the interface here instead of posting
       * disable pending events.
       */
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_FALSE;
    }
    else
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Do Nothing */
    }
  }
  else
  {
    printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
    if (ipMapTraceFlags & IPMAP_TRACE_INTF_STATE_CHANGE)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf (traceBuf, sizeof(traceBuf),
                     "[%s-%d]: intIfNum - %d is neither a Routing nor a Host Intf; "
                     "with lastNotify - %s; Should be Disabled\n",
                     __FUNCTION__, __LINE__, intIfNum,
                     ipMapRouterEventNames[pIpMapInfo->operIntf[intIfNum].lastNotify]);
      ipMapTraceWrite (traceBuf);
    }
    /* Interface should be disabled. If a down event is already pending on either
     * the interface or the router, no need to send the interface down. */
    if (pIpMapInfo->operIntf[intIfNum].asyncPending || pIpMapInfo->operRtr.asyncPending)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* tell caller not to respond to NIM. IP MAP can't process this NIM event yet
       * because another async event is still in progress. */
      if (eventCompleted)
      {
        *eventCompleted = L7_FALSE;
      }
    }
    else if ((rtrIntf != 0) &&
             (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_ENABLE))
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Interface is enabled. Need to bring it down. */
      ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_DISABLE_PENDING,
                                    L7_TRUE, L7_NULL);
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE_PENDING;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;

      /* tell caller not to respond to NIM until this event completes. */
      if (eventCompleted)
      {
        *eventCompleted = L7_FALSE;
      }
    }
    else if ((rtrIntf != 0) &&
             (pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_HOST_MODE))
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Interface is in Host Mode. Need to bring it down.
       * Assuming that no other protocols are interested in receiving Host Mode
       * notifications, directly disabling the interface here instead of posting
       * disable pending events.
       */
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_FALSE;
    }
  }

  /* if an interface disable event has completed and routing interface still
   * exists, clean up the interface. */
  if ((pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_DISABLE) &&
      (rtrIntf > 0))
  {
    printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
    if (pIpMapInfo->operIntf[intIfNum].stackEnabled == L7_TRUE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      ipMapIntfDisable(intIfNum);
    }

    /* if routing no longer configured on interface, free rtrIntf */
    if ((pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE) !=
        L7_RTR_INTF_ADMIN_MODE_ENABLE)
    {
      printf("%s(%u): HERE\n", __FUNCTION__, __LINE__);
      /* Tell IPv4 software forwarding code that interface is gone. */
      (void)rtIfNetDelete(intIfNum);

      /* router interface goes away */
      ipstkRtrIfNumRelease(rtrIntf, L7_IP_MAP_COMPONENT_ID);
      intIfNumToRtrIntf[intIfNum] = 0;
      rtrIntfMap[rtrIntf].intIfNum = 0;
      rtrIntfMap[rtrIntf].present = L7_FALSE;

      QueryData.intIfNum = intIfNum;
      QueryData.request = L7_NIM_QRY_RQST_PRESENT;
      if ((nimIntfQuery(&QueryData) == L7_SUCCESS) &&
          (QueryData.data.present == L7_TRUE))
      {
        /* Trigger NIM event to allow bridging on this interface */
        ipstkIntfChangeNimNotify(intIfNum, L7_PORT_ROUTING_DISABLED,
                                 L7_NULLPTR, &nimHandle, L7_IP_MAP_COMPONENT_ID);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determines whether all conditions have been met for
*           a routing interface to be enabled.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  IPMAP_RTR_INTF_ENABLE_ROUTING if all conditions have been met
*           IPMAP_RTR_INTF_ENABLE_HOST enable for Host interface
*           IPMAP_RTR_INTF_NO_ENABLE cannot enable
*
* @notes    The conditions checked are as follows:
*             a) routing is enabled at the node level
*             b) routing is configured on the interface
*             c) an IP address has been configured on the interface
*             d) the interface has not been acquired (e.g., for a LAG)
*             e) the interface is attached and active
*
* @end
*********************************************************************/
IPMAP_RTR_INTF_ENABLE_MODE_t
ipMapMayEnableInterface (L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 linkState;
  L7_rtrCfgCkt_t *pCfg;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
    return IPMAP_RTR_INTF_NO_ENABLE;
  }

  if ((pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE) == 0)
  {
    printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE (%08X)\n", __FUNCTION__, __LINE__, pCfg->flags);
    return IPMAP_RTR_INTF_NO_ENABLE;
  }

  if (mirrorIsActiveProbePort(intIfNum) ||
      dot3adIsLagActiveMember(intIfNum))
  {
    printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
    return IPMAP_RTR_INTF_NO_ENABLE;
  }

  if (ipMapIntfIsAttached(intIfNum) == L7_FALSE)
  {
    printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
    return IPMAP_RTR_INTF_NO_ENABLE;
  }

  rc = nimGetIntfActiveState (intIfNum, &linkState);          
  if ((rc != L7_SUCCESS) || (linkState != L7_ACTIVE))
  {
    printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
    return IPMAP_RTR_INTF_NO_ENABLE;
  }

  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
  {
    /* Either a Manual address or DHCP Should be configured */
    if (((pCfg->addrs[0].ipAddr == 0) || (pCfg->addrs[0].ipMask == 0)) &&
        ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) == 0))
    {
      printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
      return IPMAP_RTR_INTF_NO_ENABLE;
    }
    else
    {
      printf("%s(%u): IPMAP_RTR_INTF_ENABLE_HOST\n", __FUNCTION__, __LINE__);
      return IPMAP_RTR_INTF_ENABLE_HOST;
    }
  }
  else
  {
    if (((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) == 0) &&
       ((pCfg->addrs[0].ipAddr == 0) || (pCfg->addrs[0].ipMask == 0)))
    {
      printf("%s(%u): IPMAP_RTR_INTF_NO_ENABLE\n", __FUNCTION__, __LINE__);
      return IPMAP_RTR_INTF_NO_ENABLE;
    }
  }

  printf("%s(%u): IPMAP_RTR_INTF_ENABLE_ROUTING\n", __FUNCTION__, __LINE__);
  return IPMAP_RTR_INTF_ENABLE_ROUTING;
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
L7_RC_t ipMapRtrIntfMcastFwdModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{

  if (dtlIpv4MulticastIntfForwardModeSet(intIfNum, mode) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Computes interface for the ARP entry configured.
*
* @param    cfg        @b{(input)} ARP entry configured
* @param    intIfNum   @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  If valid interface could be found
* @returns  L7_FAILURE  Otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpIntfGet(L7_rtrStaticArpCfgData_t *cfg, L7_uint32 *intIfNum)
{
  nimConfigID_t nullIntfConfigId;
  L7_uint32 cfgIntIfNum = L7_INVALID_INTF;
  L7_RC_t rc = L7_SUCCESS;

  memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));

  if (NIM_CONFIG_ID_IS_EQUAL(&cfg->intfConfigId, &nullIntfConfigId) == L7_TRUE)
  {
    rc = ipMapRouterIfResolve(cfg->ipAddr, &cfgIntIfNum);
  }
  else
  {
    rc = nimIntIfFromConfigIDGet(&cfg->intfConfigId, &cfgIntIfNum);
  }

  if (rc == L7_SUCCESS)
  {
    *intIfNum = cfgIntIfNum;
  }

  return rc;
}

/*********************************************************************
* @purpose  Check all static ARP entries to see if any are enabled
*           when a given interface is enabled. If so, enable them.
*
* @param    intIfNum   @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfStaticArpEnable(L7_uint32 intIfNum)
{
  L7_IP_ADDR_t ipAddress;
  L7_IP_MASK_t subnetMask;
  L7_uint32 i;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_BOOL isUnnumbered;
  L7_uint32 configIntIfNum;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) != L7_TRUE)
    return L7_SUCCESS;

  isUnnumbered = ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0);
  /* Get primary IP address on interface and check each static ARP entry
   * against it. */
  ipAddress = pCfg->addrs[0].ipAddr;
  subnetMask = pCfg->addrs[0].ipMask;
  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    /* Get interface for the configure entry */
    if (ipMapStaticArpIntfGet(&arp->rtrStaticArpCfgData[i], &configIntIfNum) != L7_SUCCESS)
    {
      continue;
    }

    if (configIntIfNum != intIfNum)
    {
      continue;
    }

    /* Add ARP entry if unnumbered interface or numbered and IP Address is in
     * subnet configured */
    if (isUnnumbered || (!isUnnumbered && ((ipAddress & subnetMask) ==
        (arp->rtrStaticArpCfgData[i].ipAddr & subnetMask))))
    {
      ipMapArpStaticEntryAdd(arp->rtrStaticArpCfgData[i].ipAddr, intIfNum,
              arp->rtrStaticArpCfgData[i].macAddr.addr.enetAddr.addr);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Tell driver that IPv4 routing is enabled on interface.
*
* @param    intIfNum        Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Also registers for IP multicast addresses on this interface.
*
* @end
*********************************************************************/
L7_RC_t ipMapDtlIpIntfSet(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_IP_ADDR_t ipAddress;
  L7_IP_MASK_t subnetMask;
  L7_rtrCfgCkt_t *pCfg = NULL;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  ipAddress = pCfg->addrs[0].ipAddr;
  subnetMask = pCfg->addrs[0].ipMask;

  if (ipstkIntfChangeDtlNotify(intIfNum, L7_ENABLE,
                               L7_IP_MAP_COMPONENT_ID) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) == 0)
  {
    /* numbered interface */
    if ((ipAddress != 0) && (subnetMask != 0))
    {
      if (dtlIpv4IntfIPAddrSet(intIfNum, ipAddress, subnetMask) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];   
        L7_char8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_char8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiInetNtoa(ipAddress, ipAddrStr);
        osapiInetNtoa(subnetMask, ipMaskStr);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Driver failed to set IP address %s/%s on interface %s.", 
                ipAddrStr, ipMaskStr, ifName);
        return L7_FAILURE;  
      }
    }
  }

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_MULTICAST) == L7_TRUE)
  {
    /* Register for IPv4 multicast addresses on this interface. */
    if (dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_ALL_HOSTS_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to register interface %s for ALL HOSTS IP multicast address.",
              ifName);
      return L7_FAILURE;
    }

    if ((pCfg->flags & L7_RTR_INTF_HOST) == 0)
    {
      if (dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_ALL_RTRS_ADDR) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];   
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Failed to register interface %s for ALL ROUTERS IP multicast address.", 
                ifName);
        return L7_FAILURE;
      }
    }
    if (dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_LTD_BCAST_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to register interface %s for limited broadcast address.",
              ifName);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete an IP address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Removing the primary IPv4 address on an interface disables
*           the interface. Clients will receive an interface down
*           router event sequence.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressRemoveApply(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 cfgIndex = 0;
  L7_uint32 gwAddr = 0;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Remove any static ARP entries whose target IP is in the subnet being
   * removed. Have to do this before we zero the address in the config. */
  ipMapIntfStaticArpDelete(intIfNum);

  /* Remove local route. */
  (void) rtoRouteUnReserve();
  ipMapSubnetLocalRouteRemove(pCfg->addrs[0].ipAddr, pCfg->addrs[0].ipMask);

  /* Update the config */
  pCfg->addrs[0].ipAddr = 0;
  pCfg->addrs[0].ipMask = 0;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  /* If interface is up, it will go down w/o a primary IP addr. */
  if (ipMapIntfIsUp(intIfNum))
  {
      ipMapIntfUpdate(intIfNum, NULL);
  }

  /* TODO: No sure if this is the best place to do this.
   */
  /* The default gateway route learned via DHCP is removed now and we may end in
   * a situation where in there will be no default routes even if we have other
   * interfaces enabled for DHCP.
   * So, Check if any other interface is enabled for DHCP and if so get the
   * default route information from DHCP Client and update RTO.
   */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    if (cfgIndex == intIfNum)
    {
      continue;
    }
    if (ipMapMapIntfIsConfigurable(cfgIndex, &pCfg) == L7_TRUE)
    {
      if (((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0) &&
          (pCfg->addrs[0].ipAddr != 0))
      {
        if (rtoIsDhcpDefGatewayPresent (&gwAddr) != L7_TRUE)
        {
          L7_uint32 defGwAddr = 0;
          L7_uint32 rtrIfNum = 0;

          if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
          {
            if (dhcpClientRtrIntfDefaultGatewayGet (rtrIfNum, L7_MGMT_IPPORT,
                                                    &defGwAddr)
                                                 == L7_SUCCESS)
            {
              L7_routeEntry_t routeEntry;

              memset (&routeEntry, 0, sizeof(L7_routeEntry_t));
              routeEntry.ipAddr = 0;
              routeEntry.subnetMask = 0;
              routeEntry.protocol = RTO_DEFAULT;
              routeEntry.pref = _ipMapRouterPreferenceGet(ROUTE_PREF_DEFAULT_ROUTE_DHCP);
              routeEntry.metric = FD_RTR_ROUTE_DEFAULT_COST;
              routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = defGwAddr;
              routeEntry.ecmpRoutes.numOfRoutes = 1;
              rtoRouteAdd (&routeEntry); /* Update in the RTO */
              ipstkDefGwUpdate(); /* Update in the IP Stack */
              break;
            }
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Apply a change to the interface bandwidth
 *
 * @param    intIfNum        Internal Interface Number
 * @param    bandwidth       bandwidth
 *
 * @returns  L7_SUCCESS  If OSPF component exists, its return value
 * @returns  L7_FAILUR   If OSPF component exists, its return valueE
 *                       else L7_SUCCESS
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapIntfBandwidthApply(L7_uint32 intIfNum, L7_uint32 bandwidth)
{
  /* Tell OSPFv2 that interface bandwidth has changed.
   * OSPFv2 has to come back and ask for bandwidth. */
  return ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_BW_CHANGE, L7_FALSE, L7_NULL);
}

/*********************************************************************
* @purpose  Add a static arp entry
*
* @param    ipAddress  ip address of the arp entry
* @param    intIfNum   Internal interface number of the ARP entry
* @param    *pLLAddr   Pointer to L7_linkLayerAddr_t structure
*
* @returns  L7_SUCCESS if ARP entry successfully activated
* @returns  L7_NOT_EXIST if there is not yet a router interface in the
*                        same subnet as the target.
* @returns  L7_FAILURE if the target IP address is the same as the
*                      the system IP address or the IP address on the
*                      service port. Also if the IP address is an IP
*                      address on a local interface or the net directed
*                      broadcast address of a local interface.
*                      Also if interface specified is not valid.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpAddApply(L7_IP_ADDR_t ipAddress, L7_uint32 intIfNum,
    L7_linkLayerAddr_t *pLLAddr)
{
    L7_IP_ADDR_t        systemIPAddr, servPortIPAddr;
    L7_RC_t             rc = L7_FAILURE;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_uint32 rtrIfNum;
  L7_uint32 netIntIfNum = L7_INVALID_INTF;
  L7_uint32 j;
  L7_uint32 localIpAddr = 0;
  L7_uint32 localIpMask = 0;
  L7_BOOL   isUnnumbered;

    systemIPAddr =  simGetSystemIPAddr();
    servPortIPAddr = simGetServPortIPAddr();

    if ((ipAddress == systemIPAddr) || (ipAddress == servPortIPAddr))
    {
        return rc;
    }

  /* Find local IP address and mask in same subnet as target IP address. */
  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
    {
      netIntIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      if (ipMapMapIntfIsConfigurable(netIntIfNum, &pCfg))
      {
        for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
        {
          if (pCfg->addrs[j].ipAddr && pCfg->addrs[j].ipMask)
          {
            if ((pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) ==
                (ipAddress & pCfg->addrs[j].ipMask))
            {
              localIpAddr = pCfg->addrs[j].ipAddr;
              localIpMask = pCfg->addrs[j].ipMask;
              break;
            }
          }
        }
      }
    }
    if (localIpAddr && localIpMask)
      break;
  }

  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);

    /*
    If we did not find a local interface in the same subnet as the
    target, return an indication of this. This will allow the ARP
    entry to remain configured. When an address in this subnet is
    configured, then the ARP entry will be activated.

  The check however is not valid for unnumbered interfaces
    */

  if (isUnnumbered == L7_TRUE)
  {
    if (ipMapIntfIsUp(intIfNum) != L7_TRUE)
    {
          return L7_NOT_EXIST;
    }
  }
  else
  {
      if ((!localIpAddr || !localIpMask))
      {
          return L7_NOT_EXIST;
      }
  }

  /* Can't configure a static ARP entry whose target IP address is
   * a local interface address. */
    if (localIpAddr == ipAddress)
    {
        return rc;
    }

  /* Target IP address cannot be the network directed broadcast address. */
    if (localIpMask != INADDR_BROADCAST && isUnnumbered)
    {
        if ((ipAddress & (~localIpMask)) == (~localIpMask))
        {
            return rc;
        }
    }

  /* Sanity check for numbered interfaces */
  if (!isUnnumbered)
  {
    if (intIfNum == L7_INVALID_INTF)
    {
      /* Interface not specified, set it to interface found above */
      intIfNum = netIntIfNum;
    }
    else if (intIfNum != netIntIfNum)
    {
      /* If ARP entry configured on numbered interface, the IP address must not
       * belong to subnet of another interface */
      return rc;
    }
  }

    rc = ipMapArpStaticEntryAdd(ipAddress, intIfNum,
      (L7_uchar8 *)pLLAddr->addr.enetAddr.addr);

    return rc;
}


/*********************************************************************
* @purpose  Delete a static arp entry
*
* @param    ipAddress  ip address of the arp entry
* @param    intIfNum   internal interface number of entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpDeleteApply(L7_IP_ADDR_t ipAddress, L7_uint32 intIfNum)
{
    L7_IP_ADDR_t        systemIPAddr, servPortIPAddr;
    L7_RC_t             rc;
    L7_uint32           j;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_uint32 localIpAddr = 0;
  L7_uint32 localIpMask = 0;
  L7_BOOL   isUnnumbered;

    rc = L7_FAILURE;

    systemIPAddr =  simGetSystemIPAddr();
    servPortIPAddr = simGetServPortIPAddr();

    if ((ipAddress == systemIPAddr) || (ipAddress == servPortIPAddr))
    {
        return rc;
    }

  /* Find IP address and mask configured on the interface */
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }
  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);

  for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr && pCfg->addrs[j].ipMask)
    {
      if ((pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) ==
          (ipAddress & pCfg->addrs[j].ipMask))
      {
        localIpAddr = pCfg->addrs[j].ipAddr;
        localIpMask = pCfg->addrs[j].ipMask;
        break;
      }
    }
  }

    if ((!localIpAddr || !localIpMask) && !isUnnumbered)
    {
        return rc;
    }

    if (localIpAddr == ipAddress)
    {
        return rc;
    }

  if (localIpMask != INADDR_BROADCAST && !isUnnumbered)
    {
        if ((ipAddress & (~localIpMask)) == (~localIpMask))
        {
            return rc;
        }
    }

    rc = ipMapArpStaticEntryDel(ipAddress, intIfNum);

    return rc;
}

/*********************************************************************
* @purpose  Tell hardware about martian addresses.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     Martian addresses are host or network addresses which are
*            illegal destinations. Packets with a martian destination address
*            should not be forwarded.
*
*            Some hardware platforms have predefined rules to drop packets
*            sent to a martian address. Others do not. Those that do will
*            ignore the addition of these routes. Platforms that don't have
*            predefined rules to ignore martians can install a discard route
*            covering martian prefixes. Since martians are not added to
*            RTO, their installation in the hardware routing table will
*            cause a difference in the number of routes in the hardware table
*            and in RTO.
*
*            The only two martian addresses configured now are:
*
*                127.0.0.0/8 (loopback)
*                240.0.0.0/4 (class E)
*
*            In the future we may make the list of martian addresses
*            configurable.
*
* @end
*********************************************************************/
void ipMapMartianAddrsInstall(void)
{
  L7_routeEntry_t routeEntry;

  static const char *routine_name = "ipMapMartianAddrsInstall()";
  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  bzero((L7_char8 *)&routeEntry, sizeof(L7_routeEntry_t));
  /* Flag tells the hardware to discard matching packets */
  routeEntry.flags |=  L7_RTF_DISCARD;

  routeEntry.subnetMask  = L7_IP_LOOPBACK_ADDR_MASK;
  routeEntry.ipAddr =  L7_IP_LOOPBACK_ADDR_NETWORK;
  ipMapRouteEntryAdd(&routeEntry);

  routeEntry.subnetMask  = L7_CLASS_E_ADDR_MASK;
  routeEntry.ipAddr =  L7_CLASS_E_ADDR_NETWORK;
  ipMapRouteEntryAdd(&routeEntry);

  return;
}

/*********************************************************************
* @purpose  Tell hardware to remove martian addresses.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     Called when routing is disabled.
*
* @end
*********************************************************************/
void ipMapMartianAddrsRemove(void)
{
  L7_routeEntry_t routeEntry;

  static const char *routine_name = "ipMapMartianAddrsRemove()";
  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  bzero((L7_char8 *)&routeEntry, sizeof(L7_routeEntry_t));
  routeEntry.flags |=  L7_RTF_DISCARD;

  routeEntry.subnetMask  = L7_IP_LOOPBACK_ADDR_MASK;
  routeEntry.ipAddr =  L7_IP_LOOPBACK_ADDR_NETWORK;
  ipMapRouteEntryDelete(&routeEntry);

  routeEntry.subnetMask  = L7_CLASS_E_ADDR_MASK;
  routeEntry.ipAddr =  L7_CLASS_E_ADDR_NETWORK;
  ipMapRouteEntryDelete(&routeEntry);

  return;
}


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
* @notes    This is an implementation function only to be used by
*           IP MAP itself. Other components must go through the API
*           ipMapIntIfNumToRtrIntf().
*
*
* @end
*********************************************************************/
L7_RC_t _ipMapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32 *rtrIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if ((rc = nimCheckIfNumber(intIfNum)) == L7_SUCCESS)
  {
    *rtrIfNum = intIfNumToRtrIntf[intIfNum];
    if (*rtrIfNum > 0)
    {
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_ERROR;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Return Router Preference
*
* @param    index   router protocol type
*
* @returns  The router preference associated with the specified
*           protocol
*
* @notes    This is an implementation function only to be used by
*           IP MAP itself. Other components must go through the API
*           ipMapRouterPreferenceGet().
*
*
* @end
*********************************************************************/
L7_uint32 _ipMapRouterPreferenceGet(L7_uint32 index)
{
  L7_uint32 pref;

  pref = ipMapCfg->ip.route_preferences[index];

  return pref;
}

/*********************************************************************
* @purpose  Determine if this router interface number is valid
*
* @param    rtrIfNum       router interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapCheckIfNumber(L7_uint32 rtrIfNum)
{
  if (rtrIntfMap[rtrIfNum].present == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Sets Ip default Time To Live
*
* @param    void
*
* @returns  L7_uint32  arp time to live
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpDefaultTTLApply(L7_uint32 ttl)
{
  /* Not supported for standard LVL7 operation */
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Notify registered routing protocols when the interface is
* @purpose  is enabled for routing
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    event    @b{(input)} event, as listed in L7_RTR_EVENT_CHANGE_t
* @param    isAsync  @b({input}) whether clients respond asynchronously
* @param    pData    @b{(input)} event specific data, if any, to propagate with notification
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t ipMapRoutingEventChangeNotify(L7_uint32 intIfNum, L7_uint32 event,
                                      L7_BOOL isAsync, void *pData)
{
  L7_uint32 i;
  /* intentionally not initializing since this type is opaque to this app,
   * and app really has no idea how to initialize it. Initialization really
   * done through asyncEventCorrelatorCreate() below. */
  ASYNC_CORRELATOR_t correlator;
  ASYNC_EVENT_NOTIFY_INFO_t asyncNotify;
  ipAsyncNotifyData_t *pCorrelatorData;

  if ((ipMapTraceFlags & IPMAP_TRACE_ROUTER_EVENTS) &&
      (!ipMapTraceIntf || !intIfNum || (ipMapTraceIntf == intIfNum)))
  {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "IP MAP sending router event %s",
                    ipMapRouterEventNames[event]);
      if (intIfNum)
      {
        L7_uchar8 ifTraceBuf[IPMAP_TRACE_LEN_MAX];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiSnprintf(ifTraceBuf, IPMAP_TRACE_LEN_MAX, " for interface %s.", ifName);
        osapiStrncat(traceBuf, ifTraceBuf, IPMAP_TRACE_LEN_MAX);
      }
      ipMapTraceWrite(traceBuf);
  }

  memset((void *)&asyncNotify,0,sizeof( ASYNC_EVENT_NOTIFY_INFO_t));

  if (isAsync)
  {
      /* Create a correlator and populate associated event data */
      if (asyncEventCorrelatorCreate(pIpMapInfo->eventHandlerId, &correlator) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 ifString[64];
        ifString[0] = '\0';
        if (intIfNum)
        {
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(ifString, 64, " on interface %s.", ifName);
        }
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "IP MAP failed to create async event correlator for event %s%s.",
                ipMapRouterEventNames[event], ifString);
        return L7_FAILURE;
      }

      if (bufferPoolAllocate (ipMapAsyncNotifyPoolIdGet(),
                              (L7_uchar8**) &pCorrelatorData) == L7_SUCCESS)
      {
          /* correlator data gets tied to correlator and returned in complete notify */
          pCorrelatorData->eventType             = IPMAP_ASYNC_NOTIFY_TYPE_RTR;
          pCorrelatorData->type.rtrInfo.intIfNum = intIfNum;
          pCorrelatorData->type.rtrInfo.event    = event;
          pCorrelatorData->type.rtrInfo.pData    = pData;
          (void) asyncEventCorrelatorDataSet(pIpMapInfo->eventHandlerId, correlator,
                                             pCorrelatorData);
      }
      else
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          IPMAP_ERROR("Failure to get notification buffer for intf %d, %s, event %d\n",
                      intIfNum, ifName, event);
          return L7_FAILURE;
      }

      /* Start the event */
      (void) osapiSemaTake(pIpMapInfo->eventHandler.registered_rwlock, L7_WAIT_FOREVER);     
      if (asyncEventSyncStart(pIpMapInfo->eventHandlerId, correlator,
                              pCorrelatorData) != L7_SUCCESS)
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                  "Failure to send IPv4 router event for interface %s, event %d.",
                      ifName, event);
          osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);
          return L7_FAILURE;
      }

      asyncNotify.correlator = correlator;
      asyncNotify.handlerId  = pIpMapInfo->eventHandlerId;
  }

  for (i = 1; i < L7_LAST_ROUTER_PROTOCOL; i++)
  {
    if (pIpIntfStateNotifyList[i].rtrProtoId > 0)
    {
      (void) (*pIpIntfStateNotifyList[i].notify_routing_event)(intIfNum, event,
                                                             pData, &asyncNotify);
    }
  }

  /* Allow multiple async events to be in progress simultaneously. */
  if (isAsync)
    osapiSemaGive(pIpMapInfo->eventHandler.registered_rwlock);

  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Status callback from components for EVENT Notifications
*
* @param    pComplete       @b{(input)}  Status from the component
* @param    pRemainingMask  @b{(input)}  unused
* @param    pFailingMask    @b{(input)}  unused
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapAsyncEventCompleteNotify(ASYNC_EVENT_COMPLETE_INFO_t *pComplete,
                                   COMPONENT_MASK_t *pRemainingMask,
                                   COMPONENT_MASK_t *pFailingMask)
{
    ipAsyncNotifyData_t *pEventData;
    L7_uint32 j;

    /*----------------------------------------------------*/
    /* Validity check the parameters                      */
    /*----------------------------------------------------*/

    if ( (pComplete == L7_NULLPTR) ||
         (pRemainingMask == L7_NULLPTR) ||
         (pFailingMask   == L7_NULLPTR) )
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "NULL parameter to %s.", __FUNCTION__);
        return;
    }

    /* now legal for pComplete->correlator to be 0. */
    if (pComplete->handlerId == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "NULL handlerId in %s.", __FUNCTION__);
        return;
    }

    if ( (asyncEventCorrelatorDataGet(pComplete->handlerId, pComplete->correlator,
                                       (void**) &pEventData) != L7_SUCCESS) ||
         (pEventData == L7_NULLPTR) )
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "No correlator data available.");
        return;
    }

    if ((ipMapTraceFlags & IPMAP_TRACE_ROUTER_EVENTS) &&
        (!ipMapTraceIntf || !pEventData->type.rtrInfo.intIfNum ||
         (ipMapTraceIntf == pEventData->type.rtrInfo.intIfNum)))
    {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 ifString[64];
        ifString[0] = '\0';
        if (pEventData->type.rtrInfo.intIfNum)
        {
          nimGetIntfName(pEventData->type.rtrInfo.intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(ifString, 64, " on interface %s", ifName);
        }
        osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "IP MAP notified of completion of event %s%s.",
                      ipMapRouterEventNames[pEventData->type.rtrInfo.event], ifString);
        ipMapTraceWrite(traceBuf);
    }

    /*----------------------------------------------------*/
    /* Complete the asynchronous response                 */
    /*----------------------------------------------------*/

    if (pComplete->async_rc.rc != L7_SUCCESS)
    {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 ifString[64];
        ifString[0] = '\0';
        if (pEventData->type.rtrInfo.intIfNum)
        {
            nimGetIntfName(pEventData->type.rtrInfo.intIfNum, L7_SYSNAME, ifName);
            osapiSnprintf(ifString, 64, " on interface %s", ifName);
        }
        if (pComplete->async_rc.reason == ASYNC_EVENT_REASON_RC_TIMEOUT)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
                  "Router event timeout for event %s%s.",
                  ipMapRouterEventNames[pEventData->type.rtrInfo.event], ifString);

          /* Report who didn't respond. */
          for (j = 1; j < L7_LAST_ROUTER_PROTOCOL; j++)
          {
            if (COMPONENT_ISMASKBITSET((*pRemainingMask), j) != 0)
            {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
                      "Async event client %s did not respond.", ipMapEventClientNames[j]);
            }
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
                  "Event failure reason %d for event %s%s",
                  pComplete->async_rc.reason,
                  ipMapRouterEventNames[pEventData->type.rtrInfo.event], ifString);

          /* Report who failed */
        }
        for (j = 1; j < L7_LAST_ROUTER_PROTOCOL; j++)
        {
            if (COMPONENT_ISMASKBITSET((*pFailingMask), j) != 0)
            {
                L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
                        "Async event client %s reported failure.", ipMapEventClientNames[j]);
            }
        }
    }

  /*---------------------------------------------------------*/
  /* Send message to IP Processing thread about completion   */
  /*---------------------------------------------------------*/
  ipMapAsyncEventCompleteQueue(pEventData);

  /* Free the notify structure */
  bufferPoolFree( ipMapAsyncNotifyPoolIdGet(),(L7_uchar8 *) pEventData);
   /* Delete the event only after the response has been completely gotten */
  asyncEventSyncFinish(pComplete->handlerId, pComplete->correlator);
}



/*********************************************************************
* @purpose  Put async completion events on the queue to be handled by IP
*
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAsyncEventCompleteQueue( ipAsyncNotifyData_t *pEventData)
{
    ipMapMsg_t   Message;
    L7_RC_t rc = L7_SUCCESS;

    memset( (void *)&Message, 0, sizeof(ipMapMsg_t) );
    Message.msgId = IPMAP_ASYNC_EVENT_COMPLETE;
    memcpy(&(Message.type.asyncEvent), pEventData, sizeof(ipAsyncNotifyData_t) );

    rc = osapiMessageSend(ipMapProcess_Queue, &Message, sizeof(ipMapMsg_t),
                          L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);

    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                "ipMapAsyncEventCompleteQueue: osapiMessageSend failed\n");
    }

    return;
}

/*********************************************************************
* @purpose  Initializes the ip forwarding stack
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpForwardingStackInit()
{
  bzero((L7_char8*)&L7_ipstat, sizeof(struct L7_ipstat));
  bzero((L7_char8*)&L7_icmpstat, sizeof(struct L7_icmpstat));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Calculates the CheckSum of the IP Header
*
* @param    ip_data @b{(input)} pointer to the start of the IP header
* @param    hlen L7_uint32 the length of the IP header
* @param    *cksum      L7_ushort16 Checksum Value
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Even though the prototype for this is in l7_ip_api.h, the
*           implementation is not in ip_api.c, since this is more of a
*           utility function. It doesn't touch any IP MAP configuration
*           or state data, so no need to take the IP MAP read/write lock.
*           Only used by ipmRouterIfBufSend().
*
* @end
*********************************************************************/
L7_RC_t ipMapIpHeaderCksumCalculate(void *ip_data, L7_uint32 hlen, L7_ushort16 *cksum)
{
  L7_uint32 sum = 0;
  L7_ushort16 *ip;

  ip = (L7_ushort16 *)ip_data;

  if(hlen < sizeof(L7_ipHeader_t))
      return L7_FAILURE;

  while (hlen > 1) {
      sum += *ip++;
      if(sum & 0x80000000)
          sum = (sum & 0xffff) + (sum >> 16);
      hlen -=2;
  }

  if(hlen)
      sum += *ip;

  while (sum >> 16) {
      sum = (sum & 0xffff) + (sum >> 16);
  }
  *cksum = ~sum;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine which message queue an incoming IP packet goes into.
*
* @param    protocol    @b{(input)}  protocol ID from IP header
* @param    sourcePort  @b{(input)}  if TCP or UDP, the source port number
* @param    sourcePort  @b{(input)}  if TCP or UDP, the destination port number
*
* @returns  queue priority
*
* @notes    Should follow same logic as IP6MAP. Give priority to local
*           packets.
*
* @end
*********************************************************************/
e_ForwardQueue ipMapQueuePriority(L7_uchar8 protocol,
                                  L7_ushort16 sourcePort, L7_ushort16 destPort)
{
  if (protocol == IP_PROT_OSPFIGP)
    return L7_L3_FWDHIGHPRI_QUEUE;

  if ((protocol == IP_PROT_IGMP) ||
      (protocol == IP_PROT_PIM) ||
      (protocol == IP_PROT_VRRP))
    return L7_L3_FWDPRI_QUEUE;

  if ((protocol == IP_PROT_UDP) ||
      (protocol == IP_PROT_TCP))
  {
    if ((destPort == UDP_PORT_DHCP_SERV) ||
        (destPort == UDP_PORT_DHCP_CLNT) ||
        (destPort == TCP_PORT_BGP))
      return L7_L3_FWDPRI_QUEUE;

    if  ((sourcePort == UDP_PORT_DHCP_SERV) ||
         (sourcePort == UDP_PORT_DHCP_CLNT) ||
         (sourcePort == TCP_PORT_BGP))
      return L7_L3_FWDPRI_QUEUE;

  }
  return L7_L3_FWD_QUEUE;
}


/*********************************************************************
* @purpose  Removes from the ARP cache all static ARP entries whose target
*           IP address is in the same subnet as the pimary IP address on
*           the interface. Does not consider secondary IP addresses on
*           the interface. If the interface is unnumbered, remove any ARP
*           cache entries associated with the unnumbered interface.
*
* @param    intIfNum        Interface number of the interface
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapIntfStaticArpDelete(L7_uint32 intIfNum)
{
  L7_uint32 i;
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 cfgIntIfNum;
  L7_BOOL   isUnnumbered;
  nimConfigID_t intfConfigId;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return;

  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);
  memset(&intfConfigId, 0, sizeof(intfConfigId));

  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr == 0)
    {
      continue;
    }

    /* Process ARP entries with explicit inteface configured */
    if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId,
          &intfConfigId) == L7_FALSE )
    {
      if (nimIntIfFromConfigIDGet(&arp->rtrStaticArpCfgData[i].intfConfigId,
            &cfgIntIfNum) != L7_SUCCESS)
      {
        continue;
      }

      if (cfgIntIfNum != intIfNum)
      {
        continue;
      }
      /* Interface matches, delete the entry */
      ipMapArpStaticEntryDel(arp->rtrStaticArpCfgData[i].ipAddr, intIfNum);
      continue;
    }

    /* ARP entry without interface. This is only possible for numbered
     * interfaces. See if IP address is in subnet and intf is numbered */
    if (((arp->rtrStaticArpCfgData[i].ipAddr & pCfg->addrs[0].ipMask) ==
        (pCfg->addrs[0].ipAddr & pCfg->addrs[0].ipMask)) && !isUnnumbered)
    {
      ipMapArpStaticEntryDel(arp->rtrStaticArpCfgData[i].ipAddr, intIfNum);
    }
  }
}

/*********************************************************************
*
* @purpose  Determine whether an interface is in the attached state.
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_TRUE if the interface is attached.
*           L7_FALSE otherwise.
*
* @notes    The interface is considered to be attached if the state is
*           either L7_INTF_ATTACHING or L7_INTF_ATTACHED.
*
* @end
*********************************************************************/
L7_BOOL ipMapIntfIsAttached(L7_uint32 intIfNum)
{
    L7_NIM_QUERY_DATA_t queryData;

    /* get the interface state */
    queryData.intIfNum = intIfNum;
    queryData.request = L7_NIM_QRY_RQST_STATE;
    if (nimIntfQuery(&queryData) != L7_SUCCESS)
    {
        return L7_FALSE;
    }

    if ((queryData.data.state == L7_INTF_ATTACHING) ||
        (queryData.data.state == L7_INTF_ATTACHED))
        return L7_TRUE;
    else
        return L7_FALSE;
}


/*********************************************************************
* @purpose  Applies the secondary address configuration on an interface
*
* @param    intIfNum  Internal interface number where address is configured
* @param    ipAddr    Secondary IP Address to be applied.
* @param    ipMask    Subnet mask associated with the secondary IP Address
* @param    index     Index into addrs[] array for this secondary
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Can be called as a result of a configuration change or to apply
*           a secondary on a newly enabled routing interface.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddressApply(L7_uint32 intIfNum, L7_uint32 ipAddr,
                                            L7_uint32 ipMask, L7_uint32 index)
{
    L7_rtrCfgCkt_t *pCfg;
    L7_routeEntry_t routeEntry;
    L7_RC_t rc;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    static const char *routine_name = "ipMapRtrIntfSecondaryIpAddressApply()";

    IPMAP_TRACE("%s %d: %s : intf %d, %s\n",
                __FILE__, __LINE__, routine_name, intIfNum, ifName);

    /* Check input values */
    if ((ipAddr == L7_NULL_IP_ADDR) || (ipMask == L7_NULL_IP_MASK))
    {
        return L7_FAILURE;
    }

    if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
        return L7_FAILURE;
    }

  /* Apply only if the IP interface is up */
  if (!ipMapIntfIsUp(intIfNum))
  {
    return L7_SUCCESS;
  }

  /* Add the secondary address to the IP stack. */
  if (ipmRouterIfSecondaryAddrAdd(intIfNum, ipAddr, ipMask, index) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_char8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_char8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(ipAddr, ipAddrStr);
    osapiInetNtoa(ipMask, ipMaskStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to add secondary address %s/%s on interface %s to IP stack.",
            ipAddrStr, ipMaskStr, ifName);
        return L7_FAILURE;
  }

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
  {
    /* Register secondary with ARP */
    if (ipMapArpSecondaryIpAddrRegister(intIfNum, pCfg->addrs[0].ipAddr,
                                        ipAddr, ipMask) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_char8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiInetNtoa(ipAddr, ipAddrStr);
      osapiInetNtoa(ipMask, ipMaskStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to register secondary address %s/%s on interface %s with ARP.",
              ipAddrStr, ipMaskStr, ifName);

      ipmRouterIfSecondaryAddrDelete(intIfNum, ipAddr, ipMask, index);
      return L7_FAILURE;
    }
  }

  /* Add local route for secondary */
  memset(&routeEntry, 0x00, sizeof(L7_routeEntry_t));
  routeEntry.ipAddr      = (ipAddr & ipMask);
  routeEntry.subnetMask  = ipMask;
  routeEntry.protocol    = RTO_LOCAL;
  routeEntry.metric      = FD_RTR_ROUTE_DEFAULT_COST;
  routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = ipAddr;
  routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = intIfNum;
  routeEntry.ecmpRoutes.numOfRoutes = 1;
  rc = rtoRouteAdd(&routeEntry);
  /* Local routes are checkpointed. May already exist if a warm restart. */
  if ((rc != L7_SUCCESS) && ((rc != L7_ALREADY_CONFIGURED) || !pIpMapInfo->warmRestart))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_char8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_char8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(ipAddr, ipAddrStr);
    osapiInetNtoa(ipMask, ipMaskStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to add local route to %s/%s on interface %s.",
            ipAddrStr, ipMaskStr, ifName);

    if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
    {
      ipMapArpSecondaryIpAddrUnregister(intIfNum, pCfg->addrs[0].ipAddr, ipAddr, ipMask);
    }
    ipmRouterIfSecondaryAddrDelete(intIfNum, ipAddr, ipMask, index);
    return L7_FAILURE;
  }

    /* Activate static route/arp entries associated with this subnet */
  ipMapRtrIntfStaticConfigApply(intIfNum, ipAddr, ipMask);

  if ((pCfg->flags & L7_RTR_INTF_GRAT_ARP) != 0 &&
      ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
  {
    ipMapGratArpSend(intIfNum, ipAddr);
  }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the static configuration for the specified subnet
*
* @param    intIfNum @b{(input)} Internal interface number for address/mask
* @param    ipAddr   @b{(input)} IP Address of the subnet to be applied
* @param    ipMask   @b{(input)} Subnet Mask
*
* @returns  none
*
* @notes    This routine applies the static route, static arp and default
*           route configurations associated with the specified subnet.
*           It needs to be invoked when a new address (primary or secondary)
*           is configured on a router interface
*
* @end
*********************************************************************/
void ipMapRtrIntfStaticConfigApply(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                   L7_IP_MASK_t ipMask)
{
    L7_uint32           i, nh;
    L7_routeEntry_t     routeEntry;
    L7_uchar8           ipStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8           maskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32           cfgIntIfNum;
  L7_BOOL             isUnnumbered;
    static const char   *routine_name = "ipMapRtrIntfStaticConfigApply()";
  nimConfigID_t       nullIntfConfigId;

  memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));
  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);
    /* Apply the static arps in the config file to this subnet */
    for (i=0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
    {
    if (arp->rtrStaticArpCfgData[i].ipAddr == 0)
    {
      continue;
    }

    cfgIntIfNum = L7_INVALID_INTF;
    /* Process ARP entries with explicit inteface configured */
    if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId,
          &nullIntfConfigId) == L7_FALSE )
    {
      if (nimIntIfFromConfigIDGet(&arp->rtrStaticArpCfgData[i].intfConfigId,
            &cfgIntIfNum) != L7_SUCCESS)
      {
        continue;
      }
    }

    /*
     * Ignore entry under following conditions,
     * a. Interface specified in arp entry and does not match interface
     * being configured.
     * b. Interface not specified in arp entry and interface configured is
     * unnumbered.
     */

    /* Ignore if interface specified in arp entry but doesnt match interface
     * being configured */
    if (cfgIntIfNum != L7_INVALID_INTF && cfgIntIfNum != intIfNum)
    {
      continue;
    }

    /* Ignore routes without interface if interface being configure is
     * unnumbered.*/
    if (cfgIntIfNum == L7_INVALID_INTF && isUnnumbered)
    {
      continue;
    }

    /* Interface matches, now apply if unnumbered interface or next hop
     * in the subnet being configured */
    if (isUnnumbered ||
        (ipAddr & ipMask) == (arp->rtrStaticArpCfgData[i].ipAddr & ipMask))
        {
      ipMapStaticArpAddApply(arp->rtrStaticArpCfgData[i].ipAddr, intIfNum,
                             &arp->rtrStaticArpCfgData[i].macAddr);
        }
    }

    /* Apply the static routes in the config file to this interface */
    /* This includes the default route */
    for (i=0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            /*
            To activate a configured static route, its next hop address
            cannot be the IP address of the routing interface itself, but
            must belong to the same subnet
            */
          for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
                  (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
            {
              if (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != ipAddr)
              {
          cfgIntIfNum = L7_INVALID_INTF;
          if (NIM_CONFIG_ID_IS_EQUAL(&nullIntfConfigId,
                &route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId)
              == L7_FALSE )
          {
            if (nimIntIfFromConfigIDGet(
                &route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                &cfgIntIfNum) != L7_SUCCESS)
            {
              continue;
            }
          }

          /*
           * Ignore entry under following conditions,
           * a. Interface specified in route and does not match interface
           * being configured.
           * b. Interface not specified in route and interface configured is
           * unnumbered.
           */

          /* Ignore if interface specified in route but doesn't match interface
           * being configured */
          if (cfgIntIfNum != L7_INVALID_INTF && cfgIntIfNum != intIfNum)
          {
            continue;
          }

          /* Ignore routes without interface if interface being configure is
           * unnumbered.*/
          if (cfgIntIfNum == L7_INVALID_INTF && isUnnumbered)
          {
            continue;
          }

          /* Interface matches, now apply if unnumbered interface or next hop
           * in the subnet being configured */
                    if (isUnnumbered || ((ipAddr & ipMask) ==
                        (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr & ipMask)))
                    {
                        ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);

            if (ipMapSrMultipleNextHops(&route->rtrStaticRouteCfgData[i]) == L7_TRUE)
            {
              /* A static route with this network and prefix is already
               * present. So delete it from RTO */
              rtoRouteDelete(&routeEntry);
            }

            if (rtoRouteAdd(&routeEntry) != L7_SUCCESS)
            {
                memset((L7_char8 *)ipStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
                memset((L7_char8 *)maskStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
                osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipAddr, ipStr);
                osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipMask, maskStr);
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                        "%s: ERROR: Failed to configure static route to %s/%s",
                        routine_name, ipStr, maskStr);
            }
            /* ipMapSrEntryCreate() picks up all next hops. So no need to cycle through
             * the rest of the next hops for this static route. */
            break;   /* out of for loop over the next hops */
          }
        }
      }
    }
  }
}

/*********************************************************************
* @purpose  Removes the specified secondary address configuration on
*           an interface
*
* @param    intIfNum  Internal interface number out of which address
*                     is reachable
* @param    ipAddr    Secondary IP Address to be applied
* @param    ipMask    Subnet mask associated with the secondary IP Address
* @param    index     Index to addrs[] array on interface
*
* @returns  L7_SUCCESS  configuration could be successfully applied
* @returns  L7_FAILURE  invalid input params
*
* @notes    This routine must only be called with a **valid** (non-zer0)
*           IP address
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddressRemoveApply(L7_uint32 intIfNum, L7_uint32 ipAddr,
                                                  L7_uint32 ipMask, L7_uint32 index)
{
    L7_routeEntry_t routeEntry;
  L7_rtrCfgCkt_t *pCfg;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

    if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
    {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddressRemoveApply: intIfNum %d\n",
            intIfNum);
        ipMapTraceWrite(traceBuf);
    }

  /* De-activate static routes and static ARP entries on this subnet. */
  ipMapRtrIntfStaticConfigRemoveApply(intIfNum, ipAddr, ipMask);

    /* Remove the secondary address from the IP stack */
    if (ipmRouterIfSecondaryAddrDelete(intIfNum, ipAddr, ipMask, index) != L7_SUCCESS)
    {
        if (ipMapTraceFlags & IPMAP_TRACE_SECONDARY)
        {
            L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
            sprintf(traceBuf, "ipMapRtrIntfSecondaryIpAddressRemoveApply: "
              "Failed to remove address from IP stack.\n");
            ipMapTraceWrite(traceBuf);
        }
        return L7_FAILURE;
    }

    memset(&routeEntry, 0x00, sizeof(L7_routeEntry_t));
    routeEntry.ipAddr      = (ipAddr & ipMask);
    routeEntry.subnetMask  = ipMask;
    routeEntry.protocol    = RTO_LOCAL;
    routeEntry.metric      = FD_RTR_ROUTE_LOCAL_COST;
    routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = ipAddr;
    routeEntry.ecmpRoutes.numOfRoutes = 1;
    rtoRouteDelete(&routeEntry);

  if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
  {
    /* Unregister the address with ARP code */
    if (ipMapArpSecondaryIpAddrUnregister(intIfNum, pCfg->addrs[0].ipAddr,
                                          ipAddr, ipMask) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_char8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_char8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiInetNtoa(ipAddr, ipAddrStr);
      osapiInetNtoa(ipMask, ipMaskStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to deregister secondary address %s/%s on interface %s with ARP.",
              ipAddrStr, ipMaskStr, ifName);
              return L7_FAILURE;
    }
  }

    /* Notify registered users of deletion event. Not asynchronous. */
    ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE,
                                L7_FALSE, L7_NULLPTR);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes the static configuration for the specified subnet
*
* @param    intIfNum @b{(input)} Internal interface number for address/mask
* @param    ipAddr   @b{(input)} IP Address of the subnet to be applied
* @param    ipMask   @b{(input)} Subnet Mask
*
* @returns  none
*
* @notes    This routine applies the static route, static arp and default
*           route configurations associated with the specified subnet.
*           It needs to be invoked when an ip address (primary or secondary)
*           is de-configured on a router interface
*
* @end
*********************************************************************/
void ipMapRtrIntfStaticConfigRemoveApply(L7_uint32 intIfNum,
                                         L7_IP_ADDR_t ipAddr, L7_IP_MASK_t ipMask)
{
  L7_uint32           i;
  L7_uint32           cfgIntIfNum = L7_INVALID_INTF;
  L7_BOOL             isUnnumbered;
  nimConfigID_t       nullIntfConfigId;

  memset(&nullIntfConfigId, 0, sizeof(nimConfigID_t));

  isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);

    /* Remove static ARP entries associated with this interface and subnet. */
    for (i=0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
    {
    if (arp->rtrStaticArpCfgData[i].ipAddr == 0)
    {
      continue;
    }

    /* Process ARP entries with explicit interface configured */
    if ( NIM_CONFIG_ID_IS_EQUAL(&arp->rtrStaticArpCfgData[i].intfConfigId,
          &nullIntfConfigId) == L7_FALSE )
    {
      if (nimIntIfFromConfigIDGet(&arp->rtrStaticArpCfgData[i].intfConfigId,
            &cfgIntIfNum) != L7_SUCCESS)
      {
        continue;
      }

      if (cfgIntIfNum != intIfNum)
      {
        continue;
      }
    }
    /* Entry is to be removed under following conditions,
     * a. If interface is unnumbered.
     * b. If interface is numbered and configured interfaces match and
     *    IP Address of ARP within subnet being configured
     */
    if (isUnnumbered ||
        ((ipAddr & ipMask) == (arp->rtrStaticArpCfgData[i].ipAddr & ipMask)))
        {
            ipMapArpStaticEntryDel(arp->rtrStaticArpCfgData[i].ipAddr, intIfNum);
        }
    }

  ipMapSubnetStaticRoutesRemove(intIfNum, ipAddr, ipMask);
}

/*********************************************************************
* @purpose  Add into RTO any static reject routes
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticRejectRoutesAdd(void)
{
  L7_uint32 i;
  L7_routeEntry_t     routeEntry;
  L7_uchar8           ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8           maskStr[OSAPI_INET_NTOA_BUF_SIZE];

  for (i=0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {
    if (route->rtrStaticRouteCfgData[i].inUse != L7_TRUE)
      continue;

    if (route->rtrStaticRouteCfgData[i].flags & L7_RTF_REJECT)
    {
      ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);

      if (rtoRouteAdd(&routeEntry) != L7_SUCCESS)
      {
        osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipAddr, ipStr);
        osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipMask, maskStr);
        IPMAP_ERROR("Failed to add static reject route to %s/%s to RTO", ipStr, maskStr);
      }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Removes from RTO any static reject routes
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticRejectRoutesRemove(void)
{
  L7_uint32 i;
  L7_routeEntry_t     routeEntry;
  L7_uchar8           ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8           maskStr[OSAPI_INET_NTOA_BUF_SIZE];

  for (i=0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {
    if (route->rtrStaticRouteCfgData[i].inUse != L7_TRUE)
      continue;

    if (route->rtrStaticRouteCfgData[i].flags & L7_RTF_REJECT)
    {
      ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);

      if (rtoRouteDelete(&routeEntry) != L7_SUCCESS)
      {
        osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipAddr, ipStr);
        osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipMask, maskStr);
        IPMAP_ERROR("Failed to delete static reject route to %s/%s from RTO", ipStr, maskStr);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes from RTO any static routes whose next hop is on the
*           specified interface and in the given subnet.
*
* @param    ipAddr   @b{(input)} IP Address of the subnet to be applied
* @param    ipMask   @b{(input)} Subnet Mask
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapSubnetStaticRoutesRemove(L7_uint32 intIfNum,
                                      L7_IP_ADDR_t ipAddr, L7_IP_MASK_t ipMask)
{
  L7_uint32           i, nh;
  L7_routeEntry_t     routeEntry;
  L7_uchar8           ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8           maskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 srIntIfNum;
  L7_uint32 newNhIndex;
  L7_BOOL isUnnumbered = _ipMapIntfIsUnnumbered(intIfNum);
  nimConfigID_t       nullIntfConfigId;

  memset(&nullIntfConfigId, 0, sizeof (nimConfigID_t));

  for (i=0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {
    if (route->rtrStaticRouteCfgData[i].inUse != L7_TRUE)
      continue;

    /* iterate over next hops of this configured static route */
    for (newNhIndex = 0, nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
        (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
    {
      srIntIfNum = L7_INVALID_INTF;
      if (NIM_CONFIG_ID_IS_EQUAL(&nullIntfConfigId,
                                 &route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId) == L7_FALSE )
      {
        if (nimIntIfFromConfigIDGet(&route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                                    &srIntIfNum) != L7_SUCCESS)
        {
          continue;
        }
      }
      if ((srIntIfNum != L7_INVALID_INTF) && (srIntIfNum != intIfNum))
      {
        /* static route tied to a different interface */
        continue;
      }

      /* Delete static route if unnumbered interface or IP Address
       * within subnet being configured */
      if (isUnnumbered ||
          ((ipAddr & ipMask) ==
           (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr & ipMask)))
      {
        /* routeEntry does not include next hop that's gone down. */
        ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);

        if (routeEntry.ecmpRoutes.numOfRoutes == 0)
        {
          /* Only next hop has gone down. Delete route. */
          if (rtoRouteDelete(&routeEntry) != L7_SUCCESS)
          {
            osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipAddr, ipStr);
            osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipMask, maskStr);
            IPMAP_ERROR("Failed to delete static route to %s/%s", ipStr, maskStr);
          }
        }
        else
        {
          if (rtoRouteModify(&routeEntry) != L7_SUCCESS)
          {
            osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipAddr, ipStr);
            osapiInetNtoa((L7_uint32)route->rtrStaticRouteCfgData[i].ipMask, maskStr);
            IPMAP_ERROR("Failed to modify static route to %s/%s", ipStr, maskStr);
          }
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes from RTO any local routes whose next hop is out
*           the given interface.
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapSubnetLocalRouteRemove (L7_uint32 ipAddr, L7_uint32 ipMask)
{
  L7_routeEntry_t routeEntry;
  if (ipAddr && ipMask == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Error calling %s with IPv4 address of 0.", __FUNCTION__);
  }

  /* Remove the local route entry */
  memset(&routeEntry, 0x00, sizeof(L7_routeEntry_t));
  routeEntry.ipAddr      = (ipAddr & ipMask);
  routeEntry.subnetMask  = ipMask;
  routeEntry.protocol    = RTO_LOCAL;
  routeEntry.metric      = FD_RTR_ROUTE_LOCAL_COST;
  routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = ipAddr;
  routeEntry.ecmpRoutes.numOfRoutes = 1;
  /* If local route already gone, don't bark. */
  (void) rtoRouteDelete(&routeEntry);

  /* As the local route is going down, remove the associated default route
   * associated with this interface, if any.
   */
  memset (&routeEntry, 0, sizeof(L7_routeEntry_t));

  while (rtoNextRouteGet (&routeEntry, L7_FALSE) == L7_SUCCESS)
  {
    if ((routeEntry.ipAddr == 0) && (routeEntry.subnetMask == 0) &&
        (routeEntry.protocol == RTO_DEFAULT) &&
        (routeEntry.pref == _ipMapRouterPreferenceGet(ROUTE_PREF_DEFAULT_ROUTE_DHCP)) &&
        ((routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr & ipMask) ==
         (ipAddr & ipMask)))
    {
      rtoRouteDelete(&routeEntry);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the internal interface number of the numbered interface
*           pointed to by an unnnumbered interface
*
* @param    intIfNum   @b{(input)} internal interface number of unnumbered interface
*
* @returns  internal interface number of corresponding numbered interface
*
* @notes    Fails and returns 0 if intIfNum is not unnumbered.
*
* @end
*********************************************************************/
L7_uint32 ipMapNumberedIfc(L7_uint32 intIfNum)
{
  L7_uint32 numberedIfc;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  if ((ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE) ||
      ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) == 0))
      return 0;

  if (nimIntIfFromConfigIDGet(&pCfg->numberedIfc, &numberedIfc) != L7_SUCCESS)
    return 0;

  return numberedIfc;
}

/*********************************************************************
* @purpose  Get the borrowed address and mask for an unnnumbered interface
*
* @param    intIfNum   @b{(input)}  internal interface number of unnumbered interface
*
* @returns  borrowed IPv4 address. 0 if an error occurs.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 _ipMapBorrowedAddr(L7_uint32 intIfNum)
{
  L7_uint32 numbIntIfNum;   /* internal interface number of numbered interface */
  L7_rtrCfgCkt_t *pCfg;     /* config of numbered interface */

  numbIntIfNum = ipMapNumberedIfc(intIfNum);
  if (numbIntIfNum == 0)
    return 0;

  if (!ipMapMapIntfIsConfigurable(numbIntIfNum, &pCfg))
    return 0;

  return pCfg->addrs[0].ipAddr;
}

/*********************************************************************
* @purpose  Configure the association of an unnumbered interface to
*           a numbered interface.
*
* @param    intIfNum   @b{(input)} internal interface number of unnumbered interface
* @param    numberedIfc @b{(input)}  associated numbered interface. 0 to clear
*                                    current association.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    If numberedIfc is 0
*
* @end
*********************************************************************/
L7_RC_t ipMapNumberedIfcBind(L7_uint32 intIfNum, L7_uint32 numberedIfc)
{
  L7_rtrCfgCkt_t *pCfg;       /* interface config for unnumbered interface */

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
    return L7_FAILURE;

  if (numberedIfc == 0)
  {
    memset(&pCfg->numberedIfc, 0, sizeof(nimConfigID_t));
    return L7_SUCCESS;
  }

  /* Convert the internal interface number of the numbered interface to
   * a config ID. */
  return nimConfigIdGet(numberedIfc, &pCfg->numberedIfc);
}

/*********************************************************************
* @purpose  Check an IP address for a proper network number for use
*           on a routing interface.
*
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS      valid IP address
* @returns  L7_FAILURE      IP address not valid for rtr interface use
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressValidityCheck(L7_IP_ADDR_t ipAddress,
                                           L7_IP_MASK_t subnetMask)
{
  /* The following IP address network numbers are permitted for routing
   * interface assignment:
   *
   *   class A:  net 1 - 126 only
   *   class B:  all
   *   class C:  all
   *   class D:  none
   *   class E:  none
   *
   * The above list also covers all subnets of the specified networks.
   *
   * Note that for net 0, it is invalid as a class A address regardless of
   * any subnetting or supernetting.  A non-net-0 supernet (CIDR) address
   * whose network portion yields 0 is permitted however (e.g., 1.1.1.1/7).
   */

  /* an IP address or subnet mask of 0 is invalid */
  if ((ipAddress == L7_NULL_IP_ADDR) || (subnetMask == L7_NULL_IP_MASK))
    return L7_FAILURE;

  /* A host portion of all 0's or all 1's is prohibited if netmask is 30 bits or shorter. */
  if ((subnetMask != 0xFFFFFFFF) && (subnetMask != 0xFFFFFFFE))
  {
    if (((ipAddress & ~subnetMask) == L7_NULL_IP_ADDR) ||
        ((ipAddress & ~subnetMask) == ~subnetMask))
    {
      return L7_FAILURE;
    }
  }

  /* reject the class A net 0 address */
  if (((ipAddress & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == 0)
    return L7_FAILURE;

  /* reject the class A net 127 (loopback) address */
  if (((ipAddress & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET)
    return L7_FAILURE;

  /* accept all other class A */
  if ((L7_BOOL)IN_CLASSA(ipAddress) == L7_TRUE)
    return L7_SUCCESS;

  /* accept all class B */
  if ((L7_BOOL)IN_CLASSB(ipAddress) == L7_TRUE)
    return L7_SUCCESS;

  /* accept all class C */
  if ((L7_BOOL)IN_CLASSC(ipAddress) == L7_TRUE)
    return L7_SUCCESS;

  /* reject everything else (class D, E, etc.) */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Check to see if an IP address conflicts with an IP address on
*           the network or service port.
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_TRUE      conflict
* @returns  L7_FALSE     no conflict
*
* @notes    There is a conflict if the subnets overlap.
*
* @end
*********************************************************************/
L7_BOOL ipMapMgmtPortConflict(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                              L7_IP_MASK_t subnetMask)
{
  L7_uint32 ipAddr, mask;

  /* Compare address to network port address. */
  ipAddr = simGetSystemIPAddr();
  mask = simGetSystemIPNetMask();
  if (ipAddr && mask)
  {
    if ((ipAddr & mask & subnetMask) == (ipAddress & mask & subnetMask))
      return L7_TRUE;
  }

  /* Compare address to service port address */
  ipAddr = simGetServPortIPAddr();
  mask = simGetServPortIPNetMask();
  if (ipAddr && mask)
  {
    if ((ipAddr & mask & subnetMask) == (ipAddress & mask & subnetMask))
      return L7_TRUE;
  }

  return L7_FALSE;
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
* @notes    Implementation function for ipMapRtrIntfAddressConflict() API.
*
* @end
*********************************************************************/
L7_BOOL ipMapRtrIntfAddressConflictFind(L7_uint32 intIfNum,
                                        L7_IP_ADDR_t ipAddress,
                                        L7_IP_MASK_t subnetMask)
{
  L7_uint32 i, j;
  nimConfigID_t configIdNull;

  if ((ipAddress == 0) || (subnetMask == 0))
  {
    /* 0/0 not considered to conflict with any other address */
    return L7_FALSE;
  }

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Iterate through all existing interfaces to determine if this subnet is
   * already configured on another interface. */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipMapCfg->ckt[i].configId, &configIdNull))
    {
      /* this element not in use */
      continue;
    }

    for (j = 0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      /* skip holes in addrs[] array */
      if (ipMapCfg->ckt[i].addrs[j].ipAddr == L7_NULL_IP_ADDR)
      {
        continue;
      }

      if ((subnetMask & ipMapCfg->ckt[i].addrs[j].ipMask & ipAddress) ==
          (subnetMask & ipMapCfg->ckt[i].addrs[j].ipMask & ipMapCfg->ckt[i].addrs[j].ipAddr))
      {
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether a given IP address is the target IP of
*           a static ARP entry.
*
* @param    ipAddress @b{(input)} IP address to be compared
*
* @returns  L7_TRUE if there is a conflict
* @returns  L7_FALSE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipMapStaticArpIpAddressConflictCheck(L7_IP_ADDR_t ipAddress)
{
  L7_uint32 i;

  for (i = 0; i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    if (arp->rtrStaticArpCfgData[i].ipAddr == ipAddress)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether a given IP interface is up for IPv4.
*
* @param    intIfNum @b{(input)} internal interface number
*
* @returns  L7_TRUE if interface is up
* @returns  L7_FALSE otherwise
*
* @notes    An interface is considered up if IP MAP has issued an
*           L7_RTR_INTF_ENABLE for it. The interface is operational and
*           ready for use.
*
* @end
*********************************************************************/
L7_BOOL ipMapIntfIsUp(L7_uint32 intIfNum)
{
  return pIpMapInfo->operIntf[intIfNum].stackEnabled;
}

/*********************************************************************
* @purpose  Determine whether a given IP interface has an IPv4 address in
*           a given subnet.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ipAddr   @b{(input)} IP address
* @param    ipMask   @b{(input)} network mask
*
* @returns  L7_TRUE if interface has an address in given subnet
* @returns  L7_FALSE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipMapIntfAddrInSubnet(L7_uint32 intIfNum, L7_uint32 ipAddr, L7_uint32 ipMask)
{
  L7_rtrCfgCkt_t *pCfg;       /* interface config for unnumbered interface */
  L7_uint32 prefix = ipAddr & ipMask;
  L7_uint32 i;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FALSE;

  for (i = 0; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (pCfg->addrs[i].ipAddr && pCfg->addrs[i].ipMask)
    {
      if ((pCfg->addrs[i].ipAddr & pCfg->addrs[i].ipMask) == prefix)
        return L7_TRUE;
    }
  }
  return L7_FALSE;
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
L7_RC_t _ipMapRouterIfResolve(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;

  /* never match on an IP address of 0.0.0.0 */
  if (ipAddr == 0)
    return L7_FAILURE;

  /* Iterate through all routing interfaces */
  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
    {
      *intIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      if (ipMapMapIntfIsConfigurable(*intIfNum, &pCfg))
      {
        for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
        {
          if (pCfg->addrs[j].ipAddr && pCfg->addrs[j].ipMask)
          {
            if ((pCfg->addrs[j].ipAddr & pCfg->addrs[j].ipMask) ==
                (ipAddr & pCfg->addrs[j].ipMask))
            {
              return L7_SUCCESS;
            }
          }
        }
      }
    }
  }

  *intIfNum = 0;
  return L7_FAILURE;
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
L7_BOOL _ipMapRtrIntfConfigured(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg;
  L7_uint32 numbIntIfNum;

  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
    return L7_FALSE;

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FALSE;
  }

  if ((pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE) == 0)
    return L7_FALSE;

  /* If interface is unnumbered, don't require an IP address */
  if ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0)
  {
    /* Require the numbered interface to be up. */
    numbIntIfNum = ipMapNumberedIfc(intIfNum);
    return ipMapIntfIsUp(numbIntIfNum);
  }

  if ((pCfg->addrs[0].ipAddr == 0) || (pCfg->addrs[0].ipMask == 0))
    return L7_FALSE;

  return L7_TRUE;
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
L7_RC_t _ipMapIpAddressToIntf(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_uint32 j;
  L7_rtrCfgCkt_t *pCfg;

  /* never match on an IP address of 0.0.0.0 */
  if (ipAddr == 0)
    return L7_FAILURE;

  /* Iterate through all routing interfaces */
  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
    {
      *intIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      if (ipMapMapIntfIsConfigurable(*intIfNum, &pCfg))
      {
        for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
        {
          if (pCfg->addrs[j].ipAddr == ipAddr)
          {
            return L7_SUCCESS;
          }
        }
      }
    }
  }

  *intIfNum = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get router interface data for driver when enabling routing
*           interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *ipCircuit @b{(input)} pointer to IP circuit structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface is not valid for IPv4 routing
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfDataGet( L7_uint32 intIfNum, dtlRtrIntfDesc_t *ipCircuitDesc)
{
  L7_uint32 vlanId = 0;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  nimMacroPort_t macroPortIntf;
  L7_uint32 sysIntfType;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    return L7_ERROR;
  }
  /* Build router interface data structure */
  bzero ((char *) ipCircuitDesc, sizeof (dtlRtrIntfDesc_t));

  /* Set up interface information */
  ipCircuitDesc->ipCircuit.intIfNum  = intIfNum;

  /* Get VLAN ID for this routing interface */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_ERROR;

  if (sysIntfType == L7_LOGICAL_VLAN_INTF)
  {
    /* VLAN routing interface */
    if (nimGetMacroPortAssignment(intIfNum, &macroPortIntf) == L7_SUCCESS)
    {
      vlanId = (L7_uint32)macroPortIntf.macroInfo;
    }
    else
    {
      return L7_ERROR;
    }
  }
  else if (sysIntfType == L7_PHYSICAL_INTF)
  {
    /* port based routing interface. Get internal VLAN. */
    vlanId = ipstkInternalVlanIdGet(intIfNum);
  }
  ipCircuitDesc->ipCircuit.vlanId  = (L7_ushort16)vlanId;

  ipCircuitDesc->ipAddress = pCfg->addrs[0].ipAddr;
  ipCircuitDesc->subnetMask = pCfg->addrs[0].ipMask;

  nimEncapsulationTypeGet(intIfNum, &(ipCircuitDesc->llEncapsType));
  nimGetIntfMtuSize(intIfNum, &(ipCircuitDesc->mtu));

  /* Always assume broadcast capable network */
  ipCircuitDesc->flags |= DTL_IP_CIRC_BROADCAST_CAPABLE;

  /* Set up interface flags */
  if (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE)
    ipCircuitDesc->flags |= DTL_IP_CIRC_ROUTING_ENABLED;

  if (pCfg->flags & L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE)
    ipCircuitDesc->flags |= DTL_IP_CIRC_NET_DIR_BCAST_FWD;

  if (pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE)
    ipCircuitDesc->flags |= DTL_IP_CIRC_MCAST_FWD;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Take the IP MAP read/write lock, ipRwLock.
*
* @param    lockType  ({input})  whether to take read lock or write lock
* @param    wait      ({input})  how long to wait before giving up (milliseconds)
* @param    lockHolder({output}) function name of caller. Use for debugging.
*
* @returns  L7_SUCCESS  if lock successfully acquired
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapLockTake(e_ipMapLockType lockType, const L7_int32 wait,
                      const L7_uchar8 *lockHolder)
{
  L7_RC_t rc;
  L7_uint32 startTime = osapiTimeMillisecondsGet();

  if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "%s taking %s lock on task %#x. wait %d.",
            lockHolder, (lockType == IPMAP_READ_LOCK ? "READ" : "WRITE"),
            osapiTaskIdSelf(), wait);
    ipMapTraceWrite(traceBuf);
  }

  /* Make sure IP MAP has created semaphore */
  if (ipMapCnfgrState < IPMAP_PHASE_INIT_1)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "Lock take failed. Bad init state.");
      ipMapTraceWrite(traceBuf);
    }
    return L7_FAILURE;
  }

  if (lockType == IPMAP_READ_LOCK)
  {
    ipMapLockInfo->readTakeAttempts++;
    rc = osapiReadLockTake(ipRwLock, wait);
    if (rc != L7_SUCCESS)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        sprintf(traceBuf, "Read take failed.");
        ipMapTraceWrite(traceBuf);
      }
      ipMapLockInfo->readTakeFailures++;
    }
  }
  else if (lockType == IPMAP_WRITE_LOCK)
  {
    ipMapLockInfo->writeTakeAttempts++;
    rc = osapiWriteLockTake(ipRwLock, wait);
    if (rc != L7_SUCCESS)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        sprintf(traceBuf, "Write take failed.");
        ipMapTraceWrite(traceBuf);
      }
      ipMapLockInfo->writeTakeFailures++;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Bad take lock type %d in %s.",
            lockType, lockHolder);
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    L7_uint32 i;
    L7_uint32 etime = osapiTimeMillisecondsGet() - startTime;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to take IP MAP %s lock in %s() within %d ms. Waited %u ms.",
            (lockType == IPMAP_READ_LOCK ? "READ" : "WRITE"), lockHolder, wait,
            etime);

    if (strlen(ipMapLockInfo->writeLockHolder.fName) != 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "IP MAP write lock held by %s", ipMapLockInfo->writeLockHolder.fName);
    }
    else
    {
      for (i = 0; i < IPMAP_READLOCK_REC_MAX; i++)
      {
        if (strlen(ipMapLockInfo->readLockHolder[i].fName) != 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                  "IP MAP read lock held by %s",
                  ipMapLockInfo->readLockHolder[i].fName);
        }
      }
    }
    return rc;
  }

  if (lockType == IPMAP_READ_LOCK)
  {
    L7_uint32 i;
    for (i = 0; i < IPMAP_READLOCK_REC_MAX; i++)
    {
      if (strlen(ipMapLockInfo->readLockHolder[i].fName) == 0)
      {
        osapiStrncpySafe(ipMapLockInfo->readLockHolder[i].fName, lockHolder,
                         IPMAP_LOCK_HOLDER_NAME_MAX);
        break;
      }
    }
  }
  else if (lockType == IPMAP_WRITE_LOCK)
  {
    osapiStrncpySafe(ipMapLockInfo->writeLockHolder.fName, lockHolder,
                     IPMAP_LOCK_HOLDER_NAME_MAX);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Give the IP MAP read/write lock, ipRwLock.
*
* @param    lockType  ({input})  whether to give read lock or write lock
* @param    lockHolder({output}) function name of caller. Use for debugging.
*
* @returns  L7_SUCCESS  if lock successfully acquired
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapLockGive(e_ipMapLockType lockType, const L7_uchar8 *lockHolder)
{
  L7_RC_t rc = L7_FAILURE;

  if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "%s giving %s lock on task %#x.",
                  lockHolder, (lockType == IPMAP_READ_LOCK ? "READ" : "WRITE"),
                  osapiTaskIdSelf());
    ipMapTraceWrite(traceBuf);
  }

  /* Make sure IP MAP has created semaphore */
  if (ipMapCnfgrState < IPMAP_PHASE_INIT_1)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      sprintf(traceBuf, "Lock give failed. Bad init state.");
      ipMapTraceWrite(traceBuf);
    }
    return L7_FAILURE;
  }

  if (lockType == IPMAP_READ_LOCK)
  {
    ipMapLockInfo->readGiveAttempts++;
    rc = osapiReadLockGive(ipRwLock);
    if (rc != L7_SUCCESS)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        sprintf(traceBuf, "Read give failed.");
        ipMapTraceWrite(traceBuf);
      }
      ipMapLockInfo->readGiveFailures++;
    }
  }
  else if (lockType == IPMAP_WRITE_LOCK)
  {
    ipMapLockInfo->writeGiveAttempts++;
    rc = osapiWriteLockGive(ipRwLock);
    if (rc != L7_SUCCESS)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_LOCK)
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        sprintf(traceBuf, "Write give failed.");
        ipMapTraceWrite(traceBuf);
      }
      ipMapLockInfo->writeGiveFailures++;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Bad give lock type %d in %s.",
            lockType, lockHolder);
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to give IP MAP %s lock in %s()",
            (lockType == IPMAP_READ_LOCK ? "READ" : "WRITE"), lockHolder);
    return rc;
  }

  if (lockType == IPMAP_READ_LOCK)
  {
    L7_uint32 i;
    for (i = 0; i < IPMAP_READLOCK_REC_MAX; i++)
    {
      if (strncmp(ipMapLockInfo->readLockHolder[i].fName, lockHolder,
                  IPMAP_LOCK_HOLDER_NAME_MAX) == 0)
      {
        osapiStrncpySafe(ipMapLockInfo->readLockHolder[i].fName, "",
                         IPMAP_LOCK_HOLDER_NAME_MAX);
        break;
      }
    }
  }
  else /* write lock given */
  {
    osapiStrncpySafe(ipMapLockInfo->writeLockHolder.fName, "", IPMAP_LOCK_HOLDER_NAME_MAX);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  equivalent to ipMapIntfIsUnnumbered, but without locks
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL _ipMapIntfIsUnnumbered(L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */
  L7_BOOL isUnnumbered;

  isUnnumbered = (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) &&
                  ((pCfg->flags & L7_RTR_INTF_UNNUMBERED) != 0));
  return isUnnumbered;
}

/*********************************************************************
* @purpose  Check if the Interface IP Address is leased via DHCP.
*
* @param    intIfNum         Internal Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
ipMapIntfIsAddressMethodDhcp (L7_uint32 intIfNum)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;

  /* validate we have intIfNum to cfgid mapping */
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

