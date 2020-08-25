/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/qos/oem_qos.c
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
#include "strlib_qos_common.h"
#include "strlib_qos_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebQosGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebQosGetNLS(L7_int32 token)
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
  case 1015:
     errMsg = pStrInfo_common_True2;
     break;
  case 1016:
     errMsg = pStrInfo_common_False2;
     break;
  case 1394: 
     errMsg = pStrInfo_qos_SecondaryCosVal_1;
     break;
  case 1548:
    errMsg = pStrInfo_common_Proto;
    break;
  case 1704:
     errMsg = pStrInfo_qos_ip;
     break;
  case 1792:
    errMsg = pStrInfo_qos_IpAclId;
    break;
  case 1793:
    errMsg = pStrInfo_qos_Rule;
    break;
  case 1795:
    errMsg = pStrInfo_common_Direction;
    break;
  case 1803:
    errMsg = pStrInfo_qos_DstIpMask;
    break;
  case 1804:
    errMsg = pStrInfo_qos_DstL4PortKeyword;
    break;
  case 1805:
    errMsg = pStrInfo_qos_DstL4PortRange;
    break;
  case 1806:
    errMsg = pStrInfo_qos_IpDscp;
    break;
  case 1807:
    errMsg = pStrInfo_qos_IpPrecedence;
    break;
  case 1808:
    errMsg = pStrInfo_qos_TosBits;
    break;
  case 1809:
    errMsg = pStrInfo_qos_Domain_1;
    break;
  case 1810:
    errMsg = pStrInfo_qos_Echo;
    break;
  case 1811:
    errMsg = pStrInfo_common_Ftp;
    break;
  case 1812:
    errMsg = pStrInfo_qos_Ftpdata;
    break;
  case 1813:
    errMsg = pStrInfo_common_Http_1;
    break;
  case 1814:
    errMsg = pStrInfo_qos_Smtp;
    break;
  case 1815:
    errMsg = pStrInfo_common_Snmp_1;
    break;
  case 1816:
    errMsg = pStrInfo_common_Telnet;
    break;
  case 1817:
    errMsg = pStrInfo_common_Tftp_1;
    break;
  case 1818:
    errMsg = pStrInfo_qos_Www;
    break;
  case 1826:
    errMsg = pStrInfo_qos_TosMask;
    break;
  case 1827:
    errMsg = pStrInfo_qos_Icmp;
    break;
  case 1828:
    errMsg = pStrInfo_qos_Tcp;
    break;
  case 1829:
    errMsg = pStrInfo_qos_Udp;
    break;
  case 1832:
    errMsg = pStrInfo_qos_SrcIpAddr;
    break;
  case 1833:
    errMsg = pStrInfo_qos_SrcIpMask;
    break;
  case 1834:
    errMsg = pStrInfo_qos_SrcL4PortRange;
    break;
  case 1835:
    errMsg = pStrInfo_qos_SrcL4PortKeyword;
    break;
  case 1836:
    errMsg = pStrInfo_qos_ProtoKeyword;
    break;
  case 1837:
    errMsg = pStrInfo_qos_ProtoNum_1;
    break;
  case 1843:
    errMsg = pStrInfo_qos_MatchEvery;
    break;
  case 1845:
    errMsg = pStrInfo_qos_PolicyIn;
    break;
  case 1846:
    errMsg = pStrInfo_qos_PolicyOut;
    break;
  case 1849:
    errMsg = pStrInfo_qos_PolicyName_1;
    break;
  case 1852:
    errMsg = pStrInfo_qos_OfferedPkts;
    break;
  case 1853:
    errMsg = pStrInfo_qos_DiscardedPkts;
    break;
  case 1854:
    errMsg = pStrInfo_common_OspfTxPkts;
    break;
  case 1855:
    errMsg = pStrInfo_qos_CounterModeSelector;
    break;
  case 1856:
    errMsg = pStrInfo_qos_Octets;
    break;
  case 1857:
    errMsg = pStrInfo_qos_Pkts;
    break;
  case 1861:
    errMsg = pStrInfo_qos_MbrClasses;
    break;
  case 1862:
    errMsg = pStrInfo_qos_OfferedOctets;
    break;
  case 1863:
    errMsg = pStrInfo_qos_DiscardedOctets;
    break;
  case 1864:
    errMsg = pStrInfo_qos_SentOctets;
    break;
  case 1865:
    errMsg = pStrInfo_qos_TailDroppedOctets;
    break;
  case 1866:
    errMsg = pStrInfo_qos_RandomDroppedOctets;
    break;
  case 1867:
    errMsg = pStrInfo_qos_ShapeDelayedOctets;
    break;
  case 1868:
    errMsg = pStrInfo_qos_TailDroppedPkts;
    break;
  case 1869:
    errMsg = pStrInfo_qos_RandomDroppedPkts;
    break;
  case 1870:
    errMsg = pStrInfo_qos_ShapeDelayedPkts;
    break;
  case 1872:
    errMsg = pStrInfo_common_WsOperatingStatus;
    break;
  case 1874:
    errMsg = pStrInfo_qos_ServiceType;
    break;
  case 1877:
    errMsg = pStrInfo_qos_SrcL4PortNum;
    break;
  case 1878:
    errMsg = pStrInfo_qos_DstL4PortNum;
    break;
  case 2120:
    errMsg = pStrInfo_common_Igmp_1;
    break;
  case 2125:
    errMsg = pStrInfo_qos_DiffservAdminMode;
    break;
  case 2128:
    errMsg = pStrInfo_qos_ClassSelector;
    break;
  case 2129:
    errMsg = pStrInfo_qos_ClassName_1;
    break;
  case 2130:
    errMsg = pStrInfo_qos_ClassType;
    break;
  case 2131:
    errMsg = pStrInfo_qos_ClassMatchSelector;
    break;
  case 2132:
    errMsg = pStrInfo_qos_RefClass;
    break;
  case 2133:
    errMsg = pStrInfo_qos_ClassOfService;
    break;
  case 2134:
    errMsg = pStrInfo_common_DstIpAddr;
    break;
  case 2135:
    errMsg = pStrInfo_qos_DstLayer4Port;
    break;
  case 2136:
    errMsg = pStrInfo_qos_DstMacAddr;
    break;
  case 2137:
    errMsg = pStrInfo_qos_IpDscp;
    break;
  case 2138:
    errMsg = pStrInfo_qos_IpPrecedence;
    break;
  case 2139:
    errMsg = pStrInfo_qos_IpTos;
    break;
  case 2140:
    errMsg = pStrInfo_qos_ProtoKeyword;
    break;
  case 2141:
    errMsg = pStrInfo_qos_SrcIpAddr;
    break;
  case 2142:
    errMsg = pStrInfo_qos_SrcLayer4Port;
    break;
  case 2143:
    errMsg = pStrInfo_qos_SrcMacAddr;
    break;
  case 2144:
    errMsg = pStrInfo_common_VapVlan;
    break;
  case 2146:
     errMsg = pStrInfo_qos_matchCriteira;
     break;
  case 2147:
     errMsg = pStrInfo_qos_matchValues;
     break;
  case 2148:
     errMsg = pStrInfo_qos_matchExcluded;
     break;
  case 2153:
    errMsg = pStrInfo_common_ApProfileRadioScanFrequencyAll;
    break;
  case 2154:
    errMsg = pStrInfo_qos_Any;
    break;
  case 2155:
    errMsg = pStrInfo_qos_IpAcl;
    break;
  case 2169:
    errMsg = pStrInfo_common_Excl;
    break;
  case 2170:
    errMsg = pStrInfo_qos_IpMask;
    break;
  case 2171:
    errMsg = pStrInfo_qos_PortKeyword;
    break;
  case 2172:
    errMsg = pStrInfo_qos_StartPortNum;
    break;
  case 2173:
    errMsg = pStrInfo_qos_EndPortNum;
    break;
  case 2174:
    errMsg = pStrInfo_qos_Domain_1;
    break;
  case 2175:
    errMsg = pStrInfo_qos_Echo;
    break;
  case 2176:
    errMsg = pStrInfo_common_Ftp;
    break;
  case 2177:
    errMsg = pStrInfo_qos_Ftpdata;
    break;
  case 2178:
    errMsg = pStrInfo_common_Http_1;
    break;
  case 2179:
    errMsg = pStrInfo_qos_Smtp;
    break;
  case 2180:
    errMsg = pStrInfo_common_Snmp_1;
    break;
  case 2181:
    errMsg = pStrInfo_common_Telnet;
    break;
  case 2182:
    errMsg = pStrInfo_common_Tftp_1;
    break;
  case 2183:
    errMsg = pStrInfo_qos_Www;
    break;
  case 2186:
    errMsg = pStrInfo_qos_DscpVal;
    break;
  case 2187:
    errMsg = pStrInfo_qos_PrecedenceVal;
    break;
  case 2188:
    errMsg = pStrInfo_qos_TosBits;
    break;
  case 2189:
    errMsg = pStrInfo_qos_TosMask;
    break;
  case 2190:
    errMsg = pStrInfo_common_VlanId_3;
    break;
  case 2191:
    errMsg = pStrInfo_common_Icmp_1;
    break;
  case 2192:
    errMsg = pStrInfo_common_Igmp_2;
    break;
  case 2193:
    errMsg = pStrInfo_common_IpOption;
    break;
  case 2194:
    errMsg = pStrInfo_qos_Tcp_1;
    break;
  case 2195:
    errMsg = pStrInfo_qos_Udp_1;
    break;
  case 2196:
    errMsg = pStrInfo_common_Other;
    break;
  case 2199:
    errMsg = pStrInfo_qos_MibTbl;
    break;
  case 2200:
    errMsg = pStrInfo_qos_CurrentSizeMaxSize;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 8847:
    errMsg = pStrInfo_common_True2;
    break;
  case 8848:
    errMsg = pStrInfo_common_False2;
    break;
  case 10506:
    errMsg = pStrInfo_qos_PolicySelector;
    break;
  case 10507:
    errMsg = pStrInfo_qos_PolicyName_1;
    break;
  case 10508:
    errMsg = pStrInfo_qos_PolicyType_1;
    break;
  case 10511:
    errMsg = pStrInfo_qos_MbrClassList;
    break;
  case 10513:
    errMsg = pStrInfo_qos_MbrClasses;
    break;
  case 10514:
    errMsg = pStrInfo_common_In;
    break;
  case 10515:
    errMsg = pStrInfo_qos_Out;
    break;
  case 10529:
    errMsg = pStrInfo_qos_PolicyAttrSelector_1;
    break;
  case 10532:
    errMsg = pStrInfo_qos_CommittedRateKbps;
    break;
  case 10533:
    errMsg = pStrInfo_qos_CommittedBurstSizeKb;
    break;
  case 10534:
    errMsg = pStrInfo_qos_ExcessBurstSizeKb;
    break;
  case 10535:
    errMsg = pStrInfo_qos_ConformAction;
    break;
  case 10536:
    errMsg = pStrInfo_qos_ConformDscpVal;
    break;
  case 10537:
    errMsg = pStrInfo_qos_ExceedAction;
    break;
  case 10538:
    errMsg = pStrInfo_qos_ExceedDscpVal;
    break;
  case 10539:
    errMsg = pStrInfo_qos_ViolateAction;
    break;
  case 10540:
    errMsg = pStrInfo_qos_ViolateDscpVal;
    break;
  case 10542:
    errMsg = pStrInfo_qos_PolicingStyle;
    break;
  case 10543:
    errMsg = pStrInfo_qos_ConformActionSelector;
    break;
  case 10544:
    errMsg = pStrInfo_qos_ExceedActionSelector;
    break;
  case 10545:
    errMsg = pStrInfo_qos_ViolateAction;
    break;
  case 10546:
    errMsg = pStrInfo_qos_Drop;
    break;
  case 10547:
    errMsg = pStrInfo_qos_MarkIpDscp;
    break;
  case 10548:
    errMsg = pStrInfo_qos_MarkIpPrecedence;
    break;
  case 10549:
    errMsg = pStrInfo_qos_ExceedDscpKeyword;
    break;
  case 10550:
    errMsg = pStrInfo_qos_ConformDscpKeyword;
    break;
  case 10551:
    errMsg = pStrInfo_qos_ConformIpPrecedenceVal;
    break;
  case 10552:
    errMsg = pStrInfo_qos_ExceedIpPrecedenceVal;
    break;
  case 10553:
    errMsg = pStrInfo_qos_ViolateDscpKeyword;
    break;
  case 10554:
    errMsg = pStrInfo_qos_ViolateIpPrecedenceVal;
    break;
  case 10560:
    errMsg = pStrInfo_qos_MarkCos;
    break;
  case 10565:
    errMsg = pStrInfo_qos_DscpKeyword;
    break;
  case 10566:
    errMsg = pStrInfo_qos_DscpVal;
    break;
  case 10567:
    errMsg = pStrInfo_qos_IpPrecedenceVal;
    break;
  case 10568:
    errMsg = pStrInfo_qos_CosVal_1;
    break;
  case 10569:
    errMsg = pStrInfo_qos_SecondaryCosVal_1;
    break;
  case 10571:
    errMsg = pStrInfo_qos_MinThresh_2;
    break;
  case 10572:
    errMsg = pStrInfo_qos_MaxThresh_1;
    break;
  case 10573:
    errMsg = pStrInfo_qos_MaxDropProbability;
    break;
  case 10574:
    errMsg = pStrInfo_qos_SamplingRate;
    break;
  case 10575:
    errMsg = pStrInfo_qos_DecayExponent;
    break;
  case 10578:
    errMsg = pStrInfo_qos_Percent;
    break;
  case 10579:
    errMsg = pStrInfo_qos_NoClassesToAdd;
    break;
  case 10580:
    errMsg = pStrInfo_qos_NoMbrClasses;
    break;
  case 10581:
    errMsg = pStrInfo_qos_Attr;
    break;
  case 10583:
    errMsg = pStrInfo_qos_AttrDetails;
    break;
  case 10590:
    errMsg = pStrInfo_qos_MarkSecondaryCos;
    break;
  case 10592:
    errMsg = pStrInfo_qos_QueueIdVal;
    break;
  case 10600:
    errMsg = pStrInfo_qos_PeakRateKbps;
    break;
  case 10601:
    errMsg = pStrInfo_qos_PeakBurstSizeKb;
    break;
  case 10612:
    errMsg = pStrInfo_qos_ConformCosVal_1;
    break;
  case 10613:
    errMsg = pStrInfo_qos_ExceedCosVal_1;
    break;
  case 10614:
    errMsg = pStrInfo_qos_ViolateCosVal;
    break;
  case 10615:
    errMsg = pStrInfo_qos_ConformCos2Val;
    break;
  case 10616:
    errMsg = pStrInfo_qos_ExceedCos2Val;
    break;
  case 10617:
    errMsg = pStrInfo_qos_ViolateCos2Val;
    break;
  case 11590:
    errMsg = pStrInfo_common_Send;
    break;
  case 11689:
    errMsg = pStrInfo_common_Ip;
    break;
  case 11842:
    errMsg = pStrInfo_common_Action;
    break;
  case 12650:
    errMsg = pStrInfo_qos_NoAclsAreCfgured;
    break;
  case 12663:
    errMsg = pStrInfo_qos_DstL4StartPort;
    break;
  case 12664:
    errMsg = pStrInfo_qos_DstL4EndPort;
    break;
  case 12665:
    errMsg = pStrInfo_qos_SrcL4StartPort;
    break;
  case 12666:
    errMsg = pStrInfo_qos_SrcL4EndPort;
    break;
  case 12671:
    errMsg = pStrInfo_qos_SrcL4Port;
    break;
  case 12672:
    errMsg = pStrInfo_qos_DstL4Port;
    break;
  case 12758:
    errMsg = pStrInfo_qos_AvailableClassList;
    break;
  case 12803:
    errMsg = pStrInfo_qos_PortNum_1;
    break;
  case 12804:
    errMsg = pStrInfo_qos_MacMask_1;
    break;
  case 12824:
    errMsg = pStrInfo_common_HexadecimalValue;
    break;
  case 12850:
    errMsg = pStrInfo_qos_Permit;
    break;
  case 12851:
    errMsg = pStrInfo_common_Deny;
    break;
  case 15065:
    errMsg = pStrInfo_qos_Tbl_1;
    break;
  case 16133:
    errMsg = pStrInfo_common_Other;
    break;
  case 16134:
    errMsg = pStrInfo_qos_RuleId;
    break;
  case 16300:
    errMsg = pStrInfo_common_Dot1pPri;
    break;
  case 16301:
    errMsg = pStrInfo_common_TrafficClass;
    break;
  case 16303:
    errMsg = pStrInfo_qos_Current8021pPriMapping;
    break;
  case 21274:
     errMsg = pStrInfo_qos_EthertypeKey;
     break;
  case 21276:
    errMsg = pStrInfo_qos_VlanIdStart;
    break;
  case 21277:
    errMsg = pStrInfo_qos_VlanIdEnd;
    break;
  case 21281:
    errMsg = pStrInfo_qos_SecondaryVlanIdStart;
    break;
  case 21282:
    errMsg = pStrInfo_qos_SecondaryVlanIdEnd;
    break;
  case 21291:
    errMsg = pStrInfo_qos_SecondaryVlanId;
    break;
  case 22017:
    errMsg = pStrInfo_common_Logging_1;
    break;
  case 22251:
    errMsg = pStrInfo_common_PrefixPrefixlength_2;
    break;
  case 27041:
    errMsg = pStrInfo_qos_ClassLayer3Proto;
    break;
  case 27053:
    errMsg = pStrInfo_qos_DstIpv6Prefix;
    break;
  case 27054:
    errMsg = pStrInfo_qos_SrcIpv6Prefix;
    break;
  case 27055:
    errMsg = pStrInfo_qos_FlowLabel;
    break;
  case 27056:
     errMsg = pStrErr_qos_Inbound;
     break;
  case 27057:
     errMsg = pStrErr_qos_Outbound;
     break;
  case 27058:
     errMsg = pStrErr_qos_ACLType;
     break;
  case 27059:
     errMsg = pStrErr_qos_AssignQueueID;
     break;
  case 27060:
     errMsg = pStrInfo_common_VlanId_1;
     break;
  case 27061:
     errMsg = pStrErr_qos_IPV6ACL;
     break;
  case 27062:
     errMsg = pStrErr_qos_IPV6ACLName;
     break;
  case 27063:
     errMsg = pStrErr_qos_MirrorInterface;
     break;
  case 27064:
     errMsg = pStrErr_qos_RedirectInterface;
     break;
  case 27065:
     errMsg =pStrErr_qos_Protocol;
     break;
  case 27066:
     errMsg = pStrErr_qos_SourcePrefixPrefixLength;
     break;
  case 27067:
     errMsg = pStrErr_qos_DestinationPrefixPrefixLength;
     break;
  case 27068: 
      errMsg = pStrErr_qos_FlowLabel;
      break;
  case 27069:
     errMsg = pStrErr_qos_IPDSCPService;
     break;
  case 27070:
     errMsg = pStrErr_qos_SequenceNumber;
     break;
  case 27071:
     errMsg = pStrInfo_qos_MacAcl_1;
     break;
  case 21273:
     errMsg = pStrInfo_qos_SecondaryVlan;
     break;
  case 21280:
     errMsg =  pStrInfo_common_To_5;
     break;
  case 21283:
    errMsg = pStrInfo_qos_AutoVoip_Mode;
    break;
  case 21284:
    errMsg = pStrInfo_qos_AutoVoip_MinBandWidthKbps;
    break;
  case 21285:
    errMsg = pStrInfo_qos_AutoVoIP_TrafficClass;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebQosGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebQosGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebQosGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1297:
    errMsg = pStrInfo_qos_TrustModeCfg;
    break;
  case 1298:
    errMsg = pStrInfo_qos_IpPrecedenceMappingCfg;
    break;
  case 1299:
    errMsg = pStrInfo_qos_IpDscpMappingCfg;
    break;
  case 1300:
    errMsg = pStrInfo_qos_IpAclCfg;
    break;
  case 1301:
    errMsg = pStrInfo_qos_IpAclRuleCfg;
    break;
  case 1302:
    errMsg = pStrInfo_qos_AclServiceTypeCfg;
    break;
  case 1303:
    errMsg = pStrInfo_qos_IpAclSummary;
    break;
  case 1309:
    errMsg = pStrInfo_qos_DiffservCfg;
    break;
  case 1310:
    errMsg = pStrInfo_qos_DiffservClassCfg;
    break;
  case 1311:
    errMsg = pStrInfo_qos_DiffservClassSummary;
    break;
  case 1312:
    errMsg = pStrInfo_qos_ClassOfService;
    break;
  case 1313:
    errMsg = pStrInfo_common_DstIpAddr;
    break;
  case 1314:
    errMsg = pStrInfo_qos_DstLayer4Port;
    break;
  case 1315:
    errMsg = pStrInfo_qos_DstMacAddr;
    break;
  case 1316:
    errMsg = pStrInfo_qos_IpDscp;
    break;
  case 1317:
    errMsg = pStrInfo_qos_IpPrecedence;
    break;
  case 1318:
    errMsg = pStrInfo_qos_IpTos;
    break;
  case 1319:
    errMsg = pStrInfo_common_Proto;
    break;
  case 1320:
    errMsg = pStrInfo_qos_RefClass;
    break;
  case 1321:
    errMsg = pStrInfo_qos_SrcIpAddr;
    break;
  case 1322:
    errMsg = pStrInfo_qos_SrcLayer4Port;
    break;
  case 1323:
    errMsg = pStrInfo_qos_SrcMacAddr;
    break;
  case 1324:
    errMsg = pStrInfo_common_VapVlan;
    break;
  case 1325:
    errMsg = pStrInfo_qos_DiffservPolicyCfg;
    break;
  case 1326:
    errMsg = pStrInfo_qos_DiffservPolicySummary;
    break;
  case 1327:
    errMsg = pStrInfo_qos_DiffservPolicyClassDefinition;
    break;
  case 1328:
    errMsg = pStrInfo_qos_DiffservPolicyAttrSummary;
    break;
  case 1329:
    errMsg = pStrInfo_qos_DiffservServiceCfg;
    break;
  case 1330:
    errMsg = pStrInfo_qos_DiffservServiceSummary;
    break;
  case 1331:
    errMsg = pStrInfo_qos_DiffservServiceStats;
    break;
  case 1332:
    errMsg = pStrInfo_qos_DiffservServiceDetailedStats;
    break;
  case 1333:
    errMsg = pStrInfo_qos_MarkIpDscp;
    break;
  case 1334:
    errMsg = pStrInfo_qos_MarkCos;
    break;
  case 1335:
    errMsg = pStrInfo_qos_MarkIpPrecedence;
    break;
  case 1336:
    errMsg = pStrInfo_qos_PolicingAttrs;
    break;
  case 1337:
    errMsg = pStrInfo_qos_PolicingCfg;
    break;
  case 1338:
    errMsg = pStrInfo_common_Bandwidth;
    break;
  case 1339:
    errMsg = pStrInfo_qos_Expedite;
    break;
  case 1340:
    errMsg = pStrInfo_qos_RandomDrop;
    break;
  case 1341:
    errMsg = pStrInfo_qos_ShapeAverage;
    break;
  case 1342:
    errMsg = pStrInfo_qos_ShapePeak;
    break;
  case 1344:
    errMsg = pStrInfo_qos_CosIntfCfg;
    break;
  case 1345:
    errMsg = pStrInfo_qos_CosIntfQueueCfg;
    break;
  case 1346:
    errMsg = pStrInfo_qos_CosIntfQueueDropPrecedenceCfg;
    break;
  case 1347:
    errMsg = pStrInfo_qos_CosIntfQueueStatus;
    break;
  case 1348:
    errMsg = pStrInfo_qos_CosIntfQueueDropPrecedenceStatus;
    break;
  case 1349:
    errMsg = pStrInfo_qos_IntfOrVlanBasedAclSSummary;
    break;
  case 1385:
    errMsg = pStrInfo_qos_MacAclCfg;
    break;
  case 1386:
    errMsg = pStrInfo_qos_MacAclSummary;
    break;
  case 1387:
    errMsg = pStrInfo_qos_MacAclRuleCfg;
    break;
  case 1388:
    errMsg = pStrInfo_qos_AclIntfCfg;
    break;
  case 1389:
    errMsg = pStrInfo_qos_VlanBasedAclCfg;
    break;
  case 1391:
    errMsg = pStrInfo_qos_MarkSecondaryCos;
    break;
  case 1392:
    errMsg = pStrInfo_qos_RedirectIntf;
    break;
  case 1393:
    errMsg = pStrInfo_qos_AsSignQueue;
    break;
  case 1394:
    errMsg = pStrInfo_qos_DropPolicyAttrs;
    break;
  case 1395:
    errMsg = pStrInfo_qos_SecondaryClassOfService;
    break;
  case 1396:
    errMsg = pStrInfo_qos_SecondaryVlan;
    break;
  case 1397:
    errMsg = pStrInfo_qos_EtherType;
    break;
  case 1398:
    errMsg = pStrInfo_qos_MirrorIntf;
    break;
  case 1900:
    errMsg = pStrInfo_qos_Ipv6AclCfg;
    break;
  case 1901:
    errMsg = pStrInfo_qos_Ipv6AclSummary;
    break;
  case 1902:
    errMsg = pStrInfo_qos_Ipv6AclRuleCfg;
    break;
  case 1903:
    errMsg = pStrInfo_qos_DiffservSrcPrefixCfg;
    break;
  case 1904:
    errMsg = pStrInfo_qos_DiffservDstPrefixCfg;
    break;
  case 1905:
    errMsg = pStrInfo_qos_FlowLabelCfg;
    break;
  case 1906:
    errMsg = pStrInfo_qos_AutoVoIPCfg;
    break;
  case 1907:
    errMsg = pStrInfo_qos_AutoVoIPSummary;
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

L7_char8 *usmWebQosGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2297:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosTrustModeCfg;
    break;
  case 2298:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIpPrecMapCfg;
    break;
  case 2299:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIpDscpMapCfg;
    break;
  case 2300:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclAclcfg;
    break;
  case 2301:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclAclrulecfg;
    break;
  case 2302:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclAclrulecfg;
    break;
  case 2303:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclAclsum;
    break;
  case 2309:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservDiffservCfg;
    break;
  case 2310:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfg;
    break;
  case 2311:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClasssumm;
    break;
  case 2312:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleCos;
    break;
  case 2313:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleDstip;
    break;
  case 2314:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleDstl4port;
    break;
  case 2315:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleDstmac;
    break;
  case 2316:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleIpdscp;
    break;
  case 2317:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleIpprecedence;
    break;
  case 2318:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleIptos;
    break;
  case 2319:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleProto;
    break;
  case 2320:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleRefclass;
    break;
  case 2321:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleSrcip;
    break;
  case 2322:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleSrcl4port;
    break;
  case 2323:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleSrcmac;
    break;
  case 2324:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleVlan;
    break;
  case 2325:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicyCfg;
    break;
  case 2326:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicysumm;
    break;
  case 2327:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicyDef;
    break;
  case 2328:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicyattrsumm;
    break;
  case 2329:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservServiceServcfg;
    break;
  case 2330:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservServiceServsumm;
    break;
  case 2331:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservServiceServstats;
    break;
  case 2332:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservServiceServstatsdet;
    break;
  case 2333:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyMarkIpDscp;
    break;
  case 2334:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyMarkCos;
    break;
  case 2335:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyMarkIpPrec;
    break;
  case 2336:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicingAttrs;
    break;
  case 2337:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyPolicingCfg;
    break;
  case 2338:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyBandwidth;
    break;
  case 2339:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyExpedite;
    break;
  case 2340:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyRandomDrop;
    break;
  case 2341:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyShapeAverage;
    break;
  case 2342:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyShapePeak;
    break;
  case 2344:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIntfCfg;
    break;
  case 2345:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIntfQueueCfg;
    break;
  case 2346:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIntfQueueDropprecCfg;
    break;
  case 2347:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIntfQueueStatus;
    break;
  case 2348:
    errMsg = pStrInfo_qos_HtmlLinkQosCosHelpCosCosIntfQueueDropprecStatus;
    break;
  case 2349:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclAclintifvlansumm;
    break;
  case 2385:
    errMsg = pStrInfo_qos_HtmlLinkQosAclMacAclHelpMacAclMacAclCfg;
    break;
  case 2386:
    errMsg = pStrInfo_qos_HtmlLinkQosAclMacAclHelpMacAclMacAclSumm;
    break;
  case 2387:
    errMsg = pStrInfo_qos_HtmlLinkQosAclMacAclHelpMacAclMacAclRuleCfg;
    break;
  case 2388:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpIpMacAclIpMacAclCfg;
    break;
  case 2389:
    errMsg = pStrInfo_qos_HtmlLinkQosAclHelpAclVlanAclCfg;
    break;
  case 2391:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyMarkSecondaryCos;
    break;
  case 2392:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyRedirectIntf;
    break;
  case 2393:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyAssignQueue;
    break;
  case 2394:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyDropPolicingAttrs;
    break;
  case 2395:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleCos2;
    break;
  case 2396:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleVlan2;
    break;
  case 2397:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleEthertyp;
    break;
  case 2398:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservPolicyMirrorIntf;
    break;
  case 2700:
    errMsg = pStrInfo_qos_HtmlLinkQosAclIpv6AclHelpIpv6AclIpv6aclcfg;
    break;
  case 2701:
    errMsg = pStrInfo_qos_HtmlLinkQosAclIpv6AclHelpIpv6AclIpv6aclrulecfg;
    break;
  case 2702:
    errMsg = pStrInfo_qos_HtmlLinkQosAclIpv6AclHelpIpv6AclIpv6aclsum;
    break;
  case 2703:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleDestPrefix;
    break;
  case 2704:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleSrcPrefix;
    break;
  case 2705:
    errMsg = pStrInfo_qos_HtmlLinkQosDiffservHelpDiffservClassClassCfgRuleFlowLabel;
    break;
  case 2706:
    errMsg = pStrInfo_qos_HtmlLinkQosAutoVoIPHelpAutoVoIPCfg;
    break;
  case 2707:
    errMsg = pStrInfo_qos_HtmlLinkQosAutoVoIPHelpAutoVoIPSumm;
    break;
  case 2708:
    errMsg = pStrInfo_qos_HtmlLinkXuiQosDiffservHelpDiffservPolicyPolicyattrsumm;
    break;
  case 2709:
    errMsg = pStrInfo_qos_HtmlLinkXuiQosAclHelpAclVlanAclCfg;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
