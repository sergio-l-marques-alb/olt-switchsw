/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*                                                                     
* @filename   strlib_dot1ad_cli.c                                                      
*                                                                     
* @purpose    Strings Library for Dot1ad Component.                                                      
*                                                                     
* @component  Common Strings Library                                                      
*                                                                     
* @comments   none                                                    
*                                                                     
* @create     14/04/2008                                                      
*                                                                     
* @author     Rajakrishna                                
*                                                                     
* @end                                                                
*                                                                     
**************************************************************************/

#include "strlib_dot1ad_cli.h"

L7_char8*pStrInfo_dot1ad_Dot1adInterfaceType = "mode";
L7_char8 *pStrInfo_dot1ad_Dot1adInterface = "interface";

L7_char8 *pStrInfo_dot1ad_Dot1adL2tunnel = "l2tunnel";
L7_char8 *pStrInfo_dot1ad_Dot1adReservedMAC = "mac-address";
L7_char8 *pStrInfo_dot1ad_Dot1adProtocolId = "protocol-id";
L7_char8 *pStrInfo_dot1ad_Dot1adProtoId = "protocol-id";


L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni = "uni";
L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni_p = "uni-p";
L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni_s = "uni-s";
L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_switchport = "switchport";
L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_nni = "nni";
L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_nni_1 = " nni ";
L7_char8 *pStrInfo_dot1ad_Dot1adModeNNI = "dot1ad mode nni ";
L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI_P = "dot1ad mode uni-p ";
L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI_S = "dot1ad mode uni-s ";
L7_char8 *pStrInfo_dot1ad_Dot1adModeSwitchPort = "dot1ad mode switchport ";
L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI = "dot1ad mode uni ";
L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_tunnel = "tunnel";
L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_terminate = "terminate";
L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_discard = "discard";
L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_shutdown =
                                               "shutdown";
L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_discard_shutdown =
                                               "discard and shutdown";
L7_char8 *pStrInfo_dot1ad_Cmd_Dot1adTunnelAction_discard_shutdown =
                                               "discard shutdown";

L7_char8 *pStrInfo_dot1ad_ReservedMacaddr = "<reserved MAC>";
L7_char8 *pStrInfo_dot1ad_ReservedMacaddrInfo = 
                    "Enter Reserved MAC address in the range:01:80:c2:00:00:00 to 01:80:c2:00:00:3f.";
L7_char8 *pStrInfo_dot1ad_ProtocolId = "Enter Protocol ID in the range 0x0001 to 0xffff";
L7_char8 *pStrInfo_dot1ad_ProtoId = "<proto-id>";
L7_char8 *pStrInfo_dot1ad_Macaddr = "<mac-addr>";
L7_char8 *pStrInfo_dot1ad_MatchAll = "Match All";


L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType = 
                 "Configure the Dot1ad Type for the interface.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adl2tunnelaction = 
                 "Configure the Dot1ad tunnel action for the interface.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adReservedMacaddr =
                 "Enter the reserved MAC address.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adProtocolId = "Enter the 'protocol-id'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceInfo = "Enter the 'interface'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adMacAddrInfo = "Enter the 'mac-address' ";


L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_tunnel = 
                "Configure the tunnel action as 'tunnel'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_terminate = 
                "Configure the tunnel action as 'terminate'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_discard = 
                "Configure the tunnel action as 'discard'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_discardshutdown = 
                "Configure the tunnel action as 'discard and shutdown'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_nni = 
                "Configure the dot1ad interface type as 'nni'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni = 
                "Configure the dot1ad interface type as 'uni'.";
L7_char8 *pStrErr_dot1ad_CfgDot1adTunnelNoEntry =
                "No Entry for this tunnel info.";

L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni_p = 
                "Configure the dot1ad interface type as 'port based uni'.";
L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_switchport =
                "Configure the dot1ad interface type as 'ordinary switch port'.";


L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni_s = 
                "Configure the dot1ad interface type as 'service based uni'.";

L7_char8 *pStrInfo_dot1ad_CfgDot1adIntfType =
               "Use dot1ad mode {uni | nni }";

L7_char8 *pStrInfo_dot1ad_CfgDot1adIntfType_non_cpe =
               "Use dot1ad mode {uni-p | uni-s | nni | switchport}";

L7_char8 *pStrErr_dot1ad_Dot1adInterfaceType = 
               "Unable to configure Interface Type.";

L7_char8 *pStrErr_dot1ad_CfgDot1adL2Tunnel = 
                    "Use 'dot1ad l2tunnel mac-address <reserved MAC> \
[protocol-id <proto-id>] {tunnel | de-tunnel | terminate|  discard [shutdown]}'.";

L7_char8 *pStrErr_dot1ad_Dot1adTunnelAction = 
                  "Unable to configure the Tunnel Action.";


L7_char8 *pStrInfo_dot1ad_ShowDot1adInterfaceType = 
                 "Display the Dot1ad Type for the interface.";
L7_char8 *pStrInfo_dot1ad_ShowDot1adInterfaceTypeAll = 
                 "Display the Dot1ad Type for all interfaces.";
L7_char8 *pStrInfo_dot1ad_ShowDot1adIntfTypeCommand =
	          "Use 'show dot1ad mode {all | <unit/slot/port>}'";
L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelCommand =
                 "Use 'show dot1ad l2tunnel {all | interface <unit/slot/port> \
|mac-address <mac-addr> |protocol-id <proto-id>}'";

L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelInfo = 
                  "Display the Dot1ad Tunnel Information of the interface.";
L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelInfoAll = 
                  "Display the Dot1ad Tunnel Information for all interfaces.";

L7_char8 *pStrInfo_dot1ad_DisplayDot1adIntfTypeDetails =
                  "Interface    Dot1ad InterfaceType";
L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails1 =
                  "Interface     MAC-address     Protocol-Id          Action     ";
L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails2 =
                 "MAC-address        Protocol-Id     Action     ";
L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails3 =
                 "Interface     Protocol-Id        Action     ";
L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails4 =
                 "Interface     MAC-address           Action     ";

/* VLAN based L2PT strings */
L7_char8 *pStrInfo_dot1ad_CfgDot1adVlanInfo = "Enter the 'vlan-id' ";
L7_char8 *pStrInfo_dot1ad_vlanId = "vlan";

L7_char8 *pStrInfo_dot1ad_vlanId_value = "<vlan id>";

L7_char8 *pStrErr_dot1ad_CfgDot1adL2Tunnel_based_on_vlan =
                    "Use 'dot1ad l2tunnel vlan <vlan id> mac-address <reserved MAC> \
[protocol-id <proto-id>] {tunnel | de-tunnel | terminate|  discard [shutdown]}'.";

L7_char8 *pStrInfo_dot1ad_VLAN ="VLAN";
L7_char8 *pStrInfo_dot1ad_MACAddress ="MAC-Address";
L7_char8 *pStrInfo_dot1ad_Protocolid ="Protocol-Id";
L7_char8 *pStrInfo_dot1ad_ACTION ="Action";

L7_char8 *pStrInfo_dot1ad_service_not_configured= "Service specified is not configured";
