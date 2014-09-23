/******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 ******************************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_isdp.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  21/11/2007
 *
 * @author  Rostyslav Ivasiv
 *
 * @end
 *
 *****************************************************************************/
#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"
#include "cliutil.h"
#include "usmdb_isdp_api.h"
#include "usmdb_util_api.h"
#include "strlib_base_cli.h"
#include "clicommands_card.h"

/*****************************************************************************
*
* @purpose  Enables/Disables ISDP Advertise V2 mode on the switch.
*
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
* @cmdsyntax  [no] isdp advertise-v2
*
* @cmdhelp  Enables/Disables ISDP Advertise V2 mode on the switch.
*
*
* @end
*
******************************************************************************/
const L7_char8 *commandIsdpAdvertiseV2(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(usmdbIsdpV2ModeSet(L7_ENABLE) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                    pStrErr_common_FailedToSet, ewsContext,
                                                pStrInfo_base_IsdpAdvV2Enabled);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(usmdbIsdpV2ModeSet(L7_DISABLE) == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                    pStrErr_common_FailedToSet, ewsContext,
                                               pStrInfo_base_IsdpAdvV2Disabled);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*****************************************************************************
*
* @purpose  Enables/Disables ISDP on the switch.
*
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
* @cmdsyntax  [no] isdp run
*
* @cmdhelp  Enables/Disables ISDP on the switch.
*
*
* @end
*
******************************************************************************/
const L7_char8 *commandIsdpRun(EwsContext ewsContext,
                               L7_uint32 argc,
                               const L7_char8 * * argv,
                               L7_uint32 index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(usmdbIsdpModeSet(L7_ENABLE) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                    pStrErr_common_FailedToSet, ewsContext,
                                                    pStrInfo_base_IsdpEnabled);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(usmdbIsdpModeSet(L7_DISABLE) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                    pStrErr_common_FailedToSet, ewsContext,
                                                    pStrInfo_base_IsdpDisabled);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Configures holdtime for ISDP packets that switch sends.
*
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
* @cmdsyntax  isdp holdtime <10 - 255>
*
* @cmdhelp Configures holdtime for ISDP packets that switch sends.
*
* @end
*
******************************************************************************/
const L7_char8 *commandIsdpHoldtime(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_uint32 pVal, numArg = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if(cliConvertTo32BitUnsignedInteger(argv[index + numArg], &pVal) ==
                                                                     L7_SUCCESS)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmdbIsdpHoldTimeSet(pVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                    pStrErr_common_FailedToSet, ewsContext,
                                                    pStrInfo_base_IsdpHoldtime);
      }
    }
  }
  else
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                         ewsContext, pStrErr_base_ConvertToInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Configures period of time between sending new ISDP packet.
*
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
* @cmdsyntax  isdp timer <5 - 254>
*
* @cmdhelp Configure period of time between sending new ISDP packet.
*
* @end
*
******************************************************************************/
const L7_char8 *commandIsdpTimer(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{
  L7_uint32 pVal, numArg = 1;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if(cliConvertTo32BitUnsignedInteger(argv[index + numArg], &pVal) ==
                                                                    L7_SUCCESS)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmdbIsdpTimerSet(pVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_FailedToSet, ewsContext, pStrInfo_base_IsdpTimer);
      }
    }
  }
  else
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                         ewsContext, pStrErr_base_ConvertToInt);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Enables/Disables isdp on a port.
*
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
* @cmdsyntax  [no] isdp enable
*
* @cmdhelp Enables/Disables isdp on a port..
*
* @end
*
******************************************************************************/
const L7_char8 *commandIntfIsdpEnable(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index)
{
  L7_uint32 unit,s,p;
  L7_uint32 interface = 0;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
            usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if(usmdbIsdpIntfModeSet(interface, L7_ENABLE) == L7_FAILURE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0,
                                      pStrErr_common_FailedToSet, ewsContext,
                                                      pStrInfo_base_IsdpEnabled);
        }
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
            usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if(usmdbIsdpIntfModeSet(interface, L7_DISABLE) == L7_FAILURE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0,
                                      pStrErr_common_FailedToSet, ewsContext,
                                                      pStrInfo_base_IsdpDisabled);
          continue;
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
