
/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*                                                                     
* @filename   strlib_dot1ad_cli.h                                                      
*                                                                     
* @purpose    Strings Library for dot1ad component                                                      
*                                                                     
* @component  Strings Library                                                      
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

#ifndef STRLIB_DOT1AD_CLI_H
#define STRLIB_DOT1AD_CLI_H

#include "datatypes.h"
extern L7_char8*pStrInfo_dot1ad_Dot1adInterfaceType ;
extern L7_char8 *pStrInfo_dot1ad_Dot1adInterface;

extern L7_char8 *pStrInfo_dot1ad_Dot1adL2tunnel;
extern L7_char8 *pStrInfo_dot1ad_Dot1adReservedMAC;
extern L7_char8 *pStrInfo_dot1ad_Dot1adProtocolId;
extern L7_char8 *pStrInfo_dot1ad_Dot1adProtoId;


extern L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni;
extern L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_nni;
extern L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_tunnel;
extern L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_terminate;
extern L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_discard;
extern L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_shutdown;
extern L7_char8 *pStrInfo_dot1ad_Dot1adTunnelAction_discard_shutdown;
extern L7_char8 *pStrInfo_dot1ad_Cmd_Dot1adTunnelAction_discard_shutdown;
extern L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni_p;
extern L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_uni_s;

extern L7_char8 *pStrInfo_dot1ad_ReservedMacaddr;
extern L7_char8 *pStrInfo_dot1ad_ReservedMacaddrInfo;
extern L7_char8 *pStrInfo_dot1ad_ProtocolId;
extern L7_char8 *pStrInfo_dot1ad_ProtoId;
extern L7_char8 *pStrInfo_dot1ad_Macaddr;
extern L7_char8 *pStrInfo_dot1ad_MatchAll;


extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adl2tunnelaction;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adReservedMacaddr;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adProtocolId;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceInfo;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adMacAddrInfo;


extern L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_tunnel;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_terminate;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_discard;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adTunnelAction_discardshutdown;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_nni;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni;
extern L7_char8 *pStrErr_dot1ad_CfgDot1adTunnelNoEntry ;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni_p;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adInterfaceType_uni_s;


extern L7_char8 *pStrInfo_dot1ad_CfgDot1adIntfType;
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adIntfType_non_cpe;

extern L7_char8 *pStrErr_dot1ad_Dot1adInterfaceType;

extern L7_char8 *pStrErr_dot1ad_CfgDot1adL2Tunnel;

extern L7_char8 *pStrErr_dot1ad_Dot1adTunnelAction;


extern L7_char8 *pStrInfo_dot1ad_ShowDot1adInterfaceType;
extern L7_char8 *pStrInfo_dot1ad_ShowDot1adInterfaceTypeAll;
extern L7_char8 *pStrInfo_dot1ad_ShowDot1adIntfTypeCommand;
extern L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelCommand;

extern L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelInfo;
extern L7_char8 *pStrInfo_dot1ad_ShowDot1adTunnelInfoAll;

extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adIntfTypeDetails;
extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails1;
extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails2;
extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails3;
extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails4 ;

/* VLAN based L2TP strings */
extern L7_char8 *pStrInfo_dot1ad_CfgDot1adVlanInfo;
extern L7_char8 *pStrInfo_dot1ad_vlanId;
extern L7_char8 *pStrInfo_dot1ad_vlanId_value;
extern L7_char8 *pStrErr_dot1ad_CfgDot1adL2Tunnel_based_on_vlan;
extern L7_char8 *pStrInfo_dot1ad_DisplayDot1adTunnelDetails1_Based_On_Vlan;

extern L7_char8 *pStrInfo_dot1ad_Dot1adModeNNI;
extern L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI_P;
extern L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI_S;
extern L7_char8 *pStrInfo_dot1ad_Dot1adModeUNI;
extern L7_char8 *pStrInfo_dot1ad_Dot1adInterfaceType_nni_1;
#endif

