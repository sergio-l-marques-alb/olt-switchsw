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
* @filename src/mgmt/cli/base/clicommands_dhcp_client.c
*
* @purpose DHCP Client commands
*
* @component DHCP Client
*
* @comments contains the code to build the dhcp client config and show tree
* @comments 
*
* @create  04/11/2008
*
* @author  ddevi
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_dhcp_client.h"


/*********************************************************************
*
* @purpose  Build the tree node for DHCP client Global config mode
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalDhcpClient(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Client_1, 
                         pStrInfo_base_CfgDhcpClient, 
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dhcp client vendor-id-option */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_DhcpClientVendorId, 
                         pStrInfo_base_DhcpClientVendorIdHelp,
                         commandDhcpClientVendorOption, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


  /* dhcp client vendor-id-option-string <string> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_DhcpClientVendorIdStr, 
                         pStrInfo_base_DhcpClientVendorIdStrHelp,
                         commandDhcpClientVendorOptionString, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_DhcpClientVendorIdString, 
                         pStrInfo_base_DhcpClientVendorIdStringHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL,  L7_NO_OPTIONAL_PARAMS);

  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

}
/*********************************************************************
*
* @purpose  Build the tree node for DHCP L2 relay Show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    show dhcp client vendo-id-option
*
* @end
*
*********************************************************************/
void buildTreeShowDhcpClient(EwsCliCommandP depth1)
{
  EwsCliCommandP  depth2, depth3;


  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Client_1, 
                         pStrInfo_base_DhcpClientShowHelp, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  /* vendor-Id-option */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_DhcpClientVendorId, 
                         pStrInfo_base_DhcpClientVendorIdShowHelp,
                         commandShowDhcpClientVendorIdOption, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
}
