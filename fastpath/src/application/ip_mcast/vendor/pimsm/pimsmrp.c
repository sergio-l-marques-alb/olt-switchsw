/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsm_rp.c
*
* @purpose Contains functions related to all Cand-RP/Static RP .
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
#include "pimsmmacros.h"
#include "pimsmstarstarrptree.h"
#include "pimsmstargtree.h"
#include "pimsmsgtree.h"
#include "pimsmrp.h"
#include "pimsmbsr.h"


/******************************************************************************
* @purpose  This function calculates the hash value
*
* @param    grpAddr  @b{(input)} Group address
* @param    grpMask  @b{(input)} Group Mask
* @param    rpAddr   @b{(input)} RP address
*
* @returns  hash value - Unsigned integer
*
* @comments
*
* @end
******************************************************************************/
static
L7_uint32 pimsmRpGroupHashValueGet(L7_inet_addr_t grpAddr,
                                 L7_inet_addr_t grpMask,
                                 L7_inet_addr_t rpAddr)
{
  L7_uint32 grpHash,maskHash,rpHash;
  L7_uint32 output;
  /* The algorithm is as per draft-ietf-pipm-sm-v2-new-ll.txt section 4.7.2 */

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "GrpAddr :", &grpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "GrpMask :", &grpMask);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "RpAddr :", &rpAddr);
  grpHash = inetPimsmDigestGet(&grpAddr);
  maskHash = inetPimsmDigestGet(&grpMask);
  rpHash = inetPimsmDigestGet(&rpAddr);

  output = (1103515245 * ((1103515245 * (grpHash & maskHash)+12345) ^ rpHash)
            + 12345) % 0x80000000;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "output  : %ld", output);

  return output;
}
/******************************************************************************
* @purpose  To get the hash mask length to be used for RP-group mappings
*
* @param    pimsmCb   @b{(input)} Pointer to the PIM SM Control Block
* @param    hashMask  @b{(input)} address mask to be used for calculating
*                                 hash mask value in RP-group mapping
*
* @returns none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmRpGroupHashMaskLenGet(pimsmCB_t *pimsmCb,
                            L7_inet_addr_t grpAddr,
                            L7_uchar8      grpMaskLen,
                            PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                            L7_inet_addr_t *hashMask)
{
  pimsmBsrPerScopeZone_t   *pBsrNode = L7_NULLPTR;
  L7_uchar8                 hashMaskLen = 0;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," origin = %d", origin);

  if (origin != PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
  {
    pimsmDefaultHashMaskLenGet(pimsmCb->family, &hashMaskLen);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," hashMaskLen = %d", hashMaskLen);
    inetMaskLenToMask(pimsmCb->family, hashMaskLen, hashMask);
  }
  else
  {
    if (pimsmBsrInfoNodeFind(pimsmCb, grpAddr, grpMaskLen, &pBsrNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Failed to find Bsr node");
      inetAddressZeroSet(pimsmCb->family, hashMask);
      return;
    }

    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Found BSR node=", &pBsrNode->pimsmBSRAddr);

    if (pBsrNode != L7_NULLPTR)
    {
      if (pBsrNode->pimsmBSRType == PIMSM_BSR_TYPE_CBSR)
      {
        hashMaskLen = pBsrNode->zone.cbsr.pimsmElectedBSRHashMaskLen;
      }
      else
      {
        hashMaskLen = pBsrNode->pimsmBSRHashMasklen;
      }
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Found BSR hashMaskLen= %d", hashMaskLen);
    inetMaskLenToMask(pimsmCb->family, hashMaskLen, hashMask);
  }
}

/******************************************************************************
* @purpose  This function is used to delete the Group node from the Rp-Grp list
*
* @param    entry  @b{(input)} pointer ot the Grp node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpNodeDelete(L7_sll_member_t *pEntry)
{
  pimsmRpGrpNode_t  *rpGrpEntry = (pimsmRpGrpNode_t*)pEntry;

  /* free the list of groups */
  SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(rpGrpEntry->pimsmGrpList));

  /* remove it from the buffer pool */
  PIMSM_FREE (rpGrpEntry->pimsmCb->family, (void*) rpGrpEntry);

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This function is used to delete the Rp node from the Rp-Grp list
*
* @param    entry  @b{(input)}  pointer ot the Rp-Grp node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpGrpAddrNodeDelete(L7_sll_member_t *pEntry)
{
  pimsmAddrList_t  *addrListNode = (pimsmAddrList_t*)pEntry;

  if (addrListNode->pimsmRpGrpExpiryTimer != L7_NULLPTR)
  {
    appTimerDelete(addrListNode->pimsmCb->timerCb,
                   addrListNode->pimsmRpGrpExpiryTimer);
    addrListNode->pimsmRpGrpExpiryTimer = L7_NULLPTR;
  }
  /* delete the handle */
  if (addrListNode->pimsmRpGrpExpireTimerHandle != L7_NULL)
  {
   handleListNodeDelete(addrListNode->pimsmCb->handleList,
                       &addrListNode->pimsmRpGrpExpireTimerHandle);
   addrListNode->pimsmRpGrpExpireTimerHandle = L7_NULL;
  }
  /* remove it from the buffer pool */
  PIMSM_FREE (addrListNode->pimsmCb->family, (void*) addrListNode);

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  This function is used to Compare the Group address nodes.
*
* @param    node1 @b{(input)} Pointer to the Grp addr list node.
* @param    node2 @b{(input)} Pointer to the Grp addr list node.
* @param    len   @b{(input)} len of the node to compare.
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2.
*
* @comments
*
* @end
******************************************************************************/
L7_int32
pimsmGrpAddrNodeCompare(void *pNode1, void *pNode2, L7_uint32 len)
{
  pimsmAddrList_t  *addrListNode1 = (pimsmAddrList_t*)pNode1;
  pimsmAddrList_t  *addrListNode2 = (pimsmAddrList_t*)pNode2;
  L7_inet_addr_t addr1, addr2;
  L7_inet_addr_t mask1, mask2;

  /* compare the origin first */
  if (addrListNode1->pimsmOrigin != addrListNode2->pimsmOrigin)
  {
    return(addrListNode1->pimsmOrigin - addrListNode2->pimsmOrigin);
  }

  /* compare the grp address & mask combined together */
  inetMaskLenToMask(L7_INET_GET_FAMILY(&addrListNode1->pimsmIpAddr),
                    addrListNode1->pimsmMaskLen,&mask1);
  inetMaskLenToMask(L7_INET_GET_FAMILY(&addrListNode2->pimsmIpAddr),
                    addrListNode2->pimsmMaskLen,&mask2);
  inetAddressAnd(&addrListNode1->pimsmIpAddr, &mask1, &addr1);
  inetAddressAnd(&addrListNode2->pimsmIpAddr, &mask2, &addr2);

  return L7_INET_ADDR_COMPARE(&addr1,&addr2);

}


/******************************************************************************
* @purpose  This function is used to Compare the Rp address nodes.
*
* @param    node1 @b{(input)} Pointer to the RP addr list node.
* @param    node2 @b{(input)} Pointer to the RP addr list node.
* @param    len   @b{(input)} len of the node to compare.
*
* @returns   greater than zero if node1>node2
*            zero if node1==node2
*            less than zero if node1<node2.
*
* @comments
*
* @end
******************************************************************************/
L7_int32
pimsmRpAddrNodeCompare(void *pNode1, void *pNode2, L7_uint32 len)
{
  pimsmAddrList_t  *addrListNode1 = (pimsmAddrList_t*)pNode1;
  pimsmAddrList_t  *addrListNode2 = (pimsmAddrList_t*)pNode2;

  if (L7_INET_ADDR_COMPARE(&addrListNode1->pimsmIpAddr,
                           &addrListNode2->pimsmIpAddr) != 0)
  {
    return L7_INET_ADDR_COMPARE(&addrListNode1->pimsmIpAddr,
                                &addrListNode2->pimsmIpAddr);
  }

  return(addrListNode1->pimsmOrigin - addrListNode2->pimsmOrigin);
}


/*********************************************************************
* @purpose  Sets up a key for the RP set radix tree
*
* @param    addr         @b{(input)} Key value.
* @param    rpSetTreeKey @b{(output)} A pointer to the key
*
* @returns  none.
*
* @comments     Sets the 1st byte to the length of the key, initializes
*           pad bytes and sets the key bytes to addr.
*
* @end
*********************************************************************/
void pimsmRpSetKeySet(L7_inet_addr_t addr, rpSetTreeKey_t * pRpSetTreeKey)
{
  memset(pRpSetTreeKey, 0 , sizeof(rpSetTreeKey_t));
  pRpSetTreeKey->key[0] = 1 + sizeof(L7_inet_addr_t);
  memcpy(&pRpSetTreeKey->key[1],&addr,sizeof(L7_inet_addr_t));
}

/******************************************************************************
* @purpose  Initialize PIMSM RP Block info
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpInit(pimsmCB_t  *pimsmCb)
{
  pimsmRpBlock_t        *rpBlock;

  /* allocate memory for the rp block */
  pimsmCb->pimsmRpInfo = (pimsmRpBlock_t *)PIMSM_ALLOC (pimsmCb->family,
                                                        sizeof(pimsmRpBlock_t));

  if (pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"PIMSM: Memory allocation failed for RP control block\n");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;

  /* allocate memory for the rpset radix tree */
  rpBlock->pimsmRpSetTreeHeap = PIMSM_ALLOC (pimsmCb->family,
                                             (RADIX_TREE_HEAP_SIZE(PIMSM_RP_GRP_ENTRIES_MAX,
                                                                  sizeof(rpSetTreeKey_t))));
  if (rpBlock->pimsmRpSetTreeHeap == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"PIMSM: Memory allocation failed for RPset Tree Heap\n");
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock);
    return L7_FAILURE;
  }

  rpBlock->pimsmRpSetDataHeap = PIMSM_ALLOC (pimsmCb->family,
                                             (PIMSM_RP_GRP_ENTRIES_MAX * sizeof (pimsmRpSetNode_t)));
  if (rpBlock->pimsmRpSetDataHeap == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"PIMSM: Memory allocation failed for RPset Data Heap\n");
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetTreeHeap);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock);
    return L7_FAILURE;
  }

  /*create the tree */
  if (radixCreateTree(&rpBlock->pimsmRpSetTree,
                      rpBlock->pimsmRpSetDataHeap,
                      rpBlock->pimsmRpSetTreeHeap,
                      PIMSM_RP_GRP_ENTRIES_MAX,
                      (L7_uint32)(sizeof(pimsmRpSetNode_t)),
                      sizeof(rpSetTreeKey_t)) == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to create rpsetTree");
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetTreeHeap);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetDataHeap);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock);
    return L7_FAILURE;
  }

  /* create the linked lists */
  if (SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                 L7_SLL_ASCEND_ORDER, L7_NULL,
                 pimsmRpAddrNodeCompare,pimsmRpGrpNodeDelete,
                 &(rpBlock->pimsmRpGrpList)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to create RP-GRP List ");
    radixDeleteTree(&rpBlock->pimsmRpSetTree);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetTreeHeap);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetDataHeap);
    PIMSM_FREE (pimsmCb->family, (void*) rpBlock);
    return L7_FAILURE;
  }

  /* create the crp info structure */
  memset(&(rpBlock->pimsmCandRpInfo), 0, sizeof(pimsmCandRpInfo_t));
  inetAddressZeroSet(pimsmCb->family, &(rpBlock->pimsmCandRpInfo.pimsmCandRpAddr));
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  De-initialize PIMSM RP Block info
*
* @param    pimsmCb   @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpDeInit(pimsmCB_t  *pimsmCb)
{
  pimsmRpBlock_t        *rpBlock;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;
  SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(rpBlock->pimsmRpGrpList));

  radixDeleteTree(&rpBlock->pimsmRpSetTree);
  PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetTreeHeap);
  PIMSM_FREE (pimsmCb->family, (void*) rpBlock->pimsmRpSetDataHeap);
  PIMSM_FREE (pimsmCb->family, (void*) rpBlock);
  pimsmCb->pimsmRpInfo = L7_NULL;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds whether an RP already exists in the given Group list
*
* @param    pimsmCb @b{(input)} Pointer to the PIM SM Control Block
* @param    rpAddr  @b{(input)} contains the rp address.
*
* @returns  L7_TRUE    RP address already exists
* @returns  L7_FALSE   RP address does not exist
*
* @comments
*
* @end
******************************************************************************/
static
L7_BOOL pimsmRpSetTreeRpIsExists(pimsmRpSetNode_t  *pRpSetNode,
                                 L7_inet_addr_t     rpAddr,
                                 PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE *origin)
{
  pimsmAddrList_t *pAddrListNode = L7_NULLPTR;

  for (pAddrListNode = (pimsmAddrList_t *)SLLFirstGet(&(pRpSetNode->pimsmRpList));
      pAddrListNode != L7_NULLPTR;
      pAddrListNode = (pimsmAddrList_t *)SLLNextGet(&(pRpSetNode->pimsmRpList),
                                                    (L7_sll_member_t *)pAddrListNode))
  {
    if (L7_INET_ADDR_COMPARE(&pAddrListNode->pimsmIpAddr,&rpAddr) == 0)
    {
      if (pAddrListNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR  &&
          *origin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG)
      {
        pAddrListNode->pimsmOriginFlag = L7_TRUE;
      }
      *origin = pAddrListNode->pimsmOrigin;
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/******************************************************************************
* @purpose  Finds/Adds a Rp node from Rp-Grp List
*
* @param    pimsmCb        @b{(input)} Pointer to the PIM SM Control Block
* @param    pRrpConfigInfo @b{(input)} contains the rp info.
* @param    createFlag     @b{(input)} if true creates if node is not found
* @param    pRpGrpNode     @b{(output)} Pointer to the Rp-Group node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpGrpListRpNodeFind(pimsmCB_t      *pimsmCb,
                                 L7_inet_addr_t *pRpAddr,
                                 PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                                 pimsmRpGrpNode_t **ppRpGrpNode)
{
  pimsmRpGrpNode_t      *pRpGrpNode = L7_NULLPTR,rpGrpSearchNode;
  pimsmRpBlock_t        *rpBlock;

  rpBlock = pimsmCb->pimsmRpInfo;
  /* populate the search rp_node to do the search */
  memset(&rpGrpSearchNode,0,sizeof(pimsmRpGrpNode_t));
  inetCopy(&rpGrpSearchNode.pimsmRpAddr, pRpAddr);
  rpGrpSearchNode.pimsmOrigin = origin;

  pRpGrpNode = (pimsmRpGrpNode_t *)SLLFind(&(rpBlock->pimsmRpGrpList),
                                           (void*)&rpGrpSearchNode);
  if (pRpGrpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," Failed to find Rp node in Rp-Group List ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," For RP = ", pRpAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," For Origin = %d", origin);
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpGrpNode = pRpGrpNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a Rp node from Rp-Grp List
*
* @param    pimsmCb        @b{(input)} Pointer to the PIM SM Control Block
* @param    pRrpConfigInfo @b{(input)} contains the rp info.
* @param    createFlag     @b{(input)} if true creates if node is not found
* @param    pRpGrpNode     @b{(output)} Pointer to the Rp-Group node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpGrpListRpNodeAdd(pimsmCB_t                *pimsmCb,
                                pimsmCandRpConfigInfo_t  *pRrpConfigInfo,
                                pimsmRpGrpNode_t        **ppRpGrpNode)
{
  pimsmRpGrpNode_t      *pRpGrpNode = L7_NULLPTR,rpGrpSearchNode;
  pimsmRpBlock_t        *rpBlock;
  L7_inet_addr_t         grpHashMask;
  pimsmStarStarRpNode_t  *pStarStarRpNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;


  rpBlock = pimsmCb->pimsmRpInfo;
  /* populate the search rp_node to do the search */
  memset(&rpGrpSearchNode,0,sizeof(pimsmRpGrpNode_t));
  inetCopy(&rpGrpSearchNode.pimsmRpAddr, &pRrpConfigInfo->pimsmRpAddr);
  rpGrpSearchNode.pimsmOrigin = pRrpConfigInfo->pimsmOrigin;

  pRpGrpNode = (pimsmRpGrpNode_t *)SLLFind(&(rpBlock->pimsmRpGrpList),
                                           (void*)&rpGrpSearchNode);
  if (pRpGrpNode != L7_NULLPTR)
  {
    pRpGrpNode->pimsmRpPriority = pRrpConfigInfo->pimsmRpPriority;
    pRpGrpNode->pimsmRpHoldTime = pRrpConfigInfo->pimsmRpHoldTime;
    pRpGrpNode->pimsmRpHashMaskLen = pRrpConfigInfo->pimsmRpHashMaskLen;
    pimsmRpGroupHashMaskLenGet(pimsmCb, pRrpConfigInfo->pimsmGrpAddr,
                           pRrpConfigInfo->pimsmGrpMaskLen,
                           pRrpConfigInfo->pimsmOrigin,
                           &grpHashMask);
    pRpGrpNode->pimsmHashValue =
                pimsmRpGroupHashValueGet(pRrpConfigInfo->pimsmGrpAddr,
                                       grpHashMask,pRrpConfigInfo->pimsmRpAddr);
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_DEBUG,"Creating new RP node into RPGroup List");
    /* add the rp node in the rp-grp list */
    /* 1. Take one from the pool */
    if ((pRpGrpNode = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmRpGrpNode_t)))
                                == L7_NULLPTR)
    {
      /* something grossly wrong */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Failed to allocate buffer from Rp-Grouplist Pool Id");
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* 2. populate the rp_node */
    memset(pRpGrpNode,0,sizeof(pimsmRpGrpNode_t));
    inetCopy(&pRpGrpNode->pimsmRpAddr, &pRrpConfigInfo->pimsmRpAddr);
    pRpGrpNode->pimsmOrigin = pRrpConfigInfo->pimsmOrigin;
    pRpGrpNode->pimsmRpPriority = pRrpConfigInfo->pimsmRpPriority;
    pRpGrpNode->pimsmRpHashMaskLen = pRrpConfigInfo->pimsmRpHashMaskLen;
    pRpGrpNode->pimsmRpHoldTime = pRrpConfigInfo->pimsmRpHoldTime;

    pimsmRpGroupHashMaskLenGet(pimsmCb, pRrpConfigInfo->pimsmGrpAddr,
                           pRrpConfigInfo->pimsmGrpMaskLen,
                           pRrpConfigInfo->pimsmOrigin,
                           &grpHashMask);
    pRpGrpNode->pimsmHashValue =
                pimsmRpGroupHashValueGet(pRrpConfigInfo->pimsmGrpAddr,
                                       grpHashMask,pRrpConfigInfo->pimsmRpAddr);
    pRpGrpNode->pimsmCb =  pimsmCb;

    /* 3. Add the node to the list */
    if (SLLAdd(&(rpBlock->pimsmRpGrpList),(L7_sll_member_t*)pRpGrpNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Failed to add the Rp-Group node to the RpGrpList");
      PIMSM_FREE (pimsmCb->family, (void*) pRpGrpNode);
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* Create (*,*,RP) entry*/
    rc = pimsmStarStarRpNodeCreate(pimsmCb, &pRrpConfigInfo->pimsmRpAddr,
                                   &pStarStarRpNode);
    if ((rc != L7_SUCCESS) || (pStarStarRpNode == L7_NULLPTR))
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpNodeCreate failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "rpAddr :",&pRrpConfigInfo->pimsmRpAddr);
      SLLDelete(&(rpBlock->pimsmRpGrpList),(void*)pRpGrpNode);
      PIMSM_FREE (pimsmCb->family, (void*) pRpGrpNode);
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /*4 . Create the group-list linked list */
    if (SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                   L7_SLL_ASCEND_ORDER, L7_NULL,
                   pimsmGrpAddrNodeCompare, pimsmRpGrpAddrNodeDelete,
                   &(pRpGrpNode->pimsmGrpList)) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Failed to create group list for the Rp-Group node");
      SLLDelete(&(rpBlock->pimsmRpGrpList),(void*)pRpGrpNode);
      PIMSM_FREE (pimsmCb->family, (void*) pRpGrpNode);
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  *ppRpGrpNode = pRpGrpNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a Group node from Rp-Grp List
*
* @param    pimsmCb       @b{(input)} Pointer to the  PIM SM control Block.
* @param    pRpGrpNode    @b{(input)} Pointer to the pimsmRpGrpNode_t
* @param    grpAddr       @b{(input)} address of the multicast group.
* @param    grpMask       @b{(input)} the mask length of the group.
* @param    origin        @b{(input)} type of the origin.
* @param    pAddrListNode @b{(output)} Pointer to the Group Address list node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpListGrpNodeFind(pimsmCB_t         *pimsmCb,
                                  pimsmRpGrpNode_t  *pRpGrpNode,
                                  L7_inet_addr_t     grpAddr,
                                  L7_uchar8          grpMaskLen,
                                  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE  origin,
                                  pimsmAddrList_t  **ppAddrListNode)
{
  pimsmAddrList_t     *pAddrListNode =L7_NULLPTR, addrListSearchNode;

  /* 1. search for the node in the node's grp list */
  memset(&addrListSearchNode,0,sizeof(pimsmAddrList_t));
  inetCopy(&addrListSearchNode.pimsmIpAddr, &grpAddr);
  addrListSearchNode.pimsmOrigin = origin;
  addrListSearchNode.pimsmMaskLen = grpMaskLen;
  pAddrListNode = (pimsmAddrList_t *)SLLFind(&(pRpGrpNode->pimsmGrpList),(void*)&addrListSearchNode);
  if (pAddrListNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "Failed to find Group Node in Rp-Group List");
    *ppAddrListNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppAddrListNode = pAddrListNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a Group node from Rp-Grp List
*
* @param    pimsmCb       @b{(input)} Pointer to the  PIM SM control Block.
* @param    pRpGrpNode    @b{(input)} Pointer to the pimsmRpGrpNode_t
* @param    grpAddr       @b{(input)} address of the multicast group.
* @param    grpMask       @b{(input)} the mask length of the group.
* @param    origin        @b{(input)} type of the origin.
* @param    pAddrListNode @b{(output)} Pointer to the Group Address list node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpGrpListGrpNodeAdd(pimsmCB_t         *pimsmCb,
                                 pimsmRpGrpNode_t  *pRpGrpNode,
                                 L7_inet_addr_t     grpAddr,
                                 L7_char8           grpMaskLen,
                                 PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE  origin,
                                 pimsmAddrList_t  **ppAddrListNode)
{
  pimsmRpBlock_t     *rpBlock;
  pimsmAddrList_t    *pAddrListNode = L7_NULLPTR,addrListSearchNode;

  rpBlock = pimsmCb->pimsmRpInfo;
  /* 1. search for the node in the node's grp list */
  memset(&addrListSearchNode,0,sizeof(pimsmAddrList_t));
  inetCopy(&addrListSearchNode.pimsmIpAddr, &grpAddr);
  addrListSearchNode.pimsmOrigin = origin;
  addrListSearchNode.pimsmMaskLen = grpMaskLen;
  pAddrListNode = (pimsmAddrList_t *)SLLFind(&(pRpGrpNode->pimsmGrpList),
                                             (void*)&addrListSearchNode);

  if (pAddrListNode == L7_NULLPTR )
  {
    /* rpaddress does not exist to add it to the list */
    /* 1. Take one from the pool */

    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_DEBUG,"Creating new Group node for RpGroupList");
    if ((pAddrListNode = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmAddrList_t)))
                                   == L7_NULLPTR)
    {
      /* something grossly wrong */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                  "Unable to allocate buffer from pimsmRpGrpAddrListPoolId");
      *ppAddrListNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* 2. populate the rp_node */
    memset(pAddrListNode,0,sizeof(pAddrListNode));
    inetCopy(&pAddrListNode->pimsmIpAddr, &grpAddr);
    pAddrListNode->pimsmCb = pimsmCb;
    pAddrListNode->pimsmOrigin = origin;
    pAddrListNode->pimsmMaskLen = grpMaskLen;
    pAddrListNode->pimsmRpGrpNode = (void *)pRpGrpNode;
    /*create the handle */
    pAddrListNode->pimsmRpGrpExpireTimerHandle =
    handleListNodeStore(pimsmCb->handleList,pAddrListNode);

    /* 3. Add the node to the list */
    if (SLLAdd(&(pRpGrpNode->pimsmGrpList),(L7_sll_member_t*)pAddrListNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to add the group node to the RpGrplist");
      PIMSM_FREE (pimsmCb->family, (void*) pAddrListNode);
      *ppAddrListNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  *ppAddrListNode = pAddrListNode;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Finds/Adds a Group node RpSet tree
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr     @b{(input)} address of the multicast group.
* @param    grpMask     @b{(input)} mask of the multicast group.
* @param    pRpSetNode  @b{(output)} Pointer to the RpSet node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpSetTreeGrpNodeFind(pimsmCB_t         *pimsmCb,
                                  L7_inet_addr_t     grpAddr,
                                  L7_inet_addr_t     grpMask,
                                  pimsmRpSetNode_t **ppRpSetNode)
{
  rpSetTreeKey_t        rpSetGrpKey,rpSetMaskKey;
  L7_inet_addr_t         grpRange;
  pimsmRpSetNode_t  *pRpSetNode = L7_NULLPTR;

  memset(&grpRange, 0 ,sizeof(grpRange));

  inetAddressAnd(&grpAddr, &grpMask, &grpRange);

  /* find the node in the rpset */
  pimsmRpSetKeySet(grpRange,&rpSetGrpKey);
  pimsmRpSetKeySet(grpMask,&rpSetMaskKey);

  pRpSetNode = (pimsmRpSetNode_t *)radixLookupNode
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                &rpSetGrpKey, &rpSetMaskKey,L7_RN_EXACT);
  if (pRpSetNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable to find Group node in RpSet Tree");
    *ppRpSetNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpSetNode = pRpSetNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a Group node RpSet tree
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr     @b{(input)} address of the multicast group.
* @param    grpMask     @b{(input)} mask of the multicast group.
* @param    pRpSetNode  @b{(output)} Pointer to the RpSet node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpSetTreeGrpNodeAdd(pimsmCB_t         *pimsmCb,
                                 L7_inet_addr_t     grpAddr,
                                 L7_inet_addr_t     grpMask,
                                 pimsmRpSetNode_t **ppRpSetNode)
{
  rpSetTreeKey_t        rpSetGrpKey,rpSetMaskKey;
  L7_inet_addr_t        grpRange;
  pimsmRpSetNode_t     *pRpSetNode = L7_NULLPTR;

  memset(&grpRange, 0 ,sizeof(grpRange));

  inetAddressAnd(&grpAddr, &grpMask, &grpRange);

  /* find the node in the rpset */
  pimsmRpSetKeySet(grpRange,&rpSetGrpKey);
  pimsmRpSetKeySet(grpMask,&rpSetMaskKey);

  pRpSetNode = (pimsmRpSetNode_t *)radixLookupNode
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                &rpSetGrpKey, &rpSetMaskKey, L7_RN_EXACT);

  if (pRpSetNode == L7_NULLPTR)
  {
    pimsmRpSetNode_t    rpSetNode;
    L7_uchar8           maskLen;

    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_FATAL,"Creating new Grp node into RpSet tree");
    /* populate the rpSetNode */
    memset(&rpSetNode,0,sizeof(pimsmRpSetNode_t));
    pimsmRpSetKeySet(grpRange, &rpSetNode.pimsmRpSetRadixKey.pimsmGrpKey);
    pimsmRpSetKeySet(grpMask, &rpSetNode.pimsmRpSetRadixKey.pimsmMaskKey);
    inetCopy(&rpSetNode.pimsmGrpAddr , &grpAddr);
    inetMaskToMaskLen(&grpMask,&maskLen);
    rpSetNode.pimsmGrpMaskLen = maskLen;

    /* add the rpSetNode to the tree */
    pRpSetNode = (pimsmRpSetNode_t *)radixInsertEntry
                 (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                  &rpSetNode);

    if (pRpSetNode != L7_NULLPTR)
    {
      /* something is wrong during addition of the rpSetNode to the tree */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to add the rp node to the tree");
      *ppRpSetNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* get the node for further operations */
    pRpSetNode = (pimsmRpSetNode_t *)radixLookupNode
                 (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                  &rpSetGrpKey, &rpSetMaskKey, L7_RN_EXACT);
    if (pRpSetNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find the rp node in the tree");
      *ppRpSetNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /*create the linked list in the rp_set node */
    if (SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                   L7_SLL_ASCEND_ORDER, L7_NULL,
                   pimsmRpAddrNodeCompare, pimsmRpGrpAddrNodeDelete,
                   &(pRpSetNode->pimsmRpList)) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to create rp list for the  rp node in tree");
      radixDeleteEntry(&pimsmCb->pimsmRpInfo->pimsmRpSetTree,(void*)pRpSetNode);
      *ppRpSetNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  *ppRpSetNode = pRpSetNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a RP node RpSet tree
*
* @param    rpBlock        @b{(input)} Pointer to the rpBlock.
* @param    pRpSetNode     @b{(input)} Pointer to the pimsmRpSetNode_t
* @param    rpAddr         @b{(input)} address of the rp address.
* @param    origin         @b{(input)} type of the origin.
* @param    pAddrListNode  @b{(output)} Pointer to the Rp Address list node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpSetTreeRpNodeFind(pimsmCB_t        *pimsmCb,
                                 pimsmRpSetNode_t *pRpSetNode,
                                 L7_inet_addr_t    rpAddr,
                                 PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE  origin,
                                 pimsmAddrList_t **ppAddrListNode)
{
  pimsmAddrList_t    *pAddrListNode = L7_NULLPTR,addrListSearchNode;

  /* 1. search for the node in the node's rp list */
  memset(&addrListSearchNode,0,sizeof(pimsmAddrList_t));
  inetCopy(&addrListSearchNode.pimsmIpAddr, &rpAddr);
  addrListSearchNode.pimsmOrigin = origin;

  pAddrListNode = (pimsmAddrList_t *)SLLFind(&(pRpSetNode->pimsmRpList),
                                             (void*)&addrListSearchNode);
  if (pAddrListNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable find Rp node in Rp Set tree ");
    *ppAddrListNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppAddrListNode = pAddrListNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Finds/Adds a RP node RpSet tree
*
* @param    rpBlock       @b{(input)}Pointer to the rpBlock.
* @param    pRpSetNode    @b{(input)} Pointer to the pimsmRpSetNode_t
* @param    pRpGrpNode    @b{(input)} Pointer to the pimsmRpGrpNode_t
* @param    rpAddr        @b{(input)} address of the rp address.
* @param    origin        @b{(input)} type of the origin.
* @param    pAddrListNode @b{(output)} Pointer to the Rp Address list node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpSetTreeRpNodeAdd(pimsmCB_t         *pimsmCb,
                                pimsmRpSetNode_t  *pRpSetNode,
                                pimsmRpGrpNode_t  *pRpGrpNode,
                                L7_inet_addr_t     rpAddr,
                                PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE  origin,
                                pimsmAddrList_t  **ppAddrListNode)
{
  pimsmRpBlock_t     *rpBlock;
  pimsmAddrList_t    *pAddrListNode = L7_NULLPTR,addrListSearchNode;

  rpBlock = pimsmCb->pimsmRpInfo;
  /* 1. search for the node in the node's rp list */
  memset(&addrListSearchNode,0,sizeof(pimsmAddrList_t));
  inetCopy(&addrListSearchNode.pimsmIpAddr, &rpAddr);
  addrListSearchNode.pimsmOrigin = origin;

  pAddrListNode = (pimsmAddrList_t *)SLLFind(&(pRpSetNode->pimsmRpList),
                                             (void*)&addrListSearchNode);

  if (pAddrListNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_DEBUG,"Creating new Rp node into RpSet tree");
    /* rpaddress does not exist to add it to the list */
    /* 1. Take one from the pool */
    if ((pAddrListNode = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmAddrList_t)))
                                   == L7_NULLPTR)
    {
      /* something grossly wrong */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                  "Unable to allocate buffer from pimsmRpGrpAddrListPoolId");
      *ppAddrListNode = L7_NULLPTR;
      return L7_FAILURE;
    }

    /* 2. populate the pRpGrpNode */
    memset(pAddrListNode,0,sizeof(pAddrListNode));
    inetCopy(&pAddrListNode->pimsmIpAddr, &rpAddr);
    pAddrListNode->pimsmCb= pimsmCb;
    pAddrListNode->pimsmOrigin = origin;
    pAddrListNode->pimsmRpGrpNode = pRpGrpNode;
    pAddrListNode->pimsmOriginFlag = L7_FALSE;
    if (pimsmRpSetTreeRpIsExists(pRpSetNode, rpAddr, &origin) == L7_TRUE)
    {
      if (pAddrListNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR &&
          origin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG)
        pAddrListNode->pimsmOriginFlag = L7_TRUE;
    }

    /* 3. Add the node to the list */
    if (SLLAdd(&(pRpSetNode->pimsmRpList),(L7_sll_member_t*)pAddrListNode) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to add the Rp node to the GrpRpList");
      PIMSM_FREE (pimsmCb->family, (void*) pAddrListNode);
      *ppAddrListNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  *ppAddrListNode = pAddrListNode;
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose  Evaluates the prefered RP for a group/mask entry.
*
* @param    pimsmCb        @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr        @b{(input)} address of the multicast group.
* @param    grpMask        @b{(input)} mask of the multicast group.
* @param    pRpGrpNodePref @b{(output)} pointer to the Rp-Group node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   Choice is made as follows -
*              1.Choose Static Mapping with override set
*                 -> If more than one RPs,then choose one with Highest hash
*              2.Choose BSR Mappings with least priority
*                -> If more than one with same priority, choose one with  highest hash value
*              3.Static Mapping with highest hash.
*
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpSetNodePrefRpEvaluate(pimsmCB_t         *pimsmCb,
                                     L7_inet_addr_t     grpAddr,
                                     L7_inet_addr_t     grpMask,
                                     pimsmRpGrpNode_t **ppRpGrpNodePref)
{
  pimsmRpSetNode_t     *pRpSetNode;
  pimsmAddrList_t      *pAddrListRpNode;
  pimsmRpGrpNode_t     *pRpGrpNode, *pRpGrpNodePref = L7_NULLPTR;
  L7_BOOL               prefRPPresent = L7_FALSE;
  L7_RC_t               rc;

  /* find the node in the rpset */
  rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,grpAddr,
                                 grpMask,&pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find RP Set Node");
    *ppRpGrpNodePref = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* Go through all the RP entries and find the best RP */
  for (pAddrListRpNode = (pimsmAddrList_t*)SLLFirstGet(&(pRpSetNode->pimsmRpList));
      pAddrListRpNode != L7_NULLPTR;
      pAddrListRpNode = (pimsmAddrList_t*)SLLNextGet(&(pRpSetNode->pimsmRpList),
                                                     (L7_sll_member_t*)pAddrListRpNode))
  {
    pimsmRpGrpNode_t  rpGrpSearchNode;

    /* get the values from the rp nodes */
    memset(&rpGrpSearchNode,0,sizeof(pimsmRpGrpNode_t));
    inetCopy(&rpGrpSearchNode.pimsmRpAddr, &pAddrListRpNode->pimsmIpAddr);
    rpGrpSearchNode.pimsmOrigin = pAddrListRpNode->pimsmOrigin;

    pRpGrpNode = (pimsmRpGrpNode_t *)SLLFind(&(pimsmCb->pimsmRpInfo->pimsmRpGrpList),
                                             (void*)&rpGrpSearchNode);

    if (pRpGrpNode != L7_NULLPTR &&
        pRpGrpNode->pimsmOrigin != PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG)
    {
      if (pRpGrpNodePref == L7_NULLPTR)
      {
        if (pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE ||
            pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED ||
            pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
        {
          prefRPPresent = L7_TRUE;
          pRpGrpNodePref = pRpGrpNode;
          continue;
        }

      }
      else
      {
        if (pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE)
        {
          if (pRpGrpNodePref->pimsmOrigin != PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE ||
              (pRpGrpNodePref->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE &&
               pRpGrpNodePref->pimsmHashValue < pRpGrpNode->pimsmHashValue))
          {
            prefRPPresent = L7_TRUE;
            pRpGrpNodePref = pRpGrpNode;
          }
        }
        else if (pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
        {
          if (pRpGrpNodePref->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED ||
              (pRpGrpNodePref->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR &&
               pRpGrpNodePref->pimsmRpPriority > pRpGrpNode->pimsmRpPriority) ||
              (pRpGrpNodePref->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR &&
               pRpGrpNodePref->pimsmRpPriority == pRpGrpNode->pimsmRpPriority &&
               pRpGrpNodePref->pimsmHashValue < pRpGrpNode->pimsmHashValue))
          {
            prefRPPresent = L7_TRUE;
            pRpGrpNodePref = pRpGrpNode;
          }
        }
        else if (pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED)
        {
          if (pRpGrpNodePref->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED &&
              pRpGrpNodePref->pimsmHashValue < pRpGrpNode->pimsmHashValue)
          {
            prefRPPresent = L7_TRUE;
            pRpGrpNodePref = pRpGrpNode;
          }
        }
      }
    }
  } /* end of the for loop */

  if (prefRPPresent == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," Unable to find preffered RP");
    *ppRpGrpNodePref = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpGrpNodePref = pRpGrpNodePref;
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose  Deletes a Group -Rp mapping
*
* @param    pimsmCb      @b{(input)} Pointer to the PIM SM Control Block
* @param    pimsmRpInfo  @b{(input)} pointer to the GRP-RP info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This needs to be used when static RP entries/BSR messages
*               update grp-rp mapping.
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingAdd(pimsmCB_t *pimsmCb,
                             pimsmCandRpConfigInfo_t *pRpConfigInfo)
{
  pimsmRpGrpNode_t     *pRpGrpNode;
  L7_inet_addr_t        grpMask,gblPreferredRP;
  pimsmRpSetNode_t     *pRpSetNode;
  pimsmAddrList_t      *pAddrListNode;
  pimsmRpGrpNode_t     *pRpGrpNodePref;
  L7_RC_t               rc;
  L7_uint32 rtrIfNum = 0;
  L7_inet_addr_t newRpAddr;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }
  /* search in the RP list to check whether the rp entry is available */
  rc =  pimsmRpGrpListRpNodeAdd(pimsmCb,pRpConfigInfo,&pRpGrpNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to create RpGrp Node");
    return L7_FAILURE;
  }

  /* update the grp list in the rp node */
  rc = pimsmRpGrpListGrpNodeAdd(pimsmCb,pRpGrpNode,
                                pRpConfigInfo->pimsmGrpAddr,
                                pRpConfigInfo->pimsmGrpMaskLen,
                                pRpConfigInfo->pimsmOrigin,
                                &pAddrListNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to add group node ro RpGrpNode");
    return L7_FAILURE;
  }


  /* find the node in the rpset */
  inetMaskLenToMask(pimsmCb->family,pRpConfigInfo->pimsmGrpMaskLen,&grpMask);

  rc = pimsmRpSetTreeGrpNodeAdd(pimsmCb,pRpConfigInfo->pimsmGrpAddr,
                                grpMask, &pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable create RP Set Node");
    return L7_FAILURE;
  }

  rc = pimsmRpSetTreeRpNodeAdd(pimsmCb,pRpSetNode,pRpGrpNode,
                               pRpConfigInfo->pimsmRpAddr,
                               pRpConfigInfo->pimsmOrigin,
                               &pAddrListNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to add RP node to RpSetNode");
    return L7_FAILURE;
  }

  /* Calculate the preferred RP in the group list */
  pimsmRpSetNodePrefRpEvaluate(pimsmCb,pRpConfigInfo->pimsmGrpAddr,grpMask,
                                    &pRpGrpNodePref);


  if (pRpGrpNodePref  != pRpSetNode->pimsmPrefRpGrpNode)
  {
    /* Assign the new pref rp to the node and initiate the notification process */
    pRpSetNode->pimsmPrefRpGrpNode = pRpGrpNodePref;

    pimsmRpAddressGet(pimsmCb, &pRpConfigInfo->pimsmGrpAddr, &gblPreferredRP);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"local PrefRP = ",&pRpGrpNodePref->pimsmRpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," global PrefRP = ",&gblPreferredRP);

    if (L7_INET_ADDR_COMPARE(&gblPreferredRP, &pRpGrpNodePref->pimsmRpAddr) == 0)
    {
      inetCopy(&newRpAddr, &(pRpGrpNodePref->pimsmRpAddr));

      pimsmStarGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                                 pRpConfigInfo->pimsmGrpMaskLen,
                                 &newRpAddr);

      pimsmSGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                              pRpConfigInfo->pimsmGrpMaskLen,
                              &newRpAddr);

      pimsmStarGNegativeDelete(pimsmCb, &pRpConfigInfo->pimsmRpAddr);

      /* RP Address is New/Changed.
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

    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Deletes a Group -Rp mapping
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    pimsmRpInfo @b{(input)} pointer to the GRP-RP info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingDelete(pimsmCB_t  *pimsmCb,
                                pimsmCandRpConfigInfo_t *pRpConfigInfo)
{
  pimsmRpBlock_t       *rpBlock;
  pimsmRpGrpNode_t     *pRpGrpNode,*pRpGrpNodePref, *findRpGrpNode = L7_NULLPTR;
  L7_inet_addr_t        grpMask,gblPreferredRP, newRpAddr;
  pimsmRpSetNode_t     *pRpSetNode;
  pimsmAddrList_t      *pAddrListNode;
  L7_RC_t               rc;
  pimsmStarStarRpNode_t   * pStarStarRpNode = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0, origin;


  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Entered, RP :", &pRpConfigInfo->pimsmRpAddr);
  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;

  /* search in the RPGrp list to check whether the Rp node is available */
  rc =  pimsmRpGrpListRpNodeFind(pimsmCb,&pRpConfigInfo->pimsmRpAddr,
                                 pRpConfigInfo->pimsmOrigin,
                                 &pRpGrpNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find Rp Node from RpGrpList");
    return L7_FAILURE;
  }

  /* Search the grp list of the RpGroup node */
  rc = pimsmRpGrpListGrpNodeFind(pimsmCb,pRpGrpNode,
                                 pRpConfigInfo->pimsmGrpAddr,
                                 pRpConfigInfo->pimsmGrpMaskLen,
                                 pRpConfigInfo->pimsmOrigin,
                                 &pAddrListNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find group node from RpGrpList");
    return L7_FAILURE;
  }

  /* delete the grp node in the Rplist */
  SLLNodeDelete(&(pRpGrpNode->pimsmGrpList),(L7_sll_member_t*)pAddrListNode);

  /*delete the pRpGrpNode  if no more group nodes present*/
  if (SLLFirstGet(&(pRpGrpNode->pimsmGrpList)) == L7_NULLPTR)
  {
    /* delete the grp list */
    SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(pRpGrpNode->pimsmGrpList));

    /*  delete the node */
    SLLNodeDelete(&(pimsmCb->pimsmRpInfo->pimsmRpGrpList),(L7_sll_member_t*)pRpGrpNode);

    /* Call the (*,*,RP) entry delete only if the RP cuurently deleted
       is also absent with other origins */

    for (origin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_NONE;
         origin <  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_MAX;
         origin++)
    {
       if (pimsmRpGrpListRpNodeFind(pimsmCb, &pRpConfigInfo->pimsmRpAddr, origin,
                                    &findRpGrpNode) == L7_FAILURE)
       {
         break;
       }
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "No other RP srch : origin =%d", origin);
   PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "No other RP srch : RP = ",
               &pRpConfigInfo->pimsmRpAddr);

    /* delete (*,*,RP) entry if no other RP-Grp node is found with this RP address*/
    if (origin >= PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_MAX)
    {
      rc = pimsmStarStarRpFind(pimsmCb, &pRpConfigInfo->pimsmRpAddr, &pStarStarRpNode);
      if (rc == L7_SUCCESS)
      {
        pimsmStarStarRpDelete(pimsmCb,pStarStarRpNode);
      }
    }
  }

  /* get the Group node from the RpSet tree */
  inetMaskLenToMask(pimsmCb->family,pRpConfigInfo->pimsmGrpMaskLen,&grpMask);
  rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,pRpConfigInfo->pimsmGrpAddr,grpMask,
                                 &pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find RpSet node from RpSetTree");
    return L7_FAILURE;
  }

  /* get the Rp node from the RpSet tree */
  rc = pimsmRpSetTreeRpNodeFind(pimsmCb,pRpSetNode,
                                pRpConfigInfo->pimsmRpAddr,
                                pRpConfigInfo->pimsmOrigin,
                                &pAddrListNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find Rp node from RpSet node");
    return L7_FAILURE;
  }

  /* delete the Rp node  from RpSet tree */
  SLLNodeDelete(&(pRpSetNode->pimsmRpList),(L7_sll_member_t*)pAddrListNode);

  /*delete the Group node from RpSet tree if no more Rp nodes exist */
  if (SLLFirstGet(&(pRpSetNode->pimsmRpList)) == L7_NULLPTR)
  {
    L7_inet_addr_t newRpAddr;

    /* delete the grp list */
    SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID,&(pRpSetNode->pimsmRpList));

    /*  delete the node */
    radixDeleteEntry(&rpBlock->pimsmRpSetTree,(void *)pRpSetNode);

    inetAddressZeroSet(pimsmCb->family, &newRpAddr);

    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "Notifying MRT table for Rp deletion");

    pimsmStarGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                               pRpConfigInfo->pimsmGrpMaskLen,&newRpAddr);

    pimsmSGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                            pRpConfigInfo->pimsmGrpMaskLen, &newRpAddr);

  }
  else
  {
    /* Calculate the preferred RP in the group list */
    pimsmRpSetNodePrefRpEvaluate(pimsmCb,pRpConfigInfo->pimsmGrpAddr,grpMask,
                                      &pRpGrpNodePref);

    if (pRpGrpNodePref  != pRpSetNode->pimsmPrefRpGrpNode)
    {
      /* Assign the new pref rp to the node and initiate the notification process */
      pRpSetNode->pimsmPrefRpGrpNode = pRpGrpNodePref;

      pimsmRpAddressGet(pimsmCb, &pRpConfigInfo->pimsmGrpAddr, &gblPreferredRP);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"local PrefRP = ",&pRpGrpNodePref->pimsmRpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," global PrefRP = ",&gblPreferredRP);
      if (L7_INET_ADDR_COMPARE(&gblPreferredRP, &pRpGrpNodePref->pimsmRpAddr) == 0)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO, "Notifying MRT table for Rp Change");

        inetCopy(&newRpAddr, &(pRpGrpNodePref->pimsmRpAddr));
        pimsmStarGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                                   pRpConfigInfo->pimsmGrpMaskLen,
                                   &newRpAddr);

        pimsmSGTreeRPChngUpdate(pimsmCb, &(pRpConfigInfo->pimsmGrpAddr),
                                pRpConfigInfo->pimsmGrpMaskLen,
                                &newRpAddr);

        pimsmStarGNegativeDelete(pimsmCb, &pRpConfigInfo->pimsmRpAddr);
        /* RP Address is New/Changed.
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
      }
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose   This function is used to send the Candidate RP Advertisement message
*
* @param     pimsmCb           @b{(input)}  Pointer to the PIM SM Control Block
* @param     bsrAddr           @b{(input)}  bsr adress.
* @param     pCandRpConfigInfo @b{(input)}  the candiate rp info pointer.
* @param     pCandRpGrpAdvList @b{(input)}  The list of group entries to be sent.
* @param     numGrpEntries     @b{(input)} number of entries
*
* @returns  None.
*
* @comments
*
* @end
******************************************************************************/
static
void pimsmRpCandRpAdvSend(pimsmCB_t                *pimsmCb,
                          pimsmCandRpInfo_t        *pCandRpConfigInfo,
                          pimsmCandRpGrpAdvList_t  *pCandRpGrpAdvList,
                          L7_inet_addr_t            bsrAddr,
                          L7_uchar8                 numGrpEntries,
                          L7_BOOL                   allMcastGrps)
{
  L7_uchar8     *pDataTemp,*pData;
  L7_uchar8      temp;
  L7_uint32      rtrIfNum, grpIndex,dataLen =0;
  L7_inet_addr_t srcAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,
                " Sending Candidate RP advertisement");

  if (allMcastGrps == L7_FALSE && numGrpEntries == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                " Group entries for Cand RP Advertisement is zero");
    return ;
  }

  /* Prepare the PIM packet */
  pData = pimsmCb->pktTxBuf;
  memset(pData , 0 , PIMSM_PKT_SIZE_MAX);
  pDataTemp = (L7_uchar8 *)pData;

  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_CAND_RP_ADV & 0x0f)), pDataTemp);
  MCAST_PUT_BYTE(0, pDataTemp);
  MCAST_PUT_SHORT(0, pDataTemp);

  /* fill up the prefix count */
  temp = numGrpEntries;
  MCAST_PUT_BYTE(temp,pDataTemp);

  /* fill up the priority */
  temp= pCandRpConfigInfo->pimsmCandRpPriority;
  MCAST_PUT_BYTE(temp,pDataTemp);

  /* fillup the holdtime */
  MCAST_PUT_SHORT(pCandRpConfigInfo->pimsmCandRpHoldTime,pDataTemp);

  /* fillup the rp address */
  PIM_PUT_EUADDR_INET(&pCandRpConfigInfo->pimsmCandRpAddr,pDataTemp);

  /* now iterate through the grp list and fill up all teh group address */
  for (grpIndex =0;grpIndex < numGrpEntries;grpIndex++)
  {
    L7_uchar8   scoped;
    /* TBD: Do this setting of the adminscope bit only for the ZBR */
    if (inetIsAddrMulticastScope(&pCandRpGrpAdvList->grpAddr) == L7_TRUE)
      scoped = 1;
    else
      scoped = 0;

    PIM_PUT_EGADDR_INET(&pCandRpGrpAdvList->grpAddr,pCandRpGrpAdvList->grpMaskLen,
                        scoped,pDataTemp);

    /* additional check */
    pCandRpGrpAdvList = pCandRpGrpAdvList + 1;
    if ((pDataTemp - pData) > PIMSM_PKT_SIZE_MAX)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                  "Candidate Rp advertisement exceeded max pimsm packet length");
      return;
    }
  }
  dataLen = pDataTemp - pData;

  /* get the rtrinterface to send the packet */
  if (pimsmRPFInterfaceGet(pimsmCb ,&bsrAddr,&rtrIfNum) == L7_SUCCESS)
  {
    /* Cand Rp is not fwded if BSR is local to router, and entries are updated
       to the BSR as if cand Rp received if the Elected BSR is local */
    if (rtrIfNum > 0)
    {
      if (mcastIpMapGlobalIpAddressGet(pimsmCb->family, rtrIfNum, &srcAddr) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                    "Unable to get ip address for rtrIfNum = %d", rtrIfNum);
        return;
      }
      pimsmPacketSend(pimsmCb,&bsrAddr, &srcAddr, PIMSM_CAND_RP_ADV,pData,
                      dataLen,rtrIfNum);
    }
  }
  return;
}

/******************************************************************************
* @purpose   This function is used to send the Candidate RP Advertisement message
*
* @param     pimsmCb   @b{(input)}  Pointer to the PIM SM Control Block
* @param     holdTime  @b{(input)} The holdtime to be advertised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Holdtime would be zero when the RP is going down.
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpCandRpAdvCreate(pimsmCB_t *pimsmCb, L7_uint32 rpHoldTime)
{
  pimsmRpBlock_t            *rpBlock;
  pimsmBsrPerScopeZone_t    *pBsrNode;
  pimsmCandRpInfo_t         *pCandRpInfo;
  L7_BOOL                    adminScope , bsrLocal = L7_TRUE;
  pimsmCandRpGrpAdvList_t    rpGrpScopedList[PIMSM_CAND_RP_GROUPS_MAX];
  L7_uchar8                  numGrpEntries, grpMaskLenGbl;
  L7_inet_addr_t             bsrAddr, grpAddrTemp, grpAddrGbl;
  L7_BOOL                    allMcastGrps = L7_FALSE;
  L7_uint32                  grpMaskTemp,cnt ,cntBsr;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;
  pCandRpInfo = &rpBlock->pimsmCandRpInfo;


  if (rpHoldTime < pCandRpInfo->pimsmCandRpHoldTime)
  {
    pCandRpInfo->pimsmCandRpHoldTime = rpHoldTime;
  }

  inetAddressZeroSet(pimsmCb->family, &bsrAddr);


  /* 1. Go through the BSR List of the BSR's */
  for (cntBsr = 0; cntBsr < PIMSM_BSR_SCOPE_NODES_MAX; cntBsr++)
  {
   pBsrNode = pimsmCb->pimsmBsrInfo->pimsmBsrInfoList + cntBsr;
  if(inetIsAddressZero(&pBsrNode->pimsmBSRAddr) != L7_TRUE)
  {
    /* memset the buffers*/
    memset(rpGrpScopedList,0,sizeof(rpGrpScopedList));
    numGrpEntries = 0;

    /* 2. Get the scope zone the BSR supports and scan the CRP config list for the
        range of group addresses that fall in that category */
    if (inetIsAddrMulticastScope(&pBsrNode->pimsmBSRGroupRange) == L7_TRUE)
    {
      /* This is scoped bsr so consider only the rp
         that have been configured for that scope addresses */
      adminScope = L7_TRUE;
    }
    else
    {
      /* consider all the no-scoped group ranges in this packet */
      adminScope = L7_FALSE;
      if (pBsrNode->pimsmBSRType == PIMSM_BSR_TYPE_CBSR)
      {
        inetCopy (&bsrAddr, &pBsrNode->zone.cbsr.pimsmElectedBSRAddress);
      }
      else
      {
        inetCopy (&bsrAddr, &pBsrNode->pimsmBSRAddr);
      }
      /* only candidate BSRs know about elected BSR */
    }

    bsrLocal = pimsmBsrIsLocal(pimsmCb, pBsrNode->pimsmBSRGroupRange,
                        pBsrNode->pimsmBSRGroupMask);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," BSR Local = %d ", bsrLocal);

    /* iterate through the crp info to get the list of groups to advertise */
  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
  {
  if (inetIsAddressZero(
          &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr) != L7_TRUE)
  {
    inetCopy(&grpAddrTemp,
             &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr);
    grpMaskTemp = rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpMask;

    if (inetAddrCompareAddrWithMask(&pBsrNode->pimsmBSRGroupRange,
                                      pBsrNode->pimsmBSRGroupMask,
                                      &grpAddrTemp,
                                      grpMaskTemp) !=0)
      {
        continue;
      }

      if (adminScope == L7_TRUE &&
          inetIsAddrMulticastScope(&grpAddrTemp) == L7_TRUE)
      {
        /* Need to iterate BSR list to get the smallest R that can be contained
           the group list addr */
        if (pimsmBsrInfoBestGroupRangeMatch(pimsmCb,pBsrNode,
                                            grpAddrTemp,
                                            grpMaskTemp) == L7_TRUE)
        {
          if (bsrLocal == L7_TRUE)
          {
            pimsmRprGrpMappingExpireTimerSet(pimsmCb, grpAddrTemp,
                                             grpMaskTemp,
                                             pCandRpInfo->pimsmCandRpAddr,
                                             PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                             pCandRpInfo->pimsmCandRpHoldTime);
          }
          if (inetIsAllMcastGroupAddress(&grpAddrTemp) == L7_TRUE &&
              inetIsAllMcastGroupPrefixLen(pimsmCb->family,grpMaskTemp) == L7_TRUE)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," found all mcast grps in candidate RP \n");
            allMcastGrps = L7_TRUE;
            numGrpEntries = 0;
            continue;
          }
          /* consider this grp for this crp-adv to this BSR */
          inetCopy(&rpGrpScopedList[numGrpEntries].grpAddr,
                   &grpAddrTemp);
          rpGrpScopedList[numGrpEntries].grpMaskLen = grpMaskTemp;
          numGrpEntries++;
        }
      }
    }
   }

    /* 3. Advertise it to each BSR - for all BSR that have atleast one group entry */
    if (bsrLocal != L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," FIRST : Sending Cand-Rp ");
    pimsmRpCandRpAdvSend(pimsmCb,pCandRpInfo,rpGrpScopedList,
                         pBsrNode->pimsmBSRAddr,numGrpEntries,allMcastGrps);
    }

  }
   }

  /*4 . send the unscoped & missed out entries to the unscoped bsr */
  numGrpEntries = 0;
   /* global Bsr is decided (implementation specific) to have zero
      group range and grp mask */
  inetAddressZeroSet(pimsmCb->family, &grpAddrGbl);
  grpMaskLenGbl = 0;
  bsrLocal = pimsmBsrIsLocal(pimsmCb, grpAddrGbl, grpMaskLenGbl);

  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
  {
  if (inetIsAddressZero(
          &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr) != L7_TRUE)
  {
    inetCopy(&grpAddrTemp,
             &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr);
    grpMaskTemp = rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpMask;

      if (bsrLocal == L7_TRUE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," refreshing timer for grp ",
                         &grpAddrTemp);
        pimsmRprGrpMappingExpireTimerSet(pimsmCb, grpAddrTemp,
                                         grpMaskTemp,
                                         pCandRpInfo->pimsmCandRpAddr,
                                         PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                         pCandRpInfo->pimsmCandRpHoldTime);
      }
      if (inetIsAllMcastGroupAddress(&grpAddrTemp) == L7_TRUE &&
          inetIsAllMcastGroupPrefixLen(pimsmCb->family,grpMaskTemp) == L7_TRUE)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," found all mcast grps in candidate RP \n");
        allMcastGrps = L7_TRUE;
        numGrpEntries = 0;
        continue;
      }
      inetCopy(&rpGrpScopedList[numGrpEntries].grpAddr, &grpAddrTemp);
      rpGrpScopedList[numGrpEntries].grpMaskLen = grpMaskTemp;
      numGrpEntries++;
   }
}
  if (bsrLocal != L7_TRUE)
  {
  pimsmRpCandRpAdvSend(pimsmCb,pCandRpInfo,rpGrpScopedList,bsrAddr,
                       numGrpEntries,allMcastGrps);
  }
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Handler for Grp-RP Mapping Expiry Handler
*
* @param    pParam  @b{(input)}Pointer to the timeout node.
*
* @returns  void
*
* @comments
*
* @end
******************************************************************************/
static
void pimsmRpGrpMappingTimerExpiresHandler(void *pParam)
{
  L7_int32                 handle = (L7_int32)pParam;
  pimsmAddrList_t         *pAddrListNode = L7_NULLPTR;
  pimsmCandRpConfigInfo_t  rpConfigInfo;
  pimsmRpGrpNode_t  *pRpGrpNode;

  pAddrListNode = (pimsmAddrList_t*)handleListNodeRetrieve(handle);

  if (pAddrListNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," unable to retreive the rp-group node handle ");
    return;
  }
  if (pAddrListNode->pimsmRpGrpExpiryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmRpGrpExpiryTimer is NULL, But Still Expired");
    return;
  }
  pAddrListNode->pimsmRpGrpExpiryTimer = L7_NULLPTR;

  pRpGrpNode = (pimsmRpGrpNode_t*)pAddrListNode->pimsmRpGrpNode;

  inetCopy(&rpConfigInfo.pimsmGrpAddr, &pAddrListNode->pimsmIpAddr);
  rpConfigInfo.pimsmGrpMaskLen = pAddrListNode->pimsmMaskLen;
  inetCopy(&rpConfigInfo.pimsmRpAddr, &pRpGrpNode->pimsmRpAddr);
  rpConfigInfo.pimsmOrigin = pAddrListNode->pimsmOrigin;

  /* delete the entry */
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," RP : ",&rpConfigInfo.pimsmRpAddr);
  pimsmRpGrpMappingDelete(pRpGrpNode->pimsmCb,&rpConfigInfo);
}
/******************************************************************************
* @purpose   Handler when the candidate RP advertisement timer times out.
*
* @param     pimsmCb  @b{(input)}  Pointer to the PIM SM Control Block
*
* @returns   none
*
* @comments  This would initiate the sending of the crp adv msg.
*
* @end
******************************************************************************/
static
void pimsmRpCandRpAdvTimerExpiresHandler(void *pParam)
{
  pimsmCB_t *pimsmCb = (pimsmCB_t *)pParam;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"Candidate RP advertisement timer expired");
  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return;
  }

  if (pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmCandRpAdvTimer is NULL, But Still Expired");
    return;
  }
  pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer = L7_NULLPTR;

  /* just call the above enable function to do the stuff */
  pimsmRpCandRpAdvEnable(pimsmCb);
}
/******************************************************************************
* @purpose   This function is used to enable candidate RP adv.
*
* @param     pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function will be called when candidate rp is configured
*            or interface is enabled.
*
* @end
******************************************************************************/
L7_RC_t pimsmRpCandRpAdvEnable(pimsmCB_t *pimsmCb)
{
  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  /* send a message immediately */
  pimsmRpCandRpAdvCreate(pimsmCb,PIMSM_DEFAULT_CAND_RP_HOLDTIME);

  /* start the timer */
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmRpCandRpAdvTimerExpiresHandler,
                            (void*)pimsmCb,
                            PIMSM_DEFAULT_CAND_RP_ADV_PERIOD,
                            &(pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer),
                            "CRP-AT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable to start Candidate Rp advertisement time");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose   This function is used to disable candidate RP adv.
*
* @param     pimsmCb @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function will be called when candidate rp is removed,
*            or interface is disabled.
*
* @end
******************************************************************************/
static
L7_RC_t pimsmRpCandRpAdvDisable(pimsmCB_t *pimsmCb)
{
  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  if (inetIsAddressZero(&pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," RP Address is NULL");
    return L7_SUCCESS;
  }

  /* send a message immediately with holdtime zero*/
  pimsmRpCandRpAdvCreate(pimsmCb,0);

  /* stop the timer - it should not matter even if it is not started.*/
  pimsmUtilAppTimerCancel (pimsmCb, &(pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer));

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Searches for a group range in Candidate RP group list and returns
*           the index where the range is stored or a free slot index to
*           store the new range
*
* @param    pimsmCb     @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr     @b{(input)} address of the multicast group.
* @param    grpMask     @b{(input)} mask of the multicast group.
* @param    index       @b{(output)}index of array where the given group
*                                    range is stored
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   This is used when ip pim rp-candidate <intf> <group-addr> <mask>
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpCandidateFind(pimsmCB_t     *pimsmCb,
                                L7_inet_addr_t grpAddr,
                                L7_uint32      grpMaskLen,
                                L7_uint32     *index)
{
  pimsmCandRpInfo_t *candRpInfo;
  L7_uint32 cnt;

  candRpInfo = &pimsmCb->pimsmRpInfo->pimsmCandRpInfo;

  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
  {
    if (inetIsAddressZero(&candRpInfo->pimsmCandRpGrpNode[cnt].grpAddr) != L7_TRUE)
    {
      if ((L7_INET_ADDR_COMPARE(&candRpInfo->pimsmCandRpGrpNode[cnt].grpAddr,
                                &grpAddr) == 0) &&
          (candRpInfo->pimsmCandRpGrpNode[cnt].grpMask == grpMaskLen))
      {
        *index = cnt;
        return L7_SUCCESS;
      }
    }
  }
  *index = 0;
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  Specifies the number of candidate Rp-Group nodes.
*
* @param    pimsmCb     @b{(input)}  Pointer to the PIM SM Control Block
*
* @returns  count of number of candidate Rp-Group nodes
*
* @comments
*
* @end
******************************************************************************/
L7_uint32 pimsmRpGrpCandidateCount(pimsmCB_t *pimsmCb)
{
  pimsmCandRpInfo_t *candRpInfo;
  L7_uint32 i,cnt = 0;

  candRpInfo = &pimsmCb->pimsmRpInfo->pimsmCandRpInfo;

  for (i = 0; i < PIMSM_CANDIDATE_RP_GRP_MAX; i++)
  {
    if (inetIsAddressZero(&candRpInfo->pimsmCandRpGrpNode[i].grpAddr)!= L7_TRUE)
    {
      cnt++;
    }
  }
  return cnt;
}

/******************************************************************************
* @purpose  Add  Candidate RP configuration info
*
* @param    pimsmCb     @b{(input)}  Pointer to the PIM SM Control Block
* @param    pimsmRpInfo @b{(input)}  pointer to the GRP-RP info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   This is used when ip pim rp-candidate <intf> <group-addr> <mask>
*
* @end
******************************************************************************/
L7_RC_t pimsmRpCandidateInfoAdd(pimsmCB_t *pimsmCb,
                                pimsmCandRpConfigInfo_t *pRpConfigInfo)
{
  pimsmRpBlock_t        *rpBlock;
  pimsmCandRpInfo_t     *candRpInfo;
  L7_uint32              cnt;
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;

  if (inetIsAddressZero(&rpBlock->pimsmCandRpInfo.pimsmCandRpAddr) == L7_TRUE)
  {
    inetCopy(&rpBlock->pimsmCandRpInfo.pimsmCandRpAddr, &pRpConfigInfo->pimsmRpAddr);

    rpBlock->pimsmCandRpInfo.pimsmCandRpRtrIfNum =
    pRpConfigInfo->pimsmRpRtrIfNum;
    rpBlock->pimsmCandRpInfo.pimsmCandRpPriority =
    pRpConfigInfo->pimsmRpPriority;
    rpBlock->pimsmCandRpInfo.pimsmCandRpHoldTime =
    pRpConfigInfo->pimsmRpHoldTime;
    rpBlock->pimsmCandRpInfo.pimsmCandRpHashMaskLen =
    pRpConfigInfo->pimsmRpHashMaskLen;
    /* reset the group list info */
    for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
    {
      inetAddressZeroSet(pimsmCb->family,
                     &(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr));
    }
  }
  else if (L7_INET_ADDR_COMPARE(&pRpConfigInfo->pimsmRpAddr,
                                &rpBlock->pimsmCandRpInfo.pimsmCandRpAddr)== 0)
  {
    if (pimsmRpGrpCandidateFind(pimsmCb, pRpConfigInfo->pimsmGrpAddr,
                                pRpConfigInfo->pimsmGrpMaskLen,
                                &cnt) == L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Candidate RP-Group mapping already exists");
      return L7_FAILURE;
    }
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Given candidate RP is different from configured RP");
    return L7_FAILURE;
  }

  /* 1. Take one from the pool */

  candRpInfo = &rpBlock->pimsmCandRpInfo;

  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
  {
    if (inetIsAddressZero(&candRpInfo->pimsmCandRpGrpNode[cnt].grpAddr)
                          == L7_TRUE)
    {
      inetCopy(&candRpInfo->pimsmCandRpGrpNode[cnt].grpAddr,
               &pRpConfigInfo->pimsmGrpAddr);
      candRpInfo->pimsmCandRpGrpNode[cnt].grpMask =
               pRpConfigInfo->pimsmGrpMaskLen;
      candRpInfo->pimsmCandRpGrpNode[cnt].origin =
               PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG;
      break;
    }
  }

  /* Check if the Elected BSR is Local */
  if (pimsmBsrIsLocal(pimsmCb, pRpConfigInfo->pimsmGrpAddr,
                      pRpConfigInfo->pimsmGrpMaskLen) == L7_TRUE)
  {
    pRpConfigInfo->pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;

    /* Update the BSR's Hash Mask Length */
    if (pimsmBsrInfoNodeFind (pimsmCb, pRpConfigInfo->pimsmGrpAddr,
                              pRpConfigInfo->pimsmGrpMaskLen, &pBsrNode)
                           == L7_SUCCESS)
    {
      pRpConfigInfo->pimsmRpHashMaskLen = pBsrNode->pimsmBSRHashMasklen;
    }
    if (pimsmRpGrpMappingAdd(pimsmCb, pRpConfigInfo) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                  "Unable to add the candidate Rp-Grp mapping to the RpSet List");
      return L7_FAILURE;
    }
  }

  if (pimsmRpGrpCandidateCount(pimsmCb) == 1)
  {
    pimsmRpCandRpAdvEnable(pimsmCb);
  }
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose  Delete Candidate RP configuration info
*
* @param    pimsmCb      @b{(input)} Pointer to the PIM SM Control Block
* @param    pimsmRpInfo  @b{(input)} pointer to the GRP-RP info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   This is used when no ip pim rp-candidate <intf> <group-addr> <mask>
*
* @end
******************************************************************************/
L7_RC_t pimsmRpCandidateInfoDelete(pimsmCB_t *pimsmCb,
                                   pimsmCandRpConfigInfo_t *pRpConfigInfo)
{
  pimsmRpBlock_t        *rpBlock;
  L7_uint32           index,cnt;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;

  /* delete only the group entry */

  if (L7_INET_ADDR_COMPARE(&rpBlock->pimsmCandRpInfo.pimsmCandRpAddr,
                           &pRpConfigInfo->pimsmRpAddr) != 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Wrong Rp address specofied \n");
    return L7_FAILURE;
  }

  if (pimsmRpGrpCandidateFind(pimsmCb, pRpConfigInfo->pimsmGrpAddr,
                              pRpConfigInfo->pimsmGrpMaskLen,
                              &index) == L7_SUCCESS)
  {
    L7_uint32 grpCnt = 0;

    grpCnt = pimsmRpGrpCandidateCount(pimsmCb);
    if (grpCnt == 1)
    {
      pimsmRpCandRpAdvDisable(pimsmCb);
    }
    inetAddressZeroSet(pimsmCb->family,
                   &rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[index].grpAddr);
    rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[index].grpMask = 0;
    grpCnt--;

    if (grpCnt == 0)
    {
      memset(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode, 0,
             sizeof(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode));
      inetAddressZeroSet(pimsmCb->family,
                         &(rpBlock->pimsmCandRpInfo.pimsmCandRpAddr));
          /* reset the group list info */
      for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)
      {
        inetAddressZeroSet(pimsmCb->family,
                   &(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr));
      }

    }
    pRpConfigInfo->pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," candidate RP: deleting",
                     &pRpConfigInfo->pimsmRpAddr);
      /* Check if the Elected BSR is Local */
    if (pimsmBsrIsLocal(pimsmCb, pRpConfigInfo->pimsmGrpAddr,
                        pRpConfigInfo->pimsmGrpMaskLen) == L7_TRUE)
    {
      pimsmRpGrpMappingDelete(pimsmCb, pRpConfigInfo);
    }

  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
                "Unable to delete the grpmapping as grp doesnot exist in grplist");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Delete all Candidate RP configuration info
*
* @param    pimsmCb      @b{(input)} Pointer to the PIM SM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   This is used when an interface is down on which candidate RP isconfigured
*
* @end
******************************************************************************/
static L7_RC_t pimsmRpCandidateInfoPurge(pimsmCB_t *pimsmCb)
{
  pimsmRpBlock_t        *rpBlock;
  L7_uint32              i;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;
  if (inetIsAddressZero(&rpBlock->pimsmCandRpInfo.pimsmCandRpAddr) == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  pimsmRpCandRpAdvDisable(pimsmCb);
  memset(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode, 0,
         sizeof(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode));
  inetAddressZeroSet(pimsmCb->family, &(rpBlock->pimsmCandRpInfo.pimsmCandRpAddr));
  for (i = 0; i < PIMSM_CANDIDATE_RP_GRP_MAX; i++)
  {
    inetAddressZeroSet(pimsmCb->family,
                     &(rpBlock->pimsmCandRpInfo.pimsmCandRpGrpNode[i].grpAddr));
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Set the timeout for the grp-rp mapping.
*
* @param    pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr  @b{(input)} group address
* @param    grpMask  @b{(input)} group mask len
* @param    rpAddr   @b{(input)} the rp address .
* @param    origin   @b{(input)} type of the origin.
* @param    timeout  @b{(input)} the timeout in seconds
*
* @returns  none
*
* @comments
*
* @end
******************************************************************************/
void pimsmRprGrpMappingExpireTimerSet(pimsmCB_t     *pimsmCb,
                                      L7_inet_addr_t grpAddr,
                                      L7_uchar8      grpMaskLen,
                                      L7_inet_addr_t rpAddr,
                                      PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                                      L7_uint32      timeout)
{
  pimsmRpGrpNode_t *pRpGrpNode  = L7_NULLPTR;
  pimsmCandRpConfigInfo_t  pimsmRpInfo;
  pimsmAddrList_t         *pAddrGrpNode;
  L7_RC_t                  rc;

  inetCopy (&(pimsmRpInfo.pimsmRpAddr), &rpAddr);
  pimsmRpInfo.pimsmOrigin = origin;
  inetCopy(&(pimsmRpInfo.pimsmGrpAddr), &grpAddr);
  pimsmRpInfo.pimsmGrpMaskLen = grpMaskLen;

  rc = pimsmRpGrpListRpNodeFind(pimsmCb, &rpAddr, origin,&pRpGrpNode);
  if (rc == L7_SUCCESS)
  {
    rc = pimsmRpGrpListGrpNodeFind(pimsmCb, pRpGrpNode, grpAddr,
                                   grpMaskLen,origin, &pAddrGrpNode);
    if (rc == L7_SUCCESS)
    {
      if (pimsmUtilAppTimerSet (pimsmCb, pimsmRpGrpMappingTimerExpiresHandler,
                                (void*)pAddrGrpNode->pimsmRpGrpExpireTimerHandle,
                                timeout,
                                &(pAddrGrpNode->pimsmRpGrpExpiryTimer),
                                "RPG-ET")
                             != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable to start pimsmRpGrpExpiryTimer");
        return;
      }
    }
  }
  return;
}


/******************************************************************************
* @purpose  Update the entry in the rpset
*
* @param    pimsmCb    @b{(input)}Pointer to the PIM SM Control Block
* @param    grpAddr    @b{(input)} group address
* @param    grpMask    @b{(input)} group mask len
* @param    rpAddr     @b{(input)} the rp address.
* @param    rpPriority @b{(input)} the rp priority.
* @param    rpHoldTime @b{(input)} the rp holdtime.
* @param    origin     @b{(input)} type of the origin.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingUpdate(pimsmCB_t           *pimsmCb,
                                L7_inet_addr_t       pGrpAddr,
                                L7_inet_addr_t       grpMask,
                                L7_inet_addr_t       rpAddr,
                                PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                                L7_uint32            rpPriority,
                                L7_uint32            rpHoldtime)
{
  pimsmRpGrpNode_t     *pRpNode;
  pimsmRpSetNode_t     *pRpSetNode;
  pimsmAddrList_t      *pAddrListNode;
  pimsmRpGrpNode_t     *pRpGrpNodePref;
  L7_uchar8             grpMaskLen;
  L7_inet_addr_t        gblPrefRpAddr, newRpAddr;
  L7_RC_t               rc;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input parameters");
    return L7_FAILURE;
  }

  rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,pGrpAddr,
                                 grpMask,&pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable to find Group node in RpSet Tree");
    return L7_FAILURE;
  }
  rc = pimsmRpSetTreeRpNodeFind(pimsmCb,pRpSetNode,rpAddr,origin,&pAddrListNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Unable to find Rp node from RpSet ");
    return L7_FAILURE;
  }
  pRpNode = pAddrListNode->pimsmRpGrpNode;

  /* update the rpPriority and rpHoldtime */
  pRpNode->pimsmRpPriority = rpPriority;
  pRpNode->pimsmRpHoldTime = rpHoldtime;

  /* set the timer */
  inetMaskToMaskLen(&grpMask,&grpMaskLen);
  /* set the expiry time to the rp rpHoldtime */
  pimsmRprGrpMappingExpireTimerSet(pimsmCb,
                                   pGrpAddr,
                                   grpMaskLen,
                                   rpAddr,
                                   origin,
                                   rpHoldtime);

  /* Calculate the preferred RP in the group list */
  pimsmRpSetNodePrefRpEvaluate(pimsmCb,pGrpAddr,grpMask,&pRpGrpNodePref);

  if (pRpGrpNodePref  != pRpSetNode->pimsmPrefRpGrpNode)
  {
    /* Assign the new pref rp to the node and initiate the notification process */
    pRpSetNode->pimsmPrefRpGrpNode = pRpGrpNodePref;

    pimsmRpAddressGet(pimsmCb, &pGrpAddr, &gblPrefRpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"local PrefRP = ",&pRpGrpNodePref->pimsmRpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO," global PrefRP = ",&gblPrefRpAddr);
    if (L7_INET_ADDR_COMPARE(&gblPrefRpAddr, &pRpGrpNodePref->pimsmRpAddr) == 0)
    {
      inetMaskToMaskLen(&grpMask, &grpMaskLen);
      inetCopy(&newRpAddr, &(pRpGrpNodePref->pimsmRpAddr));
      pimsmStarGTreeRPChngUpdate(pimsmCb, &pGrpAddr,
                                 grpMaskLen, &newRpAddr);

      pimsmSGTreeRPChngUpdate(pimsmCb, &pGrpAddr,
                              grpMaskLen, &newRpAddr);

      pimsmStarGNegativeDelete(pimsmCb, &rpAddr);
    }
  }
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Finds an prefered RP for an group/mask mapping.
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
* @param    grp_addr   @b{(input)} group addresss
* @param    groupmask  @b{(input)} group mask
* @param    pRpGrpNode @b{(input)} Poinetr the Rp-Group node.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingPreferredGet(pimsmCB_t         *pimsmCb,
                                      L7_inet_addr_t     grpAddr,
                                      pimsmRpGrpNode_t **ppRpGrpNode)
{
  pimsmRpSetNode_t  *pRpSetNode;
  pimsmRpGrpNode_t  *pRpGrpNode = L7_NULLPTR;
  rpSetTreeKey_t     rpSetGrpKey;

  pimsmRpSetKeySet(grpAddr, &rpSetGrpKey);
  pRpSetNode = (pimsmRpSetNode_t *)radixMatchNode
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,&rpSetGrpKey, 0);
  if (pRpSetNode != L7_NULLPTR)
  {
    pRpGrpNode = pRpSetNode->pimsmPrefRpGrpNode;
    if (pRpGrpNode == L7_NULLPTR)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," No preffered RP for group ",&grpAddr);
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
    *ppRpGrpNode = pRpGrpNode;
    return L7_SUCCESS;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Unable to find RpSet node");
  *ppRpGrpNode = L7_NULLPTR;
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  To find if the elected RP address for the group.
*
* @param    pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
* @param    pGrpAddr @b{(input)} group addresss
* @param    pRpAddr  @b{(output)} the rp address is return if exists
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/

L7_RC_t pimsmRpAddressGet(pimsmCB_t * pimsmCb,
                          L7_inet_addr_t *pGrpAddr,
                          L7_inet_addr_t *pRpAddr)
{
  pimsmRpGrpNode_t *pRpGrpNode;

  if(pimsmCb == L7_NULLPTR || pGrpAddr == L7_NULLPTR || pRpAddr == L7_NULLPTR)
  {
   PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," invalid input params");
   return L7_FAILURE;
  }
  inetAddressZeroSet(pimsmCb->family, pRpAddr);
  if (pimsmRpGrpMappingPreferredGet(pimsmCb,*pGrpAddr, &pRpGrpNode)!=L7_SUCCESS)
  {
   PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
            " No preffered RP for group ",pGrpAddr);
   return L7_FAILURE;
  }
  if (pRpGrpNode != L7_NULLPTR)
  {
    inetCopy(pRpAddr,&pRpGrpNode->pimsmRpAddr);
    return L7_SUCCESS;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,
        " NULL preffered RP for group ",pGrpAddr);
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  To find if the elected RP address for the group.
*
* @param    pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
* @param    pGrpAddr @b{(input)} group addresss
* @param    pRpAddr  @b{(output)} the rp address is return if exists
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_BOOL pimsmIAmRP(pimsmCB_t * pimsmCb,
                          L7_inet_addr_t *pGrpAddr)
{
  L7_inet_addr_t  rpAddr;

  if (pimsmCb == L7_NULLPTR || pGrpAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Invalid input parameters");
    return L7_FALSE;
  }

  if ( pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr)== L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Grp is in SSM-range" );
    return L7_FALSE;
  }

  if(pimsmRpAddressGet(pimsmCb, pGrpAddr, &rpAddr)!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "RP not found" );
    return L7_FALSE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_DEBUG, "rpAddr :",&rpAddr);

  if(mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, " I am the RP for this group ");
    return(L7_TRUE);
  }
 return L7_FALSE;
}
/******************************************************************************
* @purpose  This function is used for the external UI to give the RP Group info
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr    @b{(input)} Group address
* @param    grpMask    @b{(input)} Group Mask
* @param    rpAddr     @b{(input)} RP address
* @param    origin     @b{(input)} origin type
* @param    pRpGrpNode @b{(output)} Pointer  to Rp-Group node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpNodeFind(pimsmCB_t        *pimsmCb,
                           L7_inet_addr_t    grpAddr,
                           L7_inet_addr_t    grpMask,
                           L7_inet_addr_t    rpAddr,
                           PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE   origin,
                           pimsmRpGrpNode_t **ppRpGrpNode)
{
  pimsmRpBlock_t       *rpBlock;
  pimsmRpGrpNode_t     *pRpGrpNode = L7_NULLPTR,rpGrpSearchNode;
  pimsmRpSetNode_t     *pRpSetNode;
  pimsmAddrList_t      *pAddrListNode;
  L7_RC_t               rc;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR," Invalid input parameters");
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  rpBlock = pimsmCb->pimsmRpInfo;

  rc = pimsmRpSetTreeGrpNodeFind(pimsmCb,grpAddr,
                                 grpMask, &pRpSetNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find Group node from RpSet tree");
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  rc = pimsmRpSetTreeRpNodeFind(pimsmCb,pRpSetNode,rpAddr,origin,&pAddrListNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find Rp node from RpSet node");
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* populate the search pRpGrpNode to do the search */
  memset(&rpGrpSearchNode,0,sizeof(pimsmRpGrpNode_t));
  inetCopy(&rpGrpSearchNode.pimsmRpAddr, &rpAddr);
  rpGrpSearchNode.pimsmOrigin = origin;
  pRpGrpNode = (pimsmRpGrpNode_t *)SLLFind(&(rpBlock->pimsmRpGrpList),
                                           (void*)&rpGrpSearchNode);
  if (pRpGrpNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR, "Unable to find Rp-group node from Rp-Group list");
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpGrpNode = pRpGrpNode;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  This function is used for the external UI to give the RP set info
*
* @param    pimsmCb    @b{(input)} Pointer to the PIM SM Control Block
* @param    grpAddr    @b{(input)} Group address
* @param    grpMask    @b{(input)} Group Mask
* @param    pRpSetNode @b{(output)} Pointer  to Rp-Set node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpSetNodeFind(pimsmCB_t         *pimsmCb,
                           L7_inet_addr_t     grpAddr,
                           L7_inet_addr_t     grpMask,
                           pimsmRpSetNode_t **ppRpSetNode)
{
  rpSetTreeKey_t    rpSetGrpKey,rpSetMaskKey;
  L7_inet_addr_t    grpRange;
  pimsmRpSetNode_t *pRpSetNode = L7_NULLPTR;

  memset(&grpRange, 0, sizeof(L7_inet_addr_t));
  inetAddressAnd(&grpAddr, &grpMask, &grpRange);
  /* find the node in the rpset */
  pimsmRpSetKeySet(grpRange,&rpSetGrpKey);
  pimsmRpSetKeySet(grpMask, &rpSetMaskKey);

  pRpSetNode = (pimsmRpSetNode_t *)radixLookupNode
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                &rpSetGrpKey, &rpSetMaskKey,
                L7_RN_EXACT);
  if (pRpSetNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"Unable to find Rp Set node");
    *ppRpSetNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpSetNode = pRpSetNode;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This function is used for the external UI to give the next RP set info
*
* @param    pimsmCb    @b{(input)}  Pointer to the PIM SM Control Block
* @param    grpAddr    @b{(input)}  Group address
* @param    grpMask    @b{(input)}  Group Mask
* @param    pRpSetNode @b{(output)} Pointer  to Rp-Set node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpSetNodeNextFind(pimsmCB_t         *pimsmCb,
                               L7_inet_addr_t     grpAddr,
                               L7_inet_addr_t     grpMask,
                               pimsmRpSetNode_t **ppRpSetNode)
{
  rpSetTreeKey_t     rpSetGrpKey,rpSetMaskKey;
  L7_inet_addr_t     grpRange;
  pimsmRpSetNode_t  *pRpSetNode = L7_NULLPTR;

  if (inetIsAddressZero(&grpAddr))
  {
    pRpSetNode = (pimsmRpSetNode_t *)radixGetNextEntry
                 (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                  L7_NULLPTR);
    if (pRpSetNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"The RpSet Tree is empty");
      *ppRpSetNode = L7_NULLPTR;
      return L7_FAILURE;
    }
    *ppRpSetNode = pRpSetNode;
    return L7_SUCCESS;
  }

  memset(&grpRange, 0, sizeof(L7_inet_addr_t));
  inetAddressAnd(&grpAddr, &grpMask, &grpRange);
  /* find the node in the rpset */
  pimsmRpSetKeySet(grpRange, &rpSetGrpKey);
  pimsmRpSetKeySet(grpMask, &rpSetMaskKey);

  pRpSetNode = (pimsmRpSetNode_t *)radixLookupNode
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
                &rpSetGrpKey, &rpSetMaskKey, L7_RN_EXACT);

  if (pRpSetNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"Unable to find the RpSet node");
    *ppRpSetNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  pRpSetNode = (pimsmRpSetNode_t *)radixGetNextEntry
               (&pimsmCb->pimsmRpInfo->pimsmRpSetTree, pRpSetNode);

  if (pRpSetNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"Unable to find the Next RpSet node");
    *ppRpSetNode = L7_NULLPTR;
    return L7_FAILURE;
  }
  *ppRpSetNode = pRpSetNode;
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  This function is used for the external UI to give the next RP-Grp info
*
* @param    pimsmCb     @b{(input)}  Pointer to the PIM SM Control Block
* @param    pRpSetNode  @b{(input)}  pointer to the rpset
* @param    rpAddr      @b{(input)}  RP adderss
* @param    origin      @b{(input)}  origin type
* @param    pRpGrpNode  @b{(output)} Pointer  to Rp-Group node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpNodeNextFind(pimsmCB_t         *pimsmCb,
                               pimsmRpSetNode_t  *pRpSetNode,
                               L7_inet_addr_t     rpAddr,
                               PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                               pimsmRpGrpNode_t **ppRpGrpNode)
{
/*  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR; */
  pimsmAddrList_t addrListSearchNode,*pAddrListNode = L7_NULLPTR;

  if (pimsmCb == L7_NULLPTR || pimsmCb->pimsmRpInfo == L7_NULLPTR ||
      pRpSetNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Invalid input parameters");
    *ppRpGrpNode = L7_NULLPTR;
    return L7_FAILURE;
  }

  memset(&addrListSearchNode,0,sizeof(pimsmAddrList_t));
  addrListSearchNode.pimsmOrigin = origin;
  inetCopy(&addrListSearchNode.pimsmIpAddr , &rpAddr);

  if (inetIsAddressZero(&rpAddr) != L7_TRUE)
  {
    pAddrListNode = (pimsmAddrList_t *)SLLFind(&(pRpSetNode->pimsmRpList),
                                               (L7_sll_member_t *)&addrListSearchNode);
    if (pAddrListNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Rp List for RpSet node is empty");
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
  }
  else
  {
    pAddrListNode = (pimsmAddrList_t *)SLLFirstGet(&(pRpSetNode->pimsmRpList));
    if (pAddrListNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Rp List for RpSet node is empty");
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
    if (origin == 0 &&
        pAddrListNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED )
    {
      if (pAddrListNode->pimsmRpGrpNode == L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"No Rp-Group node with STATIC origin present");
        *ppRpGrpNode = L7_NULLPTR;
        return L7_FAILURE;
      }
      *ppRpGrpNode = pAddrListNode->pimsmRpGrpNode;
      return L7_SUCCESS;
    }
    else if (pAddrListNode->pimsmOrigin == origin)
    {
      if (pAddrListNode->pimsmRpGrpNode == L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Unable to get Rp-Group node ");
        *ppRpGrpNode = L7_NULLPTR;
        return L7_FAILURE;
      }
      *ppRpGrpNode = pAddrListNode->pimsmRpGrpNode;
      return L7_SUCCESS;
    }

  }
  while (pAddrListNode != L7_NULLPTR)
  {
    pAddrListNode = (pimsmAddrList_t *)SLLNextGet(&(pRpSetNode->pimsmRpList),
                                                  (L7_sll_member_t *)pAddrListNode);
    if (pAddrListNode != L7_NULLPTR  && pAddrListNode->pimsmOrigin == origin)
    {
      if (pAddrListNode->pimsmRpGrpNode == L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"No Rp-Group node with given origin present");
        *ppRpGrpNode = L7_NULLPTR;
        return L7_FAILURE;
      }
      *ppRpGrpNode = pAddrListNode->pimsmRpGrpNode;
      return L7_SUCCESS;
    }
  }

  if (pAddrListNode != L7_NULLPTR)
  {
    if (pAddrListNode->pimsmRpGrpNode == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_ERROR,"Unable to get Rp-Group node from RP address list");
      *ppRpGrpNode = L7_NULLPTR;
      return L7_FAILURE;
    }
    *ppRpGrpNode = pAddrListNode->pimsmRpGrpNode;
    return L7_SUCCESS;
  }
  *ppRpGrpNode = L7_NULLPTR;
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  Update the rpset with interfacedown event
*
* @param    pimsmCb  @b{(input)} Pointer to the PIM SM Control Block
* @param    rtrIfNum @b{(input)} router interface number
*
* @returns  none
*
* @comments
*
* @end
***************************************************************************/
void pimsmRpIntfDown(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmRpBlock_t          *rpBlock;
  pimsmAddrList_t         *pAddrListGrpNode = L7_NULLPTR;
  pimsmCandRpConfigInfo_t  rpGrpConfigInfo;
  pimsmRpGrpNode_t        *pRpGrpNode = L7_NULLPTR, rpGrpSearchNode;
  L7_inet_addr_t           rpAddr;

  rpBlock = pimsmCb->pimsmRpInfo;
  memset(&rpGrpConfigInfo, 0,sizeof(pimsmCandRpConfigInfo_t));

  /* check if candidate RP is configured on that interface */
  if (rtrIfNum != pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpRtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_RP, PIMSM_TRACE_INFO,"Candidate RP is not configured on disbled interface");
    return;
  }
  inetCopy(&rpAddr, &(rpBlock->pimsmCandRpInfo.pimsmCandRpAddr));
  /* delete from the Candidate RP List */
  pimsmRpCandidateInfoPurge(pimsmCb);

  /* delete any candidate RP entries present in Rp-Grp List */
  memset(&rpGrpSearchNode,0,sizeof(pimsmRpGrpNode_t));
  inetCopy(&rpGrpSearchNode.pimsmRpAddr, &rpAddr);
  rpGrpSearchNode.pimsmOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
  pRpGrpNode = (pimsmRpGrpNode_t *)SLLFind(&(rpBlock->pimsmRpGrpList),
                                           (void*)&rpGrpSearchNode);
  if (pRpGrpNode != L7_NULLPTR)
  {
    pAddrListGrpNode = (pimsmAddrList_t *)SLLFirstGet
                        (&(pRpGrpNode->pimsmGrpList));
    while (pAddrListGrpNode != L7_NULLPTR)
    {
      inetCopy(&rpGrpConfigInfo.pimsmRpAddr, &pRpGrpNode->pimsmRpAddr);
      rpGrpConfigInfo.pimsmOrigin = pRpGrpNode->pimsmOrigin;
      inetCopy(&rpGrpConfigInfo.pimsmGrpAddr, &pAddrListGrpNode->pimsmIpAddr);
      rpGrpConfigInfo.pimsmGrpMaskLen = pAddrListGrpNode->pimsmMaskLen;
      pAddrListGrpNode = (pimsmAddrList_t*)SLLNextGet(&(rpBlock->pimsmRpGrpList),
                                            (L7_sll_member_t*)pAddrListGrpNode);
      pimsmRpGrpMappingDelete(pimsmCb, &rpGrpConfigInfo);

    }
  }
}
/******************************************************************************
* @purpose  Used to delete all the Grp-Rp mappings when PIM-SM is disabled
*
* @param    pimsmCb  @b{(input)}  Pointer to the PIM SM Control Block
*
* @returns  none
*
* @comments
*
* @end
******************************************************************************/
void pimsmRpGrpMappingPurge(pimsmCB_t *pimsmCb)
{
  pimsmRpSetNode_t *pRpSetNode = L7_NULLPTR;
  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR;
  pimsmAddrList_t  *pAddrListNode = L7_NULLPTR;
  pimsmRpGrpNode_t *pRpGrpNodeDelete = L7_NULLPTR;
  pimsmAddrList_t  *pAddrListNodeDelete = L7_NULLPTR;

  /* Delete all the entries in RpSet Tree */
  while ((pRpSetNode = (pimsmRpSetNode_t *)radixGetNextEntry
          (&pimsmCb->pimsmRpInfo->pimsmRpSetTree,
           L7_NULLPTR)) != L7_NULLPTR)
  {
    SLLPurge(L7_FLEX_PIMSM_MAP_COMPONENT_ID, &(pRpSetNode->pimsmRpList));
    radixDeleteEntry(&pimsmCb->pimsmRpInfo->pimsmRpSetTree,(void *)pRpSetNode);
  }

  /* Delete all the entries in RpGrp List */
  pRpGrpNode = (pimsmRpGrpNode_t *)SLLFirstGet
                                   (&(pimsmCb->pimsmRpInfo->pimsmRpGrpList));
  while (pRpGrpNode != L7_NULLPTR)
  {
    pAddrListNode = (pimsmAddrList_t *)SLLFirstGet(&(pRpGrpNode->pimsmGrpList));
    while(pAddrListNode != L7_NULLPTR)
    {
      pimsmUtilAppTimerCancel (pimsmCb, &(pAddrListNode->pimsmRpGrpExpiryTimer));
      handleListNodeDelete(pimsmCb->handleList,
                           &pAddrListNode->pimsmRpGrpExpireTimerHandle);
      pAddrListNodeDelete = pAddrListNode;
      pAddrListNode = (pimsmAddrList_t*)SLLNextGet(&(pRpGrpNode->pimsmGrpList),
                                        (L7_sll_member_t*)pAddrListNode);
      SLLNodeDelete(&(pRpGrpNode->pimsmGrpList),
                    (L7_sll_member_t *) pAddrListNodeDelete);
    }

    pRpGrpNodeDelete = pRpGrpNode;
    pRpGrpNode = (pimsmRpGrpNode_t *)SLLNextGet(&(pimsmCb->pimsmRpInfo->pimsmRpGrpList),
                                        (L7_sll_member_t*)pRpGrpNode);

    SLLDelete(&(pimsmCb->pimsmRpInfo->pimsmRpGrpList),
              (L7_sll_member_t *)pRpGrpNodeDelete);
  }

  /* Delete all the Candidate Rp Info */
  pimsmRpCandidateInfoPurge(pimsmCb);
}
