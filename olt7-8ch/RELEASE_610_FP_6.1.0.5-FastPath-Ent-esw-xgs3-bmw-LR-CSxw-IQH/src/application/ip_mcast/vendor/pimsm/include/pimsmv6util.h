
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmv6util.h
*
* @purpose   header file for PIM-SM IPv6 API's.
*
* @component 
*
* @comments  none
*
* @create   11/04/2006
*
* @author   nramu
*
* @end
*
**********************************************************************/
#ifndef _PIMSM_V6_UTIL_H_
#define _PIMSM_V6_UTIL_H_
L7_RC_t pimsmV6BestRoutesGet(pimsmCB_t *pimsmCb,
                             L7_BOOL *pMoreChanges);

L7_RC_t pimsmV6SourceAndGroupAddressGet(L7_uchar8 *data,L7_inet_addr_t *source,
                          L7_inet_addr_t *group,L7_uint32 *pPktLen);


L7_RC_t pimsmV6BestRouteLookup(L7_in6_addr_t *pIpAddr, 
                               mcastRPFInfo_t *pRtoRoute);


L7_RC_t pimsmV6IpHdrFrame(L7_uchar8 *packet,L7_uint32 *pPktLen,
                          L7_inet_addr_t *source,L7_inet_addr_t *group);

L7_RC_t pimsmV6SocketOptionsSet(L7_uint32 sockfd);
L7_RC_t pimsmV6RouteChangeBufMemAlloc(pimsmCB_t *pimsmCb);

#endif
