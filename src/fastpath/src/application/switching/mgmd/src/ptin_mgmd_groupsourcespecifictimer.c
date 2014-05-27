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
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_core.h"
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
  ptin_mgmd_eb_t *pMgmdEB;  
  RC_t            res = SUCCESS;
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
  memset(new_source, 0x00, sizeof(*new_source));
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
  ptin_mgmd_eb_t *pMgmdEB;  
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

  if(FALSE == ptin_mgmd_timer_exists(__controlBlock, *timerPtr))
  {
    ret = ptin_mgmd_timer_init(__controlBlock, timerPtr, ptin_mgmd_groupsourcespecifictimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_groupspecifictimer_start(ptinMgmdGroupInfoData_t* groupEntry, uint16 portId, uint32 clientId, ptin_IgmpProxyCfg_t *igmpCfg)
{
  groupSourceSpecificQueriesAvl_t *timerData; 
  uchar8                           queryHeader[PTIN_MGMD_MAX_FRAME_SIZE] = {0};
  uint32                           queryHeaderLength = 0;
  ptin_mgmd_externalapi_t          externalApi;
  ptinMgmdControlPkt_t             queryPckt         = {0};
  uint32                           gtTimeLeft, lmqt;
   
  if(PTIN_NULLPTR == groupEntry)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [groupAddr=%p]", groupEntry);
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting group specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return FAILURE;
  }

  /* Get Snoop Control Block */
  if (( queryPckt.cbHandle = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }
  //Saving igmpCfg
  queryPckt.cbHandle->mgmdProxyCfg=*igmpCfg;

  //Build IGMP Query header, without any sources
  buildQueryHeader(groupEntry->ports[portId].groupCMTimer.compatibilityMode, queryHeader, &queryHeaderLength, &groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, FALSE);

  //Build the IGMP Query frame
  buildIgmpFrame(queryPckt.framePayload, &queryPckt.frameLength, queryHeader, queryHeaderLength);

  //Send Group-Source specific query to all leaf ports for this service
  queryPckt.serviceId  = groupEntry->ptinMgmdGroupInfoDataKey.serviceId;  
  queryPckt.clientId = clientId;
  queryPckt.family     = PTIN_MGMD_AF_INET;
  if(SUCCESS != ptinMgmdPacketPortSend(&queryPckt, PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY, portId))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to send source specific query for service[%u]", groupEntry->ptinMgmdGroupInfoDataKey.serviceId);
    return FAILURE;
  }
  
  lmqt       = igmpCfg->querier.last_member_query_interval * igmpCfg->querier.last_member_query_count*100;//Convert from dS -> mS
  gtTimeLeft = ptin_mgmd_grouptimer_timeleft(&groupEntry->ports[portId].groupTimer)*1000;//Convert from S -> mS 

  //Set group-timer to LMQT. If this is the only interface in the root port, set the root port group-timer to LMQT as well
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting interface timer to LMQT");
  if(SUCCESS != ptin_mgmd_grouptimer_start(&groupEntry->ports[portId].groupTimer, lmqt+500, groupEntry->ptinMgmdGroupInfoDataKey, portId))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
    return FAILURE;
  }
  
  if(groupEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients==0 ||
     (TRUE == PTIN_MGMD_CLIENT_IS_MASKBITSET(groupEntry->ports[PTIN_MGMD_ROOT_PORT].clients, portId) && groupEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients == 1))
  {
    //Set group-timer to LMQT. If this is the only interface in the root port, set the root port group-timer to LMQT as well
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting root interface timer to LMQT");
    if(SUCCESS != ptin_mgmd_grouptimer_start(&groupEntry->ports[PTIN_MGMD_ROOT_PORT].groupTimer, lmqt+1000, groupEntry->ptinMgmdGroupInfoDataKey, PTIN_MGMD_ROOT_PORT))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]",
                   groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, PTIN_MGMD_ROOT_PORT);
      return FAILURE;
    }
  }  
  
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId)))
  {
    if(igmpCfg->querier.last_member_query_count>1)
    {
       //Add new entry to the AVL tree with the parameters in groupData
      if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryAdd(&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId)))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to add a new groupspecific timer entry [groupAddr=0x%08X serviceId=%u portId=%u]", groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
        return TABLE_IS_FULL;
      }
      timerData->retransmissions = igmpCfg->querier.last_member_query_count-1;
      timerData->compatibilityMode = groupEntry->ports[portId].groupCMTimer.compatibilityMode;
      timerData->clientId = clientId;      
      if(gtTimeLeft > lmqt)
      {
        timerData->supressRouterSideProcessing = TRUE;
      }
      else
      {
        timerData->supressRouterSideProcessing = FALSE;
      }

      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "New group specific timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
      if(SUCCESS != ptin_mgmd_groupsourcespecifictimer_init(&timerData->timerHandle))
      {
        PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new groupspecific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
        return FAILURE;
      }
    }
  }
  else
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Restarting groupspecific [groupAddr=0x%08X serviceId=%u portId=%u]", 
              groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
    ptin_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    ptin_mgmd_timer_stop(__controlBlock, timerData->timerHandle);
    ptin_measurement_timer_stop(1);
    timerData->retransmissions = igmpCfg->querier.last_member_query_count-1;
    timerData->clientId = clientId;

    if(igmpCfg->querier.last_member_query_count==1)
    {
      ptin_mgmd_groupsourcespecifictimer_removegroup(&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr,groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
    }
  }

  if(igmpCfg->querier.last_member_query_count>1)
  {
    ptin_measurement_timer_start(0,"ptin_mgmd_timer_start");
    //Schedule a new source-speficic query
    if(SUCCESS != ptin_mgmd_timer_start(__controlBlock, timerData->timerHandle, igmpCfg->querier.last_member_query_interval*100, &timerData->key))
    {
      ptin_measurement_timer_stop(0);
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to start group specific timer");
      return FAILURE;
    }
    ptin_measurement_timer_stop(0);
  }

  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_start(ptinMgmdGroupInfoData_t* groupEntry, uint16 portId, uint32 clientId, ptin_IgmpProxyCfg_t *igmpCfg)
{
  uchar8                                 queryHeader[PTIN_MGMD_MAX_FRAME_SIZE] = {0};
  uint32                                 queryHeaderLength = 0;
  ptin_mgmd_externalapi_t                externalApi;
  ptinMgmdControlPkt_t                   queryPckt         = {0};
  groupSourceSpecificQueriesAvl_t       *timerData; 
  groupSourceSpecificQueriesSource_t    *auxSourcePtr;
  groupSourceSpecificQueriesSource_t    *iterator;
  uint32                                 sourcesToSend = 0;
  
  if(PTIN_NULLPTR == groupEntry)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [groupAddr=%p]", groupEntry);
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting sourcespecific timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  /* Get Snoop Control Block */
  if (( queryPckt.cbHandle = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }
  //Saving igmpCfg
  queryPckt.cbHandle->mgmdProxyCfg=*igmpCfg;

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId)))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Nothing to be done for this Q(G,S) [groupAddr=0x%08X serviceId=%u portId=%u]", 
            groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
    return SUCCESS;
  }
  else
  {
    ptin_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    ptin_mgmd_timer_stop(__controlBlock, timerData->timerHandle);
    ptin_measurement_timer_stop(1);
  }

  timerData->compatibilityMode = groupEntry->ports[portId].groupCMTimer.compatibilityMode;
  timerData->clientId          = clientId;
  if(timerData->numberOfSources != 0)
  {
    if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
      return FAILURE;
    }
    
    //Build IGMP Query header, without any sources
    buildQueryHeader(groupEntry->ports[portId].groupCMTimer.compatibilityMode, queryHeader, &queryHeaderLength, &timerData->key.groupAddr, FALSE);

    //For each source with active retransmissions, add them to the IGMP Query header
    for(iterator=timerData->firstSource; iterator!=PTIN_NULLPTR; iterator=auxSourcePtr)
    {
      auxSourcePtr = iterator->next;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Adding source[%08X] to Q(G,S) [groupAddr=0x%08X serviceId=%u portId=%u]", 
                iterator->sourceAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, groupEntry->ptinMgmdGroupInfoDataKey.serviceId, portId);
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
      //Reduce group retransmissions
      if(timerData->retransmissions > 0)
      {
        --timerData->retransmissions;
      }
      //Build the IGMP Query frame
      buildIgmpFrame(queryPckt.framePayload, &queryPckt.frameLength, queryHeader, queryHeaderLength);

      //Send Group-Source specific query to all leaf ports for this service
      queryPckt.serviceId  = timerData->key.serviceId;  
      queryPckt.clientId = clientId;
      queryPckt.family     = PTIN_MGMD_AF_INET;
      if(SUCCESS != ptinMgmdPacketPortSend(&queryPckt, PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY, timerData->key.portId))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to send source specific query for service[%u]", timerData->key.serviceId);
        return FAILURE;
      }
      ptin_measurement_timer_start(0,"ptin_mgmd_timer_start");
      //Schedule a new source-speficic query
      if(SUCCESS != ptin_mgmd_timer_start(__controlBlock, timerData->timerHandle, igmpCfg->querier.last_member_query_interval*100, &timerData->key))
      {
        ptin_measurement_timer_stop(0);
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to start group specific timer");
        return FAILURE;
      }
      ptin_measurement_timer_stop(0);
    }
  }
  else
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group and Source Specific Query without Sources");
    if(SUCCESS != ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to delete Group Specific Timer [groupAddr=0x%08X serviceId=%u portId=%u]", 
              timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId, timerData->key.portId);
      return FAILURE;
    }    
  }  

  return SUCCESS;
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

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&timerKey->groupAddr, timerKey->serviceId, timerKey->portId)))
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

  ptin_measurement_timer_start(0,"ptin_mgmd_timer_start");
  ret = ptin_mgmd_timer_start(__controlBlock, timerData->timerHandle, igmpGlobalCfg.querier.last_member_query_interval*100, &timerData->key);
  ptin_measurement_timer_stop(0);
  return ret;
}


RC_t ptin_mgmd_groupsourcespecifictimer_addsource(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId, ptin_mgmd_inet_addr_t* sourceAddr)
{
  groupSourceSpecificQueriesAvl_t  *timerData;
  ptin_IgmpProxyCfg_t               igmpGlobalCfg;
  uint32                            i;   
  ptinMgmdSource_t              *sourcePtr;
  ptinMgmdGroupInfoData_t            *groupEntry;
  uint32                            lmqt;
    
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Adding new source to source specific timer [groupAddr=0x%08X serviceId=%u portId=%u sourceAddr=0x%08X]",
            groupAddr->addr.ipv4.s_addr, serviceId, portId, sourceAddr->addr.ipv4.s_addr);

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  lmqt = igmpGlobalCfg.querier.last_member_query_interval/10 * igmpGlobalCfg.querier.last_member_query_count;//dS -> S

  //Find entry in the AVL
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId)))
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
        iterator->retransmissions = igmpGlobalCfg.querier.last_member_query_count-1; //Reset retransmissions counter
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
  if (PTIN_NULLPTR == (groupEntry = ptinMgmdL3EntryFind(serviceId, groupAddr)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to find group entry [groupAddr=0x%08X serviceId=%u]", groupAddr->addr.ipv4.s_addr, serviceId);
    return FAILURE;
  }
  if ( (sourcePtr=ptinMgmdSourceFind(groupEntry,portId, sourceAddr))!=PTIN_NULLPTR)  
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting source[%08X] timer to LMQT[%u]", sourceAddr->addr.ipv4.s_addr, lmqt);
    if(SUCCESS != ptin_mgmd_sourcetimer_start(&sourcePtr->sourceTimer, lmqt, groupEntry->ptinMgmdGroupInfoDataKey, portId, sourcePtr))
    {
      PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]", groupAddr->addr.ipv4.s_addr, serviceId, portId);
      return FAILURE;
    }
  }  
  if ( (sourcePtr=ptinMgmdSourceFind(groupEntry,PTIN_MGMD_ROOT_PORT,sourceAddr)) !=PTIN_NULLPTR)  
  { 
    if(sourcePtr->numberOfClients==0 ||
     (TRUE == PTIN_MGMD_CLIENT_IS_MASKBITSET(sourcePtr->clients, portId) && sourcePtr->numberOfClients == 1))
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting source[%08X] timer to LMQT[%u]", sourceAddr->addr.ipv4.s_addr, lmqt);
      if(SUCCESS != ptin_mgmd_sourcetimer_start(&sourcePtr->sourceTimer, lmqt+1, groupEntry->ptinMgmdGroupInfoDataKey, PTIN_MGMD_ROOT_PORT, sourcePtr))
      {
        PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to restart group timer [groupAddr=0x%08X serviceId=%u portId=%u]",
                     groupAddr->addr.ipv4.s_addr, serviceId, PTIN_MGMD_ROOT_PORT);
        return FAILURE;
      }
    }    
  }
  
  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_removesource(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId, ptin_mgmd_inet_addr_t* sourceAddr)
{
  groupSourceSpecificQueriesAvl_t *timerData;
    
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Removing source from Q(G,S) timer [groupAddr=0x%08X serviceId=%u portId=%u sourceAddr=0x%08X]",
            groupAddr->addr.ipv4.s_addr, serviceId, portId, sourceAddr->addr.ipv4.s_addr);

  //Find entry in the AVL
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId)))
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
              groupAddr->addr.ipv4.s_addr, serviceId, portId);
    return SUCCESS;
  }
  else
  {
    if(timerData->numberOfSources!=0) //Group & Source Specific Query
    {
      groupSourceSpecificQueriesSource_t *iterator;
      uint32                             i; 

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
      //If no more sources exist. Remove this Group & Source Specific Query
      if(timerData->numberOfSources==0)
      {
        //Ensure that the timer is not running
        ptin_mgmd_groupsourcespecifictimer_stop(timerData->timerHandle);

        ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId);
      }
    }
    else //Group Specific Query
    {      
      //No source found      
      return SUCCESS;
    }    
  }

  return SUCCESS;
}


RC_t ptin_mgmd_groupsourcespecifictimer_removegroup(ptin_mgmd_inet_addr_t* groupAddr, uint32 serviceId, uint16 portId)
{
  groupSourceSpecificQueriesAvl_t *timerData;
    
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Removing  Q(G,S) timer [groupAddr=0x%08X serviceId=%u portId=%u]",
            groupAddr->addr.ipv4.s_addr, serviceId, portId);

  //Find entry in the AVL
  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(groupAddr, serviceId, portId)))
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
              groupAddr->addr.ipv4.s_addr, serviceId, portId);
    return SUCCESS;
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
      ptin_mgmd_inetAddressZeroSet(iterator->sourceAddr.family, &iterator->sourceAddr);
      iterator->retransmissions = 0;
      --timerData->numberOfSources;

      __groupsourcespecifictimer_delsource(timerData, iterator);
      
    }
  }

  ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId);
  
  return SUCCESS;
}

RC_t ptin_mgmd_groupsourcespecifictimer_remove_entry(groupSourceSpecificQueriesAvl_t *avlTreeEntry)
{
  groupSourceSpecificQueriesSource_t *iterator;
  uint32                             i; 

  //Ensure that the timer is not running
  ptin_mgmd_groupsourcespecifictimer_stop(avlTreeEntry->timerHandle);

  //Search for the requested source
  for(iterator=avlTreeEntry->firstSource, i=0; iterator!=PTIN_NULLPTR && i<avlTreeEntry->numberOfSources; iterator=iterator->next, ++i)
  {
    ptin_mgmd_inetAddressZeroSet(iterator->sourceAddr.family, &iterator->sourceAddr);
    iterator->retransmissions = 0;
    --avlTreeEntry->numberOfSources;

    __groupsourcespecifictimer_delsource(avlTreeEntry, iterator);
    
  }
  
  ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&avlTreeEntry->key.groupAddr, avlTreeEntry->key.serviceId, avlTreeEntry->key.portId);

  return SUCCESS;
}

RC_t ptin_mgmd_groupsourcespecifictimer_stop(PTIN_MGMD_TIMER_t timer)
{
  if(TRUE == ptin_mgmd_timer_exists(__controlBlock, timer))
  {
    if (TRUE == ptin_mgmd_timer_isRunning(__controlBlock, timer))
    {
      ptin_measurement_timer_start(1,"ptin_mgmd_timer_stop");
      ptin_mgmd_timer_stop(__controlBlock, timer);  
      ptin_measurement_timer_stop(1);
    }

    if( SUCCESS == ptin_mgmd_timer_free(__controlBlock, timer))
    {
      timer=PTIN_NULLPTR;
    }
  }
  return SUCCESS;
}


RC_t ptin_mgmd_event_groupsourcespecifictimer(groupSourceSpecificQueriesAvlKey_t* eventData)
{
  uchar8                                 queryHeader[PTIN_MGMD_MAX_FRAME_SIZE] = {0};
  uint32                                 queryHeaderLength = 0;
  ptin_mgmd_externalapi_t                externalApi;
  ptinMgmdControlPkt_t                   queryPckt         = {0};
  groupSourceSpecificQueriesAvl_t       *timerData;
  groupSourceSpecificQueriesSource_t    *iterator;
  groupSourceSpecificQueriesSource_t    *auxSourcePtr;
  uint32                                 sourcesToSend = 0;
  
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group & Source Specific Timer Expired [groupAddr=0x%08X serviceId=%u portId=%u]", eventData->groupAddr.addr.ipv4.s_addr, eventData->serviceId, eventData->portId);

  if(PTIN_NULLPTR == (timerData = ptinMgmdGroupSourceSpecificQueryAVLTreeEntryFind(&eventData->groupAddr, eventData->serviceId, eventData->portId)))
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested AVL entry [groupAddr=0x%08X serviceId=%u portId=%u]", 
            eventData->groupAddr.addr.ipv4.s_addr, eventData->serviceId, eventData->portId);
    return SUCCESS;
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return FAILURE;
  }

    /* Get Snoop Control Block */
  if (( queryPckt.cbHandle = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }
  //Saving igmpCfg
  if (ptin_mgmd_igmp_proxy_config_get(&queryPckt.cbHandle->mgmdProxyCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  //Build IGMP Query header, without any sources
  buildQueryHeader(timerData->compatibilityMode, queryHeader, &queryHeaderLength, &timerData->key.groupAddr, FALSE);

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
    //Reduce group retransmissions
    if(timerData->retransmissions > 0)
    {
      --timerData->retransmissions;
    }

    //Build the IGMP Query frame
    buildIgmpFrame(queryPckt.framePayload, &queryPckt.frameLength, queryHeader, queryHeaderLength);

    //Send Group-Source specific query to all leaf ports for this service
    queryPckt.serviceId  = timerData->key.serviceId;  
    queryPckt.clientId = timerData->clientId;
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
    
    if(SUCCESS != ptin_mgmd_groupsourcespecifictimer_stop(timerData->timerHandle))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to delete Group Specific Timer [timerData=%p]",timerData);
      return FAILURE;
    }

    if(SUCCESS != ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&timerData->key.groupAddr, timerData->key.serviceId, timerData->key.portId))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to delete Group Specific Timer [groupAddr=0x%08X serviceId=%u]", timerData->key.groupAddr.addr.ipv4.s_addr, timerData->key.serviceId);
      return FAILURE;
    }
   
  }

  return SUCCESS;
}

