/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/routingv6/oem_routingv6.c
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
#include "strlib_routingv6_common.h"
#include "strlib_routingv6_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebRoutingv6GetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebRoutingv6GetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1383:
    errMsg = pStrInfo_common_NwSummary;
    break;
  case 1387:
    errMsg = pStrInfo_common_NssaExternal;
    break;
  case 1467:
    errMsg = pStrInfo_common_MetricCost;
    break;
  case 1474:
    errMsg = pStrInfo_common_WsNwLocal;
    break;
  case 1535:
    errMsg = pStrInfo_common_NextHopIpAddr;
    break;
  case 1537:
    errMsg = pStrInfo_common_AreaId;
    break;
  case 1539:
    errMsg = pStrInfo_common_ApAge;
    break;
  case 1540:
    errMsg = pStrInfo_common_Checksum;
    break;
  case 1541:
    errMsg = pStrInfo_common_Advertisement;
    break;
  case 1544:
    errMsg = pStrInfo_common_ExternalRouting;
    break;
  case 1545:
    errMsg = pStrInfo_common_SpfRuns;
    break;
  case 1546:
    errMsg = pStrInfo_common_AreaBorderRtrCount;
    break;
  case 1547:
    errMsg = pStrInfo_common_AreaLsaChecksum;
    break;
  case 1548:
    errMsg = pStrInfo_common_Proto;
    break;
  case 1552:
    errMsg = pStrInfo_common_IntfType_1;
    break;
  case 1553:
    errMsg = pStrInfo_common_ReTxIntvlSecs;
    break;
  case 1554:
    errMsg = pStrInfo_common_HelloIntvlSecs;
    break;
  case 1555:
    errMsg = pStrInfo_common_DeadIntvlSecs;
    break;
  case 1557:
    errMsg = pStrInfo_common_LsaAckIntvlSecs;
    break;
  case 1558:
    errMsg = pStrInfo_common_DesignatedRtr;
    break;
  case 1559:
    errMsg = pStrInfo_common_BackupDesignatedRtr;
    break;
  case 1560:
    errMsg = pStrInfo_common_AuthType_1;
    break;
  case 1582:
    errMsg = pStrInfo_common_IftransitDelayIntvlSecs;
    break;
  case 1589:
    errMsg = pStrInfo_common_NumOfLinkEvents;
    break;
  case 1599:
    errMsg = pStrInfo_common_StubAreaInfo;
    break;
  case 1610:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1619:
    errMsg = pStrInfo_common_RtrId_1;
    break;
  case 1620:
    errMsg = pStrInfo_common_AbrStatus;
    break;
  case 1621:
    errMsg = pStrInfo_common_AsbrMode;
    break;
  case 1624:
    errMsg = pStrInfo_common_ExitOverflowIntvlSecs;
    break;
  case 1625:
    errMsg = pStrInfo_common_ExternalLsaCount;
    break;
  case 1626:
    errMsg = pStrInfo_common_ExternalLsaChecksum;
    break;
  case 1627:
    errMsg = pStrInfo_common_NewLsasOriginated;
    break;
  case 1628:
    errMsg = pStrInfo_common_LsasRcvd;
    break;
  case 1633:
    errMsg = pStrInfo_common_AreaId;
    break;
  case 1634:
    errMsg = pStrInfo_common_LsaType;
    break;
  case 1635:
    errMsg = pStrInfo_common_LsId;
    break;
  case 1636:
    errMsg = pStrInfo_common_Seq_1;
    break;
  case 1637:
    errMsg = pStrInfo_common_Options;
    break;
  case 1639:
    errMsg = pStrInfo_common_Intf;
    break;
  case 1640:
    errMsg = pStrInfo_common_RtrPri;
    break;
  case 1641:
    errMsg = pStrInfo_common_State;
    break;
  case 1642:
    errMsg = pStrInfo_common_Events;
    break;
  case 1645:
    errMsg = pStrInfo_common_RetransmissionQueueLen;
    break;
  case 1730:
    errMsg = pStrInfo_common_AsbrMode;
    break;
  case 1755:
    errMsg = pStrInfo_common_LsdbType;
    break;
  case 1761:
    errMsg = pStrInfo_common_ImportSummaryLsas;
    break;
  case 1879:
    errMsg = pStrInfo_common_NoValidAreasAvailable;
    break;
  case 1881:
    errMsg = pStrInfo_common_NoValidOspfIntfsAvailable;
    break;
  case 1894:
    errMsg = pStrInfo_common_Pass;
    break;
  case 1895:
    errMsg = pStrInfo_common_OspfDeadTime;
    break;
  case 1896:
    errMsg = pStrInfo_common_OspfUpTime;
    break;
  case 1897:
    errMsg = pStrInfo_common_OspfTxPkts;
    break;
  case 1898:
    errMsg = pStrInfo_common_OspfRxPkts;
    break;
  case 1899:
    errMsg = pStrInfo_common_OspfRxDiscard;
    break;
  case 1900:
    errMsg = pStrInfo_common_OspfRxBadVer;
    break;
  case 1901:
    errMsg = pStrErr_routing_OspfRxBadNw;
    break;
  case 1902:
    errMsg = pStrErr_common_OspfRxBadVirtLink;
    break;
  case 1903:
    errMsg = pStrInfo_common_OspfRxBadArea;
    break;
  case 1904:
    errMsg = pStrErr_common_OspfRxBadDestAddr;
    break;
  case 1905:
    errMsg = pStrInfo_common_OspfRxBadAuthType;
    break;
  case 1906:
    errMsg = pStrErr_common_AuthFailure;
    break;
  case 1907:
    errMsg = pStrInfo_common_OspfRxBadNbr;
    break;
  case 1908:
    errMsg = pStrErr_common_OspfRxBadPktType;
    break;
  case 1909:
    errMsg = pStrInfo_common_OspfHellosIgnored;
    break;
  case 1910:
    errMsg = pStrInfo_common_OspfHellosSent;
    break;
  case 1911:
    errMsg = pStrInfo_common_OspfHellosRecvd;
    break;
  case 1912:
    errMsg = pStrInfo_common_OspfDDSent;
    break;
  case 1913:
    errMsg = pStrInfo_common_OspfDDRcvd;
    break;
  case 1914:
    errMsg = pStrInfo_common_OspfLSReqSent;
    break;
  case 1915:
    errMsg = pStrInfo_common_OspfLSReqRcvd;
    break;
  case 1916:
    errMsg = pStrInfo_common_OspfLSUpdateSent;
    break;
  case 1917:
    errMsg = pStrInfo_common_OspfLSUpdateRcvd;
    break;
  case 1918:
    errMsg = pStrInfo_common_OspfLSAckSent;
    break;
  case 1919:
    errMsg = pStrInfo_common_OspfLSAckRcvd;
    break;

  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 4111:
    errMsg = pStrInfo_common_AreaBorderRtrCount;
    break;
  case 4112:
    errMsg = pStrInfo_common_AsBorderRtrCount;
    break;
  case 4113:
    errMsg = pStrInfo_common_AreaLsaCount;
    break;
  case 4114:
    errMsg = pStrInfo_common_IntfEvents;
    break;
  case 4115:
    errMsg = pStrInfo_common_VirtualEvents;
    break;
  case 4116:
    errMsg = pStrInfo_common_NeighborEvents;
    break;
  case 4119:
    errMsg = pStrInfo_common_Metric;
    break;
  case 4120:
    errMsg = pStrInfo_common_ComparableCost;
    break;
  case 4121:
    errMsg = pStrInfo_common_NonComparableCost;
    break;
  case 4135:
    errMsg = pStrInfo_common_AuthKey;
    break;
  case 4136:
    errMsg = pStrInfo_common_DeflMetric;
    break;
  case 4165:
    errMsg = pStrInfo_common_NeighborIpAddr_1;
    break;
  case 4187:
    errMsg = pStrInfo_common_MetricVal;
    break;
  case 4188:
    errMsg = pStrInfo_common_AuthKeyId;
    break;
  case 4195:
    errMsg = pStrInfo_common_Pref;
    break;
  case 5024:
    errMsg = pStrInfo_common_Simple_1;
    break;
  case 5030:
    errMsg = pStrInfo_common_Encrypt_1;
    break;
  case 5200:
    errMsg = pStrInfo_common_None_1;
    break;
  case 6016:
    errMsg = pStrInfo_common_Defl;
    break;
  case 6017:
    errMsg = pStrInfo_common_PimSmGrpRpMapStatic;
    break;
  case 7407:
    errMsg = pStrInfo_common_NeighborState;
    break;
  case 7603:
    errMsg = pStrInfo_common_NoRedistributeMode;
    break;
  case 7604:
    errMsg = pStrInfo_common_DeflInfoOriginate;
    break;
  case 7605:
    errMsg = pStrInfo_common_DeflMetricType;
    break;
  case 7606:
    errMsg = pStrInfo_common_TranslatorRole;
    break;
  case 7607:
    errMsg = pStrInfo_common_TranslatorStabilityIntvl;
    break;
  case 7608:
    errMsg = pStrInfo_common_TranslatorState;
    break;
  case 7609:
    errMsg = pStrInfo_common_Always;
    break;
  case 7610:
    errMsg = pStrInfo_common_Candidate;
    break;
  case 8799:
    errMsg = pStrInfo_common_VirtualLink;
    break;
  case 8808:
    errMsg = pStrInfo_common_Pri_1;
    break;
  case 8847:
    errMsg = pStrInfo_common_True2;
    break;
  case 8848:
    errMsg = pStrInfo_common_False2;
    break;
  case 8868:
    errMsg = pStrInfo_routingv6_NssaSpecificInfo;
    break;
  case 8869:
    errMsg = pStrInfo_routingv6_DeadTimerDueInSecs;
    break;
  case 8870:
    errMsg = pStrInfo_routingv6_IntifId;
    break;
  case 8871:
    errMsg = pStrInfo_routingv6_DeadTime;
    break;
  case 11110:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 11111:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 11188:
    errMsg = pStrInfo_common_NoRtrIntfsAvailable;
    break;
  case 11414:
    errMsg = pStrInfo_common_Intf;
    break;
  case 16026:
    errMsg = pStrInfo_common_NoNeighborsCfgured;
    break;
  case 16173:
    errMsg = pStrInfo_common_Metric;
    break;
  case 16174:
    errMsg = pStrInfo_common_MetricType;
    break;
  case 16188:
    errMsg = pStrInfo_common_OspfMetricExtType1;
    break;
  case 16189:
    errMsg = pStrInfo_common_OspfMetricExtType2;
    break;
  case 16190:
    errMsg = pStrInfo_common_Tag;
    break;
  case 16192:
    errMsg = pStrInfo_common_DeflInfoOriginate;
    break;
  case 16193:
    errMsg = pStrInfo_common_DeflMetric;
    break;
  case 16194:
    errMsg = pStrInfo_common_Always;
    break;
  case 16195:
    errMsg = pStrInfo_common_AvailableSrc;
    break;
  case 16196:
    errMsg = pStrInfo_common_DeflRouteAdvertise;
    break;
  case 16197:
    errMsg = pStrInfo_common_Src;
    break;
  case 16229:
    errMsg = pStrInfo_common_CfguredSrc;
    break;
  case 22046:
    errMsg = pStrInfo_common_Ipv6Prefix_1;
    break;
  case 22047:
    errMsg = pStrInfo_routingv6_Ipv6Ospfv3AreaId;
    break;
  case 22050:
    errMsg = pStrInfo_routingv6_Ospfv3AdminMode;
    break;
  case 22051:
    errMsg = pStrInfo_common_ExternalLinkStateDbaseLimit;
    break;
  case 22201:
    errMsg = pStrInfo_routingv6_RelayOption;
    break;
  case 22202:
    errMsg = pStrInfo_routingv6_RemoteIdSubOption;
    break;
  case 22207:
    errMsg = pStrInfo_common_Dhcpv6AdminMode;
    break;
  case 22208:
    errMsg = pStrInfo_common_IntfMode;
    break;
  case 22209:
    errMsg = pStrInfo_routingv6_Ipv6DhcpPoolName;
    break;
  case 22210:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRapidCommit;
    break;
  case 22211:
    errMsg = pStrInfo_common_Pref;
    break;
  case 22212:
    errMsg = pStrInfo_routingv6_NoPoolExists;
    break;
  case 22213:
    errMsg = pStrInfo_common_DstIpAddr;
    break;
  case 22214:
    errMsg = pStrInfo_routingv6_RelayIntf_1;
    break;
  case 22215:
    errMsg = pStrInfo_routingv6_RemoteId_1;
    break;
  case 22216:
    errMsg = pStrInfo_routingv6_Relay;
    break;
  case 22217:
    errMsg = pStrInfo_common_Srvr;
    break;
  case 22222:
    errMsg = pStrInfo_common_Ipv6Addr2;
    break;
  case 22223:
    errMsg = pStrInfo_routingv6_Duid;
    break;
  case 22224:
    errMsg = pStrInfo_common_RoutingIpv6RadvPrefix;
    break;
  case 22225:
    errMsg = pStrInfo_common_ExpiryTime;
    break;
  case 22226:
    errMsg = pStrInfo_routingv6_RoutingIpv6RadvValidLife;
    break;
  case 22227:
    errMsg = pStrInfo_routingv6_PreferLifetime;
    break;
  case 22228:
    errMsg = pStrInfo_routingv6_DnsSrvrAddr;
    break;
  case 22229:
    errMsg = pStrInfo_common_DoMainName;
    break;
  case 22230:
    errMsg = pStrInfo_routingv6_DelegatedPrefix;
    break;
  case 22231:
    errMsg = pStrInfo_routingv6_MsgsRcvd_1;
    break;
  case 22232:
    errMsg = pStrInfo_routingv6_Ipv6DhcpSolicitRcvdStat;
    break;
  case 22233:
    errMsg = pStrInfo_routingv6_Ipv6DhcpReqRcvdStat;
    break;
  case 22234:
    errMsg = pStrInfo_routingv6_Ipv6DhcpConfirmRcvdStat;
    break;
  case 22235:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRenewRcvdStat;
    break;
  case 22236:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRebindRcvdStat;
    break;
  case 22237:
    errMsg = pStrInfo_routingv6_Ipv6DhcpReleasedRcvdStat;
    break;
  case 22238:
    errMsg = pStrInfo_routingv6_Ipv6DhcpDeclineRcvdStat;
    break;
  case 22239:
    errMsg = pStrInfo_routingv6_Ipv6DhcpInformRcvdStat;
    break;
  case 22240:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRelayIntfRcvdStat;
    break;
  case 22241:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRelayReplyRcvdStat;
    break;
  case 22242:
    errMsg = pStrInfo_routingv6_Ipv6DhcpMalformrecievedSentStat;
    break;
  case 22243:
    errMsg = pStrInfo_routingv6_Ipv6DhcpDiscardedPktsStat;
    break;
  case 22244:
    errMsg = pStrInfo_routingv6_MsgsSent_1;
    break;
  case 22245:
    errMsg = pStrInfo_routingv6_Ipv6DhcpAdvtRcvdStat;
    break;
  case 22246:
    errMsg = pStrInfo_routingv6_Ipv6DhcpReplySentStat;
    break;
  case 22247:
    errMsg = pStrInfo_routingv6_Ipv6DhcpReconfigSentStat;
    break;
  case 22248:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRelayFwdSentStat;
    break;
  case 22249:
    errMsg = pStrInfo_routingv6_Ipv6DhcpRelayReplySentStat;
    break;
  case 22257:
    errMsg = pStrInfo_routingv6_TotalDhcpv6PktsSent;
    break;
  case 22258:
    errMsg = pStrInfo_routingv6_Ipv6DhcpTotalRcvd;
    break;
  case 22269:
    errMsg = pStrInfo_common_HostIpv6Addr2;
    break;
  case 22310:
    errMsg = pStrInfo_routingv6_AdvRtr;
    break;
  case 22311:
    errMsg = pStrInfo_routingv6_LinkId;
    break;
  case 22312:
    errMsg = pStrInfo_routingv6_RtrOpt;
    break;
  case 23110:
    errMsg = pStrInfo_routingv6_Ipv6Fwd;
    break;
  case 23111:
    errMsg = pStrInfo_routingv6_RoutingIpv6DestUnreachables;
    break;
  case 23112:
    errMsg = pStrInfo_routingv6_RoutingIpv6RateLimitInterval;
    break;
  case 23113:
    errMsg = pStrInfo_routingv6_RoutingIpv6RateLimitBurstSize;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebRoutingv6GetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1514:
    errMsg = pStrInfo_routingv6_Dhcpv6GlobalCfg;
    break;
  case 1552:
    errMsg = pStrInfo_routingv6_Ipv6UcastRouting;
    break;
  case 1556:
    errMsg = pStrInfo_routingv6_Ospfv3Intra;
    break;
  case 1557:
    errMsg = pStrInfo_routingv6_Ospfv3Inter;
    break;
  case 1558:
    errMsg = pStrInfo_routingv6_Ospfv3External;
    break;
  case 1561:
    errMsg = pStrInfo_routingv6_Ipv6PrefixPrefixLen_1;
    break;
  case 1562:
    errMsg = pStrInfo_routingv6_AllRoutes;
    break;
  case 1563:
    errMsg = pStrInfo_routingv6_BestRoutesOnly;
    break;
  case 1564:
    errMsg = pStrInfo_routingv6_RoutesDisped;
    break;
  case 1571:
    errMsg = pStrInfo_routingv6_Ipv6StatRcvd;
    break;
  case 1572:
    errMsg = pStrInfo_routingv6_Ipv6StatDelivers;
    break;
  case 1573:
    errMsg = pStrErr_routingv6_Ipv6StatHdrErrs;
    break;
  case 1574:
    errMsg = pStrErr_routingv6_Ipv6StatBigErrs;
    break;
  case 1575:
    errMsg = pStrInfo_routingv6_Ipv6StatNoRoutes;
    break;
  case 1576:
    errMsg = pStrInfo_routingv6_Ipv6StatProtos;
    break;
  case 1577:
    errMsg = pStrErr_routingv6_Ipv6StatAddrError;
    break;
  case 1578:
    errMsg = pStrInfo_routingv6_RcvdDatagramsDiscardedDutToTruncatedData;
    break;
  case 1579:
    errMsg = pStrInfo_routingv6_Ipv6StatDiscarded;
    break;
  case 1580:
    errMsg = pStrInfo_routingv6_Ipv6StatReassembled;
    break;
  case 1581:
    errMsg = pStrInfo_routingv6_Ipv6StatReassembledSuccess;
    break;
  case 1582:
    errMsg = pStrErr_routingv6_Ipv6ReassembledFails;
    break;
  case 1583:
    errMsg = pStrInfo_routingv6_Ipv6SentForwarded;
    break;
  case 1584:
    errMsg = pStrInfo_routingv6_DatagramsLocallyTrasmitted;
    break;
  case 1585:
    errMsg = pStrInfo_routingv6_Ipv6SentDiscarded;
    break;
  case 1586:
    errMsg = pStrInfo_routingv6_Ipv6StatFragmentedOk;
    break;
  case 1587:
    errMsg = pStrErr_routingv6_Ipv6StatFragmentedFailed;
    break;
  case 1588:
    errMsg = pStrInfo_routingv6_Ipv6McastRcvd;
    break;
  case 1589:
    errMsg = pStrInfo_routingv6_Ipv6McastSent;
    break;
  case 1590:
    errMsg = pStrInfo_routingv6_IcmpStatsRcvd;
    break;
  case 1591:
    errMsg = pStrErr_routingv6_IcmpStatsErrs;
    break;
  case 1592:
    errMsg = pStrInfo_routingv6_IcmpStatsUnreachAble;
    break;
  case 1593:
    errMsg = pStrInfo_routingv6_IcmpStatsUnreachAdmin;
    break;
  case 1594:
    errMsg = pStrInfo_routingv6_IcmpStatsTimeExceeds;
    break;
  case 1595:
    errMsg = pStrInfo_routingv6_IcmpStatsParmProblem;
    break;
  case 1596:
    errMsg = pStrInfo_routingv6_IcmpPktTooBig;
    break;
  case 1597:
    errMsg = pStrInfo_routingv6_IcmpStatsEchos;
    break;
  case 1598:
    errMsg = pStrInfo_routingv6_IcmpStatsEchosReply;
    break;
  case 1599:
    errMsg = pStrInfo_routingv6_IcmpStatsRtrSolicit;
    break;
  case 1600:
    errMsg = pStrInfo_routingv6_DatagramsFragmentsCreated;
    break;
  case 1601:
    errMsg = pStrInfo_routingv6_IcmpStatsRtrAdvert;
    break;
  case 1602:
    errMsg = pStrInfo_routingv6_IcmpStatsNeighSolicit;
    break;
  case 1603:
    errMsg = pStrInfo_routingv6_IcmpStatsNeighAdvert;
    break;
  case 1604:
    errMsg = pStrInfo_routingv6_IcmpStatsInRedirect;
    break;
  case 1605:
    errMsg = pStrInfo_routingv6_IcmpStatsGrpMemQuery;
    break;
  case 1606:
    errMsg = pStrInfo_routingv6_IcmpStatsGrpMemResp;
    break;
  case 1607:
    errMsg = pStrInfo_routingv6_IcmpStatsGrpMemReduct;
    break;
  case 1608:
    errMsg = pStrInfo_routingv6_IcmpStatsSentMsgs;
    break;
  case 1609:
    errMsg = pStrErr_routingv6_IcmpStatsSentErrs;
    break;
  case 1610:
    errMsg = pStrInfo_routingv6_IcmpStatsSentUnreachAble;
    break;
  case 1611:
    errMsg = pStrInfo_routingv6_IcmpStatsSentAdmin;
    break;
  case 1612:
    errMsg = pStrInfo_routingv6_IcmpStatsSentTime;
    break;
  case 1613:
    errMsg = pStrInfo_routingv6_IcmpSentParmProblem;
    break;
  case 1614:
    errMsg = pStrInfo_routingv6_IcmpSentPktTooBig;
    break;
  case 1615:
    errMsg = pStrInfo_routingv6_IcmpSentEchos;
    break;
  case 1616:
    errMsg = pStrInfo_routingv6_IcmpSentEchosReply;
    break;
  case 1617:
    errMsg = pStrInfo_routingv6_IcmpSentRtrSolicit;
    break;
  case 1618:
    errMsg = pStrInfo_routingv6_IcmpSentRtrAdvert;
    break;
  case 1619:
    errMsg = pStrInfo_routingv6_IcmpSentNeighSolicit;
    break;
  case 1620:
    errMsg = pStrInfo_routingv6_IcmpSentNeighAdvert;
    break;
  case 1621:
    errMsg = pStrInfo_routingv6_IcmpStatsOutRedirect;
    break;
  case 1622:
    errMsg = pStrInfo_routingv6_IcmpSentGrpMemQuery;
    break;
  case 1623:
    errMsg = pStrInfo_routingv6_IcmpSentGrpMemResp;
    break;
  case 1624:
    errMsg = pStrInfo_routingv6_IcmpSentGrpMemReduct;
    break;
  case 1625:
    errMsg = pStrInfo_routingv6_IcmpStatsDupAddr;
    break;
  case 1628:
    errMsg = pStrInfo_routingv6_Ipv6HopLimit_2;
    break;

  default:
    local_call=L7_TRUE;
    errMsg = usmWebRoutingv6GetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebRoutingv6GetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1500:
    errMsg = pStrInfo_routingv6_Ospfv3Cfg;
    break;
  case 1501:
    errMsg = pStrInfo_routingv6_Ospfv3RouteRedistributionSummary;
    break;
  case 1502:
    errMsg = pStrInfo_routingv6_Ospfv3RouteRedistributionCfg;
    break;
  case 1503:
    errMsg = pStrInfo_routingv6_Ospfv3Neighbors;
    break;
  case 1504:
    errMsg = pStrInfo_routingv6_Ospfv3NeighborTbl;
    break;
  case 1505:
    errMsg = pStrInfo_routingv6_Ospfv3VirtualLinkSummary;
    break;
  case 1506:
    errMsg = pStrInfo_routingv6_Ospfv3VirtualLinkCfg;
    break;
  case 1507:
    errMsg = pStrInfo_routingv6_Ospfv3VirtualLinkAuthCfg;
    break;
  case 1508:
    errMsg = pStrInfo_routingv6_Ospfv3IntfStats;
    break;
  case 1509:
    errMsg = pStrInfo_routingv6_Ospfv3IntfCfg;
    break;
  case 1510:
    errMsg = pStrInfo_routingv6_Ospfv3AreaCfg;
    break;
  case 1511:
    errMsg = pStrInfo_routingv6_Ospfv3AreaRangeCfg;
    break;
  case 1512:
    errMsg = pStrInfo_routingv6_Ospfv3StubAreaSummary;
    break;
  case 1513:
    errMsg = pStrInfo_routingv6_Ospfv3LinkStateDbase;
    break;
  case 1514:
    errMsg = pStrInfo_routingv6_Dhcpv6GlobalCfg;
    break;
  case 1515:
    errMsg = pStrInfo_routingv6_Dhcpv6Stats;
    break;
  case 1516:
    errMsg = pStrInfo_routingv6_Dhcpv6PoolCfg;
    break;
  case 1517:
    errMsg = pStrInfo_routingv6_Dhcpv6SrvrBindingsSummary;
    break;
  case 1518:
    errMsg = pStrInfo_routingv6_Dhcpv6IntfCfg;
    break;
  case 1519:
    errMsg = pStrInfo_routingv6_Dhcpv6PoolSummary;
    break;
  case 1520:
    errMsg = pStrInfo_routingv6_Ipv6IntfSummary;
    break;
  case 1521:
    errMsg = pStrInfo_routingv6_Ipv6IntfCfg;
    break;
  case 1523:
    errMsg = pStrInfo_routingv6_PrefixDelegationCfg;
    break;
  case 1551:
    errMsg = pStrInfo_routingv6_Ipv6Cfg;
    break;
  case 1555:
    errMsg = pStrInfo_routingv6_Ipv6RtrRoutePrefs;
    break;
  case 1560:
    errMsg = pStrInfo_routingv6_Ipv6RouteTbl;
    break;
  case 1567:
    errMsg = pStrInfo_routingv6_Ipv6RouteEntryCreate;
    break;
  case 1568:
    errMsg = pStrInfo_routingv6_Ipv6RouteEntryCfg;
    break;
  case 1570:
    errMsg = pStrInfo_routingv6_CfguredIpv6Routes;
    break;
  case 1626:
    errMsg = pStrInfo_routingv6_Ipv6Stats_1;
    break;
  case 1627:
    errMsg = pStrInfo_routingv6_Ipv6NeighborTbl;
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

L7_char8 *usmWebRoutingv6GetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2500:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3info;
    break;
  case 2501:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3RouteRedistributionSummary;
    break;
  case 2502:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospv3RouteRedistribution;
    break;
  case 2503:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3VirtLink;
    break;
  case 2504:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3VirtLinkSumm;
    break;
  case 2505:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3IntfStats;
    break;
  case 2506:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Intfcfg;
    break;
  case 2507:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Area;
    break;
  case 2508:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3AreaRange;
    break;
  case 2509:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3StubArea;
    break;
  case 2510:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Lsdb;
    break;
  case 2511:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6globalcfg;
    break;
  case 2512:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6stats;
    break;
  case 2513:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolcfg;
    break;
  case 2514:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolbinding;
    break;
  case 2515:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6intfcfg;
    break;
  case 2516:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolsummary;
    break;
  case 2517:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6intfsummary;
    break;
  case 2518:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6intfcfg;
    break;
  case 2520:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routerrouteprefs;
    break;
  case 2521:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routeentrycreate;
    break;
  case 2524:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6configuredroutes;
    break;
  case 2525:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routetable;
    break;
  case 2526:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6globalcfg;
    break;
  case 2527:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6StatsHelpIpv6statistics;
    break;
  case 2528:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Ipv6prefixdeleg;
    break;
  case 2529:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3NeighborTbl;
    break;
  case 2530:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Neighbors;
    break;
  case 2535:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6neighbor;
    break;
  case 2536:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3LsdbXUI;
    break;
  case 2537:
    errMsg = pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routetableXUI;
    break;

  
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
