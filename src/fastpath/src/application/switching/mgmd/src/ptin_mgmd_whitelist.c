/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    14/01/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/

#include "ptin_mgmd_whitelist.h"
#include "ptin_mgmd_cnfgr.h"
#include "ptin_mgmd_osapi.h"
#include "ptin_mgmd_avl_api.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_logger.h"


/**
 * Create a new AVL tree to hold the white-list entries.
 * 
 * @return RC_t 
 *  
 * @note: Note that the max number of entries is given by PTIN_MGMD_MAX_WHITELIST 
 */
RC_t ptinMgmdWhitelistInit(void)
{
  RC_t            res = SUCCESS;
  ptin_mgmd_eb_t *pSnoopEB;

  if ((pSnoopEB= mgmdEBGet())== PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to snoopEBGet()");
    return ERROR;
  }

  pSnoopEB->mgmdPTinWhitelistTreeHeap   = (ptin_mgmd_avlTreeTables_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_WHITELIST*sizeof(ptin_mgmd_avlTreeTables_t));
  pSnoopEB->mgmdPTinWhitelistDataHeap = (mgmdPTinWhitelistData_t *)   ptin_mgmd_malloc(PTIN_MGMD_MAX_WHITELIST*sizeof(mgmdPTinWhitelistData_t));
  if ((pSnoopEB->mgmdPTinWhitelistTreeHeap == PTIN_NULLPTR) || (pSnoopEB->mgmdPTinWhitelistDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for mgmdPTinWhitelistData_t");    
    return FAILURE;
  }

  memset(&pSnoopEB->mgmdPTinWhitelistAvlTree, 0x00, sizeof(ptin_mgmd_avlTree_t));
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->mgmdPTinWhitelistAvlTree), pSnoopEB->mgmdPTinWhitelistTreeHeap, pSnoopEB->mgmdPTinWhitelistDataHeap, 
                             PTIN_MGMD_MAX_WHITELIST, sizeof(mgmdPTinWhitelistData_t), 0x10, sizeof(mgmdPtinWhitelistDataKey_t));

  return res;
}


static RC_t ptinMgmdAddr2Cidr(ptin_mgmd_inet_addr_t *inetAddr, uint8 maskLen,ptin_mgmd_inet_addr_t *cidrAddr, uint32 *maxAddr)
{
  ptin_mgmd_inet_addr_t  maskAddr;
  char                   debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN], debug_buf2[PTIN_MGMD_IPV6_DISP_ADDR_LEN];


  if ( maskLen == 0) 
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid maskLen [%u]",maskLen);
    return FAILURE;
  }

  if(SUCCESS != ptin_mgmd_inetMaskLenToMask(inetAddr->family, maskLen, &maskAddr))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to convert [maskLen=%u] to maskAddr", maskLen);
    return FAILURE;
  }

  if (maskLen>PTIN_MGMD_INET_ADDR_GET_ALL_ONES_MASKLEN(&maskAddr))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "maskLen [%u]  > PTIN_MGMD_INET_ADDR_GET_ALL_ONES_MASKLEN [%u]",maskLen, PTIN_MGMD_INET_ADDR_GET_ALL_ONES_MASKLEN(&maskAddr));
    return FAILURE;
  }

  if(SUCCESS != ptin_mgmd_inetAddressAnd(inetAddr, &maskAddr, cidrAddr))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to convert [inetAddr=%s maskAddr=%s] to groupCIDR", ptin_mgmd_inetAddrPrint(inetAddr, debug_buf), ptin_mgmd_inetAddrPrint(&maskAddr, debug_buf2));
    return FAILURE;
  }

  if (PTIN_MGMD_MAX_WHITELIST < (*maxAddr=PTIN_MGMD_INET_ADDR_GET_MAX(inetAddr,maskLen)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Max Addresses [%u]  > PTIN_MGMD_MAX_WHITELIST [%u]",*maxAddr, PTIN_MGMD_MAX_WHITELIST);
    return FAILURE;
  }  

  return SUCCESS;
}

static RC_t ptinMgmdGetNextInetAddr(ptin_mgmd_inet_addr_t *nextInetAddr)
{  
  if (nextInetAddr->family==PTIN_MGMD_AF_INET)
  {
    if (0x0 == (nextInetAddr->addr.ipv4.s_addr+=1))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "inetAddr equal to 255.255.255.255");
      return FAILURE;      
    }        
    return SUCCESS;  
  }
  else
  {
    if (nextInetAddr->family==PTIN_MGMD_AF_INET6)
    {
      if (0x0 == (nextInetAddr->addr.ipv6.in6.addr32[3]+=1))
      {
        if (0x0 == (nextInetAddr->addr.ipv6.in6.addr32[2]+=1))
        {
          if (0x0 == (nextInetAddr->addr.ipv6.in6.addr32[1]+=1))
          {
            if (0x0 == (nextInetAddr->addr.ipv6.in6.addr32[0]+=1))
            {
              return SUCCESS;  
            }
            else
            {
              PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "inetAddr equal to FFFF:FFFF:FFFF:FFFF");
              return FAILURE;      
            }
          }
        }            
      }
      return SUCCESS;  
    }
    else
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid inet family [%u]",nextInetAddr->family);
      return FAILURE;  
    }
  }
}

/**
 * Add a new channel to the white-list. 
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 * 
 * @return Pointer to inserted item. 
 */
RC_t ptinMgmdWhitelistAdd(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, uint8 groupMaskLen, ptin_mgmd_inet_addr_t *sourceAddr, uint8 sourceMaskLen)
{
  mgmdPTinWhitelistData_t  entry;
  mgmdPTinWhitelistData_t *pData;
  ptin_mgmd_inet_addr_t    groupCIDR, //Group Address Classless Inter Domain Routing                           
                           sourceCIDR; //Source Address Classless Inter Domain Routing  
  uint32                   maxGroupAddresses,
                           maxSourceAddresses;
  ptin_mgmd_eb_t          *pSnoopEB; 
  
  // Argument validation
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (groupAddr == PTIN_NULLPTR) || (groupMaskLen==0) || (sourceMaskLen!=0 && sourceAddr == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [serviceId=%u groupAddr=%p groupMaskLen=%u sourceAddr=%p sourceMaskLen=%u]", serviceId, groupAddr, groupMaskLen, sourceAddr, sourceMaskLen);
    return FAILURE;
  }

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  //Fill the insertion key
  memset(&entry, 0x00, sizeof(entry));
  memcpy(&entry.key.serviceId, &serviceId, sizeof(entry.key.serviceId));

  if (SUCCESS != ptinMgmdAddr2Cidr(groupAddr,groupMaskLen,&groupCIDR,&maxGroupAddresses) ||      
      (sourceMaskLen != 0 && //Source List Not Empty
       SUCCESS != ptinMgmdAddr2Cidr(sourceAddr,sourceMaskLen,&sourceCIDR,&maxSourceAddresses)) )
  {
    //Error already logged
    return FAILURE;
  }

  while (maxGroupAddresses>0)
  {
    ptin_mgmd_inetCopy(&entry.key.groupAddr, &groupCIDR);

    //Empty Source List
    if(sourceMaskLen==0) 
    {
      ptin_mgmd_inetAddressZeroSet(groupCIDR.family,&sourceCIDR);
      ptin_mgmd_inetCopy(&entry.key.sourceAddr, &sourceCIDR);

      //Search
      pData = ptin_mgmd_avlInsertEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, &entry);
      if (pData == PTIN_NULL)  
      {
        //Ensure that the new entry was correcly added
        if ((pData = ptinMgmdWhitelistSearch(serviceId, &groupCIDR, &sourceCIDR, AVL_EXACT)) == PTIN_NULLPTR)
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find inserted entry");
          return FAILURE;
        }    
      }
      if (pData == &entry)
      {
        /*some error in avl tree addition*/
        return FAILURE;
      }
    }    
    else //Source List Given
    {
      while (maxSourceAddresses>0)
      {
        ptin_mgmd_inetCopy(&entry.key.sourceAddr, &sourceCIDR);

        //Search
        pData = ptin_mgmd_avlInsertEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, &entry);
        if (pData == PTIN_NULL)  
        {
          //Ensure that the new entry was correcly added
          if ((pData = ptinMgmdWhitelistSearch(serviceId, &groupCIDR, &sourceCIDR, AVL_EXACT)) == PTIN_NULLPTR)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find inserted entry");
            return FAILURE;
          }    
        }
        if (pData == &entry)
        {
          /*  some error in avl tree addition*/
          return FAILURE;
        }

        if ( SUCCESS != ptinMgmdGetNextInetAddr(&sourceCIDR))
          break;
        --maxSourceAddresses;
      }
    }

    if ( SUCCESS != ptinMgmdGetNextInetAddr(&groupCIDR))
      break;
    --maxGroupAddresses;
  }

  return SUCCESS;
}


/**
 * Remove an existing channel from the white-list.
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 *  
 * @return RC_t [NOT_EXIST if not found]
 */
RC_t ptinMgmdWhitelistRemove(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, uint8 groupMaskLen, ptin_mgmd_inet_addr_t *sourceAddr, uint8 sourceMaskLen)
{ 
  mgmdPTinWhitelistData_t *pData;
  ptin_mgmd_inet_addr_t    groupCIDR, //Group Address Classless Inter Domain Routing                           
                           sourceCIDR; //Source Address Classless Inter Domain Routing  
  uint32                   maxGroupAddresses,
                           maxSourceAddresses;
  ptin_mgmd_eb_t          *pSnoopEB; 
  
  // Argument validation
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (groupAddr == PTIN_NULLPTR) || (groupMaskLen==0) || (sourceMaskLen!=0 && sourceAddr == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [serviceId=%u groupAddr=%p groupMaskLen=%u sourceAddr=%p sourceMaskLen=%u]", serviceId, groupAddr, groupMaskLen, sourceAddr, sourceMaskLen);
    return ERROR;
  }

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  if (SUCCESS != ptinMgmdAddr2Cidr(groupAddr,groupMaskLen,&groupCIDR,&maxGroupAddresses) ||      
      (sourceMaskLen != 0 && //Source List Not Empty
       SUCCESS != ptinMgmdAddr2Cidr(sourceAddr,sourceMaskLen,&sourceCIDR,&maxSourceAddresses)) )
  {
    //Error previouly logged
    return FAILURE;
  }

  while (maxGroupAddresses>0)
  {    
    if(sourceMaskLen==0) //Empty Source List
    {
      ptin_mgmd_inetAddressZeroSet(groupCIDR.family,&sourceCIDR);
      //Search
      pData = ptinMgmdWhitelistSearch(serviceId, &groupCIDR, &sourceCIDR, AVL_EXACT);
      if (pData != PTIN_NULLPTR)
      { 
        //Delete   
        pData = ptin_mgmd_avlDeleteEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, pData);    
      }
    }
    else //Source List Given
    {
      while (maxSourceAddresses>0)
      {
        pData = ptinMgmdWhitelistSearch(serviceId, &groupCIDR, &sourceCIDR, AVL_EXACT);
        if (pData != PTIN_NULLPTR)
        { 
          //Delete   
          pData = ptin_mgmd_avlDeleteEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, pData);    
        }

        if ( SUCCESS != ptinMgmdGetNextInetAddr(&sourceCIDR))
          break;
        --maxSourceAddresses;
      }
    }

    if ( SUCCESS != ptinMgmdGetNextInetAddr(&groupCIDR))
      break;
    --maxGroupAddresses;
  }
  
  return SUCCESS;
}


/**
 * Search for the given channel in the white-list.
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 * @param flag       : Search flag [AVL_NEXT or AVL_EXACT]
 *  
 * @return Pointer to searched item or PTIN_NULLPTR if not found.
 */
mgmdPTinWhitelistData_t* ptinMgmdWhitelistSearch(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr, uint32 flag)
{
  mgmdPTinWhitelistData_t     *entry;
  mgmdPtinWhitelistDataKey_t  key;
  ptin_mgmd_eb_t             *pSnoopEB; 

  // Argument validation
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (groupAddr == PTIN_NULLPTR) || (sourceAddr == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [serviceId=%u groupAddr=%p sourceAddr=%p]", serviceId, groupAddr, sourceAddr);
    return PTIN_NULLPTR;
  }

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  //Fill the search key
  memset((void*)&key, 0x00, sizeof(key));
  memcpy(&key.serviceId, &serviceId, sizeof(key.serviceId));
  ptin_mgmd_inetCopy(&key.groupAddr, groupAddr);
  ptin_mgmd_inetCopy(&key.sourceAddr, sourceAddr);

  //Search
  entry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->mgmdPTinWhitelistAvlTree, &key, flag);
  if (flag == AVL_NEXT)
  {
    while (entry)
    {
      memcpy(&key, &entry->key, sizeof(key));
      entry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->mgmdPTinWhitelistAvlTree, &key, flag);
    }
  }
  if (entry == PTIN_NULL)
  {
    return PTIN_NULLPTR;
  }
  else
  {
    return entry;
  }
}


/**
 * Dump the current white-list.
 *  
 * @return RC_t
 */
void ptinMgmdWhitelistDump(void)
{
  mgmdPTinWhitelistData_t     *entry;  
  mgmdPtinWhitelistDataKey_t  key;
  ptin_mgmd_eb_t             *pSnoopEB;
  char                        debug_buf1[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {0};
  char                        debug_buf2[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {0};

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  //Dump the entire AVL tree
  printf("\n");
  printf("+------------+------------------+------------------+\n");
  printf("| Service ID | Group Address    | Source Address   |\n");
  printf("+------------+------------------+------------------+\n");
  memset(&key, 0x00, sizeof(mgmdPtinWhitelistDataKey_t));
  while ((entry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->mgmdPTinWhitelistAvlTree, &key, AVL_NEXT)) != PTIN_NULLPTR)
  {
    //Prepare next key
    memcpy(&key, &entry->key, sizeof(mgmdPtinWhitelistDataKey_t));

    printf("| %10u | %16s | %16s |\n", entry->key.serviceId, ptin_mgmd_inetAddrPrint(&(entry->key.groupAddr), debug_buf1), ptin_mgmd_inetAddrPrint(&(entry->key.sourceAddr), debug_buf2));
  }
  printf("+------------+------------------+------------------+\n");

}


/*************************************************************************
 * @purpose Clean Whitelist AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdWhitelistClean(void)
{
  ptin_mgmd_eb_t                *pSnoopEB;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }
  ptin_mgmd_avlPurgeAvlTree(&pSnoopEB->mgmdPTinWhitelistAvlTree,PTIN_MGMD_MAX_WHITELIST);
}
