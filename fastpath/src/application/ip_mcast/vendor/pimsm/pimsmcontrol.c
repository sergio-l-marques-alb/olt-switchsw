/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmControl.c
*
* @purpose  APIs for various event handlers.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana /nramu
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmmain.h"
#include "pimsmcontrol.h"
#include "l7_ip_api.h"
#include "rto_api.h"
#include "pimsmmrt.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmbsr.h"
#include "pimsmintf.h"
#include "pimsmrp.h"
#include "l7_pimsm_api.h"
#include "mcast_wrap.h"
#include "pimsmwrap.h"
#include "mfc_api.h"


/*********************************************************************
* @purpose  No-cache event handler -(S,G,rpt) part
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)}  (S,G,rpt) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number  
* @param    bCreate     @b{(input)} 
 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGRptMfcNoCacheProcess(pimsmCB_t *pimsmCb,
                                           pimsmSGRptNode_t *pSGRptNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  return pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_TRUE);
}



/*********************************************************************
* @purpose  No-cache event handler -(*,*,RP) part
*
* @param    pimsmCb              @b{(input)}  control block  
* @param    pStarStarRpNode      @b{(input)}  (*,*,RP) node
* @param    pSrcAddr             @b{(input)}  source address
* @param    pGrpAddr             @b{(input)}  group address
* @param    rtrIfNum             @b{(input)}  router interface number  
* @param    bCreate              @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ANVL section 13 needs data fowarding based on (*,*,RP) entries 
*           Hence the following code is required.
* @end
*********************************************************************/
static L7_RC_t pimsmStarStarRpMfcNoCacheProcess(pimsmCB_t *pimsmCb,
                      pimsmStarStarRpNode_t *pStarStarRpNode,
                      L7_inet_addr_t *pSrcAddr,
                      L7_inet_addr_t *pGrpAddr,
                      L7_uint32 rtrIfNum)
{
/* ANVL section 13 needs data fowarding based on (*,*,RP) entries 
   Hence the following code is required.*/
  if(pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex != 0)
  {
  if(pimsmStarStarRpCacheAdd(pimsmCb,pStarStarRpNode, pSrcAddr, pGrpAddr)
                          != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarStarRpCacheAdd failed");
    return L7_FAILURE;
  }

  pimsmStarStarRpMFCUpdate(pimsmCb, pStarStarRpNode, MFC_UPDATE_ENTRY, L7_TRUE); 
  }
  pimsmCheckSwitchToSpt(pimsmCb,pSrcAddr,pGrpAddr);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose wrong-iif event handler -(S,G) part 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode    @b{(input)}   (S,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
   if( iif is in inherited_olist(S,G) ) {
         send Assert(S,G) on iif
     }

* @end
*********************************************************************/
static L7_RC_t pimsmSGSSMMFCAssertGenerate(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  interface_bitset_t oif;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;

 memset(&oif,0,sizeof(interface_bitset_t));
 pimsmSGInhrtdOlist(pimsmCb,pSGNode,&oif);
 if((BITX_TEST(&oif,rtrIfNum) != L7_NULL))
 {
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
    "Incoming interface is present in SGInhrtdOlist");  
      /* data recv , trigger assert fsm */
  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  sgAssertEventInfo.eventType = 
        PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT;
  pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
 } 

 return L7_SUCCESS; 
}

/*********************************************************************
* @purpose wrong-iif event handler -(S,G) part 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode    @b{(input)}   (S,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
    if ( SPTbit(S,G) == TRUE AND iif is in inherited_olist(S,G) ) {
           send Assert(S,G) on iif
        } else if ( SPTbit(S,G) == FALSE AND
                    iif is in inherited_olist(S,G,rpt) {
           send Assert(*,G) on iif
        }
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGMFCAssertGenerate(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  interface_bitset_t oif;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  L7_RC_t rc;  
  

 memset(&oif,0,sizeof(interface_bitset_t));
 pimsmSGInhrtdOlist(pimsmCb,pSGNode,&oif);
 if((pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_TRUE) &&
    (BITX_TEST(&oif,rtrIfNum) != L7_NULL))
 {
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
    "Incoming interface is present in SGInhrtdOlist & SPTBit is set");  
      /* data recv , trigger assert fsm */
  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  sgAssertEventInfo.eventType = 
        PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT;
  pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
 } 
 else
 {
   memset(&oif,0,sizeof(interface_bitset_t));
   rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
   if(rc == L7_SUCCESS)
   {
     pimsmSGRptInhrtdOlist(pimsmCb,pSGRptNode,&oif);
   }
   if((pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_FALSE) &&
      (BITX_TEST(&oif,rtrIfNum) != L7_NULL))
   {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
     "Incoming interface is present in SGRptInhrtdOlist & SPTBit is not set");  
     if(pimsmStarGFind(pimsmCb,pGrpAddr,&pStarGNode) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"(*,G) node not found");
       return L7_FAILURE;
     }
     /* data recv , trigger assert fsm */
     memset(&starGAssertEventInfo, 0, 
          sizeof(pimsmPerIntfStarGAssertEventInfo_t));
     starGAssertEventInfo.rtrIfNum = rtrIfNum;
     inetCopy(&starGAssertEventInfo.srcAddr, pSrcAddr);
     starGAssertEventInfo.eventType = 
         PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_DATA_PKT;
     pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,
          &starGAssertEventInfo);
   }
 }
 return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  No-cache event handler -(*,G) part 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarGMfcNoCacheProcess(pimsmCB_t *pimsmCb,
                                           pimsmStarGNode_t *pStarGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  /* if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex != 0) */
  {
  if(pimsmStarGCacheAdd(pimsmCb,pStarGNode,pSrcAddr) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGCacheAdd failed");
    return L7_FAILURE;
  }

  pimsmStarGMFCUpdate(pimsmCb, pStarGNode, MFC_UPDATE_ENTRY, L7_TRUE);
  }
  pimsmCheckSwitchToSpt(pimsmCb,pSrcAddr,pGrpAddr);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  No-cache event handler -(*,G) part 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmSendPruneToRP(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_inet_addr_t *pRpAddr,
                                           L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  L7_inet_addr_t *pNbrAddr;
  L7_uint32  rtrIfNumNew = 0;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;  
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;    
  
    /*Find (*,G) Entry/(*,*,RP) entry.Check if the incoming 
      interface is different from the incoming interface in the 
      matched entry.If yes,set the SPT bit and fire a join prune 
      timer towards the RP */
      if(pimsmStarGFind(pimsmCb,pGrpAddr,&pStarGNode) == L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"(*,G) node found");
        rtrIfNumNew = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;
        pNbrAddr = &(pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor);
        /*rc = pimsmStarGCacheAdd( pimsmCb, pStarGNode, pSrcAddr );
        if(rc != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                     "pimsmStarGCacheAdd() failed");
        }*/
      }
#ifdef PIMSM_TBD 

  pimsmStarStarRpNode_t *pStarStarRpNode = L7_NULLPTR;

    /* TODO: I think we can relax the rule to sende prune
       if (S,G)incoming is different from (*,*,RP) incoming.
       As (*,*,RP) is not well tested in real scenario.
       -Satya */
      else
      {
        if(pimsmStarStarRpFind(pimsmCb,pRpAddr,&pStarStarRpNode) 
                                 == L7_SUCCESS)
        {
          rtrIfNumNew = 
          pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
          pNbrAddr = &(pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamNeighbor);
          /*rc = pimsmStarStarRpCacheAdd(pimsmCb, pStarStarRpNode, pSrcAddr, pGrpAddr);
          if(rc != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                          "pimsmStarStarRpCacheAdd() failed");
          }*/
        }
        else
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
                         "(*,G) or (*,*,RP) entry not found");
          return;
        }
      }
#endif      
      if ((rtrIfNumNew != 0) && (rtrIfNumNew != rtrIfNum))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
         "Incoming interface is different from (*,G) or (*,*,RP)'s upstream index");
        pimsmSPTbitUpdate(pimsmCb, pSGNode, rtrIfNum);
        /*Trigger a Prune towards the RP */
        if(pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr,&pSGRptNode) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGRptFind failed");
          return;
        }        
        /*Also send a (S,G)Rpt Prune towards the RP */
        memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
        pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
        if (pruneDesired ==  L7_TRUE)
        {
          upStrmSGRptEventInfo.eventType 
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
          pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);    
        }
        else
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                        "pimsmSGRptPruneDesired () is false");
        }
      }
}
/*********************************************************************
* @purpose  No-cache event handler -(S,G) part  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)}   (S,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
4.8.2.  PIM-SSM-Only Routers
Page 107
   The Keepalive Timer should be treated as always running, and SPTbit
   should be treated as always being set for an SSM address.
   Additionally, the Packet forwarding rules of Section 4.2 can be
   simplified in a PIM-SSM-only router:

     if( iif == RPF_interface(S) AND UpstreamJPState(S,G) == Joined ) {
         oiflist = inherited_olist(S,G)
     } else if( iif is in inherited_olist(S,G) ) {
         send Assert(S,G) on iif
     }

     oiflist = oiflist (-) iif
     forward packet on all interfaces in oiflist

   This is nothing more than the reduction of the normal PIM-SM
   forwarding rule, with all (S,G,rpt) and (*,G) clauses replaced with
   NULL.
* @end
*********************************************************************/
static L7_RC_t pimsmSGSSMMfcNoCacheProcess(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  L7_uint32 rtrIfNumIn =0;
  L7_uint32 upstreamJoinState =0;
  interface_bitset_t inhrtSGOif, *pOif, zeroOif;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, 
      PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION,
      PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
     
  rtrIfNumIn = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;
  memset(&zeroOif,0,sizeof(interface_bitset_t));
  memset(&inhrtSGOif,0,sizeof(interface_bitset_t));
  pOif =&zeroOif;
  pimsmSGInhrtdOlist(pimsmCb,pSGNode,&inhrtSGOif);
  upstreamJoinState = pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinState;
  pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
  if((rtrIfNumIn == rtrIfNum) &&
     (upstreamJoinState == PIMSM_UPSTRM_S_G_SM_STATE_JOINED))  
  {
   pOif = &inhrtSGOif;
  }
  else if((BITX_TEST(&inhrtSGOif,rtrIfNum) != L7_NULL))
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
     "Incoming interface is present in SGInhrtdOlist");  
       /* data recv , trigger assert fsm */
   memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
   sgAssertEventInfo.rtrIfNum = rtrIfNum;
   sgAssertEventInfo.eventType = 
         PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT;
   pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
  } 
  BITX_RESET(pOif,rtrIfNum);
  if(pimsmMfcQueue(pimsmCb,MFC_UPDATE_ENTRY,pSrcAddr,pGrpAddr,rtrIfNum,
                    pOif, L7_TRUE, &pSGNode->flags) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
         "pimsmMfcQueue failed");
    return L7_FAILURE;
  }  
  return L7_SUCCESS;    
}
/*********************************************************************
* @purpose  No-cache event handler -(S,G) part  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)}   (S,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
   On receipt of data from S to G on interface iif:
    if( DirectlyConnected(S) == TRUE AND iif == RPF_interface(S) ) {
         set KeepaliveTimer(S,G) to Keepalive_Period
         # Note: a register state transition or UpstreamJPState(S,G)
         # transition may happen as a result of restarting
         # KeepaliveTimer, and must be dealt with here.
    }

   if( iif == RPF_interface(S) AND UpstreamJPState(S,G) == Joined AND
      inherited_olist(S,G) != NULL ) {
          set KeepaliveTimer(S,G) to Keepalive_Period
   }

   Update_SPTbit(S,G,iif)
   oiflist = NULL

   if( iif == RPF_interface(S) AND SPTbit(S,G) == TRUE ) {
      oiflist = inherited_olist(S,G)
   } else if( iif == RPF_interface(RP(G)) AND SPTbit(S,G) == FALSE) {
     oiflist = inherited_olist(S,G,rpt)
     CheckSwitchToSpt(S,G)
   } else {
       if ( SPTbit(S,G) == TRUE AND iif is in inherited_olist(S,G) ) {
           send Assert(S,G) on iif
        } else if ( SPTbit(S,G) == FALSE AND
                    iif is in inherited_olist(S,G,rpt) {
           send Assert(*,G) on iif
        }
  }
* @end
*********************************************************************/
static L7_RC_t pimsmSGMfcNoCacheProcess(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  L7_uint32 rtrIfNumIn =0, rtrIfNumRp = 0;
  L7_uint32 upstreamJoinState =0;
  pimsmPerSGRegisterEventInfo_t registerEventInfo;
  L7_inet_addr_t  rpAddr;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  interface_bitset_t oif;
  L7_BOOL result= L7_FALSE;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  pimsmStarStarRpNode_t *pStarStarRpNode = L7_NULLPTR;
  L7_BOOL isSSMAddr = L7_FALSE;
  L7_RC_t  rc = L7_FAILURE;
  pimsmCache_t *kernelelCache = L7_NULLPTR;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, 
      PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION,
      PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,
      PIMSM_TRACE_INFO, "rtrIfNum : %d",rtrIfNum);

  /*Note: Directly connect is already tested before calling */

   rtrIfNumIn = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;

   isSSMAddr = pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr);
   if ( isSSMAddr == L7_TRUE)
   {
      return pimsmSGSSMMfcNoCacheProcess(pimsmCb,pSGNode,
                                  pSrcAddr,pGrpAddr,rtrIfNum);
   }
   if(pimsmRpAddressGet(pimsmCb, pGrpAddr, &rpAddr) != L7_SUCCESS)
   {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,  PIMSM_TRACE_ERROR, "RP not found" );
     return L7_FAILURE;
   }  
   if(rtrIfNumIn == rtrIfNum)
   {
     /*Start the keepalive/entry timer */
     pimsmRestartKeepaliveTimer(pimsmCb, pSGNode, PIMSM_DEFAULT_KEEPALIVE_PERIOD);
     if(mcastIpMapUnnumberedIsLocalAddress(&rpAddr,L7_NULLPTR) == L7_FALSE)
     {
       /* set reg_vif_num as outgoing interface ONLY 
         if I am not the RP */
       if(pimsmCouldRegister(pimsmCb,pSGNode) == L7_TRUE)
       {
         /*Execute Register FSM */
         memset(&registerEventInfo,0,sizeof(pimsmPerSGRegisterEventInfo_t));
         registerEventInfo.eventType = 
         PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_TRUE; 
         pimsmPerSGRegisterExecute(pimsmCb,pSGNode,&registerEventInfo);
       }
     }  
     else
     {
       /* remove reg_vif_num from outgoing interface ONLY 
         if I am the RP */
       if(pimsmCouldRegister(pimsmCb,pSGNode) == L7_FALSE)
       {
         /*Execute Register FSM */
         memset(&registerEventInfo,0,sizeof(pimsmPerSGRegisterEventInfo_t));
         registerEventInfo.eventType = 
         PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE; 
         pimsmPerSGRegisterExecute(pimsmCb,pSGNode,&registerEventInfo);
       }
     }
 
     memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
     joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
     if (joinDesired == L7_TRUE)
     {
       upStrmSGEventInfo.eventType 
       = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
     }
     else
     {
       upStrmSGEventInfo.eventType 
       = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
     }
     pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);    
   }

  memset(&oif,0,sizeof(interface_bitset_t));
  pimsmSGInhrtdOlist(pimsmCb,pSGNode,&oif);
  upstreamJoinState = pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinState;
  
  BITX_IS_EMPTY(&oif,result);  

  if((rtrIfNumIn == rtrIfNum) &&
     (upstreamJoinState == PIMSM_UPSTRM_S_G_SM_STATE_JOINED)&&
     (result == L7_FALSE))
  {
    pimsmRestartKeepaliveTimer(pimsmCb, pSGNode, PIMSM_DEFAULT_KEEPALIVE_PERIOD);
  }      
  pimsmSPTbitUpdate(pimsmCb, pSGNode, rtrIfNum);  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, 
      PIMSM_TRACE_INFO,"(S,G)SPT-bit = %d", pSGNode->pimsmSGEntry.pimsmSGSPTBit);

  if ((pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode) == L7_SUCCESS) &&
      (pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex
       != MCAST_MAX_INTERFACES))
  {
    rtrIfNumRp = pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
    rc = L7_SUCCESS;
  }

  if((rtrIfNumIn == rtrIfNum) &&
     (pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_TRUE))
  {  
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"Pure (S,G) entry fwding");
    pimsmSendPruneToRP(pimsmCb,pSGNode, pSrcAddr,pGrpAddr, 
            &rpAddr,rtrIfNum);
    memset(&oif,0,sizeof(interface_bitset_t));  
    pimsmSGInhrtdOlist(pimsmCb,pSGNode,&oif);
  }
  else if(((rc == L7_SUCCESS) && (rtrIfNumRp == rtrIfNum)) &&
          (pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_FALSE))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"SPT Bit is false");
    memset(&oif,0,sizeof(interface_bitset_t));
    if(pimsmSGRptFind(pimsmCb,pGrpAddr,pSrcAddr,&pSGRptNode) == L7_SUCCESS)
    {
        pimsmSGRptInhrtdOlist(pimsmCb,pSGRptNode,&oif);
    }    
    pimsmCheckSwitchToSpt(pimsmCb,pSrcAddr,pGrpAddr); 
    pimsmSendPruneToRP(pimsmCb,pSGNode, pSrcAddr,pGrpAddr, 
            &rpAddr,rtrIfNum);      
  } 
  else 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
             "Incoming interface is not equal to rtrIfNum");
  }

       /*TODO: delete (*,*,RP) cache */
  if (pStarStarRpNode != L7_NULLPTR)
  {
    rc = pimsmStarStarRpCacheFind(pimsmCb, pStarStarRpNode, pSrcAddr, pGrpAddr, 
        &kernelelCache);
    if(rc == L7_SUCCESS)
    {
      rc = pimsmStarStarRpCacheDelete( pimsmCb, pStarStarRpNode, kernelelCache );
      if(rc != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                   "pimsmStarStarRpCacheDelete() failed");
      }
    }
  }
  BITX_RESET(&oif,rtrIfNumIn);

  if (pimsmMfcQueue(pimsmCb,MFC_UPDATE_ENTRY,pSrcAddr,pGrpAddr,rtrIfNumIn,
                    &oif, L7_TRUE, &pSGNode->flags) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"pimsmMfcQueue failed");
    return L7_FAILURE;
  }

  /*   
   *  During the Warm Restart period, if a S,G node's Register State Machine is
   *  in JOIN, send a Null Register packet to trigger the RP to send a register stop
   *  if S,G should be forwarded on SPT.  This allows a restarting backup manager to
   *  recreate the correct forwarding state.
   */
  if ((pimsmCb->pimsmRestartInProgress == L7_TRUE) && 
      (pSGNode->pimsmSGEntry.pimsmSGDRRegisterState == PIMSM_REG_PER_S_G_SM_STATE_JOIN))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, 
                "Warm Restart: sending NULL register packet to RP");

    pimsmNullRegisterSend(pimsmCb, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                          &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

  }

  return L7_SUCCESS;  
}
/****************************************************************************
* @purpose  No-cache event handler
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  incoming router interface number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
        On receipt of data from S to G on interface iif:
         if( DirectlyConnected(S) == TRUE AND iif == RPF_interface(S) ) {
              set KeepaliveTimer(S,G) to Keepalive_Period
              # Note: a register state transition or UpstreamJPState(S,G)
              # transition may happen as a result of restarting
              # KeepaliveTimer, and must be dealt with here.
         }


         if( iif == RPF_interface(S) AND UpstreamJPState(S,G) == Joined AND      |
            inherited_olist(S,G) != NULL ) {                                     |
                set KeepaliveTimer(S,G) to Keepalive_Period                      |
         }                                                                       |

         Update_SPTbit(S,G,iif)
* @end
******************************************************************************/ 
L7_RC_t pimsmMfcNoCacheProcess(pimsmCB_t * pimsmCb, L7_inet_addr_t * pSrcAddr, 
                               L7_inet_addr_t * pGrpAddr, L7_uint32 rtrIfNum)
{
  pimsmSGNode_t * pSGNode=L7_NULLPTR;
  L7_uint32 rtrIfNumTmp;
  pimsmLongestMatch_t pimsmMRTNode;
  L7_RC_t rc;
  pimsmInterfaceEntry_t *pIntfEntry=L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"Entry");

  if(pSrcAddr == L7_NULLPTR ||
     pGrpAddr == L7_NULLPTR ||
     pimsmCb ==(pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
                "pimsmMfcNoCacheProcess rcvd CB as NULLPTR");
    return L7_FAILURE;
  }

  /* Check if PIM-SM is Active on the Incoming Interface.
   */

  if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, "PIM-SM is Not Active on Interface - %d",
                 rtrIfNum);
    return L7_FAILURE;
  }


  if(inetUnNumberedIsLocalAddress(pSrcAddr, rtrIfNum) != L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,  PIMSM_TRACE_ERROR,
       "PIMSM: Received srcAddr is local Addr.\n");
  }
  pIntfEntry->pimsmPerIntfStats.pimsmNoCacheRx ++;

  memset(&pimsmMRTNode,0,sizeof(pimsmLongestMatch_t));

  if((pimsmIAmDR(pimsmCb,rtrIfNum) == L7_TRUE ) && 
     (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE) &&
     (mcastIpMapIsDirectlyConnected(pSrcAddr,&rtrIfNumTmp) == L7_TRUE ) &&
     rtrIfNum == rtrIfNumTmp)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                "I am the DR and it is from a directly connected host");
    rc = pimsmSGNodeCreate(pimsmCb, pSrcAddr, pGrpAddr, &pSGNode,L7_TRUE);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
                  "pimsmSGNodeCreate () failed");
      return L7_FAILURE;
    }
    pimsmMRTNode.entryType = PIMSM_ENTRY_TYPE_S_G;
    pimsmMRTNode.mrtNode = (void*)pSGNode;    
  }
  else
  {
    if(pimsmMRTFind(pimsmCb,pGrpAddr,pSrcAddr,&pimsmMRTNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
                  "Matching Entry not found...Drop the packet");
      return L7_FAILURE;
    }
  }
  switch(pimsmMRTNode.entryType)
  {
    case PIMSM_ENTRY_TYPE_S_G:
      if(pimsmSGMfcNoCacheProcess(pimsmCb,pimsmMRTNode.mrtNode,
                                  pSrcAddr,pGrpAddr,rtrIfNum) 
                                  != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
    case PIMSM_ENTRY_TYPE_STAR_G:
      if(pimsmStarGMfcNoCacheProcess(pimsmCb,pimsmMRTNode.mrtNode,pSrcAddr,
                                     pGrpAddr,rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
    case PIMSM_ENTRY_TYPE_STAR_STAR_RP:
      if(pimsmStarStarRpMfcNoCacheProcess(pimsmCb,pimsmMRTNode.mrtNode,
                                          pSrcAddr,pGrpAddr,rtrIfNum) 
                                          != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarStarMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
    case PIMSM_ENTRY_TYPE_S_G_RPT:
      if(pimsmSGRptMfcNoCacheProcess(pimsmCb,pimsmMRTNode.mrtNode,
                                     pSrcAddr,pGrpAddr,rtrIfNum) 
                                     != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGRptMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
      
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  wrong-iif event handler -(*,G) part
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarGMfcWrongiifProcess(pimsmCB_t *pimsmCb,
                                           pimsmStarGNode_t *pStarGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  interface_bitset_t oif;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"RtrIfNum is %d",rtrIfNum);
  
  memset(&oif,0,sizeof(interface_bitset_t));
  pimsmStarGImdtOlist(pimsmCb,pStarGNode,&oif);
  
  if(BITX_TEST(&oif,rtrIfNum) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
      "Incoming interface is present in StarGImdtOlist");  
    /* data recv , trigger assert fsm */
    memset(&starGAssertEventInfo, 0, 
         sizeof(pimsmPerIntfStarGAssertEventInfo_t));
    starGAssertEventInfo.rtrIfNum = rtrIfNum;
    inetCopy(&starGAssertEventInfo.srcAddr, pSrcAddr);
    starGAssertEventInfo.eventType = 
        PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_DATA_PKT;
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,
         &starGAssertEventInfo);
    return L7_SUCCESS;
  }
 return L7_SUCCESS;
}

/*********************************************************************
* @purpose wrong-iif event handler -(S,G) part 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode    @b{(input)}   (S,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGMfcWrongiifProcess(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           L7_inet_addr_t *pSrcAddr,
                                           L7_inet_addr_t *pGrpAddr,
                                           L7_uint32 rtrIfNum)
{
  interface_bitset_t oif;
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;  
  L7_BOOL result = L7_FALSE;
  pimsmCache_t  * kernelelCache;
  L7_RC_t rc;  
  L7_BOOL isSSMAddr = L7_FALSE;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGNode)
     || (L7_NULLPTR == pSrcAddr) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
  isSSMAddr = pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr);  
  if ( isSSMAddr == L7_TRUE)
  {
     return pimsmSGSSMMFCAssertGenerate(pimsmCb,pSGNode,
                                 pSrcAddr,pGrpAddr,rtrIfNum);
  }
  pimsmSGMFCAssertGenerate(pimsmCb, pSGNode, pSrcAddr, pGrpAddr, rtrIfNum);
 
  if(pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"SPTbit is not set"); 
    if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                  "incoming interface is equal to upstream index");  
      memset(&oif,0,sizeof(interface_bitset_t));
      pimsmSGInhrtdOlist(pimsmCb,pSGNode,&oif);
      BITX_RESET(&oif,rtrIfNum);
      BITX_IS_EMPTY(&oif,result);
      if(result == L7_FALSE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"OifList is not empty"); 
        pimsmSPTbitUpdate(pimsmCb, pSGNode, rtrIfNum);
        if(pimsmStarGFind(pimsmCb,pGrpAddr,&pStarGNode) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGFind failed");
          return L7_FAILURE;
        }        
        if(pimsmSGRptFind(pimsmCb,pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGRptFind failed");
          return L7_FAILURE;
        }        
        /*Also send a (S,G)Rpt Prune towards the RP */
        memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
        pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
        if (pruneDesired ==  L7_TRUE)
        {
          upStrmSGRptEventInfo.eventType 
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
          pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);
        }
        else
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                        "pimsmSGRptPruneDesired () is false");
        }   
        /*TODO: delete (*,G) cache */
        rc = pimsmStarGCacheFind(pimsmCb, pStarGNode, pSrcAddr, pGrpAddr, 
         &kernelelCache);
        if(rc == L7_SUCCESS)
        {
           rc = pimsmStarGCacheDelete( pimsmCb, pStarGNode, kernelelCache );
           if(rc != L7_SUCCESS)
           {
             PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                        "pimsmStarGCacheDelete() failed");
           }
        }
        else 
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                     "pimsmStarGCacheFind() failed");
        }        
      }
      pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_TRUE);
    }
  }
 return L7_SUCCESS;
}

/*********************************************************************
* @purpose entry-expire event handler 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t pimsmMfcEntryExpireProcess(pimsmCB_t * pimsmCb, L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pGrpAddr)
{
  pimsmStarGNode_t *pStarGNode=L7_NULLPTR;
  pimsmSGNode_t *pSGNode=L7_NULLPTR;  
  pimsmCache_t  *kernelCache=L7_NULLPTR;  
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t rc1 = L7_FAILURE;  

  rc = pimsmSGFind( pimsmCb, pGrpAddr,  pSrcAddr, &pSGNode );
  if(rc == L7_SUCCESS)
  {
    if((pSGNode->flags & PIMSM_ADDED_TO_MFC) != L7_NULL )
    {
      pSGNode->flags &= ~PIMSM_ADDED_TO_MFC;
      pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_FALSE;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"PIMSM_ADDED_TO_MFC RESET in SG");      
      return L7_SUCCESS;
    }
  }
  
  rc = pimsmStarGFind( pimsmCb,  pGrpAddr, &pStarGNode );
  if(rc == L7_SUCCESS)
  {
    if((pStarGNode->flags & PIMSM_ADDED_TO_MFC) != L7_NULL )
    {
     rc1 = pimsmStarGCacheFind(pimsmCb, pStarGNode, pSrcAddr, pGrpAddr, &kernelCache);
     if(rc1 == L7_SUCCESS)
     {
        if(pimsmStarGCacheDelete( pimsmCb, pStarGNode, kernelCache ) == L7_SUCCESS)
        {
        if (SLLNumMembersGet(&(pStarGNode->pimsmStarGCacheList)) == 0)
        {
            pStarGNode->flags &= ~PIMSM_ADDED_TO_MFC;          
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"PIMSM_ADDED_TO_MFC RESET in STARG");              
          return L7_SUCCESS;
        }
     }
    }
  } 
  } 
  return L7_SUCCESS;  
}
/******************************************************************************
* @purpose  wrong-iif event handler
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  incoming router interface number  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
     oiflist = NULL
     if( iif == RPF_interface(RP(G)) AND SPTbit(S,G) == FALSE) {
       oiflist = inherited_olist(S,G,rpt)
       CheckSwitchToSpt(S,G)
     } else {
        # Note: RPF check failed
        # A transition in an Assert FSM, may cause an Assert(S,G)            
        # or Assert(*,G) message to be sent out interface iif.               
        # See section 4.6 for details.                                       
        if ( SPTbit(S,G) == TRUE AND iif is in inherited_olist(S,G) ) {
           send Assert(S,G) on iif
        } else if ( SPTbit(S,G) == FALSE AND
                    iif is in inherited_olist(S,G,rpt) {
           send Assert(*,G) on iif
        }
     }

     oiflist = oiflist (-) iif
     forward packet on all interfaces in oiflist
* @end
******************************************************************************/ 
L7_RC_t pimsmMfcWrongIfProcess(pimsmCB_t * pimsmCb, L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pGrpAddr, L7_uint32 rtrIfNum)
{
  pimsmLongestMatch_t pimsmMRTNode;
  pimsmInterfaceEntry_t *pIntfEntry=L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"Entry"); 
  if(L7_NULLPTR == pimsmCb || (L7_NULLPTR == pSrcAddr) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return L7_FAILURE;
  }

  if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, "PIM-SM is Not Active on Interface - %d",
                 rtrIfNum);
    return L7_FAILURE;
  }

  pIntfEntry->pimsmPerIntfStats.pimsmWrongiifRx++;


  if(inetIsLANScopedAddress(pGrpAddr) == L7_TRUE)
  {
    /* Don't create routing entries for the LAN scoped addresses */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR," LAN scoped addresses: ");
    return L7_FAILURE; 
  }

  memset(&pimsmMRTNode,0,sizeof(pimsmLongestMatch_t));
  if(pimsmMRTFind(pimsmCb, pGrpAddr, pSrcAddr, &pimsmMRTNode) != L7_SUCCESS)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"Grp Address is:",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR," Src Addr = ",pSrcAddr);
    return L7_FAILURE; 
  }
  switch(pimsmMRTNode.entryType)
  {
    case PIMSM_ENTRY_TYPE_S_G:
      if(pimsmSGMfcWrongiifProcess(pimsmCb,pimsmMRTNode.mrtNode,
                                  pSrcAddr,pGrpAddr,rtrIfNum) 
                                  != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
    case PIMSM_ENTRY_TYPE_STAR_G:
      if(pimsmStarGMfcWrongiifProcess(pimsmCb,pimsmMRTNode.mrtNode,pSrcAddr,
                                     pGrpAddr,rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGMfcNoCacheProcess failed");
        return L7_FAILURE;
      }
      break;
    case PIMSM_ENTRY_TYPE_STAR_STAR_RP:
    case PIMSM_ENTRY_TYPE_S_G_RPT:
    default:
      return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"Success");  
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  whole-pkt event handler
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pData       @b{(input)}  Ip pkt start

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/ 
L7_RC_t pimsmMfcWholePktProcess(pimsmCB_t *pimsmCb, L7_uchar8 *pData)
{
  L7_inet_addr_t  srcAddr, grpAddr, rpAddr;
  pimsmSGNode_t * pSGNode;
  L7_uint32 pktLen =0;
  L7_uint32     timeLeft = 0;
  L7_RC_t rc;
  
  if(L7_NULLPTR == pimsmCb || pData == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return L7_FAILURE;
  }

  if(pimsmSourceAndGroupAddressGet(pimsmCb->family,pData,&srcAddr,
         &grpAddr,&pktLen) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
                "pimsmSourceAndGroupAddressGet() failed");
    return L7_FAILURE;
  }

  if(pimsmRpAddressGet(pimsmCb, &grpAddr, &rpAddr)!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, "RP not found" );
    return L7_FAILURE;    
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_DEBUG, "rpAddr :",&rpAddr);

  if(mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, " I am the RP for this group ");
    return(L7_FAILURE);
  }
  /* At this point, (S,G) or (*,G)-kernel-cache entry should have been created 
     (S,G) or (*,G)-kernel-cache entry  will be create in no-cache handler, 
     then we get whole-pkt event.
   */
  /*rc = pimsmSGNodeCreate(pimsmCb,&srcAddr, &grpAddr, &pSGNode);*/  
  rc = pimsmSGFind(pimsmCb, &grpAddr, &srcAddr, &pSGNode);  
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"SG node not found");
    return L7_FAILURE;
  }

  if(pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
         "pSGNode->pimsmSGIEntry[PIMSM_REG_INTF_NUM] is non-NULL");
    if(pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM]->pimsmSGIJoinPruneState 
       == PIMSM_DNSTRM_S_G_SM_STATE_JOIN)
    {
      if(pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer !=L7_NULLPTR &&
         appTimerTimeLeftGet( pimsmCb->timerCb,
                       pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer,
                       &timeLeft ) == L7_SUCCESS &&
          timeLeft > 0)
      {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
            "pimsmSGDRRegisterStopTimer is running, so drop the packet");
      }
      else 
      {
        if(pimsmRegisterSend(pimsmCb,&grpAddr,0,pData,pktLen) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"pimsmRegisterSend failed");
          return L7_FAILURE;
        }
      }
    }
    return L7_SUCCESS;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"Failure");
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Find RPF info for a particular address
*
* @param    pimsmCb           @b{(input)}  control block 
* @param    pSrcAddr          @b{(input)}  address
            pBestRPFRoute     @b{(output)} RTO Best Route information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     Obtain the incoming interface and the next hop router/RPF neighbor
        toward source.
* @end
******************************************************************************/ 
L7_RC_t pimsmRPFInfoGet(pimsmCB_t *pimsmCb,
                        L7_inet_addr_t * pSrcAddr, 
                        mcastRPFInfo_t *pBestRPFRoute)
{
  L7_RC_t rc;

  if((L7_NULLPTR == pimsmCb) ||
     (L7_NULLPTR == pSrcAddr) || 
     (L7_NULLPTR == pBestRPFRoute))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_FATAL,("Invalid input parameters"));
    return L7_FAILURE;
  }

  rc = mcastRPFInfoGet(pSrcAddr, pBestRPFRoute);

  if (rc == L7_SUCCESS && pBestRPFRoute->rpfRouteProtocol == RTO_LOCAL &&
      pBestRPFRoute->rpfIfIndex != 0)
  {
     rc = pimsmNbr( pimsmCb,
    pBestRPFRoute->rpfIfIndex,
    pSrcAddr,
    &pBestRPFRoute->rpfNextHop);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO,("RTO_LOCAL route"));
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, 
      "pBestRPFRoute->rpfNextHop :",
                        &pBestRPFRoute->rpfNextHop);
  }
  /* TODO:
   In future, if static multicast routes are supported put the logic here */
  return rc ;
}

/******************************************************************************
* @purpose  RTO best-route change event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pRouteInfo     @b{(input)}  RTO Best Route information
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/ 
L7_RC_t pimsmRtoBestRouteChangeProcess(pimsmCB_t *pimsmCb,
                                       mcastRPFInfo_t *pRouteInfo)
{
  if ((pimsmCb == L7_NULLPTR) || (pRouteInfo == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfIfIndex %d",
              pRouteInfo->rpfIfIndex);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfNextHop :",
                       &(pRouteInfo->rpfNextHop));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfRouteProtocol %d",
              pRouteInfo->rpfRouteProtocol);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfRouteAddress :",
                   &(pRouteInfo->rpfRouteAddress));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfRouteMetricPref %d",
              pRouteInfo->rpfRouteMetricPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO rpfRouteMetric %d",
              pRouteInfo->rpfRouteMetric);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO status %d",
              pRouteInfo->status);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"RTO prefixLength %d",
              pRouteInfo->prefixLength);

  pimsmSGTreeBestRouteChngUpdate(pimsmCb,pRouteInfo);
  /*pimsmSGRptTreeBestRouteChngUpdate(pimsmCb,pRouteInfo);*/
  /*Note (S,G,rpt) is updated as part of (*,G), see below API*/
  pimsmStarGTreeBestRouteChngUpdate(pimsmCb,pRouteInfo);
  pimsmStarStarRpTreeBestRouteChngUpdate(pimsmCb,pRouteInfo);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Notify MFC when interface mode is changed
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    intIfNum    @b{(input)}  internal interface number
* @param    mode        @b{(input)}  interface mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmMfcIntfEventQueue(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum, 
                L7_uint32 mode)
{
  mfcInterfaceChng_t ifChange;
  L7_RC_t rc;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_DEBUG,"Enter ");

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return L7_FAILURE;
  }
  memset(&ifChange, 0, sizeof(mfcInterfaceChng_t));
  ifChange.family = pimsmCb->family;
  ifChange.rtrIfNum = rtrIfNum;
  ifChange.mode = mode;
  if(mfcMessagePost(MFC_INTERFACE_STATUS_CHNG, &ifChange,
                    sizeof(mfcInterfaceChng_t)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
                "MFC_INTERFACE_MODE_CHG_EVENT Failed.");
    return(L7_FAILURE);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_DEBUG,"Exit ");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Notify MFC to add/delete an entry in the table.
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    eventType   @b{(input)}  update or delete entry
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  incoming router interface number  
* @param    pOif        @b{(input)}  router interfaces bitset
* @param    isPureSGEntry   @b{(input)} true if (S,G) entry otherwise false
* @param    pFlags          @b{(output)} MFC mirror

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     pFlags updation should happen only is this API.
      expect for flag clear in (*,G) or (*,*,RP) entry.
* @end
******************************************************************************/
L7_RC_t pimsmMfcQueue(pimsmCB_t * pimsmCb, mfcOperationType_t operType, 
                      L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr, 
                      L7_uint32 rtrIfNum, interface_bitset_t *pOif,
                      L7_BOOL isPureSGEntry, L7_ushort16 *pFlags)
{
  mfcEntry_t mfcEntry;
  L7_BOOL result= L7_FALSE;
   

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_NORMAL, "Entry");
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }
  
  if((pimsmCb == (pimsmCB_t *)L7_NULLPTR)||
     (pSrcAddr == (L7_inet_addr_t *)L7_NULLPTR)||
     (pGrpAddr == (L7_inet_addr_t *)L7_NULLPTR)||
     (pOif == (interface_bitset_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, "Input is NULLPTR");
    return(L7_FAILURE);
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }  
  if ((*pFlags & PIMSM_ADDED_TO_MFC) == L7_NULL &&
      operType == MFC_DELETE_ENTRY) 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR,
      "Non-existing entry asked to delete from MFC");
    return(L7_SUCCESS);
  }
  memset(&mfcEntry, 0 , sizeof(mfcEntry_t));
  inetCopy(&(mfcEntry.group), pGrpAddr);
  inetCopy(&(mfcEntry.source), pSrcAddr);
  mfcEntry.iif = rtrIfNum;
  mfcEntry.mcastProtocol = L7_MRP_PIMSM;
  memcpy(&(mfcEntry.oif),pOif,sizeof(interface_bitset_t));

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, " operType = %d", operType);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, " pSrcAddr", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, " pGrpAddr", pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, " iif = %d", rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, " oif =");
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, pOif );

  /* Send to MFC */
  if(mfcMessagePost (operType, (void *) &mfcEntry , 
                     sizeof(mfcEntry_t)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_ERROR, 
      "Entry Update Event post to MFC Failed.");
    return(L7_FAILURE);
  }
  if (operType == MFC_UPDATE_ENTRY)
  {
    *pFlags |= PIMSM_ADDED_TO_MFC;
    BITX_IS_EMPTY(pOif,result);
    if(result == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO, 
         "Negative entry added to MFC");
      *pFlags |= PIMSM_NULL_OIF;
    } 
    else
    {
      *pFlags &= ~PIMSM_NULL_OIF;
    }
  }
  else if (operType == MFC_DELETE_ENTRY)
  {
    *pFlags &= ~PIMSM_NULL_OIF;
    if(L7_TRUE == isPureSGEntry) 
    {
      *pFlags &= ~PIMSM_ADDED_TO_MFC;
    }
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_NORMAL, "Exit");
  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose  global pimsm admin-mode change handler
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param		isPimsmEnabled		@b{(input)} pimsm Enable / Disable.
*
* @returns  L7_SUCCESS
* @returns	L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmGlobalAdminModeProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                                    L7_BOOL isPimsmEnabled)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  L7_uint32 adminMode = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;
  L7_APP_TMR_HNDL_t timer;
  pimsmTimerData_t *pTimerData;
  
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  if(isPimsmEnabled == L7_FALSE)
  {
    /* Cleanup any stale entries */
    pimsmMRTNodeCleanup(pimsmCb);
    pimsmSGTreePurge(pimsmCb);
    pimsmSGRptTreePurge(pimsmCb);
    pimsmStarGTreePurge(pimsmCb);
    pimsmStarStarRpPurge(pimsmCb);
    pimsmIntfPurge(pimsmCb);
    pimsmRpGrpMappingPurge(pimsmCb);
    pimsmBsrPurge(pimsmCb);
    if(pimsmCb->pimsmSPTTimer != L7_NULLPTR)
    {
      appTimerDelete(pimsmCb->timerCb, pimsmCb->pimsmSPTTimer);
      pimsmCb->pimsmSPTTimer = L7_NULLPTR;
    }
    if(pimsmCb->pimsmJPBundleTimer != L7_NULLPTR)
    {
      appTimerDelete(pimsmCb->timerCb,pimsmCb->pimsmJPBundleTimer);
      pimsmCb->pimsmJPBundleTimer = L7_NULLPTR;
    }
  }
  else
  {
    adminMode = L7_ENABLE;
    pTimerData = &pimsmCb->pimsmJPBundleTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    pTimerData->pimsmCb = pimsmCb;
  
    timer = appTimerAdd(pimsmCb->timerCb,
                        pimsmJPBundleTimerExpiryHandler,
                        (void*)pimsmCb->pimsmJPBundleTimerHandle,
                        PIMSM_DEFAULT_JP_BUNDLE_TIME,
                        "SM-JPB");
    if(timer != L7_NULLPTR)
    {
      pimsmCb->pimsmJPBundleTimer = timer;
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR, "pimsmJPBundleTimerParam TimerAdd Failed");
    }
    pTimerData = &pimsmCb->pimsmSPTTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    pTimerData->pimsmCb = pimsmCb;
 
    timer = appTimerAdd(pimsmCb->timerCb,
                        pimsmSPTTimerExpiresHandler,
                        (void*)pimsmCb->pimsmSPTTimerHandle,
                        PIMSM_DEFAULT_CHECK_INTERVAL,
                        "SM-SPT");
    if(timer != L7_NULLPTR)
    {
      pimsmCb->pimsmSPTTimer = timer;
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR, "pimsmSPTTimer TimerAdd Failed");
    }
  }
  pimsmCb->isPimsmEnabled = isPimsmEnabled;
  return rc;
}
/******************************************************************************
* @purpose  per interface interface mode (isIntfUp)change handler &
*           per interface pimsm mode (isPimsmEnabled) change handler  
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    pimsmIntfStatus   @b{ (input) } pimsm Interface Status.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmIntfStatusProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                               pimsmIntfStatus_t *pPimsmIntfStatus)
{
  pimsmCB_t        *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  L7_uint32        rtrIfNum = pPimsmIntfStatus->rtrIfNum;
  L7_uint32        intIfNum = pPimsmIntfStatus->intIfNum;
  L7_inet_addr_t   *pIntfAddr = pPimsmIntfStatus->intfAddr;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }
  
  if(pPimsmIntfStatus->isEnabled == L7_TRUE)
  {
    pimsmIntfUp(pimsmCb, rtrIfNum, intIfNum, pIntfAddr);
  }
  else
  {
    pimsmIntfDown(pimsmCb, rtrIfNum);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Handles admin-scope change 
*
* @param    pimsmCb          @b{ (input) } pimsm Control block.
* @param    pAdminScopeInfo  @b{ (input) } Admin-scope info. 
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmAdminScopeEventProcess(pimsmCB_t *pimsmCb, mcastAdminMsgInfo_t *pAdminScopeInfo)
{

  if (pimsmCb == L7_NULLPTR || pAdminScopeInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return L7_FAILURE;
  }
  if (pimsmCb->family != L7_AF_INET)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                  "Invalid family for admin-scope boundary handling");
    return L7_FAILURE;
  }
  /* P.S : The internal num to router interface conversion is already done at 
     pimsm map layer before sending an event to pimsmMap queue and stored 
     the same in pAdminScopeInfo->intIfNum */
     
  /* Update all (*,G) , (S,G) and (S,G,Rpt) entries */
  pimsmStarGAdminScopeUpdate(pimsmCb);
  pimsmSGAdminScopeUpdate(pimsmCb);
  pimsmSGRptAdminScopeUpdate(pimsmCb);
    
  return L7_SUCCESS;
}

void pimsmRestartFlagClear(pimsmCB_t *pimsmCb)
{
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return;
  }
  pimsmCb->pimsmRestartInProgress = L7_FALSE;
}

L7_BOOL pimsmRestartFlagGet(pimsmCB_t *pimsmCb)
{
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return(L7_FALSE);
  }
  return(pimsmCb->pimsmRestartInProgress);
}

/******************************************************************************
* @purpose  Add/Delete static-RP
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    pStaticRpData     @b{ (input) } pimsm static RP Data.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStaticRPProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             pimsmStaticRPdata_t *pStaticRpData)
{
  pimsmCB_t                 *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  pimsmCandRpConfigInfo_t   staticRpInfo;
  pimsmRpGrpNode_t          *pRpGrpNode = L7_NULLPTR;
  L7_inet_addr_t            grpMaskAddr;
  L7_uchar8                 buf[PIMSM_MAX_DBG_ADDR_SIZE];

  if(pimsmCb == L7_NULLPTR ||
    pStaticRpData == L7_NULLPTR ||
    pStaticRpData->pPimsmStaticRPRPAddress  == L7_NULLPTR ||
    pStaticRpData->pPimsmStaticRPGrpAddress == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return L7_FAILURE;
  }

  memset(&staticRpInfo,0,sizeof(pimsmCandRpConfigInfo_t));
  inetCopy(&staticRpInfo.pimsmGrpAddr, pStaticRpData->pPimsmStaticRPGrpAddress);
  staticRpInfo.pimsmGrpMaskLen = pStaticRpData->pimsmStaticRPGrpPrefixLength;
  inetCopy(&staticRpInfo.pimsmRpAddr, pStaticRpData->pPimsmStaticRPRPAddress);
  staticRpInfo.pimsmRpPriority = PIMSM_DEFAULT_CAND_RP_ADV_PRIORITY;
  staticRpInfo.pimsmRpHoldTime = PIMSM_DEFAULT_CAND_RP_HOLDTIME;
  inetMaskLenToMask(pimsmCb->family, 
                    staticRpInfo.pimsmGrpMaskLen, &grpMaskAddr);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "Group Address   - %s", inetAddrPrint(&staticRpInfo.pimsmGrpAddr,buf));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "Group Mask Len  - %d", staticRpInfo.pimsmGrpMaskLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "RP Address      - %s", inetAddrPrint(&staticRpInfo.pimsmRpAddr,buf));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "RP Priority     - %d", staticRpInfo.pimsmRpPriority);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "RP Hold Time    - %d", staticRpInfo.pimsmRpHoldTime);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "Group Mask Addr - %s", inetAddrPrint(&grpMaskAddr,buf));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO, "RP Conflict     - %d", pStaticRpData->pimsmStaticRPOverrideDynamic);

  if (pStaticRpData->pimsmStaticRPOverrideDynamic == L7_TRUE)
  {
    if (pimsmRpGrpNodeFind(pimsmCb, staticRpInfo.pimsmGrpAddr, 
                        grpMaskAddr, staticRpInfo.pimsmRpAddr,
                        PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED,
                        &pRpGrpNode) == L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO," RP-Group mapping already exists with STATIC origin");
      staticRpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED;
      pimsmRpGrpMappingDelete(pimsmCb, &staticRpInfo);
    }
    staticRpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE;
  }
  else
  {
    if (pimsmRpGrpNodeFind(pimsmCb, staticRpInfo.pimsmGrpAddr, 
                        grpMaskAddr, staticRpInfo.pimsmRpAddr,
                        PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE,
                        &pRpGrpNode) == L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO," RP-Group mapping already exists with STATIC- OVERRIDE origin");
        staticRpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE;
      pimsmRpGrpMappingDelete(pimsmCb, &staticRpInfo);
    }
    staticRpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED;
  }

  if (pStaticRpData->operMode == L7_TRUE)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"GrpAddr = ", 
                     pStaticRpData->pPimsmStaticRPGrpAddress);
    pimsmRpGrpMappingAdd(pimsmCb,&staticRpInfo);
  }
  else
  {
    pimsmRpGrpMappingDelete(pimsmCb,&staticRpInfo);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Add/Delete cand-RP
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    pCandRpData       @b{ (input) } pimsm candidate RP Data.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmCRPProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                        pimsmCandRPdata_t *pCandRpData)
{
  pimsmCB_t                 *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  pimsmCandRpConfigInfo_t   candRpInfo;
  L7_inet_addr_t grpAddr;
  L7_inet_addr_t grpMask;

  if(pimsmCb == L7_NULLPTR ||
    pCandRpData == L7_NULLPTR ||
    pCandRpData->pCandRPAddress  == L7_NULLPTR ||
    pCandRpData->pGrpAddress  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "invalid input params");
    return L7_FAILURE;
  }

  memset(&candRpInfo,0,sizeof(pimsmCandRpConfigInfo_t));

  inetAddressZeroSet (pimsmCb->family, &grpAddr);
  inetAddressZeroSet (pimsmCb->family, &grpMask);
  inetMaskLenToMask (pimsmCb->family, pCandRpData->grpPrefixLength, &grpMask);
  inetAddressAnd (pCandRpData->pGrpAddress, &grpMask, &grpAddr);
  inetCopy(&candRpInfo.pimsmGrpAddr, &grpAddr);
  candRpInfo.pimsmGrpMaskLen = pCandRpData->grpPrefixLength;
  inetCopy(&candRpInfo.pimsmRpAddr, pCandRpData->pCandRPAddress);
  candRpInfo.pimsmRpRtrIfNum = pCandRpData->intIfNum;

  candRpInfo.pimsmRpPriority = PIMSM_DEFAULT_CAND_RP_ADV_PRIORITY;
  candRpInfo.pimsmRpHoldTime = PIMSM_DEFAULT_CAND_RP_HOLDTIME;
  candRpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG;

  if (pCandRpData->operMode == L7_TRUE)
  {
    pimsmRpCandidateInfoAdd(pimsmCb,&candRpInfo);
  } else
  {
    pimsmRpCandidateInfoDelete(pimsmCb, &candRpInfo);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Add/Delete cand-BSR
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    pCandBsrData      @b{ (input) } pimsm Candidate BSR Data.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmCBSRProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                         pimsmCandBSRdata_t *pCandBsrData)
{
  pimsmCB_t        *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  L7_uint32        scopeId = 0;

 if (L7_NULLPTR == pimsmCb ||
    pCandBsrData == L7_NULLPTR ||
    pCandBsrData->pCandBSRAddress == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
        "invalid input params");
    return L7_FAILURE;
  }
  pimsmBsrCandBSRConfigProcess(pimsmCb, pCandBsrData->pCandBSRAddress, 
                               pCandBsrData->intIfNum,
                               pCandBsrData->candBSRPriority, 
      pCandBsrData->candBSRHashMaskLength, scopeId, pCandBsrData->operMode);
  return(L7_SUCCESS);
}


/******************************************************************************
* @purpose  First API to receive all PIMSM packets
*
* @param    pimsmCb           @b{(input)}  control block 
* @param    pMcastCtrlPkt     @b{(input)}  received control pkt info
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmMcastPktRecvProcess(pimsmCB_t * pimsmCb, 
                                 mcastControlPkt_t * pMcastCtrlPkt)
{
  if (pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_ERROR, "CB is NULLPTR");
    return L7_FAILURE;         
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "family = %d", pMcastCtrlPkt->family);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "rtrIfNum = %d", pMcastCtrlPkt->rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "pSrcAddr :", &pMcastCtrlPkt->srcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "destAddr :", &pMcastCtrlPkt->destAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "length = %d", pMcastCtrlPkt->length);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "payLoad = %p", pMcastCtrlPkt->payLoad);

  /* Copy the received packet to a local buffer and Free the MCAST Buffer */
  memset (pimsmCb->pktRxBuf, 0, PIMSM_PKT_SIZE_MAX);
  memcpy (pimsmCb->pktRxBuf, (L7_uchar8*) pMcastCtrlPkt->payLoad, PIMSM_PKT_SIZE_MAX);
  mcastCtrlPktBufferPoolFree(pimsmCb->family,pMcastCtrlPkt->payLoad);

  /* Process the Received Control Packet */
  pimsmCtrlPktRecv(pimsmCb,&(pMcastCtrlPkt->srcAddr),
                   &(pMcastCtrlPkt->destAddr),pimsmCb->pktRxBuf,
                   pMcastCtrlPkt->length, pMcastCtrlPkt->rtrIfNum,
                   pMcastCtrlPkt->ipTypeOfService);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets the data threshold rate
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    dataThresholdRate @b{ (input) } Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     Rate converted to bytes/sec used by vendor protocol
*
* @end
*********************************************************************/
L7_RC_t pimsmDataThresholdRateSet(MCAST_CB_HNDL_t pimsmCbHandle, 
                                  L7_uint32 dataThresholdRate)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  pimsmCb->pimsmDataRateBytes = (dataThresholdRate * 1000 *
                                  pimsmCb->pimsmRateCheckInterval)/10;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the register threshold rate
*
* @param    pimsmCbHandle     @b{ (input) } pimsm CB Handle.
* @param    regThresholdRate  @b{ (input) } Register Threshold Rate in 
*                                           kbits/sec.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     Rate converted to bytes/sec used by vendor protocol
*
* @end
*********************************************************************/
L7_RC_t pimsmRegisterThresholdRateSet(MCAST_CB_HNDL_t pimsmCbHandle, 
                                      L7_uint32 regThresholdRate)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  pimsmCb->pimsmRegRateBytes = (regThresholdRate * 1000 *
                                 pimsmCb->pimsmRateCheckInterval)/10;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  dr-priority change event handler
*
* @param    pimsmCbHandle  @b{ (input) } pimsm CB Handle.
* @param    rtrIfNum       @b{ (input) } Register Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmDRPriorityProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                               L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc = L7_NULL;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }

  rc = pimsmIntfDRUpdate(pimsmCb, pIntfEntry);
  return rc;
}

/*********************************************************************
* @purpose  Sets the Hello interval for an interface
*
* @param    pimsmCbHandle  @b{ (input) } pimsm CB Handle.
* @param    rtrIfnum       @b{ (input) } router interface number
* @param    helloIntvl     @b{ (input) } Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmHelloIntervalProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                                  L7_uint32 rtrIfNum, L7_uint32 helloIntvl)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;


  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  pIntfEntry->pimsmInterfaceHelloInterval = helloIntvl;

  /* set holdtime = 3.5 * hello-interval */
  pIntfEntry->pimsmInterfaceHelloHoldtime = 
      (35 * pIntfEntry->pimsmInterfaceHelloInterval)/ 10;
  return rc;
}
/*********************************************************************
* @purpose  Sets the Join Prune interval for an interface
*
* @param    pimsmCbHandle  @b{ (input) } pimsm CB Handle.
* @param    rtrIfnum       @b{ (input) } router interface number
* @param    helloIntvl     @b{ (input) } Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmJoinPruneIntervalProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                                      L7_uint32 rtrIfNum, 
                                      L7_uint32 joinPruneIntvl)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *) pimsmCbHandle;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  pIntfEntry->pimsmInterfaceJoinPruneInterval = joinPruneIntvl;
    /* set holdtime = 3.5 * hello-interval */
  pIntfEntry->pimsmInterfaceJoinPruneHoldtime= 
      (35 * pIntfEntry->pimsmInterfaceJoinPruneInterval)/10;  
  return rc;
}

/******************************************************************************
* @purpose  Process Local Membership change event
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number   
* @param    bDelete     @b{(input)}  delete flag
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmLocalMemberShipChangeUpdate(pimsmCB_t * pimsmCb, 
                                             L7_inet_addr_t * pSrcAddr, 
                                             L7_inet_addr_t * pGrpAddr, 
                                             L7_uint32 rtrIfNum,
                                             L7_BOOL bDelete,
                                             L7_BOOL starGILocalMemberShip,
                                             L7_BOOL jpMsgImdtSend)

{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Entry");

  if (pSrcAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Source is NULL");

    if (pimsmStarGLocalMemberShipChngEventPost (pimsmCb, pGrpAddr,
                                                rtrIfNum, bDelete, starGILocalMemberShip,
                                                jpMsgImdtSend)
                                             != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "pimsmStarGLocalMemberShipChngEventPost failed");
      return L7_FAILURE;
    }
  }
  else
  {
    if (pimsmSGLocalMemberShipChngEventPost (pimsmCb, pSrcAddr, pGrpAddr,
                                             rtrIfNum, bDelete,
                                             jpMsgImdtSend)
                                          != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "pimsmSGLocalMemberShipChngEventPost failed");
      return L7_FAILURE;
    }
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  IGMPv3 member include process event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pMgmdGrpInfo   @b{(input)}  igmp group info
* @param    srcCount       @b{(input)}  number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmIgmpV3MemberIncludeModeProcess(pimsmCB_t *pimsmCb,
                   mgmdMrpEventInfo_t *pMgmdGrpInfo,
                   L7_uint32 srcCount,
                   L7_BOOL jpMsgImdtSend)
{
  pimsmSGNode_t *pSGNode = L7_NULLPTR;
  pimsmSGIEntry_t *pSGIEntry = L7_NULLPTR;
  L7_BOOL bDelete = L7_FALSE;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_int32 numSrcs =0;
  L7_uint32 i = 0;

  numSrcs = srcCount;

  for (i = 0; i < numSrcs; i++)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"numSrcs - %d, currentSrc - %d", numSrcs, i);

    if (pMgmdGrpInfo->sourceList[i] == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "MGMD Update Entry Source List Buffer Ptr is NULL");
      return L7_FAILURE;
    }
    if(pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_ADD)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Action on Src is ADD");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Source Address is:",
                         &pMgmdGrpInfo->sourceList[i]->sourceAddr);
      /*Add (S,G) Entry */
      if (pimsmSGFind(pimsmCb,&pMgmdGrpInfo->group,
                  &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                  &pSGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                    "(S,G) not found, If DR, Create it.");
      }
      if((pimsmIAmDR(pimsmCb,pMgmdGrpInfo->rtrIfNum) == L7_TRUE))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"pimsmIAmDR is TRUE");
        if(pSGNode == L7_NULLPTR)
        {
          if(pimsmSGNodeCreate(pimsmCb,
                              &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                              &pMgmdGrpInfo->group,
                              &pSGNode,L7_FALSE) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGNodeCreate failed");
            continue;
          }
        }
        pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
        if(pSGIEntry == L7_NULLPTR)
        {
          if(pimsmSGIEntryCreate(pimsmCb,pSGNode,pMgmdGrpInfo->rtrIfNum)
                                 != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGIEntry Create failed");
            return L7_FAILURE;
          }
          pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
        }
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Setting Local Membership to TRUE");
        if(pSGIEntry->pimsmSGILocalMembership != L7_TRUE)
        {
          pSGIEntry->pimsmSGILocalMembership = L7_TRUE;
          bDelete = L7_FALSE;

          pimsmLocalMemberShipChangeUpdate(pimsmCb,
                           &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                           &pMgmdGrpInfo->group,
                            pMgmdGrpInfo->rtrIfNum,bDelete, L7_TRUE, jpMsgImdtSend);
        }

      }
      else
      {
        if(pSGNode != L7_NULLPTR)
        {
          pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
          if(pSGIEntry == L7_NULLPTR)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "Downstream interface entry is NULL.This should not happen");
            return L7_FAILURE;
          }
          if(pimsmIntfEntryGet(pimsmCb, pMgmdGrpInfo->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
              "IntfEntry is not available for rtrIfNum(%d)", pMgmdGrpInfo->rtrIfNum);
            return L7_FAILURE;
          }           
          if(PIMSM_INET_IS_ADDR_EQUAL(&pSGIEntry->pimsmSGIAssertWinnerAddress,
                                   &pIntfEntry->pimsmInterfaceAddr) == L7_TRUE)
          {
            if(pSGIEntry->pimsmSGILocalMembership != L7_TRUE)
            {
              pSGIEntry->pimsmSGILocalMembership = L7_TRUE;
              pimsmLocalMemberShipChangeUpdate(pimsmCb,
                            &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                            &pMgmdGrpInfo->group,
                            pMgmdGrpInfo->rtrIfNum,bDelete, L7_TRUE, jpMsgImdtSend);
              
            }
          }
        }
      }
    }
    else if(pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_DELETE)
    {
      if (pimsmSGFind(pimsmCb,
                  &pMgmdGrpInfo->group,
                  &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                  &pSGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                    "(S,G) not found");
        continue;
      }
      if(pSGNode != L7_NULLPTR)
      {
        bDelete = L7_FALSE;
        /*Delete the SGI Entry on the (*,G)Node.It is a IGMPv2 Leave */
        pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
        if(pSGIEntry != L7_NULLPTR)
        {
          if(pSGIEntry->pimsmSGILocalMembership == L7_TRUE)
          {
              pSGIEntry->pimsmSGILocalMembership = L7_FALSE;
          }
          if (pimsmIntfEntryGet(pimsmCb,pMgmdGrpInfo->rtrIfNum,&pIntfEntry) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
              "IntfEntry is not available for rtrIfNum(%d)", pMgmdGrpInfo->rtrIfNum);
            return L7_FAILURE;
          }
          if((pIntfEntry != L7_NULLPTR) && 
             (pIntfEntry->pimsmNbrCount == 0))
          {
            pimsmSGIEntryDelete(pimsmCb, pSGNode, pMgmdGrpInfo->rtrIfNum);
            bDelete = L7_TRUE;
          }
        }
        pimsmLocalMemberShipChangeUpdate(pimsmCb,
                              &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                              &pMgmdGrpInfo->group,
                              pMgmdGrpInfo->rtrIfNum,bDelete, L7_FALSE, jpMsgImdtSend);

        pimsmSGNodeTryRemove(pimsmCb,pSGNode);        
      }
    }
  }/*end of for (numSrcs) */
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  IGMPv1/v2 member include process event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pMgmdGrpInfo   @b{(input)}  igmp group info
* @param    numSrcs        @b{(input)}  number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmIgmpMemberIncludeModeProcess(pimsmCB_t * pimsmCb, 
                                                 mgmdMrpEventInfo_t *pMgmdGrpInfo,
                                                 L7_uint32 numSrcs)
{
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  pimsmInterfaceEntry_t  *pIntfEntry = L7_NULLPTR;
  L7_BOOL jpMsgImdtSend = L7_TRUE;
  L7_BOOL bDelete = L7_FALSE;
  
  if (pimsmStarGFind(pimsmCb,&pMgmdGrpInfo->group,&pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,
                 "(*,G) not found");
  }
  if(pStarGNode != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"Star G Node Found");
   
    /*Delete the SGI Entry on the (*,G)Node.It is a IGMPv2 Leave */
    if ((pStarGIEntry = pStarGNode->pimsmStarGIEntry[pMgmdGrpInfo->rtrIfNum])
                     != L7_NULLPTR)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,
                   "Star GI Entry found for rtrIfNum %d", pMgmdGrpInfo->rtrIfNum);

      if (pimsmIntfEntryGet (pimsmCb, pMgmdGrpInfo->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                    "PIMSM is not enabled on this interface - %d", pMgmdGrpInfo->rtrIfNum);
        return L7_FAILURE;
      }
      if(pStarGIEntry->pimsmStarGILocalMembership == L7_TRUE)
      {
        pStarGIEntry->pimsmStarGILocalMembership = L7_FALSE;
      }
      if ((pIntfEntry != L7_NULLPTR) && (pIntfEntry->pimsmNbrCount == 0))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,
                    "Deleting the entry as there are no neighbors");

        pimsmStarGIEntryDelete(pimsmCb,pStarGNode,pMgmdGrpInfo->rtrIfNum);
        bDelete = L7_TRUE;
      }
    }
    pimsmLocalMemberShipChangeUpdate (pimsmCb,L7_NULLPTR, 
                                          &pMgmdGrpInfo->group,
                                          pMgmdGrpInfo->rtrIfNum, bDelete,
                                          L7_FALSE, jpMsgImdtSend);
    pimsmStarGNodeTryRemove(pimsmCb,pStarGNode);
  }

  pimsmIgmpV3MemberIncludeModeProcess (pimsmCb, pMgmdGrpInfo, numSrcs, jpMsgImdtSend);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  IGMPv3 member exclude process event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pMgmdGrpInfo   @b{(input)}  igmp group info
* @param    srcCount       @b{(input)}  number of sources   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmIgmpV3MemberExcludeModeProcess(pimsmCB_t *pimsmCb, 
                                                 mgmdMrpEventInfo_t *pMgmdGrpInfo,
                                                 L7_uint32 srcCount,
                                                 L7_BOOL jpMsgImdtSend)
{
  pimsmSGNode_t *pSGNode = L7_NULLPTR;
  pimsmSGIEntry_t *pSGIEntry = L7_NULLPTR;
  L7_BOOL bDelete = L7_FALSE;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_int32 numSrcs =0;
  L7_uint32 i = 0;
  L7_uchar8  buf[IPV6_DISP_ADDR_LEN];
  numSrcs = srcCount;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  
  for (i = 0; i < numSrcs; i++)
  {
    if (pMgmdGrpInfo->sourceList[i] == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "MGMD Update Entry Source List Buffer Ptr is NULL");
      return L7_FAILURE;
    }
    if(pMgmdGrpInfo->sourceList[i]->filterMode == MGMD_FILTER_MODE_INCLUDE)
    {
      if(pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_ADD)
      {
        /*Add (S,G) entry */
        if (pimsmSGFind(pimsmCb,&pMgmdGrpInfo->group,
                      &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                      &pSGNode) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                      "(S,G) not found, If DR, Create it.");
        }
        if((pimsmIAmDR(pimsmCb,pMgmdGrpInfo->rtrIfNum) == L7_TRUE))
        {
          if(pimsmSGRptFind (pimsmCb, &pMgmdGrpInfo->group,
                          &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                           &pSGRptNode) == L7_SUCCESS)
          { 
            if(pimsmSGRptDelete(pimsmCb,&pMgmdGrpInfo->group,
              	         &pMgmdGrpInfo->sourceList[i]->sourceAddr) != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
              	          "pimsmSGRpt Delete failed %s %s",
              	           inetAddrPrint(&pMgmdGrpInfo->group,buf),
              	           inetAddrPrint(&pMgmdGrpInfo->sourceList[i]->sourceAddr,buf));
              continue;
           	}
          }
          if(pSGNode == L7_NULLPTR)
          {
            if(pimsmSGNodeCreate(pimsmCb,&pMgmdGrpInfo->sourceList[i]->sourceAddr,
                                	&pMgmdGrpInfo->group,
                                 &pSGNode,L7_FALSE) != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGNodeCreate failed");
              continue;
            }
          }
          pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
          if(pSGIEntry == L7_NULLPTR)
          {
            if(pimsmSGIEntryCreate(pimsmCb,pSGNode,pMgmdGrpInfo->rtrIfNum)
                 != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmSGIEntry Create failed");
              return L7_FAILURE;
            }
            pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
          }
          if(pSGIEntry->pimsmSGILocalMembership == L7_TRUE)
          {
            pSGIEntry->pimsmSGILocalMembership = L7_TRUE;
            pimsmLocalMemberShipChangeUpdate(pimsmCb,
                             &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                             &pMgmdGrpInfo->group,
                             pMgmdGrpInfo->rtrIfNum,bDelete, L7_TRUE, jpMsgImdtSend);
          }
        }
        else
        {
          if(pSGNode != L7_NULLPTR)
          {
            pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
            if(pSGIEntry == L7_NULLPTR)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                          "Downstream interface entry is NULL. \
                           This should not happen");
              return L7_FAILURE;
            }
            if(pimsmIntfEntryGet(pimsmCb, pMgmdGrpInfo->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                "IntfEntry is not available for rtrIfNum(%d)", pMgmdGrpInfo->rtrIfNum);
              return L7_FAILURE;
            }             
            if(PIMSM_INET_IS_ADDR_EQUAL(&pSGIEntry->pimsmSGIAssertWinnerAddress,
                                       &pIntfEntry->pimsmInterfaceAddr) == L7_TRUE)
            {
              if(pSGIEntry->pimsmSGILocalMembership == L7_TRUE)
              {
                pSGIEntry->pimsmSGILocalMembership = L7_TRUE;
                pimsmLocalMemberShipChangeUpdate(pimsmCb,
                              &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                              &pMgmdGrpInfo->group,
                              pMgmdGrpInfo->rtrIfNum,bDelete,L7_TRUE, jpMsgImdtSend);
              }
            }
          }
        }
      }
      else if(pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_DELETE)
      {
        /*Delete (S,G) entry */
        if (pimsmSGFind(pimsmCb,
                      &pMgmdGrpInfo->group,
                      &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                      &pSGNode) != L7_SUCCESS)
        {
          continue;
        }
        if(pSGNode != L7_NULLPTR)
        {
          /*Delete the SGI Entry on the (*,G)Node.It is a IGMPv2 Leave */
          pSGIEntry = pSGNode->pimsmSGIEntry[pMgmdGrpInfo->rtrIfNum];
          if(pSGIEntry != L7_NULLPTR)
          {
            if (pimsmIntfEntryGet(pimsmCb,pMgmdGrpInfo->rtrIfNum,&pIntfEntry)
                               != L7_SUCCESS)
            {
              continue;
            }
            if((pIntfEntry != L7_NULLPTR) && 
               (SLLNumMembersGet(&(pIntfEntry->pimsmNbrList)) == 0))
            {
              pimsmSGIEntryDelete(pimsmCb,pSGNode,pMgmdGrpInfo->rtrIfNum);
              bDelete = L7_TRUE;
            }
            else
            {
              if(pSGIEntry->pimsmSGILocalMembership == L7_FALSE)
              {
                continue;
              }
              pSGIEntry->pimsmSGILocalMembership = L7_FALSE;
            }
          }
          pimsmLocalMemberShipChangeUpdate(pimsmCb,
                             &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                             &pMgmdGrpInfo->group,
                             pMgmdGrpInfo->rtrIfNum,bDelete, L7_FALSE, jpMsgImdtSend);
        }
      }
    }
    else if(pMgmdGrpInfo->sourceList[i]->filterMode == MGMD_FILTER_MODE_EXCLUDE)
    {
      pimsmSGRptIEntry_t *pSGRptIEntry = L7_NULLPTR;
      pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;

      if (pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_ADD)
      {
        /* Add (S,G)Rpt entry and send a prune towards RP */
        /* Compute Prune Desired */
        if ((pimsmIAmDR (pimsmCb, pMgmdGrpInfo->rtrIfNum) == L7_TRUE))
        {
          if (pimsmSGRptFind (pimsmCb, &pMgmdGrpInfo->group,
                          &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                          &pSGRptNode) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                        "(S,G,Rpt) not found, Create it.");
          }
          if (pSGRptNode == L7_NULLPTR)
          {
            if (pimsmSGRptNodeCreate (pimsmCb,
                                      &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                                      &pMgmdGrpInfo->group,
                                      &pSGRptNode) != L7_SUCCESS)
            {
              PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                           "pimsmSGRptNodeCreate failed");
              continue;
            }
          }
          pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[pMgmdGrpInfo->rtrIfNum];
          if(pSGRptIEntry == L7_NULLPTR)
          {
            if (pimsmSGRptIEntryCreate (pimsmCb, pSGRptNode, pMgmdGrpInfo->rtrIfNum)
                                     != L7_SUCCESS)
            {
              PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                           "pimsmSGRptIEntry Create failed");
              return L7_FAILURE;
            }
            pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[pMgmdGrpInfo->rtrIfNum];
          }

          pSGRptIEntry->pimsmSGRptILocalMembership = L7_TRUE;

          /* Send a (S,G,Rpt) Prune towards the RP */
          if (pimsmSGRptPruneDesired (pimsmCb, pSGRptNode) == L7_TRUE)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR, 
                         "(S,G,Rpt) Prune Desired is TRUE");
            upStrmSGRptEventInfo.eventType = 
              PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
            pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);

            if(pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"MFC Updation failed");
            }
          }
        }
      }
      else if(pMgmdGrpInfo->sourceList[i]->action == MGMD_SOURCE_DELETE)
      {
        /* Delete (S,G)Rpt entry and send a join towards RP */
        /* Compute Prune Desired */
        if ((pimsmIAmDR (pimsmCb, pMgmdGrpInfo->rtrIfNum) == L7_TRUE))
        {
          if (pimsmSGRptFind (pimsmCb, &pMgmdGrpInfo->group,
                          &pMgmdGrpInfo->sourceList[i]->sourceAddr,
                          &pSGRptNode) != L7_SUCCESS)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                         "(S,G)Rpt Find Failed");
            continue;
          }
          if (pSGRptNode == L7_NULLPTR)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                         "(S,G)Rpt Node is NULL");
            continue;
          }

          if ((pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[pMgmdGrpInfo->rtrIfNum])
                          == L7_NULLPTR)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                         "(S,G)RptI Node is NULL for rtrIfNum - %d", pMgmdGrpInfo->rtrIfNum);
            continue;
          }

          /* Delete the (S,G)RptI Node */
          pSGRptIEntry->pimsmSGRptILocalMembership = L7_FALSE;
          if (pimsmSGRptIEntryDelete (pimsmCb, pSGRptNode, pMgmdGrpInfo->rtrIfNum)
                                   != L7_SUCCESS)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                         "pimsmSGRptIEntry Create failed");
            return L7_FAILURE;
          }

          if (pimsmSGRptPruneDesired (pimsmCb, pSGRptNode) == L7_FALSE)
          {
            PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR, 
                         "(S,G,Rpt) Prune Desired is FALSE");
            upStrmSGRptEventInfo.eventType = 
              PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
            pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);

            if(pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"MFC Updation failed");
            }
          }
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  IGMPv1/v2 member exclude process event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pMgmdGrpInfo   @b{(input)}  igmp group info
* @param    numSrcs        @b{(input)}  number of sources   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmIgmpMemberExcludeModeProcess(pimsmCB_t * pimsmCb, 
                              mgmdMrpEventInfo_t *pMgmdGrpInfo,
                              L7_uint32 numSrcs)
{
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  L7_BOOL bDelete = L7_FALSE;
  pimsmInterfaceEntry_t * pIntfEntry; 
  L7_BOOL jpMsgImdtSend = L7_TRUE;

  /* There is a possibility that this message is a EXCLUDE(S,G) message.
   * In this case, (*,G) Join and (S,G,Rpt) Prune have to go in the same
   * J/P message.  This is when the number of sources is non-zero.
   * Send the J/P message after a delay so that there will be time for the 
   * (S,G,Rpt) processing.
   * In case of EXCLUDE(NULL), it is IGMPv2 Join.  In that case the number of
   * sources is zero.  Send the J/P Message immediately.
   */
  if (pMgmdGrpInfo->numSrcs != 0)
  {
    jpMsgImdtSend = L7_FALSE;
  }

  if (pimsmStarGFind(pimsmCb,&pMgmdGrpInfo->group,&pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,
                 "(*,G) not found, If DR, Create it");
  }
  if((pimsmIAmDR(pimsmCb,pMgmdGrpInfo->rtrIfNum) == L7_TRUE))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_DEBUG,
                "I am the DR on this interface %d",pMgmdGrpInfo->rtrIfNum);
    if(pStarGNode == L7_NULLPTR)
    {
      if(pimsmStarGNodeCreate(pimsmCb,&pMgmdGrpInfo->group,
                                &pStarGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGNodeCreate failed");
        return L7_FAILURE;
      }
    }
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[pMgmdGrpInfo->rtrIfNum];
    if(pStarGIEntry == L7_NULLPTR)
    {
      if(pimsmStarGIEntryCreate(pimsmCb,pStarGNode,
                                  pMgmdGrpInfo->rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"pimsmStarGIEntry Create failed");
        return L7_FAILURE;
      }
      pStarGIEntry = pStarGNode->pimsmStarGIEntry[pMgmdGrpInfo->rtrIfNum];
    }
    if(pStarGIEntry->pimsmStarGILocalMembership != L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_DEBUG,"Setting Local MemberShip to TRUE");
      pStarGIEntry->pimsmStarGILocalMembership = L7_TRUE;

      pimsmLocalMemberShipChangeUpdate(pimsmCb,L7_NULLPTR,
                                    &pMgmdGrpInfo->group,
                                    pMgmdGrpInfo->rtrIfNum,bDelete, L7_TRUE,
                                    jpMsgImdtSend);
    }

  }
  else
  {
    if(pStarGNode != L7_NULLPTR)
    {
      pStarGIEntry = 
        pStarGNode->pimsmStarGIEntry[pMgmdGrpInfo->rtrIfNum];
      if(pStarGIEntry == L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                      "Downstream interface entry is NULL. \
                       This should not happen");
        return L7_FAILURE;
      }
      if(pimsmIntfEntryGet(pimsmCb, pMgmdGrpInfo->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
          "IntfEntry is not available for rtrIfNum(%d)", pMgmdGrpInfo->rtrIfNum);
        return L7_FAILURE;
      }  
      if((inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress ) != 
           L7_TRUE) && L7_INET_IS_ADDR_EQUAL(
                                &pStarGIEntry->pimsmStarGIAssertWinnerAddress,
                                &pIntfEntry->pimsmInterfaceAddr) == L7_TRUE)
      {
        if(pStarGIEntry->pimsmStarGILocalMembership != L7_TRUE)
        {
          pStarGIEntry->pimsmStarGILocalMembership = L7_TRUE;
          pimsmLocalMemberShipChangeUpdate(pimsmCb,L7_NULLPTR,
                                      &pMgmdGrpInfo->group,
                                      pMgmdGrpInfo->rtrIfNum,bDelete, L7_TRUE,
                                      jpMsgImdtSend);
        }
      }
    }
  }
  pimsmIgmpV3MemberExcludeModeProcess(pimsmCb,pMgmdGrpInfo,numSrcs, jpMsgImdtSend);
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose  igmp/mld member add event handler
*
* @param    pimsmCb        @b{(input)}  control block  
* @param    pMgmdGrpInfo   @b{(input)}  igmp group info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmIgmpMemberUpdateProcess(pimsmCB_t * pimsmCb,
               mgmdMrpEventInfo_t *pMgmdGrpInfo)
{
  L7_uint32 numSrcs =0;
  L7_inet_addr_t intfAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_DEBUG,"Entry");
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pMgmdGrpInfo))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  if ( pMgmdGrpInfo->rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
        "wrong  pMgmdGrpInfo->rtrIfNum = %d", pMgmdGrpInfo->rtrIfNum);
     return L7_FAILURE;   
  }
  
  numSrcs = pMgmdGrpInfo->numSrcs;
  if(inetIsLANScopedAddress(&pMgmdGrpInfo->group) == L7_TRUE)
  {
    /* Don't create routing entries for the LAN scoped addresses */
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                     "LAN scoped addresses: ",&pMgmdGrpInfo->group);
    return L7_FAILURE; 
  }

  inetAddressZeroSet(pimsmCb ->family, &intfAddr);
  if(pMgmdGrpInfo->mode == MGMD_FILTER_MODE_INCLUDE)
  {
    if(pimsmIgmpMemberIncludeModeProcess(pimsmCb,pMgmdGrpInfo,numSrcs) 
                                         != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "pimsmIgmpMemberIncludeModeProcess failed");
      return L7_FAILURE;
    }
  }
  else if(pMgmdGrpInfo->mode == MGMD_FILTER_MODE_EXCLUDE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_DEBUG,"Group Mode is EXCLUDE");
    if(pimsmIgmpMemberExcludeModeProcess(pimsmCb,pMgmdGrpInfo,numSrcs) 
                                         != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                  "pimsmIgmpMemberIncludeModeProcess failed");
      return L7_FAILURE;
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_DEBUG,"Exit"); 
  return L7_SUCCESS;
}

