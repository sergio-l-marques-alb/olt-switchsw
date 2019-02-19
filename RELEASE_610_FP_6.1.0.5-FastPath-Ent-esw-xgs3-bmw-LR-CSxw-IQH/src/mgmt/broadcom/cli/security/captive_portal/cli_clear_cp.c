/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/security/captive_portal/cli_clear_cp.c
*
* @purpose  clear commands for Captive portal
*
* @component user interface
*
* @comments none
*
* @create  07/19/2007
*
* @author  rjain, rjindal
*
* @end
*
**********************************************************************/

#include "strlib_security_cli.h"
#include "cliapi.h"
#include "clicommands_cp.h"
#include "usmdb_cpdm_user_api.h"

/*********************************************************************
* @purpose    Delete all captive Portal users
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  clear captive-portal users
*
* @cmdhelp    Delete all Captive Portal users.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserDeleteAll(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPClearUsers);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if ((cliGetCharInputID() == CLI_INPUT_EMPTY) && (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING))
  {
    cliSetCharInputID(1, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_security_CPClearUsersCmd);
    return pStrInfo_security_CPClearUsersVerify;
  }
  else if ((tolower(cliGetCharInput()) == 'y') || (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbCpdmUserEntryPurge() != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPClearUsersError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      ewsTelnetWrite(ewsContext, pStrInfp_security_CPClearedUsers);
    }
  }
  else
  {
    if (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      ewsTelnetWrite(ewsContext, pStrInfp_security_CPUsersNotCleared);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

