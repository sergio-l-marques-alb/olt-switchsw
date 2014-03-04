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

#include "ptin_mgmd_routercmtimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_routercmtimer_callback(void *param);
static RC_t  ptin_mgmd_routercmtimer_init(snoopPTinCMtimer_t* pTimer);


void* ptin_mgmd_routercmtimer_callback(void *param)
{
  PTIN_MGMD_EVENT_t eventMsg = {0}; 

  //Create a new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_ROUTERCM, (void*) &param, sizeof(snoopPTinCMtimer_t*));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }

  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_routercmtimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_routercmtimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_routercmtimer_init(snoopPTinCMtimer_t* pTimer)
{
  RC_t ret = SUCCESS;

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (PTIN_NULLPTR == pTimer)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trying to initialize a NULL timer!");
    return FAILURE;
  }

  if(FALSE == ptin_mgmd_timer_exist(pTimer->timer))
  {
    ret = ptin_mgmd_timer_init(__controlBlock, &(pTimer->timer), ptin_mgmd_routercmtimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_routercmtimer_start(ptinMgmdGroupInfoData_t *groupData, uint32 portId, ptin_IgmpProxyCfg_t *igmpCfg)
{
  RC_t               rc = SUCCESS; 
 
  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if(SUCCESS != ptin_mgmd_routercmtimer_init(&groupData->ports[portId].groupCMTimer))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new routerCM timer!");
    return FAILURE;
  }

  if (TRUE == ptin_mgmd_routercmtimer_isRunning(&groupData->ports[portId].groupCMTimer))
  {
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "prt:[%p] timeleft:[%u]",groupData->interfaces[portId].groupCMTimer.timer,ptin_mgmd_routercmtimer_timeleft(&groupData->interfaces[portId].groupCMTimer));
    ptin_mgmd_timer_stop(groupData->ports[portId].groupCMTimer.timer);
  }

  rc = ptin_mgmd_timer_start(groupData->ports[portId].groupCMTimer.timer, igmpCfg->querier.older_host_present_timeout*1000, &groupData->ports[portId].groupCMTimer);
//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "prt:[%p] timeleft:[%u]",groupData->interfaces[portId].groupCMTimer.timer,ptin_mgmd_routercmtimer_timeleft(&groupData->interfaces[portId].groupCMTimer));
  return rc;
}


RC_t ptin_mgmd_routercmtimer_stop(snoopPTinCMtimer_t* timerPtr)
{  
  if (TRUE == ptin_mgmd_routercmtimer_isRunning(timerPtr))
  { 
    ptin_mgmd_timer_stop(timerPtr->timer);
  }
  ptin_mgmd_timer_deinit(timerPtr->timer);  
  return SUCCESS;
}


uint32 ptin_mgmd_routercmtimer_timeleft(snoopPTinCMtimer_t* pTimer)
{
  if (FALSE == ptin_mgmd_routercmtimer_isRunning(pTimer))
  {
    return 0;
  }

  return ptin_mgmd_timer_timeLeft(pTimer->timer)/1000;
}


BOOL ptin_mgmd_routercmtimer_isRunning(snoopPTinCMtimer_t* pTimer)
{
  return ptin_mgmd_timer_isRunning(pTimer->timer);
}


RC_t ptin_mgmd_event_routercmtimer(snoopPTinCMtimer_t **timerData)
{
  ptin_IgmpProxyCfg_t igmpGlobalCfg;

  if (PTIN_NULLPTR == ptinMgmdL3EntryFind((*timerData)->groupKey.serviceId, &(*timerData)->groupKey.groupAddr, AVL_EXACT))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "We have an event Router Compatibility Mode Timer to process (serviceId:[%u] groupAddr:[0x%X]), but we were unable to find the entry in the AVL tree",(*timerData)->groupKey.serviceId,(*timerData)->groupKey.groupAddr.addr.ipv4.s_addr);
    return SUCCESS;
  }   
  
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Router compatibility-mode timer expired");

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting MGMD Proxy configurations");
    return ERROR;
  }

  (*timerData)->compatibilityMode = igmpGlobalCfg.clientVersion; //Restore compatibility-mode

  return SUCCESS;
}

