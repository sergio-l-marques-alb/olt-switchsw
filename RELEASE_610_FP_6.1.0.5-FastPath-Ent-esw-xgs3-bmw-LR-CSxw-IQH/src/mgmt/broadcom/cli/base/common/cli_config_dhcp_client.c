/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_config_dhcp_client.c
*
* @purpose DHCP Client config commands for the cli
*
* @component DHCP Client
*
* @comments none
*
* @create  04/11/2008
*
* @author  ddevi
*
* @end
*
*********************************************************************/

#include "strlib_base_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "usmdb_dhcp_client.h"
#include "dhcp_client_api.h"

/*********************************************************************
*
* @purpose  Configure the DHCP Vendor Option Global Mode.
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
* @cmdsyntax    [no] dhcp client vendor-id-option 
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP vendor Id option
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpClientVendorOption(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpClientVendorIdOption);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmdbDhcpVendorClassOptionAdminModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_DhcpClientVendorIdOptionEnableFail);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoDhcpClientVendorIdOption);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmdbDhcpVendorClassOptionAdminModeSet(L7_DISABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_DhcpClientVendorIdOptionDisableFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDhcpClientVendorIdOption);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure the DHCP Vendor Option string.
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
* @cmdsyntax    [no] dhcp client vendor-id-option <string>
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP vendor Id option
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpClientVendorOptionString(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 **argv, 
                                                    L7_uint32 index)
{
  L7_uint32 numArg = 0; 
  L7_uchar8 vendorIdStr[DHCP_VENDOR_CLASS_STRING_MAX+1];
  L7_uchar8 argStr = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  memset(vendorIdStr,0x00,sizeof(vendorIdStr));
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpClientVendorIdOptionStr);
    }
    if(strlen(argv[index+argStr]) > DHCP_VENDOR_CLASS_STRING_MAX)
    {
      /* Invalid string size */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DhcpClientVendorIdOptionStrLen);
    } 
    osapiStrncpy(vendorIdStr, (L7_char8 *) (argv[index + argStr]),
                 DHCP_VENDOR_CLASS_STRING_MAX);
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmdbDhcpVendorClassOptionStringSet(vendorIdStr) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_DhcpClientVendorIdOptionStrSetFail);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoDhcpClientVendorIdOptionStr);
    }
    osapiStrncpy(vendorIdStr, FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING, sizeof(vendorIdStr));
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmdbDhcpVendorClassOptionStringSet(vendorIdStr) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_DhcpClientVendorIdOptionStrResetFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDhcpClientVendorIdOptionStr);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
