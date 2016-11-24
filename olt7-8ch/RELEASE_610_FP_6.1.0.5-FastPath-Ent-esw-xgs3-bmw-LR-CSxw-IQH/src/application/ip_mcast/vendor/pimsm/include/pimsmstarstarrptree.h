/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarStarRpTree.h
*
* @purpose Contains function declarations to operate on (*,*,RP) linked list
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
#ifndef _PIMSM_STAR_STAR_RP_TREE_H
#define _PIMSM_STAR_STAR_RP_TREE_H
#include "l3_addrdefs.h"
#include "l7apptimer_api.h"
#include "pimsmdnstrmstarstarrpfsm.h"
#include "pimsmupstrmstarstarrpfsm.h"
#include "pimsmrp.h"
#include "pimsmcache.h"
#include "pimsmdefs.h"
#include "pimsmtimer.h"

typedef struct pimsmStarStarRpEntry_s
{
  L7_inet_addr_t pimsmStarStarRpRPAddress;
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE pimsmStarStarRpRPOrigin;
  L7_uint32      pimsmStarStarRpCreateTime;
  pimMode_t      pimsmStarStarRpPimMode;
  pimsmUpStrmStarStarRPStates_t pimsmStarStarRpUpstreamJoinState;
  L7_APP_TMR_HNDL_t      pimsmStarStarRpUpstreamJoinTimer;
  L7_uint32      pimsmStarStarRpUpstreamJoinTimerHandle;
  pimsmTimerData_t pimsmStarStarRpUpstreamJoinTimerParam;    
  L7_inet_addr_t pimsmStarStarRpUpstreamNeighbor;
  L7_uint32      pimsmStarStarRpRPFIfIndex;
  L7_inet_addr_t pimsmStarStarRpRPFNextHop;
  L7_RTO_PROTOCOL_INDICES_t pimsmStarStarRpRPFRouteProtocol;
  L7_inet_addr_t pimsmStarStarRpRPFRouteAddress;
  L7_uint32       pimsmStarStarRpRPFRoutePrefixLength;
  L7_uint32      pimsmStarStarRpRPFRouteMetricPref;
  L7_uint32      pimsmStarStarRpRPFRouteMetric;
  L7_short16     flags;
  interface_bitset_t  immediateOlist;
} pimsmStarStarRpEntry_t;

typedef struct pimsmStarStarRpIEntry_s
{
  L7_uint32   pimsmStarStarRpIIfIndex;
  L7_uint32      pimsmStarStarRpICreateTime;
/*  L7_BOOL             pimsmStarStarRpILocalMembership; */
  pimsmDnStrmPerIntfStarStarRPStates_t      pimsmStarStarRpIJoinPruneState;
  L7_APP_TMR_HNDL_t       pimsmStarStarRpIPrunePendingTimer;
  L7_uint32      pimsmStarStarRpIPrunePendingTimerHandle;    
  pimsmTimerData_t pimsmStarStarRpIPrunePendingTimerParam;   
  L7_APP_TMR_HNDL_t       pimsmStarStarRpIJoinExpiryTimer;
  L7_uint32      pimsmStarStarRpIJoinExpiryTimerHandle;    
  pimsmTimerData_t pimsmStarStarRpIJoinExpiryTimerParam;    
} pimsmStarStarRpIEntry_t;

/* Following data struct and APIs represents data and operations on Tree-A */

typedef struct pimsmStarStarRpNode_s 
{
  L7_sll_member_t      *next;/*Link to the next entry */
  L7_short16      flags;
  pimsmStarStarRpEntry_t   pimsmStarStarRpEntry;
  pimsmStarStarRpIEntry_t * pimsmStarStarRpIEntry[MCAST_MAX_INTERFACES]; 
      /* Array of pimsmStarStarRpI Pointers*/
  /*L7_uint32     pimsmStarStarRPEntryTimer; */
  L7_sll_t      pimsmStarStarRpCacheList;
  /*struct pimsmStarStarRpNode_s     *pSelf;    For sanity checks */       
} pimsmStarStarRpNode_t;

extern
L7_RC_t pimsmStarStarRpInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmStarStarRpDeInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmStarStarRpDelete(pimsmCB_t             *pimsmCb, 
                              pimsmStarStarRpNode_t *pStarStarRpNode);
                              
extern
L7_RC_t pimsmStarStarRpModify(pimsmCB_t             *pimsmCb, 
                              pimsmStarStarRpNode_t *pStarStarRpNode);

extern
L7_RC_t pimsmStarStarRpFirstGet(pimsmCB_t              *pimsmCb, 
                                pimsmStarStarRpNode_t **ppStarStarRpNode);

extern
L7_RC_t pimsmStarStarRpNextGet(pimsmCB_t             *pimsmCb,
                               pimsmStarStarRpNode_t *pStarStarRpNodeCurrent, 
                               pimsmStarStarRpNode_t **ppStarStarRpNode);

extern
L7_RC_t pimsmStarStarRpFind(pimsmCB_t              *pimsmCb, 
                            L7_inet_addr_t         *pRpAddr, 
                            pimsmStarStarRpNode_t **ppStarStarRpNode);

extern
L7_RC_t pimsmStarStarRpNodeCreate(pimsmCB_t              *pimsmCb,
                                  L7_inet_addr_t         *pRpAddr, 
                                  pimsmStarStarRpNode_t **ppStarStarRpNode);

extern
L7_RC_t pimsmStarStarRpIEntryCreate(pimsmCB_t             *pimsmCb, 
                                    pimsmStarStarRpNode_t *pStarStarRpNode, 
                                    L7_uint32              rtrIfNum);

extern
L7_RC_t pimsmStarStarRpIEntryDelete(pimsmCB_t             *pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                    L7_uint32              rtrIfNum);

extern L7_RC_t pimsmStarStarRpMFCUpdate(pimsmCB_t *pimsmCb,
                               pimsmStarStarRpNode_t *pStarStarRpNode, 
                               mcastEventTypes_t eventType,
                               L7_BOOL bForceUpdate);
extern
L7_RC_t pimsmStarStarRpPurge(pimsmCB_t * pimsmCb);

extern
L7_RC_t  pimsmStarStarRpTreeIntfDownUpdate(pimsmCB_t *pimsmCb, 
                                           L7_uint32  rtrIfNum);

extern
L7_RC_t pimsmStarStarRpTreeBestRouteChngUpdate(pimsmCB_t      *pimsmCb,
                                                 mcastRPFInfo_t *rpfRouteInfo);


extern L7_RC_t pimsmStarStarRpCacheInit(pimsmCB_t * pimsmCb, 
                                    struct pimsmStarStarRpNode_s * pStarStarRpNode);
extern L7_RC_t pimsmStarStarRpCacheDeInit(pimsmCB_t * pimsmCb, 
                               struct pimsmStarStarRpNode_s * pStarStarRpNode);

extern L7_RC_t pimsmStarStarRpCacheAdd (pimsmCB_t * pimsmCb, 
                                    struct pimsmStarStarRpNode_s * pStarStarRpNode,
                                    L7_inet_addr_t * srcAddr,
                                     L7_inet_addr_t      * pGrpAddr);
extern L7_RC_t pimsmStarStarRpCacheDelete(pimsmCB_t * pimsmCb,
                                      struct pimsmStarStarRpNode_s * pStarStarRpNode,
                                      pimsmCache_t  * kernelCacheEntry);
extern L7_RC_t pimsmStarStarRpCacheGetFirst(pimsmCB_t * pimsmCb, 
                                        struct pimsmStarStarRpNode_s * pStarStarRpNode,
                                        pimsmCache_t  ** kernelCacheEntry);
extern L7_RC_t pimsmStarStarRpCacheNextGet(pimsmCB_t * pimsmCb,
                                       struct pimsmStarStarRpNode_s * pStarStarRpNode,
                                       pimsmCache_t  * currentKernCacheEntry,
                                       pimsmCache_t  ** nextKernCacheEntry);
extern L7_RC_t pimsmStarStarRpCacheFind(pimsmCB_t * pimsmCb, 
                                    struct pimsmStarStarRpNode_s * pStarStarRpNode,
                                    L7_inet_addr_t    *source,
                                    L7_inet_addr_t      *group,
                                    pimsmCache_t  ** kernelCacheEntry);

extern L7_BOOL pimsmStarStarRpNodeTryRemove(pimsmCB_t * pimsmCb, 
        pimsmStarStarRpNode_t *pStarStarRpNode);                                    

extern L7_RC_t pimsmStarStarRpNegativeDelete (pimsmCB_t *pimsmCb,
                                       L7_inet_addr_t *pGrpAddr);

extern L7_RC_t pimsmStarStarRpCacheNegativeDelete (pimsmCB_t *pimsmCb,
                                       pimsmStarStarRpNode_t *pStarStarRpNode);

extern void pimsmStarStarRpNodeCleanup(pimsmCB_t * pimsmCb);        
extern void  pimsmStarStarRpNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry);


#endif /*_PIMSM_STAR_STAR_RP_TREE_H*/
