/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_mfc.c
*
* @purpose Contains cache related definitions to support the DVMRP protocol
*
* @component
*
* @comments
*
* @create 03/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "dvmrp_mfc.h"
#include "dvmrp_util.h"
#include "dvmrp_routing.h"
#include "dvmrp_interface.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_protocol.h"
#include "dvmrp_igmp.h"

/*********************************************************************
* @purpose  This function is used to update the MFC about cache outgoing list
*
* @param     source  -  @b{(input)}Pointer to the source address of the cache
* @param     group   -  @b{(input)}Pointer to the group address of the cache
* @param     iif         -  @b{(input)}   Pointer to the incoming interface of the cache
* @param     oif        -  @b{(input)}  Pointer to the outgoing interface list
* 
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpCacheMfcUpdate (L7_inet_addr_t *source, 
                              L7_inet_addr_t *group,
                              dvmrp_interface_t *iif,L7_uchar8 *oif)
{
  mfcEntry_t   mfentry;
  L7_char8 src[DVMRP_STRING_SIZE],grp[DVMRP_STRING_SIZE];

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  /* if there are outgoing interfaces for this route, set outgoing intf bitmask bits */
  if (inetAddrHtop(source, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return L7_FAILURE;    
  }
  if (inetAddrHtop(group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return L7_FAILURE;    
  }
  inetCopy(&mfentry.source,source);
  inetCopy(&mfentry.group,group);
  mfentry.iif              = iif->index;
  memcpy(&(mfentry.oif),oif,sizeof(mfentry.oif));
  mfentry.mcastProtocol    = L7_MRP_DVMRP;
  mfentry.m                = L7_NULLPTR;
  if (mfcMessagePost(MFC_UPDATE_ENTRY, &mfentry, sizeof(mfentry)) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  This function is used to update the MFC that interface has 
*                        gone down
*
* @param       dvmrpInterface     - @b{(input)}Pointer to the interface that is going down
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpUpdateMfcInterfaceDown (dvmrp_interface_t *dvmrpInterface)
{
  mfcInterfaceChng_t ifChange;
  dvmrp_t *dvmrpcb = L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  dvmrpcb = dvmrpInterface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }
  memset(&ifChange, 0, sizeof(mfcInterfaceChng_t));
  ifChange.family = dvmrpcb->family;
  ifChange.rtrIfNum = dvmrpInterface->index;
  ifChange.mode = L7_DISABLE;
  if (mfcMessagePost(MFC_INTERFACE_STATUS_CHNG, &ifChange, 
                     sizeof(ifChange)) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  This function is used to update the MFC about cache outgoing list
*
* @param     source -  @b{(input)}Pointer to the source address of the cache
* @param     group  -  @b{(input)}Pointer to the group address of the cache
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpCacheMfcDelete (L7_inet_addr_t*source, 
                              L7_inet_addr_t *group)
{
  mfcEntry_t   mfentry;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  memset(&mfentry,0,sizeof(mfentry)); 

  inetCopy(&mfentry.source,source);
  inetCopy(&mfentry.group,group);
  mfentry.mcastProtocol   = L7_MRP_DVMRP;
  mfentry.m         = L7_NULLPTR;
  if (mfcMessagePost(MFC_DELETE_ENTRY,&mfentry, sizeof(mfentry)) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  This function is used to remove the cache entry
*
* @param     dvmrpcb    -  @b{(input)}Pointer to the  DVMRP control block
* @param     entry     -  @b{(input)} Pointer to the cache entry to be removed
* 
* @returns    L7_SUCCESS
* @returns    L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t dvmrpCacheRemove(dvmrp_t *dvmrpcb, dvmrp_cache_entry_t *entry)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (L7_SUCCESS != dvmrpCacheTableCacheDelete(dvmrpcb, &entry->source, 
                                               &entry->group))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Unable to delete cache entry from cache:%d\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to handle the MFC NO CACHE event 
*
* @param     dvmrpcb     -  @b{(input)}Pointer to the  DVMRP control block
* @param     cache_info -  @b{(input)} Pointer to the Info required to create the cache 
*                                      entry
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpMfcNoCache(dvmrp_t *dvmrpcb, dvmrp_src_grp_entry_t *cache_info)
{
  dvmrp_cache_entry_t cacheEntry, *entry = L7_NULLPTR;
  dvmrp_cache_entry_t pTmpNode, *cacheData = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_inet_addr_t source;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  inetCopy(&source,&cache_info->source);

  dvmrpKeySet(&key, source);

  /* find the longest match */
  pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d No pData Entry found\n",
                __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  route = &pData->dvmrpRouteInfo;
  /* Make sure the route entry exists for this source  */
  if (route->proto == L7_DVMRP_PROTO && 
      ((route->neighbor == L7_NULLPTR) || (BIT_TEST(route->flags, DVMRP_RT_HOLD)) ||
       (BIT_TEST(route->flags, DVMRP_RT_DELETE))))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                "%s:%d This route is either in RT_HOLD or RT_DELETE state\n",
                __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /*If the packet has been recieved on the wrong interface just ignore
   *  the packet. No need of creation of any cache entry.
   */
  if (cache_info->iif!= route->interface->index)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                "%s:%d The packet arrived on a wrong interface\n",
                __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup(dvmrpcb, &cache_info->source, &cache_info->group);                     

  if (entry == L7_NULLPTR)
  {
    inetCopy(&cacheEntry.source,&cache_info->source);
    inetCopy(&cacheEntry.group,&cache_info->group);

    if (L7_SUCCESS != dvmrpCacheTableCacheAdd(dvmrpcb,&cacheEntry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "(%s):[%d]Couldn't add the cache entry to AVL TREE\n",
                  __FUNCTION__,__LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d Addition failed to the AVL Tree\n",
                  __FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    inetCopy(&pTmpNode.source,&cache_info->source);
    inetCopy(&pTmpNode.group,&cache_info->group);
    if (L7_SUCCESS != dvmrpCacheTableCacheGet(dvmrpcb,&pTmpNode, &cacheData))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "(%s):[%d]Couldn't add the cache entry to AVL TREE\n",
                  __FUNCTION__,__LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d Addition failed to the AVL Tree\n",
                  __FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    if (cacheData != L7_NULLPTR)
    {
      memset(cacheData,0,sizeof(dvmrp_cache_entry_t));
      inetCopy(&cacheData->source,&cache_info->source);
      inetCopy(&cacheData->group,&cache_info->group);
      cacheData->parent = &dvmrpcb->dvmrp_interfaces[cache_info->iif];
      cacheData->holdtime = DVMRP_CACHE_DEFAULT_LIFETIME;
      cacheData->data = route;
      cacheData->graft_sent = L7_NULLPTR;

      if (SLLCreate (L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER, L7_NULL, dvmrp_prune_comp,
                     dvmrp_SLLNodePrune_delete, &cacheData->ll_prunes)
          != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Linked list creation failed \n",
                    __FUNCTION__,__LINE__);
        return L7_FAILURE;
      }

    }
    else
    {
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d Addition failed to the AVL Tree\n",
                  __FUNCTION__,__LINE__);
      return L7_FAILURE;
    }

    entry = cacheData;
  }

  /* Outgoing list is updated in the following function */
  dvmrp_update_fn_call(MCAST_MFC_NOCACHE_EVENT, entry, cache_info->iif);
  dvmrpApplyAdminScope(dvmrpcb,entry);            

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose    This function is to update the MFC
*
* @param      entry - @b{(input)}Pointer to the  cache entry
* 
* @returns    None
*
* @notes      None
* @end
*********************************************************************/
void dvmrp_cache_mfc_update (dvmrp_cache_entry_t *entry)
{
  L7_uchar8 oif[L7_INTF_INDICES];
  L7_uchar8 bitset[L7_INTF_INDICES];  
  L7_char8 source[DVMRP_STRING_SIZE],group[DVMRP_STRING_SIZE];
  dvmrp_t *dvmrpcb = L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  dvmrpcb =entry->parent->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  if (dvmrpIfor (&entry->children, &entry->routers, &bitset, sizeof (bitset)))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                "%s:%d DVMRP_CACHE_NEGATIVE flag removed from cache entry\n",
                __FUNCTION__,__LINE__);
    entry->flags &= ~DVMRP_CACHE_NEGATIVE;

    /* if we have created a prune retry timer for this entry, turn it off */
    if (entry->prune_retry_timer)
    {
      appTimerDelete(dvmrpcb->timerHandle, entry->prune_retry_timer);
      if (entry->pruneRetransmitimerHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &entry->pruneRetransmitimerHandle);
      }
      entry->prune_retry_timer=0;      

      /* reinitialize the prune retry back-off mechanism */
      entry->count = 0;
      entry->firstPrune = 0;
    }
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                "%s:%d DVMRP_CACHE_NEGATIVE flag set to cache entry\n",
                __FUNCTION__,__LINE__);
    entry->flags |= DVMRP_CACHE_NEGATIVE;
  }
  memcpy(&oif, &entry->children, L7_INTF_INDICES);
  MCAST_BITX_OR(oif, entry->routers,oif);
  DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
              "%s:%d (Routers bitmask)entry->routers.value[0] = %d \n",
              __FUNCTION__,__LINE__,entry->routers[0]);

  DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
              "%s:%d (Children bitmask)entry->children.value[0] = %d \n",
              __FUNCTION__,__LINE__,entry->children[0]);
  if (inetAddrHtop(&entry->source, source) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return;
  }
  if (inetAddrHtop(&entry->group, group) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
              "%s:%d src=%s,grp=%s (OIF bitmask)oif[0] = %d \n",
              __FUNCTION__,__LINE__,source,group,oif[0]);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  if (L7_SUCCESS != dvmrpCacheMfcUpdate(&entry->source, &entry->group, entry->parent, oif))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d MFC failed to update HARDWARE src=%s,grp=%s (OIF bitmask)oif.value[0] = %d \n",
                __FUNCTION__,__LINE__,source,group,oif[0]);
  }
}

/*********************************************************************
* @purpose  This function is to lookup the cache
*
* @param     dvmrpcb    -  @b{(input)}Pointer to the  DVMRP control block
* @param     source  -  @b{(input)} Pointer to the source of the cache
* @param     group -    @b{(input)} Pointer to the group of the cache
* 
* @returns    cache entry
* 
* @notes   None
* @end
*********************************************************************/
dvmrp_cache_entry_t *dvmrp_cache_lookup (dvmrp_t *dvmrpcb,L7_inet_addr_t *source, 
                                         L7_inet_addr_t *group)
{
  dvmrp_cache_entry_t mfentry;
  dvmrp_cache_entry_t  *cacheEntry = L7_NULLPTR; 

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_NULLPTR;
  }
  memset(&mfentry, L7_NULL, sizeof(dvmrp_cache_entry_t));
  inetCopy(&mfentry.source,source);
  inetCopy(&mfentry.group,group);

  if (L7_SUCCESS != dvmrpCacheTableCacheGet(dvmrpcb,&mfentry, &cacheEntry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "(%s):(%d)Couldn't find the requested cache entry \n",
                __FUNCTION__,__LINE__);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return cacheEntry;
}

/*********************************************************************
* @purpose  This function is to update the cache table when the new 
*           interface is up
*           
* @param     interface - @b{(input)} Pointer to the interface which has become up
* 
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrp_cache_update_interface_up(dvmrp_interface_t *interface)
{
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;
  dvmrp_t *dvmrpcb;
  desg_fwd_t *desg_fwd=L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb= interface->global; 

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }


  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:No cache entry in the table:%d\n", __FUNCTION__, __LINE__);
  }
  while (L7_NULLPTR != entry)
  {
    if ((dvmrpMemberShipTest(dvmrpcb,&entry->group, &entry->source, interface->index) == L7_SUCCESS)
        && (entry->parent != interface))
    {
      if (((desg_fwd =dvmrp_get_desg_fwd((dvmrp_route_t *)entry->data, interface)) !=L7_NULLPTR)&&
          (desg_fwd->status == DVMRP_ENTRY_NOT_USED))
      {
        MCAST_BITX_SET (entry->children, interface->index);
        dvmrp_cache_mfc_update (entry);
      }
    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:No more cache entries in the table:%d\n", __FUNCTION__, __LINE__);
      break;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is to update the cache table when the 
*           interface goes down
*           
* @param     interface - @b{(input)} Pointer to the Interface which has become down
* 
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrp_cache_update_interface_down(dvmrp_interface_t *interface)
{
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR,*entryNext = L7_NULLPTR;
  dvmrp_prune_t *prune,*pruneNext=L7_NULLPTR;
  dvmrp_t *dvmrpcb;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }


  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d\n", __FUNCTION__, __LINE__);
    return;
  }
  while (L7_NULLPTR != entry)
  {
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,entry, &entryNext))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d\n", __FUNCTION__, __LINE__);
    }

    if (entry->parent != L7_NULLPTR && entry->parent->index == interface->index)
    {
      if (L7_SUCCESS != dvmrpCacheTableCacheDelete(dvmrpcb,&entry->source, 
                                                   &entry->group))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Unable to delete cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
        break;
      }
    }
    else if (entry->parent != L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                  "%s:%d clear index=%d the entry->routers/children bitmask\n",
                  __FUNCTION__,__LINE__,interface->index);

      MCAST_BITX_RESET(entry->children, interface->index);
      MCAST_BITX_RESET(entry->routers, interface->index);

      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                    __FUNCTION__, __LINE__);
        return;
      }

      for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
          prune != L7_NULLPTR;
          prune = pruneNext)
      {
        pruneNext =(dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune);
        if((prune->neighbor != L7_NULLPTR)&&(prune->neighbor->interface == interface))
        {
          if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), 
                                      (L7_sll_member_t *)prune))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:Deletion failed from SLL:%d\n",
                        __FUNCTION__, __LINE__);
          }

        }
      }
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      
      dvmrp_cache_mfc_update(entry);                       
      
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d(Router Bitmask in [0]) = %d \n",
                  __FUNCTION__,__LINE__,entry->routers[0]);
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d(Children Bitmask in [0]) = %d \n",
                  __FUNCTION__,__LINE__,entry->children[0]);
    }

    entry = entryNext;
  }   
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

