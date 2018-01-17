
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_clear_dai.c
*
* @purpose Dynamic ARP Inspection show commands for the cli
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create  09/29/2007
*
* @author  Kiran Kumar Kella
*
* @end
*
*********************************************************************/

#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "datatypes.h"
#include "clicommands_card.h"
#include "clicommands_dai.h"
#include "usmdb_dai_api.h"


/*********************************************************************
* @purpose  Clears Dynamic ARP Inspection statistics 
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Privileged EXEC mode
*
* @cmdsyntax for normal command: clear ip arp inspection statistics
*
* @cmdsyntax for no command: NA
*
* @cmdhelp
*
* @cmddescript 
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearDaiStats(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ClearDaiStats);
  }
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    usmDbDaiVlanStatsClear();
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

