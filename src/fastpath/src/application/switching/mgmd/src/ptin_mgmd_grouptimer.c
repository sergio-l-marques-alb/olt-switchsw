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

#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_grouptimer_callback(void *param);
static RC_t  ptin_mgmd_grouptimer_init(snoopPTinL3Grouptimer_t *pTimer);

void* ptin_mgmd_grouptimer_callback(void *param)
{
  snoopPTinL3Grouptimer_t* timerData = (snoopPTinL3Grouptimer_t*) param;
  PTIN_MGMD_EVENT_t        eventMsg = {0};

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Timer expired for [timerPtr=%p groupAddr=0x%08X serviceId=%u portId=%u]", 
            timerData->newTimerHandle, timerData->groupKey.groupAddr.addr.ipv4.s_addr, timerData->groupKey.serviceId, timerData->interfaceIdx);

  //Create a new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_GROUP, (void*) timerData, sizeof(snoopPTinL3Grouptimer_t));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }

  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_grouptimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_grouptimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_grouptimer_init(snoopPTinL3Grouptimer_t *pTimer)
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

  if(FALSE == ptin_mgmd_timer_exist(pTimer->newTimerHandle))
  {
    ret = ptin_mgmd_timer_init(__controlBlock, &(pTimer->newTimerHandle), ptin_mgmd_grouptimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_grouptimer_start(snoopPTinL3Grouptimer_t *timer, uint32 timeout, snoopPTinL3InfoDataKey_t groupKey, uint32 interfaceIdx)
{
  RC_t ret = SUCCESS;

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

   //Initialize Group Timer
  if(SUCCESS != ptin_mgmd_grouptimer_init(timer))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new group timer!");
    return FAILURE;
  }

  if (TRUE == ptin_mgmd_grouptimer_isRunning(timer))
  {
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "prt:[%p] timeleft:[%u]",timer->newTimerHandle,ptin_mgmd_grouptimer_timeleft(timer));
    ptin_mgmd_timer_stop(timer->newTimerHandle);
  }
  else
  {
    timer->groupKey     = groupKey;
    timer->interfaceIdx = interfaceIdx;
  }

  ret = ptin_mgmd_timer_start(timer->newTimerHandle, timeout*1000, timer);

//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "prt:[%p] timeleft:[%u]",timer->newTimerHandle,ptin_mgmd_grouptimer_timeleft(timer));
  return ret;
}


RC_t ptin_mgmd_grouptimer_stop(snoopPTinL3Grouptimer_t *timer)
{
  if (TRUE == ptin_mgmd_grouptimer_isRunning(timer))
  {
    ptin_mgmd_timer_stop(timer->newTimerHandle);
  }
  
  ptin_mgmd_timer_deinit(timer->newTimerHandle);
  return SUCCESS;
}


uint32 ptin_mgmd_grouptimer_timeleft(snoopPTinL3Grouptimer_t *timer)
{
  if (FALSE == ptin_mgmd_grouptimer_isRunning(timer))
  {
    return 0;
  }

  return ptin_mgmd_timer_timeLeft(timer->newTimerHandle)/1000;
}


BOOL ptin_mgmd_grouptimer_isRunning(snoopPTinL3Grouptimer_t *timer)
{
  return ptin_mgmd_timer_isRunning(timer->newTimerHandle);
}


RC_t ptin_mgmd_event_grouptimer(snoopPTinL3Grouptimer_t *timerData)
{
  char                     debug_buf[46];
  snoopPTinL3Interface_t  *interfacePtr;
  uint32                   portId;
  snoopPTinL3Source_t     *sourcePtr,
                          *sourcePtrAux;
  snoopPTinL3InfoData_t*   groupEntry;
  
//uint8              recordType=PTIN_MGMD_CHANGE_TO_INCLUDE_MODE;
  mgmdGroupRecord_t* groupPtr=PTIN_NULLPTR;       
  mgmdProxyInterface_t* proxyInterfacePtr=PTIN_NULLPTR;
  BOOL  newEntry=FALSE;

  if (PTIN_NULLPTR == (groupEntry = ptinMgmdL3EntryFind(timerData->groupKey.serviceId, &timerData->groupKey.groupAddr, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "We have an event Group Timer to process (serviceId:[%u] groupAddr:[0x%X]), but we were unable to find the entry in the AVL tree",timerData->groupKey.serviceId,timerData->groupKey.groupAddr.addr.ipv4.s_addr);
    return SUCCESS;
  }   

  //Save grouptimer's internal data 
  portId     = timerData->interfaceIdx;
  interfacePtr = &groupEntry->interfaces[portId];
  sourcePtr    = PTIN_NULLPTR;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Grouptimer expired (group:%s vlan:%u ifId:%u)", 
            ptin_mgmd_inetAddrPrint(&(groupEntry->snoopPTinL3InfoDataKey.groupAddr), debug_buf), groupEntry->snoopPTinL3InfoDataKey.serviceId, portId);

  /* Grouptimer triggers only affect groups in filter-mode EXCLUDE */
  if (interfacePtr->filtermode == PTIN_MGMD_FILTERMODE_EXCLUDE)
  {
    /* Remove every source whose timer has expired */
    for (sourcePtr=groupEntry->interfaces[portId].firstSource; sourcePtr!=PTIN_NULLPTR; sourcePtr=sourcePtrAux)
    { 
      sourcePtrAux=sourcePtr->next;
      if ((sourcePtr->status == PTIN_MGMD_SOURCESTATE_ACTIVE) && (ptin_mgmd_sourcetimer_isRunning(&sourcePtr->sourceTimer) == FALSE) && sourcePtr->isStatic==FALSE)
      {        
        ptinMgmdSourceRemove(groupEntry,portId, sourcePtr);       
      }      
    }

    /* If no sources remain, remove group. Otherwise, switch to filter-mode INCLUDE */
    if (interfacePtr->numberOfSources == 0)
    {
      if (interfacePtr->isStatic==FALSE)
      {
        if (portId==SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID)//Send To_In{0} Report to the Network
        {
          if ( (proxyInterfacePtr=ptinMgmdProxyInterfaceAdd(groupEntry->snoopPTinL3InfoDataKey.serviceId)) ==PTIN_NULLPTR)                     
          {             
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceAdd()");
            return FAILURE;
          }            
          
          if((groupPtr=ptinMgmdGroupRecordAdd(proxyInterfacePtr,PTIN_MGMD_CHANGE_TO_INCLUDE_MODE,&groupEntry->snoopPTinL3InfoDataKey.groupAddr,&newEntry ))==PTIN_NULLPTR)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
            return FAILURE;
          }

          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
          if(ptinMgmdScheduleReportMessage(groupEntry->snoopPTinL3InfoDataKey.serviceId,&groupEntry->snoopPTinL3InfoDataKey.groupAddr,PTIN_IGMP_V3_MEMBERSHIP_REPORT,0,FALSE,1,groupPtr)!=SUCCESS)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
            return FAILURE;
          } 
        }
         //Delete timer
        ptin_mgmd_grouptimer_stop(&interfacePtr->groupTimer);

        ptinMgmdInterfaceRemove(groupEntry,portId);
      }
    }
    else
    {
      interfacePtr->filtermode = PTIN_MGMD_FILTERMODE_INCLUDE;           
    }       
  }

  return SUCCESS;
}

