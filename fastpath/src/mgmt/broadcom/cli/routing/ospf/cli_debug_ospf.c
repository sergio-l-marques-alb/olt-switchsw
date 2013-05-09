/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/ospf/cli_debug_ospf.c
 *
 * @purpose Debug commands for CLI for OSPF
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/24/2006
 *
 * @author  Colin Verne
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "l7_common.h"
#include "usmdb_ospf_api.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_debug_l3.h"

/*********************************************************************
*
* @purpose  To configure ospf packet debug flag
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
* @cmdsyntax  [no] debug ospf packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugOspfPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_DebugOspfPkt_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_DebugOspfPktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */
  if (mode == L7_ENABLE)
  {
    rc = usmDbOspfPacketDebugTraceFlagSet(L7_TRUE);
  }
  else
  {
    rc = usmDbOspfPacketDebugTraceFlagSet(L7_FALSE);
  }

  if (rc == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugOspfPktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_DebugOspfPktDsbld);
    }
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

