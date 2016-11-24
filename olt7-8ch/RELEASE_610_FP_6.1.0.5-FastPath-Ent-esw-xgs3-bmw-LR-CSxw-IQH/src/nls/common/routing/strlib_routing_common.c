/*********************************************************************
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/common/routing/strlib_routing_common.c                                                      
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

#include "strlib_routing_common.h"

L7_char8 *pStrInfo_routing_ActiveState = "Active State";
L7_char8 *pStrInfo_routing_AddrListErrs = "Address List Errors";
L7_char8 *pStrInfo_routing_AdvertisementIntvlSecs = "Advertisement Interval (secs)";
L7_char8 *pStrInfo_routing_AdvertisementIntvlErrs = "Advertisement Interval Errors";
L7_char8 *pStrInfo_routing_AdvertisementRcvd = "Advertisement Received";
L7_char8 *pStrInfo_routing_AuthTypeMismatch = "Authentication Type Mismatch";
L7_char8 *pStrInfo_routing_AutoSummaryMode = "Auto Summary Mode";
L7_char8 *pStrInfo_routing_BadPktsRcvd = "Bad Packets Received";
L7_char8 *pStrInfo_routing_BadRoutesRcvd = "Bad Routes Received";
L7_char8 *pStrInfo_routing_CacheSize = "Cache Size";
L7_char8 *pStrInfo_routing_DeflTimeToLive = "Default Time to Live";
L7_char8 *pStrInfo_routing_EncapType = "Encapsulation Type";
L7_char8 *pStrInfo_routing_ForwardNetDirectedBcasts = "Forward Net Directed Broadcasts";
L7_char8 *pStrInfo_routing_Gateway_1 = "Gateway";
L7_char8 *pStrInfo_routing_Half = "Half";
L7_char8 *pStrInfo_routing_HostRoutesAcceptMode = "Host Routes Accept Mode";
L7_char8 *pStrInfo_routing_IpMtu = "IP MTU";
L7_char8 *pStrInfo_routing_IpTtlErrs = "IP TTL Errors";
L7_char8 *pStrInfo_routing_IpSrcCheckingMode = "IP source-checking mode";
L7_char8 *pStrInfo_routing_IcmpInAddrMaskReps = "IcmpInAddrMaskReps";
L7_char8 *pStrInfo_routing_IcmpInAddrMasks = "IcmpInAddrMasks";
L7_char8 *pStrInfo_routing_IcmpInDestUnreach = "IcmpInDestUnreachs";
L7_char8 *pStrInfo_routing_IcmpInEchoReps = "IcmpInEchoReps";
L7_char8 *pStrInfo_routing_IcmpInEchos = "IcmpInEchos";
L7_char8 *pStrErr_routing_IcmpInErrs = "IcmpInErrors";
L7_char8 *pStrInfo_routing_IcmpInMsgs = "IcmpInMsgs";
L7_char8 *pStrInfo_routing_IcmpInParmProbs = "IcmpInParmProbs";
L7_char8 *pStrInfo_routing_IcmpInRedirects = "IcmpInRedirects";
L7_char8 *pStrInfo_routing_IcmpInSrcQuenchs = "IcmpInSrcQuenchs";
L7_char8 *pStrInfo_routing_IcmpInTimeExceeds = "IcmpInTimeExcds";
L7_char8 *pStrInfo_routing_IcmpInTimeStampReps = "IcmpInTimestampReps";
L7_char8 *pStrInfo_routing_IcmpInTimeStamps = "IcmpInTimestamps";
L7_char8 *pStrInfo_routing_IcmpOutAddrMasks = "IcmpOutAddrMasks";
L7_char8 *pStrInfo_routing_IcmpOutDestUnreach = "IcmpOutDestUnreachs";
L7_char8 *pStrInfo_routing_IcmpOutEchoReps = "IcmpOutEchoReps";
L7_char8 *pStrErr_routing_IcmpOutErrs = "IcmpOutErrors";
L7_char8 *pStrInfo_routing_IcmpOutMsgs = "IcmpOutMsgs";
L7_char8 *pStrInfo_routing_IcmpOutEchos = "IcmpOutEchos";
L7_char8 *pStrInfo_routing_IcmpOutParmProbs = "IcmpOutParmProbs";
L7_char8 *pStrInfo_routing_IcmpOutRedirects = "IcmpOutRedirects";
L7_char8 *pStrInfo_routing_IcmpOutSrcQuenchs = "IcmpOutSrcQuenchs";
L7_char8 *pStrInfo_routing_IcmpOutTimeExceeds = "IcmpOutTimeExcds";
L7_char8 *pStrInfo_routing_IcmpOutTimeStampReps = "IcmpOutTimestampReps";
L7_char8 *pStrInfo_routing_IcmpOutTimeStamps = "IcmpOutTimestamps";
L7_char8 *pStrErr_routing_AuthType_2 = "Invalid Authentication Type";
L7_char8 *pStrErr_routing_TypePktsRcvd = "Invalid Type Packets Received";
L7_char8 *pStrInfo_routing_IpForwDatagrams = "IpForwDatagrams";
L7_char8 *pStrInfo_routing_IpFragCreates = "IpFragCreates";
L7_char8 *pStrInfo_routing_IpFragFails = "IpFragFails";
L7_char8 *pStrInfo_routing_IpFragOks = "IpFragOKs";
L7_char8 *pStrErr_routing_IpInAddrErrs = "IpInAddrErrors";
L7_char8 *pStrInfo_routing_IpInDelivers = "IpInDelivers";
L7_char8 *pStrInfo_routing_IpInDiscards = "IpInDiscards";
L7_char8 *pStrErr_routing_IpInHdrErrs = "IpInHdrErrors";
L7_char8 *pStrInfo_routing_IpInReceives = "IpInReceives";
L7_char8 *pStrInfo_routing_IpInUnknownProtos = "IpInUnknownProtos";
L7_char8 *pStrInfo_routing_IpOutDiscards = "IpOutDiscards";
L7_char8 *pStrInfo_routing_IpOutNoRoutes = "IpOutNoRoutes";
L7_char8 *pStrInfo_routing_IpOutReqs = "IpOutRequests";
L7_char8 *pStrInfo_routing_IpReasmFails = "IpReasmFails";
L7_char8 *pStrInfo_routing_IpReasmOks = "IpReasmOKs";
L7_char8 *pStrInfo_routing_IpReasmReq = "IpReasmReqds";
L7_char8 *pStrInfo_routing_IpReasmTimeout = "IpReasmTimeout";
L7_char8 *pStrInfo_routing_IpRoutingDiscards = "IpRoutingDiscards";
L7_char8 *pStrInfo_routing_LinkSpeedDataRate = "Link Speed Data Rate";
L7_char8 *pStrInfo_routing_LocalLinkLsaChecksum = "Local Link LSA Checksum";
L7_char8 *pStrInfo_routing_LocalLinkLsas = "Local Link LSAs";
L7_char8 *pStrInfo_routing_MaxHopCount = "Maximum Hop Count";
L7_char8 *pStrInfo_routing_PktLenErrs = "Packet Length Errors";
L7_char8 *pStrInfo_routing_PoisonReverse = "Poison Reverse";
L7_char8 *pStrInfo_routing_PreEmptMode = "Pre-empt Mode";
L7_char8 *pStrInfo_routing_PrioDec="Priority Decrement";
L7_char8 *pStrInfo_routing_Rfc1583Compatibility = "RFC 1583 Compatibility";
L7_char8 *pStrInfo_routing_RipAdminMode = "RIP Admin Mode";
L7_char8 *pStrInfo_routing_ReqsRcvd = "Requests Received";
L7_char8 *pStrInfo_routing_ReqsRelayed = "Requests Relayed";
L7_char8 *pStrInfo_routing_Retries_1 = "Retries";
L7_char8 *pStrInfo_routing_RtrChecksumErrs = "Router Checksum Errors";
L7_char8 *pStrInfo_routing_RtrVridErrs = "Router VRID Errors";
L7_char8 *pStrInfo_routing_RtrVerErrs = "Router Version Errors";
L7_char8 *pStrInfo_routing_MaxNumOfRoutingVlansReached = "Routing could not be enabled for this VLAN. The maximum number of routing VLANs has been reached.";
L7_char8 *pStrInfo_routing_SplitHorizonMode = "Split Horizon Mode";
L7_char8 *pStrInfo_routing_StateTransitionedToMaster = "State Transitioned to Master";
L7_char8 *pStrInfo_routing_Subnets = "Subnets";
L7_char8 *pStrInfo_routing_VmacAddr = "VMAC Address";
L7_char8 *pStrErr_routing_VrrpCfgTrackRoutNotFound = "No Tracking configuration found for this route.";
L7_char8 *pStrErr_routing_VrrpCfgTrackIntfNotFound = "No Tracking configuration found for this interface.";
L7_char8 *pStrErr_routing_VrrpCfgTrackIntfTblFull = "Tracking intreface table is full for the interface and vrid combination.";
L7_char8 *pStrErr_routing_VrrpCfgTrackRoutTblFull = "Tracking route table is full for the interface and vrid combination.";
L7_char8 *pStrErr_routing_InvalidVrrpTrackRoute  = "Invalid prefix/prefix length.";
L7_char8 *pStrInfo_routing_ZeroPriPktsRcvd = "Zero Priority Packets Received";
L7_char8 *pStrInfo_routing_ZeroPriPktsSent = "Zero Priority Packets Sent";
L7_char8 *pStrInfo_routing_NotApplicable_1 = "n/a";
L7_char8 *pStrErr_routing_IpIcmpErrorIntervalOutOfRange = "The error-interval is out of range, must be in the range of %d to %d.";
L7_char8 *pStrErr_routing_IpIcmpBucketSize = "The bucket size is out of range, must be in the range of %d to %d.";
L7_char8 *pStrInfo_routing_IpIcmpRateLimitInterval = "ICMP Rate Limit Interval";
L7_char8 *pStrInfo_routing_IpIcmpRateLimitBurstSize = "ICMP Rate Limit Burst Size";
L7_char8 *pStrInfo_routing_IpIcmpEchoReplyMode = "ICMP Echo Replies";
