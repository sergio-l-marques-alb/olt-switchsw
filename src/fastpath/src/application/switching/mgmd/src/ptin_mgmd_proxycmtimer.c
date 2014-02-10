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

#include "ptin_mgmd_proxycmtimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_cnfgr.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_proxycmtimer_callback(void *param);


void* ptin_mgmd_proxycmtimer_callback(void *param)
{
  snoopPTinCMtimer_t *timerData = (snoopPTinCMtimer_t*) param;
  PTIN_MGMD_EVENT_t  eventMsg = {0}; 

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Timer expired for [timerPtr=%p groupAddr=0x%08X serviceId=%u compatibilityMode=%u]", 
            timerData->timer, timerData->groupKey.groupAddr.addr.ipv4.s_addr, timerData->groupKey.serviceId, timerData->compatibilityMode);

  //Create a new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_PROXYCM, (void*) &param, sizeof(snoopPTinCMtimer_t*));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }

  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_proxycmtimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_proxycmtimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_proxycmtimer_init(snoopPTinCMtimer_t* pTimer)
{
  RC_t ret = SUCCESS;


  if (PTIN_NULLPTR == pTimer)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trying to initialize a NULL timer!");
    return FAILURE;
  }

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  ret = ptin_mgmd_timer_init(__controlBlock, &(pTimer->timer), ptin_mgmd_proxycmtimer_callback);
  return ret;
}


RC_t ptin_mgmd_proxycmtimer_start(uint32 serviceId)
{
  RC_t                ret = SUCCESS;
  ptin_IgmpProxyCfg_t igmpGlobalCfg;
  mgmd_cb_t       *pMgmdCB = PTIN_NULLPTR; 

  if(PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if(ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get IGMP Proxy Configurations");
    return FAILURE;
  }

  if((pMgmdCB=mgmdCBGet(PTIN_MGMD_AF_INET))==PTIN_NULLPTR)
  {   
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get pMgmdCB family:%u", PTIN_MGMD_AF_INET);   
    return FAILURE;
  }

  if(FALSE == ptin_mgmd_timer_exist(pMgmdCB->proxyCM[serviceId].timer))
  {
    if(SUCCESS != ptin_mgmd_proxycmtimer_init(&(pMgmdCB->proxyCM[serviceId])))
    {
      PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to initialize a new proxy timer for service=%u", serviceId);   
      return FAILURE;
    }
  }

  if(TRUE == ptin_mgmd_proxycmtimer_isRunning(&(pMgmdCB->proxyCM[serviceId])))
  {
    ptin_mgmd_timer_stop(pMgmdCB->proxyCM[serviceId].timer);
  }

  ret = ptin_mgmd_timer_start(pMgmdCB->proxyCM[serviceId].timer, igmpGlobalCfg.host.older_querier_present_timeout*1000, &pMgmdCB->proxyCM[serviceId]);
  return ret;
}


RC_t ptin_mgmd_proxycmtimer_stop(snoopPTinCMtimer_t* pTimer)
{
  if (TRUE == ptin_mgmd_proxycmtimer_isRunning(pTimer))
  {
    ptin_mgmd_timer_stop(pTimer->timer);
  }
  
  ptin_mgmd_timer_deinit(pTimer->timer);
  return SUCCESS;
}


uint32 ptin_mgmd_proxycmtimer_timeleft(snoopPTinCMtimer_t* pTimer)
{
  if (FALSE == ptin_mgmd_proxycmtimer_isRunning(pTimer))
  {
    return 0;
  }

  return ptin_mgmd_timer_timeLeft(pTimer->timer)/1000;
}


BOOL ptin_mgmd_proxycmtimer_isRunning(snoopPTinCMtimer_t* pTimer)
{
  return ptin_mgmd_timer_isRunning(pTimer->timer);
}


RC_t ptin_mgmd_event_proxycmtimer(snoopPTinCMtimer_t **timerData)
{
  ptin_IgmpProxyCfg_t igmpGlobalCfg;

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Proxy compatibility-mode timer expired");
  
  //Validations
  if( (PTIN_NULLPTR == timerData) || (PTIN_NULLPTR == *timerData) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [timerData:%p *timerData:%p]", timerData, *timerData);
    return ERROR;
  }

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting MGMD Proxy configurations");
    return ERROR;
  }

  (*timerData)->compatibilityMode = igmpGlobalCfg.networkVersion; //Restore compatibility-mode

  return SUCCESS;
}

