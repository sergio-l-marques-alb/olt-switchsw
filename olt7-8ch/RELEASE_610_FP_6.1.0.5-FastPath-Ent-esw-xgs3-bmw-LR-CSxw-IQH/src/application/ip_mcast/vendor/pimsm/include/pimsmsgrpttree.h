/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGRptTree.h
*
* @purpose Contains PIM-SM  avltree function declarations for (S,G,Rpt) entry
*
* @component 
*
* @comments 
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda
* @end
*
**********************************************************************/
#ifndef _PIMSM_SG_RPT_TREE_H
#define _PIMSM_SG_RPT_TREE_H
#include "l3_addrdefs.h"
#include "pimsmdefs.h"
/*#include "pimsmmain.h"*/
#include "l7apptimer_api.h"
#include "pimsmdnstrmsgrptfsm.h"
#include "pimsmupstrmsgrptfsm.h"
#include "pimsmtimer.h"
#include "rto_api.h"

enum pimsmDnStrmPerIntfSGRptStates_t;
enum pimsmUpStrmSGRptStates_s;
struct pimsmCB_s;

typedef struct pimsmSGRptEntry_s
{
  L7_inet_addr_t  pimsmSGRptGrpAddress;
  L7_inet_addr_t  pimsmSGRptSrcAddress;
  L7_uint32     pimsmSGRptCreateTime;
  pimsmUpStrmSGRptStates_t pimsmSGRptUpstreamPruneInitState; 
      /* used to initialize pimsmSGRptUpstreamPruneState */
  pimsmUpStrmSGRptStates_t pimsmSGRptUpstreamPruneState; 
  L7_APP_TMR_HNDL_t   pimsmSGRptUpstreamOverrideTimer;
  L7_uint32   pimsmSGRptUpstreamOverrideTimerHandle;
  pimsmTimerData_t pimsmSGRptUpstreamOverrideTimerParam; 
  L7_uint32   pimsmSGRptRPFIfIndex; /* = RPF_interface(RP(G))*/
  L7_inet_addr_t    pimsmSGRptUpstreamNeighbor;   /*RPFDash */
} pimsmSGRptEntry_t;

typedef struct pimsmSGRptIEntry_s
{
  /*L7_ushort16         flags;*/
  L7_uint32           pimsmSGRptIIfIndex;
  L7_uint32     pimsmSGRptICreateTime;
  L7_BOOL           pimsmSGRptILocalMembership;
  pimsmDnStrmPerIntfSGRptStates_t     pimsmSGRptIJoinPruneState;
  L7_APP_TMR_HNDL_t       pimsmSGRptIPrunePendingTimer;
  L7_uint32   pimsmSGRptIPrunePendingTimerHandle;   
  pimsmTimerData_t pimsmSGRptIPrunePendingTimerParam; 
  L7_APP_TMR_HNDL_t       pimsmSGRptIPruneExpiryTimer;
  L7_uint32   pimsmSGRptIPruneExpiryTimerHandle;      
  pimsmTimerData_t pimsmSGRptIPruneExpiryTimerParam;
} pimsmSGRptIEntry_t;
typedef struct pimsmSGRptNode_s
{
  L7_ushort16     flags;  /*Uses flags PIMSM_XXX*/
  pimsmSGRptEntry_t   pimsmSGRptEntry;
  pimsmSGRptIEntry_t * pimsmSGRptIEntry[MCAST_MAX_INTERFACES]; 
  /* Array of pimSGRptIEntry_t pointers*/
  /*L7_uint32     pimsmSGRptEntryTimer;*/
  /*struct pimsmSGRptNode_s     *pSelf;    For sanity checks */ 
  /* AVL TREE requires this as last */  
  void *avlData;
} pimsmSGRptNode_t;


extern
L7_RC_t pimsmSGRptInit(struct pimsmCB_s * pimsmCb);

extern
L7_RC_t pimsmSGRptDeInit(struct pimsmCB_s * pimsmCb);

extern
L7_RC_t pimsmSGRptAdd(struct pimsmCB_s * pimsmCb, pimsmSGRptNode_t *pSGRptNode);

extern
L7_RC_t pimsmSGRptDelete(struct pimsmCB_s * pimsmCb,
                         L7_inet_addr_t *pGrpAddr, 
                         L7_inet_addr_t *pSrcAddr);

extern
L7_RC_t pimsmSGRptModify(struct pimsmCB_s * pimsmCb, pimsmSGRptNode_t *pSGRptNode);

extern
L7_RC_t pimsmSGRptFirstGet(struct pimsmCB_s * pimsmCb, pimsmSGRptNode_t **ppSGRptNode);


extern
L7_RC_t pimsmSGRptNextGet(struct pimsmCB_s         *pimsmCb,
                          pimsmSGRptNode_t  *pSGRptNodeCurrent, 
                          pimsmSGRptNode_t **ppSGRptNode);

extern
L7_RC_t pimsmSGRptFind(struct pimsmCB_s         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,
                       L7_inet_addr_t    *pSrcAddr, 
                       pimsmSGRptNode_t **ppSGRptNode);

extern
L7_RC_t pimsmSGRptEntryFind(struct pimsmCB_s         *pimsmCb, 
                            pimsmSGRptNode_t  *pSGRptNodeCurrent,
                       L7_uint32          flag,
                       pimsmSGRptNode_t **ppSGRptNode);

extern
L7_RC_t pimsmSGRptNodeCreate(struct pimsmCB_s         *pimsmCb,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t    *pGrpAddr,
                             pimsmSGRptNode_t **ppSGRptNode);

extern
L7_RC_t pimsmSGRptIEntryCreate(struct pimsmCB_s        *pimsmCb, 
                               pimsmSGRptNode_t *pSGRptNode,
                               L7_uint32         rtrIfNum);

extern
L7_RC_t pimsmSGRptIEntryDelete(struct pimsmCB_s        *pimsmCb,
                        pimsmSGRptNode_t *pSGRptNode,
                               L7_uint32         rtrIfNum);

extern
L7_RC_t pimsmSGRptTreePurge(struct pimsmCB_s * pimsmCb);

extern
L7_RC_t  pimsmSGRptTreeIntfDownUpdate(struct pimsmCB_s * pimsmCb, L7_uint32 rtrIfNum);

extern
L7_RC_t pimsmSGRptIncludePruneInMsg(struct pimsmCB_s               *pimsmCb,
                                    struct pimsmStarGNode_s *pStarGNode);

extern
L7_RC_t pimsmSGRptStarGJoinRecvUpdate(struct pimsmCB_s      *pimsmCb,
                                      L7_inet_addr_t *pGrpAddr,
                                      L7_uint32       rtrIfNum,
                                      L7_short16      holdtime);

extern L7_RC_t pimsmSGRptMFCUpdate(struct pimsmCB_s      *pimsmCb,
                                pimsmSGRptNode_t *pSGRptNode,
                                mcastEventTypes_t eventType,
                                L7_BOOL bForceUpdate); 

extern L7_BOOL pimsmSGRptNodeTryRemove(struct pimsmCB_s    * pimsmCb, 
      pimsmSGRptNode_t *pSGRptNode);  

extern void pimsmSGRptNodeCleanup(struct pimsmCB_s * pimsmCb);      
extern L7_RC_t pimsmSGRptNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pGrpAddr);
extern L7_RC_t pimsmSGRptAdminScopeUpdate(pimsmCB_t *pimsmCb);

extern L7_RC_t pimsmSGRptTreeBestRouteChngUpdate(pimsmCB_t *pimsmCb, 
                      L7_inet_addr_t    *pGrpAddr,
                      L7_uint32 rtrIfNumRPFIndex,
                      RTO_ROUTE_EVENT_t  status);

extern L7_RC_t pimsmSGRptTreeRPChngUpdate(pimsmCB_t       *pimsmCb, 
                               L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pGrpAddr);
#endif /* _PIMSM_SG_RPT_TREE_H */
