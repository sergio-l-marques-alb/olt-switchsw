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

#include "ptin_mgmd_proxytimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h"

extern BOOL ptin_mgmd_extended_debug;

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_proxytimer_callback(void *param);
static RC_t  ptin_mgmd_proxytimer_init(PTIN_MGMD_TIMER_t *timerPtr);


void* ptin_mgmd_proxytimer_callback(void *param)
{
  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  mgmdProxyInterfaceTimer_t *timerData = (mgmdProxyInterfaceTimer_t*) param;
  PTIN_MGMD_EVENT_t         eventMsg = {0};

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Timer expired for [timerPtr=%p groupData=%p]", timerData, timerData->groupData);

  //Create new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_PROXY, (void*) timerData, sizeof(mgmdProxyInterfaceTimer_t));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }
  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_proxytimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_proxytimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_proxytimer_init(PTIN_MGMD_TIMER_t *timerPtr)
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
    ret = ptin_mgmd_timer_init(__controlBlock, timerPtr, ptin_mgmd_proxytimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_proxytimer_start(mgmdProxyInterfaceTimer_t* timer, uint32 timeout, uint8 reportType, BOOL isInterface, uint32 noOfRecords, void* groupData)
{
  RC_t ret = SUCCESS;
  uint32   newTimeOut;

  if( (PTIN_NULLPTR == timer) || (PTIN_NULLPTR == groupData) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [timer:%p groupData:%p]", timer, groupData);
    return FAILURE;
  }

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  

  if(SUCCESS != ptin_mgmd_proxytimer_init(&timer->newTimerHandle))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new proxy timer!");
    return FAILURE;
  }

  if (TRUE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
  {    
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "This timer is already running. Going to stop it!");
    if ((newTimeOut=ptin_mgmd_proxytimer_timeleft(timer))<timeout)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "timeleft<timeout",newTimeOut,timeout);
      timeout=newTimeOut;
    }
    ptin_mgmd_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    if(SUCCESS!=ptin_mgmd_timer_stop(timer->newTimerHandle))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed ptin_timer_stop()!");
      return FAILURE;
    }
  }
  else
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "New Proxy Timer %p ", timer->newTimerHandle);
    timer->groupData          = groupData;
    timer->isInterface        = isInterface;
    timer->reportType         = reportType;
    timer->noOfRecords        = noOfRecords;    
  }

  ptin_mgmd_measurement_timer_start(0,"ptin_mgmd_timer_start");
  ret = ptin_mgmd_timer_start(timer->newTimerHandle, timeout, timer);  
  ptin_mgmd_measurement_timer_stop(0);
  return ret;
}


RC_t ptin_mgmd_proxytimer_stop(mgmdProxyInterfaceTimer_t *timer)
{
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Proxy Timer Stop...");   
  if (TRUE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
  {
    ptin_mgmd_measurement_timer_start(1,"ptin_mgmd_timer_stop");
    ptin_mgmd_timer_stop(timer->newTimerHandle);
    ptin_mgmd_measurement_timer_stop(1);
  }

  
  ptin_mgmd_timer_deinit(timer->newTimerHandle);
  return SUCCESS;
}


uint32 ptin_mgmd_proxytimer_timeleft(mgmdProxyInterfaceTimer_t *timer)
{
  if (FALSE == ptin_mgmd_timer_isRunning(timer->newTimerHandle))
  {
    return 0;
  }
  uint32 timeLeft;
  ptin_mgmd_measurement_timer_start(2,"ptin_mgmd_timer_timeLeft");
  timeLeft=ptin_mgmd_timer_timeLeft(timer->newTimerHandle)/1000;
  ptin_mgmd_measurement_timer_stop(2);
  return timeLeft;
}


BOOL ptin_mgmd_proxytimer_isRunning(mgmdProxyInterfaceTimer_t *timer)
{
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"%s: timer %p", __FUNCTION__, timer->newTimerHandle);
  return ptin_mgmd_timer_isRunning(timer->newTimerHandle);
}


RC_t ptin_mgmd_event_proxytimer(mgmdProxyInterfaceTimer_t *timerData)
{
  char                      debug_buf[46];
  mgmdGroupRecord_t     *groupPtr          = PTIN_NULLPTR;
  mgmdProxyInterface_t *interfacePtr;
  BOOL                      isInterface;
  uint8                     reportType;
  uint32                    noOfRecords;
  void                      *groupData; 

  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  if (timerData->isInterface)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Proxy Interface timer expired (vlan:%u)",
            ((mgmdProxyInterface_t *) timerData->groupData)->key.serviceId);    
    interfacePtr    = (mgmdProxyInterface_t *) timerData->groupData;

    if(ptinMgmdProxyInterfaceEntryFind(interfacePtr->key.serviceId,AVL_EXACT)==PTIN_NULLPTR)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to find Interface Entry!");
      return SUCCESS;

    }
  }
  else
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Proxy Group timer expired(vlan:%u group:%s)",((mgmdGroupRecord_t *) timerData->groupData)->key.serviceId,
            ptin_mgmd_inetAddrPrint(&(((mgmdGroupRecord_t *) timerData->groupData)->key.groupAddr), debug_buf));

    groupPtr     = (mgmdGroupRecord_t *) timerData->groupData;
    interfacePtr =(mgmdProxyInterface_t*) groupPtr->interfacePtr;  
    if( (PTIN_NULLPTR == groupPtr) || (PTIN_NULLPTR == interfacePtr) )
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"This report has already been sent. Ignoring [groupPtr:%p interfacePtr:%p]", groupPtr, interfacePtr);
      return SUCCESS;
    }
      
    if(ptinMgmdProxyInterfaceEntryFind(interfacePtr->key.serviceId,AVL_EXACT)==PTIN_NULLPTR 
       || ptinMgmdProxyGroupEntryFind(groupPtr->key.serviceId, &groupPtr->key.groupAddr, groupPtr->key.recordType, AVL_EXACT)==PTIN_NULLPTR)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to find Interface or Group Entry!");
      return SUCCESS;
    }
  }


  isInterface=timerData->isInterface;
  reportType=timerData->reportType;
  noOfRecords=timerData->noOfRecords;
  groupData=timerData->groupData;

  if (interfacePtr==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"interfacePtr==PTIN_NULLPTR");
    return FAILURE;
  }
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trigger Membership Report Message");
  if (ptinMgmdScheduleReportMessage(interfacePtr->key.serviceId,&groupPtr->key.groupAddr,reportType,0,isInterface,noOfRecords,groupData)!=SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
    return FAILURE;
  } 
  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  return SUCCESS;
}

