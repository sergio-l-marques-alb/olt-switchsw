/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_wrap.c
*
* @purpose    interface (unified IPMAP-related) prototypes 
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
#ifndef INTF_WRAP_H
#define INTF_WRAP_H

#include "commdefs.h"
#include "l3_commdefs.h"
#include "l3_mcast_commdefs.h"
#include "l3_addrdefs.h"
#include "l7_mcast_api.h"
#include "mcast_inet.h"
#include "intf_bitset.h"

L7_uint32  mcastIpMapRtrAdminModeGet(L7_uint32 familyType);

L7_RC_t mcastIpMapIntIfNumToRtrIntf(L7_uchar8 familyType, 
                                L7_uint32 intIfNum, L7_uint32* rtrIfNum);
L7_RC_t mcastIpMapRtrIntfToIntIfNum(L7_uchar8 familyType, 
                                L7_uint32 rtrIfNum, L7_uint32* intIfNum);
L7_RC_t mcastIpMapRtrIntfIpAddressGet(L7_uchar8 familyType, 
                                L7_uint32 intIfNum, L7_inet_addr_t *ipAddr);
L7_RC_t mcastIpMapRtrIntfNetMaskGet(L7_uchar8 familyType, 
                           L7_uint32 intIfNum, L7_inet_addr_t *netMask);
L7_RC_t mcastIpMapRtrIntfIpAddressesGet(L7_uchar8 familyType, 
                                L7_uint32 intIfNum, L7_uint32 *numAddr,
                                L7_inet_addr_t *ipAddrList,
                                L7_uint32 *ipPrefixLen);
L7_RC_t mcastIpMapIpAddressGet(L7_uchar8 family, L7_uint32 rtrIfNum, 
                              L7_inet_addr_t *intfAddr);
L7_RC_t mcastIpMapRtrIntfGlobalIpAddressGet(L7_uchar8 familyType, 
                                L7_uint32 intIfNum, L7_inet_addr_t *ipAddr);
L7_RC_t mcastIpMapGlobalIpAddressGet(L7_uchar8 family, L7_uint32 rtrIfNum, 
                              L7_inet_addr_t *intfAddr);
L7_BOOL mcastIpMapIsLocalAddress(L7_inet_addr_t * address, 
   L7_uint32 *rtrIfNum);
L7_BOOL mcastIpMapUnnumberedIsLocalAddress(L7_inet_addr_t * address, 
                                           L7_uint32 *rtrIfNum);
L7_BOOL mcastIpMapIsDirectlyConnected(L7_inet_addr_t * address, 
   L7_uint32 *rtrIfNum);
L7_RC_t mcastRPFInterfaceGet ( L7_inet_addr_t * address,
                               L7_uint32 * rtrIfNum );
L7_RC_t mcastMacAddressCheck (L7_uchar8 family,L7_uchar8 *ipAddrBuf  ,
                               L7_uchar8 *macAddrBuf  );
L7_RC_t mcastRtrIntfMcastFwdModeSet(L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 mode);

L7_RC_t mcastRPFInfoGet(L7_inet_addr_t *src_addr,
                             mcastRPFInfo_t *rtoRoute);
L7_RC_t mcastLocalMulticastAddrUpdate(L7_uint32 familyType,L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetAddr,L7_uchar8 actionFlag);

/* To test whether a router interface is unnumbered or not */
L7_BOOL mcastIpMapIsRtrIntfUnnumbered(L7_uchar8 family, L7_uint32 rtrIfNum);
L7_BOOL mcastIpMapIsIntfUnnumbered(L7_uchar8 family, L7_uint32 intIfNum);

typedef L7_BOOL (*mrpCallback)(L7_uchar8 family, L7_uint32 rtrIfNum, 
                                 L7_inet_addr_t *nbrAddr);
L7_RC_t mcastRPFNeighborGet(L7_inet_addr_t *srcAddr,  mcastRPFInfo_t *rpfRouteInfo,
                            L7_inet_addr_t *nbrAddr,  mrpCallback IsNbrPresent);

L7_uchar8* mcastIntfBitSetToString(interface_bitset_t *pOif,
        L7_uchar8 *pStr, L7_uint32 strSize);

L7_RC_t
mcastIpAddrRtrIntfResolve (L7_inet_addr_t *ipAddr,
                           L7_uint32 *rtrIfNum);

#endif /*INTF_WRAP_H*/

