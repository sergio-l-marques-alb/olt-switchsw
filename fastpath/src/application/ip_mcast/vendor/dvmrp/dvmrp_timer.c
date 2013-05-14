/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_timer.c
*
* @purpose Contains definitions to support timers in the DVMRP protocol
*
* @component
*
* @comments
* @create 06/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "dvmrp_timer.h"
#include "dvmrp_snmp.h"
#include "dvmrp_util.h"
#include "dvmrp_mfc.h"
#include "dvmrp_nbr.h"
#include "dvmrp_api.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_protocol.h"
#include "dvmrp_routing.h"
#include "dvmrp_interface.h"
#include "dvmrp_igmp.h"
#include "dvmrp_vend_ctrl.h"
/*********************************************************************
* @purpose  This function is the expiry handler for graft timer. 
*
* @param    pParam    - -  @b{(input)} Void pointer to the timer info.

* @returns  None
*
* @notes    Graft expiry handler.
*       
* @end
*********************************************************************/
void dvmrp_graft_timeout (void *pParam)
{
  L7_uint32 now;
  dvmrp_graft_t *graft = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrp_route_t *route = L7_NULLPTR;
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_int32  handle = (L7_int32)pParam;
  dvmrp_t *dvmrpcb = L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  entry = (dvmrp_cache_entry_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == entry)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Invalid input parameter");
    return ;
  }

  if ((entry->parent == L7_NULLPTR) ||(entry->parent->global ==L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"(entry->parent == NULL) ||(entry->parent->global ==NULL)");
    return ;
  }

  dvmrpcb = entry->parent->global;  


  now = osapiUpTimeRaw();

  graft = entry->graft_sent;

  if (graft  != L7_NULLPTR)
  {
    if (!BIT_TEST (graft->entry->flags, DVMRP_CACHE_DVMRP_GRAFT) ||
        BIT_TEST (graft->entry->flags, DVMRP_CACHE_DELETE))
    {
      appTimerDelete(dvmrpcb->timerHandle, entry->graft_timer);
      if (entry->graftTimeoutHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &entry->graftTimeoutHandle);
      }
      DVMRP_FREE (L7_AF_INET, (void*) graft);        
      entry->graft_sent =L7_NULLPTR;     
      entry->graft_timer=L7_NULLPTR;
      return; 
    }


    route = (dvmrp_route_t *)entry->data;  
    if (route == NULL)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Route Pointer is NULL!!!!!!!!!!!");
      return ;
    }
    nbr = route->neighbor;
    if (nbr != L7_NULLPTR)
    {
      if (dvmrp_graft_send (entry, nbr, 1) != L7_SUCCESS)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nGraft Not Sent \n");
        return;
      }
    }

    /* exponential back-off*/
    graft->holdtime  <<= 1;
    graft->expire = now + graft->holdtime;

    if (graft->holdtime > DVMRP_CACHE_EXPIRE_TIME)
    {
      graft->expire = now;
      BIT_SET(graft->entry->flags, DVMRP_CACHE_DVMRP_GRAFT);
    }

    if (L7_NULLPTR == (entry->graft_timer = appTimerAdd(dvmrpcb->timerHandle,
                                                        dvmrp_graft_timeout,
                                                        (void *)entry->graftTimeoutHandle,
                                                        graft->holdtime,
                                                        "DV-GT2")))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s : %d Timer creation failed\n",__FUNCTION__,__LINE__);
      return;
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is the expiry handler for nbr node. 
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    nbr node is removed.
*       
* @end
*********************************************************************/
void dvmrp_neighbor_timeout (void *pParam)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;     
  L7_int32      handle = (L7_int32)pParam;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  
  nbr = (dvmrp_neighbor_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == nbr)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Invalid input parameter");
    return ;
  }

  if ((nbr->interface== L7_NULLPTR) ||(nbr->interface->global ==L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"(nbr->interface== NULL) ||(nbr->interface->global ==NULL)");
    return ;
  }

  dvmrpcb = nbr->interface->global;
  
  dvmrp_neighbor_cleanup(nbr);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);  

}

/*********************************************************************
* @purpose  This function is the expiry handler for prune timer. 
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    Prune timer expiry handler
*       
* @end
*********************************************************************/
void dvmrp_prune_expire (void *pParam)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  dvmrp_prune_t *prune = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  L7_uint32 min_prune_interval = 1/*DVMRP_PRUNE_TIMEOUT_INTERVAL*/;
  L7_uint32 now;
  L7_int32      handle = (L7_int32)pParam;

  dvmrpcb = (dvmrp_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;  
  }


  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:Prune Timer Expired:%d\n",
              __FUNCTION__,__LINE__); */

  now = osapiUpTimeRaw();
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    /* DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:No Entries in Cache table:%d\n",
                __FUNCTION__,__LINE__); */
  }
  while (L7_NULLPTR != entry)
  {
    if (L7_NULLPTR == (prune=(dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes))))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:No Prunes in Prune List:%d\n",
                  __FUNCTION__,__LINE__);            
      memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:No more Entries in Cache table:%d\n",
                    __FUNCTION__,__LINE__);
        break;
      }
    }
    while (L7_NULLPTR != prune)
    {
      dvmrp_prune_t *pNxtNode = L7_NULLPTR;

      if (L7_NULLPTR == (pNxtNode=(dvmrp_prune_t*)
                         SLLNextGet(&(entry->ll_prunes), (void*)prune)))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:No more Prunes in Prune List:%d\n",
                    __FUNCTION__,__LINE__);            
      }
      if (prune->expire > 0 && prune->expire < now)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                    "%s:set index=%d the entry->routers bitmask:%d\n",
                    __FUNCTION__,prune->neighbor->interface->index,__LINE__);
        MCAST_BITX_SET(entry->routers, prune->neighbor->interface->index);
        dvmrp_cache_mfc_update(entry);
        DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:Updated the MFC CACHE:%d\n",
                    __FUNCTION__,__LINE__);   

        if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                      __FUNCTION__, __LINE__);
          return;
        }
        if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), (L7_sll_member_t *)prune))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:prune Deletion failed:%d\n",
                      __FUNCTION__,__LINE__);              
          DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:prune Deletion failed:%d\n",
                      __FUNCTION__,__LINE__);              
        }
        osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
        /* don't need to send a graft
           since the neighbor should have timed out before this */
      }
      prune = pNxtNode;
    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:No more Entries in Cache table:%d\n",
                  __FUNCTION__,__LINE__);
      break;
    }
  }

  if (L7_NULLPTR == (dvmrpcb->expire = appTimerAdd(dvmrpcb->timerHandle,
                                                   dvmrp_prune_expire,
                                                   (void *)dvmrpcb->pruneExpireHandle,
                                                   min_prune_interval,
                                                   "DV-PT3")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    return;
  }
  /* DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:Prune Timer Added:%d\n",
              __FUNCTION__,__LINE__);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__); */
}

/*********************************************************************
* @purpose  This function is the Restart  Timer handler for route entry.
*
* @param    dvmrpcb    -  @b{(input)}Pointer to the DVMRP control block.
* @param    t               -  @b{(input)}Timer Node.
* @returns  None
*
* @notes    route entry expiry handler
*
* @end
*********************************************************************/
void dvmrpRestartRoutesTimeoutHandler(dvmrp_t *dvmrpcb,L7_uint32 t)
{

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d\n\nRestart dvmrp_routes_timeout every (140sec)\n\n",
              __FUNCTION__,__LINE__);


  if (L7_NULLPTR == (dvmrpcb->age = appTimerAdd(dvmrpcb->timerHandle,
                                                dvmrp_routes_timeout,
                                                (void *)dvmrpcb->routesTimeoutHandle, t,
                                                "DV-RT3")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    return;
  }
}

/*********************************************************************
* @purpose  This function is the expiry handler for route entry. 
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    route entry expiry handler
*       
* @end
*********************************************************************/
void dvmrp_routes_timeout (void *pParam)
{
  L7_uint32 now, t;
  L7_uint32 nexttime = 0;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  dvmrpRouteData_t *pNextData = L7_NULLPTR;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_int32      handle = (L7_int32)pParam;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: ENTERED \n",__FUNCTION__);

  dvmrpcb = (dvmrp_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;  
  }

  now= osapiUpTimeRaw();
  nexttime = now + DVMRP_TIMEOUT_INTERVAL;

  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  /* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, pData);
    route = &pData->dvmrpRouteInfo;
    /* The timeout is not for the default routes */
    if (route->utime != 0)
    {
      /* garbage collect and delete route */
      if (BIT_TEST(route->flags,DVMRP_RT_HOLD))
      {
        if (route->dtime !=0 && route->dtime <= now)
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
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"pData is NULL\n");
          }
          dvmrpcb->radix_entries--;   
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);                    
        }
        else
        {
          if (route->dtime !=0 && route->dtime < nexttime)
          {
            nexttime = route->dtime;
          }
        }
      }
      else if (BIT_TEST (route->flags, DVMRP_RT_DELETE))
      {
        if (route->dtime <= now)
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
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"pData is NULL\n");
          }
          dvmrpcb->radix_entries--;   
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);          
        }
        else
        {
          if ((t = route->dtime)< nexttime)
          {
            nexttime = t;
          }
        }
      }
      /* timeout route -- set metric to 32 and set change flag */
      else if (route->utime > 0 /* timeout is on */ &&
               (now - route->utime) >= DVMRP_TIMEOUT_INTERVAL)
      {
        route->dtime = now + DVMRP_GARBAGE_INTERVAL;
        /* keep metric */
        route->flags |= DVMRP_RT_DELETE;
        route->flags |= DVMRP_RT_CHANGE;

        /* Mark that for deletion in the Cache Table too */
        memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                      &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:There is no cache entry in the table:%d",
                      __FUNCTION__,__LINE__);
        }

        while (L7_NULLPTR != entry)
        {
          if (entry->data != L7_NULLPTR && entry->data == route)
          {
            entry->data = L7_NULLPTR;
            entry->expire = now;
            BIT_SET (entry->flags, DVMRP_CACHE_DELETE);
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, 
                                                        &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:There is no cache entry in the table:%d",
                        __FUNCTION__,__LINE__);
            break;
          }
        }      
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:%d ***dvmrpcb->changed++***\n",__FUNCTION__,__LINE__);
        dvmrpcb->changed++;
        if ((t = route->dtime ) < nexttime)
        {
          nexttime = t;
        }
      }
      /* Added by Kishore to get the nexttimeout time if all else fails */
      else
      {
        if (nexttime > (route->utime + DVMRP_TIMEOUT_INTERVAL ))
        {
          nexttime = DVMRP_TIMEOUT_INTERVAL + route->utime;
        }
      }
    }
    pData = pNextData;             
  }

  if (dvmrpcb->changed)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s: calling dvmrp_flash_timer_set function \n",__FUNCTION__);
    dvmrp_flash_timer_set (dvmrpcb);
  }

  t= nexttime - osapiUpTimeRaw();
  if (t <= 0)
  {
    t = DVMRP_TIMEOUT_INTERVAL;       /* Added by kishore since we have a all elseif above */
  }

  dvmrpRestartRoutesTimeoutHandler(dvmrpcb, t);  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is called when the Cache Remove timer
*           is expired.
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void dvmrp_Cache_Remove (void *pParam)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_int32      handle = (L7_int32)pParam;
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  entry = (dvmrp_cache_entry_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == entry)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Invalid input parameter");
    return;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s: ENTERED \n",__FUNCTION__);



  if ((entry->parent == L7_NULLPTR) ||(entry->parent->global == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s: (entry->parent == L7_NULLPTR) ||(entry->parent->global == NULL)\n",
                __FUNCTION__,__LINE__);
    return;
  }
  dvmrpcb = entry->parent->global;  

  if(entry->prune_retry_timer != L7_NULLPTR)
  {
  if (L7_SUCCESS != appTimerDelete(dvmrpcb ->timerHandle, entry->prune_retry_timer))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s: Failed to delete timer(%d) \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s: Failed to delete timer(%d) \n",
                __FUNCTION__,__LINE__);
  }
    entry->prune_retry_timer = L7_NULLPTR;
  }
  
  if (entry->pruneRetransmitimerHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb ->handle_list,
                         &entry->pruneRetransmitimerHandle);
  }

  dvmrpCacheRemove(dvmrpcb ,entry);
}

/*********************************************************************
* @purpose  This function is called when the periodic route advertiser 
*           is expired.
*            
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    Route reports are sent on all the interfaces
*       
* @end
*********************************************************************/
void dvmrp_timer_update (void *pParam )
{
  L7_int32      handle = (L7_int32)pParam;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__); */

  dvmrpcb = (dvmrp_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return; 
  }
  if (dvmrpcb->flash_update_waiting)
  {
    /* clear flash update */
    dvmrpcb->flash_update_waiting = 0; 
  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: dvmrp_route_advertise is called\n",
              __FUNCTION__);
  dvmrp_route_advertise (L7_TRUE, dvmrpcb);

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d\n\nRestart dvmrp_timer_update every (60sec)\n\n",
              __FUNCTION__,__LINE__);

  if (L7_NULLPTR == (dvmrpcb->timer = appTimerAdd(dvmrpcb->timerHandle,
                                                  dvmrp_timer_update,
                                                  (void *)dvmrpcb->updateTimerHandle,
                                                  DVMRP_UPDATE_INTERVAL,
                                                  "DV-UPD2")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    return;
  }
  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__); */
}

/*********************************************************************
* @purpose  This function is called when the flash timer  is expired. 
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.

* @returns  None
*
* @notes    only the updated routes are sent
*       
* @end
*********************************************************************/
void dvmrp_flash_update (void *pParam)
{
  L7_int32      handle = (L7_int32)pParam;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: ENTERED \n",__FUNCTION__);

  dvmrpcb = (dvmrp_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }



  if (dvmrpcb->flash_update_waiting)
  {
    dvmrpcb->flash_update_waiting = 0;
    dvmrp_route_advertise (L7_FALSE, dvmrpcb);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d dvmrp_route_advertise is called \n",
                __FUNCTION__,__LINE__);
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d dvmrpcb->flash_update_waiting == NULL so not calling"
                "dvmrp_route_advertise\n",__FUNCTION__,__LINE__);
  }

  if(dvmrpcb->flashrUpdateTimerHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb ->handle_list,
                         &dvmrpcb->flashrUpdateTimerHandle);

  }
  
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to set the flash timer. 
*
* @param     dvmrpcb  -  @b{(input)}Pointer to the DVMRP control block .
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_flash_timer_set (dvmrp_t *dvmrpcb)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: ENTERED \n",__FUNCTION__);

  if (dvmrpcb->flash_update_waiting)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s: dvmrpcb->flash_update_waiting is non-NULL so returning \n",
                __FUNCTION__);
    return;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s: starting dvmrp_flash_update will fire in (5sec) \n",__FUNCTION__);

  dvmrpcb->flashrUpdateTimerHandle =
  handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

  if (L7_NULLPTR == (dvmrpcb->flash = appTimerAdd(dvmrpcb->timerHandle,
                                                  dvmrp_flash_update,
                                                  (void *)dvmrpcb->flashrUpdateTimerHandle,
                                                  DVMRP_FLASH_INTERVAL,
                                                  "DV-FT")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Timer creation failed\n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d Timer creation failed\n",
                __FUNCTION__,__LINE__);
    return;
  }
  dvmrpcb->flash_update_waiting++;
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function sends the prune packet once again
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    This function is invoked when a prune retry timer expires.  
*           It will see if the  entry for the prune is still in the cache and
*           is still negative, if so it marks the entry for deletion by 
*           cache_timer_expire
*       
* @end
*********************************************************************/
void dvmrp_prune_retransmit_timer_expire(void * pParam)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_int32  handle = (L7_int32)pParam;
  dvmrp_t           *dvmrpcb = L7_NULLPTR;

  entry= (dvmrp_cache_entry_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == entry)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Invalid input parameter");
    return ;
  }

  if ((entry->parent == L7_NULLPTR) ||(entry->parent->global ==L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"(entry->parent == NULL) ||(entry->parent->global ==NULL)");
    return ;
  }


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  /* if entry still exists and is still negative, delete it */
  if (BIT_TEST(entry->flags, DVMRP_CACHE_NEGATIVE))
  {
    L7_uint32 now; 
    L7_uint32 expiryTime;
   entry->flags |= DVMRP_CACHE_DELETE;
    /*
     ** if the prune lifetime has expired, reset the back-off mechanism by 
     setting entry->count and entry->firstPrune to initial value, 
     else increment entry->count
     */
    if ((entry->firstPrune + entry->lifetime) <= osapiUpTimeRaw())
    {
      entry->count = 0;
      entry->firstPrune = 0;
    }
    if (L7_SUCCESS != dvmrpCacheMfcDelete(&entry->source, &entry->group))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Unable to post the MFC DEL ENTRY to MFC:%d\n",
                  __FUNCTION__,__LINE__);
    }
    dvmrpcb = entry->parent->global; 
    now = osapiUpTimeRaw();
    expiryTime = entry->expire - now;
    if(entry->cacheRemove_timer == L7_NULL)
    {
       entry->cacheRemoveHandle =
           handleListNodeStore(dvmrpcb->handle_list, (void*)entry);

    if (L7_NULLPTR == (entry->cacheRemove_timer =
                           appTimerAdd(dvmrpcb->timerHandle,
                                       dvmrp_Cache_Remove,
                                       (void *)entry->cacheRemoveHandle,
                                       expiryTime,
                                       "DV-CT")))
    {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:%d timer instantiation failed \n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,
                      "%s:%d timer instantiation failed \n",
                      __FUNCTION__,__LINE__);
    }
    }

    entry->count++;

  }
  entry->prune_retry_timer = L7_NULLPTR;
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used as a call back function in the Timer Init function 
* @param    timerCtrlBlk -@b{(input)} Timer control block
* @param    pParam       -@b{(input)} Timer info
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrpTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void *pParam)
{
  dvmrp_t       *dvmrpCb = L7_NULLPTR;

  dvmrpCb = (dvmrp_t *)pParam;
  if (dvmrpCb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\ndvmrpcb Pointer is NULL.\n\n");
    return;

  }
  /* DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s: ENTERED \n",__FUNCTION__); */
 
  if (dvmrpMapMessageQueueSend(dvmrpCb->family, MCAST_DVMRP_TIMER_EXPIRY_EVENT,
                               sizeof(L7_APP_TMR_CTRL_BLK_t),
                               timerCtrlBlk) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "MessageQueue Send Failure.\n");
    return;
  }
  /* DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s: Timer Expiry event sent \n",__FUNCTION__); */
}


