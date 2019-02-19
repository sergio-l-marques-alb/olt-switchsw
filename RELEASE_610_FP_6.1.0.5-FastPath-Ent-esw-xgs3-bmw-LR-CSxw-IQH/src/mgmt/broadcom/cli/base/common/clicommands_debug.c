/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_debug.c
*
* @purpose Debug commands
*
* @component User Interface
*
* @comments contains the code to build the root of the tree
* @comments also contains functions that allow tree navigation
*
* @create  07/14/2006
*
* @author  cpverne
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_debug.h"
#include "clicommands_support.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"

#ifdef L7_BGP_PACKAGE
#include "clicommands_debug_bgp.h"
#endif /* L7_BGP_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
#include "clicommands_debug_l3.h"
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_MCAST_PACKAGE
#include "clicommands_debug_mcast.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_debug_mcast6.h"
#endif
#endif /* L7_MCAST_PACKAGE */

#ifdef L7_IPV6_PACKAGE
#include "clicommands_debug_ipv6.h"
#endif /* L7_IPV6_PACKAGE */

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "usmdb_qos_voip_api.h"
#include "strlib_qos_cli.h"
#endif
#include "snooping_exports.h"

/*********************************************************************
* @purpose  Build the tree for debug privledged commands
*
* @param  depth1  @b{(input)) the command structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void buildTreePrivledgedDebug(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  /* debug ... */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_LogSeverityDebug, pStrInfo_base_Debug, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  #ifdef L7_ROUTING_PACKAGE
  /* debug arp */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Arp_2, pStrInfo_base_DebugArp, commandDebugArpPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  #endif

  #ifdef L7_BGP_PACKAGE
  /* debug bgp ... */
  buildTreePrivledgedDebugBgp(depth2);
  #endif /*L7_PACKAGE_BGP*/

  /* debug clear */
  depth3 = ewsCliAddNode(depth2, pStrErr_common_CfgApProfileClrCmd, pStrInfo_base_DebugClr, commandSupportClear, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* debug console */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Console, pStrInfo_base_DebugConsole, commandDebugConsole, 2,  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /*debug dot1x ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot1x_2, pStrInfo_base_DebugDot1x, commandDebugDot1xPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugDot1xPkt, commandDebugDot1xPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, pStrInfo_base_DebugDot1xRxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, pStrInfo_base_DebugDot1xTxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
/* debuf dot3ah */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot3ah, pStrInfo_base_DebugDot3ah, commandDebugDot3ahPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugDot3ahPkt, commandDebugDot3ahPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, pStrInfo_base_DebugDot3ahRxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, pStrInfo_base_DebugDot3ahTxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
#endif
  /* debug Igmp-snooping ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Igmpsnooping, CLIDEBUGSNOOP_HELP(L7_AF_INET), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug snooping packet */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, CLIDEBUGSNOOPPACKET_HELP(L7_AF_INET), commandDebugSnoopPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, CLIDEBUGSNOOPRXPACKET_HELP(L7_AF_INET), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, CLIDEBUGSNOOPTXPACKET_HELP(L7_AF_INET), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

   /*debug ip acl...*/
#ifdef L7_ROUTING_PACKAGE
  depth3 = ewsCliAddNode(depth2,pStrInfo_common_Ip_2,pStrInfo_base_DebugIP , NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  depth4 = ewsCliAddNode(depth3,pStrInfo_common_Acl_2,pStrInfo_base_DebugAcl, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_AclNo ,pStrInfo_base_DebugAclNo, commandDebugIpPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/

   /* debug all mcast components */
  #ifdef L7_MCAST_PACKAGE
  buildTreePrivledgedDebugMcast(depth3);
  #endif

#ifdef L7_VRRP_PACKAGE
  /*debug ip vrrp...*/
  depth4 = ewsCliAddNode(depth3,pStrInfo_common_Vrrp , pStrInfo_base_DebugVrrp , commandDebugVrrpPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
#endif /*L7_ROUTING_PACKAGE*/


#ifdef L7_IPV6_PACKAGE
  #ifdef L7_MCAST_PACKAGE
     /* debug all mcast components */
  depth3 = ewsCliAddNode(depth2,pStrInfo_common_Diffserv_5,pStrInfo_base_DebugIPv6 , NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  buildTreePrivledgedDebugMcast6(depth3);
  #endif
#endif

#ifdef L7_ISDP_PACKAGE
  /* debug isdp ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Isdp, pStrInfo_base_DebugIsdp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug isdp packet */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugIsdpPkt, commandDebugIsdpPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, pStrInfo_base_DebugIsdpRxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, pStrInfo_base_DebugIsdpTxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif /* L7_ISDP_PACKAGE */


  /* debug lacp ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lacp_1, pStrInfo_base_DebugLacp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug lacp packet */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugLacp, commandDebugLacpPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNOOPING_COMPONENT_ID,
                               L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* debug Mld-snooping ... */
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mldsnooping, CLIDEBUGSNOOP_HELP(L7_AF_INET6), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    /* debug snooping packet */
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, CLIDEBUGSNOOPPACKET_HELP(L7_AF_INET6), commandDebugSnoopPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, CLIDEBUGSNOOPRXPACKET_HELP(L7_AF_INET6), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, CLIDEBUGSNOOPTXPACKET_HELP(L7_AF_INET6), NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* debug routing ospf v1 */
  #ifdef L7_ROUTING_PACKAGE
  /* debug routing */
  buildTreePrivledgedDebugRouting(depth2);
  #endif /*L7_PACKAGE_ROUTING*/

  #ifdef L7_IPV6_PACKAGE
  /* debug ospfv3 ... */
  buildTreePrivledgedDebugIpv6(depth2);
  #endif /*L7_IPV6_PACKAGE*/

  /* debug ping ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Ping_3, pStrInfo_base_DebugPing, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug ping packet */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugPingPkt, commandDebugPingPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_SFLOW_PACKAGE
  /* debug sflow */
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    depth3 = ewsCliAddNode(depth2, pStrinfo_common_SflowOption, pStrInfo_base_DebugsFlow, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    /* debug sflow packet */
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pkt, pStrInfo_base_DebugsFlowPckt, commandDebugsFlowPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
#endif

  /* debug spanning-tree ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_SpanTree_2, pStrInfo_base_DebugSpanTree, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug spanning-tree packet */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclBpdusStr, pStrInfo_base_DebugSpanTreePkt, commandDebugSpanningTreePacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Receive_1, pStrInfo_base_DebugSpanTreeRxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Tx_1, pStrInfo_base_DebugSpanTreeTxPkt, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the show debugging commands tree
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
void buildTreePrivShowDebugging(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Debugging, pStrInfo_base_ShowDebug, commandShowDebugging, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
