/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/clicommands_debug_l3.h
 *
 * @purpose header for clicommands_debug_l3.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/14/2006
 *
 * @author  cpverne
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DEBUG_L3_H
#define CLICOMMANDS_DEBUG_L3_H

/*********************************************************
**
** BEGIN DEBUG ROUTING FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreePrivledgedDebugRouting(EwsCliCommandP depth1);
void buildTreePrivledgedDebugRoutingOspf(EwsCliCommandP depth1);
void buildTreePrivledgedDebugRoutingRip(EwsCliCommandP depth1);

/********************************************************************
**
** BEGIN ROUTING DEBUG FUNCTIONS COMMANDS STRUCTURE
**
*********************************************************************/
const L7_char8 *commandDebugOspfPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDebugRipPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

#endif /* CLICOMMANDS_DEBUG_L3_H */
