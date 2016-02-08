/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvmrp_igmp_group_table.c
*
* @purpose   This file contains operations on the igmp group table.
*
* @component DVMRP igmp group table maintained like an AVL Tree.
*
* @notes  
*            Functions that return values from the list always return a copy
*            of the data in the list. 
*
* @create    30/05/2006
*
* @author    Prakash/Shashidhar
*
* @end
*
**********************************************************************/
#include "dvmrp_igmp_group_table.h"

/*****************************************************************
* @purpose  Compare IP Network Address (source and group) keys and
*           indicate relationship
*
* @param    a    -  @b{(input)}       Pointer to the first key
* @param    b    -  @b{(input)}       Pointer to the second key
* @param    len  -  @b{(input)}       Length of the key (unused)
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
L7_int32 dvmrpAVLCompareGroup(const void *a, const void *b, L7_uint32 len)
{
  L7_inet_addr_t source_a, source_b, group_a, group_b;
  L7_int32 ret = L7_NULL;

  inetCopy(&source_a, &(((dvmrp_Group_entry_t *)a)->source));
  inetCopy(&source_b, &(((dvmrp_Group_entry_t *)b)->source));
  inetCopy(&group_a, &(((dvmrp_Group_entry_t *)a)->group));
  inetCopy(&group_b, &(((dvmrp_Group_entry_t *)b)->group));

  ret = L7_INET_ADDR_COMPARE(&group_a, &group_b);
  if (ret == L7_NULL)
  {
    ret = L7_INET_ADDR_COMPARE(&source_a, &source_b);
  }

  return ret;
}

/*********************************************************************
* @purpose  Initializes the igmp group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableInit(dvmrp_t *dvmrpcb)
{
  L7_uint32 avlType = 0x10;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  dvmrpcb->dvmrpGroupTableTreeHeap = DVMRP_ALLOC (L7_AF_INET,
                     (DVMRP_MAX_LOCAL_MEMB_ENTRIES * sizeof (avlTreeTables_t)));
  if (dvmrpcb->dvmrpGroupTableTreeHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "Error: unable to allocate DVMRP Cache Table tree heap.\n");
    return L7_FAILURE;
  }

  dvmrpcb->dvmrpGroupDataHeap = DVMRP_ALLOC (L7_AF_INET,
                 (DVMRP_MAX_LOCAL_MEMB_ENTRIES * sizeof (dvmrp_Group_entry_t)));
  if (dvmrpcb->dvmrpGroupDataHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "Error: unable to allocate DVMRP Group Table data heap.\n");
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpGroupTableTreeHeap);
    return L7_FAILURE;
  }

  /* The destination networks are stored in an AVL tree.
  */
  avlCreateAvlTree(&dvmrpcb->dvmrpGroupTableTree, 
                   dvmrpcb->dvmrpGroupTableTreeHeap,
                   dvmrpcb->dvmrpGroupDataHeap, DVMRP_MAX_LOCAL_MEMB_ENTRIES,
                   (L7_uint32)(sizeof(dvmrp_Group_entry_t)), avlType, 
                   sizeof(L7_inet_addr_t) * 2);

  (void)avlSetAvlTreeComparator(&dvmrpcb->dvmrpGroupTableTree, 
                                dvmrpAVLCompareGroup);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-Initializes the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableDestroy(dvmrp_t *dvmrpcb)
{

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  /* The destination networks are stored in an AVL tree.
  */
  if (L7_SUCCESS != avlDeleteAvlTree(&dvmrpcb->dvmrpGroupTableTree))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:(%d)Couldn't delete AVL TREE for Group\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;  
  }

  if (dvmrpcb->dvmrpGroupDataHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpGroupDataHeap);
    dvmrpcb->dvmrpGroupDataHeap = L7_NULLPTR;
  }
  if (dvmrpcb->dvmrpGroupTableTreeHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpGroupTableTreeHeap);
    dvmrpcb->dvmrpGroupTableTreeHeap = L7_NULLPTR;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Group to the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    cacheData -   @b{(input)} Pointer to the Route information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupAdd(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *cacheData)
{
  dvmrp_Group_entry_t *pData = L7_NULLPTR;
  L7_char8 src[IPV6_DISP_ADDR_LEN],grp[IPV6_DISP_ADDR_LEN];

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (avlTreeCount(&dvmrpcb->dvmrpGroupTableTree) >= DVMRP_MAX_LOCAL_MEMB_ENTRIES)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Number exceeded the AVL TREE limit\n");
  }

  pData = avlInsertEntry(&dvmrpcb->dvmrpGroupTableTree, cacheData);
  if (pData != L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Entry Added.\n", __FUNCTION__, __LINE__,
          inetAddrPrint(&cacheData->source,src), inetAddrPrint(&cacheData->group,grp));
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a Group from the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pSource    -  @b{(input)} Pointer to the Source of the multicast data
* @param    pGroup     -  @b{(input)} Pointer to the Multicast group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupDelete(dvmrp_t *dvmrpcb,L7_inet_addr_t *pSource, 
                                   L7_inet_addr_t *pGroup)
{
  dvmrp_Group_entry_t dummyCache;
  L7_char8 src[IPV6_DISP_ADDR_LEN],grp[IPV6_DISP_ADDR_LEN];

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  memcpy(&dummyCache.source, pSource, sizeof(L7_inet_addr_t));
  memcpy(&dummyCache.group, pGroup, sizeof(L7_inet_addr_t));

  if (avlDeleteEntry(&dvmrpcb->dvmrpGroupTableTree, &dummyCache) == 
      L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Failed deleting Group entry \n",
                __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Entry Deleted.\n", __FUNCTION__, __LINE__,
          inetAddrPrint(pSource,src), inetAddrPrint(pGroup,grp));
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check whether the cache exists in the Group table
*
* 
* @param    dvmrpcb       -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData     -    @b{(input)} Pointer to the   Route info to look for
* @param    routeData     -     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupGet(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *srcRtData,
                                dvmrp_Group_entry_t **cacheData)
{
  /* Searches for the network and gateway and returns pointer to the structure
   *  stored in the tree.
   */
  dvmrp_Group_entry_t  *pData = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (srcRtData == L7_NULLPTR)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nINVALID :Input parameter pointer value is NULL!\n\n");
    return L7_FAILURE;
  }

  pData = avlSearchLVL7(&dvmrpcb->dvmrpGroupTableTree, srcRtData, AVL_EXACT);
  if (pData == L7_NULLPTR)
  {
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
* 
* @param    dvmrpcb       -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData    -     @b{(input)} Pointer to the Cache info to look for
* @param    cacheData  -     @b{(output)} Pointer to the next Cache info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupNextGet(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *srcRtData,
                                    dvmrp_Group_entry_t **cacheData)
{
  dvmrp_Group_entry_t  *pData = L7_NULLPTR;


  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (srcRtData == L7_NULLPTR)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nINVALID :Input parameter pointer value is NULL!\n\n");
    return L7_FAILURE;
  }

  if ((inetIsAddressZero(&srcRtData->source) == L7_TRUE)&&(inetIsAddressZero(&srcRtData->group) == L7_TRUE))
  {
    /* They are asking for the first route. */
    pData = avlSearchLVL7(&dvmrpcb->dvmrpGroupTableTree, srcRtData, AVL_EXACT);
  }

  if (pData == L7_NULLPTR)
  {
    pData = avlSearchLVL7(&dvmrpcb->dvmrpGroupTableTree, srcRtData, AVL_NEXT);
  }

  if (pData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (cacheData != L7_NULLPTR)
  {
    *cacheData = pData;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the Group table
*
* 
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pTmpNode - @b{(input)}  Pointer to the group entry node which just holds the key values       
* @param    cacheData  -     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupSearch(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *pTmpNode,
                                   dvmrp_Group_entry_t **cacheData)
{
  /* Searches for the network and gateway and returns pointer to the structure
     stored in the tree.
  */
  dvmrp_Group_entry_t *pData = L7_NULLPTR;

  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__); */

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  /* Find the first route.*/
  if (avlTreeCount(&dvmrpcb->dvmrpGroupTableTree) <= L7_NULL)
  {
    return L7_FAILURE;
  }

  pData = avlSearchLVL7(&dvmrpcb->dvmrpGroupTableTree, pTmpNode, AVL_EXACT);
  if (pData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *cacheData = pData;
  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__); */
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Updates the Group Membership Table
 *
 * @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block. 
 * @param    grp_info   -  @b{(input)}   Pointer to the group information.
 * @param    event      -  @b{(input)}   Event.
 *
 * @returns  None
 *
 * @notes None
 *
 * @end
 *********************************************************************/
void dvmrpGroupSourcesUpdate(dvmrp_t *dvmrpcb, mgmdMrpEventInfo_t *grp_info, L7_uint32 event)
{
  dvmrp_Group_entry_t tmpEntry,*entry = L7_NULLPTR,cacheentry;
  L7_char8 src[IPV6_DISP_ADDR_LEN],grp[IPV6_DISP_ADDR_LEN];
  L7_uint32 srcIndex = 0;
  mrp_source_record_t *src_add = L7_NULLPTR;

  DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Entry.\n", __FUNCTION__, __LINE__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  if (inetAddrHtop(&grp_info->group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return;   
  }
  if (grp_info->numSrcs == 0) /* IGMPv1/v2 Message */
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GROUP, "[%s-%d]: IGMPv2 Message\n", __FUNCTION__, __LINE__);
    memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
    inetAddressZeroSet (L7_AF_INET, &cacheentry.group);
    inetAddressZeroSet (L7_AF_INET, &cacheentry.source);
    cacheentry.group  = grp_info->group;
    if (inetAddrHtop(&cacheentry.source, src) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
      return;   
  
    }
    if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry,&entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Entry not found for src=%s, grp=%s \n",
                  __FUNCTION__,__LINE__,src,grp);
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d Entry not found for src=%s, grp=%s \n",
                  __FUNCTION__,__LINE__,src,grp);
    }
    if (entry == L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d Creating the entry for src=%s, grp=%s \n",
                  __FUNCTION__,__LINE__,src,grp);
      memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
      inetCopy(&cacheentry.group, &grp_info->group);
      inetAddressZeroSet(L7_AF_INET, &cacheentry.source);    
      if (L7_SUCCESS != dvmrpGroupTableGroupAdd(dvmrpcb,&cacheentry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Failed adding entry for src=%s, grp=%s \n",
                    __FUNCTION__,__LINE__,src,grp);
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d Failed adding entry for src=%s, grp=%s \n",
                    __FUNCTION__,__LINE__,src,grp);
        return;
      }
      memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
      inetCopy(&cacheentry.group, &grp_info->group);
      inetAddressZeroSet(L7_AF_INET, &cacheentry.source);        
      if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Entry not found for src=%s, grp=%s \n",
                    __FUNCTION__,__LINE__,src,grp);
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d Entry not found for src=%s, grp=%s \n",
                    __FUNCTION__,__LINE__,src,grp);
        return;
      }
      entry->numSrcs = grp_info->numSrcs;   /* IGMP V2 message (no source list)*/
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d Setting the GrpIntf maskbit for src=%s,grp=%s\n",
                  __FUNCTION__,__LINE__,src,grp);
  
    }
  
  
    if (event == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      if (grp_info->mode == MGMD_FILTER_MODE_INCLUDE)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                    "%s:%d INCLUDE mode event received for grp=%s, No_Srcs=%d",
                    __FUNCTION__,__LINE__, grp, grp_info->numSrcs);
        memset(&tmpEntry, 0, sizeof(dvmrp_Group_entry_t));
        inetAddressZeroSet(L7_AF_INET, &tmpEntry.source);
        inetAddressZeroSet(L7_AF_INET, &tmpEntry.group);
        if (L7_SUCCESS != dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, 
                                                      &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:No Group Entry found in the table:%d", __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                      "%s:No Group Entry found in the table:%d", __FUNCTION__,__LINE__);
          return;
        }
  
        while (L7_NULLPTR != entry)
        {
          if (inetAddrHtop(&entry->source, src) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
            return;   
  
          }
  
          if (L7_INET_ADDR_COMPARE(&entry->group,&grp_info->group) ==0)
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                        "%s:%dClear the src mask bit for src=%s,grp=%s on rtrIfnum=%d",
                        __FUNCTION__,__LINE__,src,grp,grp_info->rtrIfNum);
  
            MCAST_BITX_RESET(entry->grpIntfBitMask, grp_info->rtrIfNum);
            MCAST_BITX_SET(entry->modeIntfBitMask, grp_info->rtrIfNum);
            MCAST_BITX_RESET(entry->srcIntfBitMask, grp_info->rtrIfNum);
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_Group_entry_t));
          entry = L7_NULLPTR;
          if (L7_SUCCESS != dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:No more Group Entries found in the table:%d",
                        __FUNCTION__,__LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                        "%s:No more Group Entries found in the table:%d",
                        __FUNCTION__,__LINE__);
            break;
          }
        }
  
      }
      else /*grp_info->mode == MGMD_FILTER_MODE_EXCLUDE*/
      {
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                    "%s:%d EXCLUDE mode event received for grp=%s, No_Srcs=%d",
                    __FUNCTION__,__LINE__, grp, grp_info->numSrcs);
        memset(&tmpEntry, 0, sizeof(dvmrp_Group_entry_t));
        inetAddressZeroSet(L7_AF_INET, &tmpEntry.source);
        inetAddressZeroSet(L7_AF_INET, &tmpEntry.group);      
        if (L7_SUCCESS != dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:No Group Entry found in the table:%d", __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                      "%s:No Group Entry found in the table:%d", __FUNCTION__, __LINE__);
          return;
        }
  
        while (L7_NULLPTR != entry)
        {
          if (inetAddrHtop(&entry->source, src) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
            return;   
  
          }
          if (L7_INET_ADDR_COMPARE(&entry->group,&grp_info->group) ==0)
          {
            MCAST_BITX_SET(entry->grpIntfBitMask, grp_info->rtrIfNum);
            MCAST_BITX_RESET(entry->modeIntfBitMask, grp_info->rtrIfNum);
            MCAST_BITX_RESET(entry->srcIntfBitMask, grp_info->rtrIfNum);
            break;
  
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_Group_entry_t));
          entry = L7_NULLPTR;
          if (L7_SUCCESS != dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:No more Group Entry found in the table:%d",
                        __FUNCTION__,__LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                        "%s:No more Group Entry found in the table:%d",
                        __FUNCTION__,__LINE__);
            break;
          }
        }/*End of while*/
      }/*else grp_info->mode == MGMD_FILTER_MODE_EXCLUDE*/
    }
  }
  else /* IGMPv3 Message */
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GROUP, "[%s-%d]: IGMPv3 Message\n", __FUNCTION__, __LINE__);

    for (srcIndex = 0; srcIndex < grp_info->numSrcs; srcIndex++)
    {
      if ((src_add = grp_info->sourceList[srcIndex]) == L7_NULLPTR)
      {
        continue;
      }
      memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
      inetCopy(&cacheentry.source, &src_add->sourceAddr);
      inetCopy(&cacheentry.group, &grp_info->group);

      DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Processing Starts...\n", __FUNCTION__, __LINE__,
              inetAddrPrint(&cacheentry.source,src), inetAddrPrint(&cacheentry.group,grp));
      entry = L7_NULLPTR;
      if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry,&entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Entry not found.\n", __FUNCTION__,__LINE__);
      }
      if (entry == L7_NULLPTR)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Entry Not Found, Trying to Add.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&cacheentry.source,src), inetAddrPrint(&cacheentry.group,grp));
        if (L7_SUCCESS != dvmrpGroupTableGroupAdd(dvmrpcb,&cacheentry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Failed adding entry for src=%s, grp=%s \n",
                      __FUNCTION__,__LINE__,src,grp);
          return;
        }
        DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Entry Added.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&cacheentry.source,src), inetAddrPrint(&cacheentry.group,grp));

        if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Entry not found.\n", __FUNCTION__,__LINE__);
          continue;
        }
      }

      entry->numSrcs = grp_info->numSrcs;
      MCAST_BITX_SET(entry->grpIntfBitMask, grp_info->rtrIfNum);
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"[%s:%d] grpIntfBitMask is SET for src=%s,grp=%s\n",
                  __FUNCTION__,__LINE__,src,grp);
      DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: GrpMode-%d, SrcFilterMode-%d, SrcAction-%d.\n", __FUNCTION__, __LINE__,
              grp_info->mode, src_add->filterMode, src_add->action);

      switch (grp_info->mode)
      {
        case MGMD_FILTER_MODE_INCLUDE:

          BITX_RESET(&entry->includeStarG, grp_info->rtrIfNum);

          if (src_add->action == MGMD_SOURCE_ADD)
          {
            /* (S,G) Include Add */
            BITX_SET(&entry->includeSG, grp_info->rtrIfNum);
            BITX_RESET(&entry->excludeSG, grp_info->rtrIfNum);
            DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Include SET, Exclude RESET.\n", __FUNCTION__, __LINE__);
          }
          else if (src_add->action == MGMD_SOURCE_DELETE)
          {
            /* (S,G) Include Delete */
            BITX_RESET(&entry->includeSG, grp_info->rtrIfNum);
            DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Include RESET.\n", __FUNCTION__, __LINE__);
            /* Check if both includeSG and excludeSG bitmaps are empty.
             * If so Delete the Entry.
             */
          }
          else /* Do Nothing */
          {
          }
          break;

        case MGMD_FILTER_MODE_EXCLUDE:

          BITX_SET(&entry->includeStarG, grp_info->rtrIfNum);

          switch (src_add->filterMode)
          {
            case MGMD_FILTER_MODE_INCLUDE:

              if (src_add->action == MGMD_SOURCE_ADD)
              {
                /* (S,G) Include Add */
                BITX_SET(&entry->includeSG, grp_info->rtrIfNum);
                BITX_RESET(&entry->excludeSG, grp_info->rtrIfNum);
                DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Include SET, Exclude RESET.\n", __FUNCTION__, __LINE__);
              }
              else if (src_add->action == MGMD_SOURCE_DELETE)
              {
                /* (S,G) Include Delete */
                BITX_RESET(&entry->includeSG, grp_info->rtrIfNum);
                DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Include RESET.\n", __FUNCTION__, __LINE__);
                /* Check if both includeSG and excludeSG bitmaps are empty.
                 * If so Delete the Entry.
                 */
              }
              else
              {
                /* Do Nothing */
              }
              break;

            case MGMD_FILTER_MODE_EXCLUDE:

              if (src_add->action == MGMD_SOURCE_ADD)
              {
                /* (S,G) Exclude Add */
                BITX_SET(&entry->excludeSG, grp_info->rtrIfNum);
                BITX_RESET(&entry->includeSG, grp_info->rtrIfNum);
                DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Exclude SET, Include RESET.\n", __FUNCTION__, __LINE__);
              }
              else if (src_add->action == MGMD_SOURCE_DELETE)
              {
                /* (S,G) Exclude Delete */
                BITX_RESET(&entry->excludeSG, grp_info->rtrIfNum);
                DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Exclude RESET.\n", __FUNCTION__, __LINE__);
                /* Check if both includeSG and excludeSG bitmaps are empty.
                 * If so Delete the Entry.
                 */
              }
              else /* Do Nothing */
              {
              }
              break;

            default:
              break; /* Do Nothing */
          } /* END - Switch - (src_add->filterMode) */
          break;

        default:
          break; /* Do Nothing */
      } /* END - Switch - (grp_info->mode) */

      DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: (%s,%s) Processing Ends...\n\n\n", __FUNCTION__, __LINE__,
              inetAddrPrint(&cacheentry.source,src), inetAddrPrint(&cacheentry.group,grp));
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:LEAVING \n\n",__FUNCTION__);
  return;
}

