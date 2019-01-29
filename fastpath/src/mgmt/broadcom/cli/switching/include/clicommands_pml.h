/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_pml.h
*
* @purpose Port mac-locking commands header file
*
* @component Port mac-locking
*
* @comments none
*
* @create  03/5/2007
*
* @author  nshrivastav
*
* @end
*
*********************************************************************/

#ifndef CLIPML_H
#define CLIPML_H

/*********************************************************
**
** BEGIN PORT MAC-LOCKING FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
extern void buildTreeGlobalSwDevPortSecurity(EwsCliCommandP depth1);
extern void buildTreeInterfaceSwDevPortSecurity(EwsCliCommandP depth1);
extern void buildTreeInterfaceSwDevPortSecuritySnmp(EwsCliCommandP depth1);
extern void buildTreePrivSwDevShowPortSecurity(EwsCliCommandP depth2);

const L7_char8 *commandPortSecurity(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortSecurityMaxDynamic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortSecurityMaxStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortSecurityMacAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortSecurityMacAddressMove(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerEnableTrapsViolationMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowPortSecurity(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortSecurityInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortSecurityDynamic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortSecurityStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortSecurityViolation(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);



#endif /* CLIDHCPFILTERING_H*/
