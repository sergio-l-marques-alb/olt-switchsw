/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/clicommands_mcast.h
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  15/05/2002
 *
 * @author  srikrishnas
 * @end
 *
 **********************************************************************/
#ifndef CLICOMMANDSMCAST_H
#define CLICOMMANDSMCAST_H

/****************************FUNCTIONS SYNTAX**************************/

/* Mcast cli show syntax error message */

#ifdef PIMSM_FOR_FUTURE_ENHANCEMENT

#endif /*PIMSM_FOR_FUTURE_ENHANCEMENT*/

/****************************FUNCTIONS SYNTAX**************************/

/****************************TREE FUNCTIONS*****************************************/
void cliTreeGlobalConfigPimsm(EwsCliCommandP depth2);
void cliTreeGlobalConfigModeDvmrp (EwsCliCommandP depth2);
void cliTreeGlobalConfigModeIgmp (EwsCliCommandP depth2);
void cliTreeGlobalConfigModePimdm (EwsCliCommandP depth2);
void cliTreeUserShowPimdmPimsm(EwsCliCommandP depth3);
void cliTreeUserShowMulticast(EwsCliCommandP depth3);
void cliTreeUserShowDvmrp(EwsCliCommandP depth3);
void cliTreeUserShowIgmp(EwsCliCommandP depth3);
void cliTreeGlobalConfigStaticRoute(EwsCliCommandP depth1);
void cliTreeInterfaceConfigPimdmPimsm(EwsCliCommandP depth2);
void cliTreeInterfaceConfigMulticast(EwsCliCommandP depth2);
void cliTreeInterfaceConfigMcast(EwsCliCommandP depth2);
void cliTreeInterfaceConfigDvmrpIgmp(EwsCliCommandP depth2);
/****************************TREE FUNCTIONS*****************************************/

/***************************HELP STRINGS********************************************/

#define CLIPIMSM_GROUP_RP_MAP_ORIGIN_BSR      3

/*************************NEW PROPOSED COMMANDS************************************/
const L7_char8 *commandIpPimTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDvmrp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpMulticast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmJoinPruneInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmSsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmSptThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimdm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmRegisterThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDvmrpMetric(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDvmrpTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDvmrpUnicastRouting(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpProxyInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpProxyResetHostStatus(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpRouterAlert(EwsContext ewsContext, L7_uint32 argc,const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpIgmpProxyUpstreamFwdDisable(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
#ifdef MGMD_TBD /*  CLI command is not there  */
const L7_char8 *commandIpIgmpQuerierSet(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
#endif
const L7_char8 *commandIpIgmpLastMemberQueryCount(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpLastMemberQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpQueryMaxResponseTime(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpRobustness(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpStartupQueryCount(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpStartupQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpIgmpProxyUnsolicitedReportInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandNoIpIgmpProxyUnsolicitedReportInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIpIgmpVersion(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpMulticastStaticroute(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpMulticastTtlThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimdmMode(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimdmHelloInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimdmHelloInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIntfIpPimsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmHelloInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmRpAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmBsrBorder(EwsContext ewsContext,
                                        L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmDrPriority(EwsContext ewsContext,
                                         L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmRpCandidate(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpPimsmBsrCandidate(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrpInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrpNeighbor(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrpNexthop(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrpPrune(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpDvmrpRoute(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpGroups(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpInterfaceMembership(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpInterfaceStats(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowIpIgmpProxy(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpProxyInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpIgmpProxyGroups(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowIpMcast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastMroute(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastMrouteGroup(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastMrouteSource(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastBoundary(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpMcastBoundary(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpMcastMrouteStatic(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimdm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimdmInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimdmInterfaceStats(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimdmNeighbor(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsmBsr(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsmInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsmNeighbor(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsmRpMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIpPimsmRpHash(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowIpPimsmRpCandidate(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

/* Mcast Clear Commands*/
const char *cliClearMcast(EwsContext ewsContext, uintf argc, const char * * argv, uintf index );
const char *cliClearMcastMroute(EwsContext ewsContext, uintf argc, const char * * argv, uintf index );
void commandShowDvmrpTrapflags(EwsContext ewsContext);
void commandShowPimTrapflags(EwsContext ewsContext);
/*************************NEW PROPOSED ROUTINES/COMMANDS *************/
/*********************************************************************
* @purpose  Get the Protocol string for a given protocol
*
* @param  protocol  @b{(input)}  Protocol Id
* @param  str       @b{(input)}  Protocol string
* @param  strLen    @b{(input)}  Protocol string length
* @param  fmt       @b{(input)}  Format string
*
* @returns  none  
* @end
*
*********************************************************************/
void cliMcastProtocolStrPrint(L7_MCAST_IANA_PROTO_ID_t protocol, L7_uchar8 *str,
                              L7_uint32 strLen, L7_char8 const *fmt);
#endif /* CLICOMMANDSMCAST_H */
