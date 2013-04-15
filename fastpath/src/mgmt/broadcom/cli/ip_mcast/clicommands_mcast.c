/*********************************************************************
 *    VV  VV LL   7   77   All Rights Reserved.
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/clicommands_mcast.c
 *
 * @purpose create the cli tree for mcast
 *
 * @component user interface
 *
 *
 * @create
 *
 * @author  srikrishnas
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include "clicommands.h"
#include "clicommands_mcast.h"
#include "cli_web_exports.h"

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface config DVMRP & IGMP commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Interface config DVMRP & IGMP commands
*
* @end
*
*********************************************************************/
void cliTreeInterfaceConfigDvmrpIgmp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  unit = cliGetUnitId();

  /* depth1 = "ip" */
  depth3 = ewsCliAddNode(depth2, pStrInfo_ipmcast_Dvmrp_2, pStrInfo_ipmcast_Dvmrp, commandIpDvmrpUnicastRouting, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Metric_1, pStrInfo_ipmcast_CfgRtrDvmrpIntfMetric, commandIpDvmrpMetric, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf),  pStrInfo_ipmcast_CfgRtrDvmrpIntfMetricParams,
                L7_DVMRP_INTERFACE_METRIC_MIN, L7_DVMRP_INTERFACE_METRIC_MAX);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Val, buf, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmp_2, pStrInfo_ipmcast_Igmp, commandIpIgmpInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LastMbrQueryCount_1, pStrInfo_ipmcast_IpIgmpLastMbrQueryCount, commandIpIgmpLastMemberQueryCount, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to20, pStrInfo_ipmcast_CfgRtrIgmpLastMemQueryCountType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LastMbrQueryIntvl_1, pStrInfo_ipmcast_IpIgmpLastMbrQueryIntvl, commandIpIgmpLastMemberQueryInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_ipmcast_CfgRtrIgmpLastMemQueryIntvlType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_QueryIntvl_1, pStrInfo_ipmcast_IpIgmpQueryIntvl, commandIpIgmpQueryInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to3600, pStrInfo_ipmcast_CfgRtrIgmpIntfIntvlType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_QueryMaxRespTime_1, pStrInfo_ipmcast_IpIgmpQueryMaxRespTime, commandIpIgmpQueryMaxResponseTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_ipmcast_CfgRtrIgmpMaxRespTimeType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Robustness_1, pStrInfo_ipmcast_IpIgmpRobustness, commandIpIgmpRobustness, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to255, pStrInfo_ipmcast_CfgRtrIgmpRobustnessType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_StartupQueryCount_1, pStrInfo_ipmcast_IpIgmpStartupQueryCount, commandIpIgmpStartupQueryCount, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to20, pStrInfo_ipmcast_CfgRtrIgmpIntfsStartupQueryCountType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_StartupQueryIntvl, pStrInfo_ipmcast_IpIgmpStartupQueryIntvl, commandIpIgmpStartupQueryInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Range1to300, pStrInfo_ipmcast_CfgRtrIgmpIntfsStartupQueryIntType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ver_2, pStrInfo_ipmcast_IpIgmpVer, commandIpIgmpVersion, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_RateMbps, pStrInfo_ipmcast_CfgRtrIgmpVerType1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Rate2Mbps_1, pStrInfo_ipmcast_CfgRtrIgmpVerType2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Num3, pStrInfo_ipmcast_CfgRtrIgmpVerType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_ipmcast_IgmpProxy_1, pStrInfo_ipmcast_CfgRtrIgmpProxy,
                         commandIpIgmpProxyInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_ResetStatus, pStrInfo_ipmcast_IpIgmpProxyResetStatus,
                         commandIpIgmpProxyResetHostStatus,L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_UnsolicitRprtIntvl,
                         pStrInfo_ipmcast_IpIgmpProxyUnSolicitedIntvl,
                         commandIpIgmpProxyUnsolicitedReportInterval,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, NULL,
                         pStrInfo_ipmcast_CfgMgmdProxyIntfUnSolicitedIntType, NULL,
                         3, L7_NODE_UINT_RANGE, L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MIN, L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MAX);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface config Multicast commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Interface config Multicast commands
*
* @end
*
*********************************************************************/
void cliTreeInterfaceConfigMcast(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  /* depth2 = "ip " */
  depth3 = ewsCliAddNode(depth2, pStrInfo_ipmcast_Mcast_2, pStrInfo_ipmcast_McastBoundary, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_Boundary, pStrInfo_ipmcast_CfgRtrMcastBoundary, commandIpMcastBoundary, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Grpipaddr, pStrInfo_ipmcast_CfgRtrMcastBoundaryIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Mask, pStrInfo_ipmcast_CfgRtrMcastBoundaryMask, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}
/*********************************************************************
*
* @purpose  Build the tree nodes for Interface config Multicast commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Interface config Multicast commands
*
* @end
*
*********************************************************************/
void cliTreeInterfaceConfigMulticast(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  /* depth2 = "ip " */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_ipmcast_McastRouting, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_TtlThresh_1, pStrInfo_ipmcast_CfgRtrMcastTtlThresh, commandIpMulticastTtlThreshold, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_ipmcast_CfgRtrMcastTtlThreshRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface config Pimdm & Pimsm commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Interface config Pimdm & Pimsm commands
*
* @end
*
*********************************************************************/
void cliTreeInterfaceConfigPimdmPimsm(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* depth2 = "config router" */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pimdm_1, pStrInfo_ipmcast_CfgRtrPimDm_1, commandIpPimdmMode, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#if 0
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_ipmcast_IpPimDmMode, commandIpPimdmMode, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#else
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_ipmcast_IpPimDmMode, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_HelloIntvl_1, pStrInfo_common_CfgRtrPimSmIntfHelloIntvl, commandIpPimdmHelloInterval, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Range10to3600, pStrInfo_common_CfgRtrPimDmHelloIntvl, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);

  /* Port Based Routing */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pimsm_1, pStrInfo_ipmcast_CfgRtrPimSm_2, commandIntfIpPimsm, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_BsrBorder_1, pStrInfo_common_IpPimSmBsrBorder, commandIpPimsmBsrBorder, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_DrPri_1, pStrInfo_common_IpPimSmDrPri, commandIpPimsmDrPriority, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647, pStrInfo_common_CfgRtrPimSmIntfDrPriRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_JoinPruneIntvl, pStrInfo_common_CfgRtrPimSmIntfMsgIntvl, commandIpPimsmJoinPruneInterval, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to18000, pStrInfo_common_CfgRtrPimSmIntfHelloIntvlRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_HelloIntvl_1, pStrInfo_common_CfgRtrPimSmIntfHelloIntvl, commandIpPimsmHelloInterval, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to18000, pStrInfo_common_CfgRtrPimSmIntfHelloIntvlRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for User Show Dvmrp commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for User Show Dvmrp  commands
*
* @end
*
*********************************************************************/
void cliTreeUserShowDvmrp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;

  /* depth3 = "show" */
  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_Dvmrp_2, pStrInfo_ipmcast_ShowIpDvmrp, commandShowIpDvmrp, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_ipmcast_ShowIpDvmrpIntf, commandShowIpDvmrpInterface, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Neighbor_1, pStrInfo_ipmcast_ShowIpDvmrpNeighbor, commandShowIpDvmrpNeighbor, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Nexthop_2, pStrInfo_ipmcast_ShowIpDvmrpNextHop, commandShowIpDvmrpNexthop, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Prune_1, pStrInfo_ipmcast_ShowIpDvmrpPrune, commandShowIpDvmrpPrune, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Route, pStrInfo_ipmcast_ShowIpDvmrpRoute, commandShowIpDvmrpRoute, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID);

}

/*********************************************************************
*
* @purpose  Build the tree nodes for User Show  Igmp commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for User Show  Igmp commands
*
* @end
*
*********************************************************************/
void cliTreeUserShowIgmp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8, depth9;

  /* depth3 = "show" */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Igmp_2, pStrInfo_ipmcast_ShowRtrIgmp, commandShowIpIgmp, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Grps, pStrInfo_ipmcast_ShowRtrMgmdGrps, commandShowIpIgmpGroups, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_ipmcast_ShowRtrMgmdGroupsDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_ipmcast_ShowIpIgmpIntf, commandShowIpIgmpInterface, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_ipmcast_Mbrship, pStrInfo_ipmcast_ShowIpIgmpIntfMbrship, commandShowIpIgmpInterfaceMembership, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_Multiipaddr, pStrInfo_ipmcast_ShowRtrIgmpIntfMbrshipType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_IgmpProxyGrpsDetail,pStrInfo_ipmcast_ShowRtrMgmdGroupsDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Stats, pStrInfo_ipmcast_ShowIpIgmpIntfStats, commandShowIpIgmpInterfaceStats, L7_NO_OPTIONAL_PARAMS);
  depth7 = buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_IgmpProxy_1, pStrInfo_ipmcast_ShowRtrIgmpProxy, commandShowIpIgmpProxy, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Grps, pStrInfo_ipmcast_ShowRtrMgmdGrps, commandShowIpIgmpProxyGroups, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_ipmcast_ShowRtrMgmdGroupsDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1,pStrInfo_ipmcast_ShowRtrIgmpProxyIntf, commandShowIpIgmpProxyInterface, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for User Show Multicast commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for User Show Multicast commands
*
* @end
*
*********************************************************************/
void cliTreeUserShowMulticast(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8, depth9;

  /* depth3 = "show ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_ipmcast_Mcast_2, pStrInfo_ipmcast_ShowIpMcast, commandShowIpMcast, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_ipmcast_ShowIpMcastIntf, commandShowIpMcastInterface, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Mroute, pStrInfo_ipmcast_ShowIpMcastMroute, commandShowIpMcastMroute, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailed, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowSummary, pStrInfo_ipmcast_ShowRtrMcastMrouteSummary, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Grp_2, pStrInfo_ipmcast_ShowIpMcastMrouteGrp, commandShowIpMcastMrouteGroup, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_Grpipaddr, pStrInfo_ipmcast_CfgRtrMcastBoundaryIp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailed, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Dot1xShowSummary, pStrInfo_ipmcast_ShowRtrMcastMrouteSummary, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_SrcPortMirror, pStrInfo_ipmcast_ShowIpMcastMrouteSrc, commandShowIpMcastMrouteSource, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_Srcipaddr, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailedSrcType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Dot1xShowSummary, pStrInfo_ipmcast_ShowRtrMcastMrouteSummary, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailed, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Static2, pStrInfo_ipmcast_ShowRtrMcastStaticRoute, commandShowIpMcastMrouteStatic, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_Srcipaddr, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailedSrcType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_Boundary, pStrInfo_ipmcast_ShowRtrMcastBoundary, commandShowIpMcastBoundary, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for User Show Pimdm & Pimsm commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for User Show Pimdm & Pimsm commands
*
* @end
*
*********************************************************************/
void cliTreeUserShowPimdmPimsm(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8;
  L7_uint32 unit;

  /* depth3 = "show" */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pimdm_1, pStrInfo_ipmcast_ShowIpPimDm_1, commandShowIpPimdm, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_common_ShowIpPimDmIntf, commandShowIpPimdmInterface, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Stats, pStrInfo_ipmcast_ShowIpPimDmIntfStats, commandShowIpPimdmInterfaceStats, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth7 = buildTreeInterfaceHelp(depth6, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Neighbor_1, pStrInfo_common_ShowIpPimDmNeighbor, commandShowIpPimdmNeighbor, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID);

  /* Port Based Routing */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pimsm_1, pStrInfo_ipmcast_ShowIpPimSm, commandShowIpPimsm, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Bsr, pStrInfo_common_ShowIpPimSmBsrTbl, commandShowIpPimsmBsr, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_ipmcast_ShowIpPimSm, commandShowIpPimsmInterface, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Neighbor_1, pStrInfo_common_ShowIpPimSmNeighbor, commandShowIpPimsmNeighbor, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_All,pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Rp, pStrInfo_common_ShowIpPimSmRp, commandShowIpPimsmRpMapping, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Mapping, pStrInfo_common_ShowIpPimSmRp, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_RpAddr_2, pStrInfo_common_ShowRPAddress, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Rphash, pStrInfo_common_ShowIpPimSmRpHash, commandShowIpPimsmRpHash, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_GrpAddr, pStrInfo_common_CfgIpPimSmStaticRpGrpAddr, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
}
/*********************************************************************
*
* @purpose  Build the tree nodes for Global config Mode Dvmrp
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Global config Mode Dvmrp
*
* @end
*
*********************************************************************/
void cliTreeGlobalConfigModeDvmrp (EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  /* depth2 = "ip" */
  unit = cliGetUnitId();

  depth3 = ewsCliAddNode(depth2, pStrInfo_ipmcast_Dvmrp_2, pStrInfo_ipmcast_Dsbld_1, commandIpDvmrp, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Trapflags, pStrInfo_ipmcast_CfgRtrDvmrpTrapMode, commandIpDvmrpTrapflags, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Global config Mode Igmp
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Global config Mode Igmp
*
* @end
*
*********************************************************************/
void cliTreeGlobalConfigModeIgmp (EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  /* depth2 = "ip" */
  unit = cliGetUnitId();

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmp_2, pStrInfo_ipmcast_DsblIgmp, commandIpIgmp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Igmp_RouterAlert, pStrInfo_ipmcast_IgmpRouterAlert,
                         commandIpIgmpRouterAlert, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
*
* @purpose  Build the tree nodes for Global config Mode Pimdm
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Global config Mode Pimdm
*
* @end
*
*********************************************************************/
void cliTreeGlobalConfigModePimdm (EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;
  L7_uint32 unit;

  /* depth2 = "ip" */
  unit = cliGetUnitId();
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pimdm_1, pStrInfo_ipmcast_DsblPimDm_1, commandIpPimdm, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_ipmcast_PimTrapflags, pStrInfo_ipmcast_CfgRtrPimTrapMode, commandIpPimTrapflags, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
*
* @purpose  Build the tree nodes for Global config Pimsm commands
*
* @param void
*
* @returntype void
*
* @notes creates the tree nodes for Global config Pimsm commands
*
* @end
*
*********************************************************************/
void cliTreeGlobalConfigPimsm(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uint32 unit;

  /* depth3 = "configure" */
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* Port Based Routing */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pimsm_1, pStrInfo_ipmcast_CfgRtrPimSm_2, commandIpPimsm, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_BsrCandidate, pStrInfo_common_CfgIpPimSmBsrCandidate, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_common_CfgIpPimSmBsrCandidate, commandIpPimsmBsrCandidate, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_Range0to32, pStrInfo_common_CfgIpPimSmMaskLen, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Range0to255, pStrInfo_common_CfgIpPimSmPri, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RegisterThresh, pStrErr_ipmcast_CfgRtrPimSmRegisterRateLimit, commandIpPimsmRegisterThreshold, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2000, pStrInfo_common_CfgRtrPimSmSptThreshRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RpAddr_3, pStrInfo_ipmcast_CfgIpPimSmStaticRp, commandIpPimsmRpAddress, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_ipmcast_RpAddr, pStrInfo_common_CfgIpPimSmStaticRpRpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_GrpAddr, pStrInfo_common_CfgIpPimSmStaticRpGrpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_ipmcast_GrpMask, pStrInfo_ipmcast_CfgIpPimSmStaticRpGrpMask, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Override, pStrErr_common_CfgRtrPimSmOver, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RpCandidate, pStrInfo_common_CfgIpPimSmRpCandidate, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_common_CfgIpPimSmRpCandidate, commandIpPimsmRpCandidate, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_GrpAddr, pStrInfo_common_CfgIpPimSmStaticRpGrpAddr, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_ipmcast_GrpMask, pStrInfo_ipmcast_CfgIpPimSmStaticRpGrpMask, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SptThresh, pStrInfo_ipmcast_CfgRtrPimSmSptThresh, commandIpPimsmSptThreshold, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2000, pStrInfo_common_CfgRtrPimSmSptThreshRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ssm, pStrInfo_ipmcast_CfgRtrPimSm, commandIpPimsmSsm, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_PassDefl, pStrInfo_ipmcast_CfgRtrPimSm_1,   NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_GrpAddr, pStrInfo_common_CfgIpPimSm, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_ipmcast_GrpMask, pStrInfo_common_CfgIpPimSm, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_FLEX_PIMSM_MAP_COMPONENT_ID);

}
/*********************************************************************
*
* @purpose  Build the tree nodes for Global config ip multicast staticroute
*
* @param void
*
* @returntype void
*
* @notes
*
* @end
*
*********************************************************************/
void cliTreeGlobalConfigStaticRoute(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;
  L7_uint32 unit;

  if ((unit = cliGetUnitId()) == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_ipmcast_McastRouting, commandIpMulticast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_McastMRoute, pStrInfo_ipmcast_CfgRtrMcastStaticRoute, commandIpMulticastStaticroute, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_McastMRouteSrcIpAddr, pStrInfo_ipmcast_ShowRtrMcastMrouteDetailedSrcType, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_McastMRouteSrcIpMask, pStrInfo_ipmcast_CfgRtrMcastStaticRouteTypeMask, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_McastMRouteRpfIpAddr, pStrInfo_ipmcast_CfgRtrMcastStaticRouteTypeRpfAddr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range1to255, pStrInfo_ipmcast_CfgRtrMcastStaticRouteTypeMetric, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

