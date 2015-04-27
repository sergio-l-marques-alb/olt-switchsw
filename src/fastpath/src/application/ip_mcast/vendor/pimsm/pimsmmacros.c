/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmCommonFSM.c
*
* @purpose Contains various macros implementation for PIM-SM protocol 
           operation
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
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmneighbor.h"
#include "l7_ip_api.h"
#include "rto_api.h"
#include "l7sll_api.h"
#include "pimsmcontrol.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"
#include "pimsmrp.h"
#include "mcast_wrap.h"
#include "dtl_l3_mcast_api.h"
#include "pimsmmrt.h"
#include "mfc_api.h"
#include "pimsm_vend_ctrl.h"

/******************************************************************************
* @purpose  Return the neighbor's primary address
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum     @b{(input)} router interface Number 
* @param    pNbrAddr         @b{(input)} Neighbor addr
* @param    primaryAddr    @b{(output)} Primary neighbor Addr
*
* @returns  L7_SUCCESS / L7_FAILURE
*
* @comments     
* The function NBR( I, A ) uses information gathered through PIM Hellomessages 
* to map the IP address A of a directly connected PIM neighborrouter on 
* interface I to the primary IP address of the same router     
* @end
******************************************************************************/
L7_RC_t pimsmNbr(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum,
         L7_inet_addr_t * pNbrAddr, L7_inet_addr_t * primaryAddr)
{
  pimsmInterfaceEntry_t *pIntfEntry;
  pimsmNeighborEntry_t  *pNbrEntry = L7_NULLPTR;
  L7_RC_t rc;
  L7_uint32 i;

  if(pimsmCb == L7_NULLPTR || 
     pNbrAddr == L7_NULLPTR || 
     primaryAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  if (pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
      " pimsm interface entry doesnot exist for rtrIfNum = %d ",rtrIfNum);
   return L7_FAILURE;
  }
  rc = pimsmNeighborGetFirst(pimsmCb, pIntfEntry, &pNbrEntry);
  while (rc == L7_SUCCESS)
  {
    if (L7_INET_ADDR_COMPARE(&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress, pNbrAddr) == 0)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
            "given addr is primary addr only");
        inetCopy(primaryAddr,pNbrAddr);
        return L7_SUCCESS;
      }
      for (i = 0;  i < PIMSM_MAX_NBR_SECONDARY_ADDR &&
           inetIsAddressZero(&pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]) == L7_FALSE;
           i++)
      {
        if (pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] == L7_FALSE)
        {
            continue;
        }
        if (L7_INET_ADDR_COMPARE(&pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i],
               pNbrAddr) == 0)
        {
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, " found primary addr = ",
            &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
          inetCopy(primaryAddr,&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
          return L7_SUCCESS;
        }
      }
   rc = pimsmNeighborNextGet(pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry);
  }
  /* 
   In case where (*,G)/(S,G) is created before upstream-neighborship
   is established, above code will fail to return any address
   as there are no neighbors. Hence (*,G)/(S,G) upstream-neighbor will
   not be updated properly.
   Assuming that RTO returns only the primary address in NextHop
   we can safely copy the same.
   */  
  inetCopy(primaryAddr,pNbrAddr);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  local_receiver_include(*,G,I)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode   @b{(input)} (*,G) node
* @param    rtrIfNum    @b{(input)}  router interface number
*
* @returns  L7_TRUE
* @returns  L7_FAILSE
*
* @comments 
*
local_receiver_include(*,G,I)" is true if the IGMP/MLD module or
other local membership mechanism has determined that local members on
interface I desire to receive all traffic sent to G (possibly excluding
traffic from a specific set of sources).
* @end
*********************************************************************/
static L7_BOOL pimsmStarGILocalReceiverInclude ( pimsmCB_t * pimsmCb,
                     pimsmStarGNode_t * pStarGNode,
                     L7_uint32         rtrIfNum )
{
  pimsmStarGIEntry_t *  pStarGIEntry;
  L7_BOOL result;

  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "pStarGIEntry[%d] is NULLPTR",  
      rtrIfNum);
    return L7_FALSE;
  }
  result = pStarGIEntry->pimsmStarGILocalMembership;
  return result;
}
/*********************************************************************
* @purpose  local_receiver_include(S,G,I)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)}  (S,G) node
* @param    rtrIfNum    @b{(input)}  router interface number
*
* @returns  L7_TRUE
* @returns  L7_FAILSE
*
* @comments 
*
The clause "local_receiver_include(S,G,I)" is true if the IGMP/MLD
module or other local membership mechanism has determined that local
members on interface I desire to receive traffic sent specifically by S
to G.
* @end
*********************************************************************/
static L7_BOOL pimsmSGILocalReceiverInclude ( pimsmCB_t * pimsmCb,
                                              pimsmSGNode_t * pSGNode,
                                              L7_uint32         rtrIfNum )
{
  pimsmSGIEntry_t *     pSGIEntry; 
  L7_BOOL result;

  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if (pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "pSGIEntry[%d] is NULLPTR", 
      rtrIfNum);
    return L7_FALSE;
  }
  
  result = pSGIEntry->pimsmSGILocalMembership ;
  return result;
}

/*********************************************************************
* @purpose  local_receiver_exclude(S,G,I)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode   @b{(input)}  (S,G,rpt) node  
* @param    rtrIfNum    @b{(input)}  router interface number 
*
* @returns  L7_TRUE
* @returns  L7_FAILSE
*
* @comments 
*
local_receiver_exclude(S,G,I)
is true if "local_receiver_include(*,G,I)" is true but none of the local
members desire to receive traffic from S.

this corresponds to pimSGRptILocalMembership 
refer draft-ietf-pim-mib-v2-06.txt

* @end
*********************************************************************/
static L7_BOOL pimsmSGILocalReceiverExclude ( pimsmCB_t * pimsmCb,
                       pimsmSGRptNode_t *pSGRptNode,
                       L7_uint32         rtrIfNum )
{

  L7_BOOL result;
  pimsmSGRptIEntry_t *pSGRptIEntry;

  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if (pSGRptIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "pSGRptIEntry[%d] is NULLPTR",
      rtrIfNum);
    return L7_FALSE;
  }
  
  result = pSGRptIEntry->pimsmSGRptILocalMembership ;
  return result;
}
/******************************************************************************
* @purpose  lost_assert(S,G,I)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node 
* @param    rtrIfNum    @b{(input)}  router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* bool lost_assert(S,G,I) {
*    if ( RPF_interface(S) == I ) {
*       return FALSE
*    } else {
*       return ( AssertWinner(S,G,I) != NULL AND
*                AssertWinner(S,G,I) != me  AND
*                (AssertWinnerMetric(S,G,I) is better
*                   than spt_assert_metric(S,I) )
*    }
*  }
*
*  assert_metric
*  spt_assert_metric(S,I) {
*     return {0,MRIB.pref(S),MRIB.metric(S),my_ip_address(I)}
*  }        
* @end
******************************************************************************/
static L7_BOOL pimsmSGILostAssert ( pimsmCB_t * pimsmCb,
                                    pimsmSGNode_t * pSGNode,
                                    L7_uint32      rtrIfNum )
{
  pimsmSGIEntry_t * pSGIEntry;
  pimsmInterfaceEntry_t * pIntfEntry; 
  
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                 "Upstream Index is equal to Downstream index" );        
    return L7_FALSE;
  }
  else
  {
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      return L7_FALSE;
    }
    if(inetIsAddressZero( &pSGIEntry->pimsmSGIAssertWinnerAddress ) !=
       L7_TRUE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Assert Winnder Address :", 
         &pSGIEntry->pimsmSGIAssertWinnerAddress );
      if(inetUnNumberedIsLocalAddress(&pSGIEntry->pimsmSGIAssertWinnerAddress, rtrIfNum)
          != L7_TRUE) {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,
         "I am not the Assert Winner" );       
        if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
            "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
          return L7_FALSE;
        } 
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  
         PIMSM_TRACE_DEBUG, "Address :",  &pIntfEntry->pimsmInterfaceAddr);
        if(
          pimsmCompareMetrics(
                             pSGIEntry->pimsmSGIAssertWinnerMetricPref,
                             pSGIEntry->pimsmSGIAssertWinnerMetric,
                             &pSGIEntry->pimsmSGIAssertWinnerAddress,
                             pSGNode->pimsmSGEntry.pimsmSGRPFRouteMetricPref,
                             pSGNode->pimsmSGEntry.pimsmSGRPFRouteMetric,
                             &pIntfEntry->pimsmInterfaceAddr) == L7_TRUE)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "I am the Assert Loser" );
          return L7_TRUE;
        }
      }
    }
  }
  return L7_FALSE;
}

/******************************************************************************
* @purpose  lost_assert(*,G,I)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node 
* @param    rtrIfNum    @b{(input)}  router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* bool lost_assert(*,G,I) {
*    if ( RPF_interface(RP(G)) == I ) {
*       return FALSE
*    } else {
*       return ( AssertWinner(*,G,I) != NULL AND
*                AssertWinner(*,G,I) != me )
*    }
*  }        
* @end
******************************************************************************/
static L7_BOOL pimsmStarGILostAssert ( pimsmCB_t * pimsmCb,
                                       pimsmStarGNode_t * pStarGNode,
                                       L7_uint32         rtrIfNum )
{
  pimsmStarGIEntry_t *  pStarGIEntry;


  /*PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,"Entry");  */
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode == ( pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }

  if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex == rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                 "Upstream index and downstream index are the same" );  
    return L7_FALSE;
  }
  else
  {
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry == ( pimsmStarGIEntry_t * )L7_NULLPTR)
    {
      return L7_FALSE;
    }
    if(inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress ) != 
      L7_TRUE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Winner Address :",
         &pStarGIEntry->pimsmStarGIAssertWinnerAddress );   
      if(inetUnNumberedIsLocalAddress(&pStarGIEntry->pimsmStarGIAssertWinnerAddress, 
         rtrIfNum)
        != L7_TRUE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "I am the not the assert winner.Return TRUE" );    
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}
/******************************************************************************
* @purpose  lost_assert(S,G,rpt,I)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)}  (S,G,rpt) node  
* @param    rtrIfNum    @b{(input)}  router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*   bool lost_assert(S,G,rpt,I) {
*    if ( RPF_interface(RP(G)) == I  OR
*         ( RPF_interface(S) == I AND SPTbit(S,G) == TRUE ) ) {
*       return FALSE
*    } else {
*       return ( AssertWinner(S,G,I) != NULL AND
*                AssertWinner(S,G,I) != me )
*    }
*  }        
* @end
******************************************************************************/
static L7_BOOL pimsmSGRptILostAssert ( pimsmCB_t * pimsmCb,
                                       pimsmSGRptNode_t * pSGRptNode,
                                       L7_uint32         rtrIfNum )
{
  L7_uint32             rtrIfNumRp= L7_NULL, rtrIfNumSrc = L7_NULL;
  pimsmSGNode_t *       pSGNode;
  pimsmSGIEntry_t *     pSGIEntry;
  L7_BOOL               bSPTBit = L7_FALSE;
  L7_RC_t       rc;


  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode == ( pimsmSGRptNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }

  rtrIfNumRp= pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex;

  if(rtrIfNumRp== rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,
        "Upstream Index towards the RP is equal tto the downstream index" );
    return L7_FALSE;
  }
  rc = pimsmSGFind( pimsmCb,
                        &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                        &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress,
                        &pSGNode );

  if(rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "spt Bit is %d", bSPTBit );
  bSPTBit = pSGNode->pimsmSGEntry.pimsmSGSPTBit;
  rtrIfNumSrc = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;

  if(( rtrIfNumSrc == rtrIfNum ) && ( bSPTBit == L7_TRUE ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,
        " rtrIfNumSrc == rtrIfNum  and SPT Bit is set" );
    return L7_FALSE;
  }
  else
  {
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      return L7_FALSE;
    }
    if(inetIsAddressZero( &pSGIEntry->pimsmSGIAssertWinnerAddress ) !=
       L7_TRUE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
            "SG Assert Winner Address :",
         &pSGIEntry->pimsmSGIAssertWinnerAddress );
      if(inetUnNumberedIsLocalAddress(&pSGIEntry->pimsmSGIAssertWinnerAddress, 
            rtrIfNum)
        != L7_TRUE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "I am not the Assert Winner " );
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}
/******************************************************************************
* @purpose  pim_include(*,G)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node    
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* pim_include(*,G) =
*   { all interfaces I such that:
*     ( ( I_am_DR( I ) AND lost_assert(*,G,I) == FALSE ) OR 
*       AssertWinner(*,G,I) == me )     AND  local_receiver_include(*,G,I) 
*    }  
* @end
******************************************************************************/
static L7_RC_t pimsmStarGInclude ( pimsmCB_t * pimsmCb,
                            pimsmStarGNode_t * pStarGNode,
                            interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmStarGIEntry_t *  pStarGIEntry;
  L7_BOOL               bIAmDR, bLostAssert;
 
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     (pStarGNode == (pimsmStarGNode_t *)L7_NULLPTR)||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                "Input is NULLPTR ");
    if(pOif != ( interface_bitset_t * )L7_NULLPTR)
    {
      BITX_RESET_ALL(pOif);
    }
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {

    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry == ( pimsmStarGIEntry_t * )L7_NULLPTR)
    {
       continue;
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,"RtrIfNum %d",rtrIfNum);
    bIAmDR = pimsmIAmDR( pimsmCb, rtrIfNum );
    bLostAssert = pimsmStarGILostAssert(pimsmCb, pStarGNode, 
                                     rtrIfNum);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,"Lost Assert is %d",
                bLostAssert); 
     if(((( bIAmDR == L7_TRUE ) && ( bLostAssert == L7_FALSE ) ) ||
        (inetUnNumberedIsLocalAddress(&pStarGIEntry->pimsmStarGIAssertWinnerAddress, 
            rtrIfNum)
            == L7_TRUE)) &&
       (pStarGIEntry->pimsmStarGILocalMembership == L7_TRUE ))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                   "Setting Intf Bit for rtrIfNum %d",
                   rtrIfNum );    
      BITX_SET( pOif, rtrIfNum );
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  pim_include(S,G)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node  
* @param    pOif        @b{(output)} router interfaces bitset    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
* pim_include(S,G) =    { 
*   all interfaces I such that:
*   ( (I_am_DR( I ) AND lost_assert(S,G,I) == FALSE )
*   OR AssertWinner(S,G,I) == me )
*   AND  local_receiver_include(S,G,I) }    
* @end
******************************************************************************/
static L7_RC_t pimsmSGInclude ( pimsmCB_t * pimsmCb,
                                pimsmSGNode_t * pSGNode,
                                interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmSGIEntry_t *     pSGIEntry; 
  L7_BOOL               bIAmDR, bLostAssert;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );     

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==
       (
       pimsmSGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    if(pOif != ( interface_bitset_t * )L7_NULLPTR)
    {
      BITX_RESET_ALL(pOif);
    }
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }
    
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      continue;
    }

    bIAmDR = pimsmIAmDR( pimsmCb, rtrIfNum );
    bLostAssert = pimsmSGILostAssert( pimsmCb, pSGNode, rtrIfNum );
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Lost Assert is %d", bLostAssert );    
    if(((( bIAmDR == L7_TRUE ) && ( bLostAssert == L7_FALSE ) ) ||
        (inetUnNumberedIsLocalAddress(&pSGIEntry->pimsmSGIAssertWinnerAddress, rtrIfNum)
            == L7_TRUE)) &&
       (  pSGIEntry->pimsmSGILocalMembership == L7_TRUE ))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                   "Setting Intf Bit for rtrIfNum %d",
                   rtrIfNum );    
      BITX_SET( pOif, rtrIfNum );
    }

  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "EXIT:Success" );        
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  pim_exclude(S,G) 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)} (S,G,Rpt) node  
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* pim_exclude(S,G) =
*   { all interfaces I such that:
*    ( (I_am_DR( I ) AND lost_assert(*,G,I) == FALSE )
*     OR AssertWinner(*,G,I) == me )
*     AND  local_receiver_exclude(S,G,I)}
@end
******************************************************************************/
static L7_RC_t pimsmSGExclude ( pimsmCB_t * pimsmCb,
                                pimsmSGRptNode_t * pSGRptNode,
                                interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmStarGNode_t      *pStarGNode = L7_NULLPTR;
  pimsmStarGIEntry_t    *pStarGIEntry = L7_NULLPTR;
  pimsmSGRptIEntry_t    *pSGRptIEntry = L7_NULLPTR;
  L7_BOOL               bIAmDR = L7_FALSE, bLostAssert = L7_FALSE;
  L7_RC_t       rc;

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode == (pimsmSGRptNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    if(pOif != ( interface_bitset_t * )L7_NULLPTR)
    {
      BITX_RESET_ALL(pOif);
    }
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  rc = pimsmStarGFind( pimsmCb,
                           &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                           &pStarGNode );
  if(rc == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "(*,G) not found ");
    return L7_FAILURE;
  }

  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    if ((pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum]) == L7_NULLPTR)
    {
      continue;
    }
    if ((pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum]) == L7_NULLPTR)
    {
      continue;
    }

    bIAmDR = pimsmIAmDR( pimsmCb, rtrIfNum );

    bLostAssert = pimsmStarGILostAssert( pimsmCb, pStarGNode, rtrIfNum );

    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Lost Assert Value is %d", bLostAssert );

    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "(*,G) Assert Winner Address :",
      &pStarGIEntry->pimsmStarGIAssertWinnerAddress );

    if(((( bIAmDR == L7_TRUE ) && ( bLostAssert == L7_FALSE ) ) ||
        (inetUnNumberedIsLocalAddress(&pStarGIEntry->pimsmStarGIAssertWinnerAddress,
            rtrIfNum)
        == L7_TRUE)) &&
       ( pimsmSGILocalReceiverExclude(pimsmCb, pSGRptNode,rtrIfNum)  ==
            L7_TRUE ))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                   "Setting Intf Bit for rtrIfNum %d",
                   rtrIfNum );    
      BITX_SET( pOif, rtrIfNum );
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  joins(*,*,RP)        
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarStarRpNode   @b{(input)} (*,*,RP) node  
* @param    pOif              @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* joins(*,*,RP) =
*    { all interfaces I such that
*      DownstreamJPState(*,*,RP,I) is either Join or
*          Prune-Pending }      
* @end
******************************************************************************/
static L7_RC_t pimsmStarStarRpJoins ( pimsmCB_t * pimsmCb,
                                      pimsmStarStarRpNode_t * pStarStarRpNode,
                                      interface_bitset_t * pOif )
{
  L7_uint32                     rtrIfNum;
  pimsmStarStarRpIEntry_t *     pStarStarRpIEntry;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }
    pStarStarRpIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
    if(pStarStarRpIEntry != ( pimsmStarStarRpIEntry_t * )L7_NULLPTR)
    {
      if(( pStarStarRpIEntry->pimsmStarStarRpIJoinPruneState ==
           PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN ) ||
         (
         pStarStarRpIEntry->pimsmStarStarRpIJoinPruneState ==
         PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_PRUNE_PENDING ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "intf Bit mask set for rtrIfNum %d",
                     rtrIfNum );
        BITX_SET( pOif, rtrIfNum );
      }
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Success" );
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  joins(*,G)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* joins(*,G) =
*    { all interfaces I such that
*      DownstreamJPState(*,G,I) is either Join or Prune-Pending }       
* @end
******************************************************************************/
static L7_RC_t pimsmStarGJoins ( pimsmCB_t * pimsmCb,
                                 pimsmStarGNode_t * pStarGNode,
                                 interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmStarGIEntry_t *  pStarGEntry;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode ==
       (
       pimsmStarGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }
    pStarGEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGEntry != ( pimsmStarGIEntry_t * )L7_NULLPTR)
    {
      if(( pStarGEntry->pimsmStarGIJoinPruneState ==
           PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN ) ||
         (
         pStarGEntry->pimsmStarGIJoinPruneState ==
         PIMSM_DNSTRM_STAR_G_SM_STATE_PRUNE_PENDING ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "Setting Bit mask for rtrIfNum %d ",
                     rtrIfNum );
        BITX_SET( pOif, rtrIfNum );
      }
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Success " );
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  joins(S,G)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* joins(S,G) =
*    { all interfaces I such that
*      DownstreamJPState(S,G,I) is either Join or Prune-Pending }       
* @end
******************************************************************************/
static L7_RC_t pimsmSGJoins ( pimsmCB_t * pimsmCb,
                              pimsmSGNode_t * pSGNode,
                              interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmSGIEntry_t *     pSGIEntry;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==
       (
       pimsmSGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry != ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      if(( pSGIEntry->pimsmSGIJoinPruneState ==
           PIMSM_DNSTRM_S_G_SM_STATE_JOIN ) ||
         ( pSGIEntry->pimsmSGIJoinPruneState ==
           PIMSM_DNSTRM_S_G_SM_STATE_PRUNE_PENDING ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "set Bit mask for rtrIfNum %d",
                     rtrIfNum );
        BITX_SET( pOif, rtrIfNum );
      }
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "EXIT: Success " );
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  prunes(S,G,rpt) 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)} (S,G,rpt) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* prunes(S,G,rpt) =
*    { all interfaces I such that
*      DownstreamJPState(S,G,rpt,I) is Prune or PruneTmp }      
* @end
******************************************************************************/
static L7_RC_t pimsmSGRptPrunes ( pimsmCB_t * pimsmCb,
                                  pimsmSGRptNode_t * pSGRptNode,
                                  interface_bitset_t * pOif )
{
  L7_uint32             rtrIfNum;
  pimsmSGRptIEntry_t *  pSGRptIEntry;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode == (pimsmSGRptNode_t *)L7_NULLPTR)||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
    if(pSGRptIEntry != ( pimsmSGRptIEntry_t * )L7_NULLPTR)
    {
      if(( pSGRptIEntry->pimsmSGRptIJoinPruneState ==
           PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE ) ||
         ( pSGRptIEntry->pimsmSGRptIJoinPruneState ==
           PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                     "Setting BitMask for rtrIfNum %d ",
                     rtrIfNum );
        BITX_SET( pOif, rtrIfNum );
      }
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Success " );
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  lost_assert(*,G) 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* lost_assert(*,G) =
*    { all interfaces I such that
*      lost_assert(*,G,I) == TRUE }     
* @end
******************************************************************************/
static L7_RC_t pimsmStarGLostAssert ( pimsmCB_t * pimsmCb,
                                      pimsmStarGNode_t * pStarGNode,
                                      interface_bitset_t * pOif )
{
  L7_uint32     rtrIfNum;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Entry" ); 
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode ==
       ( pimsmStarGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    if(pimsmStarGILostAssert( pimsmCb,
                              pStarGNode,
                              rtrIfNum ) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO,
                   "Setting intf Bit for rtrIfNum %d",
                   rtrIfNum );  
      BITX_SET( pOif, rtrIfNum );
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Success" );   
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  lost_assert(S,G,rpt)    
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)} (S,G,rpt) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* lost_assert(S,G,rpt) =
*    { all interfaces I such that
*      lost_assert(S,G,rpt,I) == TRUE }     
* @end
******************************************************************************/
static L7_RC_t pimsmSGRptLostAssert ( pimsmCB_t * pimsmCb,
                                      pimsmSGRptNode_t * pSGRptNode,
                                      interface_bitset_t * pOif )
{
  L7_uint32     rtrIfNum;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }
    if(pimsmSGRptILostAssert( pimsmCb,
                              pSGRptNode,
                              rtrIfNum ) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO,
                   "Bit Set for intf Num %d",
                   rtrIfNum );
      BITX_SET( pOif, rtrIfNum );
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Success" );
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  lost_assert(S,G)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node 
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* lost_assert(S,G) =
*    { all interfaces I such that
*      lost_assert(S,G,I) == TRUE }     
* @end
******************************************************************************/
static L7_RC_t pimsmSGLostAssert ( pimsmCB_t * pimsmCb,
                                   pimsmSGNode_t * pSGNode,
                                   interface_bitset_t * pOif )
{
  L7_uint32     rtrIfNum;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );       
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==
       (
       pimsmSGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    if(pimsmSGILostAssert( pimsmCb, pSGNode, rtrIfNum ) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO,
                   "Intf Bit set for Asserted rtrIfNum %d",
                   rtrIfNum );       
      BITX_SET( pOif, rtrIfNum );
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "EXIT:Success" );        
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  CouldAssert(S,G,I)  
*
* @param    pimsmCb    @b{(input)}  control block  
* @param    pSGNode    @b{(input)} (S,G) node 
* @param    rtrIfNum   @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* CouldAssert(S,G,I) =
*     SPTbit(S,G)==TRUE
*     AND (RPF_interface(S) != I)
*     AND (I in ( ( joins(*,*,RP(G)) (+) joins(*,G) (-) prunes(S,G,rpt) )
*                 (+) ( pim_include(*,G) (-) pim_exclude(S,G) )
*                 (-) lost_assert(*,G)
*                 (+) joins(S,G) (+) pim_include(S,G) ) )   
* @end
******************************************************************************/
L7_BOOL pimsmSGICouldAssert ( pimsmCB_t * pimsmCb,
                              pimsmSGNode_t * pSGNode,
                              L7_uint32            rtrIfNum )
{
  interface_bitset_t        oif1, oif2,
  starStarRPJoinsOif,  starGJoinsOif, starGLostAssertsOif,
  starGIncludesOif, sgRptPrunesOif, sgExcludesOif,
  sgJoinsOif, sgIncludesOif;
  pimsmStarStarRpNode_t *   pStarStarRpNode;
  pimsmStarGNode_t *        pStarGNode;
  pimsmSGRptNode_t *        pSGRptNode = L7_NULLPTR;
  L7_inet_addr_t            rpAddr;
  L7_RC_t         rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter");
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGSPTBit != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"SPt Bit is not set.Return");
    return L7_FALSE;
  }

  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "Upstream index and downstream index are same rtrIfNum = %d",
                rtrIfNum);
    return L7_FALSE;
  }

  memset(&starStarRPJoinsOif,0,sizeof(interface_bitset_t));
  if(pimsmRpAddressGet( pimsmCb,
                        &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                        &rpAddr ) == L7_SUCCESS)
  {
    rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
    if(rc == L7_SUCCESS)
    {
      /* joins(*,*,RP(G) */
      pimsmStarStarRpJoins( pimsmCb, pStarStarRpNode, &starStarRPJoinsOif );
    }
  }
  memset(&starGJoinsOif,0,sizeof(interface_bitset_t));           
  memset(&starGIncludesOif,0,sizeof(interface_bitset_t));
  memset(&starGLostAssertsOif,0,sizeof(interface_bitset_t));
  rc = pimsmStarGFind( pimsmCb,
                           &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                           &pStarGNode );
  if(rc == L7_SUCCESS)
  {
    /* joins(*,G) */
    pimsmStarGJoins( pimsmCb, pStarGNode, &starGJoinsOif );
    pimsmStarGInclude( pimsmCb, pStarGNode, &starGIncludesOif );
    pimsmStarGLostAssert( pimsmCb, pStarGNode, &starGLostAssertsOif );
  }

  memset(&sgRptPrunesOif,0,sizeof(interface_bitset_t)); 
  memset(&sgExcludesOif,0,sizeof(interface_bitset_t));
  rc = pimsmSGRptFind( pimsmCb,
                           &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                           &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                           &pSGRptNode );
  if(rc == L7_SUCCESS)
  {
    /* prunes(S,G,rpt)*/
    pimsmSGRptPrunes( pimsmCb, pSGRptNode, &sgRptPrunesOif );

    /* pim_exclude(S,G) */
    pimsmSGExclude( pimsmCb, pSGRptNode, &sgExcludesOif );
  }

  /* joins(*,*,RP(G)) (+) joins(*,G) */
  BITX_OR( &starStarRPJoinsOif, &starGJoinsOif, &oif1 );
  /* AND this with prunes(S,G,rpt) */
  BITX_MASK_AND( &oif1, &sgRptPrunesOif, &oif1 );

  /* pim_include(*,G) (-) pim_exclude(S,G) */
  BITX_MASK_AND( &starGIncludesOif, &sgExcludesOif, &oif2 );

  BITX_OR( &oif1, &oif2, &oif1 ); 
  BITX_MASK_AND( &oif1, &starGLostAssertsOif, &oif1 );

  /* joins(S,G) */
  memset(&sgJoinsOif,0,sizeof(interface_bitset_t));
  pimsmSGJoins( pimsmCb, pSGNode, &sgJoinsOif );
  BITX_OR( &oif1, &sgJoinsOif, &oif1 ); 

  /* pim_include(S,G) */
  memset(&sgIncludesOif,0,sizeof(interface_bitset_t));
  pimsmSGInclude( pimsmCb, pSGNode, &sgIncludesOif );

  BITX_OR( &oif1, &sgIncludesOif, &oif1 );    

  if(BITX_TEST( &oif1, rtrIfNum ) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "Bit is set for rtrIfNum = %d", rtrIfNum);
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit");  
  return L7_FALSE;
}
/*********************************************************************
* @purpose  
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmIsNbrPresent(L7_uchar8 family, L7_uint32 rtrIfNum, 
			L7_inet_addr_t *nbrAddr)
{
  pimsmCB_t             *pimsmCb = L7_NULLPTR;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,PIMSM_TRACE_ERROR,"PIMSM: Invalid Ctrl Block");
    return L7_FALSE;
  }
  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return L7_FALSE;
  }  
  if(pimsmNeighborFind(pimsmCb, pIntfEntry, 
  				nbrAddr, L7_NULLPTR) == L7_SUCCESS)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/******************************************************************************
* @purpose  RPF'(*,*,RP)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)}  (*,G) node
* @param    pNbrAddr    @b{(output)} upstream neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*   neighbor RPF'(*,*,RP) {
*           return NBR(RPF_interface(RP), MRIB.next_hop(RP))
*  }    
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpNeighborRpfDash ( pimsmCB_t * pimsmCb,
           struct  pimsmStarStarRpNode_s * pimsmStarStarRPNode)
{
  L7_uint32             rtrIfNum = L7_NULL;
  L7_inet_addr_t * pNbrAddr;
  
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pimsmStarStarRPNode ==
       (pimsmStarStarRpNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }

  rtrIfNum =
   pimsmStarStarRPNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
  pNbrAddr = 
    &pimsmStarStarRPNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamNeighbor;
  inetAddressZeroSet(pimsmCb->family, pNbrAddr);
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }  
  pimsmNbr( pimsmCb,
    rtrIfNum,
    &pimsmStarStarRPNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFNextHop,
    pNbrAddr);

  /* Reassign primary addr later*/
  if (pimsmCb->family == L7_AF_INET6 && 
      inetIsAddressZero(pNbrAddr) == L7_FALSE &&
      !L7_IP6_IS_ADDR_LINK_LOCAL(&(pNbrAddr->addr.ipv6)))
  {
      pimsmCb->pimsmStarStarRpNextHopUpdateCnt++;     
  }   
  return L7_SUCCESS;   
}
/******************************************************************************
* @purpose  neighbor RPF'(*,G)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pNbrAddr    @b{(output)} upstream neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*   neighbor RPF'(*,G) {
*      if (I_Am_Assert_Loser(*, G, RPF_interface(RP(G)))) {
*           return AssertWinner(*, G, RPF_interface(RP(G)))
*      } else {
*           return NBR(RPF_interface(RP(G)), MRIB.next_hop( RP(G)))
*      }
*  }    
* @end
******************************************************************************/
L7_RC_t pimsmStarGNeighborRpfDash ( pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s * pStarGNode)
{
  L7_uint32             rtrIfNum = L7_NULL;
  pimsmStarGIEntry_t *  pStarGIEntry;
  L7_inet_addr_t * pNbrAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_NORMAL, "Enter " );    
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode ==
       (pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  rtrIfNum = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,  "RTRIfNum is %d", rtrIfNum );
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == ( pimsmStarGIEntry_t * )L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,  "pStarGIEntry is NULL" );    
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,  "pStarGIEntry is %p", pStarGIEntry ); 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,  "Assert State is  %d",
              pStarGIEntry->pimsmStarGIAssertState );
  pNbrAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor;
  inetAddressZeroSet(pimsmCb->family, pNbrAddr);  
  
  if(pStarGIEntry->pimsmStarGIAssertState ==
     PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER)
  {
    inetCopy( pNbrAddr, &pStarGIEntry->pimsmStarGIAssertWinnerAddress );
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "AssertWinnerAddress is :",
                      pNbrAddr );
  }
  else
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "NextHop addr is :",  
                      &pStarGNode->pimsmStarGEntry.pimsmStarGRPFNextHop );
    pimsmNbr( pimsmCb,
              rtrIfNum,
              &pStarGNode->pimsmStarGEntry.pimsmStarGRPFNextHop,
              pNbrAddr );
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_NORMAL, "Exit " );    
  }

  /* Reassign primary addr later*/
  if (pimsmCb->family == L7_AF_INET6 && 
      inetIsAddressZero(pNbrAddr) == L7_FALSE &&
      !L7_IP6_IS_ADDR_LINK_LOCAL(&(pNbrAddr->addr.ipv6)))
  {
      pimsmCb->pimsmStarGNextHopUpdateCnt++;     
  }  
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  RPF'(S,G,rpt)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)} (S,G,rpt) node
* @param    pNbrAddr    @b{(output)} upstream neighbor address

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
* neighbor RPF'(S,G,rpt) {
*      if( I_Am_Assert_Loser(S, G, RPF_interface(RP(G)) ) ) {
*           return AssertWinner(S, G, RPF_interface(RP(G)) )
*      } else {
*           return RPF'(*,G)
*      }
* }     
* @end
******************************************************************************/
L7_RC_t pimsmSGRptNeighborRpfDash ( pimsmCB_t * pimsmCb,
                                    pimsmSGRptNode_t * pSGRptNode)
{
  L7_uint32             rtrIfNum;
  pimsmSGNode_t *       pSGNode;
  pimsmStarGNode_t *    pStarGNode;
  pimsmSGIEntry_t *     pSGIEntry;
  L7_inet_addr_t * pNbrAddr;
  
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode ==(pimsmSGRptNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  pNbrAddr = &pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamNeighbor;
  inetAddressZeroSet(pimsmCb->family, pNbrAddr);

  rtrIfNum = pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }  
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,  "RtrIfNum is %d", rtrIfNum );   
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Src Address :", 
                    &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress ); 
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Grp Address :", 
                    &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress ); 
  if(pimsmSGFind( pimsmCb,
                  &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                  &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress,
                  &pSGNode ) == L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,  "pimsmSGFind Success" );    
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG,  "pimSGIEntry is %p", pSGIEntry );  
    if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                 "(S,G,I) Entry not found! %d",
                 rtrIfNum );
      return L7_FAILURE;
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,  "Assert State is  %d",
              pSGIEntry->pimsmSGIAssertState );
    if(pSGIEntry->pimsmSGIAssertState ==
          PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER)
    {
      inetCopy(pNbrAddr,
        &pSGIEntry->pimsmSGIAssertWinnerAddress );
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
                      "SGIAssertWinnerAddress :",  
                      &pSGIEntry->pimsmSGIAssertWinnerAddress );
      /* Reassign primary addr later*/
      if (pimsmCb->family == L7_AF_INET6 && 
          inetIsAddressZero(pNbrAddr) == L7_FALSE &&
          !L7_IP6_IS_ADDR_LINK_LOCAL(&(pNbrAddr->addr.ipv6)))
      {
          pimsmCb->pimsmSGRptNextHopUpdateCnt++;     
      }   
      return L7_SUCCESS; 
    }
  }    
  if(pimsmStarGFind( pimsmCb,
                       &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                       &pStarGNode ) == L7_SUCCESS)
  {
      inetCopy(pNbrAddr,
                &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO,
                        "pimsmStarGNeighborRpfDash :",  
                        &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor );
    /* Reassign primary addr later*/
    if (pimsmCb->family == L7_AF_INET6 && 
        inetIsAddressZero(pNbrAddr) == L7_FALSE &&
        !L7_IP6_IS_ADDR_LINK_LOCAL(&(pNbrAddr->addr.ipv6)))
    {
        pimsmCb->pimsmSGRptNextHopUpdateCnt++;     
    }   

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  RPF'(S,G)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
* @param    pNbrAddr    @b{(output)} upstream neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
* neighbor RPF'(S,G) {
*      if ( I_Am_Assert_Loser(S, G, RPF_interface(S) )) {
*           return AssertWinner(S, G, RPF_interface(S) )
*      } else {
*           return NBR( RPF_interface(S), MRIB.next_hop( S ) )
*      }
*  }        
* @end
******************************************************************************/
L7_RC_t pimsmSGNeighborRpfDash ( pimsmCB_t * pimsmCb,
                                 pimsmSGNode_t * pSGNode)
{
  L7_uint32             rtrIfNum = L7_NULL;
  pimsmSGIEntry_t *     pSGIEntry;
  L7_inet_addr_t * pNbrAddr;
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==(pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }

  rtrIfNum = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;    
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }  
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "(S,G, I) Entry not found! " );
    return L7_FAILURE;
  }
  pNbrAddr =&pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor;
  inetAddressZeroSet(pimsmCb->family, pNbrAddr);  

  if(pSGIEntry->pimsmSGIAssertState ==
     PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER)
  {
    inetCopy( pNbrAddr, 
              &pSGIEntry->pimsmSGIAssertWinnerAddress );
  }
  else
  {
    pimsmNbr( pimsmCb,
              rtrIfNum,
              &pSGNode->pimsmSGEntry.pimsmSGRPFNextHop,
              pNbrAddr );
  }
  /* Reassign primary addr later*/
  if (pimsmCb->family == L7_AF_INET6 && 
      inetIsAddressZero(pNbrAddr) == L7_FALSE &&
      !L7_IP6_IS_ADDR_LINK_LOCAL(&(pNbrAddr->addr.ipv6)))
  {
      pimsmCb->pimsmSGNextHopUpdateCnt++;     
  }    
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  immediate_olist(*,*,RP)     
*
* @param    pimsmCb            @b{(input)}  control block  
* @param    pStarStarRpNode    @b{(input)} (*,*,RP) node
* @param    pOif               @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* immediate_olist(*,*,RP) =  joins(*,*,RP)      
* @end
******************************************************************************/   
L7_RC_t pimsmStarStarRpImdtOlist ( pimsmCB_t * pimsmCb,
                                   pimsmStarStarRpNode_t * pStarStarRpNode,
                                   interface_bitset_t * pOif )
{

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarStarRpNode ==
       ( pimsmStarStarRpNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  /*    immediate_olist(*,*,RP) =    joins(*,*,RP)s */
  return pimsmStarStarRpJoins( pimsmCb,
                               pStarStarRpNode,
                               pOif );
}
/******************************************************************************
* @purpose  immediate_olist(*,G)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* immediate_olist(*,G) =      
*   joins(*,G) (+) pim_include(*,G) (-) lost_assert(*,G)        
* @end
******************************************************************************/
L7_RC_t pimsmStarGImdtOlist ( pimsmCB_t * pimsmCb,
                              pimsmStarGNode_t * pStarGNode,
                              interface_bitset_t * pOif )
{
  interface_bitset_t    starGJoinsOif, starGIncludesOif, 
  starGLostAssertsOif, resultOif;

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode ==
       ( pimsmStarGNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset(pOif, 0, sizeof(interface_bitset_t));
  pimsmStarGJoins( pimsmCb, pStarGNode, &starGJoinsOif );

  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &starGJoinsOif );

  pimsmStarGInclude( pimsmCb, pStarGNode, &starGIncludesOif );

  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &starGIncludesOif );

  pimsmStarGLostAssert( pimsmCb, pStarGNode, &starGLostAssertsOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &starGLostAssertsOif );

  BITX_OR( &starGJoinsOif, &starGIncludesOif, &resultOif );

  BITX_MASK_AND( &resultOif, &starGLostAssertsOif, pOif );

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  immediate_olist(S,G)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* immediate_olist(S,G) =      
*   joins(S,G) (+) pim_include(S,G) (-) lost_assert(S,G)
* @end
******************************************************************************/
L7_RC_t pimsmSGImdtOlist ( pimsmCB_t * pimsmCb,
                           struct pimsmSGNode_s * pSGNode,
                           interface_bitset_t * pOif )
{
  interface_bitset_t    sgJoinsOif, sgIncludesOif, 
  sgLostAssertsOif, resultOif;

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==
       (struct pimsmSGNode_s * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset(pOif, 0, sizeof(interface_bitset_t));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );     
  pimsmSGJoins( pimsmCb, pSGNode, &sgJoinsOif );

  pimsmSGInclude( pimsmCb, pSGNode, &sgIncludesOif );

  pimsmSGLostAssert( pimsmCb, pSGNode, &sgLostAssertsOif );

  BITX_OR( &sgJoinsOif, &sgIncludesOif, &resultOif );

  BITX_MASK_AND( &resultOif, &sgLostAssertsOif, pOif );

  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, pOif );
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit" );        
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  inherited_olist(S,G,rpt)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGRptNode  @b{(input)} (S,G,rpt) node
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* inherited_olist(S,G,rpt) =        
*           ( joins(*,*,RP(G))  (+)     joins(*,G)  (-)     prunes(S,G,rpt) ) 
*       (+) ( pim_include(*,G)  (-)  pim_exclude(S,G))      
*       (-) ( lost_assert(*,G)    (+) lost_assert(S,G,rpt) )        
* @end
******************************************************************************/
L7_RC_t pimsmSGRptInhrtdOlist ( pimsmCB_t * pimsmCb,
                                pimsmSGRptNode_t * pSGRptNode,
                                interface_bitset_t * pOif )
{
  pimsmStarStarRpNode_t *   pStarStarRpNode;
  pimsmStarGNode_t *        pStarGNode;
  L7_inet_addr_t            rpAddr;
  interface_bitset_t        starStarRPJoinsOif, starGJoinsOif, sgRptPrunesOif, 
  starGIncludesOif, sgExcludesOif, starGLostAssertsOif, sgRptLostAssertsOif;
  interface_bitset_t        tmpOif1, tmpOif2; 
  L7_RC_t         rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode == ( pimsmSGRptNode_t * )L7_NULLPTR ) ||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );

  memset(&starStarRPJoinsOif,0,sizeof(interface_bitset_t));
  if(pimsmRpAddressGet( pimsmCb,
                        &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                        &rpAddr ) == L7_SUCCESS)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "RP Address :",  &rpAddr );
    rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
    if(rc == L7_SUCCESS)
    {
      /* joins(*,*,RP(G) */
      pimsmStarStarRpJoins( pimsmCb, pStarStarRpNode, &starStarRPJoinsOif );
    }
  }

  memset(&starGJoinsOif,0,sizeof(interface_bitset_t));
  memset(&starGIncludesOif,0,sizeof(interface_bitset_t));
  memset(&starGLostAssertsOif,0,sizeof(interface_bitset_t));
  rc = pimsmStarGFind( pimsmCb,
                           &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                           &pStarGNode );
  if(rc == L7_SUCCESS)
  {
    pimsmStarGJoins( pimsmCb, pStarGNode, &starGJoinsOif );
    pimsmStarGInclude( pimsmCb, pStarGNode, &starGIncludesOif );
    pimsmStarGLostAssert( pimsmCb, pStarGNode, &starGLostAssertsOif );    
  }

  memset(&sgRptPrunesOif,0,sizeof(interface_bitset_t));
  pimsmSGRptPrunes( pimsmCb, pSGRptNode, &sgRptPrunesOif );

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "pimsmSGRptPrunes bits set are:" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgRptPrunesOif );        
  memset(&tmpOif1,0,sizeof(interface_bitset_t));
  BITX_OR( &starStarRPJoinsOif, &starGJoinsOif, &tmpOif1 );

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "*,*,RP + *,G join bits set are:" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &tmpOif1 );        

  BITX_MASK_AND( &tmpOif1, &sgRptPrunesOif, &tmpOif1 );
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
               "Result of (*,*,RP)Join + (*,G)Join -(S,G)Rpt Prunes :" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &tmpOif1 );        


  memset(&sgExcludesOif,0,sizeof(interface_bitset_t));
  pimsmSGExclude( pimsmCb, pSGRptNode, &sgExcludesOif );

  BITX_MASK_AND( &starGIncludesOif, &sgExcludesOif, &tmpOif2 );  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
               "Result of pimsmStarGInclude - pimsmStarGInclude is:" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &tmpOif2 );
  BITX_OR( &tmpOif1, &tmpOif2, &tmpOif1 );

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "B4 Calling pimsmSGRptLostAssert" );
  memset(&sgRptLostAssertsOif,0,sizeof(interface_bitset_t));
  pimsmSGRptLostAssert( pimsmCb, pSGRptNode, &sgRptLostAssertsOif );

  BITX_OR( &starGLostAssertsOif, &sgRptLostAssertsOif, &tmpOif2 );

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
               "Result of pimsmStarGLostAssert + pimsmSGRptLostAssert is:" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &tmpOif2 );

  BITX_MASK_AND( &tmpOif1, &tmpOif2, pOif );

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Final Result  is:" );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, pOif );
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit" );
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  inherited_olist(S,G)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
* @param    pOif        @b{(output)} router interfaces bitset
*
* @returns  L7_SUCCESS
*
* @comments      
* inherited_olist(S,G) = 
*       inherited_olist(S,G,rpt) (+)  
*       joins(S,G) (+) pim_include(S,G) (-) lost_assert(S,G)    
* @end
******************************************************************************/
L7_RC_t pimsmSGInhrtdOlist ( pimsmCB_t * pimsmCb,
                             pimsmSGNode_t * pSGNode,
                             interface_bitset_t * pOif )
{
  pimsmSGRptNode_t *    pSGRptNode;
  interface_bitset_t    sgRptInhrtdOif, sgJoinsOif, 
  sgIncludesOif, sgLostAssertsOif;
  interface_bitset_t    resultOif;
  L7_RC_t       rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry" );

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode ==  ( pimsmSGNode_t * )L7_NULLPTR )||
     ( pOif == ( interface_bitset_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "%p, %p, %p ", pimsmCb, pSGNode, pOif);
    return L7_FAILURE;
  }
  memset( pOif, 0, sizeof( interface_bitset_t ) );
  memset( &sgRptInhrtdOif, 0, sizeof( interface_bitset_t ) );
  memset( &sgJoinsOif, 0, sizeof( interface_bitset_t ) );
  memset( &sgIncludesOif, 0, sizeof( interface_bitset_t ) );
  rc = pimsmSGRptFind( pimsmCb,
                           &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                           &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                           &pSGRptNode );
  if(rc == L7_SUCCESS)
  {
    pimsmSGRptInhrtdOlist( pimsmCb, pSGRptNode, &sgRptInhrtdOif );
    PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgRptInhrtdOif );
  }
  pimsmSGJoins( pimsmCb, pSGNode, &sgJoinsOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgJoinsOif );    

  pimsmSGInclude( pimsmCb, pSGNode, &sgIncludesOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgIncludesOif );

  BITX_OR( &sgRptInhrtdOif, &sgJoinsOif, &resultOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &resultOif );

  BITX_OR( &resultOif, &sgIncludesOif, &resultOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &resultOif );

  pimsmSGLostAssert( pimsmCb, pSGNode, &sgLostAssertsOif );  
  BITX_MASK_AND( &resultOif, &sgLostAssertsOif, pOif );     

  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, pOif );
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit" );    
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Calculate LAN Prune Delay option is supported on all neighbors 
*           for a given interface
*
* @param    pimsmCb     @b{(input)} PIMSM control block  
* @param    rtrIfNum    @b{(input)} interface on which LAN_Prune_Delay option
*                                   is checked
* @returns  L7_SUCCESS
*
* @comments      
*     bool
*     lan_delay_enabled(I) {
*         for each neighbor on interface I {
*             if ( neighbor.lan_prune_delay_present == false ) {
*                 return false
*             }
*         }
*         return true
*     }   
* @end
******************************************************************************/
L7_BOOL pimsmLANDelayEnabled(pimsmCB_t *pimsmCb, pimsmInterfaceEntry_t *pIntfEntry)
{
  pimsmNeighborEntry_t  *pNbrEntry;  
  L7_RC_t                rc;
  
  rc = pimsmNeighborGetFirst( pimsmCb, pIntfEntry, &pNbrEntry );
  while(rc == L7_SUCCESS)
  {
    if (pNbrEntry->pimsmNeighborLANPruneDelayPresent == L7_FALSE)
    {
      return L7_FALSE;
    }
    rc = pimsmNeighborNextGet( pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry );
  }
  return L7_TRUE;
}

/******************************************************************************
* @purpose  Calculate Effective Propagation Delay for a given interface
*
* @param    pimsmCb       @b{(input)} PIMSM control block  
* @param    rtrIfNum      @b{(input)} interface on which LAN_Prune_Delay option
*                                   is checked
* @param    overrideIntvl @b{(input)} Override interval in seconds.
*
* @returns  L7_SUCCESS
*
* @comments      
*
*     time_interval
*     Effective_Override_Interval(I) {
*         if ( lan_delay_enabled(I) == false ) {
*             return t_override_default
*         }
*         delay = Override_Interval(I)
*         for each neighbor on interface I {
*             if ( neighbor.override_interval > delay ) {
*                 delay = neighbor.override_interval
*             }
*         }
*         return delay
*     }
* 
* @end
******************************************************************************/
L7_RC_t pimsmEffectiveOverrideInterval(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum,
                                       L7_uint32 *overrideIntvl)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;  
  pimsmNeighborEntry_t  *pNbrEntry = L7_NULLPTR;  
  L7_uint32              overrideInterval;
  L7_RC_t                rc;

  *overrideIntvl = 0;
  if (pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
                 " Failed to get interface entry for rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;
  }

  if (pIntfEntry->pimsmLANPruneDelayPresent == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
     " Atleast one neighbor does not support LAN Prune Delay for rtrIfNum = %d",
     rtrIfNum);
    *overrideIntvl = PIMSM_DEFAULT_OVERRIDE_INTERVAL_MSECS / 1000;
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"OverrideInterval(%d) for rtrIfNum(%d)",
        *overrideIntvl, rtrIfNum);    
    return L7_SUCCESS;
  }

  overrideInterval = pIntfEntry->pimsmOverrideInterval;
  rc = pimsmNeighborGetFirst( pimsmCb, pIntfEntry, &pNbrEntry );
  while(rc == L7_SUCCESS)
  {
    if (pNbrEntry->pimsmNeighborLANPruneDelayPresent != L7_FALSE)
    {
     if (pNbrEntry->pimsmNeighborOverrideInterval > overrideInterval)
     {
       overrideInterval = pNbrEntry->pimsmNeighborOverrideInterval;
     }
    }
    rc = pimsmNeighborNextGet( pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry );
  }
  *overrideIntvl = overrideInterval / 1000;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"OverrideInterval(%d) for rtrIfNum(%d)",
        *overrideIntvl, rtrIfNum);  
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Calculate Effective Override Interval for a given interface
*
* @param    pimsmCb     @b{(input)} PIMSM control block  
* @param    rtrIfNum    @b{(input)} interface on which LAN_Prune_Delay option
*                                   is checked
* @param    propDelay   @b{(input)} Propagation Delay in seconds.
*
* @returns  L7_SUCCESS
*
* @comments      
*
*     time_interval
*     Effective_Propagation_Delay(I) {
*         if ( lan_delay_enabled(I) == false ) {
*             return Propagation_delay_default
*         }
*         delay = Propagation_Delay(I)
*         for each neighbor on interface I {
*             if ( neighbor.propagation_delay > delay ) {
*                 delay = neighbor.propagation_delay
*             }
*         }
*         return delay
*     }
* 
* @end
******************************************************************************/
L7_RC_t pimsmEffectivePropagationDelay(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum,
                                       L7_uint32 *propDelay)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;  
  pimsmNeighborEntry_t  *pNbrEntry = L7_NULLPTR;  
  L7_uint32              propagationDelay;
  L7_RC_t                rc;

  *propDelay=  0;
  if (pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
                 " Failed to get interface entry for rtrIfNum = %d", rtrIfNum);

    return L7_FAILURE;
  }

  if (pIntfEntry->pimsmLANPruneDelayPresent == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
     " Atleast one neighbor does not support LAN Prune Delay for rtrIfNum = %d",
     rtrIfNum);
    *propDelay = PIMSM_DEFAULT_PROPAGATION_DELAY_MSECS /1000;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"PropagationDelay(%d) for rtrIfNum(%d)",
        *propDelay, rtrIfNum);
    return L7_SUCCESS;
  }

  propagationDelay = pIntfEntry->pimsmPropagationDelay;
  rc = pimsmNeighborGetFirst( pimsmCb, pIntfEntry, &pNbrEntry );
  while(rc == L7_SUCCESS)
  {
    if (pNbrEntry->pimsmNeighborLANPruneDelayPresent != L7_FALSE)
    {
       if (pNbrEntry->pimsmNeighborPropagationDelay > propagationDelay)
       {
         propagationDelay = pNbrEntry->pimsmNeighborPropagationDelay;
       }
    }
    rc = pimsmNeighborNextGet( pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry );
  }
  *propDelay = propagationDelay / 1000;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"PropagationDelay(%d) for rtrIfNum(%d)",
        *propDelay, rtrIfNum);  
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Calculate and return Join prune override interval 
*
* @param    void    
*
* @returns   join/prune override interval in secs
*
* @comments      
*       
* @end
******************************************************************************/
L7_uint32 pimsmJoinPruneOverrideIntervalGet(pimsmCB_t *pimsmCb,L7_uint32 rtrIfNum)
{
  /********************************************************
  Propagation_delay_default   =  0.5 secs    
  Propagation_Delay(I) default is Propagation_delay_default   
  t_override_default = |    2.5 secs     
  Override_Interval(I) default is t_override_default
  bool lan_delay_enabled(I) {
      for each neighbor on interface I {
          if ( neighbor.lan_prune_delay_present == false ) {
              return false
          }
      }
      return true
    }
    Effective_Override_Interval(I) {
      if ( lan_delay_enabled(I) == false ) {
          return t_override_default
      }
      delay = Override_Interval(I)
      for each neighbor on interface I {
          if ( neighbor.override_interval > delay ) {
              delay = neighbor.override_interval
          }
      }
      return delay
    }
    Effective_Propagation_Delay(I) {
      if ( lan_delay_enabled(I) == false ) {
          return Propagation_delay_default
      }
      delay = Propagation_Delay(I)
      for each neighbor on interface I {
          if ( neighbor.propagation_delay > delay ) {
              delay = neighbor.propagation_delay
          }
      }
      return delay
    }
          By default,
          J/P_Override_Interval(I) = Effective_Propagation_Delay(I) + 
                Effective_Override_Interval(I)      
  ********************************************************/   

  L7_uint32 propagationDelay = 0;
  L7_uint32 overrideIntvl = 0;

  pimsmEffectivePropagationDelay(pimsmCb, rtrIfNum, &propagationDelay);
  pimsmEffectiveOverrideInterval(pimsmCb, rtrIfNum, &overrideIntvl); 

  return ( (propagationDelay + overrideIntvl)); 
}
/******************************************************************************
* @purpose  return the RPF interface for the given inet addr 
*
* @param    pimsmCb    @b{(input)}  control block  
* @param    pAddr      @b{(input)}   address
* @param    pRtrIfNum   @b{(output)} router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
* RPF_interface(S) is the interface the MRIB indicates would be used
* to route packets to S     
* @end
******************************************************************************/
L7_RC_t pimsmRPFInterfaceGet ( pimsmCB_t * pimsmCb,
                               L7_inet_addr_t * pAddr,
                               L7_uint32 * pRtrIfNum )
{

  return mcastRPFInterfaceGet(pAddr, pRtrIfNum);
}

/******************************************************************************
* @purpose  JoinDesired(S,G) 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*   bool JoinDesired(S,G) {
*      return( immediate_olist(S,G) != NULL
*              OR ( KeepaliveTimer(S,G) is running
*                   AND inherited_olist(S,G) != NULL ) )        
* @end
******************************************************************************/
L7_BOOL pimsmSGJoinDesired ( pimsmCB_t * pimsmCb,
                             pimsmSGNode_t * pSGNode )
{
  pimsmSGEntry_t *      pSGEntry;
  interface_bitset_t    sgImdOif, sgInhrtdOif;
  L7_uint32             timerval;
  L7_BOOL               sgImdFlag, sgInhrtFlag;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Enter " );        
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;
  pimsmSGImdtOlist( pimsmCb, pSGNode, &sgImdOif );
  BITX_IS_EMPTY( &sgImdOif, sgImdFlag );
  if(sgImdFlag == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit:TRUE (ImdtOlist != NULL)" );    
    return L7_TRUE;
  }
  pimsmSGInhrtdOlist( pimsmCb, pSGNode, &sgInhrtdOif );
  if(appTimerTimeLeftGet( pimsmCb->timerCb,
                          pSGEntry->pimsmSGKeepaliveTimer,
                          &timerval ) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_NORMAL, 
                 "pimsmSGKeepaliveTimer is not running" );    
    return L7_FALSE;

  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Time Left is %d ", timerval );    

  BITX_IS_EMPTY( &sgInhrtdOif, sgInhrtFlag );
  if(( timerval > 0 ) && ( sgInhrtFlag != L7_TRUE ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                 "Exit:TRUE (Ihrtdlst & timer) " );    
    return L7_TRUE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit:FALSE " );
  return L7_FALSE;
}

/******************************************************************************
* @purpose  RPTJoinDesired(G)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pGrpAddr    @b{(input)} group address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* bool RPTJoinDesired(G) {
*    return (JoinDesired(*,G) OR JoinDesired(*,*,RP(G))) }
*       
* @end
******************************************************************************/
L7_BOOL pimsmRPTJoinDesired ( pimsmCB_t * pimsmCb, L7_inet_addr_t * pGrpAddr )
{
  pimsmStarGNode_t *        pStarGNode;
  pimsmStarStarRpNode_t *   pStarStarRpNode;
  L7_inet_addr_t            rpAddr;
  L7_RC_t         rc;
  L7_BOOL starStarRpJoinDesired  = L7_FALSE, starGJoinDesired = L7_FALSE;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Enter " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pGrpAddr == ( L7_inet_addr_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  rc = pimsmStarGFind( pimsmCb, pGrpAddr, &pStarGNode );
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG,
                 "(*,G) node or not found" );
    rc = pimsmRpAddressGet( pimsmCb, pGrpAddr, &rpAddr );
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                   "RP not found" );
      return L7_FALSE;
    }
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "RP Address :",  &rpAddr );  
    rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                   "(*,*,Rp) node or not found" );
      return L7_FALSE;
    }
    else
    {
      starStarRpJoinDesired = pimsmStarStarRpJoinDesired( pimsmCb,
                                                          pStarStarRpNode );
    }
  }
  else
  {
    starGJoinDesired = pimsmStarGJoinDesired( pimsmCb,
                                              pStarGNode );
  }

  if(starGJoinDesired == L7_TRUE  ||
     starStarRpJoinDesired == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit:TRUE ");    
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit:FALSE " );    
  return L7_FALSE;
}


/******************************************************************************
* @purpose  PruneDesired(S,G,rpt)   
*
* @param    pimsmCb       @b{(input)}  control block  
* @param    pSGRptNode    @b{(input)} (S,G,rpt) node    
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*   bool PruneDesired(S,G,rpt) {
*       return ( RPTJoinDesired(G) AND
*                ( inherited_olist(S,G,rpt) == NULL
*                  OR (SPTbit(S,G)==L7_TRUE
*                      AND (RPF'(*,G) != RPF'(S,G)) )))
*   
* @end
******************************************************************************/
L7_BOOL pimsmSGRptPruneDesired ( pimsmCB_t * pimsmCb,
                                 pimsmSGRptNode_t * pSGRptNode )
{
  pimsmSGRptEntry_t *   pSGRptEntry;
  L7_inet_addr_t *      pGrpAddr = L7_NULLPTR,
  * pSrcAddr,
  * pRPFStarGAddr = L7_NULLPTR,
  * pRPFSGAddr = L7_NULLPTR;
  pimsmSGNode_t *       pSGNode;
  pimsmStarGNode_t *    pStarGNode;
  interface_bitset_t    oif;
  L7_BOOL               inhrtdFlag;
  L7_RC_t       rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Enter " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGRptNode == ( pimsmSGRptNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
  pGrpAddr = &pSGRptEntry->pimsmSGRptGrpAddress;
  pSrcAddr = &pSGRptEntry->pimsmSGRptSrcAddress;
  if(pimsmRPTJoinDesired( pimsmCb, pGrpAddr ) == L7_TRUE)
  {
    pimsmSGRptInhrtdOlist( pimsmCb, pSGRptNode, &oif );
    BITX_IS_EMPTY( &oif, inhrtdFlag );
    if(( inhrtdFlag == L7_FALSE ))
    {
      rc = pimsmSGFind( pimsmCb, pGrpAddr, pSrcAddr, &pSGNode );
      if(rc != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                     "(S,G) node not found" );
        return L7_FALSE;
      }
      rc = pimsmStarGFind( pimsmCb, pGrpAddr,&pStarGNode );
      if(rc != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                     "(*,G) node  not found" );
        return L7_FALSE;
      }
      pRPFStarGAddr = &(pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor);
      pRPFSGAddr = &(pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor);
      if((inetIsAddressZero(pRPFStarGAddr) == L7_TRUE )||
         (inetIsAddressZero(pRPFSGAddr) == L7_TRUE ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                     "RPF'(*,G) or RPF'(S,G) is zero" );
        return L7_FALSE;
      }
      if(( pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_TRUE ) &&
         ( PIMSM_INET_IS_ADDR_EQUAL( pRPFStarGAddr,
                                   pRPFSGAddr ) == L7_FALSE ))
      {
        return L7_TRUE;
      }
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Inhrted list is empty" );
      return L7_TRUE;
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit " ); 
  return L7_FALSE;
}

/******************************************************************************
* @purpose  Update_SPTbit(S,G,iif)  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node 
* @param    rtrIfNum        @b{(input)} router interface number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*    void
*     Update_SPTbit(S,G,iif) {
*       if ( iif == RPF_interface(S)
*             AND JoinDesired(S,G) == TRUE
*             AND ( DirectlyConnected(S) == TRUE
*                   OR RPF_interface(S) != RPF_interface(RP(G))
*                   OR inherited_olist(S,G,rpt) == NULL
*                   OR ( ( RPF'(S,G) == RPF'(*,G) ) AND
*                        ( RPF'(S,G) != NULL ) )
*                   OR ( I_Am_Assert_Loser(S,G,iif) ) {
*          Set SPTbit(S,G) to TRUE
*       }
*     }     
* @end
******************************************************************************/
void pimsmSPTbitUpdate ( pimsmCB_t * pimsmCb,
                         pimsmSGNode_t * pSGNode,
                         L7_uint32 rtrIfNum )
{
  L7_uint32             rpfIntfS = L7_NULL, rpfIntfG = L7_NULL;
  L7_inet_addr_t *      pRPFSGAddr = L7_NULLPTR,
      * pRPFStarGAddr = L7_NULLPTR,
      rpAddr;
  pimsmSGRptNode_t *    pSGRptNode;
  pimsmSGEntry_t *      pSGEntry;
  pimsmSGIEntry_t *     pSGIEntry;
  pimsmStarGNode_t *    pStarGNode;
  interface_bitset_t    oif;
  L7_BOOL               tmpOifFlag;
  L7_RC_t       rc;
  pimsmStarStarRpNode_t *   pStarStarRpNode = L7_NULLPTR;
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_NORMAL, "Enter " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;

  if(rtrIfNum != pSGEntry->pimsmSGRPFIfIndex)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      " rtrIfNum is not same as RPFIfIndex" );
    return;
  }

  if(pimsmSGJoinDesired( pimsmCb, pSGNode ) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      " pimsmSGJoinDesired() is false" );
    return;
  }

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE)
  {
    if (mcastIpMapIsDirectlyConnected( &pSGEntry->pimsmSGSrcAddress,
                                       L7_NULLPTR ) == L7_TRUE)
    {
      pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
                  " Is directly connected " );

      return;
    }
  }

  rpfIntfS = pSGEntry->pimsmSGRPFIfIndex;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "group :",
         &pSGEntry->pimsmSGGrpAddress);

  rc = pimsmRpAddressGet(pimsmCb, &pSGEntry->pimsmSGGrpAddress,
   &rpAddr);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                "pimsmRpAddressGet failed  ");
    return;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "rpAddr :",
         &rpAddr);

  rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                "pimsmStarStarRpFind() failed! ");
    return ;  
  }
  rpfIntfG = pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
  if(rpfIntfS != rpfIntfG)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      "RPF_interface(S) != RPF_interface(RP(G)) " );
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
    return;
  }

  rc = pimsmSGRptFind( pimsmCb,
                           &pSGEntry->pimsmSGGrpAddress,
                           &pSGEntry->pimsmSGSrcAddress,
                           &pSGRptNode );
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "(S,G,Rpt) not found " );
    return;
  }
  rc = pimsmSGRptInhrtdOlist( pimsmCb, pSGRptNode, &oif );
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO,
      "pimsmSGRptInhrtdOlist() failed" );
    return;
  }
  BITX_IS_EMPTY( &oif, tmpOifFlag );
  if(tmpOifFlag == L7_TRUE)
  {
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
    
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      "inherited_olist(S,G,rpt) == NULL" );      
    return;
  }

  pRPFSGAddr = &pSGEntry->pimsmSGUpstreamNeighbor;

  rc = pimsmStarGFind(pimsmCb, &pSGEntry->pimsmSGGrpAddress,
   &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                "pimsmStarGFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "pGrpAddr :",
      &pSGEntry->pimsmSGGrpAddress);
    return ;
  }
  
  pRPFStarGAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor;

  if(( PIMSM_INET_IS_ADDR_EQUAL( pRPFSGAddr,
                              pRPFStarGAddr )  == L7_TRUE) &&
     inetIsAddressZero(pRPFSGAddr) != L7_TRUE)
  {
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      "RPF'(S,G) == RPF'(*,G)" );     
    return;
  }

  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry != ( pimsmSGIEntry_t * )L7_NULLPTR &&
     (pSGIEntry->pimsmSGIAssertState ==
     PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER))
  {
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, 
      "I_Am_Assert_Loser(S,G,iif)" );       
    return;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_NORMAL, "Exit " );  
}

/***************************************************************
*
* @purpose  Gets the number of bytes received for the specified
*           source and group
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)}  Source address
* @param    pGrpAddr    @b{(input)}  Group address
* @param    rtrIfNum    @b{(input)} router interface number
* @param    *pByteCount @b{(output)} Byte Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
***************************************************************/
static L7_RC_t pimsmSourceGroupBytesGet ( pimsmCB_t * pimsmCb,
                                          L7_inet_addr_t * pSrcAddr,
                                          L7_inet_addr_t * pGrpAddr,
                                          L7_uint32 rtrIfNum,
                                          L7_uint32 * pByteCount )
{
   mfcEntry_t mfcEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter " );  
  memset(&mfcEntry,0, sizeof(mfcEntry_t));
  inetCopy(&mfcEntry.group, pGrpAddr);
  inetCopy(&mfcEntry.source, pSrcAddr);
  mfcEntry.iif = rtrIfNum;

  if (mfcIsEntryInUse(&mfcEntry)!= L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "mfcRtEntryGet() Failed" );  
    return L7_FAILURE;
  }
  /* Presently we dont have a way to get the (S,G) count from h/w
   if driver/mfc reports the entry has been used,
   report a non-zero packet count */
  *pByteCount += 1;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit " );  
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  SwitchToSptDesired(S,G) - implementation specific
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)} Source Address 
* @param    pGrpAddr    @b{(input)} Group Address    
* @param    rtrIfNum        @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* SwitchToSptDesired(S,G) is a policy function that is implementation
* defined. An "infinite threshold" policy can be implemented making
* SwitchToSptDesired(S,G) return false all the time.  A "switch on first
* packet" policy can be implemented by making SwitchToSptDesired(S,G)
* return true once a single packet has been received for the source and
* group     
* @end
******************************************************************************/
L7_BOOL pimsmSwitchToSptDesired ( pimsmCB_t * pimsmCb,
                                  L7_inet_addr_t * pSrcAddr,
                                  L7_inet_addr_t * pGrpAddr,
                                  L7_uint32       rtrIfNum )
{
  pimsmStarGNode_t *    pStarGNode;
  pimsmSGNode_t *    pSGNode;
  L7_uint32             curntByteCount, byteCount;
  pimsmCache_t *  kernelPtr;
  L7_RC_t       rc, rc2;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter " );      
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSrcAddr == ( L7_inet_addr_t * )L7_NULLPTR ) ||
     ( pGrpAddr == ( L7_inet_addr_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }

  rc = pimsmStarGFind( pimsmCb, pGrpAddr, &pStarGNode );
  if(rc == L7_SUCCESS)
  {
    rc2 = pimsmStarGCacheFind(pimsmCb, pStarGNode, pSrcAddr, pGrpAddr, &kernelPtr);
    if (rc2 == L7_SUCCESS)
    {
        curntByteCount = kernelPtr->pimsmSGByteCount;
        if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex ==
           PIMSM_REGISTER_INTF_NUM)
        {
           kernelPtr->pimsmSGByteCount = kernelPtr->pimsmSGRealtimeByteCount;
        }
        else 
        {
         byteCount = 0;
         if(pimsmSourceGroupBytesGet( pimsmCb,
                                      &kernelPtr->pimsmSrcAddr,
                                      &kernelPtr->pimsmGrpAddr,
                                      rtrIfNum,
                                      &byteCount ) != L7_SUCCESS)
         {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
                        "pimsmSourceGroupBytesGet failed " );
           return L7_FALSE;
         }
          kernelPtr->pimsmSGByteCount += byteCount;
        }
        if(curntByteCount == kernelPtr->pimsmSGByteCount)
        {
          /* Do not delete it here...It will anyways be removed in pimsmSwitchToShortestPath() */
          /* pimsmStarGCacheDelete( pimsmCb, pStarGNode, kernelPtr );
          memset(&oif, 0 , sizeof(interface_bitset_t));
          pimsmMfcQueue(pimsmCb,MFC_DELETE_ENTRY,
                     &kernelPtr->pimsmSrcAddr,
                     &kernelPtr->pimsmGrpAddr,
                     0,
                     &oif, L7_FALSE, 
                     &pStarGNode->flags); */
        }
        if(curntByteCount + pimsmCb->pimsmDataRateBytes <
           kernelPtr->pimsmSGByteCount)
        {
          if(pimsmIsLastHopRouter( pimsmCb, pStarGNode ) == L7_TRUE)
          {
            return L7_TRUE;
          }
        }
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "CurntByte Cnt: %d ",curntByteCount );                            
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "pimsmSGRealtimeByteCount : %d ",
            kernelPtr->pimsmSGRealtimeByteCount);                                          
        
        if(curntByteCount + pimsmCb->pimsmRegRateBytes <
           kernelPtr->pimsmSGByteCount)
        {
          if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex ==
             PIMSM_REGISTER_INTF_NUM)
          {
            return L7_TRUE;
          }
        }
    }
  }
  else
  {
     rc = pimsmSGFind( pimsmCb, pGrpAddr,  pSrcAddr, &pSGNode);
     if(rc == L7_SUCCESS)
     {
       /* if we found (S,G), we would like to switch immediately*/
       return L7_TRUE;
     }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit " );
  return L7_FALSE;
}

/******************************************************************************
* @purpose  CheckSwitchToSpt(S,G)
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)} Source Address 
* @param    pGrpAddr    @b{(input)} Group Address  
*
* @returns  void
*
* @comments      
*  void
*     CheckSwitchToSpt(S,G) {
*       if ( ( pim_include(*,G) (-) pim_exclude(S,G)
*              (+) pim_include(S,G) != NULL )
*            AND SwitchToSptDesired(S,G) ) {
*              # Note: Restarting the KAT will result in the SPT switch
*              restart KeepaliveTimer(S,G);
*       }
*     }     
* @end
******************************************************************************/
void pimsmCheckSwitchToSpt ( pimsmCB_t * pimsmCb,
                             L7_inet_addr_t * pSrcAddr,
                             L7_inet_addr_t * pGrpAddr )
{
  pimsmSGNode_t *       pSGNode;
  L7_uint32       rtrIfNum;
  pimsmStarGNode_t *    pStarGNode = L7_NULLPTR;
  pimsmSGEntry_t *      pSGEntry = L7_NULLPTR;
  pimsmSGRptNode_t *    pSGRptNode = L7_NULLPTR;
  interface_bitset_t    stargIncOif, sgExOif, sgIncOif, resultOif;
  L7_BOOL        resMaskFlag;
  L7_RC_t         rc2;
  L7_RC_t         rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSrcAddr == ( L7_inet_addr_t * )L7_NULLPTR ) ||
     ( pGrpAddr == ( L7_inet_addr_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return;
  }

  memset(&stargIncOif,0, sizeof(interface_bitset_t));
  rc2 = pimsmStarGFind( pimsmCb, pGrpAddr, &pStarGNode );
  if(rc2 != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "(*,G) not found " );
    return;
  }
  else
  {
    pimsmStarGInclude( pimsmCb, pStarGNode, &stargIncOif );
  }
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &stargIncOif );
  rtrIfNum = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;

  memset(&sgExOif,0, sizeof(interface_bitset_t));
  memset(&sgIncOif,0, sizeof(interface_bitset_t));

  rc2 = pimsmSGFind( pimsmCb, pGrpAddr, pSrcAddr, &pSGNode );
  if(rc2 != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "(S,G) Node not found " );
  }
  else
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    pimsmSGInclude( pimsmCb, pSGNode, &sgIncOif );
  }

  rc = pimsmSGRptFind (pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(rc == L7_SUCCESS)
  {
    pimsmSGExclude (pimsmCb, pSGRptNode, &sgExOif);
  }

  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgExOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &sgIncOif );

  BITX_MASK_AND( &stargIncOif, &sgExOif, &resultOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &resultOif );
  BITX_OR( &resultOif, &sgIncOif, &resultOif );
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, &resultOif );

  BITX_IS_EMPTY( &resultOif, resMaskFlag );

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "SourceAddr:", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "GroupAddr:", pGrpAddr);

  if(( resMaskFlag == L7_FALSE && 
       ( pimsmSwitchToSptDesired( pimsmCb,pSrcAddr,pGrpAddr,
                                  rtrIfNum) 
         == L7_TRUE ) ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "BIT mask is empty " );
    if(pSGEntry != (pimsmSGEntry_t *)L7_NULLPTR)
    {
      pimsmRestartKeepaliveTimer(pimsmCb, pSGNode, PIMSM_DEFAULT_KEEPALIVE_PERIOD);
    }
    pimsmSwitchToShortestPath(pimsmCb, pSrcAddr, pGrpAddr, pStarGNode);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit " );
}
/******************************************************************************
* @purpose  CouldRegister(S,G) 
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* bool CouldRegister(S,G) {
*     return ( I_am_DR( RPF_interface(S) ) AND
*              KeepaliveTimer(S,G) is running AND
*              DirectlyConnected(S) == TRUE )
*  }
*   
* @end
******************************************************************************/
L7_BOOL pimsmCouldRegister ( pimsmCB_t * pimsmCb,
                             pimsmSGNode_t * pSGNode )
{
  pimsmSGEntry_t *  pSGEntry;
  L7_uint32         timeLeft;

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;

  if(pimsmIAmDR( pimsmCb, pSGEntry->pimsmSGRPFIfIndex ) == L7_TRUE)
  {
    if(L7_SUCCESS == appTimerTimeLeftGet( pimsmCb->timerCb,
                                          pSGEntry->pimsmSGKeepaliveTimer,
                                          &timeLeft ) && timeLeft > 0)
    {
      if(mcastIpMapIsDirectlyConnected( &pSGEntry->pimsmSGSrcAddress,
                                        L7_NULLPTR ) == L7_TRUE)
      {
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}


/******************************************************************************
* @purpose  JoinDesired(*,*,RP) 
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    pStarStarRpNode @b{(input)}  (*,*,RP) node     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*   bool JoinDesired(*,*,RP) {
*     if immediate_olist(*,*,RP) != NULL
*         return L7_TRUE
*     else
*         return FALSE
*  }        
* @end
******************************************************************************/
L7_BOOL pimsmStarStarRpJoinDesired (pimsmCB_t * pimsmCb,
                                   pimsmStarStarRpNode_t * pStarStarRpNode )
{
  interface_bitset_t    oif;
  L7_BOOL               oifListFlag = L7_FALSE;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Entry " );
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarStarRpNode == ( pimsmStarStarRpNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  pimsmStarStarRpImdtOlist( pimsmCb, pStarStarRpNode, &oif );
  BITX_IS_EMPTY( &oif, oifListFlag );
  if(oifListFlag == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "interface bit mask is not empty " );
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "interface bit mask is  empty " );
  return L7_FALSE;
}

/******************************************************************************
* @purpose  JoinDesired(*,G)    
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)}  (*,G) node
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*  bool JoinDesired(*,G) {
*     if (immediate_olist(*,G) != NULL OR
*         (JoinDesired(*,*,RP(G)) AND
*          AssertWinner(*, G, RPF_interface(RP(G))) != NULL))
*         return L7_TRUE
*     else
*         return FALSE
*  }        
* @end
******************************************************************************/
L7_BOOL pimsmStarGJoinDesired ( pimsmCB_t * pimsmCb,
                                pimsmStarGNode_t * pStarGNode )
{
  pimsmStarGEntry_t *       pStarGEntry;
  pimsmStarGIEntry_t *      pStarGIEntry;
  pimsmStarStarRpNode_t *   pStarStarRpNode;
  L7_uint32                 rtrIfNum;
  interface_bitset_t        oif;
  L7_BOOL                   oifListFlag, starStarRpJoinDesired;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Enter" );   
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode == ( pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  memset( &oif, 0, sizeof( interface_bitset_t ) );
  pimsmStarGImdtOlist( pimsmCb, pStarGNode, &oif );
  BITX_IS_EMPTY( &oif, oifListFlag );
  if(oifListFlag == L7_FALSE)
  {
    return L7_TRUE;
  }

  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  starStarRpJoinDesired = L7_FALSE;
  rc = pimsmStarStarRpFind( pimsmCb, &pStarGEntry->pimsmStarGRPAddress,
                                &pStarStarRpNode );
  if(rc == L7_SUCCESS)
  {
    starStarRpJoinDesired = pimsmStarStarRpJoinDesired( pimsmCb,
                                                        pStarStarRpNode );
  }
  rtrIfNum = pStarGEntry->pimsmStarGRPFIfIndex;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FALSE;   
  }
  
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry != ( pimsmStarGIEntry_t * )L7_NULLPTR)
  {
    if((starStarRpJoinDesired == L7_TRUE) &&
       (inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress)
        == L7_FALSE ))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit:TRUE" );   
      return L7_TRUE;
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_DEBUG, "Exit" );    
  return L7_FALSE;
}

/******************************************************************************
* @purpose  AssertTrackingDesired(S,G,I)   
*
* @param    pimsmCb      @b{(input)}  control block  
* @param    pSGNode      @b{(input)}     (S,G) node
* @param    rtrIfNum     @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* AssertTrackingDesired(S,G,I) =
*     (I in ( ( joins(*,*,RP(G)) (+) joins(*,G) (-) prunes(S,G,rpt) )
*             (+) ( pim_include(*,G) (-) pim_exclude(S,G) )
*             (-) lost_assert(*,G)
*             (+) joins(S,G) ) )
*     OR (local_receiver_include(S,G,I) == L7_TRUE
*         AND (I_am_DR(I) OR (AssertWinner(S,G,I) == me)))
*     OR ((RPF_interface(S) == I) AND (JoinDesired(S,G) == L7_TRUE))
*     OR ((RPF_interface(RP(G)) == I) AND (JoinDesired(*,G) == L7_TRUE)
*         AND (SPTbit(S,G) == FALSE))   
* @end
******************************************************************************/
L7_BOOL pimsmSGIAssertTrackingDesired ( pimsmCB_t * pimsmCb,
                                        pimsmSGNode_t * pSGNode,
                                        L7_uint32 rtrIfNum )
{
  L7_BOOL                   bTemp = L7_FALSE;
  L7_RC_t         rc;                            
  pimsmSGEntry_t *          pSGEntry;
  pimsmSGIEntry_t *         pSGIEntry;
  L7_inet_addr_t *          pGrpAddr, * pSrcAddr, rpAddr; 
  pimsmStarStarRpNode_t *   pStarStarRpNode = L7_NULLPTR;
  pimsmStarGNode_t *        pStarGNode = L7_NULLPTR;
  pimsmSGRptNode_t *        pSGRptNode = L7_NULLPTR;
  interface_bitset_t       starStarRpJoinsOif,
  starGJoinsOif,
  sgRptPrunesOif,
  starGIncludesOif,
  sgExcludesOif,
  starGLostAssertsOif,
  sgJoinsOif,
  resultOif1,
  resultOif2;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter");
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSGNode == ( pimsmSGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FALSE;   
  }  
  pSGEntry = &pSGNode->pimsmSGEntry;
  pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  pGrpAddr = &pSGEntry->pimsmSGGrpAddress;

  BITX_RESET_ALL (&starStarRpJoinsOif);

  rc = pimsmRpAddressGet( pimsmCb, pGrpAddr, &rpAddr );
  if(rc == L7_SUCCESS)
  {
    rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
    if(rc == L7_SUCCESS)
    {
      pimsmStarStarRpJoins( pimsmCb, pStarStarRpNode, &starStarRpJoinsOif );     
    }
  }

  BITX_RESET_ALL (&starGJoinsOif);
  BITX_RESET_ALL (&starGIncludesOif);
  BITX_RESET_ALL (&starGLostAssertsOif);
  rc = pimsmStarGFind( pimsmCb, pGrpAddr, &pStarGNode );
  if(rc == L7_SUCCESS)
  {
    pimsmStarGJoins( pimsmCb, pStarGNode, &starGJoinsOif );
    pimsmStarGInclude( pimsmCb, pStarGNode, &starGIncludesOif );
    pimsmStarGLostAssert( pimsmCb, pStarGNode, &starGLostAssertsOif );
  }
  BITX_RESET_ALL (&sgRptPrunesOif);
  BITX_RESET_ALL (&sgExcludesOif);
  rc = pimsmSGRptFind( pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode );
  if(rc == L7_SUCCESS)
  {
    pimsmSGRptPrunes( pimsmCb, pSGRptNode, &sgRptPrunesOif );
    pimsmSGExclude (pimsmCb, pSGRptNode, &sgExcludesOif);
  }

  pimsmSGJoins( pimsmCb, pSGNode, &sgJoinsOif );

  /*joins(*,*,RP(G)) (+) joins(*,G)*/
  BITX_OR( &starStarRpJoinsOif, &starGJoinsOif, &resultOif1 );
  /* (-) prunes(S,G,rpt) */
  BITX_MASK_AND( &resultOif1, &sgRptPrunesOif, &resultOif1 );

  /* ( pim_include(*,G) (-) pim_exclude(S,G)*/
  BITX_MASK_AND( &starGIncludesOif, &sgExcludesOif, &resultOif2 );

  BITX_OR( &resultOif1, &resultOif2, &resultOif1 );
  /* (-) lost_assert(*,G) */
  BITX_MASK_AND( &resultOif1, &starGLostAssertsOif, &resultOif1 );

  /* (+) joins(S,G) */
  BITX_OR( &resultOif1, &sgJoinsOif, &resultOif1 );

  if(BITX_TEST( &resultOif1, rtrIfNum ) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-1 " );
    return L7_TRUE;
  }

  bTemp = pimsmSGILocalReceiverInclude(pimsmCb, pSGNode,rtrIfNum);
  if(bTemp == L7_TRUE)
  {
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry == ( pimsmSGIEntry_t * )L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning FALSE as pSGIEntry is NULL" );
      return L7_FALSE;
    }
    if((pimsmIAmDR( pimsmCb,rtrIfNum ) == L7_TRUE ) ||
        inetUnNumberedIsLocalAddress(&pSGIEntry->pimsmSGIAssertWinnerAddress,
                                rtrIfNum) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-2 " );
      return L7_TRUE;
    }
  }

  if(pSGEntry->pimsmSGRPFIfIndex == rtrIfNum  &&
     ( pimsmSGJoinDesired( pimsmCb, pSGNode ) == L7_TRUE))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-3" );
    return L7_TRUE;
  }

  if(pStarGNode != L7_NULLPTR)
  {
    if(( pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex == rtrIfNum ) &&
       ( pimsmStarGJoinDesired( pimsmCb,pStarGNode ) ==L7_TRUE ) &&
       ( pSGEntry->pimsmSGSPTBit == L7_FALSE ))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-4" );
      return L7_TRUE;
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit");
  return L7_FALSE;
}


/******************************************************************************
* @purpose  AssertTrackingDesired(*,G,I)    
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)}  (*,G) Node
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* AssertTrackingDesired(*,G,I) =
*     CouldAssert(*,G,I)
*     OR (local_receiver_include(*,G,I)==TRUE
*         AND (I_am_DR(I) OR AssertWinner(*,G,I) == me))
*     OR (RPF_interface(RP(G)) == I AND RPTJoinDesired(G))
*
* @end
******************************************************************************/
L7_BOOL pimsmStarGIAssertTrackingDesired ( pimsmCB_t * pimsmCb,
                                           pimsmStarGNode_t * pStarGNode,
                                           L7_uint32 rtrIfNum )
{
  pimsmStarGEntry_t *   pStarGEntry;
  pimsmStarGIEntry_t *  pStarGIEntry;
  L7_inet_addr_t *      pGrpAddr; 
  L7_BOOL                   bTemp = L7_FALSE;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter");
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode == ( pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FALSE;   
  }   
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  if(pimsmStarGICouldAssert( pimsmCb, pStarGNode, rtrIfNum ) 
     == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-1" );
    return L7_TRUE;
  }
  bTemp = pimsmStarGILocalReceiverInclude(pimsmCb, pStarGNode,rtrIfNum);
  if(bTemp == L7_TRUE)
  {
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry == ( pimsmStarGIEntry_t * )L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning FALSE as pStarGIEntry is NULL" );
      return L7_FALSE;
    }
    if(( pimsmIAmDR( pimsmCb, rtrIfNum ) == L7_TRUE ) ||
        inetUnNumberedIsLocalAddress(&pStarGIEntry->pimsmStarGIAssertWinnerAddress,
                                rtrIfNum) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-2" );
      return L7_TRUE;
    }
  }

  if(( pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex ==
       rtrIfNum ) &&
     ( pimsmRPTJoinDesired( pimsmCb, pGrpAddr ) == L7_TRUE ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "returning TRUE in case-3" );
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit");
  return L7_FALSE;
}

/******************************************************************************
* @purpose  CouldAssert(*,G,I)   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)}  (*,G) node
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
* CouldAssert(*,G,I) =
*    ( I in ( joins(*,*,RP(G)) (+) joins(*,G)
*             (+) pim_include(*,G))
*    AND (RPF_interface(RP(G)) != I)     

* @end
******************************************************************************/
L7_BOOL pimsmStarGICouldAssert ( pimsmCB_t * pimsmCb,
                                 pimsmStarGNode_t * pStarGNode,
                                 L7_uint32 rtrIfNum )
{
  pimsmStarGEntry_t *       pStarGEntry;
  pimsmStarStarRpNode_t *   pStarStarRpNode;
  interface_bitset_t        starStarRPJoinsOif, starGJoinsOif, 
  starGIncludesOif, resultOif;
  L7_RC_t         rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter");
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode == ( pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FALSE;
  }

  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  memset(&starStarRPJoinsOif,0,sizeof(interface_bitset_t));

  rc = pimsmStarStarRpFind( pimsmCb, 
                                &pStarGEntry->pimsmStarGRPAddress, 
                                &pStarStarRpNode );
  if(rc == L7_SUCCESS)
  {
    /* joins(*,*,RP(G) */
    pimsmStarStarRpJoins( pimsmCb, pStarStarRpNode, &starStarRPJoinsOif );
  }

  memset(&starGJoinsOif,0,sizeof(interface_bitset_t));
  memset(&starGIncludesOif,0,sizeof(interface_bitset_t));
  
  pimsmStarGJoins( pimsmCb, pStarGNode, &starGJoinsOif );
  pimsmStarGInclude( pimsmCb, pStarGNode, &starGIncludesOif );
  BITX_OR( &starStarRPJoinsOif, &starGJoinsOif, &resultOif );
  BITX_OR( &resultOif, &starGIncludesOif, &resultOif );
  
  if(BITX_TEST( &resultOif, rtrIfNum )  != L7_NULL &&
     (pStarGEntry->pimsmStarGRPFIfIndex != rtrIfNum))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "Bit is set for rtrIfNum = %d", rtrIfNum);
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit");
  return L7_FALSE;
}
/******************************************************************************
* @purpose  Check whether I'm the last Hop router  
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
*
* @returns  L7_FALSE
*
* @comments      
*       
* @end
******************************************************************************/
L7_BOOL pimsmIsLastHopRouter ( pimsmCB_t * pimsmCb,
                               pimsmStarGNode_t * pStarGNode )
{
  L7_uint32             rtrIfNum;
  pimsmStarGIEntry_t *  pStarGIEntry;

  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pStarGNode == ( pimsmStarGNode_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
    {
      continue;
    }

    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry != ( pimsmStarGIEntry_t * )L7_NULLPTR)
    {
      if(pStarGIEntry->pimsmStarGILocalMembership == L7_TRUE)
      {
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}

/******************************************************************************
* @purpose  my_assert_metric(S,G,I)    
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)} Source Address 
* @param    pGrpAddr    @b{(input)} Group Address 
* @param    rtrIfNum    @b{(input)} router interface number
            pMetric     @b{(output)} assert-metric structure
*
* @returns  L7_SUCCESS
*
* @comments      
*    assert_metric  my_assert_metric(S,G,I) 
    {
        if( CouldAssert(S,G,I) == TRUE ) 
        {          
            return spt_assert_metric(S,I)      
        } else if( CouldAssert(*,G,I) == TRUE ) 
        {          
            return rpt_assert_metric(G,I)      
        } else 
        {          
            return infinite_assert_metric()      
        }  
    }       
* @end
******************************************************************************/
L7_RC_t pimsmMyAssertMetric ( pimsmCB_t * pimsmCb,
                              L7_inet_addr_t *  pSrcAddr,
                              L7_inet_addr_t *  pGrpAddr,
                              L7_uint32 rtrIfNum,
                              pimsmAssertMetric_t * pMetric )
{
  pimsmSGNode_t * pSGNode= L7_NULLPTR;
  pimsmStarGNode_t * pStarGNode= L7_NULLPTR;
  pimsmSGEntry_t * pSGEntry= L7_NULLPTR;
  pimsmStarGEntry_t * pStarGEntry= L7_NULLPTR;
  pimsmInterfaceEntry_t * pIntfEntry; 
  
  if(( pimsmCb == ( pimsmCB_t * )L7_NULLPTR ) ||
     ( pSrcAddr== L7_NULLPTR ) ||
     ( pGrpAddr == L7_NULLPTR ) ||
     ( pMetric == ( pimsmAssertMetric_t * )L7_NULLPTR ))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR, "Input is NULLPTR " );
    return L7_FAILURE;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "pSrcAddr = ", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
               "pGrpAddr = ", pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "rtrIfNum = %d", rtrIfNum);

  memset(pMetric,0 , sizeof(pimsmAssertMetric_t));
  if(pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) == L7_SUCCESS &&
     pimsmSGICouldAssert( pimsmCb, pSGNode, rtrIfNum ) == L7_TRUE)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    pMetric->rpt_bit_flag = L7_FALSE;
    pMetric->metric_preference = pSGEntry->pimsmSGRPFRouteMetricPref;
    pMetric->route_metric = pSGEntry->pimsmSGRPFRouteMetric;
    if(pimsmIntfEntryGet(pimsmCb, pSGEntry->pimsmSGRPFIfIndex, &pIntfEntry) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
        "IntfEntry is not available for rtrIfNum(%d)", pSGEntry->pimsmSGRPFIfIndex);
      return L7_FAILURE;
    }  
    inetCopy(&pMetric->ip_address, &pIntfEntry->pimsmInterfaceAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "spt_assert_metric");

  }
  else if(pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) == L7_SUCCESS &&
   pimsmStarGICouldAssert( pimsmCb,pStarGNode, rtrIfNum ) == L7_TRUE)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    pMetric->rpt_bit_flag = L7_TRUE;
    pMetric->metric_preference = pStarGEntry->pimsmStarGRPFRouteMetricPref;
    pMetric->route_metric = pStarGEntry->pimsmStarGRPFRouteMetric;
    if(pimsmIntfEntryGet(pimsmCb, pStarGEntry->pimsmStarGRPFIfIndex, &pIntfEntry) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_ERROR,
        "IntfEntry is not available for rtrIfNum(%d)", 
        pStarGEntry->pimsmStarGRPFIfIndex);
      return L7_FAILURE;
    } 
    inetCopy(&pMetric->ip_address, &pIntfEntry->pimsmInterfaceAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "rpt_assert_metric");
  }
  else
  {
    pMetric->rpt_bit_flag = L7_TRUE;
    pMetric->metric_preference = PIMSM_DEFAULT_ASSERT_PREFERENCE;
    pMetric->route_metric = PIMSM_DEFAULT_ASSERT_METRIC;
    inetAddressZeroSet(pimsmCb ->family,  &pMetric->ip_address );
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                "infinite_assert_metric");
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose   Switch to shortest Path   
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSrcAddr    @b{(input)} Source Address 
* @param    pGrpAddr    @b{(input)} Group Address 
            pStarGNode  @b{(input)} (*,G) node
*
* @returns  L7_SUCCESS
*
* @comments      
*      
* @end
******************************************************************************/
L7_RC_t pimsmSwitchToShortestPath(pimsmCB_t * pimsmCb, 
                                  L7_inet_addr_t * pSrcAddr, 
                                  L7_inet_addr_t * pGrpAddr, 
                                  pimsmStarGNode_t *pStarGNode)
{
  pimsmSGNode_t * pSGNode;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  pimsmCache_t  * kernelelCache;
  L7_RC_t rc;  
  pimsmPerSGRegisterEventInfo_t registerEventInfo;
  pimsmSendJoinPruneData_t jpData;  
  L7_uchar8 maskLen =0;
  interface_bitset_t oif;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Enter \n" );

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, "SourceAddr:", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_DEBUG, "GroupAddr:", pGrpAddr);    

    rc = pimsmSGNodeCreate(pimsmCb, pSrcAddr, pGrpAddr, &pSGNode,L7_TRUE);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                  "pimsmSGNodeCreate failed");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);       
      return L7_FAILURE;
    }
    
    if(pimsmCouldRegister(pimsmCb, pSGNode)== L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
                   "Error: pimsmCouldRegister() is true");
    }
    else 
    {
      registerEventInfo.eventType = 
         PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE;
      pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &registerEventInfo);
    }
    if((pSGNode->flags & PIMSM_NEW) !=L7_NULL )
    {
      if ( pStarGNode != L7_NULLPTR)
      {
        rc = pimsmStarGCacheFind(pimsmCb, pStarGNode, pSrcAddr, pGrpAddr, 
         &kernelelCache);
        if(rc == L7_SUCCESS)
        {
           rc = pimsmStarGCacheDelete( pimsmCb, pStarGNode, kernelelCache );
           if(rc != L7_SUCCESS)
           {
             PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                        "pimsmStarGCacheDelete() failed");
           }
           memset(&oif, 0 , sizeof(interface_bitset_t));
           pimsmMfcQueue (pimsmCb, MFC_DELETE_ENTRY, &kernelelCache->pimsmSrcAddr,
                          &kernelelCache->pimsmGrpAddr,
                          pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex,
                          &oif, L7_FALSE, &pStarGNode->flags);
        }
        else 
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,
                     "pimsmStarGCacheFind() failed");
        }
       if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_TRUE) != L7_SUCCESS)
       {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,"MFC Updation failed");
           return L7_FAILURE;
       }  
       pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
        if (pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex != 0 &&
            pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex !=
            pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex)
        {
          /*Trigger a Prune towards the RP */
          memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
          jpData.rtrIfNum = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;
          if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum, 
                                                &jpData.holdtime)
                                             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, 
              PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
            return L7_FAILURE;
          }                                                
          jpData.addrFlags = PIMSM_ADDR_RP_BIT ;
          pimSrcMaskLenGet(pimsmCb->family,&maskLen);
          jpData.srcAddrMaskLen = maskLen;
          pimGrpMaskLenGet(pimsmCb->family,&maskLen);
          jpData.grpAddrMaskLen = maskLen;
          jpData.joinOrPruneFlag = L7_FALSE;
          jpData.pNbrAddr =&pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor;
          jpData.pSrcAddr = pSrcAddr;
          jpData.pGrpAddr = pGrpAddr;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"Sending SGRpt Join Prune");
          if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
                         "Error sending Join/Prune Message");
          }
        }
      }
      memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
      joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);

     /*Send Join to upstream only if Join Desired is True and Upstream Interface
       is not present in outgoing interface list.This case is possible when
       data and joins for the same group are sent from a non-RP router.*/
      if (joinDesired == L7_TRUE) 
      {
        upStrmSGEventInfo.eventType 
        = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
        pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
      }
      else
      {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR,
                   "Error: pimsmSGJoinDesired() is false");
      }
      pSGNode->flags &= ~PIMSM_NEW;
    } 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS,  PIMSM_TRACE_INFO, "Exit " );
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose  Compare assert metrics 
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    localPref       @b{(input)}  local preference
*           localMetric       @b{(input)}  local metric
*           pLocalAddr       @b{(input)}  local address
*           remotePref       @b{(input)}  remote preference
*           remoteMetric    @b{(input)}  remote metric
*           pRemoteAddr       @b{(input)}  remote address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments     none
     Return L7_TRUE if the local win, otherwise L7_FALSE 
*
* @end
*
******************************************************************************/
L7_BOOL pimsmCompareMetrics(L7_uint32 localPref, L7_uint32 localMetric,
   L7_inet_addr_t * pLocalAddr, L7_uint32 remotePref,
   L7_uint32 remoteMetric, L7_inet_addr_t * pRemoteAddr)
{
  /* Now lets see who has a smaller gun (aka "asserts war") */
  /* FYI, the smaller gun...err metric wins, but if the same
   * caliber, then the bigger network address wins. The order of
   * threatment is: preference, metric, address.
   */
  /* The RPT bits are already included as the most significant bits
   * of the preferences.
   */

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "localPref = 0x%x", localPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "localMetric = 0x%x", localMetric);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "pLocalAddr :",pLocalAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "remotePref = 0x%x", remotePref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "remoteMetric = 0x%x", remoteMetric);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO, "pRemoteAddr :",pRemoteAddr);

  if(remotePref > localPref) 
  {
    return L7_TRUE;
  }
  if(remotePref < localPref)
  {
    return L7_FALSE;
  }
  if(remoteMetric > localMetric)
  {
    return L7_TRUE;
  }
  if(remoteMetric < localMetric)
  {
    return L7_FALSE;
  }
  if(L7_INET_ADDR_COMPARE(pLocalAddr, pRemoteAddr) > 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}



/*********************************************************************
* @purpose  Start or Restart (S,G) keepalive timer.
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pSGNode     @b{(input)} (S,G) node
* @param    timeout     @b{(input)} Time interval for the Keepalive
*                                   timer.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmRestartKeepaliveTimer(pimsmCB_t *pimsmCb,
                                pimsmSGNode_t *pSGNode,
                                L7_uint32 timeout)
{
  pimsmTimerData_t *pTimerData = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_INFO,"Update the keepalive Timer");
  pTimerData = 
       &pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->pimsmCb = pimsmCb;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
    
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGKeepaliveTimerExpiresHandler,
                            (void*)pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerHandle,
                            timeout,
                            &(pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimer),
                            "SM-KAT")
                         != L7_SUCCESS)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MACROS, PIMSM_TRACE_ERROR, 
                 "pimsmSGKeepaliveTimer Add Failed");
     return ;
  }
}

