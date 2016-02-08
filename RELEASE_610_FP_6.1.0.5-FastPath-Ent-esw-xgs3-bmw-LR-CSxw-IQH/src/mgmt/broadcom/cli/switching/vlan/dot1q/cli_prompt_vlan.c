/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_prompt_vlan.c
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
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "cli_mode.h"

/*********************************************************************
*
* @purpose Function to set or exit from the vlan database mode.
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
const L7_char8 *cliVlanMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliVlanPrompt(index+1, pStrInfo_switching_VlanModePrompt,pStrInfo_common_PriviledgeUsrExecModePrompt, argc, argv, ewsContext);

}
