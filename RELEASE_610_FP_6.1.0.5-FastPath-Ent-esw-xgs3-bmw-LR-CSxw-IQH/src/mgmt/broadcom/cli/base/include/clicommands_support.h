/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_support.h
 *
 * @purpose header for clicommands_support.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/24/2006
 *
 * @author  cpverne
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_SUPPORT_H
#define CLICOMMANDS_SUPPORT_H

/**************************************
**
** BEGIN DEBUG FUNCTIONS HELP
**
***************************************/

/*********************************************************
**
** BEGIN DEBUG FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void cliTreePrivilegedSupport(EwsCliCommandP depth1);

/********************************************************************
**
** BEGIN SUPPORT FUNCTIONS COMMANDS STRUCTURE
**
*********************************************************************/
const L7_char8 *commandSupportClear(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSupportConsole(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandSupportSnapshotSystem(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSupportSnapshotRouting(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSupportSnapshotMulticast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandSupportSave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/*********************************************************
**
** BEGIN DEBUG SYNTAX ERROR MESSAGES
**
**********************************************************/
#endif /* CLICOMMANDS_SUPPORT_H */
