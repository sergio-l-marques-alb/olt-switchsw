
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmwrap.h
*
* @purpose   header file containg prototypes for the wrapper API's
*
* @component 
*
* @comments  none
*
* @create   04/15/2006
*
* @author   nramu
*
* @end
*
**********************************************************************/
#ifndef _PIMSM_WRAP_H_
#define _PIMSM_WRAP_H_

/*Prototypes */

/*L7_RC_t pimsmSrcMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);
L7_RC_t pimsmGrpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);*/
L7_RC_t pimsmDefaultHashMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);
L7_RC_t pimsmSourceAndGroupAddressGet(L7_uchar8 family,L7_uchar8 *data,
                                      L7_inet_addr_t *source,
                                      L7_inet_addr_t *group,
                                      L7_uint32 *pPktLen);
L7_RC_t pimsmStaticRouteAddressGet(L7_uchar8 family,L7_uint32 intIfNum,
                                   L7_inet_addr_t *staticRouteAddr,
                                   L7_inet_addr_t *staticNextHopIp,
                                   L7_uint32 *rtrIfNum);
L7_RC_t pimsmBestRouteLookup(L7_uchar8 family,L7_inet_addr_t *src_addr,
                             mcastRPFInfo_t *rtoRoute);

L7_RC_t pimsmMessageLengthValidate(L7_uchar8 family, L7_uint32 dataLen);
L7_RC_t pimsmDecapsulateAndSend(L7_uchar8 family, L7_uint32 sockFd,
                                interface_bitset_t *oifList, 
                                L7_uchar8 *data, L7_uint32 dataLen, L7_uchar8 ipTOS);
L7_RC_t pimsmEncapsulateIpPkt(L7_uchar8 family,L7_uchar8 *data,
                              L7_uint32 *pPktLen);
L7_RC_t pimsmStarStarRpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);
L7_RC_t pimsmIpHdrFrame(L7_uchar8 family,L7_uchar8 *data,L7_inet_addr_t *source,
                        L7_inet_addr_t *group,
                        L7_uint32 *pPktLen);
L7_RC_t pimsmRtoBestRoutesGet(pimsmCB_t *pimsmCb,
                              L7_BOOL *pMoreChanges);
void pimsmMaxPktFdsGet(L7_uint32 *maxPktFds);
L7_RC_t pimsmSocketOptionsSet(L7_uchar8 family,L7_uint32 sockfd);
L7_RC_t pimsmRouteChangeBufMemAlloc(pimsmCB_t *pimsmCb);
extern L7_RC_t pimsmUtilMemoryInit (L7_uint32 addrFamily);
L7_RC_t pimsmJoinPruneMsgLenGet(L7_uchar8 family,L7_uint32 *pJoinPruneMsgSize);

#endif
