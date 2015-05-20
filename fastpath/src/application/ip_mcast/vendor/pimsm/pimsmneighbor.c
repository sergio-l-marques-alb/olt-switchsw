/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmNeighbor.c
*
* @purpose Contains functions to operate on Neighbor list.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author vmurali/dsatyanarayana
* @end
*
**********************************************************************/
#include "pimsmmacros.h"
#include "buff_api.h"
#include "pimsmneighbor.h"
#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmbsr.h"
#include "pimsmdefs.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmproto.h"

/*********************************************************************
* @purpose  Compare the Neighbors
*
* @param pData1   @b{(input)} pointer to neighbor entry struct
         pData2   @b{(input)} pointer to neighbor entry struct
         keySize  @b{(input)} not used.
*
* @returns   an integer less than, equal to, or greater than zero
*
* @comments
*
* @end
*********************************************************************/
L7_int32 pimsmNeighborCompare(void *pData1, void *pData2,
      L7_uint32  keyLen)
{
  pimsmNeighborEntry_t * pNbrEntry1, *pNbrEntry2;
  L7_inet_addr_t  * pNbrAddr1, * pNbrAddr2;
  L7_uint32 result;

  if((pData1 == L7_NULLPTR) || (pData2 == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "Invalid input parameters");
    return(-1);
  }

  pNbrEntry1 = (pimsmNeighborEntry_t *)pData1;
  pNbrEntry2 = (pimsmNeighborEntry_t *)pData2;

  pNbrAddr1 = &pNbrEntry1->pimsmNeighborAddrList.pimsmPrimaryAddress;
  pNbrAddr2 = &pNbrEntry2->pimsmNeighborAddrList.pimsmPrimaryAddress;

  result =  L7_INET_ADDR_COMPARE(pNbrAddr1, pNbrAddr2);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "NBR Compare result is  %d", result);
  return(result);
}

/******************************************************************************
* @purpose  Free up the memory used for neighbor struct
*
* @param pNbrEntry   @b{(input)} pointer to neighbor entry struct
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmNeighborDestroy(L7_sll_member_t * pNbrEntry)
{
  L7_uchar8 addrFamily = 0;

  addrFamily = L7_INET_GET_FAMILY (&(((pimsmNeighborEntry_t*)pNbrEntry)->pimsmNeighborAddrList.pimsmPrimaryAddress));

  PIMSM_FREE (addrFamily, (void*) pNbrEntry);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initializes the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborInit(pimsmCB_t * pimsmCb,
         pimsmInterfaceEntry_t *pIntfEntry)
{

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR || pIntfEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"CB recvd is NULLPTR");
    return L7_FAILURE;
  }

  /* Create the Neighbor list */
  if(SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                L7_SLL_ASCEND_ORDER, 0,
                pimsmNeighborCompare,
                pimsmNeighborDestroy,
                &(pIntfEntry->pimsmNbrList)) != L7_SUCCESS)
  {

    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "PIMSM: Failed to create SLL for Neighbor\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Initializes the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborDeInit(pimsmCB_t * pimsmCb,
               pimsmInterfaceEntry_t *pIntfEntry)
{
  L7_RC_t rc;
  L7_sll_t * pNbrList;
  pimsmNeighborEntry_t *pCurrNbrEntry;

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  pNbrList = &(pIntfEntry->pimsmNbrList);
  if(pNbrList == (L7_sll_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Error: NBR List is NULL ");
    return L7_FAILURE;
  }

  if(osapiSemaTake(pimsmCb->pimsmNbrLock, L7_WAIT_FOREVER)
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaTake() failed");
    return L7_FAILURE;
  }

  pCurrNbrEntry = (pimsmNeighborEntry_t *)SLLFirstGet(pNbrList);
  while(pCurrNbrEntry != ( pimsmNeighborEntry_t * )L7_NULLPTR)
  {
    pimsmJPWorkingBuffReturn(pimsmCb, pCurrNbrEntry);
    pimsmUtilAppTimerCancel (pimsmCb, &(pCurrNbrEntry->pimsmNeighborExpiryTimer));
    handleListNodeDelete(pimsmCb->handleList,
                         &pCurrNbrEntry->pimsmNeighborExpiryTimerHandle);
    pCurrNbrEntry = (pimsmNeighborEntry_t *)SLLNextGet(pNbrList,
                        (L7_sll_member_t *)pCurrNbrEntry);
  }
  rc = SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                      &(pIntfEntry->pimsmNbrList));
  if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
    return L7_FAILURE;
  }
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Error: unable to Destroy NBR Linked List ");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds a neighbor to the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
* @param    pNbrEntry   @b{(input)} pointer to neighbor entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimsmNeighborAdd(pimsmCB_t * pimsmCb,
                                 pimsmInterfaceEntry_t *pIntfEntry,
                                pimsmNeighborEntry_t  * pNbrEntry)
{
  pimsmNeighborEntry_t * pNewNbrEntry;
  L7_BOOL               drChanged = L7_FALSE;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  if ((pNewNbrEntry = PIMSM_ALLOC (pimsmCb->family, sizeof (pimsmNeighborEntry_t)))
                                == L7_NULLPTR)
  {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory for Neighbor Entry");
      return L7_FAILURE;
  }

  memcpy(pNewNbrEntry, pNbrEntry, sizeof(pimsmNeighborEntry_t));
  pNewNbrEntry->pimsmNbrBufferPoolId = pimsmCb->pimsmNbrBufferPoolId;
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmNbrLock, L7_WAIT_FOREVER)
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaTake() failed");
    PIMSM_FREE (pimsmCb->family, (void*) pNewNbrEntry);
    return L7_FAILURE;
  }

  rc = SLLAdd (&(pIntfEntry->pimsmNbrList), (L7_sll_member_t *)pNewNbrEntry);

  if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
    return L7_FAILURE;
  }
  if(rc!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Error: unable to add to NBR Linked List ");
    PIMSM_FREE (pimsmCb->family, (void*) pNewNbrEntry);
    return L7_FAILURE;
  }

  pIntfEntry->pimsmNbrCount++;

  /* pimsmNeighborDRPriority == 0 means
     pimsmNeighborDRPriorityPresent is FALSE */

  if(pNbrEntry->pimsmNeighborDRPriority == 0 ||
     pIntfEntry->pimsmInterfaceDRPriority == 0)
  {
    /* The router with highest network address is the elected DR */
    if(L7_INET_ADDR_COMPARE(&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress,
                            &pIntfEntry->pimsmInterfaceDR) > 0)
    {
      drChanged = L7_TRUE;
      inetCopy(&pIntfEntry->pimsmInterfaceDR,
               &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
      pIntfEntry->pimsmInterfaceDRPriority = 0;
    }
  }
  else
  {
    if((pNbrEntry->pimsmNeighborDRPriority >
        pIntfEntry->pimsmInterfaceDRPriority) ||
       ((pNbrEntry->pimsmNeighborDRPriority ==
         pIntfEntry->pimsmInterfaceDRPriority) &&
        ((L7_INET_ADDR_COMPARE(&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress,
                               &pIntfEntry->pimsmInterfaceDR)> 0))))
    {
      drChanged = L7_TRUE;
      inetCopy(&pIntfEntry->pimsmInterfaceDR,
               &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
      pIntfEntry->pimsmInterfaceDRPriority =
      pNbrEntry->pimsmNeighborDRPriority;
    }
  }
  if(drChanged == L7_TRUE)
  {
    /* If the DR changed, Check with (S,G) Register FSMs (Could Register) */
    pimsmProcessDRChange(pimsmCb, pIntfEntry->pimsmInterfaceIfIndex);
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
              "Exit: Added to the list SUCCESSFULLY");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes a neighbor from the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
* @param    pNbrEntry   @b{(input)} pointer to neighbor entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborDelete(pimsmCB_t * pimsmCb,
                            pimsmInterfaceEntry_t *pIntfEntry,
                            pimsmNeighborEntry_t  * pNbrEntry)
{
  L7_RC_t rc;
  L7_BOOL updateDR = L7_FALSE;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Enter");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                   "Neighbor Address to Delete :",
                   &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);


  pimsmJPWorkingBuffReturn(pimsmCb, pNbrEntry);

  if(L7_INET_IS_ADDR_EQUAL(&pIntfEntry->pimsmInterfaceDR,
                           &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress) == L7_TRUE)
  {
    updateDR = L7_TRUE;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pNbrEntry->pimsmNeighborExpiryTimer));
  handleListNodeDelete(pimsmCb->handleList,
                       &pNbrEntry->pimsmNeighborExpiryTimerHandle);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmNbrLock, L7_WAIT_FOREVER)
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaTake() failed");
    return L7_FAILURE;
  }

  rc = SLLDelete(&(pIntfEntry->pimsmNbrList), (L7_sll_member_t *)pNbrEntry);

  if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
    return L7_FAILURE;
  }
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Error: delete failed NBR node ");
    return L7_FAILURE;
  }
  if(updateDR == L7_TRUE)
  {
    pimsmIntfDRUpdate(pimsmCb, pIntfEntry);
  }
  pIntfEntry->pimsmNbrCount--;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Exit - SUCCESS ");
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Returns the first neighbor from the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
* @param    pNbrEntry   @b{(input)} pointer to neighbor entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborGetFirst(pimsmCB_t * pimsmCb,
                              pimsmInterfaceEntry_t *pIntfEntry,
                              pimsmNeighborEntry_t **pNbrEntry)
{
  L7_sll_t  * pNbrList;
  pimsmNeighborEntry_t  *pNbrTempEntry;

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  if(&(pIntfEntry->pimsmNbrList) == (L7_sll_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
        "NbrGetFirst recvd LIST is NULLPTR");
    return L7_FAILURE;
  }
  pNbrList = &(pIntfEntry->pimsmNbrList);

  pNbrTempEntry = (pimsmNeighborEntry_t *)SLLFirstGet(pNbrList);

  if(pNbrTempEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG ,
        "NBR List First node is NULLPTR");
    return L7_FAILURE;
  }
  if (pNbrEntry != L7_NULLPTR)
  {
   *pNbrEntry = pNbrTempEntry;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the next neighbor from the Neighbor List
*
* @param    pimsmCb        @b{(input)}  control block
* @param    pIntfEntry     @b{(input)} interface entry
*           pCurrNbrEntry  @b{(input)} pointer to current neighbor
                           entry struct
*           ppNextNbrEntry @b{(output)} addr of pointer to next
                           neighbor entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborNextGet(pimsmCB_t * pimsmCb,
                             pimsmInterfaceEntry_t *pIntfEntry,
                             pimsmNeighborEntry_t  * pCurrNbrEntry,
                             pimsmNeighborEntry_t  ** ppNextNbrEntry)
{
  L7_sll_t  * pNbrList = L7_NULLPTR;
  pimsmNeighborEntry_t  *pNbrEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Entry");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pCurrNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  pNbrList = &(pIntfEntry->pimsmNbrList);
  if(pNbrList == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "NBR List is NULL");
    return L7_FAILURE;
  }

  pNbrEntry = (pimsmNeighborEntry_t *)SLLNextGet(pNbrList,
                     (L7_sll_member_t *)pCurrNbrEntry);


  if(pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG ,"NBR List Next node is NULLPTR");
    return L7_FAILURE;
  }
  if(ppNextNbrEntry != L7_NULLPTR)
  {
      *ppNextNbrEntry = pNbrEntry;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finds and returns a neighbor from the Neighbor List
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*           pSrcAddr    @b{(input)} address of neighbor to find
*           pNbrEntry   @b{(output)} addr of pointer to
                           neighbor entry struct
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmNeighborFind(pimsmCB_t * pimsmCb,
                  pimsmInterfaceEntry_t *pIntfEntry,
                  L7_inet_addr_t *pSrcAddr,
                  pimsmNeighborEntry_t ** ppNbrEntry)
{
  pimsmNeighborEntry_t nbrEntry;
  L7_sll_t  * pNbrList;
  pimsmNeighborEntry_t  *pNbrEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Enter ");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR || pSrcAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }
  memset(&nbrEntry, 0, sizeof(pimsmNeighborEntry_t));
  pNbrList = &(pIntfEntry->pimsmNbrList);
  inetCopy(&nbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress, pSrcAddr);

  pNbrEntry= (pimsmNeighborEntry_t *)SLLFind(pNbrList,
                     (L7_sll_member_t *)&nbrEntry);

  if(pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "NBR Find ret NULL ");
    return L7_FAILURE;
  }
  if(ppNbrEntry != L7_NULLPTR)
  {
    *ppNbrEntry = pNbrEntry;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "EXIT ");
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose part of Neighbor liveness timer expiry handler.
*
* @param    pimsmCb     @b{(input)}  control block
*           pSrcAddr    @b{(input)} address of neighbor
* @param    rtrIfNum    @b{(input)} router interface number
* @param
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmNLTExpiresPostEvent(pimsmCB_t *pimsmCb,
             L7_inet_addr_t * pSrcAddr, L7_uint32 rtrIfNum)
{
  pimsmStarGIEntry_t  * pStarGIEntry;
  pimsmSGIEntry_t  * pSGIEntry;
  L7_RC_t rc;
  pimsmStarGNode_t            * pStarGNode;
  pimsmSGNode_t           * pSGNode;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  pimsmSGNode_t *pSGNodeDelete = L7_NULLPTR;
  pimsmStarGNode_t *pStarGNodeDelete = L7_NULLPTR;
  pimsmStarGEntry_t  *pStarGEntry = L7_NULLPTR;
  mcastRPFInfo_t rpfRouteInfo;
  pimsmSGEntry_t  *pSGEntry = L7_NULLPTR;
  pimsmUpStrmSGEventInfo_t  upStrmSGEventInfo;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;

  if(pimsmCb == L7_NULLPTR || pSrcAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }


  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while(rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry != L7_NULLPTR &&
      inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress)
         == L7_FALSE &&
       L7_INET_IS_ADDR_EQUAL(&pStarGIEntry->pimsmStarGIAssertWinnerAddress,
                             pSrcAddr) == L7_TRUE)
    {
      memset(&starGAssertEventInfo, 0,
         sizeof(pimsmPerIntfStarGAssertEventInfo_t));
      starGAssertEventInfo.rtrIfNum = rtrIfNum;
      starGAssertEventInfo.eventType =
          PIMSM_ASSERT_STAR_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED;
      inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
      pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
      /*delete negative entries from MFC*/
      if((pStarGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                     "deleting MFC entry" );
        if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_DELETE_ENTRY, L7_FALSE)
            != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
               "Failed to delete (*,G) from MFC");
        }
      }
      /* TODO: Delete all NEGATIVE (S,G) entires too */
      pimsmSGNegativeDelete(pimsmCb,
       &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmSGRptNegativeDelete(pimsmCb,
       &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmStarStarRpNegativeDelete (pimsmCb,
       &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    }
    if (L7_INET_IS_ADDR_EQUAL(&pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor,
                              pSrcAddr) == L7_TRUE)
    {
     /*TODO: Post Upstream (*,G) RPF change event */
     if(mcastRPFNeighborGet(&pStarGNode->pimsmStarGEntry.pimsmStarGRPAddress,
           &rpfRouteInfo, &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor,
           pimsmIsNbrPresent) == L7_SUCCESS)
     {
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
        pStarGEntry = &pStarGNode->pimsmStarGEntry;
        pStarGEntry->pimsmStarGRPFIfIndex
        = rpfRouteInfo.rpfIfIndex;
        inetCopy(&pStarGEntry->pimsmStarGRPFNextHop,
                 &rpfRouteInfo.rpfNextHop);
        inetCopy(&pStarGEntry->pimsmStarGRPFRouteAddress,
                 &rpfRouteInfo.rpfRouteAddress);
        pStarGEntry->pimsmStarGRPFRouteMetricPref
        = rpfRouteInfo.rpfRouteMetricPref;
        pStarGEntry->pimsmStarGRPFRouteMetric
        = rpfRouteInfo.rpfRouteMetric;
        pStarGEntry->pimsmStarGRPFRouteProtocol
        =  rpfRouteInfo.rpfRouteProtocol;
        pStarGEntry->pimsmStarGRPFRoutePrefixLength = rpfRouteInfo.prefixLength;
        upStrmStarGEventInfo.eventType =
        PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT;
        pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,
                                &upStrmStarGEventInfo);
        if(pimsmSGRptTreeBestRouteChngUpdate(pimsmCb,
             &pStarGEntry->pimsmStarGGrpAddress,
             rpfRouteInfo.rpfIfIndex,  rpfRouteInfo.status)
           != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_RTO_INTERACTION, PIMSM_TRACE_ERROR,
             "Failed to update (S,G,Rpt) with RPF change");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
     }
     else
     {
      pStarGNodeDelete = pStarGNode;
     }
    }
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
    /* Delete the (*,G) node if the upstream interface is Down.
     */
    if(pStarGNodeDelete != L7_NULLPTR)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "(*,G) is deleted for Group :\n",
                       &pStarGNodeDelete->pimsmStarGEntry.pimsmStarGGrpAddress);
      pimsmStarGDelete(pimsmCb, &pStarGNodeDelete->pimsmStarGEntry.pimsmStarGGrpAddress);
    }
  }

  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while(rc == L7_SUCCESS)
  {
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    pSGEntry = &pSGNode->pimsmSGEntry;
    if(pSGIEntry != L7_NULLPTR &&
      inetIsAddressZero(&pSGIEntry->pimsmSGIAssertWinnerAddress) == L7_FALSE &&
       L7_INET_IS_ADDR_EQUAL(&pSGIEntry->pimsmSGIAssertWinnerAddress, pSrcAddr) == L7_TRUE)
    {
      memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
      sgAssertEventInfo.rtrIfNum = rtrIfNum;
      sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED;
      pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
      /* delete negative entries from MFC*/
      if((pSGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                     "deleting MFC entry" );
        pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
      }
    }
    if (L7_INET_IS_ADDR_EQUAL(&pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor,
                              pSrcAddr) == L7_TRUE)
    {
        /*TODO: Post Upstream (S,G) RPF change event */
      if(mcastRPFNeighborGet(&pSGNode->pimsmSGEntry.pimsmSGSrcAddress, &rpfRouteInfo,
            &pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor,pimsmIsNbrPresent) ==  L7_SUCCESS)
      {
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
         pSGEntry = &pSGNode->pimsmSGEntry;
         pSGEntry->pimsmSGRPFIfIndex
         = rpfRouteInfo.rpfIfIndex;
         inetCopy(&pSGEntry->pimsmSGRPFNextHop,
                  &rpfRouteInfo.rpfNextHop);
         inetCopy(&pSGEntry->pimsmSGRPFRouteAddress,
                  &rpfRouteInfo.rpfRouteAddress);
         pSGEntry->pimsmSGRPFRouteMetricPref
         = rpfRouteInfo.rpfRouteMetricPref;
         pSGEntry->pimsmSGRPFRouteMetric
         = rpfRouteInfo.rpfRouteMetric;
         pSGEntry->pimsmSGRPFRouteProtocol
         =  rpfRouteInfo.rpfRouteProtocol;
         pSGEntry->pimsmSGRPFRoutePrefixLength = rpfRouteInfo.prefixLength;
         upStrmSGEventInfo.eventType =
         PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT;
         pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
      }
      else
      {
         pSGNodeDelete = pSGNode;
      }
    }
    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
    if(pSGNodeDelete != L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"Deleting the (S,G)node");
      pimsmSGDelete (pimsmCb, &pSGNodeDelete->pimsmSGEntry.pimsmSGGrpAddress,
                     &pSGNodeDelete->pimsmSGEntry.pimsmSGSrcAddress);
      pSGNodeDelete = L7_NULLPTR;
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Neighbor liveness timer expiry handler.
*
* @param    pParam     @b{(input)}  timer-data handle
*
* @returns  void
*
* @comments
*
* @end
******************************************************************************/
static void pimsmNeighborTimerExpiresHandler(void *pParam)
{
  L7_uint32 rtrIfNum;
  L7_int32      handle = (L7_int32)pParam;
  pimsmTimerData_t *pTimerData;
  pimsmCB_t * pimsmCb;
  pimsmInterfaceEntry_t * pIntfEntry = L7_NULLPTR;
  pimsmNeighborEntry_t  *pCurrNbrEntry;
  L7_RC_t rc = L7_FAILURE;
  L7_inet_addr_t *pNbrAddr;
  if(pParam == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return;
  }

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pNbrAddr = &pTimerData->addr1;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, PIMSM_TRACE_INFO,
     "Neighbor Address :", pNbrAddr);
  if (pimsmIntfNbrEntryGet(pimsmCb, rtrIfNum, pNbrAddr, &pCurrNbrEntry)
        != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,PIMSM_TRACE_NORMAL,
        "Neighbor find failed");
    return;
  }
  if (pCurrNbrEntry->pimsmNeighborExpiryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmNeighborExpiryTimer is NULL, But Still Expired");
    return;
  }
  pCurrNbrEntry->pimsmNeighborExpiryTimer = L7_NULLPTR;

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL, "pIntfEntry is NULL");
    return;
  }
  rc = pimsmMapPimTrapNeighborLoss(pimsmCb->family, rtrIfNum, pCurrNbrEntry->pimsmNeighborCreateTime);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "pimsmMapPimTrapNeighborLoss failed.");
  }
  pimsmNLTExpiresPostEvent(pimsmCb, &pCurrNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress,
            rtrIfNum);
  pimsmNeighborDelete(pimsmCb, pIntfEntry, pCurrNbrEntry);
}
/******************************************************************************
* @purpose  Find secondary addr conflict and resolve.
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*           secAddr     @b{(input)} address of secondary addr
*           primaryAddr @b{(input)} address of primary addr
*
* @returns  void
*
* @comments
section 4.3.4.
If there is a conflict and the same
   secondary address was previously advertised by another neighbor, then
   only the most recently received mapping MUST be maintained, and an
   error message SHOULD be logged to the administrator in a rate-limited
   manner.
*
* @end
******************************************************************************/
static void pimsmNbrSecondaryAddrConflictResolve(pimsmCB_t *pimsmCb,
                         pimsmInterfaceEntry_t *pIntfEntry,
                         L7_inet_addr_t secAddr,
                         L7_inet_addr_t primaryAddr)
{
  pimsmNeighborEntry_t  *pimsmNbrEntry = L7_NULLPTR;
  L7_RC_t rc;
  L7_uint32 i;

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return;
  }

  rc = pimsmNeighborGetFirst(pimsmCb, pIntfEntry, &pimsmNbrEntry);
  while (rc == L7_SUCCESS)
  {
    if (L7_INET_ADDR_COMPARE(&pimsmNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress,
            &primaryAddr) == 0)
      {
        rc = pimsmNeighborNextGet(pimsmCb, pIntfEntry, pimsmNbrEntry,
                 &pimsmNbrEntry);
        continue;
      }
    for (i = 0; i < PIMSM_MAX_NBR_SECONDARY_ADDR &&
         inetIsAddressZero(&pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i])
            == L7_FALSE ; i++)
      {
        if (pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] == L7_FALSE)
        {
            continue;
        }
        if (L7_INET_ADDR_COMPARE(&pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i],
         &secAddr) == 0)
        {
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                 " conflict Secondary Addr  = ",
                 &pimsmNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
          PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
               "PIMSM: Secondary Addr conflict on rtrIfNum (%d)",
                 pIntfEntry->pimsmInterfaceIfIndex);
        pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] = L7_FALSE;
        }
      }
   rc = pimsmNeighborNextGet(pimsmCb, pIntfEntry, pimsmNbrEntry, &pimsmNbrEntry);
  }

}
/******************************************************************************
* @purpose  Secondary addr list processing.
*
* @param    pimsmCb       @b{(input)}  control block
* @param    pIntfEntry    @b{(input)} interface entry
* @param    pNbrEntry     @b{(input)} pointer to neighbor entry struct
* @param    pHelloParams  @b{(input)} hello params
*
* @returns  void
*
* @comments
*
* @end
******************************************************************************/
void pimsmNbrSecondaryAddrUpdate(pimsmCB_t * pimsmCb,
                         pimsmInterfaceEntry_t *pIntfEntry,
                         pimsmNeighborEntry_t *pNbrEntry,
                         pimsmHelloParams_t *pHelloParams)
{
  L7_uint32 i = 0, j = 0;

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Refreshing secondary addr lists ");
  /* Refresh the address list */
  memset(pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList,0,
      sizeof(pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList));
  for (i=0;i<PIMSM_MAX_NBR_SECONDARY_ADDR;i++)
  {
   inetAddressZeroSet(pimsmCb->family,&pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]);
   pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] = L7_FALSE;
  }
  i = 0;
  while(j<PIMSM_MAX_NBR_SECONDARY_ADDR &&
        inetIsAddressZero(&pHelloParams->addrList[j]) == L7_FALSE)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
      " helloParam addr = ",&pHelloParams->addrList[j]);
    /* Exclude Primary address if present in secondary address list */
    if (L7_INET_ADDR_COMPARE(&pHelloParams->addrList[j],
         &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress) != 0)
   {
     pimsmNbrSecondaryAddrConflictResolve(pimsmCb, pIntfEntry,
            pHelloParams->addrList[j],pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
     inetCopy(&pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i],&pHelloParams->addrList[j]);
     pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] = L7_TRUE;
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG," Sec addr = ",
            &pNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]);
     i++;
      j++;
      }
   else
   {
     j++;
     continue;
   }
  }
  pimsmSGNextHopUpdate(pimsmCb,pIntfEntry->pimsmInterfaceIfIndex, pNbrEntry);
  pimsmStarStarRpNextHopUpdate(pimsmCb,pIntfEntry->pimsmInterfaceIfIndex, pNbrEntry);
  pimsmStarGNextHopUpdate(pimsmCb, pIntfEntry->pimsmInterfaceIfIndex, pNbrEntry);
}

/*********************************************************************
* @purpose  Create a new neighbor
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pNbrEntry         @b{(input)} neighbor entry struct
* @param    pIntfEntry        @b{(input)} interface entry
* @param    pHelloParams      @b{(input)} hello params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        call this from receive-hello msg
* @end
*********************************************************************/
L7_RC_t pimsmNeighborCreate(pimsmCB_t * pimsmCb,L7_inet_addr_t * pNbrAddr,
                            pimsmInterfaceEntry_t *pIntfEntry,
                            pimsmHelloParams_t *pHelloParams)
{
  L7_uint32     now;
  pimsmNeighborEntry_t nbrEntry,* pNbrEntry= L7_NULLPTR;
  pimsmTimerData_t * pTimerData;
  L7_ushort16 holdtime = L7_NULL;
  L7_RC_t rc;
  L7_uint32 rtrIfNum;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Enter ");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pNbrAddr == L7_NULLPTR || pHelloParams == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  if(pHelloParams->holdtime == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "pHelloParams->holdtime = 0, not creating neighbor ");
    return L7_FAILURE;
  }

  rtrIfNum = pIntfEntry->pimsmInterfaceIfIndex;
  memset(&nbrEntry, 0, sizeof(pimsmNeighborEntry_t));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "NBR not found - creating new ");

  inetCopy(&nbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress, pNbrAddr);
  pimsmNbrSecondaryAddrUpdate(pimsmCb, pIntfEntry, &nbrEntry, pHelloParams);
  nbrEntry.pimsmNeighborIfIndex   = rtrIfNum;
  now = osapiUpTimeRaw();
  nbrEntry.pimsmNeighborCreateTime = now;
  nbrEntry.pimsmNeighborDRPriority = pHelloParams->drPriority;
  nbrEntry.pimsmNeighborGenerationIDValue = pHelloParams->genId;

  nbrEntry.pimsmNeighborLANPruneDelayPresent =
                                    pHelloParams->LANPruneDelayPresent;
  nbrEntry.pimsmNeighborTrackingSupportPresent =
                                    pHelloParams->trackingSupportPresent;
  nbrEntry.pimsmNeighborPropagationDelay = pHelloParams->propagationDelay;
  nbrEntry.pimsmNeighborOverrideInterval = pHelloParams->overrideIntvl;

  memset(&nbrEntry.pimsmBuildJPMsg, 0, sizeof(pimsmJPMsgStoreBuff_t));
  nbrEntry.pimsmBuildJPMsg.buildJPMsgUse = L7_FALSE;
  inetAddressZeroSet(pimsmCb->family, &nbrEntry.pimsmBuildJPMsg.currGrpAddr);
  holdtime = pHelloParams->holdtime;
  if(pimsmNeighborAdd(pimsmCb, pIntfEntry, &nbrEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "SLLAdd ret Error - Not added");
    return L7_FAILURE;
  }
  rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pNbrAddr, &pNbrEntry);
  if(rc == L7_SUCCESS)
  {
    /*pNbrEntry->pSelf = pNbrEntry;*/
    pNbrEntry->pimsmNeighborExpiryTimerHandle =
    handleListNodeStore(pimsmCb->handleList,
                        (void*)&pNbrEntry->pimsmNeighborExpiryTimerParam);
    pTimerData = &pNbrEntry->pimsmNeighborExpiryTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1,
      &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
    pTimerData->rtrIfNum = rtrIfNum;
    pTimerData->pimsmCb = pimsmCb;
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmNeighborTimerExpiresHandler,
                              (void*)pNbrEntry->pimsmNeighborExpiryTimerHandle,
                              holdtime,
                              &(pNbrEntry->pimsmNeighborExpiryTimer),
                              "SM-NLT")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "RegRate TimerAdd Failed");
      return L7_FAILURE;
    }
  }

  if ((pIntfEntry->pimsmLANPruneDelayPresent == L7_TRUE) &&
      (pHelloParams->LANPruneDelayPresent == L7_TRUE))
  {
    pIntfEntry->pimsmLANPruneDelayPresent = L7_TRUE;
  }
  else
  {
    pIntfEntry->pimsmLANPruneDelayPresent = L7_FALSE;
  }

  /*Get MGMD info*/
  /* Uptream Nbr might have changed.
   * Post an event to MGMD to get the Group membership details.
   */
  for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    pimsmInterfaceEntry_t *intfEntry = L7_NULLPTR;

    if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry) != L7_SUCCESS)
    {
      continue;
    }
    if (mgmdMapMRPGroupInfoGet (pimsmCb->family, rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                   "Failure in Posting the MGMD Info Get Event for rtrIfNum - %d",
                   rtrIfNum);
      continue;
    }
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Exit ");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive PIM Control pkt Processing.
*
* @param    pimsmCb     @b{(input)}  control block
*           pSrcAddr    @b{(input)} address of neighbor
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmGenIdChangePostEvent(pimsmCB_t *pimsmCb,
                                         L7_inet_addr_t * pSrcAddr,
                                         L7_uint32 rtrIfNum)
{
  pimsmStarStarRpNode_t   * pStarStarRpNode;
  pimsmStarStarRpEntry_t  * pStarStarRpEntry;
  pimsmStarGEntry_t  * pStarGEntry;
  pimsmSGEntry_t  * pSGEntry;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRPEventInfo;
  L7_RC_t rc;
  pimsmStarGNode_t            * pStarGNode;
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  pimsmSGNode_t           * pSGNode;
  pimsmUpStrmSGEventInfo_t        upStrmSGEventInfo;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;

  if(pimsmCb == L7_NULLPTR || pSrcAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  rc = pimsmStarStarRpFirstGet(pimsmCb, &pStarStarRpNode);
  while(rc == L7_SUCCESS)
  {
    pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
    if(inetIsAddressZero(&pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor)
         == L7_FALSE &&
       L7_INET_IS_ADDR_EQUAL(&pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor,
         pSrcAddr) == L7_TRUE)
    {
      memset(&upStrmStarStarRPEventInfo, 0 ,
         sizeof(pimsmUpStrmStarStarRPEventInfo_t));
      upStrmStarStarRPEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_GEN_ID_CHANGED;
      pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode,
         &upStrmStarStarRPEventInfo);
    }
    /*delete negative entries from MFC*/
    if((pStarStarRpNode->flags & PIMSM_NULL_OIF) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      if (pimsmStarStarRpMFCUpdate(pimsmCb,pStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE)
          != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
             "Failed to delete (*,G) from MFC");
      }
    }
    rc = pimsmStarStarRpNextGet(pimsmCb, pStarStarRpNode, &pStarStarRpNode);
  }

  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while(rc == L7_SUCCESS)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    if(inetIsAddressZero(&pStarGEntry->pimsmStarGUpstreamNeighbor) == L7_FALSE &&
       L7_INET_IS_ADDR_EQUAL(&pStarGEntry->pimsmStarGUpstreamNeighbor, pSrcAddr) == L7_TRUE)
    {
      memset(&upStrmStarGEventInfo, 0 , sizeof(pimsmUpStrmStarGEventInfo_t));
      upStrmStarGEventInfo.eventType =
      PIMSM_UPSTRM_STAR_G_SM_EVENT_GEN_ID_CHANGED;
      pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);
    }

    memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
    starGAssertEventInfo.rtrIfNum = rtrIfNum;
    starGAssertEventInfo.eventType =
        PIMSM_ASSERT_STAR_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED;
    inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);

    /*delete negative entries from MFC*/
    if((pStarGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_DELETE_ENTRY, L7_FALSE)
          != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
             "Failed to delete (*,G) from MFC");
      }
    }
    pimsmSGNegativeDelete(pimsmCb,
       &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    pimsmSGRptNegativeDelete(pimsmCb,
       &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    pimsmStarStarRpNegativeDelete (pimsmCb,
        &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
  }

  rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
  while(rc == L7_SUCCESS)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    if(inetIsAddressZero(&pSGEntry->pimsmSGUpstreamNeighbor) == L7_FALSE &&
       L7_INET_IS_ADDR_EQUAL(&pSGEntry->pimsmSGUpstreamNeighbor, pSrcAddr) == L7_TRUE)
    {
      memset(&upStrmSGEventInfo, 0 , sizeof(pimsmUpStrmSGEventInfo_t));
      upStrmSGEventInfo.eventType =
      PIMSM_UPSTRM_S_G_SM_EVENT_GEN_ID_CHANGED;
      pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
    }
    memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
    sgAssertEventInfo.rtrIfNum = rtrIfNum;
    sgAssertEventInfo.eventType =
        PIMSM_ASSERT_S_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED;
    pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
    /* delete negative entries from MFC*/
    if((pSGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
    }
    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create a new neighbor
*
* @param    pimsmCb         @b{(input)}  control block
* @param    pIntfEntry      @b{(input)} interface entry
* @param    pNbrEntry       @b{(input)} pointer to neighbor entry struct
* @param    pHelloParams    @b{(input)} hello params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        call this from receive-hello msg
* @end
*********************************************************************/
L7_RC_t pimsmNeighborUpdate(pimsmCB_t * pimsmCb,
                            pimsmInterfaceEntry_t *pIntfEntry,
                            pimsmNeighborEntry_t * pNbrEntry,
                            pimsmHelloParams_t *pHelloParams)
{
  L7_uint32 rtrIfNum;
  L7_RC_t rc;
  pimsmTimerData_t *pTimerData;
  L7_inet_addr_t   nbrAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Enter ");

  if(pimsmCb == L7_NULLPTR || pIntfEntry == L7_NULLPTR ||
     pNbrEntry == L7_NULLPTR || pHelloParams == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

  rtrIfNum = pIntfEntry->pimsmInterfaceIfIndex;

  /* We have the neighbor entry */
  if(0 == pHelloParams->holdtime)
  {
    /* Looks like we have a nice neighbor who is going down
     * and wants to inform us by sending "holdtime=0". Thanks
     * buddy and see you again!
     */
    inetCopy(&nbrAddr,&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
    rc = pimsmNeighborDelete(pimsmCb, pIntfEntry, pNbrEntry);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
            "pimsmNbrDelete() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
    pimsmNLTExpiresPostEvent (pimsmCb,
                              &nbrAddr,
                              rtrIfNum);

    rc = pimsmMapPimTrapNeighborLoss(pimsmCb->family, rtrIfNum, pNbrEntry->pimsmNeighborCreateTime);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                  "pimsmMapPimTrapNeighborLoss() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    /* Set the neighbor timer to the holdtime
         included in the Hello message */
    pTimerData = &pNbrEntry->pimsmNeighborExpiryTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1,
      &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
    pTimerData->pimsmCb = pimsmCb;
    pTimerData->rtrIfNum = rtrIfNum;

    if (pimsmUtilAppTimerSet (pimsmCb, pimsmNeighborTimerExpiresHandler,
                              (void*)pNbrEntry->pimsmNeighborExpiryTimerHandle,
                              pHelloParams->holdtime,
                              &(pNbrEntry->pimsmNeighborExpiryTimer),
                              "SM-NLT2")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                  "pimsmNeighborExpiryTimer TimerUpdate Failed");
      /*dont return failure, fallback to do other things */
    }
  }

  if(pNbrEntry->pimsmNeighborGenerationIDValue != 0 &&
     pHelloParams->genId !=0 &&
     pNbrEntry->pimsmNeighborGenerationIDValue != pHelloParams->genId)
  {
    /*Post events UP_STAR_STAR_RP_GEN_ID_CHANGED,
      UPSTRM_STAR_G_GEN_ID_CHANGES,
      UPSTRM_S_G_GEN_ID_CHANGED  ,
      ASSERT_S_G_CURR_WINNER_GEN_ID_CHANGED_OR_NLT_EXPIRES ,*/
    pimsmGenIdChangePostEvent(pimsmCb, &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress, rtrIfNum);
    pNbrEntry->pimsmNeighborGenerationIDValue = pHelloParams->genId;
    /* Since a new neighbour has come up, let it know your existence */
    rc = pimsmHelloSend(pimsmCb, rtrIfNum,
         &pIntfEntry->pimsmInterfaceAddr,
         pIntfEntry->pimsmInterfaceHelloHoldtime);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"pimsmHelloSend() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
    /* Unicast the BSM when a new neighbor comes up */
    pimsmBsrUnicastBSM(pimsmCb,
                       &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress,
                       rtrIfNum);
  }

  if(pNbrEntry->pimsmNeighborDRPriority != 0 &&
     pHelloParams->drPriority !=0 &&
     pNbrEntry->pimsmNeighborDRPriority != pHelloParams->drPriority)
  {
    /* change in the dr-priority */
    pNbrEntry->pimsmNeighborDRPriority = pHelloParams->drPriority;
    pimsmIntfDRUpdate(pimsmCb, pIntfEntry);
  }
  pimsmNbrSecondaryAddrUpdate(pimsmCb, pIntfEntry, pNbrEntry, pHelloParams);

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*           secAddr     @b{(input)} address of secondary addr
*           primaryAddr @b{(input)} address of primary addr
*
* @returns  void
*
* @comments
* @end
******************************************************************************/
L7_RC_t pimsmNbrPrimaryAddrGet(pimsmCB_t *pimsmCb,
                         pimsmNeighborEntry_t  *pimsmNbrEntry,
                         L7_inet_addr_t *secAddr,
                         L7_inet_addr_t *primaryAddr)
{
  L7_uint32 i;

  if(pimsmCb == L7_NULLPTR || secAddr == L7_NULLPTR ||
    pimsmNbrEntry == L7_NULLPTR || primaryAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL,
                "Invalid input parameters");
    return L7_FAILURE;
  }

 PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, PIMSM_TRACE_INFO,
 " Input secondary addr = ", secAddr);

 inetAddressZeroSet(pimsmCb->family, primaryAddr);
 for (i = 0; i < PIMSM_MAX_NBR_SECONDARY_ADDR &&
      inetIsAddressZero(&pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i])
         == L7_FALSE ; i++)
   {
     if (pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] == L7_FALSE)
     {
         continue;
     }
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, PIMSM_TRACE_INFO,
         " My neighbor addr[%d] = ", i)
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, PIMSM_TRACE_INFO,
         "",&pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]);
     if (L7_INET_ADDR_COMPARE(&pimsmNbrEntry->pimsmNeighborAddrList.pimsmSecondaryAddrList[i],
      secAddr) == 0)
     {
       inetCopy(primaryAddr,
            &pimsmNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
       PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, PIMSM_TRACE_INFO,
        " Updated Primary addr = ",
            &pimsmNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
       return L7_SUCCESS;
     }
   }
  return L7_FAILURE;
}


