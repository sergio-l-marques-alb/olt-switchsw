/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmmacros.h
*
* @purpose Contains various macros related structs for PIM-SM protocol operation
*
* @component    pimsm 
*
* @comments 
*
* @create 01/01/2006
*
* @author dsatyanarayana / vmurali
* @end
*
***********************************************************************/
#ifndef _PIMSM_COMMON_FSM_H_
#define _PIMSM_COMMON_FSM_H_
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "intf_bitset.h"
/*#include "pimsmmain.h"*/
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"

typedef   struct pimsmAssertMetric_s
{
  L7_BOOL rpt_bit_flag;    
  L7_uint32 metric_preference;    
  L7_uint32 route_metric;    
  L7_inet_addr_t ip_address;  
}pimsmAssertMetric_t;

struct pimsmSGEntry_s;
struct pimsmSGIEntry_s;
struct pimsmSGNode_s;

struct pimsmSGRptEntry_s;
struct pimsmSGRptIEntry_s;
struct pimsmSGRptNode_s;

struct pimsmStarStarRpEntry_s;
struct pimsmStarStarRpIEntry_s;
struct pimsmStarStarRpNode_s;

struct pimsmStarGIEntry_s;
struct pimsmStarGEntry_s;
struct pimsmStarGNode_s;

extern L7_BOOL pimsmLANDelayEnabled(pimsmCB_t *pimsmCb, 
                                   pimsmInterfaceEntry_t *pIntfEntry);

extern L7_RC_t pimsmEffectiveOverrideInterval(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum,
                                       L7_uint32 *overrideIntvl);

extern L7_uint32 pimsmJoinPruneOverrideIntervalGet(struct pimsmCB_s * pimsmCb,
                                                  L7_uint32 rtrIfNum);

extern L7_RC_t pimsmRPFInterfaceGet(struct pimsmCB_s * pimsmCb,
                                    L7_inet_addr_t *address, L7_uint32 *rtrIfNum);

extern L7_BOOL pimsmStarGICouldAssert(struct pimsmCB_s * pimsmCb, 
                                      struct pimsmStarGNode_s * pStarGNode, 
                                      L7_uint32 rtrIfNum);
extern L7_BOOL pimsmSGICouldAssert(struct pimsmCB_s * pimsmCb, 
                                   pimsmSGNode_t * sgNode,L7_uint32  rtrIfNum);
extern L7_BOOL pimsmSGRptPruneDesired(struct pimsmCB_s * pimsmCb, 
                                      struct pimsmSGRptNode_s * pimsmSGRptNode);

extern L7_BOOL pimsmRPTJoinDesired(struct pimsmCB_s * pimsmCb, 
                                   L7_inet_addr_t * grpAddr);
extern L7_BOOL pimsmSGJoinDesired(struct pimsmCB_s * pimsmCb, 
                                  struct pimsmSGNode_s * pimsmSGNode);
extern L7_BOOL pimsmStarGJoinDesired(struct pimsmCB_s * pimsmCb, 
                                     struct pimsmStarGNode_s * pStarGNode);
extern L7_BOOL pimsmStarStarRpJoinDesired(struct pimsmCB_s * pimsmCb,
                                          struct pimsmStarStarRpNode_s * pimsmStarStarRpNode);

extern L7_BOOL pimsmCouldRegister(struct pimsmCB_s * pimsmCb, 
                                  struct pimsmSGNode_s * pimsmSGNode);

extern L7_BOOL pimsmSwitchToSptDesired(struct pimsmCB_s * pimsmCb, 
                                       L7_inet_addr_t * source,L7_inet_addr_t * group,
                                       L7_uint32 rtrIfNum);

extern void pimsmCheckSwitchToSpt(struct pimsmCB_s * pimsmCb, L7_inet_addr_t * source,
                                  L7_inet_addr_t * group);

extern void pimsmSPTbitUpdate(struct pimsmCB_s * pimsmCb, 
                              struct pimsmSGNode_s * pimsmSGNode, L7_uint32 rtrIfNum);

extern L7_BOOL pimsmSGIAssertTrackingDesired(struct pimsmCB_s * pimsmCb, 
                                             struct pimsmSGNode_s * pimsmSGNode, 
                                             L7_uint32 rtrIfNum);
extern L7_BOOL pimsmStarGIAssertTrackingDesired(struct pimsmCB_s * pimsmCb, 
                                                pimsmStarGNode_t * pStarGNode, L7_uint32 rtrIfNum);

extern L7_RC_t pimsmSGRptNeighborRpfDash(struct pimsmCB_s * pimsmCb, 
                                         pimsmSGRptNode_t * sgRptNode);                 
extern L7_RC_t pimsmStarGNeighborRpfDash(struct pimsmCB_s * pimsmCb, 
                                         struct pimsmStarGNode_s * starGNode);                 
extern L7_RC_t pimsmSGNeighborRpfDash(struct pimsmCB_s * pimsmCb, 
                                      pimsmSGNode_t * sgNode);

extern L7_RC_t pimsmStarStarRpNeighborRpfDash ( struct pimsmCB_s * pimsmCb,
                                                struct  pimsmStarStarRpNode_s * pimsmStarStarRPNode);
extern L7_RC_t pimsmSGInhrtdOlist(struct pimsmCB_s * pimsmCb, 
                                  struct pimsmSGNode_s * sgNode, interface_bitset_t *oiflist);
extern L7_RC_t pimsmSGRptInhrtdOlist(struct pimsmCB_s * pimsmCb, 
                                     struct pimsmSGRptNode_s * sgRptNode, interface_bitset_t *oiflist);
extern L7_RC_t pimsmSGImdtOlist(struct pimsmCB_s * pimsmCb, 
                                struct pimsmSGNode_s * sgNode, interface_bitset_t *oiflist);
extern L7_RC_t pimsmStarStarRpImdtOlist(struct pimsmCB_s * pimsmCb, 
                                        struct pimsmStarStarRpNode_s * starStarRpNode,
                                        interface_bitset_t *oiflist);
extern L7_RC_t pimsmStarGImdtOlist(struct pimsmCB_s * pimsmCb, 
                                   struct pimsmStarGNode_s * starGNode, interface_bitset_t *oiflist); 

extern L7_BOOL pimsmIsLastHopRouter(struct pimsmCB_s * pimsmCb, 
                                    struct pimsmStarGNode_s * starGNode);

extern L7_RC_t pimsmSwitchToShortestPath(struct pimsmCB_s * pimsmCb, 
                                         L7_inet_addr_t * source, 
                                         L7_inet_addr_t * group, 
                                         pimsmStarGNode_t   *starGNode);
extern L7_RC_t pimsmMyAssertMetric ( struct pimsmCB_s * pimsmCb,
                              L7_inet_addr_t *  srcAddr,
                              L7_inet_addr_t *  grpAddr,
                              L7_uint32 rtrIfNum,
                              pimsmAssertMetric_t * metric );
extern L7_uint32 pimsmCompareMetrics(
                                    L7_uint32 local_preference,
                                    L7_uint32 local_metric,
                                    L7_inet_addr_t * local_address,
                                    L7_uint32 remote_preference,
                                    L7_uint32 remote_metric,
                                    L7_inet_addr_t * remote_address);
extern void pimsmRestartKeepaliveTimer(pimsmCB_t *pimsmCb,
                                       pimsmSGNode_t *pSGNode,
                                       L7_uint32 timeout);

extern L7_RC_t pimsmNbr(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum,
         L7_inet_addr_t * pNbrAddr, L7_inet_addr_t * primaryAddr);

extern L7_BOOL pimsmIsNbrPresent(L7_uchar8 family, L7_uint32 rtrIfNum, 
			L7_inet_addr_t *nbrAddr);
#endif /*_PIMSM_COMMON_FSM_H_*/

