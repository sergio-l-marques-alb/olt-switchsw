/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cli_prompt_router.c
 *
 * @purpose Command mode functions
 *
 * @component user interface
 *
 * @comments
 *
 * @create   18/08/2003
 *
 * @author   Samip
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "cli_mode.h"

/*********************************************************************
*
* @purpose Function to set or exit from the router config(ospf)mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliOSPRouterConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt(index+1, pStrInfo_common_RtrCfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_ROUTER_CONFIG_OSPF_MODE));
}

/*********************************************************************
*
* @purpose Function to set or exit from the router config(ospf)mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliIpv6OspfConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt(index+1, pStrInfo_routing_Ipv6CfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_ROUTER_CONFIG_OSPFV3_MODE));
}

/*********************************************************************
*
* @purpose Function to set or exit from the router config(rip)mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliRIPRouterConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt(index+1, pStrInfo_common_RtrCfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_ROUTER_CONFIG_RIP_MODE));
}
