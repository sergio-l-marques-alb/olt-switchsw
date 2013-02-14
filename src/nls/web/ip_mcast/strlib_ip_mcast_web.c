/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/web/ip_mcast/strlib_ip_mcast_web.c                                                      
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

#include "strlib_ip_mcast_web.h"

L7_char8 *pStrInfo_ipmcast_AddEntryToMcastBoundaryTbl = "Add the Entry to the Mcast Boundary table";
L7_char8 *pStrInfo_ipmcast_AdminModeForIpv6 = "Admin Mode";
L7_char8 *pStrInfo_ipmcast_AdminScopeBoundary = "Admin Scope Boundary";
L7_char8 *pStrInfo_ipmcast_BsrBorderIpv4 = "BSR Border";
L7_char8 *pStrInfo_ipmcast_BsrBorderIpv6 = "BSR Border";
L7_char8 *pStrInfo_ipmcast_CbsrHashMaskLen = "CBSR Hash Mask Length";
L7_char8 *pStrInfo_ipmcast_CbsrPref = "CBSR Preference";
L7_char8 *pStrInfo_ipmcast_CrpPref = "CRP Preference";
L7_char8 *pStrInfo_ipmcast_ClrRoute = "Clear Route";
L7_char8 *pStrInfo_ipmcast_Compatibility = "Compatibility";
L7_char8 *pStrInfo_ipmcast_CompBsrAddr = "Component BSR Address";
L7_char8 *pStrInfo_ipmcast_CompBsrExpiryTimeHhMmSs = "Component BSR Expiry Time (hh:mm:ss)";
L7_char8 *pStrInfo_ipmcast_CompCrpHoldTimeHhMmSs = "Component CRP Hold Time (hh:mm:ss)";
L7_char8 *pStrInfo_ipmcast_CompIdx = "Component Index";
L7_char8 *pStrInfo_ipmcast_CfgOfStaticRpEntryFailed = "Configuration of the Static RP entry Failed.";
L7_char8 *pStrInfo_ipmcast_CreateBoundary = "Create Boundary";
L7_char8 *pStrInfo_ipmcast_CreateStaticRoute = "Create Static Route";
L7_char8 *pStrInfo_ipmcast_DelMem = "DEL_MEM";
L7_char8 *pStrInfo_ipmcast_DrPriIpv4 = "DR Priority";
L7_char8 *pStrInfo_ipmcast_DrPriIpv6 = "DR Priority";
L7_char8 *pStrInfo_ipmcast_DvmrpCfgSummary = "DVMRP Configuration Summary";
L7_char8 *pStrInfo_ipmcast_DvmrpGlobalCfg = "DVMRP Global Configuration";
L7_char8 *pStrInfo_ipmcast_DvmrpIntfCfg = "DVMRP Interface Configuration";
L7_char8 *pStrInfo_ipmcast_DvmrpNextHopSummary = "DVMRP Next Hop Summary";
L7_char8 *pStrInfo_ipmcast_DvmrpPruneSummary = "DVMRP Prune Summary";
L7_char8 *pStrInfo_ipmcast_DvmrpRouteSummary = "DVMRP Route Summary";
L7_char8 *pStrInfo_ipmcast_DataThreshRateKbpsIpv4 = "Data Threshold Rate(Kbps)";
L7_char8 *pStrInfo_ipmcast_DataThreshRateKbpsIpv6 = "Data Threshold Rate(Kbps)";
L7_char8 *pStrErr_ipmcast_FailedToClrIgmpProxyIntfStats = "Failed to clear IGMP Proxy Interface Statistics";
L7_char8 *pStrInfo_ipmcast_Flags = "Flags";
L7_char8 *pStrInfo_ipmcast_GrpCompatibilityMode = "Group Compatibility Mode";
L7_char8 *pStrInfo_ipmcast_GrpIp = "Group IP";
L7_char8 *pStrInfo_ipmcast_HelloIntvlSecsIpv4 = "Hello Interval (secs)";
L7_char8 *pStrInfo_ipmcast_HelloIntvlSecsIpv6 = "Hello Interval (secs)";
L7_char8 *pStrInfo_ipmcast_HoldTimeHhMmSs = "Hold Time (hh:mm:ss)";
L7_char8 *pStrInfo_ipmcast_IdlMem = "IDL_MEM";
L7_char8 *pStrInfo_ipmcast_IgmpCacheInfo = "IGMP Cache Information";
L7_char8 *pStrInfo_ipmcast_IgmpCfgSummary = "IGMP Configuration Summary";
L7_char8 *pStrInfo_ipmcast_IgmpGlobalCfg = "IGMP Global Configuration";
L7_char8 *pStrInfo_ipmcast_IgmpIntfCfg = "IGMP Interface Configuration";
L7_char8 *pStrInfo_ipmcast_IgmpIntfDetailedMbrshipInfo_1 = "IGMP Interface Detailed Membership Info";
L7_char8 *pStrInfo_ipmcast_IgmpProxyCfgSummary = "IGMP Proxy Configuration Summary";
L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfCfg = "IGMP Proxy Interface Configuration";
L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfo = "IGMP Proxy Interface Membership Info";
L7_char8 *pStrInfo_ipmcast_IgmpProxyIntfMbrshipInfoDetailed = "IGMP Proxy Interface Membership Info Detailed";
L7_char8 *pStrInfo_ipmcast_IgmpProxyCfguredOnIntf_1 = "IGMP Proxy configured on this interface";
L7_char8 *pStrInfo_ipmcast_IgmpProxyNotOperational = "IGMP Proxy not operational";
L7_char8 *pStrInfo_ipmcast_IgmpNotIsEnbldOnRtr = "IGMP not is enabled on the router";
L7_char8 *pStrInfo_ipmcast_IncomingIntf = "Incoming Interface";
L7_char8 *pStrInfo_ipmcast_IntfHelloIntvlForIpv4 = "Interface Hello Interval";
L7_char8 *pStrInfo_ipmcast_IntfHelloIntvlForIpv6 = "Interface Hello Interval";
L7_char8 *pStrInfo_ipmcast_IntfModeForIpv4 = "Interface Mode";
L7_char8 *pStrInfo_ipmcast_IntfModeForIpv6 = "Interface Mode";
L7_char8 *pStrInfo_ipmcast_IntfNeighbors = "Interface Neighbors";
L7_char8 *pStrInfo_ipmcast_IntfParamsIpv4 = "Interface Parameters";
L7_char8 *pStrInfo_ipmcast_IntfParamsIpv6 = "Interface Parameters";
L7_char8 *pStrErr_ipmcast_GrpAddrSpecified = "Invalid Group Address specified";
L7_char8 *pStrErr_ipmcast_GrpMaskSpecified = "Invalid Group Mask specified";
L7_char8 *pStrErr_ipmcast_IpAddrSpecified = "Invalid IP Address specified";
L7_char8 *pStrErr_ipmcast_Input_3 = "Invalid Input";
L7_char8 *pStrErr_ipmcast_ValToJoinPruneIntvl = "Invalid Value to Join/Prune Interval";
L7_char8 *pStrInfo_ipmcast_InvlaidGrpMaskSpecified = "Invlaid Group Mask specified";
L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecsIpv4 = "Join Prune Interval (secs)";
L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecsIpv6 = "Join Prune Interval (secs)";
L7_char8 *pStrInfo_ipmcast_JoinPruneIntvlSecs_1 = "Join/Prune Interval (secs)";
L7_char8 *pStrInfo_ipmcast_MrouteEntryS = "MRoute Entry(s)";
L7_char8 *pStrInfo_ipmcast_MaxAddrEntries = "Maximum Address Entries";
L7_char8 *pStrInfo_ipmcast_McastAdminBoundaryCfg = "Multicast Admin Boundary Configuration";
L7_char8 *pStrInfo_ipmcast_McastAdminBoundarySummary = "Multicast Admin Boundary Summary";
L7_char8 *pStrInfo_ipmcast_McastFwdIsNotEnbld = "Multicast Forwarding is not enabled";
L7_char8 *pStrInfo_ipmcast_McastGlobalCfg = "Multicast Global Configuration";
L7_char8 *pStrInfo_ipmcast_McastIntfCfg = "Multicast Interface Configuration";
L7_char8 *pStrInfo_ipmcast_McastMrouteSummary = "Multicast MRoute Table";
L7_char8 *pStrInfo_ipmcast_McastStaticRoutesCfg = "Multicast Static Routes Configuration";
L7_char8 *pStrInfo_ipmcast_McastStaticRoutesSummary = "Multicast Static Routes Summary";
L7_char8 *pStrInfo_ipmcast_Na = "NA";
L7_char8 *pStrInfo_ipmcast_NonMem = "NON_MEM";
L7_char8 *pStrInfo_ipmcast_NeighborIp = "Neighbor IP";
L7_char8 *pStrInfo_ipmcast_NeighborParams = "Neighbor Parameters";
L7_char8 *pStrInfo_ipmcast_NetMask = "Net Mask";
L7_char8 *pStrInfo_ipmcast_NextHopIntf = "Next Hop Interface";
L7_char8 *pStrInfo_ipmcast_NoIgmpCacheInfoAvailable = "No IGMP Cache Information Available.";
L7_char8 *pStrInfo_ipmcast_NoIgmpProxyIntfAvailable = "No IGMP Proxy Interface Available";
L7_char8 *pStrErr_ipmcast_NumOfPktsForWhichGrpNotFound = "Number of Packets For Which Group Not Found";
L7_char8 *pStrErr_ipmcast_NumOfPktsForWhichSrcNotFound = "Number of Packets For Which Source Not Found";
L7_char8 *pStrInfo_ipmcast_NumOfSrcs = "Number of Sources";
L7_char8 *pStrInfo_ipmcast_OutgoingIntfs = "Outgoing Interfaces";
L7_char8 *pStrInfo_ipmcast_PimDmGlobalCfg = "PIM-DM Global Configuration";
L7_char8 *pStrInfo_ipmcast_PimDmIntfCfg = "PIM-DM Interface Configuration";
L7_char8 *pStrInfo_ipmcast_PimDmIntfSummary = "PIM-DM Interface Summary";
L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpSummary = "PIM-SM Candidate RP Summary";
L7_char8 *pStrInfo_ipmcast_PimSmCompSummary = "PIM-SM Component Summary";
L7_char8 *pStrInfo_ipmcast_PimSmGlobalCfg = "PIM-SM Global Configuration";
L7_char8 *pStrInfo_ipmcast_PimSmGlobalStatus = "PIM-SM Global Status";
L7_char8 *pStrInfo_ipmcast_PimSmIntfCfg = "PIM-SM Interface Configuration";
L7_char8 *pStrInfo_ipmcast_PimSmIntfSummary = "PIM-SM Interface Summary";
L7_char8 *pStrInfo_ipmcast_PimSmRpSetSummary = "PIM-SM RP Set Summary";
L7_char8 *pStrInfo_ipmcast_PimSmStaticRpCfg = "PIM-SM Static RP Configuration";

L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfg = "PIM-SM Candidate RP Configuration";

L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfgIPv4 = "PIM-SM Candidate RP Configuration";
L7_char8 *pStrInfo_ipmcast_PimSmCandidateRpCfgIPv6 = "PIM-SM Candidate RP Configuration";

L7_char8 *pStrInfo_ipmcast_staticRPIPV4Info = "Static RP Configuration";
L7_char8 *pStrInfo_ipmcast_staticRPIPV6Info = "Static RP Configuration";

L7_char8 *pStrInfo_ipmcast_RPAddressIPV4Info = "RP Address";
L7_char8 *pStrInfo_ipmcast_RPAddressIPV6Info = "RP Address";

L7_char8 *pStrInfo_ipmcast_GroupAddressIPV4Info = "Group Address";
L7_char8 *pStrInfo_ipmcast_GroupAddressIPV6Info = "Group Address";

L7_char8 *pStrInfo_ipmcast_GroupAddressMaskIPV4Info = "Group Mask";

L7_char8 *pStrInfo_ipmcast_HashMaskLengthIPv4 = "Hash Mask Length";
L7_char8 *pStrInfo_ipmcast_HashMaskLengthIPv6 = "Hash Mask Length";

L7_char8 *pStrInfo_ipmcast_BSRCandidateSummaryIPv4 = "BSR Candidate Summary";
L7_char8 *pStrInfo_ipmcast_BSRCandidateSummaryIPv6 = "BSR Candidate Summary";


L7_char8 *pStrInfo_ipmcast_PriorityIPv4 = "Priority";
L7_char8 *pStrInfo_ipmcast_PriorityIPv6 = "Priority";
L7_char8 *pStrInfo_ipmcast_PIMSMV4NotEnabled = "PIM-SM not configured";
L7_char8 *pStrInfo_ipmcast_PIMSMV6NotEnabled = "PIM-SM not configured";

L7_char8 *pStrInfo_ipmcast_ScopeIPv6 = "Scope";

L7_char8 *pStrInfo_ipmcast_OverrideInfo = "Override";
L7_char8 *pStrInfo_ipmcast_CountMessageInfo = "The static RP could not be created. Count of Static RPs is reached maximum.";
L7_char8 *pStrInfo_ipmcast_RPAddressInfo = "RP Address";
L7_char8 *pStrInfo_ipmcast_DeleteInfo = "Delete";
L7_char8 *pStrInfo_ipmcast_PimSmBSRCandidateCfg = "PIM-SM BSR Candidate Configuration";
L7_char8 *pStrInfo_ipmcast_PimSmBSRCandidateSummary = "PIM-SM BSR Candidate Summary";

L7_char8 *pStrInfo_ipmcast_PimSmBSRAddressIPv4 = "BSR Address";
L7_char8 *pStrInfo_ipmcast_PimSmBSRAddressIPv6 = "BSR Address";

L7_char8 *pStrInfo_ipmcast_PimSmBSRPriorityIPv4 = "BSR Priority";
L7_char8 *pStrInfo_ipmcast_PimSmBSRPriorityIPv6 = "BSR Priority";

L7_char8 *pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv4 = "BSR Hash Mask Length";
L7_char8 *pStrInfo_ipmcast_PimSmBSRHashMaskLengthIPv6 = "BSR Hash Mask Length";

L7_char8 *pStrInfo_ipmcast_PimSmBSRNextBootStrap = "Next bootstrap Message(hh:mm:ss)";

L7_char8 *pStrInfo_ipmcast_PimSmBSRNextCandidateAPAdv = "Next Candidate RP Advertisement(hh:mm:ss)";


L7_char8 *pStrInfo_ipmcast_ProxyIntfStats = "Proxy Interface Statistics";
L7_char8 *pStrInfo_ipmcast_QueriesRcvd = "Queries Received";
L7_char8 *pStrInfo_ipmcast_RpfNeighbor = "RPF Neighbor";
L7_char8 *pStrInfo_ipmcast_RegisterThreshRateKbpsIpv4 = "Register Threshold Rate(Kbps)";
L7_char8 *pStrInfo_ipmcast_RegisterThreshRateKbpsIpv6 = "Register Threshold Rate(Kbps)";
L7_char8 *pStrInfo_ipmcast_RemovalOfStaticRpEntryFailed = "Removal of the Static RP entry Failed.";
L7_char8 *pStrInfo_ipmcast_SsmIpv4 = "SSM";
L7_char8 *pStrInfo_ipmcast_SsmIpv6 = "SSM";

L7_char8 *pStrInfo_ipmcast_SsmConfigIpv4 = "SSM Configuration";
L7_char8 *pStrInfo_ipmcast_SsmConfigIpv6 = "SSM Configuration";

L7_char8 *pStrInfo_ipmcast_SsmConfig = "SSM Range Configuration";

L7_char8 *pStrInfo_ipmcast_SsmRangeFullIPv4 = "SSM Range Table Full";
L7_char8 *pStrInfo_ipmcast_SsmRangeFullIPv6 = "SSM Range Table Full";

L7_char8 *pStrInfo_ipmcast_SsmGroupAddPref = "Group Address/Prefix Length";

L7_char8 *pStrInfo_ipmcast_SsmGrpAddrIpv4 = "SSM Group Address";
L7_char8 *pStrInfo_ipmcast_SsmGrpAddrIpv6 = "SSM Group Address";
L7_char8 *pStrInfo_ipmcast_SsmGrpMaskIpv4 = "SSM Group Mask";
L7_char8 *pStrInfo_ipmcast_SsmGrpMaskIpv6 = "SSM Group Mask";
L7_char8 *pStrInfo_ipmcast_StaticEntry = "IPv4 Static RP entry";
L7_char8 *pStrInfo_ipmcast_SlotPort = "Slot.Port";
L7_char8 *pStrInfo_ipmcast_SomeOtherMcastRoutingProtoIsCurrentlyCfgured = "Some other Multicast Routing Protocol is currently configured";
L7_char8 *pStrInfo_ipmcast_SrcFilterMode = "Source Filter Mode";
L7_char8 *pStrInfo_ipmcast_SrcHosts = "Source Hosts";
L7_char8 *pStrInfo_ipmcast_SrcIp = "Source IP";
L7_char8 *pStrInfo_ipmcast_StaticMrouteEntry = "Static MRoute Entry";
L7_char8 *pStrInfo_ipmcast_StaticMrouteEntryAlreadyExists = "Static mroute entry already exists";
L7_char8 *pStrInfo_ipmcast_TtlThresh = "TTL Threshold";
L7_char8 *pStrInfo_ipmcast_TblEntryCount = "Table Entry Count";
L7_char8 *pStrInfo_ipmcast_TblHighestEntryCount = "Table Highest Entry Count";
L7_char8 *pStrInfo_ipmcast_TblMaxEntryCount = "Table Maximum Entry Count";
L7_char8 *pStrInfo_ipmcast_UnableToAddStaticMrouteEntry = "Unable to Add Static MRoute Entry";
L7_char8 *pStrInfo_ipmcast_UnableToClrMrouteEntries = "Unable to Clear MRoute Entries";
L7_char8 *pStrInfo_ipmcast_UnableToFetchOneOfStaticRouteParams = "Unable to fetch one of the static route parameters";
L7_char8 *pStrInfo_ipmcast_UpstreamNeighbor = "Upstream Neighbor";
L7_char8 *pStrInfo_ipmcast_Ver1HostTimerSecs = "Version 1 Host Timer (hh:mm:ss)";
L7_char8 *pStrInfo_ipmcast_Ver1QuerierTimeout = "Version 1 Querier Timeout";
L7_char8 *pStrInfo_ipmcast_Ver2HostTimerSecs = "Version 2 Host Timer (hh:mm:ss)";
L7_char8 *pStrInfo_ipmcast_Ver2QuerierTimeout = "Version 2 Querier Timeout";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpGlobalCfgRw = "dvmrp_global_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpIntfCfgRo = "dvmrp_interface_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpIntfCfgRw = "dvmrp_interface_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpNexthopCfgRo = "dvmrp_nexthop_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpPruneCfgRo = "dvmrp_prune_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileDvmrpRouteCfgRo = "dvmrp_route_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpglobalcfg = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrpglobalcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfcfg = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrpintfcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpintfparams = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrpintfparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrpnexthopparams = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrpnexthopparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrppruneparams = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrppruneparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastDvmrpHelpDvmrpDvmrprouteparams = "href=\"/ip_mcast/dvmrp/help_dvmrp.html#dvmrprouteparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpcacheinfo = "href=\"/ip_mcast/igmp/help_igmp.html#igmpcacheinfo\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpglobalcfg = "href=\"/ip_mcast/igmp/help_igmp.html#igmpglobalcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfg = "href=\"/ip_mcast/igmp/help_igmp.html#igmpintfcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpIgmpintfcfgparams = "href=\"/ip_mcast/igmp/help_igmp.html#igmpintfcfgparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpHelpIgmpMbrshipinfo = "href=\"/ip_mcast/igmp/help_igmp.html#membershipinfo\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyCacheinfo = "href=\"/ip_mcast/igmp/proxy/help_igmp_proxy.html#cacheinfo\"";

L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmbsrcandidate = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#bsrcandidate\"";

L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfg = "href=\"/ip_mcast/igmp/proxy/help_igmp_proxy.html#igmpproxyintfcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyIgmpproxyintfcfgparams = "href=\"/ip_mcast/igmp/proxy/help_igmp_proxy.html#igmpproxyintfcfgparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastIgmpProxyHelpIgmpProxyMbrshipinfo = "href=\"/ip_mcast/igmp/proxy/help_igmp_proxy.html#membershipinfo\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalcfg = "href=\"/ip_mcast/mcast/help_mcast.html#mcastglobalcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastglobalparams = "href=\"/ip_mcast/mcast/help_mcast.html#mcastglobalparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfcfg = "href=\"/ip_mcast/mcast/help_mcast.html#mcastintfcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastintfparams = "href=\"/ip_mcast/mcast/help_mcast.html#mcastintfparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastmroutecfg = "href=\"/ip_mcast/mcast/help_mcast.html#mcastmroutecfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopecfg = "href=\"/ip_mcast/mcast/help_mcast.html#mcastscopecfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcastscopeparams = "href=\"/ip_mcast/mcast/help_mcast.html#mcastscopeparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticcfg = "href=\"/ip_mcast/mcast/help_mcast.html#mcaststaticcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastMcastHelpMcastMcaststaticparams = "href=\"/ip_mcast/mcast/help_mcast.html#mcaststaticparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfcfg = "href=\"/ip_mcast/pim_dm/help_pim.html#pimdmintfcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimdmintfparams = "href=\"/ip_mcast/pim_dm/help_pim.html#pimdmintfparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimDmHelpPimPimglobalcfg = "href=\"/ip_mcast/pim_dm/help_pim.html#pimglobalcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcandidateparams = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmcandidateparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcfg = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmcomponentparams = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmcomponentparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifcfg = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmifcfg\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmifparams = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmifparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmparams = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmrpsetparams = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmrpsetparams\"";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmstaticrp = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmstaticrp\"";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpCacheRo = "igmp_cache_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpGlobalCfgRw = "igmp_global_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIfMbrshipInfo = "igmp_if_membership_info.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIntfCfgRo = "igmp_interface_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpIntfCfgRw = "igmp_interface_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyCacheRo = "igmp_proxy_cache_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIfMbrshipInfo = "igmp_proxy_if_membership_info.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIntfCfg = "igmp_proxy_interface_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileIgmpProxyIntfCfgRo = "igmp_proxy_interface_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastBoundaryCfg = "mcast_boundary_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastBoundaryCfgRo = "mcast_boundary_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastGlobalCfg = "mcast_global_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastIntfCfg = "mcast_interface_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastMrouteCfg = "mcast_mroute_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastMroutev6Cfg = "mcast_mroute_v6_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastStaticRoutesCfg = "mcast_static_routes_cfg.html";
L7_char8 *pStrInfo_ipmcast_HtmlFileMcastStaticRoutesShow = "mcast_static_routes_show.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmGlobalCfgRw = "pim_dm_global_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmIntfCfgRo = "pim_dm_interface_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimDmIntfCfgRw = "pim_dm_interface_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmCandidaterpShow = "pim_sm_candidaterp_show.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmCompShow = "pim_sm_component_show.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmGlobalCfgRo = "pim_sm_global_cfg_ro.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmGlobalCfgRw = "pim_sm_global_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmIntfCfgRo = "pim_sm_interface_cfg_ro.html";

L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmbsrcandidateRo = "pim_sm_bsr_candidate_ro.html";

L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmIntfCfgRw = "pim_sm_interface_cfg_rw.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmRpsetShow = "pim_sm_rpset_show.html";
L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmSsmCfg = "pim_sm_ssm_cfg_rw.html";

L7_char8 *pStrInfo_ipmcast_HtmlFilePimSmStaticrpCfg = "pim_sm_staticrp_cfg.html";

L7_char8 *pStrInfo_ipmcast_V_3 = "v";
L7_char8 *pStrErr_ipmcast_PimSmRpCandidateDeletion = "Disable RP Candidate.";
L7_char8 *pStrErr_ipmcast_PimSmRpCandidateCreation = "Enable RP Candidate.";
L7_char8 *pStrErr_ipmcast_PimSmInvalidInterface = "Invalid Interface";
L7_char8 *pStrErr_ipmcast_PimSmInvalidHashMask = "Invalid Hash Mask";
L7_char8 *pStrErr_ipmcast_PimSmInvalidPriority = "Invalid Priority";
L7_char8 *pStrErr_ipmcast_PimSmBSRCandidate = "BSR Candidate";
L7_char8 *pStrErr_ipmcast_PimSmInvalidScope = "Invalid Scope";
L7_char8 *pStrInfo_ipmcast_HtmlLinkIpMcastPimSmHelpPimSmPimsmSsmcfg = "href=\"/ip_mcast/pim_sm/help_pim_sm.html#pimsmcomponentssm\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmbsrcandidate ="href=\"/ip_mcast6/pim_sm/help_pim_sm.html#pimsmcomponentparams\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcast6PimSmHelpPimSmPimsmrpsetparams="href=\"/ip_mcast6/pim_sm/help_pim_sm.html#pimsmrpsetparams\"";
