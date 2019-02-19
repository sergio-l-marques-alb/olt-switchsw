/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/cli/ip_mcast6/strlib_ip_mcast6_cli.c                                                      
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

#include "strlib_ip_mcast6_cli.h"

L7_char8 *pStrInfo_ipmcast6_S = "%-6ss";
L7_char8 *pStrInfo_ipmcast6_Range0to25 = "<0-25>";
L7_char8 *pStrInfo_ipmcast6_Range0to32 = "<0-32>";
L7_char8 *pStrInfo_ipmcast6_Range3to15 = "<3-15>";
L7_char8 *pStrInfo_ipmcast6_Range10to3600 = "<10-3600>";
L7_char8 *pStrInfo_ipmcast6_GrpAddr = "<group-address>";
L7_char8 *pStrInfo_ipmcast6_SrcAddr = "<source-address>";
L7_char8 *pStrInfo_ipmcast6_GrpAddrPrefixlength = "<group-address/prefixlength>";
L7_char8 *pStrInfo_ipmcast6_SrcAddrPrefixlength = "<source-address/prefixlength>";
L7_char8 *pStrInfo_ipmcast6_RpAddr = "<rp-address>";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorderError = "BSR Border Set Error.";
L7_char8 *pStrInfo_ipmcast6_MldIntf = "Failed: MLD is configured on this interface.";
L7_char8 *pStrInfo_ipmcast6_MldProxyCfguredAlready = "MLD Proxy already configured";
L7_char8 *pStrInfo_ipmcast6_MldNotEnbld = "Enable MLD Admin Mode for MLD Proxy to become operational.";
L7_char8 *pStrErr_ipmcast6_ShowMldProxyIntfOprError = "MLD Proxy non-operational.";
L7_char8 *pStrInfo_ipmcast6_ShowMldProxyNotEnbld = "MLD Proxy not enabled.";
L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMld = "Clear the IPv6 MLD Information.";
L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMldTraffic = "Clear the IPv6 MLD Traffic Counters.";
L7_char8 *pStrInfo_ipmcast6_Ipv6ClrMldCounters = "Clear the IPv6 MLD interface Counters.";
L7_char8 *pStrInfo_ipmcast6_CfgRtrPimDm = "Configure Ipv6 PIM-DM parameters.";
L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6 = "Configure PIM-SM6 parameters.";
L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6_1 = "Configure SSM Range for PIM-SM6.";
L7_char8 *pStrInfo_ipmcast6_IpPimDmHelloIntvl = "Configure hello interval for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldProxy = "Configure IGMP Proxy parameters.";
L7_char8 *pStrInfo_ipmcast6_MldProxyResetStatus = "Reset MLD Proxy's host interface status parameters.";
L7_char8 *pStrInfo_ipmcast6_MldProxyUnSolicitedIntvl = "Configure MLD Proxy unsolicited report interval.";
L7_char8 *pStrInfo_ipmcast6_MldVersion = "Configure the MLD version for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryCount = "Configure the Last Member Query Count for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldLastMbrQuery = "Configure the Last Member Query Interval for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldQueryMaxRespTime = "Configure the Query Max Response Time for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldRobustness = "Configure the Robustness for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldQueryInterval = "Configure the query interval for the interface.";
L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6SptThresh = "Configure the data threshold rate for PIM-SM6.";
L7_char8 *pStrErr_ipmcast6_CfgRtrPimSm6RegisterRateLimit = "Configure the register threshold rate for PIM-SM 6.";
L7_char8 *pStrInfo_ipmcast6_CfgIpPimSm6StaticRp = "Creates/Deletes a static RP for the PIM-SM6 router.";
L7_char8 *pStrInfo_ipmcast6_CfgRtrPimSm6_2 = "Defines the SSM range access list to FF3x::/32.";
L7_char8 *pStrInfo_ipmcast6_DsblPimDm = "Disable/Enable IPv6 PIMDM.";
L7_char8 *pStrInfo_ipmcast6_ShowMldGrps = "Display MLD Group Info.";
L7_char8 *pStrInfo_ipmcast6_ShowRtrMld = "Display MLD Router Info.";
L7_char8 *pStrInfo_ipmcast6_ShowMldProxy = "Display MLD Proxy Info.";
L7_char8 *pStrInfo_ipmcast6_ShowRtrMldProxyIntf = "Display MLD Proxy information for a particular interface.";
L7_char8 *pStrInfo_ipmcast6_ShowIpPimDm = "Display IPv6 PIM-DM information.";
L7_char8 *pStrInfo_ipmcast6_ShowIpPimSm6 = "Display IPv6 PIM-SM information.";
L7_char8 *pStrInfo_ipmcast6_ShowMldIntf = "Displays MLD interface Info.";
L7_char8 *pStrInfo_ipmcast6_ShowMldTraffic = "Displays MLD traffic Info.";
L7_char8 *pStrInfo_ipmcast6_MldGrpAddr = "Enter Group Address Info.";
L7_char8 *pStrInfo_ipmcast6_MldMode = "Enables/Disables IPv6 MLD Mode.";
L7_char8 *pStrInfo_ipmcast6_MldVersionVal = "Enter MLD version.";
L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryCountVal = "Enter Last Member Query  count.";
L7_char8 *pStrInfo_ipmcast6_MldLastMbrQueryInt = "Enter Last Member Query  interval in milli-seconds.";
L7_char8 *pStrInfo_ipmcast6_CfgMldQueryIntvl = "Enter Query interval in seconds.";
L7_char8 *pStrInfo_ipmcast6_CfgMldQueryMaxRespIntvl = "Enter Max Query-Response interval in milli-seconds.";
L7_char8 *pStrInfo_ipmcast6_CfgMldRobustness = "Enter Robustness.";
L7_char8 *pStrInfo_ipmcast6_CfgIpPimSm6Scope = "Enter the scope for the interface.";
L7_char8 *pStrInfo_ipmcast6_MldIntfNumGet = "MLD Interface Number Get Failed.";
L7_char8 *pStrInfo_ipmcast6_NumNeighborAddrHhMmSsHhMmSs = "Interface Neighbor Addr             hh:mm:ss  hh:mm:ss";
L7_char8 *pStrErr_ipmcast6_MustInitPim_1 = "PIM-DM must first be initialized for the switch. \r\nUse Global Config Mode Command: ipv6 pimdm";
L7_char8 *pStrErr_ipmcast6_MustInitPimSm6 = "PIM-SM must first be initialized for the switch. \r\nUse Global Config Mode Command: ipv6 pimsm";
L7_char8 *pStrInfo_ipmcast6_RpType = "RP                       Type";
L7_char8 *pStrInfo_ipmcast6_SlotUpTimeExpiryTime = "                                    Up Time   Expiry Time";
L7_char8 *pStrErr_ipmcast6_PimSm6BsrCandidateScope = "The BSR Scope is invalid.";
L7_char8 *pStrInfo_ipmcast6_SrcIpSrcMaskRpfAddrMetricIntf = "Source IP       RPF Address     Interface Preference";
L7_char8 *pStrErr_ipmcast6_PimSm6PrefixLen = "The Prefix Length is invalid.";
L7_char8 *pStrErr_ipmcast6_MldUnableClrCounters = "Unable to clear MLD Counters.";
L7_char8 *pStrErr_ipmcast6_MldUnableTrafficClrCounters = "Unable to clear Traffic Counters.";
L7_char8 *pStrErr_ipmcast6_MldVersion = " Use 'ipv6 mld version <version>'";
L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryCount_1 = "Use 'ipv6 mld last-member-query-count <count>'";
L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryIntvl = "Use 'ipv6 mld last-member-query-interval <seconds>'";
L7_char8 *pStrErr_ipmcast6_MldQueryIntvl = "Use 'ipv6 mld query-interval <seconds>'";
L7_char8 *pStrErr_ipmcast6_MldQueryMaxRespTime_1 = "Use 'ipv6 mld query-max-response-time <seconds>'";
L7_char8 *pStrErr_ipmcast6_MldRobustness_1 = "Use 'ipv6 mld robustness <robustness>'";
L7_char8 *pStrErr_ipmcast6_DsblIpMldGlobal = "Use 'ipv6 mld router'";
L7_char8 *pStrErr_ipmcast6_MldProxyIntf = "Use 'ipv6 mld-proxy'";
L7_char8 *pStrErr_ipmcast6_MldProxyIntfNo = "Use 'no ipv6 mld-proxy'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDmHelloIntvl = "Use 'ipv6 pimdm hello-interval <seconds>'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDmMode = "Use 'ipv6 pimdm mode'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDm = "Use 'ipv6 pimdm'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorder = "Use 'ipv6 pimsm bsr-border'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidate = "Use 'ipv6 pimsm bsr-candidate vlan <vlan-id> [hash-mask-length] [priority]'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidate_Fp = "Use 'ipv6 pimsm bsr-candidate interface <slot/port>\r\n[hash-mask-length][priority]'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6DrPri = "Use 'ipv6 pimsm dr-priority <0-2147483647>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6HelloIntvl = "Use 'ipv6 pimsm hello-interval <0-18000>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6JoinPruneIntvl = "Use 'ipv6 pimsm join-prune-interval <0-18000>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RegisterThresh = "Use 'ipv6 pimsm register-threshold <0-2000>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpAddr = "Use 'ipv6 pimsm rp-address <rp-address> <group-address> <group-mask>\r\n[override]'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidate = "Use 'ipv6 pimsm rp-candidate vlan <vlan-id> <group-addres/prefixLength>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidate_Fp = "Use 'ipv6 pimsm rp-candidate interface <slot/port> <group-addres/prefixLength>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6SptThresh = "Use 'ipv6 pimsm spt-threshold <0-2000>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6 = "Use 'ipv6 pimsm ssm {default|<group-addres/prefixLength>}'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6_1 = "Use 'ipv6 pimsm'";
L7_char8 *pStrErr_ipmcast6_MldVersionNo = " Use 'no ipv6 mld version <version>'";
L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryCountNo = "Use 'no ipv6 mld last-member-query-count'";
L7_char8 *pStrErr_ipmcast6_MldLastMbrQueryIntvlNo = "Use 'no ipv6 mld last-member-query-interval'";
L7_char8 *pStrErr_ipmcast6_MldQueryIntvlNo = "Use 'no ipv6 mld query-interval'";
L7_char8 *pStrErr_ipmcast6_MldQueryMaxRespTimeNo = "Use 'no ipv6 mld query-max-response-time'";
L7_char8 *pStrErr_ipmcast6_MldRobustnessNo = "Use 'no ipv6 mld robustness'";
L7_char8 *pStrErr_ipmcast6_DsblIpMldGlobalNo = "Use 'no ipv6 mld router'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDmHelloIntvlNo = "Use 'no ipv6 pimdm hello-interval'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDmModeNo = "Use 'no ipv6 pimdm mode'";
L7_char8 *pStrErr_ipmcast6_Ipv6PimDmNo = "Use 'no ipv6 pimdm'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrBorderNo = "Use 'no ipv6 pimsm bsr-border'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidateNo = "Use 'no ipv6 pimsm bsr-candidate vlan <vlan-id>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6BsrCandidateNo_Fp = "Use 'no ipv6 pimsm bsr-candidate interface <slot/port>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6DrPriNo = "Use 'no ipv6 pimsm dr-priority'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6HelloIntvlNo = "Use 'no ipv6 pimsm hello-interval'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6JoinPruneIntvlNo = "Use 'no ipv6 pimsm join-prune-interval'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RegisterThreshNo = "Use 'no ipv6 pimsm register-threshold'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpAddrNo = "Use 'no ipv6 pimsm rp-address <rp-address> <group-address> <group-mask>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidateNo = "Use 'no ipv6 pimsm rp-candidate vlan <vlan-id> <group-addres/prefixLength>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6RpCandidateNo_Fp = "Use 'no ipv6 pimsm rp-candidate interface <slot/port> <group-addres/prefixLength>'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6SptThreshNo = "Use 'no ipv6 pimsm spt-threshold'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6_2 = "Use 'no ipv6 pimsm ssm {default|<group-address/prefixLength>}'";
L7_char8 *pStrErr_ipmcast6_IpPimSm6No = "Use 'no ipv6 pimsm'";
L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRoute = "Use 'ipv6 mroute <src-ip-addr/prefix-length> <rpf-ip-addr> [slot/port]\r\n<preference>'";
L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRouteVlan = "Use 'ipv6 mroute <src-ip-addr/prefix-length> <rpf-ip-addr> [interface vlan\r\n<vlan-id>]<preference>'";
L7_char8 *pStrErr_ipmcast6_Ipv6McastStaticRouteNo = "Use 'no ipv6 mroute <src-ip-addr/prefix-length>'";
L7_char8 *pStrErr_ipmcast6_ShowIpMldGrps = "Use 'show ipv6 mld groups {<group-address>|vlan <vlan-id>}'";
L7_char8 *pStrErr_ipmcast6_ShowIpMldIntf = "Use 'show ipv6 mld interface {all|vlan <vlan-id>}'";
L7_char8 *pStrErr_ipmcast6_ShowIpMldTraffic = "Use 'show ipv6 mld traffic'";
L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxy = "Use 'show ipv6 mld-proxy'";
L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxyIntf = "Use 'show ipv6 mld-proxy interface'";
L7_char8 *pStrErr_ipmcast6_ShowIpv6MldProxyGrps = "Use 'show ipv6 mld-proxy groups [detail]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_3 = "Use 'show ipv6 pimdm interface [{all|vlan <vlan-id>}]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_4 = "Use 'show ipv6 pimdm neighbor [{all|interface vlan <vlan-id>}]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimDm_5 = "Use 'show ipv6 pimdm'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Bsr = "Use 'show ipv6 pimsm bsr'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6IntfStats = "Use 'show ipv6 pimsm interface [{vlan <vlan-id>}]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Neighbor = "Use 'show ipv6 pimsm neighbor [{all|interface vlan <vlan-id>}]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6Rp = "Use 'show ipv6 pimsm rp mapping [rp-address/prefixLength]'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6RpHash = "Use 'show ipv6 pimsm rphash <group-address/prefixLength>'";
L7_char8 *pStrErr_ipmcast6_ShowIpPimSm6_1 = "Use 'show ipv6 pimsm'";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmBsrCandidateIntf = "ipv6 pimsm bsr-candidate %s %d %d";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmBsrCandidateIntf_Fp = "ipv6 pimsm bsr-candidate interface %s %d %d";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRegisterThresh = "ipv6 pimsm register-threshold";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpAddr = "ipv6 pimsm rp-address %s %s/%d";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpAddroverride  = "ipv6 pimsm rp-address %s %s/%d  override";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpCandidateIntf = "ipv6 pimsm rp-candidate %s %s/%d";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmRpCandidateIntf_Fp = "ipv6 pimsm rp-candidate interface %s %s/%d";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmSptThresh = "ipv6 pimsm spt-threshold";
L7_char8 *pStrInfo_ipmcast6_Ipv6PimsmSsm = "ipv6 pimsm ssm %s/%d";
L7_char8 *pStrInfo_ipmcast6_Mroute = "mroute";
L7_char8 *pStrInfo_ipmcast6_McastStaticroute = "ipv6 mroute %s/%d %s %d";
L7_char8 *pStrInfo_ipmcast6_McastStaticrouteIntfFp = "ipv6 mroute %s/%d %s %s %d";
L7_char8 *pStrInfo_ipmcast6_McastStaticrouteIntf = "ipv6 mroute %s/%d %s interface %s %d";
L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteStatic_1 = "show ipv6 mroute static";
L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMroute = "Display IPV6 multicast routing table information.";
L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteDetailed = "Display the IPV6 multicast routing table details.";
L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteSummary = "Display the IPV6 multicast routing table summary.";
L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteGrp = "Display multicast routing table entries for specified group IPV6 address.";
L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteStatic = "Use 'show ipv6 mroute static [<source-ip-addr>]'";
L7_char8 *pStrInfo_ipmcast6_CfgRtrMcastBoundaryIp = "Enter the group IPV6 address.";
L7_char8 *pStrInfo_ipmcast6_ShowIpMcastMrouteSrc = "Display multicast routing table entries for specified source IPV6 address.";
L7_char8 *pStrInfo_ipmcast6_ShowRtrMcastMrouteDetailedSrcType = "Enter the source IPV6 address.";
L7_char8 *pStrInfo_ipmcast6_GrpSrcListExpiryTime = "Source Address    ExpiryTime ";
L7_char8 *pStrInfo_ipmcast6_GrpSrcListExpiryTime_1 = "Source Address  ExpiryTime";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldGroups = "show ipv6 mld groups %s ";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldGroupsVlan = "show ipv6 mld groups vlan %s ";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldIntfAll = "show ipv6 mld interface all";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MldProxyGroups = "show ipv6 mld-proxy groups %s ";
L7_char8 *pStrInfo_ipmcast6_Range0to126 = "<0-128>";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteDetail = "show ipv6 mroute detail";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSummary = "show ipv6 mroute summary";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteGroupDetail = "show ipv6 mroute group %s detail";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteGroupSummary = "show ipv6 mroute group %s summary";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSourceDetail = "show ipv6 mroute source %s detail";
L7_char8 *pStrInfo_ipmcast6_ShowIpv6MrouteSourceSummary = "show ipv6 mroute source %s summary";
L7_char8 *pStrErr_ipmcast6_PimSmSSMRangeInvalidScopeId = "The Node or Site-local scopeIds (1 and 5) are not supported.";


L7_char8 *pStrInfo_ipmcast_DebugMld = "Configure MLD debug flags.";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6 = "Configure PIMDMv6 debug flags.";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6 = "Configure PIMSMv6 debug flags.";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6 = "Configure MDATAv6 debug flags.";

L7_char8 *pStrInfo_ipmcast_DebugMldPkt = "Turn on MLD Packet debug trace.";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6Pkt = "Turn on PIMDMv6 Packet debug trace.";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6Pkt = "Turn on PIMSMv6 Packet debug trace.";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6Pkt = "Turn on MDATAv6 Packet debug trace.";

L7_char8 *pStrInfo_ipmcast_DebugMldRxPkt = "Turn on MLD Receive Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6RxPkt = "Turn on PIMDMv6 Receive Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6RxPkt = "Turn on PIMSMv6 Receive Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6RxPkt = "Turn on MDATAv6 Receive Packet debug trace";

L7_char8 *pStrInfo_ipmcast_DebugMldTxPkt = "Turn on MLD Transmit Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6TxPkt = "Turn on PIMDMv6 Transmit Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6TxPkt = "Turn on PIMSMv6 Transmit Packet debug trace";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6TxPkt = "Turn on MDATAv6 Transmit Packet debug trace";

L7_char8 *pStrErr_ipmcast_DebugMldPkt = "Use 'debug ipv6 mld packet.";
L7_char8 *pStrErr_ipmcast_DebugPimdmV6Pkt = "Use 'debug ipv6 pimdm packet.";
L7_char8 *pStrErr_ipmcast_DebugPimsmV6Pkt = "Use 'debug ipv6 pimsm packet.";
L7_char8 *pStrErr_ipmcast_DebugMcacheV6Pkt = "Use 'debug ipv6 mcache packet.";

L7_char8 *pStrErr_ipmcast_DebugMldPktNo = "Use 'no debug ipv6 mld packet.";
L7_char8 *pStrErr_ipmcast_DebugPimdmV6PktNo = "Use 'no debug ipv6 pimdm packet.";
L7_char8 *pStrErr_ipmcast_DebugPimsmV6PktNo = "Use 'no debug ipv6 pimsm packet.";
L7_char8 *pStrErr_ipmcast_DebugMcacheV6PktNo = "Use 'no debug ipv6 mcache packet.";

L7_char8 *pStrInfo_ipmcast_DebugMldPktEnbld = "MLD Debug Packet Trace Enabled.";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6PktEnbld = "PIMDMv6 Debug Packet Trace Enabled.";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6PktEnbld = "PIMSMv6 Debug Packet Trace Enabled.";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6PktEnbld = "MCACHEv6 Debug Packet Trace Enabled.";

L7_char8 *pStrInfo_ipmcast_DebugMldPktDsbld = "MLD Debug Packet Trace Disabled.";
L7_char8 *pStrInfo_ipmcast_DebugPimdmV6PktDsbld = "PIMDMv6 Debug Packet Trace Disabled.";
L7_char8 *pStrInfo_ipmcast_DebugPimsmV6PktDsbld = "PIMSMv6 Debug Packet Trace Disabled.";
L7_char8 *pStrInfo_ipmcast_DebugMcacheV6PktDsbld = "MCACHEv6 Debug Packet Trace Disabled.";
