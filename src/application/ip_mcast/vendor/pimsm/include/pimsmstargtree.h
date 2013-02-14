/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarGTree.h
*
* @purpose Contains PIM-SM  avltree function declarations for (*,G) entry
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
#ifndef _PIMSM_STAR_G_TREE_H
#define _PIMSM_STAR_G_TREE_H
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "l7apptimer_api.h"
#include "pimsmcache.h"
#include "pimsmdnstrmstargfsm.h"
#include "pimsmupstrmstargfsm.h"
#include "pimsmstargassertfsm.h"
#include "pimsmrp.h" 
#include "pimsmdefs.h"
#include "pimsmtimer.h"


typedef struct pimsmStarGEntry_s
{
  L7_inet_addr_t    pimsmStarGGrpAddress;
  L7_uint32       pimsmStarGCreateTime;
  pimMode_t       pimsmStarGPimMode;
  L7_inet_addr_t    pimsmStarGRPAddress;
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE pimsmStarGRPOrigin;
  L7_BOOL       pimsmStarGRPIsLocal;
  pimsmUpStrmStarGStates_t pimsmStarGUpstreamJoinState;
  L7_APP_TMR_HNDL_t pimsmStarGUpstreamJoinTimer;
  L7_uint32 pimsmStarGUpstreamJoinTimerHandle;
  pimsmTimerData_t        pimsmStarGUpstreamJoinTimerParam;
  L7_inet_addr_t    pimsmStarGUpstreamNeighbor;
  L7_uint32       pimsmStarGRPFIfIndex;
  L7_inet_addr_t    pimsmStarGRPFNextHop;
  L7_RTO_PROTOCOL_INDICES_t pimsmStarGRPFRouteProtocol;
  L7_inet_addr_t    pimsmStarGRPFRouteAddress;
  L7_uint32       pimsmStarGRPFRoutePrefixLength;
  L7_uint32       pimsmStarGRPFRouteMetricPref;
  L7_uint32       pimsmStarGRPFRouteMetric;
} pimsmStarGEntry_t;

typedef struct pimsmStarGIEntry_s
{
  L7_uint32     pimsmStarGIIfIndex;
  L7_uint32       pimsmStarGICreateTime;  
  L7_BOOL       pimsmStarGILocalMembership;
  pimsmDnStrmPerIntfStarGStates_t     pimsmStarGIJoinPruneState;
  L7_APP_TMR_HNDL_t pimsmStarGIPrunePendingTimer;
  L7_uint32 pimsmStarGIPrunePendingTimerHandle;
  pimsmTimerData_t  pimsmStarGIPrunePendingTimerParam;
  L7_APP_TMR_HNDL_t pimsmStarGIJoinExpiryTimer;
  L7_uint32 pimsmStarGIJoinExpiryTimerHandle;
  pimsmTimerData_t pimsmStarGIJoinExpiryTimerParam;
  pimsmPerIntfStarGAssertStates_t       pimsmStarGIAssertState;
  L7_APP_TMR_HNDL_t pimsmStarGIAssertTimer;
  L7_uint32 pimsmStarGIAssertTimerHandle;   
  pimsmTimerData_t pimsmStarGIAssertTimerParam;
  L7_inet_addr_t    pimsmStarGIAssertWinnerAddress;
  L7_uint32       pimsmStarGIAssertWinnerMetricPref;
  L7_uint32       pimsmStarGIAssertWinnerMetric;
} pimsmStarGIEntry_t;

/* Following data struct and APIs represents data and operations on Tree-B */

typedef struct pimsmStarGNode_s
{
  L7_ushort16     flags;  /*Uses flags PIMSM_XXX*/
  pimsmStarGEntry_t   pimsmStarGEntry;
  pimsmStarGIEntry_t * pimsmStarGIEntry[MCAST_MAX_INTERFACES]; /* List of pimStarGIEntry_t */
  L7_sll_t      pimsmStarGCacheList;
 /* L7_uint32     pimsmStarGEntryTimer;*/
  /*struct pimsmStarGNode_s     *pSelf;    For sanity checks */ 
  /* AVL TREE requires this as last */  
  void *avlData;
  
} pimsmStarGNode_t;

extern
L7_RC_t pimsmStarGInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmStarGDeInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmStarGDelete(pimsmCB_t * pimsmCb, L7_inet_addr_t *pGrpAddr);

extern
L7_RC_t pimsmStarGModify(pimsmCB_t * pimsmCb, pimsmStarGNode_t *pStarGNode);

extern
L7_RC_t pimsmStarGFirstGet(pimsmCB_t * pimsmCb, pimsmStarGNode_t **ppStarGNode);

extern
L7_RC_t pimsmStarGNextGet(pimsmCB_t         *pimsmCb,
                          pimsmStarGNode_t  *pStarGNodeCurrent, 
                          pimsmStarGNode_t **ppStarGNode);
extern
L7_RC_t pimsmStarGFind(pimsmCB_t         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,  
                       pimsmStarGNode_t **ppStarGNode);

extern
L7_RC_t pimsmStarGEntryFind(pimsmCB_t        *pimsmCb, 
                            pimsmStarGNode_t *starGNodeCurrent,
                            L7_uint32         flag,
                     pimsmStarGNode_t **ppStarGNode);

extern
L7_RC_t pimsmStarGNodeCreate(pimsmCB_t * pimsmCb,
                             L7_inet_addr_t * pGrpAddr,
                             pimsmStarGNode_t **ppStarGNode);

extern
L7_RC_t pimsmStarGIEntryCreate(pimsmCB_t        *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                               L7_uint32         rtrIfNum);

extern
L7_RC_t pimsmStarGIEntryDelete(pimsmCB_t        *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                               L7_uint32         rtrIfNum);

extern
L7_RC_t pimsmStarGTreePurge(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmStarGLocalMemberShipChngEventPost(pimsmCB_t * pimsmCb, 
                                           L7_inet_addr_t * pGrpAddr, 
                                           L7_uint32 rtrIfNum,
                                           L7_BOOL isStarGIEntryDeleted,
                                           L7_BOOL starGILocalMemberShip,
                                           L7_BOOL jpMsgImdtSend);
extern
L7_RC_t  pimsmStarGTreeIntfDownUpdate(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum);

extern
L7_RC_t pimsmStarGTreeBestRouteChngUpdate(pimsmCB_t * pimsmCb, 
                                             mcastRPFInfo_t *rpfRouteInfo);

extern
L7_RC_t pimsmStarGTreeRPChngUpdate(pimsmCB_t * pimsmCb, 
                                      L7_inet_addr_t   * pGrpAddr,
                                      L7_uint32 grpPrefixLen,
                                      L7_inet_addr_t   * pRpAddr);

extern
void pimsmStarGTreeStarStarRpJoinRecvUpdate(pimsmCB_t * pimsmCb, 
                                             L7_inet_addr_t * pRpAddr, 
                                             L7_uint32 rtrIfNum);   

extern L7_RC_t pimsmStarGKernelCacheCountUpdate(pimsmCB_t * pimsmCb, 
                                           struct pimsmStarGNode_s * pStarGNode,
                                           L7_inet_addr_t    *source,
                                           L7_uint32 len);

extern L7_RC_t pimsmStarGMFCUpdate(pimsmCB_t *pimsmCb,
                                      struct pimsmStarGNode_s *pStarGNode,
                                      mfcOperationType_t eventType,
                                      L7_BOOL bForceUpdate);


extern L7_RC_t pimsmStarGCacheInit(pimsmCB_t * pimsmCb, 
                                    struct pimsmStarGNode_s * pStarGNode);
extern L7_RC_t pimsmStarGCacheDeInit(pimsmCB_t * pimsmCb, 
                               struct pimsmStarGNode_s * pStarGNode);

extern L7_RC_t pimsmStarGCacheAdd (pimsmCB_t * pimsmCb, 
                                    struct pimsmStarGNode_s * pStarGNode,
                                    L7_inet_addr_t * srcAddr);
extern L7_RC_t pimsmStarGCacheDelete(pimsmCB_t * pimsmCb,
                                      struct pimsmStarGNode_s * pStarGNode,
                                      pimsmCache_t  * kernelCacheEntry);
extern L7_RC_t pimsmStarGCacheGetFirst(pimsmCB_t * pimsmCb, 
                                        struct pimsmStarGNode_s * pStarGNode,
                                        pimsmCache_t  ** kernelCacheEntry);
extern L7_RC_t pimsmStarGCacheNextGet(pimsmCB_t * pimsmCb,
                                       struct pimsmStarGNode_s * pStarGNode,
                                       pimsmCache_t  * currentKernCacheEntry,
                                       pimsmCache_t  ** nextKernCacheEntry);
extern L7_RC_t pimsmStarGCacheFind(pimsmCB_t * pimsmCb, 
                                    struct pimsmStarGNode_s * pStarGNode,
                                    L7_inet_addr_t    *source,
                                    L7_inet_addr_t      *group,
                                    pimsmCache_t  ** kernelCacheEntry);

extern L7_BOOL pimsmStarGNodeTryRemove(pimsmCB_t * pimsmCb, 
      pimsmStarGNode_t *pStarGNode);   

extern void pimsmStarGNodeCleanup(pimsmCB_t * pimsmCb);      
extern L7_RC_t pimsmStarGNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pRpAddr);
extern L7_RC_t pimsmStarGAdminScopeUpdate(pimsmCB_t *pimsmCb);
extern void  pimsmStarGNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry);

#endif /*_PIMSM_STAR_G_TREE_H */
