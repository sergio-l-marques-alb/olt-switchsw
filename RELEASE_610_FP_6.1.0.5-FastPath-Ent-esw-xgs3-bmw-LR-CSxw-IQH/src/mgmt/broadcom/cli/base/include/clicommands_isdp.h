/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename clicommands_isdp.h
*
* @purpose header for isdp commands
*
* @component user interface
*
* @comments none
*
* @create  21/11/2007
*
* @author  Rostyslav Ivasiv
* @end
*
***********************************************************************/
#ifndef CLICOMMANDS_ISDP
#define CLICOMMANDS_ISDP

void buildTreeConfigIsdp(EwsCliCommandP depth1);
void buildTreePrivIsdp(EwsCliCommandP depth2);
void buildTreePrivClearIsdp(EwsCliCommandP depth2);
void buildTreeInterfaceConfigIsdp(EwsCliCommandP depth1);

const L7_char8 *commandIsdpTimer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIsdpHoldtime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIsdpRun(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIsdpAdvertiseV2(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIsdpNeighbors(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIsdp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandShowIsdpEntry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandShowIsdpTraffic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIsdpCounters(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearIsdpTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIntfIsdpEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIsdpInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

void cliIsdpShowNeighborWrite(EwsContext ewsContext, L7_uint32 intIfNum, L7_char8* deviceId);

#endif /* CLICOMMANDS_ISDP */
