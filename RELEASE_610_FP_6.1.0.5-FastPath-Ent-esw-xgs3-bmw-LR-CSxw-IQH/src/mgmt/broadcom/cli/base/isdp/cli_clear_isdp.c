/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename cli_clear_isdp.c
 *
 * @purpose isdp clear commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/12/2007
 *
 * @author  Rostyslav Ivasiv
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "cliutil.h"
#include "ews.h"
#include "strlib_base_cli.h"
#include "usmdb_isdp_api.h"

/*********************************************************************
*
* @purpose  Clear isdp counters(statistics).
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax clear isdp counters
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIsdpCounters(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY &&
              ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrIsdpCounters);
    return pStrInfo_base_VerifyClrAllIsdpCountersYesNo;
  }

  else if ( cliGetCharInputID() == 1 ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmdbIsdpTrafficCountersClear() != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0,
                        L7_NULLPTR, ewsContext, pStrErr_base_IsdpCounterNotClr);
        }
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                pStrErr_base_IsdpCounterNotClr);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt(ewsContext, "");
}

 /*********************************************************************
*
* @purpose  Clear all isdp neighbor entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax clear isdp table
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 * commandClearIsdpTable(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY &&
              ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrIsdpEntries);
    return pStrInfo_base_VerifyClrAllIsdpEntriesYesNo;
  }

  else if ( cliGetCharInputID() == 1 ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {

        if (usmdbIsdpNeighborTableClear() != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 0, 0, 0, L7_NULLPTR,
                                      ewsContext, pStrErr_base_IsdpTableNotClr);
        }
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                  pStrErr_base_IsdpTableNotClr);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt(ewsContext, "");
}
