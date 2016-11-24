/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/ospf/clicommands_debug_ospf.c
*
* @purpose Debug commands for OSPF component
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
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "clicommands_debug_l3.h"

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
void buildTreePrivledgedDebugRoutingOspf(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  /* debug ospf ... */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Ospf_1, pStrInfo_routing_DebugOspf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug ospf packet */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pkt, pStrInfo_routing_DebugOspfPkt, commandDebugOspfPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


