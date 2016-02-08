/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/common/routingv6/strlib_routingv6_common.c                                                      
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

#include "strlib_routingv6_common.h"

L7_char8 *pStrInfo_routingv6_Dc = "DC";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpAdvtRcvdStat = "DHCPv6 Advertisement Packets Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpConfirmRcvdStat = "DHCPv6 Confirm Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpDeclineRcvdStat = "DHCPv6 Decline Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpInformRcvdStat = "DHCPv6 Inform Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpMalformrecievedSentStat = "DHCPv6 Malformed Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRebindRcvdStat = "DHCPv6 Rebind Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpReconfigSentStat = "DHCPv6 Reconfig Packets Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayIntfRcvdStat = "DHCPv6 Relay-forward Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayFwdSentStat = "DHCPv6 Relay-forward Packets Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayReplyRcvdStat = "DHCPv6 Relay-reply Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRelayReplySentStat = "DHCPv6 Relay-reply Packets Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpReleasedRcvdStat = "DHCPv6 Release Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRenewRcvdStat = "DHCPv6 Renew Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpReplySentStat = "DHCPv6 Reply Packets Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpReqRcvdStat = "DHCPv6 Request Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpSolicitRcvdStat = "DHCPv6 Solicit Packets Received";
L7_char8 *pStrErr_routingv6_Ipv6StatFragmentedFailed = "Datagrams Failed To Fragment";
L7_char8 *pStrErr_routingv6_Ipv6ReassembledFails = "Datagrams Failed To Reassemble";
L7_char8 *pStrInfo_routingv6_Ipv6SentForwarded = "Datagrams Forwarded";
L7_char8 *pStrInfo_routingv6_Ipv6StatFragmentedOk = "Datagrams Successfully Fragmented";
L7_char8 *pStrInfo_routingv6_Ipv6StatReassembledSuccess = "Datagrams Successfully Reassembled";
L7_char8 *pStrInfo_routingv6_Ipv6SentDiscarded = "Datagrams Transmit Failed";
L7_char8 *pStrInfo_routingv6_IcmpStatsUnreachAble = "ICMPv6 Destination Unreachable Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsSentUnreachAble = "ICMPv6 Destination Unreachable Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsDupAddr = "ICMPv6 Duplicate Address Detects";
L7_char8 *pStrInfo_routingv6_IcmpStatsEchosReply = "ICMPv6 Echo Reply Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentEchosReply = "ICMPv6 Echo Reply Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsEchos = "ICMPv6 Echo Request Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentEchos = "ICMPv6 Echo Request Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemQuery = "ICMPv6 Group Membership Query Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemQuery = "ICMPv6 Group Membership Query Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemReduct = "ICMPv6 Group Membership Reduction Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemReduct = "ICMPv6 Group Membership Reduction Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsGrpMemResp = "ICMPv6 Group Membership Response Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentGrpMemResp = "ICMPv6 Group Membership Response Messages Transmitted";
L7_char8 *pStrErr_routingv6_IcmpStatsSentErrs = "ICMPv6 Messages Not Transmitted Due To Error";
L7_char8 *pStrInfo_routingv6_IcmpStatsUnreachAdmin = "ICMPv6 Messages Prohibited Administratively Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsSentAdmin = "ICMPv6 Messages Prohibited Administratively Transmitted";
L7_char8 *pStrErr_routingv6_IcmpStatsErrs = "ICMPv6 Messages With Errors Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsNeighAdvert = "ICMPv6 Neighbor Advertisement Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentNeighAdvert = "ICMPv6 Neighbor Advertisement Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsNeighSolicit = "ICMPv6 Neighbor Solicit Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentNeighSolicit = "ICMPv6 Neighbor Solicit Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpPktTooBig = "ICMPv6 Packet Too Big Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentPktTooBig = "ICMPv6 Packet Too Big Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsParmProblem = "ICMPv6 Parameter Problem Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentParmProblem = "ICMPv6 Parameter Problem Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsInRedirect = "ICMPv6 Redirect Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsOutRedirect = "ICMPv6 Redirect Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsRtrAdvert = "ICMPv6 Router Advertisement Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentRtrAdvert = "ICMPv6 Router Advertisement Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsRtrSolicit = "ICMPv6 Router Solicit Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpSentRtrSolicit = "ICMPv6 Router Solicit Messages Transmitted";
L7_char8 *pStrInfo_routingv6_IcmpStatsTimeExceeds = "ICMPv6 Time Exceeded Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsSentTime = "ICMPv6 Time Exceeded Messages Transmitted";
L7_char8 *pStrInfo_routingv6_RoutingInterNw = "Inter Network";
L7_char8 *pStrInfo_routingv6_RoutingInterRtr = "Inter Router";
L7_char8 *pStrInfo_routingv6_RoutingIntraPrefix = "Intra Prefix";
L7_char8 *pStrInfo_routingv6_RoutingLink = "Link";
L7_char8 *pStrInfo_routingv6_Ipv6McastRcvd = "Multicast Datagrams Received";
L7_char8 *pStrInfo_routingv6_Ipv6McastSent = "Multicast Datagrams Transmitted";
L7_char8 *pStrInfo_routingv6_Ipv6Ospfv3AreaId = "OSPFv3 Area ID";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpPoolName = "Pool Name";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpRapidCommit = "Rapid Commit";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpDiscardedPktsStat = "Received DHCPv6 Packets Discarded";
L7_char8 *pStrErr_routingv6_Ipv6StatHdrErrs = "Received Datagrams Discarded Due To Header Errors";
L7_char8 *pStrErr_routingv6_Ipv6StatAddrError = "Received Datagrams Discarded Due To Invalid Address";
L7_char8 *pStrErr_routingv6_Ipv6StatBigErrs = "Received Datagrams Discarded Due To MTU";
L7_char8 *pStrInfo_routingv6_Ipv6StatNoRoutes = "Received Datagrams Discarded Due To No Route";
L7_char8 *pStrInfo_routingv6_Ipv6StatDiscarded = "Received Datagrams Discarded Other";
L7_char8 *pStrInfo_routingv6_Ipv6StatDelivers = "Received Datagrams Locally Delivered";
L7_char8 *pStrInfo_routingv6_Ipv6StatReassembled = "Received Datagrams Reassembly Required";
L7_char8 *pStrInfo_routingv6_Ipv6StatProtos = "Received Datagrams With Unknown Protocol";
L7_char8 *pStrInfo_routingv6_Relay = "Relay";
L7_char8 *pStrInfo_routingv6_Ipv6DhcpTotalRcvd = "Total DHCPv6 Packets Received";
L7_char8 *pStrInfo_routingv6_Ipv6StatRcvd = "Total Datagrams Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsRcvd = "Total ICMPv6 Messages Received";
L7_char8 *pStrInfo_routingv6_IcmpStatsSentMsgs = "Total ICMPv6 Messages Transmitted";
L7_char8 *pStrInfo_routingv6_RoutingUnknownAs = "Unknown AS";
L7_char8 *pStrInfo_routingv6_RoutingUnknownArea = "Unknown Area";
L7_char8 *pStrInfo_routingv6_RoutingUnknownLink = "Unknown Link";
L7_char8 *pStrInfo_routingv6_V6 = "V6";
L7_char8 *pStrInfo_routingv6_RoutingIpv6RadvValidLife = "Valid Lifetime";
L7_char8 *pStrInfo_routingv6_Ipv6Mtu_1 = "ipv6 mtu";
L7_char8 *pStrInfo_routingv6_RoutingIpv6DestUnreachables = "IPv6 Destination Unreachables";
L7_char8 *pStrInfo_routingv6_RoutingIpv6RateLimitInterval = "ICMPv6 Rate Limit Error Interval";
L7_char8 *pStrInfo_routingv6_RoutingIpv6RateLimitBurstSize = "ICMPv6 Rate Limit Burst Size";
