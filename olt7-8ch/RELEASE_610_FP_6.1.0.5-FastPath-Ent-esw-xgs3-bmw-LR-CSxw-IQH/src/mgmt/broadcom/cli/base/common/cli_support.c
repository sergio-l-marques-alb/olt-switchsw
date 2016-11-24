/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_support.c
 *
 * @purpose config support commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  08/05/2006
 *
 * @author  Jayendra Thyagarajan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "usmdb_log_api.h"
#include "ews.h"
#include "cliutil.h"
#include "cliapi.h"
#include "cli_web_mgr_api.h"
#include "usmdb_support.h"
#include "clicommands_support.h"

/*********************************************************************
*
* @purpose  Disable debug event generation for "support" functionality.
*
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
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: support clear
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportClear(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                    index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  usmDbSupportDebugClear();

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To save trace configuration to non-volatile storage
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  debug
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportSave(EwsContext ewsContext,
                                   L7_uint32 argc,
                                   const L7_char8 * * argv,
                                   L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SupportSave_1);
  }

  /* Invoke all registered save routines */
  if (usmDbSupportDebugSave() != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SupportSaveFailed);
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable/Disable debug event generation on the console.
*
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
* @notes This command differs from "debug console" in that it allows
*        for the redirection of syslog messages, sysapiPrintf, and
*        stdout to enabled sessions. As such, it is intended to be
*        invoked only by authorized users.
*
*
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: support console
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportConsole(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                      index)
{
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NO_CMD)
  {
    rc = cliWebSupportConsoleDisplayModeSet (L7_FALSE, cliUtil.handleNum);
    /* Reset the Redirection so that the dev shell command doesn't need
     * a seperate command to enable redirection
     */
    rc = cliWebDebugTraceDisplayModeSet (L7_FALSE, cliCurrentHandleGet());
    
  }
  else
  {
    rc = cliWebSupportConsoleDisplayModeSet (L7_TRUE, cliUtil.handleNum);
    /* Set the Redirection so that the dev shell command doesn't need
     * a seperate command to enable redirection
     */
    rc = cliWebDebugTraceDisplayModeSet (L7_TRUE, cliCurrentHandleGet());
  }  
    

  /*************Set Flag for Script Successful******/
  if (rc == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Get System Snapshot from debug routines.
*
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
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: support console
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportSnapshotSystem(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                             index)
{
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  rc = usmDbSupportDebugCategoryDump (SUPPORT_CATEGORY_SYSTEM);

  /*************Set Flag for Script Successful******/
  if (rc == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Get Routing Snapshot from debug routines.
*
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
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: support console
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportSnapshotRouting(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                              index)
{
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  rc = usmDbSupportDebugCategoryDump (SUPPORT_CATEGORY_ROUTING);

  /*************Set Flag for Script Successful******/
  if (rc == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}


/*********************************************************************
*
* @purpose  Get Multicast Snapshot from debug routines.
*
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
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: support console
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportSnapshotMulticast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                              index)
{
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  rc = usmDbSupportDebugCategoryDump (SUPPORT_CATEGORY_IPMCAST);

  /*************Set Flag for Script Successful******/
  if (rc == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

