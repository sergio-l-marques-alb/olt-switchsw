/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/routing/oem_routing.c
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
#include "strlib_routing_common.h"
#include "strlib_routing_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebRoutingGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebRoutingGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1:
    errMsg = pStrInfo_common_Iso88591;
    break;
  case 1004:
    errMsg = pStrInfo_common_MacAddr_2;
    break;
  case 1005:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1007:
    errMsg = pStrInfo_routing_SlotId_1;
    break;
  case 1090:
    errMsg = pStrInfo_common_SubnetMask_3;
    break;
  case 1101:
    errMsg = pStrInfo_common_Port_2;
    break;
  case 1102:
    errMsg = pStrInfo_common_IgmpProxyAdminMode;
    break;
  case 1219:
    errMsg = pStrInfo_routing_Help_1;
    break;
  case 1313:
    errMsg = pStrInfo_routing_RipAdminMode;
    break;
  case 1314:
    errMsg = pStrInfo_common_IntfMode;
    break;
  case 1376:
    errMsg = pStrInfo_routing_IntfBridging;
    break;
  case 1377:
    errMsg = pStrInfo_routing_BridgingState;
    break;
  case 1378:
    errMsg = pStrInfo_routing_BridgingPort;
    break;
  case 1379:
    errMsg = pStrInfo_routing_BridgingSwitch;
    break;
  case 1383:
    errMsg = pStrInfo_common_NwSummary;
    break;
  case 1387:
    errMsg = pStrInfo_common_NssaExternal;
    break;
  case 1419:
    errMsg = pStrInfo_common_TimeSinceCountersLastClred;
    break;
  case 1465:
    errMsg = pStrInfo_routing_MetricDefl;
    break;
  case 1466:
    errMsg = pStrInfo_routing_MetricMonetary;
    break;
  case 1467:
    errMsg = pStrInfo_common_MetricCost;
    break;
  case 1468:
    errMsg = pStrInfo_routing_MetricThroughput;
    break;
  case 1469:
    errMsg = pStrInfo_routing_MetricDelay;
    break;
  case 1474:
    errMsg = pStrInfo_common_WsNwLocal;
    break;
  case 1476:
    errMsg = pStrInfo_common_Rip;
    break;
  case 1478:
    errMsg = pStrInfo_routing_PeerRtrIpAddr;
    break;
  case 1479:
    errMsg = pStrInfo_common_LastUpdate;
    break;
  case 1480:
    errMsg = pStrInfo_routing_RipVer;
    break;
  case 1481:
    errMsg = pStrInfo_routing_BadRipPktsRcvd;
    break;
  case 1492:
    errMsg = pStrInfo_routing_RoutingState;
    break;
  case 1532:
    errMsg = pStrInfo_routing_NwAddr;
    break;
  case 1535:
    errMsg = pStrInfo_common_NextHopIpAddr;
    break;
  case 1537:
    errMsg = pStrInfo_common_AreaId;
    break;
  case 1538:
    errMsg = pStrInfo_common_AlarmType;
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
  case 1556:
    errMsg = pStrInfo_routing_NbmaPollIntvlSecs;
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
  case 1561:
    errMsg = pStrInfo_routing_LocalLinkLsas;
    break;
  case 1562:
    errMsg = pStrInfo_routing_LocalLinkLsaChecksum;
    break;
  case 1564:
    errMsg = pStrInfo_common_OspfAreaId;
    break;
  case 1569:
    errMsg = pStrInfo_routing_DeflTimeToLive;
    break;
  case 1570:
    errMsg = pStrInfo_routing_IpSpoofingMode;
    break;
  case 1572:
    errMsg = pStrInfo_routing_IpSrcCheckingMode;
    break;
  case 1574:
    errMsg = pStrInfo_routing_ActiveRoutes;
    break;
  case 1575:
    errMsg = pStrInfo_routing_ForwardNetDirectedBcasts;
    break;
  case 1582:
    errMsg = pStrInfo_common_IftransitDelayIntvlSecs;
    break;
  case 1589:
    errMsg = pStrInfo_common_NumOfLinkEvents;
    break;
  case 1590:
    errMsg = pStrInfo_routing_ArpTblCfg;
    break;
  case 1591:
    errMsg = pStrInfo_routing_DemandAging;
    break;
  case 1592:
    errMsg = pStrInfo_common_RespTimeSecs;
    break;
  case 1593:
    errMsg = pStrInfo_routing_Retries_1;
    break;
  case 1594:
    errMsg = pStrInfo_routing_CacheSize;
    break;
  case 1599:
    errMsg = pStrInfo_common_StubAreaInfo;
    break;
  case 1610:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1611:
    errMsg = pStrInfo_routing_AdministrativeState;
    break;
  case 1613:
    errMsg = pStrInfo_routing_ActiveState;
    break;
  case 1615:
    errMsg = pStrInfo_routing_EncapType;
    break;
  case 1616:
    errMsg = pStrInfo_routing_VlanTagging_2;
    break;
  case 1618:
    errMsg = pStrInfo_routing_OspfAdminState;
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
  case 1622:
    errMsg = pStrInfo_routing_OpaqueLsaStatus;
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
  case 1630:
    errMsg = pStrInfo_common_EtherNet;
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
  case 1640:
    errMsg = pStrInfo_common_RtrPri;
    break;
  case 1641:
    errMsg = pStrInfo_common_State;
    break;
  case 1642:
    errMsg = pStrInfo_common_Events;
    break;
  case 1643:
    errMsg = pStrInfo_routing_Permanence;
    break;
  case 1644:
    errMsg = pStrInfo_routing_HellosSuppressed;
    break;
  case 1645:
    errMsg = pStrInfo_common_RetransmissionQueueLen;
    break;
  case 1651:
    errMsg = pStrInfo_routing_IpInReceives;
    break;
  case 1652:
    errMsg = pStrErr_routing_IpInHdrErrs;
    break;
  case 1653:
    errMsg = pStrErr_routing_IpInAddrErrs;
    break;
  case 1654:
    errMsg = pStrInfo_routing_IpForwDatagrams;
    break;
  case 1655:
    errMsg = pStrInfo_routing_IpInUnknownProtos;
    break;
  case 1656:
    errMsg = pStrInfo_routing_IpInDiscards;
    break;
  case 1657:
    errMsg = pStrInfo_routing_IpInDelivers;
    break;
  case 1658:
    errMsg = pStrInfo_routing_IpOutReqs;
    break;
  case 1659:
    errMsg = pStrInfo_routing_IpOutDiscards;
    break;
  case 1660:
    errMsg = pStrInfo_routing_IpOutNoRoutes;
    break;
  case 1661:
    errMsg = pStrInfo_routing_IpReasmTimeout;
    break;
  case 1662:
    errMsg = pStrInfo_routing_IpReasmReq;
    break;
  case 1663:
    errMsg = pStrInfo_routing_IpReasmFails;
    break;
  case 1664:
    errMsg = pStrInfo_routing_IpFragOks;
    break;
  case 1665:
    errMsg = pStrInfo_routing_IpFragFails;
    break;
  case 1666:
    errMsg = pStrInfo_routing_IpFragCreates;
    break;
  case 1667:
    errMsg = pStrInfo_routing_IpRoutingDiscards;
    break;
  case 1668:
    errMsg = pStrInfo_routing_IcmpInMsgs;
    break;
  case 1669:
    errMsg = pStrErr_routing_IcmpInErrs;
    break;
  case 1670:
    errMsg = pStrInfo_routing_IcmpInDestUnreach;
    break;
  case 1671:
    errMsg = pStrInfo_routing_IcmpInTimeExceeds;
    break;
  case 1672:
    errMsg = pStrInfo_routing_IcmpInParmProbs;
    break;
  case 1673:
    errMsg = pStrInfo_routing_IcmpInSrcQuenchs;
    break;
  case 1674:
    errMsg = pStrInfo_routing_IcmpInRedirects;
    break;
  case 1675:
    errMsg = pStrInfo_routing_IcmpInEchos;
    break;
  case 1676:
    errMsg = pStrInfo_routing_IcmpInEchoReps;
    break;
  case 1677:
    errMsg = pStrInfo_routing_IcmpInTimeStamps;
    break;
  case 1678:
    errMsg = pStrInfo_routing_IcmpInAddrMasks;
    break;
  case 1679:
    errMsg = pStrInfo_routing_IcmpInAddrMaskReps;
    break;
  case 1680:
    errMsg = pStrInfo_routing_IcmpOutMsgs;
    break;
  case 1681:
    errMsg = pStrErr_routing_IcmpOutErrs;
    break;
  case 1682:
    errMsg = pStrInfo_routing_IcmpOutDestUnreach;
    break;
  case 1683:
    errMsg = pStrInfo_routing_IcmpOutTimeExceeds;
    break;
  case 1684:
    errMsg = pStrInfo_routing_IcmpOutParmProbs;
    break;
  case 1685:
    errMsg = pStrInfo_routing_IcmpOutSrcQuenchs;
    break;
  case 1686:
    errMsg = pStrInfo_routing_IcmpOutRedirects;
    break;
  case 1687:
    errMsg = pStrInfo_routing_Icmpoutechos;
    break;
  case 1688:
    errMsg = pStrInfo_routing_IcmpOutEchoReps;
    break;
  case 1689:
    errMsg = pStrInfo_routing_IcmpOutTimeStamps;
    break;
  case 1690:
    errMsg = pStrInfo_routing_IcmpOutTimeStampReps;
    break;
  case 1691:
    errMsg = pStrInfo_routing_IcmpOutAddrMasks;
    break;
  case 1693:
    errMsg = pStrInfo_routing_IpReasmOks;
    break;
  case 1694:
    errMsg = pStrInfo_routing_IcmpInTimeStampReps;
    break;
  case 1695:
    errMsg = pStrInfo_routing_Icmpoutaddrmaskreps;
    break;
  case 1696:
    errMsg = pStrInfo_routing_AgeTimeSecs;
    break;
  case 1702:
    errMsg = pStrInfo_routing_RipAdminMode;
    break;
  case 1727:
    errMsg = pStrInfo_common_Snap;
    break;
  case 1730:
    errMsg = pStrInfo_common_AsbrMode;
    break;
  case 1735:
    errMsg = pStrInfo_routing_NeighborIntfIdx;
    break;
  case 1737:
    errMsg = pStrInfo_routing_RouteType;
    break;
  case 1739:
    errMsg = pStrInfo_common_OspfAdminMode;
    break;
  case 1755:
    errMsg = pStrInfo_common_LsdbType;
    break;
  case 1756:
    errMsg = pStrInfo_common_Normal;
    break;
  case 1758:
    errMsg = pStrInfo_routing_Reliability;
    break;
  case 1759:
    errMsg = pStrInfo_routing_Throughput;
    break;
  case 1760:
    errMsg = pStrInfo_common_Delay;
    break;
  case 1761:
    errMsg = pStrInfo_common_ImportSummaryLsas;
    break;
  case 1766:
    errMsg = pStrInfo_routing_TypeOfService;
    break;
  case 1785:
    errMsg = pStrInfo_common_OspfIntra;
    break;
  case 1786:
    errMsg = pStrInfo_common_OspfInter;
    break;
  case 1787:
    errMsg = pStrInfo_common_OspfExternal;
    break;
  case 1879:
    errMsg = pStrInfo_common_NoValidAreasAvailable;
    break;
  case 1880:
    errMsg = pStrInfo_routing_Area_1;
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
  case 4107:
    errMsg = pStrInfo_common_TotalNumOfRoutes;
    break;
  case 4108:
    errMsg = pStrInfo_routing_Rfc1583Compatibility;
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
  case 4124:
    errMsg = pStrInfo_routing_GlobalRouteChgs;
    break;
  case 4125:
    errMsg = pStrInfo_routing_GlobalQueries;
    break;
  case 4126:
    errMsg = pStrInfo_routing_SendVer;
    break;
  case 4127:
    errMsg = pStrInfo_routing_ReceiveVer;
    break;
  case 4129:
    errMsg = pStrInfo_common_Rip_1;
    break;
  case 4130:
    errMsg = pStrInfo_common_Rip_2;
    break;
  case 4131:
    errMsg = pStrInfo_common_Rip2;
    break;
  case 4132:
    errMsg = pStrInfo_common_Both;
    break;
  case 4133:
    errMsg = pStrInfo_common_LinkState;
    break;
  case 4135:
    errMsg = pStrInfo_common_AuthKey;
    break;
  case 4137:
    errMsg = pStrInfo_routing_BadPktsRcvd;
    break;
  case 4138:
    errMsg = pStrInfo_routing_BadRoutesRcvd;
    break;
  case 4139:
    errMsg = pStrInfo_common_UpdatesSent;
    break;
  case 4141:
    errMsg = pStrInfo_common_RoutingMode;
    break;
  case 4144:
    errMsg = pStrInfo_common_MetricType;
    break;
  case 4153:
    errMsg = pStrInfo_routing_AdvertiseMode;
    break;
  case 4154:
    errMsg = pStrInfo_routing_AdvertiseAddr;
    break;
  case 4155:
    errMsg = pStrInfo_routing_MaxAdvertiseIntvlSecs;
    break;
  case 4156:
    errMsg = pStrInfo_routing_MinAdvertiseIntvlSecs;
    break;
  case 4157:
    errMsg = pStrInfo_routing_AdvertiseLifetimeSecs;
    break;
  case 4158:
    errMsg = pStrInfo_routing_PrefLvl;
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
  case 6014:
    errMsg = pStrInfo_common_VlanId_1;
    break;
  case 6016:
    errMsg = pStrInfo_common_Defl;
    break;
  case 6017:
    errMsg = pStrInfo_common_PimSmGrpRpMapStatic;
    break;
  case 6056:
    errMsg = pStrInfo_common_WsActive;
    break;
  case 6063:
    errMsg = pStrInfo_common_WsInactive;
    break;
  case 6072:
    errMsg = pStrInfo_common_StaticReject;
    break;
  case 6215:
    errMsg = pStrInfo_common_AdministrativeMode;
    break;
  case 6216:
    errMsg = pStrInfo_common_DestUnreachables;
    break;
  case 6217:
    errMsg = pStrInfo_common_ICMPRedirects;
    break;
  case 6218:
    errMsg = pStrInfo_routing_IpIcmpEchoReplyMode;
    break;
  case 6219:
    errMsg = pStrInfo_routing_IpIcmpRateLimitInterval;
    break;
  case 6220:
    errMsg = pStrInfo_routing_IpIcmpRateLimitBurstSize;
    break;
  case 6299:
    errMsg = pStrInfo_routing_LinkSpeedDataRate;
    break;
  case 7310:
    errMsg = pStrInfo_common_Enbld;
    break;
  case 7363:
    errMsg = pStrInfo_common_SrvrIpAddr;
    break;
  case 7365:
    errMsg = pStrInfo_routing_MaxHopCount;
    break;
  case 7366:
    errMsg = pStrInfo_routing_MinWaitTimeSecs;
    break;
  case 7367:
    errMsg = pStrInfo_routing_CircuitIdOptionMode;
    break;
  case 7369:
    errMsg = pStrInfo_routing_ReqsRcvd;
    break;
  case 7370:
    errMsg = pStrInfo_routing_ReqsRelayed;
    break;
  case 7371:
    errMsg = pStrInfo_common_PktsDiscarded;
    break;
  case 7407:
    errMsg = pStrInfo_common_NeighborState;
    break;
  case 7603:
    errMsg = pStrInfo_common_NoRedistributeMode;
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
  case 8800:
    errMsg = pStrInfo_common_ConfigPriority;
    break;
  case 8805:
    errMsg = pStrInfo_common_IgmpProxyAdminMode;
    break;
  case 8806:
    errMsg = pStrInfo_routing_Vrid_1;
    break;
  case 8808:
    errMsg = pStrInfo_common_Pri_1;
    break;
  case 8809:
    errMsg = pStrInfo_routing_PreEmptMode;
    break;
  case 8810:
    errMsg = pStrInfo_routing_AdvertisementIntvlSecs;
    break;
  case 8812:
    errMsg = pStrInfo_routing_VirtualIpAddr;
    break;
  case 8813:
    errMsg = pStrInfo_routing_IntfIpAddr;
    break;
  case 8814:
    errMsg = pStrInfo_routing_VmacAddr;
    break;
  case 8815:
    errMsg = pStrInfo_routing_Owner;
    break;
  case 8816:
    errMsg = pStrInfo_common_AuthType_1;
    break;
  case 8817:
    errMsg = pStrInfo_routing_AuthType;
    break;
  case 8818:
    errMsg = pStrInfo_common_State;
    break;
  case 8819:
    errMsg = pStrInfo_common_ApStatus;
    break;
  case 8821:
    errMsg = pStrInfo_routing_RtrChecksumErrs;
    break;
  case 8822:
    errMsg = pStrInfo_routing_RtrVerErrs;
    break;
  case 8823:
    errMsg = pStrInfo_routing_RtrVridErrs;
    break;
  case 8824:
    errMsg = pStrInfo_routing_StateTransitionedToMaster;
    break;
  case 8825:
    errMsg = pStrInfo_routing_AdvertisementRcvd;
    break;
  case 8826:
    errMsg = pStrInfo_routing_AdvertisementIntvlErrs;
    break;
  case 8827:
    errMsg = pStrErr_common_AuthFailure;
    break;
  case 8828:
    errMsg = pStrInfo_routing_IpTtlErrs;
    break;
  case 8829:
    errMsg = pStrInfo_routing_ZeroPriPktsRcvd;
    break;
  case 8830:
    errMsg = pStrInfo_routing_ZeroPriPktsSent;
    break;
  case 8831:
    errMsg = pStrErr_routing_TypePktsRcvd;
    break;
  case 8832:
    errMsg = pStrInfo_routing_AddrListErrs;
    break;
  case 8833:
    errMsg = pStrErr_routing_AuthType_2;
    break;
  case 8834:
    errMsg = pStrInfo_routing_AuthTypeMismatch;
    break;
  case 8835:
    errMsg = pStrInfo_routing_PktLenErrs;
    break;
  case 8836:
    errMsg = pStrInfo_routing_AdvertisementIntvlSecs;
    break;
  case 8837:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 8838:
    errMsg = pStrInfo_routing_AuthData;
    break;
  case 8839:
    errMsg = pStrInfo_routing_0None;
    break;
  case 8840:
    errMsg = pStrInfo_routing_1Simple;
    break;
  case 8847:
    errMsg = pStrInfo_common_True2;
    break;
  case 8848:
    errMsg = pStrInfo_common_False2;
    break;
  case 8854:
    errMsg = pStrInfo_common_UpTime;
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
  case 11457:
    errMsg = pStrInfo_routing_NoVirtualRtrIntfsAvailable;
    break;
  case 11602:
    errMsg = pStrInfo_common_NeighborIpAddr;
    break;
  case 11810:
    errMsg = pStrInfo_common_Mpls;
    break;
  case 11858:
    errMsg = pStrInfo_common_Bgp4;
    break;
  case 12702:
    errMsg = pStrInfo_routing_Missing12702;
    break;
  case 15050:
    errMsg = pStrInfo_common_UnAuthorized;
    break;
  case 15061:
    errMsg = pStrInfo_routing_AutoSummaryMode;
    break;
  case 15062:
    errMsg = pStrInfo_routing_HostRoutesAcceptMode;
    break;
  case 15063:
    errMsg = pStrInfo_routing_SplitHorizonMode;
    break;
  case 15064:
    errMsg = pStrInfo_routing_PoisonReverse;
    break;
  case 16026:
    errMsg = pStrInfo_common_NoNeighborsCfgured;
    break;
  case 16031:
    errMsg = pStrInfo_common_0_1;
    break;
  case 16173:
    errMsg = pStrInfo_common_Metric;
    break;
  case 16174:
    errMsg = pStrInfo_common_MetricType;
    break;
  case 16175:
    errMsg = pStrInfo_common_Match;
    break;
  case 16176:
    errMsg = pStrInfo_common_DistributeList;
    break;
  case 16177:
    errMsg = pStrInfo_common_InternalRoutes;
    break;
  case 16178:
    errMsg = pStrInfo_common_ExternalType1Routes;
    break;
  case 16179:
    errMsg = pStrInfo_common_ExternalType2Routes;
    break;
  case 16180:
    errMsg = pStrInfo_common_NssaExternalType1Routes;
    break;
  case 16181:
    errMsg = pStrInfo_common_NssaExternalType2Routes;
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
  case 16191:
    errMsg = pStrInfo_routing_Subnets;
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
  case 16256:
    errMsg = pStrInfo_routing_DynRenew;
    break;
  case 16260:
    errMsg = pStrInfo_common_ApAge;
    break;
  case 16261:
    errMsg = pStrInfo_routing_RemoveFromTbl;
    break;
  case 16262:
    errMsg = pStrInfo_routing_AllDynEntries;
    break;
  case 16263:
    errMsg = pStrInfo_routing_AllDynAndGatewayEntries;
    break;
  case 16264:
    errMsg = pStrInfo_routing_SpecificDynGatewayEntry;
    break;
  case 16265:
    errMsg = pStrInfo_routing_RemoveIpAddr;
    break;
  case 16266:
    errMsg = pStrInfo_common_None_1;
    break;
  case 16267:
    errMsg = pStrInfo_routing_SpecificStaticEntry;
    break;
  case 21415:
    errMsg = pStrInfo_routing_LocalProxyArp;
    break;
  case 22051:
    errMsg = pStrInfo_common_ExternalLinkStateDbaseLimit;
    break;
  case 23001:
    errMsg = pStrInfo_routing_TunnelId;
    break;

  case 23002:
    errMsg = pStrInfo_common_DefaultOriginate;
    break;



  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebRoutingGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1229:
    errMsg = pStrInfo_routing_RtrRouteEntryCfg;
    break;
  default:
    local_call=L7_TRUE;
    errMsg = usmWebRoutingGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebRoutingGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1200:
    errMsg = pStrInfo_routing_ArpCreate;
    break;
  case 1201:
    errMsg = pStrInfo_routing_ArpTblCfg;
    break;
  case 1202:
    errMsg = pStrInfo_routing_IpIntfCfg;
    break;
  case 1203:
    errMsg = pStrInfo_routing_IpCfg;
    break;
  case 1204:
    errMsg = pStrInfo_routing_IpStats;
    break;
  case 1206:
    errMsg = pStrInfo_routing_BridgingCfg;
    break;
  case 1207:
    errMsg = pStrInfo_routing_OspfLsaDbase;
    break;
  case 1208:
    errMsg = pStrInfo_routing_OspfAreaRangeCfg;
    break;
  case 1209:
    errMsg = pStrInfo_routing_OspfAreaCfg;
    break;
  case 1210:
    errMsg = pStrInfo_routing_OspfIntfCfg;
    break;
  case 1211:
    errMsg = pStrInfo_routing_OspfLinkStateDbase;
    break;
  case 1212:
    errMsg = pStrInfo_routing_OspfIntfStats;
    break;
  case 1213:
    errMsg = pStrInfo_routing_OspfStubAreaSummary;
    break;
  case 1214:
    errMsg = pStrInfo_routing_OspfVirtualLinkCfg;
    break;
  case 1215:
    errMsg = pStrInfo_routing_OspfVirtualLinkSummary;
    break;
  case 1216:
    errMsg = pStrInfo_routing_OspfCfg;
    break;
  case 1217:
    errMsg = pStrInfo_routing_OspfLinkStateDbaseDetailed;
    break;
  case 1218:
    errMsg = pStrInfo_routing_BootpDhcpRelayAgentCfg;
    break;
  case 1219:
    errMsg = pStrInfo_routing_RipIntfCfg;
    break;
  case 1220:
    errMsg = pStrInfo_routing_RipIntfSummary;
    break;
  case 1221:
    errMsg = pStrInfo_routing_RipCfg;
    break;
  case 1222:
    errMsg = pStrInfo_routing_RtrRipPeers;
    break;
  case 1223:
    errMsg = pStrInfo_routing_RtrDiscCfg;
    break;
  case 1224:
    errMsg = pStrInfo_routing_RtrDiscStatus;
    break;
  case 1225:
    errMsg = pStrInfo_routing_VirtualRtrCfg;
    break;
  case 1226:
    errMsg = pStrInfo_routing_VirtualRtrStats;
    break;
  case 1227:
    errMsg = pStrInfo_routing_VirtualRtrStatus;
    break;
  case 1228:
    errMsg = pStrInfo_routing_VrrpCfg;
    break;
  case 1229:
    errMsg = pStrInfo_routing_RtrRouteEntryCfg;
    break;
  case 1230:
    errMsg = pStrInfo_routing_OspfNeighborCfg;
    break;
  case 1231:
    errMsg = pStrInfo_routing_OspfNeighborTbl;
    break;
  case 1232:
    errMsg = pStrInfo_routing_RtrRoutePrefsCfg;
    break;
  case 1233:
    errMsg = pStrInfo_routing_RtrRouteTbl;
    break;
  case 1234:
    errMsg = pStrInfo_routing_RtrBestRoutesTbl;
    break;
  case 1237:
    errMsg = pStrInfo_routing_VlanRoutingCfg;
    break;
  case 1238:
    errMsg = pStrInfo_routing_VlanRoutingSummary;
    break;
  case 1239:
    errMsg = pStrInfo_routing_BootpDhcpRelayAgentStatus;
    break;
  case 1240:
    errMsg = pStrInfo_routing_RipIntfAuthCfg;
    break;
  case 1241:
    errMsg = pStrInfo_common_OspfVirtualLinkAuthCfg;
    break;
  case 1242:
    errMsg = pStrInfo_routing_OspfIntfAuthCfg;
    break;
  case 1243:
    errMsg = pStrInfo_routing_RipRouteRedistributionSummary;
    break;
  case 1244:
    errMsg = pStrInfo_routing_OspfRouteRedistributionSummary;
    break;
  case 1245:
    errMsg = pStrInfo_routing_RipRouteRedistributionCfg;
    break;
  case 1246:
    errMsg = pStrInfo_routing_OspfRouteRedistributionCfg;
    break;
  case 1250:
    errMsg = pStrInfo_routing_RtrRouteEntryCreate;
    break;
  case 1251:
    errMsg = pStrInfo_routing_IpIntfSecondaryAddrCfg;
    break;
  case 1252:
    errMsg = pStrInfo_routing_VrrpSecondaryAddrCfg;
    break;
  case 1253:
    errMsg = pStrInfo_routing_CfguredRoutes;
    break;
  case 1254:
    errMsg = pStrInfo_routing_VrrpIntfTrackCfg;
    break;
  case 1255:
    errMsg = pStrInfo_routing_VrrpIntfTrackCfgAdd;
    break;
  case 1256:
    errMsg = pStrInfo_routing_VrrpRoutTrackCfg;
    break;
  case 1257:
    errMsg = pStrInfo_routing_VrrpRoutTrackCfgAdd;
    break;
  case 1260:
    errMsg = pStrInfo_routing_TunnelCfg;
    break;
  case 1261:
    errMsg = pStrInfo_routing_TunnelSummary;
    break;
  case 1262:
    errMsg = pStrInfo_routing_LoopbackCfg;
    break;
  case 1263:
    errMsg = pStrInfo_routing_LoopbackSummary;
    break;
  case 1280:
    errMsg = pStrInfo_routing_IpIntfHelperAddrCfg;
    break;
  case 1281:
    errMsg = pStrInfo_routing_ArpTblCfgXUI;
    break;
  case 1282:
    errMsg = pStrInfo_routing_RtrRouteTblXUI;
    break;
  case 1283:
    errMsg = pStrInfo_routing_RtrBestRoutesTblXUI;
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

L7_char8 *usmWebRoutingGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2200:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipArpCreate;
    break;
  case 2201:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipArpTbl;
    break;
  case 2202:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipIntfipcfg;
    break;
  case 2203:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipIpcfg;
    break;
  case 2204:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipIpstats;
    break;
  case 2206:
    errMsg = pStrInfo_common_HtmlFileBaseSysHelpIdx;
    break;
  case 2207:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfLsadb;
    break;
  case 2208:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfAreaRange;
    break;
  case 2209:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfarea;
    break;
  case 2210:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfintfcfg;
    break;
  case 2211:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfLsdbSumm;
    break;
  case 2212:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfintfstats;
    break;
  case 2213:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfStubsummary;
    break;
  case 2214:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfVirtLink;
    break;
  case 2215:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfVirtLinkSumm;
    break;
  case 2216:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfinfo;
    break;
  case 2217:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfLsdbDet;
    break;
  case 2218:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRelayRelayHelpCfg;
    break;
  case 2219:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipDet;
    break;
  case 2220:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipSumm;
    break;
  case 2221:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipInfo;
    break;
  case 2222:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipDet;
    break;
  case 2223:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRtrDiscHelpRtrDiscRtrDisc;
    break;
  case 2224:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRtrDiscHelpRtrDiscRtrDiscStatus;
    break;
  case 2225:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrcfg;
    break;
  case 2226:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrstats;
    break;
  case 2227:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrstatus;
    break;
  case 2228:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrrpcfg;
    break;
  case 2229:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrRtEntry;
    break;
  case 2230:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpLayer3NbrNbrDt;
    break;
  case 2231:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpLayer3NbrNbrSumm;
    break;
  case 2232:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrRtPrefs;
    break;
  case 2233:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrRtTbl;
    break;
  case 2234:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrBestRtTbl;
    break;
  case 2237:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVlanRoutingHelpVlanRoutingVrcfg;
    break;
  case 2238:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVlanRoutingHelpVlanRoutingVrsum;
    break;
  case 2239:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRelayRelayHelpStatus;
    break;
  case 2243:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipRtRdstrbtnSumm;
    break;
  case 2244:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfRtRdstrbtnSumm;
    break;
  case 2245:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRipHelpRipRipRtRdstrbtnCfg;
    break;
  case 2246:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfOspfRtRdstrbtnCfg;
    break;
  case 2250:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrRtEntryCreate;
    break;
  case 2251:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipSecIpCfg;
    break;
  case 2252:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrsecondary;
    break;
  case 2253:
    errMsg = pStrInfo_routing_HtmlLinkRoutingRoutingHelpRtrRtCfgured;
    break;
  case 2254:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrtrackintf;
    break;
  case 2255:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrtrackintfAdd;
    break;
  case 2256:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrtrackroute;
    break;
  case 2257:
    errMsg = pStrInfo_routing_HtmlLinkRoutingVrrpHelpVrrpVrtrackrouteAdd;
    break;
  case 2260:
    errMsg = pStrInfo_routing_HtmlLinkRoutingTunnelHelpTunnelTunnelcfg;
    break;
  case 2261:
    errMsg = pStrInfo_routing_HtmlLinkRoutingTunnelHelpTunnelTunnelsumm;
    break;
  case 2262:
    errMsg = pStrInfo_routing_HtmlLinkRoutingLoopbackHelpLoopbackLoopbackconfig;
    break;
  case 2263:
    errMsg = pStrInfo_routing_HtmlLinkRoutingLoopbackHelpLoopbackLoopbacksumm;
    break;
  case 2280:
    errMsg = pStrInfo_routing_HtmlLinkRoutingIpHelpLayer3ipHelperIpCfg;
    break;
  case 1281:
    errMsg = pStrInfo_routing_ArpTblCfgXUI;
    break;
  case 1282:
    errMsg = pStrInfo_routing_RtrRouteTblXUI;
    break;
  case 1283:
    errMsg = pStrInfo_routing_RtrBestRoutesTblXUI;
    break;
  case 1284:
    errMsg = pStrInfo_routing_HtmlLinkRoutingOspfHelpOspfLsadbXUI;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
