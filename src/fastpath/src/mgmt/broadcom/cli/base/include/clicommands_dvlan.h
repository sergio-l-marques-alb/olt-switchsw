/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_dvlan.h
 *
 * @purpose header for dvlan commands 
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
 ***********************************************************************/

#ifndef CLICOMMANDS_DVLAN_H
#define CLICOMMANDS_DVLAN_H



void buildTreeInterfaceSwDVlan(EwsCliCommandP depth1);
void buildTreeInterfaceSwDot1qTunnel(EwsCliCommandP depth1);
void buildTreeUserSwDevShowDot1qTunnel(EwsCliCommandP depth2);
void buildTreeUserSwDevShowDVlanTunnel(EwsCliCommandP depth2);
void buildTreeDVlanGlobalConfig(EwsCliCommandP depth1);

const L7_char8 *commandDVlanEtherType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDVlanIntfEtherType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDVlanTagging(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDVlanCustomerId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

#endif
