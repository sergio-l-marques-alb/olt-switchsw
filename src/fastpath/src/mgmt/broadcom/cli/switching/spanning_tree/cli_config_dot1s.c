/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/cli_config_dot1s.c
 *
 * @purpose Spanning Tree config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/29/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "dot1s_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "datatypes.h"
#include "clicommands_dot1s.h"
#include "clicommands_card.h"
#include "cliutil_dot1s.h"

#include "ews.h"

/*********************************************************************
 *
 * @purpose  Specifies the revision for the configuration
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
 * @cmdsyntax  [no] spanning-tree configuration revision <0-65535>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeConfigurationRevision(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argRevision = 1;
  L7_uint32 revision = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 ||
        cliConvertTo32BitUnsignedInteger(argv[index+argRevision],
          &revision) != L7_SUCCESS ||
        revision > L7_DOT1S_MAX_REVISION_VALUE ||
        revision < L7_DOT1S_MIN_REVISION_VALUE )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sCfgRevision,
          L7_DOT1S_MIN_REVISION_VALUE, L7_DOT1S_MAX_REVISION_VALUE );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoCfgRevision);
    }
    revision=L7_DOT1S_MIN_REVISION_VALUE;
  }
  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sConfigRevisionSet( unit, revision ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sCfgRevision);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specifies the name for the configuration
 * @The command implements the 'normal' and 'no' functionality of the command
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
 * @cmdsyntax  spanning-tree configuration name <name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeConfigurationName(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argName = 1;
  L7_char8 name[L7_DOT1S_MAX_CONFIG_NAME];
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */

  memset (name, 0,sizeof(name));

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sCfgName);
    }

    if (strlen(argv[index+argName]) > (DOT1S_MAX_CONFIG_NAME_SIZE))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sCfgNameLen, (DOT1S_MAX_CONFIG_NAME_SIZE));
    }
    OSAPI_STRNCPY_SAFE(name,argv[index + argName]);

    if ( cliIsPrintable( name, strlen(name) ) == L7_FAILURE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sCfgNamePrint);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoCfgName);
    }

    memset (name, 0,sizeof(name));
    rc = usmDbSwDevCtrlBurnedInMacAddrGet(unit,mac);
    osapiSnprintf(name, sizeof(name), "%02X-%02X-%02X-%02X-%02X-%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sConfigNameSet( unit, name ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sCfgName);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify whether a port is an edge port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @return cliPrompt(ewsContext)
 *
 * @note
 *
 * @cmdsyntax  [no] spanning-tree cst port edgeport
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeCstPortEdgePort(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortEdgePort_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoPortEdgePort);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode =  L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sCistPortEdgeSet(unit, iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sEdgePort);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify whether a port is an Auto edge
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @return cliPrompt(ewsContext)
 *
 * @note
 *
 * @cmdsyntax  [no] spanning-tree auto-edge
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeAutoEdge(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sAutoEdge);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoAutoEdge);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode =  L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sPortAutoEdgeSet(unit, iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sAutoEdge);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify a port for root guard
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @return cliPrompt(ewsContext)
 *
 * @note
 *
 * @cmdsyntax  [no] spanning-tree rootguard
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeRootGuard(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sRootGuard);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoRootGuard);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode =  L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sPortRestrictedRoleSet(unit, iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sRootGuard);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify a guard for port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @return cliPrompt(ewsContext)
 *
 * @note
 *
 * @cmdsyntax  [no] spanning-tree guard [loop | none | root]
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreePortGuard(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL oldLoopGuardMode, oldRootGuardMode;
  L7_BOOL newLoopGuardMode = L7_FALSE, newRootGuardMode = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortGuard);
    }

    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortGuard);
    }
    OSAPI_STRNCPY_SAFE(strMode, argv[index + argMode]);
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_switching_PortguardLoop) == 0)
    {                                              /* "loop"*/
      newLoopGuardMode = L7_TRUE;
      newRootGuardMode = L7_FALSE;
    }
    else if (strcmp(strMode, pStrInfo_common_None_3) == 0)
    {                                              /* "none"*/
      newLoopGuardMode = L7_FALSE;
      newRootGuardMode = L7_FALSE;
    }
    else if (strcmp(strMode, pStrInfo_switching_PortguardRoot) == 0)
    {                                              /* "root"*/
      newLoopGuardMode = L7_FALSE;
      newRootGuardMode = L7_TRUE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoPortGuard);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoPortGuard);
    }
      newLoopGuardMode = L7_FALSE;
      newRootGuardMode = L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sPortLoopGuardGet(unit, iface, &oldLoopGuardMode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortGuard);
        continue;
      }
      if ( usmDbDot1sPortRestrictedRoleGet(unit, iface, &oldRootGuardMode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortGuard);
        continue;
      }

      if ( newLoopGuardMode != oldLoopGuardMode )
      {
        if ( (newLoopGuardMode == L7_TRUE) && (oldRootGuardMode == L7_TRUE) )
        {
          oldRootGuardMode = L7_FALSE;
          if ( usmDbDot1sPortRestrictedRoleSet(unit, iface, oldRootGuardMode) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortGuard);
            continue;
          }
        }

        if ( usmDbDot1sPortLoopGuardSet(unit, iface, newLoopGuardMode) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortGuard);
          continue;
        }
      }

      if ( newRootGuardMode !=  oldRootGuardMode )
      {
        if ( usmDbDot1sPortRestrictedRoleSet(unit, iface, newRootGuardMode) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortGuard);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}


/*********************************************************************
 *
 * @purpose  Specify a port for tcn guard
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @return cliPrompt(ewsContext)
 *
 * @note
 *
 * @cmdsyntax  [no] spanning-tree tcnguard
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeTcnGuard(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sTcnGuard);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNot);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode =  L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sPortRestrictedTcnSet(unit, iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sTcnGuard);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify the port pathcost
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
 * @cmdsyntax  [no] spanning-tree cst port pathcost <pathcost|auto>
 *
 * @cmdhelp Set spanning tree mode on a per-port basis.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeCstPortPathCost(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argCost = 3;
  L7_uint32 s, p, iface;
  L7_char8 strPathCost[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 cost = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Check for value or Auto */
    if (strlen(argv[index+argCost]) >= sizeof(strPathCost))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortPathCost,
          L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST );
    }

    OSAPI_STRNCPY_SAFE(strPathCost, argv[index+argCost]);
    cliConvertToLowerCase(strPathCost);

    if (strcmp(strPathCost, pStrInfo_common_Auto2 /* "auto" */ ) == 0)
    {
      cost = L7_DOT1S_AUTO_PORT_PATHCOST;
    }
    else if (cliConvertTo32BitUnsignedInteger(argv[index+argCost],
          &cost) != L7_SUCCESS ||
        cost > L7_DOT1S_MAX_PORT_PATHCOST ||
        cost < L7_DOT1S_CONFIGURABLE_PATHCOST_MIN )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortPathCost,
          L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    cost = FD_DOT1S_PORT_PATH_COST;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sMstiPortPathCostSet(unit, L7_DOT1S_CIST_INSTANCE,
            iface, cost) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPathCost);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify the port priority
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
 * @cmdsyntax   [no] spanning-tree cst port priority  <0-240>
 *
 * @cmdhelp Set spanning tree priority on a per-port basis.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeCstPortPriority(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;

  L7_uint32 argPriority = 3;
  L7_uint32 priority = 0;
  L7_uint32 userPriority = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argPriority],
          &priority) != L7_SUCCESS ||
        priority > L7_DOT1S_PORT_PRIORITY_MAX ||
        priority < L7_DOT1S_PORT_PRIORITY_MIN)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortPri,
          L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX );
    }
  }

  userPriority = priority;
  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (ewsContext->commType == CLI_NO_CMD)
      {
        priority = usmDbDot1sDefaultPortPriorityGet(unit,iface);
        userPriority = priority;
      }

      if ( usmDbDot1sMstiPortPrioritySet(unit, L7_DOT1S_CIST_INSTANCE,
            iface, &priority) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPri);
        continue;
      }

      if (userPriority != priority)
      {
        memset ( buf, 0, sizeof(buf));
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        ewsTelnetPrintf (ewsContext, pStrInfo_switching_Dot1sBridgePriInfo, priority );
        cliSyntaxBottom(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specifies the spanning tree protocol version to use
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
 * @cmdsyntax  [no] spanning-tree forceversion {802.ld | 802.lw | 802.ls}
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeForceVersion(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argVersion = 1;
  L7_char8 strVersion[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 version=0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sForceVer);
    }

    if (strlen(argv[index+argVersion]) >= sizeof(strVersion))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sForceVer);
    }
    OSAPI_STRNCPY_SAFE(strVersion,argv[index + argVersion]);
    cliConvertToLowerCase(strVersion);

    if (strcmp(strVersion, pStrInfo_switching_Dot1d_1) == 0)
    {                                              /* "802.1d"*/
      version = DOT1S_FORCE_VERSION_DOT1D;
    }
    else if (strcmp(strVersion, pStrInfo_switching_Dot1w) == 0)
    {                                              /* "802.1w"*/
      version = DOT1S_FORCE_VERSION_DOT1W;
    }
    else if (strcmp(strVersion, pStrInfo_switching_Dot1s) == 0)
    {                                              /* "802.1s"*/
      version = DOT1S_FORCE_VERSION_DOT1S;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sForceVer);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoForceVer);
    }
    version = FD_DOT1S_FORCE_VERSION;

  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sForceVersionSet(unit, version ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sForceVer);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specifies the bridge forward delay parameter for the cist
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
 * @cmdsyntax  [no] spanning-tree forward-time <4-30>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeForwardTime(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argForwardDelay = 1;
  L7_uint32 forwardDelay = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sBridgeForwardDelay,
          L7_DOT1S_BRIDGE_FORWARDDELAY_MIN, L7_DOT1S_BRIDGE_FORWARDDELAY_MAX );
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argForwardDelay],
          &forwardDelay) != L7_SUCCESS ||
        forwardDelay > L7_DOT1S_BRIDGE_FORWARDDELAY_MAX ||
        forwardDelay < L7_DOT1S_BRIDGE_FORWARDDELAY_MIN )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf), pStrErr_switching_CfgDot1sBridgeForwardDelay,
          L7_DOT1S_BRIDGE_FORWARDDELAY_MIN, L7_DOT1S_BRIDGE_FORWARDDELAY_MAX );
      ewsTelnetWrite( ewsContext, buf);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeForwardDelayGreaterMaxAge);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sNoBridgeForwardDelay);
    }
    forwardDelay = FD_DOT1S_BRIDGE_FWD_DELAY;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {

    if (usmDbDot1sCistBridgeFwdDelaySet( unit, forwardDelay ) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBridgeForwardDelay);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeForwardDelayGreaterMaxAge);
    }
    osapiSleepMSec(1);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
 *
 * @purpose  Specifies the bridge maxage parameter for the cist
 * @ This command implements the 'no' and 'normal' for of commands
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
 * @cmdsyntax  [no] spanning-tree max-age <6-40>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeMaxAge(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMaxAge = 1;
  L7_uint32 maxAge = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sBridgeMaxAge,
          L7_DOT1S_BRIDGE_MAXAGE_MIN, L7_DOT1S_BRIDGE_MAXAGE_MAX );
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMaxAge],
          &maxAge) != L7_SUCCESS ||
        maxAge > L7_DOT1S_BRIDGE_MAXAGE_MAX ||
        maxAge < L7_DOT1S_BRIDGE_MAXAGE_MIN )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf), pStrErr_switching_CfgDot1sBridgeMaxAge,
          L7_DOT1S_BRIDGE_MAXAGE_MIN, L7_DOT1S_BRIDGE_MAXAGE_MAX );
      ewsTelnetWrite( ewsContext, buf);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeMaxAgeLesserForwardDelay);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sNoBridgeMaxAge);
    }
    maxAge = FD_DOT1S_BRIDGE_MAX_AGE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  osapiSleepMSec(1);

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sCistBridgeMaxAgeSet( unit, maxAge ) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBridgeMaxAge );
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeMaxAgeLesserForwardDelay);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Create a multiple spanning tree (mst) instance
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
 * @cmdsyntax  [no] spanning-tree mst instance <mstid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeMstInstance(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;
  L7_uint32 mstid;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 1 )
  {
    memset (buf, 0, sizeof(buf));
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sMstInst);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoMstInst);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if ( cliConvertTo32BitUnsignedInteger(argv[index+argMstid],
        &mstid) != L7_SUCCESS ||
      mstid > L7_DOT1S_MSTID_MAX ||
      mstid < L7_DOT1S_MSTID_MIN )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1sMstiCreate(unit, mstid) != L7_SUCCESS )
      {
        /*************Set Flag for Script Successful******/
        /********** Script should not fail if the instance already exists **************************/

        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sMstCreate);

      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1sMstiDelete(unit, mstid) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sMstDel);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specifies the priority parameter for a mst instance
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
 * @cmdsyntax  [no] spanning-tree mst priority <mstid> <0-61440>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeMstPriority(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;
  L7_uint32 mstid;

  L7_uint32 argPriority = 2;
  L7_uint32 priority;
  L7_uint32 numArg;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 2 ||
        cliConvertTo32BitUnsignedInteger(argv[index+argPriority],
          &priority) != L7_SUCCESS ||
        priority > L7_DOT1S_PRIORITY_MAX ||
        priority < L7_DOT1S_PRIORITY_MIN )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sMstPri,
          L7_DOT1S_PRIORITY_MIN, L7_DOT1S_PRIORITY_MAX );
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) == L7_SUCCESS )
    {
      if (mstid == DOT1S_CIST_ID)
      {
        /* Invoke function commandSpanningTreePriority() */
        return commandSpanningTreePriority(ewsContext,argc,argv,index);
      }
      else
      {
        if (mstid > L7_DOT1S_MSTID_MAX || mstid < L7_DOT1S_MSTID_MIN)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoMstPri);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) == L7_SUCCESS )
    {
      if (mstid == DOT1S_CIST_ID)
      {
        /* Invoke function commandSpanningTreePriority() */
        return commandSpanningTreePriority(ewsContext,argc,argv,index);
      }
      else
      {
        if (mstid > L7_DOT1S_MSTID_MAX || mstid < L7_DOT1S_MSTID_MIN)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
    }
    priority =  FD_DOT1S_BRIDGE_PRIORITY;
  }
  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    cliSpanTreeSetInstancePriority( ewsContext, mstid, priority );
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Add and deletes a VLAN to a mst instance
 * @ The function implements the 'no' and 'normal' for of the command
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
 * @cmdsyntax  [no] spanning-tree mst vlan <mstid> <vlan-list>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeMstVlan(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;
  L7_uint32 mstid;

  L7_uint32 argVlanList = 2;
  L7_int32  vlanID = 0, i;
  L7_char8 *strVlanList;
  L7_uint32 vlanCount = 0;
  L7_uint32 *vlanList;

  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 prevMstid;
  L7_uint32 numArg;
  L7_BOOL   flag = L7_TRUE;
  L7_VLAN_MASK_t vlan_mask;
  L7_uint32* prev_mstid;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 2 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sMstVlanAdd);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoMstVlanAdd);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) != L7_SUCCESS ||
      mstid > L7_DOT1S_MSTID_MAX )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
  }

  strVlanList = (L7_char8 *) argv[index + argVlanList];

  vlanList = (L7_uint32 *)osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(L7_uint32)*L7_MAX_VLANS);
    
  if (vlanList == L7_NULLPTR)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_AllocateMemoryToPerformOperation);
  }
  
  prev_mstid = (L7_uint32*)osapiMalloc(L7_CLI_WEB_COMPONENT_ID,sizeof(L7_uint32)*L7_MAX_VLANS + 1);

  if (prev_mstid == L7_NULLPTR)
  {
    osapiFree(L7_CLI_WEB_COMPONENT_ID, vlanList);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_AllocateMemoryToPerformOperation);
  }
  
  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiFree(L7_CLI_WEB_COMPONENT_ID, vlanList);
    osapiFree(L7_CLI_WEB_COMPONENT_ID, prev_mstid);
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, buf, sizeof(buf), pStrInfo_switching_VlanIdOutOfRange_1, L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, buf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  memset(&vlan_mask,0x0,sizeof(vlan_mask)); 
  memset(prev_mstid,0x0,sizeof(L7_uint32)*L7_MAX_VLANS + 1); 
  
  /*prepare the vlan mask to be sent*/
  for (i = 0; i < vlanCount; i++)
  {
    vlanID = vlanList[i];

    if ( vlanID > L7_DOT1Q_MAX_VLAN_ID || vlanID < L7_DOT1S_MIN_VLAN_ID )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, sizeof(buf), pStrErr_switching_Dot1sVlan, L7_DOT1S_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, buf);
      flag = L7_FALSE;
      continue;
    }
      
    L7_VLAN_SETMASKBIT(vlan_mask,vlanID);
     
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {

      /* Determine the previous mstid associated with this vlan for informational message */
      if (usmDbDot1sVlanToMstiConfiguredGet(unit, vlanID, &prevMstid) == L7_SUCCESS)
      {

        if (prevMstid != L7_DOT1S_CIST_INSTANCE)
        {
          /* Set prevMstid to CIST to handle as if associated with CIST.  No informational
                   message will be displayed. */
          prev_mstid[vlanID] = prevMstid;

        }
        else
        {
          prev_mstid[vlanID] = L7_DOT1S_CIST_INSTANCE;
        }
      }
    }
  }  

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDot1sMstiVlanAdd(unit, mstid, &vlan_mask) != L7_SUCCESS )
      {
       osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, sizeof(buf), pStrErr_switching_Dot1sMstVlanAdd);
       ewsTelnetWrite(ewsContext, buf);
       flag = L7_FALSE;
      }
      else 
      {
        for (i = 0; i < vlanCount; i++)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, sizeof(buf), pStrInfo_switching_Dot1sMstVlanAddInfo, 
          vlanList[i], prev_mstid[vlanList[i]], mstid );
          /* If number of vlans high, emweb buffers getting burrested.So place them in system log */
          /*ewsTelnetWrite( ewsContext, buf); */
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID, buf);
     
        }  
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDot1sMstiVlanRemove(unit, mstid, &vlan_mask) != L7_SUCCESS )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, sizeof(buf), pStrErr_switching_Dot1sMstVlanRemove);
        ewsTelnetWrite(ewsContext, buf);
        flag = L7_FALSE;
      }
    }
  }
 
  osapiFree(L7_CLI_WEB_COMPONENT_ID, vlanList);
  osapiFree(L7_CLI_WEB_COMPONENT_ID,prev_mstid);
  /*************Set Flag for Script Successful******/
  if (flag == L7_TRUE)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  };

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable/Disable spanning tree support on a port
 * @This command is executed in the global config mode
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
 * @cmdsyntax  [no] spanningtree port mode all
 *
 * @cmdhelp Set spanning tree mode on a per-port basis.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreePortModeAll(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 iface;
  L7_uint32 mode = L7_ENABLE;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL allPorts = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF + USM_LAG_INTF;
  L7_uint32 interfaceCount = 0;
  L7_uint32 exclIntfTypes = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortMode_1);
    }
    if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoPortMode);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argSlotPort]) >= sizeof(strSlotPort))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortMode_1);
    }

    OSAPI_STRNCPY_SAFE(strSlotPort, argv[index+argSlotPort]);
    cliConvertToLowerCase(strSlotPort);

    if (strcmp(strSlotPort, pStrInfo_common_All) == 0 )
    {                                         /* "all" */
      allPorts = L7_TRUE;
    }

    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    allPorts = L7_TRUE;
  }

  if (allPorts == L7_TRUE)
  {
    /* interface = get first interface, if fail, exit */
    if ( usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
          &iface ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 0, 0, 0, pStrErr_common_Error, ewsContext,
          pStrErr_common_ServiceBeAddedRetryUsingSlotPort, cliSyntaxInterfaceHelp());
    }
  }
  else
  {
    if ( cliValidateDot1sInterface(ewsContext, unit, iface ) != L7_SUCCESS )
    {
      /* Invalid port */
      return cliPrompt(ewsContext);
    }
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    do
    {
      if ( usmDbDot1sPortStateSet(unit, iface, mode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortMode);
      }

      interfaceCount++;

      if (allPorts == L7_TRUE &&
          usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
            iface, &iface ) != L7_SUCCESS )
      {
        break;
      }

    } while (allPorts == L7_TRUE && interfaceCount < DOT1S_MAX_PORT_COUNT);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose  Enable/Disable spanning tree support on a port
 * @This command is executed in the interface config mode
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
 * @cmdsyntax  [no] spanningtree port mode <slot/port | all>
 *
 * @cmdhelp Set spanning tree mode on a per-port basis.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreePortModeInterface(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface;

  L7_uint32 mode = L7_DISABLE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sPortModeIntf);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoPortModeIntf);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sPortStateSet(unit, iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortMode);
        continue;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specifies the instance priority for the spanning tree
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
 * @cmdsyntax  [no] spanning-tree priority <0-61440>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreePriority(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argPriority = 2;
  L7_uint32 priority;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPriority], &priority) != L7_SUCCESS ||
        priority > L7_DOT1S_PRIORITY_MAX ||
        priority < L7_DOT1S_PRIORITY_MIN )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sBridgePri,
          L7_DOT1S_PRIORITY_MIN, L7_DOT1S_PRIORITY_MAX );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    priority = FD_DOT1S_BRIDGE_PRIORITY;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    cliSpanTreeSetInstancePriority( ewsContext, L7_DOT1S_CIST_INSTANCE, priority );
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable/Disable spanning tree support on the switch
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
 * @cmdsyntax  [no] spanning-tree
 *
 * @cmdhelp Display spanning tree values on a per-switch basis.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTree(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode = 0;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if ( cliNumFunctionArgsGet() != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sAdminMode);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoAdminMode);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sModeSet(unit, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sAdminMode);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable bpdu migration check on a port/all ports
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
 * @cmdsyntax  spanning-tree bpdumigrationcheck all (In global config mode)
 *
 * @cmdhelp Set spanning tree bpdu migration check on a per-port basis
 * or for all ports.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeBpdumigrationcheckAll(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{

  L7_uint32 iface;
  L7_BOOL allPorts = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF + USM_LAG_INTF;
  L7_uint32 interfaceCount = 0;
  L7_uint32 exclIntfTypes = 0;
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ( numArg != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortMigCheck, cliSyntaxInterfaceHelp());
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  allPorts = L7_TRUE;
  /* interface = get first interface, if fail, exit */
  if ( usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
        &iface ) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (0,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_common_ServiceBeAddedRetryUsingSlotPort,
        cliSyntaxInterfaceHelp());
  }

  do
  {
    if ( usmDbDot1sPortForceMigrationCheck(unit, iface) != L7_SUCCESS )
    {
      /* Avoid this and try for next*/
    }
    interfaceCount++;

    if (usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
          iface, &iface ) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }

  } while (allPorts == L7_TRUE && interfaceCount < DOT1S_MAX_PORT_COUNT);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable bpdu migration check on a port/all ports
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
 * @cmdsyntax  spanning-tree bpdumigrationcheck
 *
 * @cmdhelp Set spanning tree bpdu migration check on a per-port basis
 * or for all ports.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeBpdumigrationcheck(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{

  L7_uint32 slot, port, iface;
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 argSlotPort = 2;
  L7_RC_t rc;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( argc != 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortMigCheck, cliSyntaxInterfaceHelp());
  }

  if (strlen(argv[argSlotPort]) >= sizeof(strSlotPort))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowDot1sPortSummary);
  }

  OSAPI_STRNCPY_SAFE(strSlotPort, argv[argSlotPort]);
  cliConvertToLowerCase(strSlotPort);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(strSlotPort, &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
        cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext,pStrErr_switching_Dot1sPortMigCheckInvalidPort);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &iface) != L7_SUCCESS ||
        cliValidateDot1sInterface(ewsContext, unit, iface ) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }
  }

  if ( usmDbDot1sPortForceMigrationCheck(unit, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortMigCheck);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify the port pathcost / port priority for a particular mst instance
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
 *
 * @cmdsyntax  [no] spanningtree mst <mstid> {cost{<1-200000000>|auto}|port-priority<0-240>|external-cost{<1-200000000>|auto}}
 *
 * @cmdhelp Set pathcost/port for a port for a specific multiple spanning tree instance
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeMstPortPathCostPriority(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{

  /*
   * Command is if the form spanning-tree mst
   * <mstid> { cost { <1-200000000> | auto } | port-priority <0-240> | external-cost{<1-200000000>|auto}}
   *
   * The no form is
   * no spanning-tree mst <mstid> {cost | auto | external-cost}
   *
   */

  L7_uint32 argMstid = 1;
  L7_uint32 mstid = 0;
  L7_char8 strPathCost[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 s, p, iface;

  L7_uint32 argValue = 3;
  L7_uint32 cost= 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;
  L7_uint32 priority= 0;
  L7_uint32 userPriority = 0;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* There has to be 3 parameters */
    if ( cliNumFunctionArgsGet() != 3 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sMstPortPathCost_1,
          L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST,
          L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* There has to be 2 parameters */
    if ( cliNumFunctionArgsGet() != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sNoMstPortPathCost);
    }
  }

  /* Obtain the mstid value */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) == L7_SUCCESS)
  {
    if (mstid == DOT1S_CIST_ID)
    {
      /* This is the CST case */
      if (strcmp(argv[index+2],pStrInfo_switching_ExternalCost) == 0)
      {
        return commandSpanningTreeCstPortPathExternalCost(ewsContext,
            argc,
            argv,
            index);
      }
      else if (strcmp(argv[index+2],pStrInfo_common_Cost2) == 0)
      {
        return commandSpanningTreeCstPortPathCost(ewsContext,
            argc,
            argv,
            index);
      }
      else if (strcmp(argv[index+2],pStrInfo_switching_PortPri_1) <= 0)
      {
        return commandSpanningTreeCstPortPriority(ewsContext,
            argc,
            argv,
            index);
      }
    }
    else
    {
      if (mstid > L7_DOT1S_MSTID_MAX || mstid < L7_DOT1S_MSTID_MIN)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
  }

  if (strcmp(argv[index+2],pStrInfo_common_Cost2) <= 0)
  {
    /* This is the cost token */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* Check for value or Auto */
      if (strlen(argv[index+argValue]) >= sizeof(strPathCost))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sMstPortPathCost_1,
            L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST
            ,L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX);
      }

      OSAPI_STRNCPY_SAFE(strPathCost, argv[index+argValue]);
      cliConvertToLowerCase(strPathCost);

      if (strcmp(strPathCost, pStrInfo_common_Auto2 /* "auto" */ ) == 0)
      {
        cost = L7_DOT1S_AUTO_PORT_PATHCOST;
      }
      else if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
            &cost) != L7_SUCCESS ||
          cost > L7_DOT1S_MAX_PORT_PATHCOST ||
          cost < L7_DOT1S_CONFIGURABLE_PATHCOST_MIN )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sMstPortPathCost_1,
            L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST,
            L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX);
      }
      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if ( usmDbDot1sMstiPortPathCostSet(unit, mstid, iface, cost) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPathCost);
            continue;
          }
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /* By default, the value should be cost */
      cost = L7_DOT1S_AUTO_PORT_PATHCOST;

      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if ( usmDbDot1sMstiPortPathCostSet(unit, mstid, iface, cost) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPathCostNo);
            continue;
          }
        }
      }
    }
  }
  else if (strcmp(argv[index+2],pStrInfo_switching_PortPri_1) <= 0)
  {
    /* This is the port-priority token */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
            &priority) != L7_SUCCESS ||
          priority > L7_DOT1S_PORT_PRIORITY_MAX ||
          priority < L7_DOT1S_PORT_PRIORITY_MIN )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sMstPortPathCost_1,
            L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST,
            L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX);
      }
      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        userPriority = priority;

        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if( usmDbDot1sMstiPortPrioritySet(unit, mstid, iface, &priority) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPri);
            continue;
          }
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {

      userPriority = priority;

      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if (ewsContext->commType == CLI_NO_CMD)
          {
            priority = usmDbDot1sDefaultPortPriorityGet(unit,iface);
            userPriority = priority;
          }

          if( usmDbDot1sMstiPortPrioritySet(unit, mstid, iface, &priority) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPriNo);
            continue;
          }
        }
      }
    }
    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (userPriority != priority)
      {
        memset ( buf, 0, sizeof(buf));
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        ewsTelnetPrintf (ewsContext, pStrInfo_switching_Dot1sBridgePriInfo, priority );
        cliSyntaxBottom(ewsContext);
      }
    }
  }
  else
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf), pStrErr_switching_CfgDot1sMstPortPathCost_1,
        L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST,
        L7_DOT1S_PORT_PRIORITY_MIN, L7_DOT1S_PORT_PRIORITY_MAX);
    ewsTelnetWrite( ewsContext, buf);
    cliSyntaxBottom(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set or Reset the MSTP Max Hop count parameter for the device.
 * @This command is executed in the global config mode
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
 * @cmdsyntax  [no] spanning-tree max-hops <max-hops>
 *
 * @cmdhelp sets the MSTP Max Hop count parameter for the device.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeMaxHops(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMaxHop = 1;
  L7_uint32 maxHop = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sBridgeMaxHop);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMaxHop],
          &maxHop) != L7_SUCCESS ||
        maxHop > L7_DOT1S_BRIDGE_MAXHOP_MAX ||
        maxHop < L7_DOT1S_BRIDGE_MAXHOP_MIN )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf), pStrErr_switching_CfgDot1sBridgeMaxHop);
      ewsTelnetWrite( ewsContext, buf);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeMaxHopLesser128);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sNoBridgeMaxHop);
    }
    maxHop = FD_DOT1S_BRIDGE_MAX_HOP;
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sBridgeMaxHopSet(unit, maxHop) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBridgeMaxHop );
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeMaxHopLesser128);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set or Reset the MSTP Max Hop count parameter for the device.
 * @This command is executed in the global config mode
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
 * @cmdsyntax  [no] spanning-tree max-hops <max-hops>
 *
 * @cmdhelp sets the MSTP Max Hop count parameter for the device.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeHoldCount(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argHoldCount = 1;
  L7_uint32 holdCount = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sBridgeHoldCount);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argHoldCount],
          &holdCount) != L7_SUCCESS ||
        holdCount > L7_DOT1S_BRIDGE_HOLDCOUNT_MAX ||
        holdCount < L7_DOT1S_BRIDGE_HOLDCOUNT_MIN )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf), pStrErr_switching_CfgDot1sBridgeHoldCount);
      ewsTelnetWrite( ewsContext, buf);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeHoldCount);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sNoBridgeHoldCount);
    }
    holdCount = FD_DOT1S_BRIDGE_TX_HOLD_COUNT;
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sBridgeTxHoldCountSet(unit, holdCount) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBridgeHoldCount_1 );
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_Dot1sBridgeHoldCount);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Specify the external MSTP Port Path Cost in a specific mst instance for the device
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
 * @cmdsyntax  [no] spanningtree mst <mstid> {external-cost <1-2000000000> | auto}
 *
 * @cmdhelp Sets the external MSTP Port Path Cost for a specific multiple spanning tree instance
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSpanningTreeCstPortPathExternalCost(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argCost = 3;
  L7_uint32 s, p, iface;
  L7_char8 strPathCost[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 cost = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    /* Check for value or Auto */
    if (strlen(argv[index+argCost]) >= sizeof(strPathCost))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortPathCost,
          L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST );
    }
    OSAPI_STRNCPY_SAFE(strPathCost, argv[index+argCost]);
    cliConvertToLowerCase(strPathCost);

    if (strcmp(strPathCost, pStrInfo_common_Auto2 /* "auto" */ ) == 0)
    {
      cost = L7_DOT1S_AUTO_PORT_PATHCOST;
    }
    else if (cliConvertTo32BitUnsignedInteger(argv[index+argCost],
          &cost) != L7_SUCCESS ||
        cost > L7_DOT1S_MAX_PORT_PATHCOST ||
        cost < L7_DOT1S_CONFIGURABLE_PATHCOST_MIN )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgDot1sPortPathCost,
          L7_DOT1S_CONFIGURABLE_PATHCOST_MIN, L7_DOT1S_MAX_PORT_PATHCOST );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    cost = FD_DOT1S_PORT_PATH_COST;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sCistPortExternalPathCostSet(unit,
            iface, cost) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sPortPathCost);
        continue;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To enable BPDU Guard protection on a switch
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
 * @cmdsyntax  [no] spanning-tree bpduguard
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeBpduGuard(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sBpduGuard);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoBpduGuard);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sBpduGuardSet(unit,mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBpduGuard);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable BPDU filter on the switch.
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
 * @cmdsyntax  [no] spanning-tree bpduguard default
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeBpduFilterDefault(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 numArg,unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sBpduFilter);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoBpduFilter);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1sBpduFilterSet(unit,mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBpduFilter);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable BPDU filter on the interface.
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
 * @cmdsyntax  [no] spanning-tree bpduguard
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeIntfBpduFilter(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface,unit;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sBpduGuard);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoBpduGuard);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sIntfBpduFilterSet(unit,iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBpduFilter);
        continue;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enable BPDU flood on the interface.
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
 * @cmdsyntax  [no] spanning-tree bpduflood
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpanningTreeIntfBpduFlood(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 s, p, iface,unit;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ( numArg != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sIntfBpdu);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgDot1sNoIntfBpdu);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbDot1sIntfBpduFloodSet(unit,iface, mode) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBpdu);
        continue;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
