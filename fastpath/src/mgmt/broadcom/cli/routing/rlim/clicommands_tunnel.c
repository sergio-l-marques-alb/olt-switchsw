/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/clicommands_tunnel.c
 *
 * @purpose create the cli commands for Tunneling functions
 *
 * @component user interface
 *
 *
 * @create  07/14/2005
 *
 * @author  Ravi Saladi
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
#include "defaultconfig.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#include "ews_cli.h"

/*********************************************************************
*
* @purpose  Build the Show tree for Tunneling.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowTunnelInfo(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Tunnel_1, pStrInfo_common_ShowTunnelIntf,
                          commandShowTunnelInfo, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, NULL, pStrInfo_routing_ShowTunnelIntfId, NULL,
                         3, L7_NODE_UINT_RANGE, L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

void buildTreeInterfaceIpv6TunnelDestination(EwsCliCommandP depth2)

{

  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipv6DhcpRelayDest, pStrInfo_routing_EnterDestAddr,
                         commandTunnelDestination, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Ipv4Addr, pStrInfo_routing_TunnelLocalAddr,
                         NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /* Specifing the tunnel destination address */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_ApShowRunningMode, pStrInfo_routing_Ipv6Mode,
                         commandTunnelModeIpv6Ip, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Ipv6IpMode_1, pStrInfo_routing_Ipv6IpMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Ipv6Ip6To4, pStrInfo_routing_6To4Mode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /* Specifing the source transport address */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_SrcPortMirror, pStrInfo_routing_EnterSrcAddr,
                         commandIPV6TunnelSource,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Ipv4Addr, pStrInfo_routing_TunnelLocalAddr,
                         NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_DhcpEtherNet, pStrInfo_routing_IntfType, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Config Tunnel In Interface mode.
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeTunnelInterfaceConfiguration(void)
{
  EwsCliCommandP depth1, depth2, depth3,depth4, depth5;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliInterfaceMode,
                         L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_TUNNEL_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Shutdown_2, pStrInfo_common_Shutdown,
                         commandShutdown, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Snmp_1, pStrInfo_common_Snmp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Trap_2, pStrInfo_common_Trap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LinkStatus_1, pStrInfo_common_SnmpTrapLinkStatus, commandSnmpTrapLinkStatus, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

 /* description <string>; no description */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Desc_2, pStrInfo_base_IntfDescr,
                         commandIntfDescription, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Desc, pStrInfo_base_IntfDescrInfo, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);  

 #ifdef L7_IPV6_PACKAGE
  /* Assigning the IPV6 address to the interface */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5, pStrInfo_common_CfgIpv6,  NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreeInterfaceIpv6(depth2, L7_TUNNEL_CONFIG_MODE);
  buildTreeInterfaceIpv6Mtu(depth2,L7_TUNNEL_CONFIG_MODE);
  buildTreeRoutingInterfaceNetDbOspfv3(depth2, L7_TUNNEL_CONFIG_MODE);
  buildTreeRoutingInterfaceIpv6Unreachables (depth2);
  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Tunnel_1, pStrInfo_common_Ipv6Tunnel, NULL,
                          2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreeInterfaceIpv6TunnelDestination(depth2);
}
#endif
#endif
