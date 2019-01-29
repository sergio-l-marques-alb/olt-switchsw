/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_interface.c
*
* @purpose  Contains interface related definitions to support the DVMRP 
*           protocol
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
#include "dvmrp_interface.h"
#include "dvmrp_routing.h"
#include "dvmrp_protocol.h"
#include "dvmrp_timer.h"
#include "dvmrp_mfc.h"
#include "dvmrp_util.h"
#include "dvmrp_nbr.h"
#include "dvmrp_api.h"

/*********************************************************************
* @purpose  This function is used to set metric for a given interface
*
* @param    dvmrpCbHandle  -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMetric    -  @b{(input)} dvmrp Metic.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_interface_metric_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                   dvmrp_metric_t *dvmrpMetric)
{
  dvmrp_t            *dvmrpCb = (dvmrp_t *) dvmrpCbHandle;
  dvmrp_interface_t  *interface = L7_NULLPTR;
  L7_uint32          rtrIfNum  = dvmrpMetric->rtrIfNum;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
              "%s :DVMRP_MAP_METRIC_SET rcvd\n\n",
              __FUNCTION__);
  if (dvmrpCb->dvmrpOperFlag != L7_ENABLE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
    return L7_FAILURE;
  }

  if (rtrIfNum >= MAX_INTERFACES)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nWrong Interface Number.\n\n");
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dvmrpCb->dvmrpRwLock, 
                         L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCb->dvmrp_interfaces[rtrIfNum ];

  if (dvmrpMetric->rtrIfNum == interface->index)
  {
    interface->metric_in = dvmrpMetric->metric;
  }

  osapiWriteLockGive(dvmrpCb->dvmrpRwLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to enable/disable interface admin mode.
*
* @param    dvmrpCbHandle  -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMode      -  @b{(input)} dvmrp  Mode.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_interface_adminmode_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                      dvmrp_adminmode_t *dvmrpAdminMode)
{
  dvmrp_t            *dvmrpCb = (dvmrp_t *) dvmrpCbHandle;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
              "%s :DVMRP_MAP_INTF_ADMINMODE_SET rcvd\n\n",
              __FUNCTION__);

  if (dvmrpCb->dvmrpOperFlag != L7_ENABLE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
    return L7_FAILURE;
  }
  return dvmrp_interface_activate(dvmrpCb, dvmrpAdminMode->rtrIfNum, 
                                  dvmrpAdminMode->mode,
                                  dvmrpAdminMode->ipAddr,
                                  dvmrpAdminMode->netMask,
                                  dvmrpAdminMode->intfMetric);
}

/*********************************************************************
* @purpose  This function is used to Enable/Disable DVMRP  a given interface
*
* @param    dvmrpcb  -  @b{(input)} Pointer to the  DVMRP control block
* @param    index    -  @b{(input)}  on which the protocol needs to be enabled
* @param    on       -  @b{(input)}  status of the interface-ENABLE or DISABLE
* @param    ipAddr   -  @b{(input)}  Ip address of the interface
* @param    netMask  -  @b{(input)}  ip Mask of the interface
* 
* @returns  None 
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_interface_activate (dvmrp_t *dvmrpcb, 
                                         L7_int32 index, 
                                         L7_int32 on, 
                                         L7_inet_addr_t *ipAddr, 
                                         L7_inet_addr_t *netMask,
                                         L7_uint32 intfMetric)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_uchar8 prefixlen;
  /*L7_char8 intfAddr[DVMRP_STRING_SIZE];*/
  dvmrp_neighbor_t *nbr = L7_NULLPTR, *nbrNext = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (on != L7_DISABLE)
  {
    if (MCAST_BITX_TEST(dvmrpcb->interface_mask, index))
    {
      return(L7_SUCCESS);
    }
  }
  else
  {
    if (!MCAST_BITX_TEST(dvmrpcb->interface_mask, index))
    {
      return(L7_SUCCESS);
    }
  }
  if (on != L7_DISABLE)
  {
    if (L7_FAILURE== inetMaskToMaskLen(netMask, &prefixlen))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Couldn't compute mask length\n");
      return L7_FAILURE;
    }

    MCAST_BITX_SET (dvmrpcb->interface_mask, index);
    interface =&dvmrpcb->dvmrp_interfaces[index];     
    memset(interface, L7_NULL, sizeof(dvmrp_interface_t));

    interface->index = index;
    if (SLLCreate (L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER, 
                   L7_NULL, dvmrp_neighbor_comp,
                   dvmrp_neighbor_delete, &(interface->ll_neighbors))  != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s : %d sLL creation failed \n",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    interface->genid = osapiUpTimeRaw();  /*  Intf Genid */
    interface->flags = 0;
    interface->global=dvmrpcb;
    inetCopy(&interface->primary.inetAddr.addr, ipAddr);
    /*
    if (inetAddrHtop(&interface->primary.inetAddr.addr, intfAddr) != L7_SUCCESS)
    {
      LOG_MSG("\nInetAddress to ASCII conversion Failed.\n");
      return L7_FAILURE;
    }
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,"\n %s: intfAddr=%s ", 
                __FUNCTION__,intfAddr);
    */
    interface->primary.inetAddr.maskLength=prefixlen;

    BIT_SET(interface->flags, DVMRP_VIF_LEAF);
    BIT_SET(interface->flags, DVMRP_VIF_UP);

    interface->metric_in = intfMetric;
    interface->metric_out = 0;
    interface->badPkts = 0;           /*  MIB */
    interface->badRts = 0;            /*  MIB */
    interface->sentRts = 0;           /*  MIB */
    interface->dlist_in = -1;
    interface->dlist_out = -1;


    dvmrp_direct_route_inject (interface);
    if (dvmrp_probe_send (interface, L7_NULLPTR) != L7_SUCCESS)
    {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
               "DVMRP Probe Control message Send Failed on rtrIfNum - %d"
               "DVMRP Probe control message send failed."
               " This could mostly be because of a Failure"
               " return status of the socket call sendto()."
               " As a result of this, the DVMRP neighborship"
               " could be lost in the neighboring DVMRP routers.", index);
    }

    interface->flags |= DVMRP_VIF_LEAF;

    interface->timerBlock.data1 = index;
    interface->timerBlock.data3 = L7_NULLPTR;
    interface->timerBlock.data4 = dvmrpcb;    
    interface->probePeriodicTimerHandle =
    handleListNodeStore(dvmrpcb->handle_list, (void*)&interface->timerBlock);

    if (L7_NULLPTR == (interface->probe = appTimerAdd(dvmrpcb->timerHandle,
                                                      dvmrp_probe_sendHandler,
                                                      (void *)interface->probePeriodicTimerHandle,
                                                      DVMRP_NEIGHBOR_PROBE_INTERVAL,
                                                      "DV-PRB")))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s : %d Timer creation failed\n",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }

    /* Update the igmp memberships of this interface onto the cache */
    dvmrp_cache_update_interface_up(interface);

    /* Post an event to MGMD to get the Group membership details for
     * this interface.
     */
    if (mgmdMapMRPGroupInfoGet (dvmrpcb->family, index) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "%s - %d: Failure in Posting the MGMD "
                   "Info Get Event for rtrIfNum - %d", __FUNCTION__, __LINE__, index);
      return L7_FAILURE;
    }
    if (mcastMapMRPAdminScopeInfoGet(dvmrpcb->family, index) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "%s - %d: Failure in Posting the Admin "
                   "scope Get Event for rtrIfNum - %d", __FUNCTION__, __LINE__, index);
    }

  }
  else
  {
    /* DDEVI*/
    interface = &dvmrpcb->dvmrp_interfaces[index];
    dvmrp_graceful_shutdown(interface);
    dvmrp_cache_update_interface_down(interface);
    dvmrpUpdateMfcInterfaceDown(interface);     
    dvmrp_direct_route_delete(interface);

    appTimerDelete(dvmrpcb->timerHandle, interface->probe);
    if (interface->probePeriodicTimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &interface->probePeriodicTimerHandle);
    }
    interface->probe=L7_NULLPTR;

    for (nbr = (dvmrp_neighbor_t *)SLLFirstGet(&(interface->ll_neighbors));
        nbr != L7_NULLPTR; nbr = nbrNext)
    {
      nbrNext = (dvmrp_neighbor_t *)SLLNextGet(&(interface->ll_neighbors),(void*)nbr);
      if (nbr->timeout != L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:dvmrp_neighbor_timeout is called:%d\n",
                    __FUNCTION__,__LINE__);
        dvmrp_neighbor_cleanup(nbr);

        appTimerDelete(dvmrpcb ->timerHandle, nbr->timeout);
        if (nbr->nbrTimeoutHandle != L7_NULL)
        {
          handleListNodeDelete(dvmrpcb ->handle_list,
                               &nbr->nbrTimeoutHandle);
        }
        nbr->timeout=L7_NULLPTR;
      }
      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      SLLDelete(&(interface->ll_neighbors), (L7_sll_member_t *)nbr);
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    }

    memset(interface, L7_NULL, sizeof(dvmrp_interface_t));
    interface->index = DVMRP_INVALID_INDEX;

    MCAST_BITX_RESET(dvmrpcb->interface_mask, index);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function is used to apply the policy on the incoming 
*           interface. 
*
* @param    inetAddr   - @b{(input)}  Pointer to the Network address.
* @param    metric      - @b{(input)}  Cost of the metric
* @param    interface  - @b{(input)} Pointer to the interface
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_policy_in (L7_inet_addr_t * inetAddr, 
                          L7_int32 metric, 
                          dvmrp_interface_t *interface)
{
  L7_int32 adjusted_metric = -1;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (interface == L7_NULLPTR)
  {
    return(adjusted_metric);
  }

  dvmrpcb  = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }


  if (metric < 1 || metric >= (DVMRP_UNREACHABLE * 2))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"x %p metric %d + %d (invalid metric)\n",
                inetAddr, metric, interface->metric_in);
    /*  add for bad packets */
    return(adjusted_metric);
  }
  adjusted_metric = metric + interface->metric_in;

  if ((metric < DVMRP_UNREACHABLE) && 
      (adjusted_metric >= DVMRP_UNREACHABLE))
  {
    adjusted_metric = DVMRP_UNREACHABLE;
    return(adjusted_metric);
  }

  if (metric >= DVMRP_UNREACHABLE)
  {
    return(metric);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(adjusted_metric);
}

/*********************************************************************
* @purpose  This function is used to apply the policy on the outgoing 
*           interface. 
*
* @param    inetAddr  -  @b{(input)} Pointer to the  Network address.
* @param    metric     -  @b{(input)}   Cost of the metric
* @param    interface  -  @b{(input)}  Pointer to the Incoming interface
* @param    out          -  @b{(input)}   Pointer to the Out going interface
* @param    route       -  @b{(input)}  Pointer to the Route corresponding to this network
*
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_policy_out (L7_inet_addr_t * inetAddr, 
                           L7_int32 metric, 
                           dvmrp_interface_t *interface,
                           dvmrp_interface_t *out, 
                           dvmrp_route_t * route)
{
  L7_int32 ajusted_metric = 0;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if ((interface == L7_NULLPTR) || (out == L7_NULLPTR))
  {
    return -1;
  }

  dvmrpcb  = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }


  ajusted_metric = metric + out->metric_out;
  if (ajusted_metric > DVMRP_UNREACHABLE)
  {
    ajusted_metric = DVMRP_UNREACHABLE;
  }

  if (metric == L7_NULL /* directly connected */ && interface == out)
  {
    /*
     *  Return metric of zero for directly connected subnets that lead to
     *  the neighbor getting this report.  This will cause the route to the 
     *  subnet shared with this neighbor to be included in the route report.
     *  See LVL7 issue number VSky 5658 for discussion.
     */
    return L7_NULL;

  }

  /* split horizon w/ poisoned reverse */
  if ((interface == out && ajusted_metric != DVMRP_UNREACHABLE)||
      BIT_TEST(route->flags, DVMRP_RT_POISON))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"  o %p metric %d + %d"
                "(poisoned reverse)\n", inetAddr, ajusted_metric, DVMRP_UNREACHABLE);

    /* if we are poisoning the route, add DVMRP_UNREACHABLE to 
     * the received metric we got from this neighbor.
     */
    ajusted_metric = route->received_metric;

    ajusted_metric += DVMRP_UNREACHABLE;
    return(ajusted_metric);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ajusted_metric);
}

