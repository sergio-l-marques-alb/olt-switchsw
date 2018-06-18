/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/rip/clicommands_debug_rip.c
*
* @purpose Debug commands for RIP component
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
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
void buildTreePrivledgedDebugRoutingRip(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Rip3, pStrInfo_routing_DebugRip, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* debug rip packet */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Pkt, pStrInfo_routing_DebugRipPkt, commandDebugRipPacket, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


