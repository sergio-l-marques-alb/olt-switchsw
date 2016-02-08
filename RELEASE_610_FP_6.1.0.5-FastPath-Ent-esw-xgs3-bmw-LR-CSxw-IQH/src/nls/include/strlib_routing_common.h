/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_routing_common.h                                                      
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

#ifndef STRLIB_ROUTING_COMMON_C
#define STRLIB_ROUTING_COMMON_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_routing_ActiveState;
extern L7_char8 *pStrInfo_routing_AddrListErrs;
extern L7_char8 *pStrInfo_routing_AdvertisementIntvlSecs;
extern L7_char8 *pStrInfo_routing_AdvertisementIntvlErrs;
extern L7_char8 *pStrInfo_routing_AdvertisementRcvd;
extern L7_char8 *pStrInfo_routing_AuthTypeMismatch;
extern L7_char8 *pStrInfo_routing_AutoSummaryMode;
extern L7_char8 *pStrInfo_routing_BadPktsRcvd;
extern L7_char8 *pStrInfo_routing_BadRoutesRcvd;
extern L7_char8 *pStrInfo_routing_CacheSize;
extern L7_char8 *pStrInfo_routing_DeflTimeToLive;
extern L7_char8 *pStrInfo_routing_EncapType;
extern L7_char8 *pStrInfo_routing_ForwardNetDirectedBcasts;
extern L7_char8 *pStrInfo_routing_Gateway_1;
extern L7_char8 *pStrInfo_routing_Half;
extern L7_char8 *pStrInfo_routing_HostRoutesAcceptMode;
extern L7_char8 *pStrInfo_routing_IpMtu;
extern L7_char8 *pStrInfo_routing_IpTtlErrs;
extern L7_char8 *pStrInfo_routing_IpSrcCheckingMode;
extern L7_char8 *pStrInfo_routing_IcmpInAddrMaskReps;
extern L7_char8 *pStrInfo_routing_IcmpInAddrMasks;
extern L7_char8 *pStrInfo_routing_IcmpInDestUnreach;
extern L7_char8 *pStrInfo_routing_IcmpInEchoReps;
extern L7_char8 *pStrInfo_routing_IcmpInEchos;
extern L7_char8 *pStrErr_routing_IcmpInErrs;
extern L7_char8 *pStrInfo_routing_IcmpInMsgs;
extern L7_char8 *pStrInfo_routing_IcmpInParmProbs;
extern L7_char8 *pStrInfo_routing_IcmpInRedirects;
extern L7_char8 *pStrInfo_routing_IcmpInSrcQuenchs;
extern L7_char8 *pStrInfo_routing_IcmpInTimeExceeds;
extern L7_char8 *pStrInfo_routing_IcmpInTimeStampReps;
extern L7_char8 *pStrInfo_routing_IcmpInTimeStamps;
extern L7_char8 *pStrInfo_routing_IcmpOutAddrMasks;
extern L7_char8 *pStrInfo_routing_IcmpOutDestUnreach;
extern L7_char8 *pStrInfo_routing_IcmpOutEchoReps;
extern L7_char8 *pStrInfo_routing_IcmpOutEchos;
extern L7_char8 *pStrErr_routing_IcmpOutErrs;
extern L7_char8 *pStrInfo_routing_IcmpOutMsgs;
extern L7_char8 *pStrInfo_routing_IcmpOutParmProbs;
extern L7_char8 *pStrInfo_routing_IcmpOutRedirects;
extern L7_char8 *pStrInfo_routing_IcmpOutSrcQuenchs;
extern L7_char8 *pStrInfo_routing_IcmpOutTimeExceeds;
extern L7_char8 *pStrInfo_routing_IcmpOutTimeStampReps;
extern L7_char8 *pStrInfo_routing_IcmpOutTimeStamps;
extern L7_char8 *pStrErr_routing_AuthType_2;
extern L7_char8 *pStrErr_routing_TypePktsRcvd;
extern L7_char8 *pStrInfo_routing_IpForwDatagrams;
extern L7_char8 *pStrInfo_routing_IpFragCreates;
extern L7_char8 *pStrInfo_routing_IpFragFails;
extern L7_char8 *pStrInfo_routing_IpFragOks;
extern L7_char8 *pStrErr_routing_IpInAddrErrs;
extern L7_char8 *pStrInfo_routing_IpInDelivers;
extern L7_char8 *pStrInfo_routing_IpInDiscards;
extern L7_char8 *pStrErr_routing_IpInHdrErrs;
extern L7_char8 *pStrInfo_routing_IpInReceives;
extern L7_char8 *pStrInfo_routing_IpInUnknownProtos;
extern L7_char8 *pStrInfo_routing_IpOutDiscards;
extern L7_char8 *pStrInfo_routing_IpOutNoRoutes;
extern L7_char8 *pStrInfo_routing_IpOutReqs;
extern L7_char8 *pStrInfo_routing_IpReasmFails;
extern L7_char8 *pStrInfo_routing_IpReasmOks;
extern L7_char8 *pStrInfo_routing_IpReasmReq;
extern L7_char8 *pStrInfo_routing_IpReasmTimeout;
extern L7_char8 *pStrInfo_routing_IpRoutingDiscards;
extern L7_char8 *pStrInfo_routing_LinkSpeedDataRate;
extern L7_char8 *pStrInfo_routing_LocalLinkLsaChecksum;
extern L7_char8 *pStrInfo_routing_LocalLinkLsas;
extern L7_char8 *pStrInfo_routing_MaxHopCount;
extern L7_char8 *pStrInfo_routing_PktLenErrs;
extern L7_char8 *pStrInfo_routing_PoisonReverse;
extern L7_char8 *pStrInfo_routing_PreEmptMode;
extern L7_char8 *pStrInfo_routing_PrioDec;
extern L7_char8 *pStrInfo_routing_Rfc1583Compatibility;
extern L7_char8 *pStrInfo_routing_RipAdminMode;
extern L7_char8 *pStrInfo_routing_ReqsRcvd;
extern L7_char8 *pStrInfo_routing_ReqsRelayed;
extern L7_char8 *pStrInfo_routing_Retries_1;
extern L7_char8 *pStrInfo_routing_RtrChecksumErrs;
extern L7_char8 *pStrInfo_routing_RtrVridErrs;
extern L7_char8 *pStrErr_routing_VrrpCfgTrackRoutNotFound;
extern L7_char8 *pStrErr_routing_VrrpCfgTrackIntfNotFound;
extern L7_char8 *pStrErr_routing_VrrpCfgTrackIntfTblFull;
extern L7_char8 *pStrErr_routing_VrrpCfgTrackRoutTblFull;
extern L7_char8 *pStrErr_routing_InvalidVrrpTrackRoute;
extern L7_char8 *pStrInfo_routing_RtrVerErrs;
extern L7_char8 *pStrInfo_routing_MaxNumOfRoutingVlansReached;
extern L7_char8 *pStrInfo_routing_SplitHorizonMode;
extern L7_char8 *pStrInfo_routing_StateTransitionedToMaster;
extern L7_char8 *pStrInfo_routing_Subnets;
extern L7_char8 *pStrInfo_routing_VmacAddr;
extern L7_char8 *pStrInfo_routing_ZeroPriPktsRcvd;
extern L7_char8 *pStrInfo_routing_ZeroPriPktsSent;
extern L7_char8 *pStrInfo_routing_NotApplicable_1;
extern L7_char8 *pStrErr_routing_IpIcmpErrorIntervalOutOfRange;
extern L7_char8 *pStrErr_routing_IpIcmpBucketSize;
extern L7_char8 *pStrInfo_routing_IpIcmpRateLimitInterval;
extern L7_char8 *pStrInfo_routing_IpIcmpRateLimitBurstSize;
extern L7_char8 *pStrInfo_routing_IpIcmpEchoReplyMode;
#endif

