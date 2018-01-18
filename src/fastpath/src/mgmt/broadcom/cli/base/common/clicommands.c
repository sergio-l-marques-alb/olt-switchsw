/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands.c
 *
 * @purpose create the entire cli
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  05/29/2000
 *
 * @author  Forrest Samuels and Qun He
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "snooping_exports.h"
#include "sshd_exports.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "usmdb_util_api.h"
#include "acl_exports.h"
#include "cos_exports.h"
#include "doscontrol_exports.h"
#include "dot1s_exports.h"
#include "dot1x_exports.h"
#include "poe_exports.h"
#include "sim_exports.h"
#include "log_exports.h"
#include "nim_exports.h"
#include "dot3ad_exports.h"
#include "usmdb_common.h"
#include "cliutil.h"
#include "sysapi.h"
#include "sntp_api.h"
#include "clicommands.h"
#include "clicommands_debug.h"
#include "clicommands_support.h"
#include "clicommands_voice_vlan.h"
#include "clicommands_time.h"
#ifdef L7_ISDP_PACKAGE
#include "clicommands_isdp.h"
#endif /* L7_ISDP_PACKAGE */

#ifdef L7_LLPF_PACKAGE
#include "clicommands_llpf.h"
#endif /* L7_LLPF_PACKAGE */

#ifdef L7_STACKING_PACKAGE
#include "unitmgr_api.h"
#endif

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
#include "clicommands_base_ipv6.h"
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "clicommands_cp.h"
#include "captive_portal_defaultconfig.h"
#endif

#ifdef L7_ROUTING_PACKAGE
/* layer 3 includes           */

#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ospf_api.h"

#include "usmdb_ip_api.h"
#include "clicommands_l3.h"
#include "clicommands_loopback.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#include "clicommands_ipv6.h"
#endif

#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"

#include "datatypes.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_mib_ripv2_api.h"
#endif /* L7_ROUTING_PACKAGE */

#include "clicommands_card.h"
#include "clicommands_dot1s.h"
#include "clicommands_dot1x.h"
#include "clicommands_dot1p.h"
#include "clicommands_radius.h"
#include "clicommands_usermgr.h"

#include "usmdb_support.h"
#include "usmdb_dim_api.h"

#if defined (L7_IPV6_PACKAGE) && defined (L7_MCAST_PACKAGE)
#include "clicommands_mcast6.h"
#endif /* L7_IPV6 PACKAGE && L7_MCAST_PACKAGE */
#ifdef L7_MGMT_SECURITY_PACKAGE
#include "clicommands_ssl.h"
#include "clicommands_ssh.h"
#endif /* L7_MGMT_SECURITY_PACKAGE */

#ifdef L7_WIRELESS_PACKAGE
#include "clicommands_wireless.h"
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_BGP_PACKAGE
#include "clicommands_bgp_l3.h"
#endif /* L7_BGP_PACKAGE */

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "clicommands_dot1ad.h"
#include "clicommands_dot1ad_service.h"
#endif
#ifdef L7_DOT3AH_PACKAGE
#include "clicommands_dot3ah.h"
#endif
#ifdef L7_DOT1AG_PACKAGE
#include "clicommands_dot1ag.h"
#endif
#endif

#ifdef L7_DHCP_FILTER_PACKAGE
#include "clicommands_dhcp_filtering.h"
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
#include "clicommands_dhcp_snooping.h"
#endif

#ifdef L7_DAI_PACKAGE
#include "clicommands_dai.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "clicommands_acl.h"
#endif /* L7_QOS_FLEX_PACKAGE_ACL */

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
#include "clicommands_diffserv.h"
#endif /* L7_QOS_FLEX_PACKAGE_DIFFSERV */

#ifdef L7_QOS_FLEX_PACKAGE_COS
#include "clicommands_cos.h"
#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
#include "clicommands_iscsi.h"
#endif /* L7_QOS_FLEX_PACKAGE_ISCSI */

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "clicommands_voip.h"
#endif /* L7_QOS_FLEX_PACKAGE_VOIP */

#ifdef L7_MCAST_PACKAGE
#include "clicommands_mcast.h"
#endif /* L7_MCAST_PACKAGE */

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif /* L7_STACKING_PACKAGE */

#ifdef L7_BGP_PACKAGE
#include "clicommands_bgp_l3.h"
#endif


#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "clicommands_dot1ad.h"
#include "clicommands_dot1ad_service.h"
#endif
#ifdef L7_DOT3AH_PACKAGE
#include "clicommands_dot3ah.h"
#endif

#ifdef L7_TR069_PACKAGE
#include "clicommands_tr069.h"
#endif
#endif

#include "clicommands_dvlan.h"
#include "clicommands_macvlan.h"
#include "clicommands_pbvlan.h"
#include "clicommands_ipsubnetvlan.h"

#include "clicommands_lacp.h" 
#include "clicommands_lldp.h"
#include "clicommands_dhcps.h"
#include "clicommands_protectedport.h"
#include "clicommands_pml.h"
#ifdef L7_SFLOW_PACKAGE
#include "clicommands_sflow.h"
#endif /* sFlow Package */
#include "clicommands_vlan.h"
#include "clicommands_tacacs.h" 
#include "clicommands_poe.h" 
#include "usmdb_dns_client_api.h"

#include "clicommands_debug.h"
#include "commdefs.h"
#include "usmdb_user_mgmt_api.h"

#include "clicommands_voice_vlan.h"
#include "clicommands_dhcp_client.h"

#ifdef L7_PFC_PACKAGE
  #include "clicommands_pfc.h"
  #include "pfc_exports.h"
#endif

#ifdef L7_TIMERANGES_PACKAGE
#include "clicommands_timerange.h"
#endif

#include "usmdb_sdm.h"

/* GLOBALS */
EwsCliCommandP mainMenu[FD_CLI_DEFAULT_MAX_CONNECTIONS];
cliCommon_t * cliCommon = L7_NULLPTR;
cliModes_t cliModeRO;
cliModes_t cliModeRW;
static EwsCliCommandP hiddenModeRoot = L7_NULLPTR;
static EwsCliCommandP maintenanceModeRoot = L7_NULLPTR;
L7_int32 isCreatedRO = FALSE;
L7_int32 isCreatedRW = FALSE;

#ifdef L7_QOS_PACKAGE
L7_char8 dscpHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
#endif

void cliSetThisMode (L7_uint32 mode, cliModes_t * cliModep, EwsCliCommandP node);

/*********************************************************************
 *
 * @purpose  Initialize the cli mode
 *
 * @param cliModes_t*
 *
 * @returntype void
 *
 * @notes sets pointers to all modes to NULL
 *
 * @end
 *
 *********************************************************************/
void initCliMode(cliModes_t * cliModeTemp)
{
  memset(cliModeTemp, 0, sizeof(*cliModeTemp));
  cliSetThisMode(L7_HIDDEN_COMMAND_MODE, cliModeTemp, hiddenModeRoot);
  cliSetThisMode(L7_MAINTENANCE_MODE, cliModeTemp, maintenanceModeRoot);
}

/*********************************************************************
 *
 * @purpose  Initialize the cli tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates root and then calls functions to build the rest of the tree
 *
 * @end
 *
 *********************************************************************/
void cliCommandsInit(void)
{
  L7_uint32       unit;
  L7_uint32 umodeindex;

  L7_char8 currentUser[L7_LOGIN_SIZE];

  unit = cliGetUnitId();

  umodeindex = CLI_MODE_INDEX(L7_USER_EXEC_MODE);

  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);

#if 0
  if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
  {
    /* Create main menu mainMenu[cliCurrentHandleGet()] node*/
    if( isCreatedRO == TRUE)
    {
      mainMenu[cliCurrentHandleGet()] = cliGetThisMode(L7_USER_EXEC_MODE, &cliModeRO);
      return;
    }

    initCliMode(&cliModeRO);
    cliAddUserTree();
    isCreatedRO = TRUE;

    mainMenu[cliCurrentHandleGet()] = cliGetThisMode(L7_USER_EXEC_MODE, &cliModeRO);
    return;

  }
#endif

  if( isCreatedRW == TRUE)
  {
    mainMenu[cliCurrentHandleGet()] = cliGetThisMode(L7_USER_EXEC_MODE, &cliModeRW);
    return;
  }

  initCliMode(&cliModeRW);
  cliAddUserTree();
  isCreatedRW = TRUE;
  buildTreeUserPriviledge();
  buildTreeVlanDatabase();

#ifdef L7_DHCPS_PACKAGE
  buildTreeDhcpsPoolConfigMode();
#endif

  buildTreeGlobalConfig();
  buildTreeInterfaceConfig();
  buildTreeSupport();

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  buildTreeLoopbackInterfaceConfiguration();
#ifdef L7_IPV6_PACKAGE
  buildTreeTunnelInterfaceConfiguration();
#endif
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  buildTreeMacAccessListConfig();
  buildTreeIpv6AccessListConfig();
  cliTreeIpAccessListConfig();
#endif
  /* Line config modes, Console, SSH and Telnet */
  buildTreeLineConsoleConfig();
  buildTreeLineSshConfig();
  buildTreeLineTelnetConfig();
#ifdef L7_DHCPS_PACKAGE
  buildTreeDhcpsPool();
#endif

#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agMaintenanceDomainMode();
  }
#endif
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  buildTreeDhcp6sPool();
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliDiffservBuildValDscpKeyWords(dscpHelp, sizeof(dscpHelp));
  buildTreePolicyMap();
  buildTreePolicyClassMap();
  buildTreeClassMap();
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  buildTreeCaptivePortalConfig();
  buildTreeCaptivePortalInstanceConfig();
  buildTreeCaptivePortalInstanceLocaleConfig();
  buildTreeCaptivePortalEncodedImageConfig();
#endif

#ifdef L7_WIRELESS_PACKAGE
   buildTreeWirelessConfig();
   buildTreeWirelessAPConfig();
   buildTreeWirelessNetworkConfig();
   buildTreeWirelessWdsApGroupConfig();
   buildTreeWirelessAPProfileConfig();
   buildTreeWirelessAPProfileRadioConfig();
   buildTreeWirelessAPProfileVAPConfig();
   buildTreeWirelessDevLocBldngConfig ();
   buildTreeWirelessDevLocBldngFlrConfig();
#endif /* L7_WIRELESS_PACKAGE */

/* If wireless package is not in Build, We should support Dummy wireless modes 
   to support text based configuration.*/
#ifndef L7_WIRELESS_PACKAGE
   buildTreeNonWirelessConfigSupport();
#endif /* L7_WIRELESS_PACKAGE */

  buildTreeIntAuthServUserConfig();

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
  buildTreeRouterConfigOspf();
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  buildTreeRouterConfigOspfv3();
#endif
#endif

#ifdef L7_BGP_PACKAGE
  buildTreeRouterConfigBGP();
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RIP_PACKAGE
  buildTreeRouterConfigRIP();
#endif
#endif

#ifdef L7_STACKING_PACKAGE
  buildTreeStackMode();
#endif

  /* Tacacs specific implementation. */
  buildTreeTacacsConfigMode();
  mainMenu[cliCurrentHandleGet()] = cliGetMode(L7_USER_EXEC_MODE);

  buildTreeMailServerConfigMode();

  /* Initialise and build maintanence tree */
  buildTreeMaintenanceMode();
}

/*********************************************************************
*
* @purpose  To build the Maintanence mode  tree
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
void buildTreeMaintenanceMode(void)
{
  EwsCliCommandP depth1;
  EwsCliCommandP depth2,depth3,depth4, depth5, depth6;
  
  #ifdef L7_PBVLAN_PACKAGE
  EwsCliCommandP depth7,depth8, depth9;;
  L7_uint32      unit = 0;
#endif

  depth1 = ewsCliAddNode ( NULL, NULL, NULL, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = depth3 = depth4 = depth5 = depth6 = L7_NULLPTR;

  cliSetMode(L7_MAINTENANCE_MODE, depth1);

#ifdef L7_DHCP_SNOOPING_PACKAGE


/* ip dhcp filtering */
  depth2 = ewsCliAddNode ( depth1,pStrInfo_common_IpOption,pStrInfo_common_CfgIp,
                           NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode ( depth2,pStrInfo_common_Dhcp_1,pStrInfo_base_DhcpIp,
                           NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Filtering, pStrInfo_base_DhcpFilteringGlobal,
                         commandIpDhcpSnooping, 2,  L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                           L7_NO_OPTIONAL_PARAMS);

/* ip dhcp filtering trust */
  
   depth5 = ewsCliAddNode(depth4,pStrInfo_common_Trust,pStrInfo_base_DhcpFilteringTrust,
                         commandIpDhcpSnoopingTrust, 2,  L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH);
   depth6 = ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                           L7_NO_OPTIONAL_PARAMS);
#endif




#if 0
  // depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Desc_2, pStrInfo_base_IsCmd, commandDescriptoinSyntaxChange, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Desc_2, pStrInfo_base_IsCmd, commandDescriptoinDelete, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Desc, pStrInfo_base_IntfDescrInfo, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  maintenanceModeRoot = depth1;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ROOTGUARD) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_SpanTree_2, pStrInfo_switching_SpanTree, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Rootguard, pStrInfo_switching_SpanTreeRootGuard, commandSpanningTreeRootGuard, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  /* Lineconfig  mode  -> line console in 6.0 */
  depth2 = ewsCliAddNode(depth1, "lineconfig","Enter into Line Config Mode.",cliLineConfigConsoleMode,L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_ROUTING_PACKAGE
    depth2 = ewsCliAddNode(depth1, pStrInfo_routing_Bootpdhcprelay, pStrInfo_routing_CfgRelay, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_routing_BootpDhcpRelayDsbl, commandBootpDhcpRelayEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Srvrip, pStrInfo_routing_CfgRelaySrvrIp, commandBootpDhcpRelayServerIp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif

#ifdef L7_WIRELESS_PACKAGE
  depth2 = ewsCliAddNode (depth1, "incorrect-frame-no-ack",
                            "Enable/Disable the No Acknowledgement for the correctly received frames on the radio.", 
                            commandAPProfileRadioConfigNoACK, 2,
                            L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                            NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif /* L7_WIRELESS_PACKAGE */

   if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_LAG_PRECREATE_FEATURE_ID) ) {
     depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lag_4, pStrInfo_base_IntfLag, commandInterfacePortChannel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_base_CfgLagNameString, NULL, L7_NO_OPTIONAL_PARAMS);
     depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
   }
   
   #ifdef L7_PBVLAN_PACKAGE
   if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
   {
     depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_base_CfgVlanParams, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_base_CfgProto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth4 = ewsCliAddNode(depth3, pStrInfo_common_Grp_2, pStrInfo_base_VlanProtoGrp, commandMigrationVlanProtocolGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

     depth5 = ewsCliAddNode(depth4, pStrInfo_common_Add_1, pStrInfo_base_VlanProtoGrpAdd, commandMigrationVlanProtocolGroupAdd, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth6 = ewsCliAddNode(depth5, pStrInfo_common_Proto_1, pStrInfo_base_VlanProtoGrpAdd, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth7 = ewsCliAddNode(depth6, pStrInfo_base_Grpid, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

     depth8 = ewsCliAddNode(depth7, pStrInfo_common_IpOption,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

     depth8 = ewsCliAddNode(depth7, pStrInfo_common_Arp_2,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

     depth8 = ewsCliAddNode(depth7, pStrInfo_base_Ipx_1,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

     depth5 = ewsCliAddNode(depth4, pStrInfo_base_Grpname, pStrInfo_base_CfgProtoCreateRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   }
   #endif

   buildTreeGlobalLoadBalanceMaintenanceMode(depth1);
}

/*********************************************************************
 *
 * @purpose  Initialize the devshell tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates root and then calls functions to build the rest of the tree
 *
 * @end
 *
 *********************************************************************/
void cliDevshellInit(void)
{
  buildTreeHiddenCommandMode();
}

/*********************************************************************
 *
 * @purpose  Assign the devshell tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates root and then calls functions to build the rest of the tree
 *
 * @end
 *
 *********************************************************************/
void cliDevshellAssign(void)
{
  mainMenu[cliCurrentHandleGet()] = hiddenModeRoot;
}

L7_uint32 cliGetModeIndex (L7_uint32 mode)
{
  return mode - L7_FIRST_MODE;
}

EwsCliCommandP cliGetThisMode (L7_uint32 mode, cliModes_t * cliModep)
{
  return cliModep->cliModeNode[cliGetModeIndex(mode)];
}

void cliSetThisMode (L7_uint32 mode, cliModes_t * cliModep, EwsCliCommandP node)
{
  cliModep->cliModeNode[cliGetModeIndex(mode)] = node;
}
/*********************************************************************
 *
 * @purpose  Returns the pointer to requested cli mode
 *
 * @param const L7_char8  *
 *
 * @returntype EwsCliCommandP
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
EwsCliCommandP cliGetMode (L7_uint32 mode)
{
  L7_uint32 unit;
  L7_char8 currentUser[L7_LOGIN_SIZE];
  /*get unit Id*/
  unit = cliGetUnitId();

  /*get current user*/
  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);

  /*check user type*/
  if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
  {
    return cliGetThisMode(mode, &cliModeRO);
  }

  return cliGetThisMode(mode, &cliModeRW);
}

/*********************************************************************
*
* @purpose  set parse tree to the requested mode
*
* @param const L7_char8  *
* @param EwsCliCommandP
*
* @returntype void
*
* @notes
*
* @end
*
*********************************************************************/
void cliChangeMode(L7_uint32 mode)
{
  if(mode >= L7_FIRST_MODE && mode < L7_LAST_MODE)
  {
    cliSetTree(cliGetMode(mode));
  }
}

L7_uint32 cliGetCurrentMode(void)
{
  L7_uint32 mode;
  cliModes_t * tmpMode;
  EwsCliCommandP node = mainMenu[cliCurrentHandleGet()];

  L7_uint32 unit;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  /*get unit Id*/
  unit = cliGetUnitId();

  /*get current user*/
  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);

  /*check user type*/
  if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
  {
    tmpMode = &cliModeRO;
  }
  else
  {
    tmpMode = &cliModeRW;
  }

  for(mode = L7_FIRST_MODE; mode < L7_LAST_MODE; mode++)
  {
    if (cliGetThisMode(mode, tmpMode) == node)
    {
      return mode;
    }
  }

  return 0;
}

/*********************************************************************
 *
 * @purpose  Sets the pointer to requested cli mode
 *
 * @param const L7_char8  *
 * @param EwsCliCommandP
 *
 * @returntype void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void cliSetMode(L7_uint32 mode, EwsCliCommandP objMode)
{
  L7_uint32 unit;
  L7_uint32 modeIndex;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  /*get unit Id*/
  unit = cliGetUnitId();

  /*get current user*/
  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);

  /*check user type*/
  modeIndex = cliGetModeIndex(mode);
  if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
  {
    cliModeRO.cliModeNode[modeIndex] = objMode;
  }
  else
  {
    cliModeRW.cliModeNode[modeIndex] = objMode;
  }
}

/*********************************************************************
 *
 * @purpose  Set the parse tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the user tree
 *
 * @end
 *
 *********************************************************************/
void cliSetTree( EwsCliCommandP newTree)
{
  mainMenu[cliCurrentHandleGet()] = newTree;
}

/*********************************************************************
 *
 * @purpose  Set the parse tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the user tree
 *
 * @end
 *
 *********************************************************************/
L7_char8 *cliSetRootMode(void)
{
  EwsCliCommandP privMode = cliGetMode(L7_PRIVILEGE_USER_MODE);
  EwsCliCommandP userMode = cliGetMode(L7_USER_EXEC_MODE);
  L7_uint32 handle = cliCurrentHandleGet();

  if (mainMenu[handle] == privMode)
  {
    cliPrevDepthSet(0);
    osapiSnprintf(cliCommon[handle].prompt, sizeof(cliCommon[handle].prompt),
                  "%s%s", cliUtil.systemPrompt, pStrInfo_base_RootPrompt);
    cliChangeMode (L7_USER_EXEC_MODE);
  }
  else if (mainMenu[handle] == userMode)
  {
    /*do nothing*/
  }
  else
  {
    cliPrevDepthSet(0);
    osapiSnprintf(cliCommon[handle].prompt, sizeof(cliCommon[handle].prompt),
                  "%s%s", cliUtil.systemPrompt, pStrInfo_common_PriviledgeUsrExecModePrompt);
    cliChangeMode (L7_PRIVILEGE_USER_MODE);
  }

  return cliCommon[cliUtil.handleNum].prompt;
}

EwsCliCommandP cliGetCurrentRoot (void)
{
  return mainMenu[cliCurrentHandleGet()];
}

/*********************************************************************
 *
 * @purpose  Set the parse tree to PriviledgeUser Mode
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
L7_char8 *cliSetPriviledgeUserMode(void)
{
  EwsCliCommandP privMode = cliGetMode(L7_PRIVILEGE_USER_MODE);
  L7_uint32 handle = cliCurrentHandleGet();

  if (mainMenu[handle] != privMode)
  {
    osapiSnprintf(cliCommon[handle].prompt, sizeof(cliCommon[handle].prompt),
                  "%s%s", cliUtil.systemPrompt, pStrInfo_common_PriviledgeUsrExecModePrompt);
    cliChangeMode (L7_PRIVILEGE_USER_MODE);
  }
  return cliCommon[cliUtil.handleNum].prompt;
}

/*********************************************************************
 *
 * @purpose  Initialize the user Tree
 *
 * @param void
 *
 * @returntype void
 *
 * @notes New function for user exec mode
 *
 * @end
 *
 *********************************************************************/
void cliAddUserTree(void)
{
  EwsCliCommandP depth1, depth2, depth3;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  usmDbLoginSessionUserGet(U_IDX,cliLoginSessionIndexGet(), currentUser);

  depth1= ewsCliAddNode ( NULL, NULL, NULL, cliRoot, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_USER_EXEC_MODE, depth1);

  /* No Need to check for User Access Level here. The Access level will be checked in Enable mode */

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Enbl_2, pStrInfo_base_UsrPriviledgedMode, cliUserPriviledgeMode, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_PRIVILEGE_USER_MODE);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Logout_1, pStrInfo_base_LogOut, commandLogout, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeNewPasswd(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Ping_3, pStrInfo_base_Ping_1, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePing(depth2);
#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agPingPrevilege(depth2);
  }
#endif
  depth2 = ewsCliAddNode ( depth1, pStrErr_base_QuitCmd, pStrInfo_base_LogOut, commandQuit, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeUserShow(depth1);
#ifdef L7_OUTBOUND_TELNET_PACKAGE
  buildTreeUserExecSWMgmtOutboundTelnet(depth1);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
}


/*********************************************************************
*
* @purpose  Build the ping command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes creates ping command at the given depth
*
* @end
*
*********************************************************************/
void buildTreePing(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth7, depth8, depth9;
#endif
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  EwsCliCommandP depth10;
#endif
#endif

#ifdef L7_RLIM_PACKAGE
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  EwsCliCommandP depth20;
#endif


  depth3 = ewsCliAddNode(depth2, "<ipaddress|hostname> ", pStrInfo_base_PingIp, commandPing, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  /* ping <addr> count */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Count_1, pStrInfo_base_PingCount, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to15, pStrInfo_base_PingCountRange, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddTree(depth5, depth3);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* ping <addr> interval */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_WsInputIntvl, pStrInfo_base_PingIntvl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range1to60, pStrInfo_base_PingIntvlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddTree(depth5, depth3);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* ping <addr> size*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Size_1, pStrInfo_base_PingSize_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to65507, pStrInfo_base_PingSizeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddTree(depth5, depth3);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
  /* ping <addr> source <source intf> */
  depth4 = ewsCliAddNode(depth3, "source", "Specify the source interface or address", NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddTree(depth5, depth3);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
#ifdef L7_RLIM_PACKAGE
  /* use loopback interface as source */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_LoopBack, "Use a loopback interface as the source of the ping", 
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_LoopBackIntfId, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddTree(depth6, depth3);
  depth20 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  depth5 = ewsCliAddNode(depth4, "<ip-address>", "Source IPv4 address", NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  ewsCliAddTree(depth5, depth3);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif


#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_Ipv6Ping, commandIpv6Ping, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_Ipv6PingIntf_1, commandIpv6PingInterface, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LinkLocalAddr, pStrInfo_base_Ipv6PingLocalLink, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
#endif
 if (usmDbComponentPresentCheck(L7_NULL, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE )
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_CfgServiceportKeyword, pStrInfo_base_Ipv6PingIntf_1, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_LinkLocalAddr, pStrInfo_base_Ipv6PingLocalLink, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_WsShowRunningNwMode, pStrInfo_base_Ipv6PingIntf_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LinkLocalAddr, pStrInfo_base_Ipv6PingLocalLink, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
#ifdef L7_RLIM_PACKAGE
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_LoopBack, pStrInfo_base_PingLoopBackIntf, commandIpv6PingLoopback, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, NULL, pStrInfo_common_LoopBackIntfId, NULL, 3, L7_NODE_UINT_RANGE, L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LinkLocalAddr, pStrInfo_base_Ipv6PingLocalLink, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tunnel_1, pStrInfo_base_PingTunnelIntfId, commandIpv6PingTunnel, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, NULL, pStrInfo_common_TunnelIntfId, NULL, 3, L7_NODE_UINT_RANGE, L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LinkLocalAddr, pStrInfo_base_Ipv6PingLocalLink, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /* L7_IPV6_PACKAGE */
#endif
#endif /* L7_ROUTING_PACKAGE */

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6HostAddr, pStrInfo_base_Ipv6PingHostAddr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Size_1, pStrInfo_base_Ipv6PingSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_DatagramSize, pStrInfo_base_Ipv6PingSizeRange48And2048, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif /* L7_IPV6_PACKAGE */

}

/*********************************************************************
 *
 * @purpose  Build the config command tree
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes New function to add show command in the user exec mode
 *
 * @end
 *
 *********************************************************************/
void buildTreeUserShow(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3 = 0, depth4 = 0;
#ifdef L7_ROUTING_PACKAGE
  EwsCliCommandP depth5, depth6;
#endif
  L7_uint32       unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Show_1, pStrInfo_base_Show, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
  buildTreeShowBootPdhcpRelay(depth2);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeUserExecClassMap(depth2);
#endif

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID,  L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Classofservice, pStrInfo_base_ShowClassOfService, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivDot1pShow(depth3);
  }
#ifdef L7_QOS_FLEX_PACKAGE_COS

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceIPDscpMapping(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceIPPrecedenceMapping(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceTrust(depth3);
  }

#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, 
                         pStrInfo_base_DhcpShowHelp, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeShowDhcpClient(depth3);
#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  buildTreeShowDhcpL2Relay(depth3);
#endif
#endif

  /* Denial of Service */
  if (usmDbComponentPresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeShowDenialOfService(depth2);
  }
  /* End - Denial of Service */

#ifdef L7_DVLAN_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreeUserSwDevShowDot1qTunnel(depth2);
    buildTreeUserSwDevShowDVlanTunnel(depth2);
  }
#endif

  buildTreeUserSwDevShowGarp(depth2);
  buildTreeUserSwDevShowGarpConfiguration(depth2);

  buildTreeShowHosts(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_base_ShowIp, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeShowIpAccesslist(depth3);
#endif

#ifdef L7_BGP_PACKAGE
  cliTreeShowBGPUserExec(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeShowBootPdhcpRelay(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowIpSummary, commandShowIpBrief, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpsIpShow, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DHCPS_PACKAGE
  buildTreeShowDHCPs(depth4);
#endif

#ifdef L7_DHCP_FILTER_PACKAGE
 /* command      :     show ip dhcp filtering  */
  buildTreeShowIpDhcpFiltering(depth4);
  /* command      :     show ip dhcp filtering    - End  */

#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowDvmrp(depth3);
  cliTreeUserShowIgmp(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowIpIntf, commandShowIpInterface, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Brief, pStrInfo_base_ShowIpIntfBrief, commandShowIpInterfaceBrief, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbComponentPresentCheck(unit, L7_RTR_DISC_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeShowIrdp(depth3);
  }
/* Helper Address show commands */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Helper_Addr_4, pStrInfo_base_ShowHelperIp, commandShowIpHelperAddress, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Helper, pStrInfo_base_ShowHelperIpStatistics, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_PoeStatistics, pStrInfo_base_ShowHelperIpStatistics, commandShowIpHelperStats, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowMulticast(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
  buildTreeShowOspf(depth3);
#endif
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowPimdmPimsm(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RIP_PACKAGE
  buildTreeShowRIP(depth3);
#endif
  buildTreeShowIpRoute(depth3);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Stats, pStrInfo_base_ShowIpStats, commandShowIpStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ShowIpVlanRouting, commandShowIpVlan, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_VRRP_PACKAGE
  buildTreeShowVrrp(depth3);
#endif
#endif

  /* used to detect if L7_QOS_FLEX_PACKAGE_ACL needs to create the ipv6 node */
  depth3 = L7_NULLPTR;
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowIpv6, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeShowIpv6Brief(depth3);
  buildTreeShowIpv6Dhcp(depth3);
  buildTreeShowIpv6Interface(depth3);
  buildTreeShowIpv6Neighbors(depth3);
  buildTreeShowOspfv3(depth3);
  buildTreeShowIpv6Route(depth3);
  buildTreeShowIpv6Traffic(depth3);
  buildTreeShowIpv6Vlan(depth3);
#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowIpv6Multicast(depth3);
  cliTreeUserShowIpv6Pimdm(depth3);
  buildTreeShowIpv6Mld(depth3);
#endif
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
  {
    if (L7_NULLPTR == depth3)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowIpv6, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    cliTreeShowIpv6Accesslist(depth3);
  }
#endif

  /*Check if Network Port Feature is supported */
  if (usmDbFeaturePresentCheck (unit, L7_SIM_COMPONENT_ID,
                                L7_SIM_NETWORK_PORT_FEATURE_ID)
                             == L7_TRUE)
  {
    buildTreeUserExecSWMgmtShowNetwork(depth2);
  }

  buildTreeUserExecSWMgmtShowPortChannel(depth2);
  buildTreeUserExecSWMgmtShowSerial(depth2);
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreePrivilegedExecShowServicePolicy(depth2);
#endif
  buildTreeUserExecSWMgmtShowServicePort(depth2);
  buildTreeUserExecShowSlot(depth2);
  buildTreeUserExecSWSpanShowSpanningTree(depth2);
  buildTreeUserExecShowSupported(depth2);
#ifdef L7_STACKING_PACKAGE
  buildTreeUserExecShowStack(depth2);
#endif

#ifdef L7_OUTBOUND_TELNET_PACKAGE
  buildTreeUserExecSWMgmtShowOutboundTelnet(depth2);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
  buildTreeUserExecSWMgmtShowTelnetCon(depth2);

  buildTreeUserSwDevShowVlan(depth2);
   buildTreeUserProtectedPort(depth2);

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_VRRP_PACKAGE
  buildTreeShowVrrp(depth3);
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
  buildTreeUserExecShowWireless(depth2);
#endif /* L7_WIRELESS_PACKAGE */


}

/*********************************************************************
 *
 * @purpose  Build the User priviledge tree
 *
 * @param void
 *
 * @returntype void
 *
 * @note This function create the privildge tree.
 *
 * @end
 *
 *********************************************************************/
void buildTreeUserPriviledge()
{
  EwsCliCommandP depth1,depth2,depth3, depth4, depth5;

#if defined (L7_ROUTING_PACKAGE) || defined (L7_DAI_PACKAGE)
  EwsCliCommandP depth6;
#ifdef L7_RLIM_PACKAGE
  EwsCliCommandP depth7;
#endif
#endif
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth1 = ewsCliAddNode ( NULL, NULL, NULL, cliUserPriviledgeMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_PRIVILEGE_USER_MODE, depth1);

#ifdef L7_ROUTING_PACKAGE
  buildTreePrivArp(depth1);
#endif

  if (usmDbComponentPresentCheck(unit, L7_CABLE_TEST_COMPONENT_ID) == L7_TRUE)
  {
    cliTreePrivilegeCableStatus(depth1);
  }

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  buildTreePrivCaptivePortal(depth1);
#endif

  cliTreePrivilegedConfigClear(depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Cfgure_1, pStrInfo_base_CfgMode, cliConfigMode, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_GLOBAL_CONFIG_MODE);

  cliTreePrivilegedConfigCopy(depth1);

  cliTreePrivilegedImage(depth1);

  cliTreePrivilegedFile(depth1);

  buildTreePrivSwUamDisconnect(depth1);

  /*No CliBanner command*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Clibanner, pStrInfo_base_DelCliBanner, commandNoCliBanner, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );

  /*No clibanner*/

  /* Dot1x commands*/
  buildTreePrivDot1xConfig(depth1);

  /* enable passwd command */
  buildTreeEnablePasswd(depth1);
  
  /* Command erase startup-config */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Erase, pStrInfo_base_EraseHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_StartupConfig, pStrInfo_base_StartupConfigHelp, commandEraseStartupConfig, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Switching device commands*/

  depth2 =  ewsCliAddNode ( depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreePrivHostname(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_IpOption, pStrInfo_common_CfgIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Http_1, pStrInfo_base_IpHttpCfg, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Java, pStrInfo_base_IpHttpJavaCfg, commandIpHttpJava, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreePrivIpHttpSecure(depth3);
  }
  buildTreePrivIpSsh(depth2);
#endif /* L7_MGMT_SECURITY_PACKAGE */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreePrivIpHttp(depth3);
  }

 
  /* show the debug tree */
  buildTreePrivledgedDebug(depth1);


#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
/* Remote loopback */
buildTreePrivDot3ah(depth1);
#endif /* L7_DOT3AH_PACKAGE */
#endif /* L7_METRO_FLEX_PACKAGE */

  /* show telnet tree */
  buildTreePrivIpTelnet(depth2);

  /*Check if keying feature is supported*/
  if (usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) == L7_TRUE)
  {
    depth2 =  ewsCliAddNode ( depth1, pStrInfo_base_License_1, pStrInfo_base_LicenseAdvancedHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 =  ewsCliAddNode ( depth2, pStrInfo_base_Advanced, pStrInfo_base_LicenseAdvancedHelp, commandLicenseAdvanced, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 =  ewsCliAddNode ( depth3, pStrInfo_common_Key, pStrInfo_base_Key_1, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 =  ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  cliTreePrivilegedConfigLogout(depth1);

  /*Check if Network Port Feature is supported */
  if (usmDbFeaturePresentCheck (unit, L7_SIM_COMPONENT_ID,
                                L7_SIM_NETWORK_PORT_FEATURE_ID)
                             == L7_TRUE)
  {
    buildTreePrivSWMgmtNetworkConf(depth1);
  }

  /* quit command can be accessed from Priviledge level and user level*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Ping_3, pStrInfo_base_Ping_1, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePing(depth2);
#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agPingPrevilege(depth2);
  }
#endif
  depth2 = ewsCliAddNode ( depth1, pStrErr_base_QuitCmd, pStrInfo_base_LogOut, commandQuit, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* NOTE: New reload() gets called within this existing function, */
  /*       if STACKING is supported                                */
  cliTreePrivilegedConfigPingReload(depth1);

  cliTreePrivilegedConfigConfigScript(depth1);
  buildTreePrivSWMgmtServicePortConfig(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Set_2, pStrInfo_base_Set_1, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePrivSwDevSetGmrpGvrpAdmin(depth2);

  /* Show command node*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Show_1, pStrInfo_base_Show, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_mailserver, pStrInfo_base_mailServer_Show, commandShowLoggingMailServerConfig, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, "<ip-address | hostname>", pStrInfo_base_mailServer_Show, NULL, L7_NO_OPTIONAL_PARAMS);	
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_mailserver_config, pStrInfo_base_mailServer_Show_config,NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);  
  depth4 = ewsCliAddNode ( depth3, "all", pStrInfo_base_mailServer_Show, NULL, L7_NO_OPTIONAL_PARAMS);		
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_mailserver_config, pStrInfo_base_mailServer_Show_config,NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);  


#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeShowAccesslist(depth2);
#endif

  buildTreeShowArp(depth2);

  buildTreePrivShowAuthentication(depth2);


/* show autoinstall */
#ifdef L7_AUTO_INSTALL_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_AutoInstall, pStrInfo_base_ShowAutoInstallHelp, commandShowAutoinstall, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /* L7_AUTO_INSTALL_PACKAGE */

#ifdef L7_STACKING_PACKAGE
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
                               L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
  {
    buildTreeUserExecShowAutoCopySw(depth2);
  }
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeShowBootPdhcpRelay(depth2);
#endif

  /* Dual Image */
  depth3 = ewsCliAddNode (depth2, pStrInfo_base_BootVar, pStrInfo_base_ShowBoot, commandShowBoot, 2, L7_SYNTAX_NORMAL, CLISYNTAX_SHOWBOOT);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Unit_1, CLIDIM_UNIT_HELP, NULL, 3, L7_NODE_UINT_RANGE, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  buildTreePrivShowCaptivePortal(depth2);
#endif

#ifdef L7_NSF_PACKAGE
  buildTreeShowCheckpointStats(depth2);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeUserExecClassMap(depth2);
#endif

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID,  L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Classofservice, pStrInfo_base_ShowClassOfService, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  buildTreeShowClock(depth2);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivDot1pShow(depth3);
  }

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceIPDscpMapping(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceIPPrecedenceMapping(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE)
  {
    buildTreePrivShowClassOfServiceTrust(depth3);
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
  buildTreePrivShowIscsi(depth2);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  /* Auto VoIP Show Commands */
  if (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreePrivShowAutoVoIP(depth2);
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreePrivilegedExecShowDiffserv(depth2);
#endif
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, 
                         pStrInfo_base_DhcpShowHelp, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeShowDhcpClient(depth3);
#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  buildTreeShowDhcpL2Relay(depth3);
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeShowDhcpLease(depth3);
#endif

#ifdef L7_DVLAN_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreeUserSwDevShowDot1qTunnel(depth2);
  }
#endif

  buildTreePrivDot1xShow(depth2);

  /* Denial of Service */
  if (usmDbComponentPresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeShowDenialOfService(depth2);
  }
  /* End - Denial of Service */


#ifdef L7_METRO_FLEX_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) 
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Dot1ad, 
                      pStrInfo_base_ShowDot1ad, NULL, 2, 
                      L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_DOT1AD_PACKAGE
    buildTreedot1adShow(depth3);
    buildTreePrivDot1adShow(depth3);
#endif
  }  
#endif

#if defined(L7_METRO_FLEX_PACKAGE) || defined(L7_DOT1AG_PACKAGE)
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Dot3ah,
                      pStrInfo_base_ShowEthernet, NULL, 2,
                      L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3,pStrInfo_base_Dot3ah1,pStrInfo_base_ShowDot3ah, NULL, 2,
                      L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    buildTreedot3ahShow(depth4);
  }
#endif /* L7_DOT3AH_PACKAGE */ 
#endif /* L7_METRO_FLEX_PACKAGE */

#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agShow(depth3);
  }
#endif

#ifdef L7_DVLAN_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreeUserSwDevShowDVlanTunnel(depth2);
  }
#endif

  buildTreePrivSwInfoEventLog(depth2);
  buildTreePrivShowDebugging(depth2);
  buildTreePrivSWMgmtShowForwardingDB(depth2);

  buildTreeUserSwDevShowGarp(depth2);
  buildTreeUserSwDevShowGarpConfiguration(depth2);

  buildTreeShowHosts(depth2);

  buildTreePrivSwInfoHardware(depth2);
  buildTreePrivSwDevShowIgmpInterface(depth2, L7_AF_INET);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowIpIntf, commandShowInterfaceSummary, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivSwInfoShowInterfaceEthernet(depth3);

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  buildTreeShowLoopbackInfo(depth3);
#endif
#endif

  buildTreePrivSwInfoShowInterfaceSwitchport(depth3);

#ifdef L7_ROUTING_PACKAGE

#ifdef L7_IPV6_PACKAGE
#ifdef L7_RLIM_PACKAGE
  buildTreeShowTunnelInfo(depth3);
#endif
#endif
#endif

/* 'show interfaces should show for CosQueue and/or protected ports */
#ifdef L7_QOS_FLEX_PACKAGE_COS
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Intfs_1,pStrInfo_base_ShowIntfs, NULL,
                          L7_NO_OPTIONAL_PARAMS);
   buildTreePrivSwInfoShowInterfacesCosQueue(depth3);
#else
   if (usmDbComponentPresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Intfs_1,pStrInfo_base_ShowIntfs, NULL,
                           L7_NO_OPTIONAL_PARAMS);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

  if (usmDbComponentPresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID) == L7_TRUE)
  {
   buildTreeShowInterfacesSwitchPort(depth3);
  }

#ifdef L7_QOS_FLEX_PACKAGE_COS
  buildTreePrivSwInfoShowInterfacesTailDropThreshold(depth3);
#endif /* L7_QOS_FLEX_PACKAGE_COS */

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_base_ShowIp, NULL, L7_NO_OPTIONAL_PARAMS);
 
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeShowIpAccesslist(depth3);
#endif

#ifdef L7_BGP_PACKAGE
  cliTreeShowBGPPrivMode(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowIpSummary, commandShowIpBrief, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  /* get back to this node intentionally.because it
   * will be useful to DHCP and DHCP Filtering */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpsIpShow, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DHCPS_PACKAGE
  buildTreeShowDHCPs(depth4);
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
  buildTreeShowIpDhcpSnooping(depth4);
#endif

  /* show ip address-conflict */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AddrConflict, pStrInfo_base_AddrConflictShow,
                         commandShowIpAddrConflict, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DAI_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Arp_2, pStrInfo_base_DaiShow, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ArpInspec, pStrInfo_base_DaiShow, commandShowDai, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeShowDai(depth5);

#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowDvmrp(depth3);
#endif

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreePrivShowIpHttp(depth3);
  }

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowIgmp(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowIpIntf, commandShowIpInterface, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Brief, pStrInfo_base_ShowIpIntfBrief, commandShowIpInterfaceBrief, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_RLIM_PACKAGE
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_LoopBack, pStrInfo_common_ShowLoopBackIntf, commandShowIpInterface, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, NULL, pStrInfo_common_LoopBackIntfId, NULL, 5, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_UINT_RANGE, L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  if (usmDbComponentPresentCheck(unit, L7_RTR_DISC_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeShowIrdp(depth3);
  }
/* Helper Address show commands */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Helper_Addr_4, pStrInfo_base_ShowHelperIp, commandShowIpHelperAddress, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Helper, pStrInfo_base_ShowHelperIpStatistics, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_PoeStatistics, pStrInfo_base_ShowHelperIpStatistics, commandShowIpHelperStats, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowMulticast(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
  buildTreeShowOspf(depth3);
#endif
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowPimdmPimsm(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RIP_PACKAGE
  buildTreeShowRIP(depth3);
#endif
  buildTreeShowIpRoute(depth3);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
  buildTreePrivShowIpSsh(depth3);
#endif /* L7_MGMT_SECURITY_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Stats, pStrInfo_base_ShowIpStats, commandShowIpStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) == L7_TRUE)
  {
    buildTreeShowIpVerifySource(depth3);
    buildTreeShowIpSourceBinding(depth3);
  }
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ShowIpVlanRouting, commandShowIpVlan, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_VRRP_PACKAGE
  buildTreeShowVrrp(depth3);
#endif
#endif

  /* used to detect if L7_QOS_FLEX_PACKAGE_ACL needs to create the ipv6 node */
  depth3 = L7_NULLPTR;
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowIpv6, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeShowIpv6Brief(depth3);
  buildTreeShowIpv6Dhcp(depth3);
  buildTreeShowIpv6Interface(depth3);
  buildTreeShowIpv6Neighbors(depth3);
  buildTreeShowOspfv3(depth3);
  buildTreeShowIpv6Route(depth3);
  buildTreeShowIpv6Traffic(depth3);
  buildTreeShowIpv6Vlan(depth3);
#ifdef L7_MCAST_PACKAGE
  cliTreeUserShowIpv6Multicast(depth3);
  cliTreeUserShowIpv6Pimdm(depth3);
  cliTreeUserShowIpv6Pimsm(depth3);
  buildTreeShowIpv6Mld(depth3);
#endif
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
  {
    if (L7_NULLPTR == depth3)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowIpv6, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    cliTreeShowIpv6Accesslist(depth3);
  }
#endif

#ifdef L7_ISDP_PACKAGE
  /* Adding isdp show commands to the tree.*/
  buildTreePrivIsdp(depth2);
#endif /* L7_ISDP_PACKAGE */

#ifdef L7_LLPF_PACKAGE
  /* Adding llpf show commands to the tree.*/
  buildTreePrivLlpf(depth2);
#endif /* L7_LLPF_PACKAGE */

  /*Check if keying feature is supported*/
  if (usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) == L7_TRUE)
  {
    buildTreeShowKeyingStatus(depth2);
  }

  /* Adding lacp show commands to the tree.  */
  buildTreeShowConfigLacp(depth2);

  /* Adding lldp show commands to the tree. */
  buildTreeShowConfigLldp(depth2);

  if (usmDbComponentPresentCheck(unit, L7_LOG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreePrivSwInfoLogging(depth2);
  }
  buildTreeUserExSwUamShowLogin(depth2);

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeShowMacAccessList(depth2);
#endif

#ifdef L7_TIMERANGES_PACKAGE
  /* Adding time-range show commands to the tree.*/
  cliTreeShowTimeRange(depth2);
#endif

#ifdef L7_STATIC_FILTERING_PACKAGE
  buildTreePrivSwInfoMacAddrTable(depth2);
  buildTreePrivSwDevMacAddrTable(depth2);
#endif

  buildTreePrivSwDevShowMonitor(depth2);
  buildTreePrivSwDevShowIgmpInterface(depth2, L7_AF_INET6);
  /*Check if Network Port Feature is supported */
  if (usmDbFeaturePresentCheck (unit, L7_SIM_COMPONENT_ID,
                                L7_SIM_NETWORK_PORT_FEATURE_ID)
                             == L7_TRUE)
  {
    buildTreeUserExecSWMgmtShowNetwork(depth2);
  }

#ifdef L7_NSF_PACKAGE
  buildTreeShowNsf(depth2);
#endif

  buildTreePrivShowPasswordsConfiguration(depth2);

#ifdef L7_POE_PACKAGE
/* PoE show commands */
  for (unit = 1; unit < L7_ALL_UNITS; unit++)
  {
    if (usmDbFeaturePresentCheck(unit, L7_POE_COMPONENT_ID, 
                                 L7_POE_FEATURE_ID) == L7_TRUE)
    {
       buildTreePrivPoeShow(depth2);
       break;
    }
  }
#endif

  unit = cliGetUnitId();
/* End of PoE show commands */

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreePrivilegedExecPolicyMap(depth2);
#endif

  buildTreePrivSwDevPort(depth2);
  buildTreePrivSwDevShowPortChannel(depth2);
  buildTreePrivSwDevShowPortSecurity(depth2);
  buildTreePrivCpuInfo(depth2);
  buildTreePrivilegedRadiusShowRadius(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_RunningCfg, pStrInfo_base_ShowRunningCfg,commandShowRunningConfig, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_ShowRunningCfgAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Scriptname, pStrInfo_base_ShowRunningCfgScript, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_ROUTING_PACKAGE
  /* Only SDM templates in initial release are for routing packages. So 
   * only include the show command in builds with the routing package. */
  buildTreeShowSdm(depth2);
#endif

  buildTreeUserExecSWMgmtShowSerial(depth2);
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreePrivilegedExecShowServicePolicy(depth2);
#endif
  buildTreeUserExecSWMgmtShowServicePort(depth2);
#ifdef L7_SFLOW_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    buildTreeUserExecSWMgmtShowSflow(depth2);
  }
#endif 
  buildTreeUserExecShowSlot(depth2);
  buildTreePrivSWMgmtShowSnmpData(depth2);

  if (usmDbComponentPresentCheck(unit, L7_SNTP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreePrivSwInfoSntp(depth2);
  }
  buildTreeUserExecSWSpanShowSpanningTree(depth2);
#ifdef L7_STACKING_PACKAGE
  buildTreeShowStackPort(depth2);
#endif

  buildTreePrivSwDevStormControl(depth2);
  buildTreeUserExecShowSupported(depth2);

#ifdef L7_STACKING_PACKAGE
  buildTreeUserExecShowStack(depth2);
#endif
  buildTreePrivSwInfoSysInfo(depth2);

  buildTreePrivSwInfoShowTechSupport(depth2);

#ifdef L7_OUTBOUND_TELNET_PACKAGE
  buildTreeUserExecSWMgmtShowOutboundTelnet(depth2);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
  buildTreeUserExecSWMgmtShowTelnetCon(depth2);

  buildTreeShowTerminalLength(depth2);
  buildTreePrivSWMgmtShowTrapFlags(depth2);

  buildTreeTacacsShow(depth2);

#ifdef L7_TR069_PACKAGE 
  if (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeShowTr069(depth2);
  }
#endif

  buildTreePrivSwUamShowUsers(depth2);

  buildTreePrivSwInfoShowVersion(depth2);

  buildTreeUserSwDevShowVlan(depth2);

  buildTreeShowIntAuthServUsers(depth2);

#ifdef L7_MGMT_SECURITY_PACKAGE
  buildTreePrivSWMgmtSSHConConf(depth1);
#endif /* L7_MGMT_SECURITY_PACKAGE */

 /* show the hidden support tree */
  cliTreePrivilegedSupport(depth1);

    /* Voice Vlan */
  buildTreeUserExecVoiceVlan(depth2);


   buildTreeUserProtectedPort(depth2);

#ifdef L7_WIRELESS_PACKAGE
  buildTreePrivShowWireless(depth2);
#endif /* L7_WIRELESS_PACKAGE */


#ifdef L7_PFC_PACKAGE
  /* PFC Show commands */
  if (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                            L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) 
                            == L7_TRUE)
  {
    buildTreePrivPfcShowDataCenterBridging(depth2);
  }
#endif

#ifdef L7_OUTBOUND_TELNET_PACKAGE
  buildTreeUserExecSWMgmtOutboundTelnet(depth1);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
  buildTreePrivSWMgmtTelnetConConf(depth1);

  buildTreeUserExecTerminalLength(depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Traceroute_1, pStrInfo_base_TraceRoute, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRoute(depth2);
#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agTraceroutePrevilege(depth2);
  }
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1,pStrInfo_base_Vlan, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dbase,pStrInfo_base_Vlan, cliVlanMode, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_VLAN_MODE);

#ifdef L7_WIRELESS_PACKAGE
  buildTreePrivWireless(depth1);
#endif /* L7_WIRELESS_PACKAGE */

  buildTreePrivWriteMemory(depth1);

#ifdef L7_ROUTING_PACKAGE
  buildTreePrivSWMgmtDhcpConfRelease(depth1);
#endif /* L7_ROUTING_PACKAGE */

  buildTreePrivSWMgmtDhcpConfRenew(depth1);
}

/*********************************************************************
 *
 * @purpose  Build the Vlan database mode tree.
 *
 * @param void
 *
 * @returntype void
 *
 * @note This function create the vlan database tree.
 *
 * @end
 *
 *********************************************************************/
void buildTreeVlanDatabase()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliVlanMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_VLAN_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 =  ewsCliAddNode ( depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeVlanDBSwDevVlan(depth1);
}

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
/*********************************************************************
 *
 * @purpose  Build the DHCP Pool Config Mode Tree.
 *
 * @param void
 *
 * @returntype void
 *
 * @note This function create the DHCP Pool Config tree.
 *
 * @end
 *
 *********************************************************************/
void buildTreeDhcp6sPool()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliDhcp6sPoolConfigMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_DHCP6S_POOL_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 =  ewsCliAddNode ( depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeDhcp6sPoolConfigMode(depth1);
}
#endif
#endif

/*********************************************************************
 *
 * @purpose  To build the Global config tree
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
void buildTreeGlobalConfig()
{
  EwsCliCommandP depth1,depth2,depth3,depth4, depth5, depth6, depth7, depth8,depth9, depthaaa, depthdns;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  EwsCliCommandP depth2v6 = L7_NULLPTR;
#else
#ifdef L7_IPV6_MGMT_PACKAGE
  EwsCliCommandP depth2v6 = L7_NULLPTR;
#else
#ifdef L7_ROUTING_PACKAGE
  #ifdef L7_IPV6_PACKAGE
  EwsCliCommandP depth2v6 = L7_NULLPTR;
  #endif
  #endif
#endif
#endif

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
  EwsCliCommandP depth9;
#endif

  L7_uint32  unit;

  depth1 = ewsCliAddNode ( NULL, NULL, NULL, cliConfigMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_GLOBAL_CONFIG_MODE, depth1);

  unit = cliGetUnitId ();

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeAccessList(depth1);
  cliTreeAclTrapflags(depth1);
#endif

#if defined (L7_ROUTING_PACKAGE) || defined (L7_DAI_PACKAGE)
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Arp_2, pStrInfo_base_ArpConfig, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif

#ifdef L7_ROUTING_PACKAGE
  /* ARP config mode */
  buildTreeGlobalArpBootp(depth2);
#endif

#ifdef L7_DAI_PACKAGE
  /* ARP ACL Config mode */
  buildTreeArpAccessListConfig();
  buildTreeGlobalArpAccessList(depth2);
#endif

  depthaaa = ewsCliAddNode(depth1, "aaa ", pStrInfo_base_Auth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeGlobalAuthenticationConfig(depthaaa);
  buildTreeIntAuthServUserDBConfig(depthaaa);

  if (usmDbFeaturePresentCheck (unit, L7_DOT1X_COMPONENT_ID, 
                             L7_DOT1X_VLANASSIGN_FEATURE_ID) == L7_TRUE)
  {
  buildTreeGlobalAuthorizationConfig(depth1);
  }

  buildTreeGlobalSwDevDuplexAll(depth1);

#ifdef L7_ROUTING_PACKAGE
    /* BOOTP config mode */
    buildTreeGlobalBootp(depth1);
#endif

  buildTreeGlobSWMgmtBridgeTime(depth1);

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  buildTreeGlobalCaptivePortal(depth1);
#endif

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID,  L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Classofservice, pStrInfo_base_CfgClassOfService, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    /* Dot1p commands*/
    if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
    {
      buildTreeCosDot1pConfig(depth2);
    }

#ifdef L7_QOS_FLEX_PACKAGE_COS
    cliDiffservBuildValDscpKeyWords(dscpHelp, sizeof(dscpHelp));

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      buildTreeCosIPDscpMapping(depth2);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
    {
      buildTreeCosIPPrecedenceMapping(depth2);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE)
    {
      buildTreeGlobalCosTrust(depth2);
    }
#endif /* L7_QOS_FLEX_PACKAGE_COS */
  }


  /* ClassMap mode*/
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeGlobalConfigClassMap(depth1);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  /* Cos Queue Command */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE  ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) != L7_TRUE)
  {
    buildTreeGlobalCosQueue(depth1);
  }

#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
  /* iscsi Command */
    buildTreeGlobalIscsi(depth1);
#endif /* L7_QOS_FLEX_PACKAGE_ISCSI */

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  /* Auto VoIP Commands */
  if (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeGlobalAutoVoIP(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_VOIP */


#ifdef L7_MGMT_SECURITY_PACKAGE
  buildTreeGlobalCryptoSsh(depth1);
#endif


  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpL2Relay, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeGlobalDhcpClient(depth2);
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  buildTreeGlobalDhcpL2Relay(depth2);
  #endif

  buildTreeGlobalDeletePortAll(depth1);

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeGlobalConfigDiffserv(depth1);
#endif
  /* Denial of Service */
  if (usmDbComponentPresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeGlobalDenialOfService(depth1);
  }
  /* End - Denial of Service */

  buildTreeGlobalDot1xConfig(depth1);
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
  buildTreeGlobalDot1adConfig(depth1);
#endif

#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_DhcpEtherNet, pStrInfo_common_ethernet_help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    buildTreeDot1agGlobalConfig(depth2);
  }
#endif
#endif
#ifdef L7_DVLAN_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE)
  {
     buildTreeDVlanGlobalConfig(depth1);
  }
#endif

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

/* Interface config mode */
#if defined (L7_RLIM_PACKAGE) 
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Ipv6DhcpRelayIntf_1,pStrInfo_common_EnterIntfMode, cliInterfaceMode, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#else /* !L7_RLIM_PACKAGE */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Ipv6DhcpRelayIntf_1,pStrInfo_common_EnterIntfMode, cliInterfaceMode,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
#endif /* L7_RLIM_PACKAGE */

  depth3 = buildTreeInterfaceRangeHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_INTERFACE_CONFIG_MODE);

#ifdef L7_POE_PACKAGE
   /* Global config poe mode */
   for ( unit = 1;unit < L7_ALL_UNITS; unit++ )
   {
      if (usmDbFeaturePresentCheck(unit, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) == L7_TRUE)
      {
        buildTreeGlobalPoe(depth1);
        break;
      }
   }
#endif
   unit = cliGetUnitId();
   /* End of Poe */

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  buildTreeIntfTunnelLoopback(depth2);
#endif
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_mailserver,pStrInfo_common_EnterMailServerMode, cliMailServerMode,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<ipaddress|ipv6address|host-name> ",pStrInfo_common_EnterMailServerCfg, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,  4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_MAIL_SERVER_MODE);

#ifdef L7_SFLOW_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
  /*sflow*/
    depth2 = ewsCliAddNode(depth1, pStrinfo_common_SflowOption, pStrInfo_common_CfgSflow, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    buildTreeGlobalsFlowConfig(depth2);
  }
#endif
  /* DHCP Server :  To include 'ip' node for DHCPS, DHCP Snooping, DAI etc... */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_IpOption,pStrInfo_common_CfgIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_ROUTING_PACKAGE
  cliTreeGlobalIphelper(depth2);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeIpAccessGroupAll(depth2);
  cliTreeIpAccessListConfig();
  cliTreeIpAccessList(depth2);
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_DHCPS_PACKAGE
  buildTreeDhcpsConfig(depth3);
#endif

#ifdef L7_DHCP_FILTER_PACKAGE
  /* command      : ip dhcp filtering    */
  buildTreeGlobalDhcpFiltering(depth3);
  /* command      : ip dhcp filtering    - End  */
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE

  buildTreeGlobalDhcpSnooping(depth3);

#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, "verify","Add IPSG Global configurations", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    buildTreeGlobalIpsg(depth3);
  }
#endif
#endif

  /* IP address conflict detection command */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_AddrConflictDetect,
                         pStrInfo_base_AddrConflictDetectRun, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AddrConflictDetectRun,
                         pStrInfo_base_AddrConflictDetectRun, commandIpAddrConflictDetect,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DAI_PACKAGE
  /* Dynamic ARP Inspection: "ip arp inspection..." */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Arp_2, pStrInfo_base_Dai, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ArpInspec, pStrInfo_base_Dai, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeGlobalDai(depth4);
#endif


#ifdef L7_MCAST_PACKAGE
  cliTreeGlobalConfigModeDvmrp(depth2);
#endif

/* Start : CLI Commands for DNS client component */
  buildTreeIpDnsClient(depth2);

  buildTreeIpHostnameToAddressMapping(depth2);
  buildTreeIpNameServer(depth2);
/* CLI Commands for DNS Client component */

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* HTTP and HTTPS authentication commands */
    buildTreeGlobalHTTPConfig(depth2);
  }

#ifdef L7_MCAST_PACKAGE
  cliTreeGlobalConfigModeIgmp(depth2);
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeGlobalConfigStaticRoute(depth2);
  cliTreeGlobalConfigPimsm(depth2);
  cliTreeGlobalConfigModePimdm(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeGlobalIpIcmp(depth2);
  buildTreeGlobalIpRouter(depth2);
  buildTreeGlobalIpDefaultGateway(depth2);

#ifdef L7_VRRP_PACKAGE
  buildTreeRoutingGlobalVrrp(depth2);
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
  {
    depth2v6 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5,pStrInfo_common_CfgIpv6,NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
#endif  /* #ifdef L7_QOS_FLEX_PACKAGE_ACL */

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  /* don't build node twice if done above */
  if (depth2v6 == L7_NULLPTR)
  {
    depth2v6 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5,pStrInfo_common_CfgIpv6,NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
#endif
#endif

#ifdef L7_IPV6_MGMT_PACKAGE
  /* don't build node twice if done above */
  if (depth2v6 == L7_NULLPTR)
  {
    depth2v6 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5,pStrInfo_common_CfgIpv6,NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
  {
    buildTreeIpv6AccessListConfig();
    buildTreeAccessListIpv6(depth2v6);
  }
#endif
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  buildTreeGlobalIpv6(depth2v6);
#ifdef L7_MCAST_PACKAGE
  cliTreeGlobalConfigModeIpv6Pimdm(depth2v6);
  cliTreeGlobalConfigModeIpv6Mld(depth2v6);
  cliTreeGlobalConfigModeIpv6Pimsm(depth2v6);
  cliTreeGlobalConfigIpv6StaticMRoute (depth2v6);
#endif
#endif
#endif

#if (!defined(L7_IPV6_PACKAGE) && defined(L7_IPV6_MGMT_PACKAGE))
  /*ipv6 host */
  depth3 = ewsCliAddNode(depth2v6, pStrInfo_common_Host_1, pStrInfo_common_IpHost, commandIpv6HostNameAddress, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_name, pStrInfo_common_DnsHostName, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6Addr, pStrInfo_common_HostIPv6Address, NULL,
                         2, L7_NODE_DATA_TYPE, L7_IPV6_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_ISDP_PACKAGE
  /*Config ISDP branch*/
  buildTreeConfigIsdp(depth1);
#endif /* L7_ISDP_PACKAGE */

  /* Line Config  mode*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Line_2, pStrInfo_base_LineConsoleCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Console, pStrInfo_base_LineConsoleCfg, cliLineConfigConsoleMode, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Ssh_2, pStrInfo_base_LineSshCfg, cliLineConfigSshMode, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Telnet, pStrInfo_base_LineTelnetCfg, cliLineConfigTelnetMode, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Adding lldp global config commands to the tree. */
  buildTreeGlobalConfigLldp(depth1);

  if (usmDbComponentPresentCheck(unit, L7_LOG_COMPONENT_ID) == L7_TRUE)
  {

    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Logging_3,pStrInfo_base_Logging_2, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* In-Memory/Buffered*/
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Bufed, pStrInfo_base_LoggingBufed, commandLoggingBuffered, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Comp_2, pStrInfo_base_LoggingBufedComp, commandLoggingBufferedComponent, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Comp, pStrInfo_base_LoggingCompVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Wrap, pStrInfo_base_LoggingWrap, commandLoggingBufferedWrap, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* logging CLI commands */
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_CliCmd, pStrInfo_base_Logging, commandLoggingCliCommand, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* logging email alert commands */
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_emailAlertTrapSeverityCmd, pStrInfo_base_emailAlertTrapSeverity, commandLoggingEmailAlertTrapSeverity, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, "<severity>", pStrInfo_base_emailAlertTrapSeverityHelp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);		
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_emailAlertCmd, pStrInfo_base_emailAlertLogging, commandLoggingEmailAlert, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, "<severity>", pStrInfo_base_emailAlertNonUrgentSeverity, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_emailAlertUrgentSeverityCmd, pStrInfo_base_emailAlertUrgent, commandLoggingEmailAlertUrgentSeverity, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, "none", pStrInfo_base_emailAlertUrgentNone, NULL,  L7_NO_OPTIONAL_PARAMS);		
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth5 = ewsCliAddNode ( depth4, "<severity>", pStrInfo_base_emailAlertUrgentSeverity, NULL,  L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_emailAlertFromAddressCmd, pStrInfo_base_emailAlertFromAddress, commandLoggingEmailAlertFromAddr, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, "<from-address> ", pStrInfo_base_emailAlertFromAddress, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_emailAlertLogtimeCmd, pStrInfo_base_emailAlertLogtime, commandLoggingEmailAlertLogDuration, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, "<30 -1440>", pStrInfo_base_emailAlertLogtimeHelp, NULL, 3, L7_NODE_UINT_RANGE, L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MIN, L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MAX);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_emailAlertMsgTypeCmd, pStrInfo_base_emailAlertMsgType, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);		
    depth5 = ewsCliAddNode ( depth4, "urgent", pStrInfo_base_emailAlertMsgTypeUrgent, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);				
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertToAddrCmd, pStrInfo_base_emailAlertToAddr, commandLoggingEmailAlertToAddr, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<to-addr> ", pStrInfo_base_emailAlertToAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertSubjectCmd, pStrInfo_base_emailAlertSubject, commandLoggingEmailAlertSubject, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<subject> ", pStrInfo_base_emailAlertSubject, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, "non-urgent", pStrInfo_base_emailAlertMsgTypeNonUrgent, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);				
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertToAddrCmd, pStrInfo_base_emailAlertToAddr, commandLoggingEmailAlertToAddr, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<to-addr> ", pStrInfo_base_emailAlertToAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertSubjectCmd, pStrInfo_base_emailAlertSubject, commandLoggingEmailAlertSubject, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<subject> ", pStrInfo_base_emailAlertSubject, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, "both", pStrInfo_base_emailAlertMsgTypeBoth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);				
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertToAddrCmd, pStrInfo_base_emailAlertToAddr, commandLoggingEmailAlertToAddr, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<to-addr> ", pStrInfo_base_emailAlertToAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_base_emailAlertSubjectCmd, pStrInfo_base_emailAlertSubject, commandLoggingEmailAlertSubject, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode ( depth6, "<subject> ", pStrInfo_base_emailAlertSubject, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);
		
		

    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_emailAlertTestCmd, pStrInfo_base_emailAlertTest, commandLoggingEmailAlertTest, L7_NO_OPTIONAL_PARAMS);		
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_emailAlertMsgTypeCmd, pStrInfo_base_emailAlertMsgType, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth6 = ewsCliAddNode ( depth5, "urgent", pStrInfo_base_emailAlertMsgTypeUrgent, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth7 = ewsCliAddNode ( depth6, pStrInfo_base_emailAlertMsgBodyCmd, pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode ( depth7, "<msg-body>", pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth9 = ewsCliAddNode ( depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, "non-urgent", pStrInfo_base_emailAlertMsgTypeNonUrgent, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth7 = ewsCliAddNode ( depth6, pStrInfo_base_emailAlertMsgBodyCmd, pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode ( depth7, "<msg-body>", pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth9 = ewsCliAddNode ( depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, "both", pStrInfo_base_emailAlertMsgTypeBoth, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth7 = ewsCliAddNode ( depth6, pStrInfo_base_emailAlertMsgBodyCmd, pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode ( depth7, "<msg-body>", pStrInfo_base_emailAlertMsgBody, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth9 = ewsCliAddNode ( depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
		
		
		


    /* Console */
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Console, pStrInfo_base_LoggingConsole, commandLoggingConsole, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Severitylevel07, pStrWarn_base_LoggingSeverity, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Comp_2, pStrInfo_base_LoggingConsoleComp, commandLoggingConsoleComponent, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Comp, pStrInfo_base_LoggingCompVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    /* Host */
    depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Host_2, pStrInfo_base_LoggingHost, commandLoggingHost, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, "<hostaddress|hostname> ", pStrInfo_base_LoggingAddrVal, NULL, L7_NO_OPTIONAL_PARAMS);

    depthdns = ewsCliAddNode ( depth4, "dns", pStrInfo_base_LoggingAddrTypeVal, NULL, L7_NO_OPTIONAL_PARAMS);

    depth6 = ewsCliAddNode ( depthdns, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth6 = ewsCliAddNode ( depthdns, pStrInfo_common_Port, pStrInfo_base_PortId, NULL, 3, L7_NODE_UINT_RANGE, L7_TCP_UDP_MIN_PORT, L7_TCP_UDP_MAX_PORT);
    depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth7 = ewsCliAddNode ( depth6, pStrInfo_base_Severitylevel, pStrWarn_base_LoggingSeverity, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


    depth5 = ewsCliAddNode(depth4, "ipv4", pStrInfo_base_LoggingAddrTypeVal, NULL, 2,
                           L7_OPTIONS_NODE, depthdns);


#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    depth8 = ewsCliAddNode ( depth7, pStrInfo_base_Comp, pStrInfo_base_LoggingCompVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode ( depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Reconfigure, pStrInfo_base_LoggingHostReconfigure, commandLoggingHostReconfigure, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_HostIdx, pStrInfo_base_LoggingHostReconfigureIdx, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, "<hostaddress|hostname> ", pStrInfo_base_LoggingNewAddrVal, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
    depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Remove_1, pStrInfo_base_LoggingHostRemove, commandLoggingHostRemove, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_HostIdx, pStrInfo_base_LoggingHostRemoveIdx, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) == L7_TRUE)
    {
      /* Persistent - History */
      depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Persistent, pStrInfo_base_LoggingPersistent, commandLoggingPersistent, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

      depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Severitylevel, pStrWarn_base_LoggingSeverity, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    /* Syslog */
    depth3 = ewsCliAddNode ( depth2, pStrInfo_base_LogFacilitySyslog, pStrInfo_base_LoggingSyslog, commandLoggingSyslog, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Facility_1, pStrInfo_base_LoggingFacility, commandLoggingSyslogFacility, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Facility, pStrInfo_base_LoggingFacilityVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    /* Port */
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Port_4, pStrInfo_base_LoggingPort, commandLoggingSyslogPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Portid, pStrInfo_base_PortId, NULL, 3, L7_NODE_UINT_RANGE, L7_TCP_UDP_MIN_PORT, L7_TCP_UDP_MAX_PORT);
    depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE )
  {
    buildTreeMacAccessListConfig();
    buildTreeGlobalMacAccessList(depth1);
  }
#endif

#ifdef L7_TIMERANGES_PACKAGE
  /*time-range commands*/
  buildTreeTimeRangeConfig();
  buildTreeGlobalTimeRange(depth1);
#endif

#ifdef L7_STATIC_FILTERING_PACKAGE
  buildTreeGlobalSwDevMacFilter(depth1);
#endif

  buildTreeGlobalSwDevCreateMirroring(depth1);

  /* Check for Jumbo Frames Feature Support */
  if (usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE  &&
      usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID) == L7_FALSE)
  {
    buildTreeGlobalMaxFrameSize(depth1);
  }

  /* Password Mgmt */
  buildTreeGlobalConfigPasswordMgmt(depth1);

  /* PolicyMap mode*/
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeGlobalConfigPolicyMap(depth1);
#endif

  buildTreeGlobalSwDevPort(depth1); /* protocol */

  buildTreeGlobalSwDevPortChannel(depth1);

  buildTreeGlobalSwDevPortSecurity(depth1);

#ifdef L7_PBVLAN_PACKAGE
  buildTreeGlobalSwDevModeAllVlanGroupAll(depth1); /* protocol */
#endif

  buildTreeGlobalRadiusConfig(depth1);

  /* Cos Random Detect Commands */
#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeRandomDetect(depth1, L7_TRUE);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_ROUTING_PACKAGE
  buildTreeRouterConfigMode(depth1);

  /* Only SDM templates at this point are for routing. So only show 
   * configuration command in routing builds. */
  buildTreeSdm(depth1);
#endif /* end if routing package included */

#ifdef L7_DHCPS_PACKAGE
  buildTreeDhcpsServiceConfig(depth1);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeGlobalConfigServicePolicy(depth1);
#endif

  buildTreeGlobalSwDevGarpTimer(depth1);

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_SnmpSrvr_1, pStrInfo_base_SnmpSrvr, commandSnmpServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  buildTreeGlobSWMgmtSnmpserver(depth2);
  buildTreeGlobSWMgmtSnmpTrap(depth1);

  if (usmDbComponentPresentCheck(unit, L7_SNTP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeSntp(depth1);
  }
  buildTreeGlobalSWSpanSpanningTreeConf(depth1);
  buildTreeGlobalSwDevSpeed(depth1);
#ifdef L7_STACKING_PACKAGE

  /* Stack mode*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Stack_1, pStrInfo_base_StackCfg, cliStackMode, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_STACK_MODE);
#endif

  buildTreeGlobalSwDevStormControl(depth1);

#ifdef L7_STACKING_PACKAGE
  buildTreeGlobalStack(depth1);
#endif
  buildTreeGlobalConfigTacacs(depth1);

  /* Cos Tail Drop Commands */
#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) == L7_TRUE)
  {
    buildTreeTailDrop(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_TR069_PACKAGE 
  if (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeGlobalTr069(depth1);
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE)
  {
    buildTreeGlobalTrafficShape(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

  buildTreeGlobalSwUamUsers(depth1);
  buildTreeGlobalSwDevVlan(depth1);
  buildTreeGlobalConfigVoiceVlan(depth1);
  buildTreeGlobalSwDevProtectedPort(depth1);

  buildTreeGlobalCpuMonitor(depth1);
#ifdef L7_WIRELESS_PACKAGE
  buildTreeGlobalWireless(depth1);
#endif /* L7_WIRELESS_PACKAGE */
}


/*********************************************************************
 *
 * @purpose  To build the Interface config mode
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
void buildTreeInterfaceConfig()
{
  EwsCliCommandP depth1,depth2,depth3, depth4;
  EwsCliCommandP depth2v6 = L7_NULLPTR;
  L7_BOOL        needDot1p = L7_FALSE;
  L7_uint32       unit;

  unit = cliGetUnitId();
  /* need to reference variable to prevent build warning when neither ipv6 nor QOS packages included */
  depth2v6 = depth2v6;

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    needDot1p = L7_TRUE;
  }

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliInterfaceMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_INTERFACE_CONFIG_MODE, depth1);

  buildTreeInterfaceSwDevInterfacePortChannel(depth1);

#ifdef L7_PFC_PACKAGE
  if (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                            L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) 
                            == L7_TRUE)
  {
    buildTreeDataCenterBridgingConfigCommon(depth1);
  }
#endif

#ifdef L7_ROUTING_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Bandwidth_2, pStrInfo_base_CfgBandwidth, commandRtrIntfBandwidth, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Range1to10000000, pStrInfo_base_CfgBandwidthRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) == L7_TRUE) ||
      (needDot1p == L7_TRUE))
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Classofservice, pStrInfo_base_CfgClassOfService, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* Dot1p commands*/
    if (needDot1p == L7_TRUE)
    {
      buildTreeCosDot1pConfig(depth2);
    }

#ifdef L7_QOS_FLEX_PACKAGE_COS
    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE))
    {
       buildTreeCosIPDscpMapping(depth2);
    }

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE))
    {
       buildTreeCosIPPrecedenceMapping(depth2);
    }

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) == L7_TRUE))
    {
       buildTreeInterfaceCosTrust(depth2);
    }
#endif /* L7_QOS_FLEX_PACKAGE_COS */
  }
#ifdef L7_QOS_FLEX_PACKAGE_COS
  /* check for present component and call buildTreeInterfaceCosQueue */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceCosQueue(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  /* Auto VoIP Commands */
  if (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeInterfaceAutoVoIP(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_VOIP */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Delport, pStrInfo_base_IntfDelPort, commandInterfaceDeletePort, L7_NO_OPTIONAL_PARAMS);

  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* description <string>; no description */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Desc_2, pStrInfo_base_IntfDescr,
      commandIntfDescription, 2, L7_NO_COMMAND_SUPPORTED,
      L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Desc, pStrInfo_base_IntfDescrInfo, NULL,
      L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DHCP_SNOOPING_PACKAGE
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpL2Relay, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeIntfDhcpL2Relay(depth2);
  #endif
#endif

#ifdef L7_METRO_FLEX_PACKAGE
   if (usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) 
  {
#ifdef L7_DOT1AD_PACKAGE
    buildTreeInterfaceConfigdot1ad(depth1);
#endif
  }  
#endif
  buildTreeInterfaceDot1xConfig(depth1);

#if defined(L7_METRO_FLEX_PACKAGE) || defined(L7_DOT1AG_PACKAGE)
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DhcpEtherNet, pStrInfo_common_ethernet_help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
   if (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeInterfaceConfigdot3ah(depth2);
  }
#endif /* L7_DOT3AH_PACKAGE */
#endif /* L7_METRO_FLEX_PACKAGE */

#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agInterfaceConfig(depth2);
  }
#endif /* L7_DOT3AH_PACKAGE */


#ifdef L7_DVLAN_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE)
  {
    buildTreeInterfaceSwDVlan(depth1);
  }
#endif
#ifdef L7_POE_PACKAGE
  for ( unit = 1; unit < L7_ALL_UNITS; unit++)
  {
     if (usmDbFeaturePresentCheck(unit, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) == L7_TRUE)
     {
        buildTreeInterfacePoe(depth1);
        break;
     }
  }
#endif
  unit = cliGetUnitId();

#ifdef L7_ROUTING_PACKAGE
  buildTreeRoutingInterfaceEncaps(depth1);
#endif

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Add the ip command root only if ACL or Routing are supported */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_IpOption, pStrInfo_common_CfgIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_ROUTING_PACKAGE
  buildTreeInterfaceIpHelperAddress(depth2);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  cliTreeIpAccessGroup(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeIfIpAddr(depth2);
#endif


  /* ip dhcp */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpIntf, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_DHCP_FILTER_PACKAGE
buildTreeIfIpDhcpFilteringTrust(depth3);
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
  buildTreeIfIpDhcpSnoopingTrust(depth3);
#endif

#ifdef L7_DAI_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Arp_2, pStrInfo_base_Dai, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ArpInspec, pStrInfo_base_Dai,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeIfDai(depth4);
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeInterfaceConfigDvmrpIgmp(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_RTR_DISC_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeRoutingInterfaceIrdp(depth2);
  }

  if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
  {
  buildTreeInterfaceIpLocalProxyArp(depth2);   /* local-proxy-arp */
  }
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeInterfaceConfigMcast(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeInterfaceConfigIpMtu(depth2);
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeInterfaceConfigMulticast(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
  buildTreeRoutingInterfaceNetDbOspf(depth2);
#endif
#endif

#ifdef L7_ISDP_PACKAGE
   /* Adding ISDP tree in interface mode */
  buildTreeInterfaceConfigIsdp(depth1);
#endif /* L7_ISDP_PACKAGE */

#ifdef L7_LLPF_PACKAGE
   /* Adding LLPF tree in interface mode */
  buildTreeInterfaceConfigLlpf(depth1);
#endif /* L7_LLPF_PACKAGE */

  /* Adding LACP tree in interface mode */
  buildTreeInterfaceConfigLacp(depth1);

  /* Adding LLDP tree in interface mode */
  buildTreeInterfaceConfigLldp(depth1);

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE )
  {
    buildTreeInterfaceMacAccessList(depth1);
  }
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth2v6 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5,pStrInfo_common_CfgIpv6, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeInterfaceIpv6(depth2v6, L7_INTERFACE_CONFIG_MODE);
  buildTreeInterfaceIpv6Mtu(depth2v6, L7_INTERFACE_CONFIG_MODE);
  buildTreeRoutingInterfaceNetDbOspfv3(depth2v6, L7_INTERFACE_CONFIG_MODE);
#ifdef L7_MCAST_PACKAGE
  cliTreeInterfaceConfigIpv6Pimdm(depth2v6);
  cliTreeInterfaceConfigIpv6Mld(depth2v6);
  cliTreeInterfaceConfigIpv6Pimsm(depth2v6);
#endif
  buildTreeRoutingInterfaceIpv6Unreachables(depth2v6);
#endif
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE )
  {
    /* if, due to package absence, the "ipv6" node has not been created above, create it here */
    if (depth2v6 == L7_NULLPTR)
    {
      depth2v6 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5,pStrInfo_common_CfgIpv6, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
    buildTreeInterfaceIpv6TrafficFilter(depth2v6);
  }
#endif

#ifdef L7_STATIC_FILTERING_PACKAGE
  buildTreeInterfaceSwDevMacFilter(depth1);
#endif
#ifdef L7_DVLAN_PACKAGE
  buildTreeInterfaceSwDot1qTunnel(depth1);
#endif
  /* Check for Jumbo Frames Feature Support */
  if (usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE  &&
      usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceMaxFrameSize(depth1);
  }

  buildTreeInterfaceSwDevPort(depth1);

  buildTreeInterfaceLAG(depth1);

  buildTreeInterfaceSwDevPortSecurity(depth1);/* Port MAC Locking*/

#ifdef L7_PBVLAN_PACKAGE
  buildTreeInterfaceSwDevPortVlanGroup(depth1);
#endif

#ifdef L7_MCAST_PACKAGE
  cliTreeInterfaceConfigPimdmPimsm(depth2);
#endif

#ifdef L7_ROUTING_PACKAGE
  if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
  {
  buildTreeInterfaceIpProxyArp(depth2);
  }
  buildTreeInterfaceIpRedirects(depth2);
#ifdef L7_RIP_PACKAGE
  buildTreeRoutingInterfaceRip(depth2);
#endif
  /* Not supporting unnumbered interfaces in FASTPATH. To support, uncomment
   * this node in the CLI tree. */
#if L7_UNNUMBERED_INTERFACES
  buildTreeInterfaceIpUnnumbered(depth2);
#endif
  buildTreeInterfaceIpUnreachables(depth2);
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) == L7_TRUE)
  {
    buildTreeIpsgVerifySource(depth2);
  }
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_VRRP_PACKAGE
  buildTreeRoutingInterfaceVrrp(depth2);
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE))
  {
    buildTreeRandomDetect(depth1, L7_FALSE);
  }
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreeRoutingInterfaceConfig(depth1);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  cliTreeInterfaceConfigServicePolicy(depth1);
#endif

  buildTreeInterfaceSwDevSetGarp(depth1);

  buildTreeInterfaceSwDevPortSecuritySnmp(depth1);

  buildTreeInterfaceSWSpanSpanningTreeConf(depth1);

  buildTreeInterfaceSwDevSpeed(depth1);
#ifdef L7_SFLOW_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    buildTreeInterfacesFlow(depth1);
  }
#endif

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceStormControl(depth1);
  }

#if 0 /* RJ: This is not supported on XGS platform */
#ifdef L7_STACKING_PACKAGE
  buildTreeInterfaceStack(depth1);
#endif
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
  buildTreeInterfaceDot1adSubscribe(depth1);
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    buildTreeTailDrop(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    buildTreeInterfaceTrafficShape(depth1);
  }
#endif /* L7_QOS_FLEX_PACKAGE_COS */

  buildTreeInterfaceSwDevVlan(depth1);
  buildTreeInterfaceConfigVoiceVlan(depth1);
}

/*********************************************************************
 *
 * @purpose  To build the Line config mode tree console.
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
void buildTreeLineConsoleConfig()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliLineConfigConsoleMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_LINE_CONFIG_CONSOLE_MODE, depth1);

  buildTreeLineConfigSWMgmtEnable(depth1);  /* Setting Authentication for Enable Mode */

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeLineConfigSWMgmtSetEIAParm(depth1);

  buildTreeLineConfigSWMgmtLogin(depth1);   /* Setting Login Authentication */
  buildTreeLinePasswordConfig(depth1);      /* Configure Password for this Line Mode */
#ifdef L7_OUTBOUND_TELNET_PACKAGE
  buildTreeLineConfigSWMgmtOutboundTelnetConf(depth1);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
  buildTreeLineConfigSWMgmtTelnetConf(depth1);
}

/*********************************************************************
 *
 * @purpose  To build the Line config mode tree Ssh.
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
void buildTreeLineSshConfig()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliLineConfigSshMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_LINE_CONFIG_SSH_MODE, depth1);

  buildTreeLineConfigSWMgmtEnable(depth1);  /* Setting Authentication for Enable Mode */

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeLineConfigSWMgmtLogin(depth1);   /* Setting Login Authentication */
  buildTreeLinePasswordConfig(depth1);      /* Configure Password for this Line Mode */
}

/*********************************************************************
 *
 * @purpose  To build the Line config mode tree console.
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
void buildTreeLineTelnetConfig()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliLineConfigTelnetMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_LINE_CONFIG_TELNET_MODE, depth1);

  buildTreeLineConfigSWMgmtEnable(depth1);  /* Setting Authentication for Enable Mode */

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreeLineConfigSWMgmtLogin(depth1);   /* Setting Login Authentication */
  buildTreeLinePasswordConfig(depth1);      /* Configure Password for this Line Mode */
}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for User Exec mode Clear commands
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the tree nodes for User Exec mode Clear commands
 *
 * @end
 *
 *********************************************************************/
void buildTreeUserPrivilegedCounterClear(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Counters, pStrInfo_base_ClrCounters_1, commandClearCounters, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeInterfaceHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Vlan mode Clear commands
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Vlan mode Clear commands
 *
 * @end
 *
 *********************************************************************/
void cliTreeVlanClear(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  /* depth1 = "Clear Vlan command" */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ClrVlan, commandClearVlan, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Ping & Reload commands
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode Ping & Reload commands
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedConfigPingReload(EwsCliCommandP depth1)
{
#ifdef L7_STACKING_PACKAGE
#else
  EwsCliCommandP depth2, depth3;
#endif
  #ifdef L7_STACKING_PACKAGE
  buildTreePrivilegeExecStack(depth1);
#else
  depth2 = ewsCliAddNode(depth1, pStrErr_common_ReloadCmd, pStrInfo_base_Reload, commandReload, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Logout commands
 *
 * @param void
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode Logout commands
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedConfigLogout(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

  /* depth1 = "logout command" */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Logout_1, pStrInfo_base_LogOut, commandLogout, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Clear commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedConfigClear(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5,depth6  ;
  L7_uint32       unit;

  unit = cliGetUnitId();

  /* depth1 = "clear commands" */
  depth2 = ewsCliAddNode(depth1, pStrErr_common_CfgApProfileClrCmd, pStrInfo_base_Clr_1, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_ROUTING_PACKAGE

  /*  Kavleen : Added for ARP Enhancements  */
  buildTreePrivClearArp(depth2);

#endif

  buildTreePrivClearArpSwitch(depth2);

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  buildTreePrivCaptivePortalClear(depth2);
#endif

#ifdef L7_NSF_PACKAGE
  buildTreeClearCheckpointStats(depth2);
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Cfg_7, pStrInfo_base_ClrCfg, commandClearConfig, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeUserPrivilegedCounterClear(depth2);

#ifdef L7_DHCP_SNOOPING_PACKAGE
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* clear dhcp l2relay statistics */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, 
                         pStrInfo_switching_DhcpClearStatsHelp, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeClearDhcpL2Relay(depth3);
#endif
#endif

  buildTreePrivDot1xClear(depth2);

  buildTreeClearHost(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmpsnooping, CLICLEARIGMPSNOOPING_HELP(L7_AF_INET), commandClearIgmpsnooping, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_base_IpClr, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpsIpClr, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_DHCPS_PACKAGE
  buildTreeClearDHCPs(depth4);
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
  buildTreeClearDhcpSnoop(depth4);
#endif

  /* clear ip address-conflict-detect */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AddrConflictDetect, pStrInfo_common_AddrConflictDetectStatus,
                         commandClearIpAddrConflict, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr , pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

#ifdef L7_DAI_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Arp_2, pStrInfo_base_DaiClear, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ArpInspec, pStrInfo_base_DaiClear, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeClearDai(depth5);
#endif


#ifdef L7_OSPF_PACKAGE
  buildTreeClearIpOspf(depth3);
#endif

#ifdef L7_ROUTING_PACKAGE
  buildTreePrivClearIpHelperStatistics(depth3);
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_BGP_PACKAGE
  buildTreeClearIpBGP(depth3);
#endif /* L7_BGP_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_Ipv6_1, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeGlobalClearIpv6DhcpStats(depth3);
  buildTreeGlobalClearIpv6Neighbors(depth3);
  buildTreeGlobalClearIpv6Ospf(depth3); 
  buildTreeGlobalClearIpv6Stats(depth3);
#ifdef L7_MCAST_PACKAGE
  buildTreeGlobalClearIpv6MldCounters(depth3);
#endif
#endif
#endif

#ifdef L7_ISDP_PACKAGE
  buildTreePrivClearIsdp(depth2);
#endif /* L7_ISDP_PACKAGE */
  buildTreeGlobalLldpClear(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mldsnooping, CLICLEARIGMPSNOOPING_HELP(L7_AF_INET6), commandClearIgmpsnooping, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE) 
  buildTreeClearNetwork(depth2);
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Pass_2, pStrInfo_base_ClrPass, commandClearPass, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_LAG_PRECREATE_FEATURE_ID) == L7_FALSE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lag_4, pStrInfo_base_ClrLag, commandClearPortChannel, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  buildTreePrivilegedRadiusClear(depth2);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE) 
  buildTreeClearServport(depth2);
#endif

#ifdef L7_TR069_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeClearTr069(depth2);
  }
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Traplog, pStrInfo_base_ClrTrapLog, commandClearTraplog, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  cliTreeVlanClear(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Logging_3,pStrInfo_base_Logging_2, commandClearEmailAlertStats, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_emailAlertCmd, pStrInfo_base_emailAlertLogging, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_emailalertStatisticsCmd , pStrInfo_base_emailalertStatisticsclear, NULL, L7_NO_OPTIONAL_PARAMS);	
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);	
	

#if defined(L7_METRO_FLEX_PACKAGE) || defined(L7_DOT1AG_PACKAGE)
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_DhcpEtherNet, pStrInfo_common_ethernet_help, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE)
  {
    buildTreePrivDot3ahClear(depth3);
  }
#endif
#endif

#ifdef L7_DOT1AG_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
  {
    buildTreeDot1agPrivilegeClear(depth3);
  }
#endif

#ifdef L7_WIRELESS_PACKAGE
  buildTreePrivilegedWirelessClear(depth2);
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_PFC_PACKAGE
  if (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                            L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) 
                            == L7_TRUE)
  {
    buildTreePrivPfcClearStatistics(depth2);
  }
#endif

  buildTreePrivilegedIntAuthServUsersClear(depth2);
}
void cliTreeBuildCopyUploadHelpStr(L7_uint32 unit, L7_char8* buf, L7_uint32 bufSize)
{
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_base_SerialXmodem_2 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#ifdef _L7_OS_LINUX_
  osapiStrncat(buf, pStrInfo_base_SerialYZmodem_1 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#endif

  osapiStrncat(buf, pStrInfo_base_Pipe, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_base_TftpUrl, (bufSize - strlen(buf) - 1));
  if (usmDbFeaturePresentCheck(unit,L7_FLEX_SSHD_COMPONENT_ID,
                               L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE) 
  {
    osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_base_Pipe, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_base_SecureUrl, (bufSize - strlen(buf) - 1));
  }
  osapiStrncat(buf, pStrInfo_common_CurlyBraceClose, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Period, (bufSize - strlen(buf) - 1));
}

void cliTreeBuildCopyDownloadHelpStr(L7_uint32 unit, L7_char8* buf, L7_uint32 bufSize)
{
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_base_SerialXmodem_3 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#ifdef _L7_OS_LINUX_
  osapiStrncat(buf, pStrInfo_base_SerialYZmodem_3 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#endif

  if (usmDbFeaturePresentCheck(unit,L7_FLEX_SSHD_COMPONENT_ID,
                               L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE) 
  {
    osapiStrncat(buf, pStrInfo_base_Sftp_Scp_1, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
  }
  osapiStrncat(buf, pStrInfo_base_Tftp_1, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Period, (bufSize - strlen(buf) - 1));
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Copy commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode Copy commands
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedConfigCopy(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6,depth7;
#ifdef L7_STACKING_PACKAGE
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf1[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 min,max;
#endif
  L7_uint32       unit;
  L7_char8 helpStr[4*L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image1FileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2FileName[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();

#if L7_FEAT_DIM_USE_FILENAME
  usmDbImageFileNameGet(0, image1FileName);
  usmDbImageFileNameGet(1, image2FileName);
#else
  usmDbActiveNameGet(image1FileName);
  usmDbBackupNameGet(image2FileName);
#endif

  /* depth1 = "copy commands" */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Copy_4, pStrInfo_base_Copy_2, commandCopy, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldBannerFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramClibanner, pStrInfo_base_CopyBannerFile, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldErrorLogFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramErrorlog, pStrErr_base_CopyErrorLog, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_TOOL_VALGRIND
  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldValgLogFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramValgrindlog, pStrErr_base_CopyValgLog, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldLogFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramLog, pStrInfo_base_CopyLog, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldConfigScriptFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramScript, pStrInfo_base_CopyCfgScriptUpload, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Srcfilename, pStrInfo_base_CopyCfgScriptSrcFileName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_UpldConfigFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramFastpathCfg, pStrInfo_base_Copy_1, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldConfigFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_NvramStartupCfg, pStrInfo_base_CopyStartupCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramBackupCfg, pStrInfo_base_CopyStartup2BackupCfg, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramBackupCfg, pStrInfo_base_CopyStartupCfg, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_NvramStartupCfg, pStrInfo_base_CopyBackup2StartupCfg, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldTrapLogFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NvramTraplog, pStrInfo_base_CopyTrapLog, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  depth3 = ewsCliAddNode(depth2, image1FileName, pStrInfo_base_CopyTftpCodeUpStk, NULL, L7_NO_OPTIONAL_PARAMS);
#else
  depth3 = ewsCliAddNode(depth2, image1FileName, pStrInfo_base_CopyTftpCodeUp, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /*L7_STACKING_PACKAGE*/

  depth4 = ewsCliAddNode(depth3, image2FileName, pStrInfo_base_CopyCode2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  cliGetMaxMinUnitnum(&max, &min);
  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf,sizeof(buf),pStrInfo_base_UnitImage1_1,min,max);
  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf1,sizeof(buf1),pStrInfo_base_UnitImage2_1,min,max);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_DownLoadCode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, buf1, pStrInfo_base_DownLoadCode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_UnitImage1, pStrInfo_base_DownLoadCodeAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_UnitImage2, pStrInfo_base_DownLoadCodeAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /*L7_STACKING_PACKAGE*/

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldFiles, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  depth3 = ewsCliAddNode(depth2, image2FileName, pStrInfo_base_CopyTftpCodeUpStk, NULL, L7_NO_OPTIONAL_PARAMS);
#else
  depth3 = ewsCliAddNode(depth2, image2FileName, pStrInfo_base_CopyTftpCodeUp, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /*L7_STACKING_PACKAGE*/
  depth4 = ewsCliAddNode(depth3, image1FileName, pStrInfo_base_CopyCode1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE

  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_DownLoadCode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, buf1, pStrInfo_base_DownLoadCode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_UnitImage1, pStrInfo_base_DownLoadCodeAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_UnitImage2, pStrInfo_base_DownLoadCodeAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_UpldFiles, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_SysRunningCfg, pStrInfo_base_CopySysCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_NvramStartupCfg, pStrInfo_base_CopyNvramCfg, commandCopySystemRunningConfigNvramStartupConfig, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldFiles, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyUploadHelpStr(unit, helpStr, sizeof(helpStr));

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Url, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldCliBannerFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramClibanner, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramScript, pStrInfo_base_CopyCfgScriptDownLoad, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Destfilename, pStrInfo_base_CopyCfgScriptDestFileName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_NoVal, pStrInfo_base_NoScriptValidation, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldCodeFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, image1FileName, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldCodeFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, image2FileName, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#if defined(_L7_OS_LINUX_) && defined(L7_SDK_EXCLUDES_KERNEL)
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Kernel, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldDSAFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSshkeyDsa, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldRSA1File, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSshkeyRsa1, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldRSA2File, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSshkeyRsa2, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_DnldStrongPEMFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSslpemDhstrong, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_DnldWeakPEMFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSslpemDhweak, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_DnldRootPEMFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSslpemRoot, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_DnldServerPEMFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramSslpemSrvr, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

#endif /* L7_MGMT_SECURITY_PACKAGE */

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldConfigFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_NvramStartupCfg, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_MGMT_SECURITY_PACKAGE
#ifdef L7_METRO_FLEX_PACKAGE

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* Download TR-069 SSL certificates */
    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_Dnldtr069acsRootPEMFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Nvramtr069acsSslpemRoot, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_Dnldtr069ClientPrivateKeyFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Nvramtr069ClientSslPrivKey, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_Dnldtr069ClientCertificateFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Nvramtr069ClientSslCert, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

#endif /* L7_METRO_FLEX_PACKAGE */
#endif /* L7_MGMT_SECURITY_PACKAGE */

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    memset(helpStr, 0, sizeof(helpStr));
    osapiStrncat(helpStr, pStrInfo_base_DnldBinConfigFile, (sizeof(helpStr) - strlen(helpStr) - 1));
    cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_NvramFastpathCfg, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  memset(helpStr, 0, sizeof(helpStr));
  osapiStrncat(helpStr, pStrInfo_base_DnldCodeFile, (sizeof(helpStr) - strlen(helpStr) - 1));
  cliTreeBuildCopyDownloadHelpStr(unit, helpStr, sizeof(helpStr));
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_SysImage, helpStr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IASUser, pStrInfo_base_IASUserDnldHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/******************************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Config Script commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode Config Script commands
 *
 * @end
 *
 ********************************************************************************/
void cliTreePrivilegedConfigConfigScript(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* depth1 = "script commands" */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Script, pStrInfo_base_CfgScript, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_WsInputApply, pStrInfo_base_CfgScriptApply, commandConfigScriptApply, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Scriptname, pStrInfo_base_CfgScriptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Del_1, pStrInfo_base_CfgScriptDel, commandConfigScriptDelete, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_CfgScriptDelAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Scriptname, pStrInfo_base_CfgScriptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_List, pStrInfo_base_CfgScriptList, commandConfigScriptList, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Show_1, pStrInfo_base_CfgScriptShow, commandConfigScriptShow, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Scriptname, pStrInfo_base_CfgScriptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Validate, pStrInfo_base_CfgScriptValidate_1, commandConfigScriptValidate, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Scriptname, pStrInfo_base_CfgScriptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
 *
 * @purpose  Build the Cable Status test command tree
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
void cliTreePrivilegeCableStatus(EwsCliCommandP depth1)
{
  /* depth1 = Privilege Exec Mode */
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Cablestatus, pStrInfo_base_CfgTestCableStatus, commandCablestatus, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeInterfaceHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
 *
 * @purpose  To build the hidden Command Tree
 *
 * @param void
 *
 * @returntype void
 *
 * @note     Usage of all hidden commands is reserved for the usage of
 *           designated FASTPATH support engineers and their assignees
 *
 * @end
 *
 *********************************************************************/
void buildTreeHiddenCommandMode (void)
{
  EwsCliCommandP depth1,depth2,depth3, depth4;
  L7_uint32       unit;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, NULL, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_HIDDEN_COMMAND_MODE, depth1);

  /* The timezone clock commands are hidden mode .*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_clock,pStrInfo_base_cliclock_help, commandDummy, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeGlobalClockTimeZone(depth2);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_summ_time_1, pStrInfo_base_clisummertime_help, commandNoSummerTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeGlobalClockSummerTimeDate(depth3);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_recurring, pStrInfo_base_clisummertime_recur_help, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeGlobalClockSummerTimeRecurring(depth4);
  buildTreeGlobalClockSummerTimeRecurringSpec(depth4);

  /* Build hidden node for devshell */
  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_Devshell_1, pStrInfo_base_CfgDevshell, commandDevShell, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Argument, pStrInfo_base_CfgDevshellOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Build hidden node for to enable/disable display of support tree*/
  depth2 = ewsCliAddNode ( depth1, pStrInfo_base_TechSupport_1, NULL, commandTechSupport,L7_NO_OPTIONAL_PARAMS);
  depth3 =  ewsCliAddNode ( depth2, pStrInfo_common_Key, pStrInfo_base_Key_1, NULL,L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  hiddenModeRoot = depth1;

  /*Supporting wireless modes in nonwireless builds to support text based configuration */
#ifndef L7_WIRELESS_PACKAGE
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Ws_3_Nonwireless, pStrInfo_wireless_WsCfg_2_Nonwireless,
                cliWirelessModeSupport, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_WIRELESS_CONFIG_MODE);
#endif /* L7_WIRELESS_PACKAGE */


  unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_DRIVER_SHELL_FEATURE_ID) == L7_TRUE)
  {
  depth2 = ewsCliAddNode ( depth1, "drivshell ", pStrInfo_base_CfgDevshell, commandDrvShell , L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, "<argument> ", pStrInfo_base_CfgDevshellOption, NULL , L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, "<cr> ", pStrInfo_common_NewLine, NULL , L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
 *
 * @purpose  Get hidden node
 *
 * @param void
 *
 * @returntype void
 *
 * @notes returns hidden node
 *
 * @end
 *
 *********************************************************************/
EwsCliCommandP cliGetHiddenNode(void)
{
  return hiddenModeRoot;
}

/*********************************************************************
*
* @purpose  Get Maintenance node
*
* @param void
*
* @returntype void
*
* @notes returns hidden node
*
* @end
*
*********************************************************************/
EwsCliCommandP cliGetMaintenanceNode(void)
{
  return maintenanceModeRoot;
}
/*********************************************************************
 *
 * @purpose  Build the tree to save the settings to NVRAM
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
void  buildTreePrivWriteMemory(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */

  EwsCliCommandP depth2, depth3, depth4;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Write_1, pStrInfo_base_WriteMemory, commandWriteMemory, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Memory, pStrInfo_base_WriteMemoryNvram, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
 *
 * @purpose  Build the Switch Management set prompt commands tree
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
void buildTreePrivHostname(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */

  EwsCliCommandP depth2, depth3, depth4;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Hostname_1, pStrInfo_base_HostName, commandHostname, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Hostname, pStrInfo_base_CfgPromptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Show commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeShowKeyingStatus(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

  /* command      :     show key-features    */
  depth2 = ewsCliAddNode (depth1, pStrInfo_base_KeyFeats, pStrInfo_base_KeyFeatShow, commandShowKeyFeatures, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show key-features    - End  */
}

void buildTreeSntp(EwsCliCommandP depth1)
{
  EwsCliCommandP  depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Sntp_1, pStrInfo_base_Sntp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Bcast_1, pStrInfo_base_SntpClientBcast, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPollIntvlVal, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Client_1, pStrInfo_base_SntpClient, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_PollIntvl, pStrInfo_base_SntpClientBcastPollIntvl, commandSntpClientBroadcastPollInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, NULL, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Client_1, pStrInfo_base_SntpClient, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_base_SntpClientMode, commandSntpClientMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Bcast_1, pStrInfo_base_SntpBcastClientModeVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ucast_1, pStrInfo_base_SntpUcastClientModeVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPortVal, SNTP_MIN_CLIENT_PORT, SNTP_MAX_CLIENT_PORT);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Port_4, pStrInfo_base_SntpClientPort, commandSntpClientPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Portid, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_CLIENT_PORT, SNTP_MAX_CLIENT_PORT);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPollIntvlVal, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);

  depth6 = ewsCliAddNode(depth5, NULL, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Srvr_1, pStrInfo_base_SntpSrvr, commandSntpServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE) 
  depth4 = ewsCliAddNode(depth3, "<ipaddress|ipv6address|host-name> ", pStrInfo_base_SntpSrvrAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NO_OPTIONAL_PARAMS);
#else
  depth4 = ewsCliAddNode(depth3, "<ipaddress|host-name> ", pStrInfo_base_SntpSrvrAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NO_OPTIONAL_PARAMS);
#endif

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpSrvrPri, SNTP_MIN_SERVER_PRIORITY, SNTP_MAX_SERVER_PRIORITY);
  depth5 = ewsCliAddNode(depth4, NULL, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_SERVER_PRIORITY, SNTP_MAX_SERVER_PRIORITY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpSrvrVer, SNTP_VERSION_MIN, SNTP_VERSION_MAX);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Ver, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_VERSION_MIN, SNTP_VERSION_MAX);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpSrvrPort, SNTP_MIN_SERVER_PORT, SNTP_MAX_SERVER_PORT);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Portid, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_SERVER_PORT, SNTP_MAX_SERVER_PORT);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPollIntvlVal, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ucast_1, pStrInfo_base_SntpClientUcast, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Client_1, pStrInfo_base_SntpClient, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_PollIntvl, pStrInfo_base_SntpClientUcastPollIntvl, commandSntpClientUnicastPollInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, NULL, buf, NULL, 3, L7_NODE_UINT_RANGE, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPollRetryVal, SNTP_MIN_POLL_RETRY, SNTP_MAX_POLL_RETRY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_PollRetry_2, pStrInfo_base_SntpClientUcastPollRetry, commandSntpClientUnicastPollRetry, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_PollRetry, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SntpClientPollTimeoutVal, SNTP_MIN_POLL_TIMEOUT, SNTP_MAX_POLL_TIMEOUT);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_PollTimeout_1, pStrInfo_base_SntpClientUcastPollTimeout, commandSntpClientUnicastPollTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_PollTimeout, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
/*********************************************************************
*
* @purpose  Build the tree nodes for size option of Traceroute commands
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
void buildTreePrivTraceRouteSize(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_common_Size_1, pStrInfo_base_TraceRouteSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Range0to65507, pStrInfo_base_TraceRouteSizeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for port option of Traceroute commands
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
void buildTreePrivTraceRoutePort(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_common_Port_4, pStrInfo_base_TraceRoutePort_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Range1to65535, pStrInfo_base_TraceRoutePortRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteSize( depth2 );
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for count option of Traceroute commands
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
void buildTreePrivTraceRouteCount(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_base_Count_1, pStrInfo_base_TraceRouteCount, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Range1to10, pStrInfo_base_TraceRouteCountRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRoutePort( depth2 );
  buildTreePrivTraceRouteSize( depth2 );
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for interval option of Traceroute commands
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
void buildTreePrivTraceRouteInterval(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_common_WsInputIntvl, pStrInfo_base_TraceRouteIntvl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Range1to60, pStrInfo_base_TraceRouteIntvlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteCount( depth2 );
  buildTreePrivTraceRoutePort( depth2 );
  buildTreePrivTraceRouteSize( depth2 );
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for maxFail option of Traceroute commands
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
void buildTreePrivTraceRouteMaxFail(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;
  depth1 = ewsCliAddNode (depth, pStrInfo_base_Maxfail_1, pStrInfo_base_TraceRouteMaxFail_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_0255, pStrInfo_base_TraceRouteMaxFailRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteInterval( depth2 );
  buildTreePrivTraceRouteCount( depth2 );
  buildTreePrivTraceRoutePort( depth2 );
  buildTreePrivTraceRouteSize( depth2 );
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for maxttl option of Traceroute commands
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
void buildTreePrivTraceRouteMaxTtl(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_base_Maxttl_1, pStrInfo_base_TraceRouteMaxTtl_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Range1to255, pStrInfo_base_TraceRouteMaxTtlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteMaxFail( depth2 );
  buildTreePrivTraceRouteInterval( depth2 );
  buildTreePrivTraceRouteCount( depth2 );
  buildTreePrivTraceRoutePort( depth2 );
  buildTreePrivTraceRouteSize( depth2 );
  return;
}

/*********************************************************************
*
* @purpose  Build the tree nodes for initTtl option of Traceroute commands
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
void buildTreePrivTraceRouteInitTtl(EwsCliCommandP depth)
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(depth, pStrInfo_base_Initttl_1, pStrInfo_base_TraceRouteInitTtl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Range0to255, pStrInfo_base_TraceRouteInitTtlRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteMaxTtl( depth2 );
  buildTreePrivTraceRouteMaxFail( depth2 );
  buildTreePrivTraceRouteInterval( depth2 );
  buildTreePrivTraceRouteCount( depth2 );
  buildTreePrivTraceRoutePort( depth2 );
  buildTreePrivTraceRouteSize( depth2 );
  return;
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Traceroute commands
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
void buildTreePrivTraceRoute(EwsCliCommandP depth2)
{
  /* depth1 = Privileged Exec Mode */

  EwsCliCommandP depth3, depth4;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth5, depth6;
#endif

  depth3 = ewsCliAddNode(depth2, "<ipaddr|hostname> ", pStrInfo_base_TraceRouteIpAddr, commandTraceRoute, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreePrivTraceRouteInitTtl( depth3 );
  buildTreePrivTraceRouteMaxTtl( depth3 );
  buildTreePrivTraceRouteMaxFail( depth3 );
  buildTreePrivTraceRouteInterval( depth3 );
  buildTreePrivTraceRouteCount( depth3 );
  buildTreePrivTraceRoutePort( depth3 );
  buildTreePrivTraceRouteSize( depth3 );

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_TraceRouteKeyword, commandIpv6Traceroute, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6HostAddr, pStrInfo_base_Ipv6PingHostAddr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Port, pStrInfo_base_TraceRoutePortNo, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode Image commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode Image commands
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedImage(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8 image1FileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2FileName[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_STACKING_PACKAGE
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#endif

#if L7_FEAT_DIM_USE_FILENAME
  usmDbImageFileNameGet(0, image1FileName);
  usmDbImageFileNameGet(1, image2FileName);
#else
  usmDbActiveNameGet(image1FileName);
  usmDbBackupNameGet(image2FileName);
#endif

  depth6 = 0;    /* keep the compiler quiet about not using this in standalone build */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Boot, pStrInfo_base_MarkActiveImage, commandBoot,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_BootSys, pStrInfo_base_MarkActiveImage, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, image1FileName, pStrInfo_base_MarkActiveImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, image2FileName, pStrInfo_base_MarkActiveImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
                               L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
  {
    cliTreePrivilegedAutoCopySw(depth2);
  }
#endif

#ifdef L7_AUTO_INSTALL_PACKAGE
  cliTreePrivilegedAutoinstall(depth2);
#endif /* L7_AUTO_INSTALL_PACKAGE */

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(buf, (L7_int32)sizeof(buf), pStrInfo_common_StackingUnit, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Unit_1, buf, NULL, 3, L7_NODE_UINT_RANGE, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
  depth5 = ewsCliAddNode(depth4, image1FileName, pStrInfo_base_MarkActiveImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, image2FileName, pStrInfo_base_MarkActiveImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif

}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for Privilege Exec mode File System
 *           commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes creates the tree nodes for Privilege Exec mode File system
 *        commands
 *
 * @end
 *
 *********************************************************************/
void cliTreePrivilegedFile(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_char8 image1FileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2FileName[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  L7_uchar8      buf[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(buf, sizeof(buf)-1, "<%d - %d> ", L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
#endif

#if L7_FEAT_DIM_USE_FILENAME
  usmDbImageFileNameGet(0, image1FileName);
  usmDbImageFileNameGet(1, image2FileName);
#else
  usmDbActiveNameGet(image1FileName);
  usmDbBackupNameGet(image2FileName);
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Del_1, pStrInfo_base_DelsGivenImageOnNode, commandDelete, L7_NO_OPTIONAL_PARAMS);

#if L7_FEAT_DIM_USE_FILENAME
  depth3 = ewsCliAddNode (depth2, image1FileName, pStrInfo_base_DimImage, NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  depth3 = ewsCliAddNode (depth2, image2FileName, pStrInfo_base_DimImage, NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
  depth3 = ewsCliAddNode(depth2, buf, CLIDIM_UNIT_HELP, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY, 
                         L7_OPTIONS_NODE, depth2);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif  /* L7_STACKING_PACKAGE */

#if L7_FEAT_BOOTCODE_UPDATE
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Update, CLIUPDATEBOOTCODE_HELP, commandUpdate, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_BootCode, CLIUPDATEBOOTCODE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

#ifdef L7_STACKING_PACKAGE

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Unit_1, CLIDIM_UNIT_HELP, NULL, 3, L7_NODE_UINT_RANGE, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif /* L7_STACKING_PACKAGE */

  depth2 = ewsCliAddNode (depth1, pStrInfo_base_Filedescr, pStrInfo_base_FileDescr, commandFileDescr, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2, image1FileName, pStrInfo_base_DimImage, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode (depth3, pStrInfo_base_Desc, pStrInfo_base_FileDescr, NULL, 3, L7_NODE_STRING_RANGE, 1, L7_CLI_MAX_STRING_LENGTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode (depth2, image2FileName, pStrInfo_base_DimImage, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode (depth3, pStrInfo_base_Desc, pStrInfo_base_FileDescr, NULL, 3, L7_NODE_STRING_RANGE, 1, L7_CLI_MAX_STRING_LENGTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
 *
 * @purpose  To build the TACACS+ config mode tree.
 *
 * @PAram void
 *
 * @returntype void
 *
 * @note
 *
 * @end
 *
 *********************************************************************/
void buildTreeTacacsConfigMode()
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliTacacsMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_TACACS_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeTacacsConfig(depth1);
}
/*********************************************************************
 *
 * @purpose  To build the Mail Server config mode tree.
 *
 * @PAram void
 *
 * @returntype void
 *
 * @note
 *
 * @end
 *
 *********************************************************************/
void buildTreeMailServerConfigMode()
{
  EwsCliCommandP depth1, depth2, depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliMailServerMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_MAIL_SERVER_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeMailServerConfig(depth1);
}

/* Denial Of Service CLI */
/*********************************************************************
 *
 * @purpose  Build the tree nodes for DenialOfService commands
 *
 * @param struct EwsCliCommandP
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 **********************************************************************/
void buildTreeMailServerConfig(EwsCliCommandP depth1)
{
    EwsCliCommandP depth2,depth3,depth4;
	
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_mailserverPortCmd ,pStrInfo_base_mailserverPort , commandLoggingEmailAlertSMTPServerPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode ( depth2, "<465 | 25 | 1 to 65535>", pStrInfo_base_mailserverPort , NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
		
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_mailserverSecurityCmd ,pStrInfo_base_mailserverSecurity , commandLoggingEmailAlertSMTPServerSecurity, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_MGMT_SECURITY_PACKAGE
    depth3 = ewsCliAddNode ( depth2, "tlsv1", pStrInfo_base_mailserverSecuritytlsv1, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
    depth3 = ewsCliAddNode ( depth2, "none", pStrInfo_base_mailserverSecuritynone, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth2 = ewsCliAddNode(depth1, pStrInfo_base_mailserverusernameCmd ,pStrInfo_base_mailserverusername, commandLoggingEmailAlertusername, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode ( depth2, "<username>", pStrInfo_base_mailserverusername, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth2 = ewsCliAddNode(depth1, pStrInfo_base_mailserverPasswordCmd ,pStrInfo_base_mailserverPassword, commandLoggingEmailAlertpassword, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode ( depth2, "<password>", pStrInfo_base_mailserverPassword, NULL, L7_NO_OPTIONAL_PARAMS);		
    depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}


/* Denial Of Service CLI */
/*********************************************************************
 *
 * @purpose  Build the tree nodes for DenialOfService commands
 *
 * @param struct EwsCliCommandP
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 **********************************************************************/
void buildTreeGlobalDenialOfService(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_uint32 unit;  unit = cliGetUnitId();
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_DosCntrl_1, pStrInfo_base_CfgDos, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_CfgDosAll, commandConfigDoSAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /*FB / FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_FirstFrag, pStrInfo_base_CfgDosv4FirstFrag, commandConfigDoSFirstFrag, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to255, pStrInfo_base_CfgDosTcpHdrSize, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*no FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Icmp_1, pStrInfo_base_CfgDosIcmp, commandConfigDoSICMP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to16376, pStrInfo_base_CfgDosIcmpSize, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV4_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Icmpv4, pStrInfo_base_CfgDosIcmpV4, commandConfigDoSICMP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to16376, pStrInfo_base_CfgDosIcmpSize, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV6_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Icmpv6, pStrInfo_base_CfgDosIcmpV6, commandConfigDoSICMPv6, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to16376, pStrInfo_base_CfgDosIcmpSize, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_IcmpFrag, pStrInfo_base_CfgDosIcmpFrag, commandConfigDoSICMPFrag, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*no FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_L4port, pStrInfo_base_CfgDosL4Port, commandConfigDoSL4Port, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*xgs3*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_SipDip, pStrInfo_base_CfgDosSipDip, commandConfigDoSSIPDIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_SmacDmac, pStrInfo_base_CfgDosSmacDmac, commandConfigDoSSMACDMAC, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpFinUrgPsh, pStrInfo_base_CfgDosTcpFinUrgPsh, commandConfigDoSTCPFinUrgPsh, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*no FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpFlag, pStrInfo_base_CfgDosTcpFlag, commandConfigDoSTCPFlag, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpFlagSeq, pStrInfo_base_CfgDosTcpFlagSeq, commandConfigDoSTCPFlagSeq, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*no FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpFrag, pStrInfo_base_CfgDosTcpFrag, commandConfigDoSTCPFrag, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpOffset, pStrInfo_base_CfgDosTcpOffset, commandConfigDoSTCPOffset, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpPort, pStrInfo_base_CfgDosTcpPort, commandConfigDoSTCPPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpSyn, pStrInfo_base_CfgDosTcpSyn, commandConfigDoSTCPSyn, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_TcpSynFin, pStrInfo_base_CfgDosTcpSynFin, commandConfigDoSTCPSynFin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  /*FB2*/
  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_UdpPort, pStrInfo_base_CfgDosUdpPort, commandConfigDoSUDPPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DenialOfService Show commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 **********************************************************************/
void buildTreeShowDenialOfService(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_DosCntrl_1, pStrInfo_base_ShowDos, commandShowDenialOfService, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/* End - Denial of Service CLI */

/*********************************************************************
*
* @purpose  Terminal Length build tree
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
void  buildTreeUserExecTerminalLength(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */

  EwsCliCommandP depth2, depth3, depth4, depth5;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Terminal, pStrInfo_base_CfgTerminal, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Len, pStrInfo_base_CfgTerminalLen, commandSetTerminalLength, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_0548, pStrInfo_base_CfgTerminalLenRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose build the tree for CPU Utilization
* @param EwsCliCommandP depth3
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/
void buildTreePrivCpuInfo(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Process, pStrInfo_base_ProcessHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Cpu,   pStrInfo_base_CpuHelp, commandShowProcessCpu, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Cpu_Thr,   
                         pStrInfo_base_CpuThrHelp, 
                         commandShowProcessCpuThreshold,
                         L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
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

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_ArpSwitch, pStrInfo_common_ClrArpSwitch,
                         commandClearArpSwitch, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr , pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

#ifndef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose build the tree for wireless modes in non-wireless builds 
*          to support the text based configuration
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/

void buildTreeNonWirelessConfigSupport()
{
  EwsCliCommandP depth1,depth2, depth3, depth4, depth5;

  depth1 = ewsCliAddNode (NULL, NULL, NULL, cliWirelessModeSupport,
      L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_WIRELESS_CONFIG_MODE, depth1);
  
  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_WsShowRunningNwMode_Nonwireless, pStrInfo_wireless_WsNw_Nonwireless,
                         cliWirelessNetworkModeSupport, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_wireless_NetworkId_Node_Nonwireless, pStrInfo_wireless_WsNwId_Nonwireless, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_WIRELESS_NETWORK_CONFIG_MODE);
 
  depth2 = ewsCliAddNode (depth1, pStrInfo_wireless_Ap_Nonwireless, pStrInfo_wireless_WsAp_Nonwireless, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Dbase_Nonwireless, pStrInfo_wireless_WsApLocal_Nonwireless,
                         cliWirelessAPModeSupport, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Macaddr_Nonwireless, pStrInfo_wireless_ShowWsApMac_Nonwireless, NULL, 4,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_NODE_DATA_TYPE, L7_MAC_ADDRESS_SVL_DATA_TYPE);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_WIRELESS_AP_CONFIG_MODE);
 
  depth3 = ewsCliAddNode (depth2, pStrInfo_wireless_ApShowRunningProfile_Nonwireless, pStrInfo_wireless_WsApProfile_Nonwireless, cliAPProfileModeSupport, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_wireless_ProfileId_Node_Nonwireless, pStrInfo_wireless_WsApProfileId_Nonwireless, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,L7_NODE_TYPE_MODE,L7_WIRELESS_AP_PROFILE_CONFIG_MODE);

  depth1 = ewsCliAddNode (NULL, NULL, NULL, cliWirelessAPModeSupport,
                         L7_NO_OPTIONAL_PARAMS);
  cliSetMode (L7_WIRELESS_AP_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);


  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliWirelessNetworkModeSupport,
                         L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_WIRELESS_NETWORK_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth1 = ewsCliAddNode (NULL, NULL, NULL, cliAPProfileModeSupport,
                         L7_NO_OPTIONAL_PARAMS);
  cliSetMode (L7_WIRELESS_AP_PROFILE_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode (depth1, pStrInfo_wireless_ApProfileShowRunningRadio_Nonwireless, pStrInfo_wireless_WsApProfileRadio_Nonwireless, cliAPProfileRadioModeSupport, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_wireless_RadioId_Node_Nonwireless, pStrInfo_wireless_WsApProfileRadioId_Nonwireless , NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2, L7_NODE_TYPE_MODE, L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE);

  depth1 = ewsCliAddNode (NULL, NULL, NULL, cliAPProfileRadioModeSupport, L7_NO_OPTIONAL_PARAMS);
  cliSetMode (L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode (depth1, pStrInfo_wireless_ApProfileShowRunningVap_Nonwireless, pStrInfo_wireless_WsApProfileVap_Nonwireless, cliAPProfileVAPModeSupport, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_wireless_vapId_Node_Nonwireless, pStrInfo_wireless_WsVap_Nonwireless, NULL,
                            L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NODE_TYPE_MODE, L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE); 

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliAPProfileVAPModeSupport, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
        }
#endif /* L7_WIRELESS_PACKAGE */

/*********************************************************************
*
* @purpose build the tree for autoinstall enable|disable
* @param EwsCliCommandP depth2
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/
#ifdef L7_AUTO_INSTALL_PACKAGE
void cliTreePrivilegedAutoinstall(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_AutoInstall, pStrInfo_base_BootAutoInstallHelp, commandBootAutoinstall,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_AutoInstallStart, pStrInfo_base_EnblAutoInstallHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_AutoInstallStop, pStrInfo_base_DsblAutoInstallHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* boot autoinstall auto-save.*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_AutoInstallAutoSave, 
                         pStrInfo_base_AutoInstallAutoSaveHelp,
                         commandBootAutoinstallAutoSave,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


  /* boot autoinstall retrycount.*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_AutoInstallRetryCount, 
                         pStrInfo_base_AutoInstallRetryCountHelp,
                         commandBootAutoinstallRetrycount,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_AutoInstallRetryCountRange,
                         pStrInfo_base_AutoInstallRetryCountRangeHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
}
#endif /* L7_AUTO_INSTALL_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
*
* @purpose build the tree for iphelper 
* @param EwsCliCommandP depth2
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/
void cliTreeGlobalIphelper(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_uchar8   udpBuf[L7_CLI_MAX_STRING_LENGTH];

  depth3 = ewsCliAddNode(depth2, "helper ", "Enable IP Helper", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, "enable ", "Enable IP Helper", commandIpHelperEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, "helper-address ", "Configure Helper address entry", commandIpHelperAddressSet, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, "<ipaddr> ", "IPv4 address of server", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(udpBuf, sizeof(udpBuf)-1, "<%d - %d> ", L7_IP_HELPER_MIN_PORT, L7_IP_HELPER_MAX_PORT);
  depth5 = ewsCliAddNode(depth4, udpBuf, "Destination UDP port of packets to be relayed", NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
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
  depth5 = ewsCliAddNode(depth4, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
}
#endif

/*********************************************************************
*
* @purpose  Build the tree for SDM template configuration.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeSdm(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
#ifdef L7_ROUTING_PACKAGE
  EwsCliCommandP depth5, depth6;
#endif

  depth2 = ewsCliAddNode(depth1, "sdm", "Switch database management", 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "prefer", "Select an SDM template to change system scaling factors",
                         commandSdmPrefer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth4 = ewsCliAddNode(depth3, "dual-ipv4-and-ipv6", "Support both IPv4 and IPv6",
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "default", "Default bias",
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
#endif
  depth4 = ewsCliAddNode(depth3, "ipv4-routing", "Support IPv4-only routing",
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "default", "Default bias",
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  if (usmDbSdmTemplateSupported(SDM_TEMPLATE_V4_DATA_CENTER))
  {
    depth5 = ewsCliAddNode(depth4, "data-center", "Support more ECMP next hops in IPv4 routes",
                           NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                           NULL, 2, L7_STATUS_NORMAL_ONLY, L7_NO_OPTIONAL_PARAMS);
  }
#endif
}

/*********************************************************************
*
* @purpose  Build the tree to show SDM template configuration and status.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowSdm(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
#ifdef L7_ROUTING_PACKAGE
  EwsCliCommandP depth6, depth7;
#endif

  depth3 = ewsCliAddNode(depth2, "sdm", "Switch database management", 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, "prefer", "Show active SDM template and parameters",
                         commandShowSdmPrefer, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth5 = ewsCliAddNode(depth4, "dual-ipv4-and-ipv6", "List template parameters for a dual template",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "default", "Show parameters for template with default bias",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  depth5 = ewsCliAddNode(depth4, "ipv4-routing", "List template parameters for an IPv4-only template",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "default", "Show parameters for template with default bias",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbSdmTemplateSupported(SDM_TEMPLATE_V4_DATA_CENTER))
  {
    depth6 = ewsCliAddNode(depth5, "data-center", "Show parameters for data center template",
                           NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                           NULL, L7_NO_OPTIONAL_PARAMS);
  }
#endif
}
