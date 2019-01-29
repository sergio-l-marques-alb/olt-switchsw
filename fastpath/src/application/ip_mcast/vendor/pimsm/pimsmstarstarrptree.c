/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarStarRpTree.c
*
* @purpose Contains functions to operate on (*,*,RP) linked list
*
* @component 
*
* @comments 
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda / Satya Dillikar
* @end
*
**********************************************************************/

#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmstarstarrptree.h"
#include "l7sll_api.h"
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"

/*********************************************************************
* @purpose  Compares the given (*,*,Rp) nodes
*
* @param pimsmStarStarRpNode1  @b{(input)} (*,*,Rp) Node
* @param pimsmStarStarRpNode2  @b{(input)} (*,*,Rp) Node
* @param keySize               @b{(input)} key length
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2.
*
* @comments      
*       
* @end
*********************************************************************/
static L7_int32 pimsmStarStarRpCompare(void * pNode1, 
                                      void * pNode2,
                                      L7_uint32 keySize)
{
  pimsmStarStarRpNode_t * pStarStarRpNode1, *pStarStarRpNode2;
  L7_inet_addr_t  * rpAddr1, * rpAddr2;
  L7_uint32 result;

  if((pNode1 == L7_NULLPTR) || (pNode2 == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, "NLLPTR passed for (*,*,RP) compare");
    return(1);
  }
  pStarStarRpNode1 = (pimsmStarStarRpNode_t *)pNode1;
  pStarStarRpNode2 = (pimsmStarStarRpNode_t *)pNode2;

  rpAddr1 = &pStarStarRpNode1->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress;
  rpAddr2 = &pStarStarRpNode2->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG, "RP Address1 :", rpAddr1);   
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG, "RP Address2 :", rpAddr2);   

  result = L7_INET_ADDR_COMPARE(rpAddr1, rpAddr2);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG, "(*,*,Rp)Compare result is  %d", result); 
  return(result);
}

/*********************************************************************
* @purpose  Destroys a (*,*,Rp) node
*
* @param node        @b{(input)} (*,*,Rp) Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpDestroy(L7_sll_member_t *node)
{
  L7_uchar8 addrFamily = 0;

  addrFamily = L7_INET_GET_FAMILY (&(((pimsmStarStarRpNode_t*)node)->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress));

  PIMSM_FREE (addrFamily, (void*) node);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, 
              "Freed (*,*,RP)Node!! \n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initializes the (*,*,RP) entry table
*
* @param   pimsmCb        @b{(input)} PIM SM Control block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpInit(pimsmCB_t * pimsmCb)
{
  pimsmCb->pimsmStarStarRpListLock = (void *)
         osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if(pimsmCb->pimsmStarStarRpListLock == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
                "Failed to create (*,*,RP) Semaphore ");
    return L7_FAILURE;
  }

  /* Create the Message list */
  if(SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                L7_SLL_ASCEND_ORDER, 0,
                pimsmStarStarRpCompare,
                pimsmStarStarRpDestroy,
                &(pimsmCb->pimsmStarStarRpList)) != L7_SUCCESS)
  {
    /* Destroy the Semaphore */
    osapiSemaDelete (pimsmCb->pimsmStarStarRpListLock);
    pimsmCb->pimsmStarStarRpListLock = L7_NULLPTR;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
                "Failed to allocate Star Star RP Linked List ");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Frees the (*,*,Rp) entry table 
*
* @param   pimsmCb      @b{(input)} PIM SM Control block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpDeInit(pimsmCB_t * pimsmCb)
{
  L7_RC_t retVal;
 
  if(&(pimsmCb->pimsmStarStarRpList) == (L7_sll_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "invalid input parameters");
    return L7_FAILURE;
  }
  pimsmStarStarRpPurge(pimsmCb);
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  retVal = SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID, 
                      &(pimsmCb->pimsmStarStarRpList));
  if(retVal != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "Failed to Destroy (*,*,Rp) Linked List ");
    return L7_FAILURE;
  }

  /* Destroy the Semaphore */
  retVal = osapiSemaDelete (pimsmCb->pimsmStarStarRpListLock);
  if(retVal != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "Failed to delete semaphore ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Add a mrt entry to the (*,*,RP) entry List
*
* @param    pimsmCb          @b{(input)} PIM SM Control Block
* @param   pStarStarRpNode  @b{(input)} (*,*,Rp) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmStarStarRpAdd(pimsmCB_t             *pimsmCb, 
                                  pimsmStarStarRpNode_t *pStarStarRpNode)
{
  L7_RC_t  rc;

  if(pStarStarRpNode == (pimsmStarStarRpNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "Invalid input parameters");
    return L7_FAILURE;
  }

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  rc = SLLAdd (&(pimsmCb->pimsmStarStarRpList), 
               (L7_sll_member_t *)pStarStarRpNode);  
  if(osapiSemaGive(pimsmCb->pimsmStarStarRpListLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to give semaphore "); 
    return L7_FAILURE;
  }
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "Failed to add to (*,*,RP)node ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,  "SLLAdd RetCode is %d", rc);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a mrt entry from the (*,*,RP) entry List
*
* @param    pimsmCb          @b{(input)} PIM SM Control Block
* @param pStarStarRpNode  @b{(input)} (*,*,Rp) Node 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpDelete(pimsmCB_t             *pimsmCb, 
                              pimsmStarStarRpNode_t *pStarStarRpNode)
{
  L7_RC_t   retVal;
  L7_uint32 rtrIfNum =0;
    
  if (pimsmCb == L7_NULLPTR || pStarStarRpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, "invalid input parameters");
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Addr to delete is :",
               &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);   
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamJoinTimer));
  handleListNodeDelete(pimsmCb->handleList,
  &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamJoinTimerHandle);

  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    pimsmStarStarRpIEntryDelete(pimsmCb, pStarStarRpNode, rtrIfNum);
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
               "deleting MFC entry" );
  if (pimsmStarStarRpMFCUpdate (pimsmCb, pStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE)
                             != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Failed to update (*,*,RP) Entry to MFC");
  }  
  if (&(pStarStarRpNode->pimsmStarStarRpCacheList) != L7_NULLPTR)
  {
    pimsmStarStarRpCacheDeInit(pimsmCb,pStarStarRpNode);
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  retVal =  SLLDelete(&(pimsmCb->pimsmStarStarRpList), 
                      (L7_sll_member_t *)pStarStarRpNode);

  if(osapiSemaGive(pimsmCb->pimsmStarStarRpListLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to give semaphore "); 
    return L7_FAILURE;
  }
  /* Remove the entry from the timer list */
  if(retVal != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Failed to delete (*,*,RP) node ");    
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify a mrt entry in the (*,*,RP) entry List
*
* @param    pimsmCb          @b{(input)} PIM SM Control Block
* @param pStarStarRpNode  @b{(input)} (*,*,Rp) Node    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpModify(pimsmCB_t             *pimsmCb, 
                              pimsmStarStarRpNode_t *pStarStarRpNode)
{
  MCAST_UNUSED_PARAM(pStarStarRpNode);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Check whether the (*,*,RP) entry exists in the (*,*,RP) list
*
* @param    pimsmCb           @b{(input)} PIM SM Control Block
* @param   ppStarStarRpNode  @b{(output)} (*,*,Rp) Node to Search  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpFirstGet(pimsmCB_t              *pimsmCb, 
                                pimsmStarStarRpNode_t **ppStarStarRpNode)
{
  L7_sll_t  * pStarStarRpList;
  pimsmStarStarRpNode_t *pStarStarRpNodeTmp;

  if(ppStarStarRpNode == (pimsmStarStarRpNode_t **)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG,"Invalid input parameters");   
    return L7_FAILURE;
  }

  pStarStarRpList = &(pimsmCb->pimsmStarStarRpList);

  if(pStarStarRpList == (L7_sll_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "StarStarRp List is not initialised ");
    return L7_FAILURE;
  }
  
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER)
                    != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pStarStarRpNodeTmp = (pimsmStarStarRpNode_t *)SLLFirstGet(pStarStarRpList);
  if(osapiSemaGive(pimsmCb->pimsmStarStarRpListLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  if(pStarStarRpNodeTmp == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG,"Unable to get first (*,*,Rp) node");
    return L7_FAILURE;
  }
  *ppStarStarRpNode = pStarStarRpNodeTmp;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the next (*,*,RP) entry after the specified (*,*,RP) entry 
*           in the (*,*,RP) list
*
* @param    pimsmCb                 @b{(input)} PIM SM Control Block
* @param      pStarstarRpNodeCurrent  @b{(input)} current (*,*,Rp) Node 
* @param      ppStarStarRpNode        @b{(output)} (*,*,Rp) Node   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpNextGet(pimsmCB_t             *pimsmCb,
                               pimsmStarStarRpNode_t *pStarStarRpNodeCurrent, 
                               pimsmStarStarRpNode_t **ppStarStarRpNode)
{
  L7_sll_t  * pStarStarRpList;
  pimsmStarStarRpNode_t *pStarStarRpNodeTmp;
  
  if(pStarStarRpNodeCurrent == (pimsmStarStarRpNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, 
                  "Error: StarStarRpNextGet received NULLPTR ");    
    return L7_FAILURE;
  }

  pStarStarRpList = &(pimsmCb->pimsmStarStarRpList);
  if(pStarStarRpList == (L7_sll_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_FATAL, 
                "StarStarRp List is not initialised");
    return L7_FAILURE;
  }
  
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER) 
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pStarStarRpNodeTmp= (pimsmStarStarRpNode_t *)SLLNextGet(pStarStarRpList, 
                     (L7_sll_member_t *)pStarStarRpNodeCurrent);
  if(osapiSemaGive(pimsmCb->pimsmStarStarRpListLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  if(pStarStarRpNodeTmp == (pimsmStarStarRpNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG, 
      "(Failed to get next (*,*,Rp) entry "); 
    return L7_FAILURE;
  }
 
  *ppStarStarRpNode = pStarStarRpNodeTmp;    
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Search the (*,*,RP) list
*
* @param    pimsmCb           @b{(input)} PIM SM Control Block
* @param      pRpAddr           @b{(input)} RP IP Addr 
* @param     ppStarStarRpNode  @b{(output)} (*,*,Rp) Node with RP as given RP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpFind(pimsmCB_t              *pimsmCb, 
                            L7_inet_addr_t         *pRpAddr, 
                            pimsmStarStarRpNode_t **ppStarStarRpNode)
{
  L7_sll_t  * pStarStarRpList;
  pimsmStarStarRpNode_t starStarRpNodeTemp, *pStarStarRpNode;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pRpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  pStarStarRpList = &(pimsmCb->pimsmStarStarRpList);
  memset(&starStarRpNodeTemp, 0, sizeof(pimsmStarStarRpNode_t));
  inetCopy(&starStarRpNodeTemp.pimsmStarStarRpEntry.pimsmStarStarRpRPAddress, 
           pRpAddr);

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarStarRpListLock, L7_WAIT_FOREVER) 
                   != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to take semaphore"); 
    return L7_FAILURE;
  }
  pStarStarRpNode = (pimsmStarStarRpNode_t *)SLLFind
                   (pStarStarRpList, 
                    (L7_sll_member_t *)&starStarRpNodeTemp);

  if(osapiSemaGive(pimsmCb->pimsmStarStarRpListLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to give semaphore"); 
    return L7_FAILURE;
  }
  if(pStarStarRpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG, "Failed to find (*,*,Rp) node");
    return L7_FAILURE;
  }
  if((pStarStarRpNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    return L7_FAILURE;
  }   
  if(ppStarStarRpNode != L7_NULLPTR)
  {
     *ppStarStarRpNode = pStarStarRpNode;
  }  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a New node in the (*,*,Rp) list
*
* @param    pimsmCb           @b{(input)} PIM SM Control Block
* @param   pRpAddr           @b{(input)} RP IP Addr 
* @param   ppStarStarRpNode  @b{(output)} (*,*,Rp) Node 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpNodeCreate(pimsmCB_t              *pimsmCb,
                                  L7_inet_addr_t         *pRpAddr, 
                                  pimsmStarStarRpNode_t **ppStarStarRpNode)
{
  mcastRPFInfo_t  rpfRouteInfo;
  pimsmStarStarRpNode_t * pStarStarRpNode = L7_NULLPTR, 
  *pStarStarRpNodeNew;
  pimsmStarStarRpEntry_t  * pStarStarRpEntry;
  L7_RC_t  rc;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pRpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
 
  rc = pimsmStarStarRpFind(pimsmCb, pRpAddr, &pStarStarRpNode);
  if(rc != L7_SUCCESS)
  {
    if ((pStarStarRpNodeNew = PIMSM_ALLOC (pimsmCb->family,
                                           sizeof(pimsmStarStarRpNode_t)))
                                        == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (*,*,Rp) interface pool");    
      return L7_FAILURE;
    }                       

    memset(pStarStarRpNodeNew, 0,  sizeof(pimsmStarStarRpNode_t));
    pStarStarRpEntry = &pStarStarRpNodeNew->pimsmStarStarRpEntry;
    inetAddressZeroSet(pimsmCb->family, 
        &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
    inetAddressZeroSet(pimsmCb->family, 
        &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
    inetAddressZeroSet(pimsmCb->family, 
        &pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor);

    pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
               (void*)&pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerParam);       
    inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPAddress, 
             pRpAddr);

    /*Get the RPF Info*/         
    if(pimsmRPFInfoGet(pimsmCb, pRpAddr, &rpfRouteInfo) == L7_SUCCESS)
    {
#ifdef PIMSM_NOT_NEEDED    
      /* Check if PIMSM is enabled on RPF Interface.
       * RPF IfIndex is 0 in case if RP Address is Local Address.  Skip it.
       */
      if (rpfRouteInfo.rpfIfIndex != 0)
      {
        if (pimsmInterfaceEntryGet (pimsmCb, rpfRouteInfo.rpfIfIndex) != L7_SUCCESS)
        {
          PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
                       "PIMSM is not Enabled on RPF Intf - %d", rpfRouteInfo.rpfIfIndex);
          *ppStarStarRpNode = L7_NULLPTR;
          PIMSM_FREE (pimsmCb->family, (void*) pStarStarRpNodeNew);
          return L7_FAILURE;
        }
      }
#endif
      pStarStarRpEntry->pimsmStarStarRpRPFIfIndex 
      = rpfRouteInfo.rpfIfIndex;
      inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPFNextHop, 
               &rpfRouteInfo.rpfNextHop);
      inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress, 
               &rpfRouteInfo.rpfRouteAddress);
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref 
      = rpfRouteInfo.rpfRouteMetricPref;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric 
      = rpfRouteInfo.rpfRouteMetric;
      pStarStarRpEntry->pimsmStarStarRpPimMode = PIM_MODE_NONE;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol
      = rpfRouteInfo.rpfRouteProtocol;
      pStarStarRpEntry->pimsmStarStarRpRPFRoutePrefixLength
      = rpfRouteInfo.prefixLength;
    }
    else
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, 
         "Failed to get RPF info for RP :", pRpAddr);
      /* Don't return L7_FAILURE here, later when the route
       is up, RTO will update the RPF info */
      pStarStarRpEntry->pimsmStarStarRpRPFIfIndex 
      = MCAST_MAX_INTERFACES;
      inetAddressZeroSet(pimsmCb->family, 
          &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
      inetAddressZeroSet(pimsmCb->family, 
          &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref = 0;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric = 0;
      pStarStarRpEntry->pimsmStarStarRpPimMode = PIM_MODE_NONE;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol = 0;
      pStarStarRpEntry->pimsmStarStarRpRPFRoutePrefixLength =0;
    }

    if(pimsmStarStarRpAdd(pimsmCb, pStarStarRpNodeNew) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Failed to add (*,*,Rp) node");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, "RP Address :",
                        &pStarStarRpEntry->pimsmStarStarRpRPAddress);
      *ppStarStarRpNode = L7_NULLPTR;
      PIMSM_FREE (pimsmCb->family, (void*) pStarStarRpNodeNew);
      return L7_FAILURE;    
    }
    rc = pimsmStarStarRpFind(pimsmCb, pRpAddr, &pStarStarRpNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, "Failed to find (*,*,Rp) node");      
      return L7_FAILURE;
    }
    /*pStarStarRpNode->pSelf = pStarStarRpNode;*/
    pStarStarRpEntry->pimsmStarStarRpUpstreamJoinState 
    = PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED;
    pimsmStarStarRpNeighborRpfDash(pimsmCb,pStarStarRpNode);

    pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer = L7_NULLPTR;
    if(pimsmStarStarRpCacheInit(pimsmCb, pStarStarRpNode) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "Failed to initialize Kernel Cache");
       pimsmStarStarRpDelete(pimsmCb,pStarStarRpNode);
       return L7_FAILURE;
     }
    pStarStarRpEntry->pimsmStarStarRpCreateTime = osapiUpTimeRaw();
  }
  pStarStarRpNode->flags &=  ~PIMSM_NODE_DELETE;
  *ppStarStarRpNode = pStarStarRpNode;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create (*,*,RP, I) Entry and init with default params
*
* @param   pimsmCb           @b{(input)} PIM SM Control Block
* @param   pStarStarRpNode   @b{(input)} (*,*,RP) Node
* @param   rtrIfNum          @b{(input)} router interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpIEntryCreate(pimsmCB_t             *pimsmCb, 
                                    pimsmStarStarRpNode_t *pStarStarRpNode, 
                                    L7_uint32              rtrIfNum)
{
  pimsmStarStarRpIEntry_t * pStarStarRIEntryNew;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pStarStarRpNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
 
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }
  if(pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum] 
     ==(pimsmStarStarRpIEntry_t *)L7_NULLPTR)
  {
    if ((pStarStarRIEntryNew = PIMSM_ALLOC (pimsmCb->family,
                                            sizeof(pimsmStarStarRpIEntry_t)))
                                         == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (*,*,Rp) interface pool");    
      return L7_FAILURE;
    }                       
    memset(pStarStarRIEntryNew, 0, sizeof(pimsmStarStarRpIEntry_t));
    pStarStarRIEntryNew->pimsmStarStarRpIIfIndex= rtrIfNum;
    pStarStarRIEntryNew->pimsmStarStarRpIPrunePendingTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
           (void*)&pStarStarRIEntryNew->pimsmStarStarRpIPrunePendingTimerParam);       
    pStarStarRIEntryNew->pimsmStarStarRpIJoinExpiryTimerHandle = 
    handleListNodeStore(pimsmCb->handleList,
           (void*)&pStarStarRIEntryNew->pimsmStarStarRpIJoinExpiryTimerParam);       

    /*pStarStarRIEntryNew->pimsmStarStarRpILocalMembership= L7_FALSE;*/
    pStarStarRIEntryNew->pimsmStarStarRpIJoinPruneState
    = PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO;
    pStarStarRIEntryNew->pimsmStarStarRpICreateTime = osapiUpTimeRaw();
    pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum] = pStarStarRIEntryNew;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete (*,*,RP,I) Entry 
* 
* @param   pimsmCb            @b{(input)} PIM SM Control block
* @param   pStarStarRpIEntry  @b{(input)} (*,*,RP,I) Entry  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpIEntryDelete(pimsmCB_t             *pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                    L7_uint32              rtrIfNum)
{
  pimsmStarStarRpIEntry_t * pStarStarRpIEntry = L7_NULLPTR;
  
  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pStarStarRpNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  } 
  pStarStarRpIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if (pStarStarRpIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG,
         " (*,*,Rp,%d) is not present",rtrIfNum);
   return L7_SUCCESS;
  }     

  pimsmUtilAppTimerCancel (pimsmCb, &(pStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimerHandle);     
  handleListNodeDelete(pimsmCb->handleList,
                       &pStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimerHandle);     

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_DEBUG,"Address of pointer to be freed is %p\r\n",
              pStarStarRpIEntry);
  PIMSM_FREE (pimsmCb->family, (void*) pStarStarRpIEntry);

  pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum] = L7_NULLPTR;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Update all sources from (*,*,RP) entry to MFC
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarStarRpNode  @b{(input)} (*,*,RP) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpMFCUpdate(pimsmCB_t *pimsmCb,
                               pimsmStarStarRpNode_t *pStarStarRpNode, 
                               mfcOperationType_t eventType,
                               L7_BOOL bForceUpdate)
{
  L7_RC_t rc = L7_FAILURE;
  pimsmCache_t *kernelCacheEntry = L7_NULLPTR;
  interface_bitset_t oifList;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,"Enter");  

  if(pimsmCb == L7_NULLPTR || pStarStarRpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }  

  memset(&oifList,0,sizeof(interface_bitset_t));
  if (eventType == MFC_UPDATE_ENTRY)
  {
    if((pStarStarRpNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_ERROR, 
          "Node is deleted");            
      return L7_FAILURE;
    }   
    if( (bForceUpdate == L7_FALSE) &&
        (pimsmCb->pimsmRestartInProgress == L7_FALSE) &&
        (((pStarStarRpNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_INFO, 
          "Entry is not added in MFC");
      return L7_SUCCESS;
    }    
    pimsmStarStarRpImdtOlist(pimsmCb,pStarStarRpNode,&oifList);
    BITX_RESET(&oifList,
      pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex);
  }
  else if (eventType == MFC_DELETE_ENTRY)
  {
    if(((pStarStarRpNode->flags) & PIMSM_ADDED_TO_MFC) == L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_INFO, 
          "Entry is not present in MFC");
      return L7_SUCCESS;
    } 
  }
  rc = pimsmStarStarRpCacheGetFirst(pimsmCb,pStarStarRpNode,&kernelCacheEntry);
  while(rc == L7_SUCCESS)
  {
    {
      if(pimsmMfcQueue(pimsmCb,eventType,&kernelCacheEntry->pimsmSrcAddr,
                       &kernelCacheEntry->pimsmGrpAddr,
                       pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex,
                       &oifList, L7_FALSE, &pStarStarRpNode->flags) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,"pimsmMfcQueue failed");
      }
    }

    rc = pimsmStarStarRpCacheNextGet(pimsmCb,pStarStarRpNode,kernelCacheEntry,
                                 &kernelCacheEntry);
  }

  if (SLLNumMembersGet(&(pStarStarRpNode->pimsmStarStarRpCacheList)) == 0)
  {
      pStarStarRpNode->flags &= ~PIMSM_ADDED_TO_MFC;          
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"PIMSM_ADDED_TO_MFC RESET in STARSTARRP");              
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,"Success");
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Purge all (*,*,Rp) entries
*
* @param    pimsmCb          @b{(input)} PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE

*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t pimsmStarStarRpPurge(pimsmCB_t * pimsmCb)
{
  pimsmStarStarRpNode_t       * pStarStarRpNode = L7_NULLPTR,
                              * pStarStarRpNodeNext = L7_NULLPTR;
  L7_RC_t                       rc;

  rc = pimsmStarStarRpFirstGet( pimsmCb, &pStarStarRpNode );
  while(rc == L7_SUCCESS)
  {
    rc = pimsmStarStarRpNextGet( pimsmCb,
                                 pStarStarRpNode,
                                 &pStarStarRpNodeNext );
    pimsmStarStarRpDelete(pimsmCb, pStarStarRpNode);
    pStarStarRpNode = pStarStarRpNodeNext;
  }     
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update (*,*,RP) list on interface down event
*
* @param    pimsmCb      @b{(input)} PIM SM Control Block  
* @param    rtrIfNum     @b{(input)} router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  pimsmStarStarRpTreeIntfDownUpdate(pimsmCB_t *pimsmCb, 
                                           L7_uint32  rtrIfNum)
{
  pimsmStarStarRpNode_t  *pStarStarRpNode = L7_NULLPTR;
  pimsmStarStarRpEntry_t *pStarStarRpEntry = L7_NULLPTR;
  L7_RC_t rc;
  pimsmSendJoinPruneData_t jpData;
  L7_uchar8 maskLen;
  pimsmStarStarRpIEntry_t * pStarStarRpIEntry = L7_NULLPTR;
  
  rc = pimsmStarStarRpFirstGet(pimsmCb, &pStarStarRpNode);
  while(rc == L7_SUCCESS)
  {
    pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
    if(pStarStarRpEntry->pimsmStarStarRpRPFIfIndex == rtrIfNum)
    {
      if(inetIsAddressZero(&
                           (pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor)) 
         == L7_FALSE)
      {
        /*Send Prune (*,*,RP);*/
        memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
        jpData.rtrIfNum 
        = pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;

        if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                           &jpData.holdtime)
                                             != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, 
            PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
          jpData.holdtime =  3.5 * L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL;
        }         
        jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
        pimSrcMaskLenGet(pimsmCb->family,&maskLen);  
        jpData.srcAddrMaskLen= maskLen;
        jpData.joinOrPruneFlag = L7_FALSE;
        jpData.pNbrAddr = 
        &(pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor);         
        jpData.pSrcAddr = 
        &pStarStarRpEntry->pimsmStarStarRpRPAddress;
        jpData.isStarStarRpFlag = L7_TRUE;
        if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR,"FAiled to send Join/Prune Message");
        }
      }
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, 
         "Resetting RPF info to NOT_REACHABLE route for RP :", &pStarStarRpEntry->pimsmStarStarRpRPAddress);
      /* Don't return L7_FAILURE here, later when the route
       is up, RTO will update the RPF info */
      pStarStarRpEntry->pimsmStarStarRpRPFIfIndex 
      = MCAST_MAX_INTERFACES;
      inetAddressZeroSet(pimsmCb->family, 
          &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
      inetAddressZeroSet(pimsmCb->family, 
          &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref = 0;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric = 0;
      pStarStarRpEntry->pimsmStarStarRpPimMode = PIM_MODE_NONE;
      pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol = 0;
      pStarStarRpEntry->pimsmStarStarRpRPFRoutePrefixLength =0;
    }

    pStarStarRpIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
    if (pStarStarRpIEntry != L7_NULLPTR)
    {
      pimsmStarStarRpIEntryDelete(pimsmCb, pStarStarRpNode, rtrIfNum);
      /* Update to MFC with the new OIF List*/
      pimsmStarStarRpMFCUpdate (pimsmCb, pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE);
    }
    rc = pimsmStarStarRpNextGet(pimsmCb, pStarStarRpNode, 
                                &pStarStarRpNode);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Update (*,*,Rp) list with best route change event
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
static L7_RC_t pimsmStarStarRpTreeRTOCommon(pimsmCB_t * pimsmCb, 
            pimsmStarStarRpNode_t *pStarStarRpNode,
            mcastRPFInfo_t *rpfRouteInfo)
{
  pimsmStarStarRpEntry_t *pStarStarRpEntry;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRpEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"Entered");

   pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
   memset(&upStrmStarStarRpEventInfo, 0, 
          sizeof(pimsmUpStrmStarStarRPEventInfo_t)); 
  /* Pass old RPF info as Event Param */
  upStrmStarStarRpEventInfo.rpfInfo.rpfIfIndex =
  pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;
  inetCopy(&upStrmStarStarRpEventInfo.rpfInfo.rpfNextHop,
           &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
  inetCopy(&upStrmStarStarRpEventInfo.rpfInfo.rpfRouteAddress,
           &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
  upStrmStarStarRpEventInfo.rpfInfo.rpfRouteProtocol =
          pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol;
  upStrmStarStarRpEventInfo.rpfInfo.rpfRouteMetric =
          pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric;
  upStrmStarStarRpEventInfo.rpfInfo.rpfRouteMetricPref =
          pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref;
  inetCopy(&upStrmStarStarRpEventInfo.upStrmNbrAddr,
         &pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor);

  /* Copy new RPF info to the (*,*,RP) node */
   inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress,
      &rpfRouteInfo->rpfRouteAddress);
   inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPFNextHop,
      &rpfRouteInfo->rpfNextHop);
   pStarStarRpEntry->pimsmStarStarRpRPFRoutePrefixLength =
      rpfRouteInfo->prefixLength;
   pStarStarRpEntry->pimsmStarStarRpRPFIfIndex = 
      rpfRouteInfo->rpfIfIndex;
   pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric = 
      rpfRouteInfo->rpfRouteMetric;
   pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref= 
      rpfRouteInfo->rpfRouteMetricPref;
   pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol= 
      rpfRouteInfo->rpfRouteProtocol;  
  /* Update the (*,*,RP) entry with new RP and upstream neighbor address */      
  pimsmStarStarRpNeighborRpfDash(pimsmCb,pStarStarRpNode);
  
   upStrmStarStarRpEventInfo.eventType =
   PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_NBR_CHANGED;
   pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode, 
                                &upStrmStarStarRpEventInfo);

   PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"(*,*,Rp) Rpf = %d", 
               pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);

   if(pimsmStarStarRpMFCUpdate(pimsmCb,pStarStarRpNode,
      MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
   {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"Failed to update MFC");
     return L7_FAILURE;
   }   
   return L7_SUCCESS;    
}
/*********************************************************************
* @purpose  Update (*,*,Rp) list with best route change event
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
L7_RC_t pimsmStarStarRpTreeBestRouteChngUpdate(pimsmCB_t      *pimsmCb,
                                                 mcastRPFInfo_t *pRPFRouteInfo)
{
  pimsmStarStarRpNode_t  starStarRpNode, *pStarStarRpNode;
  pimsmStarStarRpEntry_t *pStarStarRpEntry;
  L7_RC_t rc;
  L7_uint32 rtrIfNum =0;
  mcastRPFInfo_t rpfRouteInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"Entered");
  
  if(pRPFRouteInfo == (mcastRPFInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"invalid input parameters");
    return L7_FAILURE;
  }
  memcpy(&rpfRouteInfo, pRPFRouteInfo, sizeof(mcastRPFInfo_t));
  memset(&starStarRpNode, 0, sizeof(pimsmStarStarRpNode_t));
  pStarStarRpNode = &starStarRpNode;
  rc = pimsmStarStarRpFirstGet(pimsmCb, &pStarStarRpNode);
  while(rc == L7_SUCCESS)
  {
    pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;


    if(inetAddrCompareAddrWithMask(&pStarStarRpEntry->pimsmStarStarRpRPAddress, 
                                  rpfRouteInfo.prefixLength,
                                  &rpfRouteInfo.rpfRouteAddress, 
                                  rpfRouteInfo.prefixLength) == 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"found(*,*,Rp) entry");
  
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,
                       "(*,*,RP), RP = ",&pStarStarRpEntry->pimsmStarStarRpRPAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,
                       "Given route address = ", &rpfRouteInfo.rpfRouteAddress);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_INFO,"prefixLen = %d ", rpfRouteInfo.prefixLength );

      pimsmStarStarRpCacheNegativeDelete (pimsmCb,pStarStarRpNode);


      PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,"Route applies for (*,*,Rp)");

      if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rpfRouteInfo.rpfIfIndex) != L7_TRUE)
      {

        if (mcastIpMapUnnumberedIsLocalAddress(&pStarStarRpEntry->pimsmStarStarRpRPAddress,
                                               L7_NULLPTR) == L7_TRUE)
        {
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfNextHop);
          inetAddressZeroSet(pimsmCb->family, &rpfRouteInfo.rpfRouteAddress);
          rpfRouteInfo.rpfIfIndex = 0;
          rpfRouteInfo.rpfRouteMetric =  0;
          rpfRouteInfo.rpfRouteMetricPref = 0;
          rpfRouteInfo.rpfRouteProtocol = RTO_LOCAL;
        }
        else if (mcastIpMapIsDirectlyConnected(&pStarStarRpEntry->pimsmStarStarRpRPAddress,
                                               &rtrIfNum) == L7_TRUE)
        {
          inetCopy(&rpfRouteInfo.rpfNextHop,
                   &pStarStarRpEntry->pimsmStarStarRpRPAddress);
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
          if(pimsmStarStarRpIEntryDelete(pimsmCb,pStarStarRpNode,
                                 pStarStarRpEntry->pimsmStarStarRpRPFIfIndex) 
                                 != L7_SUCCESS)
          {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (*,*,RP,%d) entry",
                          pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
              break;
          }         
          pimsmStarStarRpTreeRTOCommon(pimsmCb, pStarStarRpNode, &rpfRouteInfo);
          break;

        case RTO_CHANGE_ROUTE:
           /* no need to check for nexthop/ifindex, 
            anything might change like metric */
          if (pStarStarRpEntry->pimsmStarStarRpRPFIfIndex != MCAST_MAX_INTERFACES)
          {
            if (pimsmStarStarRpIEntryDelete(pimsmCb,pStarStarRpNode,
                                            pStarStarRpEntry->pimsmStarStarRpRPFIfIndex)
                != L7_SUCCESS)
            {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
                          "Failed to delete (*,*,RP,%d) entry",
                          pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
              break;
            }
          }
          /* fall-through for remaining work*/
        case RTO_ADD_ROUTE:

          PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
            "ADD route for (*,*,Rp) rpf = %d", 
                      rpfRouteInfo.rpfIfIndex);
          if(pimsmStarStarRpIEntryCreate(pimsmCb,pStarStarRpNode,
                                    rpfRouteInfo.rpfIfIndex) 
             != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR, 
                        "Failed to create (*,*,RP,I) for upstream index %d",
                        rpfRouteInfo.rpfIfIndex);
            break;
          }         
          if((rpfRouteInfo.rpfRouteProtocol == RTO_LOCAL) ||
            (PIMSM_INET_IS_ADDR_EQUAL(&pStarStarRpEntry->pimsmStarStarRpRPFNextHop,
                                    &(rpfRouteInfo.rpfNextHop)) == L7_FALSE ) || 
             (pStarStarRpEntry->pimsmStarStarRpRPFIfIndex !=
               rpfRouteInfo.rpfIfIndex))
          {
             pimsmStarStarRpTreeRTOCommon(pimsmCb, pStarStarRpNode, &rpfRouteInfo);
          }
          else
          {
            /* Copy new RPF info to the (*,*,RP) node */
            pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric = 
                  rpfRouteInfo.rpfRouteMetric;
            pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref = 
                  rpfRouteInfo.rpfRouteMetricPref;
          }
          
          break;
        default:
          break;
      }      
    }
    rc = pimsmStarStarRpNextGet(pimsmCb, pStarStarRpNode, 
                           &pStarStarRpNode);
  }
  return L7_SUCCESS;
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
L7_RC_t pimsmStarStarRpNegativeDelete (pimsmCB_t *pimsmCb,
                                       L7_inet_addr_t *pGrpAddr)
{
  pimsmStarStarRpNode_t *currStarStarRpNode = L7_NULLPTR;
  pimsmStarStarRpNode_t *nextStarStarRpNode = L7_NULLPTR;
  pimsmCache_t *currKernelCacheEntry = L7_NULLPTR;
  pimsmCache_t *nextKernelCacheEntry = L7_NULLPTR;
 	L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pGrpAddr))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmStarStarRpFirstGet(pimsmCb,&nextStarStarRpNode);

  while(rc == L7_SUCCESS)
  {
    currStarStarRpNode = nextStarStarRpNode;

    rc = pimsmStarStarRpCacheGetFirst(pimsmCb, currStarStarRpNode,
                                      &nextKernelCacheEntry);
    while (rc == L7_SUCCESS)
    {
      currKernelCacheEntry = nextKernelCacheEntry;
      if(PIMSM_INET_IS_ADDR_EQUAL(pGrpAddr,
                                  &currKernelCacheEntry->pimsmGrpAddr) == L7_TRUE)
      {
        if((currStarStarRpNode->flags & PIMSM_NULL_OIF) != L7_NULL)
        {
          pimsmStarStarRpMFCUpdate(pimsmCb, currStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE);
        }
      }
      rc = pimsmStarStarRpCacheNextGet (pimsmCb, currStarStarRpNode, currKernelCacheEntry,
                                        &nextKernelCacheEntry);
    }
    rc = pimsmStarStarRpNextGet(pimsmCb, currStarStarRpNode, &nextStarStarRpNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update 
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param    pStarStarRpNode  @b{(input)} Group IP Addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t pimsmStarStarRpCacheNegativeDelete (pimsmCB_t *pimsmCb,
                                       pimsmStarStarRpNode_t *pStarStarRpNode)
{
  pimsmStarStarRpNode_t *currStarStarRpNode = L7_NULLPTR;
  pimsmStarStarRpNode_t *nextStarStarRpNode = L7_NULLPTR;
  pimsmCache_t *currKernelCacheEntry = L7_NULLPTR;
  pimsmCache_t *nextKernelCacheEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == pimsmCb) || (L7_NULLPTR == pStarStarRpNode))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "Invalid input parameters");
    return L7_FAILURE;
  }
  rc = pimsmStarStarRpFirstGet(pimsmCb,&nextStarStarRpNode);

  while(rc == L7_SUCCESS)
  {
    currStarStarRpNode = nextStarStarRpNode;

    rc = pimsmStarStarRpCacheGetFirst(pimsmCb, currStarStarRpNode,
                                      &nextKernelCacheEntry);
    while (rc == L7_SUCCESS)
    {
      currKernelCacheEntry = nextKernelCacheEntry;
      if((currStarStarRpNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        pimsmStarStarRpMFCUpdate(pimsmCb, currStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE);
      }
      rc = pimsmStarStarRpCacheNextGet (pimsmCb, currStarStarRpNode, currKernelCacheEntry,
                                        &nextKernelCacheEntry);
    }
    rc = pimsmStarStarRpNextGet(pimsmCb, currStarStarRpNode, &nextStarStarRpNode);
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Initializes the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarStarRpNode  @b{(input)} (*,*,RP) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpCacheInit(pimsmCB_t * pimsmCb, 
                             pimsmStarStarRpNode_t * pStarStarRpNode)
{

  return pimsmCacheInit(pimsmCb, 
               &pStarStarRpNode->pimsmStarStarRpCacheList);
}

/******************************************************************************
* @purpose  DeInitializes the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarStarRpNode  @b{(input)} (*,*,RP) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpCacheDeInit(pimsmCB_t * pimsmCb, 
                               pimsmStarStarRpNode_t * pStarStarRpNode)
{

   return pimsmCacheDeInit(pimsmCb,
               &(pStarStarRpNode->pimsmStarStarRpCacheList));
}


/******************************************************************************
* @purpose  Adds a kernelel cache entry to the Kernel Cache List
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    pStarStarRpNode  @b{(input)} (*,*,RP) node
* @param    pSrcAddr    @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpCacheAdd(pimsmCB_t * pimsmCb, 
                            pimsmStarStarRpNode_t    * pStarStarRpNode,
                            L7_inet_addr_t      * pSrcAddr,
                            L7_inet_addr_t      * pGrpAddr)
{

   return pimsmCacheAdd(pimsmCb,
               &(pStarStarRpNode->pimsmStarStarRpCacheList),
               pSrcAddr,
               pGrpAddr);
}

/******************************************************************************
* @purpose  Deletes a kernelel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarStarRpNode        @b{(input)} (*,*,RP) node
* @param    kernelCacheEntry  @b{(input)} kernelCache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpCacheDelete(pimsmCB_t * pimsmCb, 
                               pimsmStarStarRpNode_t * pStarStarRpNode,
                               pimsmCache_t  * kernelCacheEntry)
{
      return pimsmCacheDelete(pimsmCb, 
               &(pStarStarRpNode->pimsmStarStarRpCacheList),
               kernelCacheEntry,
               &pStarStarRpNode->flags);
}


/******************************************************************************
* @purpose  Return the first kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarStarRpNode        @b{(input)} (*,*,RP) node
* @param    kernelCacheEntry  @b{(input)}  pointer to kernelCache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpCacheGetFirst(pimsmCB_t * pimsmCb, 
                                 pimsmStarStarRpNode_t * pStarStarRpNode,
                                 pimsmCache_t  ** kernelCacheEntry)
{

   return pimsmCacheGetFirst(pimsmCb,
               &(pStarStarRpNode->pimsmStarStarRpCacheList),
               kernelCacheEntry);
}


/******************************************************************************
* @purpose  Returns the next kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb                @b{(input)}  control block  
* @param    pStarStarRpNode             @b{(input)} (*,*,RP) node
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
L7_RC_t pimsmStarStarRpCacheNextGet(pimsmCB_t * pimsmCb, 
                                pimsmStarStarRpNode_t * pStarStarRpNode,
                                pimsmCache_t  * currentKernCacheEntry,
                                pimsmCache_t  ** nextKernCacheEntry)
{

   return pimsmCacheNextGet(pimsmCb, 
               &(pStarStarRpNode->pimsmStarStarRpCacheList),
               currentKernCacheEntry, nextKernCacheEntry);
}

/******************************************************************************
* @purpose  Find and return the kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    pStarStarRpNode        @b{(input)} (*,*,RP) node
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
L7_RC_t pimsmStarStarRpCacheFind(pimsmCB_t * pimsmCb, 
                             pimsmStarStarRpNode_t * pStarStarRpNode,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t      *pGrpAddr,
                             pimsmCache_t  ** kernelCacheEntry)
{

   return pimsmCacheFind(pimsmCb, 
               &(pStarStarRpNode->pimsmStarStarRpCacheList),
               pSrcAddr,
               pGrpAddr,
               kernelCacheEntry);
}



/*********************************************************************
* @purpose  Test whether  mrt entry from the (*,*,RP) entry table can be deleted
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarStarRpNode  @b{(input)} (*,*,RP) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimsmStarStarRpNodeCanRemove(pimsmCB_t * pimsmCb, pimsmStarStarRpNode_t *pStarStarRpNode)
{

  pimsmStarStarRpEntry_t  *pStarStarRpEntry;
  pimsmStarStarRpIEntry_t  *pStarStarRpIEntry;  
  L7_uint32 index;

  for(index = 0; index < MCAST_MAX_INTERFACES; index++)
  {
     pStarStarRpIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[index];
     if ( pStarStarRpIEntry != L7_NULLPTR)
     {
        if((pStarStarRpIEntry->pimsmStarStarRpIJoinPruneState !=
              PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO) ||
           (pStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimer != L7_NULLPTR) || 
           (pStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimer != L7_NULLPTR))
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, 
            "(*,*,RP, %d) is active", index);        
          return L7_FALSE; 
        }
     }
  }
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
  if((pStarStarRpEntry->pimsmStarStarRpUpstreamJoinState !=
        PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED) ||
     (pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer != L7_NULLPTR))
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO, 
            "(*,*,RP) is active");
    return L7_FALSE; 
  } 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_NORMAL, 
            "(*,*,RP) is in-active");        
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL,
        "RP Address :", &pStarStarRpEntry->pimsmStarStarRpRPAddress);
  
  return L7_TRUE;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (*,*,RP) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarStarRpNode  @b{(input)} (*,*,RP) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL pimsmStarStarRpNodeTryRemove(pimsmCB_t * pimsmCb, 
        pimsmStarStarRpNode_t *pStarStarRpNode)
{
  L7_BOOL retVal;
  
  retVal = pimsmStarStarRpNodeCanRemove(pimsmCb, pStarStarRpNode);
  if(retVal == L7_TRUE) 
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_NORMAL, 
        "StarStarRpNodeCanRemove = %d", retVal);
    pStarStarRpNode->flags |=  PIMSM_NODE_DELETE;  
  }
  return retVal;
}


/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimsmCb    @b{(input)} PIM SM Control Block
* @param pStarStarRpNode  @b{(input)} (S,G) Node  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void pimsmStarStarRpNodeCleanup(pimsmCB_t * pimsmCb)
{
  L7_RC_t    rc;
  pimsmStarStarRpNode_t *pStarStarRpNode, *pStarStarRpNodeDelete = L7_NULLPTR;
  L7_inet_addr_t *pRpAddr;
   rc = pimsmStarStarRpFirstGet( pimsmCb, &pStarStarRpNode );
  while(rc == L7_SUCCESS)
  {
    if((pStarStarRpNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
    {
     pStarStarRpNodeDelete =  pStarStarRpNode;
    }
    rc = pimsmStarStarRpNextGet( pimsmCb, pStarStarRpNode, &pStarStarRpNode );
    if(pStarStarRpNodeDelete != L7_NULLPTR)
    {
      pRpAddr= &(pStarStarRpNodeDelete->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_NORMAL,
              "(*,*,RP) is deleted for RP :\n", 
                       pRpAddr);
      pimsmStarStarRpDelete(pimsmCb, pStarStarRpNodeDelete);
      pStarStarRpNodeDelete = L7_NULLPTR;
    }    
  }
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
void  pimsmStarStarRpNextHopUpdate(pimsmCB_t * pimsmCb,
                         L7_uint32 rtrIfNum,
                         pimsmNeighborEntry_t  *pimsmNbrEntry)
{
  pimsmStarStarRpNode_t        *pStarStarRpNode;
  pimsmStarStarRpEntry_t *      pStarStarRpEntry;
  L7_RC_t               rc;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRpEventInfo;
  L7_BOOL joinDesired;
  L7_inet_addr_t primaryAddr;
  
  if(pimsmCb->pimsmStarStarRpNextHopUpdateCnt <= 0)
  {
      return;
  }
  
  rc = pimsmStarStarRpFirstGet( pimsmCb, &pStarStarRpNode );
  while(rc == L7_SUCCESS)
  {    
    pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;

    if(pStarStarRpEntry->pimsmStarStarRpRPFIfIndex != rtrIfNum)
    {
       rc = pimsmStarStarRpNextGet( pimsmCb, pStarStarRpNode, &pStarStarRpNode );
       continue;
    }
    if (pimsmCb->family == L7_AF_INET6 && 
     inetIsAddressZero(&pStarStarRpEntry->pimsmStarStarRpRPFNextHop) == L7_FALSE &&
      L7_IP6_IS_ADDR_LINK_LOCAL(
          &pStarStarRpEntry->pimsmStarStarRpRPFNextHop.addr.ipv6))
    {
       rc = pimsmStarStarRpNextGet( pimsmCb, pStarStarRpNode, &pStarStarRpNode );
       continue;
    }    
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_NORMAL, 
        "(*,*,RP) Getting primary addr");
    if(pimsmNbrPrimaryAddrGet(pimsmCb,
            pimsmNbrEntry, &pStarStarRpEntry->pimsmStarStarRpRPFNextHop,
            &primaryAddr) != L7_SUCCESS)
    {
       rc = pimsmStarStarRpNextGet( pimsmCb, pStarStarRpNode, &pStarStarRpNode );
       continue;
    }
    inetCopy(&pStarStarRpEntry->pimsmStarStarRpRPFNextHop,&primaryAddr);
    pimsmCb->pimsmStarStarRpNextHopUpdateCnt--;
    pimsmStarStarRpNeighborRpfDash(pimsmCb,pStarStarRpNode);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_NORMAL, 
        "(*,*,RP) primary addr updated");
    
    memset(&upStrmStarStarRpEventInfo,0,sizeof(pimsmUpStrmStarStarRPEventInfo_t));
    joinDesired = pimsmStarStarRpJoinDesired(pimsmCb, pStarStarRpNode);
    if (joinDesired == L7_TRUE)
    {
      upStrmStarStarRpEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else
    {
      upStrmStarStarRpEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode, &upStrmStarStarRpEventInfo);    
    if(pimsmStarStarRpMFCUpdate (pimsmCb,pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_ERROR, "MFC Updation failed");
    }
    rc = pimsmStarStarRpNextGet( pimsmCb, pStarStarRpNode, &pStarStarRpNode );
  }   
}

