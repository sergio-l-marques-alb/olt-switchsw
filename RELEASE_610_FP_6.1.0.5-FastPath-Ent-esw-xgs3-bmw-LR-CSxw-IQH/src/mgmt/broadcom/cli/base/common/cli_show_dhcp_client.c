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
* @filename src/mgmt/cli/base/cli_show_dhcp_client.c
*
* @purpose DHCP Client show commands for the cli
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

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "usmdb_dhcp_client.h"
#include "cli_web_exports.h"

/*********************************************************************
*
* @purpose Displays the DHCP Client Vendor identifier information.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes      All options of this show command are included in this function
*
* @cmdsyntax  show dhcp client vendor-id-option
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDhcpClientVendorIdOption(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 **argv, 
                                                L7_uint32 index)
{
  L7_uint32 gblMode = L7_DISABLE, numArg = 0;
  L7_uchar8 eosStr = L7_EOS;
  L7_uchar8 remoteIdStr[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != L7_NULL)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_switching_DhcpL2RelayNumArgs);
  }

  cliCmdScrollSet( L7_FALSE);


  /* Vendor Identifier admin Mode */
  cliFormat(ewsContext, pStrInfo_base_DhcpClientVendorIdMode);
  if (usmdbDhcpVendorClassOptionAdminModeGet(&gblMode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, strUtilEnabledDisabledGet(gblMode, 
                                                         pStrInfo_common_NotApplicable));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
  }

  /* Vendor Identifier Option string.*/
  memset(remoteIdStr,0x00,sizeof(remoteIdStr));
  cliFormat(ewsContext, pStrInfo_base_DhcpClientVendorIdStrMode);
  if ((usmdbDhcpVendorClassOptionStringGet(remoteIdStr) == L7_SUCCESS) &&
      (osapiStrncmp(remoteIdStr, &eosStr, sizeof(L7_uchar8)) != L7_NULL))
  {
    ewsTelnetWrite(ewsContext, remoteIdStr);
  }
  else
  {
    ewsTelnetWrite(ewsContext, "Not configured");
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
