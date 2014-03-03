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
  RC_t      res = SUCCESS;
  mgmd_eb_t *pSnoopEB;

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


/**
 * Add a new channel to the white-list. 
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 * 
 * @return Pointer to inserted item. 
 */
mgmdPTinWhitelistData_t* ptinMgmdWhitelistAdd(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr)
{
  mgmdPTinWhitelistData_t entry;
  mgmdPTinWhitelistData_t *pData;
  mgmd_eb_t               *pSnoopEB; 
  
  // Argument validation
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (groupAddr == PTIN_NULLPTR) || (sourceAddr == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [serviceId=%u groupAddr=%p sourceAddr=%p]", serviceId, groupAddr, sourceAddr);
    return PTIN_NULLPTR;
  }

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  //Fill the insertion key
  memset(&entry, 0x00, sizeof(entry));
  memcpy(&entry.key.serviceId, &serviceId, sizeof(entry.key.serviceId));
  ptin_mgmd_inetCopy(&entry.key.groupAddr, groupAddr);
  ptin_mgmd_inetCopy(&entry.key.sourceAddr, sourceAddr);

  //Search
  pData = ptin_mgmd_avlInsertEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, &entry);
  if (pData == PTIN_NULL)
  {
    //Ensure that the new entry was correcly added
    if ((pData = ptinMgmdWhitelistSearch(serviceId, groupAddr, sourceAddr, AVL_EXACT)) == PTIN_NULLPTR)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find inserted entry");
      return PTIN_NULLPTR;
    }
    return pData;
  }

  if (pData == &entry)
  {
    /*some error in avl tree addition*/
    return PTIN_NULLPTR;
  }

  /*entry already exists*/
  return pData;
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
RC_t ptinMgmdWhitelistRemove(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr)
{
  RC_t                    res = SUCCESS;
  mgmdPTinWhitelistData_t *pData;
  mgmd_eb_t               *pSnoopEB; 
  
  // Argument validation
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (groupAddr == PTIN_NULLPTR) || (sourceAddr == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [serviceId=%u groupAddr=%p sourceAddr=%p]", serviceId, groupAddr, sourceAddr);
    return ERROR;
  }

  //Get MGMD execution block
  pSnoopEB = mgmdEBGet();

  //Search
  pData = ptinMgmdWhitelistSearch(serviceId, groupAddr, sourceAddr, AVL_EXACT);
  if (pData == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested entry");
    return FAILURE;
  }

  //Delete
  pData = ptin_mgmd_avlDeleteEntry(&pSnoopEB->mgmdPTinWhitelistAvlTree, pData);
  if (pData == PTIN_NULL)
  {
    /* Entry does not exist */
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Entry does not exist");
    return FAILURE;
  }

  return res;
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
  mgmd_eb_t                   *pSnoopEB; 

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
RC_t ptinMgmdWhitelistDump(void)
{
  RC_t res = SUCCESS;
  mgmdPTinWhitelistData_t     *entry;  
  mgmdPtinWhitelistDataKey_t  key;
  mgmd_eb_t                   *pSnoopEB;
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

  return res;
}

