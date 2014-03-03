/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2013-2013
*
**********************************************************************
*
* @create    20/11/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/

#include "ptin_mgmd_specificquery.h"
#include "ptin_mgmd_cnfgr.h"
#include "ptin_mgmd_osapi.h"
#include "ptin_utils_inet_addr_api.h"
#include "logger.h"


/**
 * AVLTree initialization
 */
RC_t ptinMgmdSpecificQueryAVLTreeInit(void)
{
  mgmd_eb_t *pSnoopEB;
  uint32    i;

  if ((pSnoopEB= mgmdEBGet())== PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to snoopEBGet()");
    return ERROR;
  }

  //Group-Source Specific Query initializations
  pSnoopEB->groupSourceSpecificQueryTreeHeap = (ptin_mgmd_avlTreeTables_t *)                 ptin_mgmd_malloc(PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_CHANNELS*sizeof(ptin_mgmd_avlTreeTables_t));
  pSnoopEB->groupSourceSpecificQueryDataHeap = (groupSourceSpecificQueriesAvl_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_CHANNELS*sizeof(groupSourceSpecificQueriesAvl_t));
  if ((pSnoopEB->groupSourceSpecificQueryTreeHeap == PTIN_NULLPTR) || (pSnoopEB->groupSourceSpecificQueryDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for groupSourceSpecificQueriesAvl_t");    
    return FAILURE;
  }
  memset(&pSnoopEB->groupSourceSpecificQueryAvlTree, 0x00, sizeof(ptin_mgmd_avlTree_t));
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->groupSourceSpecificQueryAvlTree), pSnoopEB->groupSourceSpecificQueryTreeHeap, pSnoopEB->groupSourceSpecificQueryDataHeap, 
                   PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_CHANNELS, sizeof(groupSourceSpecificQueriesAvl_t), 0x10, sizeof(groupSourceSpecificQueriesAvlKey_t));

  /* Create the FIFO queue for the sources */
  ptin_fifo_create(&pSnoopEB->specificQuerySourcesQueue, PTIN_MGMD_MAX_SOURCES);
  for(i=0; i<PTIN_MGMD_MAX_SOURCES; ++i)
  {
    groupSourceSpecificQueriesSource_t *new_source = (groupSourceSpecificQueriesSource_t*) ptin_mgmd_malloc(sizeof(groupSourceSpecificQueriesSource_t));

    ptin_fifo_push(pSnoopEB->specificQuerySourcesQueue, (PTIN_FIFO_ELEMENT_t)new_source);
  }

  return SUCCESS;
}


/**
 * Group-Source Specific Query AVLTree manipulation
 */
groupSourceSpecificQueriesAvl_t* ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId, uint32 flag)
{
  groupSourceSpecificQueriesAvl_t    *entry;
  groupSourceSpecificQueriesAvlKey_t key;
  mgmd_eb_t                         *pSnoopEB; 

  /* Argument validation */
  if (groupAddr == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [groupAddr=%p]", groupAddr);
    return PTIN_NULLPTR;
  }

  pSnoopEB = mgmdEBGet();

  memset((void*)&key, 0x00, sizeof(key));
  ptin_mgmd_inetCopy(&key.groupAddr, groupAddr);
  memcpy(&key.serviceId, &serviceId, sizeof(key.serviceId));
  memcpy(&key.portId, &portId, sizeof(key.portId));

  entry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->groupSourceSpecificQueryAvlTree, &key, flag);
  if (flag == AVL_NEXT)
  {
    while (entry)
    {
      memcpy(&key, &entry->key, sizeof(key));
      entry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->groupSourceSpecificQueryAvlTree, &key, flag);
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


groupSourceSpecificQueriesAvl_t* ptinMgmdGroupSourceSpecificQueryAVLTreeEntryAdd(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId)
{
  groupSourceSpecificQueriesAvl_t entry;
  groupSourceSpecificQueriesAvl_t *pData;
  mgmd_eb_t                      *pSnoopEB; 

  /* Argument validation */
  if (groupAddr == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [groupAddr=%p]", groupAddr);
    return PTIN_NULLPTR;
  }

  pSnoopEB = mgmdEBGet();

  memset(&entry, 0x00, sizeof(entry));
  ptin_mgmd_inetCopy(&entry.key.groupAddr, groupAddr);
  memcpy(&entry.key.serviceId, &serviceId, sizeof(entry.key.serviceId));
  memcpy(&entry.key.portId, &portId, sizeof(entry.key.portId));

  pData = ptin_mgmd_avlInsertEntry(&pSnoopEB->groupSourceSpecificQueryAvlTree, &entry);
  if (pData == PTIN_NULL)
  {
    /*entry was added into the avl tree*/
    if ((pData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT)) == PTIN_NULLPTR)
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


RC_t ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId)
{
  groupSourceSpecificQueriesAvl_t *pData;
  mgmd_eb_t                      *pSnoopEB; 

  /* Argument validation */
  if (groupAddr == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [groupAddr=%p]", groupAddr);
    return FAILURE;
  }

  pSnoopEB = mgmdEBGet();

  pData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT);
  if (pData == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested entry");
    return FAILURE;
  }

  pData = ptin_mgmd_avlDeleteEntry(&pSnoopEB->groupSourceSpecificQueryAvlTree, pData);
  if (pData == PTIN_NULL)
  {
    /* Entry does not exist */
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Entry does not exist");
    return FAILURE;
  }

  return SUCCESS;
}

