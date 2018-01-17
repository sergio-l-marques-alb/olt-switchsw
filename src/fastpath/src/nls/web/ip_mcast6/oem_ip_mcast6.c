/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/ip_mcast6/oem_ip_mcast6.c
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
#include "strlib_ip_mcast6_common.h"
#include "strlib_ip_mcast6_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebIpMcast6GetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebIpMcast6GetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1412:
    errMsg = pStrInfo_common_SlotPortWithoutUnit_1;
    break;
  case 1413:
    errMsg = pStrInfo_common_SlotPortWithUnit_1;
    break;    
  case 1612:
    errMsg = pStrInfo_common_Routing;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 5052:
    errMsg = pStrInfo_common_IgmpProxyVer;
    break;
  case 8854:
    errMsg = pStrInfo_common_UpTime;
    break;
  case 11188:
    errMsg = pStrInfo_common_NoRtrIntfsAvailable;
    break;
  case 11407:
    errMsg = pStrInfo_common_IntfParams;
    break;
  case 11501:
    errMsg = pStrInfo_common_QueryIntvlSecs;
    break;
  case 11502:
    errMsg = pStrInfo_common_Querier;
    break;
  case 11503:
    errMsg = pStrInfo_common_MldQueryMaxRespTimeMilliSecond;
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
    errMsg = pStrInfo_common_MldLastMbrQueryIntvlMilliSecond;
    break;
  case 11512:
    errMsg = pStrInfo_common_StartupQueryIntvlSecs;
    break;
  case 11513:
    errMsg = pStrInfo_common_StartupQueryCount;
    break;
  case 11516:
    errMsg = pStrInfo_common_LastReporter;
    break;
  case 11539:
    errMsg = pStrInfo_common_LastMbrQueryCount;
    break;
  case 11540:
    errMsg = pStrInfo_common_QuerierStatus;
    break;
  case 11562:
    errMsg = pStrInfo_common_IntfStats;
    break;
  case 12156:
    errMsg = pStrInfo_common_GrpAddr_1;
    break;
  case 16236:
    errMsg = pStrInfo_common_FilterMode;
    break;
  case 16239:
    errMsg = pStrInfo_common_ExpiryTime;
    break;
  case 22400:
    errMsg = pStrInfo_ipmcast6_MldAdminMode;
    break;
  case 22401:
    errMsg = pStrInfo_ipmcast6_MldOperationalMode;
    break;
  case 22402:
    errMsg = pStrInfo_common_MldVer;
    break;
  case 22404:
    errMsg = pStrInfo_ipmcast6_ValidMldPktsRcvd;
    break;
  case 22405:
    errMsg = pStrInfo_ipmcast6_ValidMldPktsSent;
    break;
  case 22406:
    errMsg = pStrInfo_ipmcast6_QuerierRcvd;
    break;
  case 22407:
    errMsg = pStrInfo_ipmcast6_QuerierSent;
    break;
  case 22408:
    errMsg = pStrInfo_common_ReportsRcvd;
    break;
  case 22409:
    errMsg = pStrInfo_common_ReportsSent;
    break;
  case 22410:
    errMsg = pStrInfo_common_LeavesRcvd;
    break;
  case 22411:
    errMsg = pStrInfo_common_LeavesSent;
    break;
  case 22412:
    errMsg = pStrInfo_ipmcast6_BadChecksumMldPkts;
    break;
  case 22413:
    errMsg = pStrInfo_ipmcast6_MalformedMldPkts;
    break;
  case 22414:
    errMsg = pStrInfo_ipmcast6_Ver1HostTimer;
    break;
  case 22415:
    errMsg = pStrInfo_ipmcast6_GrpCompatMode;
    break;
  case 22416:
    errMsg = pStrInfo_ipmcast6_HtmlSourceAddress;
    break;
  case 25941:
    errMsg = pStrInfo_ipmcast6_Ipv6RtrMld;
    break;
  case 25943:
    errMsg = pStrInfo_ipmcast6_QueryMaxRespTime;
    break;
  case 25944:
    errMsg = pStrInfo_ipmcast6_LastMbrQueryIntvl;
    break;
  case 25945:
    errMsg = pStrInfo_common_LastMbrQueryCount;
    break;
  case 25947:
    errMsg = pStrInfo_ipmcast6_NoMldGrpsAvailable;
    break;
  case 25948:
    errMsg = pStrInfo_ipmcast6_AdminMode;
    break;    
  case 25949:
    errMsg = pStrInfo_ipmcast6_NoMldCacheInfoAvailable;
    break;
  case 25950:
    errMsg = pStrInfo_ipmcast6_MldProxyNotOperational;
    break;
  case 25951:
    errMsg = pStrInfo_ipmcast6_NoMldProxyIntfAvailable;
    break;
  case 25952:
    errMsg = pStrInfo_common_MldQueryMaxRespTimeMilliSecond;
    break;
  case 25953:
    errMsg = pStrInfo_common_MldLastMbrQueryIntvlMilliSecond;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpMcast6GetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebIpMcast6GetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpMcast6GetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1443:
    errMsg = pStrInfo_ipmcast6_MldGlobalCfg;
    break;
  case 1444:
    errMsg = pStrInfo_ipmcast6_MldIntfCfg;
    break;
  case 1445:
    errMsg = pStrInfo_ipmcast6_MldGrpsSummary;
    break;
  case 1446:
    errMsg = pStrInfo_ipmcast6_MldIntfSummary;
    break;
  case 1447:
    errMsg = pStrInfo_ipmcast6_MldTraffic;
    break;
  case 1448:
    errMsg = pStrInfo_ipmcast6_MldProxyIntfCfg;
    break;
  case 1449:
    errMsg = pStrInfo_ipmcast6_MldProxyCfgSummary;
    break;
  case 1450:
    errMsg = pStrInfo_ipmcast6_MldProxyIntfMbrshipInfoDetailed;
    break;
  case 1451:
    errMsg = pStrInfo_ipmcast6_MldProxyIntfMbrshipInfo;
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

L7_char8 *usmWebIpMcast6GetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2443:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldGlobalcfg;
    break;
  case 2444:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldGrpsummary;
    break;
  case 2445:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldIntfcfg;
    break;
  case 2446:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldIntfsummary;
    break;
  case 2447:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldTraffic;
    break;
  case 2448:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyCacheinfo;
    break;
  case 2449:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyIgmpproxyintfcfg;
    break;
  case 2450:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyIgmpproxyintfcfgparams;
    break;
  case 2451:
    errMsg = pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyMbrshipinfo;
    break;

  
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
