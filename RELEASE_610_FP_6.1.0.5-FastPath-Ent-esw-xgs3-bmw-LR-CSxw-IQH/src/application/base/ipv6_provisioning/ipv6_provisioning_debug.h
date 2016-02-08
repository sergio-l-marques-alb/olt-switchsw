/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_debug.h
*
* @purpose Contains LVL7 ipv6 provisioning debug function prototypes
*
* @component ipv6_provisioning
*
* @comments 
*
* @create 09/04/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/




#ifndef INCLUDE_IPV6_PROVISIONING_DEBUG_H
#define INCLUDE_IPV6_PROVISIONING_DEBUG_H

L7_RC_t ipv6ProvDebugHelp(void);
L7_RC_t ipv6ProvDebugCopyToCpu(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                               L7_uint32 mode);
L7_RC_t ipv6ProvDebugBridgeSrcUspDstUsp(L7_uint32 sunit, L7_uint32 sslot, L7_uint32 sport, 
                                        L7_uint32 dunit, L7_uint32 dslot, L7_uint32 dport, 
                                        L7_uint32 mode, L7_uint32 copyToCpu);
void ipv6ProvDebugInit(void);
void ipv6ProvDebugPktHandler(L7_netBufHandle bufHandle);
L7_uint32 ipv6ProvDebugGetStats(void);
void ipv6ProvDebugClearStats(void);

#endif /*INCLUDE_IPV6_PROVISIONING_DEBUG_H*/ 
