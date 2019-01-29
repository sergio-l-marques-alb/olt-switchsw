/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_nbr.c
*
* @purpose Contains neighbor routines to support the DVMRP protocol
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
#include "dvmrp_nbr.h"
#include "buff_api.h"
#include "dvmrp_main.h"
#include "dvmrp_timer.h"
#include "dvmrp_snmp.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_mfc.h"
#include "dvmrp_igmp.h"
#include "dvmrp_protocol.h"
#include "dvmrp_util.h"

/*********************************************************************
* @purpose  This function processes the DVMRP ask neighbours packet
*
* @param    from_if -  @b{(input)} Pointer to the Interface throufh which the ask neighbours 
*                                    packet has been recieved.  
* @param    from    -  @b{(input)}Pointer to the source address of the packet
* 
* @returns  None.
*
* @notes    This function sends the response packet to the sender 
*           of the request packet
* @end
*********************************************************************/
L7_RC_t dvmrp_neighbors2 (dvmrp_interface_t *from_if, L7_dvmrp_inet_addr_t *from)
{
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_t *dvmrpcb; 
  L7_uint32 s_addr;
  L7_uint32 uiIntf = 0;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = from_if->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");            
    return L7_FAILURE;
  }

  for (uiIntf =0;uiIntf < MAX_INTERFACES;uiIntf++)
  {
    L7_ulong32 flags;
    L7_uchar8 *num_p = L7_NULLPTR;
    dvmrp_neighbor_t *nbr;
    L7_dvmrp_inet_addr_t *inetAddr;

    interface = &dvmrpcb->dvmrp_interfaces[uiIntf];

    if (interface->index != uiIntf)
    {
      break;
    }

    flags = interface->flags;
    if (interface->igmp_querier == MCAST_EVENT_MGMD_QUERIER)
    {
      flags |= DVMRP_VIF_QUERIER;
    }
    if (!BIT_TEST (interface->flags, DVMRP_VIF_UP))
    {
      flags |= DVMRP_VIF_DOWN;
    }
    inetAddr = &interface->primary.inetAddr;
    assert (inetAddr);

    for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
        nbr != L7_NULLPTR;
        nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),
                                            (void*)nbr))
    {
      if (cp - sendbuf > DVMRP_MAX_DVMRP_DATA_LEN - 12)
      {
        dvmrp_send (DVMRP_NEIGHBORS2, from,
                    sendbuf, cp - sendbuf, from_if);
        cp = sendbuf;
        num_p = L7_NULLPTR;
      }
      if (num_p == L7_NULLPTR)
      {
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &inetAddr->addr, &s_addr))
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't get the IP ADDRESS\n\n");     
          return L7_FAILURE;
        }
        MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
        MCAST_PUT_BYTE (interface->metric_in, cp);
        MCAST_PUT_BYTE (interface->threshold, cp);
        MCAST_PUT_BYTE (flags, cp);
        num_p = cp;
        MCAST_PUT_BYTE (0, cp); /* number of neighbors */
      }
      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &nbr->nbrAddr.addr, 
                                       &s_addr))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't get the IP ADDRESS\n\n");     
        return L7_FAILURE;
      }
      MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
      (*num_p)++;
    } 

  }
  if (cp - sendbuf > 0)
  {
    dvmrp_send(DVMRP_NEIGHBORS2, from, sendbuf, cp - sendbuf, from_if);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to delete the nbr node. 
*
* @param    nbr - -  @b{(input)}Pointer to the nbr node.

* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_neighbor_delete(L7_sll_member_t *pNode)
{
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = (dvmrp_neighbor_t *)pNode;
  L7_uint32 nbr_index;   

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:dvmrp_neighbor_delete is called:%d\n",
              __FUNCTION__,__LINE__);
  dvmrpcb  = nbr->interface->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  nbr_index = nbr->index;
  memset(nbr,0,sizeof(dvmrp_neighbor_t ));
  dvmrpcb ->index2neighbor[nbr_index].status  =DVMRP_ENTRY_NOT_USED;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to get the nbr node based on the 
*           source address. 
*
* @param    interface            -  @b{(input)} Interfae on which the neighbor node 
*                                                     is searched.
* @param    versionSupport   -  @b{(input)} Pointer to the version supported. 
* @param    source               -  @b{(input)} source address  
*
* @returns  Neighbour node
*
* @notes    None
*       
* @end
*********************************************************************/
dvmrp_neighbor_t *dvmrp_neighbor_lookup(dvmrp_interface_t *interface,
                                        L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source)
{
  dvmrp_neighbor_t nbrNode, *nbr = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if (interface == L7_NULLPTR)
  {
    return(nbr);
  }

  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, &source->addr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING\n",__FUNCTION__);
  return(nbr);
}

/*********************************************************************
* @purpose  This function is used to add the new neighbour to the neighbour 
*           list. 
*
* @param    interface            - @b{(input)} Pointer to the Interfae for which the 
*                                                          new neighbour is added.
* @param    versionSupport   - @b{(input)} versionSupport supported by 
*                                                    neighbour.
* @param    genid                -  @b{(input)} Generation id 
* @param    source              -  @b{(input)} source address of the neighbour

* @returns  Neighbour node
*
* @notes    None
*       
* @end
*********************************************************************/
dvmrp_neighbor_t *dvmrp_neighbor_register(dvmrp_interface_t *interface, 
                                          L7_ulong32 versionSupport, L7_ulong32 genid, 
                                          L7_dvmrp_inet_addr_t *source)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_int32 i;
  L7_int32 expire_interval = DVMRP_NEIGHBOR_EXPIRE_TIME;
  L7_int32 major = versionSupport & 0xff;
  L7_int32 minor = (versionSupport >> 8) & 0xff;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if (interface == L7_NULLPTR)
  {
    return(nbr);
  }

  dvmrpcb = interface->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_NULLPTR;
  }
  /* zero is reserved for now */
  for (i = 1; i < DVMRP_MAX_NEIGHBORS; i++)
  {
    if (dvmrpcb->index2neighbor[i].status == DVMRP_ENTRY_NOT_USED)
    {
      break;
    }
  }
  if (i >= DVMRP_MAX_NEIGHBORS)
  {
    return(L7_NULLPTR);
  }

  nbr =&dvmrpcb->index2neighbor[i];
  memset(nbr, L7_NULL, sizeof(dvmrp_neighbor_t)); 

  memcpy(&nbr->nbrAddr,source,sizeof(L7_dvmrp_inet_addr_t));
  nbr->interface = interface;
  nbr->versionSupport = versionSupport;
  nbr->index = i;
  nbr->genid = genid;
  nbr->badPkts = 0;           /*  MIB */
  nbr->badRts = 0;            /*  MIB */
  nbr->status = DVMRP_ENTRY_USED;
  /* special neighbor */
  if (genid != 0)
  {
    if (interface->nbr_count++ <= 0)
    {
      interface->flags &= ~DVMRP_VIF_LEAF;
    }

    nbr->ctime = osapiUpTimeRaw();
    nbr->utime = nbr->ctime; /*SSV*/
    if (major == 10 || major == 11 || major < 3 || (major == 3 &&minor < 5))
    {
      expire_interval = DVMRP_OLD_VER_NEIGHBOR_EXPIRE_TIME;
    }

    nbr->nbrTimeoutHandle =
    handleListNodeStore(dvmrpcb->handle_list, (void*)nbr);

    nbr->timeout = appTimerAdd(dvmrpcb->timerHandle, dvmrp_neighbor_timeout, 
                               (void *)nbr->nbrTimeoutHandle, expire_interval,
                               "DV-NLT");
    if (L7_NULLPTR == nbr->timeout)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s : %d Timer creation failed\n",
                  __FUNCTION__,__LINE__);
      return L7_NULLPTR;
    }
  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                __FUNCTION__, __LINE__);
    return L7_NULLPTR;
  }
  if (L7_SUCCESS != SLLAdd(&(interface->ll_neighbors), (L7_sll_member_t *)nbr))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s : %d Node addition to sLL failed\n",
                __FUNCTION__,__LINE__);
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return(L7_NULLPTR);
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING\n",__FUNCTION__);
  return(nbr);
}

/*********************************************************************
* @purpose  This function is used to recover the neighbour which has 
*           been set to be deleted. 
*
* @param    interface -  @b{(input)}Pointer to the interfae for which the new neighbour is 
*                                    added.
* @param    nbr      -  @b{(input)}Pointer to the neighbour node
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_neighbor_recover (dvmrp_interface_t *interface, 
                             dvmrp_neighbor_t *nbr)
{
  L7_int32 expire_interval = DVMRP_NEIGHBOR_EXPIRE_TIME;
  L7_int32 major = nbr->versionSupport & 0xff;
  L7_int32 minor = (nbr->versionSupport >> 8) & 0xff;
  L7_int32 timeLeft = 0;
  dvmrp_t *dvmrpcb = L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");            
    return;
  }

  if (major == 10 || major == 11 || major < 3 || (major == 3 &&minor < 5))
  {
    expire_interval = DVMRP_OLD_VER_NEIGHBOR_EXPIRE_TIME;
  }

  nbr->ctime = osapiUpTimeRaw();

  BIT_RESET (nbr->flags, DVMRP_NEIGHBOR_DELETE);
  if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, nbr->timeout, 
                                        &timeLeft))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Failed to get the time left\n", 
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d Failed to get the time left\n",
                __FUNCTION__,__LINE__);
  }
  if (timeLeft >= L7_NULL)
  {
    appTimerDelete(dvmrpcb->timerHandle, nbr->timeout);
    if (nbr->nbrTimeoutHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &nbr->nbrTimeoutHandle);
    }
    nbr->timeout=L7_NULLPTR;
  }
  nbr->nbrTimeoutHandle =
  handleListNodeStore(dvmrpcb->handle_list, (void*)nbr);

  if (L7_NULLPTR == (nbr->timeout = appTimerAdd(dvmrpcb->timerHandle, 
                                                dvmrp_neighbor_timeout, (void *)nbr->nbrTimeoutHandle,
                                                expire_interval,
                                                "DV-NLT2")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    return;
  }

  if (interface->nbr_count++ <= 0)
  {
    interface->flags &= ~DVMRP_VIF_LEAF;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING\n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to know whether this is the only 
*           dependent neighbour
*
* @param    bitset  -  @b{(input)} Pointer to the neighbour bit set
*                 nbr     -  @b{(input)} Pointer to the nbr node to be verified
*           
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_is_only_neighbor_dependent(neighbor_bitset_t *bitset, 
                                          dvmrp_neighbor_t *nbr)
{
  L7_int32 i;
  dvmrp_t *dvmrpcb=L7_NULLPTR; 

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((nbr->interface == L7_NULLPTR)||(nbr->interface->global == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  dvmrpcb = nbr->interface->global;
  for (i=1;i< DVMRP_MAX_NEIGHBORS;i++)
  {
    if ((MCAST_BITX_TEST(bitset->bits,i)) && 
        (dvmrpcb->index2neighbor[i]).status == DVMRP_ENTRY_USED&&
        (i != nbr->index) && 
        (dvmrpcb->index2neighbor[i].interface == nbr->interface))
    {
      return L7_FAILURE;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING\n",__FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to know whether any other dependent 
*           neighbours on this interface
*
* @param    bitset    -   @b{(input)} neighbour bit set
* @param    interface -   @b{(input)}Pointer to the interface for which the dependent 
*                            neighbours to be verified
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_any_dependents_on_this_interface(neighbor_bitset_t *bitset,
                                                dvmrp_interface_t *interface)
{
  L7_int32 i;
  dvmrp_t *dvmrpcb=L7_NULLPTR; 

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_ERROR;
  }

  for (i=1;i< DVMRP_MAX_NEIGHBORS;i++)
  {
    if ((dvmrpcb->index2neighbor[i].status == DVMRP_ENTRY_USED )&& 
        (dvmrpcb->index2neighbor[i].interface == interface)&&
        (MCAST_BITX_TEST(bitset->bits,i)))
    {
      return L7_SUCCESS;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING\n",__FUNCTION__);
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  This function does the cleanup action for nbr node. 
*
* @param    nbr    -   @b{(input)} pointer to the neighbor node.
* 
* @returns  None
*
* @notes    nbr node is removed.
*       
* @end
*********************************************************************/
void dvmrp_neighbor_cleanup (dvmrp_neighbor_t *nbr)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_uint32 now;
  L7_int32 timeLeft = L7_NULL;
  dvmrp_route_t *route = L7_NULLPTR ;
  dvmrp_cache_entry_t mfentry, tmpEntry, *entry = L7_NULLPTR,*entryNext = L7_NULLPTR;
  dvmrp_graft_t *graft = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  dvmrpRouteData_t *pData = L7_NULLPTR;
  desg_fwd_t *desg_fwd=L7_NULLPTR;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);


  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d NBR pointer is NULL \n",__FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s%d:%d NBR pointer is NULL \n",__FUNCTION__,__LINE__);
    return;
  }

  dvmrpcb = nbr->interface->global;

  /* since timeout event is queued, it may happen after deletion is done.
     so the info remains and reused later */
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d NBR flag is set to DVMRP_NEIGHBOR_DELETE so returning \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d NBR flag is set to DVMRP_NEIGHBOR_DELETE so returning \n",
                __FUNCTION__,__LINE__);
    return;
  }
  interface = nbr->interface;
  assert (interface);

  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d INTF pointer is set to DVMRP_NEIGHBOR_DELETE so returning \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d INTF pointer is set to DVMRP_NEIGHBOR_DELETE so returning \n",
                __FUNCTION__,__LINE__);
    return;
  }


  now = osapiUpTimeRaw();

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d NBR flag/state being set to DVMRP_NEIGHBOR_DELETE/"
              "L7_DVMRP_NEIGHBOR_STATE_DOWN \n",__FUNCTION__,__LINE__);
  BIT_SET (nbr->flags, DVMRP_NEIGHBOR_DELETE);
  nbr->state = L7_DVMRP_NEIGHBOR_STATE_DOWN;

  if (--interface->nbr_count <= 0)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d INTF flag is set to DVMRP_VIF_LEAF,ifindex=%d \n",
                __FUNCTION__,__LINE__,interface->index);
    interface->flags |= DVMRP_VIF_LEAF;
  }

  /* If this is really a timeout, send a trap */
  if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, nbr->timeout, 
                                        &timeLeft))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Failed to get the time left for expiry:%d",__FUNCTION__,__LINE__);
  }

  if (timeLeft <= L7_NULL)
  {
    if (L7_SUCCESS != dvmrp_send_neighbor_loss_trap(interface, nbr,
                                                    L7_DVMRP_NEIGHBOR_STATE_DOWN))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                  __FUNCTION__, __LINE__);
    }
  }

  /* Any outstanding graft will be cleaned to...*/
  memset(&mfentry, L7_NULL, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:No cache entry in the table",__FUNCTION__);
  }

  while (entry != L7_NULLPTR)
  {
    if (entry->graft_sent)
    {
      graft =entry->graft_sent;
      if(graft->neighbor == nbr)
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

    }

    memcpy(&mfentry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:No cache entry in the table",__FUNCTION__);
    }
  }
  /* 1. holdown all the routes recevied from the neighbor */
  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                "%s:%d Failed to find the First Radix Entry so returning \n",
                __FUNCTION__,__LINE__);
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
    /*  Takes care of routes whose upstream neighbor is timed out */
    if ((route->neighbor != L7_NULLPTR) && 
        (L7_INET_ADDR_COMPARE(&route->neighbor->nbrAddr.addr, &nbr->nbrAddr.addr)
         == 0) &&
        (route->flags!=DVMRP_RT_DELETE))
    {
      route->flags |= DVMRP_RT_HOLD;
      route->dtime = now + DVMRP_ROUTE_HOLD_TIME;
      /* delete the router after 120 seconds */
      if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, dvmrpcb->age, 
                                            &timeLeft))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Failed to get the time left for expiry:%d\n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:Failed to get the time left for expiry:%d\n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:%d\n\nStarted dvmrp_routes_timeout every (120sec)\n\n",
                    __FUNCTION__,__LINE__);
        dvmrpcb->routesTimeoutHandle =
        handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

        if (L7_NULLPTR == (dvmrpcb->age = appTimerAdd(dvmrpcb->timerHandle,
                                                      dvmrp_routes_timeout,
                                                      (void *)dvmrpcb->routesTimeoutHandle,
                                                      DVMRP_ROUTE_HOLD_TIME,
                                                      "DV-RT2")))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:%d timer instantiation failed returning\n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                      "%s:%d timer instantiation failed returning\n", 
                      __FUNCTION__,__LINE__);
          return;
        }
      }

      if (timeLeft > DVMRP_ROUTE_HOLD_TIME)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, 
                    "%s:TimeLeft is > DVMRP_ROUTE_HOLD_TIME(120):%d",
                    __FUNCTION__,__LINE__);

        appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->age);
        if (dvmrpcb->routesTimeoutHandle != L7_NULL)
        {
          handleListNodeDelete(dvmrpcb->handle_list,
                               &dvmrpcb->routesTimeoutHandle);
        }
        dvmrpcb->age=L7_NULL;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:%d\n\nStarted dvmrp_routes_timeout every (120sec)\n\n",
                    __FUNCTION__,__LINE__);
        dvmrpcb->routesTimeoutHandle =
        handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

        if (L7_NULLPTR == (dvmrpcb->age = appTimerAdd(dvmrpcb->timerHandle,
                                                      dvmrp_routes_timeout,
                                                      (void *)dvmrpcb->routesTimeoutHandle,
                                                      DVMRP_ROUTE_HOLD_TIME,
                                                      "DV-RT3")))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:%d timer instantiation failed \n", __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                      "%s:%d timer instantiation failed \n", __FUNCTION__,__LINE__);
          return;
        }
      }
      /* flush cache entries if it is upstream neighbor*/
      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:There is no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:There is no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
      }

      while (L7_NULLPTR != entry)
      {
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,entry, &entryNext))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:No more cache entries in the table:%d\n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                      "%s:No more cache entries in the table:%d\n",
                      __FUNCTION__,__LINE__);
        }

        if (route == (dvmrp_route_t *) entry->data)
        {
          /* neighbor is upstream...*/
          if (entry->parent == nbr->interface)
          {
            if (L7_SUCCESS != dvmrpCacheTableCacheDelete(dvmrpcb,&entry->source, &entry->group))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                          "%s:Unable to delete cache entry from cache:%d\n",
                          __FUNCTION__,__LINE__);
            }

          }
        }
        entry =entryNext;
      }
    }
    /* To take care routes whose downstream nbr is timed out */
    else if (MCAST_BITX_TEST(route->dependents.bits, nbr->index))
    {
      dvmrp_prune_t *prune = L7_NULLPTR;
      L7_int32 no_dependents_left = 0;
      /*
       * check if the neighbor we are losing is the only one that depended 
       * on us for this route on this interface... if so, remove this 
       * interface from the dependents bitset
       */
      MCAST_BITX_RESET(route->dependents.bits, nbr->index);

      if (dvmrp_is_only_neighbor_dependent(&route->dependents, nbr) == 
          L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:Nbr dependents list is reset with index=%d:%d\n",
                    __FUNCTION__,nbr->index,__LINE__);
        no_dependents_left = 1;        

      }
      /*
       * go through the cache table and update entries based on this neighbor
       *  disappearing
       */
      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:There is no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                    "%s:There is no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
      }
      while (L7_NULLPTR != entry)
      {
        /*
         * if neighbor was the last dependent and there are not group 
         * members on i'face, clear children bit for this interface
         */
        if (no_dependents_left)
        {
          if (route == (dvmrp_route_t *)entry->data)
          {
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                        "%s:clear index=%d the entry->routers bitmask:%d\n",
                        __FUNCTION__,nbr->interface->index,__LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                        "%s:clear index=%d the entry->routers bitmask:%d\n",
                        __FUNCTION__,nbr->interface->index,__LINE__);
            MCAST_BITX_RESET(entry->routers, nbr->interface->index);
            if (dvmrpMemberShipTest(dvmrpcb,&entry->group, 
                                    &entry->source, 
                                    nbr->interface->index) != L7_SUCCESS)
            {
              DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                          "%s:clear index=%d the entry->children bitmask:%d\n",
                          __FUNCTION__,nbr->interface->index,__LINE__);
              MCAST_BITX_RESET(entry->children, nbr->interface->index);
              if (route->neighbor && BIT_TEST(entry->flags, 
                                              DVMRP_CACHE_NEGATIVE))
              {
                DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                            "%s:sending the prune to neighbor :%d\n",
                            __FUNCTION__,__LINE__);
                MCAST_BITX_SET (entry->pruneSent, 
                                route->neighbor->interface->index);
                if (dvmrp_prune_send(route->neighbor->interface,
                                     &entry->source, 
                                     &entry->group, 
                                     DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                     route->neighbor) != L7_SUCCESS)
                {
                  L7_uchar8 nbr[IPV6_DISP_ADDR_LEN];
                  L7_uchar8 src[IPV6_DISP_ADDR_LEN];
                  L7_uchar8 grp[IPV6_DISP_ADDR_LEN];

                  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                          "DVMRP Prune Control message Send Failed; rtrIfNum - %d,"
                           " Neighbor - %s, SrcAddr - %s, GrpAddr - %s"
                           " DVMRP Prune control message send failed."
                           " This could mostly be because of a Failure"
                           " return status of the socket call sendto()."
                           " As a result of this, the unwanted multicast"
                           " traffic is still received and forwarded.",
                           route->neighbor->interface->index,
                           inetAddrPrint(&route->neighbor->nbrAddr.addr, nbr),
                           inetAddrPrint(&entry->source, src),
                           inetAddrPrint(&entry->group, grp));
                  return;
                }
              }
            }
            DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:Updating the MFC CACHE :%d\n",
                        __FUNCTION__,__LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:Updating the MFC CACHE :%d\n",
                        __FUNCTION__,__LINE__);
            dvmrp_cache_mfc_update(entry);                       
          }
        }

        /*
         *  Remove any prunes in the entry received from this neighbor
         */
        if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                      __FUNCTION__, __LINE__);
          return;
        }
        if (L7_NULLPTR != (prune= (dvmrp_prune_t*)
                           SLLFirstGet(&(entry->ll_prunes))))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:There is no prune entry in the table:%d\n",
                      __FUNCTION__,__LINE__);            
        }
        while (L7_NULLPTR != prune)
        {
          dvmrp_prune_t *pNxtPrune = L7_NULLPTR;

          if (L7_NULLPTR == (pNxtPrune=(dvmrp_prune_t*)
                             SLLNextGet(&(entry->ll_prunes), (void*)prune)))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                        "%s:There is no prune entry in the table:%d\n",
                        __FUNCTION__,__LINE__);
          }
          if ((prune->neighbor == nbr))
          {
            if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), 
                                        (L7_sll_member_t *)prune))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                          "%s:%d Deletion failure\n",__FUNCTION__,__LINE__);
            }
          }
          prune = pNxtPrune;
        }  
        osapiWriteLockGive(dvmrpcb->dvmrpRwLock);

        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:There is no cache entry in the table:%d\n",
                      __FUNCTION__,__LINE__);
          break;
        }
      }
    }
    /* Draft 3.2.4.2 New DF election to take care of peer neighbors */
    else if (((desg_fwd = dvmrp_get_desg_fwd(route,nbr->interface))!= L7_NULLPTR)&&
             (desg_fwd->status ==DVMRP_ENTRY_USED))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
                  "%s:New DF election to take care of peer nbrs:%d\n",
                  __FUNCTION__,__LINE__);
      if (desg_fwd->fwd_nbr == nbr)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:Calling the dvmrp_df_elect:%d\n",
                    __FUNCTION__,__LINE__);
        dvmrp_df_elect(route,nbr,DVMRP_UNREACHABLE);
      }
    }
    pData = pNextData;     
  }

  if (nbr->nbrTimeoutHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb->handle_list,
                         &nbr->nbrTimeoutHandle);
  }
  
  nbr->timeout =L7_NULLPTR;
  

  
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}


