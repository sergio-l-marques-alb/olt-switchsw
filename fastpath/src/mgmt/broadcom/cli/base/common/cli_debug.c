/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_debug.c
 *
 * @purpose Debug commands for CLI
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/14/2006
 *
 * @author  Colin Verne
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "voip_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_dhcp_client.h"

#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_debug.h"

#include "usmdb_support.h"
#include "cli_web_mgr_api.h"
#include "usmdb_iputil_api.h"
#ifdef L7_SFLOW_PACKAGE
#include "usmdb_sflow.h"
#endif
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "clicommands_voip.h"
#include "usmdb_qos_voip_api.h"
#endif
#ifdef L7_ISDP_PACKAGE
#include "usmdb_isdp_api.h"
#endif /* L7_ISDP_PACKAGE */

#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
#include "usmdb_dot3ah.h"
#endif
#endif
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
* @notes This command differs from "support console" in that it allows
*        only for the redirection of syslog messages to enabled sessions.
*        The command is visible to end users.
*        
*
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: debug console
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugConsole(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
                                    index)
{
  L7_int32 current_handle;
  L7_RC_t  rc = L7_FAILURE;
  L7_BOOL  mode;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  current_handle = cliCurrentHandleGet ();

  if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;    
  }
  else
  {
    mode = L7_TRUE;
  }

  rc = cliWebDebugTraceDisplayModeSet (mode, current_handle);

  if (rc == L7_SUCCESS)
  {
    if (mode == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugConsoleEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugConsoleDsbld);
    }

    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To configure lacp packet debug flag
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
* @cmdsyntax  [no] debug lacp packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugLacpPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg != 0)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugLacpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugLacpPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */
  if (mode == L7_ENABLE)
  {
    rc = usmDbDot3adPacketDebugTraceFlagSet(L7_TRUE);
  }
  else
  {
    rc = usmDbDot3adPacketDebugTraceFlagSet(L7_FALSE);

  }

  if (rc == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugLacpPktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugLacpPktDsbld);
    }
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To configure spanning-tree packet debug flag
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
* @cmdsyntax  [no] debug spanning-tree packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugSpanningTreePacket(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL txFlag,rxFlag;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugSpanTreePkt_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugSpanTreePktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(numArg == 1)
  {
    usmDbDot1sPacketDebugTraceFlagGet(&txFlag,&rxFlag);
    if(strcmp(argv[index+1],pStrInfo_common_Tx_1)==0)
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot1sPacketDebugTraceFlagSet(L7_TRUE,rxFlag);
      }
      else
      {
        rc = usmDbDot1sPacketDebugTraceFlagSet(L7_FALSE,rxFlag);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktTxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktTxDsbld);
        }
      }
    }
    else
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot1sPacketDebugTraceFlagSet(txFlag,L7_TRUE);
      }
      else
      {
        rc = usmDbDot1sPacketDebugTraceFlagSet(txFlag,L7_FALSE);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktRxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktRxDsbld);
        }
      }
    }
  }
  else
  {
    /* Configure the trace */
    if (mode == L7_ENABLE)
    {
      rc = usmDbDot1sPacketDebugTraceFlagSet(L7_TRUE,L7_TRUE);
    }
    else
    {
      rc = usmDbDot1sPacketDebugTraceFlagSet(L7_FALSE,L7_FALSE);
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugSpanTreePktDsbld);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To configure snoop packet debug flag
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
* @cmdsyntax  [no] debug igmpsnooping packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugSnoopPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL txFlag,rxFlag;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmpsnooping) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mldsnooping) == 0)
  {
    family = L7_AF_INET6;
  }

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET(family));
    }
    else
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_NO(family));
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(numArg == 1)
  {
    usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag,family);
    if(strcmp(argv[index+1],pStrInfo_common_Tx_1)==0)
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbSnoopPacketDebugTraceFlagSet(L7_TRUE,rxFlag,family);
      }
      else
      {
        rc = usmDbSnoopPacketDebugTraceFlagSet(L7_FALSE,rxFlag,family);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(family));
        }
        else
        {
          ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_DISABLED(family));
        }
      }
    }
    else
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbSnoopPacketDebugTraceFlagSet(txFlag,L7_TRUE,family);
      }
      else
      {
        rc = usmDbSnoopPacketDebugTraceFlagSet(txFlag,L7_FALSE,family);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(family));
        }
        else
        {
          ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_DISABLED(family));
        }
      }
    }
  }
  else
  {
    /* Configure the trace */
    if (mode == L7_ENABLE)
    {
      rc = usmDbSnoopPacketDebugTraceFlagSet(L7_TRUE,L7_TRUE,family);
    }
    else
    {
      rc = usmDbSnoopPacketDebugTraceFlagSet(L7_FALSE,L7_FALSE,family);
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(family));
      }
      else
      {
        ewsTelnetWrite( ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_DISABLED(family));
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To configure ping packet debug flag
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
* @cmdsyntax  [no] debug ping packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugPingPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg != 0)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugPingPkt_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugPingPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */
  if (mode == L7_ENABLE)
  {
    rc = usmDbPingPacketDebugTraceFlagSet(L7_TRUE);
  }
  else
  {
    rc = usmDbPingPacketDebugTraceFlagSet(L7_FALSE);
  }

  if (rc == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugPingPktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugPingPktDsbld);
    }
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To configure dot1x packet debug flag
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
* @cmdsyntax  [no] debug dot1x packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugDot1xPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL txFlag,rxFlag;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugDot1xPkt_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugDot1xPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(numArg == 1)
  {
    usmDbDot1xPacketDebugTraceFlagGet(&txFlag,&rxFlag);
    if(strcmp(argv[index+1],pStrInfo_common_Tx_1)==0)
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot1xPacketDebugTraceFlagSet(L7_TRUE,rxFlag);
      }
      else
      {
        rc = usmDbDot1xPacketDebugTraceFlagSet(L7_FALSE,rxFlag);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktTxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktTxDsbld);
        }
      }
    }
    else
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot1xPacketDebugTraceFlagSet(txFlag,L7_TRUE);
      }
      else
      {
        rc = usmDbDot1xPacketDebugTraceFlagSet(txFlag,L7_FALSE);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktRxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktRxDsbld);
        }
      }
    }
  }
  else
  {
    /* Configure the trace */
    if (mode == L7_ENABLE)
    {
      rc = usmDbDot1xPacketDebugTraceFlagSet(L7_TRUE,L7_TRUE);
    }
    else
    {
      rc = usmDbDot1xPacketDebugTraceFlagSet(L7_FALSE,L7_FALSE);
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot1xPktDsbld);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Enable/Disable debug information about DHCPv4 Client activities.
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
* @notes FASTPATH DHCP Client already has packet tracing.
*        This command turns the packet tracing on or off.
*
*
* @Mode  Privileged Exec Mode
*
* @cmdsyntax for normal command: [no] debug dhcp packet {transmit | receive}
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugDhcp(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{
  L7_RC_t  status = L7_SUCCESS;
  L7_BOOL  rxflag, txflag;
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if ((numArg > 1) || (numArg < 0))
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,
                               ewsContext, pStrErr_base_DebugDhcpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,
                               ewsContext, pStrErr_base_DebugDhcpPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (numArg == 1)
  {
    if (usmDbIpDhcpClientDebugTraceFlagGet (&rxflag, &txflag) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, pStrErr_base_DebugDhcpFailed);
    }
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (mode == L7_ENABLE)
        {
          if (usmDbIpDhcpClientDebugTraceFlagSet (rxflag, L7_TRUE) != L7_SUCCESS)
          {
            status = L7_FAILURE;
          }
        }
        else
        {
          if (usmDbIpDhcpClientDebugTraceFlagSet (rxflag, L7_FALSE) != L7_SUCCESS)
          {
            status = L7_FAILURE;
          }
        }

        if (status == L7_SUCCESS)
        {
          if (mode == L7_ENABLE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                     pStrErr_base_DebugDhcpPktTxEnbld);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                     pStrErr_base_DebugDhcpPktTxDsbld);
          }
        }
      }
    }
    else if (strcmp(argv[index+1],pStrInfo_common_Receive_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (mode == L7_ENABLE)
        {
          if (usmDbIpDhcpClientDebugTraceFlagSet (L7_TRUE, txflag) != L7_SUCCESS)
          {
            status = L7_FAILURE;
          }
        }
        else
        {
          if (usmDbIpDhcpClientDebugTraceFlagSet (L7_FALSE, txflag) != L7_SUCCESS)
          {
            status = L7_FAILURE;
          }
        }

        if (status == L7_SUCCESS)
        {
          if (mode == L7_ENABLE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                     pStrErr_base_DebugDhcpPktRxEnbld);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                     pStrErr_base_DebugDhcpPktRxDsbld);
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                             ewsContext, pStrErr_base_DebugDhcpPkt);
    }
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Configure the trace */
      if (mode == L7_ENABLE)
      {
        if (usmDbIpDhcpClientDebugTraceFlagSet (L7_TRUE, L7_TRUE) != L7_SUCCESS)
        {
          status = L7_FAILURE;
        }
      }
      else
      {
        if (usmDbIpDhcpClientDebugTraceFlagSet (L7_FALSE, L7_FALSE) != L7_SUCCESS)
        {
          status = L7_FAILURE;
        }
      }

      if (status == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                   pStrErr_base_DebugDhcpEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                   pStrErr_base_DebugDhcpDsbld);
        }
      }
    }
  }

  if (status != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                             pStrErr_common_DebugBgpCmdFailed);
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE

/*********************************************************************
*
* @purpose  To configure dot3ah packet debug flag
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
* @cmdsyntax  [no] debug dot3ah packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugDot3ahPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL txFlag,rxFlag;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugDot3ahPkt_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugDot3ahPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if(numArg == 1)
  {
    usmDbDot3ahPacketDebugTraceFlagGet(&txFlag,&rxFlag);
    if(strcmp(argv[index+1],pStrInfo_common_Tx_1)==0)
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot3ahPacketDebugTraceFlagSet(L7_TRUE,rxFlag);
      }
      else
      {
        rc = usmDbDot3ahPacketDebugTraceFlagSet(L7_FALSE,rxFlag);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktTxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktTxDsbld);
        }
      }
    }
    else
    {
      if(mode == L7_ENABLE)
      {
        rc = usmDbDot3ahPacketDebugTraceFlagSet(txFlag,L7_TRUE);
      }
      else
      {
        rc = usmDbDot3ahPacketDebugTraceFlagSet(txFlag,L7_FALSE);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktRxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktRxDsbld);
        }
      }
    }
  }
  else
  {
    /* Configure the trace */
    if (mode == L7_ENABLE)
    {
      rc = usmDbDot3ahPacketDebugTraceFlagSet(L7_TRUE,L7_TRUE);
    }
    else
    {
      rc = usmDbDot3ahPacketDebugTraceFlagSet(L7_FALSE,L7_FALSE);
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugDot3ahPktDsbld);
      }
    }
  }
  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif 
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_VRRP_PACKAGE
/*********************************************************************
*
* @purpose  To configure vrrp packet debug flag
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
* @cmdsyntax  [no] debug ip vrrp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandDebugVrrpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();


  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
  */
  if (numArg != 0)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugVrrpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugVrrpPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbIpVrrpPacketDebugTraceFlagSet(L7_TRUE);
    }
    else
    {
      rc = usmDbIpVrrpPacketDebugTraceFlagSet(L7_FALSE);
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugVrrpPktEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugVrrpPktDsbld);
      }
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif
/*********************************************************************
*
* @purpose  To configure arp packet debug flag
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
* @cmdsyntax  [no] debug arp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/                                                                            
const L7_char8  *commandDebugArpPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t   rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
  */
   if (numArg != 0)
   {
     if (mode == L7_ENABLE)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugArpPkt);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugArpPktNo);
     }
     return cliSyntaxReturnPrompt (ewsContext, "");
   }
   /* configure the trace */
   if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
   {
     if (mode == L7_ENABLE)
     {
       rc = usmDbArpMapPacketDebugTraceFlagSet(L7_TRUE);
     }
     else
     {
       rc = usmDbArpMapPacketDebugTraceFlagSet(L7_FALSE);
     }

     if (rc == L7_SUCCESS)
     {
       if (mode == L7_ENABLE)
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, PStrErr_base_DebugArpPktEnbld);
       }
       else
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugArpPktDsbld);
       }
       /*************Set Flag for Script Success******/
       ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
     }
   }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

#ifdef L7_QOS_FLEX_PACKAGE_ACL
/*********************************************************************
*
* @purpose  To configure ip packet debug flag
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
* @cmdsyntax  [no] debug ip acl
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandDebugIpPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 acl ;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t   rc = L7_FAILURE;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();


  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
  */
   if (numArg != 0)
   {
     if (mode == L7_ENABLE)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugIpAcl);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugIpAclNo);
     }

     return cliSyntaxReturnPrompt (ewsContext, "");
   }
   /* configure the trace */
   if(cliConvertTo32BitUnsignedInteger(argv[index], &acl) != L7_SUCCESS)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DebugIpAcl);
     return cliSyntaxReturnPrompt (ewsContext, "");
   }
   if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
   {
     if (mode == L7_ENABLE)
     {
       rc = usmDbIpMapPacketDebugTraceFlagSet(acl, L7_TRUE);
     }
     else
     {
       rc = usmDbIpMapPacketDebugTraceFlagSet(acl, L7_FALSE);
     }

     if (rc == L7_SUCCESS)
     {
       if (mode == L7_ENABLE)
       {
         sprintf(buf,pStrErr_base_DebugAclPktEnbld,acl);
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
       }
       else
       {
         sprintf(buf,pStrErr_base_DebugAclPktDsbld,acl);
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
       }
       /*************Set Flag for Script Success******/
       ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
     }
   }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/                                                                                                               
#endif /*L7_ROUTING_PACKAGE*/                                                                                                       

#ifdef L7_ISDP_PACKAGE
/*********************************************************************
*
* @purpose  To configure isdp receive/transmit packet debug flag
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
* @cmdsyntax  [no] debug isdp packet receive|transmit
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandDebugIsdpPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,
                                         ewsContext, pStrErr_base_DebugIsdpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,
                                       ewsContext, pStrErr_base_DebugIsdpPktNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(numArg == 1)
  {
    if(strcmp(argv[index+1],pStrInfo_common_Tx_1)==0)
    {
      if(mode == L7_ENABLE)
      {
        rc = usmdbIsdpDebugTracePacketTxModeSet(L7_ENABLE);
      }
      else
      {
        rc = usmdbIsdpDebugTracePacketTxModeSet(L7_DISABLE);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                 pStrErr_base_DebugIsdpTxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                              pStrErr_base_DebugIsdpPktTxDsbld);
        }
      }
    }
    else
    {
      if(mode == L7_ENABLE)
      {
        rc = usmdbIsdpDebugTracePacketRxModeSet(L7_ENABLE);
      }
      else
      {
        rc = usmdbIsdpDebugTracePacketRxModeSet(L7_DISABLE);
      }

      if (rc == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                 pStrErr_base_DebugIsdpRxEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                              pStrErr_base_DebugIsdpPktRxDsbld);
        }
      }
    }
  }
  else
  {
    /* Configure the trace */
    if (mode == L7_ENABLE)
    {
      if((usmdbIsdpDebugTracePacketTxModeSet(L7_ENABLE)== L7_SUCCESS) &&
           (usmdbIsdpDebugTracePacketRxModeSet(L7_ENABLE)== L7_SUCCESS))
        {
          rc = L7_SUCCESS;
        }
        else
        {
          rc = L7_FAILURE;
        }
    }
    else
    {
      if((usmdbIsdpDebugTracePacketTxModeSet(L7_DISABLE) == L7_SUCCESS) &&
          (usmdbIsdpDebugTracePacketRxModeSet(L7_DISABLE) == L7_SUCCESS))
        {
          rc = L7_SUCCESS;
        }
        else
        {
          rc = L7_FAILURE;
        }
    }

    if (rc == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                pStrErr_base_DebugIsdpPktEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                pStrErr_base_DebugIsdpPktDsbld);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                              pStrErr_common_DebugBgpCmdFailed);
  }
  else
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

#endif /* L7_ISDP_PACKAGE */
                                                                                                      
#ifdef L7_SFLOW_PACKAGE
/*********************************************************************
*
* @purpose  To configure sFlow packet debug flag
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
* @cmdsyntax  [no] debug arp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/                                                               
const L7_char8  *commandDebugsFlowPacket(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t   rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
  */
   if (numArg != 0)
   {
     if (mode == L7_ENABLE)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_DebugsFlowPacketEnbld);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_DebugsFlowPacketDsbld);
     }
     return cliSyntaxReturnPrompt (ewsContext, "");
   }
   /* configure the trace */
   if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
   {
     if (mode == L7_ENABLE)
     {
       rc = usmDbsFlowPacketDebugTraceFlagSet(L7_TRUE);
     }
     else
     {
       rc = usmDbsFlowPacketDebugTraceFlagSet(L7_FALSE);
     }

     if (rc == L7_SUCCESS)
     {
       if (mode == L7_ENABLE)
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugsFlowPacketEnabled);
       }
       else
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugsFlowPacketDisable);
       }
       /*************Set Flag for Script Success******/
       ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
     }
   }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif


#ifdef L7_QOS_FLEX_PACKAGE_VOIP
/*********************************************************************
*
* @purpose  To configure auto voip packet debug flag
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
* @cmdsyntax  [no] debug auto-voip [h323 | sccp | sip]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugVoIPPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  voipProtocol_t voipProto;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Determine the 'no' mode of this command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it against zero
   */
  if (numArg > 1)
  {
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput,
                            ewsContext, pStrErr_base_DebugVoIPPkt);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (numArg == 0)
  {
    rc = usmDbQosVoIPDebugTraceFlagSet(VOIP_PROTO_ALL, mode);

    if (rc != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPTraceFlagSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if(numArg == 1)
  {
    if(strcmp(argv[index+1],pStrInfo_qos_AutoVoIPProtoH323)==0)
    {
      voipProto = VOIP_PROTO_H323;
    }
    else if(strcmp(argv[index+1],pStrInfo_qos_AutoVoIPProtoSCCP)==0)
    {
      voipProto = VOIP_PROTO_SCCP;
    }
    else if(strcmp(argv[index+1],pStrInfo_qos_AutoVoIPProtoSIP)==0)
    {
      voipProto = VOIP_PROTO_SIP;
    }
    else
    {
      ewsTelnetWriteAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput,
                              ewsContext, pStrErr_base_DebugVoIPPkt);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    rc = usmDbQosVoIPDebugTraceFlagSet(voipProto,mode);

    if (rc != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPTraceFlagSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if (mode == L7_ENABLE)
  {
    sprintf(buf,"%s %s", argv[index+1], pStrInfo_base_TracingEnbld);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
  }
  else
  {
    sprintf(buf,"%s %s", argv[index+1], pStrInfo_base_TracingDsbld);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
  }
  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif
