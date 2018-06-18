/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
* @filename  timerange_control.c
*
* @purpose   timerange Control file
*
* @component timerange
*
* @comments  none
*
* @create    29/11/2009
*
* @author    Siva Mannem
*
* @end
*
**********************************************************************/
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "simapi.h"
#include "timerange.h"
#include "timerange_exports.h"
#include "timerange_api.h"
#include "timerange_debug.h"
#include "timerange_cnfgr.h"

extern L7_int32             timeRangeProcTaskId;
extern void                 *timeRangeProcQueue;
extern avlTree_t            *pTimeRangeTree;
extern osapiRWLock_t        timeRangeRwLock;

const L7_uchar8 *timeRangeEventNames[] = {
"",
"TIMERANGE_EVENT_START",
"TIMERANGE_EVENT_END",
"TIMERANGE_EVENT_DELETE"
};
timeRangeEventNotifyList_t  timeRangeEventNotifyList[L7_LAST_COMPONENT_ID];

static osapiTimerDescr_t    *timeRangeBaseTimer = L7_NULLPTR;
static struct tm            localTime;


/****************************************************************************
 * @purpose  Register a routine to be called for a specified time range's
 *           event
 * @param    componentID  @b{(input)} registering compoent id
 *                                      See L7_COMPONENT_ID_t for more info.
 * @param    *name        @b{(input)} pointer to the name of the registered
 *                                    function.
 *                                    up to TIMERANGE_NOTIFY_FUNC_NAME_SIZE
 *                                    characters will be stored.
 * @param    *notify      @b{(input)} pointer to a routine to be invoked for
 *                                    time range events  as listed in
 *                                    timeRangeEvent_t
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
 L7_RC_t timeRangeEventCallbackRegister(L7_COMPONENT_IDS_t  componentID,
                                                       L7_uchar8 *name,
     L7_RC_t (*notify)(L7_uchar8 *timeRangeName, timeRangeEvent_t  event))
{
  L7_RC_t rc;
  L7_char8 compname[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(componentID, compname)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (componentID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "Specified component ID (%u) is greater than"
            " L7_LAST_COMPONENT_ID", componentID);
    return(L7_FAILURE);
  }

  if ((void *)timeRangeEventNotifyList[componentID].notify != L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
    "Component with ID %u, %s is already registered\n", componentID, compname);
    return(L7_FAILURE);
  }
  osapiStrncpy(timeRangeEventNotifyList[componentID].name, name,
               sizeof(timeRangeEventNotifyList[componentID].name));
  timeRangeEventNotifyList[componentID].componentID = componentID;
  timeRangeEventNotifyList[componentID].notify = notify;

  return(L7_SUCCESS);

}

/*********************************************************************
* @purpose  Go through registered time range users and notify them of
*           time range events.
*
* @param    timeRangeName @b{(input)} time range name
* @param    event         @b{(input)} time range event
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t timeRangeEventNotify(L7_uchar8* timeRangeName, timeRangeEvent_t  event)
{
  L7_uint32 i;
  L7_char8 compName[L7_COMPONENT_NAME_MAX_LEN];

  if (timeRangeName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (event < TIMERANGE_EVENT_START || event >= TIMERANGE_EVENT_LAST)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((timeRangeEventNotifyList[i].componentID != 0) &&
         (void *)timeRangeEventNotifyList[i].notify != L7_NULLPTR)
    {
      memset(compName, 0, sizeof(compName));
      cnfgrApiComponentNameGet(i, compName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TIMERANGES_COMPONENT_ID,
              "Sending %s notification for time-range \"%s\" for %s component",
              timeRangeEventNames[event], timeRangeName, compName);
      (*timeRangeEventNotifyList[i].notify)(timeRangeName, event);
    }
  }
  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose    actions to be performed when the periodic timer expires
*
* @param    arg1    @b((input))  first argument
* @param    arg2    @b((input))  second argument
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t timeRangeTimerExpired(L7_uint32 arg1, L7_uint32 arg2)
{
  L7_RC_t        rc = L7_SUCCESS;
  timeRangeMsg_t msg;
  L7_uint32      currentTime;
  struct tm      currentLocalTime;

  osapiTimerAdd((void*)timeRangeTimerExpired, 0, 0,
                TIMERANGE_BASE_TIMEOUT, &timeRangeBaseTimer);

  currentTime = simAdjustedTimeGet();
  localtime_r((time_t *)&currentTime, &currentLocalTime);

  if ((currentLocalTime.tm_min != localTime.tm_min)   ||
     (currentLocalTime.tm_hour != localTime.tm_hour) ||
     (currentLocalTime.tm_mday != localTime.tm_mday) ||
     (currentLocalTime.tm_mon != localTime.tm_mon)   ||
     (currentLocalTime.tm_year != localTime.tm_year))
  {
    memset(&msg, 0, sizeof(timeRangeMsg_t));

    msg.msgType = TIMERANGE_TIMER_MSG;
    if (timeRangeProcQueue == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "timeRangeProcessQueue is not initialized");
      return L7_FAILURE;
    }

    rc = osapiMessageSend(timeRangeProcQueue,
                          &msg,
                          sizeof(timeRangeMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_TIMERANGES_COMPONENT_ID,
            "Failed to send message to timeRanges thread"
            "message type : %d", TIMERANGE_TIMER_MSG);
    }
  }

  localtime_r((time_t *)&currentTime, &localTime);
  return rc;
}

/*********************************************************************
* @purpose  Start the time range timer.
*
* @param    void
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
void timeRangeTimerStart(void)
{
  osapiTimerAdd((void *)timeRangeTimerExpired, L7_NULL, L7_NULL,
                TIMERANGE_BASE_TIMEOUT, &timeRangeBaseTimer);
}

/*********************************************************************
*
* @purpose  Stop the time range timer.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t timeRangeTimerStop(void)
{
  osapiTimerFree(timeRangeBaseTimer);
  timeRangeBaseTimer = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send a time Range update message.
*
* @param    timeRangeName    @b{(input)} time range name
*                             (1 to L7_TIMERANGE_NAME_LEN_MAX chars)
* @param    action            @b((input)) TIMERANGE_MODIFY or
*                                         TIMERANGE_DELETE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeUpdate(L7_uchar8* timeRangeName, timeRangeActions_t action)
{
  timeRangeMsg_t msg;
  L7_RC_t        rc;

  memset(&msg, 0, sizeof(timeRangeMsg_t));
  if (action == TIMERANGE_MODIFY)
  {
    msg.msgType         = TIMERANGE_MODIFY_MSG;
  }
  else if (TIMERANGE_DELETE == action)
  {
    msg.msgType         = TIMERANGE_DELETE_MSG;
  }

  osapiStrncpySafe((char *)msg.u.timeRangeName, (char *)timeRangeName,
                                                sizeof(msg.u.timeRangeName));
  if (timeRangeProcQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
          "timeRangeProcessQueue is not initialized");
    return L7_FAILURE;
  }

  rc = osapiMessageSend(timeRangeProcQueue,
                        &msg,
                        sizeof(timeRangeMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_TIMERANGES_COMPONENT_ID,
          "Failed to send message to timeRanges thread"
          "message type : %d", TIMERANGE_MODIFY_MSG);
  }

  return rc;
}


/*********************************************************************
*
* @purpose Process time range Timer Event
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void timeRangeProcessTimerEvent()
{
  avlTree_t              *pTree = pTimeRangeTree;
  L7_uint32              timeRangeIndex = 0;
  timeRangeStructure_t   *pTimeRange;
  timeRangeStatus_t      status;

  (void)TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock);

  while ((pTimeRange = avlSearchLVL7(pTree, &timeRangeIndex, AVL_NEXT)) != L7_NULLPTR)
  {
    if (timeRangeImpStatusGet(pTimeRange, &status) == L7_SUCCESS)
    {
      if (status != pTimeRange->status)
      {
        TIMERANGE_TRACE(TIMERANGE_DEBUG_STATE_CHANGE,
                      "timeRangeIndex %d \"%s\"  \"%s\" \n",
                      pTimeRange->timeRangeIndex,
                      pTimeRange->timeRangeName,
                      status == TIMERANGE_STATUS_ACTIVE ? "Active": "Inactive");
        if (status == TIMERANGE_STATUS_ACTIVE)
        {
          timeRangeEventNotify(pTimeRange->timeRangeName, TIMERANGE_EVENT_START);
        }
        else if (status == TIMERANGE_STATUS_INACTIVE)
        {
          timeRangeEventNotify(pTimeRange->timeRangeName, TIMERANGE_EVENT_END);
        }
        pTimeRange->status = status;
      }
    }
    timeRangeIndex = pTimeRange->timeRangeIndex;
  }

  (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
}
/*********************************************************************
*
* @purpose Process time range modify message
*
* @param    timeRangeName    @b{(input)} time range name
*                             (1 to L7_TIMERANGE_NAME_LEN_MAX chars)
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void timeRangeProcessModify(L7_uchar8* timeRangeName)
{
  timeRangeStatus_t      status;
  timeRangeStructure_t   *pTimeRange;
  L7_uint32              timeRangeIndex;

  if (timeRangeNameToIndex(timeRangeName, &timeRangeIndex) == L7_SUCCESS)
  {
    pTimeRange = (timeRangeStructure_t *) timeRangeFindTimeRange(timeRangeIndex);
    if (pTimeRange != L7_NULLPTR)
    {
     (void)TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock);

      if (timeRangeImpStatusGet(pTimeRange, &status) == L7_SUCCESS)
      {
        if (status != pTimeRange->status)
        {
          TIMERANGE_TRACE(TIMERANGE_DEBUG_STATE_CHANGE,
                        "timeRangeIndex %d \" %s \"  \" %s \" \n",
                        pTimeRange->timeRangeIndex,
                        pTimeRange->timeRangeName,
                        status == TIMERANGE_STATUS_ACTIVE ? "Active": "Inactive");

          if (status == TIMERANGE_STATUS_ACTIVE)
          {
            timeRangeEventNotify(pTimeRange->timeRangeName, TIMERANGE_EVENT_START);
          }
          else if (status == TIMERANGE_STATUS_INACTIVE)
          {
            timeRangeEventNotify(pTimeRange->timeRangeName, TIMERANGE_EVENT_END);
          }
          pTimeRange->status = status;
        }
      }
      (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    }
  }
}

/*********************************************************************
*
* @purpose task to handle all Time Range messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void timeRangeProcTask()
{
  timeRangeMsg_t msg;

  osapiTaskInitDone(L7_TIMERANGE_TASK_SYNC);
  do
  {
    memset(&msg, 0x00, TIMERANGE_MSG_SIZE);
    if (L7_SUCCESS == osapiMessageReceive(timeRangeProcQueue,
                                          (void *)&msg,
                                          TIMERANGE_MSG_SIZE,
                                          L7_WAIT_FOREVER))
    {
      switch (msg.msgType)
      {
        case (TIMERANGE_TIMER_MSG):
          TIMERANGE_TRACE(TIMERANGE_DEBUG_TIMER,
                          "Received TIMERANGE_TIMER_MSG message");
          timeRangeProcessTimerEvent();
          break;

        case (TIMERANGE_MODIFY_MSG):
          TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                          "Received TIMERANGE_MODIFY_MSG message for timerange %s",
                         msg.u.timeRangeName);
          timeRangeProcessModify(msg.u.timeRangeName);
          break;

        case (TIMERANGE_DELETE_MSG):
          TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                          "Received TIMERANGE_DELETE_MSG message for timerange %s \n"
                          "Sending TIMERANGE_EVENT_DELETE event to all"
                          " registered components...",
                          msg.u.timeRangeName);
          timeRangeEventNotify(msg.u.timeRangeName, TIMERANGE_EVENT_DELETE);
          break;

        case (TIMERANGE_CNFGR_MSG):
          TIMERANGE_TRACE(TIMERANGE_DEBUG_CNFGR,
                                "Received Configuratior message");
          timeRangeCnfgrParse(&msg.u.CmdData);
          break;
        default:
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TIMERANGES_COMPONENT_ID,
                   "timeRangeProcTask(): invalid message received on"
                   "time range processing queue: MsgType: %d", msg.msgType);
        break;
      }
    }
  } while (1);
}

/*********************************************************************
* @purpose  Start Time Range Processing task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t timeRangeProcTaskStart()
{

  timeRangeProcTaskId = osapiTaskCreate(TIMERANGE_PROC_TASK,
                               timeRangeProcTask, 0, 0,
                               FD_CNFGR_TIMERANGE_DEFAULT_STACK_SIZE,
                               FD_CNFGR_TIMERANGE_DEFAULT_TASK_PRIORITY,
                               FD_CNFGR_TIMERANGE_DEFAULT_TASK_SLICE);

  if (timeRangeProcTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR,
           L7_TIMERANGES_COMPONENT_ID,
           "Failed to create Time Range Processing Task");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_TIMERANGE_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR,
           L7_TIMERANGES_COMPONENT_ID,
           "Failed to initialize Time Range Processing Task");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

