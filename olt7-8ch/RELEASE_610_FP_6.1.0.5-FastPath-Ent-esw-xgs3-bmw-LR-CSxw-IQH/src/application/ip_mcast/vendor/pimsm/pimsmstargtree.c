/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarGTree.c
*
* @purpose Contains PIM-SM  avltree function definitions for (*,G) entry
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
#include "pimsmstargtree.h"
#include "avl_api.h"
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmmacros.h"
#include "pimsmtimer.h"
#include "pimsmcache.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"

/*********************************************************************
* @purpose  Compares the given (*,G) nodes
*
* @param pimsmStarGNode1  @b{(input)} (*,G) Node
* @param pimsmStarGNode2  @b{(input)} (*,G) Node
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
static L7_int32 pimsmStarGCompare(const void * pNode1, 
                                  const void * pNode2,
                                  L7_uint32 keySize)
{
  pimsmStarGNode_t * pStarGNode1, *pStarGNode2;
  L7_inet_addr_t  * pGrpAddr1, * pGrpAddr2;
  L7_uint32   result;

  pStarGNode1 = (pimsmStarGNode_t *)pNode1;
  pStarGNode2 = (pimsmStarGNode_t *)pNode2;

  pGrpAddr1 = &pStarGNode1->pimsmStarGEntry.pimsmStarGGrpAddress;
  pGrpAddr2 = &pStarGNode2->pimsmStarGEntry.pimsmStarGGrpAddress;

  result = L7_INET_ADDR_COMPARE(pGrpAddr1, pGrpAddr2);
  return result;
}
/*********************************************************************
* @purpose  Initializes the (*,G) entry table
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
L7_RC_t pimsmStarGInit(pimsmCB_t * pimsmCb)
{
  L7_uint32 mrtStarGTableSize = 0;

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  pimsmCb->pimsmStarGTreeHeap = PIMSM_ALLOC (pimsmCb->family, 
          (mrtStarGTableSize * sizeof (avlTreeTables_t)));
  if(pimsmCb->pimsmStarGTreeHeap == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (*,G) Table Tree Heap\n");
    return L7_FAILURE;
  }
  pimsmCb->pimsmStarGDataHeap =  PIMSM_ALLOC (pimsmCb->family, 
           (mrtStarGTableSize * sizeof (pimsmStarGNode_t)));
  if(pimsmCb->pimsmStarGDataHeap == L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmStarGTreeHeap);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "PIMSM: Memory allocation failed for (*,G) Table Data Heap\n");
    return L7_FAILURE;
  }

  /* Cleanup data structures before proceeding */
  memset (pimsmCb->pimsmStarGTreeHeap, 0, 
          mrtStarGTableSize * sizeof (avlTreeTables_t));
  memset (pimsmCb->pimsmStarGDataHeap, 0, 
          mrtStarGTableSize * sizeof (pimsmStarGNode_t));

  /* The destination networks are stored in an AVL tree.
  */
  avlCreateAvlTree(&pimsmCb->pimsmStarGTree, pimsmCb->pimsmStarGTreeHeap,
                   pimsmCb->pimsmStarGDataHeap, mrtStarGTableSize,
                   (L7_uint32)(sizeof(pimsmStarGNode_t)), PIMSM_AVL_TREE_TYPE, 
                   sizeof(L7_inet_addr_t *) * 2);

  /* TODO - define Comparator */
  (void)avlSetAvlTreeComparator(&pimsmCb->pimsmStarGTree, pimsmStarGCompare); 

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Frees the (*,G) entry table
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
L7_RC_t pimsmStarGDeInit(pimsmCB_t * pimsmCb)
{

  if(pimsmCb->pimsmStarGTreeHeap  != (avlTreeTables_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmStarGTreeHeap);
    pimsmCb->pimsmStarGTreeHeap = L7_NULLPTR;
  }
  if(pimsmCb->pimsmStarGDataHeap  != (pimsmStarGNode_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pimsmCb->pimsmStarGDataHeap);
    pimsmCb->pimsmStarGDataHeap = L7_NULLPTR; 
  }

  /* The destination networks are stored in an AVL tree.
  */
  avlDeleteAvlTree(&pimsmCb->pimsmStarGTree);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a mrt entry to the (*,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (*,G) Node  
* @param pSrcAddr    @b{(input)} Source IP Addr   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarGAdd(pimsmCB_t        *pimsmCb,
                             pimsmStarGNode_t *pStarGNode, 
                             L7_inet_addr_t   *pSrcAddr)
{
  pimsmStarGNode_t *pStarGNodeTemp = L7_NULLPTR;
  L7_uint32 mrtStarGTableSize = 0;

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  if(avlTreeCount(&pimsmCb->pimsmStarGTree) >= mrtStarGTableSize)
  {
    osapiSemaGive(pimsmCb->pimsmStarGTree.semId) ;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_PIMSM_MAP_COMPONENT_ID, "PIMSM (*,G) Table Max Limit - [%d] "
			 "Reached; Cannot accomodate any further routes. PIMSM Multicast Route table (*,G) has reached "
			 "maximum capacity and cannnot accommodate new registrations anymore.",
             mrtStarGTableSize);
    return L7_FAILURE;
  }

  pStarGNodeTemp = avlInsertEntry(&pimsmCb->pimsmStarGTree, pStarGNode);
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to give semaphore "); 
    return L7_FAILURE;
  }
  if(pStarGNodeTemp != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL,
                "Failed to add to the (*,G) node");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a mrt entry from the (*,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pGrpAddr    @b{(input)} Group IP Addr  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGDelete(pimsmCB_t * pimsmCb, L7_inet_addr_t *pGrpAddr)
{
  pimsmStarGNode_t starGNodeSearch;
  pimsmStarGNode_t *pStarGNode =L7_NULLPTR,*pStarGNodeDelete =L7_NULLPTR;
  L7_uint32       index;

  if((pimsmCb == L7_NULLPTR)||
     (pGrpAddr == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                "invalid input parameters ");
    return L7_FAILURE;    
  }  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
                "Entry Success");
  
  /* Cleanup the memory */
  memset (&starGNodeSearch, 0, sizeof (pimsmStarGNode_t));

  inetCopy(&starGNodeSearch.pimsmStarGEntry.pimsmStarGGrpAddress,pGrpAddr);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pStarGNode = avlSearchLVL7(&pimsmCb->pimsmStarGTree, &starGNodeSearch, AVL_EXACT);
  if(pStarGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "Failed to delete (*,G) node");
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinTimerHandle);

  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
    if(pStarGNode->pimsmStarGIEntry[index] != L7_NULLPTR)
    {
      pimsmStarGIEntryDelete(pimsmCb, pStarGNode,index);
    }
  }
  pimsmStarGMFCUpdate(pimsmCb,pStarGNode,MFC_DELETE_ENTRY, L7_FALSE);
  if (&(pStarGNode->pimsmStarGCacheList) != L7_NULLPTR)
  {
    pimsmStarGCacheDeInit(pimsmCb,pStarGNode);
  }
  pStarGNodeDelete = avlDeleteEntry(&pimsmCb->pimsmStarGTree, pStarGNode);
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  if(pStarGNodeDelete == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "Delete from (*,G)Tree failed! No entry found. \n ");
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify a mrt entry in the (*,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (*,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGModify(pimsmCB_t * pimsmCb, pimsmStarGNode_t *pStarGNode)
{
  MCAST_UNUSED_PARAM(pStarGNode);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Check whether the (*,G) entry exists in the (*,G) entry table
*
* @param    pimsmCb      @b{(input)} PIM SM Control Block
* @param ppStarGNode  @b{(input)} (*,G) Node to Search
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGFirstGet(pimsmCB_t * pimsmCb, pimsmStarGNode_t **ppStarGNode)
{
  pimsmStarGNode_t  starGNode;

  /* Set all elements of the node to zero */
  memset (&starGNode, 0, sizeof (pimsmStarGNode_t));
  inetAddressZeroSet(pimsmCb->family,
          &starGNode.pimsmStarGEntry.pimsmStarGGrpAddress);  
    return pimsmStarGEntryFind(pimsmCb, &starGNode, AVL_NEXT, ppStarGNode);
}


/*********************************************************************
* @purpose  Get the next (*,G) entry after the specified (*,G) entry 
*           in the (*,G) entry table
*
* @param    pimsmCb            @b{(input)} PIM SM Control Block
* @param pStarGNodeCurrent  @b{(input)} current (*,G) Node 
* @param pStarGNode         @b{(input)} (*,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGNextGet(pimsmCB_t         *pimsmCb,
                          pimsmStarGNode_t  *pStarGNodeCurrent, 
                          pimsmStarGNode_t **ppStarGNode)
{

  return pimsmStarGEntryFind(pimsmCb, pStarGNodeCurrent, AVL_NEXT, ppStarGNode);
}


/*********************************************************************
* @purpose  Find (*,G) node in the (*,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pGrpAddr    @b{(input)} Group IP Addr 
* @param pStarGNode  @b{(input)} (*,G) Node with G as given group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGFind(pimsmCB_t         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,  
                       pimsmStarGNode_t **ppStarGNode)
{
  pimsmStarGNode_t starGNode;
  L7_RC_t rc;

  if((pimsmCb == L7_NULLPTR)||
     (pGrpAddr == L7_NULLPTR) ||
     (ppStarGNode == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                "invalid input parameters ");
    return L7_FAILURE;    
  } 
  memset (&starGNode, 0, sizeof (pimsmStarGNode_t));

  inetCopy(&starGNode.pimsmStarGEntry.pimsmStarGGrpAddress, pGrpAddr);
  rc = pimsmStarGEntryFind(pimsmCb, &starGNode, AVL_EXACT, ppStarGNode);

  return rc;
  
}

/*********************************************************************
* @purpose  Find (*,G) node in the (*,G) entry table
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pGrpAddr    @b{(input)} Group IP Addr 
* @param pStarGNode  @b{(input)} (*,G) Node with G as given group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarGFindExact(pimsmCB_t         *pimsmCb,
                       L7_inet_addr_t    *pGrpAddr,  
                       pimsmStarGNode_t **ppStarGNode)
{
  pimsmStarGNode_t starGNode;
  pimsmStarGNode_t *pStarGNodeTemp;
  
  memset (&starGNode, 0, sizeof (pimsmStarGNode_t));

  inetCopy(&starGNode.pimsmStarGEntry.pimsmStarGGrpAddress, pGrpAddr);
  pStarGNodeTemp = avlSearchLVL7(&pimsmCb->pimsmStarGTree, 
                                 &starGNode, AVL_EXACT);

    if (pStarGNodeTemp == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_DEBUG,
                "Failed to find (*,G) node");
    *ppStarGNode = L7_NULLPTR;                                
    return L7_FAILURE;
  }
  *ppStarGNode = pStarGNodeTemp;
 
  return L7_SUCCESS;  
}
/*********************************************************************
* @purpose  Find (*,G) node in the (*,G) entry table
*
* @param    pimsmCb            @b{(input)} PIM SM Control Block
* @param pStarGNodeCurrent  @b{(input)} current (*,G) Node 
* @param pStarGNode         @b{(input)} (*,G) Node
* @param flag               @b{(input)} AVL_EXACT or AVL_NEXT 
* @param   pStarGNode         @b{(input)} (*,G) Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGEntryFind(pimsmCB_t        *pimsmCb, 
                            pimsmStarGNode_t *starGNodeCurrent,
                            L7_uint32         flag,
                            pimsmStarGNode_t **ppStarGNode)
{
  pimsmStarGNode_t *pStarGNodeTemp;

  if ((starGNodeCurrent == (pimsmStarGNode_t *)L7_NULLPTR) || 
      (ppStarGNode == (pimsmStarGNode_t **)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                    "invalid input parameters ");
    return L7_FAILURE;
  }
  *ppStarGNode = L7_NULLPTR;
    pStarGNodeTemp = avlSearchLVL7(&pimsmCb->pimsmStarGTree, 
                                 starGNodeCurrent, flag);

    if (pStarGNodeTemp == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_DEBUG,
                "Failed to find (*,G) node");
    *ppStarGNode = L7_NULLPTR;                                
    return L7_FAILURE;
  }

  if((pStarGNodeTemp->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    *ppStarGNode = L7_NULLPTR;   
    return L7_FAILURE;
  } 
  *ppStarGNode = pStarGNodeTemp;
 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the (*,G) entry table
*
* @param    pimsmCb      @b{(input)} PIM SM Control Block
* @param pGrpAddr     @b{(input)} Group IP Addr  
* @param ppStarGNode  @b{(input)} New (*,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGNodeCreate(pimsmCB_t * pimsmCb,
                             L7_inet_addr_t * pGrpAddr,
                             pimsmStarGNode_t **ppStarGNode)
{
  pimsmStarGEntry_t   * pStarGEntry;
  L7_inet_addr_t      rpAddr;
  pimsmStarGNode_t    *pStarGNode = L7_NULLPTR, starGNode;
  pimsmStarStarRpNode_t *pStarStarRpNode = L7_NULLPTR;
  pimsmStarStarRpEntry_t *pStarStarRpEntry = L7_NULLPTR;
  L7_BOOL   result = L7_FALSE;
  L7_uint32 ii=0; 
  interface_bitset_t  starGImdtOif;
  L7_RC_t  rc;
  L7_BOOL retVal;


  if((pGrpAddr == (L7_inet_addr_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "invalid input parameters");
    return L7_FAILURE;
  }

  /*  Don't create (*,G) for SSM range*/
  retVal = pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr);
  if(retVal == L7_TRUE)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Group is in SSM range", pGrpAddr);
    return L7_FAILURE;
  }
  rc = pimsmStarGFindExact(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "Creating new (*,G) Node");    
    memset(&starGNode, 0 ,sizeof(pimsmStarGNode_t));
    pStarGEntry = &starGNode.pimsmStarGEntry;
    inetAddressZeroSet(pimsmCb->family, 
        &pStarGEntry->pimsmStarGRPFNextHop);
    inetAddressZeroSet(pimsmCb->family, 
        &pStarGEntry->pimsmStarGRPFRouteAddress);
    inetAddressZeroSet(pimsmCb->family, 
        &pStarGEntry->pimsmStarGUpstreamNeighbor);

    inetCopy(&pStarGEntry->pimsmStarGGrpAddress, pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "GrpAddr ",
                     &pStarGEntry->pimsmStarGGrpAddress);
    if (pimsmRpAddressGet(pimsmCb, pGrpAddr,  &rpAddr) != L7_SUCCESS)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G,  PIMSM_TRACE_ERROR, "Failed t ofind RP for group",pGrpAddr);
      return L7_FAILURE;    
    }           
    inetCopy(&pStarGEntry->pimsmStarGRPAddress, &rpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "RP Address : ",
                     &pStarGEntry->pimsmStarGRPAddress); 

    if((pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode) == L7_SUCCESS) &&
       (pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex != MCAST_MAX_INTERFACES))
    {
      pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry; 
      pStarGEntry->pimsmStarGRPFIfIndex = pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "The RPFIfIndex is %d", 
                  pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
      inetCopy(&pStarGEntry->pimsmStarGRPFNextHop, &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "The Next hop is", 
                       &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
      inetCopy(&pStarGEntry->pimsmStarGRPFRouteAddress,
               &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "The RPF Route Addr is ", 
                       &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress)

      pStarGEntry->pimsmStarGRPFRouteMetric = pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"The RPFRouteMetric is %d",
                  pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric);        
      pStarGEntry->pimsmStarGRPFRouteMetricPref 
      = pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"The RPFRouteMetricPref is %d", 
                  pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref);                                              
      pStarGEntry->pimsmStarGRPFRouteProtocol = pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"The RPFRouteProtocol is %d", 
                  pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol);
      pStarGEntry->pimsmStarGRPFRoutePrefixLength = 
      pStarStarRpEntry->pimsmStarStarRpRPFRoutePrefixLength; 
    }
    else
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to get RPF info(from (*,*,RP)) for RP :",&rpAddr);
      return L7_FAILURE;
    }
    pStarGEntry->pimsmStarGRPIsLocal = 
    mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR);
                   
    if(pimsmStarGAdd(pimsmCb, &starGNode, L7_NULLPTR) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to add (*,G) node");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G,  PIMSM_TRACE_INFO, "Group Address :",
                        &pStarGEntry->pimsmStarGGrpAddress);
      return L7_FAILURE; 
    }
    rc = pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode);
    if(rc == L7_SUCCESS)
    {
     /* pStarGNode->pSelf = pStarGNode;*/
      pStarGEntry = &pStarGNode->pimsmStarGEntry;
      pStarGEntry->pimsmStarGUpstreamJoinTimerHandle = 
      handleListNodeStore(pimsmCb->handleList,
                          (void*)&pStarGEntry->pimsmStarGUpstreamJoinTimerParam); 
      pStarGEntry->pimsmStarGUpstreamJoinState = PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED;
            
      if(pimsmStarGIEntryCreate(pimsmCb,pStarGNode,
                                pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex) 
         != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                    "Failed to create (*,G,I) for upstream index %d",
                    pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex);
        pimsmStarGDelete(pimsmCb,pGrpAddr);
        return L7_FAILURE;
      }
      memset(&starGImdtOif,0,sizeof(interface_bitset_t));
      pimsmStarGImdtOlist(pimsmCb,pStarGNode,&starGImdtOif);

      BITX_IS_EMPTY(&starGImdtOif,result);

      if(result != L7_TRUE)
      {
        for(ii =1;ii < MCAST_MAX_INTERFACES;ii++)
        {
          if(BITX_TEST(&starGImdtOif,ii) != L7_NULL)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
                        "Bit set for interface index %d",ii);

            if(pimsmStarGIEntryCreate(pimsmCb,pStarGNode,ii) 
               != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                          "Failed to create (*,G,%d) entry ",ii);
            }
          }
        }/*end of for loop */
      }

      pimsmStarGNeighborRpfDash(pimsmCb,pStarGNode);
      pStarGEntry->pimsmStarGCreateTime =  osapiUpTimeRaw();
      if(pimsmStarGCacheInit(pimsmCb, pStarGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to initialize Kernel Cache");
        pimsmStarGDelete(pimsmCb,pGrpAddr);
        return L7_FAILURE;
      }
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "New (*,G) Node was not found ");    
    }
  }
  pStarGNode->flags &=  ~PIMSM_NODE_DELETE;  
  *ppStarGNode = pStarGNode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create (*,G,I) Entry and init with default params
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (*,G) Node
* @param rtrIfNum    @b{(input)} Interface Number    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGIEntryCreate(pimsmCB_t        *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                               L7_uint32         rtrIfNum)
{
  pimsmStarGIEntry_t  * pStarGIEntryNew, *pStarGIEntry;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pStarGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == (pimsmStarGIEntry_t *)L7_NULLPTR)
  {
    if ((pStarGIEntryNew = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmStarGIEntry_t)))
                                     == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (*,G) interface pool");    
      return L7_FAILURE;
    } 
    memset(pStarGIEntryNew, 0, sizeof(pimsmStarGIEntry_t));
    inetAddressZeroSet(pimsmCb ->family, 
                     &pStarGIEntryNew->pimsmStarGIAssertWinnerAddress);
    pStarGIEntryNew->pimsmStarGIIfIndex= rtrIfNum;
    pStarGIEntryNew->pimsmStarGIPrunePendingTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pStarGIEntryNew->pimsmStarGIPrunePendingTimerParam); 
    pStarGIEntryNew->pimsmStarGIJoinExpiryTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pStarGIEntryNew->pimsmStarGIJoinExpiryTimerParam); 
    pStarGIEntryNew->pimsmStarGIAssertTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pStarGIEntryNew->pimsmStarGIAssertTimerParam); 

    pStarGIEntryNew->pimsmStarGILocalMembership= L7_FALSE;
    pStarGIEntryNew->pimsmStarGIAssertState 
    = PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO;
    pStarGIEntryNew->pimsmStarGIJoinPruneState 
    = PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO;
    if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex == rtrIfNum)
    {
      pStarGIEntryNew->pimsmStarGIAssertWinnerMetric = 
      pStarGNode->pimsmStarGEntry.pimsmStarGRPFRouteMetric;
      pStarGIEntryNew->pimsmStarGIAssertWinnerMetricPref = 
      pStarGNode->pimsmStarGEntry.pimsmStarGRPFRouteMetricPref;
      pStarGIEntryNew->pimsmStarGIAssertWinnerMetricPref |= PIMSM_ASSERT_RPT_BIT;
      inetAddressZeroSet(pimsmCb ->family, 
                         &pStarGIEntryNew->pimsmStarGIAssertWinnerAddress);
    }
    pStarGIEntryNew->pimsmStarGICreateTime = osapiUpTimeRaw();
    pStarGNode->pimsmStarGIEntry[rtrIfNum] = pStarGIEntryNew;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete (*,G,I) Entry 
*
* @param   pimsmCb           @b{(input)} PIM SM Control block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGIEntryDelete(pimsmCB_t        *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                               L7_uint32         rtrIfNum)
{
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
     
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pStarGNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];

  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_DEBUG,
            " (*,G,%d) already deleted ",rtrIfNum);
   return L7_SUCCESS;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGIEntry->pimsmStarGIPrunePendingTimer));
  handleListNodeDelete(pimsmCb->handleList,
                         &pStarGIEntry->pimsmStarGIPrunePendingTimerHandle);     
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGIEntry->pimsmStarGIJoinExpiryTimer));
  handleListNodeDelete(pimsmCb->handleList,
                         &pStarGIEntry->pimsmStarGIJoinExpiryTimerHandle);
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGIEntry->pimsmStarGIAssertTimer));
  handleListNodeDelete(pimsmCb->handleList,
                         &pStarGIEntry->pimsmStarGIAssertTimerHandle);

  PIMSM_FREE (pimsmCb->family, (void*) pStarGIEntry);

  pStarGNode->pimsmStarGIEntry[rtrIfNum] = L7_NULLPTR;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Purge all (*,G) routes .
*
* @param   pimsmCb     @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmStarGTreePurge(pimsmCB_t * pimsmCb)
{
  pimsmStarGNode_t       * pStarGNode;
  pimsmStarGEntry_t *      pStarGEntry;
  L7_RC_t                  rc;
  L7_uint32                rtrIfNum;
  L7_uint32 mrtStarGTableSize = 0;

  rc = pimsmStarGFirstGet( pimsmCb, &pStarGNode );
  while(rc == L7_SUCCESS)
  {
    if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_DELETE_ENTRY, L7_FALSE) 
        != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
           "Failed to delete (*,G) from MFC");
    }
    pStarGEntry = &pStarGNode->pimsmStarGEntry;

    pimsmUtilAppTimerCancel (pimsmCb, &(pStarGEntry->pimsmStarGUpstreamJoinTimer));
    handleListNodeDelete(pimsmCb->handleList,
                         &pStarGEntry->pimsmStarGUpstreamJoinTimerHandle);

    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      pimsmStarGIEntryDelete(pimsmCb, pStarGNode, rtrIfNum);
      pStarGNode->pimsmStarGIEntry[rtrIfNum] = L7_NULLPTR;
    }  
    pimsmStarGCacheDeInit(pimsmCb,pStarGNode);
    rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
  }

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtStarGTableSize = PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  avlPurgeAvlTree(&pimsmCb->pimsmStarGTree, mrtStarGTableSize);

  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
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
void pimsmStarGTreeRpChngRemapGroup(pimsmCB_t        *pimsmCb,
                                    pimsmStarGNode_t *pStarGNode)
{
  L7_uint32 iif;
  L7_uchar8 grp[PIMSM_MAX_MSG_SIZE];

   iif = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;

   if (iif == PIMSM_REGISTER_INTF_NUM)
   {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
       "Incoming interface is Register interface ");  
      if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode,MFC_DELETE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,%s) MFC Delete Failed",
                    inetAddrPrint(&pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress, grp));
      }
   }
   else
   {      
     if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode,MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,%s) MFC Update Failed",
                   inetAddrPrint(&pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress, grp));
     }
   }
}

/*********************************************************************
* @purpose Post event to (*,G) Assert FSM
*
* @param    pimsmCb     @b{(input)} PIM SM Control Block
* @param pStarGNode     @b{(input)} (*,G) Node
* @param stopRPFIf   @b{(input)} RPF interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static 
void pimsmStarGAssertEventPost(pimsmCB_t *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                                L7_BOOL stopRPFIf)
{
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  L7_BOOL bIAmAssertWinner;
  pimsmAssertMetric_t myAssertMetric;
  L7_inet_addr_t zeroAddr;
  L7_uint32 rtrIfNum;

  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry == L7_NULLPTR)
    {
      continue;
    }
    if(inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress) 
       == L7_TRUE)
    {
      continue; 
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
    inetAddressZeroSet(pimsmCb->family, &zeroAddr);
    pimsmMyAssertMetric(pimsmCb, &zeroAddr,
      &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress,
      pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex, &myAssertMetric);
   
    bIAmAssertWinner = pimsmCompareMetrics(
      myAssertMetric.metric_preference | myAssertMetric.rpt_bit_flag,
      myAssertMetric.route_metric,
      &myAssertMetric.ip_address,
      pStarGIEntry->pimsmStarGIAssertWinnerMetricPref,
      pStarGIEntry->pimsmStarGIAssertWinnerMetric,
      &pStarGIEntry->pimsmStarGIAssertWinnerAddress);
   
    if (bIAmAssertWinner == L7_TRUE) 
    {
       memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
       starGAssertEventInfo.rtrIfNum = rtrIfNum;
       starGAssertEventInfo.eventType = 
            PIMSM_ASSERT_STAR_G_SM_EVENT_METRIC_BETTER_THAN_WINNER;
       PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,
           "my assert metric better than assert winner");       
       inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
       pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
    }
    if (stopRPFIf == L7_TRUE)
    {
      memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
      starGAssertEventInfo.rtrIfNum = rtrIfNum;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"stop begin RPF IIF");
      starGAssertEventInfo.eventType = 
      PIMSM_ASSERT_STAR_G_SM_EVENT_STOP_BEING_RPF_IFACE;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
    }
  }  
}

/*********************************************************************
* @purpose Update (*,G) tree with Local membership changed
*
* @param   pimsmCb    @b{(input)} PIM SM Control Block
* @param   pSrcAddr   @b{(input)} Src IP Addr
* @param   pGrpAddr   @b{(input)} Group IP Addr
* @param   rtrIfNum   @b{(input)} router Interface Number
* @param   isStarGIEntryDeleted    @b{(input)} Delete flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmStarGLocalMemberShipChngEventPost(pimsmCB_t * pimsmCb, 
                                               L7_inet_addr_t * pGrpAddr, 
                                               L7_uint32 rtrIfNum,
                                               L7_BOOL isStarGIEntryDeleted,
                                               L7_BOOL starGILocalMemberShip,
                                               L7_BOOL jpMsgImdtSend)
{
  pimsmStarGNode_t  * pStarGNode = L7_NULLPTR;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  L7_uchar8 grp[PIMSM_MAX_MSG_SIZE];

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }

  if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,
                "(*,%s) not found", inetAddrPrint(pGrpAddr, grp));
    return L7_FAILURE;
  }

  if (pStarGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"pimsmStarGNode not found");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"pimsmStarGNode found");
  if (isStarGIEntryDeleted == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"isStarGIEntryDeleted is FALSE");

    if (pimsmStarGICouldAssert (pimsmCb, pStarGNode, rtrIfNum) == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_NOTICE,"StarGICouldAssert is FALSE.");
      memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
      starGAssertEventInfo.rtrIfNum = rtrIfNum;
      starGAssertEventInfo.eventType =
                                PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode, &starGAssertEventInfo);
    } 

    if (pimsmStarGIAssertTrackingDesired (pimsmCb, pStarGNode,rtrIfNum) == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_NOTICE,"StarGIAssertTrackingDesired is FALSE");
      memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
      starGAssertEventInfo.rtrIfNum = rtrIfNum;
      starGAssertEventInfo.eventType =
                                   PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TRACKING;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
    }
  }

  if (pStarGNode->pimsmStarGEntry.pimsmStarGRPIsLocal == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"RP is not Local"); 
  }
    memset(&upStrmStarGEventInfo, 0, sizeof(pimsmUpStrmStarGEventInfo_t));  
    upStrmStarGEventInfo.sendImmdFlag = jpMsgImdtSend;
    if (pimsmStarGJoinDesired (pimsmCb, pStarGNode) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"StarGJoinDesired is TRUE");
      upStrmStarGEventInfo.eventType =
                                 PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_INFO,"StarGJoinDesired is FALSE");
      upStrmStarGEventInfo.eventType =
                                PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
      
    }
      pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,&upStrmStarGEventInfo);
  

  if(starGILocalMemberShip == L7_TRUE)
  {
    if (pimsmSGTreeJoinRecvEntryUpdate(pimsmCb,pGrpAddr,rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update (S,G) node  when join received");
      return L7_FAILURE;
    }
  }
  else 
  {
    if (pimsmSGTreePruneRecvEntryUpdate(pimsmCb,pGrpAddr,rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update (S,G) node  when prune received");
      return L7_FAILURE;
    }
  }
  /* Delete any (*,*,RP) resulted -ve entries */
  pimsmStarStarRpNegativeDelete(pimsmCb,&pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  /*Update only when entry exists in MFC */
  if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MGMD_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update KernelCache");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Update (*,G) Tree when an interface is down
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
L7_RC_t  pimsmStarGTreeIntfDownUpdate(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t  starGNode, *pStarGNode, *pStarGNodeDelete = L7_NULLPTR;
  pimsmStarGEntry_t *pStarGEntry;
  L7_RC_t rc;
  L7_inet_addr_t *pGrpAddr;
  pimsmSendJoinPruneData_t    jpData;
  L7_uchar8 maskLen =0;
  interface_bitset_t      starGImdtOif1, starGImdtOif2;
  L7_BOOL couldAssert;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  L7_BOOL joinDesired;  

  memset(&starGNode, 0, sizeof(pimsmStarGNode_t));
  pStarGNode = &starGNode;
  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while(rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    if(pStarGEntry->pimsmStarGRPFIfIndex == rtrIfNum)
    {
      if(inetIsAddressZero(&(pStarGEntry->pimsmStarGUpstreamNeighbor))
         == L7_FALSE)
      {
        /*Send Prune (*,G);*/
        memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
        jpData.rtrIfNum 
        = pStarGEntry->pimsmStarGRPFIfIndex;
        if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                           &jpData.holdtime)
                                             != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, 
            PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
          jpData.holdtime =  3.5 * L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL;
        }                                             
        jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
        pimSrcMaskLenGet(pimsmCb->family,&maskLen);
        jpData.srcAddrMaskLen = maskLen;
        pimGrpMaskLenGet(pimsmCb->family,&maskLen);
        jpData.grpAddrMaskLen = maskLen;
        jpData.joinOrPruneFlag = L7_FALSE;

        jpData.pNbrAddr =
        &(pStarGEntry->pimsmStarGUpstreamNeighbor);
        jpData.pSrcAddr = 
        &pStarGEntry->pimsmStarGRPAddress;
        jpData.pGrpAddr = 
        &pStarGEntry->pimsmStarGGrpAddress;
        
        if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Faield to send Join/Prune Message");
        }
        pimsmSGRptIncludePruneInMsg(pimsmCb, pStarGNode);
      }
      pStarGNodeDelete = pStarGNode;
    }
    else
    {
      pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
      if (pStarGIEntry != L7_NULLPTR)
      {
        memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
        starGAssertEventInfo.rtrIfNum = rtrIfNum;
        couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
        if(couldAssert == L7_FALSE)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"StarGICouldAssert is FALSE");
          starGAssertEventInfo.eventType = 
          PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
          inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
          pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
        } 
        else
        {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NOTICE,"StarGICouldAssert is TRUE");
        }
        
        memset(&starGImdtOif1, 0, sizeof(interface_bitset_t));
        memset(&starGImdtOif2, 0, sizeof(interface_bitset_t));        
        pimsmStarGIEntryDelete(pimsmCb, pStarGNode, rtrIfNum);
        joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
        if(joinDesired == L7_FALSE) 
        {
          upStrmStarGEventInfo.eventType 
          = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
          pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);
        }
        else
        {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                     "Error: pimsmStarGJoinDesired() is true");
        }         
        /* Update to MFC with the new OIF List.
         */
        if (pimsmStarGMFCUpdate (pimsmCb, pStarGNode, MFC_UPDATE_ENTRY, L7_FALSE)
                              != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to update (*,G) Entry to MFC");
        }
      }
    }  

    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);

    /* Delete the (*,G) node if the upstream interface is Down.
     */
    if(pStarGNodeDelete != L7_NULLPTR)
    {
      pGrpAddr= &(pStarGNodeDelete->pimsmStarGEntry.pimsmStarGGrpAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "(*,G) is deleted for Group :\n", 
                       pGrpAddr);
      pimsmStarGDelete(pimsmCb, pGrpAddr);
      pStarGNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (*,G) tree when best route has changed
*
* @param    pimsmCb        @b{(input)} PIM SM Control Block
* @param pRPFRouteInfo  @b{(input)} Route info   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarGTreeRTOCommon(pimsmCB_t * pimsmCb, 
            pimsmStarGNode_t *pStarGNode,
            mcastRPFInfo_t *rpfRouteInfo)
{
  pimsmStarGEntry_t *pStarGEntry;
  L7_uint32 rtrIfNum;
  L7_BOOL couldAssert;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;

  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO, "Enter");
  pStarGEntry = &pStarGNode->pimsmStarGEntry;  
  if(rpfRouteInfo->rpfIfIndex != pStarGEntry->pimsmStarGRPFIfIndex)
  {
    memset(&starGAssertEventInfo, 0,
          sizeof(pimsmPerIntfStarGAssertEventInfo_t));
    rtrIfNum = rpfRouteInfo->rpfIfIndex;
    starGAssertEventInfo.rtrIfNum = rtrIfNum;
    couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
    if(couldAssert == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"StarGICouldAssert is FALSE");
      starGAssertEventInfo.eventType = 
      PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,
                                 &starGAssertEventInfo);
    } 
    else
    {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_NOTICE,"StarGICouldAssert is TRUE");
    }
  }
  memset(&upStrmStarGEventInfo, 0, sizeof(pimsmUpStrmStarGEventInfo_t));
  /* Pass old RPF info as Event Param */
  upStrmStarGEventInfo.rpfInfo.rpfIfIndex =
  pStarGEntry->pimsmStarGRPFIfIndex;
  inetCopy(&upStrmStarGEventInfo.rpfInfo.rpfNextHop,
           &pStarGEntry->pimsmStarGRPFNextHop);
  inetCopy(&upStrmStarGEventInfo.rpfInfo.rpfRouteAddress,
           &pStarGEntry->pimsmStarGRPFRouteAddress);
  upStrmStarGEventInfo.rpfInfo.rpfRouteProtocol =
          pStarGEntry->pimsmStarGRPFRouteProtocol;
  upStrmStarGEventInfo.rpfInfo.rpfRouteMetric =
          pStarGEntry->pimsmStarGRPFRouteMetric;
  upStrmStarGEventInfo.rpfInfo.rpfRouteMetricPref =
          pStarGEntry->pimsmStarGRPFRouteMetricPref;
  inetCopy(&upStrmStarGEventInfo.upStrmNbrAddr,
         &pStarGEntry->pimsmStarGUpstreamNeighbor);
  inetCopy(&upStrmStarGEventInfo.rpAddr,&pStarGEntry->pimsmStarGRPAddress);

  /* Copy new RPF info to the (*,G) node */
  pStarGEntry->pimsmStarGRPFIfIndex = rpfRouteInfo->rpfIfIndex;
  inetCopy(&pStarGEntry->pimsmStarGRPFNextHop,&rpfRouteInfo->rpfNextHop);
  inetCopy(&pStarGEntry->pimsmStarGRPFRouteAddress,
         &rpfRouteInfo->rpfRouteAddress);
  pStarGEntry->pimsmStarGRPFRouteProtocol = rpfRouteInfo->rpfRouteProtocol;
  pStarGEntry->pimsmStarGRPFRouteMetric = rpfRouteInfo->rpfRouteMetric;
  pStarGEntry->pimsmStarGRPFRouteMetricPref = rpfRouteInfo->rpfRouteMetricPref;
  pStarGEntry->pimsmStarGRPFRoutePrefixLength =
     rpfRouteInfo->prefixLength;
  /* Update the (*,G) entry with new RP and upstream neighbor address */      
  pimsmStarGNeighborRpfDash(pimsmCb,pStarGNode);

  upStrmStarGEventInfo.eventType = 
  PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT;
  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, 
                          &upStrmStarGEventInfo);
  if(pimsmSGRptTreeBestRouteChngUpdate(pimsmCb, &pStarGEntry->pimsmStarGGrpAddress,
       rpfRouteInfo->rpfIfIndex,  rpfRouteInfo->status) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
       "Failed to update (S,G,Rpt) with RPF change");
    return L7_FAILURE;
  }  
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (*,G) tree when best route has changed
*
* @param    pimsmCb        @b{(input)} PIM SM Control Block
* @param pRPFRouteInfo  @b{(input)} Route info   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGTreeBestRouteChngUpdate(pimsmCB_t * pimsmCb, 
                                             mcastRPFInfo_t *pRPFRouteInfo)
{
  pimsmStarGNode_t *pStarGNode= L7_NULLPTR;
  pimsmStarGEntry_t *pStarGEntry;
  L7_RC_t rc;
  L7_BOOL stopRPFIf =L7_FALSE;  
  L7_uint32 rtrIfNum =0,rpfIfIndex=0;
  mcastRPFInfo_t rpfRouteInfo;

  if(pRPFRouteInfo == (mcastRPFInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG, "invalid input parameters"); 
    return L7_FAILURE;
  }
  memcpy(&rpfRouteInfo, pRPFRouteInfo, sizeof(mcastRPFInfo_t));

  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while(rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;

    if(inetAddrCompareAddrWithMask(&pStarGEntry->pimsmStarGRPAddress, 
                                   rpfRouteInfo.prefixLength,
                                   &rpfRouteInfo.rpfRouteAddress, 
                                   rpfRouteInfo.prefixLength) == 0)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RTO RPF Route Address:",
                       &rpfRouteInfo.rpfRouteAddress);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RTO Prefix Length is %d",
                  rpfRouteInfo.prefixLength);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RTO Next Hop is:",
                         &(rpfRouteInfo.rpfNextHop));
  
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"RPF Route Address:",
                     &pStarGEntry->pimsmStarGRPFRouteAddress);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"Prefix Length is %d",
                pStarGEntry->pimsmStarGRPFRoutePrefixLength);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_DEBUG,"Next Hop is:",
                         &pStarGEntry->pimsmStarGRPFNextHop);


      /*pimsmSGNegativeDelete(pimsmCb, 
                            &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmSGRptNegativeDelete(pimsmCb, 
                               &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress); */             
      pimsmStarGNegativeDelete(pimsmCb, 
                               &pStarGNode->pimsmStarGEntry.pimsmStarGRPAddress);
      if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rpfRouteInfo.rpfIfIndex) != L7_TRUE)
      {
        if (mcastIpMapUnnumberedIsLocalAddress(&pStarGEntry->pimsmStarGRPAddress,
                                               L7_NULLPTR) == L7_TRUE)
        {
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfNextHop);
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfRouteAddress);
          rpfRouteInfo.rpfIfIndex = 0;
          rpfRouteInfo.rpfRouteMetric =  0;
          rpfRouteInfo.rpfRouteMetricPref = 0;
          rpfRouteInfo.rpfRouteProtocol = RTO_LOCAL;
        }
        else if (mcastIpMapIsDirectlyConnected(&pStarGEntry->pimsmStarGRPAddress,
                                               &rtrIfNum) == L7_TRUE)
        {
          inetCopy(&rpfRouteInfo.rpfNextHop,
                   &pStarGEntry->pimsmStarGRPAddress);
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
          if(pimsmStarGIEntryDelete(pimsmCb,pStarGNode,
                                 pStarGEntry->pimsmStarGRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (*,G,%d) entry",
                          pStarGEntry->pimsmStarGRPFIfIndex);
              break;
          }         
          rpfIfIndex =  pStarGEntry->pimsmStarGRPFIfIndex;         
          pimsmStarGTreeRTOCommon(pimsmCb, pStarGNode, &rpfRouteInfo);
          pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex = rpfIfIndex;
          pimsmStarGDelete(pimsmCb,
                           &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
          break;

        case RTO_CHANGE_ROUTE:
          /* no need to check for nexthop/ifindex, 
          anything might change like metric */         
          if(pimsmStarGIEntryDelete(pimsmCb,pStarGNode,
                                 pStarGEntry->pimsmStarGRPFIfIndex) != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (*,G,%d) entry",
                          pStarGEntry->pimsmStarGRPFIfIndex);
              break;
          }
          /* fall-through for remaining work*/
        case RTO_ADD_ROUTE:
          if(pimsmStarGIEntryCreate(pimsmCb,pStarGNode,
                                    rpfRouteInfo.rpfIfIndex) 
             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR, 
                        "Failed to create (*,G,I) for upstream index %d",
                        rpfRouteInfo.rpfIfIndex);
            break;
          }        
          if((rpfRouteInfo.rpfRouteProtocol == RTO_LOCAL) ||
            (PIMSM_INET_IS_ADDR_EQUAL(&pStarGEntry->pimsmStarGRPFNextHop,
                                    &(rpfRouteInfo.rpfNextHop)) == L7_FALSE ) || 
             (pStarGEntry->pimsmStarGRPFIfIndex != rpfRouteInfo.rpfIfIndex))
          {
             /* Note: stopRPFIf should be updated here only, 
              otherwise pimsmStarGTreeRTOCommon() will change
              then pimsmStarGRPFIfIndex */
             if(rpfRouteInfo.rpfIfIndex != pStarGEntry->pimsmStarGRPFIfIndex)
             {
                stopRPFIf = L7_TRUE;
             }        
             pimsmStarGTreeRTOCommon(pimsmCb, pStarGNode, &rpfRouteInfo);
          }
          else
          {
            /* Copy new RPF info to the (*,G) node */
            pStarGEntry->pimsmStarGRPFRouteMetric = rpfRouteInfo.rpfRouteMetric;
            pStarGEntry->pimsmStarGRPFRouteMetricPref = rpfRouteInfo.rpfRouteMetricPref;
          }
          /*delete negative entries from MFC*/
          if((pStarGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
          {
            if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_DELETE_ENTRY, L7_FALSE) 
                != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,
                   "Failed to delete (*,G) from MFC");
            } 
            else
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO, 
                "Negative (*,G) MFC Entry deleted");
            }
          }
          pimsmStarGAssertEventPost(pimsmCb, pStarGNode, stopRPFIf);
          if(pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update MFC");   
          }          
          break;
        default:
          break;
      }
    }
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Update (*,G) Tree with RP change
*
* @param    pimsmCb       @b{(input)} PIM SM Control Block
* @param pGrpAddr      @b{(input)} Group IP Addr    
* @param grpPrefixLen  @b{(input)} Source IP Addr
* @param pRpAddr       @b{(input)} RP IP Addr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGTreeRPChngUpdate(pimsmCB_t * pimsmCb, 
                                   L7_inet_addr_t   * pGrpAddr,
                                   L7_uint32 grpPrefixLen,
                                   L7_inet_addr_t   * pRpAddr)
{
  pimsmStarGNode_t *pStarGNode= L7_NULLPTR,  *pStarGNodeDelete = L7_NULLPTR;
  pimsmStarGEntry_t *pStarGEntry;
  pimsmStarGIEntry_t  *pStarGIEntry;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  pimsmStarStarRpNode_t *pStarStarRpNode = L7_NULLPTR;
  pimsmStarStarRpEntry_t *pStarStarRpEntry = L7_NULLPTR;
  L7_uint32  rpRPFIndex = L7_NULL;
  interface_bitset_t starGImdtOif;
  L7_BOOL oifListFlag;
  L7_RC_t rc;
  L7_uint32 i;
  L7_inet_addr_t  *pGrpAddrDelete;    
  L7_BOOL isOldRPLocal = L7_FALSE;

  if (pGrpAddr == (L7_inet_addr_t *)L7_NULLPTR ||
      pRpAddr  == (L7_inet_addr_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "invalid input parameters ");  
    return L7_FAILURE;
  }
  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while (rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    if (inetAddrCompareAddrWithMask(&pStarGEntry->pimsmStarGGrpAddress, 
                                    grpPrefixLen,
                                    pGrpAddr, 
                                    grpPrefixLen) == 0)
    {
      /* Do this to get actual RP for that particular group(G) in (*,G) ,
         as the RP change for an MRT entry cannot be detected at RP code 
         because only the group range is known at RP code level which is 
         not sometimes not suffient to detect an RP change for MRT entries */
      pimsmRpAddressGet(pimsmCb, &pStarGEntry->pimsmStarGGrpAddress,
                        pRpAddr);

      if (inetIsAddressZero(pRpAddr) == L7_TRUE)
      {
        pStarGNodeDelete = pStarGNode;
      }
      else
      {
        if (PIMSM_INET_IS_ADDR_EQUAL(&pStarGEntry->pimsmStarGRPAddress,
                                   pRpAddr) == L7_FALSE)
        {
          /* if (pimsmRPFInfoGet(pimsmCb, pRpAddr, &rpfRouteInfo) == L7_SUCCESS) */
          if((pimsmStarStarRpFind(pimsmCb, pRpAddr, &pStarStarRpNode) == L7_SUCCESS) &&
             (pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex
               != MCAST_MAX_INTERFACES))
          {
            pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
            rpRPFIndex = pStarStarRpEntry->pimsmStarStarRpRPFIfIndex; 
            if (pStarGEntry->pimsmStarGRPFIfIndex != rpRPFIndex)
            {
              /* Delete all old oif (*,G,I) entries */  
              pimsmStarGImdtOlist(pimsmCb, pStarGNode,&starGImdtOif);
              BITX_IS_EMPTY( &starGImdtOif, oifListFlag );
              if (oifListFlag == L7_FALSE)
              {
                for (i = 0; i < MCAST_MAX_INTERFACES;i++)
                {
                  if (BITX_TEST(&starGImdtOif, i) != L7_NULL)
                  {
                    pStarGIEntry =
                    pStarGNode->pimsmStarGIEntry[i];
                    if (pStarGIEntry->pimsmStarGILocalMembership == L7_TRUE)
                    {
                      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
                              "Local membership present on rtrIfNum = %d",i);
                      continue;
                    }
                    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
                          " Deleting the (*,G,I) for rtrIfNum = %d",i);
                    pimsmStarGIEntryDelete(pimsmCb,  pStarGNode, i);
                  }
                }
              }

              isOldRPLocal = pStarGEntry->pimsmStarGRPIsLocal;
              if (mcastIpMapUnnumberedIsLocalAddress(pRpAddr, L7_NULLPTR) == L7_TRUE)
              {
                PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Local RP address \n");
                pStarGEntry->pimsmStarGRPIsLocal = L7_TRUE;
              }
              else
              {
                PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Non Local RP address \n");
                pStarGEntry->pimsmStarGRPIsLocal = L7_FALSE;
              }


              /* Delete (*,G,I) for old RPF index and create one for new RPF index */
              PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Deleting old RPF index : %d",
                          pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex);
              pimsmStarGIEntryDelete(pimsmCb, pStarGNode,
                                 pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex);

              PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Creating new RPF index : %d",
                         rpRPFIndex);

              if (pimsmStarGIEntryCreate(pimsmCb, pStarGNode, 
                                         rpRPFIndex) != L7_SUCCESS)
              {
                PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                            "Failed to create (*,G,I) for upstream index %d",
                            rpRPFIndex);
                return L7_FAILURE;
              }

              /* Pass old RPF info as Event Param */
              memset(&upStrmStarGEventInfo, 0, sizeof(pimsmUpStrmStarGEventInfo_t));
              upStrmStarGEventInfo.rpfInfo.rpfIfIndex =
              pStarGEntry->pimsmStarGRPFIfIndex;
              inetCopy(&upStrmStarGEventInfo.rpfInfo.rpfNextHop,
                       &pStarGEntry->pimsmStarGRPFNextHop);
              inetCopy(&upStrmStarGEventInfo.rpfInfo.rpfRouteAddress,
                       &pStarGEntry->pimsmStarGRPFRouteAddress);
              upStrmStarGEventInfo.rpfInfo.rpfRouteProtocol =
                      pStarGEntry->pimsmStarGRPFRouteProtocol;
              upStrmStarGEventInfo.rpfInfo.rpfRouteMetric =
                      pStarGEntry->pimsmStarGRPFRouteMetric;
              upStrmStarGEventInfo.rpfInfo.rpfRouteMetricPref =
                      pStarGEntry->pimsmStarGRPFRouteMetricPref;
              inetCopy(&upStrmStarGEventInfo.upStrmNbrAddr,
                     &pStarGEntry->pimsmStarGUpstreamNeighbor);
              inetCopy(&upStrmStarGEventInfo.rpAddr,&pStarGEntry->pimsmStarGRPAddress);

              /* Copy new RPF info to the (*,G) node */
              pStarGEntry->pimsmStarGRPFIfIndex = rpRPFIndex;
              inetCopy(&pStarGEntry->pimsmStarGRPFNextHop,
                       &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
              inetCopy(&pStarGEntry->pimsmStarGRPFRouteAddress,
                       &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
              pStarGEntry->pimsmStarGRPFRouteProtocol = 
                       pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol;
              pStarGEntry->pimsmStarGRPFRouteMetric = 
                       pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric;
              pStarGEntry->pimsmStarGRPFRouteMetricPref = 
                       pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref;

              /* Update the (*,G) entry with new RP and upstream neighbor address */      
              pimsmStarGNeighborRpfDash(pimsmCb,pStarGNode);

              inetCopy(&pStarGEntry->pimsmStarGRPAddress,pRpAddr);

              /* If you are still the RP, Nothing needs to be done */
              if ((isOldRPLocal == L7_TRUE) &&
                  (pStarGEntry->pimsmStarGRPIsLocal == L7_TRUE))
              {
                PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                             "Wrong Flow: RP has not changed");

                rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
                continue;
              }

                /* You are going from Non-RP to RP (or)
                 * Some RP has changed. */
                /* Send a Prune towards the old RP and a Join towards the
                 * new RP.
                 */
                upStrmStarGEventInfo.eventType =
                     PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT;
                pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);
                pimsmStarGTreeRpChngRemapGroup(pimsmCb, pStarGNode);
            }
          } 
          else 
          {
             /* No RP exists , so delete (*,G) */
             pStarGNodeDelete = pStarGNode;
          }
        }
      }
    }
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
    if (pStarGNodeDelete != L7_NULLPTR)
    {
      pGrpAddrDelete= &(pStarGNodeDelete->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmStarGDelete(pimsmCb, pGrpAddrDelete);
      pStarGNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Update (*,G) tree when (*,*,Rp) Join received
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pRpAddr    @b{(input)} Rp IP Addr
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
void pimsmStarGTreeStarStarRpJoinRecvUpdate(pimsmCB_t * pimsmCb, 
                                             L7_inet_addr_t * pRpAddr, 
                                             L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t          *pStarGNode;   
  pimsmStarGEntry_t         *pStarGEntry;
  L7_RC_t                   rc;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  
  rc = pimsmStarGFirstGet( pimsmCb, &pStarGNode );
  while(rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    if(PIMSM_INET_IS_ADDR_EQUAL(&pStarGEntry->pimsmStarGRPAddress,
                                pRpAddr) == L7_TRUE)
    {
      memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
      starGAssertEventInfo.rtrIfNum = rtrIfNum;
      starGAssertEventInfo.eventType =
      PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_JOIN;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);  
      if(pimsmStarGMFCUpdate(pimsmCb,pStarGNode, 
                       MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to update MFC");
        return;
      }
    }
    rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
  }
}


/******************************************************************************
* @purpose  Update pkt/ byte count for (*,G) entry for a particular source
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pSrcAddr    @b{(input)}  source address
* @param    len         @b{(input)}  encapsulated packet length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGKernelCacheCountUpdate(pimsmCB_t * pimsmCb, 
                                    pimsmStarGNode_t * pStarGNode,
                                    L7_inet_addr_t    *pSrcAddr,
                                    L7_uint32 len)
{
  L7_RC_t rc;
  pimsmCache_t  * kernelCache;
  L7_inet_addr_t * pGrpAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Enter");  

  if(pimsmCb == L7_NULLPTR || pStarGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }        
  
  pGrpAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress;

  rc = pimsmStarGCacheFind(pimsmCb, pStarGNode, pSrcAddr, pGrpAddr, &kernelCache);
  if(rc != L7_SUCCESS)
  {
    /* If it doesn't exist, add it. */
    if(pimsmStarGCacheAdd(pimsmCb, pStarGNode, pSrcAddr) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                  "Kernel Cache Add failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
      return L7_FAILURE;
    }
    rc = pimsmStarGCacheFind(pimsmCb, pStarGNode,
                              pSrcAddr, pGrpAddr, &kernelCache);
  }

  if(rc == L7_SUCCESS)
  {
    kernelCache->pimsmSGRealtimePktcnt++;
    kernelCache->pimsmSGRealtimeByteCount += len;
  }
  return rc;
}

/******************************************************************************
* @purpose  Update all sources from (*,G) entry to MFC
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGMFCUpdate(pimsmCB_t *pimsmCb,
                               pimsmStarGNode_t *pStarGNode, 
                               mcastEventTypes_t eventType,
                               L7_BOOL bForceUpdate)
{
  L7_RC_t rc = L7_FAILURE;
  pimsmCache_t *kernelCacheEntry = L7_NULLPTR;
  interface_bitset_t oifList;
  interface_bitset_t scopedOifList;
  L7_uint32 pimsmStarGRPFIfIndex = 0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Enter");  

  if(pimsmCb == L7_NULLPTR || pStarGNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }  
  memset(&oifList,0,sizeof(interface_bitset_t));
  if (eventType == MFC_UPDATE_ENTRY)
  {
    if((pStarGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_ERROR, 
          "Node is deleted");            
      return L7_FAILURE;
    }  
    if( (bForceUpdate == L7_FALSE) &&
        (((pStarGNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_INFO, 
          "Entry is not added in MFC");
      return L7_SUCCESS;
    }
      
    pimsmStarGImdtOlist(pimsmCb,pStarGNode,&oifList);
    BITX_RESET(&oifList, pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex);
    if (pimsmCb->family == L7_AF_INET)
    {  
     /* subtract all admin-scoped interfaces */
     BITX_RESET_ALL(&scopedOifList);
     if (mcastMapGroupScopedInterfacesGet(&(pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress),
                                          (L7_uchar8 *)&(scopedOifList)) == L7_SUCCESS)
     {
       if (BITX_TEST(&scopedOifList,pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex))
       {
         BITX_RESET_ALL(&oifList);
       }
       else
       {
         BITX_MASK_AND(&oifList, &scopedOifList, &oifList);
       }
     }
    }
  }
  else if (eventType == MFC_DELETE_ENTRY)
  {
    if(((pStarGNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_INFO, 
          "Entry is not present in MFC");
      return L7_SUCCESS;
    } 
  }
  rc = pimsmStarGCacheGetFirst(pimsmCb,pStarGNode,&kernelCacheEntry);
  while(rc == L7_SUCCESS)
  {
    pimsmStarGRPFIfIndex = pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;

    if (pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex == 0)
    {
      L7_uchar8 src[PIMSM_MAX_DBG_ADDR_SIZE];
      L7_uchar8 grp[PIMSM_MAX_DBG_ADDR_SIZE];

      if (pimsmRPFInterfaceGet (pimsmCb, &kernelCacheEntry->pimsmSrcAddr,
                                &pimsmStarGRPFIfIndex)
                             != L7_SUCCESS)
      {
        PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
                     "RPF Interface for %s Not Found, (*,%s) Entry is not added in MFC",
                     inetAddrPrint(&kernelCacheEntry->pimsmSrcAddr, src),
                     inetAddrPrint(&kernelCacheEntry->pimsmGrpAddr, grp));
        rc = pimsmStarGCacheNextGet(pimsmCb,pStarGNode,kernelCacheEntry,
                                     &kernelCacheEntry);
        continue;
      }
    }

    if(pimsmMfcQueue(pimsmCb,eventType,&kernelCacheEntry->pimsmSrcAddr,
                     &kernelCacheEntry->pimsmGrpAddr, pimsmStarGRPFIfIndex,
                     &oifList, L7_FALSE, &pStarGNode->flags) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"pimsmMfcQueue failed");
    }

    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "pimsmMfcQueue Success");
    rc = pimsmStarGCacheNextGet(pimsmCb,pStarGNode,kernelCacheEntry,
                                 &kernelCacheEntry);
  }

  if (SLLNumMembersGet(&(pStarGNode->pimsmStarGCacheList)) == 0)
  {
      pStarGNode->flags &= ~PIMSM_ADDED_TO_MFC;          
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"PIMSM_ADDED_TO_MFC RESET in STARG");              
  }
   
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Success");
  return L7_SUCCESS; 
}


/******************************************************************************
* @purpose  Initializes the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheInit(pimsmCB_t * pimsmCb, 
                             pimsmStarGNode_t * pStarGNode)
{

  return pimsmCacheInit(pimsmCb, 
               &pStarGNode->pimsmStarGCacheList);
}

/******************************************************************************
* @purpose  DeInitializes the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheDeInit(pimsmCB_t * pimsmCb, 
                               pimsmStarGNode_t * pStarGNode)
{

   return pimsmCacheDeInit(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList));
}


/******************************************************************************
* @purpose  Adds a kernelel cache entry to the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarGNode  @b{(input)} (*,G) node
* @param    pSrcAddr    @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheAdd(pimsmCB_t * pimsmCb, 
                            pimsmStarGNode_t    * pStarGNode,
                            L7_inet_addr_t      * pSrcAddr)
{

   return pimsmCacheAdd(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList),
               pSrcAddr,
               &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
}

/******************************************************************************
* @purpose  Deletes a kernelel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarGNode        @b{(input)} (*,G) node
* @param    kernelCacheEntry  @b{(input)} kernelCache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheDelete(pimsmCB_t * pimsmCb, 
                               pimsmStarGNode_t * pStarGNode,
                               pimsmCache_t  * kernelCacheEntry)
{
      return pimsmCacheDelete(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList),
               kernelCacheEntry,
               &pStarGNode->flags);
}


/******************************************************************************
* @purpose  Return the first kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarGNode        @b{(input)} (*,G) node
* @param    kernelCacheEntry  @b{(input)}  pointer to kernelCache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheGetFirst(pimsmCB_t * pimsmCb, 
                                 pimsmStarGNode_t * pStarGNode,
                                 pimsmCache_t  ** kernelCacheEntry)
{

   return pimsmCacheGetFirst(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList),
               kernelCacheEntry);
}


/******************************************************************************
* @purpose  Returns the next kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb                @b{(input)}  control block  
* @param    pStarGNode             @b{(input)} (*,G) node
* @param    currentKernCacheEntry  @b{(input)} current kernelCache entry
* @param    nextKernCacheEntry     @b{(input)} pointer current kernelCache entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheNextGet(pimsmCB_t * pimsmCb, 
                                pimsmStarGNode_t * pStarGNode,
                                pimsmCache_t  * currentKernCacheEntry,
                                pimsmCache_t  ** nextKernCacheEntry)
{

   return pimsmCacheNextGet(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList),
               currentKernCacheEntry, nextKernCacheEntry);
}

/******************************************************************************
* @purpose  Find and return the kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarGNode        @b{(input)} (*,G) node
* @param    pSrcAddr          @b{(input)}  source address
* @param    pGrpAddr          @b{(input)}  group address
* @param    kernelCacheEntry  @b{(input)}  pointer to kernelCache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarGCacheFind(pimsmCB_t * pimsmCb, 
                             pimsmStarGNode_t * pStarGNode,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t      *pGrpAddr,
                             pimsmCache_t  ** kernelCacheEntry)
{

   return pimsmCacheFind(pimsmCb, 
               &(pStarGNode->pimsmStarGCacheList),
               pSrcAddr,
               pGrpAddr,
               kernelCacheEntry);
}




/*********************************************************************
* @purpose  Test whether  mrt entry from the (S,G) entry table can be deleted
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimsmStarGNodeCanRemove(pimsmCB_t * pimsmCb, pimsmStarGNode_t *pStarGNode)
{

  pimsmStarGEntry_t  *pStarGEntry;
  pimsmStarGIEntry_t  *pStarGIEntry;  
  L7_uint32 index;

  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
     pStarGIEntry = pStarGNode->pimsmStarGIEntry[index];
     if ( pStarGIEntry != L7_NULLPTR)
     {
        if((pStarGIEntry->pimsmStarGILocalMembership == L7_TRUE) ||
           (pStarGIEntry->pimsmStarGIJoinPruneState !=
              PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO) ||
           (pStarGIEntry->pimsmStarGIAssertState !=
              PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO) ||
           (pStarGIEntry->pimsmStarGIPrunePendingTimer != L7_NULLPTR) || 
           (pStarGIEntry->pimsmStarGIJoinExpiryTimer != L7_NULLPTR) ||
           (pStarGIEntry->pimsmStarGIAssertTimer != L7_NULLPTR))
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
            "(*,G, %d) is active", index);        
          return L7_FALSE; 
        }
     }
  }
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  if((pStarGEntry->pimsmStarGUpstreamJoinState !=
        PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED) ||
     (pStarGEntry->pimsmStarGUpstreamJoinTimer != L7_NULLPTR))
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, 
            "(*,G) is active");
    return L7_FALSE; 
  } 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, 
            "(*,G) is in-active");        
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G,  PIMSM_TRACE_NORMAL, "Group Address :",
                                      &pStarGEntry->pimsmStarGGrpAddress);
 
  return L7_TRUE;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL pimsmStarGNodeTryRemove(pimsmCB_t * pimsmCb, pimsmStarGNode_t *pStarGNode)
{
  L7_BOOL retVal;
 
  retVal = pimsmStarGNodeCanRemove(pimsmCb, pStarGNode);
  if(retVal == L7_TRUE) 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, 
        "StarGNodeCanRemove = %d", retVal);
    pStarGNode->flags |=  PIMSM_NODE_DELETE;
  }
  return retVal;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarGNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void pimsmStarGNodeCleanup(pimsmCB_t * pimsmCb)
{

  pimsmStarGNode_t *pStarGNode, *pStarGNodeDelete = L7_NULLPTR;
  L7_inet_addr_t *pGrpAddr;
  pimsmStarGNode_t  starGNode;

  /* Set all elements of the node to zero */
  memset (&starGNode, 0, sizeof (pimsmStarGNode_t));
  inetAddressZeroSet(pimsmCb->family,
          &starGNode.pimsmStarGEntry.pimsmStarGGrpAddress);  
  
  pStarGNode = avlSearchLVL7(&pimsmCb->pimsmStarGTree, 
                                 &starGNode, AVL_NEXT);  
  while(pStarGNode != L7_NULLPTR)
  {
    if((pStarGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
     pStarGNodeDelete =  pStarGNode;
    }
     pStarGNode = avlSearchLVL7(&pimsmCb->pimsmStarGTree, 
                                 pStarGNode, AVL_NEXT);  
    if(pStarGNodeDelete != L7_NULLPTR)
    {
      pGrpAddr= &(pStarGNodeDelete->pimsmStarGEntry.pimsmStarGGrpAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL,
              "(*,G) is deleted for Group :\n", 
                       pGrpAddr);
      pimsmStarGDelete(pimsmCb, pGrpAddr);
      pStarGNodeDelete = L7_NULLPTR;
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
L7_RC_t pimsmStarGNegativeDelete(pimsmCB_t      *pimsmCb,
                              L7_inet_addr_t *pRpAddr)
{
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pRpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmStarGFirstGet(pimsmCb,&pStarGNode);

  while(rc == L7_SUCCESS)
  {
    if(PIMSM_INET_IS_ADDR_EQUAL(pRpAddr,
                             &pStarGNode->pimsmStarGEntry.pimsmStarGRPAddress) 
       == L7_TRUE)
    {
      if((pStarGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        pimsmStarGMFCUpdate(pimsmCb, pStarGNode, MFC_DELETE_ENTRY, L7_FALSE);
      }
      pimsmSGNegativeDelete(pimsmCb, 
        &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmSGRptNegativeDelete(pimsmCb, 
        &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmStarStarRpNegativeDelete (pimsmCb,
        &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    }
    rc = pimsmStarGNextGet(pimsmCb,pStarGNode,&pStarGNode);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update the (*,G) entries with admin-scope config change
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
L7_RC_t pimsmStarGAdminScopeUpdate(pimsmCB_t *pimsmCb)
{
  pimsmStarGNode_t *pStarGNode= L7_NULLPTR;
  L7_RC_t rc;

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while (rc == L7_SUCCESS)
  {
    if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode , MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      L7_uchar8 grp[PIMSM_MAX_MSG_SIZE];
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,%s) MFC Update Failed",
                  inetAddrPrint(&pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress, grp));
    }
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the (*,G) entries Nexthop as link-local addr.
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments : call from pimsmStarStarRpNextHopUpdate() only
*
* @end
*
*********************************************************************/
void  pimsmStarGNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry)
{
  pimsmStarGNode_t        *pStarGNode;
  pimsmStarGEntry_t *      pStarGEntry;
  L7_RC_t               rc;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  L7_BOOL joinDesired;
  L7_inet_addr_t primaryAddr;

  if(pimsmCb->pimsmStarGNextHopUpdateCnt <= 0)
  {
      return;
  }
  
  rc = pimsmStarGFirstGet( pimsmCb, &pStarGNode );
  while(rc == L7_SUCCESS)
  {    
    pStarGEntry = &pStarGNode->pimsmStarGEntry;

    if(pStarGEntry->pimsmStarGRPFIfIndex != rtrIfNum)
    {
      rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
      continue;
    }
   if (pimsmCb->family == L7_AF_INET6 && 
      inetIsAddressZero(&pStarGEntry->pimsmStarGRPFNextHop) == L7_FALSE &&
      L7_IP6_IS_ADDR_LINK_LOCAL(
          &pStarGEntry->pimsmStarGRPFNextHop.addr.ipv6))
    {
      rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
      continue;
    }    
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, 
        "(*,G) Getting primary addr");   
    if(pimsmNbrPrimaryAddrGet(pimsmCb,
            pimsmNbrEntry, &pStarGEntry->pimsmStarGRPFNextHop,
            &primaryAddr) != L7_SUCCESS)
    {
      rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
      continue;
    }
    inetCopy(&pStarGEntry->pimsmStarGRPFNextHop,&primaryAddr);
    pimsmCb->pimsmStarGNextHopUpdateCnt--;    
    pimsmStarGNeighborRpfDash(pimsmCb,pStarGNode);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, 
        "(*,G) primary addr updated");    
    memset(&upStrmStarGEventInfo,0,sizeof(pimsmUpStrmStarGEventInfo_t));
    joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
    if (joinDesired == L7_TRUE)
    {
      upStrmStarGEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else
    {
      upStrmStarGEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);    
    if(pimsmStarGMFCUpdate (pimsmCb,pStarGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "MFC Updation failed");
    }
    rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
  }   
}

