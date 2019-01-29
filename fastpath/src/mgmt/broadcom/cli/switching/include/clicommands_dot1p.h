/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_dot1p.h
 *
 * @purpose header for dot1p commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/15/2003
 *
 * @author  Jason Shaw
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DOT1P_H
#define CLICOMMANDS_DOT1P_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"

void buildTreeCosDot1pConfig(EwsCliCommandP depth1);
void buildTreePrivDot1pShow(EwsCliCommandP depth2);
void buildTreePrivShowClassOfServiceIPDscpMapping(EwsCliCommandP depth2);
void buildTreePrivShowClassOfServiceIPPrecedenceMapping(EwsCliCommandP depth2);
void buildTreePrivShowClassOfServiceTrust(EwsCliCommandP depth2);

/**********************************************************************/

const L7_char8 *commandVlanPortPriorityAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandVlanPortPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandClassofserviceDot1pmapping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowClassofServiceDot1pmapping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/**********************************************************************/

#endif
