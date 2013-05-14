/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmProto.h
*
* @purpose Contains structs related to all PIMSM send/receive API.
*
* @component 
*
* @comments 
*
* @create 01/01/2006
*
* @author vmurali/dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSMPROTO_H_
#define _PIMSMPROTO_H_
#include "l3_addrdefs.h"
#include "pim_defs.h"
/*#include "pimsmmain.h"*/
#include "pimsmintf.h"

/* packet types */
#define PIMSM_HELLO               PIM_PDU_HELLO
#define PIMSM_REGISTER            PIM_PDU_REGISTER
#define PIMSM_REGISTER_STOP       PIM_PDU_REGISTER_STOP
#define PIMSM_JOIN_PRUNE          PIM_PDU_JOIN_PRUNE
#define PIMSM_BOOTSTRAP           PIM_PDU_BOOTSTRAP
#define PIMSM_ASSERT              PIM_PDU_ASSERT
#define PIMSM_CAND_RP_ADV         PIM_PDU_CANDIDATE_RP_ADVERTISEMENT
#define PIMSM_MAX_TYPE           (PIM_PDU_CANDIDATE_RP_ADVERTISEMENT + 1)



#define PIMSM_JOIN_PRUNE_MINLEN_IPV4      (4 + 4 + 4)
#define PIMSM_JOIN_PRUNE_MINLEN_IPV6      (4 + 4 + 16)


/* PIMSM_REGISTER definitions */
#define PIMSM_MESSAGE_REGISTER_BORDER_BIT         0x80000000
#define PIMSM_MESSAGE_REGISTER_NULL_REGISTER_BIT  0x40000000

/* PIMSM_HELLO definitions */
#define PIMSM_MESSAGE_HELLO_HOLDTIME               1
#define PIMSM_MESSAGE_HELLO_HOLDTIME_LENGTH        2
#define PIMSM_MESSAGE_HELLO_HOLDTIME_FOREVER       0xffff

#define PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY        2
#define PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY_LENGTH    4
#define PIMSM_MESSAGE_HELLO_TRACKING_BIT_OFFSET     0x8000

#define PIMSM_MESSAGE_HELLO_DR_PRIORITY            19
#define PIMSM_MESSAGE_HELLO_DR_PRIORITY_LENGTH        4

#define PIMSM_MESSAGE_HELLO_GENERATION_ID          20
#define PIMSM_MESSAGE_HELLO_GENERATION_ID_LENGTH      4

#define PIMSM_MESSAGE_HELLO_ADDRESS_LIST              24
#define PIMSM_MESSAGE_HELLO_ADDRESS_LIST_LENGTH       4

typedef struct pimsmSendJoinPruneData_s
{
  L7_BOOL         isStarStarRpFlag; /* is a (*,*,RP) join/prune.*/
  L7_inet_addr_t *  pNbrAddr;
  L7_ushort16     holdtime;
  L7_inet_addr_t *  pSrcAddr;  
  L7_uchar8       srcAddrMaskLen;  
  L7_inet_addr_t *  pGrpAddr; 
  L7_uchar8       grpAddrMaskLen;
  L7_uchar8       addrFlags;
  L7_uint32       rtrIfNum;
  L7_BOOL   joinOrPruneFlag; /* true= join , false = prune*/
  L7_BOOL   pruneEchoFlag; /*true = prune echo should be sent.
                             false =it should not be sent */
  L7_BOOL   sendImmdFlag;    /* Send JP pkt immediately, dont do bundling */                           
}pimsmSendJoinPruneData_t;

typedef struct pimsmHelloParams_s
{
  L7_ushort16 holdtime;
  L7_uint32 genId;
  L7_uint32 drPriority;
  L7_BOOL   LANPruneDelayPresent;
  L7_BOOL   trackingSupportPresent;
  L7_uint32 propagationDelay;
  L7_uint32 overrideIntvl;
  L7_inet_addr_t addrList[PIMSM_MAX_NBR_SECONDARY_ADDR];
} pimsmHelloParams_t;


struct pimsmCB_s;

extern L7_RC_t pimsmRegisterSend(struct pimsmCB_s *pimsmCb,
                                 L7_inet_addr_t * group, L7_uint32 reg_flags,
                                 L7_char8 *ipPkt, L7_uint32 ipPktLen);
extern L7_RC_t pimsmRegisterStopSend(struct pimsmCB_s *pimsmCb,
                                     L7_uint32 rtrIfNum, L7_inet_addr_t * reg_dst, 
                                     L7_inet_addr_t * inner_grp,
                                     L7_inet_addr_t * inner_src);
extern L7_RC_t pimsmNullRegisterSend(struct pimsmCB_s * pimsmCb, 
                                     L7_inet_addr_t * source, 
                                     L7_inet_addr_t * group);
extern L7_RC_t pimsmHelloSend(struct pimsmCB_s * pimsmCb, 
                              L7_uint32 rtrIfNum, 
                              L7_inet_addr_t *intfAddr,
                              L7_short16 holdtime);
extern L7_RC_t pimsmAssertCancelSend(struct pimsmCB_s *pimsmCb, L7_inet_addr_t * source,
                                     L7_inet_addr_t * group, L7_uint32  rtrIfNum);
extern L7_RC_t pimsmAssertSend(struct pimsmCB_s *pimsmCb, L7_inet_addr_t * source,
                               L7_inet_addr_t * group, L7_uint32  rtrIfNum,
                               L7_uint32 local_preference, L7_uint32 local_metric);
extern L7_RC_t pimsmJoinPruneSend(struct pimsmCB_s * pimsmCb, 
                                  pimsmSendJoinPruneData_t * jpData);
extern L7_RC_t pimsmCtrlPktRecv(struct pimsmCB_s *pimsmCb,L7_inet_addr_t *srcAddr_n, 
                                L7_inet_addr_t *dstAddr_n,
                                L7_char8 *pimHeader, L7_uint32 pimHeaderLen, 
                                L7_uint32 rtrIfNum, L7_uchar8 ipTypeOfService);
extern L7_RC_t  pimsmPacketSend(struct pimsmCB_s *pimsmCb,
                                L7_inet_addr_t * dstAddr, L7_inet_addr_t *pSrcAddr,
                                L7_uint32 msgType,
                                L7_uchar8 *data, L7_int32 datalen,
                                L7_uint32 rtrIfNum);

extern L7_RC_t pimsmJoinPruneImdtSend(pimsmCB_t *pimsmCb,
                                      pimsmSendJoinPruneData_t * pJPData);

extern void pimsmJPWorkingBuffReturn(struct pimsmCB_s *pimsmCb,
                                     pimsmNeighborEntry_t *pim_nbr);
extern void pimsmJPBundleTimerExpiryHandler(void *pParam);
#endif /* _PIMSMPROTO_H_ */

