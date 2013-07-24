/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_switch_device.c
 *
 * @purpose create the cli for Switching Device
 *
 * @component user interface
 *
 * @comments contains the code to build the tree for Switching Device
 * @comments also contains functions that allow tree navigation
 *
 * @create  07/04/20003
 *
 * @author  Deepa
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_ip_mcast_cli.h"

#include "clicommands_card.h"
#include "clicommands_dot1p.h"
#include "clicommands_lacp.h"
#include "clicommands_vlan.h"
#include "clicommands_protectedport.h"
#include "clicommands_pbvlan.h"
#include "clicommands_macvlan.h"
#include "clicommands_ipsubnetvlan.h"
#include "clicommands_snooping_querier.h"
#include "clicommands_dai.h"
#include "snooping_exports.h"
#include "dot3ad_exports.h"
#include "usmdb_sim_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"

#ifdef L7_RLIM_PACKAGE
#include "clicommands_loopback.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif /* end of L7_IPV6_PACKAGE */
#endif /* end of L7_RLIM_PACKAGE */

/*********************************************************************
*
* @purpose  Build the Switch Device show storm control commands tree
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

void buildTreePrivSwDevStormControl(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_StormCntrl_1, pStrInfo_base_ShowSwitchCfg,commandShowStormControl, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_ShowStormAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device show port commands tree
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
void buildTreePrivSwDevPort(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;
#ifdef L7_RLIM_PACKAGE
  EwsCliCommandP depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#endif /* L7_RLIM_PACKAGE */

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Port_4, pStrInfo_base_ShowPort, commandShowPort, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Desc_2, pStrInfo_base_IntfDesc, commandShowPortDescription, L7_NO_OPTIONAL_PARAMS);
  depth5 =  buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_LoopBack, pStrInfo_routing_LoopBackIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_LoopBackIntfId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_IPV6_PACKAGE
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Tunnel_1, pStrInfo_common_Ipv6Tunnel,  NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_TunnelIntfId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /* L7_IPV6_PACKAGE */
#endif /* L7_RLIM_PACKAGE */
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_PBVLAN_PACKAGE
  buildTreePrivSwPbvlan(depth3);
#endif
}

/*********************************************************************
*
* @purpose  Build the Switch Device set gmrp commands tree
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

void buildTreePrivSwDevSetGmrpGvrpAdmin(EwsCliCommandP depth2)
{
  /* depth2 = "set" */
  EwsCliCommandP depth3, depth4, depth5;

  buildTreePrivSWMgmtSetPrompt(depth2);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gmrp_1, pStrInfo_base_CfgGarpGmrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Adminmode, pStrInfo_base_CfgGarpGmrpAdminMode, commandSetGMRPAdminMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gvrp, pStrInfo_base_CfgGarpGvrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Adminmode, pStrInfo_base_CfgGarpGvrpAdminMode, commandSetGVRPAdminMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Switch Device show igmpsnooping command tree
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

void buildTreePrivSwDevShowIgmpInterface(EwsCliCommandP depth2,
                                         L7_uchar8 family)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }
  if (family == L7_AF_INET)
  {
    if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                 L7_IGMP_SNOOPING_FEATURE_SUPPORTED) != L7_TRUE)
    {
      return;
    }
  }
  else
  {
    if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                 L7_MLD_SNOOPING_FEATURE_SUPPORTED) != L7_TRUE)
    {
      return;
    }
  }

  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE)
  {
    if (family == L7_AF_INET)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmpsnooping, CLISHOWIGMPSNOOPING_HELP(family), commandShowIGMPSnooping, L7_NO_OPTIONAL_PARAMS);
    }
    else
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mldsnooping, CLISHOWIGMPSNOOPING_HELP(family), commandShowIGMPSnooping, L7_NO_OPTIONAL_PARAMS);
    }
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Mrouter, CLISHOWIGMPSNOOPING_MROUTER_HELP(family), commandShowIGMPSnoopingMrouterInterface, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1,pStrInfo_base_ShowIgmpSnoopingMrouterIntf, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth5 = ewsCliAddNode(depth4, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ShowIgmpSnoopingMrouterVlan, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    /* Querier Tree */
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Querier_1, CLISHOWIGMPSNOOPING_QUERIER_HELP(family), commandShowSnoopingQuerier, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth5 = ewsCliAddNode(depth4, pStrInfo_common_IgmpProxyGrpsDetail,CLISHOWIGMPSNOOPING_QUERIER_DETAIL_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth5 = ewsCliAddNode(depth4, pStrInfo_common_MacAclVlan_1, CLISHOWIGMPSNOOPING_QUERIER_VLANID_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5,  buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                 (family == L7_AF_INET)
                                 ? L7_IGMP_SNOOPING_FEATURE_PER_VLAN
                                 : L7_MLD_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, buf, CLISHOWIGMPSNOOPING_VLANID_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

}

/*********************************************************************
*
* @purpose  Build the Switch Device show mirroring commands tree
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

void buildTreePrivSwDevShowMonitor(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Monitor, pStrInfo_base_ShowMonitor, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Session_1, pStrInfo_base_ShowMonitorSession, commandShowMonitor, L7_NO_OPTIONAL_PARAMS);

  sprintf(buf, "<%d-%d> ", 1, L7_MIRRORING_MAX_SESSIONS);

  depth5 = ewsCliAddNode(depth4, buf,  pStrInfo_base_ShowMonitorSessionRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Device show garp configuration commands tree
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

void buildTreeUserSwDevShowGarpConfiguration(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gmrp_1, pStrInfo_base_ShowMfdbGmrp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cfg_8, pStrInfo_base_ShowMfdbGmrp, commandShowGmrpConfiguration, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gvrp, pStrInfo_base_ShowGarpGvrp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cfg_8, pStrInfo_base_ShowGarpGvrp, commandShowGvrpConfiguration, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Device show garp commands tree
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

void buildTreeUserSwDevShowGarp(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Garp_1, pStrInfo_base_ShowGarp, commandShowGarp, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch Device show vlan commands tree
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

void buildTreeUserSwDevShowVlan(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
#ifdef L7_ROUTING_PACKAGE
  L7_uchar8 *intUsageHelp = "Show VLANs assigned to port-based routing interfaces";
#endif

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ShowVlan, commandShowVlan, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if(((usmDbComponentPresentCheck(unit, L7_VLAN_IPSUBNET_COMPONENT_ID)) ==  L7_TRUE)||
     ((usmDbComponentPresentCheck(unit, L7_VLAN_MAC_COMPONENT_ID)) ==  L7_TRUE))
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Association, pStrInfo_base_ShowVlanAssociationToVlan,
                           NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_MACVLAN_PACKAGE
  buildTreeUserSwDevShowVlanMacVlan(depth4);
#endif

#ifdef L7_IPVLAN_PACKAGE
  buildTreeUserSwDevShowVlanIpVlan(depth4);
#endif
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowVlanSummary, commandShowVlanBrief, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Port_4, pStrInfo_base_ShowVlanPort, commandShowVlanPort, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
  /* show vlan internal usage */
  depth4 = ewsCliAddNode(depth3, "internal", intUsageHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "usage", intUsageHelp, commandShowVlanInternal, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/*********************************************************************
*
* @purpose  Build the Show ARP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/

void buildTreeShowArp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

#ifdef L7_DAI_PACKAGE
  EwsCliCommandP depth6;
#endif

#ifdef L7_ROUTING_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Arp_2, pStrInfo_base_ShowArp, commandShowArp, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowArpBrief, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Switch_1, pStrInfo_base_ShowArpSwitch, commandShowArpSwitch, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#else

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Arp_2, pStrInfo_base_ShowArp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Switch_1, pStrInfo_base_ShowArpSwitch, commandShowArpSwitch, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_DAI_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ArpAcl, pStrInfo_base_ArpAclShow, commandShowArpAcl, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_ArpAclName, pStrInfo_base_ArpAclShow, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}



/*********************************************************************
*
* @purpose  Build the Switch Device Global Delete Port command tree
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

void buildTreeGlobalDeletePortAll(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Delport, pStrInfo_base_IntfDelPortAll, commandGlobalDeletePortAll, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_AllIntfLinkTrap, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeGlobalSwDevInterfaceLinkTrap(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Linktrap, pStrInfo_base_CfgPortLinkTrap, commandInterfaceLinktrap, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_IntfLagLinkTrapAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}
void buildTreeGlobalSwDevInterfaceName(EwsCliCommandP depth1)
{

  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_ApProfileShowRunningName, pStrInfo_base_IntfName, commandPortChannelName, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_CfgLagNameString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_IntfLagNameAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_CfgLagNameString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  Build the Switch Device garp commands tree
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

void buildTreeInterfaceSwDevSetGarp(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 family;
  L7_uint32 idx;

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Set_2, pStrInfo_base_Cfg_3, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Garp_1, pStrInfo_base_CfgGarp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Timer_1, pStrInfo_base_Timer, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Join, pStrInfo_base_CfgGarpJoinTime, commandSetGarpTimerJoin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range10to100, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Leave, pStrInfo_base_CfgGarpLeaveTime, commandSetGarpTimerLeave, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range20to600, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Leaveall, pStrInfo_base_SetGarpTimerLeaveAll, commandSetGarpTimerLeaveAllTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range200to6000, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gmrp_1, pStrInfo_base_CfgGarpGmrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Intfmode, pStrInfo_base_SetGmrpIntfMode, commandSetGMRPInterfaceMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gvrp, pStrInfo_base_CfgGarpGvrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Intfmode, pStrInfo_base_CfgGarpGvrpIntfMode, commandSetGVRPInterfaceMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE)
  {
    for (idx = 0; idx < 2; idx++)
    {

      if (idx == 0)
      {
        if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                     L7_IGMP_SNOOPING_FEATURE_SUPPORTED)
            != L7_TRUE)
        {
          continue;
        }
        family = L7_AF_INET;
      }
      else
      {
        if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                     L7_MLD_SNOOPING_FEATURE_SUPPORTED)
            != L7_TRUE)
        {
          continue;
        }
        family = L7_AF_INET6;
      }

      if (family == L7_AF_INET)
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmp_2, CLISETIGMP_HELP(family), commandSetIGMPInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
      else
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mld_1, CLISETIGMP_HELP(family), commandSetIGMPInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }

      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_FastLeave, pStrInfo_base_SetIgmpFastLeave, commandSetIGMPInterfaceFastLeave, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_GrpMbrshipIntvl_1, CLICONFIGIGMPSNOOPINGGROUPMEMBERSHIP_HELP(family), commandSetIGMPInterfaceGroupMembershipInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_CfgIgmpSnoopingGrpMbrParm, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Maxresponse, CLISETIGMPMAXRESPONSE_HELP(family), commandSetIGMPInterfaceMaxResponse, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_RESPONSE_TIME : L7_MLD_SNOOPING_MIN_RESPONSE_TIME,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_RESPONSE_TIME : L7_MLD_SNOOPING_MAX_RESPONSE_TIME);
      depth5 = ewsCliAddNode(depth4, buf, CLICONFIGIGMPSNOOPINGMAXRESPONSEPARM_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Mcrtrexpiretime, pStrInfo_base_SetIgmpMCastRtrExpireTime, commandSetIGMPInterfaceMcRtrExpireTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ",
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_CfgIgmpSnoopingMcrtrExpireTime, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Mrouter, pStrInfo_base_SetIgmpSnoopingMrouter, commandSetIGMPMrouterInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_SetIgmpSnoopingMrouterIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Vid, pStrInfo_common_DiffservVlanId, NULL, 5, L7_NODE_UINT_RANGE, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Shutdown_2, pStrInfo_common_Shutdown, commandShutdown, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Snmp_1, pStrInfo_common_Snmp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Trap_2, pStrInfo_common_Trap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LinkStatus_1, pStrInfo_common_SnmpTrapLinkStatus, commandSnmpTrapLinkStatus, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Switch Device speed and vlan commands tree
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

void buildTreeInterfaceSwDevSpeed(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Speed_2, pStrInfo_base_Speed, commandSpeed, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Num10, pStrInfo_base_CfgPortPhyMode10Mbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_FullDuplex, pStrInfo_base_CfgFull, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_HalfDuplex, pStrInfo_base_CfgHalfDuplex, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Num100, pStrInfo_base_CfgPortPhyMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_FullDuplex, pStrInfo_base_CfgFull, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_HalfDuplex, pStrInfo_base_CfgHalfDuplex, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch Device speed and vlan commands tree
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

void buildTreeInterfaceSwDevVlan(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protectedPortGroupRange[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_base_CfgVlanParams, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Acceptframe, pStrInfo_base_CfgVlanPortAcceptFrame, commandVlanAcceptFrame, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_AcceptFrameAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_VlanOnly_1, pStrInfo_base_CfgVlan, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AdmitUntaggedOnly_1, pStrInfo_base_CfgUntaggedVlan, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1Q_COMPONENT_ID, L7_DOT1Q_INGRESSFILTER_FEATURE_ID) != L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Ingressfilter, pStrInfo_base_CfgVlanPortIngressFilter, commandVlanIngressFilter, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Participation_1, pStrInfo_base_CfgVlanParticipation, commandVlanParticipation, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auto2, pStrInfo_base_CfgVlanParticipationAuto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4,  pStrInfo_common_VlanList_1, vlanRangeHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Excl_1, pStrInfo_base_CfgVlanParticipationExcl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4,  pStrInfo_common_VlanList_1, vlanRangeHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Incl_1, pStrInfo_base_CfgVlanParticipationIncl, NULL, L7_NO_OPTIONAL_PARAMS); 
  depth5 = ewsCliAddNode(depth4,  pStrInfo_common_VlanList_1, vlanRangeHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceVlanPortDot1pPriority(depth2);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Pvid, pStrInfo_base_CfgVlanPortId, commandVlanPvid, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Tagging_1, pStrInfo_base_VlanTagging, commandVlanTagging, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_VlanList_1, vlanRangeHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID) == L7_TRUE)
  {
    /* Switch Port Protected command in interface config mode */
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Switchport, pStrInfo_base_ProtectedPortParams,
                           NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Protected,pStrInfo_base_CfgProtectedPort,
                           commandSwitchPortProtected, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
    {
      sprintf(protectedPortGroupRange, "<%d-%d> ", 0, L7_PROTECTED_PORT_MAX_GROUPS-1);
      depth4 = ewsCliAddNode(depth3, protectedPortGroupRange, pStrInfo_base_ProtectedPortGrp,
                             NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                             L7_NO_COMMAND_SUPPORTED, L7_NO_OPTIONAL_PARAMS);
    }
    else
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                             L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device duplex commands tree
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

void buildTreeInterfaceSwDevDuplex(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_AutoNegotiate, pStrInfo_base_Duplex, commandAutoNegotiate, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}


/*********************************************************************
*
* @purpose  Build the Switch Device port l
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

void buildTreeInterfaceSwDevPort(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Port_4, pStrInfo_base_Port_1, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lacpmode, pStrInfo_base_PortLacpMode, commandPortLacpMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lacptimeout, pStrInfo_base_PortLacpTimeout, commandPortLacpTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Actor, pStrInfo_base_CfgLacpActor, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Long, pStrInfo_base_CfgLacpLongTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Short_1, pStrInfo_base_CfgLacpShortTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Partner, pStrInfo_base_CfgLacpPartner, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Short_1, pStrInfo_base_CfgLacpShortTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Long, pStrInfo_base_CfgLacpLongTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Device garp, igmp, and slot commands tree
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

void buildTreeGlobalSwDevGarpTimer(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 family = L7_AF_INET;
  L7_uint32 unit, idx;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Set_2, pStrInfo_base_Cfg_3, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_CHASSIS
  buildTreeConfigGlobalAlarmModes(depth2);
#endif
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Garp_1, pStrInfo_base_CfgGarp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Timer_1, pStrInfo_base_Timer, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Join, pStrInfo_base_CfgGarpJoinTime, commandSetGarpTimerJoinAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range10to100, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Leave, pStrInfo_base_CfgGarpLeaveTime, commandSetGarpTimerLeaveAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range20to600, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Leaveall, pStrInfo_base_SetGarpTimerLeaveAll, commandSetGarpTimerLeaveAllTimeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range200to6000, pStrInfo_base_CfgGarpTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gmrp_1, pStrInfo_base_CfgGarpGmrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Intfmode, pStrInfo_base_SetGmrpIntfMode, commandSetGMRPInterfaceModeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Gvrp, pStrInfo_base_CfgGarpGvrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Intfmode, pStrInfo_base_CfgGarpGvrpIntfMode, commandSetGVRPInterfaceModeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE)
  {
    for (idx = 0; idx < 2; idx++)
    {
      if (idx == 0)
      {
        if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                     L7_IGMP_SNOOPING_FEATURE_SUPPORTED)
            != L7_TRUE)
        {
          continue;
        }
        family = L7_AF_INET;
      }
      else if (idx == 1)
      {
        if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                     L7_MLD_SNOOPING_FEATURE_SUPPORTED)
            != L7_TRUE)
        {
          continue;
        }
        family = L7_AF_INET6;
      }
      if (family == L7_AF_INET)
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmp_2, CLISETIGMPGLOBAL_HELP(family), commandSetIGMPGlobal, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
      else
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mld_1, CLISETIGMPGLOBAL_HELP(family), commandSetIGMPGlobal, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_GrpMbrshipIntvl_1, CLICONFIGIGMPSNOOPINGGROUPMEMBERSHIP_HELP(family), commandSetIGMPInterfaceGroupMembershipIntervalAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL,(family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_CfgIgmpSnoopingGrpMbrParm, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Intfmode, CLIINTERFACEMODE_HELP(family), commandSetIGMPInterfaceModeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Maxresponse, CLISETIGMPMAXRESPONSE_HELP(family), commandSetIGMPInterfaceMaxResponseAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_RESPONSE_TIME : L7_MLD_SNOOPING_MIN_RESPONSE_TIME,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_RESPONSE_TIME : L7_MLD_SNOOPING_MAX_RESPONSE_TIME);
      depth5 = ewsCliAddNode(depth4, buf, CLICONFIGIGMPSNOOPINGMAXRESPONSEPARM_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Mcrtrexpiretime, pStrInfo_base_SetIgmpMCastRtrExpireTime, commandSetIGMPInterfaceMcRtrExpireTimeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(buf, sizeof(buf), "<%d-%d> ",
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_CfgIgmpSnoopingMcrtrExpireTime, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      /* Querier */
      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Querier_1, CLISETSNOOP_QUERIER_HELP(family), commandSetSnoopingQuerier, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, CLISETSNOOP_QUERIERMODE_HELP(family), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_4, pStrInfo_base_SetSnoopQuerierAddr, commandSetSnoopingQuerierAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_IpAddr, CLISETSNOOP_ADDR_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth5 = ewsCliAddNode(depth4, pStrInfo_common_QueryIntvl_1, pStrInfo_base_SetSnoopQuerier_1, commandSetSnoopingQuerierQueryInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      if (family == L7_AF_INET)
      {
        sprintf(buf, "<%d-%d> ", L7_IGMP_SNOOPING_QUERIER_MIN_QUERY_INTERVAL, L7_IGMP_SNOOPING_QUERIER_MAX_QUERY_INTERVAL);
      }
      else
      {
        sprintf(buf, "<%d-%d> ", L7_MLD_SNOOPING_QUERIER_MIN_QUERY_INTERVAL, L7_MLD_SNOOPING_QUERIER_MAX_QUERY_INTERVAL);
      }
      depth6 = ewsCliAddNode(depth5, buf, pStrInfo_base_SnoopQuerier, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Timer_1, pStrInfo_base_SetSnoopQuerierExpiry, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Expiry_1, pStrInfo_base_SetSnoopQuerierExpiry, commandSetSnoopingQuerierExpiryInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      if (family == L7_AF_INET)
      {
        sprintf(buf, "<%d-%d> ", L7_IGMP_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL, L7_IGMP_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL);
      }
      else
      {
        sprintf(buf, "<%d-%d> ", L7_MLD_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL, L7_MLD_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL);
      }
      depth7 = ewsCliAddNode(depth6, buf, pStrInfo_base_SnoopQuerierExpiry, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (family == L7_AF_INET)
      {
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ver_2, CLISETSNOOP_QUERIER_VERSION_HELP(family), commandSetSnoopingQuerierVersion, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
        sprintf(buf, "<%d-%d> ", L7_IGMP_SNOOPING_QUERIER_MIN_VERSION, L7_IGMP_SNOOPING_QUERIER_MAX_VERSION);
        depth6 = ewsCliAddNode(depth5, buf, CLISNOOP_QUERIER_VERSION_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

        /* Router alert Check */
        depth4 = ewsCliAddNode(depth3, pStrInfo_common_Igmp_RouterAlert, pStrInfo_ipmcast_IgmpRouterAlert,
                               commandIpIgmpSnoopingRouterAlert, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
    }
  }

  buildTreeGlobalSlotSet(depth2);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Shutdown_2, pStrInfo_common_Shutdown, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_ShutdownAll, commandShutdownAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifndef L7_CHASSIS
  buildTreeGlobalSlotCard(depth1);
#endif
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Snmp_1, pStrInfo_common_Snmp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Trap_2, pStrInfo_common_Trap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LinkStatus_1, pStrInfo_base_LinksTatus, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_SnmpTrapLinkStatusAll, commandSnmpTrapLinkStatusAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Switch Device vlan commands tree
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

void buildTreeGlobalSwDevVlan(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_base_CfgVlanParams, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Participation_1, pStrInfo_base_CfgVlanParticipation, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_VlanParticipationAll, commandVlanParticipationAll, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Auto2, pStrInfo_base_CfgVlanParticipationAuto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Excl_1, pStrInfo_base_CfgVlanParticipationExcl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Incl_1, pStrInfo_base_CfgVlanParticipationIncl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Port_4, pStrInfo_base_CfgVlanPort, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Acceptframe, pStrInfo_base_CfgVlanPortAcceptFrame, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_VlanPortAcceptFrameAll, commandVlanPortAcceptFrameAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_All, pStrInfo_base_AcceptFrameAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_VlanOnly_1, pStrInfo_base_CfgVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_AdmitUntaggedOnly_1, pStrInfo_base_CfgUntaggedVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1Q_COMPONENT_ID, L7_DOT1Q_INGRESSFILTER_FEATURE_ID) != L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ingressfilter, pStrInfo_base_CfgVlanPortIngressFilter, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_VlanPortIngressFilterAll, commandVlanPortIngressFilterAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceVlanPortDot1pPriorityAll(depth3);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Pvid, pStrInfo_base_CfgVlanPortId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_VlanPortPvidAll, commandVlanPortPvidAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Tagging_1, pStrInfo_base_VlanTagging, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_VlanPortTaggingAll, commandVlanPortTaggingAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_PBVLAN_PACKAGE
  buildTreeGlobalSwDevVlanPbvlan(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeGlobalInternalVlan(depth2);
#endif
}
/*********************************************************************
*
* @purpose  Build the Switch Device speed all commands tree
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

void buildTreeGlobalSwDevSpeed(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */

  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Speed_2, pStrInfo_base_Speed, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_SpeedAll, commandSpeedAll, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Num10, pStrInfo_base_CfgPortPhyMode10Mbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_FullDuplex, pStrInfo_base_CfgFull, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_HalfDuplex, pStrInfo_base_CfgHalfDuplex, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Num100, pStrInfo_base_CfgPortPhyMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_FullDuplex, pStrInfo_base_CfgFull, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_HalfDuplex, pStrInfo_base_CfgHalfDuplex, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#if 0
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Num1000, pStrInfo_base_CfgPortPhyMode1000Mbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_FullDuplex, pStrInfo_base_CfgFull, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/*********************************************************************
*
* @purpose  Build the Switch Device storm control commands tree
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

void buildTreeGlobalSwDevStormControl(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */

  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_StormCntrl_1, pStrInfo_base_StormCntrl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bcast_1, pStrInfo_base_SwitchCfgStormCntrlBcastAll, commandStormControlBcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  else if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bcast_1, pStrInfo_base_SwitchCfgStormCntrlBcast, commandStormControlBcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_base_SwitchCfgStormCntrlMcastAll, commandStormControlMcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  else if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_base_SwitchCfgStormCntrlMcast, commandStormControlMcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ucast_1, pStrInfo_base_SwitchCfgStormCntrlUcastAll, commandStormControlUcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  else if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ucast_1, pStrInfo_base_SwitchCfgStormCntrlUcast, commandStormControlUcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Flowcontrol, pStrInfo_base_CfgSwitchFlowCntrl, commandStormControlFlowControl, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the Interface config storm control commands tree
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

void buildTreeInterfaceStormControl(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */

  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_StormCntrl_1, pStrInfo_base_StormCntrl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bcast_1, pStrInfo_base_SwitchCfgStormCntrlBcast, commandStormControlIntfBcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_base_SwitchCfgStormCntrlMcast, commandStormControlIntfMcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  }
  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ucast_1, pStrInfo_base_SwitchCfgStormCntrlUcast, commandStormControlIntfUcast, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Lvl, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to100, pStrInfo_base_SwitchCfgStormCntrlThreshSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rate, pStrInfo_base_SwitchCfgStormCntrlThresh, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to14880000, pStrInfo_base_SwitchCfgStormCntrlThreshRateSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
}
/*********************************************************************
*
* @purpose  Build the Switch Device port lacpmode and vlan group commands tree
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

void buildTreeGlobalSwDevPort(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Port_4, pStrInfo_base_Port_1, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lacpmode, pStrInfo_base_PortLacpMode, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Enbl_2, pStrInfo_base_PortLacpModeAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_PortLacpModeAll, commandPortLacpModeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lacptimeout, pStrInfo_base_PortLacpTimeoutAll, commandPortLacpTimeoutAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Actor, pStrInfo_base_CfgLacpActor, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Short_1, pStrInfo_base_CfgLacpShortTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Long, pStrInfo_base_CfgLacpLongTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Partner, pStrInfo_base_CfgLacpPartner, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Short_1, pStrInfo_base_CfgLacpShortTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Long, pStrInfo_base_CfgLacpLongTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch Device Duplex commands tree
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

void buildTreeGlobalSwDevDuplexAll(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_AutoNegotiate, pStrInfo_base_Duplex, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_DuplexAll, commandAutoNegotiateAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Switch Device Mirroring commands tree
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

void buildTreeGlobalSwDevCreateMirroring(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth9 = 0; /* keep compiler happly */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Monitor, pStrInfo_base_CreateMirroring, commandPortMirroring, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Session_1, pStrInfo_base_CreateMirroring, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  sprintf(buf, "<%d-%d> ", 1, L7_MIRRORING_MAX_SESSIONS);

  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_CfgMirroringSessionRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayDest, pStrInfo_base_CfgMirroringDest, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_CfgMirroringIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 =  buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ApShowRunningMode, pStrInfo_base_CfgMirroringMode, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SrcPortMirror, pStrInfo_base_CfgMirroringSrc, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_CfgMirroringIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 =  buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
                            L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_base_Ingress, pStrInfo_base_CfgMirroringRess, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_base_Egress, pStrInfo_base_CfgMirrorEgress, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device vlan commands tree
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

void buildTreeVlanDBSwDevVlan(EwsCliCommandP depth1)
{
  /* depth1 = "Vlan Database" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_ROUTING_PACKAGE
  L7_char8 intfIdRange[L7_CLI_MAX_STRING_LENGTH];
#endif
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL setCmd = L7_FALSE;
  L7_uint32 idx;
  L7_uchar8 family;
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

#ifdef L7_PBVLAN_PACKAGE
  buildTreeVlanDBSwDevVlanPbvlan(depth1); 
#endif

  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE &&
      (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID, L7_IGMP_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE
       || usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID, L7_MLD_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE)
  )
  {
    if (setCmd == L7_FALSE)
    {
      depth2 = ewsCliAddNode(depth1, pStrInfo_common_Set_2, pStrInfo_base_Cfg_3, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      setCmd = L7_TRUE;
    }
    for (idx = 0; idx < 2; idx++)
    {
      if (idx == 0)
      {
        family = L7_AF_INET;
        if ((usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                      L7_IGMP_SNOOPING_FEATURE_SUPPORTED)
             != L7_TRUE) ||
            (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                      L7_IGMP_SNOOPING_FEATURE_PER_VLAN)
             != L7_TRUE)
        )
        {
          continue;
        }
      }
      else
      {
        if ((usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                      L7_MLD_SNOOPING_FEATURE_SUPPORTED)
             != L7_TRUE) ||
            (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                      L7_MLD_SNOOPING_FEATURE_PER_VLAN)
             != L7_TRUE)
        )
        {
          continue;
        }
        family = L7_AF_INET6;
      }
      if (family == L7_AF_INET)
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmp_2, CLISETIGMPVLAN_HELP(family), commandSetIGMPVlan, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
      else
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mld_1, CLISETIGMPVLAN_HELP(family), commandSetIGMPVlan, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      }
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_FastLeave, pStrInfo_base_SetIgmpVlanFastLeave, commandSetIGMPVlanFastLeave, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_GrpMbrshipIntvl_1, pStrInfo_base_SetIgmpVlanGrpMbrshipIntvl, commandSetIGMPVlanGroupMembershipInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(stat, sizeof(stat), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL :
                    L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL);    depth6 = ewsCliAddNode(depth5, stat, pStrInfo_base_SetIgmpVlanGrpMbrshipIntvlCheck, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Maxresponse, pStrInfo_base_SetIgmpVlanMaxRespTime, commandSetIGMPVlanMaxResponseTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(stat, sizeof(stat), "<%d-%d> ", (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_RESPONSE_TIME : L7_MLD_SNOOPING_MIN_RESPONSE_TIME
                    ,(family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_RESPONSE_TIME : L7_MLD_SNOOPING_MAX_RESPONSE_TIME);
      depth6 = ewsCliAddNode(depth5, stat, pStrInfo_base_SetIgmpVlanMaxRespTimePara, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Mcrtrexpiretime, pStrInfo_base_SetIgmpVlanMcastRtrExpiryTime, commandSetIGMPVlanMulticastRouterExpiryTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      osapiSnprintf(stat, sizeof(stat), "<%d-%d> ",
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME,
                    (family == L7_AF_INET) ? L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME : L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME);

      depth6 = ewsCliAddNode(depth5, stat, pStrInfo_base_SetIgmpVlanMcastRtrExpiryTimePara, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      /* Querier */
      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Querier_1, CLISETSNOOP_QUERIERVLAN_HELP(family), commandSetSnoopingQuerierVlanMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, CLISETSNOOP_QUERIERVLANMODE_HELP(family), NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_4, pStrInfo_base_SetSnoopQuerierVlanAddr, commandSetSnoopingQuerierVlanAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_IpAddr, CLISETSNOOP_ADDR_HELP(family), NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Election, pStrInfo_base_SetSnoopQuerier, L7_NO_OPTIONAL_PARAMS, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Participate, pStrInfo_base_SetSnoopQuerier, commandSetSnoopingQuerierElectionVlanMode,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    }
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_base_VlanCreate, commandVlanSwDev, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_VlanList_1, vlanRangeHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if(((usmDbComponentPresentCheck(unit, L7_VLAN_IPSUBNET_COMPONENT_ID)) ==  L7_TRUE)||
     ((usmDbComponentPresentCheck(unit, L7_VLAN_MAC_COMPONENT_ID)) ==  L7_TRUE))
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Association, pStrInfo_base_VlanAssociation,
                           NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_MACVLAN_PACKAGE
  buildTreeVlanDBSwDevVlanMacVlan(depth3); 
#endif

#ifdef L7_IPVLAN_PACKAGE
   buildTreeVlanDBSwDevVlanIpVlan(depth3); 
#endif
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Makestatic, pStrInfo_base_CfgVlanMakeStatic, commandVlanMakeStatic, L7_NO_OPTIONAL_PARAMS);
  sprintf(tmp, "<%d-%d> ", (L7_DOT1Q_MIN_VLAN_ID+1), L7_DOT1Q_MAX_VLAN_ID);
  depth4 = ewsCliAddNode(depth3, tmp, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_ApProfileShowRunningName, pStrInfo_base_CfgVlanName, commandVlanName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Newname, pStrInfo_common_ApProfileNameString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_ROUTING_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Routing_1, pStrInfo_base_VlanRouting, commandIpVlanRouting, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(intfIdRange, "<%d-%d> ", 1, L7_MAX_NUM_VLAN_INTF);
  depth5 = ewsCliAddNode(depth4, intfIdRange, "Enter interface ID", NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/* rhelbaoui */
/*********************************************************************
*
* @purpose  Build the Interface MaxFrameSize tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeInterfaceMaxFrameSize(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if(cnfgrBaseTechnologySubtypeGet() == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115)
  {
    sprintf(buf,"%d ", PLAT_MAX_FRAME_SIZE);
  }
  else
  {
    sprintf(buf,"<%d-%d> ",L7_MIN_FRAME_SIZE, L7_MAX_FRAME_SIZE);
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Mtu, pStrInfo_common_CfgDeflMTU, commandMaxFrameSize, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_common_CfgMTU, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Global MaxFrameSize tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalMaxFrameSize(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf,"<%d-%d> ",L7_MIN_FRAME_SIZE, L7_MAX_FRAME_SIZE);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Mtu, pStrInfo_common_CfgDeflMTU, commandGlobalMaxFrameSize, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_common_CfgMTU, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Show Terminal length tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/

void buildTreeShowTerminalLength(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Terminal, pStrInfo_base_ShowTerminal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Len, pStrInfo_base_ShowTerminalLen, commandShowTerminalLength, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the port-channel load-balance tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/

void buildTreeGlobalLoadBalance(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_BOOL   rc,intfrc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID);
  intfrc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_INTF_FEATURE_ID);
  if (rc == L7_TRUE || intfrc == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_LoadBalance, pStrInfo_base_LoadBalanceLag, commandPortChannelLoadBalanceGlobalLAG, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSAEthIntf, pStrInfo_base_HashModeSAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDAEthIntf, pStrInfo_base_HashModeDAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADAEthIntf, pStrInfo_base_HashModeSADAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSA, pStrInfo_base_HashModeSA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDA, pStrInfo_base_HashModeDA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADA, pStrInfo_base_HashModeSADA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcIP, pStrInfo_base_HashModeSrcIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDestIP, pStrInfo_base_HashModeDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcDestIP, pStrInfo_base_HashModeSrcDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
        /* Enhanced hashing mode */
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, 
                                  L7_DOT3AD_HASHMODE_ENHANCED_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeEnhanced, 
                             pStrInfo_base_HashModeEnhanced, NULL, 2, 
                             L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    if (intfrc == L7_TRUE)
    {
      depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID,L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, 
                             pStrInfo_base_CfgLoadBalanceIntfAll, 
                             NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, 
                             pStrInfo_common_NewLine, NULL, 2, 
                             L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    if (intfrc == L7_TRUE)
    {
      depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NO_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    }

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  }
}

/*********************************************************************
*
* @purpose  Build the port-channel load-balance maintenance tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/

void buildTreeGlobalLoadBalanceMaintenanceMode(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_BOOL   rc, intfrc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID);
  intfrc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_INTF_FEATURE_ID);

  if (rc == L7_TRUE || intfrc == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lag_4, pStrInfo_base_IntfLagExisting, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth3 = ewsCliAddNode(depth2, pStrInfo_base_LoadBalance, pStrInfo_base_LoadBalanceLag, commandPortChannelLoadBalanceGlobalLAG, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSAEthIntf, pStrInfo_base_HashModeSAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDAEthIntf, pStrInfo_base_HashModeDAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADAEthIntf, pStrInfo_base_HashModeSADAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSA, pStrInfo_base_HashModeSA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDA, pStrInfo_base_HashModeDA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADA, pStrInfo_base_HashModeSADA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcIP, pStrInfo_base_HashModeSrcIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDestIP, pStrInfo_base_HashModeDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcDestIP, pStrInfo_base_HashModeSrcDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      if (intfrc == L7_TRUE)
      {
        depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      }
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    if (intfrc == L7_TRUE)
    {
      depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NO_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    }

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_CfgLoadBalanceIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  }
}

/*********************************************************************
*
* @purpose  Build the Global CPU Utilization/Memory monitor tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalCpuMonitor(EwsCliCommandP depth1)
{
  L7_uint32 maxThreshold = 256, numBytesTotal, numBytesAlloc, numBytesFree;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth15;
  EwsCliCommandP depth8, depth9, depth10, depth11, depth12, depth13, depth14;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Build tree for the Memory threshold command */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_CpuMemory, 
                         pStrInfo_CpuMemoryThreshold_Description, 
                         commandGlobalCpuMemoryThreshold, 2, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_CpuMemoryFree, 
                         pStrInfo_CpuMemoryThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_CpuMemoryWatermark, 
                         pStrInfo_CpuMemoryThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_CpuMemoryWatermarkProcessor, 
                         pStrInfo_CpuMemoryThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (osapiGetMemInfo(&numBytesTotal, &numBytesAlloc, &numBytesFree) == L7_SUCCESS)
  {
    maxThreshold = numBytesTotal/1024;
  }

  osapiSnprintf(buf, sizeof(buf), "<%d-%d>", 1, maxThreshold);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_CfgMemoryThreshold,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  
  /* Build tree for the Cpu util threshold command */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Process, 
                         pStrInfo_ProcessCpuThreshold_Description, 
                         commandGlobalCpuUtilizationThreshold, 2, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_ProcessCpu, 
                         pStrInfo_ProcessCpuThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ProcessCpuThr, 
                         pStrInfo_ProcessCpuThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_ProcessThrType, 
                         pStrInfo_ProcessCpuThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_ProcessCpuThrTypeTotal, 
                         pStrInfo_ProcessCpuThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_ProcessCpuThrTypeTotalRising, 
                         pStrInfo_ProcessCpuRisingThreshold_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d>", 
                FD_SIM_DEFAULT_CPU_UTIL_MIN_THRESHOLD, 
                FD_SIM_DEFAULT_CPU_UTIL_MAX_THRESHOLD);
  depth8 = ewsCliAddNode(depth7, buf, pStrInfo_common_CfgCpuRisingThreshold,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_ProcessCpuThrTypeTotalInterval, 
                         pStrInfo_ProcessCpuRisingThresholdInt_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);


  osapiSnprintf(buf, sizeof(buf), "<%d-%d>", 
                FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD, 
                FD_SIM_DEFAULT_CPU_UTIL_MAX_PERIOD);
  depth10 = ewsCliAddNode(depth9, buf, pStrInfo_common_CfgCpuRisingInterval,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);

  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth11 = ewsCliAddNode(depth10, pStrInfo_base_ProcessCpuThrTypeTotalFalling,
                         pStrInfo_ProcessCpuFallingThreshold_Description,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d>", 
                FD_SIM_DEFAULT_CPU_UTIL_MIN_THRESHOLD, 
                FD_SIM_DEFAULT_CPU_UTIL_MAX_THRESHOLD);
  depth12 = ewsCliAddNode(depth11, buf, pStrInfo_common_CfgCpuFallingThreshold,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);

  depth13 = ewsCliAddNode(depth12, pStrInfo_base_ProcessCpuThrTypeTotalInterval, 
                         pStrInfo_ProcessCpuFallingThresholdInt_Description, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);


  osapiSnprintf(buf, sizeof(buf), "<%d-%d>", 
                FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD, 
                FD_SIM_DEFAULT_CPU_UTIL_MAX_PERIOD);
  depth14 = ewsCliAddNode(depth13, buf, pStrInfo_common_CfgCpuFallingInterval,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                         L7_STATUS_NORMAL_ONLY);


  depth15 = ewsCliAddNode(depth14, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  
  return;                       
}
