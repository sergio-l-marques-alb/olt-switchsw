/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_db.c
*
* @purpose   SFlow instance datastore access
*
* @component sflow
*
* @comments  Provides APIs to access the sampler, poller instances
*
* @create 23-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#include "sflow_db.h"
extern SFLOW_agent_t agent;
/*********************************************************************
* @purpose  Adds a sampler instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    SFLOW_sampler_mib_t  @b{(input)} reference to this sampler 
*                                            configuration
*
* @returns  L7_FAILURE on error while inserting
* @returns  L7_ALREADY_CONFIGURED entry is already present
* @returns  L7_SUCCESS up on successful addition
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowSamplerInstanceAdd(L7_uint32 dsIndex, L7_uint32 instance,
                                SFLOW_sampler_mib_t *pSamplerCfg)
{
  SFLOW_sampler_t      samplerEntry;
  SFLOW_sampler_t     *pData;

  memset(&samplerEntry, 0x00, sizeof(SFLOW_sampler_t));
  samplerEntry.key.dsIndex    = dsIndex;
  samplerEntry.key.sFlowInstance   = instance;
  samplerEntry.samplerCfg = pSamplerCfg;

  osapiSemaTake(agent.samplerAvlTree.semId, L7_WAIT_FOREVER);
  pData = avlInsertEntry(&agent.samplerAvlTree, &samplerEntry);
  osapiSemaGive(agent.samplerAvlTree.semId);

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/
    return L7_SUCCESS;
  }

  if (pData == &samplerEntry)
  {
    /*some error in avl tree addition*/
    return L7_FAILURE;
  }

   /*entry already exists*/
   return L7_ALREADY_CONFIGURED;
}
/*********************************************************************
* @purpose  Deletes a sampler instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
*
* @returns  L7_FAILURE on error while deleting
* @returns  L7_SUCCESS up on successful deletion
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowSamplerInstanceDelete(L7_uint32 dsIndex, L7_uint32 instance)
{
  SFLOW_sampler_t  *pData;
  SFLOW_sampler_t  *samplerEntry;

  pData = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  samplerEntry = pData;

  osapiSemaTake(agent.samplerAvlTree.semId, L7_WAIT_FOREVER);
  pData = avlDeleteEntry(&agent.samplerAvlTree, pData);
  osapiSemaGive(agent.samplerAvlTree.semId);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  if (pData == samplerEntry)
  {
    /* Entry deleted */
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Finds a sampler instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    flag     @b{(input)} Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
* @returns  L7_NULLPTR if no such instance exists
* @returns  reference to the sampler instance  up on success
*
* @notes    none
*
* @end
*********************************************************************/
SFLOW_sampler_t *sFlowSamplerInstanceGet(L7_uint32 dsIndex, L7_uint32 instance,
                                         L7_uint32 flag)
{
  SFLOW_sampler_t         *samplerEntry;
  SFLOW_sampler_DataKey_t  key;

  memset((void *)&key, 0x00, sizeof(SFLOW_sampler_DataKey_t));
  key.dsIndex  = dsIndex;
  key.sFlowInstance = instance;

  samplerEntry = avlSearchLVL7(&agent.samplerAvlTree, &key, flag);
  if (samplerEntry == L7_NULL)
  {
    return L7_NULLPTR;
  }
  else
  {
    return samplerEntry;
  }
}
/******************************************************************************
  sFlow Poller instances Processing Routines
*******************************************************************************/
/*********************************************************************
* @purpose  Adds a poller instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    SFLOW_poller_mib_t  @b{(input)} reference to this poller
*                                            configuration
*
* @returns  L7_FAILURE on error while inserting
* @returns  L7_ALREADY_CONFIGURED entry is already present
* @returns  L7_SUCCESS up on successful addition
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowPollerInstanceAdd(L7_uint32 dsIndex, L7_uint32 instance,
                               SFLOW_poller_mib_t *pPollerCfg)
{
  SFLOW_poller_t      pollerEntry;
  SFLOW_poller_t     *pData;

  memset(&pollerEntry, 0x00, sizeof(SFLOW_poller_t));
  pollerEntry.key.dsIndex    = dsIndex;
  pollerEntry.key.sFlowInstance   = instance;
  pollerEntry.pollerCfg      = pPollerCfg;

  osapiSemaTake(agent.pollerAvlTree.semId, L7_WAIT_FOREVER);
  pData = avlInsertEntry(&agent.pollerAvlTree, &pollerEntry);
  osapiSemaGive(agent.pollerAvlTree.semId);

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/
    return L7_SUCCESS;
  }

  if (pData == &pollerEntry)
  {
    /*some error in avl tree addition*/
    return L7_FAILURE;
  }

   /*entry already exists*/
   return L7_ALREADY_CONFIGURED;
}
/*********************************************************************
* @purpose  Deletes a poller instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
*
* @returns  L7_FAILURE on error while deleting
* @returns  L7_SUCCESS up on successful deletion
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowPollerInstanceDelete(L7_uint32 dsIndex, L7_uint32 instance)
{
  SFLOW_poller_t  *pData;
  SFLOW_poller_t  *pollerEntry;

  pData = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  pollerEntry = pData;

  osapiSemaTake(agent.pollerAvlTree.semId, L7_WAIT_FOREVER);
  pData = avlDeleteEntry(&agent.pollerAvlTree, pData);
  osapiSemaGive(agent.pollerAvlTree.semId);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  if (pData == pollerEntry)
  {
    /* Entry deleted */
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Finds a poller instance
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    flag     @b{(input)} Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
* @returns  L7_NULLPTR if no such instance exists
* @returns  reference to the poller instance  up on success
*
* @notes    none
*
* @end
*********************************************************************/
SFLOW_poller_t *sFlowPollerInstanceGet(L7_uint32 dsIndex, L7_uint32 instance,
                                       L7_uint32 flag)
{
  SFLOW_poller_t         *pollerEntry;
  SFLOW_poller_DataKey_t  key;

  memset((void *)&key, 0x00, sizeof(SFLOW_poller_DataKey_t));
  key.dsIndex  = dsIndex;
  key.sFlowInstance = instance;

  pollerEntry = avlSearchLVL7(&agent.pollerAvlTree, &key, flag);
  if (pollerEntry == L7_NULL)
  {
    return L7_NULLPTR;
  }
  else
  {
    return pollerEntry;
  }
}

