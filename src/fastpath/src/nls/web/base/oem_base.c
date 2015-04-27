/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/base/oem_base.c
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
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebBaseGetPageHdr1(L7_int32 token);

static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebBaseGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1:
    errMsg = pStrInfo_common_Iso88591;
    break;
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
    errMsg = pStrInfo_base_SysIPAddr;
    break;
  case 1008:
    errMsg = pStrInfo_base_BcastStormRecoveryMode;
    break;
  case 1009:
    errMsg = pStrInfo_common_ApHostAddr;
    break;
  case 1025:
    errMsg = pStrInfo_base_802Dot3XFlowCntrlMode;
    break;
  case 1027:
    errMsg = pStrInfo_common_SysDesc_1;
    break;
  case 1028:
    errMsg = pStrInfo_base_MachineType;
    break;
  case 1029:
    errMsg = pStrInfo_base_MachineModel;
    break;
  case 1030:
    errMsg = pStrInfo_common_ApSerialNum;
    break;
  case 1031:
    errMsg = pStrInfo_base_FruNum;
    break;
  case 1032:
    errMsg = pStrInfo_common_ApPartNum;
    break;
  case 1033:
    errMsg = pStrInfo_base_MaintenanceLvl;
    break;
  case 1034:
    errMsg = pStrInfo_base_Manufacturer;
    break;
  case 1035:
    errMsg = pStrInfo_base_BaseMacAddr;
    break;
  case 1040:
    errMsg = pStrInfo_common_ApSoftwareVer;
    break;
  case 1090:
    errMsg = pStrInfo_common_SubnetMask_3;
    break;
  case 1091:
    errMsg = pStrInfo_base_DeflGateway;
    break;
  case 1093:
    errMsg = pStrInfo_base_BurnedInMacAddr;
    break;
  case 1094:
    errMsg = pStrInfo_common_LocallyAdministeredMacAddr;
    break;
  case 1095:
    errMsg = pStrInfo_base_MacAddrType;
    break;
  case 1102:
    errMsg = pStrInfo_common_IgmpProxyAdminMode;
    break;
  case 1103:
    errMsg = pStrInfo_common_PhyMode;
    break;
  case 1104:
    errMsg = pStrInfo_base_PhyStatus;
    break;
  case 1105:
    errMsg = pStrInfo_common_LinkStatus;
    break;
  case 1106:
    errMsg = pStrInfo_common_LinkTrap;
    break;
  case 1107:
    errMsg = pStrInfo_common_IfIdx;
    break;
  case 1108:
    errMsg = pStrInfo_common_ApProfileRadioScanFrequencyAll;
    break;
  case 1114:
    errMsg = pStrInfo_base_OctetsRcvd;
    break;
  case 1115:
    errMsg = pStrErr_common_PktsRcvdWithout;
    break;
  case 1116:
    errMsg = pStrInfo_common_BcastPktsRcvd;
    break;
  case 1117:
    errMsg = pStrInfo_common_McastPktsRcvd;
    break;
  case 1138:
    errMsg = pStrInfo_base_UcastPktsRcvd;
    break;
  case 1140:
    errMsg = pStrInfo_base_ReceivePktsDiscarded;
    break;
  case 1143:
    errMsg = pStrInfo_base_OctetsTxed;
    break;
  case 1144:
    errMsg = pStrInfo_base_UcastPktsTxed;
    break;
  case 1146:
    errMsg = pStrInfo_base_TxPktsDiscarded;
    break;
  case 1147:
    errMsg = pStrErr_common_TxPktErrs;
    break;
  case 1148:
    errMsg = pStrInfo_base_TxQueueLenPkts;
    break;
  case 1151:
    errMsg = pStrInfo_common_CollisionFrames;
    break;
  case 1158:
    errMsg = pStrInfo_base_ExercisingFuncWillCauseAllCfgParamsToBeResetToDeflVals;
    break;
  case 1161:
    errMsg = pStrInfo_base_ExercisingFuncWillCauseAllSysLoginPasswdsToBeResetToDeflVals;
    break;
  case 1163:
    errMsg = pStrInfo_base_SnmpCommunityName;
    break;
  case 1164:
    errMsg = pStrInfo_common_AccessMode;
    break;
  case 1165:
    errMsg = pStrInfo_common_ApStatus;
    break;
  case 1172:
    errMsg = pStrInfo_base_SerialPortLoginTimeoutMinutes;
    break;
  case 1173:
    errMsg = pStrInfo_base_BaudRateBps;
    break;
  case 1178:
    errMsg = pStrInfo_base_CharSizeBits;
    break;
  case 1179:
    errMsg = pStrInfo_base_FlowCntrl;
    break;
  case 1180:
    errMsg = pStrInfo_base_Parity;
    break;
  case 1181:
    errMsg = pStrInfo_base_StopBits;
    break;
  case 1226:
    errMsg = pStrInfo_base_BcastPktsTxed;
    break;
  case 1230:
    errMsg = pStrInfo_common_SysName;
    break;
  case 1231:
    errMsg = pStrInfo_base_SysLocation;
    break;
  case 1232:
    errMsg = pStrInfo_base_SysContact;
    break;
  case 1233:
    errMsg = pStrInfo_base_SysObjectId;
    break;
  case 1234:
    errMsg = pStrInfo_common_ApSysUpTime;
    break;
  case 1235:
    errMsg = pStrInfo_base_MibsSupported;
    break;
  case 1236:
    errMsg = pStrInfo_base_ResettingSwitchWillCauseAllOperationsOfSwitchToStopSessionWillBeBrokenAndYouWillHaveToLogInAgainAfterSwitchHasRebootedAnyUnsavedChgsWillBeLost;
    break;
  case 1247:
    errMsg = pStrErr_common_ClientFailureAuth;
    break;
  case 1248:
    errMsg = pStrInfo_base_LinkUpDown;
    break;
  case 1249:
    errMsg = pStrInfo_base_MultipleUsrs;
    break;
  case 1250:
    errMsg = pStrInfo_base_SpanTree_1;
    break;
  case 1252:
    errMsg = pStrInfo_base_NumOfTrapsSinceLastReset;
    break;
  case 1253:
    errMsg = pStrInfo_base_NumOfTrapsSinceLogLastViewed;
    break;
  case 1254:
    errMsg = pStrInfo_common_Log_1;
    break;
  case 1255:
    errMsg = pStrInfo_common_ApSysUpTime;
    break;
  case 1256:
    errMsg = pStrInfo_base_Trap_1;
    break;
  case 1259:
    errMsg = pStrInfo_base_SnmpCommunityName;
    break;
  case 1263:
    errMsg = pStrInfo_base_FileType;
    break;
  case 1264:
    errMsg = pStrInfo_base_TftpSrvrAddrType;
    break;
  case 1265:
    errMsg = pStrInfo_base_TftpFilePath;
    break;
  case 1266:
    errMsg = pStrInfo_base_TftpFileName;
    break;
  case 1267:
    errMsg = pStrInfo_common_StartFileTransfer;
    break;
  case 1269:
    errMsg = pStrInfo_common_ConfirmPasswd;
    break;
  case 1271:
    errMsg = pStrInfo_base_WebMode;
    break;
  case 1282:
    errMsg = pStrInfo_base_NwCfgIPv4ProtoCurrent;
    break;
  case 1283:
    errMsg = pStrInfo_common_Bgp4Traps;
    break;
  case 1284:
    errMsg = pStrInfo_base_McastStormRecoveryMode;
    break;
  case 1285:
    errMsg = pStrInfo_base_UcastStormRecoveryMode;
    break;
  case 1286:
    errMsg = pStrInfo_base_BcastStormRecoveryLvl;
    break;
  case 1287:
    errMsg = pStrInfo_base_McastStormRecoveryLvl;
    break;
  case 1288:
    errMsg = pStrInfo_base_UcastStormRecoveryLvl;
    break;
  case 1300:
    errMsg = pStrErr_base_TotalPktsRcvdWithoutErrs;
    break;
  case 1304:
    errMsg = pStrErr_common_PktsTxedWithoutErrs;
    break;
  case 1307:
    errMsg = pStrInfo_base_McastPktsTxed;
    break;
  case 1309:
    errMsg = pStrInfo_common_TftpSrvrAddr;
    break;
  case 1310:
    errMsg = pStrInfo_base_MacAddrSearch;
    break;
  case 1314:
    errMsg = pStrInfo_common_IntfMode;
    break;
  case 1320:
    errMsg = pStrInfo_base_ClientIpAddr;
    break;
  case 1321:
    errMsg = pStrInfo_base_ClientIpMask;
    break;
  case 1336:
    errMsg = pStrInfo_base_PortType;
    break;
  case 1337:
    errMsg = pStrInfo_common_StpMode;
    break;
  case 1338:
    errMsg = pStrInfo_base_FwdState;
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
  case 1342:
    errMsg = pStrInfo_base_PasswdMinLen;
    break;
  case 1343:
    errMsg = pStrInfo_base_PasswdAgingDays;
    break;
  case 1344:
    errMsg = pStrInfo_base_PasswdHistory;
    break;
  case 1345:
    errMsg = pStrInfo_base_LockoutAttempts;
    break;
  case 1346:
    errMsg = pStrInfo_base_LockoutStatus;
    break;
  case 1347:
    errMsg = pStrInfo_base_PasswdExpirationDate;
    break;
  case 1356:
    errMsg = pStrInfo_base_ServicePortCfgProtoCurrent;
    break;
  case 1375:
    errMsg = pStrErr_common_PktsRcvdWith;
    break;
  case 1400:
    errMsg = pStrInfo_base_AddrEntriesCurrentlyInUse;
    break;
  case 1402:
    errMsg = pStrInfo_base_MostAddrEntriesEverUsed;
    break;
  case 1406:
    errMsg = pStrInfo_base_VlanEntriesCurrentlyInUse;
    break;
  case 1407:
    errMsg = pStrInfo_base_MaxVlanEntries;
    break;
  case 1408:
    errMsg = pStrInfo_base_MostVlanEntriesEverUsed;
    break;
  case 1409:
    errMsg = pStrInfo_base_DynVlanEntries;
    break;
  case 1410:
    errMsg = pStrInfo_base_VlanDels;
    break;
  case 1419:
    errMsg = pStrInfo_common_TimeSinceCountersLastClred;
    break;
  case 1447:
    errMsg = pStrInfo_base_AddrEntriesInUse;
    break;
  case 1450:
    errMsg = pStrInfo_base_StaticVlanEntries;
    break;
  case 1452:
    errMsg = pStrInfo_base_MustEnblOspfAdminMode;
    break;
  case 1543:
    errMsg = pStrInfo_base_AgingIntvlSecs;
    break;
  case 1548:
    errMsg = pStrInfo_common_Proto;
    break;
  case 1606:
    errMsg = pStrInfo_base_YouHaveMadeCfgsToSwitchWithoutSavingmWouldYouLikeToSaveChgsBeforeYouResetSys;
    break;
  case 1639:
    errMsg = pStrInfo_common_Intf;
    break;
  case 1733:
    errMsg = pStrInfo_base_OspfTraps;
    break;
  case 1740:
    errMsg = pStrInfo_base_JavaMode;
    break;
  case 1795:
    errMsg = pStrInfo_common_Direction;
    break;
  case 1885:
    errMsg = pStrInfo_base_MaxFrameSize;
    break;
  case 2112:
    errMsg = pStrInfo_common_Filter;
    break;
  case 2310:
    errMsg = pStrInfo_base_BcastStormMode;
    break;
  case 2311:
    errMsg = pStrInfo_base_BcastStormLvl;
    break;
  case 2312:
    errMsg = pStrInfo_base_McastStormMode;
    break;
  case 2313:
    errMsg = pStrInfo_base_McastStormLvl;
    break;
  case 2314:
    errMsg = pStrInfo_base_UcastStormMode;
    break;
  case 2315:
    errMsg = pStrInfo_base_UcastStormLvl;
    break;
  case 2320:
    errMsg = pStrInfo_base_DenialOfServiceSipDip;
    break;
  case 2321:
    errMsg = pStrInfo_base_DenialOfServiceFirstFragment;
    break;
  case 2322:
    errMsg = pStrInfo_base_DenialOfServiceMinTcpHdrSize;
    break;
  case 2323:
    errMsg = pStrInfo_base_DenialOfServiceTcpFragment;
    break;
  case 2324:
    errMsg = pStrInfo_base_DenialOfServiceTcpFlag;
    break;
  case 2325:
    errMsg = pStrInfo_base_DenialOfServiceL4Port;
    break;
  case 2326:
    errMsg = pStrInfo_base_DenialOfServiceIcmp;
    break;
  case 2327:
    errMsg = pStrInfo_base_DenialOfServiceMaxIcmpSize;
    break;
  case 2328:
    errMsg = pStrInfo_base_DenialOfServiceIcmpv4;
    break;
  case 2329:
    errMsg = pStrInfo_base_DenialOfServiceMaxIcmpv4Size;
    break;
  case 2330:
    errMsg = pStrInfo_base_DenialOfServiceIcmpv6;
    break;
  case 2331:
    errMsg = pStrInfo_base_DenialOfServiceMaxIcmpv6Size;
    break;
  case 2332:
    errMsg = pStrInfo_base_DenialOfServiceIcmpfrag;
    break;
  case 2333:
    errMsg = pStrInfo_base_DenialOfServiceSmacDmac;
    break;
  case 2334:
    errMsg = pStrInfo_base_DenialOfServiceTcpFinUrgPsh;
    break;
  case 2335:
    errMsg = pStrInfo_base_DenialOfServiceTcpFlagSeq;
    break;
  case 2336:
    errMsg = pStrInfo_base_DenialOfServiceTcpOffset;
    break;
  case 2337:
    errMsg = pStrInfo_base_DenialOfServiceTcpSyn;
    break;
  case 2338:
    errMsg = pStrInfo_base_DenialOfServiceTcpSynFin;
    break;
  case 2339:
    errMsg = pStrInfo_base_DenialOfServiceTcpPort;
    break;
  case 2340:
    errMsg = pStrInfo_base_DenialOfServiceUdpPort;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 4006:
    errMsg = pStrInfo_base_BurnedIn;
    break;
  case 4007:
    errMsg = pStrInfo_base_LocallyAdministered;
    break;
  case 4205:
    errMsg = pStrInfo_common_Name_1;
    break;
  case 4400:
    errMsg = pStrInfo_base_NoteMaxFrameSizeWillBeAppliedSysWideAndSetForAllPorts;
    break;
  case 5000:
    errMsg = pStrInfo_base_FwdDbase;
    break;
  case 5001:
    errMsg = pStrInfo_common_MacAddr_2;
    break;
  case 5003:
    errMsg = pStrInfo_common_ApStatus;
    break;
  case 5006:
    errMsg = pStrInfo_base_Learned;
    break;
  case 5015:
    errMsg = pStrInfo_base_SavingAllAppliedChgsWillCauseAllChgsToCfgPanelsThatWereAppliedButNotSavedToBeSavedThusRetainingNewValsAcrossASysReboot;
    break;
  case 5018:
    errMsg = pStrInfo_common_Dsbld;
    break;
  case 5025:
    errMsg = pStrInfo_common_Md5;
    break;
  case 5120:
    errMsg = pStrInfo_base_DownloadingCfgWillForceAResetOnCompletionConfirmToContinue;
    break;
  case 5200:
    errMsg = pStrInfo_common_None_1;
    break;
  case 5201:
    errMsg = pStrInfo_base_Bootp;
    break;
  case 5202:
    errMsg = pStrInfo_base_Dhcp;
    break;
  case 5307:
    errMsg = pStrInfo_base_ConfirmToResetSysCfg;
    break;
  case 5310:
    errMsg = pStrInfo_common_Desc_1;
    break;
  case 6410:
    errMsg = pStrInfo_base_CustomerId;
    break;
  case 6411:
    errMsg = pStrInfo_base_IntfEthertype;
    break;
  case 6412:
    errMsg = pStrInfo_base_Dot1qTag;
    break;
  case 6413:
    errMsg = pStrInfo_base_VmanTag;
    break;
  case 6414:
    errMsg = pStrInfo_base_CustomTag;
    break;
  case 6415:
    errMsg = pStrInfo_base_CustomVal;
    break;
  case 7022:
    errMsg = pStrInfo_common_Ping;
    break;
  case 7243:
    errMsg = pStrInfo_base_ConfirmToResetSysAllConnsWillBeLostToRegainConnYouWillHaveToLogInAgainLater;
    break;
  case 7320:
    errMsg = pStrInfo_base_ReadWrite;
    break;
  case 7321:
    errMsg = pStrInfo_base_ReadOnly;
    break;
  case 7351:
    errMsg = pStrInfo_base_LacpMode;
    break;
  case 7525:
    errMsg = pStrInfo_base_SysSntpSyncTime;
    break;
  case 7526:
    errMsg = pStrInfo_base_SysSntpSyncTime_fail;
    break;
  case 9004:
    errMsg = pStrInfo_common_PimTraps;
    break;
  case 9005:
    errMsg = pStrInfo_common_DvmrpTraps;
    break;
  case 11115:
    errMsg = pStrInfo_common_Unit_2;
    break;
  case 11122:
    errMsg = pStrInfo_common_Mode_1;
    break;
  case 11712:
    errMsg = pStrInfo_common_Community;
    break;
  case 11756:
    errMsg = pStrInfo_base_WebAutoInstallStart;
    break;
  case 11757:
    errMsg = pStrInfo_base_WebAutoInstallStop;
    break;
  case 11815:
    errMsg = pStrInfo_common_Community;
    break;
  case 12393:
    errMsg = pStrInfo_common_PortRole;
    break;
  case 12399:
    errMsg = pStrInfo_common_Cst;
    break;
  case 12403:
    errMsg = pStrInfo_common_MstId;
    break;
  case 12683:
    errMsg = pStrInfo_base_CableStatus;
    break;
  case 12684:
    errMsg = pStrInfo_base_CableLen;
    break;
  case 12685:
    errMsg = pStrErr_base_FailureLocation;
    break;
  case 12690:
    errMsg = pStrInfo_base_Sha;
    break;
  case 12691:
    errMsg = pStrInfo_base_Des;
    break;
  case 12692:
    errMsg = pStrInfo_base_AuthProto;
    break;
  case 12693:
    errMsg = pStrInfo_base_EncryptionProto;
    break;
  case 12694:
    errMsg = pStrInfo_base_EncryptionKey;
    break;
  case 12695:
    errMsg = pStrInfo_base_SnmpV3UsrCfg;
    break;
  case 12697:
    errMsg = pStrInfo_base_CableTestResults;
    break;
  case 12732:
    errMsg = pStrInfo_base_FlowCntrlMode;
    break;
  case 12852:
    errMsg = pStrInfo_base_SnmpV3AccessMode;
    break;
  case 14105:
    errMsg = pStrInfo_common_MgmtUnit_1;
    break;
  case 16102:
    errMsg = pStrInfo_common_LocalAuth;
    break;
  case 16103:
    errMsg = pStrInfo_common_RadiusAuth;
    break;
  case 16104:
    errMsg = pStrInfo_base_RejectAuth;
    break;
  case 16107:
    errMsg = pStrInfo_base_LoginUsrs;
    break;
  case 16108:
    errMsg = pStrInfo_base_Dot1xPortSecurityUsrs;
    break;
  case 16109:
    errMsg = pStrInfo_base_Method1_1;
    break;
  case 16110:
    errMsg = pStrInfo_base_Method2_1;
    break;
  case 16111:
    errMsg = pStrInfo_base_Method3_1;
    break;
  case 16112:
    errMsg = pStrInfo_base_MethodList;
    break;
  case 16113:
    errMsg = pStrInfo_common_Local;
    break;
  case 16121:
    errMsg = pStrInfo_base_Usr_1;
    break;
  case 16122:
    errMsg = pStrInfo_base_UnDefinedAuth;
    break;
  case 16123:
    errMsg = pStrInfo_common_Radius;
    break;
  case 16271:
    errMsg = pStrInfo_base_SnmpVer;
    break;
  case 16305:
    errMsg = pStrInfo_base_Crit;
    break;
  case 16306:
    errMsg = pStrInfo_base_High;
    break;
  case 16307:
    errMsg = pStrInfo_base_Low;
    break;
  case 16350:
    errMsg = pStrInfo_common_Id;
    break;
  case 16351:
    errMsg = pStrInfo_common_ClientUsrName;
    break;
  case 16352:
    errMsg = pStrInfo_base_ConnFrom;
    break;
  case 16353:
    errMsg = pStrInfo_base_IdleTime;
    break;
  case 16354:
    errMsg = pStrInfo_base_SessionTime;
    break;
  case 16355:
    errMsg = pStrInfo_base_SessionType;
    break;
  case 21147:
    errMsg = pStrInfo_base_DstPort;
    break;
  case 21181:
    errMsg = pStrInfo_base_TxAndRx;
    break;
  case 21182:
    errMsg = pStrInfo_base_IngressDirection;
    break;
  case 21183:
    errMsg = pStrInfo_common_EgressDirection;
    break;
  case 21184:
    errMsg = pStrInfo_base_Session;
    break;
  case 21185:
    errMsg = pStrInfo_base_SrcPortS;
    break;
  case 21280:
    errMsg = pStrInfo_common_To_5;
    break;
  case 21293:
    errMsg = pStrInfo_base_ImageName;
    break;
  case 21294:
    errMsg = pStrInfo_base_Image1Desc;
    break;
  case 21295:
    errMsg = pStrInfo_base_Image2Desc;
    break;
  case 21296:
    errMsg = pStrInfo_base_Image1Ver;
    break;
  case 21297:
    errMsg = pStrInfo_base_Image2Ver;
    break;
  case 21298:
    errMsg = pStrInfo_base_CurrentActive;
    break;
  case 21299:
    errMsg = pStrInfo_base_NextActive;
    break;
  case 21503:
    errMsg = pStrInfo_common_PortDesc;
    break;
  case 21550:
    errMsg = pStrInfo_base_PhyAddr;
    break;
  case 21551:
    errMsg = pStrInfo_base_PortlistBitOffset;
    break;
  case 22016:
    errMsg = pStrInfo_base_NumTrapLogCapacity;
    break;
  case 22020:
    errMsg = pStrInfo_common_AclTraps;
    break;
  case 22046:
    errMsg = pStrInfo_common_Ipv6Prefix_1;
    break;
  case 22074:
    errMsg = pStrInfo_base_Tacacs_1;
    break;
  case 22075:
     errMsg = pStr_Info_common_AlphanumericCharacters;
     break;
  case 22076:
     errMsg = pStrInfo_common_PimSmGrpRpMapStatic;
     break;
  case 22077:
     errMsg = pStrInfo_common_Dyn_1;
     break;
  case 22078:
     errMsg = pStrInfo_poe_class_based;
     break;
  case 22079:
     errMsg = pStrInfo_poe_user_defined;
     break;
  case 22080:
     errMsg = pStrInfo_poe_auto_reset_mode;
     break;
  case 22081:
     errMsg = pStrInfo_poe_high_power_mode;
     break;
  case 22082:
     errMsg = pStrInfo_poe_legacy;
     break;
  case 22083:
     errMsg = pStrInfo_poe_4pt_legacy;
     break;
  case 22084:
     errMsg = pStrInfo_poe_4pt;
     break;
  case 22085:
     errMsg = pStrInfo_poe_2pt;
     break;
  case 22086:
     errMsg = pStrInfo_poe_2pt_legacy;
     break;
  case 22087:
     errMsg = pStrInfo_poe_detection_mode;
     break;
  case 22088:
     errMsg = pStrInfo_poe_temperature;
     break;
  case 22089:
     errMsg = pStrInfo_poe_power_limit_type;
     break;
  case 22090:
     errMsg = pStrInfo_poe_power_mgmt_mode;
     break;
  case 27164:
     errMsg = pStrInfo_common_CaptivePortal;
     break;
  case 27401:
    errMsg = pStrInfo_base_WebAutoInstall;
    break;  
  case 27402:
    errMsg = pStrInfo_base_WebAutoInstallMode;
    break;
  case 27403:
    errMsg = pStrInfo_base_WebAutoInstallState;
    break;
  case 27404:
    errMsg = pStrInfo_base_HttpEraseStartupConfigScript;
    break;
  case 27405:
    errMsg = pStrInfo_base_WebAutoInstallAutoSaveMode;
    break;
  case 27406:
    errMsg = pStrInfo_base_WebAutoInstallRetryCount;
    break;
  case 27407:
    errMsg = pStrInfo_base_WebAutoInstallAutoSaveState;
    break;
  case 27408:
    errMsg = pStrInfo_base_WebAutoInstallRetryInfiniteState;
    break;
  case 27409:
    errMsg = pStrInfo_base_DhcpVendorClassIdMode;
    break;
  case 27410:
    errMsg = pStrInfo_base_DhcpVendorClassIdStr;
    break;

  case 27421:
     errMsg = pStrInfo_base_IsdpMessageInterval;
     break;
  case 27422:
     errMsg = pStrInfo_base_IsdpMode;
     break;
  case 27423:
     errMsg = pStrInfo_base_IsdpHoldtimeInterval;
     break;
  case 27424:
     errMsg = pStrInfo_base_IsdpDevIdTitle;
     break;
  case 27425:
     errMsg = pStrInfo_base_IsdpV2Mode;
     break;
  case 27426:
     errMsg = pStrInfo_base_IsdpDevIdFormatCpbTitle;
     break;
  case 27427:
     errMsg = pStrInfo_base_IsdpDevIdFormatTitle;
     break;
  case 27428:
     errMsg = pStrInfo_base_StatIsdpPduRx;
     break;
  case 27429:
     errMsg = pStrInfo_base_StatIsdpPduTx;
     break;
  case 27430:
     errMsg = pStrInfo_base_StatIsdpV1PduRx;
     break;
  case 27431:
     errMsg = pStrInfo_base_StatIsdpV1PduTx;
     break;
  case 27432:
     errMsg = pStrInfo_base_StatIsdpV2PduRx;
     break;
  case 27433:
     errMsg = pStrInfo_base_StatIsdpV2PduTx;
     break;
  case 27434:
     errMsg = pStrInfo_base_StatIsdpBadHeader;
     break;
  case 27435:
     errMsg = pStrInfo_base_StatIsdpChecksumErr;
     break;
  case 27436:
     errMsg = pStrInfo_base_StatIsdpTransmissionFailure;
     break;
  case 27437:
     errMsg = pStrInfo_base_StatIsdpInvalidPduFormat;
     break;
  case 27438:
     errMsg = pStrInfo_base_StatIsdpIPAddressTableFull;
     break;
  case 27439:
     errMsg = pStrInfo_base_StatIsdpTableFull;
     break;
  case 27440:
     errMsg = pStrInfo_base_IsdpDevIdTitle;
     break;
  case 27441:
     errMsg = pStrInfo_common_Intf;
     break;
  case 27442:
     errMsg = pStrInfo_base_IsdpHoldtimeTitle;
     break;
  case 27443:
     errMsg = pStrInfo_base_IsdpCpbTitle;
     break;
  case 27444:
     errMsg = pStrInfo_base_IsdpPlatformTitle;
     break;
  case 27445:
     errMsg = pStrInfo_base_IsdpPortIdTitle;
     break;
  case 27446:
     errMsg = pStrInfo_base_IsdpTitleHlp;
     break;
  case 27447:
     errMsg = pStrInfo_base_IsdpIpAddrTitle;
     break;
  case 27448:
     errMsg = pStrInfo_base_IsdpVersionTitle;
     break;
  case 27449:
     errMsg = pStrInfo_base_IsdpLastTimeChanged;
     break;
  case 27450:
     errMsg = pStrInfo_base_IsdpProtVer;
     break;

  case 27203:
    errMsg = pStrInfo_base_Dot3ahLanOamTestResults;
    break;

  case 28001:
    errMsg = pStrInfo_base_TR069AcsUrl;
    break;
  case 28002:
     errMsg = pStrInfo_base_TR069AcsUser;
     break;
  case 28003:
     errMsg = pStrInfo_base_TR069AcsPeriodicInformMode;
     break;
  case 28004:
    errMsg = pStrInfo_base_TR069AcsPeriodicInformInterval;
    break;
  case 28005:
    errMsg = pStrInfo_base_TR069AcsPeriodicInformTime;
    break;
  case 28006:
    errMsg = pStrInfo_base_TR069AcsUpgradesManaged;
    break;
  case 28007:
    errMsg = pStrInfo_base_TR069AcsConnectionRequestUser;
    break;
  case 28008:
    errMsg = pStrInfo_base_TR069AcsConnectionRequestUrl;
    break;
  case 28009:
    errMsg = pStrInfo_base_TR069AcsParameterKey;
    break;
  case 28011:
    errMsg = pStrInfo_base_TR069StatsInformMsgsSent;
    break;
  case 28012:
    errMsg = pStrInfo_base_TR069StatsConnReqRxed;
    break;
  case 28013:
    errMsg = pStrInfo_base_TR069StatsTotalFaults;
    break;
  case 28014:
    errMsg = pStrInfo_base_TR069StatsMethoNotSuppFaults;
    break;
  case 28015:
    errMsg = pStrInfo_base_TR069StatsReqDeniedFaults;
    break;
  case 28016:
    errMsg = pStrInfo_base_TR069StatsInternalErrors;
    break;
  case 28017:
    errMsg = pStrInfo_base_TR069StatsInvalidArgFaults;
    break;
  case 28018:
    errMsg = pStrInfo_base_TR069StatsResrcsExceededFaults;
    break;
  case 28019:
    errMsg = pStrInfo_base_TR069StatsInvalidParamNameFaults;
    break;
  case 28020:
    errMsg = pStrInfo_base_TR069StatsInvalidParamTypeFaults;
    break;
  case 28021:
    errMsg = pStrInfo_base_TR069StatsInvalidParamValueFaults;
    break;
  case 28022:
    errMsg = pStrInfo_base_TR069StatsInvalidWriteAttemptFaults;
    break;
  case 28023:
    errMsg = pStrInfo_base_TR069StatsNotificationReqRejections;
    break;
  case 28024:
    errMsg = pStrInfo_base_TR069StatsDnloadFailures;
    break;
  case 28025:
    errMsg = pStrInfo_base_TR069StatsUploadFailures;
    break;
  case 28026:
    errMsg = pStrInfo_base_TR069StatsFileTransferServerAuthFailures;
    break;
  case 28027:
    errMsg = pStrInfo_base_TR069StatsVendorDefaultFaults;
    break;
  case 28028:
    errMsg = pStrInfo_base_TR069AcsUserPasswd;
    break;
  case 28029:
    errMsg = pStrInfo_base_TR069AcsUserPasswdConfirm;
    break;
  case 28030:
    errMsg = pStrInfo_base_TR069AcsConnReqUserPasswd;
    break;
  case 28031:
    errMsg = pStrInfo_base_TR069AcsConnReqUserPasswdConfirm;
    break;
  case 28032:
    errMsg = pStrInfo_common_True2;
    break;
  case 28033:
   errMsg = pStrInfo_common_False2;
   break;
  case 28034:
    errMsg = pStrInfo_base_TR069AcsRpcMethods;
    break;
  case 28040:
    errMsg = pStrInfo_base_Tr069ServerCACert;
    break;
  case 28041:
    errMsg = pStrInfo_base_Tr069ClientCert;
    break;
  case 28042:
    errMsg = pStrInfo_base_Tr069ClientPrivateKey;
    break;
  case 28043:
    errMsg = pStrInfo_common_WirelessModeTraps;
    break;
  
  default:
    local_call=L7_TRUE;
    errMsg = usmWebBaseGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
 return errMsg;
}

L7_char8 *usmWebBaseGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1010:
    errMsg = pStrInfo_base_ConfirmCfgFileDownload;
    break;
  case 1048:
    errMsg = pStrInfo_base_SysUsageThresh;
    break;
  case 1054:
    errMsg = pStrInfo_base_DhcpSrvrConflictsInfo;
    break;
  case 1055:
    errMsg = pStrInfo_base_DhcpSrvrPoolOptions;
    break;
  case 1056:
    errMsg = pStrInfo_base_BufedLogCfg;
    break;
  case 1057:
    errMsg = pStrInfo_base_BufedLogs;
    break;
  case 1058:
    errMsg = pStrInfo_base_PersistentLogCfg;
    break;
  case 1059:
    errMsg = pStrInfo_base_PersistentLogs;
    break;
  case 1060:
    errMsg = pStrInfo_base_ConsoleLogCfg;
    break;
  case 1061:
    errMsg = pStrInfo_common_Pri_1;
    break;
  case 1062:
    errMsg = pStrInfo_base_power_limit;
    break;
  case 1063:
    errMsg = pStrInfo_base_MaxSysPower;
    break;
  case 1064:
    errMsg = pStrInfo_base_CurrSysPower;
    break;
  default:
    local_call=L7_TRUE;
    errMsg = usmWebBaseGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebBaseGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1000:
    errMsg = pStrInfo_common_SysDesc_1;
    break;
  case 1001:
    errMsg = pStrInfo_base_NwConnectivityCfg;
    break;
  case 1002:
    errMsg = pStrInfo_base_FwdDbaseSearch;
    break;
  case 1003:
    errMsg = pStrInfo_base_SwitchCfg;
    break;
  case 1004:
    errMsg = pStrInfo_base_ServicePortCfg;
    break;
  case 1005:
    errMsg = pStrInfo_common_DownloadFileToSwitch;
    break;
  case 1006:
    errMsg = pStrInfo_base_UploadFileFromSwitch;
    break;
  case 1007:
    errMsg = pStrInfo_base_TelnetSessionCfg;
    break;
  case 1008:
    errMsg = pStrInfo_base_SerialPortCfg;
    break;
  case 1009:
    errMsg = pStrInfo_base_UsrAccounts;
    break;
  case 1011:
    errMsg = pStrInfo_base_ResetCfgToDefls;
    break;
  case 1012:
    errMsg = pStrInfo_base_SysReset;
    break;
  case 1013:
    errMsg = pStrInfo_base_EventLog;
    break;
  case 1014:
    errMsg = pStrInfo_base_FwdDbaseCfg;
    break;
  case 1015:
    errMsg = pStrInfo_base_InventoryInfo;
    break;
  case 1017:
    errMsg = pStrInfo_base_SaveAllAppliedChgs;
    break;
  case 1018:
    errMsg = pStrInfo_base_SysReset;
    break;
  case 1019:
    errMsg = pStrInfo_base_ResetCfgToDefls;
    break;
  case 1020:
    errMsg = pStrInfo_base_ResetPasswdsToDefls;
    break;
  case 1021:
    errMsg = pStrInfo_common_Ping;
    break;
  case 1022:
    errMsg = pStrInfo_base_TrapFlagsCfg;
    break;
  case 1023:
    errMsg = pStrInfo_base_TrapLog_1;
    break;
  case 1024:
    errMsg = pStrInfo_base_CableTest_1;
    break;
  case 1025:
    errMsg = pStrInfo_base_ArpCache;
    break;
  case 1027:
    errMsg = pStrInfo_base_PortDetailedStats;
    break;
  case 1028:
    errMsg = pStrInfo_base_PortSummaryStats;
    break;
  case 1029:
    errMsg = pStrInfo_base_SwitchDetailedStats;
    break;
  case 1030:
    errMsg = pStrInfo_base_SwitchSummaryStats;
    break;
  case 1031:
    errMsg = pStrInfo_base_SnmpCommunityCfg;
    break;
  case 1033:
    errMsg = pStrInfo_base_SnmpSupportedMibs;
    break;
  case 1034:
    errMsg = pStrInfo_base_SnmpTrapReceiverCfg;
    break;
  case 1035:
    errMsg = pStrInfo_base_PortCfg;
    break;
  case 1036:
    errMsg = pStrInfo_base_PortSummary;
    break;
  case 1037:
    errMsg = pStrInfo_base_AuthListCfg;
    break;
  case 1038:
    errMsg = pStrInfo_base_UsrLoginCfg;
    break;
  case 1041:
    errMsg = pStrInfo_base_AuthListSummary;
    break;
  case 1043:
    errMsg = pStrInfo_base_LoginSessions;
    break;
  case 1044:
    errMsg = pStrInfo_base_SnmpTrapReceiverSummary;
    break;
  case 1045:
    errMsg = pStrInfo_base_NwNdpSummary;
    break;
  case 1046:
    errMsg = pStrInfo_base_ServicePortNdpSummary;
    break;
  case 1047:
    errMsg = pStrInfo_base_PoeCfg;
    break;
  case 1049:
    errMsg = pStrInfo_base_DhcpSrvrGlobalCfg;
    break;
  case 1050:
    errMsg = pStrInfo_base_DhcpSrvrPoolCfg;
    break;
  case 1051:
    errMsg = pStrInfo_base_DhcpSrvrResetCfg;
    break;
  case 1052:
    errMsg = pStrInfo_base_DhcpSrvrBindingsInfo;
    break;
  case 1053:
    errMsg = pStrInfo_base_DhcpSrvrStats;
    break;
  case 1054:
    errMsg = pStrInfo_base_DhcpSrvrConflictsInfo;
    break;
  case 1055:
    errMsg = pStrInfo_base_DhcpSrvrPoolOptions;
    break;
  case 1056:
    errMsg = pStrInfo_base_BufedLogCfg;
    break;
  case 1057:
    errMsg = pStrInfo_base_BufedLogs;
    break;
  case 1058:
    errMsg = pStrInfo_base_PersistentLogCfg;
    break;
  case 1059:
    errMsg = pStrInfo_base_PersistentLogs;
    break;
  case 1060:
    errMsg = pStrInfo_base_ConsoleLogCfg;
    break;
  case 1061:
    errMsg = pStrInfo_base_SyslogCfg;
    break;
  case 1062:
    errMsg = pStrInfo_base_HostsCfg;
    break;
  case 1063:
    errMsg = pStrInfo_base_SntpGlobalCfg;
    break;
  case 1064:
    errMsg = pStrInfo_base_SntpGlobalStatus;
    break;
  case 1065:
    errMsg = pStrInfo_base_SntpSrvrCfg;
    break;
  case 1066:
    errMsg = pStrInfo_base_SntpSrvrStatus;
    break;
  case 1067:
    errMsg = pStrInfo_base_License;
    break;
  case 1068:
    errMsg = pStrInfo_base_OutboundTelnetClientCfg;
    break;
  case 1069:
    errMsg = pStrInfo_base_PoeCfg;
    break;
  case 1070:
    errMsg = pStrInfo_base_DenialOfServiceCfg;
    break;
  case 1071:
    errMsg = pStrInfo_base_HttpFileDownload;
    break;
  case 1072:
    errMsg = pStrInfo_base_PoeStatus;
    break;
  case 1082:
    errMsg = pStrInfo_base_MultiplePortMirroring;
    break;
  case 1092:
    errMsg = pStrInfo_base_CmdLoggerCfg;
    break;
  case 1094:
    errMsg = pStrInfo_base_MultiplePortMirroringAddSrcPorts;
    break;
  case 1095:
    errMsg = pStrInfo_base_DualImageStatus;
    break;
  case 1096:
    errMsg = pStrInfo_base_DualImageCfg;
    break;
  case 1097:
    errMsg = pStrInfo_common_PortDesc;
    break;
  case 1098:
    errMsg = pStrInfo_base_MultiplePortMirroringRemoveSrcPorts;
    break;
  case 1099:
    errMsg = pStrInfo_base_Traceroute;
    break;
  case 1100:
    errMsg = pStrInfo_base_HttpConfiguration;
    break;
  case 1101:
    errMsg = pStrInfo_base_PasswordManagement;
    break;
  case 1143:
    errMsg = pStrInfo_base_DoubleVlanTunneling;
    break;
  case 1144:
    errMsg = pStrInfo_base_DoubleVlanTunnelingSummary;
    break;
  case 1281:
    errMsg = pStrInfo_base_sFlowAgentSummery;
    break;
  case 1282:
    errMsg = pStrInfo_base_sFlowReceiverCfg;
    break;
  case 1283:
    errMsg = pStrInfo_base_sFlowSamplerCfg;
    break;
  case 1284:
    errMsg = pStrInfo_base_sFlowPollerCfg;
    break;
  case 1451:
    errMsg = pStrInfo_base_CardCfg;
    break;
  case 1452:
    errMsg = pStrInfo_base_SlotSummary;
    break;
  case 1458:
    errMsg = pStrInfo_base_SupportedCards;
    break;
   case 1831:
    errMsg = pStrInfo_base_dnsglobalcfg;
    break;
    case 1832:
    errMsg = pStrInfo_base_dnsservercfg;
    break;
    case 1833:
    errMsg = pStrInfo_base_dnshostnameipmapcfg;
    break;
    case 1834:
    errMsg = pStrInfo_base_dnsdomainlistcfg;
    break;
    case 1835:
    errMsg = pStrInfo_base_dnshostnameipmapsummary;    
    break;
    case 2901:
    errMsg = pStrInfo_base_IsdpGlobConfHeader;
    break;
    case 2902:
    errMsg = pStrInfo_base_IsdpStatsHeader;
    break;
    case 2903:
    errMsg = pStrInfo_base_IsdpIntfHeader;
    break;
    case 2904:
    errMsg = pStrInfo_base_IsdpCacheHeader;
    break;
    case 1900:
    errMsg = pStrInfo_base_DhcpVendorClassIdOptions;
    break;
    case 2000:
    errMsg = pStrInfo_base_TR069Configuration;
    break;
    case 2001:
    errMsg = pStrInfo_base_TR069Statistics;
    break;
    case 2002:
    errMsg = pStrInfo_base_TR069RpcMethods;
    break;
    case 27164:
    errMsg = pStrInfo_base_WebAutoInstall;
    break;
    case 27167:
    errMsg = pStrInfo_base_WebAutoInstallErase;
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

L7_char8 *usmWebBaseGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2000:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoSysdesc;
    break;
  case 2001:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgNetcon;
    break;
  case 2002:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoMacaddr;
    break;
  case 2003:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgSwcfg;
    break;
  case 2004:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgService;
    break;
  case 2005:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpTransferDownload;
    break;
  case 2006:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpTransferUpload;
    break;
  case 2007:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgTelnetcfg;
    break;
  case 2008:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgSerport;
    break;
  case 2009:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpUsrsUsracct;
    break;
  case 2010:
    errMsg = pStrInfo_common_HtmlFileBaseSysHelpIdx;
    break;
  case 2012:
    errMsg = pStrInfo_common_HtmlFileBaseSysHelpIdx;
    break;
  case 2013:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoEventlog;
    break;
  case 2014:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoMacaddrcfg;
    break;
  case 2015:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoInvinfo;
    break;
  case 2017:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgSavechange;
    break;
  case 2018:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgSysrest;
    break;
  case 2019:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgRescfg;
    break;
  case 2020:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpUsrsRespass;
    break;
  case 2021:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgPing;
    break;
  case 2022:
    errMsg = pStrInfo_base_HtmlLinkBaseSysTrapmgrHelpTrapsTrapflag;
    break;
  case 2023:
    errMsg = pStrInfo_base_HtmlLinkBaseSysTrapmgrHelpTrapsTraplog;
    break;
  case 2024:
    errMsg = pStrInfo_base_HtmlFileBaseSysHelpCableTest;
    break;
  case 2025:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSysinfoArpcache;
    break;
  case 2027:
    errMsg = pStrInfo_base_HtmlLinkBaseSysStatsHelpStatsPortdetstat;
    break;
  case 2028:
    errMsg = pStrInfo_base_HtmlLinkBaseSysStatsHelpStatsPortsumstat;
    break;
  case 2029:
    errMsg = pStrInfo_base_HtmlLinkBaseSysStatsHelpStatsSwstat;
    break;
  case 2030:
    errMsg = pStrInfo_base_HtmlLinkBaseSysStatsHelpStatsSwsumstat;
    break;
  case 2031:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSnmpHelpSnmpSnmpcomm;
    break;
  case 2032:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgPortmon;
    break;
  case 2033:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSnmpHelpSnmpMibsupport;
    break;
  case 2034:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSnmpHelpSnmpTraprcvr;
    break;
  case 2035:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgNpcfg;
    break;
  case 2036:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgPortstat;
    break;
  case 2037:
    errMsg = pStrInfo_base_HtmlLinkBaseSysUsrMgrHelpUsrmgrLogincfg;
    break;
  case 2038:
    errMsg = pStrInfo_base_HtmlLinkBaseSysUsrMgrHelpUsrmgrUsrlogincfg;
    break;
  case 2041:
    errMsg = pStrInfo_base_HtmlLinkBaseSysUsrMgrHelpUsrmgrUsrloginsumm;
    break;
  case 2043:
    errMsg = pStrInfo_base_HtmlLinkBaseSysUsrMgrHelpUsrmgrLoginsessions;
    break;
  case 2044:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgNetndpsumm;
    break;
  case 2045:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgServndpsumm;
    break;
  case 2046:
    errMsg = pStrInfo_base_HtmlLinkBaseDnsClientHelpGblCfg;
    break;
  case 2047:
    errMsg = pStrInfo_base_HtmlLinkBaseDnsClientHelpSrvCfg;
    break;
  case 2048:
    errMsg = pStrInfo_base_HtmlLinkBaseDnsClientHelpHostIpCfg;
    break;
  case 2049:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpGlobalCfg;
    break;
  case 2050:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpPoolCfg;
    break;
  case 2051:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpResetCfg;
    break;
  case 2052:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpBindingsInfo;
    break;
  case 2053:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpSrvrInfo;
    break;
  case 2054:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpConflictsInfo;
    break;
  case 2055:
    errMsg = pStrInfo_base_HtmlLinkBaseDhcpHelpDhcpDhcpPoolOptions;
    break;
  case 2056:
    errMsg = pStrInfo_base_HtmlLinkBaseSntpHelpSntpSntpGlobalCfg;
    break;
  case 2057:
    errMsg = pStrInfo_base_HtmlLinkBaseSntpHelpSntpSntpGlobalStatus;
    break;
  case 2058:
    errMsg = pStrInfo_base_HtmlLinkBaseSntpHelpSntpSntpSrvrCfg;
    break;
  case 2059:
    errMsg = pStrInfo_base_HtmlLinkBaseSntpHelpSntpSntpSrvrStatus;
    break;
  case 2061:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgInmem;
    break;
  case 2062:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgMsgs;
    break;
  case 2063:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgPers;
    break;
  case 2064:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgMsgs1;
    break;
  case 2065:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgConsole;
    break;
  case 2066:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgSyslogCfg;
    break;
  case 2067:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgHosts;
    break;
  case 2068:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgTelnetcCfg;
    break;
  case 2070:
    errMsg = pStrInfo_base_HtmlFileBaseSysHelpDosCfg;
    break;
  case 2071:
    errMsg = pStrInfo_base_HtmlLinkBaseDnsClientHelpDlCfg;
    break;
  case 2072:
    errMsg = pStrInfo_base_HtmlLinkBaseDnsClientHelpHostIpSum;
    break;
  case 2073:
    errMsg = pStrInfo_base_HttpFileDownloadHelp;
    break;
  case 2074:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpsFlowAgent;
    break;
  case 2075:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpsFlowRcvr;
    break;
  case 2076:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpsFlowPoller;
    break;
  case 2077:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpsFlowSampler;
    break; 
  case 2092:
    errMsg = pStrInfo_base_HtmlLinkBaseSysLogHelpSyslogCfgCmdLogger;
    break;
  case 2094:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgAddSrcPort;
    break;
  case 2095:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpTransferImagestatus;
    break;
  case 2096:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpTransferImagecfg;
    break;
  case 2097:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgPortdes;
    break;
  case 2098:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpPortcfgRmvSrcPort;
    break;
  case 2099:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpSwitchCfgTraceroute;
    break;
  case 2100:
    errMsg = pStrInfo_base_HtmlLinkBaseSysHelpHttpConfiguration;
    break;
  case 2101:
    errMsg = pStrInfo_base_HtmlLinkBaseSysUsrMgrHelpPasswordManagement;
    break;
  case 2143:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpDvlanDvlanTag;
    break;
  case 2144:
    errMsg = pStrInfo_base_HtmlLinkBaseSysPortHelpDvlanDvlanTagSumm;
    break;
  case 2451:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSlotHelpSlotCardcfg;
    break;
  case 2452:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSlotHelpSlotSlotsumm;
    break;
  case 2458:
    errMsg = pStrInfo_base_HtmlLinkBaseSysSlotHelpSlotSuppcard;
    break;
  case 2459:
    errMsg = pStrInfo_base_HtmlLinkBasePoeCfg;
    break;
  case 2460:
    errMsg = pStrInfo_base_HtmlLinkBasePoeStatus;
    break;
  case 2088:
    errMsg = pStrInfo_base_HtmlLinkBaseSysIsdpCache;
    break;
  case 2089:
    errMsg = pStrInfo_base_HtmlLinkBaseSysIsdpIntf;
    break;
  case 2090:
      errMsg = pStrInfo_base_HtmlLinkBaseSysIsdpStat;
    break;
  case 2091:
    errMsg = pStrInfo_base_HtmlLinkBaseSysIsdpGlobalConf;
    break;
  case 2701:
    errMsg = pStrInfo_base_HtmlDhcpVendorClassIdOptions;
    break;
  case 2750:
    errMsg = pStrInfo_base_HttpAutoInstallHelp;
    break;
  case 2760:
    errMsg = pStrInfo_base_HttpErasehelp;
    break;
  case 2761:
    errMsg = pStrInfo_base_HtmlLinkXuiBaseSysHelpSysinfoEventlog;
    break;
  case 2762:
    errMsg = pStrInfo_base_HtmlLinkXuiBaseSysHelpSysinfoFDBSearch;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
