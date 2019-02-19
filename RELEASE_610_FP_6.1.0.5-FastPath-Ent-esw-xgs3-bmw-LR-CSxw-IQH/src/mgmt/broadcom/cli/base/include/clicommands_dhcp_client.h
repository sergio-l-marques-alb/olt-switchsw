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
* @purpose DHCP Snooping config commands for the cli
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
                                              const L7_char8 **argv, L7_uint32 index);

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
                                                    L7_uint32 index);
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
                                                L7_uint32 index);

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
void buildTreeGlobalDhcpClient(EwsCliCommandP depth1);
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
void buildTreeShowDhcpClient(EwsCliCommandP depth1);

