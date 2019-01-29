/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGRptTree.c
*
* @purpose Contains PIM-SM  avltree function definitions for (S,G,Rpt) entry
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
#include "pimsmsgrpttree.h"
#include "avl_api.h"
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmmacros.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"

/*********************************************************************
* @purpose  Compares the given (S,G,Rpt) nodes
*
* @param pimsmSGNode1  @b{(input)} (S,G,Rpt) Node
* @param pimsmSGNode2  @b{(input)} (S,G,Rpt) Node
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
static L7_int32 pimsmSGRptCompare(const void *pNode1, 
                                  const void *pNode2,
                                  L7_uint32   keySize)
{
  pimsmSGRptNode_t * pSGRptNode1, *pSGRptNode2;
  L7_inet_addr_t  * pSrcAddr1, * pSrcAddr2;
  L7_inet_addr_t  * pGrpAddr1, * pGrpAddr2;
  L7_uint32   result;

  pSGRptNode1 = (pimsmSGRptNode_t *)pNode1;
  pSGRptNode2 = (pimsmSGRptNode_t *)pNode2;

  pSrcAddr1 = &pSGRptNode1->pimsmSGRptEntry.pimsmSGRptSrcAddress;
  pSrcAddr2 = &pSGRptNode2->pimsmSGRptEntry.pimsmSGRptSrcAddress;
  pGrpAddr1 = &pSGRptNode1->pimsmSGRptEntry.pimsmSGRptGrpAddress;
  pGrpAddr2 = &pSGRptNode2->pimsmSGRptEntry.pimsmSGRptGrpAddress;

  result = L7_INET_ADDR_COMPARE(pGrpAddr1, pGrpAddr2);
  if(result == 0)
  {
    return L7_INET_ADDR_COMPARE(pSrcAddr1, pSrcAddr2);
  }
  return result;
}
/*********************************************************************
* @purpose  Initializes the (S,G,Rpt) entry table
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
L7_RC_t pimsmSGRptInit(pimsmCB_t * pimsmCb)
{
  L7_uint32 mrtSGRptTableSize = 0;

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  pimsmCb->pimsmSGRptTreeHeap = PIMSM_ALLOC (pimsmCb->family, 
                                             (mrtSGRptTableSize * 
                                             sizeof (avlTreeTables_t)));

  if(pimsmCb->pimsmSGRptTreeHeap == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (S,G,rpt) Table Tree Heap\n");
    return L7_FAILURE;
  }
  pimsmCb->pimsmSGRptDataHeap =  PIMSM_ALLOC (pimsmCb->family,
                                              (mrtSGRptTableSize * 
                                              sizeof (pimsmSGRptNode_t)));
  if(pimsmCb->pimsmSGRptDataHeap == L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGRptTreeHeap);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (S,G,rpt) Table Data Heap\n");
    return L7_FAILURE;
  }

  /* Cleanup data structures before proceeding */
  memset (pimsmCb->pimsmSGRptTreeHeap, 0, mrtSGRptTableSize * 
          sizeof (avlTreeTables_t));
  memset (pimsmCb->pimsmSGRptDataHeap, 0, mrtSGRptTableSize * 
          sizeof (pimsmSGRptNode_t));

  /* The destination networks are stored in an AVL tree. */
  avlCreateAvlTree(&pimsmCb->pimsmSGRptTree, pimsmCb->pimsmSGRptTreeHeap,
                   pimsmCb->pimsmSGRptDataHeap, mrtSGRptTableSize,
                   (L7_uint32)(sizeof(pimsmSGRptNode_t)), PIMSM_AVL_TREE_TYPE, 
                   sizeof(L7_inet_addr_t) * 2);
  (void)avlSetAvlTreeComparator(&pimsmCb->pimsmSGRptTree, pimsmSGRptCompare); 

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Frees the (S,G,Rpt) entry table
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
L7_RC_t pimsmSGRptDeInit(pimsmCB_t * pimsmCb)
{
  
  if(pimsmCb->pimsmSGRptTreeHeap != (avlTreeTables_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGRptTreeHeap);
    pimsmCb->pimsmSGRptTreeHeap = L7_NULLPTR;
  }
  if(pimsmCb->pimsmSGRptDataHeap != (pimsmSGRptNode_t *) L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmSGRptDataHeap);
    pimsmCb->pimsmSGRptDataHeap = L7_NULLPTR;
  }
  pimsmCb->pimsmSGRptDataHeap = L7_NULLPTR;
  /* The destination networks are stored in an AVL tree.
  */
  avlDeleteAvlTree(&pimsmCb->pimsmSGRptTree);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a mrt entry to the (S,G,Rpt) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G,Rpt) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptAdd(pimsmCB_t * pimsmCb, pimsmSGRptNode_t *pSGRptNode)
{
  pimsmSGRptNode_t *pSGRptNodeTemp = L7_NULLPTR;
  L7_uint32 mrtSGRptTableSize = 0;

  if(pimsmCb == L7_NULLPTR || 
     pSGRptNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  if(avlTreeCount(&pimsmCb->pimsmSGRptTree) >= mrtSGRptTableSize)
  {

    osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) ;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, "PIMSM (S,G,RPt) Table Max Limit - "
			"[%d] Reached; Cannot accomodate any further routes. PIMSM Multicast Route table (S,G,RPt) "
			"has reached maximum capacity and cannnot accommodate new registrations anymore.",
             mrtSGRptTableSize);
    return L7_FAILURE;
  }

  pSGRptNodeTemp = avlInsertEntry(&pimsmCb->pimsmSGRptTree, pSGRptNode);
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }

  if(pSGRptNodeTemp != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Unable to add to the (S,G,Rpt) tree! avlInsert Failed. ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a mrt entry from the (S,G,Rpt) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pGrpAddr  @b{(input)} Source IP addr     
* @param pSrcAddr   @b{(input)} Group IP Addr  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptDelete(pimsmCB_t * pimsmCb,
                         L7_inet_addr_t *pGrpAddr, 
                         L7_inet_addr_t *pSrcAddr)
{
  pimsmSGRptNode_t sgRptNodeSearch;
  pimsmSGRptNode_t * pSGRptNode,*pSGRptNodeDelete =L7_NULLPTR;
  L7_uint32   index;
  
  if(pimsmCb == L7_NULLPTR || 
     pGrpAddr == L7_NULLPTR ||
     pSrcAddr == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }
  /* Cleanup the memory */  
  memset (&sgRptNodeSearch, 0, sizeof (pimsmSGRptNode_t));

  inetCopy(&sgRptNodeSearch.pimsmSGRptEntry.pimsmSGRptSrcAddress, pSrcAddr);
  inetCopy(&sgRptNodeSearch.pimsmSGRptEntry.pimsmSGRptGrpAddress, pGrpAddr);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pSGRptNodeDelete = avlSearchLVL7(&pimsmCb->pimsmSGRptTree, &sgRptNodeSearch,
                                    AVL_EXACT);
  if(pSGRptNodeDelete == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to delete (S,G,Rpt) node ");
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
    if(pSGRptNodeDelete->pimsmSGRptIEntry[index] != L7_NULLPTR)
    {
      pimsmSGRptIEntryDelete(pimsmCb,
                             pSGRptNodeDelete,index);
    }
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptUpstreamOverrideTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptUpstreamOverrideTimerHandle);
  pimsmSGRptMFCUpdate(pimsmCb, pSGRptNodeDelete, MFC_DELETE_ENTRY, L7_FALSE);
  pSGRptNode = avlDeleteEntry(&pimsmCb->pimsmSGRptTree,pSGRptNodeDelete);
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  if(pSGRptNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to delete (S,G,Rpt) node ");
    return L7_SUCCESS;

  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify a mrt entry in the (S,G,Rpt) entry table
*
* @param routeEntry  @b{(input)} (S,G,Rpt) node         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptModify(pimsmCB_t * pimsmCb, pimsmSGRptNode_t *pSGRptNode)
{
  MCAST_UNUSED_PARAM(pSGRptNode);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Check whether the (S,G,Rpt) entry exists in the (S,G,Rpt) 
*        entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G,Rpt) Node          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptFirstGet(pimsmCB_t * pimsmCb, pimsmSGRptNode_t **ppSGRptNode)
{
  pimsmSGRptNode_t  sgRptNode;

  /* Set all elements of the node to zero */
    memset (&sgRptNode, 0, sizeof (pimsmSGRptNode_t));
    inetAddressZeroSet(pimsmCb->family,
          &sgRptNode.pimsmSGRptEntry.pimsmSGRptGrpAddress);
    inetAddressZeroSet(pimsmCb->family,
          &sgRptNode.pimsmSGRptEntry.pimsmSGRptSrcAddress);    
    return pimsmSGRptEntryFind(pimsmCb, &sgRptNode, AVL_NEXT, ppSGRptNode);
}


/*********************************************************************
* @purpose  Get the next (S,G,Rpt) entry after the specified (S,G,Rpt) entry 
*           in the (S,G,Rpt) entry table
*
* @param    pimsmCb            @b{(input)} PIM SM Control Block
* @param pSGRptNodeCurrent  @b{(input)} Current (S,G,Rpt) Node    
* @param ppSGrptNode        @b{(input)} Pointer to next (S,G,Rpt) Node         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptNextGet(pimsmCB_t         *pimsmCb,
                          pimsmSGRptNode_t  *pSGRptNodeCurrent, 
                          pimsmSGRptNode_t **ppSGRptNode)
{
  return pimsmSGRptEntryFind(pimsmCb, pSGRptNodeCurrent, AVL_NEXT, ppSGRptNode);
}

/*********************************************************************
* @purpose  Search the (S,G,Rpt) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pGrpAddr    @b{(input)} Group IP Addr    
* @param   pSrcAddr    @b{(input)} Source IP Addr   
* @param   ppSGNode    @b{(output)} Pointer to (S,G,Rpt) Node           
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptFind(pimsmCB_t         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,
                       L7_inet_addr_t    *pSrcAddr, 
                       pimsmSGRptNode_t **ppSGRptNode)
{
  pimsmSGRptNode_t sgRptNode;
  L7_RC_t rc;
  
  memset (&sgRptNode, 0, sizeof (pimsmSGRptNode_t));
  inetCopy(&sgRptNode.pimsmSGRptEntry.pimsmSGRptGrpAddress, pGrpAddr);
  inetCopy(&sgRptNode.pimsmSGRptEntry.pimsmSGRptSrcAddress,  pSrcAddr);
  rc = pimsmSGRptEntryFind(pimsmCb, &sgRptNode, AVL_EXACT, ppSGRptNode);
  
  return rc;      
}

/*********************************************************************
* @purpose  Search the (S,G,Rpt) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   pGrpAddr    @b{(input)} Group IP Addr    
* @param   pSrcAddr    @b{(input)} Source IP Addr   
* @param   ppSGNode    @b{(output)} Pointer to (S,G,Rpt) Node           
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmSGRptFindExact(pimsmCB_t         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,
                       L7_inet_addr_t    *pSrcAddr, 
                       pimsmSGRptNode_t **ppSGRptNode)
{
  pimsmSGRptNode_t sgRptNode;
  pimsmSGRptNode_t *pSGRptNodeTemp;
  
  memset (&sgRptNode, 0, sizeof (pimsmSGRptNode_t));
  inetCopy(&sgRptNode.pimsmSGRptEntry.pimsmSGRptGrpAddress, pGrpAddr);
  inetCopy(&sgRptNode.pimsmSGRptEntry.pimsmSGRptSrcAddress,  pSrcAddr);
  
  pSGRptNodeTemp = avlSearchLVL7(&pimsmCb->pimsmSGRptTree, &sgRptNode,
                                 AVL_EXACT);
    

  if (pSGRptNodeTemp == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_DEBUG,
        "Failed to find (S,G,Rpt) node");
    *ppSGRptNode = L7_NULLPTR;         
    return L7_FAILURE;
  }
  *ppSGRptNode = pSGRptNodeTemp;
  return L7_SUCCESS;     
}


/*********************************************************************
* @purpose  Search the (S,G) entry table
*
* @param    pimsmCb            @b{(input)} PIM SM Control Block
* @param pSGRptNodeCurrent  @b{(input)} Current (S,G,Rpt) Node    
* @param ppSGrptNode        @b{(input)} Pointer to next (S,G,Rpt) Node
* @param flag               @b{(input)} AVL_EXACT or AVL_NEXT 
* @param   ppSGrptNode        @b{(output)} Pointer to next (S,G,Rpt) Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptEntryFind(pimsmCB_t         *pimsmCb, 
                            pimsmSGRptNode_t  *pSGRptNodeCurrent,
                       L7_uint32          flag,
                       pimsmSGRptNode_t **ppSGRptNode)
{
  pimsmSGRptNode_t *pSGRptNodeTemp;


  if((pSGRptNodeCurrent == L7_NULLPTR) ||(ppSGRptNode == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
             "invalid input parameters");

    return L7_FAILURE;
  }
  *ppSGRptNode = L7_NULLPTR;
  pSGRptNodeTemp = avlSearchLVL7(&pimsmCb->pimsmSGRptTree, pSGRptNodeCurrent,
                                 flag);
    

    if (pSGRptNodeTemp == L7_NULLPTR)
  {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_DEBUG,"Failed to find (S,G,Rpt) node");
    *ppSGRptNode = L7_NULLPTR;         
    return L7_FAILURE;
  }
  if((pSGRptNodeTemp->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    *ppSGRptNode = L7_NULLPTR;   
    return L7_FAILURE;
  } 
  *ppSGRptNode = pSGRptNodeTemp;
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Create a new (S,G, Rpt) node in the (S,G,Rpt) tree
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSrcAddr    @b{(input)} Source IP Addr  
* @param pGrpAddr    @b{(input)} Group IP Addr    
* @param ppSGNode    @b{(input)} Pointer to (S,G,Rpt) Node     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptNodeCreate(pimsmCB_t         *pimsmCb,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t    *pGrpAddr,
                             pimsmSGRptNode_t **ppSGRptNode)
{
  pimsmSGRptEntry_t * pSGRptEntry = L7_NULLPTR;
  pimsmSGRptNode_t  * pSGRptNode, sgRptNodeNew;
  L7_inet_addr_t    rpAddr;
  interface_bitset_t  sgRptInhrtdOif;
  L7_BOOL             result = L7_FALSE;
  L7_uint32           ii =0;
  L7_RC_t rc;
  L7_BOOL retVal;
  pimsmStarStarRpNode_t *   pStarStarRpNode = L7_NULLPTR;
  
  if((pSrcAddr == (L7_inet_addr_t *)L7_NULLPTR)||
     (pGrpAddr== (L7_inet_addr_t *)L7_NULLPTR)||
     (ppSGRptNode== (pimsmSGRptNode_t **)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, 
                "invalid input parameters ");
    return L7_FAILURE;    
  }
  /* SSM address range - No (S,G,rpt) node */
  retVal = pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr);
  if(retVal == L7_TRUE)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Group is In SSM Range", pGrpAddr);
    return L7_FAILURE;
  }
  rc = pimsmSGRptFindExact(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(rc != L7_SUCCESS)
  {
    memset(&sgRptNodeNew, 0 ,sizeof(pimsmSGRptNode_t));
    pSGRptEntry = &sgRptNodeNew.pimsmSGRptEntry;

    inetCopy(&pSGRptEntry->pimsmSGRptSrcAddress, pSrcAddr);
    inetCopy(&pSGRptEntry->pimsmSGRptGrpAddress, pGrpAddr);
    
    if(pimsmRPTJoinDesired(pimsmCb,pGrpAddr) == L7_TRUE)
    {
      pSGRptEntry->pimsmSGRptUpstreamPruneInitState
        = PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT;
    }
    else
    { 
      pSGRptEntry->pimsmSGRptUpstreamPruneInitState 
        = PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED;
    }
    pSGRptEntry->pimsmSGRptUpstreamPruneState = 
        pSGRptEntry->pimsmSGRptUpstreamPruneInitState;
    
    if(pimsmSGRptAdd(pimsmCb, &sgRptNodeNew) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Failed to add (S,G,Rpt) node");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT,  PIMSM_TRACE_INFO, " Group Address :",
                        &pSGRptEntry->pimsmSGRptGrpAddress);

      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT,  PIMSM_TRACE_INFO, "Source SAddress :", 
                        &pSGRptEntry->pimsmSGRptSrcAddress);
      *ppSGRptNode = L7_NULLPTR;
      return L7_FAILURE;    
    }
    if(pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) 
       != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Failed to find added (S,G,Rpt) node");
      return L7_FAILURE;    
    }
    pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
    /*pSGRptNode->pSelf = pSGRptNode;*/
    pSGRptEntry->pimsmSGRptUpstreamOverrideTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGRptEntry->pimsmSGRptUpstreamOverrideTimerParam); 
    if(pimsmRpAddressGet(pimsmCb, pGrpAddr,  &rpAddr) != L7_SUCCESS)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "RP not found for group : ",pGrpAddr);
      return L7_FAILURE;    
    }

    rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );   
    if(rc == L7_SUCCESS)
    {
      pSGRptEntry->pimsmSGRptRPFIfIndex= 
        pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
         "The RPFIfIndex is %d", pSGRptEntry->pimsmSGRptRPFIfIndex);
    }
    else
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
               "Failed to get RPF interface for RP :", &rpAddr);
      pimsmSGRptDelete(pimsmCb, pGrpAddr, pSrcAddr);
      return L7_FAILURE;
    } 
    if(pimsmSGRptIEntryCreate(pimsmCb,pSGRptNode,
                              pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex)
                              != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, 
                  "Failed to create (S,G,I) for upstream index %d",
                  pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex);
      pimsmSGRptDelete(pimsmCb, pGrpAddr, pSrcAddr);
      return L7_FAILURE;
    }
    pimsmSGRptNeighborRpfDash(pimsmCb, pSGRptNode);  
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
      "SGRptNeighborRpfDash :",
         &pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamNeighbor);     
    pSGRptNode->pimsmSGRptEntry.pimsmSGRptCreateTime = osapiUpTimeRaw();
    memset(&sgRptInhrtdOif,0,sizeof(interface_bitset_t));
    pimsmSGRptInhrtdOlist(pimsmCb,pSGRptNode,&sgRptInhrtdOif);
    

    BITX_IS_EMPTY(&sgRptInhrtdOif,result);
    if(result != L7_TRUE)
    {
      for(ii =1;ii < MCAST_MAX_INTERFACES;ii++)
      {
        if(BITX_TEST(&sgRptInhrtdOif,ii) != L7_NULL)
        {
          if(pimsmSGRptIEntryCreate(pimsmCb,pSGRptNode,ii) 
             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, 
                        "Failed to create (S.G,Rpt,%d)",ii);
          }
        }
      }/*end of for loop */
    }
  }
  pSGRptNode->flags &=  ~PIMSM_NODE_DELETE;
  *ppSGRptNode = pSGRptNode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create (S,G,Rpt,I) Entry and init with default params
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G,Rpt) Node
* @param rtrIfNum    @b{(input)} router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmSGRptIEntryCreate(pimsmCB_t        *pimsmCb, 
                               pimsmSGRptNode_t *pSGRptNode,
                               L7_uint32         rtrIfNum)
{
  pimsmSGRptIEntry_t * pSGRptIEntryNew;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGRptNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  
  if(pSGRptNode->pimsmSGRptIEntry[rtrIfNum] ==(pimsmSGRptIEntry_t *)L7_NULLPTR)
  {
    if ((pSGRptIEntryNew = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmSGRptIEntry_t)))
                                     == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (S,G,Rpt) interface pool");       
      return L7_FAILURE;
    }                          
    memset(pSGRptIEntryNew, 0, sizeof(pimsmSGRptIEntry_t));
    pSGRptIEntryNew->pimsmSGRptIIfIndex= rtrIfNum;
    pSGRptIEntryNew->pimsmSGRptIPrunePendingTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGRptIEntryNew->pimsmSGRptIPrunePendingTimerParam); 
    pSGRptIEntryNew->pimsmSGRptIPruneExpiryTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pSGRptIEntryNew->pimsmSGRptIPruneExpiryTimerParam); 
    pSGRptIEntryNew->pimsmSGRptILocalMembership = L7_FALSE;
    pSGRptIEntryNew->pimsmSGRptIJoinPruneState
    = PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO;
    pSGRptIEntryNew->pimsmSGRptICreateTime = osapiUpTimeRaw();
    pSGRptNode->pimsmSGRptIEntry[rtrIfNum] = pSGRptIEntryNew;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete (S,G,Rpt,I) Entry
*
* @param    pimsmCB       @b{(input)} Control block
* @param    pSGRptIEntry  @b{(input)} (S,G,Rpt,I) Entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t pimsmSGRptIEntryDelete(pimsmCB_t        *pimsmCb,
                        pimsmSGRptNode_t *pSGRptNode,
                               L7_uint32         rtrIfNum)
{
  pimsmSGRptIEntry_t *pSGRptIEntry = L7_NULLPTR;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGRptNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if (pSGRptIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_DEBUG,
         " (S,G,Rpt,%d) already deleted ",rtrIfNum);
   return L7_SUCCESS;
  }   
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptIEntry->pimsmSGRptIPrunePendingTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGRptIEntry->pimsmSGRptIPrunePendingTimerHandle);

  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptIEntry->pimsmSGRptIPruneExpiryTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pSGRptIEntry->pimsmSGRptIPruneExpiryTimerHandle);

  PIMSM_FREE (pimsmCb->family, (void*) pSGRptIEntry);
  pSGRptNode->pimsmSGRptIEntry[rtrIfNum] = L7_NULLPTR;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Purge all (s,G,Rpt)routes .
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGRptTreePurge(pimsmCB_t * pimsmCb)
{
  pimsmSGRptNode_t        * pSGRptNode;
  pimsmSGRptEntry_t *       pSGRptEntry;
  L7_RC_t                   rc;
  L7_uint32                 rtrIfNum;
  L7_uint32                 mrtSGRptTableSize = 0;

  rc = pimsmSGRptFirstGet( pimsmCb, &pSGRptNode );
  while(rc == L7_SUCCESS)
  {
    pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_DELETE_ENTRY, L7_FALSE);
    pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;  
    pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptEntry->pimsmSGRptUpstreamOverrideTimer));
    handleListNodeDelete(pimsmCb->handleList,
                         &pSGRptEntry->pimsmSGRptUpstreamOverrideTimerHandle);

    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      pimsmSGRptIEntryDelete(pimsmCb, pSGRptNode,rtrIfNum);
    }    
   rc = pimsmSGRptNextGet( pimsmCb, pSGRptNode, &pSGRptNode );
  }    

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  avlPurgeAvlTree(&pimsmCb->pimsmSGRptTree, mrtSGRptTableSize);
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "Failed to give semaphore "); 
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update (*,G) Tree when an interface is down
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param   rtrIfNum    @b{(input)} router Interface Number   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  pimsmSGRptTreeIntfDownUpdate(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmSGRptNode_t  sgRptNode, *pSGRptNode,*pSGRptNodeDelete = L7_NULLPTR;
  pimsmSGRptEntry_t *pSGRptEntry;
  L7_RC_t rc;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr; 
 pimsmSGRptIEntry_t *pSGRptIEntry = L7_NULLPTR;
 
  memset(&sgRptNode, 0, sizeof(pimsmSGRptNode_t));
  pSGRptNode = &sgRptNode;
  rc = pimsmSGRptFirstGet(pimsmCb, &pSGRptNode);
  while(rc == L7_SUCCESS)
  {
    pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
    if(pSGRptEntry->pimsmSGRptRPFIfIndex == rtrIfNum)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"Disabled interface is RPF interface");
      pSGRptNodeDelete = pSGRptNode;
    }
    else
    {
      pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
      if (pSGRptIEntry != L7_NULLPTR)
      {    
        if (pimsmSGRptIEntryDelete(pimsmCb, pSGRptNode, rtrIfNum) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"Failed to delete (S,G,Rpt,%d) entry",rtrIfNum);
        }
  
        /* Update to MFC with the new OIF List.
         */
        if (pimsmSGRptMFCUpdate (pimsmCb, pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Failed to update (S,G,Rpt) Entry to MFC");
        }
      }
    }

    rc = pimsmSGRptNextGet(pimsmCb, pSGRptNode, &pSGRptNode);
    if(pSGRptNodeDelete != L7_NULLPTR)
    {
      pSrcAddr= &(pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptSrcAddress);
      pGrpAddr= &(pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptGrpAddress);
      pimsmSGRptDelete(pimsmCb, pGrpAddr, pSrcAddr);
      pSGRptNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To include (S,G,Rpt) Prune state
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G,Rpt) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
4.5.8.  (S,G,rpt) Periodic Messages

When a router is going to send a Join(*,G), it should use the following
pseudocode, for each (S,G) for which it has state, to decide whether to
include a Prune(S,G,rpt) in the compound Join/Prune message:

  if( SPTbit(S,G) == TRUE ) {
      # Note: If receiving (S,G) on the SPT, we only prune off the
      # shared tree if the RPF neighbors differ.
       if( RPF'(*,G) != RPF'(S,G) ) {
           add Prune(S,G,rpt) to compound message
       }
  } else if ( inherited_olist(S,G,rpt) == NULL ) {
    #  Note: all (*,G) olist interfaces received RPT prunes for (S,G).
    add Prune(S,G,rpt) to compound message
  } else if ( RPF'(*,G) != RPF'(S,G,rpt) {
    # Note: we joined the shared tree, but there was an (S,G) assert and
    # the source tree RPF neighbor is different.
    add Prune(S,G,rpt) to compound message
  }

* @end
*********************************************************************/

#if 0
#define PIMSM_ADD_S_G_RPT_PRUNE(pSrcAddr)                                   \
        rtrIfNum = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;        \
        memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));               \
        jpData.rtrIfNum = rtrIfNum;                                         \
        jpData.pNbrAddr = pStarGNbrAddr;                                    \
        if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,            \
                                             &jpData.holdtime)              \
                                             != L7_SUCCESS)                 \
        {                                                                   \
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,                      \
            PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed"); \
          rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );                \
          continue;                                                         \
        }                                                                   \
        jpData.addrFlags =  PIMSM_ADDR_RP_BIT;                              \
        jpData.pSrcAddr = pSrcAddr;                                         \
        pimSrcMaskLenGet(pimsmCb->family,&maskLen);                       \
        jpData.srcAddrMaskLen = maskLen;                                    \
        jpData.pGrpAddr = pGrpAddr;                                         \
        pimGrpMaskLenGet(pimsmCb->family,&maskLen);                       \
        jpData.grpAddrMaskLen = maskLen;                                    \
        jpData.joinOrPruneFlag  = L7_FALSE;                                 \
        if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)              \
        {                                                                   \
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,           \
            "Failed to send Join/Prune Message");                           \
        }                                             
#endif

static L7_RC_t
pimsmSGRptPruneFromSGAdd (pimsmCB_t *pimsmCb,
                          pimsmSGNode_t *pSGNode,
                          L7_uint32 pimsmStarGRPFIfIndex,
                          L7_inet_addr_t *pStarGNbrAddr,
                          pimsmSendJoinPruneData_t *jpData)
{
  L7_uchar8 maskLen = 0;

  if ((pimsmCb == L7_NULLPTR) || (pSGNode == L7_NULLPTR) ||
      (pStarGNbrAddr == L7_NULLPTR) || (jpData == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memset(jpData, 0, sizeof(pimsmSendJoinPruneData_t));

  jpData->rtrIfNum = pimsmStarGRPFIfIndex;
  jpData->pNbrAddr = pStarGNbrAddr;
  if (pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, pimsmStarGRPFIfIndex,
                                     &jpData->holdtime) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to get the holdtime for rtrIfNum - %d", pimsmStarGRPFIfIndex);
    jpData->holdtime = (35 * PIMSM_DEFAULT_JOIN_PRUNE_PERIOD)/10;
  }
  jpData->addrFlags =  PIMSM_ADDR_RP_BIT;

  jpData->pSrcAddr = &pSGNode->pimsmSGEntry.pimsmSGSrcAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData->srcAddrMaskLen = maskLen;

  jpData->pGrpAddr = &pSGNode->pimsmSGEntry.pimsmSGGrpAddress;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData->grpAddrMaskLen = maskLen;

  jpData->joinOrPruneFlag  = L7_FALSE;
  if(pimsmJoinPruneSend(pimsmCb, jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
      "Failed to send Join/Prune Message");
  }

  return L7_SUCCESS;
}


static L7_RC_t
pimsmSGRptPruneFromSGRptAdd (pimsmCB_t *pimsmCb,
                             pimsmSGRptNode_t *pSGRptNode,
                             L7_uint32 pimsmStarGRPFIfIndex,
                             L7_inet_addr_t *pStarGNbrAddr,
                             pimsmSendJoinPruneData_t *jpData)
{
  L7_uchar8 maskLen = 0;

  if ((pimsmCb == L7_NULLPTR) || (pSGRptNode == L7_NULLPTR) ||
      (pStarGNbrAddr == L7_NULLPTR) || (jpData == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memset(jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData->rtrIfNum = pimsmStarGRPFIfIndex;
  jpData->pNbrAddr = pStarGNbrAddr;
  if (pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, pimsmStarGRPFIfIndex,
                                     &jpData->holdtime) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to get the holdtime for rtrIfNum - %d", pimsmStarGRPFIfIndex);
    jpData->holdtime = (35 * PIMSM_DEFAULT_JOIN_PRUNE_PERIOD)/10;
  }
  jpData->addrFlags =  PIMSM_ADDR_RP_BIT;

  jpData->pSrcAddr = &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData->srcAddrMaskLen = maskLen;

  jpData->pGrpAddr = &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData->grpAddrMaskLen = maskLen;

  jpData->joinOrPruneFlag  = L7_FALSE;
  if(pimsmJoinPruneSend(pimsmCb, jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
      "Failed to send Join/Prune Message");
  }

  return L7_SUCCESS;
}

        
L7_RC_t pimsmSGRptIncludePruneInMsg(pimsmCB_t        *pimsmCb,
                                    pimsmStarGNode_t *pStarGNode)
{
  pimsmSGNode_t     * pSGNode;
  L7_RC_t             rc;
  L7_inet_addr_t  *pStarGNbrAddr, *pSGNbrAddr, *pGrpAddr, *pSGRptNbrAddr;
  pimsmSGRptNode_t *    pSGRptNode= L7_NULLPTR;
  interface_bitset_t  sgRptInhrtdOif;
  L7_BOOL   oifNull;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32         rtrIfNum;
  
  pGrpAddr = &(pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pStarGNbrAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor;
  rtrIfNum = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "pStarGNbrAddr :",pStarGNbrAddr);

  rc = pimsmSGFirstGet( pimsmCb, &pSGNode);
  while(rc == L7_SUCCESS)
  {
    if (pSGNode == L7_NULLPTR)
    {
      break;
    }
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,&(pSGNode->pimsmSGEntry.pimsmSGGrpAddress)) == L7_TRUE)
    {
      if(pSGNode->pimsmSGEntry.pimsmSGSPTBit == L7_TRUE)
      {
        pSGNbrAddr = &pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor;
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "pSGNbrAddr :",pSGNbrAddr);
        if(PIMSM_INET_IS_ADDR_EQUAL(pStarGNbrAddr, pSGNbrAddr) == L7_FALSE)
        {
          /* add Prune(S,G,rpt) to compound message */
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
            "SPT= TRUE & RPF'(*,G) != RPF'(S,G), sending (S,G,rpt) prune");
          pimsmSGRptPruneFromSGAdd (pimsmCb, pSGNode, rtrIfNum, pStarGNbrAddr,
                                    &jpData);
        }
      }
    }
    rc = pimsmSGNextGet( pimsmCb, pSGNode, &pSGNode );
  }

  rc = pimsmSGRptFirstGet( pimsmCb, &pSGRptNode);
  while(rc == L7_SUCCESS)
      {
    if (pSGRptNode == L7_NULLPTR)
        {
      break;
        }
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,&(pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress)) == L7_TRUE)
    {
        BITX_RESET_ALL(&sgRptInhrtdOif);
      pimsmSGRptInhrtdOlist(pimsmCb, pSGRptNode, &sgRptInhrtdOif);
        BITX_IS_EMPTY(&sgRptInhrtdOif, oifNull);
        if(oifNull == L7_TRUE)
        {
          /* add Prune(S,G,rpt) to compound message */
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
          "SPT= FALSE & inherited_olist(S,G,rpt) == NULL, sending (S,G,rpt) prune");
        pimsmSGRptPruneFromSGRptAdd (pimsmCb, pSGRptNode, rtrIfNum, pStarGNbrAddr,
                                     &jpData);
        }
        else
        {
          pSGRptNbrAddr = &pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamNeighbor;
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "sgRptNbrAddr :", pSGRptNbrAddr);
        if(PIMSM_INET_IS_ADDR_EQUAL(pStarGNbrAddr, pSGRptNbrAddr) == L7_FALSE)
          {
            /* add Prune(S,G,rpt) to compound message */
             PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
             "SPT= FALSE &  RPF'(*,G) != RPF'(S,G,rpt) , sending (S,G,rpt) prune");
          pimsmSGRptPruneFromSGRptAdd (pimsmCb, pSGRptNode, rtrIfNum, pStarGNbrAddr,
                                       &jpData);
          }
        }
      }
    rc = pimsmSGRptNextGet (pimsmCb, pSGRptNode, &pSGRptNode);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Update (*,G) Tree with RP change
*
* @param    pimsmCb       @b{(input)} PIM SM Control Block
* @param   pGrpAddr      @b{(input)} Group IP Addr    
* @param   grpPrefixLen  @b{(input)} Source IP Addr
* @param   pRpAddr       @b{(input)} RP IP Addr   
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
static
void pimsmSGRptTreeRpChngRemapGroup(pimsmCB_t        *pimsmCb,
                                    pimsmSGRptNode_t *pSGRptNode)
{
  L7_uint32 iif;

   iif = pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex;

   if (iif == PIMSM_REGISTER_INTF_NUM)
   {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
       "Incoming interface is Register interface ");  
      pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode,MFC_DELETE_ENTRY, L7_FALSE) ;
   }
   else
   {      
     pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode,MFC_UPDATE_ENTRY, L7_FALSE) ;
   }
}
/*********************************************************************
* @purpose Update (S,G,Rpt) tree with RPF interface change
*
* @param    pimsmCb             @b{(input)} PIM SM Control Block
* @param rtrIfNumRPFIndex    @b{(input)} new RPF Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
static void pimsmSGRptTreeRTOCommon(pimsmCB_t * pimsmCb, 
             pimsmSGRptNode_t *pSGRptNode)
{
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;

  /* Update the (S,G,rpt) entry with new RP and upstream neighbor address */      
  pimsmSGRptNeighborRpfDash(pimsmCb,pSGRptNode);
    /* TODO: Post event changes */
  memset(&upStrmSGRptEventInfo, 0, sizeof(pimsmUpStrmSGRptEventInfo_t));
  upStrmSGRptEventInfo.rtrIfNum =
  pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex;
  upStrmSGRptEventInfo.eventType =
       PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPF_CHANGED;
  pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);
  pimsmSGRptTreeRpChngRemapGroup(pimsmCb, pSGRptNode);        
 
}
            
L7_RC_t pimsmSGRptTreeBestRouteChngUpdate(pimsmCB_t *pimsmCb, 
                      L7_inet_addr_t    *pGrpAddr,
                      L7_uint32 rtrIfNumRPFIndex,
                      RTO_ROUTE_EVENT_t  status)
{
  L7_RC_t rc = L7_FAILURE;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  pimsmSGRptEntry_t *pSGRptEntry = L7_NULLPTR;

  if(L7_NULLPTR == pimsmCb || pGrpAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Invalid input parameter");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFirstGet(pimsmCb,&pSGRptNode);

  while(rc == L7_SUCCESS)
  {
    pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
    if (PIMSM_INET_IS_ADDR_EQUAL(&pSGRptEntry->pimsmSGRptGrpAddress, 
      pGrpAddr) == L7_TRUE)
    {    
      switch (status)
      {
        case RTO_DELETE_ROUTE:
          pimsmSGRptTreeRTOCommon(pimsmCb,pSGRptNode);
          if(pimsmSGRptIEntryDelete(pimsmCb,pSGRptNode,
                                 pSGRptEntry->pimsmSGRptRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                          "Failed to (S,G,%d) entry",
                          pSGRptEntry->pimsmSGRptRPFIfIndex);
              break;
          }
          pSGRptEntry->pimsmSGRptRPFIfIndex = MCAST_MAX_INTERFACES;
          /*pimsmSGRptDelete(pimsmCb, &pSGRptEntry->pimsmSGRptGrpAddress,
            &pSGRptEntry->pimsmSGRptSrcAddress);*/
          break;

        case RTO_CHANGE_ROUTE:
          if(pimsmSGRptIEntryDelete(pimsmCb,pSGRptNode,
                                 pSGRptEntry->pimsmSGRptRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                          "Failed to (S,G,%d) entry",
                          pSGRptEntry->pimsmSGRptRPFIfIndex);
              break;
          }
          /* fall-through for remaining work*/
        case RTO_ADD_ROUTE:
          if(pimsmSGRptIEntryCreate(pimsmCb,pSGRptNode,
                                    rtrIfNumRPFIndex) 
             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, 
                        "Failed to create (S,G,I) for upstream index %d",
                        rtrIfNumRPFIndex);
            break;
          }
         /*delete negative entries from MFC*/
          if((pSGRptNode->flags & PIMSM_NULL_OIF) != L7_NULL)
          {
            pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_DELETE_ENTRY, L7_FALSE);             
          }     
          if(pSGRptEntry->pimsmSGRptRPFIfIndex != rtrIfNumRPFIndex)
          {
            pimsmSGRptTreeRTOCommon(pimsmCb,pSGRptNode);         
            pSGRptEntry->pimsmSGRptRPFIfIndex = rtrIfNumRPFIndex;
          }
   
          
          break;          
        default:
          break;
      }
    }
    rc = pimsmSGRptNextGet(pimsmCb,pSGRptNode,&pSGRptNode);
  }

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose Process (S,G,Rpt) Entry
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pGrpAddr    @b{(input)} Group IP Addr 
* @param rtrIfNum    @b{(input)} router Interface Number
* @param holdtime    @b{(input)} Holdtime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmSGRptStarGJoinRecvUpdate(pimsmCB_t      *pimsmCb,
                                      L7_inet_addr_t *pGrpAddr,
                                      L7_uint32       rtrIfNum,
                                      L7_short16      holdtime)
{

  L7_RC_t rc = L7_FAILURE, rc1;
  pimsmSGRptNode_t *pSGRptNode= L7_NULLPTR;
  pimsmDnStrmPerIntfSGRptEventInfo_t dnStrmSGRptEventInfo; 

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFirstGet(pimsmCb,&pSGRptNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                             &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress) 
       == L7_TRUE)
    {
      rc1 = pimsmSGRptIEntryCreate(pimsmCb, pSGRptNode, rtrIfNum);
      if(rc1 != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, 
                    "Failed to create (S,G,Rpt,%d) entry", rtrIfNum);
        return L7_FAILURE;
      }
      memset(&dnStrmSGRptEventInfo, 0 , 
             sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));
      dnStrmSGRptEventInfo.eventType =
      PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_JOIN_STAR_G;
      dnStrmSGRptEventInfo.rtrIfNum  = rtrIfNum;
      dnStrmSGRptEventInfo.holdtime = holdtime;   
      pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode,&dnStrmSGRptEventInfo);
      if(pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
            "Failed to update (S,G,rpt) MFC exact");
        return L7_FAILURE;
      }      
    }
    rc = pimsmSGRptNextGet(pimsmCb,pSGRptNode,&pSGRptNode);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update MFC with the(S,G) Entry
*
* @param    pimsmCb  @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G) Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/

L7_RC_t pimsmSGRptMFCUpdate(pimsmCB_t     *pimsmCb,
                                pimsmSGRptNode_t *pSGRptNode,
                                mfcOperationType_t eventType,
                                L7_BOOL bForceUpdate)
{
  interface_bitset_t sgRptInhrtdOif;
  interface_bitset_t scopedOifList;
  pimsmSGNode_t * pSGNode;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pSGRptNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  memset(&sgRptInhrtdOif,0,sizeof(interface_bitset_t));
  if (eventType == MFC_UPDATE_ENTRY)
  {  
    if((pSGRptNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, 
          "Node is deleted");            
      return L7_FAILURE;
    }  
    if (pimsmSGFind(pimsmCb, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
          &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress, &pSGNode) == L7_SUCCESS)
    {
      if (pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                    "(S,G)MFC Updation failed");
        return L7_FAILURE;
      }
      /*pimsmSPTbitUpdate(pimsmCb,pSGNode,pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex);*/
      pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_TRUE;
      return L7_SUCCESS;      
    }
    if( (bForceUpdate == L7_FALSE) &&
        (pimsmCb->pimsmRestartInProgress == L7_FALSE) &&
        (((pSGRptNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO, 
          "Entry is not added in MFC");
      return L7_SUCCESS;
    }     
    pimsmSGRptInhrtdOlist(pimsmCb,pSGRptNode,&sgRptInhrtdOif);
    BITX_RESET(&sgRptInhrtdOif, 
        pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex);
    if(BITX_TEST(&sgRptInhrtdOif,PIMSM_REGISTER_INTF_NUM) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
            "Register interface in oif");
      return L7_SUCCESS;
    } 
    if (pimsmCb->family == L7_AF_INET)
    {    
        /* subtract all admin-scoped interfaces */
      BITX_RESET_ALL(&scopedOifList);
      if (mcastMapGroupScopedInterfacesGet(&(pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress),
                                           (L7_uchar8 *)&(scopedOifList)) == L7_SUCCESS)
      {
        if (BITX_TEST(&scopedOifList,pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex))
        {
          BITX_RESET_ALL(&sgRptInhrtdOif);
        }
        else
        {
          BITX_MASK_AND(&sgRptInhrtdOif, &scopedOifList, &sgRptInhrtdOif);
        }
      }
    }
  }
  else if (eventType == MFC_DELETE_ENTRY)
  {
    if(((pSGRptNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO, 
          "Entry is not present in MFC");
      return L7_SUCCESS;
    } 
  }
  PIMSM_TRACE_BITX(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, &sgRptInhrtdOif );    
  if(pimsmMfcQueue(pimsmCb,eventType,
                   &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress,
                   &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress,
                   pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex,
                   &sgRptInhrtdOif, L7_TRUE, &pSGRptNode->flags) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"failed to update MFC");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Test whether  mrt entry from the (S,G) entry table can be deleted
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimsmSGRptNodeCanRemove(pimsmCB_t * pimsmCb, pimsmSGRptNode_t *pSGRptNode)
{

  pimsmSGRptEntry_t  *pSGRptEntry;
  pimsmSGRptIEntry_t  *pSGRptIEntry;  
  L7_uint32 index;
  pimsmSGNode_t *pSGNode =L7_NULLPTR;
  L7_RC_t rc;

  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
  rc = pimsmSGFind(pimsmCb, &pSGRptEntry->pimsmSGRptGrpAddress,
                   &pSGRptEntry->pimsmSGRptSrcAddress, &pSGNode);
  if (rc == L7_SUCCESS)
  {
     if((pSGNode->flags & PIMSM_NODE_DELETE) == L7_NULL)
     {
         return L7_FALSE;
     }      
  }
  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
     pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[index];
     if ( pSGRptIEntry != L7_NULLPTR)
     {
        if((pSGRptIEntry->pimsmSGRptIJoinPruneState !=
              PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO) ||
           (pSGRptIEntry->pimsmSGRptIPrunePendingTimer != L7_NULLPTR) || 
           (pSGRptIEntry->pimsmSGRptIPruneExpiryTimer != L7_NULLPTR))
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
            "(S,G, rpt, %d) is active", index);        
          return L7_FALSE; 
        }
     }
  }

  if((pSGRptEntry->pimsmSGRptUpstreamPruneState !=
        pSGRptEntry->pimsmSGRptUpstreamPruneInitState) ||
     (pSGRptEntry->pimsmSGRptUpstreamOverrideTimer != L7_NULLPTR))
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
            "(S,G, rpt) is active");
    return L7_FALSE; 
  } 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, 
            "(S,G, rpt) is in-active");        
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT,  PIMSM_TRACE_NORMAL, "Group Address :",
                                      &pSGRptEntry->pimsmSGRptGrpAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT,  PIMSM_TRACE_NORMAL, "Source Address :", 
                                      &pSGRptEntry->pimsmSGRptSrcAddress);
  
  return L7_TRUE;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL pimsmSGRptNodeTryRemove(pimsmCB_t * pimsmCb, pimsmSGRptNode_t *pSGRptNode)
{
  L7_BOOL retVal;
  retVal = pimsmSGRptNodeCanRemove(pimsmCb, pSGRptNode);
  if(retVal == L7_TRUE) 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, 
        "SGRptNodeCanRemove = %d", retVal);
    pSGRptNode->flags |=  PIMSM_NODE_DELETE;
  }
  return retVal;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pSGRptNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void pimsmSGRptNodeCleanup(pimsmCB_t * pimsmCb)
{
  pimsmSGRptNode_t *pSGRptNode, *pSGRptNodeDelete = L7_NULLPTR;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pimsmSGRptNode_t  sGRptNodeSearch;

  /* Set all elements of the node to zero */
  memset (&sGRptNodeSearch, 0, sizeof (pimsmSGRptNode_t));
  inetAddressZeroSet(pimsmCb->family,
        &sGRptNodeSearch.pimsmSGRptEntry.pimsmSGRptGrpAddress);
  inetAddressZeroSet(pimsmCb->family,
        &sGRptNodeSearch.pimsmSGRptEntry.pimsmSGRptSrcAddress);  
  
  pSGRptNode = avlSearchLVL7(&pimsmCb->pimsmSGRptTree, 
            &sGRptNodeSearch, AVL_NEXT);
  
  while(pSGRptNode != L7_NULLPTR)
  {
    if((pSGRptNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
     pSGRptNodeDelete =  pSGRptNode;
    }
     pSGRptNode = avlSearchLVL7(&pimsmCb->pimsmSGRptTree, 
            pSGRptNode, AVL_NEXT);    
    if(pSGRptNodeDelete != L7_NULLPTR)
    {
      pGrpAddr= &(pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptGrpAddress);
      pSrcAddr= &(pSGRptNodeDelete->pimsmSGRptEntry.pimsmSGRptSrcAddress);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL,
              "(S,G,rpt) is deleted ");
      pimsmSGRptDelete(pimsmCb, pGrpAddr, pSrcAddr);
      pSGRptNodeDelete = L7_NULLPTR;
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
L7_RC_t pimsmSGRptNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pGrpAddr)
{
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFirstGet(pimsmCb,&pSGRptNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                             &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress) 
       == L7_TRUE)
    {
      if((pSGRptNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_DELETE_ENTRY, L7_FALSE);
      }
    }
    rc = pimsmSGRptNextGet(pimsmCb,pSGRptNode,&pSGRptNode);
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
L7_RC_t pimsmSGRptAdminScopeUpdate(pimsmCB_t *pimsmCb)
{
  pimsmSGRptNode_t *pSGRptNode= L7_NULLPTR;
  L7_RC_t rc;

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFirstGet(pimsmCb, &pSGRptNode);
  while (rc == L7_SUCCESS)
  {
    pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode , MFC_UPDATE_ENTRY, L7_FALSE);
    rc = pimsmSGRptNextGet(pimsmCb, pSGRptNode, &pSGRptNode);
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
L7_RC_t pimsmSGRptTreeRPChngUpdate(pimsmCB_t       *pimsmCb, 
                               L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pGrpAddr)
{
  pimsmSGRptNode_t *pSGRptNode= L7_NULLPTR;
  pimsmSGRptEntry_t *pSGRptEntry; 
  pimsmSGRptIEntry_t *pSGRptIEntry; 
  pimsmStarStarRpNode_t *pStarStarRpNode = L7_NULLPTR;
  L7_uint32  rpRPFIndex = L7_NULL;
  interface_bitset_t  pSGRptInhrtdOif; 
  L7_BOOL bNullOif;
  L7_RC_t rc;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  pimsmStarStarRpEntry_t *pStarStarRpEntry = L7_NULLPTR;
  L7_inet_addr_t rpAddr;
  L7_uint32 i;
  
  /* RP changed should be applied irrespective of spt-bit value 
  see ANVL 11.9 */ 
    rc = pimsmSGRptFind(pimsmCb,pGrpAddr,pSrcAddr,&pSGRptNode);
    if (rc == L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"(S,G,Rpt) entry present ");
      pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
  
     /* Do this to get actual RP for that particular group(G) in (*,G) ,
        as the RP change for an MRT entry cannot be detected at RP code 
        because only the group range is known at RP code level which is 
        not sometimes not suffient to detect an RP change for MRT entries */
      if(pimsmRpAddressGet(pimsmCb, pGrpAddr,
                          &rpAddr) != L7_SUCCESS)
      {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
        "No RP entry present ");
       return L7_FAILURE;
      }
    
      if((pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode) == L7_SUCCESS) &&
         (pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex
           != MCAST_MAX_INTERFACES))
      {
        pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;            
        rpRPFIndex = pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
        
        if (pSGRptEntry->pimsmSGRptRPFIfIndex != rpRPFIndex)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "RPF chnged to %d",
                             rpRPFIndex);
           /* Delete all old oif (S,G,rpt, I) entries */  
          pimsmSGRptInhrtdOlist(pimsmCb, pSGRptNode, &pSGRptInhrtdOif);
          BITX_IS_EMPTY( &pSGRptInhrtdOif, bNullOif );
          if (bNullOif == L7_FALSE)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"(S,G,Rpt) oif is NULL");
            for (i = 0; i < MCAST_MAX_INTERFACES;i++)
            {
              if (BITX_TEST(&pSGRptInhrtdOif, i) != L7_NULL)
              {
                pSGRptIEntry =
                pSGRptNode->pimsmSGRptIEntry[i];
                if (pSGRptIEntry->pimsmSGRptILocalMembership == L7_TRUE)
                {
                  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
                          "Local membership present on rtrIfNum = %d",i);
                  continue;
                }
                PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
                      " Deleting the (S,G,rpt,I) for rtrIfNum = %d",i);
                pimsmSGRptIEntryDelete(pimsmCb,  pSGRptNode, i);
              }
            }
          }
          else
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"(S,G,Rpt) oif is NULL");
          }
         /* Delete (S,G,rpt,I) for old RPF index and create one for new RPF index */
          pimsmSGRptIEntryDelete(pimsmCb, pSGRptNode,
                             pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex);
  
          PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"Creating new RPF index : %d",
                     rpRPFIndex);
  
          if (pimsmSGRptIEntryCreate(pimsmCb, pSGRptNode, 
                                     rpRPFIndex) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                        "Failed to create (S,G,rpt,I) for upstream index %d",
                        rpRPFIndex);
            return L7_FAILURE;
          }  
          pSGRptEntry->pimsmSGRptRPFIfIndex = rpRPFIndex;
          
          /* Update the (S,G,rpt) entry with new RP and upstream neighbor address */      
          pimsmSGRptNeighborRpfDash(pimsmCb,pSGRptNode);
  
         /* You are going from Non-RP to RP (or)
          * Some RP has changed. */
         /* Send a Prune towards the old RP and a Join towards the
          * new RP.
          */
         memset(&upStrmSGRptEventInfo, 0, sizeof(pimsmUpStrmSGRptEventInfo_t));
         upStrmSGRptEventInfo.rtrIfNum =
         pSGRptEntry->pimsmSGRptRPFIfIndex;
         upStrmSGRptEventInfo.eventType =
              PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPF_CHANGED;
         pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);
         pimsmSGRptTreeRpChngRemapGroup(pimsmCb, pSGRptNode);                
        }
      }
      else
      {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, 
        "No RP entry present ");
       return L7_FAILURE;
      }
   }
  return L7_SUCCESS;
}

