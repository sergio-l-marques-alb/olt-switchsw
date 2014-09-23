/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename src/nls/web/wireless/oem_wireless.c
*                                                                     
* @purpose    OEM String Helper Functions                                                      
*                                                                     
* @component  WEB                                                      
*                                                                     
* @comments   none                                                    
*                                                                     
* @create     03/22/2007                                                      
*                                                                     
* @author     Rama Sasthri, Kristipati                                
*                                                                     
* @end                                                                
*                                                                     
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_wireless_common.h"
#include "strlib_wireless_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebWirelessGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebWirelessGetNLS(L7_int32 token)
{
  L7_char8 *errMsg;

  switch(token)
  {
    case 996:
      errMsg = pStrInfo_common_Lvl7SyssFastpathSwitchApplet;
      break;
    case 1000:
      errMsg = pStrInfo_common_ClientUsrName;
      break;
    case 1001:
      errMsg = pStrInfo_common_Passwd_1;
      break;
    case 1004:
      errMsg = pStrInfo_common_MacAddr_2;
      break;
    case 1005:
      errMsg = pStrInfo_common_ApIpAddr;
      break;
    case 1017:
      errMsg = pStrInfo_wireless_Debug_1;
      break;
    case 1030:
      errMsg = pStrInfo_common_ApSerialNum;
      break;
    case 1032:
      errMsg = pStrInfo_common_ApPartNum;
      break;
    case 1040:
      errMsg = pStrInfo_common_ApSoftwareVer;
      break;
    case 1103:
      errMsg = pStrInfo_common_PhyMode;
      break;
    case 1117:
      errMsg = pStrInfo_common_McastPktsRcvd;
      break;
    case 1165:
      errMsg = pStrInfo_common_ApStatus;
      break;
    case 1234:
      errMsg = pStrInfo_common_ApSysUpTime;
      break;
    case 1269:
      errMsg = pStrInfo_common_ConfirmPasswd;
      break;
    case 1497:
      errMsg = pStrInfo_common_Nw;
      break;
    case 1539:
      errMsg = pStrInfo_common_ApAge;
      break;
    case 1540:
      errMsg = pStrInfo_common_ClientNwTime;
      break;
    case 2144:
      errMsg = pStrInfo_common_VapVlan;
      break;
    case 4001:
      errMsg = pStrInfo_common_Dsbl_1;
      break;
    case 4002:
      errMsg = pStrInfo_common_Enbl_1;
      break;
    case 12859:
      errMsg = pStrInfo_common_web_None_4;
      break;
    case 15007:
      errMsg = pStrInfo_common_ApProtoVer;
      break;
    case 15963:
      errMsg = pStrInfo_wireless_RADIUS;
      break;
    case 16350:
      errMsg = pStrInfo_common_Id;
      break;
    case 24006:
      errMsg = pStrInfo_wireless_CountryCode;
      break;
    case 24010:
      errMsg = pStrInfo_wireless_ApClientQos_Disp;
      break;
    case 24011:
      errMsg = pStrInfo_wireless_ApClientQosMode_Disp;
      break;
    case 24012:
      errMsg = pStrInfo_wireless_NetClientQos_Disp;
      break;
    case 24013:
      errMsg = pStrInfo_wireless_ClientQosMode_Disp;
      break;
    case 24060:
      errMsg = pStrInfo_wireless_PeerGrpId;
      break;
    case 24061:
      errMsg = pStrInfo_wireless_ClientRoamTimeoutSecs;
      break;
    case 24062:
      errMsg = pStrInfo_wireless_RfScanStatusHours;
      break;
    case 24063:
      errMsg = pStrInfo_wireless_ApFailureStatusHours;
      break;
    case 24064:
      errMsg = pStrInfo_wireless_ClientFailureStatusHours;
      break;
    case 24065:
      errMsg = pStrInfo_wireless_AdHocClientStatusHours;
      break;
    case 24070:
      errMsg = pStrErr_wireless_L3IpDisc;
      break;
    case 24071:
      errMsg = pStrInfo_wireless_L2VlanDisc;
      break;
    case 24074:
      errMsg = pStrInfo_common_ApStatus;
      break;
    case 24075:
      errMsg = pStrInfo_wireless_Vlan14094;
      break;
    case 24091:
      errMsg = pStrInfo_wireless_PeerManagedApMac;
      break;
    case 24092:
      errMsg = pStrInfo_wireless_WsPeerSwitchIpAddr;
      break;
    case 24093:
      errMsg = pStrInfo_wireless_ApIpAddr;
      break;
    case 24094:
      errMsg = pStrInfo_wireless_NoPeerManagedApsExist;
      break;
    case 24095:
      errMsg = pStrInfo_wireless_TunnelIpMtuSize;
      break;
    case 24097:
      errMsg = pStrInfo_wireless_ManagingSwitch;
      break;
    case 24098:
      errMsg = pStrInfo_wireless_SwitchMacAddr;
      break;
    case 24099:
      errMsg = pStrInfo_wireless_ReportingSwitch;
      break;
    case 24100:
      errMsg = pStrInfo_wireless_AssociatingSwitch;
      break;
    case 24101:
      errMsg = pStrInfo_wireless_MACAuthMode_1;
      break;
    case 24102:
      errMsg = pStrInfo_wireless_LocationTriggerDevType;
      break;
    case 24103:
      errMsg = pStrInfo_wireless_LocationTriggerDevMAC;
      break;
    case 24104:
      errMsg = pStrInfo_wireless_LocationTriggerDevMACList;
      break;
    case 24105:
      errMsg = pStrInfo_wireless_LocationTriggerDevMACEnter;
      break;
    case 24106:
      errMsg = pStrInfo_wireless_LocationTriggerBuilding;
      break;
    case 24107:
      errMsg = pStrInfo_wireless_LocationTriggerFloor;
      break;
    case 24108:
      errMsg = pStrInfo_wireless_LocationTriggerSentryMode;
      break;
    case 24109:
      errMsg = pStrInfo_wireless_LocationTriggerStatus;
      break;
    case 24110:
      errMsg = pStrInfo_wireless_LocationTriggerNumOfAps;
      break;
    case 24150:
      errMsg = pStrInfo_wireless_CurrentReceiveStatus;
      break;
    case 24151:
      errMsg = pStrInfo_wireless_WsPeerSwitchIpAddr;
      break;
    case 24152:
      errMsg = pStrInfo_wireless_Cfg_2;
      break;
    case 24153:
      errMsg = pStrInfo_wireless_WsCfg_6;
      break;
    case 24162:
      errMsg = pStrInfo_common_TacacsShowGlobal;
      break;
    case 24163:
      errMsg = pStrInfo_wireless_WsCfgDiscCfgMask;
      break;
    case 24164:
      errMsg = pStrInfo_wireless_ChannelPower;
      break;
    case 24165:
      errMsg = pStrInfo_wireless_WsCfgValidApDbaseCfgMask;
      break;
    case 24166:
      errMsg = pStrInfo_wireless_ApProfiles;
      break;
    case 24167:
      errMsg = pStrInfo_wireless_WsCfgCfgStatus;
      break;
    case 24168:
      errMsg = pStrInfo_wireless_WsCfgSwitchIpAddr;
      break;
    case 24169:
      errMsg = pStrInfo_wireless_PeerIpAddr;
      break;
    case 24175:
      errMsg = pStrInfo_wireless_TotalCount;
      break;
    case 24176:
      errMsg = pStrInfo_wireless_SuccessCount;
      break;
    case 24177:
      errMsg = pStrInfo_wireless_FailureCount;
      break;
    case 24180:
      errMsg = pStrInfo_wireless_WsCfgLastCfgRcvd;
      break;
    case 24181:
      errMsg = pStrInfo_wireless_KnownClient;
      break;
    case 24182:
      errMsg = pStrInfo_wireless_WdsGroup;
      break;
    case 24204:
      errMsg = pStrErr_wireless_WsApFailureTraps;
      break;
    case 24205:
      errMsg = pStrInfo_wireless_WsRogueApTraps;
      break;
    case 24206:
      errMsg = pStrInfo_wireless_WsApStateChgTraps;
      break;
    case 24208:
      errMsg = pStrErr_wireless_WsClientFailureTraps;
      break;
    case 24209:
      errMsg = pStrInfo_wireless_WsClientStateChgTraps;
      break;
    case 24211:
      errMsg = pStrInfo_wireless_WsRfScanTraps;
      break;
    case 24213:
      errMsg = pStrInfo_wireless_WsStatusTraps;
      break;
    case 24214:
      errMsg = pStrInfo_wireless_WsPeerWsTraps;
      break;
    case 24215:
      errMsg = pStrInfo_wireless_WidsStatusTraps;
      break;
    case 24300:
      errMsg = pStrInfo_wireless_RRM;
      break;
    case 24301:
      errMsg = pStrInfo_wireless_PeerSwitches;
      break;
    case 24302:
      errMsg = pStrInfo_wireless_ManagedAccessPoints;
      break;
    case 24303:
      errMsg = pStrInfo_wireless_StandaloneAccessPoints;
      break;
    case 24304:
      errMsg = pStrInfo_wireless_RogueAccessPoints;
      break;
    case 24305:
      errMsg = pStrInfo_wireless_ClientAssociated;
      break;
    case 24306:
      errMsg = pStrInfo_wireless_WlanUtil;
      break;
    case 24307:
      errMsg = pStrInfo_wireless_TotalClients;
      break;
    case 24308:
      errMsg = pStrInfo_wireless_ClientAuthenticated;
      break;
    case 24309:
      errMsg = pStrInfo_wireless_ClusterPriority_Disp;
      break;
    case 24310:
      errMsg = pStrInfo_wireless_ClusterControllerInd_Disp;
      break;
    case 24311:
      errMsg = pStrInfo_wireless_ClusterControllerIpAddress_Disp;
      break;
    case 24312:
      errMsg = pStrErr_wireless_WSNotClusterControllerErrorMsg;
      break;
    case 24401:
      errMsg = pStrInfo_wireless_ApStatBytesTx;
      break;
    case 24402:
      errMsg = pStrInfo_wireless_ApStatBytesRx;
      break;
    case 24403:
      errMsg = pStrInfo_wireless_ApStatPktsTx;
      break;
    case 24404:
      errMsg = pStrInfo_wireless_ApStatPktsRx;
      break;
    case 24406:
      errMsg = pStrInfo_wireless_TotalAccessPoints;
      break;
    case 24407:
      errMsg = pStrErr_wireless_ConnFailedAccessPoints;
      break;
    case 24408:
      errMsg = pStrInfo_wireless_DiscoveredAccessPoints;
      break;
    case 24409:
      errMsg = pStrInfo_wireless_ApStatDropBytesTx;
      break;
    case 24410:
      errMsg = pStrInfo_wireless_ApStatDropBytesRx;
      break;
    case 24411:
      errMsg = pStrInfo_wireless_ApStatDropPktsTx;
      break;
    case 24412:
      errMsg = pStrInfo_wireless_ApStatDropPktsRx;
      break;
    case 24501:
      errMsg = pStrInfo_wireless_ApValidation;
      break;
    case 24502:
      errMsg = pStrInfo_wireless_ApAuth;
      break;
    case 24504:
      errMsg = pStrInfo_wireless_FilePath;
      break;
    case 24505:
      errMsg = pStrInfo_wireless_FileName;
      break;
    case 24506:
      errMsg = pStrInfo_wireless_Local;
      break;
    case 24507:
      errMsg = pStrInfo_wireless_RADIUS;
      break;
    case 24511:
      errMsg = pStrInfo_wireless_NoEntriesCfguredInIpPollingList;
      break;
    case 24514:
      errMsg = pStrInfo_wireless_GrpSize;
      break;
    case 24515:
      errMsg = pStrInfo_wireless_ManagedAp;
      break;
    case 24516:
      errMsg = pStrInfo_wireless_ServerAddress;
      break;
    case 24517:
      errMsg = pStrInfo_wireless_DownloadCount;
      break;
    case 24518:
      errMsg = pStrInfo_wireless_SuccessCount;
      break;
    case 24519:
      errMsg = pStrInfo_wireless_FailureCount;
      break;
    case 24520:
      errMsg = pStrInfo_wireless_WsChannelMode;
      break;
    case 24521:
      errMsg = pStrInfo_wireless_WsIntvl;
      break;
    case 24522:
      errMsg = pStrInfo_wireless_WsFixedTime;
      break;
    case 24523:
      errMsg = pStrInfo_wireless_WsHistoryDepth;
      break;
    case 24524:
      errMsg = pStrInfo_wireless_WsChannelPlan;
      break;
    case 24525:
      errMsg = pStrInfo_wireless_PowerAdjustmentMode;
      break;
    case 24526:
      errMsg = pStrInfo_wireless_PowerAdjustmentIntvlMinutes;
      break;
    case 24527:
      errMsg = pStrInfo_wireless_WsLastAlgoTime;
      break;
    case 24534:
      errMsg = pStrInfo_wireless_CurrentPower;
      break;
    case 24535:
      errMsg = pStrInfo_wireless_NewPower;
      break;
    case 24537:
      errMsg = pStrInfo_wireless_ChannelStatus;
      break;
    case 24538:
      errMsg = pStrInfo_wireless_PowerStatus;
      break;
    case 24541:
      errMsg = pStrInfo_wireless_CurrentChannel;
      break;
    case 24542:
      errMsg = pStrInfo_wireless_NewChannel;
      break;
    case 24543:
      errMsg = pStrInfo_common_WsOperatingStatus;
      break;
    case 24544:
      errMsg = pStrInfo_wireless_WsLastIteration;
      break;
    case 24547:
      errMsg = pStrInfo_wireless_Iteration;
      break;
    case 24548:
      errMsg = pStrInfo_wireless_ChannelPlanModeNotManual;
      break;
    case 24549:
      errMsg = pStrInfo_wireless_PowerPlanModeNotManual;
      break;
    case 24550:
      errMsg = pStrInfo_wireless_Manual;
      break;
    case 24551:
      errMsg = pStrInfo_wireless_Interval;
      break;
    case 24552:
      errMsg = pStrInfo_wireless_FixedTime;
      break;
    case 24557:
      errMsg = pStrInfo_wireless_EnblDebug;
      break;
    case 24559:
      errMsg = pStrInfo_wireless_NoChannelPlanHistoryEntriesExist;
      break;
    case 24560:
      errMsg = pStrInfo_wireless_ApTunnelIpAddr;
      break;
    case 24561:
      errMsg = pStrInfo_wireless_NoProposedChannelPlanEntriesExist;
      break;
    case 24562:
      errMsg = pStrInfo_wireless_NoProposedPowerAdjustmentEntriesExist;
      break;
    case 24563:
      errMsg = pStrInfo_wireless_AbortCount;
      break;
    case 24568:
      errMsg = pStrInfo_wireless_CurrentIteration;
      break;
    case 24569:
      errMsg = pStrInfo_wireless_DetectedIPAddr;
      break;
    case 24601:
      errMsg = pStrInfo_wireless_ApMacAddr;
      break;
    case 24603:
      errMsg = pStrInfo_wireless_ApLocation;
      break;
    case 24604:
      errMsg = pStrInfo_wireless_ManagedMode;
      break;
    case 24605:
      errMsg = pStrInfo_common_Managed;
      break;
    case 24606:
      errMsg = pStrInfo_common_Standalone;
      break;
    case 24607:
      errMsg = pStrInfo_common_Rogue;
      break;
    case 24608:
      errMsg = pStrInfo_wireless_AuthPasswd;
      break;
    case 24609:
      errMsg = pStrInfo_wireless_ApProfileId;
      break;
    case 24610:
      errMsg = pStrInfo_wireless_ApSwVersion;
      break;
    case 24617:
      errMsg = pStrInfo_wireless_Channel_1;
      break;
    case 24618:
      errMsg = pStrInfo_wireless_Power_1;
      break;
    case 24623:
      errMsg = pStrInfo_wireless_MACAddressPeerManaged;
      break;
    case 24624:
      errMsg = pStrInfo_wireless_MACAddressPeerAssociated;
      break;
    case 25002:
      errMsg = pStrInfo_wireless_SwitchIpAddr;
      break;
    case 25003:
      errMsg = pStrInfo_wireless_VendorBrId;
      break;
    case 25004:
      errMsg = pStrInfo_wireless_ProtoBrVer;
      break;
    case 25005:
      errMsg = pStrInfo_common_ApAge;
      break;
    case 25016:
      errMsg = pStrInfo_wireless_NoDataAvailableForPeerSwitchStatus;
      break;
    case 25017:
      errMsg = pStrInfo_wireless_SoftwareBrVer;
      break;
    case 25018:
      errMsg = pStrInfo_wireless_DiscBrReason;
      break;
    case 25022:
      errMsg = pStrInfo_wireless_ApProfileProfileName;
      break;
    case 25033:
      errMsg = pStrInfo_common_Deny;
      break;
    case 25038:
      errMsg = pStrInfo_wireless_ApProfileProfileStatus;
      break;
    case 25050:
      errMsg = pStrInfo_wireless_ApCfgStatus;
      break;
    case 25051:
      errMsg = pStrInfo_wireless_NoManagedAccessPointsExist;
      break;
    case 25073:
      errMsg = pStrInfo_wireless_ApVendorId;
      break;
    case 25074:
      errMsg = pStrInfo_wireless_ApHardwareType;
      break;
    case 25075:
      errMsg = pStrInfo_wireless_ApDiscReason;
      break;
    case 25076:
      errMsg = pStrInfo_wireless_ApCodeDownloadStatus;
      break;
    case 25078:
      errMsg = pStrInfo_wireless_WsTxPower;
      break;
    case 25079:
      errMsg = pStrInfo_wireless_ApStatEthPktsRx;
      break;
    case 25080:
      errMsg = pStrInfo_wireless_ApStatEthBytesRx;
      break;
    case 25082:
      errMsg = pStrInfo_wireless_ApStatEthPktsTx;
      break;
    case 25083:
      errMsg = pStrInfo_wireless_ApStatEthBytesTx;
      break;
    case 25084:
      errMsg = pStrErr_common_ApStatTotalTxErrs;
      break;
    case 25085:
      errMsg = pStrErr_wireless_ApStatTotalRxErrs;
      break;
    case 25086:
      errMsg = pStrInfo_wireless_Discover;
      break;
    case 25087:
      errMsg = pStrInfo_wireless_TxRate_1;
      break;
    case 25088:
      errMsg = pStrInfo_wireless_ApProfileRadioBeaconIntvl;
      break;
    case 25090:
      errMsg = pStrInfo_wireless_NoRfScanEntriesExist;
      break;
    case 25096:
      errMsg = pStrErr_wireless_ApFailureType;
      break;
    case 25097:
      errMsg = pStrInfo_wireless_ValidationFailures;
      break;
    case 25098:
      errMsg = pStrInfo_wireless_AuthFailures;
      break;
    case 25099:
      errMsg = pStrInfo_wireless_NoApAuthFailureEntriesExist;
      break;
    case 25101:
      errMsg = pStrErr_wireless_ApVapStatAssocFailure;
      break;
    case 25102:
      errMsg = pStrErr_wireless_ApVapStatAuthFailure;
      break;
    case 25103:
      errMsg = pStrInfo_wireless_ApRadioStatTxFrag;
      break;
    case 25104:
      errMsg = pStrInfo_wireless_ApRadioStatMcastTxFrame;
      break;
    case 25105:
      errMsg = pStrErr_wireless_ApRadioStatFailed;
      break;
    case 25106:
      errMsg = pStrInfo_wireless_ApRadioStatRetry;
      break;
    case 25107:
      errMsg = pStrInfo_wireless_ApRadioStat;
      break;
    case 25108:
      errMsg = pStrInfo_wireless_ApRadioStatFrameDup;
      break;
    case 25109:
      errMsg = pStrInfo_wireless_ApRadioStatRtsSuccess;
      break;
    case 25110:
      errMsg = pStrErr_wireless_ApRadioStatRtsFailure;
      break;
    case 25111:
      errMsg = pStrErr_wireless_ApRadioStatAckFailure;
      break;
    case 25112:
      errMsg = pStrInfo_common_ApRadioStatRxFrag;
      break;
    case 25113:
      errMsg = pStrInfo_wireless_ApRadioStatMcastRxFrame;
      break;
    case 25114:
      errMsg = pStrErr_wireless_ApRadioStatFcsErrs;
      break;
    case 25115:
      errMsg = pStrInfo_wireless_ApRadioStatTxFrame;
      break;
    case 25116:
      errMsg = pStrInfo_wireless_ApRadioStatWepEncrypt;
      break;
    case 25130:
      errMsg = pStrInfo_wireless_ChannelEligible;
      break;
    case 25131:
      errMsg = pStrInfo_wireless_ChannelIndicator;
      break;
    case 25132:
      errMsg = pStrInfo_wireless_ChannelManualAdj;
      break;
    case 25133:
      errMsg = pStrInfo_wireless_PowerIndicator;
      break;
    case 25134:
      errMsg = pStrInfo_wireless_PowerManualAdj;
      break;
    case 25136:
      errMsg = pStrInfo_wireless_NeighborsTotal;
      break;
    case 25138:
      errMsg = pStrInfo_wireless_ApClientAssociation;
      break;
    case 25141:
      errMsg = pStrInfo_wireless_CfgFailureElement;
      break;
    case 25142:
      errMsg = pStrErr_wireless_ApCfgStatusFailureMsg;
      break;
    case 25150:
      errMsg = pStrInfo_wireless_NoNeighborApsExist;
      break;
    case 25152:
      errMsg = pStrInfo_wireless_NoNeighborClientsExist;
      break;
    case 25153:
      errMsg = pStrInfo_wireless_ApResetStatus;
      break;
    case 25163:
      errMsg = pStrInfo_wireless_ApClientAuth;
      break;
    case 25164:
      errMsg = pStrInfo_wireless_RadarDetectionRequired;
      break;
    case 25165:
      errMsg = pStrInfo_wireless_RadarDetected;
      break;
    case 25167:
      errMsg = pStrInfo_wireless_TimeSinceRadarLastDetected;
      break;
    case 25168:
      errMsg = pStrInfo_wireless_CountryRegDoMain2;
      break;
    case 25169:
      errMsg = pStrInfo_wireless_CountryRegDoMain;
      break;
    case 25174:
      errMsg = pStrInfo_wireless_WsAdjStatus;
      break;
    case 25175:
      errMsg = pStrInfo_wireless_ManagedApBrCount;
      break;
    case 25176:
      errMsg = pStrInfo_wireless_SupportedChannel;
      break;
    case 25177:
      errMsg = pStrInfo_wireless_WsPeerGrpAdjStatus;
      break;
    case 25178:
      errMsg = pStrInfo_common_ApIpSubnetMask;
      break;
    case 25179:
      errMsg = pStrInfo_wireless_SupportedChannels;
      break;
    case 25204:
      errMsg = pStrInfo_wireless_ClientTxDataRate;
      break;
    case 25205:
      errMsg = pStrInfo_wireless_NoAssociatedClients;
      break;
    case 25211:
      errMsg = pStrInfo_wireless_DuplicatesRcvd;
      break;
    case 25212:
      errMsg = pStrInfo_common_ApRadioStatRxFrag;
      break;
    case 25213:
      errMsg = pStrInfo_wireless_ApRadioStatTxFrag;
      break;
    case 25214:
      errMsg = pStrInfo_wireless_TxRetries;
      break;
    case 25215:
      errMsg = pStrInfo_wireless_TxRetriesFailed;
      break;
    case 25216:
      errMsg = pStrInfo_wireless_ClientStatPktsRx;
      break;
    case 25217:
      errMsg = pStrInfo_wireless_ClientStatBytesRx;
      break;
    case 25218:
      errMsg = pStrInfo_wireless_ClientStatPktsTx;
      break;
    case 25219:
      errMsg = pStrInfo_wireless_ClientStatBytesTx;
      break;
    case 25225:
      errMsg = pStrInfo_wireless_AuthFailures;
      break;
    case 25226:
      errMsg = pStrInfo_wireless_AssociationFailures;
      break;
    case 25227:
      errMsg = pStrInfo_wireless_NoClientAuthFailureEntriesExist;
      break;
    case 25228:
      errMsg = pStrInfo_wireless_Bssid;
      break;
    case 25245:
      errMsg = pStrInfo_wireless_ClientStatDropPktsRx;
      break;
    case 25246:
      errMsg = pStrInfo_wireless_ClientStatDropBytesRx;
      break;
    case 25247:
      errMsg = pStrInfo_wireless_ClientStatDropPktsTx;
      break;
    case 25248:
      errMsg = pStrInfo_wireless_ClientStatDropBytesTx;
      break;
    case 25249:
      errMsg = pStrInfo_wireless_DetectedClientsStatusHours;
      break;
    case 25250:
      errMsg = pStrInfo_wireless_ClientNetBiosName;
      break;
    case 25231:
      errMsg = pStrInfo_wireless_ClientInactivePeriod;
      break;
    case 25232:
      errMsg = pStrInfo_wireless_NoAdHocClientsDetected;
      break;
    case 25351:
      errMsg = pStrInfo_wireless_APHWTypeID;
      break;
    case 25352:
      errMsg = pStrInfo_wireless_APHWTypeDescription;
      break;
    case 25353:
      errMsg = pStrInfo_wireless_APHWTypeNumRadios;
      break;
    case 25354:
      errMsg = pStrInfo_wireless_APHWTypeRadioIndex;
      break;
    case 25355:
      errMsg = pStrInfo_wireless_APHWTypeRadioTypeDescription;
      break;
    case 25356:
      errMsg = pStrInfo_wireless_APHWTypeRadioVAPCount;
      break;
    case 25357:
      errMsg = pStrInfo_wireless_APHWTypeRadio80211aSupport;
      break;
    case 25358:
      errMsg = pStrInfo_wireless_APHWTypeRadio80211bgSupport;
      break;
    case 25359:
      errMsg = pStrInfo_wireless_APHWTypeRadio80211nSupport;
      break;
    case 25362:
      errMsg = pStrInfo_wireless_APHWTypeRadioInvalid;
      break;
    case 25363:
      errMsg = pStrInfo_wireless_APHWTypeImageID;
      break;
  case 25365:
    errMsg = pStrInfo_wireless_APHWDualBootSupport;
    break;
    case 25401:
     errMsg = pStrInfo_wireless_ApImageType1;
     break;
    case 25402:
      errMsg = pStrInfo_wireless_ApImageFileName1;
      break;
    case 25403:
      errMsg = pStrInfo_wireless_ApImageFilePath1;
      break;
    case 25404:
      errMsg = pStrInfo_wireless_ApImageType2;
      break;
    case 25405:
      errMsg = pStrInfo_wireless_ApImageFileName2;
      break;
    case 25406:
      errMsg = pStrInfo_wireless_ApImageFilePath2;
      break;
    case 25407:
      errMsg = pStrInfo_wireless_DownloadImageType;
      break;
    case 25408:
      errMsg = pStrInfo_wireless_ApImageTypeAll;
      break;
    case 25409:
      errMsg = pStrInfo_wireless_ApImageTypeID;
      break;
    case 25410:
      errMsg = pStrInfo_wireless_ApImageTypeDescription;
      break;
    case 25411:
     errMsg = pStrInfo_wireless_ApImageType3;
     break;
    case 25412:
      errMsg = pStrInfo_wireless_ApImageFileName3;
      break;
    case 25413:
      errMsg = pStrInfo_wireless_ApImageFilePath3;
      break;
    case 25414:
      errMsg = pStrInfo_wireless_ApImageType4;
      break; 
    case 25500:
      errMsg = pStrInfo_wireless_ApProfile;
      break;
    case 25501:
      errMsg = pStrInfo_wireless_RadioIf;
      break;
    case 25502:
      errMsg = pStrInfo_wireless_VapId;
      break;
    case 25503:
      errMsg = pStrInfo_wireless_WsNwSsid;
      break;
    case 25504:
      errMsg = pStrInfo_common_VapVlan;
      break;
    case 25506:
      errMsg = pStrInfo_wireless_WsNwHideSsid;
      break;
    case 25507:
      errMsg = pStrInfo_wireless_IgnoreBcast;
      break;
    case 25508:
      errMsg = pStrInfo_wireless_L3Tunnel;
      break;
    case 25509:
      errMsg = pStrInfo_wireless_L3TunnelSubnet;
      break;
    case 25510:
      errMsg = pStrInfo_wireless_L3TunnelMask;
      break;
    case 25511:
      errMsg = pStrInfo_wireless_WsNwMacAuth;
      break;
    case 25513:
      errMsg = pStrInfo_wireless_Security;
      break;
    case 25514:
      errMsg = pStrInfo_common_EgressDirection;
      break;
    case 25515:
      errMsg = pStrInfo_wireless_WsNwWepKeyType;
      break;
    case 25516:
      errMsg = pStrInfo_wireless_WsNwWepKeyLen;
      break;
    case 25517:
      errMsg = pStrInfo_wireless_WepKeys;
      break;
    case 25518:
      errMsg = pStrInfo_wireless_RadiusIpAddr;
      break;
    case 25519:
      errMsg = pStrInfo_wireless_RadiusSecret;
      break;
    case 25520:
      errMsg = pStrInfo_wireless_WsNwRadiusAcctMode;
      break;
    case 25522:
      errMsg = pStrInfo_wireless_WsNwWpaVer;
      break;
    case 25523:
      errMsg = pStrInfo_wireless_WsNwWpaCipher;
      break;
    case 25524:
      errMsg = pStrInfo_wireless_WsNwWpaKeyType;
      break;
    case 25525:
      errMsg = pStrInfo_wireless_WsNwWpaKey;
      break;
    case 25526:
      errMsg = pStrInfo_wireless_WsNwWpa;
      break;
    case 25527:
      errMsg = pStrInfo_wireless_WsNwWpa2;
      break;
    case 25529:
      errMsg = pStrInfo_wireless_PreAuth;
      break;
    case 25530:
      errMsg = pStrInfo_wireless_PreAuthLimit;
      break;
    case 25532:
      errMsg = pStrInfo_wireless_KeyFwd;
      break;
    case 25533:
      errMsg = pStrInfo_wireless_KeyCachingHoldTime;
      break;
    case 25535:
      errMsg = pStrInfo_wireless_VapMode;
      break;
    case 25541:
      errMsg = pStrInfo_wireless_WsNwAsCii;
      break;
    case 25542:
      errMsg = pStrInfo_wireless_WsNwHex;
      break;
    case 25543:
      errMsg = pStrInfo_wireless_WsNwTkip;
      break;
    case 25544:
      errMsg = pStrInfo_wireless_CcmpAes;
      break;
    case 25546:
      errMsg = pStrInfo_common_None_1;
      break;
    case 25547:
      errMsg = pStrInfo_wireless_Wep;
      break;
    case 25548:
      errMsg = pStrInfo_wireless_WsNwWpaWpa2;
      break;
    case 25549:
      errMsg = pStrInfo_wireless_WsNwStaticWep;
      break;
    case 25550:
      errMsg = pStrInfo_wireless_WepIeee8021x;
      break;
    case 25551:
      errMsg = pStrInfo_wireless_WsNwTunnelStatus;
      break;
    case 25557:
      errMsg = pStrInfo_wireless_AutoEligible;
      break;
    case 25558:
      errMsg = pStrInfo_wireless_UseProfile;
      break;
    case 25559:
      errMsg = pStrInfo_wireless_Edit;
      break;
    case 25560:
      errMsg = pStrErr_common_ClientFailureAuth;
      break;
    case 25561:
      errMsg = pStrInfo_wireless_WsNwOpenSys;
      break;
    case 25562:
      errMsg = pStrInfo_wireless_WsNwSharedKey;
      break;
    case 25563:
      errMsg = pStrInfo_wireless_WsNwWpaPersonal;
      break;
    case 25564:
      errMsg = pStrInfo_wireless_WsNwWpaEnterprise;
      break;
    case 25565:
      errMsg = pStrInfo_wireless_Redirect;
      break;
    case 25566:
      errMsg = pStrInfo_common_None_1;
      break;
    case 25567:
      errMsg = pStrInfo_wireless_HTTP;
      break;
    case 25568:
      errMsg = pStrInfo_common_Ip;
      break;
    case 25569:
      errMsg = pStrInfo_wireless_WsNwRedirectUrl;
      break;
    case 25570:
      errMsg = pStrInfo_wireless_WsNwRedirectIp;
      break;
    case 25571:
      errMsg = pStrInfo_wireless_WsNwIntfIndex;
      break;
    case 25701:
      errMsg = pStrInfo_common_State;
      break;
    case 25702:
      errMsg = pStrInfo_wireless_5ghz_description;
      break;
    case 25703:
      errMsg = pStrInfo_wireless_24ghz_description;
      break;
    case 25704:
      errMsg = pStrInfo_common_Mode_1;
      break;
    case 25705:
      errMsg = pStrInfo_wireless_PhyModeDot11A;
      break;
    case 25706:
      errMsg = pStrInfo_wireless_WsRadioType;
      break;
    case 25707:
      errMsg = pStrInfo_wireless_RfScanOtherChannels;
      break;
    case 25708:
      errMsg = pStrInfo_wireless_RfScanSentry;
      break;
    case 25711:
      errMsg = pStrInfo_wireless_RfScanSentryChannels;
      break;
    case 25713:
      errMsg = pStrInfo_wireless_RfScanIntvlSecs;
      break;
    case 25714:
      errMsg = pStrInfo_wireless_RfScanDurationMsecs;
      break;
    case 25717:
      errMsg = pStrInfo_wireless_RateLimiting;
      break;
    case 25718:
      errMsg = pStrInfo_wireless_RateLimitPktsSec;
      break;
    case 25719:
      errMsg = pStrInfo_wireless_RateLimitBurstPktsSec;
      break;
    case 25720:
      errMsg = pStrInfo_wireless_BeaconIntvl;
      break;
    case 25721:
      errMsg = pStrInfo_wireless_DtimPeriodBeacons;
      break;
    case 25722:
      errMsg = pStrInfo_wireless_FragThreshBytes;
      break;
    case 25723:
      errMsg = pStrInfo_wireless_ApProfileRadioRtsThresh_1;
      break;
    case 25724:
      errMsg = pStrInfo_wireless_ShortRetries;
      break;
    case 25725:
      errMsg = pStrInfo_wireless_LongRetries;
      break;
    case 25726:
      errMsg = pStrInfo_wireless_TxLifetimeMsecs;
      break;
    case 25727:
      errMsg = pStrInfo_wireless_ReceiveLifetimeMsecs;
      break;
    case 25728:
      errMsg = pStrInfo_wireless_ApProfileRadioMaxClients_1;
      break;
    case 25729:
      errMsg = pStrInfo_wireless_AutoChannel;
      break;
    case 25731:
      errMsg = pStrInfo_wireless_AutoPower;
      break;
    case 25732:
      errMsg = pStrInfo_wireless_ApProfileRadioDefTxPower;
      break;
    case 25733:
      errMsg = pStrInfo_wireless_ApProfileRadioWmmMode;
      break;
    case 25734:
      errMsg = pStrInfo_wireless_ApProfileRadioLoad;
      break;
    case 25735:
      errMsg = pStrInfo_wireless_ApProfileRadioUtilization_1;
      break;
    case 25736:
      errMsg = pStrInfo_wireless_RateSetsMbps;
      break;
    case 25737:
      errMsg = pStrInfo_wireless_Supported;
      break;
    case 25738:
      errMsg = pStrInfo_wireless_Basic;
      break;
    case 25739:
      errMsg = pStrInfo_wireless_ApProfileStationIs;
      break;
    case 25740:
      errMsg = pStrInfo_wireless_ChannelBandwidth;
      break;
    case 25741:
      errMsg = pStrInfo_wireless_PrimaryChannel;
      break;
    case 25742:
      errMsg = pStrInfo_wireless_Lower;
      break;
    case 25743:
      errMsg = pStrInfo_wireless_Upper;
      break;
    case 25744:
      errMsg = pStrInfo_wireless_AvailableMCSIndices;
      break;
    case 25752:
      errMsg = pStrInfo_wireless_ApEdcaParams;
      break;
    case 25753:
      errMsg = pStrInfo_wireless_StationEdcaParams;
      break;
    case 25754:
      errMsg = pStrInfo_wireless_AifsMsecs;
      break;
    case 25755:
      errMsg = pStrInfo_wireless_CwminMsecs;
      break;
    case 25756:
      errMsg = pStrInfo_wireless_CwmaxMsecs;
      break;
    case 25766:
      errMsg = pStrInfo_wireless_NoChannelsAreCurrentlySupported;
      break;
    case 25767:
      errMsg = pStrInfo_wireless_MaxBurstMicroSecs_1;
      break;
    case 25768:
      errMsg = pStrInfo_wireless_TxopLimitMsecs;
      break;
    case 25769:
      errMsg = pStrInfo_wireless_Queue;
      break;
    case 25770:
      errMsg = pStrInfo_wireless_Data0Voice;
      break;
    case 25771:
      errMsg = pStrInfo_wireless_Data1Video;
      break;
    case 25772:
      errMsg = pStrInfo_wireless_Data2BestEffort;
      break;
    case 25773:
      errMsg = pStrInfo_wireless_Data3Background;
      break;
    case 25798:
      errMsg = pStrInfo_wireless_Num1500;
      break;
    case 25799:
      errMsg = pStrInfo_wireless_Num1520;
      break;
    case 25800:
      errMsg = pStrInfo_wireless_macauth_whitelist;
      break;
    case 25801:
      errMsg = pStrInfo_wireless_macauth_blacklist;
      break;
    case 25901:
      errMsg = pStrInfo_wireless_EnblWlanSwitch;
      break;
    case 25902:
      errMsg = pStrInfo_wireless_WlanSwitchOperationalStatus;
      break;
    case 25903:
      errMsg = pStrInfo_wireless_WlanSwitchDsblReason;
      break;
    case 25904:
      errMsg = pStrInfo_wireless_NoDataAvailableForSwitchStats;
      break;
    case 25905:
      errMsg = pStrInfo_wireless_MaxAssocClients;
      break;
    case 25906:
      errMsg = pStrInfo_wireless_MaxAccessPoints;
      break;
    case 25907:
      errMsg =pStrInfo_wireless_MaxAPsInPeerGroup;
      break;
    case 25908:
      errMsg =pStrInfo_wireless_WsRadiusAuthSrvrConfiguredStatus;
      break;
    case 25909:
      errMsg =pStrInfo_wireless_WsRadiusAcctSrvrConfiguredStatus;
      break;
    case 25910:
      errMsg =pStrInfo_wireless_WsKnownClientRadiusSrvrStatus;
      break;
    
    case 25911:
      errMsg =pStrInfo_wireless_SwitchStaticIPAddr;
      break;
    case 25572:
      errMsg =pStrInfo_wireless_WsRadiusAuthSrvrName;
      break;
    case 25573:
      errMsg =pStrInfo_wireless_WsRadiusAuthSrvrConfigured;
      break;
    case 25574:
      errMsg =pStrInfo_wireless_WsRadiusAcctSrvrName;
      break;
    case 25575:
      errMsg =pStrInfo_wireless_WsRadiusAcctSrvrConfigured;
      break;
    case 25576:
      errMsg =pStrInfo_wireless_WsNwUseRadiusConfig;
      break;
    case 25577:
      errMsg =pStrInfo_wireless_WsKnownClientRadiusSrvrName;
      break;
    case 25578:
      errMsg =pStrInfo_wireless_WsKnownClientNickName;
      break;
    case 25579:
      errMsg =pStrInfo_wireless_WsKnownClientAuthenAction;
      break;
    case 25580:
      errMsg =pStrInfo_wireless_WsKnownClientAuthenGlobal;
      break;
    case 25581:
      errMsg =pStrInfo_wireless_WsKnownClientAuthenGrant;
      break;
    case 25582:
      errMsg =pStrInfo_wireless_WsKnownClientAuthenDeny;
      break;
    case 25583:
       errMsg =pStrInfo_wireless_WsKnownClientMAC;
       break;
    case 25584:
      errMsg = pStrInfo_wireless_NoDetectedClientEntries;
      break;
    case 25585:
      errMsg = pStrInfo_wireless_ApProfileRadioAntennaDiv;
      break;
    case 25586:
      errMsg = pStrInfo_wireless_ApProfileRadioAntennaSel;
      break;
    case 25587:
      errMsg = pStrInfo_wireless_ApProfileRadioAntennaSelLeftRight;
      break;
    case 25588:
      errMsg = pStrInfo_wireless_ApProfileRadioAntennaSelLeftMiddle;
      break;
    case 25589:
      errMsg = pStrInfo_wireless_ApProfileRadioAntennaSelMiddleRight;
      break;
    case 27164:
      errMsg = pStrInfo_common_CaptivePortal;
      break;
    case 25590:
      errMsg = pStrInfo_wireless_BcastKeyRefreshRate;
      break;
    case 25591:
      errMsg = pStrInfo_wireless_SessionKeyRefreshRate;
      break;
    case 25592:
      errMsg = pStrInfo_wireless_NoKnownClientEntries;
      break;
    case 25593:
      errMsg = pStrInfo_wireless_FilePath1;
      break;
    case 25594:
      errMsg = pStrInfo_wireless_FileName1;
      break;
case 25595:
      errMsg = pStrInfo_wireless_OUI;
      break;
    case 25596:
      errMsg = pStrInfo_wireless_OUIValue;
      break;

    case 25597:
      errMsg = pStrInfo_wireless_DetectedCltPreAuthHistDepthMax;
      break;
    case 25598:
      errMsg = pStrInfo_wireless_DetectedCltPreAuthHistDepthTotal;
      break;
    case 25599:
      errMsg = pStrInfo_wireless_DetectedCltRoamHistDepthMax;
      break;
    case 25600:
      errMsg = pStrInfo_wireless_DetectedCltRoamHistDepthTotal;
      break;
    case 25601:
      errMsg = pStrErr_wireless_DetectedCltPreAuthHistNoEntries;
      break; 
    case 25602:
      errMsg = pStrInfo_wireless_NoRfScanEntryNotExist;
      break;
    case 25603:
      errMsg = pStrInfo_wireless_NoAPProvisioningEntries;
      break;
    case 25604:
      errMsg = pStrInfo_wireless_NoRrmNeighborsExist;
      break;
    case 25605:
      errMsg = pStrInfo_wireless_NoRRMChLoadHistoryEntries;
      break;
    case 25606:
      errMsg = pStrInfo_wireless_TspecVideoTrafficStreams;
      break;
    case 25607:
      errMsg = pStrInfo_wireless_TspecVoiceTrafficStreams;
      break;
    case 25608:
      errMsg = pStrInfo_wireless_TspecTotalTrafficStreamClients;
      break;
    case 25609:
      errMsg = pStrInfo_wireless_TspecTotalTrafficStreamRoamingClients;
      break;
    case 25610:
      errMsg = pStrInfo_wireless_TspecViolatePktsRecvd;
      break;
    case 25611:
      errMsg = pStrInfo_wireless_TspecViolatePktsTransmitted;
      break;
    case 25612:
      errMsg = pStrInfo_wireless_TspecStatistics;
      break;
    case 25613:
      errMsg = pStrInfo_wireless_TspecAccessCategory;
      break;
    case 25614:
      errMsg = pStrInfo_wireless_TspecAccessCategoryVoice;
      break;
    case 25615:
      errMsg = pStrInfo_wireless_TspecAccessCategoryVideo;
      break;
    case 25616:
      errMsg = pStrInfo_wireless_TspecTotalPktsRecvd;
      break;
    case 25617:
      errMsg = pStrInfo_wireless_TspecTotalPktsTrasmitted;
      break;
    case 25618:
      errMsg = pStrInfo_wireless_TspecTotalBytesRecvd;
      break;
    case 25619:
      errMsg = pStrInfo_wireless_TspecTotalBytesTrasmitted;
      break;
    case 25620:
      errMsg = pStrInfo_wireless_TspecTotalAccepted;
      break;
    case 25621:
      errMsg = pStrInfo_wireless_TspecTotalRejected;
      break;
    case 25622:
      errMsg = pStrInfo_wireless_TspecTotalRoamAccepted;
      break;
    case 25623:
      errMsg = pStrInfo_wireless_TspecTotalRoamRejected;
      break;
    case 25624:
      errMsg = pStrInfo_wireless_TspecStatus;
      break;
    case 25625:
      errMsg = pStrInfo_wireless_TspecOperationalStatus;
      break;
    case 25626:
      errMsg = pStrInfo_wireless_TspecActiveStreams;
      break;
    case 25627:
      errMsg = pStrInfo_wireless_TspecTrafficStreamClients;
      break;
    case 25628:
      errMsg = pStrInfo_wireless_TspecTrafficStreamRoamingClients;
      break;
    case 25629:
      errMsg = pStrInfo_wireless_TspecMediumTimeAdmitted;
      break;
    case 25630:
      errMsg = pStrInfo_wireless_TspecMediumTimeUnalloc;
      break;
    case 25631:
      errMsg = pStrInfo_wireless_TspecMediumRoamingUnalloc;
      break;
    case 25632:
      errMsg = pStrInfo_wireless_WsWdsGroupId;
      break;
    case 25633:
      errMsg = pStrInfo_wireless_WsWdsAPConnectionStatus;
      break;
    case 25634:
      errMsg = pStrInfo_wireless_WsWdsSatelliteMode;
      break;
    case 25635:
      errMsg = pStrInfo_wireless_WsWdsStpRootMode;
      break; 
    case 25636:
      errMsg = pStrInfo_wireless_WsWdsRootPathCost;
      break;
    case 25637:
      errMsg = pStrInfo_wireless_WsWdsEthStpState;
      break;
    case 25638:
      errMsg = pStrInfo_wireless_WsWdsEthPortMode;
      break;
    case 25639:
      errMsg = pStrInfo_wireless_WsWdsEthPortLinkState;
      break;
    case 25640:
      errMsg = pStrInfo_wireless_NoWsDevLocFlr;
      break;
    case 25641:
      errMsg = pStrInfo_wireless_NoWsDevLocAp;
      break;
    default:
      errMsg = pStrInfo_common_NlsError;
      break;
  }
  return (errMsg);
}



L7_char8 *usmWebWirelessGetOEM(L7_int32 token)
{
  L7_char8 *errMsg;

  switch(token)
  {
    default:
      local_call=L7_TRUE;
      errMsg = usmWebWirelessGetPageHdr1(token);
      local_call=L7_FALSE;
      break;
  }
  return (errMsg);
}

L7_char8 *usmWebWirelessGetPageHdr1(L7_int32 token)
{
  L7_char8 *errMsg;

  switch(token)
  {
    case 1005:
      errMsg = pStrInfo_common_DownloadFileToSwitch;
      break;
    case 1701:
      errMsg = pStrInfo_wireless_WsGlobalCfg;
      break;
    case 1703:
      errMsg = pStrInfo_wireless_WsSnmpTrapCfg;
      break;
    case 1704:
      errMsg = pStrInfo_wireless_WsGlobalStatusStats;
      break;
    case 1706:
      errMsg = pStrInfo_wireless_WsSoftwareDownload;
      break;
    case 1708:
      errMsg = pStrInfo_wireless_WsDiscCfg;
      break;
    case 1711:
      errMsg = pStrInfo_wireless_WsDiscStatus;
      break;
    case 1712:
      errMsg = pStrInfo_wireless_RegulatoryDomain;
      break;
    case 1720:
      errMsg = pStrInfo_wireless_ValidAccessPointCfg;
      break;
    case 1721:
      errMsg = pStrInfo_wireless_ValidAccessPointSummary;
      break;
    case 1722:
      errMsg = pStrInfo_wireless_SwitchStats;
      break;
    case 1723:      
      errMsg = pStrInfo_wireless_KnownClientSumm;      
      break;  
    case 1724:      
      errMsg = pStrInfo_wireless_KnownClientConfiguration;      
      break;  
    case 1725:
      errMsg = pStrInfo_wireless_OUIDataBaseSummary;
      break;
    case 1726:
      errMsg = pStrInfo_wireless_LocationTrigger;
      break;
    case 1730:
      errMsg = pStrInfo_wireless_RfCfg;
      break;
    case 1731:
      errMsg = pStrInfo_wireless_ChannelPlanHistory;
      break;
    case 1732:
      errMsg = pStrInfo_wireless_ManualChannelPlan;
      break;
    case 1734:
      errMsg = pStrInfo_wireless_ManualPowerAdjustments;
      break;
    case 1740:
      errMsg = pStrInfo_wireless_WsCentTnnlCfg;
      break;
    case 1741:
      errMsg = pStrInfo_wireless_WsCentTnnlStatus;
      break;
    case 1750:
      errMsg = pStrInfo_wireless_PeerSwitchStatus;
      break;
    case 1752:
      errMsg = pStrInfo_wireless_PeerSwitchConfigurationReceiveStatus;
      break;
    case 1753:
      errMsg = pStrInfo_wireless_PeerSwitchConfigurationRequestStatus;
      break;
    case 1754:
      errMsg = pStrInfo_wireless_PeerSwitchConfigurationEnableDisable;
      break;
    case 1755:
      errMsg = pStrInfo_wireless_PeerSwitchManagedAPStatus;
      break;
    case 1756:
      errMsg = pStrInfo_wireless_PeerSwitchConfigurationStatus;
      break;
    case 1761:
      errMsg = pStrInfo_wireless_APHWCapability;
      break;
    case 1762:
      errMsg = pStrInfo_wireless_APHWRadioCapability;
      break;
    case 1763:
      errMsg = pStrInfo_wireless_APImageCapability;
      break;
    case 1764:
      errMsg = pStrInfo_wireless_APImageVersion;
      break;
    case 1780:
      errMsg = pStrInfo_wireless_AccessPointProfileGlobalCfg;
      break;
    case 1781:
      errMsg = pStrInfo_wireless_AccessPointProfileRadioCfg;
      break;
    case 1782:
      errMsg = pStrInfo_wireless_AccessPointProfileQosCfg;
      break;
    case 1783:
      errMsg = pStrInfo_wireless_AccessPointProfileVapCfg;
      break;
    case 1784:
      errMsg = pStrInfo_wireless_AccessPointProfileSummary;
      break;
    case 1791:
      errMsg = pStrInfo_wireless_WsDeflVapCfg;
      break;
    case 1792:
      errMsg = pStrInfo_wireless_WsDeflProfileCfg;
      break;
    case 1793:
      errMsg = pStrInfo_wireless_WsDeflRadioCfg;
      break;
    case 1794:
      errMsg = pStrInfo_wireless_WsNwCfg;
      break;
    case 1795:
      errMsg = pStrInfo_wireless_WsNwSummary;
      break;
    case 1801:
      errMsg = pStrInfo_wireless_ManagedAccessPointStatus;
      break;
    case 1802:
      errMsg = pStrInfo_wireless_ManagedAccessPointRadioStatus;
      break;
    case 1803:
      errMsg = pStrInfo_wireless_ManagedAccessPointVapStatus;
      break;
    case 1804:
      errMsg = pStrInfo_wireless_AccessPointRfScanStatus;
      break;
    case 1805:
      errMsg = pStrInfo_wireless_AccessPointAuthFailureStatus;
      break;
    case 1806:
      errMsg = pStrInfo_wireless_ManagedAccessPointNeighborApStatus;
      break;
    case 1807:
      errMsg = pStrInfo_wireless_ManagedAccessPointNeighborClientStatus;
      break;
    case 1808:
      errMsg = pStrInfo_wireless_ManagedAccessPointStats;
      break;
    case 1810:
      errMsg = pStrInfo_wireless_ManagedApReset;
      break;
    case 1811:
      errMsg = pStrInfo_wireless_ManagedApAdvanced;
      break;
    case 1812:
      errMsg = pStrInfo_wireless_ManagedApDebug;
      break;
    case 1813:
      errMsg = pStrInfo_wireless_ManagedApChannelPowerAdjust;
      break;
    case 1814:      
      errMsg = pStrInfo_wireless_APTriangulationStatus;      
      break;  
    case 1816:
      errMsg = pStrInfo_wireless_AssociatedClientStatus;
      break;
    case 1817:
      errMsg = pStrInfo_wireless_AdHocClientStatus;
      break;
    case 1818:
      errMsg = pStrInfo_wireless_VapAssociatedClientStatus;
      break;
    case 1819:
      errMsg = pStrInfo_wireless_ClientAuthFailureStatus;
      break;
    case 1820:
      errMsg = pStrInfo_wireless_SsidAssociatedClientStatus;
      break;
    case 1821:
      errMsg = pStrInfo_wireless_AssociatedClientNeighborApStatus;
      break;
    case 1822:
      errMsg = pStrInfo_wireless_AssociatedClientStats;
      break;
    case 1823:
      errMsg = pStrInfo_wireless_SwitchAssociatedClientStatus;
      break;
    case 1824:      
      errMsg = pStrInfo_wireless_WidsApConfiguration;      
      break;  
    case 1825:      
      errMsg = pStrInfo_wireless_WidsAPRogueClassification;      
      break;  
    case 1827:
      errMsg = pStrInfo_wireless_AssociatedClientQosStatus;
      break;
    case 1835:      
      errMsg = pStrInfo_wireless_WidsAPDeAuthenticationAttackStatus;      
      break;  
    case 1836:      
      errMsg = pStrInfo_wireless_WidsClientConfiguration;      
      break;
    case 1837:
      errMsg = pStrInfo_wireless_DetectedClientStatus;
      break;
    case 1838:
      errMsg = pStrInfo_wireless_WidsClientRogueClassification;
      break;
    case 1839:
      errMsg = pStrInfo_wireless_DetectedClientPreAuth;
      break;
    case 1840:
      errMsg = pStrInfo_wireless_distTunnel_globalCfg;
      break;
    case 1841:
      errMsg = pStrInfo_wireless_distTunnel_clientStatus;
      break;
    case 1842:
      errMsg = pStrInfo_wireless_distTunnel_managedapStatus;
      break;
    case 1843:
      errMsg = pStrInfo_wireless_distTunnel_managedapStats;
      break;
    case 1844:
      errMsg = pStrInfo_wireless_DetectedClientPreAuthSumm;
      break;
    case 1845:
      errMsg = pStrInfo_wireless_DetectedClientRoamHistorySumm;
      break;
    case 1846:
      errMsg = pStrInfo_wireless_DetectedClientTriangulation;
      break;
    case 1847:
      errMsg = pStrInfo_wireless_DetectedClientRoamHistory;
      break;
    case 1848:
      errMsg = pStrInfo_wireless_APProvisioningSummary;
      break;
    case 1849:
      errMsg = pStrInfo_wireless_RRMNeighborsSummary;
      break;
    case 1850:
      errMsg = pStrInfo_wireless_RRMChLoadHistory;
      break;
    case 1851:
      errMsg = pStrInfo_wireless_WdsGroupStatusSummary;
      break;
    case 1852:
      errMsg = pStrInfo_wireless_BuildingSummary;
      break;
   case 1853:
      errMsg = pStrInfo_wireless_FloorSummary;
      break;
   case 1854:
      errMsg = pStrInfo_wireless_ApSummary;
      break;
   case 1855:
      errMsg = pStrInfo_wireless_DevApSummary;
      break;
    case 1856:
      errMsg = pStrInfo_wireless_TriangApSummary;
      break;
    case 1857:
      errMsg = pStrInfo_wireless_TriangClientSummary;
      break;
    case 1858:
      errMsg = pStrInfo_wireless_WdsGroupAPStatusSummary;
      break;
    case 1859:
      errMsg = pStrInfo_wireless_WdsGroupLinkStatusSummary;
      break;
    case 1860:
      errMsg = pStrInfo_wireless_WdsGroupLinkStatisticsSummary;
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

L7_char8 *usmWebWirelessGetPageHdr2(L7_int32 token)
{
  L7_char8 *errMsg;

  switch(token)
  {
    case 2601:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApProfileSummary;
      break;
    case 2602:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAdvancedGlobalSettings;
      break;
    case 2603:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpSnmpTraps;
      break;
    case 2604:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpQosCfg;
      break;
    case 2605:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpKnownClientCfg;
      break;
    case 2606:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRegDomain;
      break;
    case 2607:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientSwitchStatus;
      break;
    case 2608:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpGlobalCfg;
      break;
    case 2609:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDisc;
      break;
    case 2610:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRadioCfg;
      break;
    case 2611:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpVapCfg;
      break;
    case 2612:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpNwCfg;
      break;
    case 2613:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpValidApSummary;
      break;
    case 2614:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpValidApCfg;
      break;
    case 2615:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAaaRadius;
      break;
    case 2616:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientDetailStatus;
      break;
    case 2617:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpKnownClientCfgDetail;
      break;
    case 2618:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpNetworkList;
      break;
    case 2623:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientFailureDetail;
      break;
    case 2624:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRfScan;
      break;
    case 2625:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRfScanDetail;
      break;
    case 2626:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAdHocClient;
      break;
    case 2627:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApFailure;
      break;
    case 2628:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApFailureDetail;
      break;
    case 2629:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientFailure;
      break;
    case 2630:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApReset;
      break;
    case 2631:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRfCfg;
      break;
    case 2632:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpChannelHistory;
      break;
    case 2633:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManualChannel;
      break;
    case 2634:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManualPower;
      break;
    case 2635:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApUpgrade;
      break;
    case 2637:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApDebug;
      break;
    case 2638:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApDebug2;
      break;
    case 2639:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpPowerChannelAdjust;
      break;
    case 2640:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpGlobalStatus;
      break;
    case 2641:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDiscStatus;
      break;
    case 2642:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpPeerSwitchStatus;
      break;
    case 2643:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpConfigurationReceiveStatus;
      break;
    case 2644:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpConfigurationSendStatus;
      break;
    case 2645:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApStatus;
      break;
    case 2646:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApStatusDetail;
      break;
    case 2647:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApRadioStatus;
      break;
    case 2648:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApRadioDetail;
      break;
    case 2649:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApVapStatus;
      break;
    case 2650:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApNeighborAp;
      break;
    case 2651:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApNeighborApClient;
      break;
    case 2652:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApWlanStats;
      break;
    case 2653:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApEthStats;
      break;
    case 2654:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApStatsDetail;
      break;
    case 2655:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApRadioStats;
      break;
    case 2656:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApVapStats;
      break;
    case 2657:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientStatus;
      break;
    case 2658:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientStatusDetail;
      break;
    case 2659:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientVapStatus;
      break;
    case 2660:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientSsidStatus;
      break;
    case 2661:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientNeighborStatus;
      break;
    case 2662:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientAssociationSummary;
      break;
    case 2663:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientAssociationDetail;
      break;
    case 2664:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientSessionSummary;
      break;
    case 2665:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpClientSessionDetail;
      break;
    case 2666:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpNvDownload;
      break;
    case 2668:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpConfigurationConfig;
      break;
    case 2669:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpPeerManagedAPStatus;
      break;
    case 2670:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpConfigurationPeerReceiveStatus;
      break;
    case 2671:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPHWCapability;
      break;
    case 2672:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPHWRadioCapability;
      break;
    case 2673:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPImageCapability;
      break;
    case 2674:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpWidsApConfiguration;
      break;
    case 2675:
      errMsg = pStrInfo_wireless_HtmlFileHelpSwitchStats;
      break;
    case 2676:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpWidsClientConfiguration;
      break;
    case 2677:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPTriangulationStatus;
      break;
    case 2678:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpWidsAPRogueClassification;
      break;
    case 2679:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPImageVersion;
      break;
    case 2680:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpManagedApClientQosStatus;
      break;
    case 2682:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpLocationTrigger;
      break;
    case 2688:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpWidsAPDeAuthenticationAttackStatus;
      break;
    case 2690:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpCentTnnlCfg;
      break;
    case 2691:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpCentTnnlStatus;
      break;
    case 2692:
      errMsg = pStrInfo_wireless_HtmlFilewsHelpdistTunnel_globalCfg;
      break;
    case 2693:
      errMsg = pStrInfo_wireless_HtmlFilewsHelpdistTunnel_clientStatus;
      break;
    case 2694:
      errMsg = pStrInfo_wireless_HtmlFilewsHelpdistTunnel_managedapStatus;
      break;
    case 2695:
      errMsg = pStrInfo_wireless_HtmlFilewsHelpdistTunnel_managedapStats;
      break;    
    case 2696:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientStatus;
      break;
    case 2697:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpWidsClientRogueClassification;
      break;
    case 2698:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientPreAuth;
      break;
    case 2699:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientPreAuthSumm;
      break;
    case 2700:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientRoamHistorySumm;
      break;
    case 2701:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientTriangulation;
      break;
    case 2702:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDetectedClientRoamHistory;
      break;
    case 2703:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpOUIDataBase;
      break;
    /*XUI Help Pages*/
    case 2704:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiAdHocClient;
      break;
    case 2705:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientStatus;
      break;
    case 2706:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientNeighborStatus;
      break;
    case 2707:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientPreAuth;
      break;
    case 2708:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientRoamHistorySumm;
      break;
    case 2709:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientSsidStatus;
      break;
    case 2710:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiClientAssociationSummary;
      break;
    case 2711:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiClientSessionSummary;
      break;
    case 2712:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientStatus;
      break;
    case 2713:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiClientSwitchStatus;
      break;
    case 2714:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientVapStatus;
      break;
    case 2715:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiConfigurationPeerReceiveStatus;
      break;
    case 2716:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApUpgrade;
      break;
    case 2717:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApFailure;
      break;
    case 2718:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiPeerManagedAPStatus;
      break;
    case 2719:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiPeerSwitchStatus;
      break;
    case 2720:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiKnownClientCfg;
      break;
    case 2723:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiValidApSummary;
      break;
    case 2724:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiOUIDataBase;
      break;
    case 2725:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApReset;
      break;
    case 2726:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApDebug;
      break;
    case 2727:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiRfScan; 
      break;
    case 2728:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApStatus;
      break;
    case 2729:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApNeighborAp;
      break;
    case 2730:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApNeighborApClient; 
      break;
    case 2731:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApRadioStatus;
      break;
    case 2732:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApWlanStats;
      break;
    case 2733:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApEthStats;
      break;
    case 2734:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientPreAuthSumm;
      break;
    case 2735:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiRfScanDetail;
      break;  
    case 2736:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientDetailStatus;
      break;
    case 2737:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientRoamHistory;
      break; 
    case 2738:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientStatusDetail;
      break;
    case 2739:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiManagedApClientQosStatus;
      break;
    case 2740:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiWidsClientRogueClassification;
      break;
    case 2741:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiAPTriangulationStatus;
      break;
    case 2742:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiWidsAPRogueClassification;  
      break;
    case 2743:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDetectedClientTriangulation;
      break;
    case 2747:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiAssocClientStatisticsAssocDetail;
      break;
    case 2748:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiAssocClientStatisticsSessionDetail;
      break;
    case 2744:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpAPProvisioningSummary;
      break;
    case 2745:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRrmNeighborsSummary;
      break;
    case 2746:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpRRMChLoadHistorySummary;
      break;
    case 2749:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApDebug1;
      break;
    case 2750:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApChannelPowerAdjust;
      break;
    case 2751:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiSwitchStats;
      break; 
    case 2752:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiRFConfiguration;
      break;
    case 2753:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiManagedAPRadioDetailStatus;
      break;
    case 2754:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiDefaultProfileVAPSummary;
      break; 
    case 2755:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiManualChannelPlan;
      break;
    case 2756:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiVAPNetworkConfig;
      break;
    case 2757:
      errMsg = pStrInfo_wireless_HtmlFileHelpXuiAPProvisioningSummary;
      break;
    case 2758:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApProfileSummary;
      break; 
    case 2759:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApProfileGlobalConfig;
      break;
    case 2760:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApProfileRadioConfig;
      break;
    case 2761:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiApProfileVAPConfig;
      break;
    case 2762:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiRrmNeighborsSummary;
      break;
    case 2763:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiRRMChLoadHistorySummary;
      break;
    case 2764:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiAPImageVersion;
      break;
    case 2765:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiCentTnnlCfg;
      break;
    case 2766:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiNvDownload;
      break;
    case 2767:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiDiscovery;
      break;
    case 2768:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiConfigurationSendStatus;
      break;
    case 2769:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiCentTnnlCfg;
      break;
    case 2770:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiRadioConfig;
      break;  
    case 2780:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiWdsGroupStatusSummary;
      break;
    case 2781:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpBuildingSummary;
      break;
    case 2782:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpFlrSummary;
      break;
    case 2783:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpApSummary;
      break;
    case 2784:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpDevApSummary;
      break;
     case 2785:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiTriangApSummary;
      break;
     case 2786:
      errMsg = pStrInfo_wireless_HtmlFileWsHelpXuiTriangClientSummary;
      break;
    default:
      errMsg = pStrInfo_common_NlsError;
      break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}

