/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/clicommands_dot1x.h
 *
 * @purpose header for dot1x commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/04/2003
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DOT1X_H
#define CLICOMMANDS_DOT1X_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"

void buildTreeGlobalDot1xConfig(EwsCliCommandP depth1);
void buildTreeInterfaceDot1xConfig(EwsCliCommandP depth1);
void buildTreePrivDot1xClear(EwsCliCommandP depth2);
void buildTreePrivDot1xConfig(EwsCliCommandP depth1);
void buildTreePrivDot1xShow(EwsCliCommandP depth2);
void buildTreeGlobalAuthorizationConfig(EwsCliCommandP depth1);

/**********************************************************************/

const L7_char8 *commandDot1xDefaultLogin(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xLogin(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xMaxRequests(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xGuestVlanId(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xReauthentication(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xSystemAuthControl(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xUser(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandDot1xPortControl(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xPortControlAll(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandDot1xTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandClearDot1xStatistics(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandDot1xControlDir(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xInitialize(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xReauthenticate(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandShowDot1x(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowDot1xUsers(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowDot1xClients(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xRadiusVlanAssignment(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandDot1xMaxUserInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDot1xUnatuthenticatedVlanId(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

/* Supplicant CLI functions */
const L7_char8 *commandDot1xPaeCapability(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,
                                          L7_uint32 index);
const L7_char8 *commandDot1xSupplicantPortControl(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index);

const L7_char8 *commandDot1xSupplicantTimeout(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index);
const L7_char8 *commandDot1xSupplicantUser(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index);


/*********************************************************************
*
* @purpose  Set the maximum starts for the specified port
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
* @notes
*
* @cmdsyntax  [no] dot1x max-start  <count>
*
* @cmdhelp Set the maximum start EAPOL messages to be sent on a
*          per-port basis consequently in the absence of Authenticator.
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xMaxStarts(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index);


/**********************************************************************/

#endif
