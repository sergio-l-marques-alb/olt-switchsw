/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_ip_mcast_web.h                                                      
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

#ifndef STRLIB_IP_MCAST_WEB_C
#define STRLIB_IP_MCAST_WEB_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_ipmcast_AddEntryToMcastBoundaryTbl;
extern L7_char8 *pStrInfo_ipmcast_AdminModeForIpv6;
extern L7_char8 *pStrInfo_ipmcast_AdminScopeBoundary;
extern L7_char8 *pStrInfo_ipmcast_BsrBorderIpv4;
extern L7_char8 *pStrInfo_ipmcast_BsrBorderIpv6;
extern L7_char8 *pStrInfo_ipmcast_CbsrHashMaskLen;
extern L7_char8 *pStrInfo_ipmcast_CbsrPref;
extern L7_char8 *pStrInfo_ipmcast_CrpPref;
extern L7_char8 *pStrInfo_ipmcast_ClrRoute;
extern L7_char8 *pStrInfo_ipmcast_Compatibility;
extern L7_char8 *pStrInfo_ipmcast_CompBsrAddr;
extern L7_char8 *pStrInfo_ipmcast_CompBsrExpiryTimeHhMmSs;
extern L7_char8 *pStrInfo_ipmcast_CompCrpHoldTimeHhMmSs;
extern L7_char8 *pStrInfo_ipmcast_CompIdx;
extern L7_char8 *pStrInfo_ipmcast_CfgOfStaticRpEntryFailed;
extern L7_char8 *pStrInfo_ipmcast_CreateBoundary;
extern L7_char8 *pStrInfo_ipmcast_CreateStaticRoute;
extern L7_char8 *pStrInfo_ipmcast_DelMem;
extern L7_char8 *pStrInfo_ipmcast_DrPriIpv4;
extern L7_char8 *pStrInfo_ipmcast_DrPriIpv6;
extern L7_char8 *pStrInfo_ipmcast_DvmrpCfgSummary;
extern L7_char8 *pStrInfo_ipmcast_DvmrpGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_DvmrpIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_DvmrpNextHopSummary;
extern L7_char8 *pStrInfo_ipmcast_DvmrpPruneSummary;
extern L7_char8 *pStrInfo_ipmcast_DvmrpRouteSummary;
extern L7_char8 *pStrInfo_ipmcast_DataThreshRateKbpsIpv4;
extern L7_char8 *pStrInfo_ipmcast_DataThreshRateKbpsIpv6;
extern L7_char8 *pStrErr_ipmcast_FailedToClrIgmpProxyIntfStats;
extern L7_char8 *pStrInfo_ipmcast_Flags;
extern L7_char8 *pStrInfo_ipmcast_GrpCompatibilityMode;
extern L7_char8 *pStrInfo_ipmcast_GrpIp;
extern L7_char8 *pStrInfo_ipmcast_HelloIntvlSecsIpv4;
extern L7_char8 *pStrInfo_ipmcast_HelloIntvlSecsIpv6;
extern L7_char8 *pStrInfo_ipmcast_HoldTimeHhMmSs;
extern L7_char8 *pStrInfo_ipmcast_IdlMem;
extern L7_char8 *pStrInfo_ipmcast_IgmpCacheInfo;
extern L7_char8 *pStrInfo_ipmcast_IgmpCfgSummary;
extern L7_char8 *pStrInfo_ipmcast_IgmpGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_IgmpIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_IgmpIntfDetailedMbrshipInfo_1;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyCfgSummary;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfo;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfoDetailed;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyCfguredOnIntf_1;
extern L7_char8 *pStrInfo_ipmcast_IgmpProxyNotOperational;
extern L7_char8 *pStrInfo_ipmcast_IgmpNotIsEnbldOnRtr;
extern L7_char8 *pStrInfo_ipmcast_IncomingIntf;
extern L7_char8 *pStrInfo_ipmcast_IntfHelloIntvlForIpv4;
extern L7_char8 *pStrInfo_ipmcast_IntfHelloIntvlForIpv6;
extern L7_char8 *pStrInfo_ipmcast_IntfModeForIpv4;
extern L7_char8 *pStrInfo_ipmcast_IntfModeForIpv6;
extern L7_char8 *pStrInfo_ipmcast_IntfNeighbors;
extern L7_char8 *pStrInfo_ipmcast_IntfParamsIpv4;
extern L7_char8 *pStrInfo_ipmcast_IntfParamsIpv6;
extern L7_char8 *pStrErr_ipmcast_GrpAddrSpecified;
extern L7_char8 *pStrErr_ipmcast_GrpMaskSpecified;
extern L7_char8 *pStrErr_ipmcast_IpAddrSpecified;
extern L7_char8 *pStrErr_ipmcast_Input_3;
extern L7_char8 *pStrErr_ipmcast_ValToJoinPruneIntvl;
extern L7_char8 *pStrInfo_ipmcast_InvlaidGrpMaskSpecified;
extern L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecsIpv4;
extern L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecsIpv6;
extern L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecs_1;
extern L7_char8 *pStrInfo_ipmcast_MrouteEntryS;
extern L7_char8 *pStrInfo_ipmcast_MaxAddrEntries;
extern L7_char8 *pStrInfo_ipmcast_McastAdminBoundaryCfg;
extern L7_char8 *pStrInfo_ipmcast_McastAdminBoundarySummary;
extern L7_char8 *pStrInfo_ipmcast_McastFwdIsNotEnbld;
extern L7_char8 *pStrInfo_ipmcast_McastGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_McastIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_McastMrouteSummary;
extern L7_char8 *pStrInfo_ipmcast_McastStaticRoutesCfg;
extern L7_char8 *pStrInfo_ipmcast_McastStaticRoutesSummary;
extern L7_char8 *pStrInfo_ipmcast_Na;
extern L7_char8 *pStrInfo_ipmcast_NonMem;
extern L7_char8 *pStrInfo_ipmcast_NeighborIp;
extern L7_char8 *pStrInfo_ipmcast_NeighborParams;
extern L7_char8 *pStrInfo_ipmcast_NetMask;
extern L7_char8 *pStrInfo_ipmcast_NextHopIntf;
extern L7_char8 *pStrInfo_ipmcast_NoIgmpCacheInfoAvailable;
extern L7_char8 *pStrInfo_ipmcast_NoIgmpProxyIntfAvailable;
extern L7_char8 *pStrErr_ipmcast_NumOfPktsForWhichGrpNotFound;
extern L7_char8 *pStrErr_ipmcast_NumOfPktsForWhichSrcNotFound;
extern L7_char8 *pStrInfo_ipmcast_NumOfSrcs;
extern L7_char8 *pStrInfo_ipmcast_OutgoingIntfs;
extern L7_char8 *pStrInfo_ipmcast_PimDmGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_PimDmIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_PimDmIntfSummary;
extern L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpSummary;
extern L7_char8 *pStrInfo_ipmcast_PimSmCompSummary;
extern L7_char8 *pStrInfo_ipmcast_PimSmGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_PimSmGlobalStatus;
extern L7_char8 *pStrInfo_ipmcast_PimSmIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_PimSmIntfSummary;
extern L7_char8 *pStrInfo_ipmcast_PimSmRpSetSummary;
extern L7_char8 *pStrInfo_ipmcast_PimSmStaticRpCfg;
extern L7_char8 *pStrInfo_ipmcast_ProxyIntfStats;
extern L7_char8 *pStrInfo_ipmcast_QueriesRcvd;
extern L7_char8 *pStrInfo_ipmcast_RpfNeighbor;
extern L7_char8 *pStrInfo_ipmcast_RegisterThreshRateKbpsIpv4;
extern L7_char8 *pStrInfo_ipmcast_RegisterThreshRateKbpsIpv6;
extern L7_char8 *pStrInfo_ipmcast_RemovalOfStaticRpEntryFailed;
extern L7_char8 *pStrInfo_ipmcast_SsmIpv4;
extern L7_char8 *pStrInfo_ipmcast_SsmIpv6;
extern L7_char8 *pStrInfo_ipmcast_SsmGrpAddrIpv4;
extern L7_char8 *pStrInfo_ipmcast_SsmGrpAddrIpv6;
extern L7_char8 *pStrInfo_ipmcast_SsmGrpMaskIpv4;
extern L7_char8 *pStrInfo_ipmcast_SsmGrpMaskIpv6;
extern L7_char8 *pStrInfo_ipmcast_SlotPort;
extern L7_char8 *pStrInfo_ipmcast_SomeOtherMcastRoutingProtoIsCurrentlyCfgured;
extern L7_char8 *pStrInfo_ipmcast_SrcFilterMode;
extern L7_char8 *pStrInfo_ipmcast_SrcHosts;
extern L7_char8 *pStrInfo_ipmcast_SrcIp;
extern L7_char8 *pStrInfo_ipmcast_StaticMrouteEntry;
extern L7_char8 *pStrInfo_ipmcast_StaticMrouteEntryAlreadyExists;
extern L7_char8 *pStrInfo_ipmcast_TtlThresh;
extern L7_char8 *pStrInfo_ipmcast_TblEntryCount;
extern L7_char8 *pStrInfo_ipmcast_TblHighestEntryCount;
extern L7_char8 *pStrInfo_ipmcast_TblMaxEntryCount;
extern L7_char8 *pStrInfo_ipmcast_UnableToAddStaticMrouteEntry;
extern L7_char8 *pStrInfo_ipmcast_UnableToClrMrouteEntries;
extern L7_char8 *pStrInfo_ipmcast_UnableToFetchOneOfStaticRouteParams;
extern L7_char8 *pStrInfo_ipmcast_UpstreamNeighbor;
extern L7_char8 *pStrInfo_ipmcast_Ver1HostTimerSecs;
extern L7_char8 *pStrInfo_ipmcast_Ver1QuerierTimeout;
extern L7_char8 *pStrInfo_ipmcast_Ver2HostTimerSecs;
extern L7_char8 *pStrInfo_ipmcast_Ver2QuerierTimeout;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpGlobalCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpIntfCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpIntfCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpNexthopCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpPruneCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpRouteCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpglobalcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpnexthopparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrppruneparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrprouteparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpcacheinfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpglobalcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfgparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpMbrshipinfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyCacheinfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfgparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyMbrshipinfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastmroutecfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopecfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopeparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimglobalcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcandidateparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcomponentparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifcfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmrpsetparams;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmstaticrp;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpCacheRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpGlobalCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIfMbrshipInfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIntfCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIntfCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyCacheRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIfMbrshipInfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIntfCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastBoundaryCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastBoundaryCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastGlobalCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastIntfCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastMrouteCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastMroutev6Cfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastStaticRoutesCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlFileMcastStaticRoutesShow;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmGlobalCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmIntfCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmIntfCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmCandidaterpShow;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmCompShow;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmGlobalCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmGlobalCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmIntfCfgRo;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmIntfCfgRw;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmRpsetShow;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmStaticrpCfg;
extern L7_char8 *pStrInfo_ipmcast_V_3;
extern L7_char8 *pStrInfo_ipmcast_StaticEntry;
extern L7_char8 *pStrErr_ipmcast_PimSmRpCandidateDeletion;
extern L7_char8 *pStrErr_ipmcast_PimSmRpCandidateCreation;
extern L7_char8 *pStrErr_ipmcast_PimSmInvalidInterface;
extern L7_char8 *pStrErr_ipmcast_PimSmInvalidHashMask;
extern L7_char8 *pStrErr_ipmcast_PimSmInvalidPriority;
extern L7_char8 *pStrErr_ipmcast_PimSmBSRCandidate;
extern L7_char8 *pStrErr_ipmcast_PimSmInvalidScope;

extern L7_char8 *pStrInfo_ipmcast_staticRPIPV4Info;
extern L7_char8 *pStrInfo_ipmcast_staticRPIPV6Info;

extern L7_char8 *pStrInfo_ipmcast_RPAddressIPV4Info;
extern L7_char8 *pStrInfo_ipmcast_RPAddressIPV6Info;

extern L7_char8 *pStrInfo_ipmcast_GroupAddressIPV4Info;
extern L7_char8 *pStrInfo_ipmcast_GroupAddressIPV6Info;

extern L7_char8 *pStrInfo_ipmcast_GroupAddressMaskIPV4Info;

extern L7_char8 *pStrInfo_ipmcast_OverrideInfo;
extern L7_char8 *pStrInfo_ipmcast_CountMessageInfo;
extern L7_char8 *pStrInfo_ipmcast_RPAddressInfo;
extern L7_char8 *pStrInfo_ipmcast_DeleteInfo;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmbsrcandidate;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRCandidateCfg;
extern L7_char8 *pStrInfo_ipmcast_HashMaskLengthIPv4;
extern L7_char8 *pStrInfo_ipmcast_HashMaskLengthIPv6;
extern L7_char8 *pStrInfo_ipmcast_PriorityIPv4;
extern L7_char8 *pStrInfo_ipmcast_PriorityIPv6;
extern L7_char8 *pStrInfo_ipmcast_ScopeIPv6;
extern L7_char8 *pStrInfo_ipmcast_PIMSMV4NotEnabled;
extern L7_char8 *pStrInfo_ipmcast_PIMSMV6NotEnabled;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRCandidateSummary;

extern L7_char8 *pStrInfo_ipmcast_PimSmBSRAddressIPv4;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRAddressIPv6;

extern L7_char8 *pStrInfo_ipmcast_PimSmBSRPriorityIPv4;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRPriorityIPv6;

extern L7_char8 *pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv4;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv6;

extern L7_char8 *pStrInfo_ipmcast_PimSmBSRNextBootStrap;
extern L7_char8 *pStrInfo_ipmcast_PimSmBSRNextCandidateAPAdv;

extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmbsrcandidateRo;

extern L7_char8 *pStrInfo_ipmcast_BSRCandidateSummaryIPv4;
extern L7_char8 *pStrInfo_ipmcast_BSRCandidateSummaryIPv6;
extern L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfg;

extern L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfgIPv4;
extern L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfgIPv6;

extern L7_char8 *pStrInfo_ipmcast_SsmConfigIpv4;
extern L7_char8 *pStrInfo_ipmcast_SsmConfigIpv6;

extern L7_char8 *pStrInfo_ipmcast_SsmConfig;

extern L7_char8 *pStrInfo_ipmcast_SsmRangeFullIPv4;
extern L7_char8 *pStrInfo_ipmcast_SsmRangeFullIPv6;

extern L7_char8 *pStrInfo_ipmcast_SsmGroupAddPref;
extern L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmSsmCfg;
extern L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmSsmcfg;
extern L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmbsrcandidate;
extern L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmrpsetparams;
#endif

