/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGTree.h
*
* @purpose Contains PIM-SM  avltree function declarations for (S,G) entry
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
#ifndef _PIMSM_SG_TREE_H
#define _PIMSM_SG_TREE_H
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "l7apptimer_api.h"
#include "pimsmtimer.h"
#include "pimsmdefs.h"
/*#include "pimsmmain.h"*/
#include "pimsmsgregisterfsm.h"
#include "pimsmdnstrmsgfsm.h"
#include "pimsmupstrmsgfsm.h"
#include "pimsmsgassertfsm.h"
#include "pimsmsgregisterfsm.h"



typedef struct pimsmSGEntry_s
{
  L7_inet_addr_t  pimsmSGGrpAddress;
  L7_inet_addr_t  pimsmSGSrcAddress;
  L7_uint32       pimsmSGCreateTime;
  pimMode_t       pimsmSGPimMode;
  pimsmUpStrmSGStates_t 
  pimsmSGUpstreamJoinState;
  L7_APP_TMR_HNDL_t pimsmSGUpstreamJoinTimer;
  L7_uint32  pimsmSGUpstreamJoinTimerHandle;
  pimsmTimerData_t  pimsmSGUpstreamJoinTimerParam;
  L7_inet_addr_t    pimsmSGUpstreamNeighbor;
  L7_uint32     pimsmSGRPFIfIndex;
  L7_inet_addr_t    pimsmSGRPFNextHop;
  L7_RTO_PROTOCOL_INDICES_t     
  pimsmSGRPFRouteProtocol;
  L7_inet_addr_t    pimsmSGRPFRouteAddress;
  L7_uint32     pimsmSGRPFRoutePrefixLength;   
  L7_uint32     pimsmSGRPFRouteMetricPref;
  L7_uint32     pimsmSGRPFRouteMetric;
  L7_BOOL       pimsmSGSPTBit;
  L7_APP_TMR_HNDL_t pimsmSGKeepaliveTimer;
  pimsmTimerData_t pimsmSGKeepaliveTimerParam;
  L7_uint32 pimsmSGKeepaliveTimerHandle;
  pimsmRegPerSGStates_t    
  pimsmSGDRRegisterState;
  L7_APP_TMR_HNDL_t pimsmSGDRRegisterStopTimer;
  pimsmTimerData_t pimsmSGDRRegisterStopTimerParam; 
  L7_uint32  pimsmSGDRRegisterStopTimerHandle;
  L7_inet_addr_t    pimsmSGRPRegisterPMBRAddress;
} pimsmSGEntry_t;

typedef struct pimsmSGIEntry_s
{
  L7_uint32     pimsmSGIIfIndex;
  L7_uint32       pimsmSGICreateTime;
  L7_BOOL       pimsmSGILocalMembership;
  pimsmDnStrmPerIntfSGStates_t    
  pimsmSGIJoinPruneState;
  L7_APP_TMR_HNDL_t pimsmSGIPrunePendingTimer;
  L7_uint32  pimsmSGIPrunePendingTimerHandle;   
  pimsmTimerData_t pimsmSGIPrunePendingTimerParam;  
  L7_APP_TMR_HNDL_t pimsmSGIJoinExpiryTimer;
  L7_uint32  pimsmSGIJoinExpiryTimerHandle;
  pimsmTimerData_t pimsmSGIJoinExpiryTimerParam;  
  pimsmPerIntfSGAssertStates_t      
  pimsmSGIAssertState;
  L7_APP_TMR_HNDL_t pimsmSGIAssertTimer;
  L7_uint32 pimsmSGIAssertTimerHandle;
  pimsmTimerData_t pimsmSGIAssertTimerParam;  
  L7_inet_addr_t    pimsmSGIAssertWinnerAddress;
  L7_uint32     pimsmSGIAssertWinnerMetricPref;
  L7_uint32     pimsmSGIAssertWinnerMetric;
} pimsmSGIEntry_t;

typedef struct pimsmSGNode_s 
{
  L7_ushort16      flags;  /*Uses flags PIMSM_XXX*/
  pimsmSGEntry_t    pimsmSGEntry;
  pimsmSGIEntry_t *   pimsmSGIEntry[MCAST_MAX_INTERFACES];
   /* Array of pimSGIEntry_t Pointers*/
 /* struct pimsmSGNode_s     *pSelf;    For sanity checks */ 
  /* AVL TREE requires this as last */  
  void *avlData;
 
} pimsmSGNode_t;

extern
L7_RC_t pimsmSGInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmSGDeInit(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmSGDelete(pimsmCB_t * pimsmCb,
                      L7_inet_addr_t *pGrpAddr,
                      L7_inet_addr_t *pSrcAddr);

extern
L7_RC_t pimsmSGModify(pimsmCB_t * pimsmCb, pimsmSGNode_t *pSGNode);

extern
L7_RC_t pimsmSGFirstGet(pimsmCB_t * pimsmCb, pimsmSGNode_t **ppSGNode);

extern
L7_RC_t pimsmSGNextGet(pimsmCB_t * pimsmCb,
                       pimsmSGNode_t   *pSGNodeCurrent,
                       pimsmSGNode_t **ppSGNode);

extern
L7_RC_t pimsmSGFind(pimsmCB_t * pimsmCb,
                    L7_inet_addr_t *pGrpAddr,
               L7_inet_addr_t *pSrcAddr, 
               pimsmSGNode_t **ppSGNode);

extern
L7_RC_t pimsmSGEntryFind(pimsmCB_t       *pimsmCb, 
                         pimsmSGNode_t   *pSGNodeCurrent,
                         L7_uint32        flag,
                       pimsmSGNode_t  **ppSGNode);

extern
L7_RC_t pimsmSGNodeCreate(pimsmCB_t * pimsmCb,
                          L7_inet_addr_t * pSrcAddr, 
                          L7_inet_addr_t * pGrpAddr, 
                          pimsmSGNode_t **ppSGNode,
                          L7_BOOL bStartKAT);

extern
L7_RC_t pimsmSGIEntryCreate(pimsmCB_t     *pimsmCb,
                            pimsmSGNode_t *pSGNode, 
                            L7_uint32      rtrIfNum);

extern
L7_RC_t pimsmSGIEntryDelete(pimsmCB_t *pimsmCb,
                            pimsmSGNode_t *pSGNode,
                            L7_uint32 rtrIfNum);

extern
L7_RC_t pimsmSGTreePurge(pimsmCB_t * pimsmCb);

extern
L7_RC_t pimsmSGLocalMemberShipChngEventPost(pimsmCB_t      *pimsmCb, 
                                            L7_inet_addr_t *pSrcAddr, 
                                            L7_inet_addr_t *pGrpAddr, 
                                            L7_uint32       rtrIfNum,
                                            L7_BOOL         bDelete,
                                            L7_BOOL jpMsgImdtSend);

extern
L7_RC_t pimsmSGMFCUpdate(pimsmCB_t     *pimsmCb,
                                pimsmSGNode_t *pSGNode,
                                mcastEventTypes_t eventType,
                                L7_BOOL bForceUpdate);

extern
L7_RC_t  pimsmSGTreeIntfDownUpdate(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum);

extern
L7_RC_t pimsmSGTreeBestRouteChngUpdate(pimsmCB_t * pimsmCb,
                                         mcastRPFInfo_t *pRPFRouteInfo);

extern
L7_RC_t pimsmSGTreeRPChngUpdate(pimsmCB_t       *pimsmCb, 
                                   L7_inet_addr_t *pGrpAddr,
                                   L7_uchar8       grpPrefixLen,
                                   L7_inet_addr_t *pRpAddr);

extern
L7_RC_t pimsmSGTreeJoinRecvEntryUpdate(pimsmCB_t      *pimsmCb,
                                       L7_inet_addr_t *pGrpAddr,
                                       L7_uint32       rtrIfNum);


extern
L7_RC_t pimsmSGTreePruneRecvEntryUpdate(pimsmCB_t      *pimsmCb,
                                        L7_inet_addr_t *pGrpAddr,
                                        L7_uint32       rtrIfNum);

extern L7_BOOL pimsmSGNodeTryRemove(pimsmCB_t * pimsmCb, 
                                          pimsmSGNode_t *pSGNode);
                                          
extern void pimsmSGNodeCleanup(pimsmCB_t * pimsmCb);
extern L7_RC_t pimsmSGNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pGrpAddr);
extern L7_RC_t pimsmSGAdminScopeUpdate(pimsmCB_t *pimsmCb);

extern void  pimsmSGNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry);

#endif /*_PIMSM_SG_TREE_H*/


