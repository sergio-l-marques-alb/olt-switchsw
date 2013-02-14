/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/clicommands_debug_mcast.h
 *
 * @purpose header for clicommands_debug_mcast.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  12/10/2007 (Dec 10th, 2007)
 *
 * @author  ddevi
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DEBUG_MCAST_H
#define CLICOMMANDS_DEBUG_MCAST_H

/*********************************************************
**
** BEGIN DEBUG MULTICAST FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreePrivledgedDebugMcast(EwsCliCommandP depth1);

/********************************************************************
**
** BEGIN MCAST DEBUG FUNCTIONS 
**
*********************************************************************/
const L7_char8 *commandDebugIgmpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDebugDvmrpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDebugPimdmPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDebugPimsmPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDebugMcachePacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

#endif /* CLICOMMANDS_DEBUG_L3_H */
