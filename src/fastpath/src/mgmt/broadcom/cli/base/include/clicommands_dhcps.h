/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_dhcps.h
 *
 * @purpose header for dhcp server commands 
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

#ifndef CLICOMMANDS_DHCPS_H
#define CLICOMMANDS_DHCPS_H


void buildTreeDhcpsPoolConfigMode();
void buildTreeDhcpsConfig(EwsCliCommandP depth2);
void buildTreeDhcpsServiceConfig(EwsCliCommandP depth1);
void buildTreeShowDHCPs(EwsCliCommandP depth4);
void buildTreeClearDHCPs(EwsCliCommandP depth4);
void buildTreeDhcpsPool();

const L7_char8 *commandLease(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDhcpPingPackets(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandServiceDhcp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetwork(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDnsServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDefaultRouter(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandHost(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandHardwareAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClientName(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClientIdentifier(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearIpDhcpBinding(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearIpDhcpServerStatistics(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDhcpConflictLogging(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearIpDhcpConflict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandNetbiosNameServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetbiosNodeType(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDomainName(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandBootfile(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDhcpBootpAutomatic(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNextServer(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandOption(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
L7_BOOL cliConvertClientId(L7_uchar8 * buf, L7_uchar8 * clientId, L7_uint32 * clientIdBytes);

const L7_char8  *commandShowDhcpsBinding(EwsContext ewsContext, L7_uint32 argc, 
                                         const L7_char8 * * argv, L7_uint32 index);
const L7_char8  *commandShowDhcpsStatistics(EwsContext ewsContext, L7_uint32 argc,
                                            const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowDhcpsGlobalConfig(EwsContext ewsContext, L7_uint32 argc, 
                                             const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowDhcpsPoolConfig(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowDhcpsConflict(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIpDhcpExcludedAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

#endif
