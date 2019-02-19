/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_debug.c
*
* @purpose Contains the debugging routines for the DVMRP protocol
*
* @component
*
* @comments
*
* @create 25/01/2006
*
* @author Prakash
* @end
*
**********************************************************************/
#include "dvmrp_debug.h"
#include "dvmrp_api.h"
#include "dvmrp_util.h"
#include "dvmrp_cache_table.h"
#include "mcast_wrap.h"
#include "l3_mcast_commdefs.h"
#include "dvmrp_vend_ctrl.h"
#include "dvmrp_igmp_group_table.h"
#include "heap_api.h"

extern dvmrp_t *dvmrpCB;

/*------------------DEBUG ROUTINES------------------------------*/
/*********************************************************************
* @purpose  This function is used to print the interface table
*
* @param     addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugIntfShow(L7_uint32 addrFamily)
{
  dvmrp_interface_t *intf = L7_NULLPTR;
  L7_uint32 uiIntf = 0;
  dvmrp_t *dvmrpcb=L7_NULLPTR;      
  L7_uint32 s_addr;  
  L7_int32 timeLeft;    
  L7_ulong32 probeExpTime;    

  DVMRP_DEBUG_PRINTF("\n\nMCAST Heap-ID - 0x%x", mcastMapHeapIdGet(addrFamily));


  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }
  

  for (uiIntf=0;uiIntf < MAX_INTERFACES;uiIntf++)
  {
    intf = &dvmrpcb->dvmrp_interfaces[uiIntf];
    if (intf->index != uiIntf)
    {
     continue;
    }

    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &intf->primary.inetAddr.addr, &s_addr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }
    

    DVMRP_DEBUG_PRINTF("index:%d\n", intf->index);
    DVMRP_DEBUG_PRINTF("Interface Address:%x\n", s_addr);    
    DVMRP_DEBUG_PRINTF("metric_in:%d\n", intf->metric_in);
    DVMRP_DEBUG_PRINTF("metric_out:%d\n", intf->metric_out);
    DVMRP_DEBUG_PRINTF("Flags:%lx\n\n", intf->flags);

    if(intf->igmp_querier == MCAST_EVENT_MGMD_QUERIER)
    {
      DVMRP_DEBUG_PRINTF("On this interface this router is the IGMP querier\n\n");    
    }
    else
    {
      DVMRP_DEBUG_PRINTF("On this interface this router is not the IGMP querier\n\n");    
    }

    if (intf->probe != L7_NULLPTR)
    {
      if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, intf->probe ,
                                            &timeLeft))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Failed to get the time left for expiry:%d",
                    __FUNCTION__,__LINE__);
      }
    }
    else
    {
      timeLeft = 0;
    }
    probeExpTime = (timeLeft <= 0) ? 0 : timeLeft;
    DVMRP_DEBUG_PRINTF("time left for probe to be sent : %d\n", probeExpTime);                
    DVMRP_DEBUG_PRINTF("No of neighbors at present on this interface :%lx\n\n", intf->nbr_count);                                    
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}
/*********************************************************************
 * @purpose  This function is used to print the neighbours on an interface
 *
 * @param     addrFamily -@b{(input)}  FamilyType
 *
 *                  intfindex -  @b{(input)} Interface for which the user is 
 *                                     interested in the neighbours
 * @returns  None
 *
 *
 * @notes   None
 * @end
 *********************************************************************/
void dvmrpDebugNbrShow(L7_uint32 addrFamily,L7_int32 rtrIfNum)
{
  dvmrp_interface_t *intf = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_uint32 s_addr;
  dvmrp_t *dvmrpcb=L7_NULLPTR;    
  L7_int32 timeLeft;  
  L7_ulong32 nbrExpTime;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  intf = &dvmrpcb->dvmrp_interfaces[rtrIfNum];      
  if (intf->index != rtrIfNum)
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(intf->ll_neighbors));
      nbr != L7_NULLPTR;
      nbr = (dvmrp_neighbor_t*)SLLNextGet(&(intf->ll_neighbors),(void*)nbr))
  {
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &nbr->nbrAddr.addr, &s_addr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }
    DVMRP_DEBUG_PRINTF("Nbr Prefix:%x\n", s_addr);
    DVMRP_DEBUG_PRINTF("Nbr PrefixLen:%d\n", nbr->nbrAddr.maskLength);
    DVMRP_DEBUG_PRINTF("Nbr Flags:%lx\n", nbr->flags);
    DVMRP_DEBUG_PRINTF("Nbr State: %d\n",nbr->state);
    DVMRP_DEBUG_PRINTF("Nbr genid: %d\n",nbr->genid);    
    DVMRP_DEBUG_PRINTF("No of bad packets recieved from this neighbor: %d\n",nbr->badPkts);        
    DVMRP_DEBUG_PRINTF("No of bad Routes  recieved from this neighbor: %d\n",nbr->badRts);            

    if (nbr->timeout != L7_NULLPTR)
    {
      if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, nbr->timeout,
                                            &timeLeft))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Failed to get the time left for expiry:%d",
                    __FUNCTION__,__LINE__);
      }
    }
    else
    {
      timeLeft = 0;
    }
    nbrExpTime = (timeLeft <= 0) ? 0 : timeLeft;
    DVMRP_DEBUG_PRINTF("Neighbor expiry time : %d\n",nbrExpTime);                
  }          
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}

/*********************************************************************
 * @purpose  This function is used to print the route table
 *
 * @param    addrFamily -@b{(input)}  FamilyType
 * @returns   None
 *
 *
 * @notes   None
 * @end
 *********************************************************************/
void dvmrpDebugRoutesShow(L7_uint32 addrFamily)
{
  dvmrp_route_t *rt = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrpRouteData_t *pNextData = L7_NULLPTR;
  L7_uint32 i, s_addr;
  dvmrp_t *dvmrpcb=L7_NULLPTR;  


  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  /* Gets the first entry in the Tree */
  pData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, L7_NULLPTR);  

  if (pData == L7_NULLPTR)
  {
    /* If network is not found then we are done.*/
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  /* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpcb->dvmrpRouteTreeData, pData);

    rt = &pData->dvmrpRouteInfo;

    DVMRP_DEBUG_PRINTF("Rt Flags:%lx\t", rt->flags);
    DVMRP_DEBUG_PRINTF("Route is marked : ");
    if (BIT_TEST(rt->flags,DVMRP_RT_HOLD))
    {
      DVMRP_DEBUG_PRINTF("HOLD\t");
    }
    else if (BIT_TEST(rt->flags,DVMRP_RT_DELETE))
    {
      DVMRP_DEBUG_PRINTF("DELETED\t");
    }
    else
      DVMRP_DEBUG_PRINTF("ACTIVE\t");
    DVMRP_DEBUG_PRINTF("Expire Time : %d\n",
                       (L7_int32)(rt->dtime - osapiUpTimeRaw()));
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &rt->networkAddr.addr, 
                                     &s_addr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }
    DVMRP_DEBUG_PRINTF("Rt Prefix:%x\t", s_addr);
    DVMRP_DEBUG_PRINTF("Rt Metric:%d\t", rt->metric);
    if (rt->neighbor != L7_NULLPTR)
    {
      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                       &rt->neighbor->nbrAddr.addr, &s_addr))
      {
        DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
        osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
        return;
      }
      DVMRP_DEBUG_PRINTF("Rt Nbr Prefix:%x\t", s_addr);
      DVMRP_DEBUG_PRINTF("Rt Nbr Intf:%d\n", 
                         rt->neighbor->interface->index);
    }
    else
      DVMRP_DEBUG_PRINTF("\n");
    DVMRP_DEBUG_PRINTF("Route Dependents:");
    for (i = 0; i < DVMRP_MAX_NEIGHBORS;i++)
    {
      if (MCAST_BITX_TEST(rt->dependents.bits,i))
      {
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                         &dvmrpcb->index2neighbor[i].nbrAddr.addr, &s_addr))
        {
          DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          return;
        }
        DVMRP_DEBUG_PRINTF(" %x \n", s_addr);
      }
    }
    DVMRP_DEBUG_PRINTF("Designated forwarders:\n");
    for (i = 0; i < MAX_INTERFACES;i++)
    {
      if (rt->desg_fwd[i].status == DVMRP_ENTRY_USED)
      {
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                         &rt->desg_fwd[i].fwd_nbr->nbrAddr.addr, &s_addr))
        {
          DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          return;
        }
        DVMRP_DEBUG_PRINTF("Nbr: %x is DF Interface: %d metric:%d\n", s_addr,i,
                           rt->desg_fwd[i].metric);
      }
    }
    pData = pNextData;      
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}
/*********************************************************************
* @purpose  This function is used to print the prunes for the given netowrk
*
* @param       addrFamily -@b{(input)}  FamilyType
*                    src -  @b{(input)} source network
* 
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugPrunesShow(L7_uint32 addrFamily,L7_int32 src)
{
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;
  dvmrp_prune_t *prune = L7_NULLPTR;
  L7_uint32 srcAddr = L7_NULL;
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;
  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d", __FUNCTION__, __LINE__);
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }
  while (L7_NULLPTR != entry)
  {
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &entry->source, &srcAddr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }
    if (srcAddr == src)
    {
      break;
    }

    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d",
                  __FUNCTION__,__LINE__);
      break;
    }
  }   

  if (entry == L7_NULLPTR)
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
      prune != L7_NULLPTR;
      prune = (dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune))
  {
    if (prune->neighbor != L7_NULLPTR)
    {
      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                       &prune->neighbor->nbrAddr.addr, &srcAddr))
      {
        DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");      
        osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
        return;
      }
      DVMRP_DEBUG_PRINTF("Prune Nbr Prefix:%x\n", srcAddr);
      DVMRP_DEBUG_PRINTF("Prune Nbr Flags:%ld\n", 
                         prune->neighbor->flags);
      DVMRP_DEBUG_PRINTF("Prune Life time:%ld\n", 
                         prune->lifetime);
      DVMRP_DEBUG_PRINTF("Prune recieval time:%ld\n", 
                         prune->received);
      DVMRP_DEBUG_PRINTF("Prune Expirry time:%ld\n", 
                         (prune->expire -osapiUpTimeRaw()));
    }
  }          
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}
/*********************************************************************
* @purpose  This function is used to print the grafts sent
*
* @param    addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugGraftsShow(L7_uint32 addrFamily)
{
  dvmrp_graft_t *graft;
  L7_uint32 srcAddr = L7_NULL;
  L7_uint32 nbrAddr = L7_NULL;
  dvmrp_cache_entry_t mfentry, *entry = L7_NULLPTR;
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }


  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  memset(&mfentry, L7_NULL, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:No cache entry in the table",__FUNCTION__);
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return ;
  }

  while (entry != L7_NULLPTR)
  {
    if (entry->graft_sent != L7_NULLPTR)
    {
      graft = entry->graft_sent;
      if (graft->neighbor != L7_NULLPTR)
      {
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &graft->entry->source, 
                                         &srcAddr))
        {
          DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          return;
        }
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                         &graft->neighbor->nbrAddr.addr, &nbrAddr))
        {
          DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          return;
        }

        DVMRP_DEBUG_PRINTF("Graft Nbr Prefix:%x\n", nbrAddr);
        DVMRP_DEBUG_PRINTF("Graft Nbr Flags:%ld\n\n", 
                           graft->neighbor->flags);
        DVMRP_DEBUG_PRINTF("Graft Entry Src:%x\n", srcAddr);
        DVMRP_DEBUG_PRINTF("Graft hold time:%x\n", graft->holdtime);        
        DVMRP_DEBUG_PRINTF("Graft recieval time:%x\n", graft->received);        
        DVMRP_DEBUG_PRINTF("Graft timer expiry:%x\n", graft->expire);                
      }
    }

    memcpy(&mfentry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&mfentry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:No cache entry in the table",__FUNCTION__);
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return ;
    }
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}

/*********************************************************************
* @purpose  This function is used to print the stats
*
* @param    addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugStatsShow(L7_uint32 addrFamily, L7_int32 rtrIfNum)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;  
  dvmrp_interface_t *intf = L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }


  if (MCAST_BITX_TEST(dvmrpcb->interface_mask, rtrIfNum))
  {
    intf = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
    if (intf->index != rtrIfNum)
    {
      DVMRP_DEBUG_PRINTF("\n\nInterface is not UP \n\n");
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);     
      return;
    }

    DVMRP_DEBUG_PRINTF("Total no of valid probe packets recieved:%lx\n\n", intf->probePktsRecievedCount);                
    DVMRP_DEBUG_PRINTF("Total no of valid report packets recieved:%lx\n\n", intf->reportPktsRecievedCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Prune  packets recieved:%lx\n\n", intf->prunePktsRecievedCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Graft  packets recieved:%lx\n\n", intf->graftPktsRecievedCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Graft Ack packets recieved:%lx\n\n", intf->graftAckPktsRecievedCount);                        
    DVMRP_DEBUG_PRINTF("Total no of valid Nbr2  packets recieved:%lx\n\n", intf->nbr2PktsRecievedCount);                            
    
    DVMRP_DEBUG_PRINTF("Total no of valid probe packets sent out:%lx\n\n", intf->probePktsSentCount);                
    DVMRP_DEBUG_PRINTF("Total no of valid report packets sent out:%lx\n\n", intf->reportPktsSentCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Prune  packets sent out:%lx\n\n", intf->prunePktsSentCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Graft  packets sent out:%lx\n\n", intf->graftPktsSentCount);                    
    DVMRP_DEBUG_PRINTF("Total no of valid Graft Ack packets sent out:%lx\n\n", intf->graftAckPktsSentCount);                        
      
    DVMRP_DEBUG_PRINTF("Total no of bad packets recieved:%lx\n\n", intf->badPkts);    
    DVMRP_DEBUG_PRINTF("Total no of bad Routes recieved:%lx\n\n", intf->badRts);        
    DVMRP_DEBUG_PRINTF("Total no of packets sent out:%lx\n\n", intf->sentRts);            
    DVMRP_DEBUG_PRINTF("Total no of bad probe packets recieved:%lx\n\n", intf->badProbePkts);                
    DVMRP_DEBUG_PRINTF("Total no of bad report packets recieved:%lx\n\n", intf->badReportPkts);                    
    DVMRP_DEBUG_PRINTF("Total no of bad Prune  packets recieved:%lx\n\n", intf->badPrunePkts);                    
    DVMRP_DEBUG_PRINTF("Total no of bad Graft  packets recieved:%lx\n\n", intf->badGraftPkts);                    
    DVMRP_DEBUG_PRINTF("Total no of bad Graft Ack packets recieved:%lx\n\n", intf->badGraftAckPkts);                        
    DVMRP_DEBUG_PRINTF("Total no of bad Nbr2  packets recieved:%lx\n\n", intf->badNbr2Pkts);                            

  }
  else
  {
    DVMRP_DEBUG_PRINTF("\n\nInterface is not UP \n\n");
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}

/*********************************************************************
* @purpose  This function is used to print Interface stats
*
* @param     addrFamily -@b{(input)}  FamilyType
*            intfindex -  @b{(input)} Interface for which the user is 
*                                     interested in clearing the stats

* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/

void dvmrpDebugStatsClear(L7_uint32 addrFamily,L7_int32 rtrIfNum)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;    
  dvmrp_interface_t *intf = L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  intf = &dvmrpcb->dvmrp_interfaces[rtrIfNum];      
  if (intf->index != rtrIfNum)
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  intf->probePktsRecievedCount     =0;              
  intf->reportPktsRecievedCount    =0;                    
  intf->prunePktsRecievedCount     =0;                    
  intf->graftPktsRecievedCount     =0;                    
  intf->graftAckPktsRecievedCount  =0;                        
  intf->nbr2PktsRecievedCount      =0;                            
  intf->probePktsSentCount         =0;                
  intf->reportPktsSentCount        =0;                    
  intf->prunePktsSentCount         =0;                    
  intf->graftPktsSentCount         =0;                    
  intf->graftAckPktsSentCount      =0;                        
  intf->badPkts                    =0;    
  intf->badRts                     =0;        
  intf->sentRts                    =0;            
  intf->badProbePkts               =0;                
  intf->badReportPkts              =0;                    
  intf->badPrunePkts               =0;                    
  intf->badGraftPkts               =0;                    
  intf->badGraftAckPkts            =0;                        
  intf->badNbr2Pkts                =0;                            

  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
}


/*************************************************************************************
* @purpose  This devshell command is provided to see the Operational state of DVMRP.
*
* @param   addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*************************************************************************************/
void dvmrpDebugOperStatusShow(L7_uint32 addrFamily)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  if (dvmrpcb ->dvmrpOperFlag == L7_ENABLE)
  {
    DVMRP_DEBUG_PRINTF("dvmrpOperFlag is L7_ENABLE\n");
  }
  else
  {
    DVMRP_DEBUG_PRINTF("dvmrpOperFlag is L7_DISABLE\n");
  }
}
/*************************************************************************************
* @purpose  This devshell command is provided to dump the control block.
*
* @param   addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*************************************************************************************/

void dvmrpDebugCBShow(L7_uint32 addrFamily)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;  
  L7_uint32 index;
  L7_uchar8 addr[DVMRP_MAX_DBG_ADDR_SIZE];  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  
  DVMRP_DEBUG_PRINTF("protocol:%lx\n\n", dvmrpcb->proto );                
  DVMRP_DEBUG_PRINTF("Family:%lx\n\n", dvmrpcb->family );                
  DVMRP_DEBUG_PRINTF("sockfd:%lx\n\n", dvmrpcb->sockFd);                
  DVMRP_DEBUG_PRINTF("interface bit map:%lx\n\n");                

  for (index = 1; index <= MAX_INTERFACES; index++)
    if (MCAST_BITX_TEST (dvmrpcb->interface_mask, index))
      DVMRP_DEBUG_PRINTF (" %02d ", index);

  DVMRP_DEBUG_PRINTF("Timer Control block:%lx\n\n", dvmrpcb->timerHandle );                    
  DVMRP_DEBUG_PRINTF("Update timer:%lx\n\n", dvmrpcb->timer );                      
  DVMRP_DEBUG_PRINTF("Route timer:%lx\n\n", dvmrpcb->age );                        
  DVMRP_DEBUG_PRINTF("Flash timer:%lx\n\n", dvmrpcb->flash );                        
  DVMRP_DEBUG_PRINTF("Prune timer:%lx\n\n", dvmrpcb->expire );                          
  DVMRP_DEBUG_PRINTF("All routers Address:%lx\n\n", inetAddrPrint( &(dvmrpcb->all_routers.addr),addr));                          
  DVMRP_DEBUG_PRINTF("flash update waiting:%lx\n\n", dvmrpcb->flash_update_waiting);                          
  DVMRP_DEBUG_PRINTF("routing information changed:%lx\n\n", dvmrpcb->changed);                          
  DVMRP_DEBUG_PRINTF("Version supported by this router:%lx\n\n", dvmrpcb->versionSupport);                            
  DVMRP_DEBUG_PRINTF("dvmrpasbList:%lx\n\n", dvmrpcb->dvmrpasbList);                            
  DVMRP_DEBUG_PRINTF("dvmrpCacheTableTreeHeap:%lx\n\n", dvmrpcb->dvmrpCacheTableTreeHeap);                            
  DVMRP_DEBUG_PRINTF("dvmrpCacheDataHeap:%lx\n\n", dvmrpcb->dvmrpCacheDataHeap);                            
  DVMRP_DEBUG_PRINTF("dvmrpGroupTableTreeHeap:%lx\n\n", dvmrpcb->dvmrpGroupTableTreeHeap);                            
  DVMRP_DEBUG_PRINTF("dvmrpGroupDataHeap:%lx\n\n", dvmrpcb->dvmrpGroupDataHeap);                            
  DVMRP_DEBUG_PRINTF("dvmrpRouteDataHeap:%lx\n\n", dvmrpcb->dvmrpRouteDataHeap);                            
  DVMRP_DEBUG_PRINTF("dvmrpRouteTreeHeap:%lx\n\n", dvmrpcb->dvmrpRouteTreeHeap);                            
  DVMRP_DEBUG_PRINTF("radix_entries:%lx\n\n", dvmrpcb->radix_entries);                            
  DVMRP_DEBUG_PRINTF("handle_list:%lx\n\n", dvmrpcb->handle_list);                            
  DVMRP_DEBUG_PRINTF("updateTimerHandle:%lx\n\n", dvmrpcb->updateTimerHandle);                              
  DVMRP_DEBUG_PRINTF("routesTimeoutHandle:%lx\n\n", dvmrpcb->routesTimeoutHandle);                              
  DVMRP_DEBUG_PRINTF("pruneExpireHandle:%lx\n\n", dvmrpcb->pruneExpireHandle);                              
  DVMRP_DEBUG_PRINTF("flashrUpdateTimerHandle:%lx\n\n", dvmrpcb->flashrUpdateTimerHandle);                                
  
}
/*************************************************************************************
* @purpose 
*
* @param   addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*************************************************************************************/
void dvmrpDebugTimersShow(L7_uint32 addrFamily)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }
  appTimerDebugShow(dvmrpcb->timerHandle);
}

/*********************************************************************
* @purpose  This function is used to print the Group membership table
*
* @param       addrFamily -@b{(input)}  FamilyType
* 
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugMgmdShow(L7_uint32 addrFamily, L7_int32 count)
{
  dvmrp_Group_entry_t tmpEntry,*entry = L7_NULLPTR;
  dvmrp_t *dvmrpcb=L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uint32 noOfGroups = 0;
  L7_int32 localcount=0;
  L7_int32 groupNum = 0;
  L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8 src[IPV6_DISP_ADDR_LEN];

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;
  }

  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  DVMRP_DEBUG_PRINTF ("\n******************************\n");
  DVMRP_DEBUG_PRINTF ("***** DVMRP - IGMP TABLE *****\n");
  DVMRP_DEBUG_PRINTF ("******************************\n");

  if ((noOfGroups = avlTreeCount (&dvmrpcb->dvmrpGroupTableTree)) == 0)
  {
    DVMRP_DEBUG_PRINTF("\nDVMRP Group Membership Table is Empty.\n");
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  DVMRP_DEBUG_PRINTF("\nNo Of Group Entries - %d .\n", noOfGroups);

  memset(&tmpEntry, 0, sizeof(dvmrp_Group_entry_t));
  inetAddressZeroSet(L7_AF_INET, &tmpEntry.source);
  inetAddressZeroSet(L7_AF_INET, &tmpEntry.group);

  if (L7_SUCCESS != dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }

  while (dvmrpGroupTableGroupNextGet(dvmrpcb,&tmpEntry, &entry) == L7_SUCCESS)
  {
    if((count != 0) &&(localcount >= count))
    {
      break;
    }
    localcount++;

    memcpy(&tmpEntry, entry, sizeof(dvmrp_Group_entry_t));

    DVMRP_DEBUG_PRINTF("\n#%d.\n", ++groupNum);
    DVMRP_DEBUG_PRINTF("Group Address    - %s.\n", inetAddrPrint(&entry->group,grp));

    if (entry->numSrcs == 0)
    {
      DVMRP_DEBUG_PRINTF("  Group Mask Bit rtrIfNum   -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (MCAST_BITX_TEST(entry->grpIntfBitMask, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
  
      DVMRP_DEBUG_PRINTF("  Mode Mask Bit rtrIfNum    -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (MCAST_BITX_TEST(entry->modeIntfBitMask, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
  
      DVMRP_DEBUG_PRINTF("  Source Mask Bit rtrIfNum  -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (MCAST_BITX_TEST(entry->srcIntfBitMask, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
    }
    else
    {
      DVMRP_DEBUG_PRINTF("  Source Address - %s.\n", inetAddrPrint(&entry->source,src));

      DVMRP_DEBUG_PRINTF("  (*,G) Include Mask Bit rtrIfNum -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (BITX_TEST(&entry->includeStarG, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
  
      DVMRP_DEBUG_PRINTF("  (S,G) Include Mask Bit rtrIfNum -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (BITX_TEST(&entry->includeSG, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
  
      DVMRP_DEBUG_PRINTF("  (S,G) Exclude Mask Bit rtrIfNum -");
      for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
      {
        if (BITX_TEST(&entry->excludeSG, rtrIfNum) != 0)
        {
          DVMRP_DEBUG_PRINTF(" %d", rtrIfNum);
        }
      }
      DVMRP_DEBUG_PRINTF(" \n");
    }
  }

  DVMRP_DEBUG_PRINTF ("******************************\n");
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  return;
}


/*********************************************************************
*
* @purpose  To display the info of the Admin Scope Boundary Database
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
dvmrpDebugAdminScopeShow (L7_uint32 addrFamily)
{
  dvmrp_t *dvmrpcb=L7_NULLPTR;    
  dvmrpASBNode_t *asbCurrNode = L7_NULLPTR;
  dvmrpASBNode_t *asbNextNode = L7_NULLPTR;
  L7_uchar8 grp[DVMRP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grpMask[DVMRP_MAX_DBG_ADDR_SIZE];
  L7_uint32 index = 0;
  
  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;

  }

  /* Display the Admin Scope Database Contents */
  if ((asbCurrNode = (dvmrpASBNode_t*) SLLFirstGet (&(dvmrpcb->dvmrpasbList))) != L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("Next Node Addr - 0x%x.\n", asbCurrNode->next);
    DVMRP_DEBUG_PRINTF("Group Address  - %s.\n", inetAddrPrint (&(asbCurrNode->grpAddr), grp));
    DVMRP_DEBUG_PRINTF("Group Mask  - %s.\n", inetAddrPrint (&(asbCurrNode->grpMask), grpMask));
    DVMRP_DEBUG_PRINTF("Interfaces Set - ");
    for (index = 1; index <= MAX_INTERFACES; index++)
      if (MCAST_BITX_TEST (asbCurrNode->intfMask, index))
        DVMRP_DEBUG_PRINTF(" %02d ", index);
    DVMRP_DEBUG_PRINTF(".\n");
  }

  while ((asbNextNode = (dvmrpASBNode_t*) SLLNextGet (&(dvmrpcb->dvmrpasbList),
                                                (L7_sll_member_t*) asbCurrNode))
                      != L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("----- ----- -----\n");
    DVMRP_DEBUG_PRINTF("Next Node Addr - 0x%x.\n", asbNextNode->next);
    DVMRP_DEBUG_PRINTF("Group Address  - %s.\n", inetAddrPrint (&(asbNextNode->grpAddr), grp));
    DVMRP_DEBUG_PRINTF("Group Mask  - %s.\n", inetAddrPrint (&(asbCurrNode->grpMask), grpMask));
    DVMRP_DEBUG_PRINTF("Interfaces Set - ");
    for (index = 1; index <= MAX_INTERFACES; index++)
      if (MCAST_BITX_TEST (asbNextNode->intfMask, index))
        DVMRP_DEBUG_PRINTF(" %02d ", index);
    DVMRP_DEBUG_PRINTF(".\n");
    asbCurrNode = asbNextNode;
  }

}

/*********************************************************************
* @purpose  This function is used to print the Group membership table
*
* @param       addrFamily -@b{(input)}  FamilyType
* 
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugMRTTableShow(L7_uint32 addrFamily, L7_int32 count)
{
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;
  L7_uint32 GrpAddr = L7_NULL;
  L7_uint32 SrcAddr = L7_NULL;  
  dvmrp_t *dvmrpcb=L7_NULLPTR;  
  L7_int32 index;
  L7_int32 localcount=L7_NULL;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    DVMRP_DEBUG_PRINTF("\n\ndvmrpcb Pointer is NULL!!!!!!!!! \n\n");
    return;
  }


  if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG_PRINTF("\n\nCould Not Access the semaphore.\n\n");
    return ;
  }

  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));

  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
    return;
  }
  while (L7_NULLPTR != entry)
  {

    if((count != L7_NULL)&&(localcount >= count))
    {
      break;
    }
    localcount++;    
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &entry->source, &SrcAddr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }

    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &entry->group, &GrpAddr))
    {
      DVMRP_DEBUG_PRINTF("Couldn't get the IP ADDRESS\n\n");     
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      return;
    }

    DVMRP_DEBUG_PRINTF("SrcAddress:%x\n", SrcAddr);
    DVMRP_DEBUG_PRINTF("GrpAddress:%x\n", GrpAddr);

    if(entry->parent != NULL)
    {
      DVMRP_DEBUG_PRINTF("Incoming interface:%x\n", entry->parent->index);
    }

    DVMRP_DEBUG_PRINTF("direct members \n");    
    for (index = 1; index <= MAX_INTERFACES; index++)
    if (MCAST_BITX_TEST (entry->children, index))
      DVMRP_DEBUG_PRINTF (" %02d ", index);

    DVMRP_DEBUG_PRINTF("\n Down stream routers \n");    
    for (index = 1; index <= MAX_INTERFACES; index++)
    if (MCAST_BITX_TEST (entry->routers, index))
      DVMRP_DEBUG_PRINTF (" %02d ", index);

    if(entry->graft_timer != L7_NULLPTR)
    {
      DVMRP_DEBUG_PRINTF("\n Graft timer running\n");
    }
    else
    {
      DVMRP_DEBUG_PRINTF("\n Graft timer not running\n");
    }
      
    DVMRP_DEBUG_PRINTF("Entry Flags\n",entry->flags);     

    DVMRP_DEBUG_PRINTF("Entry expiry time \n",(entry->expire -(osapiUpTimeRaw())));     

    DVMRP_DEBUG_PRINTF("Prunes recieved on \n");
    for (index = 1; index <= MAX_INTERFACES; index++)
    if (MCAST_BITX_TEST (entry->pruneRcvd, index))
      DVMRP_DEBUG_PRINTF (" %02d ", index);

    DVMRP_DEBUG_PRINTF("\n Prunes retry interval::%x\n",entry->prune_retry_interval);    

    if(entry->prune_retry_timer != L7_NULLPTR)
    {
      DVMRP_DEBUG_PRINTF("prune retry timer running\n");
    }
    else
    {
      DVMRP_DEBUG_PRINTF("prune retry timer not running\n");
    }
    if(entry->cacheRemove_timer != L7_NULLPTR)
    {
      DVMRP_DEBUG_PRINTF("cache remove timer running\n");
    }
    else
    {
      DVMRP_DEBUG_PRINTF("cahce remove timer not running\n");
    }

    DVMRP_DEBUG_PRINTF("cache remove handle :%x\n",entry->cacheRemoveHandle);
    DVMRP_DEBUG_PRINTF("prune Retransmitimer Handle:%x\n",entry->pruneRetransmitimerHandle);
    DVMRP_DEBUG_PRINTF("graft Timeout Handle:%x\n",entry->graftTimeoutHandle);
    DVMRP_DEBUG_PRINTF("\n");      

    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
       break;
    }
  }   

  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}

/******************************************************************************
* @purpose  Help command: show all debug APIs
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void dvmrpDebugHelp ()
{
   DVMRP_DEBUG_PRINTF("\n For enabling debugs for DVMRP following two dev shell functions have to be called.");
   DVMRP_DEBUG_PRINTF("\n For selectively enabling the debugs call dvmrpDebugFlagSet instead of dvmrpDebugAllSet  )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugEnable() )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugDisable() )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugAllSet() ");   
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugAllReset() ");   
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugFlagSet ( flag =0..11 ) ");   
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugFlagReset ( flag =0..11 ) ");   
   
   DVMRP_DEBUG_PRINTF("\n     family = 1 or 2 ");   
   DVMRP_DEBUG_PRINTF("\n     intfIfNum = device port num");   
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugIntfShow( family )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugNbrShow( family, intIfnum) ");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugRoutesShow( family ) ");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugPrunesShow(family,srcAddress)");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugGraftsShow( family )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugStatsShow( family,intIfNum )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugStatsClear( family,intIfNum )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugOperStatusShow( family )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugCBShow(family )");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugMgmdShow( family ) ");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugAdminScopeShow (family)");
   DVMRP_DEBUG_PRINTF("\n dvmrpDebugMRTTableShow( family,count  )");
}

/*------------------DEBUG ROUTINES END------------------------------*/

