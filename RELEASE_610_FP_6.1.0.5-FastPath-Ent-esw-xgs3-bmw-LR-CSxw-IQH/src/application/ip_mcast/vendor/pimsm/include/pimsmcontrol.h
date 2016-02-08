/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmControl.h
*
* @purpose  APIs for various event handlers.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSMCONTROL_H_
#define _PIMSMCONTROL_H_
#include "l3_addrdefs.h"
#include "pimsmdefs.h"
/*#include "pimsmmain.h"*/
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"

struct pimsmCB_s;

extern L7_RC_t pimsmMfcNoCacheProcess(struct pimsmCB_s * pimsmCb,
                                      L7_inet_addr_t * pSrcAddr, L7_inet_addr_t * pGrpAddr, 
                                      L7_uint32 rtrIfNum);
extern L7_RC_t pimsmMfcWrongIfProcess(struct pimsmCB_s * pimsmCb,
                                      L7_inet_addr_t * pSrcAddr, L7_inet_addr_t * pGrpAddr,
                                      L7_uint32 rtrIfNum);
extern L7_RC_t pimsmMfcEntryExpireProcess(pimsmCB_t * pimsmCb, L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pGrpAddr);

extern L7_RC_t pimsmMfcWholePktProcess(struct pimsmCB_s *pimsmCb, L7_uchar8 *pData);
extern L7_RC_t pimsmMfcIntfEventQueue(struct pimsmCB_s * pimsmCb, 
      L7_uint32 rtrIfNum, L7_uint32 mode);
extern L7_RC_t pimsmMfcQueue(struct pimsmCB_s * pimsmCb,
                             mfcOperationType_t operType, L7_inet_addr_t *pSrcAddr,
                             L7_inet_addr_t *pGrpAddr, L7_uint32 rtrIfNum, 
                             interface_bitset_t *pOif, 
                             L7_BOOL isTrueSGEntry, 
                             L7_ushort16 *pFlags);



extern L7_RC_t pimsmIgmpMemberAddProcess(struct pimsmCB_s * pimsmCb,
                                         L7_inet_addr_t * pSrcAddr, L7_inet_addr_t * pGrpAddr,
                                         L7_uint32 rtrIfNum);
extern L7_RC_t pimsmIgmpMemberDelProcess(struct pimsmCB_s * pimsmCb,
                                         L7_inet_addr_t * pSrcAddr, L7_inet_addr_t * pGrpAddr, 
                                         L7_uint32 rtrIfNum);


extern L7_RC_t pimsmRtoBestRouteChangeProcess(struct pimsmCB_s * pimsmCb,
                                              mcastRPFInfo_t *pRouteInfo);
extern L7_RC_t pimsmRPFInfoGet(struct pimsmCB_s * pimsmCb, 
                               L7_inet_addr_t * pSrcAddr, mcastRPFInfo_t *pBestRPFRoute);


extern L7_RC_t pimsmMcastPktRecvProcess(struct pimsmCB_s * pimsmCb,
                                        mcastControlPkt_t * mcastCrlPkt);
extern L7_RC_t pimsmIgmpMemberUpdateProcess(struct pimsmCB_s * pimsmCb, mgmdMrpEventInfo_t *pMgmdGrpInfo);

extern L7_RC_t pimsmAdminScopeEventProcess(pimsmCB_t *pimsmCb,
                                           mcastAdminMsgInfo_t *pAdminScopeInfo);

#endif /* _PIMSMCONTROL_H_*/

