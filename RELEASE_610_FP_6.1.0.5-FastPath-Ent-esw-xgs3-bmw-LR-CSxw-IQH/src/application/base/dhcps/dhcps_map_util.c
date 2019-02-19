/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  dhcps_map_util.c
* @purpose   DHCP Server Mapping system utility infrastructure
* @component DHCP Server Mapping  utility Layer
* @comments  none
* @create    09/12/2003
* @author    athakur
* @end
*             
**********************************************************************/

#include "l7_dhcpsinclude.h"
                    
extern dhcpsMapCtrl_t   dhcpsMapCtrl_g;
extern L7_dhcpsMapCfg_t *pDhcpsMapCfgData;
extern L7_dhcpsMapLeaseCfg_t *pDhcpsMapLeaseCfgData;
extern dhcpsInfo_t      *pDhcpsInfo;

/*********************************************************************
* @purpose  Insert before the given node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
* @param    newlink        @b{(input)} Pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListInsertBeforeLink(dhcpsLink_t * link, dhcpsLink_t * newlink)
{
  L7_assert(L7_NULLPTR == link);
  L7_assert(L7_NULLPTR == newlink);

  newlink->next = link;
  newlink->prev = link->prev;

  if(L7_NULLPTR != link->prev)
  {
    (link->prev)->next = newlink;
  }

  link->prev = newlink;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Insert after the given node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
* @param    newlink        @b{(input)} Pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListInsertAfterLink(dhcpsLink_t * link, dhcpsLink_t * newlink)
{
  L7_assert(L7_NULLPTR == link);
  L7_assert(L7_NULLPTR == newlink);

  newlink->next = link->next;
  newlink->prev = link;

  if(L7_NULLPTR != link->next)
  {
    (link->next)->prev = newlink;
  }

  link->next = newlink;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Remove the given node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListRemoveLink(dhcpsLink_t * link)
{
  L7_assert(L7_NULLPTR == link);

  /* stand alone? */
  if( (0 == link->next) && (0 == link->prev) )
  {
    return L7_FAILURE;
  }
  /* head node? */
  else if(0 == link->prev)
  {
    /* make next head */
    (link->next)->prev = 0;
  }
  /* tail node? */
  else if(0 == link->next)
  {
    /* make prev tail */
    (link->prev)->next = 0;
  }
  /* intermidiate node */
  else
  {
    (link->prev)->next = link->next;
    (link->next)->prev = link->prev;
  }

  /* detach */
  link->next = L7_NULLPTR;
  link->prev = L7_NULLPTR;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the first node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
* @param    first          @b{(input)}  Pointer to pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListFirstNode(dhcpsLink_t * link, dhcpsLink_t * * first)
{
  
  dhcpsListGetHead(link, first);

  if(L7_NULLPTR != *first)
  {
    if(L7_NULLPTR != (*first)->next)
    {
      *first = (*first)->next;
    }
    else
    {
      *first = L7_NULLPTR;
    }
  }

  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Get the head node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
* @param    head           @b{(input)}  Pointer to pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListGetHead(dhcpsLink_t * link, dhcpsLink_t * * head)
{
  L7_assert(L7_NULLPTR == link);
  L7_assert(L7_NULLPTR == head);

  *head = 0;
  
  while(link->prev != L7_NULLPTR)
  {
    link = link->prev;
  }

  *head = link;

  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Get the tail node
*
* @param    link           @b{(input)}  Pointer to the Linked list structure
* @param    tail           @b{(input)}  Pointer to pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListGetTail(dhcpsLink_t * link, dhcpsLink_t * * tail)
{
  L7_assert(L7_NULLPTR == link);
  L7_assert(L7_NULLPTR == tail);

  *tail = 0;
  
  while(link->next != L7_NULLPTR)
  {
    link = link->next;
  }

  *tail = link;

  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Count the number of nodes in the list
*
* @param    link           @b{(input)} Pointer to the Linked list structure
*
* @returns  count (number of nodes)
*
* @notes    
*       
* @end
*********************************************************************/
L7_uint32 dhcpsListGetCount(dhcpsLink_t * link)
{
  dhcpsLink_t *tempLink;
  L7_uint32   count;

  L7_assert(L7_NULLPTR == link);
  
  dhcpsListGetHead(link, &tempLink);

  /* count but exclude head! */
  count = 0;
  while(tempLink->next != L7_NULLPTR)
  {
    if(tempLink == tempLink->next)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
                    "Error: circular loop detected in dhcpsListGetCount\n");
      count = 0;
      break;
    }
    
    tempLink = tempLink->next;
    count++;
  }
  
  return count;
}
/*********************************************************************
* @purpose  Log message by traversing the list
*
* @param    link     @b{(input)}  Pointer to the Linked list structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsListDebugPrint(dhcpsLink_t * link)
{
  dhcpsLink_t * tempLink;

  dhcpsListGetHead(link, &tempLink);

  while(tempLink != L7_NULLPTR)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Node %d\tobj  %d\nprev %d\tnext %d\n\n", 
      (int)tempLink, (int)tempLink->object, (int)tempLink->prev, (int)tempLink->next);
    
    tempLink = tempLink->next;
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create manual pool
*
* @param    pPoolCfg     @b{(input)}  Pointer to pool config structure
* @param    pool         @b{(output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsCreateManualPool(poolCfgData_t * pPoolCfg, dhcpsPoolNode_t * * pool)
{
  dhcpsLeaseNode_t  *pLease;

  /* check inputs */
  if(L7_NULLPTR == pPoolCfg || L7_NULLPTR == pool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "MANUAL POOL creation failed, NULL passed\n");
    return L7_FAILURE;
  }
  else if(0 == pPoolCfg->ipAddr)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "MANUAL POOL creation failed, 0 ipaddr passed\n");
    return L7_FAILURE;
  }
  else if(0 == pPoolCfg->hostAddrLength && 0 == pPoolCfg->hostIdLength)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "MANUAL POOL creation failed, either hwaddr or clientID must be set\n");
    return L7_FAILURE;
  }
  
  /* check if active leases exist */
  if(dhcpsFindLeaseNodeByIpAddr(pPoolCfg->ipAddr, &pLease) == L7_SUCCESS)
  {
    if(ACTIVE_LEASE == pLease->leaseData->state)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "MANUAL POOL creation failed, active lease exists for ipaddr %x\n", pPoolCfg->ipAddr);
      return L7_FAILURE;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "MANUAL POOL deallocating existing %x lease\n", pPoolCfg->ipAddr);
      dhcpsDeallocateLease(&pLease);
    }

  }

  /* create pool */
  if(dhcpsCreatePool(pPoolCfg, pool) == L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "MANUAL POOL created pool for ipaddr %x\n", pPoolCfg->ipAddr);
    return L7_SUCCESS;
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "MANUAL POOL creation failed for ipaddr %x\n", pPoolCfg->ipAddr);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Create an address pool
*
* @param    pPoolCfg     @b{(input)}  Pointer to pool config structure
* @param    pool         @b{(output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsCreatePool(poolCfgData_t * pPoolCfg, dhcpsPoolNode_t * * pool)
{
  dhcpsLink_t     *link;
  dhcpsLink_t     *prevlink;
  dhcpsPoolNode_t *tempPool;

  if(L7_NULLPTR == pPoolCfg || L7_NULLPTR == pool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to CreateDynamicPool is a NULL ptr\n");
    return L7_FAILURE;
  }

  /* check if ipAddr is configured */
  if(0 == (pPoolCfg->ipAddr & pPoolCfg->ipMask))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Cannot create pool node with ipaddr 0\n");
    return L7_FAILURE;
  }

  /* iterate server pools list, find unique location for ipaddr */
  link = &(pDhcpsInfo->dhcpsPoolsHead);
  prevlink = link;
  while(L7_NULLPTR != link)
  {
    tempPool = (dhcpsPoolNode_t *)link->object;
            
    /* is head? (head has no object!) */
    if(L7_NULLPTR != tempPool)
    {
      if(tempPool->poolData->ipAddr == pPoolCfg->ipAddr)
      {
        /* ipaddr not unique! */
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Pool node already exists for ipaddr %x\n", pPoolCfg->ipAddr);
        return(L7_ALREADY_CONFIGURED);
      }
      else if(tempPool->poolData->ipAddr > pPoolCfg->ipAddr)
      {
        break;
      }
    }

    prevlink = link;
    link = link->next;
  }

  /* alloc node */
  *pool = (dhcpsPoolNode_t *) osapiMalloc(L7_DHCPS_MAP_COMPONENT_ID, sizeof(dhcpsPoolNode_t));
  if(L7_NULLPTR == *pool)
  {
    /* log memory alloc failed */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Unable to allocate memory for pool node!\n");
    return L7_ERROR;
  }

  /* set node and cfg info */
  memset(*pool, 0, sizeof(dhcpsPoolNode_t));
  (*pool)->poolData = pPoolCfg;
  (*pool)->poolsLink.object = *pool;

  dhcpsInitPoolOption((*pool)->optionFlag);

  dhcpsSetPoolOption(*pool); 


  /* insert in list in order of ipaddr */
  if(L7_NULLPTR == link)
  {
    /* insert at tail */
    dhcpsListInsertAfterLink(prevlink, &(*pool)->poolsLink);
  }
  else
  {
    /* insert before item with larger ipaddr */
    dhcpsListInsertBeforeLink(link, &(*pool)->poolsLink);
  }
  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Created pool node for ipaddr %x\n", 
    (*pool)->poolData->ipAddr);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroy the pool node
*
* @param    ppPool         @b{(input/output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsDestroyPool(dhcpsPoolNode_t * * ppPool)
{
  dhcpsLeaseNode_t  *pLease;
  dhcpsLink_t       *pLink;
  L7_uint32         ipAddr;

  if(L7_NULLPTR == ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to DestroyPool not provided!\n");
    return L7_FAILURE;
  }
  else if(L7_NULLPTR == *ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to DestroyPool is a NULL ptr\n");
    return L7_FAILURE;
  }

  ipAddr = (*ppPool)->poolData->ipAddr;

  /* remove from server pools list */
  if(L7_SUCCESS != dhcpsListRemoveLink( &((*ppPool)->poolsLink) ) )
  {
    /* log failure here */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Unable to delink pool node from active pools\n");
    return L7_ERROR;
  }

  /* get first item */
  pLink = ((*ppPool)->poolLeasesHead).next;

  /* deallocate pool leases */
  while(L7_NULLPTR != pLink)
  {
    pLease = (dhcpsLeaseNode_t *)pLink->object;
    
    /* deallocate lease and remove all refs! */
    if(dhcpsDeallocateLease(&pLease) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Unable to deallocate lease while destroying pool\n");
      return L7_ERROR;
    }

    /* get next : deallocate lease removes the deallocated lease 
       from pool list, so we keep using the first item on the list 
       until there are none! */
    pLink = ((*ppPool)->poolLeasesHead).next;
  }

  /* release pool config */
  memset((*ppPool)->poolData, 0, sizeof(poolCfgData_t));

  /* free pool node memory */
  osapiFree(L7_DHCPS_MAP_COMPONENT_ID, *ppPool);
  
  *ppPool = L7_NULLPTR;

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Destroyed pool for ipaddr %x\n", ipAddr);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Search a pool node by IP address
*
* @param    ipAddr         @b{(input)}         IP address
* @param    ppPool         @b{(input/output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsFindPoolByIp(L7_IP_ADDR_t ipAddr, dhcpsPoolNode_t * * ppPool)
{
  dhcpsLink_t     *pLink;
  dhcpsPoolNode_t *pPool;

  if(L7_NULLPTR == ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
                  "Argument Pool to FindPoolByIp is a NULL ptr\n");
    return L7_ERROR;
  }
  else
  {
    *ppPool = L7_NULLPTR;
  }

  /* get first item */
  pLink = (pDhcpsInfo->dhcpsPoolsHead).next;

  /* iterate pool leases */
  while(L7_NULLPTR != pLink)
  {
    pPool = (dhcpsPoolNode_t *)pLink->object;

    if(L7_NULLPTR != pPool)
    {      
      if(ipAddr == pPool->poolData->ipAddr)
      {
        /* matches */
        *ppPool = pPool;
        break;
      }
      else if(ipAddr < pPool->poolData->ipAddr)
      {
        /* pools are in ascending order of ipaddr, so no match for this ipaddr! */
        break;
      }
    }
    else
    {
      break;
    }
    
    /* get next */
    pLink = (pPool->poolsLink).next;
  }

  if(L7_NULLPTR == *ppPool)
    return L7_FAILURE;
  else 
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get a pool node by matching client subnet
*
* @param    ipAddr         @b{(input)}   IP address
* @param    pClientKey     @b{(input)}   Client key
* @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsMatchPool(L7_IP_ADDR_t ipAddr, dhcpsClientLeaseKey_t * pClientKey, dhcpsPoolNode_t * * ppPool)
{
  dhcpsPoolNode_t       *pTempPool = L7_NULLPTR;
  dhcpsClientLeaseKey_t poolKey; 

  *ppPool = L7_NULLPTR;
  
  /* get pool matching client subnet */
  dhcpsMatchFirstPoolForSubnet(ipAddr, &pTempPool);

  while(L7_NULLPTR != pTempPool)
  {
    if(MANUAL_POOL == pTempPool->poolData->poolType)
    {
      getClientKey(pTempPool->poolData->hostIdentifier, 
                   pTempPool->poolData->hostIdLength,
                   pTempPool->poolData->hostAddr, 
                   pTempPool->poolData->hostAddrtype, 
                   pTempPool->poolData->hostAddrLength, 
                   &poolKey);

      if(memcmp(pClientKey, &poolKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
      {
        *ppPool = pTempPool;

        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "Matched manual pool with addr %x for client incoming on %x\n",
          pTempPool->poolData->ipAddr, ipAddr);

        break;
      }
    }
    else
    {
      /* dynamic pool! this should only execute once (the first match
       * should be the dynamic pool for the incoming subnet, followed
       * by all matching manual pools
       */
      if(L7_NULLPTR == *ppPool)
      {
        *ppPool = pTempPool;
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
          "Found 2 dynamic pools %x and %x for addr %x\n",
          *ppPool, pTempPool, ipAddr);
      }
    }

    dhcpsMatchNextPoolForSubnet(ipAddr, &pTempPool);
  }

  if(*ppPool != L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Matches the subnet address against the pool for validity,
*           returns the first matching pool.
*
* @param    subnetAddr      @b{(input)}  Subnet address
* @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsMatchFirstPoolForSubnet(L7_IP_ADDR_t subnetAddr, dhcpsPoolNode_t * * ppPool)
{
  dhcpsLink_t     *pLink;
  dhcpsPoolNode_t *pPool;

  if(L7_NULLPTR == ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
                  "Argument Pool to MatchFirstPoolBySubnet is a NULL ptr\n");
    return L7_FAILURE;
  }
  else
  {
    *ppPool = L7_NULLPTR;
  }

  /* get first item */
  pLink = (pDhcpsInfo->dhcpsPoolsHead).next;

  /* iterate pool leases */
  while(L7_NULLPTR != pLink)
  {
    pPool = (dhcpsPoolNode_t *)pLink->object;

    if(L7_NULLPTR != pPool)
    {      
      /* does the subnet addr match the pool's subnet? */
      if((subnetAddr & pPool->poolData->ipMask) == 
         (pPool->poolData->ipAddr & pPool->poolData->ipMask) )
      {
        /* matches */
        *ppPool = pPool;
        break;
      }
      else if((subnetAddr & pPool->poolData->ipMask) < 
              (pPool->poolData->ipAddr & pPool->poolData->ipMask) )
      {
        /* pools are in ascending order of ipaddr, so no match for this ipaddr! */
        break;
      }
    }
    else
    {
      break;
    }
    
    /* get next */
    pLink = (pPool->poolsLink).next;
  }

  /* found? */
  if(L7_NULLPTR == *ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
                  "Subnet addr %x does not match any configured pool\n", subnetAddr);
    return L7_FAILURE;
  }
  else
  {
    return L7_SUCCESS;
  }
}
/*********************************************************************
* @purpose  Matches the subnet address against the pool for validity,
*           returns the next matching pool.
*
* @param    subnetAddr      @b{(input)}  Subnet address
* @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsMatchNextPoolForSubnet(L7_IP_ADDR_t subnetAddr, dhcpsPoolNode_t * * ppPool)
{
  dhcpsLink_t     *pLink;
  dhcpsPoolNode_t *pPool;

  if(L7_NULLPTR == ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to MatchNextPoolBySubnet is NULL\n");
    return L7_FAILURE;
  }
  else if(L7_NULLPTR == *ppPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to FindNextPoolBySubnet is a NULL ptr\n");
    return L7_FAILURE;
  }
  else
  {
    pPool   = *ppPool;
    *ppPool = L7_NULLPTR;
  }

  /* get first item */
  pLink = pPool->poolsLink.next;

  if(L7_NULLPTR != pLink)
  {      
    pPool = (dhcpsPoolNode_t *)pLink->object;

    /* does the subnetAddr belong to the pool's subnet? */
    if((subnetAddr & pPool->poolData->ipMask) == 
       (pPool->poolData->ipAddr & pPool->poolData->ipMask) )
    {
      /* matches */
      *ppPool = pPool;
    }
  }
    
  if(L7_NULLPTR == *ppPool)
    return L7_FAILURE;
  else 
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Search for the first lease node by pool
*
* @param    pPool      @b{(input)}   Pointer to the pool node
* @param    ppLease    @b{(output)}  Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsFindFirstLeaseByPool(dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease)
{
  dhcpsLink_t     *pLink;

  if(L7_NULLPTR == pPool || L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument(s) to FindFirstLeaseByPool is a NULL ptr\n");
    return L7_FAILURE;
  }

  *ppLease = L7_NULLPTR;

  /* get first link */
  pLink = (pPool->poolLeasesHead).next;

  if(L7_NULLPTR != pLink)
  {
    if(L7_NULLPTR != pLink->object)
    {
      /* set value */
      *ppLease = (dhcpsLeaseNode_t *) pLink->object;
    
      return L7_SUCCESS;
    }
  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
    "Could not find any leases for pool: %s\n", pPool->poolData->poolName);
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Search for the next lease node by pool
*
* @param    pLease         @b{(input)}   Pointer to the lease node
* @param    ppNextLease    @b{(output)}  Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsFindNextLeaseByPool(dhcpsLeaseNode_t * pLease, dhcpsLeaseNode_t * * ppNextLease)
{
  dhcpsLink_t     *pLink;

  if(L7_NULLPTR == pLease || L7_NULLPTR == ppNextLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument(s) to FindNextLeaseByPool is a NULL ptr\n");
    return L7_FAILURE;
  }

  *ppNextLease = L7_NULLPTR;

  pLink = (pLease->poolLeasesLink).next;

  if(L7_NULLPTR != pLink)
  {
    if(L7_NULLPTR != pLink->object)
    {
      /* set value */
      *ppNextLease = (dhcpsLeaseNode_t *) pLink->object;
    
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Search for the lease node by IP Address
*
* @param    ipAddr         @b{(input)}  IP Address
* @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsFindLeaseNodeByIpAddr(L7_IP_ADDR_t ipAddr, dhcpsLeaseNode_t * * ppLease)
{
  int i = 0;

  *ppLease = L7_NULLPTR;
  
  if(0 == ipAddr)
  {
    return L7_FAILURE;
  }
  
  for(i = 0; i < L7_DHCPS_MAX_LEASE_NUMBER; i++)
  {
    if(L7_NULLPTR != pDhcpsInfo->dhcpsLeaseNodes[i].leaseData)
    {
      if(pDhcpsInfo->dhcpsLeaseNodes[i].leaseData->ipAddr == ipAddr)
      {
        *ppLease = &(pDhcpsInfo->dhcpsLeaseNodes[i]);
        break;
      }
    }
  }
  
  if(L7_NULLPTR != (*ppLease))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Search for the lease node by IP Address
*
* @param    pPool          @b{(input)}  Pointer to the pool node
* @param    ipAddr         @b{(input)}  IP Address
* @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsFindLeaseByIp(dhcpsPoolNode_t * pPool, L7_IP_ADDR_t ipAddr, dhcpsLeaseNode_t * * ppLease)
{
  dhcpsLink_t       *pLink;
  dhcpsLeaseNode_t  *pLease;

  if(L7_NULLPTR == pPool || L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument(s) to FindLeaseByIp is a NULL ptr\n");
    return L7_FAILURE;
  }

  *ppLease = L7_NULLPTR;

  pLink = (pPool->poolLeasesHead).next;

  while(L7_NULLPTR != pLink)
  {
    pLease = (dhcpsLeaseNode_t *) pLink->object;

    if(L7_NULLPTR != pLease)
    {
      if(ipAddr == pLease->leaseData->ipAddr)
      {
        *ppLease = pLease;
        break;
      }
      else if(ipAddr < pLease->leaseData->ipAddr)
      {
        /* list is sorted in increasing order, so no match! */
        break;
      }
    }
    
    /* check for circular loops */
    if(pLink == pLink->next)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
                    "Error: circular loop in dhcpsFindLeaseByIp\n");
      *ppLease = L7_NULLPTR;
      break;
    }

    /* get next */
    pLink = pLink->next;
  }

  if(L7_NULLPTR != *ppLease)
  {
    return L7_SUCCESS;
  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Could not find any leases for ipAddr: %x\n", ipAddr); 

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Create lease node
*
* @param    index          @b{(input)}  index of the array
* @param    pPool         @b{(input)}   Pointer to the pool node
* @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsCreateLeaseNode(L7_uint32 index, dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease)
{
  dhcpsLink_t           *link;
  dhcpsLink_t           *prevlink;
  dhcpsLeaseNode_t      *tempLease;
  dhcpsLeaseNode_t      *pLease;
  dhcpsClientLeaseKey_t clientKey;

  /* sanity */
  if(L7_NULLPTR == pPool || L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument(s) to dhcpsAddLease is a NULL ptr\n");
    return L7_ERROR;
  }
  else
  {
    *ppLease = L7_NULLPTR;
  }

  /* init the lease node */
  pLease  = &(pDhcpsInfo->dhcpsLeaseNodes[index]);    

  memset(pLease, 0, sizeof(dhcpsLeaseNode_t));


  /* setup node */
  pLease->leaseData                   = &(pDhcpsMapLeaseCfgData->lease[index]);
  pLease->parentPool                  = pPool;
  pLease->poolLeasesLink.object       = pLease;
  pLease->scheduledLeasesLink.object  = pLease;
  pLease->clientLeasesLink.object     = pLease;

  /* iterate pools lease list */
  prevlink  = &(pPool->poolLeasesHead);
  link      = prevlink->next;
  while(L7_NULLPTR != link)
  {
    tempLease = (dhcpsLeaseNode_t *)link->object;
          
    if(tempLease->leaseData->ipAddr > pLease->leaseData->ipAddr)
    {
      break;
    }

    prevlink = link;
    link = link->next;
  }

  /* add lease to pool leases */
  if(L7_NULLPTR == link)
  {
    /* insert at tail */
    dhcpsListInsertAfterLink(prevlink, &(pLease->poolLeasesLink));
  }
  else
  {
    /* insert before item with larger ipaddr */
    dhcpsListInsertBeforeLink(link, &(pLease->poolLeasesLink));
  }

  /* get client key */
  getClientKey(pLease->leaseData->clientIdentifier, pLease->leaseData->clientIdLength,
               pLease->leaseData->hwAddr,
               pLease->leaseData->hAddrtype,  pLease->leaseData->hwAddrLength,
               &clientKey);

  /* add lease to client table */
  if(dhcpsInsertClientLeaseTableEntry(&clientKey, pLease) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "allocate lease failed for IP %x, could not insert client lease table entry\n", 
      pLease->leaseData->ipAddr);

    /* rollback all allocations */
    dhcpsListRemoveLink(&(pLease->poolLeasesLink));
    memset(pLease, 0, sizeof(dhcpsLeaseNode_t)); 
    pLease = L7_NULLPTR;

    return L7_FAILURE;
  }

  /* done! */
  *ppLease = pLease;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Find lease by IP Address, verify if it can be given, then allocate lease
*
* @param    pPool             @b{(input)}  Pointer to the pool node
* @param    ipAddr            @b{(input)}  IP Address
* @param    clientIdentifier  @b{(input)}  Client Identifier
* @param    clientIdLen       @b{(input)}  Client Identifier Length
* @param    hwAddr            @b{(input)}  Hardware Address
* @param    hAddrtype         @b{(input)}  Hardware Address Type
* @param    ppLease           @b{(output)} Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsAllocateLease(dhcpsPoolNode_t *  pPool, L7_IP_ADDR_t ipAddr, 
  L7_uchar8 *clientIdentifier, L7_uchar8 clientIdLen, L7_uchar8 *hwAddr, L7_uchar8 hAddrtype, L7_uchar8 hwAddrLength,
  dhcpsLeaseNode_t * * ppLease)
{
  L7_uint32             index;
  L7_uint32             victimNodeIndex;
  dhcpsLeaseNode_t      *pLease       = L7_NULLPTR;
  dhcpsLeaseNode_t      *pVictimLease = L7_NULLPTR;

  /* sanity */
  if(L7_NULLPTR == pPool || L7_NULLPTR == ppLease || L7_NULLPTR == hwAddr)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument(s) to Allocate Lease is a NULL ptr\n");
    return L7_FAILURE;
  }

  *ppLease = L7_NULLPTR;

  /* IP unique in pool? */
  if(dhcpsFindLeaseByIp(pPool, ipAddr, &pLease) == L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Allocate Failed! lease already allocated for ip %x\n", ipAddr);
    return L7_ALREADY_CONFIGURED;
  }  

  /* IP belongs to pool?
   * - mask IP for DYNAMIC pool
   * - compare direct for MANUAL
   */
  if(pPool->poolData->ipAddr != 
     (pPool->poolData->poolType == MANUAL_POOL ? ipAddr : 
     (ipAddr & pPool->poolData->ipMask)))
  {
    /* ip was of wrong subnet */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "Cannot allocate lease for IP %x. Does not match pool ipaddr %x\n", 
      ipAddr, pPool->poolData->ipAddr);
    return L7_FAILURE;
  }

  /* find a free cfg */
  index       = 0;
  victimNodeIndex = L7_DHCPS_MAX_LEASE_NUMBER;
  
  while(index < L7_DHCPS_MAX_LEASE_NUMBER)
  {
    if(0 == pDhcpsMapLeaseCfgData->lease[index].ipAddr)
    {
      break;
    }
    else
    {
      /* select oldest expired lease as victim if insufficient resources */
      if(EXPIRED_LEASE == pDhcpsMapLeaseCfgData->lease[index].state)
      {
        if(L7_NULLPTR != pDhcpsInfo->dhcpsLeaseNodes[index].leaseData)
        {
          if(victimNodeIndex < L7_DHCPS_MAX_LEASE_NUMBER &&
            L7_NULLPTR != pDhcpsInfo->dhcpsLeaseNodes[victimNodeIndex].leaseData)
          {
            /* index is used to cross ref into the Nodes array.
             * Code is based on the assumption that both arrays have
             * have max L7_DHCPS_MAX_LEASE_NUMBER items.
             */
            if(pDhcpsInfo->dhcpsLeaseNodes[index].leaseEndTime <
               pDhcpsInfo->dhcpsLeaseNodes[victimNodeIndex].leaseEndTime)
            {
              victimNodeIndex = index;
            }
          }
          else
          {
            victimNodeIndex = index;
          }
        }
      }
    }

    index++;
  }

  /* insufficent resources, but found victim? */
  if(index >= L7_DHCPS_MAX_LEASE_NUMBER && victimNodeIndex < L7_DHCPS_MAX_LEASE_NUMBER)
  {
    pVictimLease = &(pDhcpsInfo->dhcpsLeaseNodes[victimNodeIndex]);

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "Allocate recovering expired lease entry from ipaddr %x for ipaddr %x\n", 
      pDhcpsMapLeaseCfgData->lease[victimNodeIndex].ipAddr, ipAddr);

    if(dhcpsDeallocateLease(&pVictimLease) == L7_SUCCESS)
    {
      index = victimNodeIndex;
    }
  }
    

  /* found? */
  if(index < L7_DHCPS_MAX_LEASE_NUMBER)
  {
    /* setup lease cfg */
    pDhcpsMapLeaseCfgData->lease[index].ipAddr       = ipAddr;
    pDhcpsMapLeaseCfgData->lease[index].ipMask       = pPool->poolData->ipMask;
    pDhcpsMapLeaseCfgData->lease[index].hAddrtype    = hAddrtype;
    pDhcpsMapLeaseCfgData->lease[index].hwAddrLength = hwAddrLength;
    pDhcpsMapLeaseCfgData->lease[index].state        = FREE_LEASE;

    memcpy(pDhcpsMapLeaseCfgData->lease[index].hwAddr, hwAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);

    if(L7_NULLPTR != clientIdentifier && 0 != clientIdLen)
    {
      pDhcpsMapLeaseCfgData->lease[index].clientIdLength = clientIdLen;
      memcpy(pDhcpsMapLeaseCfgData->lease[index].clientIdentifier, clientIdentifier, clientIdLen);
    }

    (void)osapiStrncpySafe(pDhcpsMapLeaseCfgData->lease[index].poolName, pPool->poolData->poolName,L7_DHCPS_POOL_NAME_MAXLEN);

    /* add lease to client table and init lease node*/
    if(dhcpsCreateLeaseNode(index, pPool, &pLease) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "Allocate lease failed for IP %x, could not init lease node\n", 
        ipAddr);

      /* rollback all allocation of cfg element */
      memset(pLease->leaseData, 0, sizeof(leaseCfgData_t));

      return L7_FAILURE;
    }

    /* done! */
    *ppLease = pLease;
  }
  else
  {
    /* no free cfg elements */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "Cannot allocate lease for IP %x, Cfg array is full\n", ipAddr);
    return L7_TABLE_IS_FULL;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Deallocate lease
*
* @param    ppLease   @b{(input/output)} Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsDeallocateLease(dhcpsLeaseNode_t * * ppLease)
{
  L7_IP_ADDR_t ipAddr;

  if(L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease argument to Deallocate Lease is NULL\n");
    return L7_FAILURE;
  }
  else if(L7_NULLPTR == *ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease argument to Deallocate Lease is a NULL ptr\n");
    return L7_FAILURE;
  }

  ipAddr = (*ppLease)->leaseData->ipAddr;

  dhcpsTransitionLeaseState(*ppLease, FREE_LEASE);

  /* remove from pools list */
  dhcpsListRemoveLink(&(*ppLease)->poolLeasesLink);

  /* remove from scheduled list */
  dhcpsListRemoveLink(&(*ppLease)->scheduledLeasesLink);

  /* remove from client table */
  dhcpsRemoveClientLeaseTableEntry(*ppLease);
  
  /* deallocate cfg */
  memset((*ppLease)->leaseData, 0, sizeof(leaseCfgData_t));

  /* free lease node */
  memset((*ppLease), 0, sizeof(dhcpsLeaseNode_t));

  /* set out param */
  *ppLease = L7_NULLPTR;

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Deallocated lease for IP addr %x\n", ipAddr);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get Client Key 
*
* @param    pClientIdentifier  @b{(input)}  Pointer to the Client Identifier
* @param    clientIdLen        @b{(input)}  Length of the Client Identifier
* @param    pHwAddr            @b{(input)}  Pointer to the Hardware Address
* @param    hAddrtype          @b{(input)}  Hardware Address Type
* @param    hwAddrLength       @b{(input)}  Hardware Address Length 
* @param    pLeaseKey          @b{(output)} Pointer to the client lease key 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t getClientKey(L7_uchar8 * pClientIdentifier, L7_uchar8 clientIdLen, L7_uchar8 * pHwAddr, L7_uchar8 hAddrtype, L7_uchar8 hwAddrLength, dhcpsClientLeaseKey_t * pLeaseKey)
{
  /* blank out key */
  memset(pLeaseKey, 0, sizeof(dhcpsClientLeaseKey_t));
 
  if(hAddrtype == 6 && hwAddrLength == 1)
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "WARNING: hwAddrType & Length may be WRONG!!\n");

  if(L7_NULLPTR != pClientIdentifier)
  {
    if(clientIdLen != 0)
    {
      memcpy(pLeaseKey->keyData, pClientIdentifier, L7_DHCPS_CLIENT_ID_MAXLEN);
      pLeaseKey->keyType = DHCPS_CLIENTID_KEY_TYPE;

      return L7_SUCCESS;
    }
  }

  if(L7_NULLPTR != pHwAddr)
  {
    if(hwAddrLength > L7_DHCPS_HARDWARE_ADDR_MAXLEN)
      hwAddrLength = L7_DHCPS_HARDWARE_ADDR_MAXLEN;

    pLeaseKey->keyType = hAddrtype;
    memcpy(pLeaseKey->keyData, pHwAddr, hwAddrLength);

    return L7_SUCCESS;
  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unable to create client Key with NULL input\n");

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Schedule lease for the given number of seconds  
*
* @param    pLease          @b{(input/output)}  Pointer to the lease node
* @param    secs            @b{(input)}  Number of seconds for which lease is scheduled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsScheduleLease(dhcpsLeaseNode_t * pLease, L7_uint32 secs)
{
  dhcpsLink_t       *pLink       = L7_NULLPTR;
  dhcpsLink_t       *pTailLink   = L7_NULLPTR;
  dhcpsLeaseNode_t  *pTempLease  = L7_NULLPTR;

  if(L7_NULLPTR == pLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease argument to ScheduleLease is a NULL ptr\n");
    return L7_FAILURE;
  }
 
  if(L7_TRUE == pLease->leaseData->IsBootp)
  {
    /* bootp lease - do nothing! */
    return L7_SUCCESS;
  }
  
  /* setup time in seconds*/
  pLease->leaseStartTime = osapiUpTimeRaw();
  pLease->leaseEndTime = pLease->leaseStartTime + secs;
  
  pLink = pDhcpsInfo->dhcpsScheduledLeaseHead.next;

  while(L7_NULLPTR != pLink)
  {
    pTempLease = (dhcpsLeaseNode_t*)pLink->object;

    /* expires later? */
    if(pTempLease->leaseEndTime > pLease->leaseEndTime)
    {
      break;
    }

    pLink = pLink->next;
  }

  if(L7_NULLPTR != pLink)
  {
    dhcpsListInsertBeforeLink(pLink, &pLease->scheduledLeasesLink);
  }
  else
  {
    if(dhcpsListGetTail(&pDhcpsInfo->dhcpsScheduledLeaseHead, &pTailLink) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Error: could not get tail for scheduling lease\n");
    }
    else
    {
      dhcpsListInsertAfterLink(pTailLink, &pLease->scheduledLeasesLink);
    }
  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Scheduled lease IP addr %x at %u for expiry at %u\n",
    pLease->leaseData->ipAddr, 
    pLease->leaseStartTime, 
    pLease->leaseEndTime);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Unschedule lease 
*
* @param    pLease          @b{(input/output)}  Pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsUnscheduleLease(dhcpsLeaseNode_t * pLease)
{
  if(L7_NULLPTR == pLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease argument to UnscheduleLease is a NULL ptr\n");
    return L7_FAILURE;
  }
  
  if(dhcpsListRemoveLink(&pLease->scheduledLeasesLink) == L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unscheduled lease IP addr %x\n", 
      pLease->leaseData->ipAddr);

    return L7_SUCCESS;
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO, "Unable to Unscheduled lease for IP addr %x (May already be unscheduled)\n", 
      pLease->leaseData->ipAddr);

    return L7_FAILURE;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get expired leases 
*
* @param    ppExpiredLease  @b{(input/output)}  Pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dhcpsGetExpiredLease(dhcpsLeaseNode_t * * ppExpiredLease)
{
  L7_uint32         currTime;
  dhcpsLink_t       *pLink;
  dhcpsLeaseNode_t  *pTempLease;

  if(L7_NULLPTR == ppExpiredLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease argument to GetExpiredLease is a NULL ptr\n");
    return L7_FAILURE;
  }

  /* get current time in seconds */
  currTime = osapiUpTimeRaw();

  *ppExpiredLease = L7_NULLPTR;

  pLink = pDhcpsInfo->dhcpsScheduledLeaseHead.next;

  if(L7_NULLPTR != pLink)
  {
    pTempLease = (dhcpsLeaseNode_t*)pLink->object;

    /* expired? */
    if(pTempLease->leaseEndTime <= currTime)
    {      
      dhcpsListRemoveLink(pLink);
      *ppExpiredLease = pTempLease;

      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Lease expired for IP addr %x\n",
        (*ppExpiredLease)->leaseData->ipAddr);
    }    
  }

  if(L7_NULLPTR != *ppExpiredLease)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
  
}
/*********************************************************************
* @purpose  Create the client lease table using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsClientLeaseTableCreate(void)
{
  L7_uint32     avlType;

  avlType = 0x10;   /* common constant for all */


  /* delete the AVL tree */
  dhcpsClientLeaseTableDelete();

  (pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeEntryMax = L7_DHCPS_MAX_LEASE_NUMBER;

  memset((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeHeap, 
    0, sizeof((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeHeap));
  memset((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseDataHeap, 
    0, sizeof((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseDataHeap));
  memset(&((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData), 
    0, sizeof((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData));

  avlCreateAvlTree(&((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData), 
                   (pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeHeap, 
                   (pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseDataHeap,
                   (pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeEntryMax, 
                   sizeof(dhcpsClientLeaseNode_t), avlType,
                   sizeof(dhcpsClientLeaseKey_t));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Insert an entry into the DHCPS Client lookup table
*
* @param    pClientKey  @b{(input)} client key
* @param    pLease      @b{(input)} client lease
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsInsertClientLeaseTableEntry(dhcpsClientLeaseKey_t * pClientKey, dhcpsLeaseNode_t * pLease)
{
  dhcpsClientLeaseNode_t  *pNode;
  avlTree_t               *pTree;
  dhcpsClientLeaseNode_t  tempTableEntry;
  dhcpsLink_t             *pLink;
  L7_BOOL                 isDuplicate;
  dhcpsClientLeaseKey_t   leaseKey;

  if(pLease == L7_NULLPTR || pClientKey == L7_NULLPTR)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
      "Argument(s) to InsertClientLeaseTableEntry is a NULL ptr\n");

    return L7_FAILURE;
  }

  /* delink lease from client leases link */
  dhcpsListRemoveLink(&pLease->clientLeasesLink);

  /* setup temp table entry with key and lease*/
  memset(&tempTableEntry, 0, sizeof(dhcpsClientLeaseNode_t));
  memcpy(&tempTableEntry.key, pClientKey, sizeof(dhcpsClientLeaseKey_t));

  /* try to insert */
  pTree = &((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData);
  pNode = (dhcpsClientLeaseNode_t *) avlInsertEntry(pTree, &tempTableEntry);

  if(pNode == &tempTableEntry)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
      "Error adding entry to table for lease IP %x\n", 
      LEASE_DATA(pLease)->ipAddr);    

    return L7_FAILURE;                  /* node not inserted in table */    
  }
  else if(pNode != L7_NULL)
  {
    /* entry exists */

    /* add lease to list (TBD check for duplicates!) */
    pLink = &pNode->clientLeaseNodes;
    isDuplicate = L7_FALSE;
    while(L7_NULLPTR != pLink)
    {
      if(pLink->object == pLease)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Entry %x already in client leases table link list\n",
          pLease->leaseData->ipAddr); 
        isDuplicate = L7_TRUE;
        break;
      }
      else if(pLink->next == pLink)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, 
          "Error: circular entry in client leases table\n"); 
        return L7_FAILURE;
      }

      pLink = pLink->next;
    } 

    if(L7_FALSE == isDuplicate)
    {
      if(dhcpsListInsertAfterLink(&pNode->clientLeaseNodes, 
        &pLease->clientLeasesLink) == L7_SUCCESS)
      { 
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Entry %x added in client leases table link list\n",
          pLease->leaseData->ipAddr); 
      } 
      else
      { 
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Error: Failed to add entry %x in client leases table link list\n",
          pLease->leaseData->ipAddr); 
      } 
    }
  }
  else
  {
    /* setup temp table entry with key and lease*/
    memset(&leaseKey, 0, sizeof(dhcpsClientLeaseKey_t));
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype, 
                LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

    /* try to find new node */
    pTree = &((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData);
    pNode = (dhcpsClientLeaseNode_t *) avlSearchLVL7(pTree, &leaseKey, L7_MATCH_EXACT);

    if (pNode != L7_NULL)
    {
      if(dhcpsListInsertAfterLink(&pNode->clientLeaseNodes, 
                                  &pLease->clientLeasesLink) == L7_SUCCESS)
      { 
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Added entry to table for lease IP %x\n",
                      pLease->leaseData->ipAddr);
      } 
      else
      { 
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Error: Failed in adding entry %x to client leases table link list\n",
          pLease->leaseData->ipAddr); 
      }
    } 
    else
    { 
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
          "Error: Failed to find newly added entry %x in client leases table\n",
          pLease->leaseData->ipAddr); 
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Delete an entry from the DHCPS Client lookup table
*
* @param    pLease       @b{(input)} Pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsRemoveClientLeaseTableEntry(dhcpsLeaseNode_t * pLease)
{
  avlTree_t               *pTree;
  dhcpsClientLeaseNode_t  *pNode;
  dhcpsLink_t             *pLink;
  dhcpsClientLeaseKey_t   leaseKey;

  if(pLease == L7_NULLPTR)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
      "Argument(s) to RemoveClientLeaseTableEntry is a NULL ptr\n");

    return L7_FAILURE;
  }

  /* setup temp table entry with key and lease*/
  memset(&leaseKey, 0, sizeof(dhcpsClientLeaseKey_t));
  getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype, 
                LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

  /* try to remove */
  pTree = &((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData);
  pNode = (dhcpsClientLeaseNode_t *) avlSearchLVL7(pTree, &leaseKey, L7_MATCH_EXACT);

  if (pNode == L7_NULL)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "Lease %x entry removal failed, client not found in table\n", 
      pLease->leaseData->ipAddr);

    return L7_FAILURE;                  /* node not found in table */
  }

  /* remove the lease from the client list */
  pLink = pNode->clientLeaseNodes.next;

  while(pLink != L7_NULLPTR)
  {
    if(pLease == (dhcpsLeaseNode_t *)pLink->object)
    {
      break;
    }

    pLink = pLink->next;
  }

  /* found? */
  if(pLink != L7_NULLPTR)
  {
    /* remove from list */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "Lease %u entry being removed from client table list prev %u next %u\n",
      pLink->object, pLink->prev, pLink->next);
    dhcpsListRemoveLink(pLink);
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "Lease %x entry removal failed, not found in client list\n", 
      pLease->leaseData->ipAddr);

    return L7_FAILURE;    /* lease not in client list */
  }

  /* delete entry if no leases left */
  if(dhcpsListGetCount(&pNode->clientLeaseNodes) == 0)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Removing client table entry as there are no Leases\n");
    avlDeleteEntry(pTree, pNode);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Find the lease of the client by client key 
*
* @param    pClientKey  @b{(input)} Pointer to the client key
* @param    pPool       @b{(input)} Pointer to the pool node
* @param    ppLease     @b{(output)} Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsFindClientLease(dhcpsClientLeaseKey_t * pClientKey, dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease)
{
  dhcpsClientLeaseNode_t  *pNode;
  dhcpsLink_t             *pLink;
  dhcpsLeaseNode_t        *pTempLease;

  if(ppLease == L7_NULLPTR || pClientKey == L7_NULLPTR)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
      "Argument(s) to FindClientLeaseTableEntry is a NULL ptr\n");

    return L7_FAILURE;
  }

  *ppLease = L7_NULLPTR;

  /* search */
  if(dhcpsClientLeaseTableSearch(pClientKey, &pNode) == L7_SUCCESS)
  {
    pLink = pNode->clientLeaseNodes.next; 
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "FindClientLease did not find a lease entry in the table\n");
    return L7_FAILURE;
  }

  if (pLink == L7_NULLPTR)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "FindClientLease found an entry without leases\n");
    return L7_FAILURE;
  }

  if(L7_NULLPTR == pPool)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "FindClientLease (no pool matching) found an entry\n");
    *ppLease = (dhcpsLeaseNode_t *)pLink->object;
  }
  else
  {
    /* find lease from specified pool */
    while(L7_NULLPTR != pLink)
    {
      pTempLease = (dhcpsLeaseNode_t *)pLink->object;
  
      if(L7_NULLPTR != pTempLease)
      {
        if(pTempLease->parentPool == pPool)
        {
          *ppLease = pTempLease;
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
            "FindClientLease (with pool matching) found an entry\n");
          break;
        }
      }

      if(pLink == pLink->next)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "FindClientLease: Error circular loop detected\n");
        break;
      }

      pLink = pLink->next; 
    }
  }

  if (*ppLease == L7_NULLPTR)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Search for the client in the client table by client key 
*
* @param    pClientKey    @b{(input)} Pointer to the client key
* @param    ppClientNode  @b{(input)} Pointer to pointer to the client lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsClientLeaseTableSearch(dhcpsClientLeaseKey_t * pClientKey, dhcpsClientLeaseNode_t **ppClientNode)
{
  avlTree_t              *pTree;
  dhcpsClientLeaseNode_t  tempTableEntry;

  if(ppClientNode == L7_NULLPTR || pClientKey == L7_NULLPTR)
    return L7_FAILURE;

  /* setup temp table entry with key */
  memset(&tempTableEntry, 0, sizeof(dhcpsClientLeaseNode_t));
  memcpy(&(tempTableEntry.key), pClientKey, sizeof(dhcpsClientLeaseKey_t));
  
  pTree = &((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData);
  *ppClientNode = (dhcpsClientLeaseNode_t *) avlSearchLVL7(pTree, &tempTableEntry.key, AVL_EXACT);

  if (*ppClientNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */
  else
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the client lease table 
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsClientLeaseTableDelete(void)
{
  if(((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData).semId)
  {
    avlDeleteAvlTree(&((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData));
    ((pDhcpsInfo->dhcpsClientLeaseTable).dhcpsClientLeaseTreeData).semId = 0;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find next lease of the client 
*
* @param    pPool    @b{(input)} Pointer to the pool node 
* @param    ppLease  @b{(output)} Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsFindNextLeaseByClient(dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease)
{
  dhcpsLink_t      *pLink;
  dhcpsLeaseNode_t *pLease;
  dhcpsLeaseNode_t *pPrevLease;

  if(L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to FindNextClientLeaseTableEntry is NULL\n");
    return L7_FAILURE;
  }
  else if(L7_NULLPTR == *ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Argument to FindNextClientLeaseTableEntry is a NULL ptr\n");
    *ppLease    = L7_NULLPTR;
    return L7_FAILURE;
  }
  else
  {
    pPrevLease  = *ppLease;
    *ppLease    = L7_NULLPTR;
  }
 
  
  /* get next */
  pLink = pPrevLease->clientLeasesLink.next;
  if(L7_NULLPTR == pLink)
    return L7_FAILURE;

  if(L7_NULLPTR == pPool)
  {
    *ppLease = (dhcpsLeaseNode_t *)pLink->object;
  }
  else
  {
    /* find lease from specified pool */
    while(L7_NULLPTR != pLink)
    {
      pLease = (dhcpsLeaseNode_t *)pLink->object;
  
      if(L7_NULLPTR != pLease)
      {
        if(pLease->parentPool == pPool)
        {
          *ppLease = pLease;
          break;
        }
      }

      pLink = pLink->next; 
    }
  }

  if(L7_NULLPTR == *ppLease)
    return L7_FAILURE;
  else 
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get pool index
*
* @param    poolName   @b{(input)}  Pool name
* @param    poolIndex  @b{(output)} Pointer to pool index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsGetPoolIndex(L7_char8 *poolName, L7_uint32 *poolIndex)
{
  L7_uint32 i;

   for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
   {
    if( strncmp(pDhcpsMapCfgData->pool[i].poolName,poolName,
                                     L7_DHCPS_POOL_NAME_MAXLEN) == 0)
     {
        *poolIndex = i;
        return L7_SUCCESS;
     }
   }

    *poolIndex = -1;
    return L7_NOT_EXIST;

}
/*********************************************************************
* @purpose  Check if the specified ip address & subnet mask conflicts 
*           with the network or service port configuration and whether
*           the ip is already configured on another interface
*
*
* @param    ipAddr   @b{(input)}  IP Address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsIsConflictingIp(L7_uint32 ipAddr)
{
  L7_uint32 ipSvrAddr;

#ifdef L7_ROUTING_PACKAGE
  L7_uint32 intIfNum = 0;
#endif

  /* 
  ** Check if the specified ip address & subnet mask conflicts with the network
  ** or service port configuration
  */
  ipSvrAddr = simGetSystemIPAddr();  

  if(ipAddr == ipSvrAddr) 
  { 
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "ipaddr %x conflicts with system ip\n", ipAddr);
    return L7_TRUE;
  }

  ipSvrAddr = simGetServPortIPAddr();  
  if(ipAddr == ipSvrAddr) 
  { 
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "ipaddr %x conflicts with service port ip\n", ipAddr);
    return L7_TRUE;
  }
#ifdef L7_ROUTING_PACKAGE
  else if(ipMapIpAddressToIntf(ipAddr, &intIfNum) == L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, 
        "ipaddr %x conflicts with router ip on intf %u\n", 
        ipAddr, intIfNum);
    return L7_TRUE;
  }
#endif

  return L7_FALSE;
}


/*********************************************************************
* @purpose  Validate subnet
*
* @param    subnetIp     @b{(input)}  Subnet Address
* @param    subnetMask   @b{(input)}  Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsValidateSubnet(L7_uint32 subnetIp, L7_uint32 subnetMask)
{
  L7_uint32 ipAddr, mask;
  L7_uint32 i;

  if( (subnetIp & subnetMask) == 0 )
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "Pool Validation Failed! Pool ipAddr & mask cannot be null!\n");
  
    return L7_FAILURE;
  }

  if( (subnetIp & subnetMask) != subnetIp )
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "Pool Validation Failed! Pool ipAddr %x must be the network address %x!\n", 
      subnetIp, (subnetIp & subnetMask));
  
    return L7_FAILURE;
  }

  /* Get ipaddress of each pool and compare with ip */
  for(i= 0  ; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    if(( *pDhcpsMapCfgData->pool[i].poolName != L7_NULL) &&
       (pDhcpsMapCfgData->pool[i].poolType == DYNAMIC_POOL) ) 
    {
      ipAddr =  pDhcpsMapCfgData->pool[i].ipAddr;
      mask =  pDhcpsMapCfgData->pool[i].ipMask;

      if((ipAddr != L7_NULL_IP_ADDR)  &&
         (subnetIp & subnetMask & mask ) == ( ipAddr & subnetMask & mask ))
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "Pool Validation Failed! Pool %x and existing pool %x overlap\n",
          subnetIp, ipAddr);
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Find scheduled lease by IP
*
* @param    ipAddr       @b{(input)}  IP Address
* @param    ppLease      @b{(output)} Pointer to pointer to the lease node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsFindScheduledLeaseByIp(L7_uint32 ipAddr, dhcpsLeaseNode_t **ppLease )
{
  L7_RC_t           rc; 
  dhcpsLink_t       *pLink;
  dhcpsLeaseNode_t  *pTempLease;

  if(L7_NULLPTR == ppLease)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Lease pointer to active lease is a NULL ptr\n");
    return L7_FAILURE;
  }
 
  pLink = pDhcpsInfo->dhcpsScheduledLeaseHead.next;

  rc = L7_ERROR;
  while(L7_NULLPTR != pLink)
  {
    pTempLease = (dhcpsLeaseNode_t*)pLink->object;

    /* matches with given ip? */
    if(pTempLease->leaseData->ipAddr == ipAddr ) 
    {
      *ppLease = pTempLease;
      rc = L7_SUCCESS;
      break;
    }

    pLink = pLink->next;
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if an IP Address is configured on the pool
*
* @param    pPool             @b{(input)}  Pool node
* @param    ipAddr            @b{(input)}  IP Address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsCheckIfConfigured(dhcpsPoolNode_t * pPool, L7_IP_ADDR_t ipAddr)
{
  L7_uint32 index = 0;
  L7_uint32 i = 0;
  L7_uchar8* pPackArray = POOL_DATA(pPool)->options;
  dhcpsPackOption_t *pCurrTLV = (dhcpsPackOption_t *)pPackArray;

  /* check against default routers */
  for(index = 0; index < L7_DHCPS_DEFAULT_ROUTER_MAX; index++)
    if(ipAddr == POOL_DATA(pPool)->defaultrouterAddrs[index])
      return L7_TRUE;

  /* check against DNS servers */
  for(index = 0; index < L7_DHCPS_DNS_SERVER_MAX; index++)
    if(ipAddr == POOL_DATA(pPool)->DNSServerAddrs[index])
      return L7_TRUE;

  /* check against netBiosNameServer */
  for(index = 0; index < L7_DHCPS_NETBIOS_NAME_SERVER_MAX; index++)
    if(ipAddr == POOL_DATA(pPool)->netBiosNameServer[index])
      return L7_TRUE;

  /* check against next server */
  if(ipAddr == POOL_DATA(pPool)->nextServer)
    return L7_TRUE;

  /* iterate TLV options */
  index = 0;
  while(pCurrTLV->code != 0  && index < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    /* configured ipaddr? */
    if(4 == pCurrTLV->code || /* Time Server Option */
       7 == pCurrTLV->code || /* Log Server Option */
       8 == pCurrTLV->code || /* Cookie Server Option */
       9 == pCurrTLV->code || /* LPR Server Option */
       10 == pCurrTLV->code || /* Impress Server Option */
       11 == pCurrTLV->code || /* Resource Location Server Option */
       16 == pCurrTLV->code || /* Swap Server */
       32 == pCurrTLV->code || /* Router Solicitation Address Option */
       41 == pCurrTLV->code || /* Network Information Servers Option */
       42 == pCurrTLV->code || /* Network Time Protocol Servers Option */
       45 == pCurrTLV->code || /* NetBIOS over TCP/IP Datagram Distribution Server Option */
       48 == pCurrTLV->code || /* X Window System Font Server Option */
       49 == pCurrTLV->code || /* X Window System Display Manager Option */
       65 == pCurrTLV->code || /* Network Information Service+ Servers Option */
       68 == pCurrTLV->code || /* Mobile IP Home Agent option */
       69 == pCurrTLV->code || /* Simple Mail Transport Protocol (SMTP) Server Option */
       70 == pCurrTLV->code || /* Post Office Protocol (POP3) Server Option */
       71 == pCurrTLV->code || /* Network News Transport Protocol (NNTP) Server Option */
       72 == pCurrTLV->code || /* Default World Wide Web (WWW) Server Option */
       73 == pCurrTLV->code || /* Default Finger Server Option */
       74 == pCurrTLV->code || /* Default Internet Relay Chat (IRC) Server Option */
       75 == pCurrTLV->code || /* StreetTalk Server Option */
       76 == pCurrTLV->code /* StreetTalk Directory Assistance (STDA) Server Option */
       )
    {
      /* test configured addrs */
      for(i = 0; i < PACKTLV_GET_LENGTH(pCurrTLV); i += 4)
      {
        if(memcmp(&ipAddr, &pCurrTLV->value[i], 4)==0)
          return L7_TRUE;
      }
    }
    else if(33 == pCurrTLV->code) /* Static Route Option */
    {
      /* test router addrs in d.d.d.d, r.r.r.r ... */
      for(i = 4; i < PACKTLV_GET_LENGTH(pCurrTLV); i += 8)
      {
	  if(memcmp(&ipAddr, &pCurrTLV->value[i], 4)==0)
	    return L7_TRUE;
      }
    }

    /* get next */
    index += PACKTLV_SIZE(pCurrTLV);
    pCurrTLV = (dhcpsPackOption_t *) &pPackArray[index];
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check if an IP Address is excluded
*
* @param    ipAddr            @b{(input)}  IP Address
* @param    isManualExcluded  @b{(input)}  Flag to consider manual as excluded
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsCheckIfExcluded(L7_IP_ADDR_t ipAddr, L7_BOOL isManualExcluded)
{
  dhcpsPoolNode_t *pPool = L7_NULLPTR;
  L7_uint32 i;

  /* check for conflicts with switch */
  if(dhcpsIsConflictingIp(ipAddr) == L7_TRUE)
    return L7_TRUE;
  
  /* check for manual pools */
  if(L7_TRUE == isManualExcluded)
    if(dhcpsFindPoolByIp(ipAddr, &pPool) == L7_SUCCESS)
      return L7_TRUE;
      
  /* check against excluded addrs list */
  for(i=0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if(pDhcpsMapCfgData->excludedAddrs[i].fromAddr >0 &&
       ipAddr >= pDhcpsMapCfgData->excludedAddrs[i].fromAddr &&
       ipAddr <= pDhcpsMapCfgData->excludedAddrs[i].toAddr)
        return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Initialise Option data
*
* @param    optionFlag            @b{(input)}  List of options.
*
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsInitPoolOption(L7_BOOL  *optionFlag)
{
  /* Array of option to be used during option field packing */
  memset(optionFlag, 0, sizeof(L7_BOOL)*L7_DHCPS_OPTION_CODE_MAX );

  optionFlag[DHCPS_OPTION_TYPE_SUBNET_MASK] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_ROUTER] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_DNS_SERVER] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_HOST_NAME] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_DOMAINNAME] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_BROADCAST_ADDR] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_NETBIOSNAMESERVER] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_NETBIOSNODETYPE] = L7_TRUE;
  optionFlag[DHCPS_OPTION_TYPE_BOOTFILE_NAME] = L7_TRUE;
}

/*********************************************************************
* @purpose  Find the tlv option from buffer
*
* @param    pPackArray    @b{(input)}  DHCPS option array.
* @param    code          @b{(input)}  DHCPS option type.
* @param    ppTLV         @b{(output)} DHCPS option ptr.
* @param    pIndex        @b{(output)} Index of option in array.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsFindTLVOption(L7_uchar8* pPackArray, L7_uchar8 code, dhcpsPackOption_t * * ppTLV, L7_uint32 * pIndex)
{
  L7_uint32 index = 0;
  dhcpsPackOption_t *pCurrTLV = (dhcpsPackOption_t *)pPackArray;

  *ppTLV  = L7_NULLPTR;
  *pIndex = 0;

  while(pCurrTLV->code != 0  && index < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    if(code == pCurrTLV->code) 
    {
      *ppTLV  = pCurrTLV;
      *pIndex = index;
      return L7_SUCCESS ;
    }
    else
    { 
      /* get next */
      index += PACKTLV_SIZE(pCurrTLV);
      pCurrTLV = (dhcpsPackOption_t *) &pPackArray[index];
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the tlv data from buffer
*
* @param    pPackArray    @b{(input)}  DHCPS option array.
* @param    code          @b{(input)}  DHCPS option type.
* @param    buffer        @b{(output)} DHCPS option value.
* @param    pLength       @b{(output)} DHCPS option length.
* @param    pDataFormat   @b{(output)} DHCPS stored data format.
* @param    pStatus       @b{(output)} DHCPS data option status.
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsGetTLVOption( L7_uchar8* pPackArray, L7_uchar8 code, 
              L7_uchar8* buffer, L7_uint32* pLength, 
              L7_uchar8* pDataFormat, L7_uchar8 *pStatus)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 index = 0;
  dhcpsPackOption_t * pTLV = L7_NULLPTR;

  if(dhcpsFindTLVOption(pPackArray, code, &pTLV, &index) == L7_SUCCESS)
  {
    *pStatus     = pTLV->status; 
    *pDataFormat = pTLV->format;
    *pLength     = PACKTLV_GET_LENGTH(pTLV);

    if(0 != *pLength) 
      memcpy(buffer, pTLV->value, *pLength); 

    rc = L7_SUCCESS;
  }
  
  return rc;
}

/*********************************************************************
* @purpose  Set the tlv data from buffer
*
* @param    pPackArray     @b{(output)}  DHCPS option array.
* @param    code           @b{(input)}  DHCPS option type.
* @param    status         @b{(input)}  DHCPS option status.
* @param    buffer         @b{(input)} DHCPS option value.
* @param    length         @b{(input)} DHCPS option length.
* @param    dataFormat     @b{(input)} DHCPS stored data format.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSetTLVOption( L7_uchar8* pPackArray, L7_uchar8 code, 
    L7_uchar8 status, L7_uchar8* data, L7_uint32 length, L7_uchar8 dataFormat)
{
  L7_uint32 i=0;
  L7_uchar8 tempBuff[L7_DHCPS_POOL_OPTION_BUFFER];

  dhcpsPackOption_t *pTLV = (dhcpsPackOption_t *) pPackArray;
  dhcpsPackOption_t *pNewTLV = (dhcpsPackOption_t *) tempBuff;

  /* init new TLV */
  pNewTLV->code   = code;
  pNewTLV->status = status;
  pNewTLV->format = dataFormat;
  PACKTLV_SET_LENGTH(pNewTLV, length);
  if(0 != length)
    memcpy(pNewTLV->value, data, length);

  /* delete if already present */
  dhcpsTLVOptionDelete(pPackArray, code);

  /* go to end */
  while(pTLV->code != 0 && i < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    /* get next */
    i += PACKTLV_SIZE(pTLV);
    pTLV = (dhcpsPackOption_t *) &pPackArray[i];
  }

  /* exceeded buffer? */
  if( (i + PACKTLV_SIZE(pNewTLV)) > L7_DHCPS_POOL_OPTION_BUFFER)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "TLV could not be set for %d. Too long\r\n", code);
    return L7_FAILURE;
  }

  /* copy across */
  memcpy(&pPackArray[i], (L7_uchar8 *)pNewTLV, PACKTLV_SIZE(pNewTLV));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create TLV option row
*
* @param    pPackArray     @b{(input)}  DHCPS option array.
* @param    code           @b{(input)}  DHCPS option type.
*
* @returns  L7_SUCCESS
* @returns  L7_ALREADY_CONFIGURED
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTLVOptionCreate( L7_uchar8* pPackArray, L7_uchar8 code)
{
  dhcpsPackOption_t *pTLV = L7_NULLPTR;
  L7_uchar8 buf[1];
  L7_uint32 index = 0;

  if(dhcpsFindTLVOption(pPackArray, code, &pTLV, &index) == L7_SUCCESS)
    return L7_ALREADY_CONFIGURED;

  if(dhcpsSetTLVOption(pPackArray, code, L7_DHCPS_NOT_READY, 
                       buf, 0, L7_DHCPS_NONE) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the first TLV code
*
* @param    pPackArray     @b{(input)}  DHCPS option array.
* @param    pCode          @b{(output)}  DHCPS option type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTLVOptionGetFirst( L7_uchar8* pPackArray, L7_uchar8* pCode)
{
  L7_uint32 index = 0;
  L7_uchar8 minCode = 0;
  dhcpsPackOption_t *pCurrTLV = (dhcpsPackOption_t *)pPackArray;

  /* set first */
  minCode = pCurrTLV->code;

  /* iterate all */
  while(pCurrTLV->code != 0  && index < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    /* save min code */
    if(pCurrTLV->code < minCode)
      minCode = pCurrTLV->code;

    /* get next */
    index += PACKTLV_SIZE(pCurrTLV);
    pCurrTLV = (dhcpsPackOption_t *) &pPackArray[index];
  }

  *pCode = minCode;

  if(0 != *pCode)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next TLV option code
*
* @param    pPackArray     @b{(input)}  DHCPS option array.
* @param    prevCode       @b{(input)}  DHCPS option type.
* @param    pNextCode      @b{(output)}  DHCPS option type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTLVOptionGetNext( L7_uchar8* pPackArray, L7_uchar8 prevCode, 
        L7_uchar8* pCode)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 index = 0;
  L7_uchar8 nextCode = 0;
  dhcpsPackOption_t *pCurrTLV = (dhcpsPackOption_t *)pPackArray;

  *pCode = 0;

  /* iterate all items */
  while(pCurrTLV->code != 0  && index < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    if(pCurrTLV->code > prevCode)
    {
      /* save least larger value! */
      if(0 == nextCode || pCurrTLV->code < nextCode)
      {
        nextCode = pCurrTLV->code;
        rc = L7_SUCCESS;
      }
    }

    /* get next */
    index += PACKTLV_SIZE(pCurrTLV);
    pCurrTLV = (dhcpsPackOption_t *) &pPackArray[index];
  }

  if(L7_SUCCESS == rc)
  {
    *pCode = nextCode;
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete the tlv option field
*
* @param    pPackArray     @b{(input)}  DHCPS option array.
* @param    code           @b{(input)}  DHCPS option type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTLVOptionDelete( L7_uchar8* pPackArray, L7_uchar8 code)
{
  L7_uint32 oldIndex      = 0;
  L7_uint32 oldSize       = 0;
  L7_uint32 nextIndex     = 0;
  L7_uint32 remSize       = 0;
  L7_uchar8 tempBuf[L7_DHCPS_POOL_OPTION_BUFFER];
  dhcpsPackOption_t *pTLV = L7_NULLPTR;
  

  /* find tlv*/
  if(dhcpsFindTLVOption(pPackArray, code, &pTLV, &oldIndex) == L7_SUCCESS)
  {
    oldSize  = PACKTLV_SIZE(pTLV);
    nextIndex = oldIndex + oldSize;
    remSize  = L7_DHCPS_POOL_OPTION_BUFFER - nextIndex;

    memset(tempBuf, 0, L7_DHCPS_POOL_OPTION_BUFFER);

    /* make temp copy */ 
    memcpy(tempBuf, &pPackArray[nextIndex], remSize);
    
    /* clear */
    memset(&pPackArray[oldIndex], 0, oldSize + remSize);

    /* copy back */
    memcpy(&pPackArray[oldIndex], tempBuf, remSize);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose Set the option flag for the configured option in pool node
*
* @param    pPoolNode @b{(input)}  DHCPS pool node.
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsSetPoolOption(dhcpsPoolNode_t * pPoolNode)
{
  L7_uchar8 * pPackArray = pPoolNode->poolData->options;
  dhcpsPackOption_t* pTLV = (dhcpsPackOption_t *)pPackArray;
  L7_uint32 optionFlagIndex;
  L7_uint32 i=0;

  while(pTLV->code != 0 && i < L7_DHCPS_POOL_OPTION_BUFFER)
  {
    if ( pTLV->status == L7_DHCPS_ACTIVE )
    {
      optionFlagIndex = pTLV->code;
      (pPoolNode)->optionFlag[optionFlagIndex] = L7_TRUE;
    }
    /* get next */
    i += PACKTLV_SIZE(pTLV);
    pTLV = (dhcpsPackOption_t *) &pPackArray[i];
  }
}



/*********************************************************************
* @purpose  Get the strlen where maximum length of a string is fixed
*
* @param    L7_uchar8     @b{(input)}  Binary String
* @param    maxLen        @b{(input)}  Maximum possible length
*
* @returns  L7_uint32
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dhcpsStrnlen(L7_uchar8 * pStr, L7_uint32 maxLen)
{
  L7_uint32 i = 0;

  if(L7_NULLPTR == pStr)
    return 0;

  for(i = 0; (*pStr != L7_NULL) && (i < maxLen); pStr++, i++); 

  return i;
}

/*********************************************************************
* @purpose  Convert a binary string to Hex
*
* @param    binData         @b{(input)} Binary Data
* @param    binDataLength   @b{(input)} Binary Data Length in bytes
* @param    hexStrLength    @b{(input)} Output String Length in bytes
* @param    hexStr          @b{(output)} Hex String
*
* @returns  bytes converted
*
* @notes    conversion is restricted to hexStrLength
*
* @end
*********************************************************************/
L7_uint32 dhcpsBin2Hex(L7_uchar8 * binData, L7_uint32 binDataLength, L7_uint32 hexStrLength, L7_char8 * hexStr)
{
  L7_uint32 counter;
  L7_char8 temp[5], firstChar, lastChar;

  if(!hexStr || !hexStrLength || !binData || !binDataLength)
    return 0;

  memset(hexStr, 0, hexStrLength);

  for (counter=0 ; counter < binDataLength && hexStrLength > 2; counter++)
  {
    memset(temp, 0x00, sizeof(temp));
    firstChar = (binData[counter] & 0xF0) >> 4;
    lastChar  = binData[counter] & 0x0F;
    sprintf(temp,"%x%x", firstChar, lastChar);
    hexStrLength -= 2;

    if ((counter%2) == 1)
    {
      if (counter != (binDataLength-1))
      {
        strcat(temp,":");
        hexStrLength--;
      }
    }

    strcat(hexStr, temp);
  }

  return counter;
}

