/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmBsr.h
*
* @purpose   All C-BSR structs and macros.
*
* @component pimsm
*
* @comments  
*
* @create   01/01/2006
*
* @author   dsatyanarayana
*
* @end
*
**********************************************************************/
#ifndef _PIMSMBSR_H_
#define _PIMSMBSR_H_

#include "l3_addrdefs.h"
#include "pimsmtimer.h"
#include "l7handle_api.h"
#include "pimsmcandbsrfsm.h"
#include "pimsmnoncandbsrfsm.h"



typedef enum
{
  PIMSM_BSR_TYPE_NONE = 0, 
  PIMSM_BSR_TYPE_CBSR,
  PIMSM_BSR_TYPE_NONBSR,
  PIMSM_BSR_TYPE_MAX
}pimsmBSRType_t; /* defines the type - whether it is configured as candidate BSR or not */

typedef struct pimsmBSMMsgRpNode_s
{
  L7_inet_addr_t               pimsmRpAddr;
  L7_ushort16                  pimsmRpHoldtime;
  L7_uint32                    pimsmRpPriority;
}pimsmBSMMsgRpNode_t;

typedef struct pimsmBSMMsgGrpNode_s
{
  L7_inet_addr_t                pimsmGrpAddr;
  L7_uchar8                     pimsmGrpMaskLength;
  L7_ushort16                   pimsmRpCnt;
  L7_ushort16                   pimsmFragRpCnt;
}pimsmBSMMsgGrpNode_t;

typedef struct pimsmFragRpNode_s
{
  struct pimsmFragRpNode_s  *next;
  L7_inet_addr_t               pimsmRpAddr;
  L7_ushort16                  pimsmRpHoldtime;
  L7_uint32                    pimsmRpPriority;
}pimsmFragRpNode_t;

typedef struct pimsmFragGrpRpNode_s
{
  struct pimsmFragGrpRpNode_s  *next;
  L7_inet_addr_t                pimsmGrpAddr;
  L7_uchar8                     pimsmGrpMaskLength; 
  L7_ushort16                   pimsmRpCnt;
  L7_ushort16                   pimsmFragRpCnt;
  L7_sll_t                      pimsmRpList;  /* node is pimsmFragRpNode_s */
}pimsmFragGrpRpNode_t;


typedef struct pimsmBsrFragmentInfo_s
{
  L7_ushort16                 pimsmFragTag;
  L7_sll_t                    pimsmGrpRpList; /* node is pimsmFragGrpRpNode_s */
}pimsmBsrFragmentInfo_t;

typedef struct pimsmBSMPacketNode_s
{
  L7_uchar8     pimsmBSMPkt[PIMSM_PKT_SIZE_MAX];
  L7_uint32     pimsmBSMPktLen;
  L7_BOOL       pimsmInUse;
}pimsmBSMPacketNode_t;

typedef struct pimsmBsrPerScopeZone_s 
{
  struct pimsmBsrPerScopeZone_s     *next;
  pimsmBSRType_t       pimsmBSRType;   /* configured type of this bsr */
  L7_inet_addr_t       pimsmBSRGroupRange; /* ipaddr of the scoped/nonscoped zone */
  L7_uchar8            pimsmBSRGroupMask;    
  L7_inet_addr_t       pimsmBSRAddr; /* ipaddr of the bsr */
  L7_uint32            pimsmBSRPriority; 
  L7_ushort16          pimsmBSRHashMasklen; 
  pimsmBSMPacketNode_t pimsmLastBSMPktNode[PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX];
  L7_APP_TMR_HNDL_t    pimsmBSRTimer;    /* the bootstrap timer */
  L7_uint32            pimsmBSRTimerHandle; /* to safeguard timeout functions */
  L7_uint32            ctime;        /* creation time */
  pimsmBsrFragmentInfo_t pimsmBSRFragInfo;

  union 
  {
    struct 
    {
      pimsmPerScopeZoneCandBSRStates_t     pimsmCandBSRState;
      L7_uint32                    pimsmElectedBSRPriority;
      L7_uchar8                    pimsmElectedBSRHashMaskLen;
      L7_inet_addr_t               pimsmElectedBSRAddress; 
      L7_ushort16                  pimsmLastFragUsed; /* this is last frag id used */ 
    }cbsr;
    struct 
    {
      pimsmPerScopeZoneNonCandBSRStates_t     pimsmNonCandBSRState; 
      L7_APP_TMR_HNDL_t            pimsmBSRScopeZoneExpireTimer; /* scope zone expiry timer */
    }nbsr;
  }zone;

  /* info for to backtrack for clean up */
  pimsmCB_t       *pimsmCb;
}pimsmBsrPerScopeZone_t;

typedef struct pimsmCandBSRInfo_s 
{
  L7_inet_addr_t  pimsmBSRCandidateBSRAddress;     /* my_bsr address */
  L7_uint32 pimsmBSRCandidateBSRPriority;         /* my_bsr priority */
  L7_uint32 pimsmBSRCandidateBSRHashMaskLength;   /* my_bsr hash_mask */
  L7_uint32 pimsmBSRCandidateBSRrtrIfNum;         /* rtrIfNum over which BSR is configured */
  L7_BOOL pimsmBSRCandidateBSRElectedBSR;         /* am THE BSR*/
} pimsmCandBSRInfo_t;

typedef struct pimsmBSRBlock_s
{
  pimsmBsrPerScopeZone_t  pimsmBsrInfoList[PIMSM_BSR_SCOPE_NODES_MAX];
  pimsmCandBSRInfo_t      pimsmCandBSRInfo;
}pimsmBSRBlock_t;

extern
L7_RC_t pimsmBsrInit(pimsmCB_t * pimsmCB);

extern
L7_RC_t pimsmBsrDeInit(pimsmCB_t *pimsmCb);

extern
L7_RC_t pimsmBsrInfoNodeFind(pimsmCB_t                *pimsmCb, 
                             L7_inet_addr_t            grpRange,
                             L7_uchar8                 grpMaskLen,
                             pimsmBsrPerScopeZone_t  **ppBsrNode);
extern 
L7_RC_t pimsmBsrInfoNodeDelete(pimsmCB_t               *pimsmCb, 
                               pimsmBsrPerScopeZone_t  *pBsrNode);
extern
L7_BOOL pimsmBsrInfoBestGroupRangeMatch(pimsmCB_t              *pimsmCb,
                                        pimsmBsrPerScopeZone_t *pBsrNode,
                                        L7_inet_addr_t          grpAddr,
                                        L7_uchar8               grpMasklen);
extern 
L7_BOOL pimsmBsrIsLocal(pimsmCB_t     *pimsmCb,
                        L7_inet_addr_t grpAddr,
                        L7_uchar8      grpMaskLen);
extern
L7_uint32 pimsmBsrRandOverrideIntervalGet(pimsmCB_t              *pimsmCb,
                                          pimsmBsrPerScopeZone_t *pBsrNode);
extern
void pimsmBsrPacketRpGrpMappingStore(pimsmCB_t              *pimsmCb,
                                     pimsmBsrPerScopeZone_t *pBsrNode,
                                     L7_uchar8              *pimHeader,
                                     L7_uint32               pimPktLen);
extern
L7_RC_t pimsmBsrRpGrpMappingRefresh(pimsmCB_t              *pimsmCb,
                                    pimsmBsrPerScopeZone_t *pBsrNode);
extern
void pimsmBsrPacketForward(pimsmCB_t               *pimsmCb,
                           pimsmBsrPerScopeZone_t  *pBsrNode,
                           L7_uchar8               *pimHeader,
                           L7_uint32                pimPktLen, 
                           L7_uint32                rtrIfNum);
extern
void pimsmBsrPacketOriginate(pimsmCB_t              *pimsmCb,
                             pimsmBsrPerScopeZone_t *pBsrNode,
                             L7_inet_addr_t         *pNbrAddr,
                             L7_uint32               rtrIfNum);	
extern
void  pimsmBsrCandRpToRpGrpListGet(pimsmCB_t              *pimsmCb,
                                   pimsmBsrPerScopeZone_t *pBsrNode);
extern
L7_RC_t pimsmBsrBootstrapRecv(pimsmCB_t       *pimsmCb, 
                              L7_inet_addr_t  *pSrcAddr,
                              L7_inet_addr_t  *pDestAddr,
                              L7_uchar8       *pimHeader,
                              L7_uint32        pimPktLen, 
                              L7_uint32        rtrIfNum);
extern
L7_RC_t pimsmBsrCandRpAdvRecv(pimsmCB_t       *pimsmCb, 
                              L7_inet_addr_t  *pSrcAddr,
                              L7_inet_addr_t  *pDestAddr,
                              L7_uchar8       *pimHeader,
                              L7_uint32        pimPktLen, 
                              L7_uint32        rtrIfNum);

extern
L7_RC_t pimsmBsrUnicastBSM(pimsmCB_t      *pimsmCb, 
                           L7_inet_addr_t *pNbrAddr,
                           L7_uint32       rtrIfNum);
extern
L7_RC_t pimsmBsrCandBSRConfigProcess(pimsmCB_t      *pimsmCb,
                                     L7_inet_addr_t *pBsrAddr,
                                     L7_uint32       rtrIfNum,
                                     L7_uint32       bsrPriority,
                                     L7_uint32       bsrHashMaskLen,
                                     L7_uint32       scopeId, 
                                     L7_BOOL         operMode);
extern
L7_RC_t pimsmBsrElectedBsrGet(pimsmCB_t               *pimsmCb,
                              pimsmBsrPerScopeZone_t **ppBsrNode);

extern
L7_RC_t pimsmBsrElectedBsrNextGet(pimsmCB_t               *pimsmCb,
                                  pimsmBsrPerScopeZone_t **ppBsrNode);
extern
void pimsmBsrIntfDown(pimsmCB_t *pimsmCb,L7_uint32 rtrIfNum);

extern
void pimsmBsrPurge(pimsmCB_t *pimsmCb);

#endif /* end of the file */


