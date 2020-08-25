/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/ip_mcast/oem_ip_mcast.c
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
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebIpMcastGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebIpMcastGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1:
    errMsg = pStrInfo_common_Iso88591;
    break;
  case 1005:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1090:
    errMsg = pStrInfo_common_SubnetMask_3;
    break;
  case 1102:
    errMsg = pStrInfo_ipmcast_AdminMode;
    break;
  case 1401:
    errMsg = pStrInfo_ipmcast_MaxAddrEntries;
    break;
  case 1558:
    errMsg = pStrInfo_common_DesignatedRtr;
    break;
  case 1610:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1639:
    errMsg = pStrInfo_common_Intf;
    break;
  case 2111:
    errMsg = pStrInfo_common_Comp_1;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 4107:
    errMsg = pStrInfo_common_TotalNumOfRoutes;
    break;
  case 5052:
    errMsg = pStrInfo_common_IgmpProxyVer;
    break;
  case 8805:
    errMsg = pStrInfo_ipmcast_AdminMode;
    break;
  case 8837:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 8867:
    errMsg = pStrInfo_common_Grp_1;
    break;
  case 11103:
    errMsg = pStrInfo_common_Proto;
    break;
  case 11104:
    errMsg = pStrInfo_ipmcast_TblEntryCount;
    break;
  case 11105:
    errMsg = pStrInfo_ipmcast_TblHighestEntryCount;
    break;
  case 11106:
    errMsg = pStrInfo_ipmcast_TblMaxEntryCount;
    break;
  case 11109:
    errMsg = pStrInfo_ipmcast_AdminMode;
    break;
  case 11110:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 11111:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 11120:
    errMsg = pStrInfo_common_Src;
    break;
  case 11129:
    errMsg = pStrInfo_ipmcast_SrcIp;
    break;
  case 11130:
    errMsg = pStrInfo_ipmcast_GrpIp;
    break;
  case 11131:
    errMsg = pStrInfo_ipmcast_IncomingIntf;
    break;
  case 11132:
    errMsg = pStrInfo_ipmcast_OutgoingIntfs;
    break;
  case 11133:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 11134:
    errMsg = pStrInfo_common_UpTimeHhMmSs;
    break;
  case 11135:
    errMsg = pStrInfo_ipmcast_RpfNeighbor;
    break;
  case 11137:
    errMsg = pStrInfo_ipmcast_Flags;
    break;
  case 11145:
    errMsg = pStrInfo_ipmcast_SrcMask;
    break;
  case 11148:
    errMsg = pStrInfo_common_GrpMask_1;
    break;
  case 11155:
    errMsg = pStrInfo_ipmcast_SrcAddr_1;
    break;
  case 11156:
    errMsg = pStrInfo_ipmcast_SrcMask;
    break;
  case 11157:
    errMsg = pStrInfo_ipmcast_RpfAddr;
    break;
  case 11158:
    errMsg = pStrInfo_common_Proto;
    break;
  case 11165:
    errMsg = pStrErr_ipmcast_NumOfPktsForWhichSrcNotFound;
    break;
  case 11166:
    errMsg = pStrErr_ipmcast_NumOfPktsForWhichGrpNotFound;
    break;
  case 11183:
    errMsg = pStrInfo_ipmcast_TtlThresh;
    break;
  case 11187:
    errMsg = pStrInfo_common_Metric;
    break;
  case 11188:
    errMsg = pStrInfo_common_NoRtrIntfsAvailable;
    break;
  case 11302:
    errMsg = pStrInfo_common_HelloIntvlSecs;
    break;
  case 11303:
    errMsg = pStrInfo_common_NeighborCount;
    break;
  case 11304:
    errMsg = pStrInfo_ipmcast_IntfNeighbors;
    break;
  case 11311:
    errMsg = pStrInfo_common_AdminModeForIpv4;
    break;
  case 11312:
    errMsg = pStrInfo_ipmcast_AdminModeForIpv6;
    break;
  case 11313:
    errMsg = pStrInfo_ipmcast_IntfModeForIpv4;
    break;
  case 11314:
    errMsg = pStrInfo_ipmcast_IntfModeForIpv6;
    break;
  case 11315:
    errMsg = pStrInfo_ipmcast_IntfHelloIntvlForIpv4;
    break;
  case 11316:
    errMsg = pStrInfo_ipmcast_IntfHelloIntvlForIpv6;
    break;
  case 11317:
    errMsg = pStrInfo_ipmcast_DataThreshRateKbpsIpv4;
    break;
  case 11318:
    errMsg = pStrInfo_ipmcast_DataThreshRateKbpsIpv6;
    break;
  case 11319:
    errMsg = pStrInfo_ipmcast_RegisterThreshRateKbpsIpv4;
    break;
  case 11320:
    errMsg = pStrInfo_ipmcast_RegisterThreshRateKbpsIpv6;
    break;
  case 11321:
    errMsg = pStrInfo_ipmcast_SsmIpv4;
    break;
  case 11322:
    errMsg = pStrInfo_ipmcast_SsmIpv6;
    break;
  case 11323:
    errMsg = pStrInfo_ipmcast_SsmGrpAddrIpv4;
    break;
  case 11324:
    errMsg = pStrInfo_ipmcast_SsmGrpAddrIpv6;
    break;
  case 11325:
    errMsg = pStrInfo_ipmcast_SsmGrpMaskIpv4;
    break;
  case 11326:
    errMsg = pStrInfo_ipmcast_SsmGrpMaskIpv6;
    break;
  case 11328:
    errMsg = pStrInfo_ipmcast_HelloIntvlSecsIpv4;
    break;
  case 11329:
    errMsg = pStrInfo_ipmcast_HelloIntvlSecsIpv6;
    break;
  case 11330:
    errMsg = pStrInfo_ipmcast_JoinPruneIntvlSecsIpv4;
    break;
  case 11331:
    errMsg = pStrInfo_ipmcast_JoinPruneIntvlSecsIpv6;
    break;
  case 11332:
    errMsg = pStrInfo_ipmcast_BsrBorderIpv4;
    break;
  case 11333:
    errMsg = pStrInfo_ipmcast_BsrBorderIpv6;
    break;
  case 11334:
    errMsg = pStrInfo_ipmcast_DrPriIpv4;
    break;
  case 11335:
    errMsg = pStrInfo_ipmcast_DrPriIpv6;
    break;
  case 11402:
    errMsg = pStrInfo_ipmcast_IntfMetric;
    break;
  case 11405:
    errMsg = pStrInfo_ipmcast_GenerationId;
    break;
  case 11406:
    errMsg = pStrInfo_ipmcast_ReachableRoutes;
    break;
  case 11407:
    errMsg = pStrInfo_common_IntfParams;
    break;
  case 11408:
    errMsg = pStrInfo_ipmcast_LocalAddr;
    break;
  case 11409:
    errMsg = pStrInfo_ipmcast_RcvdBadPkts;
    break;
  case 11410:
    errMsg = pStrInfo_ipmcast_RcvdBadRoutes;
    break;
  case 11411:
    errMsg = pStrInfo_ipmcast_SentRoutes;
    break;
  case 11413:
    errMsg = pStrInfo_ipmcast_UpstreamNeighbor;
    break;
  case 11414:
    errMsg = pStrInfo_common_Intf;
    break;
  case 11415:
    errMsg = pStrInfo_common_Metric;
    break;
  case 11416:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 11417:
    errMsg = pStrInfo_common_UpTimeHhMmSs;
    break;
  case 11419:
    errMsg = pStrInfo_common_AlarmType;
    break;
  case 11422:
    errMsg = pStrInfo_ipmcast_NeighborParams;
    break;
  case 11423:
    errMsg = pStrInfo_ipmcast_NeighborIp;
    break;
  case 11424:
    errMsg = pStrInfo_common_UpTimeHhMmSs;
    break;
  case 11425:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 11426:
    errMsg = pStrInfo_ipmcast_GenerationId;
    break;
  case 11427:
    errMsg = pStrInfo_ipmcast_MajorVer;
    break;
  case 11428:
    errMsg = pStrInfo_ipmcast_MinorVer;
    break;
  case 11429:
    errMsg = pStrInfo_ipmcast_Capabilities;
    break;
  case 11430:
    errMsg = pStrInfo_ipmcast_RcvdRoutes;
    break;
  case 11431:
    errMsg = pStrInfo_common_State;
    break;
  case 11434:
    errMsg = pStrInfo_ipmcast_SrcIp;
    break;
  case 11435:
    errMsg = pStrInfo_ipmcast_SrcMask;
    break;
  case 11436:
    errMsg = pStrInfo_ipmcast_NextHopIntf;
    break;
  case 11437:
    errMsg = pStrInfo_ipmcast_GrpIp;
    break;
  case 11501:
    errMsg = pStrInfo_common_QueryIntvlSecs;
    break;
  case 11502:
    errMsg = pStrInfo_common_Querier;
    break;
  case 11503:
    errMsg = pStrInfo_common_QueryMaxRespTime110OfASecond;
    break;
  case 11504:
    errMsg = pStrInfo_common_QuerierUpTimeSecs;
    break;
  case 11505:
    errMsg = pStrInfo_common_QuerierExpiryTimeSecs;
    break;
  case 11506:
    errMsg = pStrInfo_common_WrongVerQueries;
    break;
  case 11507:
    errMsg = pStrInfo_common_NumOfJoins;
    break;
  case 11508:
    errMsg = pStrInfo_common_NumOfGrps;
    break;
  case 11509:
    errMsg = pStrInfo_common_Robustness;
    break;
  case 11510:
    errMsg = pStrInfo_common_LastMbrQueryIntvl110OfASecond;
    break;
  case 11511:
    errMsg = pStrInfo_common_IntfMode;
    break;
  case 11512:
    errMsg = pStrInfo_common_StartupQueryIntvlSecs;
    break;
  case 11513:
    errMsg = pStrInfo_common_StartupQueryCount;
    break;
  case 11515:
    errMsg = pStrInfo_ipmcast_IgmpCacheInfo;
    break;
  case 11516:
    errMsg = pStrInfo_common_LastReporter;
    break;
  case 11517:
    errMsg = pStrInfo_common_UpTimeSecs;
    break;
  case 11518:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 11519:
    errMsg = pStrInfo_ipmcast_Ver1HostTimerSecs;
    break;
  case 11539:
    errMsg = pStrInfo_common_LastMbrQueryCount;
    break;
  case 11540:
    errMsg = pStrInfo_common_QuerierStatus;
    break;
  case 11544:
    errMsg = pStrInfo_ipmcast_McastGrpIp;
    break;
  case 11562:
    errMsg = pStrInfo_common_IntfStats;
    break;
  case 11958:
    errMsg = pStrInfo_common_State;
    break;
  case 12016:
    errMsg = pStrInfo_ipmcast_ProtoState;
    break;
  case 12111:
    errMsg = pStrInfo_ipmcast_JoinPruneIntvlSecs_1;
    break;
  case 12114:
    errMsg = pStrInfo_common_HelloIntvlSecs;
    break;
  case 12116:
    errMsg = pStrInfo_ipmcast_CbsrPref;
    break;
  case 12134:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 12135:
    errMsg = pStrInfo_ipmcast_NetMask;
    break;
  case 12139:
    errMsg = pStrInfo_common_UpTimeHhMmSs;
    break;
  case 12140:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 12156:
    errMsg = pStrInfo_common_GrpAddr_1;
    break;
  case 12157:
    errMsg = pStrInfo_common_Addr_2;
    break;
  case 12159:
    errMsg = pStrInfo_ipmcast_CompIdx;
    break;
  case 12160:
    errMsg = pStrInfo_ipmcast_CompBsrAddr;
    break;
  case 12161:
    errMsg = pStrInfo_ipmcast_CompBsrExpiryTimeHhMmSs;
    break;
  case 12162:
    errMsg = pStrInfo_ipmcast_CompCrpHoldTimeHhMmSs;
    break;
  case 12164:
    errMsg = pStrInfo_ipmcast_HoldTimeHhMmSs;
    break;
  case 12165:
    errMsg = pStrInfo_common_ExpiryTimeHhMmSs;
    break;
  case 16234:
    errMsg = pStrInfo_ipmcast_Ver2HostTimerSecs;
    break;
  case 16235:
    errMsg = pStrInfo_ipmcast_Compatibility;
    break;
  case 16236:
    errMsg = pStrInfo_common_FilterMode;
    break;
  case 16237:
    errMsg = pStrInfo_ipmcast_SrcIp;
    break;
  case 16239:
    errMsg = pStrInfo_common_ExpiryTime;
    break;
  case 16240:
    errMsg = pStrInfo_ipmcast_GrpCompatibilityMode;
    break;
  case 16241:
    errMsg = pStrInfo_ipmcast_SrcFilterMode;
    break;
  case 16242:
    errMsg = pStrInfo_ipmcast_SrcHosts;
    break;
  case 22112:
    errMsg = pStrInfo_ipmcast_MgmdProxyReportIntvl;
    break;
  case 22113:
    errMsg = pStrInfo_ipmcast_NoIgmpCacheInfoAvailable;
    break;
  case 22114:
    errMsg = pStrInfo_ipmcast_Ver1QuerierTimeout;
    break;
  case 22115:
    errMsg = pStrInfo_ipmcast_Ver2QuerierTimeout;
    break;
  case 22116:
    errMsg = pStrInfo_ipmcast_QueriesRcvd;
    break;
  case 22117:
    errMsg = pStrInfo_common_ReportsRcvd;
    break;
  case 22118:
    errMsg = pStrInfo_common_ReportsSent;
    break;
  case 22119:
    errMsg = pStrInfo_common_LeavesRcvd;
    break;
  case 22120:
    errMsg = pStrInfo_common_LeavesSent;
    break;
  case 22125:
    errMsg = pStrInfo_ipmcast_MgmdProxyStartFrequency;
    break;
  case 22126:
    errMsg = pStrInfo_ipmcast_NoIgmpProxyIntfAvailable;
    break;
  case 22130:
    errMsg = pStrInfo_ipmcast_ProxyIntfStats;
    break;
  case 22131:
    errMsg = pStrInfo_ipmcast_NumOfSrcs;
    break;
  case 22132:
    errMsg = pStrInfo_ipmcast_IgmpProxyOprMode;
    break;
  case 22139:
    errMsg = pStrInfo_ipmcast_IgmpProxyNotOperational;
    break;
  case 22416:
    errMsg = pStrInfo_common_DrPri;
    break;
  case 22418:
    errMsg = pStrInfo_common_BsrBorder;
    break;
  case 25940:
    errMsg = pStrInfo_ipmcast_IntfParamsIpv6;
    break;
  case 25949:
    errMsg = pStrInfo_ipmcast_IntfParamsIpv4;
    break;

  case 11342:
    errMsg = pStrInfo_ipmcast_staticRPIPV4Info;
    break;

  case 11336:
    errMsg = pStrInfo_ipmcast_RPAddressIPV4Info;
    break;

  case 11337:
    errMsg = pStrInfo_ipmcast_RPAddressIPV6Info;
    break;

  case 11338:
    errMsg = pStrInfo_ipmcast_GroupAddressIPV4Info;
    break;

  case 11339:
    errMsg = pStrInfo_ipmcast_GroupAddressIPV6Info;
    break;

  case 11340:
    errMsg = pStrInfo_ipmcast_GroupAddressMaskIPV4Info;
    break;

  case 11344:
    errMsg = pStrInfo_ipmcast_OverrideInfo;
    break;

  case 11341:
    errMsg = pStrInfo_ipmcast_CountMessageInfo;
    break;

  case 22417:
    errMsg = pStrInfo_ipmcast_RPAddressInfo;
    break;

  case 2150:
    errMsg = pStrInfo_ipmcast_DeleteInfo;
    break;
    
  case 11343:
    errMsg = pStrInfo_ipmcast_staticRPIPV6Info;
    break;




   case 11347:
    errMsg = pStrInfo_ipmcast_HashMaskLengthIPv4;
    break;

   case 11348:
    errMsg = pStrInfo_ipmcast_HashMaskLengthIPv6;
    break;

   case 11349:
    errMsg = pStrInfo_ipmcast_PriorityIPv4;
    break;

   case 11350:
    errMsg = pStrInfo_ipmcast_PriorityIPv6;
    break;

   case 11351:
    errMsg = pStrInfo_ipmcast_ScopeIPv6;
    break;

   case 11352:
    errMsg = pStrInfo_ipmcast_PIMSMV4NotEnabled;
    break;

   case 11353:
    errMsg = pStrInfo_ipmcast_PIMSMV6NotEnabled;
    break;

  case 1443:
    errMsg = pStrInfo_ipmcast_PimSmBSRCandidateSummary;
    break;

  case 1444:
    errMsg = pStrInfo_ipmcast_PimSmBSRAddressIPv4;
    break;

  case 1445:
    errMsg = pStrInfo_ipmcast_PimSmBSRAddressIPv6;
    break;


  case 1446:
    errMsg = pStrInfo_ipmcast_PimSmBSRPriorityIPv4;
    break;

  case 1447:
    errMsg = pStrInfo_ipmcast_PimSmBSRPriorityIPv6;
    break;
    
    

  case 1448:
    errMsg = pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv4;
    break;

   case 1449:
    errMsg = pStrInfo_ipmcast_PimSmBSRCandidateCfg;
    break;

  case 1452:
    errMsg = pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv6;
    break;

  case 1450:
    errMsg = pStrInfo_ipmcast_PimSmBSRNextBootStrap;
    break;

    

  case 1451:
    errMsg = pStrInfo_ipmcast_PimSmBSRNextCandidateAPAdv;
    break;

  case 1453:
    errMsg = pStrInfo_ipmcast_BSRCandidateSummaryIPv4;
    break;

  case 1454:
    errMsg = pStrInfo_ipmcast_BSRCandidateSummaryIPv6;
    break;    

  case 1455:
    errMsg = pStrInfo_ipmcast_PimSmCandidateRpCfgIPv4;
    break;

  case 1456:
    errMsg = pStrInfo_ipmcast_PimSmCandidateRpCfgIPv6;
    break;

  case 1457:
    errMsg = pStrInfo_ipmcast_SsmConfigIpv4;
    break;

  case 1458:
    errMsg = pStrInfo_ipmcast_SsmConfigIpv6;
    break;    


  case 1459:
    errMsg = pStrInfo_ipmcast_SsmRangeFullIPv4;
    break; 

  case 1460:
    errMsg = pStrInfo_ipmcast_SsmRangeFullIPv6;
    break;     



  case 1461:
    errMsg = pStrInfo_ipmcast_SsmGroupAddPref;
    break;

   case 1462:
    errMsg = pStrInfo_ipmcast_SsmConfig;
    break;
    
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpMcastGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebIpMcastGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpMcastGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1400:
    errMsg = pStrInfo_ipmcast_DvmrpGlobalCfg;
    break;
  case 1401:
    errMsg = pStrInfo_ipmcast_DvmrpCfgSummary;
    break;
  case 1402:
    errMsg = pStrInfo_ipmcast_DvmrpIntfCfg;
    break;
  case 1403:
    errMsg = pStrInfo_ipmcast_DvmrpNextHopSummary;
    break;
  case 1404:
    errMsg = pStrInfo_ipmcast_DvmrpPruneSummary;
    break;
  case 1405:
    errMsg = pStrInfo_ipmcast_DvmrpRouteSummary;
    break;
  case 1406:
    errMsg = pStrInfo_ipmcast_IgmpCacheInfo;
    break;
  case 1407:
    errMsg = pStrInfo_ipmcast_IgmpGlobalCfg;
    break;
  case 1408:
    errMsg = pStrInfo_ipmcast_IgmpCfgSummary;
    break;
  case 1409:
    errMsg = pStrInfo_ipmcast_IgmpIntfCfg;
    break;
  case 1410:
    errMsg = pStrInfo_ipmcast_McastAdminBoundaryCfg;
    break;
  case 1411:
    errMsg = pStrInfo_ipmcast_McastAdminBoundarySummary;
    break;
  case 1412:
    errMsg = pStrInfo_ipmcast_McastGlobalCfg;
    break;
  case 1413:
    errMsg = pStrInfo_ipmcast_McastGlobalCfg;
    break;
  case 1414:
    errMsg = pStrInfo_ipmcast_McastIntfCfg;
    break;
  case 1415:
    errMsg = pStrInfo_ipmcast_McastIntfCfg;
    break;
  case 1416:
    errMsg = pStrInfo_ipmcast_McastMrouteSummary;
    break;
  case 1421:
    errMsg = pStrInfo_ipmcast_McastStaticRoutesCfg;
    break;
  case 1422:
    errMsg = pStrInfo_ipmcast_McastStaticRoutesSummary;
    break;
  case 1427:
    errMsg = pStrInfo_ipmcast_PimDmGlobalCfg;
    break;
  case 1428:
    errMsg = pStrInfo_ipmcast_PimDmIntfSummary;
    break;
  case 1429:
    errMsg = pStrInfo_ipmcast_PimDmIntfCfg;
    break;
  case 1430:
    errMsg = pStrInfo_ipmcast_PimSmCandidateRpSummary;
    break;
  case 1431:
    errMsg = pStrInfo_ipmcast_PimSmCompSummary;
    break;
  case 1432:
    errMsg = pStrInfo_ipmcast_PimSmGlobalStatus;
    break;
  case 1433:
    errMsg = pStrInfo_ipmcast_PimSmGlobalCfg;
    break;
  case 1434:
    errMsg = pStrInfo_ipmcast_PimSmIntfSummary;
    break;
  case 1435:
    errMsg = pStrInfo_ipmcast_PimSmIntfCfg;
    break;
  case 1436:
    errMsg = pStrInfo_ipmcast_PimSmRpSetSummary;
    break;
  case 1437:
    errMsg = pStrInfo_ipmcast_PimSmStaticRpCfg;
    break;
  case 1438:
    errMsg = pStrInfo_ipmcast_IgmpIntfDetailedMbrshipInfo_1;
    break;
  case 1439:
    errMsg = pStrInfo_ipmcast_IgmpProxyIntfCfg;
    break;
  case 1440:
    errMsg = pStrInfo_ipmcast_IgmpProxyCfgSummary;
    break;
  case 1441:
    errMsg = pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfoDetailed;
    break;
  case 1442:
    errMsg = pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfo;
    break;

  case 1443:
    errMsg = pStrInfo_ipmcast_PimSmBSRCandidateSummary;
    break;

   case 1449:
    errMsg = pStrInfo_ipmcast_PimSmBSRCandidateCfg;
    break;

   case 1450:
    errMsg = pStrInfo_ipmcast_PimSmCandidateRpCfg;
    break;

  

   case 1451:
    errMsg = pStrInfo_ipmcast_SsmConfig;
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

L7_char8 *usmWebIpMcastGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2400:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpglobalcfg;
    break;
  case 2401:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfparams;
    break;
  case 2402:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfcfg;
    break;
  case 2403:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpnexthopparams;
    break;
  case 2404:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrppruneparams;
    break;
  case 2405:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrprouteparams;
    break;
  case 2406:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpcacheinfo;
    break;
  case 2407:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpglobalcfg;
    break;
  case 2408:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfgparams;
    break;
  case 2409:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfg;
    break;
  case 2410:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopecfg;
    break;
  case 2411:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopeparams;
    break;
  case 2412:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalcfg;
    break;
  case 2413:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalparams;
    break;
  case 2414:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfcfg;
    break;
  case 2415:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfparams;
    break;
  case 2416:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastmroutecfg;
    break;
  case 2421:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticcfg;
    break;
  case 2422:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticparams;
    break;
  case 2427:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimglobalcfg;
    break;
  case 2428:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfparams;
    break;
  case 2429:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfcfg;
    break;
  case 2430:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcandidateparams;
    break;
  case 2431:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcomponentparams;
    break;
  case 2432:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmparams;
    break;
  case 2433:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcfg;
    break;
  case 2434:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifparams;
    break;
  case 2435:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifcfg;
    break;
  case 2436:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmrpsetparams;
    break;
  case 2437:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmstaticrp;
    break;
  case 2438:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpMbrshipinfo;
    break;
  case 2439:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfg;
    break;
  case 2440:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfgparams;
    break;
  case 2441:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyMbrshipinfo;
    break;
  case 2442:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyCacheinfo;
    break;

  case 2443:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmbsrcandidate;
    break;
  case 2706:
    errMsg = pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmSsmcfg;  
    break;

  case 2707:
     errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmbsrcandidate;
     break;

  case 2708:
     errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmrpsetparams;
     break;


  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
