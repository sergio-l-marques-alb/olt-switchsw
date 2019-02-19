/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_tree.c
 *
 * @purpose cli functions that allow users to move up the tree
 *
 * @component user interface
 *
 * @comments
 *
 * @create  08/10/2000
 *
 * @author  Forrest Samuels
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"

/*********************************************************************
*
* @purpose  used for all things typed in at the root
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliDepth(CLIROOT_DEPTH, CLIROOT_PROMPT, CLIROOT_UPPROMPT, argc, argv, ewsContext)
*
* @notes none
*
* @end
*
*********************************************************************/
const L7_char8 *cliRoot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLIROOT_DEPTH, pStrInfo_base_RootPrompt, pStrInfo_base_RootPrompt, argc, argv, ewsContext);
}
