/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_protectedport.h
*
* @purpose Protected port commands header file
*
* @component Protected ports
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

#ifndef CLIPROTECTEDPORT_H
#define CLIPROTECTEDPORT_H

extern void buildTreeUserProtectedPort(EwsCliCommandP depth2);
extern void buildTreeShowInterfacesSwitchPort(EwsCliCommandP depth3);
extern void buildTreeGlobalSwDevProtectedPort( EwsCliCommandP depth1);

const L7_char8 * commandSwitchPortProtectedGroupName(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *
commandSwitchPortProtected(EwsContext ewsContext, L7_uint32 argc,
                           const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *
commandShowSwitchPortProtected(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *
commandShowInterfacesSwitchPort(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index);

#endif /* CLIPROTECTEDPORT_H*/
