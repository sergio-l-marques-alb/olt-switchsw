/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/common/clicommands_l3.c
*
* @purpose create the cli commands for Routing functions
*
* @component user interface
*
*
* @create  05/14/2002
*
* @author  Kathy McDowell
* @author  Samip Garg
*
*
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "usmdb_util_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ospf_api.h"
#include "usmdb_ospfv3_api.h"
#include "usmdb_ip_api.h"
#include "clicommands_l3.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif

#include "clicommands_loopback.h"
#ifdef L7_BGP_PACKAGE
#include "clicommands_bgp_l3.h"
#endif
#ifdef L7_MCAST_PACKAGE
#include "clicommands_mcast.h"
#endif /* L7_MCAST_PACKAGE */

#include "datatypes.h"

#include "usmdb_mib_ospf_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"

/*********************************************************************
*
* @purpose  Build the Show tree for BootP.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowBootPdhcpRelay(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Bootpdhcprelay, pStrInfo_routing_ShowRelayIntf, commandShowBootpDhcpRelay, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Show Route tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowIpRoute(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8;

  L7_BOOL with_all_option = L7_TRUE;

  /* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Route, pStrErr_routing_ShowRtrRoute, commandShowIpRoute, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildRouteProtocolTree(depth4, with_all_option);
  buildRouteAllOptionTree(depth4);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_IpAddr, pStrInfo_routing_RouteDstIp, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildRouteProtocolTree(depth5, !with_all_option);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Mask, pStrInfo_routing_RouteDstMask, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildRouteProtocolTree(depth6, with_all_option);
  buildRouteAllOptionTree(depth6);
  depth7 = ewsCliAddNode(depth6, pStrInfo_routing_LongerPrefixesOption, pStrInfo_routing_RouteDstLongerPrefixes, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildRouteProtocolTree(depth7, with_all_option);
  buildRouteAllOptionTree(depth7);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dot1xShowSummary, pStrInfo_common_Ipv6RoutesSummary, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildRouteAllOptionTree(depth5);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Prefs, pStrErr_common_ShowRtrRoutePrefs, commandShowIpRoutePreferences, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the <protocol> sub-tree for "show ip route" tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildRouteProtocolTree(EwsCliCommandP depth1, L7_BOOL with_all_option)
{
  EwsCliCommandP depth2, depth3;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Connected_1, pStrInfo_routing_RouteConnected, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_OSPF_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Ospf_1, pStrInfo_routing_RouteOspf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if(with_all_option == L7_TRUE)
  {
    buildRouteAllOptionTree(depth2);
  }
#endif

#ifdef L7_RIP_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Rip3, pStrInfo_routing_RtrIp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if(with_all_option == L7_TRUE)
  {
    buildRouteAllOptionTree(depth2);
  }
#endif

#ifdef L7_BGP_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Bgp_1, pStrInfo_routing_RouteBgp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if(with_all_option == L7_TRUE)
  {
    buildRouteAllOptionTree(depth2);
  }
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Static2, pStrInfo_routing_RoutesTatic, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if(with_all_option == L7_TRUE)
  {
    buildRouteAllOptionTree(depth2);
  }
}

/*********************************************************************
*
* @purpose  Build the "all" sub-tree for "show ip route" tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildRouteAllOptionTree(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_All, pStrInfo_routing_RouteAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  Build the Show IRDP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowIrdp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6;

/* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Irdp, pStrErr_routing_ShowRtrRtrDisc, commandShowIpIrdp, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the privileged arp commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreePrivArp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Arp_2, pStrInfo_routing_ArpPriv, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Purge, pStrInfo_routing_ArpPurge, commandArpPurge, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_routing_ArpPurgeIpAddr_1, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, NULL, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the privileged clear arp commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreePrivClearArp(EwsCliCommandP depth2)
{
    EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_ArpCache_1, pStrInfo_routing_ClrArpCache, commandClearArpCache, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Gateway_2, pStrInfo_routing_ClrArpCacheGateway, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the privileged clear IP Stack's arp entry commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreePrivClearArpSwitch(EwsCliCommandP depth2)
{
 EwsCliCommandP depth3, depth4;
 depth3 = ewsCliAddNode(depth2, pStrInfo_routing_ArpSwitch, pStrInfo_routing_ClrArpCache,
commandClearArpSwitch, L7_NO_OPTIONAL_PARAMS);
 depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr , pStrInfo_common_NewLine, NULL,
L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the config arp commands
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreeGlobalArpBootp(EwsCliCommandP depth2)
{
    EwsCliCommandP depth3, depth4, depth5;
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

    osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipaddr, pStrInfo_routing_CfgArpCreate, commandArp, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_MAC_ADDRESS_SVL_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Cachesize, pStrInfo_routing_CfgArpCacheSize, commandArpCacheSize, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_routing_CfgArpCacheSizeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Dynrenew, pStrInfo_routing_ArpDynRenew, commandArpDynamicRenew, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, NULL, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode (depth2, pStrInfo_routing_Resptime, pStrInfo_routing_CfgArpRespTime, commandArpRespTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to10, pStrInfo_common_CfgTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Retries_2, pStrInfo_routing_CfgArpRetries, commandArpRetries, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Range0to10, pStrInfo_routing_CfgArpRetriesRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Timeout_2, pStrInfo_routing_CfgArpAgeTime, commandArpTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Kavleen : Changed L7_IP_ARP_AGE_TIME_MAX from 3600 secs to 21600 secs   */
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Range15to21600, pStrInfo_routing_ArpTimeoutRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the config Bootp commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
*
* @end
*
********************************************************************/
void buildTreeGlobalBootp(EwsCliCommandP depth1)
{
    EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Bootpdhcprelay, pStrInfo_routing_CfgRelay, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Cidoptmode, pStrInfo_routing_CfgRelayCircuitIdOptionMode, commandBootpDhcpRelayCIdOptMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Maxhopcount, pStrInfo_routing_CfgRelayMaxHopCount, commandBootpDhcpRelayMaxHopCount, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Range1to16, pStrInfo_routing_CfgRelayMaxHopCountRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Minwaittime, pStrInfo_routing_CfgRelayMinWaitTime, commandBootpDhcpRelayMinWaitTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to100, pStrInfo_routing_CfgRelayMinWaitTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the tree for vlan internal commands
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalInternalVlan(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uchar8 *help = "Configure allocation of internal VLANs";
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  depth3 = ewsCliAddNode(depth2, "internal ", help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, "allocation ", help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);  
  depth5 = ewsCliAddNode(depth4, "base ", "Configure base VLAN ID for internal allocation", commandVlanInternalAllocationBase, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, "policy ", "Allocate internal VLAN ID in ascending or descending order",
                         commandVlanInternalAllocationPolicy, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "ascending ", "Allocate internal VLAN IDs in ascending order", NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "descending ", "Allocate internal VLAN IDs in descending order", NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Config Global IP tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeIfIpAddr(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Addr_4, pStrInfo_routing_CfgIpIntfNwCreate, commandIpAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_SubnetMask, pStrInfo_common_DiffservIpMask, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  unit = cliGetUnitId();
  if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_MULTINETTING) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Secondary_1, pStrInfo_routing_EnterSecondary, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

}

/*********************************************************************
*
* @purpose  Build the Config Global IP ICMP tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalIpIcmp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Icmp_1, pStrInfo_routing_IpIcmp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_EchoReply, pStrInfo_routing_IpIcmpEchoReply, commandIpIcmpEchoReply, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ErrorInterval, pStrInfo_routing_IpIcmpErrorInterval, commandIpIcmpErrorInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647, pStrInfo_routing_IpIcmpErrorIntervalRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY); 
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6  = ewsCliAddNode(depth5, pStrInfo_common_Range1to200, pStrInfo_routing_IpIcmpBurstSize, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
 depth3 = ewsCliAddNode(depth2, pStrInfo_common_Redirects, pStrInfo_routing_IpRedirects, commandIpGlobalRedirects, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
 depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Config Global IP Router tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalIpRouter(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5, depth6, depth6a, depth7, depth8;

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_RTO_PREFERENCE_MIN, L7_RTO_PREFERENCE_MAX);

  /* ip route <pfx> <mask> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Route, "Create or delete a static route", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Nwaddr, "Enter the destination prefix", commandIpRoute, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Subnetmask, "Enter the destination network mask", NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);

  /* support "no ip route <addr> <mask>" to delete all static routes to this destination */
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, "Delete all static routes to this network", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* no ip route <pfx> <mask> <pref> to revert pref to default value */
  depth6 = ewsCliAddNode(depth5, buf, "Revert the route preference to its default", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* (no) ip route <pfx> <mask> <nexthop> */
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Nexthopip, pStrErr_routing_CfgRtrRouteCreateNextHopIp, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* ip route <pfx> <mask> <nexthop> <pref> */
  depth7 = ewsCliAddNode(depth6, buf, "Set the route preference", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  unit = cliGetUnitId();
  if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_REJECTROUTE) == L7_TRUE)
  {
    /* (no) ip route <pfx> <mask> Null0 */
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_IntfNull0, pStrInfo_common_CfgStaticRouteNull0, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* ip route <pfx> <maxk> Null0 <pref> */
    depth7 = ewsCliAddNode(depth6, buf, "Set the route preference", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }

  /* (no) ip route default */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_PassDefl, pStrErr_routing_CfgRtrRouteDeflCreate, commandIpRouteDefault, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Nexthopip, pStrErr_routing_CfgRtrRouteCreateNextHopIp, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, buf, pStrErr_routing_CfgRtrRtrDiscPrefRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Distance, pStrInfo_common_IpRouteDistance, commandIpRouteDistance, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to255, pStrInfo_common_IpRouteDistanceVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Routing_1, pStrInfo_routing_IpRouting, commandIpRouting, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* support this no form to revert pref to default:  "no ip route <dest> <mask> <pref>" */
  depth6a = ewsCliAddNode(depth5, buf, pStrInfo_common_IpRouteDistance, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6a, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* support "ip route <dest> <mask> <nexthop> <pref>" */
  depth7 = ewsCliAddNode(depth6, buf, pStrInfo_common_IpRouteDistance, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
}

/*********************************************************************
*
*
* @purpose  Build the Routing Interface Encapsulation tree.
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceEncaps(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Encap, pStrInfo_routing_CfgIntfEncaps, commandEncapsulation, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_DhcpEtherNet, pStrInfo_routing_CfgIntfEncapsType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Snap2, pStrInfo_routing_CfgIntfEncapsType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Routing Interface Configuration tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceConfig(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Routing_1, pStrInfo_routing_IntfRouting, commandInterfaceRouting, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Routing Interface IRDP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceIrdp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Irdp, pStrInfo_routing_IpIrdp, commandIpIrdp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Addr_4, pStrInfo_routing_CfgRtrDiscAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_1, pStrInfo_routing_EnterIpIrdpAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_HoldTime_1, pStrInfo_routing_IpIrdpHoldTime, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Range600to9000, pStrInfo_routing_IpIrdpHoldTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_MaxAdvertIntvl, pStrInfo_routing_CfgRtrDiscMaxIntvl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Range4to1800_1, pStrInfo_routing_IpIrdpMaxIntvlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_MinAdvertIntvl, pStrInfo_routing_CfgRtrDiscMinIntvl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Range3to600, pStrInfo_routing_IpIrdpMinIntvlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pref_1, pStrInfo_routing_CfgRtrDiscPref, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_21474836482147483647, pStrErr_routing_CfgRtrRtrDiscPrefRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Routing Interface IP Proxy Arp tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpProxyArp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  /* DEPTH2 = "ip " */
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_ProxyArp, pStrInfo_routing_IpProxyArp, commandProxyArp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Routing Interface IP Local Proxy Arp tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpLocalProxyArp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  /* DEPTH2 = "ip " */

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_LocalProxyArp_1, pStrInfo_routing_IpLocalProxyArp, commandLocalProxyArp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

#ifdef L7_UNNUMBERED_INTERFACES
/*********************************************************************
*
* @purpose  Build the Routing Interface IP unnumbered tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpUnnumbered(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_uchar8 buf[32];

  /* DEPTH2 = "ip " */

  depth3 = ewsCliAddNode(depth2, "unnumbered ", CLIIPUNNUMBERED_HELP, 
                         commandIpUnnumbered, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, CLI_LOOPBACK, CLIIPUNNUMBEREDLOOPBACK_HELP, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY); 
  sprintf(buf, "<%d-%d> ", L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth5 = ewsCliAddNode(depth4, buf, CLIIPUNNUMBEREDLOOPID_HELP, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "<cr> ", CLINEWLINE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, "<u/s/p> ", CLIIPUNNUMBEREDINTF_HELP, NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, "<cr> ", CLINEWLINE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
}
#endif

/*********************************************************************
*
* @purpose  Build the Routing Configuration tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRouterConfigMode(EwsCliCommandP depth1)
{
#if defined (L7_OSPF_PACKAGE) || defined (L7_RIP_PACKAGE) || defined (L7_BGP_PACKAGE)
   EwsCliCommandP depth2, depth3, depth4;
#endif

#ifdef L7_BGP_PACKAGE
   EwsCliCommandP depth5;
#endif

   /* Router Config mode*/
#if defined (L7_OSPF_PACKAGE) || defined (L7_RIP_PACKAGE) || defined (L7_BGP_PACKAGE)
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Rtr_3,pStrInfo_common_RtrCfg,NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_BGP_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bgp_1,pStrInfo_routing_BgpRtrCfg,cliRouterConfigBgp4Mode, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_BGP_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to65535,pStrInfo_routing_CfgRtrBgpAsNumParams,NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_BGP_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_BGP_MAP_COMPONENT_ID, L7_NODE_TYPE_MODE, L7_ROUTER_CONFIG_BGP4_MODE);
#endif

#ifdef L7_OSPF_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ospf_1,pStrInfo_common_OspfRtrCfg,cliOSPRouterConfigMode, 2, L7_KEY_RESTRICTED_NODE,L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_TYPE_MODE, L7_ROUTER_CONFIG_OSPF_MODE);
#endif

#ifdef L7_RIP_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Rip3,pStrInfo_routing_RipRtrCfg,cliRIPRouterConfigMode, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_ROUTER_CONFIG_RIP_MODE);
#endif
}

/*********************************************************************
*
* @purpose  Build the Switch Management ip mtu configuration
*           command tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeInterfaceConfigIpMtu(EwsCliCommandP depth2)
{
  EwsCliCommandP  depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

#ifdef _L7_OS_LINUX_
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_L3_MIN_IP_MTU,
                          L7_L3_MAX_IP_MTU);
#endif
#ifdef _L7_OS_VXWORKS_
    osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_L3_MIN_IP_MTU,
                        L7_MAX_FRAME_SIZE - (L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + L7_ETHERNET_CHECKSUM_LEN));
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mtu, pStrInfo_routing_IpMtu_1, commandIpMtu, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_routing_IpMtuFrameSize, NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the  Redirects tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpRedirects(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Redirects, pStrInfo_routing_IpRedirects, commandIpRedirects,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Unreachables tree.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpUnreachables(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Unreachables, pStrInfo_routing_IpUnreachables, commandIpUnreachables, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}


#ifdef L7_RLIM_PACKAGE
/*********************************************************************
 * *
 * * @purpose  Build the tree for Tunnel and Loopback
 * *
 * * @param EwsCliCommandP depth2
 * *
 * * @returntype void
 * *
 * * @end
 * *
 * *********************************************************************/
void buildTreeIntfTunnelLoopback(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_LoopBack, pStrInfo_routing_LoopBackIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_LoopBackIntfId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_IPV6_PACKAGE
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Tunnel_1, pStrInfo_common_Ipv6Tunnel,  NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_TunnelIntfId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/*********************************************************************
 * *
 * * @purpose  Build the tree nodes for Traceroute commands
 * *
 * * @param struct EwsCliCommandP
 * *
 * * @returntype void
 * *
 * * @notes none
 * *
 * * @end
 * *
 * *********************************************************************/
void buildTreeShowTunnelLoopback(EwsCliCommandP depth1)
{
#ifdef L7_IPV6_PACKAGE
    buildTreeShowTunnelInfo(depth1);
#endif
}

#endif /* L7_RLIM_PACKAGE */

/*********************************************************************
*
* @purpose  Build the tree for "clear ip helper statistics"
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreePrivClearIpHelperStatistics(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6;

  depth4 = ewsCliAddNode(depth3, "helper ", "Clear IP helper statistics", NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "statistics ", "Clear IP helper statistics", commandClearIpHelperStatistics, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "<cr>  ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree for "ip helper-address"
*
* @param EwsCliCommandP depth2   "ip"
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void buildTreeInterfaceIpHelperAddress(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, "helper-address ", "Configure Helper address entry",
                         commandIntfIpHelperAddressSet, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, "<ipaddr> ", "IPv4 address of server", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, "<udp-port> ", "Destination UDP port of packets to be relayed", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "dhcp ", "Relay DHCP (UDP port 67) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "domain ", "Relay DNS (UDP port 53) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "isakmp ", "Relay ISAKMP (UDP port 500) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "mobile-ip ", "Relay Mobile IP (UDP port 434) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "nameserver ", "Relay IEN-116 Name Service (UDP 42) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "netbios-dgm ", "Relay NetBIOS Datagram Server (UDP port 138) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "netbios-ns ", "Relay NetBIOS Name Server (UDP port 137) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "ntp ", "Relay network time protocol (UDP port 123) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "pim-auto-rp ", "Relay PIM auto RP (UDP port 496) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "rip ", "Relay RIP (UDP port 520) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "tacacs ", "Relay TACACS (UDP port 49) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "tftp ", "Relay TFTP (UDP port 69) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "time ", "Relay time service (UDP port 37) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* (no) ip helper-address <ipaddr> */
  depth5 = ewsCliAddNode(depth4, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* no ip helper-address */
  depth4 = ewsCliAddNode(depth3, "<cr> ", "Delete all helper addresses on this interface", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode(depth3, "discard ", "Drop matching packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, "<udp-port> ", "Destination UDP port of packets to be relayed", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "dhcp ", "Relay DHCP (UDP port 67) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "domain ", "Relay DNS (UDP port 53) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "isakmp ", "Relay ISAKMP (UDP port 500) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "mobile-ip ", "Relay Mobile IP (UDP port 434) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "nameserver ", "Relay IEN-116 Name Service (UDP 42) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "netbios-dgm ", "Relay NetBIOS Datagram Server (UDP port 138) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "netbios-ns ", "Relay NetBIOS Name Server (UDP port 137) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "ntp ", "Relay network time protocol (UDP port 123) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "pim-auto-rp ", "Relay PIM auto RP (UDP port 496) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "rip ", "Relay RIP (UDP port 520) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "tacacs ", "Relay TACACS (UDP port 49) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "tftp ", "Relay TFTP (UDP port 69) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "time ", "Relay time service (UDP port 37) packets", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* (no) ip helper-address discard */
  depth5 = ewsCliAddNode(depth4, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);  
}

