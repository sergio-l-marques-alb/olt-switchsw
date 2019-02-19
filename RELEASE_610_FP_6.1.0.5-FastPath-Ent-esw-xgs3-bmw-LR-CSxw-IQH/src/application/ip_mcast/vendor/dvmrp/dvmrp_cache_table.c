/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvmrp_cache_table.c
*
* @purpose   This file contains operations on the cache table.
*
* @component DVMRP cache table maintained like an AVL Tree.
*
* @notes  Access to elements in the list is protected by a semaphore. 
*            Functions that return values from the list always return a copy
*            of the data in the list. 
*
* @create    10/02/2006
*
* @author    Prakash/Shashidhar
*
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_mfc.h"

/*****************************************************************
* @purpose  Compare IP Network Address (source and group) keys and
*           indicate relationship
*
* @param    a     -  @b{(input)}      Pointer to the first key
* @param    b     -  @b{(input)}      Pointer to the second key
* @param    len   -  @b{(input)}      Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   At this time, these are compared the same way they would
*          be if they were compared with memcmp() on a big-endian platform.
*          It might be more useful to compare them by some other criterion
*          (shorter masks first, for example).
*
* @end
*****************************************************************/
L7_int32 dvmrpAVLCacheCompare(const void *a, const void *b, L7_uint32 len)
{
  dvmrp_cache_entry_t *pKey1 = (dvmrp_cache_entry_t *) a;
  dvmrp_cache_entry_t *pKey2 = (dvmrp_cache_entry_t *) b;
  register L7_int32 retVal = 0;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE (&(pKey1->group),
                                      &(pKey2->group))) != 0)
  {
    return retVal;
  }

  retVal = L7_INET_ADDR_COMPARE (&(pKey1->source), &(pKey2->source));

  return retVal;
}

/*********************************************************************
* @purpose  Initializes the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableInit(dvmrp_t *dvmrpcb)
{
  L7_uint32 avlType = 0x10;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  dvmrpcb->dvmrpCacheTableTreeHeap = DVMRP_ALLOC (L7_AF_INET,
                         (DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES * sizeof (avlTreeTables_t)));
  if (dvmrpcb->dvmrpCacheTableTreeHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "Error: unable to allocate DVMRP Cache Table tree heap.\n");
    return L7_FAILURE;
  }

  dvmrpcb->dvmrpCacheDataHeap = DVMRP_ALLOC (L7_AF_INET,
                         (DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES * sizeof (dvmrp_cache_entry_t)));
  if (dvmrpcb->dvmrpCacheDataHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "Error: unable to allocate DVMRP Cache Table data heap.\n");
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpCacheTableTreeHeap);
    return L7_FAILURE;
  }

/* The destination networks are stored in an AVL tree.
*/
  avlCreateAvlTree(&dvmrpcb->dvmrpCacheTableTree, 
                   dvmrpcb->dvmrpCacheTableTreeHeap,
                   dvmrpcb->dvmrpCacheDataHeap, DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES,
                   (L7_uint32)(sizeof(dvmrp_cache_entry_t)), 
                   avlType, sizeof(L7_inet_addr_t) * 2);

  (void)avlSetAvlTreeComparator(&dvmrpcb->dvmrpCacheTableTree, 
                                dvmrpAVLCacheCompare);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-Initializes the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableDestroy(dvmrp_t *dvmrpcb)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

/* The destination networks are stored in an AVL tree.
*/
  if (L7_SUCCESS != avlDeleteAvlTree(&dvmrpcb->dvmrpCacheTableTree))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:(%d)Couldn't delete AVL TREE for CACHE\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (dvmrpcb->dvmrpCacheDataHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpCacheDataHeap);
    dvmrpcb->dvmrpCacheDataHeap = L7_NULLPTR;
  }
  if (dvmrpcb->dvmrpCacheTableTreeHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpCacheTableTreeHeap);
    dvmrpcb->dvmrpCacheTableTreeHeap = L7_NULLPTR;  
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a cache to the cache table
*
* @param    dvmrpcb    -  @b{(input)} Pointer to the DVMRP control block.
* @param    cacheData   -  @b{(input)} Pointer to cache information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheAdd(dvmrp_t *dvmrpcb, dvmrp_cache_entry_t *cacheData)
{
  dvmrp_cache_entry_t *pData = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (avlTreeCount(&dvmrpcb->dvmrpCacheTableTree) >= DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Number exceeded the AVL TREE limit\n");
  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  pData = avlInsertEntry(&dvmrpcb->dvmrpCacheTableTree, cacheData);
  if (pData != L7_NULLPTR)
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return L7_FAILURE;
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a cache from the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pSource   -  @b{(input)} pointer to Source of the multicast data
* @param    pGroup    -  @b{(input)} pointer to Multicast group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheDelete(dvmrp_t *dvmrpcb,L7_inet_addr_t *pSource, 
                                   L7_inet_addr_t *pGroup)
{
  dvmrp_cache_entry_t dummyCache,*entry=L7_NULLPTR;
  dvmrp_prune_t *prune,*pruneNext=L7_NULLPTR;
  L7_inet_addr_t srcAddr, grpAddr;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  inetCopy(&srcAddr, pSource);
  inetCopy(&grpAddr, pGroup);
  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  entry =dvmrp_cache_lookup(dvmrpcb,pSource,pGroup);
  if (entry == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Cache entry Not found \n", 
                __FUNCTION__,__LINE__);
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return L7_FAILURE;
  }


  if (entry->prune_retry_timer)
  {
    appTimerDelete(dvmrpcb->timerHandle, entry->prune_retry_timer);
    if (entry->pruneRetransmitimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &entry->pruneRetransmitimerHandle);
    }
    entry->prune_retry_timer=L7_NULLPTR;

  }

  if (entry->graft_sent)
  {
    appTimerDelete(dvmrpcb->timerHandle, entry->graft_timer);
    if (entry->graftTimeoutHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &entry->graftTimeoutHandle);
    }
    DVMRP_FREE (L7_AF_INET, (void*) entry->graft_sent);        
    entry->graft_sent=L7_NULLPTR;   
    entry->graft_timer=L7_NULLPTR;

  }

  if (entry->cacheRemove_timer != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, entry->cacheRemove_timer);
    if (entry->cacheRemoveHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &entry->cacheRemoveHandle);
    }
    entry->cacheRemove_timer=L7_NULLPTR;    
  }
  

  for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
      prune != L7_NULLPTR;
      prune = pruneNext)
  {
    pruneNext =(dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune);
    if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), 
                                (L7_sll_member_t *)prune))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Deletion failed from SLL:%d\n",
                  __FUNCTION__, __LINE__);
    }
  }

  memcpy(&dummyCache.source, pSource, sizeof(L7_inet_addr_t));
  memcpy(&dummyCache.group, pGroup, sizeof(L7_inet_addr_t));

  if (avlDeleteEntry(&dvmrpcb->dvmrpCacheTableTree, &dummyCache) == 
      L7_NULLPTR)
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return L7_FAILURE;
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);

  /* Delete entry from MFC after dvmrp lock release */
  if (L7_SUCCESS != dvmrpCacheMfcDelete(&srcAddr,&grpAddr))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:MFC Cache either not there or could not be deleted:%d\n",
                __FUNCTION__,__LINE__);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check whether the cache exists in the cache table
*
* @param    dvmrpcb    -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData   -  @b{(input)}   Route info to look for
* @param    routeData  -  @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheGet(dvmrp_t *dvmrpcb, dvmrp_cache_entry_t *srcRtData,
                                dvmrp_cache_entry_t **cacheData)
{
/* Searches for the network and gateway and returns pointer to the structure
  *  stored in the tree.
 */
  dvmrp_cache_entry_t  *pData = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (srcRtData == L7_NULLPTR)
  {
    *cacheData = L7_NULLPTR;
    return L7_FAILURE;
  }

  pData = avlSearchLVL7(&dvmrpcb->dvmrpCacheTableTree, srcRtData, AVL_EXACT);
  if (pData == L7_NULLPTR)
  {
    *cacheData = L7_NULLPTR;
    return L7_FAILURE;
  }

  if (cacheData != L7_NULLPTR)
  {
    *cacheData = pData;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next cache after the specified cache in the cache table
*
* @param    dvmrpcb    -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData   -  @b{(input)} Pointer to the Cache info to look for
* @param    cacheData  -  @b{(output)} Pointer to the next Cache info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheNextGet(dvmrp_t *dvmrpcb, dvmrp_cache_entry_t *srcRtData,
                                    dvmrp_cache_entry_t **cacheData)
{
  dvmrp_cache_entry_t  *pData = L7_NULLPTR;

  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__); */

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }


  if (srcRtData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pData = avlSearchLVL7(&dvmrpcb->dvmrpCacheTableTree, srcRtData, AVL_NEXT);

  if (pData == L7_NULLPTR)
  {
    *cacheData = L7_NULLPTR;
    return L7_FAILURE;
  }

  if (cacheData != L7_NULLPTR)
  {
    *cacheData = pData;
  }

  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__); */
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  This function is used to flush the dvmrp cache table.
 * 
 * @param    dvmrpcb -@b{(input)}  Pointer to the DVMRP control block.
 * 
 * @returns  None
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
void dvmrp_cache_table_flush(dvmrp_t *dvmrpcb)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR,*entryNext = L7_NULLPTR;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb, &tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d\n", __FUNCTION__, __LINE__);
  }

  while (L7_NULLPTR != entry)
  {
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb, entry, &entryNext))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d\n",
                  __FUNCTION__,__LINE__);
    }

    if (L7_SUCCESS != dvmrpCacheTableCacheDelete(dvmrpcb,&entry->source, &entry->group))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Unable to delete cache entry from cache:%d\n",
                  __FUNCTION__,__LINE__);
    }

    entry =entryNext;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

