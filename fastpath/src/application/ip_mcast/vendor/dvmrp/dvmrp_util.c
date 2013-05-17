/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_util.c
*
* @purpose Contains utility routines to be used by DVMRP protocol
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
#include "dvmrp_util.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_routing.h"
#include "dvmrp_protocol.h"
#include "dvmrp_igmp_group_table.h"
#include "dvmrp_timer.h"

/* Number of elements in DVMRP heap pool list, including dummy end-of-list element */
#define DVMRP_POOL_LIST_SIZE  14
heapBuffPool_t dvmrpV4HeapPoolList[DVMRP_POOL_LIST_SIZE]; 


/*********************************************************************
*
* @purpose  Build the DVMRP buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment DVMRP_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
static heapBuffPool_t *dvmrpHeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(dvmrpV4HeapPoolList, 0, sizeof(heapBuffPool_t) * DVMRP_POOL_LIST_SIZE);

  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_t);
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_CB", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* MRT (S,G) Database Buffers */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_cache_entry_t);
  dvmrpV4HeapPoolList[i].buffCount = DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_CAHCE_ENTRY", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Prune Buffers */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_prune_t);
  dvmrpV4HeapPoolList[i].buffCount = (((MCAST_MAX_INTERFACES - 1)/10) * DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES);
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_PRUNES", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Graft Buffers */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_graft_t);
  dvmrpV4HeapPoolList[i].buffCount = DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_GRAFTS", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Timer Event Buffers */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_timer_event_t);
  dvmrpV4HeapPoolList[i].buffCount = DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_TIMER_EVENTS", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MRT (S,G) Database */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES;
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_CACHE_TBL_TREE_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MRT (S,G) Database */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_cache_entry_t) * DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES;
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_CACHE_TBL_DATA_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Admin Scope Boundary Database Buffers */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrpASBNode_t);
  dvmrpV4HeapPoolList[i].buffCount = DVMRP_MAX_ADMIN_SCOPE_ENTRIES;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_ADM_SCP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Radix Tree Heap Buffers for Unicast Routes Database */
  dvmrpV4HeapPoolList[i].buffSize = (RADIX_TREE_HEAP_SIZE(DVMRP_L3_ROUTE_TBL_SIZE_TOTAL, sizeof(dvmrpTreeKey_t)));
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_ROUTING_TREE_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Radix Data Heap Buffers for Unicast Routes Database */
  dvmrpV4HeapPoolList[i].buffSize = (RADIX_DATA_HEAP_SIZE(DVMRP_L3_ROUTE_TBL_SIZE_TOTAL, sizeof(dvmrpRouteData_t)));
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_ROUTING_DATA_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD Database */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * DVMRP_MAX_LOCAL_MEMB_ENTRIES;
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_MGMD_TREE_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD Database */
  dvmrpV4HeapPoolList[i].buffSize = sizeof (dvmrp_Group_entry_t) * DVMRP_MAX_LOCAL_MEMB_ENTRIES;
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_MGMD_DATA_HEAP", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* report entries */
  dvmrpV4HeapPoolList[i].buffSize = sizeof(dvmrp_report_t) * platRtrRouteMaxEntriesGet();
  dvmrpV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "DVMRP_REPORT_ENTRIES", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  dvmrpV4HeapPoolList[i].buffSize = 0;
  dvmrpV4HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(dvmrpV4HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  dvmrpV4HeapPoolList[i].flags = 0;
  i++;

  if (i != DVMRP_POOL_LIST_SIZE)
  {
    LOG_ERROR(i);
  }
  return dvmrpV4HeapPoolList;
}

/*********************************************************************
* @purpose  This function is used to check whether the mask is a valid 
*           mask or not. 
* @param     mask   -  @b{(input)} Given mask .
* 
* @returns  0/1
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 inet_mask_valid (L7_uchar8 *mask)
{
  L7_ulong32 lmask = osapiNtohl (*(L7_ulong32 *)mask);

  return(~(((lmask & -lmask) - 1) | lmask) == 0);
}

/*********************************************************************
* @purpose  This function is used to bring the neighbors down.
*
* @param   dvmrpcb - @b{(input)} Pointer to the DVMRP Control block
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_neighbours_down (dvmrp_t *dvmrpcb)
{
  int nbrIndex;
  dvmrp_neighbor_t *pNbr=L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  for (nbrIndex = 1; nbrIndex< DVMRP_MAX_NEIGHBORS; nbrIndex++)
  {
    pNbr = &dvmrpcb->index2neighbor[nbrIndex];
    if (pNbr->status == DVMRP_ENTRY_NOT_USED)
    {
      continue;
    }
    pNbr->state = L7_DVMRP_NEIGHBOR_STATE_DOWN;

  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  This function is used to shut down DVMRP gracefully
*
* @param     interface -Pointer to the interface on which DVMRP should be shut down
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_graceful_shutdown(dvmrp_interface_t *interface)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrp_interface_t *tempvif = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_BOOL active_nbr;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_uint32 uiIntf = 0;
  dvmrp_report_t  *dvmrp_ann_rt = L7_NULLPTR; 
  L7_uint32 counter=0;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  assert (MCAST_BITX_TEST (dvmrpcb->interface_mask, interface->index));
  if (!BIT_TEST (interface->flags, DVMRP_VIF_UP))
  {
    return;
  }
  if (BIT_TEST (interface->flags, DVMRP_VIF_LEAF))
  {
    /*DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"DVMRP_VIF_LEAF is true In ADvert ROUTE...\n");*/
    return;
  }

  memset(dvmrpcb->report_entries, L7_NULL, platRtrRouteMaxEntriesGet() * sizeof(dvmrp_report_t ));    

  dvmrp_ann_rt = dvmrpcb->report_entries;
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

    /* send all reach-able routes with infinity exclude directly injected
     * route for that interface
     */
    if ((route->metric != DVMRP_UNREACHABLE) &&
        (route->interface->index == interface->index))
    {
      memcpy(&dvmrp_ann_rt[counter].srcNetwork,&route->networkAddr,sizeof(L7_dvmrp_inet_addr_t));
      dvmrp_ann_rt[counter].metric = DVMRP_UNREACHABLE;
      counter++;
    }

    pData = pNextData;
  }
  for (uiIntf=0;uiIntf < MAX_INTERFACES;uiIntf++)
  {
    tempvif = &dvmrpcb->dvmrp_interfaces[uiIntf];
    if (tempvif->index != uiIntf)
    {
      continue;
    }
    assert (MCAST_BITX_TEST (dvmrpcb->interface_mask, tempvif->index));
    if (!BIT_TEST (tempvif->flags, DVMRP_VIF_UP))
    {
      continue;
    }
    if (BIT_TEST (tempvif->flags, DVMRP_VIF_LEAF))
    {
      continue;
    }
    if (tempvif == interface)
    {
      continue;
    }
    active_nbr = L7_FALSE;
    for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(tempvif->ll_neighbors));
        nbr != L7_NULLPTR;
        nbr = (dvmrp_neighbor_t*)SLLNextGet(&(tempvif->ll_neighbors),(void*)nbr))
    {
      if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_ACTIVE)
      {
        active_nbr = L7_TRUE;
        break;
      }
    }
    if (active_nbr && counter  > 0)
    {
      if (dvmrp_routes_send (counter, tempvif) == L7_FAILURE)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nReport Not Sent \n");
      }
    }
  }


  dvmrp_ann_rt = dvmrpcb->report_entries;
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

    /* send all reach-able routes with infinity exclude directly injected
     * route for that interface
     */
    if ((route->metric != DVMRP_UNREACHABLE) &&
        (route->interface->index != interface->index))
    {
      memcpy(&dvmrp_ann_rt[counter].srcNetwork,&route->networkAddr,sizeof(L7_dvmrp_inet_addr_t));
      dvmrp_ann_rt[counter].metric = DVMRP_UNREACHABLE;
      counter++;
    }

    pData = pNextData;
  }

  active_nbr = L7_FALSE;
  for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
      nbr != L7_NULLPTR;
      nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
  {
    if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_ACTIVE)
    {
      active_nbr = L7_TRUE;
      break;
    }
  }
  if (active_nbr && counter  > 0)
  {
    if (dvmrp_routes_send (counter, interface) == L7_FAILURE)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nReport Not Sent \n");
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
*
* @purpose  The delete function is provided to delete both data and
*           node part of the SLL Node.
*
* @param    node -  @b{(input)} The pointer to the node that is to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    -   None.
*
* @end
*********************************************************************/
L7_RC_t dvmrp_SLLNodePrune_delete(L7_sll_member_t *node)
{
  dvmrp_prune_t *prune=L7_NULLPTR;
  dvmrp_t *dvmrpcb= L7_NULLPTR;

  prune= ((dvmrp_prune_t * )node);

  if ((prune == NULL) ||(prune->neighbor == NULL)
      ||(prune->neighbor->interface == L7_NULLPTR)||(prune->neighbor->interface->global == L7_NULLPTR))
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nCould not get the control block\n");
    return L7_FAILURE;
  }

  dvmrpcb = prune->neighbor->interface->global;
  DVMRP_FREE (L7_AF_INET, (void*) node);        
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function compares the interfaces. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* returns Zero or Non Zero
* 
* @notes    This function uses interface index as 
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_interface_comp (void *node1, void *node2, L7_uint32 len)
{
  dvmrp_interface_t *a = (dvmrp_interface_t *)node1;
  dvmrp_interface_t *b = (dvmrp_interface_t *)node2;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((a == L7_NULLPTR) || (b == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_interface_comp: improper pamrams passed\n");
    return(1);
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(a->index - b->index);
}
/*********************************************************************
* @purpose  This function compares the neighbors. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
* 
* @returns  Zero or Non Zero
*
* @notes    This function uses both index and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_neighbor_comp (void *node1, void *node2, L7_uint32 len)
{
  L7_int32 ret = L7_NULL;

  dvmrp_neighbor_t *a = (dvmrp_neighbor_t *)node1;
  dvmrp_neighbor_t *b = (dvmrp_neighbor_t *)node2;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((a == L7_NULLPTR) || (b == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_neighbor_comp: improper pamrams passed\n");
    return(1);
  }

  ret = L7_INET_ADDR_COMPARE(&a->nbrAddr.addr, &b->nbrAddr.addr);
  if (ret == L7_NULL)
  {
    ret = a->interface->index-b->interface->index;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}

/*********************************************************************
* @purpose  This function compares the prunes. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* @returns Zero or Non Zero
*
* @notes    This function uses both expire and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_prune_comp (void *node1, void *node2, L7_uint32 len)
{
  L7_int32 ret = L7_NULL;

  dvmrp_prune_t *a = (dvmrp_prune_t *)node1;
  dvmrp_prune_t *b = (dvmrp_prune_t *)node2;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((a == L7_NULLPTR) || (b == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_interface_comp: improper pamrams passed\n");
    return(1);
  }
  ret = a->expire-b->expire;
  if (ret == L7_NULL)
  {
    ret = L7_INET_ADDR_COMPARE(&a->neighbor->nbrAddr.addr,
                               &b->neighbor->nbrAddr.addr);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}

/*********************************************************************
* @purpose  This function compares the grafts. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.

* @returns  Zero or Non Zero
*
* @notes    This function uses both expire and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_comp (void *node1, void *node2, L7_uint32 len)
{
  L7_int32 ret = L7_NULL;

  dvmrp_graft_t *a = (dvmrp_graft_t *)node1;
  dvmrp_graft_t *b = (dvmrp_graft_t *)node2;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((a == L7_NULLPTR) || (b == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_interface_comp: improper pamrams passed\n");
    return(1);
  }
  ret = a->expire - b->expire;
  if (ret == L7_NULL)
  {
    ret = L7_INET_ADDR_COMPARE(&a->neighbor->nbrAddr.addr,
                               &b->neighbor->nbrAddr.addr);
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}

/*********************************************************************
* @purpose  This function compares the reports.
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* @returns  Zero or Non Zero
* @notes    This function uses both srcNetwork and neighbor addresses as
*           the key for comparision.
*
* @end
*********************************************************************/
L7_int32 dvmrp_report_comp (void *node1, void *node2, L7_uint32 len)
{
  L7_int32 ret = L7_NULL;

  dvmrp_report_t *a = (dvmrp_report_t *)node1;
  dvmrp_report_t *b = (dvmrp_report_t *)node2;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if ((a == L7_NULLPTR) || (b == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_interface_comp: improper pamrams passed\n");
    return(1);
  }
  ret = L7_INET_ADDR_COMPARE(&a->srcNetwork.addr,
                             &b->srcNetwork.addr);
  if (ret == L7_NULL)
  {
    ret = L7_INET_ADDR_COMPARE(&a->neighbor->nbrAddr.addr,
                               &b->neighbor->nbrAddr.addr);
  }
  if (ret == L7_NULL)
  {
    ret = a->metric - b->metric;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}

/*********************************************************************
* @purpose  This function is used to increment the count of bad 
*           packets recieved
*
* @param    interface  -  @b{(input)} interface from which the bad packet came
* @param    nbr -        @b{(input)} neighbour from whome the bad packet came
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_badpkts_inc(dvmrp_interface_t *interface, dvmrp_neighbor_t *nbr)
{
  if (interface)
    interface->badPkts++;
  if (nbr)
    nbr->badPkts++;
}

/*********************************************************************
* @purpose  This function is used to increment the count of bad routes recieved
*
* @param     interface -  @b{(input)} interface from which the bad route came
* @param     nbr       -  @b{(input)} neighbour from whome the bad route came
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_badRts_inc(dvmrp_interface_t *interface, dvmrp_neighbor_t *nbr)
{
  if (interface)
  {
    interface->badRts++;
  }
  if (nbr)
  {
    nbr->badRts++;
  }
}

/*********************************************************************
* @purpose  This function is used to check whether prune to be sent or not
*
* @param     dvmrpcb  - @b{(input)} Pointer to the DVMRP control block
* @param     src          - @b{(input)} source address for which the prune to be sent
* @param     grp         -  @b{(input)} group address for which the prune to be sent
* @param     bitlen      -  @b{(input)}-bitlen of the source network
* 
* @returns   1  - if no prune was sent earlier to this source network for 
*                 that group.
*                 0 - if a prune is already sent for that source netowrk for that 
*                 group.
*
* @notes    checks to see if a prune is already sent for that source network 
*           or not.
*            
* @end
*********************************************************************/
L7_int32 dvmrp_send_prune_check(dvmrp_t *dvmrpcb, L7_inet_addr_t *src,L7_inet_addr_t *grp, 
                                L7_uint32 bitlen)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb, &tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d\n",__FUNCTION__,__LINE__);
  }

  while (L7_NULLPTR != entry)
  {
    /* If the Source address matches then prune needs to be applied */
    if ((inetAddrCompareAddrWithMask(&entry->source,bitlen,src,bitlen) == L7_NULL)&&
        (L7_INET_ADDR_COMPARE(&entry->group.addr,grp) == L7_NULL))
    {
      if (L7_INET_ADDR_COMPARE(&entry->source.addr, src) == L7_NULL)
      {
        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb, &tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:There is no cache entry in the table:%d\n",__FUNCTION__,__LINE__);
          break;
        }
        continue;
      }
      if ((entry->count != 0) || (entry->firstPrune !=0))
      {
        return L7_SUCCESS;
      }
    }

    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb, &tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d\n",__FUNCTION__,__LINE__);
      break;
    }
  }     
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  sets up a key for the DVMRP radix tree
*
* @param    p_key - @b{(output)} A pointer to the key
* @param    addr            - @b{(input)} Key value.
*
* @notes    Sets the 1st byte to the length of the key, initializes
*           pad bytes and sets the key bytes to addr.
*
* @end
*********************************************************************/
void dvmrpKeySet(dvmrpTreeKey_t * p_key, L7_inet_addr_t addr)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  memset(p_key, 0, sizeof(dvmrpTreeKey_t));
  *((L7_int32 *)((p_key)->reserved)) = -1;  /* ffff out pad bytes */
  (p_key)->reserved[0] = sizeof(dvmrpTreeKey_t);  /* set 1st byte as length */
  inetCopy(&(p_key)->addr,&addr); /* set address */

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/*********************************************************************
* @purpose  Determines if the route table is full.
*
* @param    dvmrpcb  - @b{(input)} Pointer to the control block 
* @returns  L7_TRUE   If the table is full
* @returns  L7_FALSE  If the table is not full
*
* @notes    Internal utility function.  The calling function is responsible
*           for acquiring the route table semaphore.
*
* @end
*********************************************************************/
L7_BOOL dvmrpIsFull (dvmrp_t *dvmrpcb)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (dvmrpcb->radix_entries  >= platRtrRouteMaxEntriesGet())
    return L7_TRUE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  returns the default mask
*
* @param    family  - @b{(input)} family type 
* @returns  returns default mask
*
* @notes    Internal utility function.  
*
* @end
*********************************************************************/
L7_int32 dvmrpDefaultNetmask (L7_uchar8     family)
{
  if (family == L7_AF_INET)
    return 32;

  if (family == L7_AF_INET6)
    return 128;

  return 0;
}

/*********************************************************************
* @purpose  Returns whether the content of pointer is Zero or not.
*
* @param    *ptr   -    @b{(input)}  Pointer to Data element.
* @param    size   -    @b{(input)}  Size of the data element.
*
* @returns  0 : if content of data element is non zero
* @returns  1 : if content of data element is zero
*
* @notes   None
*
* @end
*********************************************************************/
L7_int32 dvmrpIfzero (void *ptr, L7_int32 size)
{
  L7_uchar8 *p = ptr;
  L7_int32 zero = 0;
  while (size--)
  {
    zero |= *p++;
  }
  return(zero == 0);
}

/*********************************************************************
* @purpose Boolean OR's the first 2 args, and puts into third arg.
*
* @param    *a   -     @b{(input)}
* @param    *b   -     @b{(input)}
* @param    *c   -     @b{(input)}
* @param    size   -   @b{(input)}
*
* @returns
*
* @notes   None
*
* @end
*********************************************************************/
L7_int32 dvmrpIfor (void *a, void *b, void *c, L7_int32 size)
{
  L7_uchar8 *p = a;
  L7_uchar8 *q = b;
  L7_uchar8 *r = c;
  L7_int32 zero = 0;
  while (size--)
  {
    zero |= (*p | *q);
    if (r)
    {
      *r++ = (*p | *q);
    }
    p++;
    q++;
  }
  return(zero);
}

/*********************************************************************
*
* @purpose  Initialize the DVMRP Heap Memory
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
dvmrpUtilMemoryInit (L7_uint32 addrFamily)
{
  heapBuffPool_t *dvmrpHeapPoolList = L7_NULLPTR;

  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Entry");

  if (addrFamily == L7_AF_INET)
  {
    dvmrpHeapPoolList = dvmrpHeapPoolListGet();
  }
  else
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES,
                 "Bad/Unsupported Address Family - %d", addrFamily);
    return L7_FAILURE;
  }

  if (heapInit (mcastMapHeapIdGet(addrFamily), dvmrpHeapPoolList,
                HEAP_FLAGS_NONE)
             != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "DVMRP Heap Initialization Failed");
    return L7_FAILURE;
  }

  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

