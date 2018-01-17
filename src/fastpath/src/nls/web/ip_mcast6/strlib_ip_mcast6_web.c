/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/web/ip_mcast6/strlib_ip_mcast6_web.c                                                      
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

#include "strlib_ip_mcast6_web.h"

L7_char8 *pStrInfo_ipmcast6_GlobalMld = "Global MLD";
L7_char8 *pStrInfo_common_AdminModeForIpv6 = "Admin Mode";
L7_char8 *pStrInfo_ipmcast6_AdminMode = "Admin Mode";
L7_char8 *pStrInfo_ipmcast6_Ipv6RtrMld = "IPv6 Router MLD";
L7_char8 *pStrInfo_ipmcast6_LastMbrQueryIntvl = "Last Member Query Interval";
L7_char8 *pStrInfo_ipmcast6_MldGlobalCfg = "MLD Global Configuration";
L7_char8 *pStrInfo_ipmcast6_MldGrpsSummary = "MLD Groups Summary";
L7_char8 *pStrInfo_ipmcast6_MldIntfCfg = "MLD Interface Configuration";
L7_char8 *pStrInfo_ipmcast6_MldIntfSummary = "MLD Interface Summary";
L7_char8 *pStrInfo_ipmcast6_MldTraffic = "MLD Traffic";
L7_char8 *pStrInfo_ipmcast6_StaticEntry = "IPv6 Static RP entry";
L7_char8 *pStrInfo_ipmcast6_InvalidPimsmIpv6Address = "Invalid PIMSM IPv6 Address specified";
L7_char8 *pStrInfo_ipmcast6_ErrorDeleteStaticRPEntry = "ERROR: Could not delete IPv6 Static RP entry";
L7_char8 *pStrInfo_ipmcast6_InvalidPimsmIpv6GrpAddress = "Invalid PIMSM IPv6 Group Address specified";
L7_char8 *pStrInfo_ipmcast6_NoMldGrpsAvailable = "No MLD Groups available";
L7_char8 *pStrInfo_ipmcast6_QueryMaxRespTime = "Query Max Response Time";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldGlobalcfg = "href=\"/ip_mcast6/mld/help_mld.html#globalcfg\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldGrpsummary = "href=\"/ip_mcast6/mld/help_mld.html#groupsummary\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldIntfcfg = "href=\"/ip_mcast6/mld/help_mld.html#interfacecfg\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldIntfsummary = "href=\"/ip_mcast6/mld/help_mld.html#interfacesummary\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldHelpMldTraffic = "href=\"/ip_mcast6/mld/help_mld.html#traffic\"";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldGlobalCfg = "mld_global_cfg.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldGrpSummary = "mld_group_summary.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldIntfCfg = "mld_interface_cfg.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldIntfSummary = "mld_interface_summary.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldTraffic = "mld_traffic.html";
L7_char8 *pStrInfo_ipmcast6_HtmlSourceAddress = "Source Address (Expiry Time)";
L7_char8 *pStrErr_routingv6_Ipv6PimsmAddr = "Invalid IPv6 PIM-SM Address";
L7_char8 *pStrErr_routingv6_Ipv6Mask = "Invalid IPv6 Mask";
L7_char8 *pStrErr_routingv6_Ipv6GrpAddr = "Invalid Group Address specified";
L7_char8 *pStrErr_routingv6_Ipv6SSMRange = "Could not delete SSM Range";
L7_char8 *pStrInfo_ipmcast_MldNotIsEnbldOnRtr = "MLD not is enabled on the router";
L7_char8 *pStrErr_ipmcast6_FailedToClrMldProxyIntfStats = "Failed to clear MLD Proxy Interface Statistics";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldProxyCacheRo = "mld_proxy_cache_ro.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldProxyIfMbrshipInfo = "mld_proxy_if_membership_info.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldProxyIntfCfg = "mld_proxy_interface_cfg.html";
L7_char8 *pStrInfo_ipmcast6_HtmlFileMldProxyIntfCfgRo = "mld_proxy_interface_cfg_ro.html";

L7_char8 *pStrInfo_ipmcast6_MldProxyCfgSummary = "MLD Proxy Configuration Summary";
L7_char8 *pStrInfo_ipmcast6_MldProxyIntfCfg = "MLD Proxy Interface Configuration";
L7_char8 *pStrInfo_ipmcast6_MldProxyIntfMbrshipInfo = "MLD Proxy Interface Membership Info";
L7_char8 *pStrInfo_ipmcast6_MldProxyIntfMbrshipInfoDetailed = "MLD Proxy Interface Membership Info Detailed";
L7_char8 *pStrInfo_ipmcast6_MldProxyCfguredOnIntf_1 = "MLD Proxy configured on this interface";

L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyCacheinfo = "href=\"/ip_mcast6/mld/proxy/help_mld_proxy.html#cacheinfo\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyIgmpproxyintfcfg = "href=\"/ip_mcast6/mld/proxy/help_mld_proxy.html#mldproxyintfcfg\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyIgmpproxyintfcfgparams = "href=\"/ip_mcast6/mld/proxy/help_mld_proxy.html#mldproxyintfcfgparams\"";
L7_char8 *pStrInfo_ipmcast6_HtmlLinkIpMcastMldProxyHelpMldProxyMbrshipinfo = "href=\"/ip_mcast6/mld/proxy/help_mld_proxy.html#membershipinfo\"";
L7_char8 *pStrInfo_ipmcast6_NoMldCacheInfoAvailable =
                              "No MLD Cache Information Available.";
L7_char8 *pStrInfo_ipmcast6_MldProxyNotOperational = "MLD Proxy not operational";
L7_char8 *pStrInfo_ipmcast6_NoMldProxyIntfAvailable = "No MLD Proxy Interface Available";
