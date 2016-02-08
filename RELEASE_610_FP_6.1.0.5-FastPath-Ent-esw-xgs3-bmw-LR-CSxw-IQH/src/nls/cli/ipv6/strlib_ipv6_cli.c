/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/cli/ipv6/strlib_ipv6_cli.c                                                      
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

#include "strlib_ipv6_cli.h"

L7_char8 *pStrInfo_ipv6_Ms = "%4d ms %c";
L7_char8 *pStrInfo_ipv6_Ipv6AddrMacAddrIsrtrStateUpdated = "IPv6 Address                             MAC Address       isRtr State    Updated";
L7_char8 *pStrErr_ipv6_Ipv6InvalidGlobalAddr = "Invalid IPv6 global address.";
L7_char8 *pStrErr_ipv6_Ipv6InvalidLinkLocalAddr = "Invalid IPv6 link-local address.";
L7_char8 *pStrErr_ipv6_InputDstIpv6AddrCantBeALinkLocalAddr = "Invalid Input: Destination IPv6 address cannot be a Link Local Address.";
L7_char8 *pStrErr_ipv6_InputDstIpv6Addr = "Invalid Input: Destination IPv6 address.";
L7_char8 *pStrErr_ipv6_InputIpv6GatewayAddr = "Invalid Input: IPv6 gateway address.";
L7_char8 *pStrErr_ipv6_CantAddIpv6AddrInvalidPrefix = "Invalid prefix length. Could not add IPv6 address to interface.";
L7_char8 *pStrInfo_ipv6_NoIpv6GatewayAddr = "No IPv6 gateway address configured";
L7_char8 *pStrInfo_ipv6_NoIpv6GlobalAddr = "No IPv6 global addresses configured";
L7_char8 *pStrInfo_ipv6_TraceRouteInvalidPortNo = "Port no. is out of range and must be in the range of %d to %d.";
L7_char8 *pStrInfo_ipv6_TraceRouteTracingRoute = "Tracing route over a maximum of %d hops";
L7_char8 *pStrErr_ipv6_CfgNwIpv6Addr = "Use 'network ipv6 address <prefix>/<prefix_length> [eui64]'.";
L7_char8 *pStrErr_ipv6_CfgIpv6Addr = "Use 'ipv6 address <prefix>/<prefix_length> [eui64]'.";
L7_char8 *pStrErr_ipv6_CfgNwIpv6Gateway = "Use 'network ipv6 gateway <ipv6-address>'.";
L7_char8 *pStrErr_ipv6_CfgIpv6Gateway = "Use 'ipv6 gateway <ipv6-address>'.";
L7_char8 *pStrErr_ipv6_CfgNwNoIpv6Addr = "Use 'no network ipv6 address [<prefix>/<prefix-length> [eui64]]'.";
L7_char8 *pStrErr_ipv6_CfgNoIpv6Addr = "Use 'no ipv6 address [<prefix>/<prefix-length> [eui64]]'.";
L7_char8 *pStrErr_ipv6_CfgNwNoIpv6Gateway = "Use 'no network ipv6 gateway'.";
L7_char8 *pStrErr_ipv6_CfgNoIpv6Gateway = "Use 'no ipv6 gateway'.";
L7_char8 *pStrErr_ipv6_CfgServPortNoIpv6Addr = "Use 'no serviceport ipv6 address [<prefix>/<prefix-length> [eui64]]'.";
L7_char8 *pStrErr_ipv6_CfgServPortNoIpv6Enbl = "Use 'no serviceport ipv6 enable '.";
L7_char8 *pStrErr_ipv6_CfgServPortNoIpv6Gateway = "Use 'no serviceport ipv6 gateway'.";
L7_char8 *pStrErr_ipv6_Ipv6PingAddr_1 = "Use 'ping ipv6 <ipv6-address> [size <datagram-size>]'.";
L7_char8 *pStrErr_ipv6_Ipv6PingIntf = "Use 'ping ipv6 interface <%s> <link-local-address> [size <datagram-size>]'.";
L7_char8 *pStrErr_ipv6_Ipv6Ping_1 = "Use 'ping ipv6 {{<ipv6-address> [size <datagram-size>]} | {{{interface <%s>} |\r\n{tunnel <tunnel-id>} | {loopback <loopback-id>} } <link-local-address>\r\n[size <datagram-size>]}'.";
L7_char8 *pStrErr_ipv6_CfgServPortIpv6Addr = "Use 'serviceport ipv6 address <prefix>/<prefix_length> [eui64]'.";
L7_char8 *pStrErr_ipv6_CfgServPortIpv6Enbl = "Use 'serviceport ipv6 enable '.";
L7_char8 *pStrErr_ipv6_CfgServPortIpv6Gateway = "Use 'serviceport ipv6 gateway <ipv6-address>'.";
L7_char8 *pStrErr_ipv6_ShowIpv6NwNdp_1 = "Use 'show network ndp'.";
L7_char8 *pStrErr_ipv6_ShowIpv6ServiceportNdp_1 = "Use 'show serviceport ndp'.";
L7_char8 *pStrInfo_ipv6_UseTracerouteIpv6IpaddrPortno = "Use 'traceroute ipv6 <ipaddr> [<portno>]'.";
L7_char8 *pStrInfo_ipv6_NwIpv6AddrAutoConfig = "network ipv6 address autoconfig";
L7_char8 *pStrInfo_ipv6_Ipv6AddrAutoConfig = "ipv6 address autoconfig";
L7_char8 *pStrInfo_ipv6_NwIpv6AddrDhcp = "network ipv6 address dhcp";
L7_char8 *pStrInfo_ipv6_Ipv6AddrDhcp = "ipv6 address dhcp";
L7_char8 *pStrInfo_ipv6_NwIpv6Addr = "network ipv6 address %s/%d";
L7_char8 *pStrInfo_ipv6_Ipv6Addr = "ipv6 address %s/%d";
L7_char8 *pStrInfo_ipv6_NwIpv6AddrEui64 = "network ipv6 address %s/%d eui64";
L7_char8 *pStrInfo_ipv6_Ipv6AddrEui64 = "ipv6 address %s/%d eui64";
L7_char8 *pStrInfo_ipv6_NwIpv6Enbl = "network ipv6 enable";
L7_char8 *pStrInfo_ipv6_Ipv6Enbl = "ipv6 enable";
L7_char8 *pStrInfo_ipv6_NwIpv6Gateway = "network ipv6 gateway %s";
L7_char8 *pStrInfo_ipv6_Ipv6Gateway = "ipv6 gateway %s";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6AddrAutoConfig="serviceport ipv6 address autoconfig";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6AddrDhcp="serviceport ipv6 address dhcp";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6Addr = "serviceport ipv6 address %s/%d";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6AddrEui64 = "serviceport ipv6 address %s/%d eui64";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6Enbl = "serviceport ipv6 enable";
L7_char8 *pStrInfo_ipv6_ServiceportIpv6Gateway = "serviceport ipv6 gateway %s";
L7_char8 *pStrInfo_ipv6_TraceRouteCantTraceRoute = "trace the route.";

