/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGTree.c
*
* @purpose Contains PIM-SM  avltree function definitions for (S,G) entry
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
#include "pimsmdefs.h"
#include "pimsmsgtree.h"
#include "avl_api.h"
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmmacros.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"

/*********************************************************************
* @purpose  Compares the given (S,G) nodes
*
* @param pimsmSGNode1  @b{(input)} (S,G) Node
* @param pimsmSGNode2  @b{(input)} (S,G) Node
* @param keySize       @b{(input)} key length
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2.
*
* @comments      
*       
* @end
*********************************************************************/
static L7_int32 pimsmSGCompare(const void * pimsmSGNode1, 
                               const void * pimsmSGNode2,
                               L7_uint32 keySize)
{
  pimsmSGNode_t * pSGNode1, *pSGNode2;
  L7_inet_addr_t  *pSrcAddr1, * pSrcAddr2;
  L7_inet_addr_t  *pGrpAddr1, * pGrpAddr2;
  L7_uint32   result;

  pSGNode1 = (pimsmSGNode_t *)pimsmSGNode1;
  pSGNode2 = (pimsmSGNode_t *)pimsmSGNode2;

  pSrcAddr1 = &pSGNode1->pimsmSGEntry.pimsmSGSrcAddress;
  pSrcAddr2 = &pSGNode2->pimsmSGEntry.pimsmSGSrcAddress;
  pGrpAddr1 = &pSGNode1->pimsmSGEntry.pimsmSGGrpAddress;
  pGrpAddr2 = &pSGNode2->pimsmSGEntry.pimsmSGGrpAddress;

  result = L7_INET_ADDR_COMPARE(pGrpAddr1, pGrpAddr2);
  if(result == 0)
  {
    return L7_INET_ADDR_COMPARE(pSrcAddr1, pSrcAddr2);
  }
  return result;
}
/*********************************************************************
* @purpose  Initializes the (S,G) entry table
*
* @param    pimsmCb @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmSGInit(pimsmCB_t * pimsmCb)
{
  L7_uint32 mrtSGTableSize = 0;

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGTableSize = PIMSM_S_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGTableSize = PIMSM_S_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  pimsmCb->pimsmSGTreeHeap = PIMSM_ALLOC (pimsmCb->family,
                                          (mrtSGTableSize * 
                                          sizeof (avlTreeTables_t)));

  if(pimsmCb->pimsmSGTreeHeap == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (S,G) Table Tree Heap\n");
    return L7_FAILURE;
  }

  pimsmCb->pimsmSGDataHeap =  PIMSM_ALLOC (pimsmCb->family, 
                                           (mrtSGTableSize * sizeof 
                                           (pimsmSGNode_t)));
  if(pimsmCb->pimsmSGDataHeap == L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGTreeHeap);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (S,G) Table Data Heap\n");
    return L7_FAILURE;
  }

  /* Cleanup data structures before proceeding */
  memset(pimsmCb->pimsmSGTreeHeap, 0, mrtSGTableSize * sizeof 
         (avlTreeTables_t));
  memset(pimsmCb->pimsmSGDataHeap, 0, mrtSGTableSize * sizeof 
         (pimsmSGNode_t));

  /* The destination networks are stored in an AVL tree.
  */
  avlCreateAvlTree(&pimsmCb->pimsmSGTree, pimsmCb->pimsmSGTreeHeap,
                   pimsmCb->pimsmSGDataHeap, mrtSGTableSize,
                   (L7_uint32)(sizeof(pimsmSGNode_t)), PIMSM_AVL_TREE_TYPE, 
                   sizeof(L7_inet_addr_t) * 2);

  avlSetAvlTreeComparator(&pimsmCb->pimsmSGTree, pimsmSGCompare); 

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Frees the (S,G) entry table
*
* @param    pimsmCb @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmSGDeInit(pimsmCB_t * pimsmCb)
{
  if(pimsmCb->pimsmSGTreeHeap != (avlTreeTables_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGTreeHeap);
  }
  pimsmCb->pimsmSGTreeHeap = L7_NULLPTR;

  if(pimsmCb->pimsmSGDataHeap != (pimsmSGNode_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGDataHeap);
  }
  pimsmCb->pimsmSGDataHeap = L7_NULLPTR;

  /* The destination networks are stored in an AVL tree.
  */
  if(avlDeleteAvlTree(&pimsmCb->pimsmSGTree) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "Failed to delete (S,G) Tree.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a mrt entry to the (S,G) entry table
*
* @param    pimsmCb  @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGAdd(pimsmCB_t * pimsmCb, pimsmSGNode_t *pSGNode)
{
  pimsmSGNode_t *pSGNodeTemp = L7_NULLPTR;
  L7_uint32 mrtSGTableSize = 0;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGTableSize = PIMSM_S_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGTableSize = PIMSM_S_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
      "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  if(avlTreeCount(&pimsmCb->pimsmSGTree) >= mrtSGTableSize)
  {
    osapiSemaGive(pimsmCb->pimsmSGTree.semId) ;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, "PIMSM (S,G) Table Max Limit - [%d] Reached; "
			 "Cannot accomodate any further routes. PIMSM Multicast Route table (S,G) has reached maximum capacity"
			 " and cannnot accommodate new registrations anymore.",
             mrtSGTableSize);
    return L7_FAILURE;
  }

  pSGNodeTemp = avlInsertEntry(&pimsmCb->pimsmSGTree, pSGNode);
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
      "Failed to give semaphore "); 
    return L7_FAILURE;
  }
  if(pSGNodeTemp != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "Duplicate Item or Error in inserting ,Address is %p",
                pSGNodeTemp);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pGgrpAddr  @b{(input)} Group IP Addr    
* @param pSrcAddr   @b{(input)} Source IP Addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGDelete(pimsmCB_t * pimsmCb,
                      L7_inet_addr_t *pGrpAddr,
                      L7_inet_addr_t *pSrcAddr)
{
  pimsmSGNode_t sGNodeSearch, *pSGNode = L7_NULLPTR,*pSGNodeDelete = L7_NULLPTR;
  L7_uint32       index;
  L7_RC_t       retVal;
  pimsmSGRptNode_t *    sgRptNode;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr)
    || (L7_NULLPTR == pSrcAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
   PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
                "Entry success");

  memset(&sGNodeSearch,0,sizeof(pimsmSGNode_t));
  inetCopy(&sGNodeSearch.pimsmSGEntry.pimsmSGGrpAddress, pGrpAddr);
  inetCopy(&sGNodeSearch.pimsmSGEntry.pimsmSGSrcAddress, pSrcAddr);

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
      "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pSGNodeDelete = avlSearchLVL7(&pimsmCb->pimsmSGTree, &sGNodeSearch, AVL_EXACT);

  if(pSGNodeDelete == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "Failed  to delete from (S,G)node");
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
        "Failed to give semaphore"); 
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  retVal = pimsmSGRptFind( pimsmCb,
                           &pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress,
                           &pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress,
                           &sgRptNode );
         
  if(retVal == L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_INFO, "(S,G,Rpt) found" );
    if(((sgRptNode->flags & PIMSM_ADDED_TO_MFC) == L7_NULL) &&  
      sgRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamPruneState == 
      sgRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamPruneInitState &&
      sgRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamOverrideTimer == 
      L7_NULLPTR)        
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_INFO, "Deleting (S,G,Rpt) node" );
      pimsmSGRptDelete(pimsmCb,
                           &pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress,
                           &pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress);
    }
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pSGNodeDelete->pimsmSGEntry.pimsmSGKeepaliveTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGNodeDelete->pimsmSGEntry.pimsmSGDRRegisterStopTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGNodeDelete->pimsmSGEntry.pimsmSGUpstreamJoinTimer));

  memset(&pSGNodeDelete->pimsmSGEntry.pimsmSGKeepaliveTimerParam, 0, 
         sizeof(pimsmTimerData_t));
  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
    if(pSGNodeDelete->pimsmSGIEntry[index] != L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG,
                  "Deleting (S,G,%d) entry",index); 
      pimsmSGIEntryDelete(pimsmCb,pSGNodeDelete, index);
    }
  }

  handleListNodeDelete(pimsmCb->handleList,
                       &pSGNodeDelete->pimsmSGEntry.pimsmSGDRRegisterStopTimerHandle);
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGNodeDelete->pimsmSGEntry.pimsmSGKeepaliveTimerHandle);
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGNodeDelete->pimsmSGEntry.pimsmSGUpstreamJoinTimerHandle);

  pimsmSGMFCUpdate(pimsmCb, pSGNodeDelete, MFC_DELETE_ENTRY, L7_FALSE);
  pSGNodeDelete->pimsmSGEntry.pimsmSGSPTBit = L7_FALSE;  
  pSGNode = avlDeleteEntry(&pimsmCb->pimsmSGTree, pSGNodeDelete);
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "Failed to take semaphore "); 
    return L7_FAILURE;
  }
  if(pSGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G) node does not exist");
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify a mrt entry in the (S,G) entry table
*
* @param    pimsmCb  @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGModify(pimsmCB_t * pimsmCb, pimsmSGNode_t *pSGNode)
{
  MCAST_UNUSED_PARAM(pSGNode);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Check whether the mrt entry exists in the (S,G) entry table
*
* @param    pimsmCb  @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGFirstGet(pimsmCB_t * pimsmCb, pimsmSGNode_t **ppSGNode)
{
  pimsmSGNode_t  sGNodeSearch;

  /* Set all elements of the node to zero */
  memset (&sGNodeSearch, 0, sizeof (pimsmSGNode_t));
  inetAddressZeroSet(pimsmCb->family,
        &sGNodeSearch.pimsmSGEntry.pimsmSGGrpAddress);
  inetAddressZeroSet(pimsmCb->family,
        &sGNodeSearch.pimsmSGEntry.pimsmSGSrcAddress);  
    return pimsmSGEntryFind(pimsmCb, &sGNodeSearch, AVL_NEXT, ppSGNode);

}


/*********************************************************************
* @purpose  Get the next (S,G) entry after the specified (S,G) entry 
*           in the (S,G) entry table
*
* @param    pimsmCb         @b{(input)} PIM SM Control Block
* @param   pSGNodeCurrent  @b{(input)} Current (S,G) Node    
* @param   ppSGNode        @b{(output)} Pointer to next (S,G) Node 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGNextGet(pimsmCB_t      *pimsmCb,
                       pimsmSGNode_t  *pSGNodeCurrent,
                       pimsmSGNode_t **ppSGNode)
{
  return pimsmSGEntryFind(pimsmCb, pSGNodeCurrent, AVL_NEXT, ppSGNode);
}


/*********************************************************************
* @purpose  Search the (S,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pGgrpAddr   @b{(input)} Group IP Addr    
* @param   pSrcAddr    @b{(input)} Source IP Addr   
* @param   ppSGNode    @b{(output)} Pointer to (S,G) Node   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGFind(pimsmCB_t * pimsmCb,
                    L7_inet_addr_t *pGrpAddr,
               L7_inet_addr_t *pSrcAddr, 
               pimsmSGNode_t **ppSGNode)
{
    pimsmSGNode_t sgNodeSearch;
    L7_RC_t rc;
  
    memset (&sgNodeSearch, 0, sizeof (pimsmSGNode_t));
    inetCopy(&sgNodeSearch.pimsmSGEntry.pimsmSGGrpAddress,  pGrpAddr);
    inetCopy(&sgNodeSearch.pimsmSGEntry.pimsmSGSrcAddress,  pSrcAddr);
    rc = pimsmSGEntryFind(pimsmCb, &sgNodeSearch, AVL_EXACT, ppSGNode);

    return rc;
}

/*********************************************************************
* @purpose  Search the (S,G) entry table
*
* @param    pimsmCb         @b{(input)} PIM SM Control Block
* @param pSGNodeCurrent  @b{(input)} Current (S,G) Node    
* @param ppSGNode        @b{(input)} Pointer to next (S,G) Node 
* @param flag            @b{(input)} AVL_EXACT or AVL_NEXT 
* @param   ppSGNode        @b{(ouput)} Pointer to next (S,G) Node 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGFindExact(pimsmCB_t * pimsmCb,
                    L7_inet_addr_t *pGrpAddr,
               L7_inet_addr_t *pSrcAddr, 
               pimsmSGNode_t **ppSGNode)
{
    pimsmSGNode_t *pSGNode;
    pimsmSGNode_t sgNodeSearch;
  
    memset (&sgNodeSearch, 0, sizeof (pimsmSGNode_t));
    inetCopy(&sgNodeSearch.pimsmSGEntry.pimsmSGGrpAddress,  pGrpAddr);
    inetCopy(&sgNodeSearch.pimsmSGEntry.pimsmSGSrcAddress,  pSrcAddr);

   pSGNode = avlSearchLVL7(&pimsmCb->pimsmSGTree, &sgNodeSearch, AVL_EXACT);

  if(pSGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG,
        "(S,G) node does not exist");
    *ppSGNode = L7_NULLPTR;   
    return L7_FAILURE;
  }
  *ppSGNode = pSGNode;
 
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Search the (S,G) entry table
*
* @param    pimsmCb         @b{(input)} PIM SM Control Block
* @param pSGNodeCurrent  @b{(input)} Current (S,G) Node    
* @param ppSGNode        @b{(input)} Pointer to next (S,G) Node 
* @param flag            @b{(input)} AVL_EXACT or AVL_NEXT 
* @param   ppSGNode        @b{(ouput)} Pointer to next (S,G) Node 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGEntryFind(pimsmCB_t       *pimsmCb, 
                         pimsmSGNode_t   *pSGNodeCurrent,
                         L7_uint32        flag,
                       pimsmSGNode_t  **ppSGNode)
                       
{
  pimsmSGNode_t *pSGNode;

  if ((pSGNodeCurrent == L7_NULLPTR) || (ppSGNode == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
             "curnt or next route entry passed NULL PTR!");

    return L7_FAILURE;
  }
  *ppSGNode = L7_NULLPTR;
  pSGNode = avlSearchLVL7(&pimsmCb->pimsmSGTree, pSGNodeCurrent, flag);

  if(pSGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG,"(S,G) node does not exist");
    *ppSGNode = L7_NULLPTR;   
    return L7_FAILURE;
  }
  if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    *ppSGNode = L7_NULLPTR;   
    return L7_FAILURE;
  } 
  *ppSGNode = pSGNode;
 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the (S,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pSrcAddr    @b{(input)} Source IP Addr    
* @param   pGrpAddr    @b{(input)} Group IP Addr    
* @param   ppSGNode    @b{(output)} Pointer to (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGNodeCreate(pimsmCB_t * pimsmCb,
                          L7_inet_addr_t * pSrcAddr, 
                          L7_inet_addr_t * pGrpAddr, 
                          pimsmSGNode_t **ppSGNode,
                          L7_BOOL bStartKAT)
{
  mcastRPFInfo_t  rpfRouteInfo;
  pimsmSGEntry_t  *pSGEntry = L7_NULLPTR;
  L7_inet_addr_t   upStrmAddr;
  pimsmTimerData_t *pTimerData = L7_NULLPTR;     
  pimsmSGNode_t   * pSGNode, sgNode;
  L7_RC_t  rc;
  pimsmSGRptNode_t   *pSGRptNode= L7_NULLPTR;
  interface_bitset_t  sgInhrtdOig;
  L7_BOOL             result = L7_FALSE;
  L7_uint32           ii =0;

 
  if (pimsmCb == L7_NULLPTR || pSrcAddr == L7_NULLPTR ||
      pGrpAddr == L7_NULLPTR || ppSGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "invalid input parameters");
    return L7_FAILURE;
  }
  inetAddressZeroSet(pimsmCb ->family, &upStrmAddr);   

  rc = pimsmSGFindExact(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    memset(&sgNode, 0, sizeof (pimsmSGNode_t));
    pSGEntry = &sgNode.pimsmSGEntry;
    sgNode.flags |= PIMSM_NEW;
    inetCopy(&pSGEntry->pimsmSGSrcAddress, pSrcAddr);
    inetCopy(&pSGEntry->pimsmSGGrpAddress, pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Source Address is ",pSrcAddr);

    if (pimsmMapIsInSsmRange (pimsmCb->family, pGrpAddr) == L7_TRUE)
    {
      pSGEntry->pimsmSGPimMode = PIM_MODE_SSM;
    }
    else
    {
      pSGEntry->pimsmSGPimMode = PIM_MODE_ASM;
    }

    /* Clear the SPT flag */
    pSGEntry->pimsmSGSPTBit = L7_FALSE;
    memset(&rpfRouteInfo,0,sizeof(rpfRouteInfo));
    if(pimsmRPFInfoGet(pimsmCb, pSrcAddr, &rpfRouteInfo) == L7_SUCCESS)
    {
      /* Check if PIMSM is enabled on RPF Interface */
      if (pimsmInterfaceEntryGet (pimsmCb, rpfRouteInfo.rpfIfIndex) != L7_SUCCESS)
      {
        PIMSM_TRACE (PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
                     "PIMSM is not Enabled on RPF Interface - %d",
                     rpfRouteInfo.rpfIfIndex);
        return L7_FAILURE;
      }

      pSGEntry->pimsmSGRPFIfIndex 
      = rpfRouteInfo.rpfIfIndex;
      inetCopy(&pSGEntry->pimsmSGRPFNextHop, 
               &rpfRouteInfo.rpfNextHop);
      inetCopy(&pSGEntry->pimsmSGRPFRouteAddress, 
               &rpfRouteInfo.rpfRouteAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"RPF Route Adress is ",
                                        &pSGEntry->pimsmSGRPFRouteAddress);
      pSGEntry->pimsmSGRPFRouteMetricPref 
      = rpfRouteInfo.rpfRouteMetricPref;
      pSGEntry->pimsmSGRPFRouteMetric 
      = rpfRouteInfo.rpfRouteMetric;
      pSGEntry->pimsmSGRPFRouteProtocol 
      =  rpfRouteInfo.rpfRouteProtocol;
      pSGEntry->pimsmSGRPFRoutePrefixLength = rpfRouteInfo.prefixLength;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Prefix Length is %d",
                                   pSGEntry->pimsmSGRPFRoutePrefixLength);
    }
    else
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
            "Failed to get RTO route to Source", pSrcAddr);
      return L7_FAILURE;
    }

    pSGEntry->pimsmSGDRRegisterState 
    = PIMSM_REG_PER_S_G_SM_STATE_NO_INFO;

    pSGEntry->pimsmSGCreateTime =  osapiUpTimeRaw();
    pSGEntry->pimsmSGUpstreamJoinState = PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED;
    if(pimsmSGAdd(pimsmCb, &sgNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"pimsmSGAdd() failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_INFO, "Group Address :",
                                          &pSGEntry->pimsmSGGrpAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_INFO, "Source Address :", 
                                          &pSGEntry->pimsmSGSrcAddress);
      *ppSGNode = L7_NULLPTR;
      return L7_FAILURE;                  
    }
    rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
    if(rc  == L7_SUCCESS)
    {
      /*pSGNode->pSelf = pSGNode;*/
      pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinTimerHandle = 
      handleListNodeStore(pimsmCb->handleList,
                   (void*)&pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinTimerParam); 
      pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerHandle = 
      handleListNodeStore(pimsmCb->handleList,
                   (void*)&pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerParam); 
      pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerHandle = 
      handleListNodeStore(pimsmCb->handleList,
                   (void*)&pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerParam); 

      pTimerData = &pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerParam;

      memset(pTimerData, 0 , sizeof(pimsmTimerData_t));

      pTimerData->pimsmCb = (pimsmCB_t *)pimsmCb;
      inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
      inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
     /* As per the draft, keepAliveTimer should be started only when
      then data-traffic is startedi, not when (S,G) entry is created */
      if(bStartKAT == L7_TRUE)
      {
        if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGKeepaliveTimerExpiresHandler,
                                  (void*) pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerHandle,
                                  PIMSM_DEFAULT_KEEPALIVE_PERIOD,
                                  &(pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimer),
                                  "SM-KAT2")
                               != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                      "Failed to add KeeplAlive Timer");
          pimsmSGDelete(pimsmCb,pGrpAddr,pSrcAddr);
          return L7_FAILURE;
        }
      }

      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Upstream index is %d",
                  pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex); 

      if(pimsmSGIEntryCreate(pimsmCb,pSGNode,
                             pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                    "Failed to create (S,G,I) for upstream index %d",
                    pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex);
        pimsmSGDelete(pimsmCb,pGrpAddr,pSrcAddr);
        return L7_FAILURE;
      }
      pimsmSGNeighborRpfDash(pimsmCb,pSGNode);
      
      memset(&sgInhrtdOig,0,sizeof(interface_bitset_t));
      /*TODO:  SPTBit is always false, so create (S,G,rpt) Node  */
      if (pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr) != L7_TRUE)
      {
        if(pimsmSGRptNodeCreate(pimsmCb,pSrcAddr,pGrpAddr,&pSGRptNode) 
           != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to create (S,G,Rpt)node");
          pimsmSGDelete(pimsmCb,pGrpAddr,pSrcAddr);
          *ppSGNode = L7_NULLPTR;
          return L7_FAILURE;
        }
      }
      pimsmSGInhrtdOlist(pimsmCb,pSGNode,&sgInhrtdOig);

      BITX_IS_EMPTY(&sgInhrtdOig,result);
      if(result != L7_TRUE)
      {
        for(ii =1;ii < MCAST_MAX_INTERFACES;ii++)
        {
          if(BITX_TEST(&sgInhrtdOig,ii) != L7_NULL)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, 
                        "Bit set for interface index %d",ii);

            if(pimsmSGIEntryCreate(pimsmCb,pSGNode,ii) 
               != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                          "Failed to create (S,G,%d) entry",ii);
            }
          }
        }/*end of for loop */
      }
    }
  }
  pSGNode->flags &=  ~PIMSM_NODE_DELETE;
  *ppSGNode = pSGNode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create (S,G,I) Entry and init with default params
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSGNode     @b{(input)} (S,G) Node
* @param rtrIfNum    @b{(input)} router Interface Number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGIEntryCreate(pimsmCB_t     *pimsmCb,
                            pimsmSGNode_t *pSGNode, 
                            L7_uint32      rtrIfNum)
{
  pimsmSGIEntry_t * pSGIEntryNew;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"rtrIfNum = %d ",rtrIfNum);

  if(pSGNode->pimsmSGIEntry[rtrIfNum] == (pimsmSGIEntry_t *)L7_NULLPTR)
  {
    if ((pSGIEntryNew = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmSGIEntry_t)))
                                  == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (S,G) interface pool");    
      return L7_FAILURE;
    }       
    memset(pSGIEntryNew, 0, sizeof(pimsmSGIEntry_t));
    inetAddressZeroSet(pimsmCb ->family, &pSGIEntryNew->pimsmSGIAssertWinnerAddress);
    pSGIEntryNew->pimsmSGIIfIndex = rtrIfNum;
    pSGIEntryNew->pimsmSGIPrunePendingTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGIEntryNew->pimsmSGIPrunePendingTimerParam);       
    pSGIEntryNew->pimsmSGIJoinExpiryTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGIEntryNew->pimsmSGIJoinExpiryTimerParam);       
    pSGIEntryNew->pimsmSGIAssertTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGIEntryNew->pimsmSGIAssertTimerParam);       

    pSGIEntryNew->pimsmSGILocalMembership = L7_FALSE;
    pSGIEntryNew->pimsmSGIAssertState = PIMSM_ASSERT_S_G_SM_STATE_NO_INFO;
    pSGIEntryNew->pimsmSGIJoinPruneState = PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO;
    if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
    {
      pSGIEntryNew->pimsmSGIAssertWinnerMetric = 
      pSGNode->pimsmSGEntry.pimsmSGRPFRouteMetric;
      pSGIEntryNew->pimsmSGIAssertWinnerMetricPref = 
      pSGNode->pimsmSGEntry.pimsmSGRPFRouteMetricPref;
      inetAddressZeroSet(pimsmCb ->family, &pSGIEntryNew->pimsmSGIAssertWinnerAddress);
    }

    pSGIEntryNew->pimsmSGICreateTime = osapiUpTimeRaw();
    pSGNode->pimsmSGIEntry[rtrIfNum] = pSGIEntryNew;
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NOTICE, "(S,G,%d) entry already exists", rtrIfNum);
    return L7_SUCCESS;        
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete (S,G,I) Entry 
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pSGNode     @b{(input)} (S,G) Node
* @param   rtrIfNum    @b{(input)} router Interface Number  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGIEntryDelete(pimsmCB_t *pimsmCb,
                            pimsmSGNode_t *pSGNode,
                            L7_uint32 rtrIfNum)
{
  pimsmSGIEntry_t *pSGIEntry = L7_NULLPTR;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];

  if (pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, 
      " (S,G,%d) already deleted ",rtrIfNum);
   return L7_SUCCESS;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pSGIEntry->pimsmSGIPrunePendingTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGIEntry->pimsmSGIJoinExpiryTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGIEntry->pimsmSGIAssertTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGIEntry->pimsmSGIPrunePendingTimerHandle);     
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGIEntry->pimsmSGIJoinExpiryTimerHandle);     
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGIEntry->pimsmSGIAssertTimerHandle);  
  PIMSM_FREE (pimsmCb->family, (void*) pSGIEntry);

  pSGNode->pimsmSGIEntry[rtrIfNum] = L7_NULLPTR;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Purge all (S,G) routes .
*
* @param    pimsmCb       @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGTreePurge(pimsmCB_t * pimsmCb)
{
  pimsmSGNode_t        *pSGNode;
  pimsmSGEntry_t *      pSGEntry;
  L7_RC_t               rc;
  L7_uint32             rtrIfNum;
  L7_uint32 mrtSGTableSize = 0;

  rc = pimsmSGFirstGet( pimsmCb, &pSGNode );
  while(rc == L7_SUCCESS)
  {
    pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_FALSE;
    pSGEntry = &pSGNode->pimsmSGEntry;

    pimsmUtilAppTimerCancel (pimsmCb, &(pSGEntry->pimsmSGKeepaliveTimer));
    pimsmUtilAppTimerCancel (pimsmCb, &(pSGEntry->pimsmSGDRRegisterStopTimer));
    pimsmUtilAppTimerCancel (pimsmCb, &(pSGEntry->pimsmSGUpstreamJoinTimer));
    handleListNodeDelete(pimsmCb->handleList,
                         &pSGEntry->pimsmSGDRRegisterStopTimerHandle);
    handleListNodeDelete(pimsmCb->handleList,
                         &pSGEntry->pimsmSGKeepaliveTimerHandle);
    handleListNodeDelete(pimsmCb->handleList,
                         &pSGEntry->pimsmSGUpstreamJoinTimerHandle);    

    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      pimsmSGIEntryDelete(pimsmCb, pSGNode, rtrIfNum);
    }
    rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
  }      

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGTableSize = PIMSM_S_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGTableSize = PIMSM_S_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }

  avlPurgeAvlTree(&pimsmCb->pimsmSGTree, mrtSGTableSize);

  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Post event to (S,G) Assert FSM
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pSGNode     @b{(input)} (S,G) Node
* @param   stopRPFIf   @b{(input)} RPF interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static void pimsmSGAssertEventPost(pimsmCB_t      *pimsmCb,
                                    pimsmSGNode_t *pSGNode,
                                    L7_BOOL        stopRPFIf)
{
  pimsmSGIEntry_t *pSGIEntry;
  L7_uint32 rtrIfNum;
  L7_BOOL bIAmAssertWinner;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmAssertMetric_t myAssertMetric;

   for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
   {
     pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
     if(pSGIEntry == L7_NULLPTR)
     {
       continue;
     }
     if(inetIsAddressZero(&pSGIEntry->pimsmSGIAssertWinnerAddress) 
       == L7_TRUE)
     {
       continue;
     }
     PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
     pimsmMyAssertMetric(pimsmCb, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
       &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
       pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex, &myAssertMetric);
    
     bIAmAssertWinner = pimsmCompareMetrics(
       myAssertMetric.metric_preference | myAssertMetric.rpt_bit_flag,
       myAssertMetric.route_metric,
       &myAssertMetric.ip_address,
       pSGIEntry->pimsmSGIAssertWinnerMetricPref,
       pSGIEntry->pimsmSGIAssertWinnerMetric,
       &pSGIEntry->pimsmSGIAssertWinnerAddress);
     if (bIAmAssertWinner == L7_TRUE) 
     {
      memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
      sgAssertEventInfo.rtrIfNum = rtrIfNum;
      sgAssertEventInfo.eventType = 
           PIMSM_ASSERT_S_G_SM_EVENT_METRIC_BETTER_THAN_WINNER;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"my assert metric better than assert winner");
      pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
     }
     if (stopRPFIf == L7_TRUE)
     {
      memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
      sgAssertEventInfo.rtrIfNum = rtrIfNum;
      sgAssertEventInfo.eventType = 
      PIMSM_ASSERT_S_G_SM_EVENT_STOP_BEING_RPF_IFACE;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO, "stop being RPF IIF");
      pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
     }
   }
}


/*********************************************************************
* @purpose Update (S,G) tree with Local membership changed
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param   pSrcAddr   @b{(input)} Src IP Addr
* @param   pGrpAddr   @b{(input)} Group IP Addr
* @param   rtrIfNum   @b{(input)} router Interface Number
* @param   bDelete    @b{(input)} Delete flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/

L7_RC_t pimsmSGLocalMemberShipChngEventPost(pimsmCB_t      *pimsmCb, 
                                            L7_inet_addr_t *pSrcAddr, 
                                            L7_inet_addr_t *pGrpAddr, 
                                            L7_uint32       rtrIfNum,
                                            L7_BOOL         bDelete,
                                            L7_BOOL         jpMsgImdtSend)
{
  pimsmSGNode_t * pSGNode =L7_NULLPTR;
  pimsmSGRptNode_t  * pSGRptNode;
  pimsmPerIntfSGAssertEventInfo_t sgAssertEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL result = L7_FALSE;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSrcAddr) ||
     (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  
  if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to find (S,G) node");
    return L7_FAILURE;
  }
  if(pSGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to find (S,G) node");
    return L7_FAILURE;
  }
  else
  {
    if (bDelete == L7_FALSE)
    {
      result = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
      if (result == L7_FALSE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGICouldAssert is FALSE");
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
        pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode, &sgAssertEventInfo);
      }
      else 
      {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGICouldAssert is True.");
      }
      result = pimsmSGIAssertTrackingDesired(pimsmCb, pSGNode,rtrIfNum);
      if (result == L7_FALSE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGIAssertTrackingDesired is FALSE");
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING;
        pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
      }
      else 
      {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGIAssertTrackingDesired is True.");
      }
    }
    memset(&upStrmSGEventInfo, 0 , 
           sizeof(pimsmUpStrmSGEventInfo_t));
    upStrmSGEventInfo.sendImmdFlag = jpMsgImdtSend;
    result = pimsmSGJoinDesired(pimsmCb, pSGNode);
    if (result == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGJoinDesired is TRUE");
      upStrmSGEventInfo.eventType =
        PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else if (result == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"SGJoinDesired is FALSE");
      upStrmSGEventInfo.eventType =
        PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  }
  if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,
                "(S,G,Rpt) node not found");
  }
  if (pSGRptNode != (pimsmSGRptNode_t *)L7_NULLPTR)
  {
    memset(&upStrmSGRptEventInfo, 0, sizeof(pimsmUpStrmSGRptEventInfo_t));
    result = pimsmSGRptPruneDesired(pimsmCb, pSGRptNode);
    if (result == L7_TRUE)
    {
      upStrmSGRptEventInfo.eventType = 
        PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
      pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);
    }
    else if (result == L7_FALSE)
    {
      upStrmSGRptEventInfo.eventType = 
        PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
      pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);
    }
  }

  /*Update only when entry exists in MFC */
  if (pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update (S,G) exact");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Update MFC with the(S,G) Entry
*
* @param    pimsmCb  @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/

L7_RC_t pimsmSGMFCUpdate(pimsmCB_t     *pimsmCb,
                         pimsmSGNode_t *pSGNode,
                         mfcOperationType_t eventType,
                         L7_BOOL bForceUpdate)
{
  interface_bitset_t sgInhrtdOif;
  interface_bitset_t scopedOifList;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Enter");  

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  memset(&sgInhrtdOif,0,sizeof(interface_bitset_t));
  if (eventType == MFC_UPDATE_ENTRY)
  {  
    if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, 
          "Node is deleted");            
      return L7_FAILURE;
    }  
    if( (bForceUpdate == L7_FALSE) &&
        (pimsmCb->pimsmRestartInProgress == L7_FALSE) &&
        (((pSGNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO, 
          "Entry is not added in MFC");
      return L7_SUCCESS;
    }    
    pimsmSGInhrtdOlist(pimsmCb,pSGNode,&sgInhrtdOif);
    BITX_RESET(&sgInhrtdOif, 
        pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex);
    if(BITX_TEST(&sgInhrtdOif,PIMSM_REGISTER_INTF_NUM) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
            "Register interface in oif");
      return L7_SUCCESS;
    }    
   if (pimsmCb->family == L7_AF_INET)
   {        
    /* subtract all admin-scoped interfaces */
    BITX_RESET_ALL(&scopedOifList);
    if (mcastMapGroupScopedInterfacesGet(&(pSGNode->pimsmSGEntry.pimsmSGGrpAddress),
                                         (L7_uchar8 *)&(scopedOifList)) == L7_SUCCESS)
    {
      if (BITX_TEST(&scopedOifList,pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex))
      {
        BITX_RESET_ALL(&sgInhrtdOif);
      }
      else
      {
        BITX_MASK_AND(&sgInhrtdOif, &scopedOifList, &sgInhrtdOif);
      }
    }
   }
  }
  else if (eventType == MFC_DELETE_ENTRY)
  {
    if(((pSGNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO, 
          "Entry is not present in MFC");
      return L7_SUCCESS;
    } 
  }
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, &sgInhrtdOif );    
  if(pimsmMfcQueue(pimsmCb,eventType,
                   &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                   &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                   pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex,
                   &sgInhrtdOif, L7_TRUE, &pSGNode->flags) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"failed to update MFC");
    return L7_FAILURE;
  }

  if(eventType == MFC_UPDATE_ENTRY )
  {
    pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Success");  
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (*,G) Tree when an interface is down
*
* @param    pimsmCb   @b{(input)} PIM SM Control Block
* @param   rtrIfNum  @b{(input)} Interface Number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  pimsmSGTreeIntfDownUpdate(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmSGNode_t  sgNode, *pSGNode, *pSGNodeDelete = L7_NULLPTR;
  pimsmSGEntry_t *pSGEntry;
  L7_RC_t rc;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr; 
  pimsmSendJoinPruneData_t    jpData;
  L7_uchar8 maskLen =0;
  interface_bitset_t      SGImdtOif1, SGImdtOif2;
  L7_BOOL couldAssert;
  pimsmSGIEntry_t *pSGIEntry = L7_NULLPTR;  
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  

  memset(&sgNode, 0, sizeof(pimsmSGNode_t));
  pSGNode = &sgNode;
  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while(rc == L7_SUCCESS)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    if(pSGEntry->pimsmSGRPFIfIndex == rtrIfNum)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
                  "Disabled interface is RPF interface",
                  rtrIfNum);
      if(inetIsAddressZero(&(pSGEntry->pimsmSGUpstreamNeighbor)) 
         == L7_FALSE)
      {
        /*Send Prune (S,G);*/
        memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
        jpData.rtrIfNum = pSGEntry->pimsmSGRPFIfIndex;
        if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                           &jpData.holdtime)
                                             != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, 
            PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
          jpData.holdtime = 3.5 * L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL;
        }                                           
        jpData.pNbrAddr 
        = &(pSGEntry->pimsmSGUpstreamNeighbor);
        jpData.addrFlags = 0;
        jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
        pimSrcMaskLenGet(pimsmCb->family,&maskLen);
        jpData.srcAddrMaskLen = maskLen;
        jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
        pimGrpMaskLenGet(pimsmCb->family,&maskLen);
        jpData.grpAddrMaskLen = maskLen;
        jpData.joinOrPruneFlag = L7_FALSE;
        if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Failed to send (S,G) Join/Prune Message");
        }
      }
      pSGNodeDelete = pSGNode;
    }
    else
    {
      pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
      if (pSGIEntry != L7_NULLPTR)
      {
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        couldAssert = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
        if(couldAssert == L7_FALSE)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"SGICouldAssert is FALSE");
          sgAssertEventInfo.eventType = 
          PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
          pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
        } 
        else
        {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NOTICE,"SGICouldAssert is FALSE");
        }      
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"In else case,SGI entry");
        memset(&SGImdtOif1, 0, sizeof(interface_bitset_t));
        memset(&SGImdtOif2, 0, sizeof(interface_bitset_t));            
        if (pimsmSGIEntryDelete(pimsmCb, pSGNode,rtrIfNum) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to delete (S,G,%d)", rtrIfNum);
        }
        memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
        joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
        if(joinDesired == L7_FALSE) 
        {
          upStrmSGEventInfo.eventType 
          = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
          pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
        }
        else
        {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                     "Error: pimsmSGJoinDesired() is true");
        }        
       if (pimsmSGMFCUpdate (pimsmCb, pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
       {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to update (S,G) Entry to MFC");
       }
      }
    }    

    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
    if(pSGNodeDelete != L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Deleting the (S,G)node");
      pSrcAddr= &(pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress);
      pGrpAddr= &(pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress);
      pimsmSGDelete(pimsmCb, pGrpAddr, pSrcAddr);
      pSGNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (S,G) tree when best route has changed
*
* @param    pimsmCb        @b{(input)} PIM SM Control Block
* @param   pRPFRouteInfo  @b{(input)} Route info    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGTreeRTOCommon(pimsmCB_t * pimsmCb, 
            pimsmSGNode_t *pSGNode,
            mcastRPFInfo_t *pRPFRouteInfo)
{
  pimsmSGEntry_t *pSGEntry;
  pimsmUpStrmSGEventInfo_t  upStrmSGEventInfo;
  L7_uint32 rtrIfNum;
  L7_BOOL couldAssert;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  L7_BOOL stopRPFIf =L7_FALSE;
  
  pSGEntry = &pSGNode->pimsmSGEntry;   
  if( pRPFRouteInfo->rpfIfIndex != pSGEntry->pimsmSGRPFIfIndex)
    {
       stopRPFIf = L7_TRUE;
       memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
       rtrIfNum = pRPFRouteInfo->rpfIfIndex;
       sgAssertEventInfo.rtrIfNum = rtrIfNum;
       couldAssert = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
       if(couldAssert == L7_FALSE)
       {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"SGICouldAssert is FALSE");
         sgAssertEventInfo.eventType = 
         PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
         pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
       } 
       else
       {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_NOTICE,"SGICouldAssert is TRUE");
       }
    } 
    memset(&upStrmSGEventInfo, 0, sizeof(pimsmUpStrmSGEventInfo_t));
    /* Pass old RPF info as Event Param */
    upStrmSGEventInfo.rpfInfo.rpfIfIndex =
    pSGEntry->pimsmSGRPFIfIndex;
    inetCopy(&upStrmSGEventInfo.rpfInfo.rpfNextHop,
             &pSGEntry->pimsmSGRPFNextHop);
    inetCopy(&upStrmSGEventInfo.rpfInfo.rpfRouteAddress,
             &pSGEntry->pimsmSGRPFRouteAddress);
    upStrmSGEventInfo.rpfInfo.rpfRouteProtocol =
            pSGEntry->pimsmSGRPFRouteProtocol;
    upStrmSGEventInfo.rpfInfo.rpfRouteMetric =
            pSGEntry->pimsmSGRPFRouteMetric;
    upStrmSGEventInfo.rpfInfo.rpfRouteMetricPref =
            pSGEntry->pimsmSGRPFRouteMetricPref;
    inetCopy(&upStrmSGEventInfo.upStrmNbrAddr,
           &pSGEntry->pimsmSGUpstreamNeighbor);
  
    /* Copy new RPF info to the (S,G) node */
    pSGEntry->pimsmSGRPFIfIndex = pRPFRouteInfo->rpfIfIndex;
    inetCopy(&pSGEntry->pimsmSGRPFNextHop,&pRPFRouteInfo->rpfNextHop);
    inetCopy(&pSGEntry->pimsmSGRPFRouteAddress,
           &pRPFRouteInfo->rpfRouteAddress);
    pSGEntry->pimsmSGRPFRouteProtocol = pRPFRouteInfo->rpfRouteProtocol;
    pSGEntry->pimsmSGRPFRouteMetric = pRPFRouteInfo->rpfRouteMetric;
    pSGEntry->pimsmSGRPFRouteMetricPref = pRPFRouteInfo->rpfRouteMetricPref;
    pSGEntry->pimsmSGRPFRoutePrefixLength =
       pRPFRouteInfo->prefixLength;
    /* Update the (*,G) entry with new RP and upstream neighbor address */      
    pimsmSGNeighborRpfDash(pimsmCb,pSGNode);    
    upStrmSGEventInfo.eventType = 
    PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT;
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
    pimsmSGAssertEventPost(pimsmCb, pSGNode, stopRPFIf);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RpfIfIndex is %d",
                           pRPFRouteInfo->rpfIfIndex);
 
   return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (S,G) tree when best route has changed
*
* @param    pimsmCb        @b{(input)} PIM SM Control Block
* @param   pRPFRouteInfo  @b{(input)} Route info    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGTreeBestRouteChngUpdate(pimsmCB_t * pimsmCb,
                                         mcastRPFInfo_t *pRPFRouteInfo)
{
  pimsmSGNode_t *pSGNode;
  pimsmSGEntry_t *pSGEntry;
  L7_RC_t rc;
  L7_BOOL stopRPFIf =L7_FALSE;
  L7_uint32 rtrIfNum =0,rpfIfIndex=0;
  mcastRPFInfo_t rpfRouteInfo;

  if(pRPFRouteInfo == (mcastRPFInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"pRPFRouteInfo is NULLPTR ");            
    return L7_FAILURE;
  }
  memcpy(&rpfRouteInfo, pRPFRouteInfo, sizeof(mcastRPFInfo_t));

  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while(rc == L7_SUCCESS)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    if(inetAddrCompareAddrWithMask(&pSGEntry->pimsmSGRPFRouteAddress, 
                                        pSGEntry->pimsmSGRPFRoutePrefixLength,
                                        &rpfRouteInfo.rpfRouteAddress, 
                                        rpfRouteInfo.prefixLength) == 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"rto Prefix Length is %d",
                         pRPFRouteInfo->prefixLength);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"Prefix Length is %d",
                         pSGEntry->pimsmSGRPFRoutePrefixLength);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"rto RPF Route Addr is",
                         &pRPFRouteInfo->rpfRouteAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RPF Route Addr is",
                         &pSGEntry->pimsmSGRPFRouteAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"rto Next Hop is",
                         &pRPFRouteInfo->rpfNextHop);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"Next Hop is",
                         &pSGEntry->pimsmSGRPFNextHop);

      pimsmSGNegativeDelete(pimsmCb, 
                            &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
      pimsmSGRptNegativeDelete(pimsmCb, 
                               &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);              
      pimsmStarStarRpNegativeDelete (pimsmCb,
                               &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

      if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rpfRouteInfo.rpfIfIndex) != L7_TRUE)
      {
        if (mcastIpMapUnnumberedIsLocalAddress(&pSGEntry->pimsmSGSrcAddress,
                                               L7_NULLPTR) == L7_TRUE)
        {
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfNextHop);
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfRouteAddress);
          rpfRouteInfo.rpfIfIndex = 0;
          rpfRouteInfo.rpfRouteMetric =  0;
          rpfRouteInfo.rpfRouteMetricPref = 0;
          rpfRouteInfo.rpfRouteProtocol = RTO_LOCAL;
        }
        else if (mcastIpMapIsDirectlyConnected(&pSGEntry->pimsmSGSrcAddress,
                                               &rtrIfNum) == L7_TRUE)
        {
          inetCopy(&rpfRouteInfo.rpfNextHop,
                   &pSGEntry->pimsmSGSrcAddress);
          rpfRouteInfo.rpfIfIndex = rtrIfNum;
          rpfRouteInfo.rpfRouteMetric =  0;
          rpfRouteInfo.rpfRouteMetricPref = 0;
          rpfRouteInfo.rpfRouteProtocol = RTO_LOCAL;
        }
      }
    
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"inetAddrCompareAddrWithMask successful");
      switch (rpfRouteInfo.status)
      {
        case RTO_DELETE_ROUTE:
          if(pimsmSGIEntryDelete(pimsmCb,pSGNode,
                                 pSGEntry->pimsmSGRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (S,G,%d) entry",
                          pSGEntry->pimsmSGRPFIfIndex);
              break;
          }
          rpfIfIndex =  pSGEntry->pimsmSGRPFIfIndex;         
          pimsmSGTreeRTOCommon(pimsmCb, pSGNode, &rpfRouteInfo);
          pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex = rpfIfIndex;         
          pimsmSGDelete(pimsmCb,&pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                        &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
          break;

        case RTO_CHANGE_ROUTE:
          /* no need to check for nexthop/ifindex, 
          anything might change like metric */
          if(pimsmSGIEntryDelete(pimsmCb,pSGNode,
                                 pSGEntry->pimsmSGRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (S,G,%d) entry",
                          pSGEntry->pimsmSGRPFIfIndex);
              break;
          }
          /* fall-through for remaining work*/
        case RTO_ADD_ROUTE:          
          if(pimsmSGIEntryCreate(pimsmCb,pSGNode,
                                    rpfRouteInfo.rpfIfIndex) 
             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR, 
                        "Failed to create (S,G,I) for upstream index %d",
                        rpfRouteInfo.rpfIfIndex);
            break;
          }       
          if((rpfRouteInfo.rpfRouteProtocol == RTO_LOCAL) ||
            (PIMSM_INET_IS_ADDR_EQUAL(&pSGEntry->pimsmSGRPFNextHop,
                  &(rpfRouteInfo.rpfNextHop)) == L7_FALSE ) || 
             (pSGEntry->pimsmSGRPFIfIndex != rpfRouteInfo.rpfIfIndex))
          {
             /* Note: stopRPFIf should be updated here only, 
              otherwise pimsmSGTreeRTOCommon() will change
              then pimsmSGRPFIfIndex */            
            if(rpfRouteInfo.rpfIfIndex != pSGEntry->pimsmSGRPFIfIndex)
            {
               stopRPFIf = L7_TRUE;
            }            
            pimsmSGTreeRTOCommon(pimsmCb, pSGNode, &rpfRouteInfo);
          } 
          else
          {
            /* Copy new RPF info to the (S,G) node */
            pSGEntry->pimsmSGRPFRouteMetric = rpfRouteInfo.rpfRouteMetric;
            pSGEntry->pimsmSGRPFRouteMetricPref = rpfRouteInfo.rpfRouteMetricPref;
          }

          pimsmSGAssertEventPost(pimsmCb, pSGNode, stopRPFIf);          
          if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                        "Failed to update (S,G) node exact");
          }            
          break;
        default:
          break;
      }
    }
        rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Update SG Tree with RP change
*
* @param    pimsmCb       @b{(input)} PIM SM Control Block
* @param   pGrpAddr      @b{(input)} Group IP Addr    
* @param   grpPrefixLen  @b{(input)} Source IP Addr
* @param   pRpAddr       @b{(input)} RP IP Addr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGTreeRPChngUpdate(pimsmCB_t       *pimsmCb, 
                                L7_inet_addr_t *pGrpAddr,
                                L7_uchar8       grpPrefixLen,
                                L7_inet_addr_t *pRpAddr)
{
  pimsmSGNode_t *pSGNode= L7_NULLPTR, *pSGNodeDelete = L7_NULLPTR;
  pimsmSGEntry_t *pSGEntry;
  pimsmPerSGRegisterEventInfo_t   sgRegisterEventInfo;
  L7_inet_addr_t   *pSrcAddrDelete, *pGrpAddrDelete;
  L7_RC_t rc;
  
  if (pGrpAddr == (L7_inet_addr_t *)L7_NULLPTR ||
      pRpAddr  == (L7_inet_addr_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, "Invalid input parameters");  
    return L7_FAILURE;
  }
  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while (rc == L7_SUCCESS)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    if (inetAddrCompareAddrWithMask(&pSGEntry->pimsmSGGrpAddress, 
                                    grpPrefixLen,
                                    pGrpAddr, 
                                    grpPrefixLen) == 0)
    {
      if (inetIsAddressZero(pRpAddr) ==L7_TRUE)
      {
        pSGNodeDelete = pSGNode;
      }
      else
      {
        if (pimsmSGRptTreeRPChngUpdate(pimsmCb,
             &pSGEntry->pimsmSGSrcAddress,&pSGEntry->pimsmSGGrpAddress )
             != L7_SUCCESS)
        {
          pSGNodeDelete = pSGNode;
        }
        memset(&sgRegisterEventInfo, 0 , sizeof(pimsmPerSGRegisterEventInfo_t));
        sgRegisterEventInfo.eventType = PIMSM_REG_PER_S_G_SM_EVENT_RP_CHANGED;
        pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &sgRegisterEventInfo);  
  
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
              " Deleting entries from MFC ");
        pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);            
      }
    }
    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
    if (pSGNodeDelete != L7_NULLPTR)
    {
      pSrcAddrDelete= &(pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress);
      pGrpAddrDelete= &(pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress);
      pimsmSGDelete(pimsmCb, pGrpAddrDelete, pSrcAddrDelete);
      pSGNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update SG Tree whwn (S,G) join received
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pGgrpAddr  @b{(input)} Group IP Addr
* @param rtrIfNum   @b{(input)} router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/

L7_RC_t pimsmSGTreeJoinRecvEntryUpdate(pimsmCB_t      *pimsmCb,
                                       L7_inet_addr_t *pGrpAddr,
                                       L7_uint32       rtrIfNum)
{
  pimsmSGNode_t *pSGNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL result = L7_FALSE;
  pimsmPerIntfSGAssertEventInfo_t sgAssertEventInfo;
  L7_BOOL             joinDesired;  
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
   PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NOTICE,"Entry");

  rc = pimsmSGFirstGet(pimsmCb,&pSGNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                             &pSGNode->pimsmSGEntry.pimsmSGGrpAddress) 
       == L7_TRUE)
    {

      if(pimsmSGIEntryCreate(pimsmCb,pSGNode,rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to create (S,G,%d)",rtrIfNum);
        return L7_FAILURE;
      }
      memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
      upStrmSGEventInfo.sendImmdFlag = L7_TRUE;      
      joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
      if(joinDesired == L7_TRUE)
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
      result = pimsmSGIAssertTrackingDesired(pimsmCb, pSGNode,rtrIfNum);
      if (result == L7_FALSE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"SGIAssertTrackingDesired is FALSE");
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING;
        pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
      }
      else 
      {
         PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NOTICE,"SGIAssertTrackingDesired is True.");
      }
      if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to update (S,G) node exact");
        return L7_FAILURE;
      }
    }
    rc = pimsmSGNextGet(pimsmCb,pSGNode,&pSGNode);
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Update SG Tree whwn (S,G) prune received
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pGgrpAddr  @b{(input)} Group IP Addr
* @param rtrIfNum   @b{(input)} router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGTreePruneRecvEntryUpdate(pimsmCB_t      *pimsmCb,
                                        L7_inet_addr_t *pGrpAddr,
                                        L7_uint32       rtrIfNum)
{
  pimsmSGNode_t *pSGNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL             joinDesired;  
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGFirstGet(pimsmCb,&pSGNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                             &pSGNode->pimsmSGEntry.pimsmSGGrpAddress) 
       == L7_TRUE)
    {
      if(pimsmSGIEntryDelete(pimsmCb,pSGNode,rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to delete (S,G,%d)", rtrIfNum);
        rc = pimsmSGNextGet(pimsmCb,pSGNode,&pSGNode);
        continue;
      }
      pSGNode->pimsmSGIEntry[rtrIfNum] = L7_NULLPTR;
      memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
      upStrmSGEventInfo.sendImmdFlag = L7_TRUE;            
      joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
      if(joinDesired == L7_TRUE)
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
      if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to update (S,G) node exact");
        rc = pimsmSGNextGet(pimsmCb,pSGNode,&pSGNode);
        continue;
      }
    }
    rc = pimsmSGNextGet(pimsmCb,pSGNode,&pSGNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test whether  mrt entry from the (S,G) entry table can be deleted
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimsmSGNodeCanRemove(pimsmCB_t * pimsmCb, pimsmSGNode_t *pSGNode)
{

  pimsmSGEntry_t  *pSGEntry;
  pimsmSGIEntry_t  *pSGIEntry;  
  L7_uint32 index;

  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
     pSGIEntry = pSGNode->pimsmSGIEntry[index];
     if ( pSGIEntry != L7_NULLPTR)
     {
        if((pSGIEntry->pimsmSGILocalMembership == L7_TRUE) ||
           (pSGIEntry->pimsmSGIJoinPruneState !=
              PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO) ||
           (pSGIEntry->pimsmSGIAssertState !=
              PIMSM_ASSERT_S_G_SM_STATE_NO_INFO) ||
           (pSGIEntry->pimsmSGIPrunePendingTimer != L7_NULLPTR) || 
           (pSGIEntry->pimsmSGIJoinExpiryTimer != L7_NULLPTR) ||
           (pSGIEntry->pimsmSGIAssertTimer != L7_NULLPTR))
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, 
            "(S,G, %d) is active", index);        
          return L7_FALSE; 
        }
     }
  }
  pSGEntry = &pSGNode->pimsmSGEntry;
  if((pSGEntry->pimsmSGUpstreamJoinState !=
        PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED) ||
     (pSGEntry->pimsmSGDRRegisterState !=
        PIMSM_REG_PER_S_G_SM_STATE_NO_INFO) ||
     (pSGEntry->pimsmSGUpstreamJoinTimer != L7_NULLPTR) || 
     (pSGEntry->pimsmSGKeepaliveTimer != L7_NULLPTR) ||
     (pSGEntry->pimsmSGDRRegisterStopTimer != L7_NULLPTR) ||
     (pSGEntry->pimsmSGSPTBit != L7_FALSE))
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, 
            "(S,G) is active");
    return L7_FALSE; 
  } 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, 
            "(S,G) is in-active");        
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_NORMAL, "Group Address :",
                                      &pSGEntry->pimsmSGGrpAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G,  PIMSM_TRACE_NORMAL, "Source Address :", 
                                      &pSGEntry->pimsmSGSrcAddress);
  
  return L7_TRUE;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL pimsmSGNodeTryRemove(pimsmCB_t * pimsmCb, pimsmSGNode_t *pSGNode)
{
  L7_BOOL retVal;
  pimsmSGRptNode_t *pSGRptNode;
  L7_RC_t rc;

  retVal = pimsmSGNodeCanRemove(pimsmCb, pSGNode);
  if(retVal == L7_TRUE) 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, 
        "SGNodeCanRemove = %d", retVal);
    pSGNode->flags |=  PIMSM_NODE_DELETE;  
    rc = pimsmSGRptFind( pimsmCb,
                             &pSGNode->pimsmSGEntry.pimsmSGGrpAddress,
                             &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                             &pSGRptNode );
           
    if(rc == L7_SUCCESS)
    {  
      retVal = pimsmSGRptNodeTryRemove(pimsmCb, pSGRptNode);
    }
  }
  return retVal;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void pimsmSGNodeCleanup(pimsmCB_t * pimsmCb)
{
  pimsmSGNode_t *pSGNode, *pSGNodeDelete = L7_NULLPTR;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pimsmSGNode_t  sGNodeSearch;

  /* Set all elements of the node to zero */
  memset (&sGNodeSearch, 0, sizeof (pimsmSGNode_t));
  inetAddressZeroSet(pimsmCb->family,
        &sGNodeSearch.pimsmSGEntry.pimsmSGGrpAddress);
  inetAddressZeroSet(pimsmCb->family,
        &sGNodeSearch.pimsmSGEntry.pimsmSGSrcAddress);  
  
  pSGNode = avlSearchLVL7(&pimsmCb->pimsmSGTree, &sGNodeSearch, AVL_NEXT);
  
  while(pSGNode != L7_NULLPTR)
  {
    if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
     pSGNodeDelete =  pSGNode;
    }
    pSGNode = avlSearchLVL7(&pimsmCb->pimsmSGTree,pSGNode, AVL_NEXT);
    if(pSGNodeDelete != L7_NULLPTR)
    {
      pGrpAddr= &(pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress);
      pSrcAddr= &(pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL,
              "(S,G) is deleted ");
      pimsmSGDelete(pimsmCb, pGrpAddr, pSrcAddr);
      pSGNodeDelete = L7_NULLPTR;
    }    
  }
}


/*********************************************************************
* @purpose  Update 
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pGgrpAddr  @b{(input)} Group IP Addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pGrpAddr)
{
  pimsmSGNode_t *pSGNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGFirstGet(pimsmCb,&pSGNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                             &pSGNode->pimsmSGEntry.pimsmSGGrpAddress) 
       == L7_TRUE)
    {
      if((pSGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
      }
    }
    rc = pimsmSGNextGet(pimsmCb,pSGNode,&pSGNode);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update the (S,G) entries with admin-scope config change
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGAdminScopeUpdate(pimsmCB_t *pimsmCb)
{
  pimsmSGNode_t *pSGNode= L7_NULLPTR;
  L7_RC_t rc;

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while (rc == L7_SUCCESS)
  {
    if (pimsmSGMFCUpdate(pimsmCb,pSGNode , MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      L7_uchar8 src[PIMSM_MAX_MSG_SIZE];
      L7_uchar8 grp[PIMSM_MAX_MSG_SIZE];
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(%s,%s) MFC Update Failed",
                  inetAddrPrint(&pSGNode->pimsmSGEntry.pimsmSGSrcAddress, src),
                  inetAddrPrint(&pSGNode->pimsmSGEntry.pimsmSGGrpAddress, grp));
    }
    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update the (S,G) entries Nexthop as link-local addr.
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
void  pimsmSGNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry)
{
  pimsmSGNode_t        *pSGNode;
  pimsmSGEntry_t *      pSGEntry;
  L7_RC_t               rc;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  L7_inet_addr_t primaryAddr;  
  
  if(pimsmCb->pimsmSGNextHopUpdateCnt <= 0)
  {
      return;
  }
  
  rc = pimsmSGFirstGet( pimsmCb, &pSGNode );
  while(rc == L7_SUCCESS)
  {    
    pSGEntry = &pSGNode->pimsmSGEntry;

    if(pSGEntry->pimsmSGRPFIfIndex != rtrIfNum)
    {
      rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
      continue;
    }
    if (pimsmCb->family == L7_AF_INET6 && 
      inetIsAddressZero(&pSGEntry->pimsmSGRPFNextHop) == L7_FALSE &&
      (L7_IP6_IS_ADDR_LINK_LOCAL(&pSGEntry->pimsmSGRPFNextHop.addr.ipv6)))
    {
      rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
      continue;
    }    
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, 
        "(S,G) Getting primary addr");   
    
    if(pimsmNbrPrimaryAddrGet(pimsmCb,
            pimsmNbrEntry, &pSGEntry->pimsmSGRPFNextHop,
            &primaryAddr)!=L7_SUCCESS)
    {
      rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
      continue;
    }
    inetCopy(&pSGEntry->pimsmSGRPFNextHop,&primaryAddr);
    pimsmCb->pimsmSGNextHopUpdateCnt--;
    pimsmSGNeighborRpfDash(pimsmCb,pSGNode);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, 
        "(S,G) primary addr updated");    
    
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
    if(pimsmSGMFCUpdate (pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "MFC Updation failed");
    }
    rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
  }   
}

