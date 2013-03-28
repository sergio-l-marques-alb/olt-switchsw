/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/bgp/oem_bgp.c
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
#include "strlib_bgp_common.h"
#include "strlib_bgp_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebBgpGetPageHdr1(L7_int32 token);

static L7_BOOL local_call=L7_FALSE;
L7_char8 *usmWebBgpGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1005:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1090:
    errMsg = pStrInfo_common_SubnetMask_3;
    break;
  case 1164:
    errMsg = pStrInfo_common_AccessMode;
    break;
  case 1548:
    errMsg = pStrInfo_common_Proto;
    break;
  case 1560:
    errMsg = pStrInfo_common_AuthType_1;
    break;
  case 1882:
    errMsg = pStrInfo_bgp_NoPeersAvailable;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 4135:
    errMsg = pStrInfo_common_AuthKey;
    break;
  case 4139:
    errMsg = pStrInfo_common_UpdatesSent;
    break;
  case 5200:
    errMsg = pStrInfo_common_None_1;
    break;
  case 8818:
    errMsg = pStrInfo_common_State;
    break;
  case 11592:
    errMsg = pStrInfo_bgp_NlriEntry;
    break;
  case 11601:
    errMsg = pStrInfo_common_IgmpProxyVer;
    break;
  case 11606:
    errMsg = pStrInfo_common_RoutingIpv6RadvPrefix;
    break;
  case 11607:
    errMsg = pStrInfo_bgp_PrefixLenBits;
    break;
  case 11608:
    errMsg = pStrInfo_bgp_AggregateEffect;
    break;
  case 11609:
    errMsg = pStrInfo_bgp_AdvertiseFeasibility;
    break;
  case 11611:
    errMsg = pStrInfo_bgp_Advertise;
    break;
  case 11612:
    errMsg = pStrInfo_bgp_Donotadvertise;
    break;
  case 11614:
    errMsg = pStrInfo_bgp_AddrFamily;
    break;
  case 11618:
    errMsg = pStrInfo_common_IgmpProxyAdminMode;
    break;
  case 11620:
    errMsg = pStrInfo_bgp_LocalAutonomousSys;
    break;
  case 11621:
    errMsg = pStrInfo_bgp_LocalId;
    break;
  case 11622:
    errMsg = pStrInfo_bgp_PropagateMedMode;
    break;
  case 11623:
    errMsg = pStrInfo_bgp_CalculateMedMode;
    break;
  case 11624:
    errMsg = pStrInfo_bgp_ExternalBorderRtrsAdvertisementMode;
    break;
  case 11625:
    errMsg = pStrInfo_bgp_MininumAsOriginationIntvlSecs;
    break;
  case 11626:
    errMsg = pStrInfo_bgp_MininumRouteAdvertisementIntvlSecs;
    break;
  case 11628:
    errMsg = pStrInfo_bgp_RouteReflectorMode;
    break;
  case 11629:
    errMsg = pStrInfo_bgp_ClusterId_1;
    break;
  case 11630:
    errMsg = pStrInfo_bgp_ConfederationId;
    break;
  case 11631:
    errMsg = pStrInfo_bgp_AutoRestartMode;
    break;
  case 11633:
    errMsg = pStrInfo_bgp_AddrAggregationEntry;
    break;
  case 11634:
    errMsg = pStrInfo_bgp_RouteLocalOrigin;
    break;
  case 11635:
    errMsg = pStrInfo_bgp_RouteLocalMed;
    break;
  case 11636:
    errMsg = pStrInfo_bgp_RouteLocalPref;
    break;
  case 11637:
    errMsg = pStrInfo_bgp_SuppRessMode;
    break;
  case 11638:
    errMsg = pStrInfo_bgp_RouteCommunity;
    break;
  case 11639:
    errMsg = pStrInfo_bgp_NextHopAddrLen;
    break;
  case 11640:
    errMsg = pStrInfo_bgp_PathAttrAggregationMode;
    break;
  case 11641:
    errMsg = pStrInfo_bgp_AddrAggregationMode;
    break;
  case 11644:
    errMsg = pStrInfo_bgp_PeerId;
    break;
  case 11645:
    errMsg = pStrInfo_bgp_PeerState;
    break;
  case 11646:
    errMsg = pStrInfo_bgp_PeerAdminStatus;
    break;
  case 11647:
    errMsg = pStrInfo_bgp_NegotiatedVer;
    break;
  case 11648:
    errMsg = pStrInfo_common_LocalPort;
    break;
  case 11649:
    errMsg = pStrInfo_bgp_RemoteAddr;
    break;
  case 11650:
    errMsg = pStrInfo_bgp_RemotePort;
    break;
  case 11651:
    errMsg = pStrInfo_bgp_RemoteAs;
    break;
  case 11652:
    errMsg = pStrInfo_bgp_ConnRetryIntvlSecs;
    break;
  case 11653:
    errMsg = pStrInfo_bgp_CfguredHoldTimeSecs;
    break;
  case 11654:
    errMsg = pStrInfo_bgp_NegotiatedHoldTimeSecs;
    break;
  case 11655:
    errMsg = pStrInfo_bgp_CfguredKeepAliveTimeSecs;
    break;
  case 11658:
    errMsg = pStrInfo_bgp_RouteReflector;
    break;
  case 11659:
    errMsg = pStrInfo_bgp_ConfederationMbr;
    break;
  case 11660:
    errMsg = pStrInfo_bgp_NextHopSelfMode;
    break;
  case 11662:
    errMsg = pStrInfo_bgp_LocalIntfAddr;
    break;
  case 11663:
    errMsg = pStrInfo_bgp_MsgSendLimit;
    break;
  case 11664:
    errMsg = pStrInfo_bgp_TransmissionDelayIntvlSecs;
    break;
  case 11665:
    errMsg = pStrInfo_bgp_UpdatesRcvd;
    break;
  case 11666:
    errMsg = pStrInfo_bgp_TotalMsgsRcvd;
    break;
  case 11667:
    errMsg = pStrInfo_bgp_TotalMsgsSent;
    break;
  case 11668:
    errMsg = pStrInfo_bgp_EstablishedTransitions;
    break;
  case 11669:
    errMsg = pStrInfo_bgp_EstablishedTime;
    break;
  case 11670:
    errMsg = pStrInfo_bgp_TimeElapsedSinceLastUpdate;
    break;
  case 11671:
    errMsg = pStrErr_bgp_Last_1;
    break;
  case 11673:
    errMsg = pStrInfo_common_HoldTimeSecs;
    break;
  case 11674:
    errMsg = pStrInfo_bgp_KeepAliveTimeSecs;
    break;
  case 11676:
    errMsg = pStrInfo_bgp_RouteFlapMode;
    break;
  case 11677:
    errMsg = pStrInfo_bgp_SuppRessLimit;
    break;
  case 11678:
    errMsg = pStrInfo_bgp_ReuseLimit;
    break;
  case 11679:
    errMsg = pStrInfo_bgp_PenaltyIncrement;
    break;
  case 11680:
    errMsg = pStrInfo_bgp_DeltaTimeSecs;
    break;
  case 11681:
    errMsg = pStrInfo_bgp_FlapMaxTimeSecs;
    break;
  case 11682:
    errMsg = pStrInfo_bgp_DampingFactor;
    break;
  case 11683:
    errMsg = pStrInfo_bgp_ReuseSize;
    break;
  case 11686:
    errMsg = pStrInfo_bgp_SnpaAddr;
    break;
  case 11687:
    errMsg = pStrInfo_bgp_SnpaAddrLenBits;
    break;
  case 11689:
    errMsg = pStrInfo_common_Ip;
    break;
  case 11690:
    errMsg = pStrInfo_common_Ipv6_3;
    break;
  case 11691:
    errMsg = pStrInfo_bgp_Nsap;
    break;
  case 11692:
    errMsg = pStrInfo_bgp_Hdlc;
    break;
  case 11693:
    errMsg = pStrInfo_bgp_Bbn;
    break;
  case 11694:
    errMsg = pStrInfo_bgp_Num802;
    break;
  case 11695:
    errMsg = pStrInfo_bgp_E163;
    break;
  case 11696:
    errMsg = pStrInfo_bgp_E164;
    break;
  case 11697:
    errMsg = pStrInfo_bgp_F169;
    break;
  case 11698:
    errMsg = pStrInfo_bgp_X121;
    break;
  case 11699:
    errMsg = pStrInfo_common_Ipx;
    break;
  case 11700:
    errMsg = pStrInfo_bgp_Appletalk;
    break;
  case 11701:
    errMsg = pStrInfo_bgp_DecnetIv;
    break;
  case 11702:
    errMsg = pStrInfo_bgp_Banyanvines;
    break;
  case 11703:
    errMsg = pStrInfo_common_Ucast;
    break;
  case 11704:
    errMsg = pStrInfo_bgp_Mcast_1;
    break;
  case 11705:
    errMsg = pStrInfo_bgp_UcastMcast;
    break;
  case 11706:
    errMsg = pStrInfo_bgp_Labeldist;
    break;
  case 11708:
    errMsg = pStrInfo_bgp_MultiProto;
    break;
  case 11711:
    errMsg = pStrInfo_bgp_RouteREflect;
    break;
  case 11712:
    errMsg = pStrInfo_common_Community;
    break;
  case 11713:
    errMsg = pStrInfo_bgp_Confederation;
    break;
  case 11756:
    errMsg = pStrInfo_common_Start_1;
    break;
  case 11757:
    errMsg = pStrInfo_bgp_Stop_1;
    break;
  case 11758:
    errMsg = pStrInfo_common_Client;
    break;
  case 11759:
    errMsg = pStrInfo_bgp_NonClient;
    break;
  case 11760:
    errMsg = pStrInfo_common_Null;
    break;
  case 11761:
    errMsg = pStrInfo_common_Simple_1;
    break;
  case 11762:
    errMsg = pStrInfo_common_Encrypt_1;
    break;
  case 11780:
    errMsg = pStrInfo_bgp_DecayDecrement;
    break;
  case 11781:
    errMsg = pStrInfo_bgp_TimeCreated;
    break;
  case 11782:
    errMsg = pStrInfo_bgp_TimeSuppressed;
    break;
  case 11783:
    errMsg = pStrInfo_bgp_EventState;
    break;
  case 11784:
    errMsg = pStrInfo_bgp_PenaltyVal;
    break;
  case 11786:
    errMsg = pStrInfo_common_NextHop;
    break;
  case 11787:
    errMsg = pStrInfo_bgp_VpncosId;
    break;
  case 11795:
    errMsg = pStrInfo_bgp_AsPathSegment;
    break;
  case 11796:
    errMsg = pStrInfo_bgp_AtomicAggregate;
    break;
  case 11797:
    errMsg = pStrInfo_bgp_AggregatorAs;
    break;
  case 11798:
    errMsg = pStrInfo_bgp_AggregatorAddr;
    break;
  case 11799:
    errMsg = pStrInfo_bgp_CalculatedLocalPref;
    break;
  case 11801:
    errMsg = pStrInfo_bgp_Best;
    break;
  case 11806:
    errMsg = pStrInfo_bgp_InternalIn;
    break;
  case 11807:
    errMsg = pStrInfo_bgp_InternalOut;
    break;
  case 11811:
    errMsg = pStrInfo_bgp_AsPath;
    break;
  case 11812:
    errMsg = pStrInfo_bgp_Origin;
    break;
  case 11813:
    errMsg = pStrInfo_bgp_LocalPref;
    break;
  case 11814:
    errMsg = pStrInfo_bgp_MultiExitDisc;
    break;
  case 11815:
    errMsg = pStrInfo_common_Community;
    break;
  case 11816:
    errMsg = pStrInfo_bgp_ClusterNum;
    break;
  case 11817:
    errMsg = pStrInfo_bgp_FlowSpec;
    break;
  case 11818:
    errMsg = pStrInfo_bgp_DestIpPref;
    break;
  case 11819:
    errMsg = pStrInfo_common_NextHop;
    break;
  case 11820:
    errMsg = pStrInfo_bgp_MaxSetupPri;
    break;
  case 11821:
    errMsg = pStrInfo_bgp_MaxHoldPri;
    break;
  case 11822:
    errMsg = pStrInfo_bgp_PolicyString;
    break;
  case 11823:
    errMsg = pStrInfo_bgp_UsrDefined;
    break;
  case 11824:
    errMsg = pStrInfo_bgp_AsPathLenBits;
    break;
  case 11825:
    errMsg = pStrInfo_bgp_ProtoId;
    break;
  case 11826:
    errMsg = pStrInfo_bgp_OspfDestinType;
    break;
  case 11827:
    errMsg = pStrInfo_bgp_Peer;
    break;
  case 11828:
    errMsg = pStrInfo_bgp_AtomicAggregate;
    break;
  case 11829:
    errMsg = pStrInfo_bgp_AggregatorAs;
    break;
  case 11830:
    errMsg = pStrInfo_bgp_AggregatorId;
    break;
  case 11840:
    errMsg = pStrInfo_bgp_MatchType;
    break;
  case 11841:
    errMsg = pStrInfo_bgp_Range_1;
    break;
  case 11842:
    errMsg = pStrInfo_common_Action;
    break;
  case 11847:
    errMsg = pStrInfo_bgp_PolicyIdx;
    break;
  case 11865:
    errMsg = pStrInfo_bgp_Sec;
    break;
  case 11866:
    errMsg = pStrInfo_bgp_Key_Maxlength;
    break;
  case 11915:
    errMsg = pStrInfo_bgp_SendNow;
    break;
  case 11966:
    errMsg = pStrInfo_bgp_Feasible;
    break;
  case 11967:
    errMsg = pStrInfo_bgp_Unfeasible;
    break;
  case 11968:
    errMsg = pStrInfo_common_Send_1;
    break;
  case 11969:
    errMsg = pStrInfo_bgp_Donotsend;
    break;
  case 11976:
    errMsg = pStrInfo_common_Permit_1;
    break;
  case 11977:
    errMsg = pStrInfo_common_Deny_2;
    break;
  case 12003:
    errMsg = pStrInfo_common_Add_1;
    break;
  case 12004:
    errMsg = pStrInfo_bgp_Modify;
    break;
  case 12005:
    errMsg = pStrInfo_common_Del_1;
    break;
  case 12011:
    errMsg = pStrInfo_bgp_Igp;
    break;
  case 12012:
    errMsg = pStrInfo_bgp_Egp;
    break;
  case 12013:
    errMsg = pStrInfo_bgp_Incomplete;
    break;
  case 12040:
    errMsg = pStrInfo_bgp_PrefixLen_4;
    break;
  case 12510:
    errMsg = pStrInfo_bgp_RangeIpAddr;
    break;
  case 12511:
    errMsg = pStrInfo_bgp_RangeBetween;
    break;
  case 12512:
    errMsg = pStrInfo_bgp_RangeGreaterThan;
    break;
  case 12513:
    errMsg = pStrInfo_bgp_RangeLessThan;
    break;
  case 12514:
    errMsg = pStrInfo_bgp_RangeEqualTo;
    break;
  case 12515:
    errMsg = pStrInfo_bgp_MinVal;
    break;
  case 12516:
    errMsg = pStrInfo_bgp_MaxVal;
    break;
  case 12517:
    errMsg = pStrInfo_bgp_Val_2;
    break;
  case 12518:
    errMsg = pStrInfo_bgp_UnknownSelection;
    break;
  case 12522:
    errMsg = pStrInfo_bgp_PolicyRangeCfg;
    break;
  case 12523:
    errMsg = pStrInfo_bgp_PolicyActionCfg;
    break;
  case 12525:
    errMsg = pStrInfo_bgp_Modifier;
    break;
  case 12527:
    errMsg = pStrInfo_bgp_IpAddrVal1;
    break;
  case 12528:
    errMsg = pStrInfo_bgp_IpAddrVal2;
    break;
  case 12529:
    errMsg = pStrInfo_bgp_IntegerVal1;
    break;
  case 12530:
    errMsg = pStrInfo_bgp_IntegerVal2;
    break;
  case 12600:
    errMsg = pStrInfo_bgp_AddrFamilyId;
    break;
  case 12601:
    errMsg = pStrInfo_bgp_SubsequentAddrFamilyId;
    break;
  case 12613:
    errMsg = pStrInfo_common_RtrId_1;
    break;
  case 14061:
    errMsg = pStrInfo_bgp_UnknownAttrs;
    break;
  case 16173:
    errMsg = pStrInfo_common_Metric;
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
  case 16192:
    errMsg = pStrInfo_common_DeflInfoOriginate;
    break;
  case 16193:
    errMsg = pStrInfo_common_DeflMetric;
    break;
  case 16195:
    errMsg = pStrInfo_common_AvailableSrc;
    break;
  case 16197:
    errMsg = pStrInfo_common_Src;
    break;
  case 16229:
    errMsg = pStrInfo_common_CfguredSrc;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebBgpGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebBgpGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebBgpGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1350:
    errMsg = pStrInfo_bgp_Bgp4AddrAggregationCfg;
    break;
  case 1351:
    errMsg = pStrInfo_bgp_Bgp4DampenedPathsTbl;
    break;
  case 1352:
    errMsg = pStrInfo_bgp_AddrFamilyCfg;
    break;
  case 1353:
    errMsg = pStrInfo_bgp_Bgp4GlobalCfg;
    break;
  case 1354:
    errMsg = pStrInfo_bgp_ClusterIdCfg;
    break;
  case 1355:
    errMsg = pStrInfo_bgp_ConfederationIdCfg;
    break;
  case 1356:
    errMsg = pStrInfo_bgp_Bgp4LocalNlriCfg;
    break;
  case 1357:
    errMsg = pStrInfo_bgp_Bgp4LocalOriginatedPathCfg;
    break;
  case 1358:
    errMsg = pStrInfo_bgp_Bgp4PathAttrsTbl;
    break;
  case 1359:
    errMsg = pStrInfo_bgp_Bgp4PeerAddrFamilyCfg;
    break;
  case 1360:
    errMsg = pStrInfo_bgp_Bgp4PeerAuthCfg;
    break;
  case 1361:
    errMsg = pStrInfo_bgp_Bgp4PeerCfg;
    break;
  case 1362:
    errMsg = pStrInfo_bgp_Bgp4PeerStats;
    break;
  case 1363:
    errMsg = pStrInfo_bgp_Bgp4PolicyActionCfg;
    break;
  case 1364:
    errMsg = pStrInfo_bgp_Bgp4PolicyActionTypeSelection;
    break;
  case 1365:
    errMsg = pStrInfo_bgp_Bgp4PolicyCfg;
    break;
  case 1366:
    errMsg = pStrInfo_bgp_Bgp4PolicyRangeCfg;
    break;
  case 1367:
    errMsg = pStrInfo_bgp_Bgp4PolicyRangeTypeSelection;
    break;
  case 1368:
    errMsg = pStrInfo_bgp_Bgp4PolicyCfgSummary;
    break;
  case 1369:
    errMsg = pStrInfo_bgp_Bgp4RouteFlapCfg;
    break;
  case 1370:
    errMsg = pStrInfo_bgp_Bgp4RouteTblSummary;
    break;
  case 1371:
    errMsg = pStrInfo_bgp_Bgp4SnpaCfg;
    break;
  case 1372:
    errMsg = pStrInfo_bgp_Bgp4RouteRedistributionCfg;
    break;
  case 1373:
    errMsg = pStrInfo_bgp_Bgp4RouteRedistributionSummary;
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

L7_char8 *usmWebBgpGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2350:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpaddraggrcfg;
    break;
  case 2351:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpdamppaths;
    break;
  case 2352:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgppolicycfg;
    break;
  case 2353:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpglobalcfg;
    break;
  case 2354:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpglobalcfg;
    break;
  case 2355:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpglobalcfg;
    break;
  case 2356:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpnlricfg;
    break;
  case 2357:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgplocalcfg;
    break;
  case 2358:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgppathattrtable;
    break;
  case 2359:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgppolicycfg;
    break;
  case 2360:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgppeercfg;
    break;
  case 2361:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPeerBgppeercfg;
    break;
  case 2362:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPeerBgppeerparams;
    break;
  case 2363:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicyactionconfig;
    break;
  case 2364:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicyactionselection;
    break;
  case 2365:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicycfg;
    break;
  case 2366:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicyrangeconfig;
    break;
  case 2367:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicyrangeselection;
    break;
  case 2368:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpPolicyBgppolicysumm;
    break;
  case 2369:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpRouteBgprouteflapcfg;
    break;
  case 2370:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpRouteBgproutetable;
    break;
  case 2371:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpsnpacfg;
    break;
  case 2372:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpRtRdstrbtnCfg;
    break;
  case 2373:
    errMsg = pStrInfo_bgp_HtmlLinkBgpHelpBgpBgpRtRdstrbtnSumm;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
