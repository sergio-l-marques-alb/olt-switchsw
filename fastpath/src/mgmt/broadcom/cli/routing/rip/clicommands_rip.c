/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/rip/clicommands_rip.c
*
* @purpose create the cli commands for RIP functions
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"
#include "clicommands_l3.h"

#include "datatypes.h"

#include "usmdb_util_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"


/* Prototype for RIP Route Redistribution Tree Function */
void buildTreeRIPRouteRedistribution(EwsCliCommandP depth1);
void buildTreeRIPDefaultMetric(EwsCliCommandP depth1);
void buildTreeRIPDistributeList(EwsCliCommandP depth1);
EwsCliCommandP buildTreeRIPRouteRedistributionMetric(EwsCliCommandP depth1);
void buildTreeMatchOptions(EwsCliCommandP depth1, L7_uint32 count, EwsCliCommandOptions opt, L7_COMPONENT_IDS_t componentId);

/*********************************************************************
*
* @purpose  Build the Show RIP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowRIP(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;

/* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Rip3, pStrErr_routing_ShowRtrRipInfo, commandShowRIP, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrErr_routing_ShowRtrRipIntf, commandShowIpRIPInterface, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Brief, pStrErr_routing_ShowRtrRipIntfSummary, commandShowIpRIPInterfaceBrief, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  To build the Router RIP config mode
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
void buildTreeRouterConfigRIP()
{
  EwsCliCommandP depth1;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliRIPRouterConfigMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_ROUTER_CONFIG_RIP_MODE, depth1);

  buildTreeRouterRIP(depth1);
}

/*********************************************************************
*
* @purpose  Build the Router RIP config Tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRouterRIP(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_char8 rangeBuf[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(rangeBuf, sizeof(rangeBuf),  "<%d-%d> ", L7_RTO_PREFERENCE_MIN, L7_RTO_PREFERENCE_MAX);

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_AutoSummary,pStrErr_routing_CfgRtrRipAutoSummary, commandRIPAutoSummary, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DeflInfo, pStrInfo_routing_IpRipIntfDeflInfo, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Originate, pStrInfo_routing_IpRipIntfDeflInfo, commandDefaultInformationOriginateRip, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreeRIPDefaultMetric(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Distance, pStrInfo_routing_IpRipDistance, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Rip3, pStrInfo_routing_IpRipDistance, commandDistanceRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, rangeBuf, pStrInfo_routing_RipPrefRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeRIPDistributeList(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Enbl_2, pStrInfo_routing_IpRipEnbl, commandEnableRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Hostroutesaccept, pStrErr_routing_CfgRtrRipHostRoutesAccept, commandRIPHostRoutesAccept, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreeRIPRouteRedistribution(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_routing_SplitHorizon, pStrInfo_routing_IpRipSplitHorizonMode, commandRIPSplitHorizon, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_None_3, pStrInfo_routing_IpRipSplitHorizonModeNone, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Poison, pStrInfo_routing_IpRipSplitHorizonModePoison, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Simple_2, pStrInfo_routing_IpRipSplitHorizonModeSimple, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Routing Interface RIP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceRip(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Rip3, pStrErr_routing_CfgRtrRip, commandIpRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_routing_IpRipIntfNoAuth, commandRIPAuthenticationNone, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Encrypt_2, pStrErr_routing_CfgRtrRipIntfAuthEncrypt_1, commandRIPAuthenticationEncrypt, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Key, pStrErr_routing_CfgRtrRipKey, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_routing_Keyid, pStrErr_routing_CfgRtrRipKeyId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_None_3, pStrErr_routing_CfgRtrRipIntfAuthNone_1, commandRIPAuthenticationNone, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Simple_2, pStrErr_routing_CfgRtrRipIntfAuthSimple_1, commandRIPAuthenticationSimple, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Key, pStrErr_routing_CfgRtrRipKey, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Receive_1, pStrErr_routing_CfgRtrRipIntfVerRecieve, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ver_2, pStrErr_routing_CfgRtrRipIntfVer, commandIpRIPReceiveVersion, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Both_1, pStrErr_routing_CfgRtrRipIntfVerRecieveBoth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrErr_routing_CfgRtrRipIntfVerRecieveNone, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Rip12, pStrErr_routing_CfgRtrRipIntfVerReceiverIp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Rip2_1, pStrErr_routing_CfgRtrRipIntfVerReceiverIp2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Send_1, pStrErr_routing_CfgRtrRipIntfVerSend,NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ver_2, pStrErr_routing_CfgRtrRipIntfVer, commandIpRIPSendVersion, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_routing_IpRipIntfVerSendNone, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Rip12, pStrInfo_routing_IpRipIntfVerSendRip1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Rip1c2, pStrInfo_routing_IpRipIntfVerSendRip1c, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Rip2_1, pStrInfo_routing_IpRipIntfVerSendRip2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
*
* @purpose  Build the RIP Route-Redistribution tree (using option nodes)
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRIPRouteRedistribution(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  EwsCliCommandP tree3, tree4;
  EwsCliCommandP depth_ret, depth_new;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Redistribute_1, pStrInfo_routing_RipRedistribute, commandRedistributeRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Alphabetic Tree */
  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bgp_1, pStrInfo_common_RipRedistributeProtoBgp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth_ret = buildTreeRIPRouteRedistributionMetric(depth3);
    depth_new = ewsCliAddNode(depth_ret, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Connected_1, pStrInfo_common_BgpRedistributeProtoConnected, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth_ret = buildTreeRIPRouteRedistributionMetric(depth3);
  depth_new = ewsCliAddNode(depth_ret, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Static2, pStrInfo_common_BgpRedistributeProtosTatic, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth_ret = buildTreeRIPRouteRedistributionMetric(depth3);
  depth_new = ewsCliAddNode(depth_ret, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_OSPF_MAP_COMPONENT_ID) == L7_TRUE)
  {
    tree3 = ewsCliAddNode(depth2, pStrInfo_common_Ospf_1, pStrInfo_common_BgpRedistributeProtoOspf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    tree4 = ewsCliAddNode(tree3, pStrInfo_common_Match_1, pStrInfo_common_RedistributeMatch, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(tree4, pStrInfo_common_Internal_2, pStrInfo_routing_RipRedistributeOspfMatchInternal, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, tree4);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(tree4, pStrInfo_common_External, pStrInfo_routing_RipRedistributeOspfMatchExternal, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_MAX_DISPLAY, 2);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_RateMbps, pStrInfo_common_BgpRedistributeOspfMatchExtType1, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, tree4);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Rate2Mbps_1, pStrInfo_common_BgpRedistributeOspfMatchExtType2, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, tree4);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(tree4, pStrInfo_common_NssaExternal_1, pStrInfo_routing_RipRedistributeOspfMatchNssaExternal, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_MAX_DISPLAY, 2);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_RateMbps, pStrInfo_common_BgpRedistributeOspfMatchNssaExtType1, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, tree4);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Rate2Mbps_1, pStrInfo_common_BgpRedistributeOspfMatchNssaExtType2, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, tree4);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(tree3, pStrInfo_common_Metric_1, pStrInfo_routing_RipRedistributeMetric, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to15, pStrInfo_routing_RipRedistributeMetricVal, NULL, 2, L7_OPTIONS_NODE, tree3);
    depth4 = ewsCliAddNode(tree3, pStrInfo_common_Metric_1, pStrInfo_routing_RipRedistributeMetric, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY, L7_OPTIONS_NODE, tree3);

    depth4 = ewsCliAddNode(tree3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

EwsCliCommandP buildTreeRIPRouteRedistributionMetric(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Metric_1, pStrInfo_routing_RipRedistributeMetric, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range1to15, pStrInfo_routing_RipRedistributeMetricVal, NULL, L7_NO_OPTIONAL_PARAMS);

  return depth3;
}

/*********************************************************************
*
* @purpose  Build the RIP 'Default-metric' tree for Route-Redistribution.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRIPDefaultMetric(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 rangeBuf[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(rangeBuf, sizeof(rangeBuf),  "<%d-%d> ", L7_RIP_CONF_METRIC_MIN, L7_RIP_CONF_METRIC_MAX);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DeflMetric_1, pStrInfo_routing_RipRedistributeDeflMetric, commandDefaultMetricRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, rangeBuf, pStrInfo_routing_RipRedistributeDeflMetricVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the RIP 'Distribute-list' tree for Route-Redistribution.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRIPDistributeList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DistributeList_1, pStrInfo_routing_RipRedistributeDistributeList, commandDistributeListRIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range1to199, pStrInfo_routing_RipRedistributeListFilter, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclOutStr, pStrInfo_routing_RipRedistributeDistributeListOut, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Bgp_1, pStrInfo_common_RipRedistributeProtoBgp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Connected_1, pStrInfo_common_BgpRedistributeProtoConnected, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_OSPF_MAP_COMPONENT_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ospf_1, pStrInfo_common_BgpRedistributeProtoOspf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Static2, pStrInfo_common_BgpRedistributeProtosTatic, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

