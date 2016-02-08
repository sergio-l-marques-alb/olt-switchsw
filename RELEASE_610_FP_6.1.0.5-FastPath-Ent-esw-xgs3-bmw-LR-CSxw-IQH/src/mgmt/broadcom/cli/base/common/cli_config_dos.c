/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_dos.c
 *
 * @purpose config commands for the Denial of Service cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/01/2005
 *
 * @update
 *
 * @author  esmiley
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "usmdb_dos_api.h"
#include "usmdb_util_api.h"
#include "doscontrol_exports.h"

/* Denial of Service Commands */


/*********************************************************************
*
* @purpose  Configure Denial of Service Functions
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
* @notes To configure all Denial of Service Features
*
*
* @cmdsyntax dos-control all
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSAll(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosIcmpFragCheck);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoIcmpFrag);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  { 
    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
    {
       if (usmdbDoSFirstFragModeSet(unit,mode) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosv4FirstFrag_2);
       } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (usmdbDoSICMPModeSet(unit,mode) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmp_1);
       } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSICMPFragModeSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmpFrag);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSL4PortSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosL4Port_1);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSSIPDIPSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosSipDip_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSSMACDMACSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosSmacDmac_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFinUrgPshSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFinUrgPsh_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFlagSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFlag_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFlagSeqSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFlagSeq_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFragSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFrag_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPOffsetSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpOffset_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPPortSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpPort_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPSynSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpSyn_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPSynFinSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpSynFin_2);
      } 
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSUDPPortSet(unit,mode) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosUdpPort_2);
      } 
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service SIP=DIP Function
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
* @notes To configure the Denial of Service SIP=DIP Feature
*
*
* @cmdsyntax dos-control sipdip
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSSIPDIP(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosSipDip_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoSipDip);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSSIPDIPSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosSipDip_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service SMAC=DMAC Function
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
* @notes To configure the Denial of Service SMAC=DMAC Feature
*
*
* @cmdsyntax dos-control smacdmac
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSSMACDMAC(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosSmacDmac_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoSmacDmac);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSSMACDMACSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosSmacDmac_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service First Fragment Function
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
* @notes To configure the Denial of Service First Fragment Feature
*
*
* @cmdsyntax dos-control firstfrag [<minTcpHdrLength 0-255>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSFirstFrag(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_int32 minTcpHdrSize = 0;

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
    if (numArg == 1)
    {
      mode = L7_ENABLE;

      minTcpHdrSize = atoi(argv[index+1]);
    }
    else if (numArg == 0)
    {
      mode = L7_ENABLE;

      minTcpHdrSize = FD_DOS_DEFAULT_MINTCPHDR_SIZE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosv4FirstFrag_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoV4FirstFrag);
    }
    else
    {
      mode = L7_DISABLE;
      minTcpHdrSize = FD_DOS_DEFAULT_MINTCPHDR_SIZE;
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSFirstFragModeSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosv4FirstFrag_2);
    }
    if (usmdbDoSFirstFragSizeSet(unit,minTcpHdrSize) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosv4FirstFrag_2);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP Fragment Function
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
* @notes To configure the Denial of Service TCP Fragment Feature
*
*
* @cmdsyntax dos-control tcpfrag
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPFrag(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpFrag_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpFrag);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPFragSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFrag_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP Offset Function
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
* @notes To configure the Denial of Service TCP Offset Feature
*
*
* @cmdsyntax dos-control tcpoffset
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPOffset(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpOffset_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpOffset);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPOffsetSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpOffset_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP Port Function
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
* @notes To configure the Denial of Service TCP Port Feature
*
*
* @cmdsyntax dos-control tcpport
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPPort(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpPort_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpPort);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPPortSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpPort_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service UDP Port Function
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
* @notes To configure the Denial of Service UDP Port Feature
*
*
* @cmdsyntax dos-control udpport
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSUDPPort(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosUdpPort_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoUdpPort);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSUDPPortSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosUdpPort_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP SYN Function
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
* @notes To configure the Denial of Service TCP SYN Feature
*
*
* @cmdsyntax dos-control tcpsyn
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPSyn(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpSyn_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpSyn);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPSynSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpSyn_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP SYN&FIN Function
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
* @notes To configure the Denial of Service TCP SYN&FIN Feature
*
*
* @cmdsyntax dos-control tcpsynfin
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPSynFin(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpSynFin_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpSynFin);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPSynFinSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpSynFin_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP Flag Function
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
* @notes To configure the Denial of Service TCP Flag Feature
*
*
* @cmdsyntax dos-control tcpflag
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPFlag(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpFlag_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpFlag);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPFlagSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFlag_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP Flag & Sequence Function
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
* @notes To configure the Denial of Service TCP Flag & Sequence Feature
*
*
* @cmdsyntax dos-control tcpflagseq
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPFlagSeq(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpFlagSeq_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpFlagSeq);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPFlagSeqSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFlag_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service TCP FIN&URG&PSH Function
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
* @notes To configure the Denial of Service TCP FIN&URG&PSH Feature
*
*
* @cmdsyntax dos-control tcpfinurgpsh
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSTCPFinUrgPsh(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosTcpFinUrgPsh_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoTcpFinUrgPsh);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSTCPFinUrgPshSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosTcpFlag_2);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service L4 Port Function
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
* @notes To configure the Denial of Service L4 Port Feature
*
*
* @cmdsyntax dos-control l4port
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSL4Port(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosL4PortCheck);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoDosL4PortCheck);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSL4PortSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosL4Port_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service ICMP Function
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
* @notes To configure the Denial of Service ICMP Feature
*
*
* @cmdsyntax dos-control icmp [<maxIcmpPktSize 0-1023>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSICMP(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_int32 maxIcmpSize = 0;

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
    if (numArg == 1)
    {
      mode = L7_ENABLE;

      maxIcmpSize = atoi(argv[index+1]);
    }
    else if (numArg == 0)
    {
      mode = L7_ENABLE;

      maxIcmpSize = FD_DOS_DEFAULT_MAXICMP_SIZE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosIcmpCheck);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoIcmp);
    }
    else
    {
      mode = L7_DISABLE;
      maxIcmpSize = FD_DOS_DEFAULT_MAXICMP_SIZE;
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSICMPModeSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmp_1);
    }
    if (usmdbDoSICMPSizeSet(unit,maxIcmpSize) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmp_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service ICMPv6 Function
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
* @notes To configure the Denial of Service ICMPv6 Feature
*
*
* @cmdsyntax dos-control icmpv6 [<maxIcmpPktSize 0-1023>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSICMPv6(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_int32 maxIcmpSize = 0;

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
    if (numArg == 1)
    {
      mode = L7_ENABLE;

      maxIcmpSize = atoi(argv[index+1]);
    }
    else if (numArg == 0)
    {
      mode = L7_ENABLE;

      maxIcmpSize = FD_DOS_DEFAULT_MAXICMP_SIZE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosIcmpv6Check);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoIcmpv6);
    }
    else
    {
      mode = L7_DISABLE;
      maxIcmpSize = FD_DOS_DEFAULT_MAXICMP_SIZE;
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSICMPv6SizeSet(unit,maxIcmpSize) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmpv6_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure Denial of Service ICMP Fragment Function
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
* @notes To configure the Denial of Service ICMP Fragment Feature
*
*
* @cmdsyntax dos-control icmpfrag
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigDoSICMPFrag(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode = L7_DISABLE;
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

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosIcmpFragCheck);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDosNoIcmpFrag);
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

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmdbDoSICMPFragModeSet(unit,mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgDosIcmpFrag);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/* End - Denial of Service Commands */
