
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmbsr.c
*
* @purpose Contains functions related to BSR Functionality .
*
* @component pimsm
*
* @comments 
*
* @create 08/05/2006
*
* @author Ramu/Satya
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmdefs.h"
#include "pimsmcontrol.h"
#include "pimsmrp.h"
#include "pimsmbsr.h"
#include "pimsmcandbsrfsm.h"
#include "pimsmnoncandbsrfsm.h"
#include "pimsmmacros.h"

/******************************************************************************
* @purpose  Find log2 of given number
*
* @param        num     @b{(input)} 
*
* @returns  L7_uint32 : (log2(num))
*
* @comments      
*       
* @end
******************************************************************************/
static
L7_uint32 pimsmLog2 ( L7_uint32 num )
{
  L7_uint32     log_mask;
  L7_uint32     log_of_2;

  log_mask = sizeof( num ) << 3;
  log_mask = ( 1 << ( log_mask - 1 ) );  /* Set the leftmost bit to `1` */
  for (log_of_2 = ( sizeof( num ) << 3 ) - 1 ; log_of_2; log_of_2--)
  {
    if (num & log_mask)
    {
      break;
    }
    else
    {
      log_mask >>= 1;  /* Start shifting `1` on right */
    }
  }
  return log_of_2;
}

/******************************************************************************
* @purpose  This function is used to Compare the Fragment group nodes.
*
* @param     node1      @b{(input)} Pointer to the Fragmented Group node.
* @param     node2      @b{(input)} Pointer to the Fragmented Group node.
* @param     len        @b{(input)} len of the node to compare.
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2. 
*
* @comments  
*       
* @end
******************************************************************************/
static
L7_int32 pimsmBsrFragGrpNodeCompare(void *node1, void *node2,L7_uint32 len)
{
  pimsmFragGrpRpNode_t  *fragGrpEntry1 = (pimsmFragGrpRpNode_t*)node1;
  pimsmFragGrpRpNode_t  *fragGrpEntry2 = (pimsmFragGrpRpNode_t*)node2;

  return inetAddrCompareAddrWithMask(&fragGrpEntry1->pimsmGrpAddr,
                                     fragGrpEntry1->pimsmGrpMaskLength,
                                     &fragGrpEntry2->pimsmGrpAddr,
                                     fragGrpEntry2->pimsmGrpMaskLength);
}
/******************************************************************************
* @purpose  This function is used to Compare the Fragment Rp nodes.
*
* @param     node1      @b{(input)} Pointer to the Fragmented RP node.
* @param     node2      @b{(input)} Pointer to the Fragmented RP node
* @param     len        @b{(input)} len of the node to compare.
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2. 
*
* @comments  
*       
* @end
******************************************************************************/
static
L7_int32 pimsmBsrFragRpNodeCompare(void *node1, void *node2,L7_uint32 len)
{
  pimsmFragRpNode_t  *fragRpEntry1 = (pimsmFragRpNode_t *)node1;
  pimsmFragRpNode_t  *fragRpEntry2 = (pimsmFragRpNode_t *)node2;

  return L7_INET_ADDR_COMPARE(&fragRpEntry1->pimsmRpAddr, 
                              &fragRpEntry2->pimsmRpAddr);
}
/******************************************************************************
* @purpose  This function is used to delete the BSR node from the list
*
* @param    entry     @b{(input)} Pointer to the BSR node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static 
L7_RC_t pimsmBsrNodeDelete(pimsmCB_t *pimsmCb,pimsmBsrPerScopeZone_t  *bsrEntry)
{
  
  /* free the grp-rp list in the bsr node */
  SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
             &(bsrEntry->pimsmBSRFragInfo.pimsmGrpRpList));

   /* free the fragmented RpGrp List*/
  SLLPurge(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(bsrEntry->pimsmBSRFragInfo.pimsmGrpRpList));

  /* delete the timers */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," BSR timer deleted ");
  pimsmUtilAppTimerCancel (pimsmCb, &(bsrEntry->pimsmBSRTimer));

  if (bsrEntry->pimsmBSRType == PIMSM_BSR_TYPE_NONBSR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,"BSR Scope zone timer deleted ");
    pimsmUtilAppTimerCancel (pimsmCb, &(bsrEntry->zone.nbsr.pimsmBSRScopeZoneExpireTimer));
  }

  /* remove the handle */
  handleListNodeDelete(bsrEntry->pimsmCb->handleList,
                         &bsrEntry->pimsmBSRTimerHandle);
  /* Delete the entry */
  memset(bsrEntry, 0, sizeof(pimsmBsrPerScopeZone_t));
  inetAddressZeroSet(pimsmCb->family, &bsrEntry->pimsmBSRAddr);
  inetAddressZeroSet(pimsmCb->family, &bsrEntry->pimsmBSRGroupRange);
  inetAddressZeroSet(pimsmCb->family, 
                       &bsrEntry->zone.cbsr.pimsmElectedBSRAddress);  
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This function is used to delete the Fragmented Group node
*
* @param    entry     @b{(input)} Pointer to the Fragmented Group node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static 
L7_RC_t pimsmBsrFragGrpNodeDelete(L7_sll_member_t *entry)
{
  pimsmFragGrpRpNode_t  *fragGrpNode = (pimsmFragGrpRpNode_t *)entry;
  L7_uchar8 addrFamily = 0;

  /* delete the rp list of the group node */
  SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(fragGrpNode->pimsmRpList)); 

  addrFamily = L7_INET_GET_FAMILY (&(((pimsmFragGrpRpNode_t*)entry)->pimsmGrpAddr));

  /* free the entry */
  PIMSM_FREE (addrFamily, (void*) fragGrpNode);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This function is used to delete the Fragmented RP node
*
* @param    entry     @b{(input)} Pointer to the Fragmented RP node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static
L7_RC_t pimsmBsrFragRpNodeDelete(L7_sll_member_t *entry)
{
  pimsmFragRpNode_t  *fragRpNode = (pimsmFragRpNode_t *)entry;
  L7_uchar8 addrFamily = 0;

  addrFamily = L7_INET_GET_FAMILY (&(((pimsmFragGrpRpNode_t*)entry)->pimsmGrpAddr));

  /* free the entry */
  PIMSM_FREE (addrFamily, (void*) fragRpNode);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Initialize PIMSM BSR Block info
*
* @param    pimsmCb    @b{(input)}Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrInit(pimsmCB_t *pimsmCb)
{
  L7_uint32 i;
  /* allocate memory for the rp block */
  pimsmCb->pimsmBsrInfo = (pimsmBSRBlock_t *) PIMSM_ALLOC
                           (pimsmCb->family, sizeof(pimsmBSRBlock_t));

  if (pimsmCb->pimsmBsrInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"Memory allocation for BSR control block failed");
    return L7_FAILURE;
  }

  /* initialize all BSR node list */
  memset(pimsmCb->pimsmBsrInfo->pimsmBsrInfoList, 0, 
         sizeof(pimsmCb->pimsmBsrInfo->pimsmBsrInfoList));
 
  for (i = 0; i < PIMSM_BSR_SCOPE_NODES_MAX; i++)
  {
    inetAddressZeroSet(pimsmCb->family, 
                       &pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[i].pimsmBSRAddr);
    inetAddressZeroSet(pimsmCb->family, 
                &pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[i].pimsmBSRGroupRange);
    inetAddressZeroSet(pimsmCb->family, 
                &pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[i].zone.cbsr.pimsmElectedBSRAddress);     
  }
  
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Deinitialize PIMSM BSR Block info
*
* @param    pimsmCb   @b{(input)}Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrDeInit(pimsmCB_t *pimsmCb)
{
/*  SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
             pimsmCb->pimsmBsrInfo->pimsmBSRInfoList); */
  PIMSM_FREE (pimsmCb->family, (void *)pimsmCb->pimsmBsrInfo);
  pimsmCb->pimsmBsrInfo = L7_NULL;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Find the bsr node in the  BSR list.
*
* @param   pimsmCb        @b{(input)} Pointer to the PIM SM Control Block
* @param   grpRange       @b{(input)} Range of group addr this bsr is addressing.
* @param   grpMask        @b{(input)} the group mask length of the bsr.
* @param   pBsrNode       @b{(output)} Pointer to the BSR node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrInfoNodeFind(pimsmCB_t               *pimsmCb, 
                             L7_inet_addr_t           grpRange,
                             L7_uchar8                grpMaskLen,
                             pimsmBsrPerScopeZone_t **ppBsrNode)
{
  pimsmBsrPerScopeZone_t    bsrSearchNode,*pBsrEntry;
  L7_uint32                 cnt;
  L7_inet_addr_t            addr1, addr2;
  L7_inet_addr_t            mask1, mask2;

  *ppBsrNode = L7_NULLPTR;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmBsrInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Invalid input parameters");
    return L7_FAILURE;
  }

  if (inetIsAddrMulticastScope(&grpRange) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, " scoped range");
    inetCopy(&bsrSearchNode.pimsmBSRGroupRange, &grpRange);
    bsrSearchNode.pimsmBSRGroupMask = grpMaskLen;
  }
  else
  {
    inetAddressZeroSet(pimsmCb->family, &(bsrSearchNode.pimsmBSRGroupRange));
    bsrSearchNode.pimsmBSRGroupMask = 0;
  }

  for (cnt = 0; cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
  {
    pBsrEntry = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cnt;

    if (inetIsAddressZero(&pBsrEntry->pimsmBSRAddr) != L7_TRUE)
    {
      /* compare the grp address & mask combined together to get the scope*/
      inetMaskLenToMask(pimsmCb->family, pBsrEntry->pimsmBSRGroupMask,&mask1);
      inetMaskLenToMask(pimsmCb->family, bsrSearchNode.pimsmBSRGroupMask , &mask2);
      inetAddressAnd(&pBsrEntry->pimsmBSRGroupRange, &mask1, &addr1);
      inetAddressAnd(&(bsrSearchNode.pimsmBSRGroupRange), &mask2, &addr2);
      if (L7_INET_ADDR_COMPARE(&addr1,&addr2) == 0)
      {
        *ppBsrNode = pBsrEntry;
        return L7_SUCCESS;
      }
    }
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," Unable to find BSR node ");
  *ppBsrNode = L7_NULLPTR;
  return L7_FAILURE;  
}
/******************************************************************************
* @purpose  Find or Add the bsr node in the  BSR list.
*
* @param   pimsmCb        @b{(input)} Pointer to the PIM SM Control Block
* @param   grpRange       @b{(input)} Range of group addr this bsr is addressing.
* @param   grpMask        @b{(input)} the group mask length of the bsr.
* @param   bsrAddr        @b{(input)} address of the BSR 
* @param   bsrPriority    @b{(input)} BSR priority
* @param   bsrHashMasklen @b{(input)} Hash Mask Length. 
* @param   bsrType        @b{(input)} whether it is candidate or noncandidate.
* @param   pimsmBSRNode   @b{(output)}Pointer to the BSR Info Node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The type is given by the caller, for static config it is
*           candidate BSR, else non-candidate BSR. 
*       
* @end
******************************************************************************/
static L7_RC_t pimsmBsrInfoNodeAdd(pimsmCB_t               *pimsmCb, 
                                   L7_inet_addr_t           grpRange,
                                   L7_uchar8                grpMaskLen,
                                   L7_inet_addr_t           bsrAddr, 
                                   L7_uint32                bsrPriority,
                                   L7_uint32                bsrHashMasklen,
                                   pimsmBSRType_t           bsrType,
                                   pimsmBsrPerScopeZone_t **ppBsrNode)
{
  pimsmBsrPerScopeZone_t *pBsrEntry = L7_NULLPTR;
  L7_uint32               cnt;   

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmBsrInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR," Invalid input parameters ");
    *ppBsrNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  if (pimsmBsrInfoNodeFind(pimsmCb, grpRange, grpMaskLen, &pBsrEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,
      "BSR Node could not be found")
  }

  if (pBsrEntry != L7_NULLPTR)
  {
    if ((bsrType != pBsrEntry->pimsmBSRType)&&
        pBsrEntry->pimsmBSRType == PIMSM_BSR_TYPE_NONBSR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,
        "existing non candidateBSR node is replaced by candidate BSR")
      pimsmBsrInfoNodeDelete(pimsmCb, pBsrEntry);
      pBsrEntry = L7_NULLPTR;
    }
    else
    {
      if ((bsrType == pBsrEntry->pimsmBSRType) &&
          (pBsrEntry->pimsmBSRHashMasklen != bsrHashMasklen 
           || pBsrEntry->pimsmBSRPriority != bsrPriority))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,
            "existing  candidateBSR node has priority/ hashmasklen changed")
        pimsmBsrInfoNodeDelete(pimsmCb, pBsrEntry);
        pBsrEntry= L7_NULLPTR;
      }
    }
  }
  if (pBsrEntry == L7_NULLPTR)
  {
    pimsmBsrPerScopeZone_t  *bsrNode = L7_NULLPTR;

    for (cnt = 0; cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
    {
      pBsrEntry = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cnt;
      if (inetIsAddressZero(&pBsrEntry->pimsmBSRAddr) == L7_TRUE)
      {   
        bsrNode = pBsrEntry;
        break;
      }
    }

    if (cnt == PIMSM_BSR_SCOPE_NODES_MAX)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Failed to allocate memory for BSR node");
      *ppBsrNode = L7_NULLPTR;
      return L7_FAILURE;
    }
    /* create the entry and add the stuff */
    inetCopy(&(bsrNode->pimsmBSRGroupRange), &grpRange);
    bsrNode->pimsmBSRGroupMask= grpMaskLen;
    inetCopy(&(bsrNode->pimsmBSRAddr), &bsrAddr);
    bsrNode->pimsmBSRPriority= bsrPriority;
    bsrNode->pimsmBSRHashMasklen = (L7_ushort16)bsrHashMasklen;
    bsrNode->pimsmBSRType = bsrType;
    bsrNode->ctime = osapiUpTimeRaw();
    bsrNode->pimsmCb = pimsmCb;
    bsrNode->pimsmBSRTimerHandle = handleListNodeStore(pimsmCb->handleList,
                                                       bsrNode);
    if (bsrType == PIMSM_BSR_TYPE_CBSR)
    {
      pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRElectedBSR
                                                                     = L7_FALSE; 
      bsrNode->zone.cbsr.pimsmCandBSRState = 
      PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR;
      bsrNode->zone.cbsr.pimsmElectedBSRPriority= bsrPriority;
      bsrNode->zone.cbsr.pimsmElectedBSRHashMaskLen = bsrHashMasklen;
      inetCopy(&(bsrNode->zone.cbsr.pimsmElectedBSRAddress), &bsrAddr);
      if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrCandidateBootStrapTimerExpiresHandler,
                                (void*)bsrNode->pimsmBSRTimerHandle,
                                pimsmBsrRandOverrideIntervalGet(pimsmCb,bsrNode),
                                &(bsrNode->pimsmBSRTimer),
                                "SM-BSR") != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR,  PIMSM_TRACE_ERROR, "BSR Timer Add Failed");
      }
    }
    else
    {
      if (inetIsAddrMulticastScope(&grpRange) == L7_TRUE)
      {
        /* set the state to no info */
        bsrNode->zone.nbsr.pimsmNonCandBSRState =
        PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO;
      }
      else
      {
        /* set the state to accept any */
        bsrNode->zone.nbsr.pimsmNonCandBSRState = 
        PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY;

        /*  start the scope zone timer */
        if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrNonCandidateScopeZoneTimerExpiresHandler,
                                  (void*)bsrNode->pimsmBSRTimerHandle,
                                  PIMSM_DEFAULT_BOOTSTRAP_SZ_TIMEOUT,
                                  &(bsrNode->zone.nbsr.pimsmBSRScopeZoneExpireTimer),
                                  "SM-SZT")
                               != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR,  PIMSM_TRACE_ERROR, "pimsmBSRScopeZoneExpireTimer Timer Add Failed");
        }
      }
    }

    /* create the linked list for the fragment BSM storage */
    if (SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                   L7_SLL_ASCEND_ORDER, L7_NULL,
                   pimsmBsrFragGrpNodeCompare,pimsmBsrFragGrpNodeDelete,
                   &(bsrNode->pimsmBSRFragInfo.pimsmGrpRpList)) != L7_SUCCESS)
    {
      PIMSM_FREE (pimsmCb->family, (void*) bsrNode);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Unable to create SLL for the frag list");
      *ppBsrNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    *ppBsrNode = bsrNode;
    return L7_SUCCESS;
  }
  *ppBsrNode = pBsrEntry;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Delete the bsr node in the  BSR list.
*
* @param    pimsmCb     @b{(input)}Pointer to the PIM SM Control Block
* @param    pBsrNode    @b{(input)}the pointer to the BSR node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The type is given by the caller, for static config it is candidate BSR,
*            else it is non-candidate BSR
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrInfoNodeDelete(pimsmCB_t               *pimsmCb, 
                               pimsmBsrPerScopeZone_t  *pBsrNode) 
{

  if (pimsmCb == L7_NULLPTR || pBsrNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR," Invalid input parameters ");
    return L7_FAILURE;
  }

  pimsmBsrNodeDelete(pimsmCb,pBsrNode);

  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  This function is used to add the rp node to the frag list.
*
* @param  pimsmCb           @b{(input)} Pointer to the control block
* @param  pFragGrpNode      @b{(input)} Pointer to the frag group node.
* @param  rpAddr            @b{(input)} the RP address
* @param  rpHoldtime        @b{(input)} the holdtime for the RP.
* @param  rpPriority        @b{(input)} RP priority.
* @param  pFragRpNode       @b{(output)} Fragmented RP node found/Created
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static
L7_RC_t pimsmBsrFragRpNodeAdd(pimsmCB_t              *pimsmCb,
                              pimsmFragGrpRpNode_t   *pFragGrpNode,
                              L7_inet_addr_t          rpAddr,
                              L7_ushort16             rpHoldtime,
                              L7_uint32               rpPriority,
                              pimsmFragRpNode_t     **ppFragRpNode)
{
  pimsmFragRpNode_t   *pFragRpNode = L7_NULLPTR,fragRpSearchNode;

  /*check if the node exists */
  if (pimsmCb == L7_NULLPTR || pFragGrpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Invalid input parameters");
	 *ppFragRpNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  fragRpSearchNode.pimsmRpAddr = rpAddr;
  pFragRpNode = (pimsmFragRpNode_t *)SLLFind(&(pFragGrpNode->pimsmRpList),
                                              (void*)&fragRpSearchNode);

  if (pFragRpNode == L7_NULLPTR)
  {
    /* node not found so create the node */
    if ((pFragRpNode = PIMSM_ALLOC (pimsmCb->family, sizeof (pimsmFragRpNode_t)))
                                 == L7_NULLPTR)
    {
      /* something grossly wrong */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Failed to allocate buffer from fragRp Pool");
      *ppFragRpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* populate the node */
    inetCopy(&pFragRpNode->pimsmRpAddr, &rpAddr);
    pFragRpNode->pimsmRpHoldtime = rpHoldtime;
    pFragRpNode->pimsmRpPriority = rpPriority;

    /* create the Linked list for the rp list */
    /* add it to the list */
    if (SLLAdd(&(pFragGrpNode->pimsmRpList),(L7_sll_member_t*)pFragRpNode) 
        != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, 
			         "Failed to add the fragmented Rp node to the Rplist"); 
      PIMSM_FREE (pimsmCb->family, (void*) pFragRpNode);
      *ppFragRpNode = L7_NULLPTR;   
      return L7_FAILURE;
    }
  }
  *ppFragRpNode = pFragRpNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  This function is used to find the grp node to the frag list.
*
* @param   pimsmCb         @b{(input)} Pointer to the control block
* @param   pBsrNode        @b{(input)} Pointer to the bsr node.
* @param   grpAddr         @b{(input)} the group address
* @param   grpMask         @b{(input)} Group mask length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static
L7_RC_t pimsmBsrFragGrpNodeFind(pimsmCB_t              *pimsmCb,
                                pimsmBsrPerScopeZone_t *pBsrNode,
                                L7_inet_addr_t          grpAddr,
                                L7_uchar8               grpMaskLen)
{
  pimsmFragGrpRpNode_t   *pimsmFragGrpRpEntry, pimsmSearchNode;

  /*check if the node exists */
  if ( (pimsmCb == L7_NULLPTR) ||
       (pBsrNode == L7_NULLPTR) )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Invalid input parameters");
    return L7_FAILURE;
  }

  inetCopy(&pimsmSearchNode.pimsmGrpAddr, &grpAddr);
  pimsmSearchNode.pimsmGrpMaskLength = grpMaskLen;
  pimsmFragGrpRpEntry = (pimsmFragGrpRpNode_t *)SLLFind
                        (&(pBsrNode->pimsmBSRFragInfo.pimsmGrpRpList),
                         (void*)&pimsmSearchNode);
  if (pimsmFragGrpRpEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"Failed to find fragmented Group node ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  This function is used to add the grp node to the frag list.
*
* @param   pimsmCb         @b{(input)}Pointer to the control block
* @param   pBsrNode        @b{(input)} Pointer to the bsr node.
* @param   grpAddr         @b{(input)} the group address
* @param   rpCnt           @b{(input)}the total rp count
* @param   fragRpCnt       @b{(input)} The fragment rp count.
* @param   pFragGrpRpNode  @b{(output)} The fragment Group node found/created.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
static 
L7_RC_t pimsmBsrFragGrpNodeAdd(pimsmCB_t               *pimsmCb,
                               pimsmBsrPerScopeZone_t  *pBsrNode,
                               L7_inet_addr_t           grpAddr,
                               L7_uchar8                grpMaskLen,
                               L7_ushort16              rpCnt,
                               L7_ushort16              fragRpCnt,
                               pimsmFragGrpRpNode_t   **ppFragGrpRpNode)
{
  pimsmFragGrpRpNode_t   *pimsmFragGrpRpEntry;
  pimsmFragGrpRpNode_t    pimsmSearchNode;

  /*check if the node exists */
  if ( (pimsmCb == L7_NULLPTR) ||
       (pBsrNode == L7_NULLPTR) )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Invalid input parameters");
	 *ppFragGrpRpNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  inetCopy(&pimsmSearchNode.pimsmGrpAddr, &grpAddr);
  pimsmSearchNode.pimsmGrpMaskLength = grpMaskLen;
  pimsmFragGrpRpEntry = (pimsmFragGrpRpNode_t *)SLLFind
                        (&(pBsrNode->pimsmBSRFragInfo.pimsmGrpRpList),
                         (void*)&pimsmSearchNode);

  if (pimsmFragGrpRpEntry == L7_NULLPTR)
  {
    /* node not found so create the node */
    if ((pimsmFragGrpRpEntry = PIMSM_ALLOC (pimsmCb->family,
                                            sizeof (pimsmFragGrpRpNode_t)))
                                         == L7_NULLPTR)
    {
      /* something grossly wrong */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, 
                  "PIMSM : Failed to allocate memory from Fragment Group pool");
      *ppFragGrpRpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* populate the node */
    inetCopy(&pimsmFragGrpRpEntry->pimsmGrpAddr, &grpAddr);
    pimsmFragGrpRpEntry->pimsmGrpMaskLength= grpMaskLen;
    pimsmFragGrpRpEntry->pimsmRpCnt = rpCnt;
    pimsmFragGrpRpEntry->pimsmFragRpCnt= fragRpCnt;

    /* create the Linked list for the rp list */
    /* create the linked list for the fragment BSM storage */
    if (SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                   L7_SLL_ASCEND_ORDER, L7_NULL,
                   pimsmBsrFragRpNodeCompare,pimsmBsrFragRpNodeDelete,
                   &(pimsmFragGrpRpEntry->pimsmRpList)) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Unable to create the fragRp list");
      PIMSM_FREE (pimsmCb->family, (void*) pimsmFragGrpRpEntry);
      *ppFragGrpRpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* add it to the list */
    if (SLLAdd(&(pBsrNode->pimsmBSRFragInfo.pimsmGrpRpList),
               (L7_sll_member_t*)pimsmFragGrpRpEntry) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,
                  "Unable to add the fragmented group node to Grplist"); 
      SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(pimsmFragGrpRpEntry->pimsmRpList));
      PIMSM_FREE (pimsmCb->family, (void*) pimsmFragGrpRpEntry);
      *ppFragGrpRpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  *ppFragGrpRpNode = pimsmFragGrpRpEntry;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Find the best group range match from the list of bsr nodes.
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrNode    @b{(input)} the pointer to the bsr node.
* @param    grpAddr     @b{(input)} he group address
* @param    grpMasklen  @b{(input)} the group mask length
*
* @returns L7_TRUE  - If true the given node is the best match.
* @returns L7_FALSE - The given node is not the prefect match.
*
* @comments      
*       
* @end
******************************************************************************/
L7_BOOL pimsmBsrInfoBestGroupRangeMatch(pimsmCB_t              *pimsmCb,
                                        pimsmBsrPerScopeZone_t *pBsrNode,
                                        L7_inet_addr_t          grpAddr,
                                        L7_uchar8               grpMasklen)  
{
  pimsmBsrPerScopeZone_t    *bsrTempNode;
  L7_uint32                  cnt;

  /* iterate through the list to find the longest prefix match */
  for (cnt = 0 ;cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
  {
   bsrTempNode = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cnt;
   if (inetIsAddressZero(&bsrTempNode->pimsmBSRAddr) != L7_TRUE)
   {
    if (bsrTempNode != pBsrNode)
    {
      if (inetAddrCompareAddrWithMask(&bsrTempNode->pimsmBSRGroupRange,
                                      bsrTempNode->pimsmBSRGroupMask,
                                      &grpAddr,grpMasklen) == 0)
      {
        /* That the temp_node too can cater to the group addr/mask */
        if (inetAddrIsScopedInsideAnother(&bsrTempNode->pimsmBSRGroupRange,
                                          bsrTempNode->pimsmBSRGroupMask,
                                          &pBsrNode->pimsmBSRGroupRange,
                                          pBsrNode->pimsmBSRGroupMask) == L7_TRUE)
        {
          return L7_FALSE;
        }
      }
    }
  }
 }

  return L7_TRUE;
}

/******************************************************************************
* @purpose  Validate a Bootstrap message
*
* @param    pimsmCb     @b{(input)} pointer to the control block.    
* @param    pBsrNode    @b{(input)} the pointer to the bsr node.
* @param    pSrcAddr    @b{(input)} source address
* @param    pDstAddr    @b{(input)} destination address
* @param    pBsrAddr    @b{(input)} bsr address.
* @param    pGrpAddr    @b{(input)} first group address
* @param    adminScope  @b{(input)} admin scope flag
* @param    rtrifNUm    @b{(input)} router interface number 
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
******************************************************************************/
static 
L7_BOOL pimsmBsrPacketValidate(pimsmCB_t              *pimsmCb, 
                               pimsmBsrPerScopeZone_t *pBsrNode,
                               L7_inet_addr_t         *pSrcAddr,
                               L7_inet_addr_t         *pDestAddr, 
                               L7_inet_addr_t         *pBsrAddr,
                               L7_inet_addr_t         *pGrpAddr,
                               L7_BOOL                 adminScope,
                               L7_uint32               rtrIfNum)
{
  mcastRPFInfo_t    bestRPFRouteInfo;
  L7_uint32         rtrIfNumTemp;

  /* Check if the source of the packet is directly connected and has come from a neighbor */
  if (((mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE) && 
       (inetIsDirectlyConnected(pSrcAddr,rtrIfNum) == L7_FALSE) ) || 
      pimsmNeighborEntryGet((L7_VOIDPTR)pimsmCb,rtrIfNum,pSrcAddr) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"directly connected or no hello state for that BSR");
    return L7_FALSE;
  }

  /* check the destination of the packet if it is directed to all PIM Routers */
  if (inetIsAllPimRouterAddress(pDestAddr) == L7_TRUE)
  {
    memset(&bestRPFRouteInfo, 0, sizeof(mcastRPFInfo_t));

    /* do the rpf check on the source address */
    if (pimsmRPFInfoGet(pimsmCb,pBsrAddr,&bestRPFRouteInfo) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"Failed to get RPF Info for BSR in BSM packet");
      return L7_FALSE;
    }

    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," iif = %d , rpfIf = %d\n", rtrIfNum,
                bestRPFRouteInfo.rpfIfIndex);

    /*  The condition for nexthop address has been removed  since :
        In IPv6 multicast, when the BSR is directly connected, the rpfNextHop
        being returned is global address and the IPv6 src being compared is of 
        linklocal address, which is not possible for comparision. Instead RPF
        interface is considered */

    if (rtrIfNum != bestRPFRouteInfo.rpfIfIndex)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"BSM not received from RPF nexthop");
      return L7_FALSE;
    }
  }
  /* check the destination of the packet is destined to me - Check is done only
     for the incoming interface */
  else if (mcastIpMapUnnumberedIsLocalAddress(pDestAddr, &rtrIfNumTemp) == L7_TRUE)
  {     
    /* check to see if any previous BSM has been accepted or
       BS_period has been expired since startup */
    L7_uint32  now = osapiUpTimeRaw();

    /*If I am the BSR address and receive a BSR message ,drop silently */
    if (mcastIpMapUnnumberedIsLocalAddress(pBsrAddr, &rtrIfNumTemp) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"I am the BSR,and I have received the BSR message.Silently dropping it");
      return L7_FALSE;

    }
    if (pBsrNode->pimsmLastBSMPktNode[0].pimsmInUse == L7_TRUE || 
        (now - pBsrNode->ctime) > PIMSM_DEFAULT_BOOTSTRAP_PERIOD)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"already received a BSM at startup");
      return L7_FALSE;
    }
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"unknown destination  address in BSM");
    return L7_FALSE;
  }

  /* check if the message has arrived on a admin scope border interface */
  if (adminScope == L7_TRUE)
  {
    return mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,rtrIfNum,pGrpAddr);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," BSM vadidation passed \n");
  return L7_TRUE;
}


/******************************************************************************
* @purpose  Compare BSR weights
*
* @param   BSR Priority  @b{(input)} BSR Priority1
* @param   BSR Address   @b{(input)} BSR Address1
* @param   BSR Priority  @b{(input)} BSR Priority2 
* @param   BSR Address   @b{(input)} BSR Address2
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments  return L7_TRUE if (p1,addr1) is higher in weight
*            else return L7_FALSE 
*
* @end
******************************************************************************/   
static
L7_BOOL pimsmBsrIsPrefferedBSR(L7_uint32       priority1, 
                               L7_inet_addr_t *addr1,
                               L7_uint32       priority2,
                               L7_inet_addr_t *addr2)
{
  if (priority1 > priority2 ||
      ( priority1 == priority2 && (L7_INET_ADDR_COMPARE(addr1,addr2) >= 0 )))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/******************************************************************************
* @purpose  To Check the elected BSR is local or not
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr     @b{(input)} group Range
* @param    grpMaskLen  @b{(input)} group Prefix Length
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments      
*       
* @end
***************************************************************************/
L7_BOOL pimsmBsrIsLocal(pimsmCB_t     *pimsmCb,
                        L7_inet_addr_t grpAddr,
                        L7_uchar8      grpMaskLen)
{
  pimsmBsrPerScopeZone_t   *pBsrNode = L7_NULLPTR;

  if (pimsmBsrInfoNodeFind(pimsmCb, grpAddr, grpMaskLen, &pBsrNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,"Failed to find Bsr node");
    return L7_FALSE;    
  }

  if (pBsrNode != L7_NULLPTR)
  {
    if (pBsrNode->pimsmBSRType == PIMSM_BSR_TYPE_CBSR)
    {
      L7_uint32 rtrIfNum;

      if (mcastIpMapUnnumberedIsLocalAddress(&pBsrNode->zone.cbsr.pimsmElectedBSRAddress, 
                                   &rtrIfNum) == L7_TRUE)
      {
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}

/******************************************************************************
* @purpose  To calculate Bootstrap Random override interval 
*
* @param    pimsmCb   @b{(input)} pointer to the pimsm control block.
* @param    pBsrNode  @b{(input)} pointer to the bsr node.
*
* @returns  override interval
*
* @comments This algorithm is defined in section 5 of draft_ietf_pim_sm_bsr_05 
*       
* @end
******************************************************************************/
L7_uint32 pimsmBsrRandOverrideIntervalGet(pimsmCB_t              *pimsmCb,
                                          pimsmBsrPerScopeZone_t *pBsrNode)
{
  L7_uint32      priorityDelay, bsrRandOverride, addrDelay;
  L7_uint32      bestPriority;
  L7_inet_addr_t bestAddr;   

  /* calculate the best priority */
  if (pBsrNode->pimsmBSRPriority> pBsrNode->zone.cbsr.pimsmElectedBSRPriority)
    bestPriority = pBsrNode->pimsmBSRPriority;
  else
    bestPriority = pBsrNode->zone.cbsr.pimsmElectedBSRPriority;

  /* calculate the best address */
  if (L7_INET_ADDR_COMPARE(&pBsrNode->pimsmBSRAddr,
                           &pBsrNode->zone.cbsr.pimsmElectedBSRAddress) > 0)
    inetCopy(&bestAddr, &pBsrNode->pimsmBSRAddr);
  else
    inetCopy(&bestAddr, &pBsrNode->zone.cbsr.pimsmElectedBSRAddress);

  /* calculate the priority delay */
  priorityDelay = 2 * pimsmLog2((1 + bestPriority + pBsrNode->pimsmBSRPriority)); 

  /* calculate the addr delay */
  if (bestPriority == pBsrNode->pimsmBSRPriority)
  {
    addrDelay = pimsmLog2( 1+ inetPimsmDigestGet(&bestAddr) - 
                           inetPimsmDigestGet(&pBsrNode->pimsmBSRAddr));
    addrDelay = addrDelay/(L7_INET_GET_MAX_MASK_LEN(pimsmCb->family)/2);
  }
  else
  {
    addrDelay = 2 - (inetPimsmDigestGet(&pBsrNode->pimsmBSRAddr)/(1<<31));
  }

  bsrRandOverride = 5 + priorityDelay + addrDelay;
  return bsrRandOverride;
}
/******************************************************************************
* @purpose  To trigger the candidate and noncandidate BSR state machine
*
* @param    pimsmCb       @b{(input)}  Pointer to the PIM SM Control Block
* @param    pBsrNode      @b{(input)}  Pointer to the BSR Info Node
* @param    bsrAddr       @b{(input)}  BSR address of the bsr message
* @param    bsrPriority   @b{(input)}  BSR Priority
* @param    pimHeader     @b{(input)}  the pointer to the pim packet.
* @param    pimPktLen     @b{(input)}  length of the pim packet
* @param    rtrIfNum      @b{(input)}  router interface number.
*
* @returns  none
*
* @comments      
*       
* @end
******************************************************************************/
static
void pimsmBsrBsrFsmTrigger(pimsmCB_t               *pimsmCb,
                           pimsmBsrPerScopeZone_t  *pBsrNode,
                           L7_inet_addr_t           bsrAddr,
                           L7_uint32                bsrPriority,
                           L7_uchar8                bsrHashMaskLen,
                           L7_uchar8               *pimHeader,
                           L7_uint32                pimPktLen, 
                           L7_uint32                rtrIfNum)
{
  if (pBsrNode->pimsmBSRType== PIMSM_BSR_TYPE_CBSR)
  {
    /* call the CBSR FSM */
    pimsmPerScopeZoneCandBSREventInfo_t     candBsrEventInfo;

    /*populate the event */
    memset(&candBsrEventInfo,0,sizeof(candBsrEventInfo));
    candBsrEventInfo.pBsrNode = pBsrNode;
    candBsrEventInfo.pimsmPktInfo.pimHeader = pimHeader;
    candBsrEventInfo.pimsmPktInfo.pimPktLen = pimPktLen;
    candBsrEventInfo.pimsmPktInfo.rtrIfNum = rtrIfNum;

    if (pimsmBsrIsPrefferedBSR(bsrPriority,&bsrAddr,
                               pBsrNode->zone.cbsr.pimsmElectedBSRPriority,
                               &pBsrNode->zone.cbsr.pimsmElectedBSRAddress) == L7_TRUE)
    {
      candBsrEventInfo.eventType = PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_PREFERRED_BSM;
      inetCopy(&pBsrNode->zone.cbsr.pimsmElectedBSRAddress, &bsrAddr);
      pBsrNode->zone.cbsr.pimsmElectedBSRPriority = bsrPriority;
      pBsrNode->zone.cbsr.pimsmElectedBSRHashMaskLen = bsrHashMaskLen;
    }
    else
    {
      if (L7_INET_ADDR_COMPARE(&pBsrNode->zone.cbsr.pimsmElectedBSRAddress, 
                               &bsrAddr))
      {
        candBsrEventInfo.eventType = 
        PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM;
      }
      else
      {
        candBsrEventInfo.eventType =
        PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM_FROM_ELECTED_BSR;\
      }
    }
    /* call the FSM trigger function */
    pimsmPerScopeZoneCandBSRExecute(pimsmCb,&candBsrEventInfo);

  }
  else
  {
    /* call the NBSR FSM */
    pimsmPerScopeZoneNonCandBSREventInfo_t  nonCandBsrEventInfo;

    /*populate the event */
    memset(&nonCandBsrEventInfo,0,sizeof(nonCandBsrEventInfo));
    nonCandBsrEventInfo.pBsrNode = pBsrNode;
    nonCandBsrEventInfo.pimsmPktInfo.pimHeader = pimHeader;
    nonCandBsrEventInfo.pimsmPktInfo.pimPktLen = pimPktLen;
    nonCandBsrEventInfo.pimsmPktInfo.rtrIfNum = rtrIfNum;
    nonCandBsrEventInfo.eventType = PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_BSM;

    if (pBsrNode->zone.nbsr.pimsmNonCandBSRState ==
	 	                               PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED)
    {
      if (pimsmBsrIsPrefferedBSR(bsrPriority,&bsrAddr,
                                 pBsrNode->pimsmBSRPriority,
                                 &pBsrNode->pimsmBSRAddr) == L7_TRUE)
      {
        inetCopy(&pBsrNode->pimsmBSRAddr, &bsrAddr);
        pBsrNode->pimsmBSRPriority = bsrPriority;
        pBsrNode->pimsmBSRHashMasklen = bsrHashMaskLen;
        nonCandBsrEventInfo.eventType =
		  	         PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_PREFERRED_BSM;
      }
      else
      {
        nonCandBsrEventInfo.eventType = 
		  	         PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM;
      }
    }
    else
    {
      inetCopy(&pBsrNode->pimsmBSRAddr, &bsrAddr);
      pBsrNode->pimsmBSRPriority = bsrPriority;
      pBsrNode->pimsmBSRHashMasklen = bsrHashMaskLen;
    }
    pimsmPerScopeZoneNonCandBSRExecute(pimsmCb,&nonCandBsrEventInfo);
  }
  return;
}


/******************************************************************************
* @purpose  To extract the Group info from the BSM packet
*
* @param    pData       @b{(input)} the pointer to the pim message.
* @param    grpNode     @b{(output)} pointer to the rp info
*
* @returns  number of bytes read in the buffer.
*
* @comments  
*       
* @end
******************************************************************************/
static
L7_uint32 pimsmBsrPacketGrpInfoExtract(L7_uchar8            *pData,
                                       pimsmBSMMsgGrpNode_t *pGrpNode)
{
  L7_uchar8               *pDataTemp, reserved;
  pim_encod_grp_addr_t    encodGrpAddr;

  memset(&encodGrpAddr,0,sizeof(pim_encod_grp_addr_t));
  
  pDataTemp = pData;
  PIM_GET_EGADDR_INET(&encodGrpAddr,pDataTemp);
  inetAddressSet(encodGrpAddr.addr_family,&encodGrpAddr.addr,
                 &pGrpNode->pimsmGrpAddr);
  pGrpNode->pimsmGrpMaskLength = encodGrpAddr.masklen;
  MCAST_GET_BYTE(pGrpNode->pimsmRpCnt,pDataTemp);
  MCAST_GET_BYTE(pGrpNode->pimsmFragRpCnt,pDataTemp);
  MCAST_GET_SHORT(reserved, pDataTemp);
  MCAST_UNUSED_PARAM (reserved);

  return((L7_uint32)pDataTemp - (L7_uint32)pData);
}

/******************************************************************************
* @purpose  To extract the RP info from the BSM packet
*
* @param    pimPkt   @b{(input)} the pointer to the pim message.
* @param    rpNode   @b{(output)} pointer to the rp info
*
* @returns number of bytes read in the buffer.
*
* @comments  
*       
* @end
******************************************************************************/
static
L7_uint32 pimsmBsrPacketRpInfoExtract(L7_uchar8           *pData, 
                                      pimsmBSMMsgRpNode_t *pRpNode)
{
  L7_uchar8       *pDataTemp;
  L7_uchar8        reserved;
  pim_encod_uni_addr_t    encodRpAddr;

  memset(&encodRpAddr,0,sizeof(pim_encod_uni_addr_t));
  
  pDataTemp = pData;
  PIM_GET_EUADDR_INET(&encodRpAddr,pDataTemp);
  inetAddressSet(encodRpAddr.addr_family,&encodRpAddr.addr,&pRpNode->pimsmRpAddr);
  MCAST_GET_SHORT(pRpNode->pimsmRpHoldtime,pDataTemp);
  MCAST_GET_BYTE(pRpNode->pimsmRpPriority,pDataTemp);
  MCAST_GET_BYTE(reserved,pDataTemp);
  MCAST_UNUSED_PARAM (reserved);

  return((L7_uint32)pDataTemp - (L7_uint32)pData);
}

/******************************************************************************
* @purpose  To store the grp-rp mappings in the fragment list 
*
* @param    pimsmCb      @b{(input)} pointer to the pimsm control block.
* @param    pBsrNode     @b{(input)} pointer to the bsr node.
* @param    pimPkt       @b{(input)} pointer to the BSM buffer  
* @param    grpNode      @b{(input)} pointer to the grp info
* @param    fragId       @b{(input)} fragement id.
*
* @returns number of bytes read in the buffer.
*
* @comments  
*       
* @end
******************************************************************************/
static L7_uint32 pimsmBsrPacketFragListStore(pimsmCB_t              *pimsmCb,
                                             pimsmBsrPerScopeZone_t *pBsrNode,
                                             L7_uchar8              *pData, 
                                             pimsmBSMMsgGrpNode_t   *grpNode,
                                             L7_ushort16             fragId)
{
  L7_uchar8                  *pDataTemp = pData;
  pimsmFragGrpRpNode_t       *pFragGrpNode = L7_NULLPTR;
  L7_uint32                   count=0,len=0;
  L7_BOOL                     firsttime=L7_FALSE;
  L7_RC_t                     rc = L7_FAILURE;

  /* check the fragment id */
  if (pBsrNode->pimsmBSRFragInfo.pimsmFragTag != fragId)
  {
    /* this is not the same frag as stored before so just drop the previously
    stored fragment list */
    SLLPurge(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
             &(pBsrNode->pimsmBSRFragInfo.pimsmGrpRpList));
  }

  /* check if the group node exists in the frag list if not add it */
  rc = pimsmBsrFragGrpNodeFind(pimsmCb,pBsrNode,
                               grpNode->pimsmGrpAddr,
                               grpNode->pimsmGrpMaskLength);

  if (rc == L7_FAILURE)
  {
    rc = pimsmBsrFragGrpNodeAdd(pimsmCb,pBsrNode,
                                grpNode->pimsmGrpAddr,
                                grpNode->pimsmGrpMaskLength,
                                grpNode->pimsmRpCnt,
                                grpNode->pimsmFragRpCnt,
                                &pFragGrpNode);
    firsttime = L7_TRUE;
    if ((rc == L7_FAILURE) || (pFragGrpNode == L7_NULLPTR))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"failed to add fragmented group node ");
      return L7_NULL;
    }
  }

  /* extract all the rp info and store it in the frag list */
  while (count != grpNode->pimsmFragRpCnt)
  {
    pimsmBSMMsgRpNode_t   rpNode;
    pimsmFragRpNode_t     *pFragRpNode;


    len = pimsmBsrPacketRpInfoExtract(pDataTemp,&rpNode);
    pDataTemp +=len;
    count++;

    /* add this to the frag list */
    rc = pimsmBsrFragRpNodeAdd(pimsmCb,pFragGrpNode,rpNode.pimsmRpAddr,
                               rpNode.pimsmRpHoldtime,
                               rpNode.pimsmRpPriority,
                               &pFragRpNode);
    if (rc == L7_FAILURE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"failed to add fragmented RP node");
      return L7_NULL;
    }
  }
  /* update the frag count in the frag group node */
  if (firsttime != L7_TRUE)
  {
    pFragGrpNode->pimsmFragRpCnt += grpNode->pimsmFragRpCnt;

    if (pFragGrpNode->pimsmFragRpCnt >= pFragGrpNode->pimsmRpCnt)
    {
      pimsmFragRpNode_t      *pFragRpNode;
      pimsmCandRpConfigInfo_t   candRpConfigInfo;

      /* got all the nodes belonging to this group so aplly the group rp mapping */
      while ((pFragRpNode = (pimsmFragRpNode_t *)
              SLLFirstGet(&(pFragGrpNode->pimsmRpList))) != L7_NULLPTR)
      {
        /* apply this to the rpset */
        /* apply the rp mapping */
        inetCopy(&candRpConfigInfo.pimsmGrpAddr, &pFragGrpNode->pimsmGrpAddr);
        candRpConfigInfo.pimsmGrpMaskLen = pFragGrpNode->pimsmGrpMaskLength;
        candRpConfigInfo.pimsmRpHashMaskLen = pBsrNode->pimsmBSRHashMasklen;
        inetCopy(&candRpConfigInfo.pimsmRpAddr, &pFragRpNode->pimsmRpAddr);
        candRpConfigInfo.pimsmRpHoldTime = pFragRpNode->pimsmRpHoldtime;
        candRpConfigInfo.pimsmRpPriority = pFragRpNode->pimsmRpPriority;
        candRpConfigInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;

        if (pimsmRpGrpMappingAdd(pimsmCb,&candRpConfigInfo) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR," failed to add RP -Group mapping ");
          return L7_NULL;
        }
        /* set the expiry time to the rp holdtime */
        pimsmRprGrpMappingExpireTimerSet(pimsmCb,
                                         pFragGrpNode->pimsmGrpAddr,
                                         pFragGrpNode->pimsmGrpMaskLength,
                                         pFragRpNode->pimsmRpAddr,
                                         PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                         pFragRpNode->pimsmRpHoldtime);
        /* delete the node */
        SLLDelete(&(pFragGrpNode->pimsmRpList),(L7_sll_member_t*)pFragRpNode);
      }
      /* delete the frag node belonging to this group afterwards */
      SLLDelete(&(pBsrNode->pimsmBSRFragInfo.pimsmGrpRpList),
                (L7_sll_member_t*)pFragGrpNode);
    }
  }
  return((L7_uint32)pDataTemp - (L7_uint32)pData);
}


/******************************************************************************
* @purpose  To store the RP-Group mapping obtained from the BSR packet 
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrNode    @b{(input)} the pointer to BSR node.
* @param    pimHeader   @b{(input)} the pointer to the pim message.
* @param    pimPktLen   @b{(input)} length of the pim buffer
*
* @returns  None
*
* @comments  
*       
* @end
******************************************************************************/
void pimsmBsrPacketRpGrpMappingStore(pimsmCB_t              *pimsmCb,
                                     pimsmBsrPerScopeZone_t *pBsrNode,
                                     L7_uchar8              *pimHeader,
                                     L7_uint32               pimPktLen)
{
  L7_uchar8                      *pData;
  L7_short16                      bsrFragmentTag;
  L7_uchar8                       bsrHashMaskLen;
  L7_uint32                       bsrPriority;
  pim_encod_uni_addr_t            encodBsrAddr;
  L7_inet_addr_t                  bsrAddr;
  pimsmBSMPacketNode_t            pBsrPktNode, *pFragPacketNode = L7_NULLPTR;
  L7_uint32                       i,len =0;

  memset(&encodBsrAddr, 0,sizeof(pim_encod_uni_addr_t));
  /* parse the packet to get the bsm header info */
  pData = (L7_uchar8*)(pimHeader + PIM_HEARDER_SIZE);

  /* Parse the PIMSM_BOOTSTRAP message */
  MCAST_GET_SHORT(bsrFragmentTag,pData);
  MCAST_GET_BYTE(bsrHashMaskLen,pData);
  MCAST_GET_BYTE(bsrPriority,pData);
  MCAST_UNUSED_PARAM (bsrPriority); /* NOTE: TBD: Looks like bsrPriority processing is not implemented. */
  PIM_GET_EUADDR_INET( &encodBsrAddr,pData);
  inetAddressSet(encodBsrAddr.addr_family,&encodBsrAddr.addr,&bsrAddr);

  while (((L7_uint32)pData - (L7_uint32)pimHeader) < pimPktLen)
  {
    L7_uint32 count;
    pimsmBSMMsgGrpNode_t    grpNode;

    /* get each of the group address */
    len = pimsmBsrPacketGrpInfoExtract(pData,&grpNode);
    pData += len;

    /* check if the fragment contains all the rp's */
    if (grpNode.pimsmRpCnt == grpNode.pimsmFragRpCnt)
    {
      count=0 ;
      while (count < grpNode.pimsmFragRpCnt && 
             ((L7_uint32)pData - (L7_uint32)pimHeader) < pimPktLen)
      {
        pimsmBSMMsgRpNode_t   bsrRpNode;
        pimsmCandRpConfigInfo_t   rpConfigInfo;

        len = pimsmBsrPacketRpInfoExtract(pData,&bsrRpNode);
        pData +=len;
        /* apply the rp mapping */
        inetCopy(&rpConfigInfo.pimsmGrpAddr, &grpNode.pimsmGrpAddr);
        rpConfigInfo.pimsmGrpMaskLen = grpNode.pimsmGrpMaskLength;
        rpConfigInfo.pimsmRpHashMaskLen = bsrHashMaskLen;
        inetCopy(&rpConfigInfo.pimsmRpAddr, &bsrRpNode.pimsmRpAddr);

        rpConfigInfo.pimsmRpHoldTime = bsrRpNode.pimsmRpHoldtime;
        rpConfigInfo.pimsmRpPriority = bsrRpNode.pimsmRpPriority;
        rpConfigInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;

        if (pimsmRpGrpMappingAdd(pimsmCb,&rpConfigInfo) != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR," failed to add RP -Group mapping ");
          return;
        }
        /* set the expiry time to the rp holdtime */
        pimsmRprGrpMappingExpireTimerSet(pimsmCb,
                                         grpNode.pimsmGrpAddr,
                                         grpNode.pimsmGrpMaskLength,
                                         bsrRpNode.pimsmRpAddr,
                                         PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                         bsrRpNode.pimsmRpHoldtime);

        count++;
      }

      /* update the hash mask length of all the rpmappings for this group/mask */  
    }
    else
    {
      /* store the group-rp mapping the fragment list */
      len = pimsmBsrPacketFragListStore(pimsmCb,pBsrNode,pData,
                                        &grpNode,bsrFragmentTag);
      pData += len;
    }
  }

  memcpy(pBsrPktNode.pimsmBSMPkt, pimHeader, pimPktLen);
  pBsrPktNode.pimsmBSMPktLen = pimPktLen;
  pBsrPktNode.pimsmInUse = L7_TRUE;

  /* delete the existing pkts with different fragment Id */
  if (pBsrNode->pimsmBSRFragInfo.pimsmFragTag != bsrFragmentTag)
  {
    memset(pBsrNode->pimsmLastBSMPktNode, 0,
           sizeof(pBsrNode->pimsmLastBSMPktNode));
  }
  /* add it to the list */
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,"another fragment of same BSM packet is received");
    if (pBsrNode->pimsmLastBSMPktNode[0].pimsmInUse == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,"No last BSM has been stored,"
                                    "this is the first BSM received");
      memcpy(pBsrNode->pimsmLastBSMPktNode, &pBsrPktNode, sizeof(pBsrPktNode));
      return;
    }
    for (i = 0; i< PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX; i++)
    {
      if (pBsrNode->pimsmLastBSMPktNode[i].pimsmInUse == L7_TRUE)
      {
        if (memcmp(pBsrNode->pimsmLastBSMPktNode[i].pimsmBSMPkt, pimHeader,
                   pimPktLen) == 0)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO, "received BSM fragment/packet and "
                      "existing BSM fragment/packet are same");
          return;
        }
      }
    }
  }
  for (i = 0; i< PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX; i++)
  {
    if (pBsrNode->pimsmLastBSMPktNode[i].pimsmInUse == L7_FALSE)
    {  
      pFragPacketNode = pBsrNode->pimsmLastBSMPktNode + i;
      memcpy(pFragPacketNode,&pBsrPktNode, sizeof(pBsrPktNode));
      break;
    }
  }
  pBsrNode->pimsmBSRFragInfo.pimsmFragTag = bsrFragmentTag;
  return;
}



/******************************************************************************
* @purpose  To Refresh the RP Set when the BSR times out.
*
* @param    pimsmCb    @b{(input)}  pointer to the pimsm control block.
* @param    pBsrNode   @b{(input)}  pointer to the bsr node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrRpGrpMappingRefresh(pimsmCB_t              *pimsmCb,
                                    pimsmBsrPerScopeZone_t *pBsrNode)
{
  pimsmBSMPacketNode_t   *pBsrPktNode;
  L7_uint32 i;

  /* Go through the list of BSMF to do the store rp set */
  for (i = 0; i < PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX; i++)
  {
    pBsrPktNode = pBsrNode->pimsmLastBSMPktNode + i;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG," BSM packet is used to refresh RP Group mapping");
    if (pBsrPktNode->pimsmInUse == L7_TRUE)
    {
      /* Issue the store rp command to reset the hold timer of the grp-rp mapping */
      pimsmBsrPacketRpGrpMappingStore(pimsmCb,pBsrNode,
                                      pBsrPktNode->pimsmBSMPkt,
                                      pBsrPktNode->pimsmBSMPktLen);
    }
    else
    {
      /* Applied all the BSM fragments */
      break;
    }
 }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  To Forward the BSM Message through all interfaces having neighbors
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrNode    @b{(input)} the bsr_node.
* @param    pimHeader   @b{(input)} the pointer to the pim message.
* @param    pimPktLen   @b{(input)} length of the pim packet
* @param    rtrIfNum    @b{(input)} router interface number.
*
* @returns  none
*
* @comments  forward the message on all the interfaces that have PIM neighbors 
*            except the adminscope interfaces 
*       
* @end
******************************************************************************/
void pimsmBsrPacketForward(pimsmCB_t               *pimsmCb,
                           pimsmBsrPerScopeZone_t  *pBsrNode,
                           L7_uchar8               *pimHeader,
                           L7_uint32                pimPktLen, 
                           L7_uint32                rtrIfNum)
{
  L7_uint32         rtrIfNumTemp = 0, bsrBorder;
  L7_inet_addr_t    bsrAddr; 
  pimsmInterfaceEntry_t *pIntfEntry;


  if (pBsrNode->pimsmBSRType== PIMSM_BSR_TYPE_CBSR)
  {
    inetCopy(&bsrAddr,&pBsrNode->zone.cbsr.pimsmElectedBSRAddress);
  }
  else
  {
    inetCopy(&bsrAddr,&pBsrNode->pimsmBSRAddr);
  }

  for (rtrIfNumTemp = 1; rtrIfNumTemp < MCAST_MAX_INTERFACES; rtrIfNumTemp++)
  {
    if (pimsmIntfEntryGet(pimsmCb, rtrIfNumTemp , &pIntfEntry) == L7_SUCCESS)
    {
      /* check if any bsr border is configured on that interface */
      if (pimsmMapProtocolInterfaceBsrBorderGet(pimsmCb->family, 
                                                rtrIfNumTemp, &bsrBorder) !=L7_SUCCESS)
      {
        continue;
      }
      else
      {
        if (bsrBorder == L7_ENABLE)
        {
          continue;
        }
      }      
      /* check whether there are any pim neighbors on this interface */
      if (pIntfEntry->pimsmNbrCount > 0)
      {
        /* There exist at least one neighbor so send the packet */
        L7_BOOL     retFlag;

        /* check if the adminscope boundary for this group */
        if (inetIsAddrMulticastScope(&pBsrNode->pimsmBSRGroupRange) == L7_TRUE)
        {
          retFlag = mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,rtrIfNumTemp,
                                                &pBsrNode->pimsmBSRGroupRange);
          if (retFlag ==L7_TRUE)
          {
            continue;
          }
        }
        pimsmPacketSend(pimsmCb,&pimsmCb->allPimRoutersGrpAddr, 
               &pIntfEntry->pimsmInterfaceAddr, 
                          PIMSM_BOOTSTRAP, pimHeader, pimPktLen, rtrIfNumTemp);
      }
    }
  }
  return;
}
/******************************************************************************
* @purpose  To send the BSM fragment through PIM SM interfaces having neighbors
*
* @param    pimsmCB    @b{(input)} pointer to the pimsm control block.
* @param    pBsrNode   @b{(input)} pointer to the bsr node.
* @param    pData      @b{(input)} the buffer to send
* @param    dataLen    @b{(input)} length of the data packet.
*
* @returns  None
*
* @comments 
*       
* @end
******************************************************************************/
static void pimsmBsrPacketInterfaceSend(pimsmCB_t              *pimsmCb,
                                        pimsmBsrPerScopeZone_t *pBsrNode,  
                                        L7_uchar8              *pData,
                                        L7_uint32               dataLen)
{
  L7_uint32      rtrIfNumTemp = 0, bsrBorder;
  pimsmInterfaceEntry_t *pIntfEntry;

  for (rtrIfNumTemp = 1; rtrIfNumTemp < MCAST_MAX_INTERFACES; rtrIfNumTemp++)
  {
    if (pimsmIntfEntryGet(pimsmCb, rtrIfNumTemp , &pIntfEntry) == L7_SUCCESS)
    {
      /* check if any bsr border is configured on that interface */
      if (pimsmMapProtocolInterfaceBsrBorderGet(pimsmCb->family, rtrIfNumTemp, 
                                                &bsrBorder) !=L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,
        " Failed to get BSR border status for rtrIfNum = %d ",rtrIfNumTemp);
        continue;
      }
      else
      {
        if (bsrBorder == L7_ENABLE)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,
           " Unable to send BSM on rtrIfNum = %d as BSR border is set",
           rtrIfNumTemp);
          continue;
        }
      }

      /* check whether there are any pim neighbors on this interface */
      if (pIntfEntry->pimsmNbrCount > 0)
      {
        /* There exist at least one neighbor so send the packet */
        L7_BOOL     retFlag;

        /* check if the adminscope boundary for this group */
        if (inetIsAddrMulticastScope(&pBsrNode->pimsmBSRGroupRange) == L7_TRUE)
        {
          retFlag = mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,rtrIfNumTemp,
                                                   &pBsrNode->pimsmBSRGroupRange);
          if (retFlag ==L7_TRUE)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,
               " Unable to send BSM on rtrIfNum = %d as admin scope is set",
               rtrIfNumTemp);            
             continue;
           }
        }
        pimsmPacketSend(pimsmCb,&pimsmCb->allPimRoutersGrpAddr, 
                        &pIntfEntry->pimsmInterfaceAddr,
                        PIMSM_BOOTSTRAP, pData,dataLen, rtrIfNumTemp);

      }
    }
  }
  return;
}

/******************************************************************************
* @purpose  To originate the BSR message when the router is elected BSR
*
* @param    pimsmCB       @b{(input)} pointer to the pimsm control block.
* @param    pBsrNode      @b{(input)} pointer to the bsr node.
* @param    neighborAddr  @b{(input)}address of the neighbor
* @param    rtrIfNum      @b{(input)} Interface number to send the message
*
* @returns  None
*
* @comments  If BSM needs to be send on all PIM SM interface then neighbor addr 
*            and intfIfnum should be zero.
*       
* @end
******************************************************************************/
void pimsmBsrPacketOriginate(pimsmCB_t              *pimsmCb,
                             pimsmBsrPerScopeZone_t *pBsrNode,
                             L7_inet_addr_t         *pNbrAddr,
                             L7_uint32               rtrIfNum)
{
  L7_uint32  fragTag = 0,bsrHeaderOffset,dataLen = 0;
  L7_uchar8  *pData;
  L7_uchar8  *pDataTemp = L7_NULLPTR;
  L7_BOOL adminScope;
  pimsmRpSetNode_t *pRpSetNode = L7_NULL;
  pimsmAddrList_t  *pFragRpNode = L7_NULL,*pRpNode = L7_NULL;
  L7_BOOL       noMoreInfo,fragmentedRp;
  L7_uchar8 *fragRpCntPtr = L7_NULLPTR, *rpCntPtr = L7_NULLPTR;
  L7_inet_addr_t  bsrGrpMask;
  L7_RC_t        rc;
  pimsmInterfaceEntry_t * pIntfEntry;  
  
  /* Prepare the PIM packet */
  pData = pimsmCb->pktTxBuf;
  memset(pData , 0 , PIMSM_PKT_SIZE_MAX);
  pDataTemp = (L7_uchar8 *)pData;
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_BOOTSTRAP & 0x0f)), pDataTemp);
  MCAST_PUT_BYTE(0, pDataTemp);
  MCAST_PUT_SHORT(0, pDataTemp);

  /* find out if the bsm message is for scoped zone or global zone */
  if (inetIsAddrMulticastScope(&pBsrNode->pimsmBSRGroupRange) == L7_TRUE)
  {
    /* This is scoped bsr so consider only the grp 
       that have been configured for that scope addresses */
    adminScope = L7_TRUE;
  }
  else
  {
    /* consider all the no-scoped group ranges  */
    adminScope = L7_FALSE;
  }

  /* calculate the frag id */
  do
  {
    fragTag = PIMSM_RANDOM();
  }while (pBsrNode->zone.cbsr.pimsmLastFragUsed== fragTag); 
  pBsrNode->zone.cbsr.pimsmLastFragUsed = fragTag; 
  /* fill the bsr header info */

  MCAST_PUT_SHORT(fragTag,pDataTemp);
  /* fillup the hash bsrGrpMask len */
  MCAST_PUT_BYTE(pBsrNode->pimsmBSRHashMasklen, pDataTemp);

  /* fillup the BSR priority */
  MCAST_PUT_BYTE(pBsrNode->pimsmBSRPriority, pDataTemp);

  /* fillup the BSR address in encoded unicast format*/
  PIM_PUT_EUADDR_INET(&pBsrNode->pimsmBSRAddr, pDataTemp);

  /* calculate the header offset */
  bsrHeaderOffset = (L7_uint32)pData - (L7_uint32)pDataTemp;

  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)pBsrNode->pimsmBSRGroupMask,
                    &bsrGrpMask);

  rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,pBsrNode->pimsmBSRGroupRange,
                                 bsrGrpMask,&pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    pimsmRpSetNodeNextFind(pimsmCb,pBsrNode->pimsmBSRGroupRange,
                           bsrGrpMask,&pRpSetNode);
  }

  while (1)
  {
    L7_BOOL  fillGrpEntry;
    L7_uchar8 *rpSetNodeTemp = L7_NULL;

    noMoreInfo = L7_TRUE;
    /* fill the grp/rp info as much as possible */
    while (pRpSetNode != L7_NULLPTR)
    {
      L7_inet_addr_t next_group,next_mask;
      L7_ushort16 rpCnt = 0,fragRpCnt = 0;

      inetCopy(&next_group, &pRpSetNode->pimsmGrpAddr);
      inetMaskLenToMask(pimsmCb->family,(L7_uchar8)pRpSetNode->pimsmGrpMaskLen,
                        &next_mask);
      fillGrpEntry = L7_FALSE;
      if (adminScope == L7_FALSE &&
          inetIsAddrMulticastScope(&pRpSetNode->pimsmGrpAddr) != L7_TRUE)
      {
        /* This is global scope so consider only the global scope group addresses */
        fillGrpEntry = L7_TRUE;

      }
      else if (adminScope == L7_TRUE && 
               inetIsAddrMulticastScope(&pRpSetNode->pimsmGrpAddr) == L7_TRUE &&
               pimsmBsrInfoBestGroupRangeMatch(pimsmCb,pBsrNode,
                                        pRpSetNode->pimsmGrpAddr,
                                        pRpSetNode->pimsmGrpMaskLen) == L7_TRUE)
      {
        /* This is scoped zone so consider only the scoped zone group addresses */
        if (inetAddrCompareAddrWithMask(&pRpSetNode->pimsmGrpAddr,
                                        pRpSetNode->pimsmGrpMaskLen,
                                        &pBsrNode->pimsmBSRGroupRange,
                                        pBsrNode->pimsmBSRGroupMask) == 0)
        {
          fillGrpEntry = L7_TRUE;
        }
        else
        {
          noMoreInfo = L7_FALSE;
        }

      }

      if (fillGrpEntry == L7_TRUE)
      {
        /* fill up the group entry and look into rp info */
        rpSetNodeTemp = pDataTemp;

        PIM_PUT_EGADDR_INET(&pRpSetNode->pimsmGrpAddr,pRpSetNode->pimsmGrpMaskLen,
                            adminScope, pDataTemp);
        rpCntPtr = pDataTemp;
        MCAST_PUT_BYTE(rpCnt , pDataTemp);
        fragRpCntPtr = pDataTemp;   
        MCAST_PUT_BYTE(fragRpCnt,pDataTemp);
        MCAST_PUT_SHORT(0, pDataTemp);        

        /* now go trough the rp info and fill the buffer */
        if (pFragRpNode == L7_NULLPTR)
          pRpNode = (pimsmAddrList_t*)SLLFirstGet(&(pRpSetNode->pimsmRpList));
        else
          pRpNode = (pimsmAddrList_t*)SLLNextGet(&(pRpSetNode->pimsmRpList),
                                                 (L7_sll_member_t*)pFragRpNode);

        /* reset the fragemented rp entry */
        pFragRpNode = L7_NULLPTR;
        fragmentedRp = L7_FALSE;
        while (pRpNode != L7_NULLPTR)
        {
          if (pRpNode->pimsmOriginFlag == L7_FALSE)
          {
            if (pRpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG ||
                pRpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
            {
              pimsmRpGrpNode_t     *pRpGrpNode;
              pRpGrpNode = (pimsmRpGrpNode_t*)pRpNode->pimsmRpGrpNode;

              dataLen = (L7_uint32)pDataTemp - (L7_uint32)pData;
              /* fill the rp details */
              if (dataLen < PIMSM_PKT_SIZE_MAX)
              {
                PIM_PUT_EUADDR_INET(&(pRpGrpNode->pimsmRpAddr), pDataTemp);
                fragRpCnt++;
                MCAST_PUT_SHORT(pRpGrpNode->pimsmRpHoldTime, pDataTemp);
                MCAST_PUT_BYTE(pRpGrpNode->pimsmRpPriority, pDataTemp);
                MCAST_PUT_BYTE(0, pDataTemp);
              }
              else if ((dataLen >= PIMSM_PKT_SIZE_MAX) && fragmentedRp == L7_FALSE)
              {
                fragmentedRp = L7_TRUE;
                pFragRpNode = pRpNode;
              }
              rpCnt++;
            }
          }
          /* check the length to do the fragmentation */
          pRpNode = (pimsmAddrList_t*)SLLNextGet(&(pRpSetNode->pimsmRpList),
                                                 (L7_sll_member_t*)pRpNode);
        }
        if (pRpNode == L7_NULLPTR)
        {
          if (fragRpCnt == 0)
          {
            memset(rpSetNodeTemp, 0, pDataTemp - rpSetNodeTemp);
            pDataTemp = rpSetNodeTemp;
          }
          else
          {
            MCAST_PUT_BYTE(fragRpCnt, fragRpCntPtr);
            MCAST_UNUSED_PARAM (fragRpCntPtr);
            MCAST_PUT_BYTE(rpCnt, rpCntPtr);
            MCAST_UNUSED_PARAM (rpCntPtr);
          }
        }
      }
      if (dataLen >= PIMSM_PKT_SIZE_MAX)
      {
        break; 
      }
      pimsmRpSetNodeNextFind(pimsmCb,next_group,next_mask,&pRpSetNode);
    }

    if (pRpSetNode == L7_NULLPTR)
    {
      noMoreInfo = L7_FALSE;
    }

    dataLen = (L7_uint32)pDataTemp - (L7_uint32)pData;
    /* send the packet */
    if (rtrIfNum == 0)
    {
      pimsmBsrPacketInterfaceSend(pimsmCb, pBsrNode, pData, dataLen);
    }
    else
    {
      if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) == L7_SUCCESS)
      {
        pimsmPacketSend(pimsmCb,pNbrAddr,&pIntfEntry->pimsmInterfaceAddr,PIMSM_BOOTSTRAP,
                        pData,dataLen,rtrIfNum);
      } 
    }

    /* decide whether to loopback or not */
    if (noMoreInfo == L7_FALSE)
    {
      break;
    }
    /* clean up the buffer for next fragment */
    memset((pData + bsrHeaderOffset),0,(sizeof(pData)-bsrHeaderOffset));
  } /* end of the endless while loop */
  return;
}  

/******************************************************************************
* @purpose  Update the Rpset with candidate RP info when router becomes elected BSR
*
* @param    pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrNode @b{(input)} Pointer to the BSR node
*
* @returns  none
*
* @comments      
*       
* @end
***************************************************************************/
void  pimsmBsrCandRpToRpGrpListGet(pimsmCB_t              *pimsmCb,
                                   pimsmBsrPerScopeZone_t *pBsrNode)
{
  pimsmRpBlock_t         *rpBlock;
  pimsmAddrList_t        *pRpGrpAddrNode;
  pimsmRpGrpNode_t       *pRpGrpNode = L7_NULLPTR;
  pimsmCandRpConfigInfo_t rpConfigInfo;
  L7_inet_addr_t          grpMask, grpAddrTemp;
  L7_BOOL                 bsrScoped = L7_FALSE;
  L7_RC_t                 rc;
  L7_uint32               grpMaskTemp,cnt;

  rpBlock = pimsmCb->pimsmRpInfo;  
  if (inetIsAddressZero(&rpBlock->pimsmCandRpInfo.pimsmCandRpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_NOTICE, "No candidate RP to move to RpGrpList");
    return ;
  }


  /* Check if the elected BSR is scoped or not*/
  if (inetIsAddrMulticastScope(&pBsrNode->pimsmBSRGroupRange) == L7_TRUE)
  {
    bsrScoped = L7_TRUE;
  }
  else
  {
    bsrScoped = L7_FALSE;
  }

  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
  {
  if (inetIsAddressZero(
          &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr) != L7_TRUE)
  {
    inetCopy(&grpAddrTemp, 
             &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr);
    grpMaskTemp = rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpMask;
    memset(&rpConfigInfo, 0,sizeof(pimsmCandRpConfigInfo_t));

    if (bsrScoped == L7_TRUE)
    {
      if (inetAddrCompareAddrWithMask(&pBsrNode->pimsmBSRGroupRange,
                                      pBsrNode->pimsmBSRGroupMask,
                                      &grpAddrTemp,
                                      grpMaskTemp) !=0)
      {
        continue;
      }
    }
    else
    {
      if (inetIsAddrMulticastScope(&grpAddrTemp) == L7_TRUE)
      {
        continue;
      }
    }

    inetCopy(&rpConfigInfo.pimsmGrpAddr, &grpAddrTemp);
    rpConfigInfo.pimsmGrpMaskLen = grpMaskTemp;
    inetCopy(&rpConfigInfo.pimsmRpAddr, &rpBlock->pimsmCandRpInfo.pimsmCandRpAddr);
    rpConfigInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
    rpConfigInfo.pimsmRpHoldTime = rpBlock->pimsmCandRpInfo.pimsmCandRpHoldTime;
    rpConfigInfo.pimsmRpPriority = rpBlock->pimsmCandRpInfo.pimsmCandRpPriority;

    inetMaskLenToMask(pimsmCb->family, grpMaskTemp, &grpMask);
    rc = pimsmRpGrpNodeFind(pimsmCb,rpConfigInfo.pimsmGrpAddr,
                            grpMask,rpConfigInfo.pimsmRpAddr,
                            rpConfigInfo.pimsmOrigin,
                            &pRpGrpNode);



    if (rc == L7_SUCCESS)
    {
      rc = pimsmRpGrpListGrpNodeFind(pimsmCb, pRpGrpNode, 
                                     rpConfigInfo.pimsmGrpAddr,
                                     rpConfigInfo.pimsmGrpMaskLen,
                                     rpConfigInfo.pimsmOrigin,&pRpGrpAddrNode);
      if (rc == L7_SUCCESS)
      {
        continue;
      }
    }
    rpConfigInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
    pimsmRpGrpMappingAdd(pimsmCb, &rpConfigInfo);
    pimsmRprGrpMappingExpireTimerSet(pimsmCb, rpConfigInfo.pimsmGrpAddr, 
                                     rpConfigInfo.pimsmGrpMaskLen,
                                     rpConfigInfo.pimsmRpAddr,
                                     PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR, 
                                     rpConfigInfo.pimsmRpHoldTime);

  }
  }
}


/******************************************************************************
* @purpose  Method to receive the BSM packets.
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    pSrcAddr    @b{(input)} IP source address of the bsr message
* @param    pDestAddr   @b{(input)} IP destination address of the bsr mesasge
* @param    pimHeader   @b{(input)} the pointer to the pim message.
* @param    pimPktLen   @b{(input)} length of the pim buffer
* @param    rtrIfNum    @b{(input)} router interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrBootstrapRecv(pimsmCB_t       *pimsmCb, 
                              L7_inet_addr_t  *pSrcAddr,
                              L7_inet_addr_t  *pDestAddr,
                              L7_uchar8       *pimHeader,
                              L7_uint32        pimPktLen, 
                              L7_uint32        rtrIfNum)
{
  pimsmBsrPerScopeZone_t         *pBsrNode;
  L7_uchar8                      *pData;
  L7_short16                      bsrFragmentTag;
  L7_uchar8                       bsrHashMasklen;
  L7_uint32                       bsrPriority;
  pim_encod_uni_addr_t            encodBsrAddr;
  L7_inet_addr_t                  bsrAddr,grpAddr;
  pim_encod_grp_addr_t            encodGrpAddr;
  L7_uchar8                       grpMaskLen = 0;  
  L7_BOOL                         bsrScope, bsrAddedNewly = L7_FALSE;
  L7_RC_t                         rc;

  
  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmBsrInfo == L7_NULLPTR 
      || pimHeader == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR," Invalid input parameters ");
    return L7_FAILURE;
  }
  memset(&encodBsrAddr, 0, sizeof(pim_encod_uni_addr_t));
  memset(&encodGrpAddr, 0, sizeof(pim_encod_grp_addr_t));

  /* parse the packet to get the bsm header info */
  pData = (L7_uchar8*)(pimHeader + PIM_HEARDER_SIZE);

  /* Parse the PIMSM_BOOTSTRAP message */
  MCAST_GET_SHORT(bsrFragmentTag,pData);
  MCAST_UNUSED_PARAM (bsrFragmentTag);
  MCAST_GET_BYTE(bsrHashMasklen,pData);
  MCAST_GET_BYTE(bsrPriority,pData);
  PIM_GET_EUADDR_INET(&encodBsrAddr,pData);
  inetAddressSet(encodBsrAddr.addr_family,&encodBsrAddr.addr,&bsrAddr);

  PIM_GET_EGADDR_INET( &encodGrpAddr,pData);
  inetAddressSet(encodGrpAddr.addr_family,&encodGrpAddr.addr,&grpAddr);
  grpMaskLen = encodGrpAddr.masklen;
  MCAST_UNUSED_PARAM (pData);

  /*check if the group address has the admin scope bit set */
  if (inetIsAddrMulticastScope(&grpAddr) == L7_TRUE)
  {
    /* find the scope zone bsr address */
    bsrScope = L7_TRUE;
  }
  else
  {
    inetAddressZeroSet(pimsmCb->family, &grpAddr);
    /* find the global bsr address */
    grpMaskLen = 0;
    bsrScope = L7_FALSE;
  }

  rc = pimsmBsrInfoNodeFind(pimsmCb,grpAddr,grpMaskLen, &pBsrNode);
  if (rc == L7_SUCCESS && pBsrNode != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG," BSR already exists \n");
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,"\n BSR is newly created\n");
    pimsmBsrInfoNodeAdd(pimsmCb,grpAddr,grpMaskLen,
                             bsrAddr,bsrPriority,bsrHashMasklen,
                             PIMSM_BSR_TYPE_NONBSR, &pBsrNode);
    bsrAddedNewly = L7_TRUE;

    if (pBsrNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"unable to add BSR node to BSRList ");
      return L7_FAILURE;
    }
  }
  /* Do the validation of the received BSR message based on the draft 
     draft_ietf_pim-sm_bsr-05 section 3.1.3. */
  if (pimsmBsrPacketValidate(pimsmCb,pBsrNode,pSrcAddr,pDestAddr,&bsrAddr,
                             &grpAddr,bsrScope,rtrIfNum) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"invalid BSM packet received ");
    if (bsrAddedNewly == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_DEBUG,
                  "\n deleting newly created BSR node as validation failed\n");
      pimsmBsrInfoNodeDelete(pimsmCb, pBsrNode);
    }
    return L7_FAILURE;
  }

  /* do the triggering of the BSR FSM's */
  pimsmBsrBsrFsmTrigger(pimsmCb,pBsrNode,bsrAddr,bsrPriority, bsrHashMasklen,
                        pimHeader, pimPktLen,rtrIfNum);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Method to receive the Candidate Rp ADV Message
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
* @param    pSrcAddr   @b{(input)} source address of the CRP ADV message
* @param    pDestAddr  @b{(input)} destination address of the CRP ADV message
* @param    pimHeader  @b{(input)} the pointer to the pim message.
* @param    pimPktLen  @b{(input)} length of the pim buffer
* @param    rtrIfNum   @b{(input)} router interface number.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrCandRpAdvRecv(pimsmCB_t       *pimsmCb, 
                              L7_inet_addr_t  *pSrcAddr,
                              L7_inet_addr_t  *pDestAddr,
                              L7_uchar8       *pimHeader,
                              L7_uint32        pimPktLen, 
                              L7_uint32        rtrIfNum)
{
  L7_uchar8   prefixCount,rpPriority;
  L7_ushort16 rpHoldtime;
  L7_uint32   dataLen = 0, rtrIfNumTemp;
  pim_encod_uni_addr_t    encodRpAddr;
  pim_encod_grp_addr_t    encodGrpAddr;
  L7_inet_addr_t          rpAddr,grpAddr, grpRangeAddr;
  L7_uchar8               grpMaskLen = 0, grpRangeMaskLen = 0;
  pimsmBsrPerScopeZone_t    *pBsrNode;
  L7_uchar8     *pData;
  L7_RC_t       rc = L7_FAILURE;


  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmBsrInfo == L7_NULLPTR 
      || pimHeader == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"invalid input parameters");
    return L7_FAILURE;
  }

  memset(&encodGrpAddr, 0, sizeof(pim_encod_grp_addr_t));
  memset(&encodRpAddr, 0, sizeof(pim_encod_uni_addr_t));

  /* check if the destination is correct */
  if (mcastIpMapUnnumberedIsLocalAddress(pDestAddr, &rtrIfNumTemp) != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,
                "Candidate RP advertisement received from local interface ");
    return L7_FAILURE;
  }

  /* extract the CRP ADV Header info */
  pData = (L7_uchar8*)(pimHeader + PIM_HEARDER_SIZE);

  /* Parse the CRP message */
  MCAST_GET_BYTE(prefixCount,pData);
  MCAST_GET_BYTE(rpPriority,pData);
  MCAST_GET_SHORT(rpHoldtime,pData);
  dataLen = (L7_uint32)pData - (L7_uint32)pimHeader;

  if (pimPktLen <= dataLen)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,
                "data pkt length for CRP Adverisement is less than expected ");
    return L7_FAILURE;
  }

  /* extract the Rp Address */
  PIM_GET_EUADDR_INET( &encodRpAddr,pData);
  inetAddressSet(encodRpAddr.addr_family,&encodRpAddr.addr,&rpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,
              " Cand RP Advertisement prefix cnt = %d\n", prefixCount);

  while ( ((L7_uint32)(pData - pimHeader)) < pimPktLen)
  {
    L7_inet_addr_t   grpMask;
    pimsmRpGrpNode_t   *pRpGrpNode;

    /* extract the group address */
    if (prefixCount == 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," Received CRP Advertisement has all groups set");
      inetAllMcastGroupAddressInit(pimsmCb->family, &grpAddr);
      inetAllMcastGroupPrefixLenInit(pimsmCb->family, &grpMaskLen);
    }
    else
    {
      PIM_GET_EGADDR_INET(&encodGrpAddr,pData);
      inetAddressSet(encodGrpAddr.addr_family,&encodGrpAddr.addr,&grpAddr);
      grpMaskLen = encodGrpAddr.masklen;
    }

    if (inetIsAddrMulticastScope(&grpAddr) == L7_TRUE)
    {
      /* find the scope zone bsr address */
      inetCopy(&grpRangeAddr, &grpAddr);
      grpRangeMaskLen = grpMaskLen;
    }
    else
    {
      /* find the global bsr address */
      inetAddressZeroSet(pimsmCb->family, &grpRangeAddr);
      grpRangeMaskLen = 0;
    }

    inetMaskLenToMask(pimsmCb->family,(L7_uchar8)grpMaskLen,&grpMask);

    /* Check for each group range if I am the elected BSR else ignore the group*/
    rc = pimsmBsrInfoNodeFind(pimsmCb,grpRangeAddr, grpRangeMaskLen, &pBsrNode);
    if (rc == L7_SUCCESS)
    {
      /* check to see if I am the elected BSR */
      if (pBsrNode->pimsmBSRType== PIMSM_BSR_TYPE_CBSR &&
          pBsrNode->zone.cbsr.pimsmCandBSRState == 
                                      PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR)
      {
        /* check to see if the rp-set node exists if not create one */
        rc = pimsmRpGrpNodeFind(pimsmCb,grpAddr,grpMask,
                                rpAddr,PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                &pRpGrpNode); 
        if (rc != L7_SUCCESS)
        {
          pimsmCandRpConfigInfo_t   crpInfo;

          /* add the entry into the rp-set */
          inetCopy(&crpInfo.pimsmGrpAddr, &grpAddr);
          crpInfo.pimsmGrpMaskLen = grpMaskLen;
          crpInfo.pimsmRpHashMaskLen = pBsrNode->pimsmBSRHashMasklen;
          inetCopy(&crpInfo.pimsmRpAddr, &rpAddr);
          crpInfo.pimsmRpHoldTime = rpHoldtime;
          crpInfo.pimsmRpPriority = rpPriority;
          crpInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
          if (pimsmRpGrpMappingAdd(pimsmCb,&crpInfo) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          pimsmRprGrpMappingExpireTimerSet(pimsmCb,crpInfo.pimsmGrpAddr,
                                           crpInfo.pimsmGrpMaskLen,
                                           crpInfo.pimsmRpAddr,
                                           PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                           crpInfo.pimsmRpHoldTime);


        }
        else
        {
          /* update priority of the mapping */
          pimsmRpGrpMappingUpdate(pimsmCb,grpAddr,grpMask,rpAddr,
                                  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                  rpPriority,rpHoldtime);
        }
      }
    }
    if (prefixCount == 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO,
         " Received CRP Advertisement has all groups set");
      break;
    }
    prefixCount--;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  To unicast BSM when a new neighbor is found
*
* @param    pimsmCb   @b{(input)} Pointer to the PIM SM Control Block
* @param    rtrIfNum  @b{(input)} interface on which BSM to be sent
* @param    nbrAddr   @b{(input)} new neighbor address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrUnicastBSM(pimsmCB_t      *pimsmCb, 
                           L7_inet_addr_t *pNbrAddr,
                           L7_uint32       rtrIfNum)
{
  pimsmBsrPerScopeZone_t  *pBsrNode = L7_NULLPTR;
  L7_uint32                cnt;
  L7_uint32                rpfRtrIfNum = 0;

  for (cnt = 0; cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
  {
   pBsrNode = &(pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[cnt]);
   if (inetIsAddressZero(&pBsrNode->pimsmBSRAddr) != L7_TRUE)
   {
     /* Do not Unicast the BSM on the interface on which the the BSR info
      * is learnt.
      */
     if (pimsmRPFInterfaceGet (pimsmCb, &pBsrNode->pimsmBSRAddr, &rpfRtrIfNum)
                            != L7_SUCCESS)
     {
       continue;
     }
     if (rtrIfNum == rpfRtrIfNum)
     {
       continue;
     }

     PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_INFO," originating BSM on "
                 "rtrIfNum - %d", rtrIfNum);
     pimsmBsrPacketOriginate(pimsmCb, pBsrNode, pNbrAddr, rtrIfNum);
   }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  To set the candidate BSR configuration
*
* @param    pimsmCb         @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrAddr        @b{(input)} BSR Address
* @param    bsrPriority     @b{(input)} BSR Priority
* @param    bsrHashMaskLen  @b{(input)} hashmask length for BSR.
* @param    operMode        @b{(input)} add/delete
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/

L7_RC_t pimsmBsrCandBSRConfigProcess(pimsmCB_t      *pimsmCb,
                                     L7_inet_addr_t *pBsrAddr,
                                     L7_uint32       bsrRtrIfNum,
                                     L7_uint32       bsrPriority,
                                     L7_uint32       bsrHashMaskLen,
                                     L7_uint32       scopeId, 
                                     L7_BOOL         operMode)
{
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;
  L7_inet_addr_t grpRange, bsrAddrTemp;
  L7_uchar8   grpMaskLen = 0;
  L7_RC_t   rc;

  memset(&grpRange, 0,sizeof(L7_inet_addr_t));
  if (scopeId == 0)
  {
    inetAddressZeroSet(pimsmCb->family, &grpRange);
    grpMaskLen = 0;  
  }
  else
  {
    /* TBD : grp range setting shd be made specific to scope zones based on given scope */  
  }

  inetCopy(&bsrAddrTemp, pBsrAddr);

  if (operMode == L7_TRUE)
  {
    rc = pimsmBsrInfoNodeAdd(pimsmCb,grpRange, grpMaskLen, bsrAddrTemp, bsrPriority,
                             bsrHashMaskLen, PIMSM_BSR_TYPE_CBSR, &pBsrNode);
    if (rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR,"Failed to add BSR node ");
      return L7_FAILURE;
    }
    inetCopy(&(pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRAddress),
             pBsrAddr);
    pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRPriority =
    bsrPriority;
    pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRHashMaskLength =
    bsrHashMaskLen;
    pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRrtrIfNum =
    bsrRtrIfNum;
  }
  else if (operMode == L7_FALSE)
  {
    pimsmBsrPerScopeZone_t  *bsrNode = L7_NULLPTR;

    rc = pimsmBsrInfoNodeFind(pimsmCb,grpRange, grpMaskLen, &bsrNode);
    if (rc == L7_SUCCESS)
    {
      pimsmBsrInfoNodeDelete(pimsmCb, bsrNode);
      /*memset(&(pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo), 0,sizeof(pimsmCandBSRInfo_t));*/
       inetAddressZeroSet(pimsmCb->family,
       &pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRAddress);
       
       pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRElectedBSR 
                                                                   = L7_FALSE;
      pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRHashMaskLength = 0;
      pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRPriority = 0;
      pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRrtrIfNum = 0;
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "BSR node is not present to delete \n");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Used by external API to get the Elected BSR
*
* @param    pimsmCb   @b{(input)} Pointer to the PIM SM Control Block
* @param    pBsrNode  @b{(input)}the pointer to the BSR node.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrElectedBsrGet(pimsmCB_t               *pimsmCb,
                              pimsmBsrPerScopeZone_t **ppBsrNode)
{
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;
  L7_inet_addr_t          grpRange;
  L7_uchar8               grpMaskLen = 0;

  inetAddressZeroSet(pimsmCb->family,&(grpRange));

  if (pimsmBsrInfoNodeFind(pimsmCb,grpRange,grpMaskLen,&pBsrNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR, PIMSM_TRACE_ERROR, "Failed to find BSR node \n");
    return L7_FAILURE;    
  }

  if (pBsrNode != L7_NULLPTR && pBsrNode->pimsmBSRType == PIMSM_BSR_TYPE_CBSR)
  {
    if (inetIsAddressZero(&(pBsrNode->zone.cbsr.pimsmElectedBSRAddress)) != L7_TRUE)
    {
      *ppBsrNode = pBsrNode;
      return L7_SUCCESS;
    }
  }
  *ppBsrNode = L7_NULLPTR;
  return L7_FAILURE;
}
/******************************************************************************
* @purpose Used by external API to get the next entry for Elected BSR
*
* @param   pimsmCb   @b{(input)} Pointer to the PIM SM Control Block
* @param   pBsrNode  @b{(input)}the pointer to the BSR node.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmBsrElectedBsrNextGet(pimsmCB_t               *pimsmCb,
                                  pimsmBsrPerScopeZone_t **ppBsrNode)
{
  pimsmBsrPerScopeZone_t *pBsrNodeNext = L7_NULLPTR,*pBsrNode = L7_NULLPTR;
  L7_inet_addr_t          grpRange;
  L7_uchar8               grpMaskLen, cnt, cntTemp = 0;  
  L7_inet_addr_t          addr1, addr2;
  L7_inet_addr_t          mask1, mask2;

  inetAddressZeroSet(pimsmCb->family, &(grpRange));
  grpMaskLen = 0;

  for (cnt = 0; cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
  {
    pBsrNode = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cnt;
    if (inetIsAddressZero(&pBsrNode->pimsmBSRAddr) != L7_TRUE)
    {
      /* compare the grp address & mask combined together to get the scope */
      inetMaskLenToMask(pimsmCb->family, pBsrNode->pimsmBSRGroupMask,&mask1);
      inetMaskLenToMask(pimsmCb->family, grpMaskLen, &mask2);
      inetAddressAnd(&pBsrNode->pimsmBSRGroupRange, &mask1, &addr1);
      inetAddressAnd(&grpRange, &mask2, &addr2);
      if (L7_INET_ADDR_COMPARE(&addr1,&addr2) == 0)
      {
        for (cntTemp = cnt; cntTemp < PIMSM_BSR_SCOPE_NODES_MAX; cntTemp++)
        {
          pBsrNodeNext = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cntTemp;
          if (inetIsAddressZero(&pBsrNodeNext->pimsmBSRAddr) != L7_TRUE)
          {
            if (pBsrNodeNext->pimsmBSRType== PIMSM_BSR_TYPE_CBSR)
            {
              if (inetIsAddressZero(&(pBsrNode->zone.cbsr.pimsmElectedBSRAddress))
                                    != L7_TRUE)
              {
                *ppBsrNode = pBsrNodeNext;
                 return L7_SUCCESS;
              }
            }  
          }
        }
      }
    }
  }

  *ppBsrNode = L7_NULLPTR;
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Update the Bsr List with interfacedown event
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
* @param    rtrIfNum   @b{(input)} router interface number
*
* @returns  none
*
* @comments      
*       
* @end
***************************************************************************/
void pimsmBsrIntfDown(pimsmCB_t *pimsmCb,L7_uint32 rtrIfNum)
{
  L7_inet_addr_t intfIpMask;
  pimsmBsrPerScopeZone_t *pBsrNode, *pBsrNodeDelete = L7_NULLPTR;
  pimsmRpGrpNode_t *pRpGrpNode;
  L7_RC_t         rc;
  L7_uint32       cnt;
  pimsmInterfaceEntry_t * pIntfEntry;  
  
  if (rtrIfNum != pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRrtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR,  PIMSM_TRACE_ERROR,
      "BSR is not configured for rtrIfNum(%d)", rtrIfNum);
    return;
  } 
 
  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR,  PIMSM_TRACE_ERROR,
      "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
    return;
  } 
    
  for (cnt = 0; cnt < PIMSM_BSR_SCOPE_NODES_MAX; cnt++)
  {
   pBsrNode = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cnt;
   if (inetIsAddressZero(&pBsrNode->pimsmBSRAddr) != L7_TRUE)
   {
     /* delete all the Rp-Grp entries from that BSR */
      pimsmRpSetNode_t *pRpSetNode = L7_NULLPTR;

      /* delete the grp list under that BSR */      
      inetMaskLenToMask(pimsmCb->family,(L7_uchar8)pBsrNode->pimsmBSRGroupMask,
                        &intfIpMask);

      rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,pBsrNode->pimsmBSRGroupRange,
                                     intfIpMask,&pRpSetNode);

      if (rc != L7_SUCCESS)
      {
        pimsmRpSetNodeNextFind(pimsmCb,pBsrNode->pimsmBSRGroupRange,
                               intfIpMask,&pRpSetNode);
      }

      while (pRpSetNode != L7_NULLPTR)
      {
        L7_inet_addr_t nextGrpRangeAddr, nextGrpMask;

        inetMaskLenToMask(pimsmCb->family,(L7_uchar8)pRpSetNode->pimsmGrpMaskLen,
                          &intfIpMask);
        if (pimsmRpGrpNodeFind(pimsmCb, pRpSetNode->pimsmGrpAddr,
                               intfIpMask, pIntfEntry->pimsmInterfaceAddr,
                               PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                               &pRpGrpNode) == L7_SUCCESS)
        {
          pimsmCandRpConfigInfo_t rpConfigInfo;
          memset(&rpConfigInfo, 0, sizeof(rpConfigInfo));
          inetCopy(&rpConfigInfo.pimsmRpAddr, &pRpGrpNode->pimsmRpAddr);
          rpConfigInfo.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
          inetCopy(&rpConfigInfo.pimsmGrpAddr, &pRpSetNode->pimsmGrpAddr);
          rpConfigInfo.pimsmGrpMaskLen = pRpSetNode->pimsmGrpMaskLen;
          pimsmRpGrpMappingDelete(pimsmCb, &rpConfigInfo);
        }
        inetCopy(&nextGrpRangeAddr, &pRpSetNode->pimsmGrpAddr);
        inetMaskLenToMask(pimsmCb->family, pRpSetNode->pimsmGrpMaskLen, &nextGrpMask);
        pimsmRpSetNodeNextFind(pimsmCb, nextGrpRangeAddr, nextGrpMask,&pRpSetNode);
      }
      pBsrNodeDelete = pBsrNode;
      /*delete the BSR entry */
      pimsmBsrInfoNodeDelete(pimsmCb, pBsrNodeDelete);
   }
 }
}

/******************************************************************************
* @purpose  Used to delete all the BSR entries when PIM-SM is disabled 
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  none
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmBsrPurge(pimsmCB_t *pimsmCb)
{
  L7_uint32 i;
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;

  for (i = 0; i < PIMSM_BSR_SCOPE_NODES_MAX; i++)
  {
    if(inetIsAddressZero(&pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[i].pimsmBSRAddr)
                         != L7_TRUE)
    {
      pBsrNode = &pimsmCb->pimsmBsrInfo->pimsmBsrInfoList[i];
      pimsmBsrNodeDelete(pimsmCb,pBsrNode);
    }
   
  }
                        
}

