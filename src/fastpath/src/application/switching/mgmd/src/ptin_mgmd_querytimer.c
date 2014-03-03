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

#include "ptin_mgmd_querytimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_core.h"
#include "ptin_mgmd_db.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_querytimer_callback(void *param);
static RC_t  ptin_mgmd_querytimer_init(PTIN_MGMD_TIMER_t *timerPtr);

void* ptin_mgmd_querytimer_callback(void *param)
{
  ptinMgmdL3Querytimer_t*  timerData = (ptinMgmdL3Querytimer_t*) param;
  PTIN_MGMD_EVENT_t        eventMsg = {0};

  mgmdPtinQuerierTimerKey_t querierTimerKey;
  ptin_IgmpProxyCfg_t igmpGlobalCfg;
  uint32 timeout;
  ptinMgmdQuerierInfoData_t*  queryData=(ptinMgmdQuerierInfoData_t*) timerData->queryData;

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg)!=SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to get IGMP Proxy Configurations"); 
    return PTIN_NULLPTR;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Query Timer Expired (serviceId:%u , family:%u startUpQueryFlag:%u startUpQueryCount:%u), schedulling new General Query",
            queryData->key.serviceId,timerData->family,queryData->startUpQueryFlag,timerData->startUpQueryCount);
  
  if (queryData->startUpQueryFlag==TRUE)      
  {
    if ((++timerData->startUpQueryCount)>=igmpGlobalCfg.querier.startup_query_count)
    {
      queryData->startUpQueryFlag=FALSE;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Disabling  startup query flag (serviceId:[%u] , family:[%u])",queryData->key.serviceId,timerData->family);
      timeout=igmpGlobalCfg.querier.query_interval;    
    }
    else
    {
      timeout=igmpGlobalCfg.querier.startup_query_interval;              
    }    
  }
  else
  {
    timeout=igmpGlobalCfg.querier.query_interval;        
  }
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting Query Timeout to Query Interval %u",timeout);
 
  if(SUCCESS!=ptin_mgmd_querytimer_start(timerData, timeout, timerData->queryData,timerData->family))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to start Query Timer sId:%u",queryData->key.serviceId);
    return PTIN_NULLPTR;
  }


  memset(&querierTimerKey, 0x00, sizeof(querierTimerKey));
  querierTimerKey.querierKey=queryData->key;
  querierTimerKey.family=timerData->family;

  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_QUERY, (void*) &querierTimerKey, sizeof(querierTimerKey));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }
  
  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_querytimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_querytimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_querytimer_init(PTIN_MGMD_TIMER_t *timerPtr)
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
    ret = ptin_mgmd_timer_init(__controlBlock, timerPtr, ptin_mgmd_querytimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_querytimer_start(ptinMgmdL3Querytimer_t* timer, uint32 timeout, void* queryData, uint8 family)
{
  RC_t ret = SUCCESS;

  ptinMgmdQuerierInfoData_t *pMgmdEntry;

  if(queryData==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters");
    return FAILURE;
  }

  pMgmdEntry=(ptinMgmdQuerierInfoData_t*) queryData;
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ServiceId:%u family:%u", pMgmdEntry->key.serviceId,family);

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if(SUCCESS != ptin_mgmd_querytimer_init(&timer->newTimerHandle))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new query timer!");
    return FAILURE;
  }

  if (TRUE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "This timer is already running. Going to stop it!");
    ptin_mgmd_timer_stop(timer->newTimerHandle);
  }
  else
  {
    
    timer->queryData = queryData;    
    timer->family    = family;
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "New Query Timer (ServiceId:%u family:%u)", pMgmdEntry->key.serviceId,family);
  }

  ret = ptin_mgmd_timer_start(timer->newTimerHandle, timeout*1000, timer);
  return ret;
}


RC_t ptin_mgmd_querytimer_stop(ptinMgmdL3Querytimer_t *timer)
{
  if(&(timer->newTimerHandle)!=PTIN_NULLPTR)
  {
    if (TRUE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
    {
      ptin_mgmd_timer_stop(timer->newTimerHandle);
    }
    ptin_mgmd_timer_deinit(timer->newTimerHandle);
  }
  else
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"NULL Query Timer Handle"); 
  }  
  
  return SUCCESS;
}


uint32 ptin_mgmd_querytimer_timeleft(ptinMgmdL3Querytimer_t *timer)
{
  if (FALSE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
  {
    return 0;
  }

  return ptin_mgmd_timer_timeLeft(timer->newTimerHandle)/1000;
}


BOOL ptin_mgmd_querytimer_isRunning(ptinMgmdL3Querytimer_t *timer)
{
  return ptin_mgmd_timer_isRunning(timer->newTimerHandle);
}

RC_t ptin_mgmd_event_querytimer(mgmdPtinQuerierTimerKey_t* eventData)
{
  mgmdPtinQuerierTimerKey_t querierTimerKey; 
  ptinMgmdQuerierInfoData_t *mgmdPTinQuerierPtr;

  if (eventData==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [eventData=%p]", eventData);
    return ERROR;
  }

  memcpy(&querierTimerKey, eventData, sizeof(querierTimerKey));  
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Processing Event Query Timer (serviceId:%u family:%u)",querierTimerKey.querierKey.serviceId,querierTimerKey.family);

  if ((mgmdPTinQuerierPtr=ptinMgmdQueryEntryFind(querierTimerKey.querierKey.serviceId,querierTimerKey.family,AVL_EXACT))==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "We have an event Query type to process (serviceId:%u family:%u), but we were unable to find the timer!",querierTimerKey.querierKey.serviceId,querierTimerKey.family);
    return SUCCESS;
  }
  else
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Triggering a General Query (serviceId:%u family:%u)",querierTimerKey.querierKey.serviceId,querierTimerKey.family);
    ptinMgmdGeneralQuerySend(querierTimerKey.querierKey.serviceId,querierTimerKey.family);
  }  

  return SUCCESS;
}

