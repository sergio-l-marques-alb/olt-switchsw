/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2013-2013
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
  ptinMgmdRootCMtimer_t *timerData = (ptinMgmdRootCMtimer_t*) param;
  PTIN_MGMD_EVENT_t  eventMsg = {0}; 

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Timer expired for [timerPtr=%p serviceId=%u compatibilityMode=%u]", 
            timerData->timer, timerData->serviceId, timerData->compatibilityMode);

  //Create a new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_PROXYCM, (void*) &param, sizeof(ptinMgmdRootCMtimer_t*));
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


RC_t ptin_mgmd_proxycmtimer_init(ptinMgmdRootCMtimer_t* pTimer)
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


RC_t ptin_mgmd_proxycmtimer_start(uint32 posId, ptin_mgmd_cb_t* pMgmdCB, ptin_IgmpProxyCfg_t *igmpCfg)
{
  RC_t                ret = SUCCESS;  

  if(PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if(FALSE == ptin_mgmd_timer_exists(__controlBlock, pMgmdCB->proxyCM[posId].timer))
  {
    if(SUCCESS != ptin_mgmd_proxycmtimer_init(&(pMgmdCB->proxyCM[posId])))
    {
      PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error: Unable to initialize a new proxy timer for posId=%u", posId);   
      return FAILURE;
    }
  }

  if(TRUE == ptin_mgmd_proxycmtimer_isRunning(&(pMgmdCB->proxyCM[posId])))
  {
    ptin_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    ptin_mgmd_timer_stop(__controlBlock, pMgmdCB->proxyCM[posId].timer);
    ptin_measurement_timer_stop(1);
  }

  ptin_measurement_timer_start(0,"ptin_mgmd_timer_start");
  ret = ptin_mgmd_timer_start(__controlBlock, pMgmdCB->proxyCM[posId].timer, igmpCfg->host.older_querier_present_timeout*1000, &pMgmdCB->proxyCM[posId]);
  ptin_measurement_timer_stop(0);
  return ret;
}


RC_t ptin_mgmd_proxycmtimer_stop(ptinMgmdRootCMtimer_t* pTimer)
{
  if (TRUE == ptin_mgmd_proxycmtimer_isRunning(pTimer))
  {
    ptin_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    ptin_mgmd_timer_stop(__controlBlock, pTimer->timer);
    ptin_measurement_timer_stop(1);
  }
  
  ptin_mgmd_timer_free(__controlBlock, pTimer->timer);
  return SUCCESS;
}


uint32 ptin_mgmd_proxycmtimer_timeleft(ptinMgmdRootCMtimer_t* pTimer)
{
  if (FALSE == ptin_mgmd_proxycmtimer_isRunning(pTimer))
  {
    return 0;
  }
  
  uint32 timeLeft;
  ptin_measurement_timer_start(2,"ptin_mgmd_timer_timeLeft");
  timeLeft=ptin_mgmd_timer_timeLeft(__controlBlock, pTimer->timer)/1000;
  ptin_measurement_timer_stop(2);
  return timeLeft;
}


BOOL ptin_mgmd_proxycmtimer_isRunning(ptinMgmdRootCMtimer_t* pTimer)
{
  return ptin_mgmd_timer_isRunning(__controlBlock, pTimer->timer);
}


RC_t ptin_mgmd_event_proxycmtimer(ptinMgmdRootCMtimer_t **timerData)
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

