/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename clicommands_dot1x_auth_serv.h
 *
 * @purpose header for dot1x commands in clicommands.c
 *
 * @component Command Line Interface
 *
 * @comments none
 *
 * @create  11/20/2009
 *
 * @author  pradeepk
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DOT1X_AUTH_SERV_H
#define CLICOMMANDS_DOT1X_AUTH_SERV_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "l7_common.h"

void buildTreeDot1xAuthServUserConfig();
void buildTreeShowDot1xAuthServUsers (EwsCliCommandP depth1);
void buildTreePrivilegedDot1xAuthServUsersClear (EwsCliCommandP depth1);

/**********************************************************************/
const L7_char8 *commandDot1xAuthServUserPassword(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowDot1xAuthServUsersOnly (EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandClearDot1xAuthServUsers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
#endif


