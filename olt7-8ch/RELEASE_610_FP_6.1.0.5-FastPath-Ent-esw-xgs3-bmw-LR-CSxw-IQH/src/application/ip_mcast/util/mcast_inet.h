/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_inet.h
*
* @purpose
*
*
* @comments   
*
* @create     01/16/2006
*
* @author     Satya Dillikar
* @end
*
**********************************************************************/

#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
      
/* Is a local address*/
L7_BOOL inetIsLocalAddress(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum);
/* Is a local address , takes care of unnumbered interfaces also*/
L7_BOOL inetUnNumberedIsLocalAddress(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum);
/*Is a directly connected address */
L7_BOOL inetIsDirectlyConnected(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum);
/* set addr to 224.0.0.0(ipv4) & ...*/
L7_RC_t inetAllMcastGroupAddressInit(L7_uchar8 family, L7_inet_addr_t *addr);
/* set the mask to all multicast groups */
L7_RC_t inetAllMcastGroupPrefixLenInit(L7_uchar8 family, L7_uchar8 *pMaskLen);
/* test addr ...*/
L7_BOOL inetIsAllMcastGroupAddress(L7_inet_addr_t *addr);
/* test all mcast group prefix length ...*/
L7_BOOL inetIsAllMcastGroupPrefixLen(L7_uchar8 family, L7_uchar8 mask);
/* set addr to 224.0.0.13(ipv4) & ...*/
L7_RC_t inetAllPimRouterAddressInit(L7_uchar8 family, L7_inet_addr_t *addr);
/* test addr ...*/
L7_BOOL inetIsAllPimRouterAddress(L7_inet_addr_t *addr);
L7_uint32 inetPimsmDigestGet(L7_inet_addr_t *addr);
/* Tests for ALL_IGMPv3_ROUTERS address */
L7_BOOL inetIsAddressIgmpv3Routers(L7_inet_addr_t *addr);
/* Sets ALL_IGMPv3_ROUTERS address */
L7_RC_t inetIgmpv3RouterAddressInit(L7_uchar8 family, L7_inet_addr_t *addr) ;
/* Sets ALL_HOSTS address */
L7_RC_t inetAllHostsAddressInit(L7_uchar8 family, L7_inet_addr_t *addr);
/* Sets ALL_ROUTERS address */
L7_RC_t inetAllRoutersAddressInit(L7_uchar8 family, L7_inet_addr_t *addr);
/* Test for admin-scope boundary range (239.0.0.0/8) */
L7_BOOL mcastIsAddrAdminScopeRange(L7_inet_addr_t *grpAddr);
/* 
 * CAUTION: Always check for == L7_TRUE or == L7_FALSE
            Don't check for  != L7_TRUE or != L7_FALSE
            because of -1 being other return value.
 */
#define MCAST_INET_IS_ADDR_EQUAL(xaddr, yaddr)               \
    (((inetIsAddressZero((xaddr)) == L7_TRUE) ||             \
    (inetIsAddressZero((yaddr)) == L7_TRUE)) ? -1 :          \
    L7_INET_IS_ADDR_EQUAL((xaddr), (yaddr)))

