/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_ip_mcast6_cli.h                                                      
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

#ifndef STRLIB_IP_MCAST6_CLI_C
#define STRLIB_IP_MCAST6_CLI_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_ipmcast6_S;
extern L7_char8 *pStrInfo_ipmcast6_Range0to25;
extern L7_char8 *pStrInfo_ipmcast6_Range0to32;
extern L7_char8 *pStrInfo_ipmcast6_Range3to15;
extern L7_char8 *pStrInfo_ipmcast6_Range10to3600;
extern L7_char8 *pStrInfo_ipmcast6_SrcAddrPrefixlength;
extern L7_char8 *pStrInfo_ipmcast6_RpAddr;
extern L7_char8 *pStrInfo_ipmcast6_GrpAddr;
extern L7_char8 *pStrInfo_ipmcast6_SrcAddr;
extern L7_char8 *pStrInfo_ipmcast6_GrpAddrPrefixlength;
extern L7_char8 *pStrInfo_ipmcast6_RpAddrPrefixlength;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorderError;
extern L7_char8 *pStrErr_ipmcast6_ShowMldProxyIntfOprError;
extern L7_char8 *pStrInfo_ipmcast6_MldIntf;
extern L7_char8 *pStrInfo_ipmcast6_MldProxyCfguredAlready;
extern L7_char8 *pStrInfo_ipmcast6_MldNotEnbld;
extern L7_char8 *pStrInfo_ipmcast6_ShowMldProxyNotEnbld;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMld;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMldTraffic;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMldCounters;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrPimDm;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6_1;
extern L7_char8 *pStrInfo_ipmcast6_IpPimDmHelloIntvl;
extern L7_char8 *pStrInfo_ipmcast6_MldProxy;
extern L7_char8 *pStrInfo_ipmcast6_MldProxyResetStatus;
extern L7_char8 *pStrInfo_ipmcast6_MldProxyUnSolicitedIntvl;
extern L7_char8 *pStrInfo_ipmcast6_MldVersion;
extern L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryCount;
extern L7_char8 *pStrInfo_ipmcast6_MldLastMbrQuery;
extern L7_char8 *pStrInfo_ipmcast6_MldQueryMaxRespTime;
extern L7_char8 *pStrInfo_ipmcast6_MldRobustness;
extern L7_char8 *pStrInfo_ipmcast6_MldQueryInterval;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6SptThresh;
extern L7_char8 *pStrErr_ipmcast6_CfgRtrPimSm6RegisterRateLimit;
extern L7_char8 *pStrInfo_ipmcast6_CfgIpPimSm6StaticRp;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6_2;
extern L7_char8 *pStrInfo_ipmcast6_DsblPimDm;
extern L7_char8 *pStrInfo_ipmcast6_ShowMldGrps;
extern L7_char8 *pStrInfo_ipmcast6_ShowRtrMld;
extern L7_char8 *pStrInfo_ipmcast6_ShowMldProxy;
extern L7_char8 *pStrInfo_ipmcast6_ShowRtrMldProxyIntf;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpPimDm;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpPimSm6;
extern L7_char8 *pStrInfo_ipmcast6_ShowMldIntf;
extern L7_char8 *pStrInfo_ipmcast6_ShowMldTraffic;
extern L7_char8 *pStrInfo_ipmcast6_MldGrpAddr;
extern L7_char8 *pStrInfo_ipmcast6_MldMode;
extern L7_char8 *pStrInfo_ipmcast6_MldVersionVal;
extern L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryCountVal;
extern L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryInt;
extern L7_char8 *pStrInfo_ipmcast6_CfgMldQueryIntvl;
extern L7_char8 *pStrInfo_ipmcast6_CfgMldQueryMaxRespIntvl;
extern L7_char8 *pStrInfo_ipmcast6_CfgMldRobustness;
extern L7_char8 *pStrInfo_ipmcast6_CfgIpPimSm6Scope;
extern L7_char8 *pStrInfo_ipmcast6_MldIntfNumGet;
extern L7_char8 *pStrInfo_ipmcast6_NumNeighborAddrHhMmSsHhMmSs;
extern L7_char8 *pStrErr_ipmcast6_MustInitPim_1;
extern L7_char8 *pStrErr_ipmcast6_MustInitPimSm6;
extern L7_char8 *pStrInfo_ipmcast6_RpType;
extern L7_char8 *pStrInfo_ipmcast6_SlotUpTimeExpiryTime;
extern L7_char8 *pStrErr_ipmcast6_PimSm6BsrCandidateScope;
extern L7_char8 *pStrErr_ipmcast6_PimSm6PrefixLen;
extern L7_char8 *pStrErr_ipmcast6_MldUnableClrCounters;
extern L7_char8 *pStrErr_ipmcast6_MldUnableTrafficClrCounters;
extern L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryCount_1;
extern L7_char8 *pStrErr_ipmcast6_MldVersion;
extern L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryIntvl;
extern L7_char8 *pStrErr_ipmcast6_MldQueryIntvl;
extern L7_char8 *pStrErr_ipmcast6_MldQueryMaxRespTime_1;
extern L7_char8 *pStrErr_ipmcast6_MldRobustness_1;
extern L7_char8 *pStrErr_ipmcast6_DsblIpMldGlobal;
extern L7_char8 *pStrErr_ipmcast6_MldProxyIntf;
extern L7_char8 *pStrErr_ipmcast6_MldProxyIntfNo;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDmHelloIntvl;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDmMode;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDm;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorder;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidate;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidate_Fp;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6DrPri;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6HelloIntvl;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6JoinPruneIntvl;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RegisterThresh;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpAddr;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidate;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidate_Fp;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6SptThresh;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6_1;
extern L7_char8 *pStrErr_ipmcast6_MldVersionNo;
extern L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryCountNo;
extern L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryIntvlNo;
extern L7_char8 *pStrErr_ipmcast6_MldQueryIntvlNo;
extern L7_char8 *pStrErr_ipmcast6_MldQueryMaxRespTimeNo;
extern L7_char8 *pStrErr_ipmcast6_MldRobustnessNo;
extern L7_char8 *pStrErr_ipmcast6_DsblIpMldGlobalNo;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDmHelloIntvlNo;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDmModeNo;
extern L7_char8 *pStrErr_ipmcast6_Ipv6PimDmNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorderNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidateNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidateNo_Fp;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6DrPriNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6HelloIntvlNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6JoinPruneIntvlNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RegisterThreshNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpAddrNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidateNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidateNo_Fp;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6SptThreshNo;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6_2;
extern L7_char8 *pStrErr_ipmcast6_IpPimSm6No;
extern L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRoute;
extern L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRouteVlan;
extern L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRouteNo;
extern L7_char8 *pStrErr_ipmcast6_ShowIpMldGrps;
extern L7_char8 *pStrErr_ipmcast6_ShowIpMldIntf;
extern L7_char8 *pStrErr_ipmcast6_ShowIpMldTraffic;
extern L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxy;
extern L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxyIntf;
extern L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxyGrps;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_3;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_4;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_5;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Bsr;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6IntfStats;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Neighbor;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Rp;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6RpHash;
extern L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6_1;
extern L7_char8 *pStrInfo_ipmcast6_RpAddrPrefixlength_1;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmBsrCandidateIntf;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmBsrCandidateIntf_Fp;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRegisterThresh;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpAddr;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpAddroverride;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpCandidateIntf;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpCandidateIntf_Fp;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmSptThresh;
extern L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmSsm;
extern L7_char8 *pStrInfo_ipmcast6_Mroute;
extern L7_char8 *pStrInfo_ipmcast6_McastStaticroute;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteStatic_1;
extern L7_char8 *pStrInfo_ipmcast6_McastStaticrouteIntf;
extern L7_char8 *pStrInfo_ipmcast6_McastStaticrouteIntfFp;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMroute;
extern L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteDetailed;
extern L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteSummary;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteGrp;
extern L7_char8 *pStrInfo_ipmcast6_CfgRtrMcastBoundaryIp;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteSrc;
extern L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteDetailedSrcType;
extern L7_char8 *pStrInfo_ipmcast6_GrpSrcListExpiryTime;
extern L7_char8 *pStrInfo_ipmcast6_GrpSrcListExpiryTime_1;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldGroups;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldGroupsVlan;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldIntfAll;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldProxyGroups;
extern L7_char8 *pStrInfo_ipmcast6_Range0to126;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteDetail;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSummary;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteGroupDetail;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteGroupSummary;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSourceDetail;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSourceSummary;
extern L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteStatic;
extern L7_char8 *pStrInfo_ipmcast_DebugMld;
extern L7_char8 *pStrInfo_ipmcast6_SrcIpSrcMaskRpfAddrMetricIntf;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6;
extern L7_char8 *pStrErr_ipmcast6_PimSmSSMRangeInvalidScopeId;

extern L7_char8 *pStrInfo_ipmcast_DebugMldPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6Pkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6Pkt;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6Pkt;

extern L7_char8 *pStrInfo_ipmcast_DebugMldRxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6RxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6RxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6RxPkt;

extern L7_char8 *pStrInfo_ipmcast_DebugMldTxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6TxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6TxPkt;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6TxPkt;

extern L7_char8 *pStrErr_ipmcast_DebugMldPkt;
extern L7_char8 *pStrErr_ipmcast_DebugPimdmV6Pkt;
extern L7_char8 *pStrErr_ipmcast_DebugPimsmV6Pkt;
extern L7_char8 *pStrErr_ipmcast_DebugMcacheV6Pkt;

extern L7_char8 *pStrErr_ipmcast_DebugMldPktNo;
extern L7_char8 *pStrErr_ipmcast_DebugPimdmV6PktNo;
extern L7_char8 *pStrErr_ipmcast_DebugPimsmV6PktNo;
extern L7_char8 *pStrErr_ipmcast_DebugMcacheV6PktNo;

extern L7_char8 *pStrInfo_ipmcast_DebugMldPktEnbld;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6PktEnbld;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6PktEnbld;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6PktEnbld;

extern L7_char8 *pStrInfo_ipmcast_DebugMldPktDsbld;
extern L7_char8 *pStrInfo_ipmcast_DebugPimdmV6PktDsbld;
extern L7_char8 *pStrInfo_ipmcast_DebugPimsmV6PktDsbld;
extern L7_char8 *pStrInfo_ipmcast_DebugMcacheV6PktDsbld;
#endif

