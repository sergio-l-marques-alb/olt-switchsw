/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/routing/clicommands_l3.h
*
* @purpose header for clicommands_l3.c
*
* @component user interface
*
* @comments none
*
* @create  04/13/2001
*
* @author  Diogenes De Los Santos
* @author  Samip Garg
* @end
*
**********************************************************************/

#ifndef CLICOMMANDSL3_H
#define CLICOMMANDSL3_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliutil_l3.h"

#define CLIDEFAULTIPADDRESS                                 -536870911

/* OSPF areaID max and min values */
#define CLIOSPFAREAID_MIN				   0
#define CLIOSPFAREAID_MAX				   4294967295U

/*--------------------------------
**
** BEGIN LAYER 3 FUNCTIONS TREE STRUCTURE  SHOW & CONFIG
**
sub command movement functions  These functions are defined in the cli_tree.c
--------------------------------*/
/* BEGIN SHOW */

const char *cliShowIp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouter(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterIp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterIpInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterRoute(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspf(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfArea(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowIpOspfAreaRange(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfNeighbor(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfVirtif(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfLsdb(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterOspfStub(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterRIP(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliShowRouterRIPInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
/* END SHOW */

const L7_char8 *commandClearIpOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpOspfConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpOspfRedistribution(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpOspfCounters(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpOspfNeighbor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

/* BEGIN CONFIG */

/* CONFIG FUNCTIONS NAMES MODIFIED */

const char *commandIpVlanRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandVlanInternalAllocationBase(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandVlanInternalAllocationPolicy(EwsContext ewsContext, L7_uint32 argc, 
                                                    const L7_char8 **argv, L7_uint32 index);

const char *commandIpAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandArpTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandArpCacheSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandArp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandArpRespTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandArpRetries(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandEncapsulation(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandInterfaceRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpIcmpEchoReply(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpIcmpErrorInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const char *commandRouterId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandTrapflagsOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *command1583Compatibility(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandEnableOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaAreaId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaAreaIdStub(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSA(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSANoSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSANoRedistribute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSADefaultInfoOriginate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSATranslatorRole(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaNSSATranslatorStabilityInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandDistanceOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandExitOverflowInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandOspfSpfTimers (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandOspfPassive (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandOspfPassiveInterface (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkArea(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandExternalLsdbLimit(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandMaximumPaths(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandCapabilityOpaque(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAutoCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfAreaid(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfHelloInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfDeadInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfRetransmitInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfTransmitDelay(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfAuthenticationNone(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfAuthenticationSimple(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpOspfAuthenticationEncrypt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t commandIpOspfVirtifAuthentication(EwsContext ewsContext, const L7_char8 * * argv, L7_uint32 index, L7_uint32 intType, L7_uchar8 * strKey, L7_uint32 keyId );
L7_RC_t commandIpOspfAuthentication(EwsContext ewsContext, const L7_char8 * * argv, L7_uint32 index,L7_uint32 intType, L7_uchar8 * strKey,L7_uint32 keyId);
const char *commandIpOspfCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaAreaIdDefaultCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaAreaIdStubSummaryLSA(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaAreaIdRange(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpOspfMtuIgnore(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfRouterId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaAreaId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaAreaIdStub(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSA(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSANoSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSANoRedistribute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSADefaultInfoOriginate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSATranslatorRole(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaNSSATranslatorStabilityInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfDistance(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfExitOverflowInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfExternalLsdbLimit(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfMaximumPaths(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAutoCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaid(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6Ospf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfHelloInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfDeadInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfRetransmitInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfTransmitDelay(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAuthenticationNone(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAuthenticationSimple(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAuthenticationEncrypt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t commandIpv6OspfAuthentication(EwsContext ewsContext, const L7_char8 * * argv, L7_uint32 index,L7_uint32 intType, L7_uchar8 * strKey,L7_uint32 keyId);
const char *commandIpv6OspfCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaAreaIdDefaultCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaAreaIdStubSummaryLSA(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaAreaIdRange(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpv6OspfMtuIgnore(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfAreaVirtualLink(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *ipv6Ospfv3VirtifHandleHelloInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index,L7_uint32 neighbor, L7_uint32 areaId);
const char *ipv6Ospfv3VirtifHandleDeadInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *ipv6Ospfv3VirtifHandleRxmtInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *ipv6Ospfv3VirtifHandleTransmitDelay( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *commandEnableRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPAutoSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPHostRoutesAccept(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPSplitHorizon(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPAuthenticationNone(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPAuthenticationSimple(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandRIPAuthenticationEncrypt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t commandRIPAuthentication(EwsContext ewsContext,const L7_char8 * * argv, L7_uint32 index,L7_uint32 intType,L7_uchar8 * strKey,L7_uint32 keyId );
const char *commandIpRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRIPReceiveVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRIPSendVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandDistanceRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRouteDefault(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpRouteDistance(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpIrdp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *IrdpHandleMaxadvertinterval( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface);
const char *IrdpHandleMinadvertinterval( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface);
const char *IrdpHandleHoldTime( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface);
const char *IrdpHandlePreference( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface);
const char *IrdpHandleAddress( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface);
const char *commandBootpDhcpRelayMaxHopCount(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandBootpDhcpRelayMinWaitTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandBootpDhcpRelayEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandBootpDhcpRelayServerIp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandBootpDhcpRelayCIdOptMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandAreaVirtualLink(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *VirtifHandleAuthentication( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index);
const char *VirtifHandleHelloInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index,L7_uint32 neighbor, L7_uint32 areaId);
const char *VirtifHandleDeadInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *VirtifHandleRxmtInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *VirtifHandleTransmitDelay( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv, L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId);
const char *commandEcmpMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandProxyArp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandLocalProxyArp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandVrrpVrIDAuthentication(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandIPVrrp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandVrrpVrIDIp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandVrrpVrIDPreempt(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandVrrpVrIDPriority(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandRouterVrrpRemovedetails(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandRouterVrrpInterfaceRouterid(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandVrrpVrIDTimersAdvertise(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandVrrpVrIDTrackInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandVrrpVrIDTrackIpRoute(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandVrrpInterfaceRouterid(EwsContext ewsContext, uintf argc, const char * * argv,uintf index);
const char *commandIPVrrpID(EwsContext ewsContext, uintf argc, const char * * argv,uintf index);
const L7_char8 *commandIpOspfNetwork(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

/* Action function for RIP Route-Redistribution */
const L7_char8 *commandRedistributeRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDefaultMetricRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDistributeListRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* Action function for OSPF Route-Redistribution  */
const L7_char8 *commandRedistributeOSPF(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDefaultMetricOSPF(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDistributeListOSPF(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const char *commandDefaultInformationOriginateRip(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandDefaultInformationOriginateOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpv6OspfDefaultInformationOriginate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* END CONFIG */

/* SHOW FUNCTIONS NAMES MODIFIED */

const char *commandShowIpInterfaceBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowBootpDhcpRelay(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowVrrp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowVrrpInterfaceBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowVrrpInterfaceStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRIPInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRIPInterfaceBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRouteBestRoutes(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRouteEntry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpRoutePreferences(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpIrdp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfArea(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfABRs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfASBRs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfDatabase(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfInterfaceBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfInterfaceStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfNeighbor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfStatistics(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfStubTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfVirtualLinks(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpOspfVirtualLinksBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowVrrpInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfAreaRange(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowIpv6Ospf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfArea(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfABRs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfASBRs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfDatabase(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfInterfaceBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfInterfaceStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfNeighbor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfStubTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfVirtualLinks(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpv6OspfVirtualLinksBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/*--------------------------------
**
** END LAYER 3 FUNCTIONS TREE STRUCTURE  SHOW & CONFIG
**
--------------------------------*/

/*--------------------------------
**
** BEGIN LAYER 3 FUNCTIONS COMMANDS STRUCTURE CONFIG & SHOW
**
config functions
--------------------------------*/
/* BEGIN SHOW COMMANDS PROTOTYPES */
const char *commandShowArpTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowArp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowBridging(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowIpInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowIpSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowIpStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowIpVlan(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterRouteTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRouteBestRoutes(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRouteEntry(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRoutePreferences(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfInfo(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfInterfaceInfo(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfArea(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfAreaRange(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfInterfaceSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterIpInterfaceSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterOspfNeighborTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfNeighborDetailed(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterOspfLsdbSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfVirtifDetailed(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterOspfVirtifSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterRIPInfo(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRIPInterfaceDetailed(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRIPInterfaceSummary(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowRouterRIPPeer(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterRouterDiscovery(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

const char *commandShowRouterOspfInterfaceStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowRouterBootpDhcpRelay(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIpMtu(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandIpRedirects(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpGlobalRedirects(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpUnreachables(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRtrIntfBandwidth(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandNetDirectBcast(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowRouterOspfStubTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowRouterVrrpInterfaceDetailed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowRouterVrrpInterfaceStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowRouterVrrpInterfaceSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void commandShowOspfTrapflags(EwsContext ewsContext);

/* Kavleen : Added prototype of command functions for ARP */
const L7_char8 *commandClearArpCache(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearArpSwitch(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandArpDynamicRenew(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandArpPurge(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t cliRunningConfigRIPRedistribute(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfRedistribute(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfDistance(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfv3Redistribute(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfv3Distance(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIpRoute(EwsContext ewsContext, L7_uint32 unit);
/* END SHOW COMMANDS PROTOTYPES */

/********************New Function Declarations for Routing start ****************/

void buildTreeShowBootPdhcpRelay(EwsCliCommandP depth1);
void buildTreeShowIrdp(EwsCliCommandP depth2);
void buildTreeShowOspf(EwsCliCommandP depth2);
void buildTreeShowOspfv3(EwsCliCommandP depth2);
void buildTreeShowRIP(EwsCliCommandP depth2);
void buildTreeShowIpRoute(EwsCliCommandP depth1);
void buildRouteProtocolTree(EwsCliCommandP depth1, L7_BOOL with_all_option);
void buildRouteAllOptionTree(EwsCliCommandP depth1);
void buildTreeShowVrrp(EwsCliCommandP depth1);
void buildTreePrivArp(EwsCliCommandP depth1);
void buildTreePrivClearArp(EwsCliCommandP depth2);
void buildTreePrivClearArpSwitch(EwsCliCommandP depth2);
void buildTreeGlobalArpBootp(EwsCliCommandP depth1);
void buildTreeGlobalBootp(EwsCliCommandP depth1);
void buildTreeGlobalInternalVlan(EwsCliCommandP depth2);
void buildTreeIfIpAddr(EwsCliCommandP depth2);
void buildTreeGlobalIpFwd(EwsCliCommandP depth2);
void buildTreeGlobalIpRouter(EwsCliCommandP depth2);
void buildTreeGlobalIpIcmp(EwsCliCommandP depth2);
void buildTreeRoutingInterfaceEncaps(EwsCliCommandP depth1);
void buildTreeRoutingInterfaceConfig(EwsCliCommandP depth1);
void buildTreeRoutingInterfaceIrdp(EwsCliCommandP depth1);
void buildTreeRoutingInterfaceNetDbOspf(EwsCliCommandP depth2);
void buildTreeRoutingInterfaceNetDbOspfv3(EwsCliCommandP depth2);
void buildTreeRoutingInterfaceRip(EwsCliCommandP depth1);
void buildTreeInterfaceIpProxyArp(EwsCliCommandP depth1);
void buildTreeInterfaceIpLocalProxyArp(EwsCliCommandP depth1);
void buildTreeInterfaceIpUnnumbered(EwsCliCommandP depth2);
void buildTreeRoutingInterfaceVrrp(EwsCliCommandP depth1);
void buildTreeRoutingGlobalVrrp(EwsCliCommandP depth1);
void buildTreeRouterConfigMode(EwsCliCommandP depth1);
void buildTreeRouterOspf(EwsCliCommandP depth1);
void buildTreeRouterOspfTrapflags(EwsCliCommandP depth1);
void buildTreeRouterOspfv3(EwsCliCommandP depth1);
void buildTreeRouterRIP(EwsCliCommandP depth1);
L7_RC_t rtoRouteMetricChange(L7_uint32 destAddr, L7_uint32 destMask, L7_uint32 metric);
L7_RC_t cliRunningConfigIntfIpRoute(EwsContext ewsContext, L7_uint32 unit);

void buildTreeShowTunnelLoopback(EwsCliCommandP depth2);
void buildTreeIntfTunnelLoopback(EwsCliCommandP depth2);
void buildTreeClearIpOspf(EwsCliCommandP clearIp);
void buildTreeInterfaceIpHelperAddress(EwsCliCommandP depth2);
void buildTreePrivClearIpHelperStatistics(EwsCliCommandP depth3);
const L7_char8 *commandIpHelperEnable(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIntfIpHelperAddressSet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpHelperAddressSet(EwsContext ewsContext, L7_uint32 argc,  const L7_char8 **argv,  L7_uint32 index);
const L7_char8 *commandShowIpHelperAddress(EwsContext ewsContext, L7_uint32 argc,  const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandShowIpHelperStats(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpHelperStatistics(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index);

/********************New Function Declarations for Routing/Interface Commands Ends ****************/

#endif
