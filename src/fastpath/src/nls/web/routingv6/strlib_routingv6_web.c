/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/web/routingv6/strlib_routingv6_web.c                                                      
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

#include "strlib_routingv6_web.h"

L7_char8 *pStrInfo_routingv6_NoLimit = "%d (No Limit)";
L7_char8 *pStrInfo_routingv6_HtmlFileRoutingLoopbackLoopbackSumm = "/routing/loopback/loopback_summ.html";
L7_char8 *pStrInfo_routingv6_HtmlFileRoutingTunnelTunnel = "/routing/tunnel/tunnel.html";
L7_char8 *pStrInfo_routingv6_HtmlFileRoutingTunnelTunnelSumm = "/routing/tunnel/tunnel_summ.html";
L7_char8 *pStrInfo_routingv6_AdvRtr = "Adv. Router";
L7_char8 *pStrInfo_routingv6_AllRoutes = "All Routes";
L7_char8 *pStrInfo_routingv6_BestRoutesOnly = "Best Routes Only";
L7_char8 *pStrInfo_routingv6_ClrIpv6Neighbors = "Clear IPv6 Neighbors";
L7_char8 *pStrInfo_routingv6_CfguredIpv6Routes = "Configured IPv6 Routes";
L7_char8 *pStrInfo_routingv6_Dhcpv6_1 = "DHCPv6";
L7_char8 *pStrInfo_routingv6_Dhcpv6GlobalCfg = "DHCPv6 Global Configuration";
L7_char8 *pStrInfo_routingv6_Dhcpv6IntfCfg = "DHCPv6 Interface Configuration";
L7_char8 *pStrInfo_routingv6_Dhcpv6PoolCfg = "DHCPv6 Pool Configuration";
L7_char8 *pStrInfo_routingv6_Dhcpv6PoolSummary = "DHCPv6 Pool Summary";
L7_char8 *pStrInfo_routingv6_Dhcpv6SrvrBindingsSummary = "DHCPv6 Server Bindings Summary";
L7_char8 *pStrInfo_routingv6_Dhcpv6Stats = "DHCPv6 Statistics";
L7_char8 *pStrInfo_routingv6_Dhcpv6Stats_1 = "DHCPv6 Stats";
L7_char8 *pStrInfo_routingv6_DnsSrvrAddr = "DNS Server Address";
L7_char8 *pStrInfo_routingv6_Duid = "DUID";
L7_char8 *pStrInfo_routingv6_DatagramsFragmentsCreated = "Datagrams Fragments Created";
L7_char8 *pStrInfo_routingv6_DatagramsLocallyTrasmitted = "Datagrams Locally Trasmitted";
L7_char8 *pStrInfo_routingv6_DeadTime = "Dead Time";
L7_char8 *pStrInfo_routingv6_DeadTimerDueInSecs = "Dead Timer Due in (secs)";
L7_char8 *pStrInfo_routingv6_DeflRouteMetric = "Default Route Metric";
L7_char8 *pStrInfo_routingv6_DelegatedPrefix = "Delegated Prefix";
L7_char8 *pStrInfo_routingv6_DelDomain = "Delete Domain";
L7_char8 *pStrInfo_routingv6_DelHost = "Delete Host";
L7_char8 *pStrInfo_routingv6_DelPool = "Delete Pool";
L7_char8 *pStrInfo_routingv6_DelSrvr = "Delete Server";
L7_char8 *pStrInfo_routingv6_Ipv6Cfg = "IPv6 Global Configuration";
L7_char8 *pStrInfo_routingv6_Ipv6Fwd = "IPv6 Forwarding";
L7_char8 *pStrInfo_routingv6_Ipv6HopLimit_2 = "Hop Limit";
L7_char8 *pStrInfo_routingv6_Ipv6IntfCfg = "IPv6 Interface Configuration";
L7_char8 *pStrInfo_routingv6_Ipv6IntfSummary = "IPv6 Interface Summary";
L7_char8 *pStrInfo_routingv6_Ipv6NeighborTbl = "IPv6 Neighbor Table";
L7_char8 *pStrInfo_routingv6_Ipv6NextHopAddr = "IPv6 Next Hop Address";
L7_char8 *pStrInfo_routingv6_Ipv6PrefixPrefixLen_1 = "IPv6 Prefix/Prefix Length";
L7_char8 *pStrInfo_routingv6_Ipv6RouteEntryCfg = "IPv6 Route Entry Configuration";
L7_char8 *pStrInfo_routingv6_Ipv6RouteEntryCreate = "IPv6 Route Entry Create";
L7_char8 *pStrInfo_routingv6_Ipv6RouteTbl = "IPv6 Route Table";
L7_char8 *pStrInfo_routingv6_Ipv6RtrRoutePrefs = "IPv6 Router Route Preferences";
L7_char8 *pStrInfo_routingv6_Ipv6StaticRoute_1 = "IPv6 Static Route";
L7_char8 *pStrInfo_routingv6_Ipv6Stats_1 = "IPv6 Statistics";
L7_char8 *pStrInfo_routingv6_Ipv6UcastRouting = "IPv6 Unicast Routing";
L7_char8 *pStrInfo_routingv6_IntifId = "IntIf ID";
L7_char8 *pStrErr_routingv6_DomainNameLen = "Invalid Domain Name Length";
L7_char8 *pStrErr_routingv6_Ipv6Addr_2 = "Invalid IPv6 Address";
L7_char8 *pStrErr_routingv6_PoolNameLen = "Invalid Pool Name Length";
L7_char8 *pStrErr_routingv6_InvalidIpV6Pref = "Invalid preference value %u. Setting preference to default value %d";
L7_char8 *pStrErr_routingv6_PrefValThatConflictsWithOspfv3 = "Invalid preference value that conflicts with OSPFv3";
L7_char8 *pStrErr_routingv6_RangeOfValidRangeIsTo = "Invalid range of %s. Valid range is %u to %lu.";
L7_char8 *pStrErr_routingv6_RemoteId = "Invalid remote ID.";
L7_char8 *pStrErr_routingv6_StaticPrefValThatConflictsWithOspfv3 = "Invalid static preference value that conflicts with OSPFv3";
L7_char8 *pStrInfo_routingv6_LinkId = "Link ID";
L7_char8 *pStrInfo_routingv6_MsgsRcvd_1 = "Messages Received:";
L7_char8 *pStrInfo_routingv6_MsgsSent_1 = "Messages Sent:";
L7_char8 *pStrInfo_routingv6_NssaSpecificInfo = "NSSA Specific Information";
L7_char8 *pStrInfo_routingv6_NextHopIpv6Addr = "Next Hop IPv6 Address";
L7_char8 *pStrInfo_routingv6_NoPoolExists = "No Pool Exists";
L7_char8 *pStrInfo_routingv6_NumOfBestRoutes = "Number of Best Routes";
L7_char8 *pStrInfo_routingv6_NumOfRoutes = "Number of Routes";
L7_char8 *pStrInfo_routingv6_Ospfv3AdminMode = "OSPFv3 Admin Mode";
L7_char8 *pStrInfo_routingv6_Ospfv3AreaCfg = "OSPFv3 Area Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3AreaRangeCfg = "OSPFv3 Area Range Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3Cfg = "OSPFv3 Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3ExtType1 = "OSPFv3 Ext Type-1";
L7_char8 *pStrInfo_routingv6_Ospfv3ExtType2 = "OSPFv3 Ext Type-2";
L7_char8 *pStrInfo_routingv6_Ospfv3ExtType1_1 = "OSPFv3 Ext Type1";
L7_char8 *pStrInfo_routingv6_Ospfv3External = "OSPFv3 External";
L7_char8 *pStrInfo_routingv6_Ospfv3ExternalRoutePref = "OSPFv3 External Route Preference";
L7_char8 *pStrInfo_routingv6_Ospfv3Inter = "OSPFv3 Inter";
L7_char8 *pStrInfo_routingv6_Ospfv3InterRoutePref = "OSPFv3 Inter Route Preference";
L7_char8 *pStrInfo_routingv6_Ospfv3IntfCfg = "OSPFv3 Interface Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3IntfStats = "OSPFv3 Interface Statistics";
L7_char8 *pStrInfo_routingv6_Ospfv3Intra = "OSPFv3 Intra";
L7_char8 *pStrInfo_routingv6_Ospfv3IntraRoutePref = "OSPFv3 Intra Route Preference";
L7_char8 *pStrInfo_routingv6_Ospfv3LinkStateDbase = "OSPFv3 Link State Database";
L7_char8 *pStrInfo_routingv6_Ospfv3NssaExtType1 = "OSPFv3 NSSA Ext Type-1";
L7_char8 *pStrInfo_routingv6_Ospfv3NssaExtType2 = "OSPFv3 NSSA Ext Type-2";
L7_char8 *pStrInfo_routingv6_Ospfv3NeighborTbl = "OSPFv3 Neighbor Table";
L7_char8 *pStrInfo_routingv6_Ospfv3Neighbors = "OSPFv3 Neighbors";
L7_char8 *pStrInfo_routingv6_Ospfv3RouteRedistributionCfg = "OSPFv3 Route Redistribution Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3RouteRedistributionSummary = "OSPFv3 Route Redistribution Summary";
L7_char8 *pStrInfo_routingv6_Ospfv3StubAreaSummary = "OSPFv3 Stub Area Summary";
L7_char8 *pStrInfo_routingv6_Ospfv3VirtualLinkAuthCfg = "OSPFv3 Virtual Link Authentication Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3VirtualLinkCfg = "OSPFv3 Virtual Link Configuration";
L7_char8 *pStrInfo_routingv6_Ospfv3VirtualLinkSummary = "OSPFv3 Virtual Link Summary";
L7_char8 *pStrInfo_routingv6_Ospfv3MustBeDsbldBeforeChangingRtrId = "OSPFv3 must be disabled before changing the Router Id.";
L7_char8 *pStrInfo_routingv6_PointToPoint_1 = "Point-to-point";
L7_char8 *pStrInfo_routingv6_PreferLifetime = "Prefer Lifetime";
L7_char8 *pStrInfo_routingv6_PrefixDelegationCfg = "Prefix Delegation Configuration";
L7_char8 *pStrInfo_routingv6_PrefixPrefixLen_1 = "Prefix/Prefix Length";
L7_char8 *pStrInfo_routingv6_RcvdDatagramsDiscardedDutToTruncatedData = "Received Datagrams Discarded Dut To Truncated Data";
L7_char8 *pStrInfo_routingv6_RelayIntf_1 = "Relay Interface";
L7_char8 *pStrInfo_routingv6_RelayOption = "Relay Option";
L7_char8 *pStrInfo_routingv6_RemoteId_1 = "Remote ID";
L7_char8 *pStrInfo_routingv6_RemoteIdSubOption = "Remote-id Sub-option";
L7_char8 *pStrInfo_routingv6_RoutePref = "Route Preference";
L7_char8 *pStrInfo_routingv6_RoutesDisped = "Routes Displayed";
L7_char8 *pStrInfo_routingv6_RoutingShldBeEnbldOnIntfBeforeCfguringMtuVal = "Routing should be enabled on the interface before configuring MTU value";
L7_char8 *pStrInfo_routingv6_RtrOpt = "Rtr Opt.";
L7_char8 *pStrInfo_routingv6_StaticRoutePref = "Static Route Preference";
L7_char8 *pStrErr_routingv6_TblFull = "Table is full for %s";
L7_char8 *pStrInfo_routingv6_TotalDhcpv6PktsSent = "Total DHCPv6 Packets Sent";
L7_char8 *pStrErr_routingv6_InvalidDhcp6Del = "Unable to delete DHCPv6 server on interface %u/%u/%u";
L7_char8 *pStrInfo_routingv6_UnableToGet = "Unable to get: %s";
L7_char8 *pStrErr_routingv6_InvalidIpV6IntfAddr = "Unable to set DHCPv6 interface configuration for interface %u/%u/%u";
L7_char8 *pStrInfo_routingv6_UnableToSetDhcpv6RelayIntfCfgSelectIntfOrEnterDstPrefix = "Unable to set DHCPv6 relay interface configuration. Select interface or enter destination prefix.";
L7_char8 *pStrErr_routingv6_InvalidDhcp6Intf = "Unable to set DHCPv6 server on interface %u/%u/%u";
L7_char8 *pStrInfo_routingv6_UnableToSetMtuOfAnIntf = "Unable to set MTU of an Interface";
L7_char8 *pStrInfo_routingv6_UcastRouting = "Unicast Routing";
L7_char8 *pStrInfo_routingv6_ValidLifetimeShldBeGreaternPreferLifetimeSettingPerferLifetimeEqualToValidLifetime = "Valid Lifetime should be greater then Prefer Lifetime. Setting Perfer Lifetime equal to Valid Lifetime";
L7_char8 *pStrInfo_routingv6_ValidLifetimeShldAlwaysBeGreaternPreferLifetime = "Valid lifetime should always be greater then prefer lifetime.";
L7_char8 *pStrInfo_routingv6_ValIsOutOfRangeForValidRangeIsTo = "Value (%d) is out of range for %s. The valid range is %u to %lu.";
L7_char8 *pStrInfo_routingv6_CantSetModeToNoneDelCfgOnAnIntfToSetModeToNone = "cannot set mode to none. Delete configuration on an interface to set mode to none";
L7_char8 *pStrInfo_routingv6_DelIpv6DhcpPoolAsItIsUsedInCfg = "delete IPv6 DHCP pool as it is used in configuration";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6BindingInfo = "dhcpv6_binding_info.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6GlobalCfg = "dhcpv6_global_config.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6IntfCfg = "dhcpv6_interface_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6PoolCfg = "dhcpv6_pool_config.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6PoolSummary = "dhcpv6_pool_summary.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6PrefixCfg = "dhcpv6_prefix_config.html";
L7_char8 *pStrInfo_routingv6_HtmlFileDhcpv6Stats = "dhcpv6_stats.html";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6neighbor = "href=\"/routingv6/base/ipv6_help.html#ipv6Neighbor\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6globalcfg = "href=\"/routingv6/base/ipv6_help.html#ipv6globalcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6intfcfg = "href=\"/routingv6/base/ipv6_help.html#ipv6intfcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6HelpIpv6intfsummary = "href=\"/routingv6/base/ipv6_help.html#ipv6intfsummary\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6BaseIpv6StatsHelpIpv6statistics = "href=\"/routingv6/base/ipv6_statistics_help.html#ipv6statistics\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6globalcfg = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6globalcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6intfcfg = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6intfcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolbinding = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6poolbinding\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolcfg = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6poolcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6poolsummary = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6poolsummary\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Dhcpv6stats = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#dhcpv6stats\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Dhcpv6HelpDhcpv6Ipv6prefixdeleg = "href=\"/routingv6/dhcpv6/help_dhcpv6.html#ipv6prefixdeleg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Area = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_area\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3AreaRange = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_area_range\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3IntfStats = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_intf_stats\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Intfcfg = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_intfcfg\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Lsdb = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_lsdb\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3NeighborTbl = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_neighbor_table\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3Neighbors = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_neighbors\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3RouteRedistributionSummary = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_route_redistribution_summary\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3StubArea = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3_stub_area\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3info = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospfv3info\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospv3RouteRedistribution = "href=\"/routingv6/ospfv3/help_ospfv3.html#ospv3_route_redistribution\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3VirtLink = "href=\"/routingv6/ospfv3/help_ospfv3.html#virt_link\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3VirtLinkSumm = "href=\"/routingv6/ospfv3/help_ospfv3.html#virt_link_summ\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6configuredroutes = "href=\"/routingv6/routingv6/help_ipv6_route.html#ipv6ConfiguredRoutes\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routeentrycreate = "href=\"/routingv6/routingv6/help_ipv6_route.html#ipv6RouteEntryCreate\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routetable = "href=\"/routingv6/routingv6/help_ipv6_route.html#ipv6RouteTable\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routerrouteprefs = "href=\"/routingv6/routingv6/help_ipv6_route.html#ipv6RouterRoutePrefs\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Ospfv3HelpOspfv3Ospfv3LsdbXUI = "href=\"/help_OSPFv3LSDB.html\"";
L7_char8 *pStrInfo_routingv6_HtmlLinkRoutingv6Routingv6HelpIpv6RouteIpv6routetableXUI = "href=\"/help_IPv6RouteTable.html\"";
L7_char8 *pStrInfo_routingv6_Ipv6AdministrativeMode = "ipv6 administrative mode";
L7_char8 *pStrInfo_routingv6_Ipv6DuplicateAddrDetection = "ipv6 duplicate address detection";
L7_char8 *pStrInfo_routingv6_Ipv6LifetimeIntvl = "ipv6 lifetime interval";
L7_char8 *pStrInfo_routingv6_Ipv6ManagedFlag = "ipv6 managed flag";
L7_char8 *pStrInfo_routingv6_Ipv6Mode_2 = "ipv6 mode";
L7_char8 *pStrInfo_routingv6_Ipv6NeighborSolicitTransmissionIntvl = "ipv6 neighbor solicit transmission interval";
L7_char8 *pStrInfo_routingv6_Ipv6OtherFlag = "ipv6 other flag";
L7_char8 *pStrInfo_routingv6_Ipv6ReachableIntvl = "ipv6 reachable interval";
L7_char8 *pStrInfo_routingv6_Ipv6ReachableTime = "ipv6 reachable time";
L7_char8 *pStrInfo_routingv6_Ipv6RtrAdvertiseIntvl = "ipv6 router advertise interval";
L7_char8 *pStrInfo_routingv6_Ipv6RtrAdvertiseLifetimeIntvl = "ipv6 router advertise lifetime interval";
L7_char8 *pStrInfo_routingv6_Ipv6RtrAdvertiseMaxAdvIntvl = "ipv6 router advertise max adv interval";
L7_char8 *pStrInfo_routingv6_Ipv6SuppressFlag = "ipv6 suppress flag";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6GlobalCfg = "ipv6_global_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6IntfCfg = "ipv6_interface_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6IntfCfgRo = "ipv6_interface_cfg_ro.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6IntfSummary = "ipv6_interface_summary.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6Neighbors = "ipv6_neighbors.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteCfg = "IPv6RoutesConfigured.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteEntryCfg = "ipv6_route_entry_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteEntryCfgRwGlobal = "ipv6_route_entry_cfg_rw_global.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteEntryCfgRwLinklocal = "ipv6_route_entry_cfg_rw_linklocal.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteEntryCfgRwStaticReject = "ipv6_route_entry_cfg_rw_static_rej.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteNbrDetailed = "ipv6_route_nbr_detailed.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteNbrSumm = "ipv6_route_nbr_summ.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RouteTbl = "ipv6_route_table.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6RtrRoutePrefs = "ipv6_router_route_prefs.html";
L7_char8 *pStrInfo_routingv6_HtmlFileIpv6Stats = "ipv6_statistics.html";
L7_char8 *pStrInfo_routingv6_NeighborSolicitTransmissionIntvl = "neighbor solicit transmission interval";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3AreaRangeCfgRo = "ospfv3_area_range_cfg_ro.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3AreaRangeCfgRw = "ospfv3_area_range_cfg_rw.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3AreaRo = "ospfv3_area_ro.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3AreaRw = "ospfv3_area_rw.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3IntfCfg = "ospfv3_interface_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3IntfStats = "ospfv3_interface_stats.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3LsdbSumm = "ospfv3_lsdb_summ.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3RredistributeCfg = "ospfv3_rredistribute_cfg.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3RredistributeCfgCfgured = "ospfv3_rredistribute_cfg_configured.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3RredistributeSumm = "ospfv3_rredistribute_summ.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3StubAreaSummary = "ospfv3_stub_area_summary.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3VirtLinks = "ospfv3_virt_links.html";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3VirtLinksAuth = "ospfv3_virt_links_auth.html";
L7_char8 *pStrInfo_routingv6_PreferLifetime_1 = "prefer lifetime";
L7_char8 *pStrInfo_routingv6_PrefixAttrs = "prefix attributes";
L7_char8 *pStrInfo_routingv6_HtmlFileRtrOspfInfoRo = "router_ospf_info_ro.html";
L7_char8 *pStrInfo_routingv6_HtmlFileRtrOspfv3InfoRo = "router_ospfv3_info_ro.html";
L7_char8 *pStrInfo_routingv6_HtmlFileRtrOspfv3InfoRw = "router_ospfv3_info_rw.html";
L7_char8 *pStrInfo_routingv6_RoutingMode_1 = "routing mode";
L7_char8 *pStrInfo_routingv6_ValidLifetime = "valid lifetime";
L7_char8 *pStrInfo_routingv6_HtmlFileOspfv3VlinkSumm = "ospfv3_vlink_summ.html";

