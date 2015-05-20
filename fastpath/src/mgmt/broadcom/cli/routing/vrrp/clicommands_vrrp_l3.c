/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/vrrp/clicommands_vrrp_l3.c
*
* @purpose create the cli commands for VRRP functions
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

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"
#include "clicommands_l3.h"

#include "datatypes.h"

#include "usmdb_util_api.h"
#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"
#include "vrrp_exports.h"

/*********************************************************************
*
* @purpose  Build the Show VRRP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowVrrp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8, depth9;

  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Vrrp_1, pStrInfo_routing_ShowVrrp, commandShowVrrp, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_routing_ShowVrrpIntf, commandShowVrrpInterface, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth6 = buildTreeInterfaceHelp(depth5, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_routing_Vrid, pStrInfo_routing_CfgVrrpIntfIdNum, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Brief, pStrInfo_routing_ShowVrrpIntfSummary, commandShowVrrpInterfaceBrief, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Stats, pStrInfo_routing_ShowVrrpIntfStats, commandShowVrrpInterfaceStats, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth7 = buildTreeInterfaceHelp(depth6, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_routing_Vrid, pStrInfo_routing_CfgVrrpIntfIdNum, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
}

/*********************************************************************
*
* @purpose  Build the Routing Interface VRRP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingGlobalVrrp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Vrrp_1, pStrInfo_routing_VrrpDsbl, commandIPVrrp, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the Routing Interface VRRP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeRoutingInterfaceVrrp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10, depth11;
  L7_uchar8 decrRange[16];

  /* depth2 = "ip " */

  depth3 = ewsCliAddNode(depth2, pStrInfo_routing_Vrrp_1, pStrInfo_routing_CfgVrrp, commandVrrpInterfaceRouterid, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to255, pStrInfo_routing_CfgVrrpIntfIdNum, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_routing_CfgVrrpIntfVridNewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Auth_1,pStrInfo_routing_CfgVrrpIntfAuthType, commandVrrpVrIDAuthentication, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_routing_IpVrrpVirtualIntfAuthNone, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Simple_2, pStrInfo_routing_IpVrrpVirtualIntfAuthSimple, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Key, pStrErr_routing_CfgRtrRipKey, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_IpOption, pStrInfo_routing_CfgVrrpIntfIpAddr, commandVrrpVrIDIp, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Addr, pStrInfo_routing_CfgVrrpIntfIpAddr_1, NULL, 6, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_routing_Secondary_1, pStrInfo_routing_EnterSecondary, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ApShowRunningMode, pStrInfo_routing_CfgVrrpIntfVridMode, commandIPVrrpID, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Preempt, pStrInfo_routing_CfgVrrpIntfPremptMode, commandVrrpVrIDPreempt, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_AcceptMode, pStrInfo_routing_CfgVrrpIntfAcceptMode, commandVrrpVrIDAcceptMode, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_routing_CfgVrrpIntfPri_1, commandVrrpVrIDPriority, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range1to254, pStrInfo_routing_CfgVrrpIntfPri, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Timers, pStrInfo_routing_VrrpTimers, commandVrrpVrIDTimersAdvertise, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Advertise_1, pStrInfo_routing_VrrpTimerAdvertise, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Range1to255, pStrInfo_common_CfgTime, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Track, pStrInfo_routing_VrrpTrack, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_routing_VrrpTrackIntf, commandVrrpVrIDTrackInterface, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = buildTreeInterfaceHelp(depth6, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Decrement, pStrInfo_routing_VrrpPriorityDecrement, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Range1to254, pStrInfo_routing_CfgVrrpIntfPri, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IpOption, pStrInfo_routing_VrrpTrackIPRoute, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);   
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Route, pStrInfo_routing_VrrpTrackIPRoute, commandVrrpVrIDTrackIpRoute, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);   

  depth8 = ewsCliAddNode(depth7,  pStrInfo_common_IpAddressPfxLen,pStrInfo_routing_VrrpPfxLen, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH); 
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Decrement, pStrInfo_routing_VrrpPriorityDecrement, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  osapiSnprintf(decrRange, 16, "<%u-%u>", L7_VRRP_TRACK_ROUTE_DECREMENT_MIN, L7_VRRP_TRACK_ROUTE_DECREMENT_MAX);
  depth10 = ewsCliAddNode(depth9, decrRange, "Enter amount to decrement router priority", NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_VRRP_MAP_COMPONENT_ID);
  
}

