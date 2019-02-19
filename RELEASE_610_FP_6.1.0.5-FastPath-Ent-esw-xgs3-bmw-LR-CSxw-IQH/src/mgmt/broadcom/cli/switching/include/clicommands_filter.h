/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_macvlan.h
 *
 * @purpose header for Mac Vlan commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  nshrivastav
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_FILTER_H
#define CLICOMMANDS_FILTER_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"


/*********************************************************************
 *
 * @purpose  create a static MAC filter
 *
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
 * @cmdsyntax  macfilter <macaddr> <vlan>
 *
 * @cmdhelp Create a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a static MAC filter entry fo the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*********************************************************************
 *
 * @purpose  Adds a destination port to a static MAC filter
 *
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
 * @cmdsyntax  macfilter adddest <macaddr> <vlanid>
 *
 * @cmdhelp Adds a destination port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a destination port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddDest(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*********************************************************************
 *
 * @purpose  Adds a destination port to a static MAC filter
 *
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
 * @cmdsyntax  macfilter adddest all  <macaddr> <vlanid>
 *
 * @cmdhelp Adds a destination port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a destination port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddDestAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*********************************************************************
 *
 * @purpose  Adds a source port to a static MAC filter
 *
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
 * @cmdsyntax  macfilter addsrc <macaddr> <vlan>
 *
 * @cmdhelp Adds a source port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a source port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddSrc(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*********************************************************************
 *
 * @purpose  Adds a source port to a static MAC filter
 *
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
 * @cmdsyntax  macfilter addsrc all <macaddr> <vlan>
 *
 * @cmdhelp Adds a source port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a source port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddSrcAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* Build tree functions. */

/*********************************************************************
*
* @purpose  Build the Switch Device macfilter commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreeGlobalSwDevMacFilter(EwsCliCommandP depth1);
/*********************************************************************
*
* @purpose  Build the Switch Device macfilter commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreeInterfaceSwDevMacFilter(EwsCliCommandP depth1);
/*********************************************************************
*
* @purpose  Build the Switch Device show mac-address-table commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreePrivSwDevMacAddrTable(EwsCliCommandP depth2);

#endif
