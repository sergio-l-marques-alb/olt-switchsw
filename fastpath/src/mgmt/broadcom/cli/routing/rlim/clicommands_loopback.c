/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/routing/clicommands_loopback.c
*
* @purpose create the cli commands for Loopback Intf functions
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
#include "clicommands_loopback.h"
#include "ews_cli.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_ipv6.h"
#endif

/*********************************************************************
*
* @purpose  Build the Show tree for Loopback Interface.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeShowLoopbackInfo(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_LoopBack, pStrInfo_common_ShowLoopBackIntf,
      commandShowLoopbackInfo, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
      L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, NULL, pStrInfo_common_ShowLoopBackIntf, NULL,
      3, L7_NODE_UINT_RANGE, L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
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
void buildTreeLoopbackInterfaceConfiguration(void)
{
  EwsCliCommandP depth1, depth2, depth3, depth4, depth5, depth6, depth7;
#ifdef L7_OSPF_PACKAGE
 EwsCliCommandP depth8;
#endif

  L7_uint32 unit;

  unit = cliGetUnitId();

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliInterfaceMode,
      L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_LOOPBACK_CONFIG_MODE, depth1);

  /* description <string>; no description */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Desc_2, pStrInfo_base_IntfDescr,
                         commandIntfDescription, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Desc, pStrInfo_base_IntfDescrInfo, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Shutdown_2, pStrInfo_common_Shutdown,
    commandShutdown, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Snmp_1, pStrInfo_common_Snmp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Trap_2, pStrInfo_common_Trap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LinkStatus_1, pStrInfo_common_SnmpTrapLinkStatus, commandSnmpTrapLinkStatus, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Assigning the IPv4 Address to the interface */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_IpOption, pStrInfo_common_CfgIp,
    NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Addr_4, pStrInfo_routing_CfgIpIntfNwCreate,
    commandIpAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
            L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_routing_Ipv4Addr,
                         pStrInfo_common_DiffservIpAddr, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mask, pStrInfo_common_DiffservIpMask,
    NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
    2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Secondary_1, pStrInfo_routing_EnterSecondary, NULL, 2,
      L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
      L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_OSPF_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ospf_1, pStrInfo_common_CfgRtrOspf, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* ip ospf cost */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cost2, pStrInfo_common_CfgRtrOspfIntfCost, commandIpOspfCost, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to65535, pStrInfo_common_CfgRtrOspfIntfCostRange, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);

  /* ip ospf area */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Area_2, pStrInfo_common_CfgRtrOspfIntfAreaId, commandIpOspfAreaid, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_ABCD, pStrInfo_common_EnterIpAddrAreaId, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to2147483647_2, pStrInfo_common_EnterAreaIdInDecimal, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_routing_Second, pStrInfo_routing_IpOspfSecond_2, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_routing_IpOspfSecond_1, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_OSPF_MAP_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
#endif

#ifdef L7_IPV6_PACKAGE
  /* Assigning the IPv6 Address to the interface */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Diffserv_5, pStrInfo_common_CfgIpv6,  NULL,
      2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   buildTreeRoutingInterfaceNetDbOspfv3(depth2, L7_LOOPBACK_CONFIG_MODE);
   buildTreeInterfaceIpv6(depth2, L7_LOOPBACK_CONFIG_MODE);
#endif

}
