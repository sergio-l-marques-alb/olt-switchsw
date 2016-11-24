/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/routing/common/clicommands_debug_l3.c
*
* @purpose Debug commands for routing components
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
void buildTreePrivledgedDebugRouting(EwsCliCommandP depth1)
{
#ifdef L7_OSPF_PACKAGE
  /* OSPF Debug */
  buildTreePrivledgedDebugRoutingOspf(depth1);
#endif

#ifdef L7_RIP_PACKAGE
  /* RIP Debug */
  buildTreePrivledgedDebugRoutingRip(depth1);
#endif
}

