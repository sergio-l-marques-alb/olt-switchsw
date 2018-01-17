/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/switching/oem_switching.c
*
* @purpose    OEM String Helper Functions
*
* @component  WEB
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

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_switching_common.h"
#include "strlib_switching_web.h"
#include "strlib_base_web.h"
#include "datatypes.h"

L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebSwitchingGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebSwitchingGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1004:
    errMsg = pStrInfo_common_MacAddr_2;
    break;
  case 1005:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1090:
    errMsg = pStrInfo_common_SubnetMask_3;
    break;
  case 1102:
    errMsg = pStrInfo_common_IgmpProxyAdminMode;
    break;
  case 1105:
    errMsg = pStrInfo_common_LinkStatus;
    break;
  case 1106:
    errMsg = pStrInfo_common_LinkTrap;
    break;
  case 1108:
    errMsg = pStrInfo_common_ApProfileRadioScanFrequencyAll;
    break;
  case 1205:
    errMsg = pStrInfo_switching_PortId_2;
    break;
  case 1214:
    errMsg = pStrInfo_switching_SpanTreeAdminMode;
    break;
  case 1230:
    errMsg = pStrInfo_common_SysName;
    break;
  case 1337:
    errMsg = pStrInfo_common_StpMode;
    break;
  case 1339:
    errMsg = pStrInfo_common_Dot1d;
    break;
  case 1340:
    errMsg = pStrInfo_common_Fast;
    break;
  case 1341:
    errMsg = pStrInfo_common_ApProfileRadioStatusOff;
    break;
  case 1479:
    errMsg = pStrInfo_common_LastUpdate;
    break;
  case 1538:
    errMsg = pStrInfo_common_AlarmType;
    break;
  case 1639:
    errMsg = pStrInfo_common_Intf;
    break;
  case 1883:
    errMsg = pStrInfo_switching_NoMstsAvailable;
    break;
  case 2057:
    errMsg = pStrInfo_switching_GrpName;
    break;
  case 2058:
    errMsg = pStrInfo_switching_GrpId;
    break;
  case 2059:
    errMsg = pStrInfo_switching_Protos;
    break;
  case 2060:
    errMsg = pStrInfo_common_VapVlan;
    break;
  case 2061:
    errMsg = pStrInfo_switching_Intfs;
    break;
  case 2111:
    errMsg = pStrInfo_common_Comp_1;
    break;
  case 2113:
    errMsg = pStrInfo_common_MaxMfdbTblEntries;
    break;
  case 2114:
    errMsg = pStrInfo_common_MostMfdbEntriesSinceLastReset;
    break;
  case 2115:
    errMsg = pStrInfo_common_CurrentEntries;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 4133:
    errMsg = pStrInfo_common_LinkState;
    break;
  case 4168:
    errMsg = pStrInfo_switching_MacFilter;
    break;
  case 4169:
    errMsg = pStrInfo_switching_SrcPortMask;
    break;
  case 4170:
    errMsg = pStrInfo_switching_DstPortMask;
    break;
  case 4181:
    errMsg = pStrInfo_switching_SrcPortMbrs;
    break;
  case 4182:
    errMsg = pStrInfo_switching_DstPortMbrs;
    break;
  case 5003:
    errMsg = pStrInfo_common_ApStatus;
    break;
  case 5308:
    errMsg = pStrInfo_switching_ListingOfAllPortsOnSwitch;
    break;
  case 5310:
    errMsg = pStrInfo_common_Desc_1;
    break;
  case 5904:
    errMsg = pStrInfo_switching_SwitchGvrp;
    break;
  case 5906:
    errMsg = pStrInfo_switching_SwitchGmrp;
    break;
  case 5999:
    errMsg = pStrInfo_switching_AreYouSureYouWishToResetAllVlanCfg;
    break;
  case 6003:
    errMsg = pStrInfo_switching_GvrpMode;
    break;
  case 6005:
    errMsg = pStrInfo_switching_GarpTimers;
    break;
  case 6006:
    errMsg = pStrInfo_switching_JoinTimerCentisecs;
    break;
  case 6007:
    errMsg = pStrInfo_switching_LeaveTimerCentisecs;
    break;
  case 6008:
    errMsg = pStrInfo_switching_LeaveAllTimerCentisecs;
    break;
  case 6010:
    errMsg = pStrInfo_switching_ExercisingFuncWillCauseAllVlanCfgParamsToBeResetToDeflVals;
    break;
  case 6012:
    errMsg = pStrInfo_switching_VlanName_1;
    break;
  case 6013:
    errMsg = pStrInfo_switching_VlanType;
    break;
  case 6014:
    errMsg = pStrInfo_common_VlanId_1;
    break;
  case 6021:
    errMsg = pStrInfo_switching_PortVlanId;
    break;
  case 6022:
    errMsg = pStrInfo_switching_AcceptableFrameTypes;
    break;
  case 6023:
    errMsg = pStrInfo_switching_IngressFiltering;
    break;
  case 6024:
    errMsg = pStrInfo_switching_PortGvrpMode;
    break;
  case 6026:
    errMsg = pStrInfo_switching_VlanOnly;
    break;
  case 6027:
    errMsg = pStrInfo_switching_AdmitAll;
    break;
  case 6030:
    errMsg = pStrInfo_switching_VlanIdAndName;
    break;
  case 6032:
    errMsg = pStrInfo_switching_Tagging;
    break;
  case 6035:
    errMsg = pStrInfo_switching_Participation;
    break;
  case 6037:
    errMsg = pStrInfo_common_Incl;
    break;
  case 6038:
    errMsg = pStrInfo_common_Excl;
    break;
  case 6039:
    errMsg = pStrInfo_switching_AutoDetect;
    break;
  case 6040:
    errMsg = pStrInfo_switching_Tagged;
    break;
  case 6041:
    errMsg = pStrInfo_switching_UnTagged;
    break;
  case 6068:
    errMsg = pStrInfo_switching_PortVlanIdCfgured;
    break;
  case 6069:
    errMsg = pStrInfo_switching_PortVlanIdCurrent;
    break;
  case 6070:
    errMsg = pStrInfo_switching_IngressFilteringCfgured;
    break;
  case 6071:
    errMsg = pStrInfo_switching_IngressFilteringCurrent;
    break;
  case 6209:
    errMsg = pStrInfo_switching_LagName;
    break;
  case 6215:
    errMsg = pStrInfo_common_AdministrativeMode;
    break;
  case 6230:
    errMsg = pStrInfo_switching_LagMbrs;
    break;
  case 6231:
    errMsg = pStrInfo_switching_LoadBalance;
    break;
  case 6232:
    errMsg = pStrInfo_switching_HashModeSaVlan;
    break;
  case 6233:
    errMsg = pStrInfo_switching_HashModeDaVlan;
    break;
  case 6234:
    errMsg = pStrInfo_switching_HashModeSaDaVlan;
    break;
  case 6235:
    errMsg = pStrInfo_switching_HashModeSrcIP;
    break;
  case 6236:
    errMsg = pStrInfo_switching_HashModeDestIP;
    break;
  case 6237:
    errMsg = pStrInfo_switching_HashModeSrcDestIP;
    break;
  case 6305:
    errMsg = pStrInfo_switching_MbrshipConflicts;
    break;
  case 7041:
    errMsg = pStrInfo_common_SysDesc_1;
    break;
  case 7302:
    errMsg = pStrInfo_common_Lag;
    break;
  case 7504:
    errMsg = pStrInfo_common_McastCntrlFrameCount;
    break;
  case 7505:
    errMsg = pStrInfo_switching_DataFramesForwardedByCpu;
    break;
  case 7506:
    errMsg = pStrInfo_common_IntfsEnbldForIgmpSnooping;
    break;
  case 7524:
    errMsg = pStrInfo_switching_VlanIdsEnbldForIgmpSnooping;
    break;
  case 7527:
    errMsg = pStrInfo_common_IntfsEnbldForMldSnooping;
    break;
  case 7528:
    errMsg = pStrInfo_switching_VlanIdsEnbldForMldSnooping;
    break;
  case 7530:
    errMsg = pStrInfo_switching_SnoopingQuerierAddr;
    break;
  case 7531:
    errMsg = pStrInfo_common_IgmpVer;
    break;
  case 7532:
    errMsg = pStrInfo_switching_QueryIntvl;
    break;
  case 7533:
    errMsg = pStrInfo_switching_SnoopQuerierQuerierInt;
    break;
  case 7534:
    errMsg = pStrInfo_switching_SnoopingQuerierAdminMode;
    break;
  case 7535:
    errMsg = pStrInfo_common_MldVer;
    break;
  case 7537:
    errMsg = pStrInfo_switching_SnoopingQuerierVlanAddr;
    break;
  case 7538:
    errMsg = pStrInfo_switching_SnoopQuerier_1;
    break;
  case 7539:
    errMsg = pStrInfo_switching_SnoopQuerierOperState;
    break;
  case 7540:
    errMsg = pStrInfo_switching_OperationalVer;
    break;
  case 7541:
    errMsg = pStrInfo_switching_OperationalMaxRespTime;
    break;
  case 7542:
    errMsg = pStrInfo_switching_SnoopLastQuerierAddr;
    break;
  case 7543:
    errMsg = pStrInfo_switching_SnoopLastQuerierVer;
    break;
  case 7600:
    errMsg = pStrInfo_switching_PortGmrpMode;
    break;
  case 7601:
    errMsg = pStrInfo_switching_GmrpMode;
    break;
  case 8808:
    errMsg = pStrInfo_common_Pri_1;
    break;
  case 8867:
    errMsg = pStrInfo_common_Grp_1;
    break;
  case 12391:
    errMsg = pStrInfo_switching_0Auto;
    break;
  case 12393:
    errMsg = pStrInfo_common_PortRole;
    break;
  case 12399:
    errMsg = pStrInfo_common_mstid;
    break;
  case 12400:
    errMsg = pStrInfo_switching_CfgFmtSelectOr;
    break;
  case 12401:
    errMsg = pStrInfo_switching_CfgName;
    break;
  case 12402:
    errMsg = pStrInfo_switching_CfgRevisionLvl;
    break;
  case 12403:
    errMsg = pStrInfo_common_MstId;
    break;
  case 12405:
    errMsg = pStrInfo_switching_Fid;
    break;
  case 12406:
    errMsg = pStrInfo_switching_Vid_1;
    break;
  case 12408:
    errMsg = pStrInfo_switching_BridgePri;
    break;
  case 12409:
    errMsg = pStrInfo_switching_BridgeMaxAgeSecs;
    break;
  case 12410:
    errMsg = pStrInfo_switching_BridgeHelloTimeSecs;
    break;
  case 12411:
    errMsg = pStrInfo_switching_BridgeForwardDelaySecs;
    break;
  case 12413:
    errMsg = pStrInfo_switching_TimeSinceTopologyChg;
    break;
  case 12414:
    errMsg = pStrInfo_switching_TopologyChgCount;
    break;
  case 12415:
    errMsg = pStrInfo_switching_TopologyChg;
    break;
  case 12416:
    errMsg = pStrInfo_switching_DesignatedRoot;
    break;
  case 12417:
    errMsg = pStrInfo_switching_DesignatedPortCost;
    break;
  case 12418:
    errMsg = pStrInfo_switching_RootPort_1;
    break;
  case 12419:
    errMsg = pStrInfo_switching_MaxAgeSecs;
    break;
  case 12420:
    errMsg = pStrInfo_switching_ForwardDelaySecs;
    break;
  case 12421:
    errMsg = pStrInfo_common_HoldTimeSecs;
    break;
  case 12422:
    errMsg = pStrInfo_switching_CstRegionalRoot;
    break;
  case 12423:
    errMsg = pStrInfo_switching_CstPathCost_1;
    break;
  case 12425:
    errMsg = pStrInfo_switching_BridgeId;
    break;
  case 12429:
    errMsg = pStrInfo_switching_ForceProtoVer;
    break;
  case 12430:
    errMsg = pStrInfo_switching_DesignatedCost;
    break;
  case 12432:
    errMsg = pStrInfo_switching_PortPri;
    break;
  case 12433:
    errMsg = pStrInfo_switching_PortPathCost;
    break;
  case 12434:
    errMsg = pStrInfo_switching_AdminEdgePort;
    break;
  case 12436:
    errMsg = pStrInfo_switching_EdgePort;
    break;
  case 12437:
    errMsg = pStrInfo_switching_PointToPointMac;
    break;
  case 12438:
    errMsg = pStrInfo_switching_TopologyChgAck;
    break;
  case 12439:
    errMsg = pStrInfo_switching_DesignatedBridge;
    break;
  case 12440:
    errMsg = pStrInfo_switching_DesignatedPort_1;
    break;
  case 12441:
    errMsg = pStrInfo_switching_HelloTimeSecs;
    break;
  case 12442:
    errMsg = pStrInfo_switching_Ieee802Dot1D;
    break;
  case 12443:
    errMsg = pStrInfo_switching_Ieee802Dot1W;
    break;
  case 12444:
    errMsg = pStrInfo_switching_Ieee802Dot1S;
    break;
  case 12446:
    errMsg = pStrInfo_common_StpBpdusRcvd;
    break;
  case 12447:
    errMsg = pStrInfo_common_StpBpdusTxed;
    break;
  case 12448:
    errMsg = pStrInfo_common_RstpBpdusRcvd;
    break;
  case 12449:
    errMsg = pStrInfo_common_RstpBpdusTxed;
    break;
  case 12450:
    errMsg = pStrInfo_common_MstpBpdusRcvd;
    break;
  case 12451:
    errMsg = pStrInfo_common_MstpBpdusTxed;
    break;
  case 12452:
    errMsg = pStrInfo_switching_CfgDigestKey;
    break;
  case 12481:
    errMsg = pStrInfo_switching_PortFwdState;
    break;
  case 12491:
    errMsg = pStrInfo_switching_PortMode;
    break;
  case 12492:
    errMsg = pStrInfo_switching_PortUpTimeSinceCountersLastClred;
    break;
  case 12542:
    errMsg = pStrInfo_switching_LoopInconsistentState;
    break;
  case 12543:
    errMsg = pStrInfo_switching_TransitionsIntoLoopInconsistentState;
    break;
  case 12544:
    errMsg = pStrInfo_switching_TransitionsOutOfLoopInconsistentState;
    break;
  case 12834:
    errMsg = pStrInfo_switching_BpduGuard;
    break;
  case 12835:
    errMsg = pStrInfo_switching_BpduFilter;
    break;
  case 12837:
    errMsg = pStrInfo_switching_BpduFlood;
    break;
  case 12838:
    errMsg = pStrInfo_switching_BpduGuardEffect;
    break;
  case 14004:
    errMsg = pStrInfo_switching_AutoCalculatePortPathCost;
    break;
  case 14005:
    errMsg = pStrInfo_switching_AutoEdge;
    break;
  case 14006:
    errMsg = pStrInfo_switching_RootGuard;
    break;
  case 14007:
    errMsg = pStrInfo_switching_TcnGuard;
    break;
  case 14008:
    errMsg = pStrInfo_switching_LoopGuard;
    break;
  case 16300:
    errMsg = pStrInfo_common_Dot1pPri;
    break;
  case 16301:
    errMsg = pStrInfo_common_TrafficClass;
    break;
  case 16302:
    errMsg = pStrInfo_switching_Mst;
    break;
  case 18011:
    errMsg = pStrInfo_switching_FastLeaveAdminMode;
    break;
  case 18012:
    errMsg = pStrInfo_common_GrpMbrshipIntvl;
    break;
  case 18013:
    errMsg = pStrInfo_common_MaxRespTime;
    break;
  case 18014:
    errMsg = pStrInfo_common_McastRtrExpiryTime;
    break;
  case 20019:
    errMsg = pStrInfo_switching_GvrpMustFirstBeEnbldOnSwitchBeforeYouCanPerformGarpCfgOnPortreIsAlsoUpToA10SecondDelayOnCfgIfYouHaveAlreadyEnbldGvrpOnSwitchTryGoingToPageAgain;
    break;
  case 20057:
    errMsg = pStrInfo_switching_LagType;
    break;
  case 20058:
    errMsg = pStrInfo_switching_CfguredPorts;
    break;
  case 20059:
    errMsg = pStrInfo_switching_ActivePorts;
    break;
  case 21232:
    errMsg = pStrInfo_switching_McastRtr;
    break;
  case 21250:
    errMsg = pStrInfo_common_IntfTrustMode;
    break;
  case 21501:
    errMsg = pStrInfo_switching_StaticMode;
    break;
  case 21503:
    errMsg = pStrInfo_common_PortDesc;
    break;
  case 21504:
    errMsg = pStrInfo_switching_TxTotal;
    break;
  case 21505:
    errMsg = pStrInfo_switching_ReceiveTotal;
    break;
  case 21506:
    errMsg = pStrInfo_common_OspfRxDiscard;
    break;
  case 21507:
    errMsg = pStrInfo_switching_Errs;
    break;
  case 21508:
    errMsg = pStrInfo_switching_Ageouts;
    break;
  case 21509:
    errMsg = pStrInfo_switching_TlvDiscards;
    break;
  case 21510:
    errMsg = pStrInfo_switching_TlvUnknowns;
    break;
  case 21511:
    errMsg = pStrInfo_common_TotalInserts;
    break;
  case 21512:
    errMsg = pStrInfo_common_TotalDels;
    break;
  case 21513:
    errMsg = pStrInfo_common_TotalDrops;
    break;
  case 21514:
    errMsg = pStrInfo_switching_LocalIntf_1;
    break;
  case 21515:
    errMsg = pStrInfo_common_TxIntvl;
    break;
  case 21516:
    errMsg = pStrInfo_switching_SysCapabilitiesSupported_1;
    break;
  case 21517:
    errMsg = pStrInfo_switching_MgmtAddr;
    break;
  case 21518:
    errMsg = pStrInfo_switching_RemoteDevice;
    break;
  case 21519:
    errMsg = pStrInfo_common_TxHoldMultiplier;
    break;
  case 21520:
    errMsg = pStrInfo_common_TotalAgeouts;
    break;
  case 21521:
    errMsg = pStrInfo_switching_ChassisIdSubtype_1;
    break;
  case 21522:
    errMsg = pStrInfo_switching_ChassisId_1;
    break;
  case 21523:
    errMsg = pStrInfo_switching_PortIdSubtype_1;
    break;
  case 21524:
    errMsg = pStrInfo_switching_SysCapabilitiesEnbld_1;
    break;
  case 21525:
    errMsg = pStrInfo_switching_TimeToLive_1;
    break;
  case 21526:
    errMsg = pStrInfo_switching_ReInitializationDelay;
    break;
  case 21527:
    errMsg = pStrInfo_common_NotificationIntvl;
    break;
  case 21528:
    errMsg = pStrInfo_switching_Tx;
    break;
  case 21529:
    errMsg = pStrInfo_switching_Receive;
    break;
  case 21530:
    errMsg = pStrInfo_switching_TxMgmtInfo;
    break;
  case 21531:
    errMsg = pStrInfo_switching_Notify;
    break;
  case 21532:
    errMsg = pStrInfo_switching_SysCapabilities;
    break;
  case 21533:
    errMsg = pStrInfo_switching_OptionalTlvS;
    break;
  case 22048:
    errMsg = pStrInfo_switching_ProtectedPortS;
    break;
  case 22049:
    errMsg = pStrInfo_switching_RemoteId;
    break;
  case 22061:
    errMsg = pStrInfo_switching_NoIpSubnetBasedVlanCfgured;
    break;
  case 22066:
    errMsg = pStrInfo_common_NoLldpDataHasBeenRcvdOnIntf;
    break;
  case 22067:
    errMsg = pStrInfo_switching_NoMacBasedVlanCfgured;
    break;
  case 22075:
    errMsg = pStrInfo_common_NoLocalIntfsAreEnbldToTxLldpData;
    break;
  case 22076:
    errMsg = pStrInfo_common_NoLocalIntfsAreEnbldToReceiveLldpData;
    break;
  case 22077:
    errMsg = pStrInfo_common_NoLocalIntfsAreEnbldToTxReceiveLldpData;
    break;
  case 23107:
    errMsg = pStrInfo_switching_TrustMode;
    break;
/* med*/
  case 23108:
    errMsg = pStrInfo_switching_FastStartRepeatCount;
    break;
  case 23109:
     errMsg = pStrInfo_switching_DeviceClass;
     break;
  case 23110:
     errMsg = pStrInfo_switching_LLDPMedMode;
     break;
  case 23111:
     errMsg = pStrInfo_switching_ConfigNotifyMode;
     break;
  case 23112:
     errMsg = pStrInfo_switching_TransmittedTLVs;
     break;
  case 23113:
     errMsg = pStrInfo_switching_MEDCapabilities;
     break;
  case 23114:
     errMsg = pStrInfo_switching_NetworkPolicy;
     break;
  case 23115:
     errMsg = pStrInfo_switching_LocationIdentification;
     break;
  case 23116:
     errMsg = pStrInfo_switching_ExtendedPowerViaMDIPSE;
     break;
  case 23117:
     errMsg = pStrInfo_switching_ExtendedPowerViaMDIPD;
     break;
  case 23118:
     errMsg = pStrInfo_switching_Inventory;
     break;
  case 23119:
     errMsg = pStrInfo_common_Link_Status;
     break;
  case 23120:
     errMsg = pStrInfo_common_WsOperatingStatus;
     break;
  case 23121:
     errMsg = pStrInfo_switching_MEDStatus;
     break;
  case 23122:
     errMsg = pStrInfo_switching_NotificationStatus;
     break;
  case 23123:
     errMsg = pStrInfo_switching_NetworkPoliciesInformation;
     break;
  case 23124:
     errMsg = pStrInfo_switching_MediaApplicationType;
     break;
  case 23125:
     errMsg = pStrInfo_common_Pri_1;
     break;
  case 23126:
     errMsg = pStrInfo_switching_DSCP;
     break;
  case 23127:
     errMsg = pStrInfo_switching_TaggedBitStatus;
     break;
  case 23128:
     errMsg = pStrInfo_switching_UnknownBitStatus;
     break;
  case 23129:
     errMsg = pStrInfo_common_ApSerialNum;
     break;
  case 23130:
     errMsg = pStrInfo_switching_InventoryInformation;
     break;
  case 23131:
       errMsg = pStrInfo_switching_HardwareRevision;
       break;
  case 23132:
        errMsg = pStrInfo_switching_FirmwareRevision;
        break;
  case 23133:
        errMsg = pStrInfo_switching_SoftwareRevision;
        break;
  case 23134:
        errMsg = pStrInfo_switching_ManufacturerName;
        break;
  case 23135:
        errMsg = pStrInfo_switching_ModelName;
        break;
  case 23136:
        errMsg = pStrInfo_switching_AssetID;
        break;
  case 23137:
        errMsg = pStrInfo_switching_LocationInformation;
        break;
  case 23138:
        errMsg = pStrInfo_switching_SubType;
        break;
  case 23139:
        errMsg = pStrInfo_switching_ExtendedPoE;
        break;
  case 23140:
        errMsg = pStrInfo_switching_DeviceType;
        break;
  case 23141:
        errMsg = pStrInfo_switching_LocationInformation_1;
        break;
  case 23142:
        errMsg = pStrInfo_switching_Available;
        break;
  case 23143:
        errMsg = pStrInfo_switching_Source;
        break;
  case 23144:
        errMsg = pStrInfo_switching_ExtendedPoEPD;
        break;
  case 23145:
        errMsg = pStrInfo_switching_Required;
        break;
  case 23146:
     errMsg = pStrInfo_switching_ExtendedPoEPSE;
     break;
  case 23147:
     errMsg = pStrInfo_switching_LocalinterfacesarenotenabledtotransmitLLDPMEDdata;
     break;
  case 23148:
     errMsg = pStrInfo_switching_NoLLDPMEDLocaldatapresentonthisinterface;
     break;
  case 23149:  
     errMsg = pStrInfo_switching_CapabilityInformation;
     break;
  case 23150:  
     errMsg = pStrInfo_switching_CapabilitiesSupported;
     break;
  case 23151:  
     errMsg = pStrInfo_switching_CababilitiesEnabled;
     break;
  case 23152:
     errMsg = pStrInfo_switching_Remoteclientsarenotavaiableonthisinterface;
     break;
  case 23153:
     errMsg = pStrInfo_switching_LocalinterfacesarenotenabledtoRecieveLLDPMEDdata;
     break;
  case 23154:
     errMsg = pStrInfo_switching_NoLLDPMEDremotedatarecievedonthisinterface;
     break;
  case 23155:
     errMsg = pStrInfo_switching_VoiceVlanAdminModeDisplay;
     break;
  case 23156:
     errMsg = pStrInfo_switching_VoiceVlanIntfMode;
     break;
  case 23157:
     errMsg = pStrInfo_switching_VoiceVlanCosOverRideMode;
     break;
  case 23160:
     errMsg = pStrInfo_switching_DynamicArpInspection;
     break;
  case 23161:
     errMsg = pStrInfo_switching_LoggingInvalidPackets;
     break;
  case 23162:
     errMsg = pStrInfo_switching_ArpACLName;
     break;
  case 23163:
     errMsg = pStrInfo_switching_StaticFalg;     
     break;
  case 23164:
     errMsg = pStrInfo_switching_SenderIPAddress;
     break;
  case 23165:
     errMsg = pStrInfo_switching_SenderMACAddress;
     break;
  case 23166:
     errMsg = pStrInfo_switching_TrustedState;
     break;
  case 23167:
     errMsg = pStrInfo_switching_RateLimit;
     break;
  case 23168:
     errMsg = pStrInfo_switching_BurstInterval;
     break;
  case 23169:
     errMsg = pStrInfo_switching_SrcMacValidate ;
     break;
  case 23170:
     errMsg = pStrInfo_switching_DestMacValidate;
     break;
  case 23171:
     errMsg = pStrInfo_switching_IpAddrValidate ;
     break;
  case 23172:
     errMsg = pStrInfo_switching_DhcpDrops;
     break;
  case 23173:
    errMsg = pStrInfo_switching_AclDrops;
    break;
  case 23174:
    errMsg = pStrInfo_switching_DhcpPermits;
    break;
  case 23175:
    errMsg = pStrInfo_switching_AclPermits;
    break;
  case 23176:
     errMsg = pStrInfo_switching_BadSrcMac;
     break;
  case 23177:
    errMsg = pStrInfo_switching_BadDestMac;
    break;
  case 23178:
     errMsg = pStrInfo_switching_InvalidIP;
     break;
  case 23179:
     errMsg = pStrInfo_switching_Forwarded;
     break;
  case 23180:
     errMsg = pStrInfo_switching_Dropped;
     break;
  case 23181:
     errMsg = pStrInfo_switching_NoArpAclsFound;
     break;
  case 23182:
     errMsg = pStrInfo_switching_NoDaiVlansFound;
     break;
  case 24000:
     errMsg = pStrInfo_switching_DhcpSnoopingMode;
     break;
  case 24001:
     errMsg = pStrInfo_switching_VerifyMacAddress;
     break;
  case 24002:
     errMsg = pStrInfo_switching_StoreLocally;
     break;
  case 24003:
     errMsg = pStrInfo_switching_RemoteFileName;
     break;
  case 24004:
     errMsg = pStrInfo_switching_RemoteIpAddress;
     break;
  case 24005:
     errMsg = pStrInfo_switching_WriteDelay;
     break;
  case 24006:
     errMsg = pStrInfo_switching_MacFails;
     break;
  case 24007:
     errMsg = pStrInfo_switching_ClientIfcMismatch;
     break;
  case 24008:
     errMsg = pStrInfo_switching_DHCPServerMsgsRcvd;
     break;
  case 24009:
     errMsg = pStrInfo_switching_NoDHCPEnabledIntfFound;
     break;
  case 24010:
      errMsg = pStrInfo_switching_Expiry;
      break;
  case 24011:
     errMsg = pStrInfo_switching_LeaseTime; 
     break;
  case 24012:
     errMsg = pStrInfo_switching_IPSG;
     break;
  case 24013:
     errMsg = pStrInfo_switching_PortSecurity;
     break;
  case 24014:
     errMsg = pStrInfo_switching_FilterType;
     break;
  case 24051:
     errMsg = pStrInfo_switching_DhcpL2RelayMode; 
     break;
  case 24052:
     errMsg = pStrInfo_switching_DhcpL2RelayTrustedState; 
     break;
  case 24053:
     errMsg = pStrInfo_switching_DhcpL2RelayCircuitIdMode; 
     break;
  case 24054:
     errMsg = pStrInfo_switching_DhcpL2RelayRemoteIdString; 
     break;
  case 24055:
    errMsg = pStrInfo_switching_DhcpL2RelayUntrustedSrvMsgsWithOpt82;
    break;
  case 24056:
    errMsg = pStrInfo_switching_DhcpL2RelayUntrustedCliMsgsWithOpt82;
    break;
  case 24057:
    errMsg = pStrInfo_switching_DhcpL2RelayTrustedSrvMsgsWithoutOpt82;
    break;
  case 24058:
    errMsg = pStrInfo_switching_DhcpL2RelayTrustedliMsgsWithoutOpt82;
    break;

  case 24059:
     errMsg = pStrInfo_common_Dot3ahTxMultiplier;
    break;

   case 24060:
     errMsg = pStrInfo_common_Dot3ahLinkTimeOut;
    break;

   case 24061:
     errMsg = pStrInfo_common_Dot3ahRemLbTimeOut;
    break;

   case 24062:
     errMsg = pStrInfo_common_Dot3ahErrorFrameThresholdHigh;
    break;

   case 24063:
     errMsg = pStrInfo_common_Dot3ahErrorFrameThresholdLow;
    break;

   case 24064:
     errMsg = pStrInfo_common_Dot3ahErrorFrameThresholdWindow;
    break;

   case 24065:
     errMsg = pStrInfo_common_Dot3ahErrorFrameThreshold;
    break;
   
   case 24066:
     errMsg = pStrInfo_common_Dot3ah3;
    break;

    case 24067:
     errMsg = pStrInfo_common_Dot3ahEntityMode;
    break;

    case 24068:
     errMsg = pStrInfo_common_Dot3ahActiveMode;
    break;

    case 24069:
     errMsg = pStrInfo_common_Dot3ahPassiveMode;
    break;

    case 24070:
     errMsg = pStrInfo_switching_RemLbCapabilities;
    break;
    case 24071:
     errMsg = pStrInfo_switching_LinkEventCapabilities;
    break;
    case 24072:
     errMsg = pStrInfo_switching_OrgSpecInfoCapabilities;
    break;
    case 24073:
     errMsg = pStrInfo_switching_OrgSpecEventCapabilities;
    break;
    case 24074:
     errMsg = pStrInfo_switching_OrgSpecTLVCapabilities;
    break;

    case 24075:
     errMsg = pStrInfo_common_NoLocalIntfsAreEnbldToReceiveDot3ahData;
    break;

    case 24076:
     errMsg = pStrInfo_common_NoLocalIntfsAreEnbldToLANOAMData;
    break;

    case 24077:
     errMsg = pStrInfo_switching_Dot3ahLocalRevision;
    break;

    case 24078:
     errMsg = pStrInfo_switching_Dot3ahRemRevision;
    break;

    case 24079:
     errMsg = pStrInfo_switching_Dot3ahTxNumOAMPDUS;
    break;

    case 24080:
     errMsg = pStrInfo_switching_Dot3ahRxNumOAMPDUS;
    break;

    case 24081:
     errMsg = pStrInfo_switching_Dot3ahTxNumErrOAMPDUS;
    break;

    case 24082:
     errMsg = pStrInfo_switching_Dot3ahRxNumErrOAMPDUS;
    break;

    case 24083:
     errMsg = pStrInfo_switching_Dot3ahNumTimesNodesAdded;
    break;

    case 24084:
     errMsg = pStrInfo_switching_Dot3ahCurrNumNodes;
    break;

    case 24085:
     errMsg = pStrInfo_switching_Dot3ahNumTimesNodesDeleted;
    break;

    case 24086:
     errMsg = pStrInfo_switching_Dot3ahNumTxInfoPdu;
    break;

    case 24087:
     errMsg = pStrInfo_switching_Dot3ahNumTxRemLbPdu;
    break;

    case 24088:
     errMsg = pStrInfo_switching_Dot3ahNumTxEventPdu;
    break;

    case 24089:
     errMsg = pStrInfo_switching_Dot3ahNumTxOrgSpecPdu;
    break;


    case 24090:
     errMsg = pStrInfo_switching_Dot3ahNumRxInfoPdu;
    break;

    case 24091:
     errMsg = pStrInfo_switching_Dot3ahNumRxRemLbPdu;
    break;

    case 24092:
     errMsg = pStrInfo_switching_Dot3ahNumRxEventPdu;
    break;

    case 24093:
     errMsg = pStrInfo_switching_Dot3ahNumRxOrgSpecPdu;
    break;

    case 24094:
     errMsg = pStrInfo_switching_SwitchDot3ah;
    break;

    case 24095:
     errMsg = pStrInfo_switching_PortDot3ahMode;
    break;

    case 24096:
     errMsg = pStrInfo_switching_PortLANOAMMode;
    break;

    case 24097:
     errMsg = pStrInfo_switching_PortLANOAMLbType;
    break;

    case 24098:
     errMsg = pStrInfo_switching_PortLANOAMLbMACMode;
    break;

    case 24099:
     errMsg = pStrInfo_switching_PortLANOAMLbPHYMode;
    break;

    case 24100:
     errMsg = pStrInfo_switching_PortLANOAMLbNoneType;
    break;

    case 24101:
      errMsg =  pStrInfo_common_LastFault;
    break;
  
   case 24102:
         errMsg = pStrInfo_switching_Dot3ahOptionalCapS;
    break;
  
   case 24103:
         errMsg = pStrInfo_metro_ErrorFramePeriodThreshold;
    break;

   case 24104:
         errMsg = pStrInfo_metro_ErrorFrameSummThreshold;
    break;
  case 24105:
        errMsg = pStrInfo_switching_DhcpL2RelaySubscriptionString;
        break;
  case 24106:
       errMsg = pStrInfo_switching_DhcpL2RelaySubscriptionEntry;
       break;
  case 24107:
       errMsg = pStrInfo_switching_DhcpL2RelaySubscriptionName;
       break;
  case 1363:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceMACAddr;
    break;
  case 1364:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceMACAddr;
    break;
  case 1365:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceOui;
    break;
  case 1366:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceOui;
    break;
  case 1367:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceVSInfo;
    break;
  case 1368:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceVSInfo;
    break;

  case 1369:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceState;
    break;
  case 1370:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceState;
    break;

  case 1371:
    errMsg = pStrInfo_switching_Dot3ahIntfStatus;
    break;
  case 1372:
    errMsg = pStrInfo_switching_Dot3ahLocalMode;
    break;
  case 1373:
    errMsg = pStrInfo_switching_Dot3ahRemoteMode;
    break;

  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebSwitchingGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1108:
    errMsg = pStrInfo_common_IgmpSnoopingIntfCfg;
    break;
  case 1109:
    errMsg = pStrInfo_switching_IgmpSnoopingGlobalCfgAndStatus;
    break;
  case 1150:
    errMsg = pStrInfo_switching_LagSwitchCfg;
    break;
  case 1166:
    errMsg = pStrInfo_switching_IgmpSnoopingVlanCfg;
    break;
  case 1167:
    errMsg = pStrInfo_switching_IgmpSnoopingVlanStatus;
    break;

  case 1380:
    errMsg = pStrInfo_switching_Dot1adServiceConfig;
    break;
  case 1381:
    errMsg = pStrInfo_switching_Dot1adServiceSumm;
    break;
  case 1382:
    errMsg = pStrInfo_switching_Dot1adSubCfg;
    break;
  case 1383:
    errMsg = pStrInfo_switching_Dot1adSubSumm;
    break;
  case 1384:
    errMsg = pStrInfo_switching_Dot1adIntfType;
    break;
  case 1385:
    errMsg = pStrInfo_switching_Dot1adTunnelCfg;
    break;


  default:
    local_call=L7_TRUE;
    errMsg = usmWebSwitchingGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebSwitchingGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1026:
    errMsg = pStrInfo_switching_VlanCfg;
    break;
  case 1101:
    errMsg = pStrInfo_switching_MfdbMldSnoopingTbl;
    break;
  case 1102:
    errMsg = pStrInfo_switching_MfdbIgmpSnoopingTbl;
    break;
  case 1103:
    errMsg = pStrInfo_switching_McastFwdDbaseStats;
    break;
  case 1104:
    errMsg = pStrInfo_switching_MfdbGmrpTbl;
    break;
  case 1105:
    errMsg = pStrInfo_switching_McastFwdDbaseTbl;
    break;
  case 1106:
    errMsg = pStrInfo_switching_LagStatus;
    break;
  case 1107:
    errMsg = pStrInfo_switching_LagCfg;
    break;
  case 1108:
    errMsg = pStrInfo_common_IgmpSnoopingIntfCfg;
    break;
  case 1109:
    errMsg = pStrInfo_switching_IgmpSnoopingGlobalCfgAndStatus;
    break;
  case 1110:
    errMsg = pStrInfo_switching_GarpStatus;
    break;
  case 1111:
    errMsg = pStrInfo_switching_GarpPortCfg;
    break;
  case 1112:
    errMsg = pStrInfo_switching_GarpSwitchCfg;
    break;
  case 1113:
    errMsg = pStrInfo_switching_MacFilterSummary;
    break;
  case 1114:
    errMsg = pStrInfo_switching_MacFilterCfg;
    break;
  case 1115:
    errMsg = pStrInfo_switching_ProtoBasedVlanSummary;
    break;
  case 1116:
    errMsg = pStrInfo_switching_ProtoBasedVlanCfg;
    break;
  case 1117:
    errMsg = pStrInfo_switching_ResetCfgToFactoryDefls;
    break;
  case 1118:
    errMsg = pStrInfo_switching_VlanPortCfg;
    break;
  case 1119:
    errMsg = pStrInfo_switching_VlanStatus;
    break;
  case 1120:
    errMsg = pStrInfo_switching_ResetVlanCfg;
    break;
    case 1121:
    errMsg = "VLAN Internal Usage";
    break;
  case 1123:
    errMsg = pStrInfo_switching_SpanTreeSwitchCfgStatus;
    break;
  case 1124:
    errMsg = pStrInfo_switching_SpanTreeStats;
    break;
  case 1125:
    errMsg = pStrInfo_switching_SpanTreeCstCfgStatus;
    break;
  case 1126:
    errMsg = pStrInfo_switching_SpanTreeMstPortCfgStatus;
    break;
  case 1127:
    errMsg = pStrInfo_switching_SpanTreeMstCfgStatus;
    break;
  case 1128:
    errMsg = pStrInfo_switching_SpanTreeCstPortCfgStatus;
    break;
  case 1142:
    errMsg = pStrInfo_switching_VlanPortSummary;
    break;
  case 1147:
    errMsg = pStrInfo_switching_IgmpSnoopingQuerierCfg;
    break;
  case 1148:
    errMsg = pStrInfo_switching_IgmpSnoopingQuerierVlanCfg;
    break;
  case 1149:
    errMsg = pStrInfo_switching_IgmpSnoopingQuerierStatus;
    break;
  case 1152:
    errMsg = pStrInfo_common_MldSnoopingIntfCfg;
    break;
  case 1153:
    errMsg = pStrInfo_switching_MldSnoopingGlobalCfgAndStatus;
    break;
  case 1155:
    errMsg = pStrInfo_switching_MldSnoopingVlanCfg;
    break;
  case 1156:
    errMsg = pStrInfo_switching_MldSnoopingVlanStatus;
    break;
  case 1160:
    errMsg = pStrInfo_switching_Dot1pPriMapping;
    break;
  case 1161:
    errMsg = pStrInfo_switching_McastRtrStats;
    break;
  case 1162:
    errMsg = pStrInfo_switching_McastRtrCfg;
    break;
  case 1164:
    errMsg = pStrInfo_switching_McastRtrVlanCfg;
    break;
  case 1165:
    errMsg = pStrInfo_switching_McastRtrVlanStats;
    break;
  case 1166:
    errMsg = pStrInfo_switching_IgmpSnoopingVlanCfg;
    break;
  case 1167:
    errMsg = pStrInfo_switching_IgmpSnoopingVlanStatus;
    break;
  case 1168:
    errMsg = pStrInfo_switching_IgmpSnoopingQuerierVlanCfgSummary;
    break;
  case 1170:
    errMsg = pStrInfo_switching_PortSecurityAdministration;
    break;
  case 1171:
    errMsg = pStrInfo_switching_PortSecurityViolationStatus;
    break;
  case 1172:
    errMsg = pStrInfo_switching_PortSecurityDynallyLearnedMacAddres;
    break;
  case 1173:
    errMsg = pStrInfo_switching_PortSecurityIntfCfg;
    break;
  case 1174:
    errMsg = pStrInfo_switching_PortSecurityStaticallyCfguredMacAddres;
    break;
  case 1175:
    errMsg = pStrInfo_switching_MacBasedVlanCfg;
    break;
  case 1176:
    errMsg = pStrInfo_switching_IpSubnetBasedVlanCfg;
    break;
  case 1177:
    errMsg = pStrInfo_switching_MacBasedVlanSummary;
    break;
  case 1178:
    errMsg = pStrInfo_switching_IpSubnetBasedVlanSummary;
    break;
  case 1179:
    errMsg = pStrInfo_switching_DhcpFilteringCfg;
    break;
  case 1180:
    errMsg = pStrInfo_switching_DhcpFilteringIntfCfg;
    break;
  case 1181:
    errMsg = pStrInfo_switching_DhcpFilteringIntfSummary;
    break;
  case 1185:
    errMsg = pStrInfo_common_LldpGlobalCfg;
    break;
  case 1186:
    errMsg = pStrInfo_common_LldpIntfCfg;
    break;
  case 1187:
    errMsg = pStrInfo_switching_LldpStats;
    break;
  case 1188:
    errMsg = pStrInfo_switching_LldpLocalDeviceInfo;
    break;
  case 1189:
    errMsg = pStrInfo_switching_LldpIntfSummary;
    break;
  case 1190:
    errMsg = pStrInfo_switching_LldpRemoteDeviceInfo;
    break;
  case 1191:
    errMsg = pStrInfo_common_LldpLocalDeviceSummary;
    break;
  case 1192:
    errMsg = pStrInfo_common_LldpRemoteDeviceSummary;
    break;
  case 1193:
    errMsg = pStrInfo_switching_ProtectedPortsCfg;
    break;
  case 1194:
    errMsg = pStrInfo_switching_ProtectedPortsSummary;
    break;
  case 1195:
    errMsg = pStrInfo_switching_MldSnoopingQuerierCfg;
    break;
  case 1196:
    errMsg = pStrInfo_switching_MldSnoopingQuerierVlanCfg;
    break;
  case 1197:
    errMsg = pStrInfo_switching_MldSnoopingQuerierVlanStatus;
    break;
  case 1198:
    errMsg = pStrInfo_switching_MldSnoopingQuerierVlanCfgSummary;
    break;
  case 1199:
    errMsg = pStrInfo_switching_LldpMedGlobalConfiguration;
    break;
  case 1200:
    errMsg = pStrInfo_switching_LLDPMEDInterfaceConfiguration;
    break;
  case 1201:
    errMsg = pStrInfo_switching_LLDPMEDInterfaceSummary;
    break;
  case 1202:
    errMsg = pStrInfo_switching_LLDPMEDLocalDeviceInformation;
    break;
  case 1203:
    errMsg = pStrInfo_switching_LLDPMEDRemoteDeviceInformation;
    break;
  case 1204:
    errMsg = pStrInfo_switching_voiceVlanCfg;
    break;
  case 1205:
    errMsg = pStrInfo_switching_DynamicArpInspVlanCfg;
    break;
  case 1206:
    errMsg = pStrInfo_switching_DynamicArpInspIntfCfg;
    break;
  case 1207:
    errMsg = pStrInfo_switching_DynamicArpInspArpAclCfg;
    break;
  case 1208:
    errMsg = pStrInfo_switching_DynamicArpInspArpAclRuleCfg;
    break;
  case 1209:
    errMsg = pStrInfo_switching_DynamicArpInspCfg;
    break;
  case 1210:
    errMsg = pStrInfo_switching_DynamicArpInspStats;
    break;
  case 1300:
    errMsg = pStrInfo_switching_DhcpSnoopBindngCfg;
    break;
  case 1301:
    errMsg = pStrInfo_switching_DhcpSnoopCfg;
    break;
  case 1303:
    errMsg = pStrInfo_switching_DhcpSnoopIntfCfg;
    break;
  case 1304:
    errMsg = pStrInfo_switching_DhcpSnoopPersistCfg;
    break;
  case 1305:
    errMsg = pStrInfo_switching_DhcpSnoopStats;
    break;
  case 1306:
    errMsg = pStrInfo_switching_DhcpSnoopVlanCfg;
    break;
  case 1307:
    errMsg = pStrInfo_switching_IpsgBindingCfg;
    break;
  case 1309:
    errMsg = pStrInfo_switching_IpsgIntfCfg;
    break;
  case 1351:
    errMsg = pStrInfo_switching_DhcpL2RelayCfg;
    break;
  case 1352:
    errMsg = pStrInfo_switching_DhcpL2RelayIntfCfg;
    break;
  case 1353:
    errMsg = pStrInfo_switching_DhcpL2RelayVlanCfg;
    break;
  case 1354:
    errMsg = pStrInfo_switching_DhcpL2RelayIntfStats;
    break;
  case 1355:
    errMsg = pStrInfo_switching_DhcpL2RelayIntfStatsDisp;
    break;
  

case 1356:
    errMsg = pStrInfo_switching_Dot3ahIntfStatsDisp;
    break;
  case 1357:
    errMsg = pStrInfo_common_LanOamIntfCfg;
    break;
  case 1358:
    errMsg = pStrInfo_common_Dot3ahIntfCfg;
    break;
  case 1359:
    errMsg = pStrInfo_switching_Dot3ahIntfSummary;
    break;
  case 1360:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceInfo;
    break;
  case 1361:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceInfo;
    break;
  case 1362:
    errMsg = pStrInfo_switching_Dot3ahStats;
    break;

  case 1363:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceMACAddr;
    break;
  case 1364:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceMACAddr;
    break;
  case 1365:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceOui;
    break;
  case 1366:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceOui;
    break;
  case 1367:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceVSInfo;
    break;
  case 1368:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceVSInfo;
    break;
  case 1369:
    errMsg = pStrInfo_switching_Dot3ahLocalDeviceState;
    break;
  case 1370:
    errMsg = pStrInfo_switching_Dot3ahRemoteDeviceState;
    break;

  case 1371:
    errMsg = pStrInfo_switching_Dot3ahIntfStatus;
    break;
  case 1372:
    errMsg = pStrInfo_switching_Dot3ahLocalMode;
    break;
  case 1373:
    errMsg = pStrInfo_switching_Dot3ahRemoteMode;
    break;
  case 1374:
    errMsg = pStrInfo_switching_Dot3ahStatus;
    break;
  case 1375:
    errMsg = pStrInfo_switching_LanOamStats;
    break;

  case 1380:
    errMsg = pStrInfo_switching_Dot1adServiceConfig;
    break;
  case 1381:
    errMsg = pStrInfo_switching_Dot1adServiceSumm;
    break;
  case 1382:
    errMsg = pStrInfo_switching_Dot1adSubCfg;
    break;
  case 1383:
    errMsg = pStrInfo_switching_Dot1adSubSumm;
    break;
  case 1384:
    errMsg = pStrInfo_switching_Dot1adIntfType;
    break;
  case 1385:
    errMsg = pStrInfo_switching_Dot1adTunnelCfg;
    break;
 case 1390:
    errMsg = pStrInfo_switching_DhcpL2RelayIntfSubscriptionCfg;
    break;
  case 1391:
    errMsg = pStrInfo_switching_DhcpL2RelayIntfSubscriptionSummary;
    break;





  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  if( local_call == L7_TRUE )
     return errMsg;
  else
     return usmWebPageHeader1stGet(errMsg);
}

L7_char8 *usmWebSwitchingGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2026:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1qHelpVlanVlanconfig;
    break;
  case 2101:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMfdbHelpMfdbMldTbl;
    break;
  case 2102:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMfdbHelpMfdbIgmpTbl;
    break;
  case 2103:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMfdbHelpMfdbMfdbStats;
    break;
  case 2104:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMfdbHelpMfdbGmrpTbl;
    break;
  case 2105:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMfdbHelpMfdbMfdbTbl;
    break;
  case 2106:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLagHelpLagTrunkstat;
    break;
  case 2107:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLagHelpLagTrunkcfg;
    break;
  case 2108:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingIgmpIntf;
    break;
  case 2109:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingSwitch;
    break;
  case 2110:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingGarpHelpGarpGarpstat;
    break;
  case 2111:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingGarpHelpGarpPortgarpcfg;
    break;
  case 2112:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingGarpHelpGarpGarpcfg;
    break;
  case 2113:
    errMsg = pStrInfo_switching_HtmlLinkBaseSysHelpMacfilterSwcfgsumm;
    break;
  case 2114:
    errMsg = pStrInfo_switching_HtmlLinkBaseSysHelpMacfilterSwcfg;
    break;
  case 2115:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1vHelpPbvlanPbvlanSummary;
    break;
  case 2116:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1vHelpPbvlanPbvlanCfg;
    break;
  case 2118:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1qHelpVlanQnpcfg;
    break;
  case 2119:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1qHelpVlanVlanstat;
    break;
  case 2120:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1qHelpVlanResvlan;
    break;
  case 2121:
    errMsg = "href=\"/switching/dot1q/help_vlan.html#intusage\"";
    break;
  case 2123:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpMstpswitch;
    break;
  case 2124:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpStats;
    break;
  case 2125:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpCistcfg;
    break;
  case 2126:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpMstiportcfg;
    break;
  case 2127:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpMsticfg;
    break;
  case 2128:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingSpanTreeHelpMstpCistportcfg;
    break;
  case 2142:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1qHelpVlanQnpsumm;
    break;
  case 2147:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingQuerierQuerierCfg;
    break;
  case 2148:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingQuerierQuerierVlanCfg;
    break;
  case 2149:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingQuerierQuerierVlanStatus;
    break;
  case 2152:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMldIntf;
    break;
  case 2153:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingSwitch;
    break;
  case 2154:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMulRutStat;
    break;
  case 2155:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMrcfg;
    break;
  case 2156:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMulVlanCfg;
    break;
  case 2157:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMulVlanStat;
    break;
  case 2158:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMldVlanCfg;
    break;
  case 2159:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingMldVlanStats;
    break;
  case 2160:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDot1pHelpDot1pDot1pprioritymapping;
    break;
  case 2161:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingMulRutStat;
    break;
  case 2162:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingMrcfg;
    break;
  case 2164:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingMulVlanCfg;
    break;
  case 2165:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingMulVlanStat;
    break;
  case 2166:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingIgmpVlanCfg;
    break;
  case 2167:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingIgmpVlanStats;
    break;
  case 2168:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIgmpSnoopingHelpIgmpSnoopingQuerierQuerierVlanSumm;
    break;
  case 2170:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingPmlHelpPortmaclockingPortmaclockingconfiguration;
    break;
  case 2171:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingPmlHelpPortmaclockingPortmacviolationstatus;
    break;
  case 2172:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingPmlHelpPortmaclockingPortmaclockingintifsetup;
    break;
  case 2173:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingPmlHelpPortmaclockingPortmaclockingstatic;
    break;
  case 2174:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingPmlHelpPortmaclockingPortmaclockingdynamic;
    break;
  case 2175:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMacVlanHelpMacVlanMacvlancfg;
    break;
  case 2176:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIpsubnetvlanHelpIpsubnetVlanSubnetvlancfg;
    break;
  case 2177:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMacVlanHelpMacVlanMacvlansumm;
    break;
  case 2178:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingIpsubnetvlanHelpIpsubnetVlanSubnetvlansumm;
    break;
  case 2179:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpFilteringHelpDhcpFilteringDhcpfiltercfg;
    break;
  case 2180:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpFilteringHelpDhcpFilteringDhcpfilterintfcfg;
    break;
  case 2181:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpFilteringHelpDhcpFilteringDhcpfilterintfsumm;
    break;
  case 2185:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpcfg;
    break;
  case 2186:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpintfcfg;
    break;
  case 2187:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpstat;
    break;
  case 2188:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldplocaldevice;
    break;
  case 2189:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpintfsumm;
    break;
  case 2190:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpremotedevice;
    break;
  case 2191:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldplocaldevicesumm;
    break;
  case 2192:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingLldpHelpLldpLldpremotedevicesumm;
    break;
  case 2193:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingProtectedPortsHelpProtectedPortsProtPortCfg;
    break;
  case 2194:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingProtectedPortsHelpProtectedPortsProtPortSumm;
    break;
  case 2195:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingQuerierQuerierCfg;
    break;
  case 2196:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingQuerierQuerierVlanCfg;
    break;
  case 2197:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingQuerierQuerierVlanStatus;
    break;
  case 2198:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingMldSnoopingHelpMldSnoopingQuerierQuerierVlanSumm;
    break;
  case 2199:
     errMsg = pStrInfo_switching_LldpMedGlobalConfigurationHelp;
     break;
  case 2200:
     errMsg = pStrInfo_switching_LLDPMEDInterfaceConfigurationHelp;
     break;
  case 2201:
     errMsg = pStrInfo_switching_LLDPMEDInterfaceSummaryHelp;
     break;
  case 2202:
     errMsg = pStrInfo_switching_LLDPMEDLocalDeviceInformationHelp;
     break;
  case 2203:
     errMsg = pStrInfo_switching_LLDPMEDRemoteDeviceInformationHelp;
     break;
  case 2204:
     errMsg = pStrInfo_switching_VoiceVlanCfgHelp;
     break;
  case 2205:
    errMsg =  pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiVlanCfg;
    break;
  case 2206:
    errMsg =  pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiIntfCfg;
    break;
  case 2207:
    errMsg =  pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiArpCfg;
    break;
  case 2208:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiArpAclRuleCfg;
    break;
  case 2209:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiCfg;
    break;
  case 2210:
    errMsg = pStrInfo_switching_HtmlLinkSwitchingDaiHelpDaiStats ;
    break;   
  case 2300:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpBindngCfg;
     break;
  case 2301:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpCfg;
     break;
  case 2303:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpIntfCfg;
     break;
  case 2304:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpPersistCfg;
     break;
  case 2305:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpStats;
     break;
  case 2306:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpVlanCfg;
     break;
  case 2307:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpIpsgBindingCfg;
     break;
  case 2309:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpSnoopingHelpIpsgIntfCfg;
     break;
  case 2351:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpCfg;
     break;
  case 2352:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpIntfCfg;
     break;
  case 2353:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpVlanCfg;
     break;
  case 2354:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpIntfStats;
     break;

  case 2355:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahcfg;
     break;

  case 2356:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahintfcfg;
     break;
  case 2357:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahintfsumm;
     break;
  case 2358:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahlocaldevice;
     break;
  case 2359:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahlocaldevicesumm;
     break;

  case 2360:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahlocaldevicesumm;
     break;

  case 2361:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahstat;
     break;

  case 2362:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahstatus;
     break;
  case 2363:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahDot3ahLocalstatus;
     break;

 case 2364:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahLanOamintfcfg;
     break;

 case 2365:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingDot3ahHelpDot3ahLanOamstat;
     break;

 case 2370:
     errMsg = pStrInfo_switching_Dot1adHelpServiceConfig;
     break;
 case 2371:
     errMsg = pStrInfo_switching_Dot1adHelpServiceSumm;
     break;
 case 2372:
     errMsg = pStrInfo_switching_Dot1adHelpSubCfg;
     break;
 case 2373:
     errMsg = pStrInfo_switching_Dot1adHelpSubSumm;
     break;
 case 2374:
     errMsg = pStrInfo_switching_Dot1adHelpIntfTypeCfg;
     break;
 case 2375:
     errMsg = pStrInfo_switching_Dot1adHelpTunnelCfg;
     break;
 case 2376:
     errMsg =pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpSubscCfg;
     break;
case 2377:
     errMsg =pStrInfo_switching_HtmlLinkSwitchingDhcpL2RelayHelpSubscSumm;
     break;
case 2378:
     errMsg =pStrInfo_switching_HtmlLinkXuiSwitchingDhcpSnoopingHelpVlanCfg;
     break;
case 2379:
     errMsg =pStrInfo_switching_HtmlLinkXuiSwitchingDhcpSnoopingHelpBindngCfg;
     break;
case 2380:
     errMsg =pStrInfo_switching_HtmlLinkXuiSwitchingDhcpSnoopingHelpIpsgBindingCfg;
     break;
 case 2381:
     errMsg = pStrInfo_switching_DynamicArpInspectionXUIVlanCfg;
     break;
 case 2382:
     errMsg = pStrInfo_switching_DynamicArpInspectionXUIStatistics;
     break;
 case 2383:
     errMsg = pStrInfo_switching_HtmlLinkSwitchingXUIDhcpL2RelayVlanCfg;
     break;
 case 2384:
    errMsg = pStrInfo_switching_HtmlLinkXuiSwitchingMfdbHelpMfdbGmrpTbl;
    break;
 case 2385:
    errMsg = pStrInfo_switching_HtmlLinkXuiSwitchingMfdbHelpMfdbIgmpTbl;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
