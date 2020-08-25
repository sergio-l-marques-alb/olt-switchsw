/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/common/cli_show_running_config_l3.c
 *
 * @purpose show running config commands for routing
 *
 * @component user interface
 *
 * @comments
 *
 * @create  18/08/2003
 *
 * @author  Samip
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_mib_relay_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_dot1q_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_igmp_api.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_pimsm_api.h"
#endif

#include "comm_mask.h"
#include "config_script_api.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"

#include "usmdb_rtrdisc_api.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_defaultconfig.h"
#include "clicommands_loopback.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif

#include "cli_show_running_config.h"
#include "util_enumstr.h"

/*********************************************************************
 * @purpose  To print the running configuration of arp Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningConfigArpInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 j;
  L7_char8 * pBuf;
  L7_arpEntry_t * staticEntries;
  L7_uint32 intfUnit, intfSlot, intfPort;

  if (usmDbIpArpDynamicRenewGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_IP_DEFAULT_ARP_DYNAMIC_RENEW,pStrInfo_routing_ArpDynrenew);
  }

  if (usmDbIpArpAgeTimeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_IP_DEFAULT_ARP_AGE_TIME,pStrInfo_routing_ArpTimeout);
  }
  if (usmDbIpArpRespTimeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_IP_DEFAULT_ARP_RESP_TIME,pStrInfo_routing_ArpResptime);
  }
  if (usmDbIpArpRetriesGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_IP_DEFAULT_ARP_RETRIES,pStrInfo_routing_ArpRetries);
  }

  if (usmDbIpArpCacheSizeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,platRtrArpMaxEntriesGet(),pStrInfo_routing_ArpCachesize);
  }

  memset(stat, 0, sizeof(stat));
  memset(buf, 0, sizeof(buf));
  staticEntries = (L7_arpEntry_t *) osapiMalloc(L7_CLI_WEB_COMPONENT_ID,
                                                L7_IP_ARP_CACHE_STATIC_MAX * sizeof(L7_arpEntry_t));
  if (usmDbIpMapStaticArpGetAll(unit, staticEntries) == L7_SUCCESS)
  {
    for (i = 0; (i < L7_IP_ARP_CACHE_STATIC_MAX) && (staticEntries[i].ipAddr != 0); i++)
    {
      osapiInetNtoa(staticEntries[i].ipAddr, buf);
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_Arp_3, buf);

      memset(buf, 0, sizeof(buf));
      for (j = 0, pBuf = buf; j < L7_MAC_ADDR_LEN-1; j++, pBuf+=3)
      {
        osapiSnprintf(pBuf, sizeof(buf), "%2.2X:", staticEntries[i].macAddr.addr.enetAddr.addr[j]);
      }
      osapiSnprintf(pBuf, sizeof(buf), "%2.2X", staticEntries[i].macAddr.addr.enetAddr.addr[j]);
      OSAPI_STRNCAT(stat, buf);
      pBuf+=2;

      /* Show interface if configured */
      if (staticEntries[i].intIfNum != L7_INVALID_INTF)
      {
        if (usmDbUnitSlotPortGet(staticEntries[i].intIfNum, &intfUnit,
                                 &intfSlot, &intfPort) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), " %s", 
                        cliGetIntfName(staticEntries[i].intIfNum, intfUnit, intfSlot, intfPort));
          OSAPI_STRNCAT(stat, buf);
        }
      }

      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  osapiFree(L7_CLI_WEB_COMPONENT_ID, staticEntries);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of bootpdhcrelay Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningConfigBootpDhcpRelayInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbBootpDhcpRelayMaxHopCountGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MAXHOPCOUNT,pStrInfo_routing_BootpdhcprelayMaxhopcount);
  }
  if (usmDbBootpDhcpRelayMinWaitTimeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MINWAITTIME,pStrInfo_routing_BootpdhcprelayMinwaittime);
  }

  if (usmDbBootpDhcpRelayCircuitIdOptionModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIRCUITIDOPTION,pStrInfo_routing_BootpdhcprelayCidoptmode);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of udp relay / IP helper
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigUdpRelayInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 intIfNum = IH_INTF_ANY;
  L7_uint32 serverAddr;
  L7_ushort16 udpPort;
  L7_BOOL discard;
  L7_uint32 hitCount;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 serverAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 udpPortStr[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 val;

  /* Global enable/disable of IP helper */
  if (usmDbIpHelperAdminModeGet(&val) == L7_SUCCESS)
  {
    if ((val != FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch (val)
      {
        case L7_DISABLE:
          sprintf(buf, "\r\nno ip helper enable");
          break;
        case L7_ENABLE:
          sprintf(buf, "\r\nip helper enable");
          break;
        default:
          /* Do nothing */
          break;
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  rc = usmDbIpHelperAddressFirst(&intIfNum, &udpPort, &serverAddr, &discard,
                                 &hitCount);
  while (rc == L7_SUCCESS)
  {
    /* per interface entries are listed with the interface. */
    if (intIfNum == IH_INTF_ANY)
    {
      /* Can't configure global discard entries. So serverAddr must be set. */
      usmDbInetNtoa(serverAddr, serverAddrStr);
      if (udpPort == IH_UDP_PORT_DEFAULT)
        udpPortStr[0] = '\0';
      else
        strUtilUdpPortToString(udpPort, udpPortStr);

      osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH,
                    "\r\nip helper-address %s %s", serverAddrStr, udpPortStr);

      EWSWRITEBUFFER(ewsContext, buf);
    }
    rc = usmDbIpHelperAddressNext(&intIfNum, &udpPort, &serverAddr, &discard,
                                  &hitCount);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of internal VLAN configuration
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigInternalVlan(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanId;

  vlanId = usmDbDot1qBaseInternalVlanIdGet();
  if ((vlanId != FD_DOT1Q_BASE_INTERNAL_VLAN) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
  {
    osapiSnprintf(buf, sizeof(buf), "\r\nvlan internal allocation base %u", vlanId);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  if ((usmDbDot1qInternalVlanPolicyGet() != FD_DOT1Q_INTERNAL_VLAN_POLICY) || 
      (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
  {
    osapiSnprintf(buf, sizeof(buf), "\r\nvlan internal allocation policy %s", 
                  (usmDbDot1qInternalVlanPolicyGet() == DOT1Q_INT_VLAN_POLICY_ASCENDING) ? 
                  "ascending" : "descending");
    EWSWRITEBUFFER(ewsContext, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of non interface ip
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigIntfIpRoute(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_uint32 burstsize;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  /*ip non interface Specific */
  if (usmDbIpRtrAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_ADMIN_MODE,pStrInfo_routing_IpRouting_1);
  }

  if(usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_STATIC, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_RTR_DEFAULT_FORWARDING,pStrInfo_routing_IpRouteDistance_1);
  }
  if (usmDbIpMapRtrICMPEchoReplyModeGet (&val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val, FD_RTR_DEFAULT_ICMP_ECHO_REPLY_MODE, pStrInfo_routing_IpIcmpEchoReply_1);
  }
  if (usmDbIpMapRtrICMPRedirectsModeGet (&val) == L7_SUCCESS)
  { 
    cliShowCmdEnable(ewsContext,val, FD_RTR_DEFAULT_ICMP_REDIRECT_MODE, pStrInfo_routing_IpRedirects_1);
  }
  if (usmDbIpMapRtrICMPRatelimitGet (&burstsize, &val) == L7_SUCCESS) 
  {
    if (burstsize != FD_RTR_DEFAULT_ICMP_RATE_LIMIT_SIZE)
    {
      osapiSnprintf(stat, sizeof(stat),"%s %u %u", pStrInfo_routing_IpIcmpErrorInterval_1, val, burstsize);
       EWSWRITEBUFFER (ewsContext, stat);
    }
    else if (val != FD_RTR_DEFAULT_ICMP_RATE_LIMIT_INTERVAL)
    {
       osapiSnprintf(stat, sizeof(stat),"%s %u", pStrInfo_routing_IpIcmpErrorInterval_1,val);      
       EWSWRITEBUFFER (ewsContext, stat);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of Visible Interface for Routing
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningVisibleRoutingInterfaceInfo(EwsContext ewsContext, L7_uint32 unit,
    L7_uint32 interface)
{
  L7_uint32 retVal, val, i;
  L7_int32 ival;
  L7_uint32 encapType;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_IP_MASK_t mask, intfMask = L7_NULL_IP_MASK;
  L7_IP_ADDR_t ipAddr, intfIPAddr = L7_NULL_IP_ADDR;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_BOOL isUnnumbered;
  L7_uint32 numberedIfc;
  L7_uint32 numbUnit, numbPort, numbSlot;
  L7_char8 serverAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 udpPortStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intIfNum;
  L7_uint32 serverAddr;
  L7_ushort16 udpPort;
  L7_BOOL discard;
  L7_uint32 hitCount;
  L7_RC_t retValue = L7_FAILURE;
  L7_uint32 defMinAdvInt;
  L7_uint32 defAdvLifetime;
  L7_INTF_IP_ADDR_METHOD_t method;

#ifdef L7_RIP_PACKAGE
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_RIP+1];
#endif

#ifdef L7_VRRP_PACKAGE
  L7_AUTH_TYPES_t vrrpAuthType;
  L7_BOOL tryNextVrrp = L7_TRUE;
  L7_BOOL boolVal;
  L7_uchar8 priority, advIntvl;
  L7_uchar8 vrId, nextVrId, rc;
  L7_uint32 vrrpInterface, nextVrrpInterface, nextIpAddr, trackIntf=0;
  L7_uint32 maskLength;
  L7_char8 strFinal[L7_CLI_MAX_STRING_LENGTH];
#endif

#ifdef L7_OSPF_PACKAGE
  L7_uint32 intAreaId, secondaries;
  L7_uint32 ospfAuthType;
  L7_uchar8 ospfAuthKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_uint32 ospfIntfType;
  L7_uint32 defaultOspfNetType;
#endif

#ifdef L7_MCAST_PACKAGE
  L7_ushort16  dvmrpMetric;
  L7_uint32 ipGrp = 0, ipMask = 0;
  L7_uint32 intIface =1;
  L7_uint32 interval;
  L7_inet_addr_t inetIpGrp, inetIpMask;
#endif

  if(usmDbValidateRtrIntf(unit, interface) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
    /* get Interface routing status */
    if (usmDbIpRtrIntfModeConfigurable(unit, interface) == L7_TRUE)
    {
      if (usmDbIpRtrIntfModeGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_INTF_ADMIN_MODE,pStrInfo_common_Routing_1);
    }
  }
    if ((usmDbIpUnnumberedGet(interface, &isUnnumbered, &numberedIfc) == L7_SUCCESS) &&
        ((isUnnumbered != FD_RTR_INTF_UNNUMBERED) ||
        (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      memset(stat, 0, sizeof(stat));
      if (isUnnumbered)
      {
        strcpy(stat, "\r\nip unnumbered ");
        if (usmDbUnitSlotPortGet(numberedIfc, &numbUnit, &numbSlot, &numbPort) == L7_SUCCESS)
        {
          strcat(stat, cliGetIntfName(numberedIfc, numbUnit, numbSlot, numbPort));
        }
        else
        {
          /* Some error in configuration. Just ignore it */
          strcpy(stat, "\r\n");
        }
      }
      else
        strcpy(stat, "\r\nno ip unnumbered ");
      EWSWRITEBUFFER(ewsContext, stat);
    }

    if (usmDbIpRtrIntfCfgIpAddrListGet(unit, interface, ipAddrList) == L7_SUCCESS)
    {

      /* Primary IP Address */
      intfIPAddr = ipAddrList[0].ipAddr;
      intfMask = ipAddrList[0].ipMask;

      memset(stat, 0, sizeof(stat));
      memset(buf, 0, sizeof(buf));
      if (usmDbIpRtrIntfIpAddressMethodGet(interface, &method) == L7_SUCCESS)
      {
        if (method == L7_INTF_IP_ADDR_METHOD_DHCP)
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_routing_IpAddr_3);
          OSAPI_STRNCAT(stat, pStrInfo_common_Dhcp_1);
        }
        else
        {
          if ((intfIPAddr != L7_NULL_IP_ADDR) &&
              (usmDbInetNtoa(intfIPAddr, buf) == L7_SUCCESS))
          {
            OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_routing_IpAddr_3);
            OSAPI_STRNCAT(stat, buf);
          }
          memset(buf, 0, sizeof(buf));
          if ((intfMask != L7_NULL_IP_MASK) &&
              (usmDbInetNtoa(intfMask, buf) == L7_SUCCESS))
          {
            OSAPI_STRNCAT(stat, " ");
            OSAPI_STRNCAT(stat, buf);
          }
        }  
      }
      if (strlen(stat) > 0)
      {
        EWSWRITEBUFFER(ewsContext, stat);
      }

      if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_MULTINETTING) == L7_TRUE)
      {
        /* Secondary */

        for (i = 1; i <= L7_L3_NUM_IP_ADDRS - 1; i++)
        {
          ipAddr = ipAddrList[i].ipAddr;
          mask = ipAddrList[i].ipMask;

          memset(stat, 0, sizeof(stat));
          memset(buf, 0, sizeof(buf));
          if ((ipAddr != L7_NULL_IP_ADDR) &&
              (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS))
          {
            OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 2, L7_NULLPTR, stat, pStrInfo_routing_IpAddr_3);
            OSAPI_STRNCAT(stat, buf);
          }
          memset(buf, 0, sizeof(buf));
          if ((mask != L7_NULL_IP_MASK) &&
              (usmDbInetNtoa(mask, buf) == L7_SUCCESS))
          {
            OSAPI_STRNCAT(stat, "  ");
            OSAPI_STRNCAT(stat, buf);
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 0, L7_NULLPTR, stat, pStrInfo_routing_Secondary_1);
          }

          if (strlen(stat) > 0)
          {
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }
    }

    if (usmDbIpNetDirectBcastsGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST,pStrInfo_routing_IpNetdirbcast);
      }

    if (usmDbIfEncapsulationTypeGet(interface, &encapType) == L7_SUCCESS)
  {
      if ((encapType != FD_NIM_ENCAPSULATION_TYPE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        memset(stat, 0, sizeof(stat));
        switch (encapType)
        {
        case  L7_ENCAP_ETHERNET:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_EncapEthernet);
        EWSWRITEBUFFER(ewsContext, stat);
          break;
        case L7_ENCAP_802:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_EncapSnap);
        EWSWRITEBUFFER(ewsContext, stat);
          break;
        default:
          /* Do nothing */
          break;
        }

    }
      }


    /* IP helper addresses on this interface */
    retValue = usmDbIpHelperAddressFirst(&intIfNum, &udpPort, &serverAddr, &discard, &hitCount);
    while (retValue == L7_SUCCESS)
    {
      if (intIfNum == interface)
      {
        if (discard)
        {
          strcpy(serverAddrStr, "discard");
        }
        else
        {
          usmDbInetNtoa(serverAddr, serverAddrStr);
        }

        if (udpPort == IH_UDP_PORT_DEFAULT)
        { 
          udpPortStr[0] = '\0';
        }
        else
        {
          strUtilUdpPortToString(udpPort, udpPortStr);
        }
        osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH,
                      "\r\nip helper-address %s %s", serverAddrStr, udpPortStr);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      retValue = usmDbIpHelperAddressNext(&intIfNum, &udpPort, &serverAddr, &discard, &hitCount);
    }


#ifdef L7_VRRP_PACKAGE
    if (usmDbComponentPresentCheck(unit, L7_VRRP_MAP_COMPONENT_ID) &&
        usmDbVrrpIsValidIntf(unit, interface) == L7_TRUE)
    {
      /* get VRID for the interface */

      if (usmDbVrrpOperFirstGet(unit,&vrId, &vrrpInterface) == L7_SUCCESS)
      {
        tryNextVrrp = L7_TRUE;

        while (tryNextVrrp == L7_TRUE)
        {
          /* Matching vrrp interface found*/
          if (interface == vrrpInterface)
          {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_IpVrrp_1, vrId);
            EWSWRITEBUFFER(ewsContext, stat);

            if (usmDbVrrpOperVrrpModeGet(unit,vrId,interface,&val) == L7_SUCCESS)
          {
              if ((val !=  FD_VRRP_DEFAULT_START_STATE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_IpVrrpMode, vrId);
              EWSWRITEBUFFER( ewsContext, stat);
            }
              }
            /* IP Address */
            if ((usmDbVrrpIpAddressNextGet(unit, vrId, interface, 0, &ipAddr) == L7_SUCCESS &&
                  (ipAddr !=  FD_VRRP_DEFAULT_IP_ADDRESS))|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
            {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_routing_IpVrrpIp, vrId);
            memset(stat, 0, sizeof(stat));
            if (usmDbInetNtoa( ipAddr, stat) == L7_SUCCESS)
            {
              OSAPI_STRNCAT(buf, stat);
            }
              EWSWRITEBUFFER(ewsContext, buf);

              do
              {
                rc = usmDbVrrpIpAddressNextGet(unit, vrId, interface, ipAddr, &nextIpAddr);

                if (rc != L7_SUCCESS || nextIpAddr == L7_NULL_IP_ADDR)
              {
                break;
              }
                if (ipAddr !=  FD_VRRP_DEFAULT_IP_ADDRESS || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
                {
                  ipAddr = nextIpAddr;

                osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_routing_IpVrrpIp, vrId);
                  memset(stat, 0, sizeof(stat));
                  if (usmDbInetNtoa( ipAddr, stat) == L7_SUCCESS)
                  {
                    OSAPI_STRNCAT(buf, stat);
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, buf, pStrInfo_routing_Secondary_1);
                  }
                  EWSWRITEBUFFER(ewsContext, buf);
                }
              } while (rc == L7_SUCCESS);
            }

            /* vrrp tracking interface */
            do
            {
              rc = usmDbVrrpVrIdIntfTrackIntfNextGet(unit, vrId, interface, &trackIntf, &priority);

              if (rc != L7_SUCCESS )
              {
                break;
              }
              if (trackIntf !=  0 )
              {
                if (usmDbUnitSlotPortGet(trackIntf, &numbUnit, &numbSlot, &numbPort) == L7_SUCCESS)
                {
                  osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(numbUnit, numbSlot, numbPort));
                  osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_IpVrrpTrackIntf, vrId,stat);
                  if ((priority != FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
                  {
                    osapiSnprintf(stat, sizeof(stat),pStrInfo_routing_decrement,priority);
                    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, buf,  stat);
                  }
                  EWSWRITEBUFFER(ewsContext, buf);
                }

              }
              else
              {
                break;
              }
            } while (rc == L7_SUCCESS);

             /* end of vrrp tracking interface command */

            /* vrrp tracking  Route */
            ipAddr = 0;
            mask = 0;
            priority = 0;
            rc = usmDbVrrpVridIntfTrackRouteNextGet(unit, vrId, interface, &ipAddr, &mask, &priority);
            while (rc == L7_SUCCESS)
            {
              memset(strFinal, 0, sizeof(strFinal));
              memset(buf, 0, sizeof(buf));
              (void)usmDbInetNtoa(ipAddr, buf);
              osapiSnprintf(stat, sizeof(stat),  "%s/", buf);
              OSAPI_STRNCAT(strFinal, stat);

              memset(buf, 0, sizeof(buf));
              maskLength = usmCliMaskLength(mask);
              osapiSnprintf(stat, sizeof(stat), "%d", maskLength);
              OSAPI_STRNCAT(strFinal, stat);
              
              osapiSnprintfAddBlanks(1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  
                                     pStrInfo_routing_IpVrrpTrackRoute, vrId, strFinal);
              if ((priority != FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT) || 
                  (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
              {
                osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_decrement, priority);
                OSAPI_STRNCAT_ADD_BLANKS(0, 0, 1, 0, L7_NULLPTR, buf, stat);
              }

              EWSWRITEBUFFER(ewsContext, buf);
              rc = usmDbVrrpVridIntfTrackRouteNextGet(unit, vrId, interface, &ipAddr, &mask, &priority);
            }
            
            /* end of  vrrp tracking route command */

            /* priority */
            if (usmDbVrrpConfigPriorityGet(unit,vrId,interface,&priority) == L7_SUCCESS)
            {
              if ((priority != L7_VRRP_INTF_PRIORITY_MAX) && ((priority != FD_VRRP_DEFAULT_PRIORITY ) ||
                  EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
              {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_IpVrrpPri,vrId, priority);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }

            /* advertisement interval */
            if (usmDbVrrpOperAdvertisementIntervalGet(unit,vrId,interface,&advIntvl) == L7_SUCCESS)
            {
              if ((advIntvl != FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_IpVrrpTimersAdvertise,vrId,advIntvl);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }

            /* preempt mode */
            if (usmDbVrrpOperPreemptModeGet(unit,vrId,interface,&boolVal) == L7_SUCCESS)
            {
            osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_VrrpPreempt,vrId);
            cliShowCmdTrue(ewsContext, boolVal,FD_VRRP_DEFAULT_PREEMPT, stat);
            }

            /* accept mode */
            if (usmDbVrrpOperAcceptModeGet(unit,vrId,interface,&boolVal) == L7_SUCCESS)
            {
              osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_VrrpAcceptMode, vrId);
              cliShowCmdTrue(ewsContext, boolVal, FD_VRRP_DEFAULT_ACCEPT_MODE, stat);
            }

            /* auth type */
            if (usmDbVrrpOperAuthTypeGet(unit,vrId,interface,&vrrpAuthType) == L7_SUCCESS)
            {
              if ((vrrpAuthType != FD_VRRP_DEFAULT_AUTH_TYPE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                if (vrrpAuthType == L7_AUTH_TYPE_NONE)
                {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_IpVrrpAuthNone,vrId);
                EWSWRITEBUFFER(ewsContext, stat);
                }
                else if (vrrpAuthType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
                {
                osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_IpVrrpAuthSimple,vrId);
                memset(buf, 0, sizeof(buf));
                  if (usmDbVrrpOperAuthKeyGet(unit, vrId, interface, buf) == L7_SUCCESS)
                {
                    OSAPI_STRNCAT(stat, "\"");
                    OSAPI_STRNCAT(stat, buf);
                    OSAPI_STRNCAT(stat, "\"");
                EWSWRITEBUFFER(ewsContext, stat);
              }

              }
            }
          }
          }

          if (usmDbVrrpOperNextGet(unit, vrId,vrrpInterface,&nextVrId,
                &nextVrrpInterface) == L7_SUCCESS)
          {
            vrId = nextVrId;
            vrrpInterface = nextVrrpInterface;
          }
          else
        {
            tryNextVrrp = L7_FALSE;
        }

        } /*End of vrrp while */
      } /* End of vrrp if */
    }  /* End of component present check for vrrp */
#endif

#ifdef L7_OSPF_PACKAGE
    /* ospf config info */
    if (usmDbOspfIsValidIntf(unit, interface) == L7_TRUE)
    {
      /* Here we shall go through the "ip ospf area" commands. Hence we query
       * for the "configured" values of AdminMode, AreaId, SecondariesFlag
       * that got configured on the interface through "ip ospf area" commands
       */
      if (usmDbOspfIntfConfigAdminModeGet(unit, interface, &val) == L7_SUCCESS)
    {
        if ((val != FD_OSPF_INTF_DEFAULT_ADMIN_MODE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if ((usmDbOspfIntfConfigAreaIdGet(unit, interface, &intAreaId) == L7_SUCCESS) &&
              (usmDbOspfIntfConfigSecondariesFlagGet(unit, interface, &secondaries) == L7_SUCCESS))
          {
            memset(stat, 0, sizeof(stat));
            usmDbInetNtoa(intAreaId, stat);
            if(val == L7_ENABLE)
            {
              if(secondaries == L7_ENABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_IpOspfArea_1, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            else
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_IpOspfAreaSecondariesNone, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
          }
          else
          {
            if(secondaries == L7_ENABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_NoIpOspfAreaSecondariesNone);
              EWSWRITEBUFFER(ewsContext, buf);
            }

            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_NoIpOspfArea);
            EWSWRITEBUFFER(ewsContext, buf);
          }
        }
      }
    }

      if (usmDbIpRtrIntfIpAddressGet(unit, interface, &ipAddr, &mask) == L7_SUCCESS)
      {
        if (usmDbOspfIfMetricValueGet(unit, interface, 0, &val) == L7_SUCCESS)
        {
          if (usmDbOspfIsIfMetricConfigured(unit, ipAddr, interface, 0) == L7_TRUE)
          {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_IpOspfCost, val);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }

      /* OSPFv2 Network Type */
      memset(stat, 0, sizeof(stat));
      if (( usmDbOspfIfTypeGet(interface, &ospfIntfType) == L7_SUCCESS) &&
          (usmDbOspfDefaultIfType(interface, &defaultOspfNetType) == L7_SUCCESS))
      {
        if ( ospfIntfType != defaultOspfNetType )
        {
          switch (ospfIntfType)
          {
            case L7_OSPF_INTF_BROADCAST:
              sprintf(stat, "\r\nip ospf network broadcast ");
              break;
            case L7_OSPF_INTF_PTP:
              sprintf(stat, "\r\nip ospf network point-to-point ");
              break;
            default:
              break;
          }
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }

      if (usmDbOspfIntfPriorityGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_OSPF_INTF_DEFAULT_PRIORITY,pStrInfo_routing_IpOspfPri);
    }
      if (usmDbOspfIntfTransitDelayGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY,pStrInfo_routing_IpOspfTxDelay);
    }
      if (usmDbOspfIntfRxmtIntervalGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL,pStrInfo_routing_IpOspfReTxIntvl);
    }

      if (usmDbOspfIntfHelloIntervalGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL,pStrInfo_routing_IpOspfHelloIntvl);
    }

      if (usmDbOspfIntfDeadIntervalGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL,pStrInfo_routing_IpOspfDeadIntvl);
    }

      if (usmDbOspfIntfAuthTypeGet(unit, interface, &ospfAuthType) == L7_SUCCESS)
    {
        if ((ospfAuthType != FD_OSPF_INTF_DEFAULT_AUTH_TYPE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
        osapiSnprintf(stat, sizeof(stat),  "\r\n%s %s", pStrInfo_routing_IpOspfAuth, strUtilAuthTypeGet(ospfAuthType, ""));
          if (ospfAuthType == L7_AUTH_TYPE_NONE)
          {
              EWSWRITEBUFFER(ewsContext, stat);
          }

          /* get the Auth Key */
          if (ospfAuthType == L7_AUTH_TYPE_SIMPLE_PASSWORD || ospfAuthType == L7_AUTH_TYPE_MD5)
          {
            if (usmDbOspfIntfAuthKeyActualGet(unit, interface, ospfAuthKey) == L7_SUCCESS)
            {
            osapiSnprintf(buf, sizeof(buf), " %s", ospfAuthKey);
              OSAPI_STRNCAT(stat, buf);

              if (ospfAuthType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
              {
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }
          }

          /* get the Auth Key */
          if (ospfAuthType == L7_AUTH_TYPE_MD5)
          {
            if (usmDbOspfIntfAuthKeyIdGet(unit, interface, &val) == L7_SUCCESS)
            {
              osapiSnprintf(buf, sizeof(buf),  " %u", val);
              OSAPI_STRNCAT(stat, buf);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
      }
        }
    }
    /* end ospf config info */
#endif

#ifdef L7_IPV6_PACKAGE
    cliRunningConfigInterfaceIpv6(ewsContext, unit, interface);
    cliRunningConfigIpv6Info(ewsContext, unit, interface);
#endif

#ifdef L7_RIP_PACKAGE
    /* rip config info */

    if (usmDbRip2IsValidIntf(unit, interface) == L7_TRUE)
    {
      /*get the RIP mode */
      if (usmDbComponentPresentCheck(unit, L7_RIP_MAP_COMPONENT_ID) == L7_TRUE)
      {
        if (usmDbRip2IfConfStatusGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_RIP_INTF_DEFAULT_ADMIN_MODE,pStrInfo_routing_IpRip);
      }
        if (usmDbRip2IfConfAuthTypeGet(unit, interface, &val) == L7_SUCCESS)
      {
        if ((val != FD_RIP_INTF_DEFAULT_AUTH_TYPE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintf(stat, sizeof(stat),  "\r\n%s %s", pStrInfo_routing_IpRipAuth, 
                        strUtilAuthTypeGet(val, ""));
            if (val == L7_AUTH_TYPE_NONE)
          {
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else if (val == L7_AUTH_TYPE_SIMPLE_PASSWORD || val == L7_AUTH_TYPE_MD5)
            {
              if (usmDbRip2IfConfAuthKeyActualGet(unit, interface,authKey) == L7_SUCCESS)
              {
                osapiSnprintf(buf, sizeof(buf), " %s ",authKey);
                OSAPI_STRNCAT(stat, buf);
                if (val == L7_AUTH_TYPE_SIMPLE_PASSWORD)
              {
                  EWSWRITEBUFFER(ewsContext, stat);
              }
              }
            }
            memset(buf, 0, sizeof(buf));
            if (val == L7_AUTH_TYPE_MD5)
            {
              if (usmDbRip2IfConfAuthKeyIdGet(unit, interface, &val) == L7_SUCCESS)
              {
                osapiSnprintf(buf, sizeof(buf),  "%d ", val);
                OSAPI_STRNCAT(stat, buf);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }

          }
      }

        if (usmDbRip2IfConfDefaultMetricGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,val,FD_RIP_INTF_DEFAULT_AUTH_TYPE,pStrInfo_routing_IpRipDeflInfoOriginate);
          }

        if (usmDbRip2IfConfSendGet(unit, interface, &val) == L7_SUCCESS)
      {
          if ((val != FD_RIP_INTF_DEFAULT_VER_SEND)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
            switch (val)                                                           /* None, RIP-1, RIP-2, RIP-1c  */
            {
            case L7_RIP_CONF_SEND_DO_NOT_SEND:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_NoIpRipSendVer);
              break;
            case L7_RIP_CONF_SEND_RIPV1:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipSendVerRip1);
              break;
            case L7_RIP_CONF_SEND_RIPV1_COMPAT:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipSendVerRip1c);
              break;
            case L7_RIP_CONF_SEND_RIPV2:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipSendVerRip2);
              break;
            default:
              /* Do nothing */
              break;
          }
            }
          }

        if (usmDbRip2IfConfReceiveGet(unit, interface, &val) == L7_SUCCESS)
      {
          if ((val != FD_RIP_INTF_DEFAULT_VER_RECV)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
            switch (val)
            {
            case L7_RIP_CONF_RECV_DO_NOT_RECEIVE:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_NoIpRipReceiveVer);
              break;
            case L7_RIP_CONF_RECV_RIPV1:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipReceiveVerRip1);
              break;
            case L7_RIP_CONF_RECV_RIPV1_RIPV2:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipReceiveVerBoth);
              break;
            case L7_RIP_CONF_RECV_RIPV2:
            EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_IpRipReceiveVerRip2);
              break;
            default:
              /* Do nothing */
              break;
          }
            }
          }
      }
    }
    /* end rip config info */
#endif

    /* rtrdiscovery config info */
    if (usmDbRtrDiscIsValidIntf(unit, interface) == L7_TRUE)
    {
      /* rtrdiscovery config info */
      if (usmDbRtrDiscAdvertiseGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_ADVERTISEMENT_MODE,pStrInfo_routing_IpIrdp_1);
      }
      /* Must list minadvint and lifetime after maxadvint, because setting maxadvint automatically
       * changes minadvint and lifetime. If there are configured values for minadvint and lifetime, 
       * we want to push those values later and override the auto adjustments based on 
       * maxadvint. */
      if (usmDbRtrDiscMaxAdvIntervalGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,val,FD_RTR_DEFAULT_MAX_ADV_INTERVAL,pStrInfo_routing_IpIrdpMaxadvertinterval);
      }
      if ((usmDbRtrDiscMinAdvIntervalGet(unit, interface, &val) == L7_SUCCESS) &&
          (usmDbRtrDiscMinAdvIntervalDefaultGet(interface, &defMinAdvInt) == L7_SUCCESS))
      {
        cliShowCmdInt(ewsContext, val, defMinAdvInt, pStrInfo_routing_IpIrdpMinadvertinterval);
      }
      if ((usmDbRtrDiscAdvLifetimeGet(unit, interface, &val) == L7_SUCCESS) && 
          (usmDbRtrDiscAdvLifetimeDefaultGet(interface, &defAdvLifetime) == L7_SUCCESS))
      {
        cliShowCmdInt(ewsContext, val, defAdvLifetime, pStrInfo_routing_IpIrdpHoldtime);
      }
      if (usmDbRtrDiscPreferenceLevelGet(unit, interface, &ival) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext, ival, FD_RTR_DEFAULT_PREFERENCE_LEVEL, pStrInfo_routing_IpIrdpPref);
      }
  
      if (usmDbRtrDiscAdvAddrGet(unit, interface, &val) == L7_SUCCESS)
      {
        if (val != FD_RTR_DEFAULT_ADV_ADDR)
        {
          if (val == 0xFFFFFFFF)
          {
            EWSWRITEBUFFER(ewsContext, "no ip irdp multicast");
          }
          else if (val == 0xE0000001)
          {
            EWSWRITEBUFFER(ewsContext, pStrInfo_routing_IpIrdpAddr_1);
          }
        }
      }
    }
    /* end rtrdiscovery config info */

  if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
  {
    /* Proxy ARP */
    if (usmDbProxyArpGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_PROXY_ARP_ADMIN_MODE,pStrInfo_routing_IpProxyArp_2);
    }

    /* Local proxy ARP */

    if (usmDbLocalProxyArpGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_LOCAL_PROXY_ARP_ADMIN_MODE,pStrInfo_routing_IpLocalProxyArp_2);
    }
  }

#ifdef L7_IP_MCAST_PACKAGE
    /* dvmrp config info */
    if (usmDbDvmrpIsValidIntf(unit, interface) == L7_TRUE)
    {
      if (usmDbDvmrpIntfAdminModeGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DVMRP_INTF_DEFAULT_ADMIN_MODE,pStrInfo_common_IpDvmrp_1);
      }
      if (usmDbDvmrpIntfMetricGet( unit, interface, &dvmrpMetric ) ==  L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,dvmrpMetric,FD_DVMRP_INTF_DEFAULT_METRIC,pStrInfo_routing_IpDvmrpMetric_1);
      }
    }

    /* end dvmrp config info */
#endif

#ifdef L7_MCAST_PACKAGE
    /* router-igmp and igmp-proxy config info */
    if (usmDbMgmdIsValidIntf(unit, interface) == L7_TRUE)
    {
      if (usmDbMgmdInterfaceModeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_IGMP_INTF_DEFAULT_ADMIN_MODE,pStrInfo_common_IpIgmp);
    }
       if (usmDbMgmdProxyInterfaceModeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_IGMP_PROXY_INTF_DEFAULT_ADMIN_MODE,pStrInfo_routing_IpIgmpProxy);
    }
       if (usmDbMgmdProxyInterfaceUnsolicitedIntervalGet(unit, L7_AF_INET, interface, &interval) == L7_SUCCESS)
       {
      cliShowCmdInt(ewsContext, interval, FD_IGMP_DEFAULT_UNSOLICITED_REPORT_INTERVAL,
                    pStrInfo_routing_IpIgmpProxyUnsolicitRprtIntvl);
       }

      if (usmDbMgmdInterfaceVersionGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_DEFAULT_VER,pStrInfo_routing_IpIgmpVer_2);
        }

      if (usmDbMgmdInterfaceQueryIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_QUERY_INTERVAL,pStrInfo_routing_IpIgmpQueryIntvl_2);
        }

      if (usmDbMgmdInterfaceQueryMaxResponseTimeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_QUERY_RESPONSE_INTERVAL,pStrInfo_routing_IpIgmpQueryMaxResponseTime);
        }

      if (usmDbMgmdInterfaceRobustnessGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_ROBUST_VARIABLE,pStrInfo_routing_IpIgmpRobustness_2);
        }

      if (usmDbMgmdInterfaceStartupQueryIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_STARTUP_QUERY_INTERVAL,pStrInfo_routing_IpIgmpStartupQueryIntvl_2);
        }

      if (usmDbMgmdInterfaceStartupQueryCountGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_STARTUP_QUERY_COUNT,pStrInfo_routing_IpIgmpStartupQueryCount_2);
        }

      if (usmDbMgmdInterfaceLastMembQueryIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_LAST_MEMBER_QUERY_INTERVAL,pStrInfo_routing_IpIgmpLastMbrQueryIntvl_2);
        }

      if (usmDbMgmdLastMembQueryCountGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_IGMP_LAST_MEMBER_QUERY_COUNT,pStrInfo_routing_IpIgmpLastMbrQueryCount_2);
        }
       /* end router-igmp and igmp-proxy config info */

      /* router mcast config info */
      if (usmDbMcastIsValidIntf(unit, interface) == L7_TRUE &&
          usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
      {
        if (usmDbMcastIpMRouteInterfaceTtlGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliShowCmdInt(ewsContext,val,FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD,pStrInfo_routing_IpMcastTtlThresh_1);
        }
      }
    }
    /* end router mcast config info */

    /* pimdm config info */

    if (usmDbPimdmIsValidIntf(unit, interface) == L7_TRUE)
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_PIMDM_MAP_COMPONENT_ID))
      {
        /* pimdm config info */
        if (usmDbPimAdminModeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
        {
          cliShowCmdEnable(ewsContext,val,FD_PIMDM_INTF_DEFAULT_MODE,pStrInfo_common_IpPimdm);
        }
  
        if (usmDbPimInterfaceHelloIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
        {
          cliShowCmdInt(ewsContext,val,FD_PIMDM_INTF_DEFAULT_HELLO_INTERVAL,pStrInfo_routing_IpPimdmHelloIntvl);
        }
      }
    }
    /* end pimdm config info */

    /* pimsm config info */
    if (usmDbPimsmIsValidIntf(unit, interface) == L7_TRUE)
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_PIMSM_MAP_COMPONENT_ID))
      {
      if (usmDbPimsmInterfaceModeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_PIMSM_DEFAULT_INTERFACE_MODE,pStrInfo_common_IpPimsm);
      }
      if (usmDbPimsmInterfaceHelloIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,val,FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL,pStrInfo_routing_IpPimsmHelloIntvl);
      }
      if (usmDbPimsmInterfaceJoinPruneIntervalGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,val,FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL,pStrInfo_routing_IpPimsmJoinPruneIntvl);
      }
      if (usmDbPimsmInterfaceBsrBorderGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_PIMSM_DEFAULT_BSR_BORDER,pStrInfo_routing_IpPimsmBsrBorder);
      }
      if (usmDbPimsmInterfaceDRPriorityGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdInt(ewsContext,val,FD_PIMSM_DEFAULT_INTERFACE_DR_PRIORITY,pStrInfo_routing_IpPimsmDrPri);
      }
    }
   }
    /* end pimsm config info */

    inetAddressSet(L7_AF_INET, &ipGrp, &inetIpGrp);
    inetAddressSet(L7_AF_INET, &ipMask, &inetIpMask);

    if (usmDbMcastIsValidIntf(unit, interface) == L7_TRUE)
    {
      while (usmDbMcastMrouteBoundaryEntryNextGet(unit, &intIface, &inetIpGrp,
            &inetIpMask) == L7_SUCCESS)
      {
        inetAddressGet(L7_AF_INET, &inetIpGrp, &ipGrp);
        inetAddressGet(L7_AF_INET, &inetIpMask, &ipMask);
        if (intIface== interface)
        {
          memset(buf, 0, sizeof(buf));
          memset(stat, 0, sizeof(stat));
          if (usmDbInetNtoa(ipGrp, buf) == L7_SUCCESS)
          {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_IpMcastBoundary_1, buf);
          memset(buf, 0, sizeof(buf));
          if (usmDbInetNtoa(ipMask, buf) == L7_SUCCESS)
          {
            OSAPI_STRNCAT(stat, buf);
          }
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }
    }
  }
#endif    /* end if mcast package included */

  if (usmDbIntfIpMtuGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_IP_DEFAULT_IP_MTU,pStrInfo_routing_IpMtu_2);
  }
  if (usmDbIpMapICMPUnreachablesModeGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_ICMP_UNREACHABLES_MODE, pStrInfo_routing_IpUnreachables_1);
  } 
  if (usmDbIpMapIfICMPRedirectsModeGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RTR_DEFAULT_ICMP_REDIRECT_MODE, pStrInfo_routing_IpRedirects_1);
  }
#ifdef L7_OSPF_PACKAGE
    if (usmDbOspfIsValidIntf(unit, interface) == L7_TRUE &&
      (usmDbOspfIntfMtuIgnoreGet(unit, interface, &val) == L7_SUCCESS))
  {
    cliShowCmdTrue(ewsContext,val,FD_OSPF_DEFAULT_MTU_IGNORE,pStrInfo_routing_IpOspfMtuIgnore_1);
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of static IP route
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigIpRoute(EwsContext ewsContext, L7_uint32 unit)
{
  L7_RC_t rc;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 pfxStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 maskStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 nhStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 prefStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 intfStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 nh;
  L7_uint32 defaultPref;
  L7_routeEntry_t * staticEntries;
  L7_uint32 intfUnit, intfSlot, intfPort;
  L7_uint32 globalDefGwAddr = 0;

  if (usmDbIpGlobalDefaultGatewayAddressGet (&globalDefGwAddr, L7_FALSE)
                                          == L7_SUCCESS)
  {
    if (globalDefGwAddr != L7_NULL)
    {
      memset(stat, 0, sizeof(stat));
      rc = usmDbInetNtoa(globalDefGwAddr, pfxStr);
      osapiSnprintf(stat, sizeof(stat), "\r\n%s %.15s",
                    pStrInfo_routing_IpDefGatway, pfxStr);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  /* Get default preference for static routes to know whether to
   * display preference or not. */
  usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_STATIC, &defaultPref);

  staticEntries = (L7_routeEntry_t *) osapiMalloc(L7_CLI_WEB_COMPONENT_ID,
                                                  FD_RTR_MAX_STATIC_ROUTES * sizeof(L7_routeEntry_t));
  rc = usmDbIpMapStaticRouteGetAll(unit, staticEntries);
  for (i = 0; (i < FD_RTR_MAX_STATIC_ROUTES) && (staticEntries[i].protocol == RTO_STATIC); i++)
  {
    rc = usmDbInetNtoa(staticEntries[i].ipAddr, pfxStr);
    rc = usmDbInetNtoa(staticEntries[i].subnetMask, maskStr);
    if (staticEntries[i].pref != defaultPref)
    {
      osapiSnprintf(prefStr, sizeof(prefStr),  "%u", staticEntries[i].pref);
    }
    else
    {
      OSAPI_STRNCPY_SAFE(prefStr, pStrInfo_common_EmptyString);
    }

    if (staticEntries[i].flags & L7_RTF_REJECT)
    {
      osapiSnprintf(stat, sizeof(stat), "\r\n%s %s %s %s %s", 
                pStrInfo_routing_IpRoute, pfxStr, maskStr, pStrInfo_common_IntfNull0, prefStr);
      EWSWRITEBUFFER(ewsContext, stat);
      continue;
    }

    /* Each static route can contain multiple next hops. List each next hop
     * as a separate "ip route" command. */
    for (nh = 0; nh < staticEntries[i].ecmpRoutes.numOfRoutes; nh++)
    {
      /* If this static route is same as Global Default Gateway, Skip it */
      if ((staticEntries[i].ipAddr == 0) && (staticEntries[i].subnetMask == 0) &&
          (staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr == globalDefGwAddr))
      {
        continue;
      }
      rc = usmDbInetNtoa(staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr, nhStr);
      if (staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum != L7_INVALID_INTF )
      {
        if (usmDbUnitSlotPortGet(
              staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum,
              &intfUnit, &intfSlot, &intfPort) == L7_SUCCESS)
        {
          sprintf(intfStr,"%s", 
                  cliGetIntfName(staticEntries[i].ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum, 
                                 intfUnit, intfSlot, intfPort));
        }
        else
        {
          sprintf(intfStr,"%s", pStrInfo_common_Blank);
        }
        sprintf(stat, "\r\n%s %s %s interface %s %s %s", 
                pStrInfo_routing_IpRoute, pfxStr, maskStr, intfStr, nhStr, prefStr);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "\r\n%s %s %s %s %s", 
                      pStrInfo_routing_IpRoute, pfxStr, maskStr, nhStr, prefStr);
      }
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  osapiFree(L7_CLI_WEB_COMPONENT_ID, staticEntries);

  return L7_SUCCESS;
}

