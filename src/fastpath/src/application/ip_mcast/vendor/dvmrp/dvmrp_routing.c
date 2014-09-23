/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_routing.c
*
* @purpose Contains routines for the routing in the DVMRP protocol
*
* @component
*
* @comments
*
* @create 06/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "dvmrp_routing.h"
#include "dvmrp_protocol.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_mfc.h"
#include "dvmrp_util.h"
#include "dvmrp_nbr.h"
#include "dvmrp_interface.h"
#include "dvmrp_snmp.h"
#include "dvmrp_timer.h"
#include "dvmrp_igmp.h"
/*********************************************************************
* @purpose  This function is used to delete the direct routes
*
* @param     interface -  @b{(input)}Pointer to the Interface for which the direct routes
*                                     should be deleted
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_direct_route_delete(dvmrp_interface_t* interface)
{
  dvmrp_route_t *route;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  if (pData == L7_NULLPTR)
  {
    /* If network is not found then we are done.*/
    return;
  }

  /* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    dvmrpRouteData_t *pNextData = L7_NULLPTR;

    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, pData);

    route = &pData->dvmrpRouteInfo;

    if (route->interface == interface)
    {
      dvmrp_route_del (route);

      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
        return;
      }
      pData = radixDeleteEntry(&dvmrpcb->dvmrpRouteTreeData, pData);
      if (pData == L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:%d radixDeleteEntry failed\n",__FUNCTION__,__LINE__);
      }
      else
      {
        dvmrpcb->radix_entries--;
      }
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    }
    pData = pNextData;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to get the DF for this route on
*           this interface
*
* @param    route       - @b{(input)}Pointer to the route entry for which the DF is required
* @param    interface   - @b{(input)}Pointer to the  interface on whichthe DF is required
*
* @returns  designated forwarder
*
* @notes    None
*
* @end
*********************************************************************/
desg_fwd_t* dvmrp_get_desg_fwd(dvmrp_route_t *route,
                               dvmrp_interface_t *interface)
{
  return &route->desg_fwd[interface->index];
}

/*********************************************************************
* @purpose  This function is used to delete the route entry.
*
* @param     route   -  @b{(input)}Pointer to the Route entry to be deleted .
*
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_route_del (dvmrp_route_t *route)
{
  L7_int32 index;
  dvmrp_t *dvmrpcb =L7_NULLPTR;
  dvmrp_cache_entry_t mfentry;
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR, *entryNext = L7_NULLPTR;



  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((route->interface == L7_NULLPTR) ||(route->interface->global == L7_NULLPTR))
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nEither (route->interface = L7_NULLPTR)Or (route->interface->global == L7_NULLPTR)\n\n");
    return;
  }
  dvmrpcb = route->interface->global; 


  memset(&mfentry, L7_NULL, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:No cache entry in the table",__FUNCTION__);
    return ;
  }

  while (entry != L7_NULLPTR)
  {
    if (entry->graft_sent != L7_NULLPTR)
    {
      appTimerDelete(dvmrpcb->timerHandle, entry->graft_timer);
      if (entry->graftTimeoutHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &entry->graftTimeoutHandle);
      }
      DVMRP_FREE (L7_AF_INET, (void*) entry->graft_sent);        
      entry->graft_sent = L7_NULLPTR;      
      entry->graft_timer=L7_NULLPTR;

    }

    memcpy(&mfentry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:No cache entry in the table",__FUNCTION__);
    }
  }
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d\n",
                __FUNCTION__,__LINE__);
  }

  while (L7_NULLPTR != entry)
  {

    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,entry, &entryNext))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d\n",
                  __FUNCTION__,__LINE__);
      break;
    }

    if (entry->data != L7_NULLPTR && entry->data == route)
    {
      if (L7_SUCCESS != dvmrpCacheTableCacheDelete(dvmrpcb,&entry->source, &entry->group))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Unable to delete cache entry from cache:%d\n",
                    __FUNCTION__,__LINE__);
      }
    }
    entry =entryNext;
  }


  for (index=1;index<MAX_INTERFACES;index++)
  {
    if (route->desg_fwd[index].status ==DVMRP_ENTRY_USED)
    {
      memset(&route->desg_fwd[index],0,sizeof(desg_fwd_t));
      route->desg_fwd[index].status = DVMRP_ENTRY_NOT_USED;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to reset the dependency. 
*
* @param     dvmrpcb -  @b{(input)}Pointer to the DVMRP control block .
* @param     route     -  @b{(input)}Pointer to the route entry for this dependency is removed .
* @param     nbr        -  @b{(input)}Pointer to the nbr node  
*            
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_dependent_reset(dvmrp_t *dvmrpcb, dvmrp_route_t * route, dvmrp_neighbor_t *nbr)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return;
  }

  if (MCAST_BITX_TEST(route->dependents.bits,nbr->index))
  {
    MCAST_BITX_RESET (route->dependents.bits, nbr->index);

    if (dvmrp_is_only_neighbor_dependent(&route->dependents, nbr) == L7_SUCCESS)
    {

      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != 
          dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:There is no cache entry in the table:%d", 
                    __FUNCTION__, __LINE__);
      }
      while (L7_NULLPTR != entry)
      {
        dvmrp_cache_entry_t *entryNext = L7_NULLPTR;


        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                      &entryNext))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                      "%s:There is no cache entry in the table:%d", 
                      __FUNCTION__, __LINE__);
        }


        if (entry->data != route)
        {
          entry = entryNext;
          continue;
        }
        if (MCAST_BITX_TEST (entry->routers, nbr->interface->index))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:clear index=%d the entry->routers bitmask:%d\n",
                      __FUNCTION__, nbr->interface->index, __LINE__);
          MCAST_BITX_RESET(entry->routers, nbr->interface->index);
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:Updating the dvmrp_cache_mfc_update:%d\n", 
                      __FUNCTION__, __LINE__);
          dvmrp_cache_mfc_update (entry);
          if (BIT_TEST(entry->flags,DVMRP_CACHE_NEGATIVE) && route->neighbor)
          {
            MCAST_BITX_SET(entry->pruneSent, 
                           route->neighbor->interface->index);
            if (dvmrp_prune_send (route->neighbor->interface, &entry->source, 
                                  &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                  route->neighbor) != L7_SUCCESS)
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Prune Not Sent \n",
                          __FUNCTION__, __LINE__);
              return;
            }
          }
        }
        entry = entryNext;
      }     
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}

/*********************************************************************
* @purpose  This function stores routes to be advertised in a linked list. 
*
* @param   all                         -  @b{(input)}   Flag to indicate whether all the 
*                                                            routes to be included or only the 
*                                                              modified ones.
* @param   interface                -  @b{(input)}Pointer to the interface to which routes to be 
*                                                              advertised.
* @param   no_of_rts_advertise -  @b{(output)} no of routes would be stored in this 
*                                                                      list
*                                            
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/

void dvmrp_routes_prepare(L7_int32 all, dvmrp_interface_t *interface,
                          L7_uint32 *no_of_rts_advertise)
{
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  dvmrpRouteData_t *pData = L7_NULLPTR;

  L7_uint32 counter = 0;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                "%s:First Node is missing, so returning :%d\n",
                __FUNCTION__, __LINE__);
    /* If network is not found then we are done.*/
    return;
  }
  memset(dvmrpcb->report_entries, L7_NULL, platRtrRouteMaxEntriesGet() * sizeof(dvmrp_report_t ));    

  /* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    dvmrpRouteData_t *pNextData = L7_NULLPTR;
    dvmrp_route_t *route = L7_NULLPTR;
    L7_inet_addr_t *inetAddr = L7_NULLPTR; 
    dvmrp_interface_t *neighbor_vif;
    L7_int32 metric;

    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, pData);
    route = &pData->dvmrpRouteInfo;
    inetAddr = &route->networkAddr.addr;

    /* doing ouput processing and only sending changed routes */
    if (!all && !BIT_TEST (route->flags, DVMRP_RT_CHANGE))
    {
      pData = pNextData;      
      continue;
    }

    /* skip dummy route */
    if (route->interface == L7_NULLPTR && route->neighbor == L7_NULLPTR)
    {
      pData = pNextData;      
      continue;
    }

    assert (route->interface);
    if (route->interface == L7_NULLPTR)
    {
      pData = pNextData;      
      continue;
    }
    neighbor_vif = &dvmrpcb->dvmrp_interfaces[route->interface->index];
    if ((metric = dvmrp_policy_out (inetAddr, route->metric,
                                    neighbor_vif, interface, route)) < 0)
    {
      pData = pNextData;      
      continue;
    }
    if (BIT_TEST(route->flags,DVMRP_RT_HOLD) ||
        BIT_TEST(route->flags,DVMRP_RT_DELETE))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                  "%s:Setting the metric=DVMRP_UNREACHABLE:%d\n",
                  __FUNCTION__, __LINE__);
      metric = DVMRP_UNREACHABLE;
    }
    memcpy(&dvmrpcb->report_entries[counter].srcNetwork, &route->networkAddr,
           sizeof(L7_dvmrp_inet_addr_t));
    dvmrpcb->report_entries[counter].metric = metric;
    counter++;
    pData = pNextData;              
  }
  *no_of_rts_advertise = counter;
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}

/*********************************************************************
* @purpose  This function advertises routes on all interfaces
*
* @param     all            -  @b{(input)}  Flag to indicate whether all the 
*                                            routes to be included or only the 
*                                           modified ones.
* @param     dvmrpcb   -  @b{(input)}Pointer to the DVMRP control block
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_route_advertise(L7_int32 all, dvmrp_t *dvmrpcb)
{
  L7_BOOL active_nbr = L7_FALSE;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_uint32 uiIntf = 0;
  L7_uint32 no_of_rts_advertise = 0;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: ENTERED \n", __FUNCTION__);
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return;
  }

  /* nothing changed */
  if (!dvmrpcb->changed && !all)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:nothing changed, returning :%d\n",
                __FUNCTION__, __LINE__);
    return;
  }


  for (uiIntf=0;uiIntf < MAX_INTERFACES;uiIntf++)
  {

    interface = &dvmrpcb->dvmrp_interfaces[uiIntf];

    if (interface->index != uiIntf)
    {
      continue;
    }

    assert (MCAST_BITX_TEST (dvmrpcb->interface_mask, interface->index));
    if (!BIT_TEST (interface->flags, DVMRP_VIF_UP))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                  "%s:DVMRP_VIF_UP not set, so returning :%d\n",
                  __FUNCTION__, __LINE__);
      continue;
    }
    if (BIT_TEST (interface->flags, DVMRP_VIF_LEAF))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                  "%s:DVMRP_VIF_LEAF set, so returning :%d\n", 
                  __FUNCTION__, __LINE__);
      continue;
    }
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:No_of_Nbrs=%d for RtrIfNum=%d :%d\n",
                __FUNCTION__, SLLNumMembersGet(&(interface->ll_neighbors)), 
                interface->index, __LINE__);
    active_nbr = L7_FALSE;
    for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
        nbr != L7_NULLPTR;
        nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
    {
      if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_ACTIVE)
      {
        active_nbr = L7_TRUE;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: There atleast one ACTIVE NBR found\n", __FUNCTION__);
        break;
      }
    }
    if (active_nbr)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:preparing routes to send :%d\n", 
                  __FUNCTION__, __LINE__);

      memset(dvmrpcb->report_entries, L7_NULL, platRtrRouteMaxEntriesGet() * sizeof(dvmrp_report_t ));
      dvmrp_routes_prepare (all, interface, &no_of_rts_advertise);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:no_of_rts_advertise=%d\n",__FUNCTION__,no_of_rts_advertise);
      if (no_of_rts_advertise > 0)
      {
        if (dvmrp_routes_send (no_of_rts_advertise, interface) == L7_FAILURE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Report Not Sent \n",
                      __FUNCTION__, __LINE__);
        }
      }
    }
    else
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:active_nbr flag not set:%d\n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:active_nbr flag not set:%d\n", 
                  __FUNCTION__, __LINE__);
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:No_of_Interfaces=%d :%d\n",
              __FUNCTION__, uiIntf,__LINE__);

  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  if (pData == L7_NULLPTR)
  {
    /* If network is not found then we are done.*/
    return;
  }
  /* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    dvmrpRouteData_t *pNextData = L7_NULLPTR;
    dvmrp_route_t *route = L7_NULLPTR;

    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, pData);

    route = &pData->dvmrpRouteInfo;
    if (route == L7_NULLPTR)
    {
      pData = pNextData;        
      continue;
    }
    /* clearing change flag */
    route->flags &= ~(DVMRP_RT_CHANGE);
    pData = pNextData;     
  }
  dvmrpcb->changed = L7_NULL;
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: dvmrpcb->changed = L7_NULL \n", 
              __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}

/*********************************************************************
* @purpose  This function is called when ever cache table needs to eb updated
*
* @param     code    -  @b{(input)}  type of event.
* @param     entry   -  @b{(input)}Pointer to the cache entry to be updated
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_update_fn_call(L7_int32 code, dvmrp_cache_entry_t *entry, 
                              L7_uint32 ifindex) 
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_int32 prune = 1;
  L7_uint32 now;
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  L7_inet_addr_t source;    
  L7_uint32 s_addr = L7_NULL;
  L7_char8 src[IPV6_DISP_ADDR_LEN], grp[IPV6_DISP_ADDR_LEN],nbrAddr[IPV6_DISP_ADDR_LEN];
  L7_uint32 uiIntf = 0;
  L7_inet_addr_t  tmp;  
  desg_fwd_t *desg_fwd=L7_NULLPTR;   
  dvmrp_neighbor_t *nbr = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);

  if ((entry->parent== L7_NULLPTR) ||(entry->parent->global == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  dvmrpcb = entry->parent->global;

  inetCopy(&source,&entry->source);

  inetCopy(&tmp,&source); 
  inetAddrHton(&tmp, &source); 

  if (inetAddrHtop(&entry->source, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return L7_FAILURE;


  }
  if (inetAddrHtop(&entry->group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return L7_FAILURE;


  }

  now = osapiUpTimeRaw();
  switch (code)
  {
    case MCAST_MFC_NOCACHE_EVENT:

      dvmrpKeySet(&key, source);

      /* find the longest match */
      pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
      if (pData == L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, "%s:%d pData is L7_NULLPTR \n",
                    __FUNCTION__, __LINE__);
        return(L7_ERROR);
      }
      route = &pData->dvmrpRouteInfo;

      if (route->proto == L7_DVMRP_PROTO && 
          ((route->neighbor == L7_NULLPTR) ||
           (BIT_TEST(route->flags, DVMRP_RT_HOLD)) ||
           (BIT_TEST(route->flags, DVMRP_RT_DELETE))))
      {
        /* this is the special route to trap all others */
        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                    "%s:%d Route flag set to "
                    "DVMRP_RT_HOLD/DVMRP_RT_DELETE\n", __FUNCTION__, 
                    __LINE__);
        entry->parent = L7_NULLPTR;
        return(L7_ERROR);
      }
      assert (route->interface);
      entry->parent = route->interface;
      for (uiIntf =0;uiIntf < MAX_INTERFACES;uiIntf++)
      {
        interface = &dvmrpcb->dvmrp_interfaces[uiIntf];
        if ((interface->index != uiIntf) || (interface == route->interface))
        {
          continue;
        }
        if (interface->flags & DVMRP_VIF_LEAF)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:%d This intface(ifindex=%d) is a "
                      "DVMRP_VIF_LEAF\n", __FUNCTION__, __LINE__, 
                      interface->index);
          if (dvmrpMemberShipTest(dvmrpcb,&entry->group, &entry->source, 
                                  interface->index) == L7_SUCCESS)
          {
            MCAST_BITX_SET (entry->children, interface->index);
            prune = 0;
          }
        }
        else
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:%d This intface(ifindex=%d) not a "
                      "DVMRP_VIF_LEAF\n", __FUNCTION__, __LINE__, 
                      interface->index);
          /*
           * Check if EVERY neighbor on this interface has pruned this entry,
           * if so, do not add interface to outgoing interfaces
           */
          /* 
           * we have one or more prunes, so check prune list against 
           * dependent list 
           */
          if (SLLNumMembersGet(&(entry->ll_prunes)) > 0)
          {
            dvmrp_prune_t* pruneNode=L7_NULLPTR;
            neighbor_bitset_t bitset;

            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:%d Prunes present for this entry\n", 
                        __FUNCTION__, __LINE__);
            /* start with all dependent neighbors for this route */
            bitset = route->dependents;
            for (pruneNode = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
                pruneNode != L7_NULLPTR;
                pruneNode = (dvmrp_prune_t*)SLLNextGet(
                                                      &(entry->ll_prunes),(void*)pruneNode))
            {
              /* clear any bit corresponding to a neighbor that has pruned */
              MCAST_BITX_RESET (bitset.bits, pruneNode->neighbor->index);  
              DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                          "%s:%d clear bit of nbrIndex=%d, from route "
                          "dependence list\n", __FUNCTION__, __LINE__, 
                          pruneNode->neighbor->index);
            }
            
            for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
                nbr != L7_NULLPTR;
                nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
            {
              if (MCAST_BITX_TEST(bitset.bits, nbr->index))
              {
                MCAST_BITX_SET(entry->routers, interface->index);    
                break;          
              }
            }
            /*
             * if bitset != 0 then at least one neighbor in the 
             * route->dependents 
             * bit mask has NOT pruned this traffic,
             * so add the interface to the cache entry children bitset
             */
            if ((dvmrpMemberShipTest(dvmrpcb,&entry->group, 
                                      &entry->source, 
                                      interface->index) == L7_SUCCESS) &&
                (((desg_fwd = dvmrp_get_desg_fwd(route,interface)) != L7_NULLPTR)&&
                 (desg_fwd->status == DVMRP_ENTRY_NOT_USED)))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                          "%s:%d setting the RtrIfNum=%d, to children "
                          "bitmask\n", __FUNCTION__, __LINE__, 
                          interface->index);
              MCAST_BITX_SET (entry->children, interface->index);
              prune = 0;
            }
          }
          else  /* no prunes, so add the outgoing interface */
          {
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:%d No Prunes present for this entry, src=%s\n",
                        __FUNCTION__, __LINE__, src);
            if (((desg_fwd =dvmrp_get_desg_fwd(route,interface)) != L7_NULLPTR)&&
                (desg_fwd->status == DVMRP_ENTRY_NOT_USED))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                          "%s:%d No Designated Forwarder elected, src=%s\n",
                          __FUNCTION__, __LINE__, src);
              if (dvmrp_any_dependents_on_this_interface(&route->dependents,
                                                         interface) == L7_SUCCESS)
              {
                DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                            "%s:%d Router bitmask is set the index=%d for "
                            "the src=%s\n", __FUNCTION__, __LINE__, 
                            interface->index, src);
                MCAST_BITX_SET (entry->routers, interface->index);
                prune = 0;    
              }
              if ((dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                        interface->index) == L7_SUCCESS))
              {
                DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                            "%s:%d setting the RtrIfNum=%d, to children "
                            "bitmask, src=%s\n", __FUNCTION__, __LINE__, 
                            interface->index, osapiInet_ntoa(s_addr));
                MCAST_BITX_SET (entry->children, interface->index);
                prune = 0;
              }
            }
            else
            {
              DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                          "%s:%d dvmrp desg fwd status != DVMRP_ENTRY_NOT_USED\n",
                          __FUNCTION__, __LINE__);
            }
          }
        }
      }
      DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, "%s:%d updating the Cache Entry\n",
                  __FUNCTION__, __LINE__);
      dvmrp_cache_mfc_update(entry);
      /* initialize entry holdtime to default value */
      entry->holdtime = DVMRP_CACHE_EXPIRE_TIME;
      entry->expire = now +entry->holdtime;

      /* excepting a direct connected source */
      if (prune && route->neighbor)
      {
        /* check that the retry interval has lapsed before re-pruning */
        if ((now - entry->lastPrune) >= entry->prune_retry_interval)
        {
          MCAST_BITX_SET(entry->pruneSent, 
                         route->neighbor->interface->index);
          if (dvmrp_prune_send (route->neighbor->interface, &entry->source, 
                                &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                route->neighbor) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nPrune Not Sent \n");
            return L7_FAILURE;
          }

          /* If we continue to get data from a neighbor who claims to be 
           * pruning, send a trap 
           */
          if ((entry->count != 0) && ((entry->count % 5) == 0))
          {
            L7_ulong32 capabilities = 
            (route->neighbor->versionSupport >> 16) & 0x000F;

            interface = entry->parent;      
            if (interface != L7_NULLPTR)
            {
              if (capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)  && 
                  interface)
              {
                if (L7_SUCCESS != dvmrp_send_neighbor_not_pruning_trap(interface, route->neighbor,
                                                                       capabilities))
                {
                  DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                              __FUNCTION__, __LINE__);
                }
              }
            }
          }
        }
      }
      return(L7_SUCCESS);
    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
      break;
    case MCAST_MFC_WRONGIF_EVENT:
      break;
    case DVMRP_EVENT_MFC_ADD_MEMBER:
      /*
       * If the cache entry is marked for deletion and timer started to remove on
       * expiry, delete the timer now, as the join came for that cache
       */
      if (entry->cacheRemove_timer != L7_NULLPTR)
      {
        if (L7_SUCCESS != appTimerDelete(dvmrpcb->timerHandle, entry->cacheRemove_timer))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Failed to remove the timer grp=%s,src=%s on index=%d\n",
                      __FUNCTION__,grp,src,entry->parent->index);
        }
        else
        {
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Successfully removed the timer grp=%s,src=%s on index=%d\n",
                      __FUNCTION__,grp,src,entry->parent->index);
        }
        if (entry->cacheRemoveHandle != L7_NULL)
        {
          handleListNodeDelete(dvmrpcb->handle_list,
                               &entry->cacheRemoveHandle);
        }
        entry->cacheRemove_timer=L7_NULLPTR;
      }
      if (entry->data)
      {
        route = entry->data;
        /* check if we have pruned, make sure we have upstream neighbor - 
         * if so, graft entry 
         */
        if (((entry->count != 0) || (entry->firstPrune !=0))
            && (route->neighbor))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Graft being sent for grp=%s,src=%s on index=%d\n",
                      __FUNCTION__,grp,src,entry->parent->index);
          DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s: Graft being sent for grp=%s,src=%s on index=%d\n",
                      __FUNCTION__,grp,src,entry->parent->index);
          if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nGraft Not Sent \n");
            return L7_FAILURE;
          }
        }
        if (entry->parent->index != ifindex)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Setting index=%d to OIF list for grp=%s,src=%s\n",
                      __FUNCTION__,ifindex,grp,src);
          MCAST_BITX_SET(entry->children, ifindex);
          dvmrp_cache_mfc_update (entry);
        }
      }
      break;
    case DVMRP_EVENT_MFC_DEL_MEMBER:
      if (entry->data)
      {
        route = entry->data;
        if (entry->parent->index != ifindex)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Clearing index=%d from OIF list for grp=%s,src=%s\n",
                      __FUNCTION__,ifindex,grp,src);
          MCAST_BITX_RESET(entry->children, ifindex);
          dvmrp_cache_mfc_update (entry);
        }
        if (route->neighbor && BIT_TEST(entry->flags, DVMRP_CACHE_NEGATIVE))
        {
          if (inetAddrHtop(&route->neighbor->nbrAddr.addr, nbrAddr) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
            return L7_FAILURE;


          }
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Sending PRUNE on index=%d to nbr=%s for grp=%s,src=%s\n",
                      __FUNCTION__,entry->parent->index,nbrAddr,grp,src);
          MCAST_BITX_SET (entry->pruneSent, 
                          route->neighbor->interface->index);
          if (dvmrp_prune_send(route->neighbor->interface, &entry->source, 
                               &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                               route->neighbor) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nPrune Not Sent \n");
            return L7_FAILURE;
          }
        }
      }
      break;
    case DVMRP_EVENT_ADMIN_SCOPE_ADD:
      {
        MCAST_BITX_RESET(entry->children, ifindex);
        MCAST_BITX_RESET(entry->routers, ifindex);
        dvmrp_cache_mfc_update (entry);
        route = entry->data;
        if ((route != L7_NULLPTR) && (route->neighbor && BIT_TEST(entry->flags, DVMRP_CACHE_NEGATIVE)))
        {
          if (!MCAST_BITX_TEST (entry->pruneSent,
                                route->neighbor->interface->index))
          {
            if (dvmrp_prune_send(route->neighbor->interface, &entry->source, 
                                 &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                 route->neighbor) != L7_SUCCESS)
            {
              DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nPrune Not Sent \n");
              return L7_FAILURE;
            }
            MCAST_BITX_SET (entry->pruneSent, 
                            route->neighbor->interface->index);
          }
        }
      }
      break;
    case DVMRP_EVENT_ADMIN_SCOPE_DELETE:
      {
        L7_uchar8 pOif[L7_INTF_INDICES];
        L7_BOOL isEmpty = L7_FALSE;
        if (entry->cacheRemove_timer != L7_NULLPTR)
        {
          if (L7_SUCCESS != appTimerDelete(dvmrpcb->timerHandle, entry->cacheRemove_timer))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Failed to remove the timer grp=%s,src=%s on index=%d\n",
                        __FUNCTION__,grp,src,entry->parent->index);
          }
          else
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Successfully removed the timer grp=%s,src=%s on index=%d\n",
                        __FUNCTION__,grp,src,entry->parent->index);
          }
          if (entry->cacheRemoveHandle != L7_NULL)
          {
            handleListNodeDelete(dvmrpcb->handle_list,
                                 &entry->cacheRemoveHandle);
          }
          entry->cacheRemove_timer=L7_NULLPTR;
        }
        interface = &dvmrpcb->dvmrp_interfaces[ifindex];
        if (dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                 ifindex) == L7_SUCCESS)
        {
          MCAST_BITX_SET(entry->children, ifindex);
        }
        if (dvmrp_any_dependents_on_this_interface(&entry->data->dependents, 
                                                   interface) == L7_SUCCESS)
        {
          MCAST_BITX_SET(entry->routers, ifindex);
        }

        MCAST_BITX_OR(entry->children, entry->routers, pOif);
        MCAST_BITX_IS_EMPTY(pOif, isEmpty);
        if (isEmpty == L7_FALSE)
        {
          route = entry->data;
          /* check if we have pruned, make sure we have upstream neighbor - 
           * if so, graft entry 
           */
          if ((route != L7_NULLPTR) && ((entry->count != 0) || (entry->firstPrune !=0))
              && (route->neighbor))
          {
            if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
            {
              DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nGraft Not Sent \n");
              return L7_FAILURE;
            }
          }
        }
        dvmrp_cache_mfc_update (entry);
      }
      break;
    default:
      assert (0);
      break;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function is called to inject direct routes
* 
* @param     interface  -  @b{(input)}Pointer to the interface for which the direct route
*                                       is being introduced
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_direct_route_inject(dvmrp_interface_t *interface)
{
  L7_uint32 now;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  dvmrp_route_t route;
  dvmrpRouteData_t routeData, *pData = L7_NULLPTR;
  L7_dvmrp_inet_addr_t *network = L7_NULLPTR,network1;
  L7_inet_addr_t net_mask;
  L7_char8 net[IPV6_DISP_ADDR_LEN];
  L7_inet_addr_t  route_network,tmp;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nINVALID : Interface is NULL!\n\n");
    return;
  }

  now = osapiUpTimeRaw();
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  if (!BIT_TEST (interface->flags, DVMRP_VIF_UP))
  {
    return;
  }

  network = &interface->primary.inetAddr;
  if (inetAddrHtop(&network->addr, net) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
    return;


  }
  if (network->addr.family == dvmrpcb->family)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: Adding the direct route for intf IP=%s\n",
                __FUNCTION__,net);
    memset(&net_mask, 0, sizeof(L7_inet_addr_t));
    memcpy(&network1,network,sizeof(L7_dvmrp_inet_addr_t));

    if (L7_SUCCESS != inetMaskLenToMask(dvmrpcb->family, network->maskLength, 
                                        &net_mask))
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
      return;
    }
    /* Always zero host bits in destination prefix. */
    inetCopy(&tmp,&network1.addr);
    inetAddrHton(&tmp, &network1.addr); 
    memset(&route_network,0,sizeof(L7_inet_addr_t));      
    if (inetAddressAnd(&network1.addr,&net_mask, &route_network) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
      return;
    }
    inetCopy(&network1.addr,&route_network);

    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:Adding the direct route for net=%s\n",
                __FUNCTION__,net);
    /* create new one for masking */
    dvmrp_new_route (DVMRP_LOCAL_ROUTE, &network1, interface, 
                     nbr, 0, 0, &route );
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:route ptr==%p for net=%s\n",
                __FUNCTION__,&route, net);


    route.ctime = now;
    /* timeout off */
    route.utime = 0;
    memset(&routeData, 0, sizeof(dvmrpRouteData_t));
    dvmrpKeySet(&routeData.network, route_network);
    dvmrpKeySet(&routeData.netmask, net_mask);

    /* See if we already have a route to this destination. */
    pData = radixLookupNode(&dvmrpcb->dvmrpRouteTreeData, &routeData.network, 
                            &routeData.netmask, L7_RN_EXACT);

    /* If this is a new network then check that we have not exceeded
     * the maximum route capacity.
     */
    if ((pData == L7_NULLPTR) && (dvmrpIsFull(dvmrpcb) == L7_TRUE))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:pData is NULL && dvmrpIsFull for net=%s\n",
                  __FUNCTION__,net);
      return;
    }
    if (pData == L7_NULLPTR)
    {
      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
        return ;
      }

      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                  "\n%s:No_of_entries=%d,Inserting a New direct route for net=%s\n",
                  __FUNCTION__,radixTreeCount(&dvmrpcb->dvmrpRouteTreeData),net);
      /* Insert new destination */
      pData = radixInsertEntry(&dvmrpcb->dvmrpRouteTreeData, &routeData);
      /* NULL indicates successful insertion */
      if (pData == L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:No_of_entries=%d,Inserting is successful for net=%s\n",
                    __FUNCTION__,radixTreeCount(&dvmrpcb->dvmrpRouteTreeData),net);
        dvmrpcb->radix_entries++;
      }
      else
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Failed to insert pData\n");
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:Failed to insert pData for net=%s\n",
                    __FUNCTION__,net);
      }
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);

      /* Find the new entry so that we can attach route info. */
      pData = radixLookupNode(&dvmrpcb->dvmrpRouteTreeData, 
                              &routeData.network, &routeData.netmask, L7_RN_EXACT);
      if (!pData)
      {
        /* Database corruption. */
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "Error: rto.c radix corruption detected!\n");
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:radix corruption detected! net=%s\n",
                    __FUNCTION__,net);
        return;
      }
    }
    else
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:Route already exists! net=%s\n",
                  __FUNCTION__,net);
    }
    memcpy(&pData->dvmrpRouteInfo,&route,sizeof(dvmrp_route_t));
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"\n%s:Route is added net=%s\n",
                __FUNCTION__,net);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "\n%s:pData ptr==%p for net=%s\n",
                __FUNCTION__,pData, net);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to update the route entry. 
*
* @param     inetAddr -  @b{(input)}Pointer to the network address .
*                  metric   -   @b{(input)} -Metric of this netwrok
*                  nbr      -    @b{(input)}Pointer to the nbr node  
*                  now      -   @b{(input)} current time        
*            
* @returns  L7_ERROR/L7_FAILURE/L7_SUCCESS
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_route_update (L7_dvmrp_inet_addr_t *inetAddr, L7_int32 metric,
                             dvmrp_neighbor_t *nbr, L7_uint32 now)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_int32 adjusted_metric;
  dvmrp_route_t *route = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  L7_inet_addr_t  inetAddr1;
  L7_inet_addr_t net_mask;
  dvmrpRouteData_t routeData;
  L7_uint32 nbrIndex = L7_NULL;
  L7_char8 dstNetwork[IPV6_DISP_ADDR_LEN], NbrRtr[IPV6_DISP_ADDR_LEN], mask[IPV6_DISP_ADDR_LEN];
  L7_inet_addr_t tmp,route_network; 
  desg_fwd_t *desg_fwd=L7_NULLPTR;  


  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if (inetAddrHtop(&inetAddr->addr, dstNetwork) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);


  }
  if (inetAddrHtop(&nbr->nbrAddr.addr, NbrRtr) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);


  }
  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Nbr pointer is NULL!!!!!!!!.\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);
  }
  interface = nbr->interface;
  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Interface Pointer Null.\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
              "%s:%d Updating the route for net=%s,nbr=%s on i/f=%d \n",
              __FUNCTION__, __LINE__, dstNetwork, NbrRtr, interface->index);
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                "%s: nbr->flags = DVMRP_NEIGHBOR_DELETE \n", __FUNCTION__);
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return(L7_ERROR);
  }
  dvmrpcb = nbr->interface->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);   
  }



  assert ((metric & 0x80) == 0);
  assert (nbr);

  if ((adjusted_metric = dvmrp_policy_in (&inetAddr->addr, metric, 
                                          interface)) < 0)
  {
    dvmrp_badRts_inc(interface,nbr);
    return(L7_ERROR);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: adjusted_metric =  %d\n", 
              __FUNCTION__, adjusted_metric);
  inetCopy(&tmp,&inetAddr->addr);
  inetAddrHton(&tmp, &inetAddr1);  

  memset(&routeData, 0, sizeof(dvmrpRouteData_t));
  memset(&net_mask, 0, sizeof(L7_inet_addr_t));

  if (L7_SUCCESS != inetMaskLenToMask(dvmrpcb->family, inetAddr->maskLength, 
                                      &net_mask))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert masklen to mask\n",
                __FUNCTION__, __LINE__); 
    return(L7_ERROR);
  }
  if (inetAddrHtop(&net_mask, mask) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return(L7_ERROR);


  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d maskLength=%d,netmask=%s \n",
              __FUNCTION__, __LINE__, inetAddr->maskLength, mask);
  /* Always zero host bits in destination prefix. */

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d No_Of_Radix_Nodes=%d\n",
              __FUNCTION__,__LINE__,radixTreeCount(&dvmrpcb->dvmrpRouteTreeData));
  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(&inetAddr1,&net_mask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Inet And operation Failed!!!!!!!!\n",
                __FUNCTION__, __LINE__); 
    return L7_FAILURE;
  }
  dvmrpKeySet(&routeData.network, route_network);
  dvmrpKeySet(&routeData.netmask, net_mask);

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  /* See if we already have a route to this destination. */
  pData = radixLookupNode(&dvmrpcb->dvmrpRouteTreeData, &routeData.network,
                          &routeData.netmask, L7_RN_EXACT);

  /* If this is a new network then check that we have not exceeded
   * the maximum route capacity.
   */
  if ((pData == L7_NULLPTR) && (dvmrpIsFull(dvmrpcb) == L7_TRUE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                "%s:%d Route LookUp failed for net=%s,nbr=%s on i/f=%d \n",
                __FUNCTION__, __LINE__, dstNetwork, NbrRtr,
                interface->index);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Route LookUp failed for net=%s,nbr=%s on i/f=%d \n",
                __FUNCTION__, __LINE__, dstNetwork, NbrRtr, 
                interface->index);
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return(L7_ERROR);
  }
  if (pData == L7_NULLPTR)
  {
    /* Insert new destination */
    pData = radixInsertEntry(&dvmrpcb->dvmrpRouteTreeData, &routeData);
    /* NULL indicates successful insertion */
    if (pData == L7_NULLPTR)
    {
      dvmrpcb->radix_entries++;
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                  "%s:%d New Route addition success for net=%s,nbr=%s on "
                  "i/f=%d \n", __FUNCTION__, __LINE__, dstNetwork, NbrRtr, 
                  interface->index);
    }
    else
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Failed to insert pData\n");
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: Failed to insert pData \n", 
                  __FUNCTION__);
    }

    /* Find the new entry so that we can attach route info. */
    pData = radixLookupNode(&dvmrpcb->dvmrpRouteTreeData, &routeData.network,
                            &routeData.netmask, L7_RN_EXACT);
    if (!pData)
    {
      /* Database corruption. */
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "Error: rto.c radix corruption detected!\n");
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                  "Error: rto.c radix corruption detected!\n");
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return(L7_ERROR);
    }
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d Radix Node is already present net=%s,nbr=%s on i/f=%d \n",
                __FUNCTION__,__LINE__,dstNetwork,NbrRtr,interface->index);
  }
  route = &pData->dvmrpRouteInfo;
  if (route->entryStatus == DVMRP_ROUTE_ENTRY_NOT_USED)
  {
    if (adjusted_metric >= DVMRP_UNREACHABLE)
    {
      pData = radixDeleteEntry(&dvmrpcb->dvmrpRouteTreeData, pData);
      if (pData == L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d radixDeleteEntry failed\n", 
                    __FUNCTION__, __LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d radixDeleteEntry failed\n", 
                    __FUNCTION__, __LINE__);
      }

      dvmrpcb->radix_entries--;
      dvmrp_badRts_inc(interface,nbr);
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return(L7_FAILURE);
    }
    /* section 3.4.6.A*/
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d New Route Entry being created\n",
                __FUNCTION__, __LINE__);
    dvmrp_new_route (L7_DVMRP_PROTO, inetAddr, interface,
                     nbr, adjusted_metric, metric,route);
    route->ctime = route->utime = now;
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return(L7_SUCCESS);
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d Route Pointer is NON-NULL net=%s,nbr=%s on i/f=%d \n",
                __FUNCTION__,__LINE__,dstNetwork,NbrRtr,interface->index);
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
              "%s:%d Attached Route Info for net=%s,nbr=%s on i/f=%d \n",
              __FUNCTION__, __LINE__, dstNetwork, NbrRtr, interface->index);

  /* dependencies check */
  if (metric < DVMRP_UNREACHABLE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d metric < DVMRP_UNREACHABLE\n", 
                __FUNCTION__, __LINE__);
    /*Received metric <infinity ...RFC 3.4.6.B.1*/
    dvmrp_dependent_reset(dvmrpcb,route,nbr);
    /* TODO: Designated forwarder checks*/
    dvmrp_df_elect(route,nbr,metric);
    if (adjusted_metric > route->metric && !BIT_TEST(route->flags,DVMRP_RT_HOLD)
        && !BIT_TEST(route->flags,DVMRP_RT_DELETE ))
    {
      /*Received adjusted_metric > route->metric RFC 3.4.6.B.1.a*/
      if (nbr == route->neighbor)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                    "%s:%d adjusted_metric > route->metric\n", 
                    __FUNCTION__, __LINE__);
        /* Check needs to be made that the adjusted metric is not unreachable,
         * if it is unreachable then this route needs to be deleted and flash 
         * update needs to be sent 
         */
        if (adjusted_metric >= DVMRP_UNREACHABLE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                      "%s:%d adjusted_metric >= DVMRP_UNREACHABLE\n", 
                      __FUNCTION__, __LINE__);
          route->flags |= DVMRP_RT_DELETE;
          route->flags |= DVMRP_RT_CHANGE;
          route->metric = DVMRP_UNREACHABLE;
          route->received_metric = metric;
          route->dtime = now + DVMRP_GARBAGE_INTERVAL;
          dvmrpcb->changed++;
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n", 
                      __FUNCTION__, __LINE__);
          return L7_SUCCESS;
        }
        else
        {
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                      "%s:%d adjusted_metric < DVMRP_UNREACHABLE\n",
                      __FUNCTION__, __LINE__);
          route->flags |= DVMRP_RT_CHANGE;
          route->metric = adjusted_metric;
          route->received_metric = metric;
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n", 
                      __FUNCTION__, __LINE__);
          dvmrpcb->changed++;
          return L7_SUCCESS;
        }
      }
      else
      {
        /* adjusted metric greater than existing and reporting neighbor is not
           same as neighbor in best route, so do not update (skip) */
        return L7_FAILURE; /* goto next route*/
      }
    }
    else if (adjusted_metric < route->metric)
    {
      /*Received adjusted_metric < route->metric RFC 3.4.6.B.1.b*/
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d adjusted_metric < route->metric\n",
                  __FUNCTION__, __LINE__);
      route->metric = adjusted_metric;
      route->received_metric = metric;

      if (route->interface != interface) /*Merges from REL_K*/
      {
        if (route->desg_fwd[route->interface->index].status==  DVMRP_ENTRY_USED)
        {
          memset(&route->desg_fwd[route->interface->index],0,sizeof(desg_fwd_t));
          route->desg_fwd[route->interface->index].status = DVMRP_ENTRY_NOT_USED;
        }
      }

      if (nbr != route->neighbor)
      {
        /* update upstream neighbor*/
        memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                      &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                      "%s:There is no cache entry in the table:%d\n", 
                      __FUNCTION__, __LINE__);
        }
        while (L7_NULLPTR != entry)
        {
          if (entry->data == route)
          {
            entry->parent = nbr->interface;
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:Updating the dvmrp_cache_mfc_update:%d\n", 
                        __FUNCTION__, __LINE__);
            dvmrp_cache_mfc_update (entry);
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                        &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                        "%s:There is no cache entry in the table:%d\n", 
                        __FUNCTION__, __LINE__);
            break;
          }
        }
      }
      if (BIT_TEST(route->flags,DVMRP_RT_HOLD)|| 
          BIT_TEST(route->flags,DVMRP_RT_DELETE))
      {
        if ((nbr == route->neighbor) && 
            (route->neighbor->state 
             == L7_DVMRP_NEIGHBOR_STATE_DOWN))
        {
          route->neighbor->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
        }
        BIT_RESET(route->flags,DVMRP_RT_HOLD);
        BIT_RESET(route->flags,DVMRP_RT_DELETE);
      }
      route->flags |= DVMRP_RT_CHANGE;
      route->neighbor = nbr;
      route->interface = interface;
      route->utime = now;
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n", 
                  __FUNCTION__, __LINE__);
      dvmrpcb->changed++;
      return L7_SUCCESS;
    }
    else if (adjusted_metric == route->metric)
    {
      /*Received adjusted_metric == route->metric RFC 3.4.6.B.1.c*/
      if (nbr == route->neighbor)
      {
        route->utime = now;
        if (BIT_TEST(route->flags,DVMRP_RT_HOLD) || 
            BIT_TEST(route->flags,DVMRP_RT_DELETE))
        {
          BIT_RESET(route->flags,DVMRP_RT_HOLD);
          BIT_RESET(route->flags,DVMRP_RT_DELETE);
          route->neighbor->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
          route->flags |= DVMRP_RT_CHANGE;
          /* even though the adjusted metric hasn't changed, if the incoming 
           * interface metric changed concurrently with a complementary 
           * change in the received metric value, the adjusted metric may be 
           * the same and the received metric be different, so update the 
           * value in route
           */
          route->received_metric = metric;
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n",
                      __FUNCTION__, __LINE__);
          dvmrpcb->changed++;
          return L7_SUCCESS;
        }
      }
      else if ((L7_INET_ADDR_COMPARE(&nbr->nbrAddr.addr, 
                                     &route->neighbor->nbrAddr.addr) 
                < L7_NULL) &&
               !BIT_TEST(route->flags,DVMRP_RT_HOLD) && 
               !BIT_TEST(route->flags,DVMRP_RT_DELETE))
      {
        if (route->interface != interface)
        {
          if (route->desg_fwd[route->interface->index].status == DVMRP_ENTRY_USED)
          {
            memset(&route->desg_fwd[route->interface->index],0,sizeof(desg_fwd_t));
            route->desg_fwd[route->interface->index].status = DVMRP_ENTRY_NOT_USED;
          }
        }

        route->flags |= DVMRP_RT_CHANGE;
        route->neighbor = nbr;
        route->interface = interface;
        route->utime = now;
        route->received_metric = metric;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n",
                    __FUNCTION__, __LINE__);
        dvmrpcb->changed++;
        /* update upstream neighbor*/
        memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                      &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                      "%s:There is no cache entry in the table:%d\n",
                      __FUNCTION__, __LINE__);
        }

        while (L7_NULLPTR != entry)
        {
          if (entry->data == route)
          {
            entry->parent = nbr->interface;
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:Updating the dvmrp_cache_mfc_update:%d\n", 
                        __FUNCTION__, __LINE__);
            dvmrp_cache_mfc_update (entry);
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                        &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:There is no cache entry in the table:%d\n",
                        __FUNCTION__, __LINE__);
            break;
          }
        }
        return L7_SUCCESS;
      }
    }
  }
  else if (metric == DVMRP_UNREACHABLE && !BIT_TEST(route->flags,DVMRP_RT_HOLD)
           && !BIT_TEST(route->flags,DVMRP_RT_DELETE))
  {
    /*Received metric == infinity ...RFC 3.4.6.B.2*/
    /* become designated forwarder if nbr was one:*/
    dvmrp_df_elect(route,nbr,metric);
    if (route->interface == interface && route->neighbor == nbr)
    {
      /*Received adjusted_metric == route->metric RFC 3.4.6.B.2.a*/
      if (route->metric < DVMRP_UNREACHABLE)
      {
        route->flags |= DVMRP_RT_DELETE;
        route->flags |= DVMRP_RT_CHANGE;
        route->metric = DVMRP_UNREACHABLE;
        route->received_metric = metric;
        route->dtime = now + DVMRP_GARBAGE_INTERVAL;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n", 
                    __FUNCTION__, __LINE__);
        dvmrpcb->changed++;
        return L7_SUCCESS;
      }
    }
    else
    {
      /*Received adjusted_metric == route->metric RFC 3.4.6.B.2.b*/
      dvmrp_dependent_reset(dvmrpcb,route,nbr);
    }
  }
  else if (metric > DVMRP_UNREACHABLE && metric < (2 * DVMRP_UNREACHABLE) &&
           !BIT_TEST(route->flags,DVMRP_RT_HOLD) && 
           !BIT_TEST(route->flags,DVMRP_RT_DELETE))
  {
    /*infinity < received metric < 2 * infinity...RFC 3.4.6.B.3*/
    /* if sender is designated forward, become designated forwarder*/

    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                "%s:%d Poisseon expressed Route recved for net=%s,nbr=%s "
                "on i/f=%d \n", __FUNCTION__, __LINE__, dstNetwork, NbrRtr,
                interface->index);
    dvmrp_df_elect(route,nbr,metric);

    /*Neighbor on upstream interface RFC 3.4.6.B.3.b*/
    if (route->interface == interface && route->neighbor == nbr)
    {
      if (route->metric < DVMRP_UNREACHABLE)
      {
        route->flags |= DVMRP_RT_DELETE;
        route->flags |= DVMRP_RT_CHANGE;
        route->metric = DVMRP_UNREACHABLE;
        route->received_metric = metric;
        route->dtime = now + DVMRP_GARBAGE_INTERVAL;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d ***dvmrpcb->changed++***\n",
                    __FUNCTION__, __LINE__);
        dvmrpcb->changed++;
        return L7_SUCCESS;
      }
    }
    /* Condition for downstream */
    else if (route->interface != interface)
    {
      /* if this is the first time this neighbor has declared downstream 
       * dependency, see if we have sent any prunes for traffic from the 
       * source network for this route
       */           
      if (!MCAST_BITX_TEST(route->dependents.bits, nbr->index))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                    "%s:%d nbrIndex(%d) not present in Dependents list "
                    "net=%s,nbr=%s \n", __FUNCTION__, __LINE__, nbr->index,
                    dstNetwork, NbrRtr);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                    "%s:%d nbrIndex(%d) intfIndex(%d) for  list net=%s,"
                    "nbr=%s \n", __FUNCTION__, __LINE__, nbr->index, 
                    nbr->interface->index, dstNetwork, NbrRtr);
        memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                      &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                      "%s:There is no cache entry in the table:%d\n",
                      __FUNCTION__, __LINE__);
        }

        while (L7_NULLPTR != entry)
        {
          /* is this entry associated with this route? */
          if (entry->data == route)
          {
            /* have we sent a prune? */
            if (entry->count != 0 || entry->firstPrune != 0)
            {
              /* if source network is reached via an upstream neighbor 
               * (i.e. source is not direct) send a graft 
               */
              if (route->neighbor)
              {
                if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
                {
                  DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Not Sent \n",
                              __FUNCTION__, __LINE__);
                }

              }
            }
          }
          if (entry->data != route)
          {
            memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
            if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                          &entry))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                          "%s:There is no cache entry in the table:%d\n", 
                          __FUNCTION__, __LINE__);
              DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                          "%s:There is no cache entry in the table:%d\n", 
                          __FUNCTION__, __LINE__);
              break;
            }
            continue;
          }
          if (((desg_fwd = dvmrp_get_desg_fwd(route,nbr->interface)) != L7_NULLPTR) && 
              (desg_fwd->status == DVMRP_ENTRY_NOT_USED)&&
              (entry->parent != nbr->interface))
          {
            for (nbrIndex = 0; nbrIndex < DVMRP_MAX_NEIGHBORS; nbrIndex++)
            {
              dvmrp_neighbor_t *tmpNbr = L7_NULLPTR;
              if (MCAST_BITX_TEST (route->dependents.bits, nbrIndex))
              {
                tmpNbr = &dvmrpcb->index2neighbor[nbrIndex];
                if (tmpNbr->status == DVMRP_ENTRY_USED)
                {
                  DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                              "%s:set index=%d the entry->routers "
                              "bitmask:%d\n", __FUNCTION__, 
                              tmpNbr->interface->index, __LINE__);
                  MCAST_BITX_SET(entry->routers, 
                                 tmpNbr->interface->index);
                }
              }
            }
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:set index=%d the entry->routers bitmask:%d\n",
                        __FUNCTION__, nbr->interface->index, __LINE__);
            MCAST_BITX_SET(entry->routers, nbr->interface->index);
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                        "%s:Updating the dvmrp_cache_mfc_update:%d\n",
                        __FUNCTION__, __LINE__);
            dvmrp_cache_mfc_update (entry);
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                        &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:There is no cache entry in the table:%d\n",
                        __FUNCTION__, __LINE__);
            break;
          }
        }
      }
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                  "%s:%d Route Dependents list updated for net=%s,nbr=%s "
                  "on nbrIndex=%d \n", __FUNCTION__, __LINE__, dstNetwork,
                  NbrRtr, nbr->index);
      MCAST_BITX_SET (route->dependents.bits, nbr->index);
    }
  }
  else if (metric > ( 2 * DVMRP_UNREACHABLE))
  {
    /*received metric > 2 * infinity...RFC 3.4.6.B.3*/
    dvmrp_badRts_inc(interface,nbr);
    return L7_FAILURE;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: LEAVING \n", __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  This function is used to process the report. 
*
* @param     dvmrpcb          -  @b{(input)}Pointer to the DVMRP control block .
* @param    no_of_reports  -  @b{(input)}  No of Reports
* 
* @returns  L7_FAILURE/L7_SUCCESS
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_report_process(dvmrp_t *dvmrpcb, L7_uint32 no_of_reports)
{
  dvmrp_report_t *dvmrp_report = L7_NULLPTR;
  L7_int32 n = 0;
  L7_int32 counter = 0;
  L7_uint32 now;
  dvmrp_report_t *aReport=L7_NULLPTR; 

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }



  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  now = osapiUpTimeRaw();
  aReport  = dvmrpcb->report_entries;

  for (dvmrp_report = &aReport[counter];
      counter < no_of_reports;
      dvmrp_report = &aReport[counter])
  {
    n += dvmrp_route_update (&dvmrp_report->srcNetwork, dvmrp_report->metric,
                             dvmrp_report->neighbor, now);
    counter++;
  }
  if (dvmrpcb->changed)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                "%s: calling dvmrp_flash_timer_set function \n",
                __FUNCTION__);
    dvmrp_flash_timer_set (dvmrpcb);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n",__FUNCTION__);
  return(n);
}

/*********************************************************************
* @purpose  This function is used to create the new route entry. 
*
* @aparm     Proto                 -  @b{(input)}DVMRP protocol identifier
* @param     inetAddr            -  @b{(input)}Pointer to the Network address
* @param     interface           -  @b{(input)}Pointer to the interface through which this route 
*                                                  has been recieved.
* @param     metric               -  @b{(input)}metric of this network
* @param     received_metric -  @b{(input)}- recieved metric
* @param     received_metric -  @b{(input)}- recieved metric
* @param     route                 -  @b{(input)}
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void  dvmrp_new_route (L7_int32 proto, 
                       L7_dvmrp_inet_addr_t *inetAddr, 
                       dvmrp_interface_t* interface,
                       dvmrp_neighbor_t *neighbor, 
                       L7_int32 metric, L7_int32 received_metric, dvmrp_route_t  *route )
{
  L7_char8 net[IPV6_DISP_ADDR_LEN];
  dvmrp_t *dvmrpcb;  
  L7_int32  count=0;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : interface is NULL!\n\n");
    return;
  }

  dvmrpcb = interface->global;
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }


  inetAddrHtop(&inetAddr->addr, net);
  memset(route, L7_NULL, sizeof(dvmrp_route_t));

  route->proto = proto;
  memcpy(&route->networkAddr,inetAddr,sizeof(L7_dvmrp_inet_addr_t));
  route->metric = metric & 0x7f;
  route->received_metric = received_metric & 0x7f;
  route->interface = interface;
  route->dtime = 0;
  route->neighbor = neighbor;
  if (neighbor && interface)
  {
    assert (interface == neighbor->interface);
  }
  route->flags |= DVMRP_RT_CHANGE;
  route->entryStatus = DVMRP_ROUTE_ENTRY_USED;  

  for (count=0;count <MAX_INTERFACES;count++)
  {
    route->desg_fwd[count].status = DVMRP_ENTRY_NOT_USED;
  }
  dvmrpcb->changed++;
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d ***dvmrpCB->changed++***\n",__FUNCTION__,__LINE__);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return;
}

