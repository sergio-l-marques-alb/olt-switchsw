/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_pbvlan.h
 *
 * @purpose header for protocol-based vlan commands in clicommands.c
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

#ifndef CLICOMMANDS_PBVLAN_H
#define CLICOMMANDS_PBVLAN_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"

const L7_char8 *commandVlanProtocolGroup(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandVlanProtocolGroupRemove(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandVlanProtocolGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandProtocolGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandProtocolVlanGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandProtocolVlanGroupAddAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index);

const char *commandShowProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowPortProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

/* Build tree functions. */
void buildTreePrivSwPbvlan(EwsCliCommandP depth3);
void buildTreeInterfaceSwDevPortVlanGroup(EwsCliCommandP depth1);
void buildTreeGlobalSwDevVlanPbvlan(EwsCliCommandP depth2);
void buildTreeGlobalSwDevModeAllVlanGroupAll(EwsCliCommandP depth1);
void buildTreeVlanDBSwDevVlanPbvlan(EwsCliCommandP depth1);







#endif
