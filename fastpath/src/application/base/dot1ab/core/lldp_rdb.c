/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename lldp_rdb.c
*
* @purpose To Handle the remote database for lldp
*
* @component
*
* @comments
*
* @create 10/22/2007
*
* @author PKB
* @end
*
**********************************************************************/

#include "lldp.h"
#include "lldp_util.h"
#include "lldp_debug.h"
#include "avl_api.h"
#include "l7sll_api.h"
#include "buff_api.h"

/* Global parameters */
static avlTree_t                lldpRemDbTree;
static avlTreeTables_t          *lldpRemDbTreeHeap=L7_NULLPTR;
static lldpRemDataEntry_t       *lldpRemDbDataHeap=L7_NULLPTR;

static L7_sll_t                 lldpRemDbIndexList;
static L7_uint32                lldpRemDBIndexBufferPoolId=0;
static L7_uint32                lldpRemIndex   = 0;

/* extern declarations */
extern L7_uint32            lldpMgmtAddrPoolId;
extern L7_uint32            lldpUnknownTLVPoolId;
extern L7_uint32            lldpOrgDefInfoPoolId;


/* structure for the index */
typedef struct lldpRemDBIndexNode_s
{
  struct L7_sll_member_s *next;
  L7_uint32    timestamp;
  L7_uint32    intIntfNum;
  L7_uint32    remIndex;
  lldpRemDataEntry_t  *remDataNode;
}lldpRemDBIndexNode_t;

/*********************************************************************
*
* @purpose  To compare the two keys given based on the inputs.
*
* @param    data1  @b{(input)}The pointer to the first key.
* @param    data2  @b{(input)}The pointer to the second key.
* @param    keyLen @b{(input)}The length of the key to be compared.
*
* @returns  Less than 0, if node1 < node 2.
* @returns  Zero, if node1 == node2
* @returns  More than 0, if node1 > node2.
*
* @notes    Key Length is ignored for this comparison.
*
* @end
*
*********************************************************************/
static
L7_int32 lldpRemDbIndexListCompare(void *data1,
                         void *data2,
                         L7_uint32 keyLen)
{
  lldpRemDBIndexNode_t *pEntry1, *pEntry2;

  if(data1 == L7_NULLPTR || data2 == L7_NULLPTR)
    return 1;

  pEntry1 = (lldpRemDBIndexNode_t *)data1;
  pEntry2 = (lldpRemDBIndexNode_t *)data2;

  if(pEntry1->timestamp == pEntry2->timestamp)
  {
    if(pEntry1->intIntfNum == pEntry2->intIntfNum)
    {
      if(pEntry1->remIndex == pEntry2->remIndex)
      {
        return 0;
      }
      else
      {
        return (pEntry1->remIndex - pEntry2->remIndex);
      }
    }
    else
    {
      return (pEntry1->intIntfNum - pEntry2->intIntfNum);
    }
  }
  else
  {
    return (pEntry1->timestamp - pEntry2->timestamp);
  }
}

/*********************************************************************
* @purpose  Compare keys for the AVL tree. Key consists of
*           struct lldpRemDataKey_s
*
* @param    a     Pointer to first key
* @param    b     Pointer to second key
* @param    len   Length of the key
*
* @returns  0     if keys are equal
* @returns  1     if a > b
* @returns  -1    if a < b
*
* @notes    none
*
* @end
*********************************************************************/
int lldpRemDbEntryCompare(const void *a, const void *b, size_t len)
{
  lldpRemDataKey_t  key1, key2;
  L7_uint32         i;
  L7_BOOL           cid1IsZero = L7_TRUE, cid2IsZero = L7_TRUE;

  if (len != sizeof(lldpRemDataKey_t))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "Invalid key length in lldpRemDbEntryCompare\n");
    L7_assert(1);                     /* need to know about this */
    return -1;
  }

  key1 = *((lldpRemDataKey_t *) a);
  key2 = *((lldpRemDataKey_t *) b);

  if (key1.chassisIdLength != key2.chassisIdLength)
  {
      return memcmp(a, b, len);
  }

  if (memcmp(&key1.chassisId[0], &key2.chassisId[0], LLDP_TLV_CHASSIS_ID_NTWK_ADDR_FAM_SIZE) != 0)
  {
      return memcmp(a, b, len);
  }

  for (i = LLDP_TLV_CHASSIS_ID_NTWK_ADDR_FAM_SIZE; i < key1.chassisIdLength; i++)
  {
    if (key1.chassisId[i] != 0)
    {
      cid1IsZero = L7_FALSE;
    }

    if (key2.chassisId[i] != 0)
    {
      cid2IsZero = L7_FALSE;
    }
  }

  if (cid1IsZero == cid2IsZero)
  {
      return memcmp(a, b, len);
  }

  if (cid1IsZero == L7_TRUE)
  {
      memcpy(&key1.chassisId[0], &key2.chassisId[0], key1.chassisIdLength);
  }
  else
  {
      memcpy(&key2.chassisId[0], &key1.chassisId[0], key2.chassisIdLength);
  }

  return memcmp(&key1, &key2, len);
}

/*********************************************************************
* @purpose  Initialize Remote entity Database
*
* @param    nodeCount    @b{(input)} The number of nodes to be created.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDBInit(L7_uint32 nodeCount)
{
  /* Allocate the Heap structures */
  lldpRemDbTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_LLDP_COMPONENT_ID,
                                                     nodeCount * sizeof(avlTreeTables_t));

  lldpRemDbDataHeap  = (lldpRemDataEntry_t *)osapiMalloc(L7_LLDP_COMPONENT_ID,
                                                         nodeCount * sizeof(lldpRemDataEntry_t));

  /* validate the pointers*/
  if ((lldpRemDbTreeHeap == L7_NULLPTR) ||(lldpRemDbDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s: Error allocating data for lldp remote DB Tree \n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* AVL Tree creations - dot1xLogicalPortTreeDb*/
  avlCreateAvlTree(&(lldpRemDbTree),  lldpRemDbTreeHeap,
                   lldpRemDbDataHeap, nodeCount,
                   sizeof(lldpRemDataEntry_t), 0x10,
                   sizeof(lldpRemDataKey_t));

  (void)avlSetAvlTreeComparator(&lldpRemDbTree, lldpRemDbEntryCompare);

   /* Buffer pool for the index data */
  if (bufferPoolInit(nodeCount,sizeof(lldpRemDBIndexNode_t) , "lldp remote index Bufs",
                     &(lldpRemDBIndexBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s: Error allocating buffers\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Create linked list for group membership timers for this snoop entry */
  if (SLLCreate(L7_LLDP_COMPONENT_ID, L7_SLL_ASCEND_ORDER,
               (3*sizeof(L7_uint32)), lldpRemDbIndexListCompare, L7_NULL,
               &(lldpRemDbIndexList)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s: Failed to lldp remote index linked list \n",__FUNCTION__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  DeInitialize Remote entity Database
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDBDeInit(void)
{
  /* Destroy the AVL Tree */
  if(lldpRemDbTree.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&lldpRemDbTree);
  }

  /* Give up the memory */
  if (lldpRemDbTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpRemDbTreeHeap);
    lldpRemDbTreeHeap = L7_NULLPTR;
  }

  if (lldpRemDbDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_LLDP_COMPONENT_ID, lldpRemDbDataHeap);
    lldpRemDbDataHeap = L7_NULLPTR;
  }

  if(lldpRemDBIndexBufferPoolId != 0)
  {
    bufferPoolDelete(lldpRemDBIndexBufferPoolId);
    lldpRemDBIndexBufferPoolId = 0;
  }

  SLLDestroy(L7_LLDP_COMPONENT_ID, &lldpRemDbIndexList);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear Remote entity Database
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDBClear(void)
{
  lldpRemDataKey_t key;
  lldpRemDataEntry_t *remEntry;

  memset(&key,0,sizeof(lldpRemDataKey_t));
  while((remEntry=lldpRemDataEntryGetNext(&key))!=L7_NULLPTR)
  {
    lldpRemEntryDelete(remEntry);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To Get a llpd remote Info Node
*
* @param    key  @b{(input)} The key to the remote Info Node
*
* @returns  remote Info Node
*
* @comments none
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpRemDataEntryGet(lldpRemDataKey_t *key)
{
  lldpRemDataEntry_t  *entry = L7_NULLPTR;
  entry = (lldpRemDataEntry_t *)avlSearchLVL7(&lldpRemDbTree,key,AVL_EXACT);
  return entry;
}

/*********************************************************************
* @purpose  To Get Next llpd remote Info Node
*
* @param    key  @b{(input)} The key to the remote Info Node
*
* @returns  remote Info Node
*
* @comments none
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpRemDataEntryGetNext(lldpRemDataKey_t *key)
{
  lldpRemDataEntry_t  *entry = L7_NULLPTR;
  entry = (lldpRemDataEntry_t *)avlSearchLVL7(&lldpRemDbTree,key,AVL_NEXT);
  return entry;
}

/*********************************************************************
* @purpose  To Get the next neighbor from the remote database
*
* @param    intIfNum  @b{(input)} The internal interface
* @param    prevEntry @b{(input)} The previous remote Info Node
*
* @returns  remote Info Node
*
* @comments none
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpRemDataNeighborGetNext(L7_uint32 intIfNum, lldpRemDataEntry_t *prevEntry)
{
  lldpRemDataKey_t    key;
  lldpRemDataEntry_t  *entry = L7_NULLPTR;

  if(prevEntry == L7_NULLPTR)
  {
    memset(&key,0,sizeof(lldpRemDataKey_t));
    key.intIntfNum = intIfNum;
  }
  else
  {
    memcpy(&key,prevEntry,sizeof(lldpRemDataKey_t));
  }
  entry = (lldpRemDataEntry_t *)avlSearchLVL7(&lldpRemDbTree,&key,AVL_NEXT);
  if(entry != L7_NULLPTR && entry->intIntfNum == intIfNum)
  {
    return entry;
  }
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  To Get the next neighbor from the remote database
*
* @param    intIfNum  @b{(input)} The internal interface
*
* @returns  L7_TRUE/L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL lldpRemDataNeighborIsLast(L7_uint32 intIfNum)
{
  if(lldpRemDataNeighborGetNext(intIfNum,L7_NULLPTR)!=L7_NULLPTR)
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Process a interface down event to update the remote db
*
* @param    intIfNum  @b{(input)} The internal interface
* @param    flag      @b{(input)} To do remote Device remove check
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  lldpRemDataInterfaceDelete(L7_uint32 intIfNum,L7_BOOL flag)
{
  lldpRemDataEntry_t *currEntry;
  L7_RC_t   rc = L7_FAILURE;
  while((currEntry=lldpRemDataNeighborGetNext(intIfNum,L7_NULLPTR))!=L7_NULLPTR)
  {
    if(flag == L7_TRUE)
    {
      LLDP_TRACE(LLDP_DEBUG_MED, "intf %d down, check for device removal", intIfNum);
      lldpMedRemDeviceRemoveCheck(currEntry);
    }
    lldpRemEntryDelete(currEntry);
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  Clear a remote data entry and return any allocated buffers
*
* @param    lldpRemDataEntry_t *entry  @b((input)) remote data entry
*
* @returns  void
*
* @notes    This does not clear the timer
*
* @end
*********************************************************************/
void lldpRemEntryDelete(lldpRemDataEntry_t *entry)
{
  L7_uchar8 *p = L7_NULLPTR;
  L7_uchar8 *next = L7_NULLPTR;
  lldpRemDBIndexNode_t  *indexNode;

  if(entry == L7_NULLPTR)
  {
    return;
  }

  /* return any allocated buffers to buffer pools */
  p = (L7_uchar8 *)entry->mgmtAddrList;
  while (p != L7_NULLPTR)
  {
    next = (L7_uchar8 *)((lldpMgmtAddrEntry_t *)p)->next;
    bufferPoolFree(lldpMgmtAddrPoolId, p);
    p = next;
  }

  p = (L7_uchar8 *)entry->orgDefInfoList;
  while (p != L7_NULLPTR)
  {
    next = (L7_uchar8 *)((lldpOrgDefInfoEntry_t *)p)->next;
    bufferPoolFree(lldpOrgDefInfoPoolId, p);
    p = next;
  }

  p = (L7_uchar8 *)entry->unknownTLVList;
  while (p != L7_NULLPTR)
  {
    next = (L7_uchar8 *)((lldpUnknownTLVEntry_t *)p)->next;
    bufferPoolFree(lldpUnknownTLVPoolId, p);
    p = next;
  }

  if(entry->rxTTL != 0)
  {
   /* lldpTimerDelete();*/
  }

  /* clear the index  */
  indexNode = entry->remDBIndex;
  if(indexNode != L7_NULLPTR)
  {
    /* remove it from the list */
    SLLRemove(&lldpRemDbIndexList,(L7_sll_member_t*)indexNode);
    /* free the buffer */
    bufferPoolFree(lldpRemDBIndexBufferPoolId,(L7_uchar8*)indexNode);
    /* free the avl tree node */
    avlDeleteEntry(&lldpRemDbTree,entry);
  }
  return;
}

/*********************************************************************
* @purpose  Add a remote data entry and return the new entry
*
* @param    lldpRemDataEntry_t *entry  @b((input)) remote data entry
*
* @returns  remote data entry
*
* @notes
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpRemEntryAdd(lldpRemDataEntry_t *entry)
{
  lldpRemDataEntry_t *currEntry;
  lldpRemDBIndexNode_t  *indexNode;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(entry->intIntfNum, L7_SYSNAME, ifName);

  currEntry = avlInsertEntry(&lldpRemDbTree,entry);
  if (currEntry == entry)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s:%d Error Adding the node to the Remote Tree for intf %s \n",
            __FUNCTION__,__FILE__,ifName);
    return L7_NULLPTR;
  }
  currEntry = lldpRemDataEntryGet((lldpRemDataKey_t*)entry);
  if (currEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s:%d Error Retrieving the node from the Remote Tree for intf %s \n",
            __FUNCTION__,__FILE__,ifName);
    return L7_NULLPTR;
  }
  /* allocate the index list */
  if (bufferPoolAllocate(lldpRemDBIndexBufferPoolId,
                         (L7_uchar8 **)&indexNode) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s:%d Error allocating the node to the remote index for intf %s \n",
            __FUNCTION__,__FILE__,ifName);
    lldpRemEntryDelete(currEntry);
    avlDeleteEntry(&lldpRemDbTree,currEntry);
    return L7_NULLPTR;
  }

  indexNode->timestamp = currEntry->timestamp = osapiUpTimeRaw();
  indexNode->intIntfNum = currEntry->intIntfNum;
  if(lldpRemIndex == 0xFFFFFFFF)
  {
    lldpRemIndex = 1;
  }
  else
  {
      lldpRemIndex++;
  }
  currEntry->remIndex = indexNode->remIndex = lldpRemIndex;

  if (SLLAdd(&lldpRemDbIndexList, (L7_sll_member_t *)indexNode)!= L7_SUCCESS)
  {
    /* Free the previously allocated bufferpool */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "%s: Failed to add the node to the remote node list\n",__FUNCTION__);
    bufferPoolFree(lldpRemDBIndexBufferPoolId,(L7_uchar8*)indexNode);
    lldpRemEntryDelete(currEntry);
    if(lldpRemIndex > 0)
    {
      lldpRemIndex--;
    }
    avlDeleteEntry(&lldpRemDbTree,currEntry);
    return L7_NULLPTR;
  }

  currEntry->remDBIndex = indexNode;
  indexNode->remDataNode = currEntry;
  return currEntry;
}

/*********************************************************************
* @purpose  To Get a llpd remote Info Node via timeStamp Index
*
* @param    L7_uint32 timestamp @b((input))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex  @b((input))  remote data index
*
* @returns  remote Info Node
*
* @comments none
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpRemDataEntryTimestampIndexGet(L7_uint32 timestamp,
                                                      L7_uint32 intIfNum,
                                                      L7_uint32 remIndex)
{
  lldpRemDBIndexNode_t  searchNode,*pEntry;

  memset(&searchNode,0,sizeof(lldpRemDBIndexNode_t));
  searchNode.timestamp = timestamp;
  searchNode.intIntfNum = intIfNum;
  searchNode.remIndex = remIndex;

  if ((pEntry = (lldpRemDBIndexNode_t *)SLLFind(&lldpRemDbIndexList,
                                         (void *)&searchNode))== L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  return pEntry->remDataNode;
}

/*********************************************************************
* @purpose  To Get Next llpd remote Info Node via timeStamp Index
*
* @param    L7_uint32 timestamp @b((output))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((output))   internal interface number
* @param    L7_uint32 remIndex  @b((output))  remote data index
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t lldpRemDataEntryTimestampIndexGetNext(L7_uint32 *timestamp,
                                              L7_uint32 *intIfNum,
                                              L7_uint32 *remIndex)
{
  lldpRemDBIndexNode_t  searchNode,*pEntry;

  memset(&searchNode,0,sizeof(lldpRemDBIndexNode_t));
  searchNode.timestamp = *timestamp;
  searchNode.intIntfNum = *intIfNum;
  searchNode.remIndex = *remIndex;

  if ((pEntry = (lldpRemDBIndexNode_t *)SLLFindNext(&lldpRemDbIndexList,
                                         (void *)&searchNode))== L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *timestamp = pEntry->timestamp;
  *intIfNum = pEntry->intIntfNum;
  *remIndex = pEntry->remIndex;
  return L7_SUCCESS;
}

/*************************************************************************************
* @purpose  To check if a Remote Device exists on this interface with the same SMAC
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uchar8 * srcMac   @b((intput))  source MAC
*
* @returns  lldpRemDataEntry_t If exists else L7_NULLPTR
*
* @comments none
*
* @end
*****************************************************************************************/
lldpRemDataEntry_t *lldpRemDataEntryMacCheck(L7_uint32 intIfNum, L7_uchar8 *srcMac)
{
  lldpRemDataEntry_t *remEntry,*prevEntry=L7_NULLPTR;

  /*Check the MAC of the device on this interface.
    If it exists, return the pointer to the entry, otherwise return NULL.
   */
  while((remEntry = lldpRemDataNeighborGetNext(intIfNum,prevEntry)) != L7_NULLPTR)
  {
    if(memcmp(remEntry->srcMac,srcMac,L7_MAC_ADDR_LEN) != 0)
    {
      prevEntry = remEntry;
    }
    else
    {
      break;
    }
  }
  return remEntry;
}

