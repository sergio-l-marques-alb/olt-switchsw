/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_clear_dhcp_snooping.c
*
* @purpose DHCP snooping show commands for the cli
*
* @component DHCP snooping
*
* @comments none
*
* @create  4/3/2007
*
* @author  rrice
*
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "datatypes.h"
#include "clicommands_card.h"
#include "clicommands_dhcp_snooping.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_util_api.h"


/*********************************************************************
* @purpose  Clears DHCP snooping statistics 
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
* @cmdsyntax for normal command: clear ip dhcp snooping statistics
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
const L7_char8 *commandClearIpDhcpSnoopingStats(EwsContext ewsContext, L7_uint32 argc,
                                                const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ClearDhcpSnoopingStats);
  }
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    usmDbDsIntfStatsClear();
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Clears DHCP snooping bindings table 
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
* @cmdsyntax for normal command: clear ip dhcp snooping binding
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
const L7_char8 *commandClearIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 u, s, p;
  L7_uint32 numArg = cliNumFunctionArgsGet();

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ClearDhcpSnoopingBinding);
  }

  if (numArg == 1)
  {
    /* Get interface user specified */
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+1], &u, &s, &p) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      u = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+1], &s, 
                              &p, &intIfNum) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDsBindingClear(intIfNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error,  
                                            ewsContext, 
                                            "while clearing the DHCP snooping bindings");
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Clears DHCP L2 Relay statistics 
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
* @cmdsyntax for normal command: clear dhcp l2relay statistics
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
const L7_char8 *commandClearDhcpL2RelayStats(EwsContext ewsContext, L7_uint32 argc,
                                                const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum = 0;
  L7_uchar8 *showOption;
  L7_uint32 unit, slot, port, intfIndex;
  L7_BOOL intfOption = L7_FALSE;
  L7_uint32 numArg = cliNumFunctionArgsGet(),arg = 1;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_ClearDhcpL2RelayStats);
  }
  showOption = (L7_uchar8 *)(argv[index + arg]);
 /* Do all validations for each of the options in show command.*/
  if (osapiStrncmp(showOption, pStrInfo_base_all_1, sizeof(pStrInfo_base_all_1)) != L7_NULL)
  {
    if (cliIsStackingSupported() != L7_TRUE)
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, showOption, &slot,
                              &port, &intIfNum) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (cliValidSpecificUSPCheck(showOption, &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }
      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    intfOption = L7_TRUE;
  }


  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Clear all interface stats for 'all' option */
    if (intfOption != L7_TRUE)
    {
      for (intfIndex = 1; intfIndex < L7_MAX_INTERFACE_COUNT; intfIndex++)
      {
        usmDbDsL2RelayIntfStatsClear(intfIndex);
      }
    }
    else
    {
      usmDbDsL2RelayIntfStatsClear(intIfNum);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
#endif
