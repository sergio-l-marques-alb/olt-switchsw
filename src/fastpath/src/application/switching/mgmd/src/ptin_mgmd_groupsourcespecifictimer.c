/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/

#include "ptin_mgmd_groupsourcespecifictimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "logger.h"
#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"
#include "snooping.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_util.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_cnfgr.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_groupsourcespecifictimer_callback(void *param);
static RC_t  ptin_mgmd_groupsourcespecifictimer_init(PTIN_MGMD_TIMER_t *timerPtr);
static RC_t  ptin_mgmd_groupsourcespecifictimer_restart(groupSourceSpecificQueriesAvlKey_t* timerKey);
static RC_t  ptin_mgmd_groupsourcespecifictimer_stop(PTIN_MGMD_TIMER_t timer);

static RC_t  __groupsourcespecifictimer_addsource(groupSourceSpecificQueriesAvl_t *avlTree, ptin_mgmd_inet_addr_t* sourceAddr, uint8 retransmissions);
static RC_t  __groupsourcespecifictimer_delsource(groupSourceSpecificQueriesAvl_t *avlTree, groupSourceSpecificQueriesSource_t *source);


RC_t __groupsourcespecifictimer_addsource(groupSourceSpecificQueriesAvl_t *avlTree, ptin_mgmd_inet_addr_t* sourceAddr, uint8 retransmissions)
{
  mgmd_eb_t *pMgmdEB;  
  RC_t      res = SUCCESS;
  groupSourceSpecificQueriesSource_t *new_source;

  //Validations
  if( (PTIN_NULLPTR == avlTree) || (PTIN_NULLPTR == sourceAddr) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [avlTree=%p sourceAddr=%p]", avlTree, sourceAddr);
    return (res = FAILURE);
  }

  if ((pMgmdEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdEBGet()");
    return FAILURE;
  }

  //Get a new source from the source queue
  if(SUCCESS != (res = ptin_fifo_pop(pMgmdEB->specificQuerySourcesQueue, (PTIN_FIFO_ELEMENT_t*) &new_source)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Source buffer is full");
    return res;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "I was given the source %p", new_source);

  //Configure the new source
  memset(new_source, 0x00, sizeof(groupSourceSpecificQueriesSource_t));
  ptin_mgmd_inetCopy(&new_source->sourceAddr, sourceAddr);
  new_source->retransmissions = retransmissions;
  new_source->prev            = PTIN_NULLPTR;
  new_source->next            = avlTree->firstSource;

  //Add it to the existing source list
  if(avlTree->firstSource == PTIN_NULLPTR)
  { 
    avlTree->lastSource = new_source;
  }
  else
  {
    avlTree->firstSource->prev = new_source;
  }
  avlTree->firstSource = new_source;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "First[%p] Last[%p] Next[%p] Prev[%p]", avlTree->firstSource, avlTree->lastSource, new_source->next, new_source->prev);

  ++avlTree->numberOfSources;

  return res;
}

RC_t __groupsourcespecifictimer_delsource(groupSourceSpecificQueriesAvl_t *avlTree, groupSourceSpecificQueriesSource_t *source)
{
  mgmd_eb_t *pMgmdEB;  
  RC_t res = SUCCESS;

  //Validations
  if( (PTIN_NULLPTR == avlTree) || (PTIN_NULLPTR == source) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [avlTree=%p source=%p]", avlTree, source);
    return (res = FAILURE);
  }
  if(avlTree->numberOfSources == 0)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trying to delete a source[%08X] even though there are no sources in the source list", source->sourceAddr.addr.ipv4.s_addr);
    return (res = FAILURE);
  }

  if ((pMgmdEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdEBGet()");
    return FAILURE;
  }

  //Remove the source from the list
  if(source->prev != PTIN_NULLPTR)
  {
    source->prev->next = source->next;
  }
  if(source->next != PTIN_NULLPTR)
  {
    source->next->prev = source->prev;
  }
  if(source == avlTree->firstSource)
  {
    avlTree->firstSource = source->next;
  }
  if(source == avlTree->lastSource)
  {
    avlTree->lastSource = source->prev;
  }

  //Decrement the number of sources in the list
  --avlTree->numberOfSources;

  //Release the source
  if(SUCCESS != (res = ptin_fifo_push(pMgmdEB->specificQuerySourcesQueue, (PTIN_FIFO_ELEMENT_t) source)))
  {
    return res;
  }

  return res;
}


void* ptin_mgmd_groupsourcespecifictimer_callback(void *param)
{
  PTIN_MGMD_EVENT_t eventMsg = {0}; 

  if(PTIN_NULLPTR == param)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [param=%p]", param);
    return PTIN_NULLPTR;
  }

  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_GROUPSOURCEQUERY, (void*) param, sizeof(groupSourceSpecificQueriesAvlKey_t));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }
  
  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_groupsourcespecifictimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_init(PTIN_MGMD_TIMER_t *timerPtr)
{
  RC_t ret = SUCCESS;

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (PTIN_NULLPTR == timerPtr)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trying to initialize a NULL timer!");
    return FAILURE;
  }

  if(FALSE == ptin_mgmd_timer_exist(*timerPtr))
  {
    ret = ptin_mgmd_timer_init(__controlBlock, timerPtr, ptin_mgmd_groupsourcespecifictimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_groupspecifictimer_start(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId)
{
  RC_t                            ret           = SUCCESS;
  ptin_IgmpProxyCfg_t             igmpGlobalCfg;
  groupSourceSpecificQueriesAvl_t *timerData; 
  snoopPTinL3InfoData_t           *groupEntry;

  if(PTIN_NULLPTR == groupAddr)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [groupAddr=%p]", groupAddr);
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting group specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT)))
  {
    uint32 gtTimeLeft, lmqt;
    int32  rootPortBitmap;

    //Add new entry to the AVL tree with the parameters in groupData
    if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryAdd(groupAddr, serviceId, portId)))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to add a new groupspecific timer entry [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return TABLE_IS_FULL;
    }
    timerData->retransmissions = igmpGlobalCfg.querier.last_member_query_count;

    //Set supress router-side processing flag if current grouptimer is higher than LMQT
    if (PTIN_NULLPTR == (groupEntry = ptinMgmdL3EntryFind(serviceId, groupAddr, AVL_EXACT)))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to find group entry [groupAddr=0x%08X serviceId=%u]", groupAddr->addr.ipv4.s_addr, serviceId);
      return FAILURE;
    }
    lmqt       = igmpGlobalCfg.querier.last_member_query_interval * igmpGlobalCfg.querier.last_member_query_count;
    gtTimeLeft = ptin_mgmd_grouptimer_timeleft(&groupEntry->interfaces[portId].groupTimer);
    if(gtTimeLeft > lmqt)
    {
      timerData->supressRouterSideProcessing = TRUE;
    }
    else
    {
      timerData->supressRouterSideProcessing = FALSE;
    }

    //Set group-timer to LMQT. If this is the only interface in the root port, set the root port group-timer to LMQT as well
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting interface timer to LMQT");
    if(SUCCESS != ptin_mgmd_grouptimer_start(&groupEntry->interfaces[portId].groupTimer, lmqt, groupEntry->snoopPTinL3InfoDataKey, portId))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return FAILURE;
    }
    PTIN_MGMD_UNSET_MASKBIT(groupEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID].clients, portId);
    PTIN_MGMD_CLIENT_NONZEROMASK(groupEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID].clients, rootPortBitmap);
    if(-1 == rootPortBitmap)
    {
      //Set group-timer to LMQT. If this is the only interface in the root port, set the root port group-timer to LMQT as well
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting root interface timer to LMQT");
      if(SUCCESS != ptin_mgmd_grouptimer_start(&groupEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID].groupTimer, lmqt+1, groupEntry->snoopPTinL3InfoDataKey, SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID))
      {
        PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]",
                     groupAddr->addr.ipv4.s_addr, serviceId, SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID);
        return FAILURE;
      }
    }
    PTIN_MGMD_SET_MASKBIT(groupEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID].clients, portId); //Restore root port client bitmap

    if(SUCCESS != ptin_mgmd_groupsourcespecifictimer_init(&timerData->timerHandle))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new groupspecific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return FAILURE;
    }
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "New group specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);
  }
  else
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Restarting groupspecific [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);
    ptin_mgmd_timer_stop(timerData->timerHandle);
    timerData->retransmissions = igmpGlobalCfg.querier.last_member_query_count;
  }

  ret = ptin_mgmd_timer_start(timerData->timerHandle, 0, &timerData->key); //Send a Q(G) immediatly
  return ret;
}


RC_t ptin_mgmd_groupsourcespecifictimer_start(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId)
{
  RC_t                            ret           = SUCCESS;
  ptin_IgmpProxyCfg_t             igmpGlobalCfg;
  groupSourceSpecificQueriesAvl_t *timerData; 

  if(PTIN_NULLPTR == groupAddr)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [groupAddr=%p]", groupAddr);
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting sourcespecific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Nothing to be done for this Q(G,S) [groupAddr=0x%08X serviceId=%u portId=%u]", 
            groupAddr->addr.ipv4.s_addr, serviceId, portId);
    return SUCCESS;
  }
  else
  {
    ptin_mgmd_timer_stop(timerData->timerHandle);
  }

  if(timerData->numberOfSources != 0)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Scheduling group-source specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);
    ret = ptin_mgmd_timer_start(timerData->timerHandle, 0, &timerData->key); //Send a Q(G,S) immediatly
  }
  
  //If this Q(G,S) does not have any more sources or group retransmissions, remote it
  if( (timerData->numberOfSources == 0) && (timerData->retransmissions == 0) )
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "No retransmissions left for this Group Specific Timer");
    if(SUCCESS != ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to delete Group Specific Timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);
      return FAILURE;
    }
    ptin_mgmd_timer_deinit(timerData->timerHandle);
  }
  return ret;
}


RC_t  ptin_mgmd_groupsourcespecifictimer_restart(groupSourceSpecificQueriesAvlKey_t* timerKey)
{
  RC_t                            ret           = SUCCESS;
  ptin_IgmpProxyCfg_t             igmpGlobalCfg;
  groupSourceSpecificQueriesAvl_t *timerData    = PTIN_NULLPTR; 

  if(PTIN_NULLPTR == timerKey)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [timerKey=%p]", timerKey);
    return FAILURE;
  }

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&timerKey->groupAddr, timerKey->serviceId, timerKey->portId, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested AVL entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
            timerKey->groupAddr.addr.ipv4.s_addr, timerKey->serviceId, timerKey->portId);
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Restarting group specific timer [groupAddr=0x%08X serviceId=%u portId=%u retransmissions=%u]", 
            timerKey->groupAddr.addr.ipv4.s_addr, timerKey->serviceId, timerKey->portId, timerData->retransmissions);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  ret = ptin_mgmd_timer_start(timerData->timerHandle, igmpGlobalCfg.querier.last_member_query_interval*1000, &timerData->key);
  return ret;
}


RC_t ptin_mgmd_groupsourcespecifictimer_addsource(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId, ptin_mgmd_inet_addr_t* sourceAddr)
{
  groupSourceSpecificQueriesAvl_t  *timerData;
  ptin_IgmpProxyCfg_t               igmpGlobalCfg;
  uint32                            i;   
  snoopPTinL3Source_t              *sourcePtr;
  snoopPTinL3InfoData_t            *groupEntry;
  uint32                            lmqt;
    
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Adding new source to source specific timer [groupAddr=0x%08X serviceId=%u portId=%u sourceAddr=0x%08X]",
            groupAddr->addr.ipv4.s_addr, serviceId, portId, sourceAddr->addr.ipv4.s_addr);

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  lmqt = igmpGlobalCfg.querier.last_member_query_interval * igmpGlobalCfg.querier.last_member_query_count;

  //Find entry in the AVL
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT)))
  {
    //Add new entry to the AVL tree with the parameters in groupData
    if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryAdd(groupAddr, serviceId, portId)))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to add a new sourcespecific timer entry [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return TABLE_IS_FULL;
    }

    if(SUCCESS != ptin_mgmd_groupsourcespecifictimer_init(&timerData->timerHandle))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new groupspecific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return FAILURE;
    }
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "New group-source specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);

    //Add source to the list
    timerData->numberOfSources = 0;
    timerData->firstSource     = PTIN_NULLPTR;
    timerData->lastSource      = PTIN_NULLPTR;
    if(SUCCESS != __groupsourcespecifictimer_addsource(timerData, sourceAddr, igmpGlobalCfg.querier.last_member_query_count))
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Q(G,S) source buffer is full! [groupAddr=0x%08X sourceAddr=0x%08X serviceId=%u portId=%u]", 
            groupAddr->addr.ipv4.s_addr, sourceAddr->addr.ipv4.s_addr, serviceId, portId);
      return TABLE_IS_FULL;
    }
  }
  else
  {
    BOOL  source_found = FALSE;
    groupSourceSpecificQueriesSource_t *iterator;

    //Ensure that the timer is not running
    ptin_mgmd_groupsourcespecifictimer_stop(timerData->timerHandle);

    //First search for this source. If not found, add a new source
    for(iterator=timerData->firstSource, i=0; iterator!=PTIN_NULLPTR && i<timerData->numberOfSources; iterator=iterator->next, ++i)
    {
      if(TRUE == PTIN_MGMD_INET_IS_ADDR_EQUAL(&iterator->sourceAddr, sourceAddr))
      {
        iterator->retransmissions = igmpGlobalCfg.querier.last_member_query_count; //Reset retransmissions counter
        source_found = TRUE;
        break;
      }
    }
    if(source_found == FALSE)
    {
      if(SUCCESS != __groupsourcespecifictimer_addsource(timerData, sourceAddr, igmpGlobalCfg.querier.last_member_query_count))
      {
        PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Q(G,S) source buffer is full! [groupAddr=0x%08X sourceAddr=0x%08X serviceId=%u portId=%u]", 
              groupAddr->addr.ipv4.s_addr, sourceAddr->addr.ipv4.s_addr, serviceId, portId);
        return TABLE_IS_FULL;
      }
    }
  }

  //Set source-timer to LMQT. If this is the only interface in the root port, set the root port source-timer to LMQT as well
  if (PTIN_NULLPTR == (groupEntry = ptinMgmdL3EntryFind(serviceId, groupAddr, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to find group entry [groupAddr=0x%08X serviceId=%u]", groupAddr->addr.ipv4.s_addr, serviceId);
    return FAILURE;
  }
  if ( (sourcePtr=ptinMgmdSourceFind(groupEntry,portId, sourceAddr))!=PTIN_NULLPTR)  
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting source[%08X] timer to LMQT[%u]", sourceAddr->addr.ipv4.s_addr, lmqt);
    if(SUCCESS != ptin_mgmd_sourcetimer_start(&sourcePtr->sourceTimer, lmqt, groupEntry->snoopPTinL3InfoDataKey, portId, sourcePtr))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return FAILURE;
    }
  }  
  if ( (sourcePtr=ptinMgmdSourceFind(groupEntry,SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID,sourceAddr)) !=PTIN_NULLPTR)  
  {   
    int32  rootPortBitmap;

    PTIN_MGMD_UNSET_MASKBIT(sourcePtr->clients, portId);
    PTIN_MGMD_CLIENT_NONZEROMASK(sourcePtr->clients, rootPortBitmap);
    if(-1 == rootPortBitmap)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting source[%08X] timer to LMQT[%u]", sourceAddr->addr.ipv4.s_addr, lmqt);
      if(SUCCESS != ptin_mgmd_sourcetimer_start(&sourcePtr->sourceTimer, lmqt+1, groupEntry->snoopPTinL3InfoDataKey, SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID, sourcePtr))
      {
        PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]",
                     groupAddr->addr.ipv4.s_addr, serviceId, SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID);
        return FAILURE;
      }
    }
    PTIN_MGMD_SET_MASKBIT(sourcePtr->clients, portId); //Restore root port client bitmap
  }
  
  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_removesource(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId, ptin_mgmd_inet_addr_t* sourceAddr)
{
  groupSourceSpecificQueriesAvl_t *timerData;
    
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Removing source from Q(G,S) timer [groupAddr=0x%08X serviceId=%u portId=%u sourceAddr=0x%08X]",
            groupAddr->addr.ipv4.s_addr, serviceId, portId, sourceAddr->addr.ipv4.s_addr);

  //Find entry in the AVL
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
              groupAddr->addr.ipv4.s_addr, serviceId, portId);
    return FAILURE;
  }
  else
  {
    groupSourceSpecificQueriesSource_t *iterator;
    uint32                             i; 

    //Ensure that the timer is not running
    ptin_mgmd_groupsourcespecifictimer_stop(timerData->timerHandle);

    //Search for the requested source
    for(iterator=timerData->firstSource, i=0; iterator!=PTIN_NULLPTR && i<timerData->numberOfSources; iterator=iterator->next, ++i)
    {
      if(TRUE == PTIN_MGMD_INET_IS_ADDR_EQUAL(&iterator->sourceAddr, sourceAddr))
      {
        ptin_mgmd_inetAddressZeroSet(iterator->sourceAddr.family, &iterator->sourceAddr);
        iterator->retransmissions = 0;
        --timerData->numberOfSources;

        __groupsourcespecifictimer_delsource(timerData, iterator);

        break;
      }
    }
  }

  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_stop(PTIN_MGMD_TIMER_t timer)
{
  if (TRUE == ptin_mgmd_timer_isRunning(timer))
  {
    ptin_mgmd_timer_stop(timer);
  }
  
  return SUCCESS;
}


RC_t ptin_mgmd_event_groupsourcespecifictimer(groupSourceSpecificQueriesAvlKey_t* eventData)
{
  uchar8                             queryHeader[PTIN_MGMD_MAX_FRAME_SIZE] = {0};
  uint32                             queryHeaderLength = 0;
  ptin_mgmd_externalapi_t            externalApi;
  ptinMgmdControlPkt_t              queryPckt         = {0};
  groupSourceSpecificQueriesAvl_t    *timerData;
  groupSourceSpecificQueriesSource_t *iterator, *auxSourcePtr;
  uint32                             sourcesToSend = 0;

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group & Source Specific Timer Expired [groupAddr=0x%08X serviceId=%u portId=%u]", eventData->groupAddr.addr.ipv4.s_addr, eventData->serviceId, eventData->portId);

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&eventData->groupAddr, eventData->serviceId, eventData->portId, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested AVL entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
            eventData->groupAddr.addr.ipv4.s_addr, eventData->serviceId, eventData->portId);
    return FAILURE;
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return FAILURE;
  }

  //Build IGMP Query header, without any sources
  buildQueryHeader(PTIN_IGMP_VERSION_3, queryHeader, &queryHeaderLength, &timerData->key.groupAddr, FALSE);

  //For each source with active retransmissions, add them to the IGMP Query header
  for(iterator=timerData->firstSource; iterator!=PTIN_NULLPTR; iterator=auxSourcePtr)
  {
    auxSourcePtr = iterator->next;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Adding source[%08X] to Q(G,S) [groupAddr=0x%08X serviceId=%u portId=%u]", 
              iterator->sourceAddr.addr.ipv4.s_addr, eventData->groupAddr.addr.ipv4.s_addr, eventData->serviceId, eventData->portId);
    addSourceToQuery(queryHeader, &queryHeaderLength, &iterator->sourceAddr);
    ++sourcesToSend;

    //Reduce the number of retransmissions. If it reached 0, remove the source from the list
    if(--iterator->retransmissions == 0)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Last retransmission for source[%08X]", iterator->sourceAddr.addr.ipv4.s_addr);
      __groupsourcespecifictimer_delsource(timerData, iterator);
    }
  }

  if( (sourcesToSend > 0) || (timerData->retransmissions > 0) )
  {
    ptin_IgmpProxyCfg_t igmpGlobalCfg;

    if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
      return FAILURE;
    }

    //Reduce group retransmissions
    if(timerData->retransmissions > 0)
    {
      --timerData->retransmissions;
    }

    //Build the IGMP Query frame
    buildIgmpFrame(queryPckt.framePayload, &queryPckt.frameLength, queryHeader, queryHeaderLength);

    //Send Group-Source specific query to all leaf ports for this service
    queryPckt.serviceId  = timerData->key.serviceId;  
    queryPckt.clientId = (uint32)-1;
    queryPckt.family     = PTIN_MGMD_AF_INET;
    if(SUCCESS != ptinMgmdPacketPortSend(&queryPckt, PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY, timerData->key.portId))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to send source specific query for service[%u]", timerData->key.serviceId);
      return FAILURE;
    }

    //Schedule a new source-speficic query
    if(SUCCESS != ptin_mgmd_groupsourcespecifictimer_restart(&timerData->key))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to start group specific timer");
      return FAILURE;
    }
  }
  else
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "No more retransmissions left for this Group Specific Timer");
    if(SUCCESS != ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to delete Group Specific Timer [groupAddr=0x%08X serviceId=%u]", timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId);
      return FAILURE;
    }
    ptin_mgmd_timer_deinit(timerData->timerHandle);
  }

  return SUCCESS;
}

