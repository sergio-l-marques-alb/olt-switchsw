
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_config_dai.c
*
* @purpose Dynamic ARP Inspection config commands for the cli
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create  09/28/2007
*
* @author  Kiran Kumar Kella
*
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dai.h"
#include "usmdb_dai_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"
#include "ews.h"

/*********************************************************************
* @purpose  Configure optional validation checks for DAI
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax   ip arp inspection validate {[src-mac] [dst-mac] [ip]} 
*
* @cmdhelp
*
* @cmddescript  
*
* @end
*
*********************************************************************/
const L7_char8 *commandDaiValidateOptions(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg == 0) || (numArg > 3))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiValidate);
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(numArg == 1)
      {
        if(strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0)
        {
          if(usmDbDaiVerifySMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiSMacFail);
          }
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
        else if(strcmp(argv[index+1], pStrInfo_base_DstMac) == 0)
        {
          if(usmDbDaiVerifyDMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiDMacFail);
          }
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
        else if(strcmp(argv[index+1], pStrInfo_common_Ip_2) == 0)
        {
          if(usmDbDaiVerifyIPSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiIpFail);
          }
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
        }
      }
      else if(numArg == 2)
      {
        if((strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0) &&
           (strcmp(argv[index+2], pStrInfo_base_DstMac) == 0))
        {
          if(usmDbDaiVerifySMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiSMacFail);
          }
          if(usmDbDaiVerifyDMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiDMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
        else if((strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0) &&
                (strcmp(argv[index+2], pStrInfo_common_Ip_2) == 0))
        {
          if(usmDbDaiVerifySMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiSMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiIpFail);
          }
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
        }
        else if((strcmp(argv[index+1], pStrInfo_base_DstMac) == 0) &&
                (strcmp(argv[index+2], pStrInfo_common_Ip_2) == 0))
        {
          if(usmDbDaiVerifyDMacSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiDMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_TRUE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiIpFail);
          }
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
        }
      }
      else if(numArg == 3)
      {
        if(usmDbDaiVerifySMacSet(L7_TRUE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiSMacFail);
        }
        if(usmDbDaiVerifyDMacSet(L7_TRUE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiDMacFail);
        }
        if(usmDbDaiVerifyIPSet(L7_TRUE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiIpFail);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ((numArg == 0) || (numArg > 3))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiNoValidate);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(numArg == 1)
      {
        if(strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0)
        {
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
        }
        else if(strcmp(argv[index+1], pStrInfo_base_DstMac) == 0)
        {
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
        }
        else if(strcmp(argv[index+1], pStrInfo_common_Ip_2) == 0)
        {
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
      }
      else if(numArg == 2)
      {
        if((strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0) &&
           (strcmp(argv[index+2], pStrInfo_base_DstMac) == 0))
        {
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
        }
        else if((strcmp(argv[index+1], pStrInfo_base_SrcMac) == 0) &&
                (strcmp(argv[index+2], pStrInfo_common_Ip_2) == 0))
        {
          if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoSMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
        else if((strcmp(argv[index+1], pStrInfo_base_DstMac) == 0) &&
                (strcmp(argv[index+2], pStrInfo_common_Ip_2) == 0))
        {
          if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoDMacFail);
          }
          if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
          {
            cliSyntaxNewLine(ewsContext);
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_base_DaiNoIpFail);
          }
        }
      }
      else if(numArg == 3)
      {
        if(usmDbDaiVerifySMacSet(L7_FALSE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiNoSMacFail);
        }
        if(usmDbDaiVerifyDMacSet(L7_FALSE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiNoDMacFail);
        }
        if(usmDbDaiVerifyIPSet(L7_FALSE) != L7_SUCCESS)
        {
          cliSyntaxNewLine(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DaiNoIpFail);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDaiValidate);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Enable or disable DAI on a list of VLANs. 
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip arp inspection vlan <vlan-list> [logging]
*
* @cmdhelp    User can specify a list of vlans using comma seperated
*             vlan ranges. A range is
*             specified by two VLAN IDs separated by a hyphen.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDaiVlan(EwsContext ewsContext, L7_uint32 argc, 
                               const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 i = 0, vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 arg1 = 1, arg2 = 2;
  L7_uint32 enable;        /* whether to enable or disable on these VLANS */
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if ((numArg == 0) || (numArg > 2) ||
      ((numArg == 2) && (strcmp(argv[index+arg2], pStrInfo_base_LoggingInvalid) != 0)))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDaiVlan);
  }

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + arg1];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Check if any vlan in the list is invalid */
  for(i = 0; i < vlanCount; i++)
  {
    if((vlanList[i] > L7_MAX_VLANS) || (vlanList[i] <= 0))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, tempBuf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDaiVlan);
  }

  if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) && (vlanCount))
  {
    if(numArg == 1)
    {
      vlanStart = vlanList[0];
      for(i = 0; i < vlanCount; i++)
      {
        if(1 == vlanCount)
        {
          vlanEnd = vlanStart;
        }
        else
        {
          if((i+1) == vlanCount)
          {
            vlanEnd = vlanList[i];
          }
          else if(vlanList[i+1] == vlanList[i]+1)
          {
            continue;
          }
          else
          {
            vlanEnd = vlanList[i];
          }
        }
        
        rc = usmDbDaiVlanEnableRangeSet(vlanStart, vlanEnd, enable);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                  ewsContext, 
                                                  pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
          }
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                ewsContext, pStrErr_base_DaiVlanFail);
        }
        if((i+1) < vlanCount)
        {
          vlanStart = vlanList[i+1];
        }
      }
    }
    else if((numArg == 2) && (strcmp(argv[index+arg2], pStrInfo_base_LoggingInvalid) == 0))
    {
      vlanStart = vlanList[0];
      for(i = 0; i < vlanCount; i++)
      {
        if(1 == vlanCount)
        {
          vlanEnd = vlanStart;
        }
        else
        {
          if((i+1) == vlanCount)
          {
            vlanEnd = vlanList[i];
          }
          else if(vlanList[i+1] == vlanList[i]+1)
          {
            continue;
          }
          else
          {
            vlanEnd = vlanList[i];
          }
        }
        rc = usmDbDaiVlanLoggingEnableRangeSet(vlanStart, vlanEnd, enable);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                  ewsContext, 
                                                  pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
          }
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                ewsContext, pStrErr_base_DaiVlanLoggingFail);
        }
        if((i+1) < vlanCount)
        {
          vlanStart = vlanList[i+1];
        }
      }
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure ARP ACL name for a vlan list with or without static flag
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip arp inspection filter <acl-name> vlan <vlan-list> [static]
*
* @cmdhelp    User can specify a list of vlans using comma seperated
*             vlan ranges. A range is
*             specified by two VLAN IDs separated by a hyphen.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDaiVlanArpAclFilter(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 i = 0, vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 arg1 = 1, arg2 = 2, arg3 = 3, arg4 = 4;
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uint32 enable, staticFlag = L7_DISABLE;
  L7_uchar8 aclName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if ((numArg < 3) || (numArg > 4) || (strcmp(argv[index+arg2], pStrInfo_base_VlanKeyword) != 0)
      || ((numArg == 4) && (strcmp(argv[index+arg4], pStrInfo_base_AclStatic) != 0)))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDaiVlanArpAclFilter);
  }
  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + arg3];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Check if any vlan in the list is invalid */
  for(i = 0; i < vlanCount; i++)
  {
    if((vlanList[i] > L7_MAX_VLANS) || (vlanList[i] <= 0))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, tempBuf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  OSAPI_STRNCPY_SAFE(aclName, argv[index + arg1]);
  if(numArg == 4)
  {
    staticFlag = L7_ENABLE;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDaiVlanArpAclFilter);
  }

  if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) && (vlanCount))
  {
    vlanStart = vlanList[0];
    for(i = 0; i < vlanCount; i++)
    {
      if(1 == vlanCount)
      {
        vlanEnd = vlanStart;
      }
      else
      {
        if((i+1) == vlanCount)
        {
          vlanEnd = vlanList[i];
        }
        else if(vlanList[i+1] == vlanList[i]+1)
        {
          continue;
        }
        else
        {
          vlanEnd = vlanList[i];
        }
      }
      rc = usmDbDaiVlanARPAclRangeSet(aclName, vlanStart, vlanEnd,
                                      enable, staticFlag);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_DaiVlanArpAclFilter);
      }
      if((i+1) < vlanCount)
      {
        vlanStart = vlanList[i+1];
      }
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Change the DAI trust for an interface
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip arp inspection trust
*
* @cmdhelp Configure interface as trusted or untrusted for DAI
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDaiIfTrust(EwsContext ewsContext, L7_uint32 argc, 
                                  const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_BOOL trust;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiIfTrust);
    }
    trust = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiNoIfTrust);
    }
    trust = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDaiIfTrust);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDaiIntfTrustSet(intIfNum, trust);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_DaiIfTrustFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Change the rate limit settings for an interface
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip arp inspection limit {rate <pps> [burst interval <seconds>] | none}
*
* @cmdhelp Configure rate limit and burst interval values
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDaiIfRateLimit(EwsContext ewsContext, L7_uint32 argc, 
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0, rate, burst;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 2) && (numArg != 5))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiIfRateLimit);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiNoIfRateLimit);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDaiIfRateLimit);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(strcmp(argv[index+1], pStrInfo_base_LimitNone) == L7_SUCCESS)
      {
        rc = usmDbDaiIntfRateLimitSet(intIfNum, L7_DAI_RATE_LIMIT_NONE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                ewsContext, pStrErr_base_DaiIfRateFail);
        }
      }
      else if(cliConvertTo32BitUnsignedInteger(argv[index+2], &rate) == L7_SUCCESS)
      {
        rc = usmDbDaiIntfRateLimitSet(intIfNum, rate);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                ewsContext, pStrErr_base_DaiIfRateFail);
        }
        if(numArg == 5)
        { 
          if(cliConvertTo32BitUnsignedInteger(argv[index+5], &burst) == L7_SUCCESS)
          {
            rc = usmDbDaiIntfBurstIntervalSet(intIfNum, burst);
            if (rc != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                    ewsContext, pStrErr_base_DaiIfBurstFail);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                                  ewsContext, pStrErr_base_CfgDaiIfRateLimit);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                              ewsContext, pStrErr_base_CfgDaiIfRateLimit);
      }
    }
  }
  else
  {
    if((usmDbDaiIntfRateLimitSet(intIfNum, FD_DAI_RATE_LIMIT) != L7_SUCCESS) ||
       (usmDbDaiIntfBurstIntervalSet(intIfNum, FD_DAI_BURST_INTERVAL) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDaiIfRateLimit);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Creates/deletes ARP ACL Rule.
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  permit ip host <sender-ip> mac host <sender-mac>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpAclPermitRule(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit, numArg;
  L7_uint32 argIPaddr = 1, argMacAddr = 4, hostIPAddr;
  L7_uchar8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 hostMACaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 arpAclName[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 4)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_CfgArpAcl);
  }

  OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
  /* Convert Ip address into 32 bit integer */
  if (usmDbInetAton(strIPaddr, &hostIPAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                           pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }

  memset(hostMACaddr, 0, sizeof(hostMACaddr));
  OSAPI_STRNCPY_SAFE(hostMACaddr, argv[index + argMacAddr]);

  strcpy(arpAclName, EWSARPACLNAME(ewsContext));
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Do nothing if the rule already exists in the acl */
      if (usmDbArpAclRuleGet(arpAclName, hostIPAddr, hostMACaddr) == L7_SUCCESS)
      {
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return cliPrompt(ewsContext);
      }

      rc = usmDbArpAclRuleAdd(arpAclName, hostIPAddr, hostMACaddr);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, 
                                                 pStrInfo_MaxOfArpAclRulesAlreadyCfgured,
                                                 L7_ARP_ACL_RULES_MAX);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                                 pStrInfo_ArpAclRule_UnableToCreate);
        }
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /* check if the rule exists in the acl */
    if (usmDbArpAclRuleGet(arpAclName, hostIPAddr, hostMACaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                             pStrErr_ArpAclRuleDoesntExist);
    }
    rc = usmDbArpAclRuleDelete(arpAclName, hostIPAddr, hostMACaddr);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                             pStrErr_ArpAclRuleDelFail);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

