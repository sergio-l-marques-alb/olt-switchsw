/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_routingv6_common.h                                                      
*                                                                     
* @purpose    Strings Library                                                      
*                                                                     
* @component  Common Strings Library                                                      
*                                                                     
* @comments   none                                                    
*                                                                     
* @create     01/10/2007                                                      
*                                                                     
* @author     Rama Sasthri, Kristipati                                
*                                                                     
* @end                                                                
*                                                                     
*********************************************************************/

#ifndef STRLIB_ROUTINGV6_COMMON_C
#define STRLIB_ROUTINGV6_COMMON_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_routingv6_Dc;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpAdvtRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpConfirmRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpDeclineRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpInformRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpMalformrecievedSentStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRebindRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpReconfigSentStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayIntfRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayFwdSentStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayReplyRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayReplySentStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpReleasedRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRenewRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpReplySentStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpReqRcvdStat;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpSolicitRcvdStat;
extern L7_char8 *pStrErr_routingv6_Ipv6StatFragmentedFailed;
extern L7_char8 *pStrErr_routingv6_Ipv6ReassembledFails;
extern L7_char8 *pStrInfo_routingv6_Ipv6SentForwarded;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatFragmentedOk;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatReassembledSuccess;
extern L7_char8 *pStrInfo_routingv6_Ipv6SentDiscarded;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsUnreachAble;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsSentUnreachAble;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsDupAddr;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsEchosReply;
extern L7_char8 *pStrInfo_routingv6_IcmpSentEchosReply;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsEchos;
extern L7_char8 *pStrInfo_routingv6_IcmpSentEchos;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemQuery;
extern L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemQuery;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemReduct;
extern L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemReduct;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemResp;
extern L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemResp;
extern L7_char8 *pStrErr_routingv6_IcmpStatsSentErrs;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsUnreachAdmin;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsSentAdmin;
extern L7_char8 *pStrErr_routingv6_IcmpStatsErrs;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsNeighAdvert;
extern L7_char8 *pStrInfo_routingv6_IcmpSentNeighAdvert;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsNeighSolicit;
extern L7_char8 *pStrInfo_routingv6_IcmpSentNeighSolicit;
extern L7_char8 *pStrInfo_routingv6_IcmpPktTooBig;
extern L7_char8 *pStrInfo_routingv6_IcmpSentPktTooBig;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsParmProblem;
extern L7_char8 *pStrInfo_routingv6_IcmpSentParmProblem;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsInRedirect;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsOutRedirect;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsRtrAdvert;
extern L7_char8 *pStrInfo_routingv6_IcmpSentRtrAdvert;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsRtrSolicit;
extern L7_char8 *pStrInfo_routingv6_IcmpSentRtrSolicit;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsTimeExceeds;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsSentTime;
extern L7_char8 *pStrInfo_routingv6_RoutingInterNw;
extern L7_char8 *pStrInfo_routingv6_RoutingInterRtr;
extern L7_char8 *pStrInfo_routingv6_RoutingIntraPrefix;
extern L7_char8 *pStrInfo_routingv6_RoutingLink;
extern L7_char8 *pStrInfo_routingv6_RoutingGrace;
extern L7_char8 *pStrInfo_routingv6_Ipv6McastRcvd;
extern L7_char8 *pStrInfo_routingv6_Ipv6McastSent;
extern L7_char8 *pStrInfo_routingv6_Ipv6Ospfv3AreaId;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpPoolName;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpRapidCommit;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpDiscardedPktsStat;
extern L7_char8 *pStrErr_routingv6_Ipv6StatHdrErrs;
extern L7_char8 *pStrErr_routingv6_Ipv6StatAddrError;
extern L7_char8 *pStrErr_routingv6_Ipv6StatBigErrs;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatNoRoutes;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatDiscarded;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatDelivers;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatReassembled;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatProtos;
extern L7_char8 *pStrInfo_routingv6_Relay;
extern L7_char8 *pStrInfo_routingv6_Ipv6DhcpTotalRcvd;
extern L7_char8 *pStrInfo_routingv6_Ipv6StatRcvd;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsRcvd;
extern L7_char8 *pStrInfo_routingv6_IcmpStatsSentMsgs;
extern L7_char8 *pStrInfo_routingv6_RoutingUnknownAs;
extern L7_char8 *pStrInfo_routingv6_RoutingUnknownArea;
extern L7_char8 *pStrInfo_routingv6_RoutingUnknownLink;
extern L7_char8 *pStrInfo_routingv6_V6;
extern L7_char8 *pStrInfo_routingv6_RoutingIpv6RadvValidLife;
extern L7_char8 *pStrInfo_routingv6_Ipv6Mtu_1;
extern L7_char8 *pStrInfo_routingv6_RoutingIpv6DestUnreachables;
extern L7_char8 *pStrInfo_routingv6_RoutingIpv6RateLimitInterval;
extern L7_char8 *pStrInfo_routingv6_RoutingIpv6RateLimitBurstSize;
#endif

