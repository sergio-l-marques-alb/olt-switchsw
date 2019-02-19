/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_v6.h
*
* @purpose    interface or any  v6 utility prototypes
*
* @component  Mcast utilities
*
* @comments   none
*
* @create     04/17/2006
*
* @author     gkk/dsatyanarayana
*
* @end
**********************************************************************/
#ifndef INTF_V6_H
#define INTF_V6_H

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "mcast_wrap.h"
#include "ipv6_commdefs.h"

L7_uint32 mcastIp6MapRtrAdminModeGet();
L7_RC_t mcastIp6MapRtrIntfIpAddrListGet(L7_uint32 intIfNum, L7_uint32 *numAddr,
                                        L7_inet_addr_t *ipAddrList,
                                        L7_uint32 *ipPrefixLen);
L7_RC_t mcastIp6MapRtrIntfGlobalIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr);
L7_RC_t mcastIp6MapRtrIntfIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr);
L7_RC_t mcastIp6MapRtrIntfNetMaskGet(L7_uint32 intIfNum, L7_inet_addr_t *netMask);
L7_RC_t mcastIp6MapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum);
L7_RC_t mcastIp6MapRtrIntfToIntIfNum(L7_uint32 rtrIfNum, L7_uint32* intIfNum);
L7_RC_t mcastV6BestRouteLookup(L7_in6_addr_t *pIpAddr, 
                               mcastRPFInfo_t *pRtoRoute);
L7_BOOL mcastIp6MapIsLocalAddr(L7_uint32 intIfNum, L7_in6_addr_t *ipAddr);
L7_RC_t mcastIp6MapRtrIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_ipv6IntfAddr_t *pAddr);
L7_RC_t mcastIp6MapPrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len);
L7_RC_t mcastIp6LocalMulticastAddrUpdate(L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetAddr,L7_uchar8 actionFlag);
L7_RC_t mcastV6RPFNeighborGet(L7_in6_addr_t *pIpAddr, mcastRPFInfo_t *rpfRouteInfo,
                              L7_inet_addr_t *nbrAddr,  mrpCallback IsNbrPresent);
L7_RC_t
mcastV6IpAddrIntfResolve (L7_inet_addr_t *ipAddr,
                          L7_uint32 *intIfNum);

#endif /* INTF_V6_H */

