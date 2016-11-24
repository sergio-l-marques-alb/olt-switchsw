/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_common_web.h                                                      
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

#ifndef STRLIB_COMMON_WEB_C
#define STRLIB_COMMON_WEB_C

#include "datatypes.h"
extern L7_char8 *pStrErr_common_FieldInvalidFmt;
extern L7_char8 *pStrErr_common_FieldIeeeMac;
extern L7_char8 *pStrErr_common_FieldOUI;
extern L7_char8 *pStrErr_common_FieldIeeeMac_Unicast;
extern L7_char8 *pStrErr_common_FieldIeeeMac_Nonzero;
extern L7_char8 *pStrErr_common_FieldDottedNetmask;
extern L7_char8 *pStrErr_common_FieldDottedIp;
extern L7_char8 *pStrInfo_common_Br;
extern L7_char8 *pStrErr_common_FailedToSet_1;
extern L7_char8 *pStrErr_common_FailedToSetReason;
extern L7_char8 *pStrErr_common_FailedToSetInt;
extern L7_char8 *pStrErr_common_FailedToSetStr;
extern L7_char8 *pStrInfo_common_Nbsp;
extern L7_char8 *pStrInfo_common_NlsError;
extern L7_char8 *pStrInfo_common_HtmlFileBaseSysFileDownload;
extern L7_char8 *pStrInfo_common_HtmlFileRoutingLoopbackLoopback;
extern L7_char8 *pStrInfo_common_0_1;
extern L7_char8 *pStrInfo_common_00_1;
extern L7_char8 *pStrInfo_common_Dot1d;
extern L7_char8 *pStrInfo_common_Dot1pPri;
extern L7_char8 *pStrInfo_common_Br_2;
extern L7_char8 *pStrInfo_common_Br_4;
extern L7_char8 *pStrInfo_common_Add;
extern L7_char8 *pStrInfo_common_AdminModeForIpv4;
extern L7_char8 *pStrInfo_common_Advertisement;
extern L7_char8 *pStrInfo_common_Apply;
extern L7_char8 *pStrInfo_common_DefInfoOriginNotSet;
extern L7_char8 *pStrInfo_common_AvailableSrc;
extern L7_char8 *pStrInfo_common_AvailableSrcNotSelected;
extern L7_char8 *pStrInfo_common_Back;
extern L7_char8 *pStrInfo_common_Bgp;
extern L7_char8 *pStrInfo_common_BackupDesignatedRtr_1;
extern L7_char8 *pStrInfo_common_Cancel;
extern L7_char8 *pStrInfo_common_Checksum;
extern L7_char8 *pStrInfo_common_ClientQos;
extern L7_char8 *pStrInfo_common_Clr;
extern L7_char8 *pStrInfo_common_ClrAll;
extern L7_char8 *pStrInfo_common_ClrStats;
extern L7_char8 *pStrInfo_common_ComparableCost;
extern L7_char8 *pStrInfo_common_Comp_1;
extern L7_char8 *pStrInfo_common_Cfgure;
extern L7_char8 *pStrInfo_common_CfguredSrc;
extern L7_char8 *pStrInfo_common_ConfirmPasswd;
extern L7_char8 *pStrInfo_common_Copy;
extern L7_char8 *pStrInfo_common_Create;
extern L7_char8 *pStrInfo_common_CreateNssa;
extern L7_char8 *pStrInfo_common_CreateNewVirtualLink;
extern L7_char8 *pStrInfo_common_CreateStubArea;
extern L7_char8 *pStrErr_common_CantDelAreaNotCfgured_1;
extern L7_char8 *pStrErr_common_CantDelAreaActiveIntf_1;
extern L7_char8 *pStrInfo_common_Dhcpv6AdminMode;
extern L7_char8 *pStrInfo_common_DeadIntvlSecs;
extern L7_char8 *pStrInfo_common_DeflMetricVal;
extern L7_char8 *pStrInfo_common_DeflRoute;
extern L7_char8 *pStrInfo_common_DelAll;
extern L7_char8 *pStrInfo_common_DelNssa;
extern L7_char8 *pStrInfo_common_DelStubArea;
extern L7_char8 *pStrInfo_common_DesignatedRtr_1;
extern L7_char8 *pStrInfo_common_DistributeListValForSelectedSrc;
extern L7_char8 *pStrInfo_common_DownloadFileToSwitch;
extern L7_char8 *pStrInfo_common_Encrypt_1;
extern L7_char8 *pStrInfo_common_Error_3;
extern L7_char8 *pStrInfo_common_ExitOverflowIntvlSecs;
extern L7_char8 *pStrInfo_common_ExpiryTime;
extern L7_char8 *pStrInfo_common_ExternalType1Routes;
extern L7_char8 *pStrInfo_common_ExternalType2Routes;
extern L7_char8 *pStrErr_common_FailedToClrDeflMetricVal;
extern L7_char8 *pStrErr_common_FailedToClrDistributeListValForSelectedSrc;
extern L7_char8 *pStrErr_common_FailedToClrMetricValForSelectedSrc;
extern L7_char8 *pStrErr_common_FailedToDelBgpRouteRedistributionForSelectedSrc;
extern L7_char8 *pStrErr_common_FailedToAdd;
extern L7_char8 *pStrErr_common_FailedToClr;
extern L7_char8 *pStrErr_common_FailedToCreate;
extern L7_char8 *pStrErr_common_FailedToCreateReason;
extern L7_char8 *pStrErr_common_FailedToDel;
extern L7_char8 *pStrErr_common_FailedToDelReason;
extern L7_char8 *pStrErr_common_FailedToGet;
extern L7_char8 *pStrErr_common_FailedToReset;
extern L7_char8 *pStrInfo_common_Fast;
extern L7_char8 *pStrInfo_common_FormSubmissionFailedNoActionTaken;
extern L7_char8 *pStrInfo_common_HexadecimalValue;
extern L7_char8 *pStrInfo_common_HoldTimeSecs;
extern L7_char8 *pStrInfo_common_Id;
extern L7_char8 *pStrInfo_common_Ipv4_2;
extern L7_char8 *pStrInfo_common_Ipv6Prefix_1;
extern L7_char8 *pStrInfo_common_IftransitDelayIntvlSecs;
extern L7_char8 *pStrErr_common_Alnum;
extern L7_char8 *pStrInfo_common_IntfEvents;
extern L7_char8 *pStrInfo_common_IntfParams;
extern L7_char8 *pStrInfo_common_IntfStats;
extern L7_char8 *pStrInfo_common_IntfTrustMode;
extern L7_char8 *pStrInfo_common_IntfType_1;
extern L7_char8 *pStrInfo_common_Internal_1;
extern L7_char8 *pStrInfo_common_InternalRoutes;
extern L7_char8 *pStrErr_common_AclIdValidRange1199;
extern L7_char8 *pStrErr_common_Ipv6PrefixAndPrefixLen;
extern L7_char8 *pStrErr_common_FieldInvalidInput2;
extern L7_char8 *pStrErr_common_MetricValValidRange016777214;
extern L7_char8 *pStrErr_common_TagValValidRange04294967295;
extern L7_char8 *pStrErr_common_Val_1;
extern L7_char8 *pStrErr_common_ValFieldNoActionTaken;
extern L7_char8 *pStrInfo_common_LsId;
extern L7_char8 *pStrInfo_common_LsaAckIntvlSecs;
extern L7_char8 *pStrInfo_common_LsaType;
extern L7_char8 *pStrInfo_common_LsdbType;
extern L7_char8 *pStrInfo_common_Lvl7SyssFastpathSwitchApplet;
extern L7_char8 *pStrInfo_common_LinkStatus;
extern L7_char8 *pStrInfo_common_LinkTrap;
extern L7_char8 *pStrInfo_common_Logging_1;
extern L7_char8 *pStrInfo_common_Login;
extern L7_char8 *pStrInfo_common_Loopback;
extern L7_char8 *pStrInfo_common_MstId;
extern L7_char8 *pStrInfo_common_MtuIgnore;
extern L7_char8 *pStrInfo_common_MacAddrIsRestricted;
extern L7_char8 *pStrInfo_common_MgmtUnit_1;
extern L7_char8 *pStrInfo_common_Match;
extern L7_char8 *pStrInfo_common_MatchValForSelectedSrc;
extern L7_char8 *pStrInfo_common_MetricTypeForSelectedSrc;
extern L7_char8 *pStrInfo_common_MetricVal;
extern L7_char8 *pStrInfo_common_MetricValForSelectedSrc;
extern L7_char8 *pStrInfo_common_NA;
extern L7_char8 *pStrInfo_common_Nssa;
extern L7_char8 *pStrInfo_common_NssaExternal;
extern L7_char8 *pStrInfo_common_NssaExternalType1;
extern L7_char8 *pStrInfo_common_NssaExternalType1Routes;
extern L7_char8 *pStrInfo_common_NssaExternalType2;
extern L7_char8 *pStrInfo_common_NssaExternalType2Routes;
extern L7_char8 *pStrInfo_common_NeighborRtrIdCantBeSameAsLocalRtrId;
extern L7_char8 *pStrInfo_common_NwLinks_1;
extern L7_char8 *pStrInfo_common_NextHopIpAddr;
extern L7_char8 *pStrInfo_common_NoNeighborsCfgured;
extern L7_char8 *pStrInfo_common_NoRtrIntfsAvailable;
extern L7_char8 *pStrInfo_common_NoValidAreasAvailable;
extern L7_char8 *pStrInfo_common_NoValidOspfIntfsAvailable;
extern L7_char8 *pStrInfo_common_NoRedistributeMode;
extern L7_char8 *pStrInfo_common_NoSummaryMode;
extern L7_char8 *pStrInfo_common_NonComparableCost;
extern L7_char8 *pStrInfo_common_NonCfguredUsr;
extern L7_char8 *pStrInfo_common_Ospf;
extern L7_char8 *pStrInfo_common_Rlim;
extern L7_char8 *pStrInfo_common_OspfVirtualLinkAuthCfg;
extern L7_char8 *pStrInfo_common_OspfHasNotBeenInitialized;
extern L7_char8 *pStrInfo_common_OneOfCheckboxesInMatchMustBeSelected;
extern L7_char8 *pStrInfo_common_OperationFailed;
extern L7_char8 *pStrInfo_common_OtherDesignatedRtr;
extern L7_char8 *pStrInfo_common_PassiveMode;
extern L7_char8 *pStrInfo_common_Passwd_1;
extern L7_char8 *pStrInfo_common_PasswdAndConfirmPasswdFieldsDoNotMatch;
extern L7_char8 *pStrErr_common_ImageReqDenied;
extern L7_char8 *pStrInfo_common_PointToPoint;
extern L7_char8 *pStrInfo_common_Lag;
extern L7_char8 *pStrInfo_common_PortDesc;
extern L7_char8 *pStrInfo_common_Pref;
extern L7_char8 *pStrInfo_common_PrefixPrefixlength_2;
extern L7_char8 *pStrInfo_common_RejectRoute;
extern L7_char8 *pStrInfo_common_Reachable;
extern L7_char8 *pStrInfo_common_Refresh;
extern L7_char8 *pStrInfo_common_Remove;
extern L7_char8 *pStrInfo_common_Reset;
extern L7_char8 *pStrInfo_common_RespTimeSecs;
extern L7_char8 *pStrInfo_common_ReTxIntvlSecs;
extern L7_char8 *pStrInfo_common_RtrLinks_1;
extern L7_char8 *pStrInfo_common_SpfRuns;
extern L7_char8 *pStrInfo_common_StpMode;
extern L7_char8 *pStrInfo_common_Search;
extern L7_char8 *pStrInfo_common_Seq_1;
extern L7_char8 *pStrInfo_common_Start_1;
extern L7_char8 *pStrInfo_common_StartFileTransfer;
extern L7_char8 *pStrInfo_common_StubArea;
extern L7_char8 *pStrInfo_common_StubAreaInfo;
extern L7_char8 *pStrInfo_common_SubmissionFailure;
extern L7_char8 *pStrInfo_common_Submit;
extern L7_char8 *pStrInfo_common_SubnetMaskMustHaveContiguousOnes;
extern L7_char8 *pStrInfo_common_TftpSrvrAddr;
extern L7_char8 *pStrInfo_common_TagValForSelectedSrc;
extern L7_char8 *pStrInfo_common_TagToDeflVal;
extern L7_char8 *pStrInfo_common_AuthPasswdIsOutOfRangeValidRangeIsToAlnumChars;
extern L7_char8 *pStrInfo_common_MostSignificantByteOfMacAddrShldHaveMaskXxxxXx10;
extern L7_char8 *pStrInfo_common_TrafficClass;
extern L7_char8 *pStrInfo_common_TransitAreaCantBeAStubNssa;
extern L7_char8 *pStrErr_common_FieldOutOfRange;
extern L7_char8 *pStrErr_common_FieldOutOfRangeUint;
extern L7_char8 *pStrErr_common_PortAlreadyInUse;
extern L7_char8 *pStrInfo_common_VirtualLink;
extern L7_char8 *pStrInfo_common_Waiting_1;
extern L7_char8 *pStrInfo_common_Addnew;
extern L7_char8 *pStrInfo_common_ClrStats_1;
extern L7_char8 *pStrInfo_common_Hours_2;
extern L7_char8 *pStrInfo_common_HtmlFileBaseSysHelpIdx;
extern L7_char8 *pStrInfo_common_Iso88591;
extern L7_char8 *pStrInfo_common_LoopbackX;
extern L7_char8 *pStrInfo_common_HtmlFileOspfVlinkSumm;
extern L7_char8 *pStrInfo_common_HtmlFileStatsRtrOspfLsdb;
extern L7_char8 *pStrInfo_common_To_5;
extern L7_char8 *pStrInfo_common_TunnelX;
extern L7_char8 *pStrInfo_common_web_None_4;
extern L7_char8 *pStrErr_common_FaildsFlowOwner;
extern L7_char8 *pStrErr_common_InvalidsFlowOwner;
extern L7_char8 *pStrErr_common_InvalidsFlowRcvrTimeout;
extern L7_char8 *pStrErr_common_UnclaimsFlowOwner;
extern L7_char8 *pStrinfo_common_sFlowUnclaimRcvr;
extern L7_char8 *pStrinfo_common_sFlowUnclaimRcvr_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrRcvrTimeout;
extern L7_char8 *pStrinfo_common_sFlowRcvrMaxSize;
extern L7_char8 *pStrinfo_common_sFlowRcvrAddrType;
extern L7_char8 *pStrinfo_common_sFlowRcvrPort;
extern L7_char8 *pStrinfo_common_sFlowRcvrDVersion;
extern L7_char8 *pStrinfo_common_sFlowPollerInterval;
extern L7_char8 *pStrinfo_common_sFlowMaxSize;
extern L7_char8 *pStrinfo_common_sFlowRcvrIndex;
extern L7_char8 *pStrinfo_common_sFlowSamplingRate;
extern L7_char8 *pStrinfo_common_sFlowRcvrAddress_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrDataVersion_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrOwner_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrTimeout_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrDataSize_1;
extern L7_char8 *pStrinfo_common_sFlowRcvrPort_1;
extern L7_char8 *pStrinfo_common_InactivesFlowRcvr;
extern L7_char8 *pStrinfo_common_sFlowRcvr_1;
extern L7_char8 *pStrinfo_common_sFlowCpInterval;
extern L7_char8 *pStrinfo_common_sFlowFsMaxHeaderSize;
extern L7_char8 *pStrinfo_common_sFlowFsSamplingRate;
extern L7_char8 *pStrInfo_common_RuleIpMac;
extern L7_char8 *pStrInfo_common_Block;
extern L7_char8 *pStrInfo_common_Unblock;
extern L7_char8 *pStrErr_common_ValForValidRange;
extern L7_char8 *pStrInfo_common_InvalidMacAddr;
#endif

