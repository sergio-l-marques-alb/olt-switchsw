/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_debug_mcast.c
 *
 * @purpose Debug commands for CLI for MULTICAST
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/12/2007
 *
 * @author  ddevi
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_pimsm_api.h"
#endif

#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

/*********************************************************************
*
* @purpose  To configure igmp packet debug flag
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
* @cmdsyntax  [no] debug ip igmp packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugIgmpPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_BOOL givenTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 modeMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugIgmpPktEnbld); 
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugIgmpPktDsbld); 
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugIgmpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugIgmpPktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */

  if(numArg == 1)
  {
    usmDbMgmdPacketDebugTraceFlagGet(L7_AF_INET, &rxFlag, &txFlag);
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      givenTxFlag =  L7_TRUE;
    }

    if(givenTxFlag == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_TRUE);
      }
      else
      {
        rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_FALSE);
      }
    }
    else  
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET, L7_TRUE, txFlag);
      }
      else
      {
        rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET, L7_FALSE, txFlag);
      }
    }
  }
  else 
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET,L7_TRUE, L7_TRUE);
    }
    else
    {
      rc = usmDbMgmdPacketDebugTraceFlagSet(L7_AF_INET,L7_FALSE, L7_FALSE);
    }
  }

  if (rc == L7_SUCCESS)
  {
    memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
    if (numArg == 1)
    {
      if(givenTxFlag == L7_TRUE)
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastTxOnly);
      }
      else
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                        "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxOnly);
      }
    }
    else
    {
      osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxTx);
    }
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, dispMsg);
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
* @purpose  To configure dvmrp packet debug flag
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
* @cmdsyntax  [no] debug ip dvmrp packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugDvmrpPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_BOOL givenTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 modeMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugDvmrpPktEnbld); 
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugDvmrpPktDsbld); 
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugDvmrpPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugDvmrpPktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */

  if(numArg == 1)
  {
    usmDbDvmrpPacketDebugTraceFlagGet(&rxFlag, &txFlag);
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      givenTxFlag =  L7_TRUE;
    }

    if(givenTxFlag == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbDvmrpPacketDebugTraceFlagSet(rxFlag, L7_TRUE);
      }
      else
      {
        rc = usmDbDvmrpPacketDebugTraceFlagSet(rxFlag, L7_FALSE);
      }
    }
    else  
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbDvmrpPacketDebugTraceFlagSet(L7_TRUE, txFlag);
      }
      else
      {
        rc = usmDbDvmrpPacketDebugTraceFlagSet(L7_FALSE, txFlag);
      }
    }
  }
  else 
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbDvmrpPacketDebugTraceFlagSet(L7_TRUE, L7_TRUE);
    }
    else
    {
      rc = usmDbDvmrpPacketDebugTraceFlagSet(L7_FALSE, L7_FALSE);
    }
  }

  if (rc == L7_SUCCESS)
  {
    memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
    if (numArg == 1)
    {
      if(givenTxFlag == L7_TRUE)
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastTxOnly);
      }
      else
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                        "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxOnly);
      }
    }
    else
    {
      osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxTx);
    }
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, dispMsg);
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
* @purpose  To configure pimdm packet debug flag
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
* @cmdsyntax  [no] debug ip pimdm packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugPimdmPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_BOOL givenTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 modeMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugPimdmPktEnbld); 
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugPimdmPktDsbld); 
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugPimdmPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugPimdmPktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */

  if(numArg == 1)
  {
    usmDbPimdmPacketDebugTraceFlagGet(L7_AF_INET, &rxFlag, &txFlag);
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      givenTxFlag =  L7_TRUE;
    }

    if(givenTxFlag == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_TRUE);
      }
      else
      {
        rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_FALSE);
      }
    }
    else  
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET, L7_TRUE, txFlag);
      }
      else
      {
        rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET, L7_FALSE, txFlag);
      }
    }
  }
  else 
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET,L7_TRUE, L7_TRUE);
    }
    else
    {
      rc = usmDbPimdmPacketDebugTraceFlagSet(L7_AF_INET,L7_FALSE, L7_FALSE);
    }
  }

  if (rc == L7_SUCCESS)
  {
    memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
    if (numArg == 1)
    {
      if(givenTxFlag == L7_TRUE)
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastTxOnly);
      }
      else
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                        "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxOnly);
      }
    }
    else
    {
      osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxTx);
    }
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, dispMsg);
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
* @purpose  To configure pimsm packet debug flag
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
* @cmdsyntax  [no] debug ip pimsm packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugPimsmPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_BOOL givenTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 modeMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugPimsmPktEnbld); 
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugPimsmPktDsbld); 
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugPimsmPkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugPimsmPktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */

  if(numArg == 1)
  {
    usmDbPimsmPacketDebugTraceFlagGet(L7_AF_INET, &rxFlag, &txFlag);
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      givenTxFlag =  L7_TRUE;
    }

    if(givenTxFlag == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_TRUE);
      }
      else
      {
        rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_FALSE);
      }
    }
    else  
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET, L7_TRUE, txFlag);
      }
      else
      {
        rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET, L7_FALSE, txFlag);
      }
    }
  }
  else 
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET,L7_TRUE, L7_TRUE);
    }
    else
    {
      rc = usmDbPimsmPacketDebugTraceFlagSet(L7_AF_INET,L7_FALSE, L7_FALSE);
    }
  }

  if (rc == L7_SUCCESS)
  {
    memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
    if (numArg == 1)
    {
      if(givenTxFlag == L7_TRUE)
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastTxOnly);
      }
      else
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                        "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxOnly);
      }
    }
    else
    {
      osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxTx);
    }
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, dispMsg);
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
* @purpose  To configure mcache (data) packet debug flag
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
* @cmdsyntax  [no] debug ip mcache  packet
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDebugMcachePacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_BOOL givenTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 modeMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugMcachePktEnbld); 
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    osapiSnprintf(modeMsg, L7_CLI_MAX_STRING_LENGTH, pStrInfo_ipmcast_DebugMcachePktDsbld); 
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg > 1)
  {
    if (mode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugMcachePkt);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DebugMcachePktNo);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Configure the trace */

  if(numArg == 1)
  {
    usmDbMcachePacketDebugTraceFlagGet(L7_AF_INET, &rxFlag, &txFlag);
    if (strcmp(argv[index+1],pStrInfo_common_Tx_1) == 0)
    {
      givenTxFlag =  L7_TRUE;
    }

    if(givenTxFlag == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_TRUE);
      }
      else
      {
        rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET,rxFlag, L7_FALSE);
      }
    }
    else  
    {
      if (mode == L7_ENABLE)
      {
        rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET, L7_TRUE, txFlag);
      }
      else
      {
        rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET, L7_FALSE, txFlag);
      }
    }
  }
  else 
  {
    if (mode == L7_ENABLE)
    {
      rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET,L7_TRUE, L7_TRUE);
    }
    else
    {
      rc = usmDbMcachePacketDebugTraceFlagSet(L7_AF_INET,L7_FALSE, L7_FALSE);
    }
  }

  if (rc == L7_SUCCESS)
  {
    memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
    if (numArg == 1)
    {
      if(givenTxFlag == L7_TRUE)
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastTxOnly);
      }
      else
      {
        osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                        "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxOnly);
      }
    }
    else
    {
      osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,
                     "%s%s", modeMsg, pStrInfo_ipmcast_DebugMcastRxTx);
    }
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, dispMsg);
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_DebugBgpCmdFailed);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

