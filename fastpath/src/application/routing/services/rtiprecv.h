
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rtiprecv.h
*
* @purpose Internal definitions for the Route Table Object component.
*
* @component 
*
* @create 03/22/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "rtmbuf.h"

extern void rtLvl7IPRecv(L7_netBufHandle bufHandle,
                         sysnet_pdu_info_t *pduInfo);
extern void rtcreateIcmpErrPckt(struct rtmbuf *m, struct rtmbuf *mcopy);
struct rtmbuf *rtm_getm(short waitFlag, short mbufType);
extern void rtm_freem(struct rtmbuf *m);
extern void rtcopyMbuf(struct rtmbuf *m, struct rtmbuf *mcopy);
extern L7_uint32 rtFindRoute(L7_uint32 *outgoingIntf, L7_uint32 *nextHopIP, 
                             L7_uint32 dstAddr, L7_uint32 srcAddr);
extern L7_RC_t rtSelectNextHop(L7_routeEntry_t *bestRoute, L7_uint32 dstAddr,
                               L7_uint32 srcAddr, L7_uint32 *outgoingIntf, 
                               L7_uint32 *nextHopIP);
extern L7_uint32 rtFindRouteWithTOS(L7_uint32 *destIntf, L7_uint32 *routerIP, 
                                    L7_uint32 dstAddr, L7_uint32 srcAddr,
                                    L7_uint32 tosvalue);
extern void rtCheckOurIntf(L7_uint32 *MyLocalIfIndex, L7_uint32 dstAddr);
extern L7_uint32 rtGetIPAddrOfIntf(L7_uint32 intIfNum);
extern L7_uint32 rtGetUpStatusOfIntf(L7_uint32 intIfNum);
extern L7_uint32 rtGetMaxCountOfIntf();
extern L7_uint32 rtGetSubnetMaskOfIntf(L7_uint32 intIfNum);
extern L7_RC_t rtGetNetDirBcastAddrOfIntf(L7_uint32 intIfNum, L7_uint32 *pBcastAddr);

L7_RC_t rtIpRecvCnfgrInitPhase1Process(void);
void rtIpRecvCnfgrFiniPhase1Process(void);
L7_uint32 rtMemPoolIdGet (void);
L7_RC_t rtIfNetCreate(L7_uint32 intIfNum);
L7_RC_t rtIfNetDelete(L7_uint32 intIfNum);
L7_RC_t rtIfNetUp(L7_uint32 intIfNum);
L7_RC_t rtIfNetDown(L7_uint32 intIfNum);

extern L7_uint32 rtInIPForwardArpTable (L7_uint32 dest_ip_addr, L7_uint32 netMask, 
                                        struct rtmbuf *mcopy, 
                                        L7_uint32 ipIfIndex, L7_uint32 srcrt);
extern L7_RC_t ipmRecvLocal(L7_netBufHandle netBufHandle, 
                            L7_uint32 ipIfNum);
extern L7_RC_t ipmRouterIfFrameProcess (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
