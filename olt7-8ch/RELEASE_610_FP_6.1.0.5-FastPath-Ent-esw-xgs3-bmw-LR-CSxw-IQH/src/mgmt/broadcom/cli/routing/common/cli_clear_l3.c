/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2005
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename cli_clear_l3.c
*
* @purpose clear commands for the cli layer 3
*
* @component user interface
*
* @comments none
*
* @create  07/27/2001  
*
* @author  Jason Shaw 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "cliapi.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_relay_api.h"

/*********************************************************************
* @purpose  Clears IP helper statistics
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
* @cmdsyntax for normal command: clear ip helper statistics
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
const L7_char8 *commandClearIpHelperStatistics(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 0)
  {
    ewsTelnetWrite(ewsContext, "\r\nIncorrect input! Use 'clear ip helper statistics'.");
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbIpHelperStatisticsClear() != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, "\r\nFailed to clear IP helper statistics");
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
