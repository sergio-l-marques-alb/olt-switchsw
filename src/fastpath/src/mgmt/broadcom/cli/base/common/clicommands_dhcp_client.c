/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
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

#ifdef L7_ROUTING_PACKAGE
/**********************************************************************
*
* @purpose  Build the tree node for DHCP lease parameters Show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    show dhcp lease parameters.
*
* @end
*
**********************************************************************/
void  buildTreeShowDhcpLease(EwsCliCommandP depth3)
{
   EwsCliCommandP depth4, depth5,depth6,depth7;
   depth4 = ewsCliAddNode(depth3, pStrInfo_common_Lease, pStrInfo_base_ShowCfgDhcpLease,
                          commandShowDhcpLease,L7_NO_OPTIONAL_PARAMS);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowIpIntf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, 
                                   L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, L7_NO_OPTIONAL_PARAMS);
}
#endif /* L7_ROUTING_PACKAGE */

