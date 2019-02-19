/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/ospf/clicommands_ospf.c
*
* @purpose create the cli commands for OSPF functions
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
#include "cli_web_exports.h"

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ospf_api.h"
#include "usmdb_ip_api.h"
#include "clicommands_l3.h"

#include "clicommands_loopback.h"

#include "datatypes.h"

#include "usmdb_mib_ospf_api.h"
#include "usmdb_util_api.h"
#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"

/* Prototype for OSPF Route Redistribution Tree Function */

void buildTreeOSPFRouteRedistributionMetricType(EwsCliCommandP depth1);
void buildTreeOSPFRouteRedistributionTopMetricType(EwsCliCommandP depth1);
void buildTreeOSPFRouteRedistributionMetric(EwsCliCommandP depth1);
void buildTreeOSPFRouteRedistributionProtocol(EwsCliCommandP depth2,L7_REDIST_RT_INDICES_t protocol);

void buildTreeOSPFRouteRedistribution(EwsCliCommandP depth1);
void buildTreeOSPFDefaultMetric(EwsCliCommandP depth1);
void buildTreeOSPFDistributeList(EwsCliCommandP depth1);

void buildTreeShowOspfDatabaseAdvRouter(EwsCliCommandP depth0)
{
  EwsCliCommandP depth1, depth2;

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_AdvRtr_1, pStrInfo_common_OspfDbAdvrtr, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Rtrid, pStrInfo_common_DiffservIpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* (or) */
  depth1 = ewsCliAddNode(depth0, pStrInfo_common_SelfOriginate, pStrInfo_common_OspfDbSelfOrig, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

void buildTreeShowOspfDatabaseLsId(EwsCliCommandP depth0)
{
  EwsCliCommandP depth1;

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_Lsid, pStrInfo_common_OspfDbLsid, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseAdvRouter(depth1);
}

void buildTreeShowOspfDatabaseLsType(EwsCliCommandP depth0)
{
  EwsCliCommandP depth1, depth2;

  depth1 = ewsCliAddNode(depth0, pStrInfo_routing_AsbrSummary, pStrInfo_routing_OspfDbAsbr, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_External, pStrInfo_common_OspfDbExternal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_WsShowRunningNwMode, pStrInfo_common_OspfDbNw, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_NssaExternal_1, pStrInfo_common_OspfDbNssa, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_OpaqueArea, pStrInfo_common_OspfDbAreaOpaque, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_OpaqueAs, pStrInfo_common_OspfDbAsOpaque, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_OpaqueLink, pStrInfo_common_OspfDbLinkOpaque, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);
  
  depth1 = ewsCliAddNode(depth0, pStrInfo_common_Rtr_3, pStrInfo_common_OspfDbRtr, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);

  depth1 = ewsCliAddNode(depth0, pStrInfo_common_Dot1xShowSummary, pStrInfo_routing_OspfDbSummary, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);
}

void buildTreeShowOspfDatabase(EwsCliCommandP depth0)
{
  EwsCliCommandP depth1, depth2, depth3;

  /* optional areaid "show ip ospf [<areaid>] database [database-summary]" */
  depth1 = ewsCliAddNode(depth0, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dbase, pStrInfo_common_ShowRtrOspfLsaDbase, commandShowIpOspfDatabase, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsType(depth2);
  buildTreeShowOspfDatabaseLsId(depth2);
  buildTreeShowOspfDatabaseAdvRouter(depth2);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_DbaseSummary, pStrInfo_common_ShowIpOspfDbaseSumm, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* "show ip ospf database [database-summary]" */
  depth1 = ewsCliAddNode(depth0, pStrInfo_common_Dbase, pStrInfo_common_ShowRtrOspfLsaDbase, commandShowIpOspfDatabase, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth1, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabaseLsType(depth1);
  buildTreeShowOspfDatabaseLsId(depth1);
  buildTreeShowOspfDatabaseAdvRouter(depth1);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DbaseSummary, pStrInfo_common_ShowIpOspfDbaseSumm, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

/*********************************************************************
*
* @purpose  Build the Show OSPF tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowOspf(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8;
#ifdef L7_RLIM_PACKAGE
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#endif

/* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ospf_1, pStrInfo_common_ShowRtrOspfInfo, commandShowIpOspf, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Area_2, pStrInfo_common_ShowRtrOspfArea, commandShowIpOspfArea, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Abr, pStrInfo_common_ShowIpOspfAbr, commandShowIpOspfABRs, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Asbr, pStrInfo_common_ShowIpOspfAsbr, commandShowIpOspfASBRs, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  buildTreeShowOspfDatabase(depth4);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_common_ShowRtrOspfIntf, commandShowIpOspfInterface, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Brief, pStrInfo_common_ShowIpOspfIntfBrief, commandShowIpOspfInterfaceBrief, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  /* loopback */
#ifdef L7_RLIM_PACKAGE
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_LoopBack, pStrInfo_common_ShowLoopBackIntf, commandShowIpOspfInterface, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, buf, pStrInfo_common_LoopBackIntfId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Stats, pStrInfo_common_ShowRtrOspfIntfStats, commandShowIpOspfInterfaceStats, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = buildTreeInterfaceHelp(depth6, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Neighbor_1, pStrInfo_common_ShowRtrOspfNeighbor, commandShowIpOspfNeighbor, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_common_ShowIpOspfNeighborIf, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = buildTreeInterfaceHelp(depth6, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Ipaddr, pStrInfo_common_ShowRtrOspfNeighborId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipaddr, pStrInfo_common_ShowRtrOspfNeighborId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclRangeStr, pStrInfo_common_ShowRtrOspfAreaRange, commandShowIpOspfAreaRange, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dot1xShowStats, pStrInfo_routing_ShowIpOspfStats, commandShowIpOspfStatistics, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Stub, pStrInfo_common_ShowRtrOspfStub, commandShowIpOspfStubTable, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Tbl_2, pStrInfo_common_ShowRtrOspfStubTbl, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_VirtualLink_1, pStrInfo_common_ShowRtrOspfVirtualIntf, commandShowIpOspfVirtualLinks, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Neighbor, pStrInfo_common_EnterNeighbourIpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Brief, pStrInfo_common_ShowIpOspfVirtualLinksBrief, commandShowIpOspfVirtualLinksBrief, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

/*********************************************************************
*
* @purpose  To build the Router ospf config mode
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeRouterConfigOspf()
{
  EwsCliCommandP depth1;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliOSPRouterConfigMode, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  cliSetMode(L7_ROUTER_CONFIG_OSPF_MODE, depth1);

  buildTreeRouterOspf(depth1);
}
/*********************************************************************
*
* @purpose  Build the Router Ospf config Tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRouterOspf(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 help[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_1583compatibility, pStrInfo_routing_IpOspf, command1583Compatibility, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_WsShowRunningNwMode, pStrInfo_routing_IpOspfNw, commandNetworkArea, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Wildcardmask, pStrInfo_routing_IpOspf_1, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Area_2, pStrInfo_common_IpOspfArea, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Area_2, pStrInfo_common_IpOspfArea, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Areaid, pStrInfo_common_EnterAreaId, commandAreaAreaId, 6, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);

#if 0
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_common_IpOspfAreaAuth, commandIpOspfAreaAreaIdAuthentication, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Encrypt_2, pStrInfo_common_EnterAuthTypeEncrypt, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_None_3, pStrInfo_common_EnterAuthTypeNone, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Simple_2, pStrInfo_common_EnterAuthTypesSimple, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
#endif

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_DeflCost, pStrInfo_common_CfgRtrOspfAreaStubMetricDeflCost, commandAreaAreaIdDefaultCost, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to16777215, pStrInfo_common_IpOspfAreaDeflRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Nssa_1, pStrInfo_common_IpOspfAreaNssa, commandAreaNSSA, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_DeflInfoOriginate_1, pStrInfo_common_IpOspfAreaNssaDeflInfoOr, commandAreaNSSADefaultInfoOriginate, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Range1to16777214, pStrInfo_common_IpOspfAreaNssaMetric, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Comparable_1, pStrInfo_common_IpOspfAreaNssaMetricType1, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_NonComparable_1, pStrInfo_common_IpOspfAreaNssaMetricType2, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_NoRedistribute_1, pStrInfo_common_IpOspfAreaNssaNoRedist, commandAreaNSSANoRedistribute, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_NoSummary, pStrInfo_common_IpOspfAreaNssaNoSummary, commandAreaNSSANoSummary, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_TranslatorRole_1, pStrInfo_common_IpOspfAreaNssaTrans, commandAreaNSSATranslatorRole, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Always_1, pStrInfo_common_IpOspfAreaNssaTrans_1, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Candidate_1, pStrInfo_common_IpOspfAreaNssaTrans_1, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_TranslatorStabIntv, pStrInfo_common_IpOspfAreaNssaStabilityIntvl, commandAreaNSSATranslatorStabilityInterval, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to3600, pStrInfo_common_IpOspfAreaNssaStabilityIntvlParms, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclRangeStr, pStrInfo_common_IpOspfAreaRange, commandAreaAreaIdRange, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, 6, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Subnetmask, pStrInfo_routing_IpOspfSubnetMask, NULL, 6, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_SummaryLink_1, pStrInfo_common_IpOspfAreaRangeLinkStateDbaseSumm, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Advertise_1, pStrInfo_common_IpOspfAreaRangeAdtype, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_NoAdvertise, pStrInfo_common_IpOspfAreaRangeNoAdtype, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_NssaExternalLink, pStrInfo_common_IpOspfAreaRangeLinkStateDbaseNssaExt, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Advertise_1, pStrInfo_common_IpOspfAreaRangeAdtype, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_NoAdvertise, pStrInfo_common_IpOspfAreaRangeNoAdtype, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Stub, pStrInfo_common_IpOspfAreaStub, commandAreaAreaIdStub, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_NoSummary, pStrInfo_common_CfgRtrOspfAreaStubSummaryLsa, commandAreaAreaIdStubSummaryLSA, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_VirtualLink_1, pStrInfo_common_IpOspfVirtualIntfLink, commandAreaVirtualLink, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Neighborid, pStrInfo_common_EnterNeighbourIpAddr, NULL, 6, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Auth_1, pStrInfo_routing_IpOspfVirtualIntfAuth, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_None_3, pStrInfo_routing_IpOspfVirtualIntfAuthNone, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Simple_2, pStrInfo_routing_IpOspfVirtualIntfAuthSimple, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Key, pStrInfo_routing_IpOspfKey, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Encrypt_2,pStrInfo_routing_IpOspfVirtualIntfAuthEncrypt, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Key, pStrInfo_routing_IpOspfKey, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Range0to255, pStrInfo_routing_IpOspfKeyId, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth10= ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_DeadIntvl_1, pStrInfo_common_IpOspfVirtualIntfIntvlDead, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range1to65535, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_HelloIntvl_1, pStrInfo_common_IpOspfVirtualIntfIntvlHello, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range1to65535, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_ReTxIntvl_1, pStrInfo_common_IpOspfVirtualIntfIntvlRx, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range0to3600, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_TxDelay, pStrInfo_common_IpOspfVirtualIntfTransDelay, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range0to3600, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_OSPF_MIN_REF_BW, L7_OSPF_MAX_REF_BW );

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_AutoCost, pStrInfo_common_OspfAutoCost, commandAutoCost, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_RefBandwidth, pStrInfo_common_OspfRef, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_OspfRef_1, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Capability, pStrInfo_routing_CfgRtrOspfCapability, commandCapabilityOpaque, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Opaque,pStrInfo_routing_CfgRtrOspfOpaqueCapability, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4= ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DeflInfo, pStrInfo_common_IpOspfDeflInfo, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Originate, pStrInfo_common_IpOspfDeflInfoOr, commandDefaultInformationOriginateOspf, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Always_1, pStrInfo_common_IpOspfDeflInfoAlways, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Metric_1, pStrInfo_common_IpOspfDeflInfoMetric, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Range1to16777214, pStrInfo_common_IpOspfDeflInfoMetricVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Range1to2, pStrInfo_common_IpOspfDeflInfoMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range1to2, pStrInfo_common_IpOspfDeflInfoMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Metric_1, pStrInfo_common_IpOspfDeflInfoMetric, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
      
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Range1to16777214, pStrInfo_common_IpOspfDeflInfoMetricVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range1to2, pStrInfo_common_IpOspfDeflInfoMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MetricType_1, pStrInfo_common_IpOspfDeflInfoMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to2, pStrInfo_common_IpOspfDeflInfoMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  buildTreeOSPFDefaultMetric(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Distance, pStrInfo_common_IpDistance, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ospf_1, pStrInfo_common_IpOspfDistance, commandDistanceOspf, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_External, pStrInfo_common_IpOspfRouteTypeExternal, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to255, pStrInfo_routing_RipPrefRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Inter, pStrInfo_common_IpOspfRouteTypeInter, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to255, pStrInfo_routing_RipPrefRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Intra, pStrInfo_common_IpOspfRouteTypeIntra, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to255, pStrInfo_routing_RipPrefRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  buildTreeOSPFDistributeList(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Enbl_2, pStrInfo_common_IpOspfEnbl, commandEnableOspf, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode( depth1, pStrInfo_common_Exit,pStrInfo_common_ToExitMode, NULL, 0);
  depth3 = ewsCliAddNode( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_ExitOverflowIntvl_1, pStrInfo_common_CfgRtrOspfExitOverflowIntvl, commandExitOverflowInterval, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range0to2147483647, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_ExternalLsdbLimit, pStrInfo_common_CfgRtrOspfExtLinkStateDbaseLimit, commandExternalLsdbLimit, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_12147483647_1, pStrInfo_common_CfgRtrOspfExtLinkStateDbaseLimitRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_1_2, platRtrRouteMaxEqualCostEntriesGet());

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MaxPaths_1, pStrInfo_common_CfgMaxPaths, commandMaximumPaths, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(help, sizeof(help),  pStrInfo_common_EnterMaxPaths, platRtrRouteMaxEqualCostEntriesGet());

  depth3 = ewsCliAddNode(depth2, buf, help, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_RtrId_2, pStrInfo_common_CfgRtrId, commandRouterId, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipaddress, pStrInfo_common_DiffservIpAddr, NULL, 6, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  buildTreeOSPFRouteRedistribution(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Timers, pStrInfo_routing_TimersOspf, commandOspfSpfTimers, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Spf, pStrInfo_routing_SpfOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to65535, pStrInfo_routing_OspfSpfDelay, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_routing_OspfSpfHoldTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_PassiveIntf, pStrInfo_common_OspfPass_1, commandOspfPassive, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_PassDefl, pStrInfo_common_OspfPass, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth3 = buildTreeInterfaceHelp(depth2, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeRouterOspfTrapflags(depth1);
}

/*********************************************************************
*
* @purpose  Build the Router OSPF TRAPFLAGS  config Tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRouterOspfTrapflags(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Trapflags, pStrInfo_common_TrapFlagsOspf, commandTrapflagsOspf, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    
  depth3 = ewsCliAddNode(depth2, pStrErr_common_OspfTrapErrs, pStrErr_common_TrapErrsOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrErr_common_OspfTrapAuthFailure, pStrErr_common_TrapIfAuthFailureOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapBadPkt, pStrInfo_common_TrapRxBadPktOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrErr_common_OspfTrapCfg, pStrErr_common_TrapIfCfgErrorOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrErr_common_OspfTrapVirtAuthFailure, pStrErr_common_TrapVirtualIntfAuthFailureOspf, NULL, 4, L7_KEY_RESTRICTED_NODE,L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapVirtBadPkt, pStrInfo_common_TrapVirtBadPktOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrErr_common_OspfTrapVirtCfg, pStrErr_common_TrapVirtualIntfCfgErrorOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


  depth3 = ewsCliAddNode(depth2, pStrInfo_common_OspfTrapLsa, pStrInfo_common_TrapLsaOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapMaxAgeLsa, pStrInfo_common_TrapMaxAgeLsaOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapOriginateLsa, pStrInfo_common_TrapOriginateLsaOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_OspfTrapOverflow, pStrInfo_common_TrapOverflowOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrap, pStrInfo_common_TrapLinkStateDbaseOverflowOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapDbAp, pStrInfo_common_TrapLinkStateDbaseAp, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_OspfTrapReTx, pStrInfo_common_TrapReTxOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapTxReTx, pStrInfo_common_TrapTxReTxOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapVirtIfTxReTx, pStrInfo_common_TrapVirtualIntfTxReTxOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_OspfTrapStateChg, pStrInfo_common_TrapsStateChgOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_TrapAllOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapIfStateChg, pStrInfo_common_TrapIfStateChgOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapNbrStateChg, pStrInfo_common_TrapNbrStateChgOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapVirtIfStateChg, pStrInfo_common_TrapVirtualIntfStateChgOspf, NULL,4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapVirtNbrStateChg, pStrInfo_common_TrapVirtNbrStateChgOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/**************************OSPF - NET DIRECT BROADCAST INTERFACE TREE **********/

/*********************************************************************
*
* @purpose  Build the Routing Interface NetDirect Broadcast and OSPF tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceNetDbOspf(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Netdirbcast, pStrInfo_routing_IpIntfNetDirBcast, commandNetDirectBcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ospf_1, pStrInfo_common_CfgRtrOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Area_2, pStrInfo_common_CfgRtrOspfIntfAreaId, commandIpOspfAreaid, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond_1, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ABCD, pStrInfo_common_EnterIpAddrAreaId, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID );
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_EnterAreaIdInDecimal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID );
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_routing_IpOspfIntfNoAuth, commandIpOspfAuthenticationNone, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Encrypt_2, pStrInfo_routing_CfgRtrOspfIntfAuthEncrypt, commandIpOspfAuthenticationEncrypt, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Key, pStrInfo_routing_IpOspfKey, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_routing_Keyid, pStrInfo_routing_IpOspfKeyId, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_None_3, pStrInfo_routing_CfgRtrOspfIntfAuthNone, commandIpOspfAuthenticationNone, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Simple_2, pStrInfo_routing_CfgRtrOspfIntfAuthSimple, commandIpOspfAuthenticationSimple, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Key, pStrInfo_routing_IpOspfKey, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cost2, pStrInfo_common_CfgRtrOspfIntfCost, commandIpOspfCost, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to65535, pStrInfo_common_CfgRtrOspfIntfCostRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_DeadIntvl_1, pStrInfo_common_CfgRtrOspfIntfIntvlDead, commandIpOspfDeadInterval, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to65535, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_HelloIntvl_1, pStrInfo_common_CfgRtrOspfIntfIntvlHello, commandIpOspfHelloInterval, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to65535, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MtuIgnore_1, pStrInfo_common_IpOspfMtuIgnore, commandIpOspfMtuIgnore, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SwitchPri_1, pStrInfo_common_CfgRtrOspfIntfPri, commandIpOspfPriority, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_common_CfgRtrOspfIntfPriRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ReTxIntvl_1, pStrInfo_common_CfgRtrOspfIntfIntvlReTx, commandIpOspfRetransmitInterval, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to3600, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_TxDelay, pStrInfo_common_CfgRtrOspfIntfIfTransitDelay, commandIpOspfTransmitDelay, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to3600, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, "network ", pStrInfo_common_CfgRtrOspfIntfNetwork, commandIpOspfNetwork, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, "broadcast ", pStrInfo_common_CfgRtrOspfIntfNetworkBroadcast, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, "point-to-point ", pStrInfo_common_CfgRtrOspfIntfNetworkP2P, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}


void buildTreeOSPFRouteRedistributionTopMetricType(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MetricType_1, pStrInfo_common_OspfRedistributeMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_12, pStrInfo_routing_OspfRedistributeMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_OspfRedistributeTagVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /*No form*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
}

void buildTreeOSPFRouteRedistributionMetricType(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MetricType_1, pStrInfo_common_OspfRedistributeMetricType, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_12, pStrInfo_routing_OspfRedistributeMetricTypeVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_OspfRedistributeTagVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

void buildTreeOSPFRouteRedistributionMetric(EwsCliCommandP depth1)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_OSPF_REDIST_METRIC_MIN, L7_OSPF_REDIST_METRIC_MAX);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Metric_1, pStrInfo_common_OspfRedistributeMetric, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_common_OspfRedistributeMetricVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  buildTreeOSPFRouteRedistributionMetricType(depth3);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_OspfRedistributeTagVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

/* Tree specific for no command STARTS*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_MetricType_1, pStrInfo_common_OspfRedistributeMetricType, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
/* Tree specific for no command ENDS*/
}

void buildTreeOSPFRouteRedistributionProtocol(EwsCliCommandP depth2,L7_REDIST_RT_INDICES_t protocol)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  unit = cliGetUnitId();

  if (protocol == REDIST_RT_BGP && usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bgp_1, pStrInfo_common_RipRedistributeProtoBgp, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  else if (protocol == REDIST_RT_RIP)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Rip3, pStrInfo_common_BgpRedistributeProtoRip, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  else if (protocol == REDIST_RT_STATIC)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Static2, pStrInfo_common_BgpRedistributeProtosTatic, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  else if (protocol == REDIST_RT_LOCAL)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Connected_1, pStrInfo_common_BgpRedistributeProtoConnected, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  else
  {
    return;
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeOSPFRouteRedistributionMetric(depth3);

  buildTreeOSPFRouteRedistributionTopMetricType(depth3);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Tag_1, pStrInfo_common_OspfRedistributeTag, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_OspfRedistributeTagVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /*No form only*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Subnets_1, pStrInfo_routing_OspfRedistributeSubnet, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

}

/*********************************************************************
*
* @purpose  Build the OSPF Route-Redistribution tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeOSPFRouteRedistribution(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Redistribute_1, pStrInfo_common_OspfRedistribute, commandRedistributeOSPF, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeOSPFRouteRedistributionProtocol(depth2,REDIST_RT_BGP);
  }

  buildTreeOSPFRouteRedistributionProtocol(depth2,REDIST_RT_RIP);

  buildTreeOSPFRouteRedistributionProtocol(depth2,REDIST_RT_STATIC);

  buildTreeOSPFRouteRedistributionProtocol(depth2,REDIST_RT_LOCAL);
}

/*********************************************************************
*
* @purpose  Build the OSPF 'Default-metric' tree for Route-Redistribution.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeOSPFDefaultMetric(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DeflMetric_1, pStrInfo_common_OspfRedistributeDeflMetric, commandDefaultMetricOSPF, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Range1to16777214, pStrInfo_common_OspfRedistributeDeflMetricVal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

/*********************************************************************
*
* @purpose  Build the OSPF 'Distribute-list' tree for Route-Redistribution.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeOSPFDistributeList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DistributeList_1, pStrInfo_routing_OspfRedistributeDistributeList, commandDistributeListOSPF, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range1to199, pStrInfo_routing_RipRedistributeListFilter, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclOutStr, pStrInfo_routing_RipRedistributeDistributeListOut, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Bgp_1, pStrInfo_common_RipRedistributeProtoBgp, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Rip3, pStrInfo_common_BgpRedistributeProtoRip, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Static2, pStrInfo_common_BgpRedistributeProtosTatic, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Connected_1, pStrInfo_common_BgpRedistributeProtoConnected, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the clear ip ospf tree.
*
*
* @param EwsCliCommandP depth1    pointer to "clear ip" node
*
* @returntype void
*
* @notes      If user gives an optional keyword, only one can be given.
*             The optional keywords are redistribution, counters, and neighbor.
*
* @end
*
*********************************************************************/
void buildTreeClearIpOspf(EwsCliCommandP clearIp)
{
  EwsCliCommandP clearIpOspf, clearIpOspfRedist, clearIpOspfCounters;
  EwsCliCommandP clearIpOspfConfig;
  EwsCliCommandP clearIpOspfNbr, clearIpOspfNbrIf, clearIpOspfNbrIfIf;
  EwsCliCommandP clearIpOspfNbrIfRid;
  EwsCliCommandP clearIpOspfNbrRid;
  EwsCliCommandP cr;

  /* clear ip ospf */
  clearIpOspf = ewsCliAddNode(clearIp, "ospf", "Clear OSPF protocol", 
                              commandClearIpOspf, 2, L7_KEY_RESTRICTED_NODE, 
                              L7_OSPF_MAP_COMPONENT_ID);
  cr = ewsCliAddNode(clearIpOspf, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                     L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf configuration */
  clearIpOspfConfig = ewsCliAddNode(clearIpOspf, "configuration", "Restore OSPF configuration to defaults",
                                    commandClearIpOspfConfiguration, 2, L7_KEY_RESTRICTED_NODE, 
                                    L7_OSPF_MAP_COMPONENT_ID);
  cr = ewsCliAddNode(clearIpOspfConfig, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                     L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf redistribution */
  clearIpOspfRedist = ewsCliAddNode(clearIpOspf, "redistribution", "Flush and reoriginate external LSAs",
                                    commandClearIpOspfRedistribution, 2, L7_KEY_RESTRICTED_NODE, 
                                    L7_OSPF_MAP_COMPONENT_ID);
  cr = ewsCliAddNode(clearIpOspfRedist, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                     L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf counters */
  clearIpOspfCounters = ewsCliAddNode(clearIpOspf, "counters", "Clear OSPF counters",
                                      commandClearIpOspfCounters, 2, L7_KEY_RESTRICTED_NODE, 
                                      L7_OSPF_MAP_COMPONENT_ID);
  cr = ewsCliAddNode(clearIpOspfCounters, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                     L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf neighbor */
  clearIpOspfNbr = ewsCliAddNode(clearIpOspf, "neighbor", "Bounce all OSPF neighbors",
                                 commandClearIpOspfNeighbor, 2, L7_KEY_RESTRICTED_NODE, 
                                 L7_OSPF_MAP_COMPONENT_ID);
  cr = ewsCliAddNode(clearIpOspfNbr, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                     L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf neighbor interface <usp> */
  clearIpOspfNbrIf = ewsCliAddNode (clearIpOspfNbr, "interface", 
                                    pStrInfo_common_ShowIpOspfNeighborIf, NULL, 2, 
                                    L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  clearIpOspfNbrIfIf = buildTreeInterfaceHelp(clearIpOspfNbrIf, 2, L7_KEY_RESTRICTED_NODE, 
                                              L7_OSPF_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  ewsCliAddNode(clearIpOspfNbrIfIf, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf neighbor interface <usp> <router-id> */
  clearIpOspfNbrIfRid = ewsCliAddNode(clearIpOspfNbrIfIf, pStrInfo_common_Ipaddr, 
                                      pStrInfo_common_ShowRtrOspfNeighborId, 
                                      NULL, 4, L7_KEY_RESTRICTED_NODE, 
                                      L7_OSPF_MAP_COMPONENT_ID, L7_NODE_DATA_TYPE, 
                                      L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddNode(clearIpOspfNbrIfRid, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* clear ip ospf neighbor <router-id> */
  clearIpOspfNbrRid = ewsCliAddNode(clearIpOspfNbr, pStrInfo_common_Ipaddr, 
                                    pStrInfo_common_ShowRtrOspfNeighborId, 
                                    NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, 
                                    L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddNode(clearIpOspfNbrRid, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
}

