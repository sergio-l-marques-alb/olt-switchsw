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

#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_db.h"
#include "logger.h"
#include "ptin_mgmd_cnfgr.h"

static PTIN_MGMD_TIMER_CB_t __controlBlock = PTIN_NULLPTR;

static void* ptin_mgmd_sourcetimer_callback(void *param);
static RC_t  ptin_mgmd_sourcetimer_init(snoopPTinL3Sourcetimer_t *timer);


void* ptin_mgmd_sourcetimer_callback(void *param)
{
  snoopPTinL3Sourcetimer_t* timerData = (snoopPTinL3Sourcetimer_t*) param;
  PTIN_MGMD_EVENT_t         eventMsg = {0};

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Timer expired for [timerPtr=%p groupAddr=0x%08X serviceId=%u portId=%u sourceAddr=0x%08X]", 
            timerData->newTimerHandle, timerData->groupKey.groupAddr.addr.ipv4.s_addr, timerData->groupKey.serviceId, timerData->interfaceIdx, timerData->sourcePtr->sourceAddr.addr.ipv4.s_addr);

  //Create new timer event
  ptin_mgmd_event_timer_create(&eventMsg, PTIN_MGMD_EVENT_TIMER_TYPE_SOURCE, (void*) timerData, sizeof(snoopPTinL3Sourcetimer_t));
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&eventMsg))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add event to the message queue");
  }

  return PTIN_NULLPTR;
}

RC_t ptin_mgmd_sourcetimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock)
{
  __controlBlock = controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_sourcetimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock)
{
  if (PTIN_NULLPTR == controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [controlBlock:%p]", controlBlock);
    return FAILURE;
  }

  *controlBlock = __controlBlock;
  return SUCCESS;
}


RC_t ptin_mgmd_sourcetimer_init(snoopPTinL3Sourcetimer_t *timer)
{
  RC_t ret = SUCCESS;

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if (PTIN_NULLPTR == timer)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Trying to initialize a NULL timer!");
    return FAILURE;
  }

  if(FALSE == ptin_mgmd_timer_exist(timer->newTimerHandle))
  {
    ret = ptin_mgmd_timer_init(__controlBlock, &(timer->newTimerHandle), ptin_mgmd_sourcetimer_callback);
  }
  return ret;
}


RC_t ptin_mgmd_sourcetimer_start(snoopPTinL3Sourcetimer_t *timer, uint32 timeout, snoopPTinL3InfoDataKey_t groupKey, uint32 interfaceIdx,  snoopPTinL3Source_t *sourcePtr)
{
  RC_t ret = SUCCESS;

  if (PTIN_NULLPTR == __controlBlock)
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ControlBlock has not been initialized yet!");
    return FAILURE;
  }

  if(SUCCESS != ptin_mgmd_sourcetimer_init(timer))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize a new source timer!");
    return FAILURE;
  }

  if (TRUE == ptin_mgmd_sourcetimer_isRunning(timer))
  {
    ptin_mgmd_timer_stop(timer->newTimerHandle);
  } 
  else
  {
    timer->groupKey    = groupKey;
    timer->interfaceIdx = interfaceIdx;
    timer->sourcePtr    = sourcePtr;
  }

  ret = ptin_mgmd_timer_start(timer->newTimerHandle, timeout*1000, timer);
  return ret;
}


RC_t ptin_mgmd_sourcetimer_stop(snoopPTinL3Sourcetimer_t *timer)
{
  if (TRUE == ptin_mgmd_sourcetimer_isRunning(timer))
  {
    ptin_mgmd_timer_stop(timer->newTimerHandle);
  }
  
  ptin_mgmd_timer_deinit(timer->newTimerHandle);
  return SUCCESS;
}


uint32 ptin_mgmd_sourcetimer_timeleft(snoopPTinL3Sourcetimer_t *timer)
{
  if (FALSE == ptin_mgmd_sourcetimer_isRunning(timer))
  {
    return 0;
  }

  return ptin_mgmd_timer_timeLeft(timer->newTimerHandle)/1000;
}


BOOL ptin_mgmd_sourcetimer_isRunning(snoopPTinL3Sourcetimer_t *timer)
{
  return ptin_mgmd_timer_isRunning(timer->newTimerHandle);
}


RC_t ptin_mgmd_event_sourcetimer(snoopPTinL3Sourcetimer_t *timerData)
{
  uint32                     intIfNum;
  char                       debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN],debug_buf2[PTIN_MGMD_IPV6_DISP_ADDR_LEN];
  snoopPTinL3Interface_t     *interfacePtr;
  snoopPTinL3Source_t        *sourcePtr;
  snoopPTinL3InfoData_t*     groupData;  

  mgmdGroupRecord_t         *groupPtr=PTIN_NULLPTR;
  mgmdProxyInterface_t      *proxyInterfacePtr=PTIN_NULLPTR;
  mgmd_cb_t             *pMmgmdCB;
  BOOL                       newEntry;
  RC_t                       rc;

  if (PTIN_NULLPTR == (groupData = ptinMgmdL3EntryFind(timerData->groupKey.serviceId, &timerData->groupKey.groupAddr, AVL_EXACT)))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "We have an event Source Timer to process (serviceId:[%u] groupAddr:[0x%X]), but we were unable to find the entry in the AVL tree",timerData->groupKey.serviceId,timerData->groupKey.groupAddr.addr.ipv4.s_addr);
    return SUCCESS;
  }   
    
  //Saving Timer Variables
  intIfNum     = timerData->interfaceIdx;
  sourcePtr    = timerData->sourcePtr;
  interfacePtr = &groupData->interfaces[intIfNum];  

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Sourcetimer expired (group:%s vlan:%u ifId:%u sourceAddr:%s)", 
            ptin_mgmd_inetAddrPrint(&(groupData->snoopPTinL3InfoDataKey.groupAddr), debug_buf), groupData->snoopPTinL3InfoDataKey.serviceId, intIfNum, ptin_mgmd_inetAddrPrint(&sourcePtr->sourceAddr,debug_buf2));

  if (interfacePtr->filtermode == PTIN_MGMD_FILTERMODE_INCLUDE)
  {
    if (sourcePtr->isStatic==FALSE)
    {
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID)
      {
        if ( (proxyInterfacePtr=ptinMgmdProxyInterfaceAdd(groupData->snoopPTinL3InfoDataKey.serviceId)) ==PTIN_NULLPTR)                     
        {             
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceAdd()");
          return FAILURE;
        }            
        /*This is the Last Source, Send a ToIn{}*/
        if (interfacePtr->numberOfSources==1)
        {           
          if((groupPtr=ptinMgmdGroupRecordAdd(proxyInterfacePtr,PTIN_MGMD_CHANGE_TO_INCLUDE_MODE,&groupData->snoopPTinL3InfoDataKey.groupAddr,&newEntry ))==PTIN_NULLPTR)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
            return FAILURE;
          }
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
          if(ptinMgmdScheduleReportMessage(groupData->snoopPTinL3InfoDataKey.serviceId,&groupData->snoopPTinL3InfoDataKey.groupAddr,PTIN_IGMP_V3_MEMBERSHIP_REPORT,0,FALSE,1,groupPtr)!=SUCCESS)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
            return FAILURE;
          }         
        }
        //Send a Block{}
        else  
        {
           /* Get Mgmd Control Block */
           if (( pMmgmdCB = mgmdCBGet(AF_INET)) == PTIN_NULLPTR)
           {
             PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
             return FAILURE;
           }
          //We only send a Block Record if we are working in V3 Mode
          if(pMmgmdCB->proxyCM[groupData->snoopPTinL3InfoDataKey.serviceId].compatibilityMode==PTIN_MGMD_COMPATIBILITY_V3)
          {
            if((groupPtr=ptinMgmdGroupRecordAdd(proxyInterfacePtr,PTIN_MGMD_BLOCK_OLD_SOURCES,&groupData->snoopPTinL3InfoDataKey.groupAddr,&newEntry ))==PTIN_NULLPTR)
            {
              PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
              return FAILURE;
            }
            if (FAILURE == (rc=ptinMgmdGroupRecordSourcedAdd(groupPtr, &sourcePtr->sourceAddr)))
            {
             PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");
             return FAILURE;
            }
            else if (rc==SUCCESS)
            {
              PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
              if(ptinMgmdScheduleReportMessage(groupData->snoopPTinL3InfoDataKey.serviceId,&groupData->snoopPTinL3InfoDataKey.groupAddr,PTIN_IGMP_V3_MEMBERSHIP_REPORT,0,FALSE,1,groupPtr)!=SUCCESS)
              {
                PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
                return FAILURE;
              }         
            }
          }
        }
      }
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Removing source %s", ptin_mgmd_inetAddrPrint(&(sourcePtr->sourceAddr), debug_buf));        
      
      /* Remove source */
      ptinMgmdSourceRemove(groupData,intIfNum, sourcePtr);          
          
    }
    /* If no more sources remain, remove interface */
    if (interfacePtr->numberOfSources == 0 && interfacePtr->isStatic==FALSE)
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Interface has no more sources, thus it is being removed.");
      ptinMgmdInterfaceRemove(groupData,intIfNum);
    }
  }
  return SUCCESS;
}

