/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    Periodic timer service.c
* @purpose     maintain simple periodic timers for interested components
* @component   SIM
* @comments    This service will maintain per interface timer so the
*              components do not have to create their own task. If the component
*              requires more than one timer, it will need to have its own
*              timer thread
* @create      10/24/08
* @author      akulkarn
* @end
*
**********************************************************************/
#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "sim_pts_api.h"

void *ptsSema;
static L7_uint32 periodicTimerServiceHandle = 0;
regComps_t *timersList = L7_NULLPTR;
REG_USERS_t users[PTS_MAX_USERS];
L7_uint32 debugTimerHelp = 0;
static L7_uint32 sim_pts_task_id = L7_ERROR;

L7_RC_t simPts_task();

void debugTimerHelpEnable(L7_uint32 flag)
{
  debugTimerHelp = flag;
}
L7_RC_t simPtsInit()
{
  ptsSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptsSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unable to create helperTimer semaphore()\n");
    return(L7_FAILURE);
  }

  timersList = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(regComps_t)*(PTS_MAX_PORTS));
  if (timersList == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_SIM_COMPONENT_ID, "Cannot allocate memory for helper timer list");
    return L7_FAILURE;
  }

  memset(timersList, 0, sizeof(regComps_t)*(PTS_MAX_PORTS));

  sim_pts_task_id = (L7_uint32)osapiTaskCreate( "simPts_task", (void *)simPts_task, 0, 0,
                                                L7_DEFAULT_STACK_SIZE,
                                                L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                                L7_DEFAULT_TASK_SLICE);



  if (osapiWaitForTaskInit (L7_SIM_PTS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_SIM_COMPONENT_ID, "Unable to initialize simPts_task()\n");
    return(L7_FAILURE);
  }

  if (sim_pts_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_SIM_COMPONENT_ID, "Failed to create PTS tasks.\n");
    return(L7_FAILURE);
  }

  return L7_SUCCESS;
}

void simPtsFini()
{
  if (sim_pts_task_id != L7_ERROR)
    osapiTaskDelete(sim_pts_task_id);
}


/* Register with the helper service to receive periodic notifications after interval*/
L7_RC_t simPtsRegister(L7_COMPONENT_IDS_t comp, L7_uint32 prio,
                       SIM_PTS_CB_FUNC_t callbackFunc,
                       L7_uint32 *userHandle)
{
  L7_uint32 i;

  if ((comp < L7_FIRST_COMPONENT_ID) || (comp > L7_LAST_COMPONENT_ID))
  {
    return L7_FAILURE;
  }

  /* Add the component ID to the list of users and store the interval*/
  for (i =0; i < PTS_MAX_USERS ; i++)
  {
    if (users[i].valid == L7_TRUE )
    {
      continue;
    }
    users[i].valid = L7_TRUE;
    users[i].compId = comp;
    users[i].priority = prio;
    users[i].ptsTimerCallback = callbackFunc;
    break;

  }

  if (i == PTS_MAX_USERS )
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Component %d cannot be registered for helper timer", comp);
    return L7_TABLE_IS_FULL;
  }

  *userHandle = i;
  return L7_SUCCESS;
}

static L7_RC_t ptsTimerRecordGet(L7_uint32 userHandle, L7_uint32 portIndex, timer_block_t **timerRecord)
{

  if (userHandle >= PTS_MAX_USERS )
  {
    return L7_FAILURE;
  }

  /* we can may be relax the rule to not maintain the record for index 0.
     This will allow any other user not based on port to use pts.
     Keep it for now.
  */
  if ((portIndex < 1) ||
      (portIndex > PTS_MAX_PORTS ))
  {
    return L7_FAILURE;
  }

  *timerRecord = &timersList[portIndex][userHandle];
  return L7_SUCCESS;

}

/* Periodically Notify the component when the timer expires*/
L7_RC_t simPtsTimerStart(L7_uint32 userHandle, L7_uint32 portIndex,
                         L7_uint32 interval)
{
  timer_block_t *timerRecord = L7_NULL;
  L7_RC_t rc;

  SIM_PTS_PRINT("%s user handle %d  port index (%d) interval %d \n",
                __FUNCTION__, userHandle, portIndex, interval);

  if (interval == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
            "Comp %d can not create timer on port index (%d) "
            "with zero seconds as timeout  \n",
            users[userHandle].compId, portIndex);
    return L7_FAILURE;
  }
  osapiSemaTake(ptsSema, L7_WAIT_FOREVER);
  rc = ptsTimerRecordGet(userHandle, portIndex, &timerRecord);

  if (rc == L7_SUCCESS)
  {
    timerRecord->valid = L7_TRUE;
    timerRecord->origTimeout = interval;
    timerRecord->currTime = interval;
    SIM_PTS_PRINT("%s starting timer : user handle %d  port index "
                  "(%d) interval %d \n", __FUNCTION__,
                  userHandle, portIndex, interval);
  }

  osapiSemaGive(ptsSema);

  return rc;
}

/* stop the timer for this interface for this port*/
L7_RC_t simPtsTimerStop(L7_uint32 userHandle, L7_uint32 portIndex)
{
  timer_block_t *timerRecord = L7_NULL;
  L7_RC_t rc;

  SIM_PTS_PRINT("%s user handle %d  port index (%d)\n",
                __FUNCTION__, userHandle, portIndex);
  osapiSemaTake(ptsSema, L7_WAIT_FOREVER);
  rc = ptsTimerRecordGet(userHandle, portIndex, &timerRecord);

  if (rc == L7_SUCCESS)
  {
    timerRecord->valid = L7_FALSE;
  }
  osapiSemaGive(ptsSema);
  return L7_SUCCESS;
}

L7_RC_t simPtsTimerModify(L7_uint32 userHandle, L7_uint32 portIndex, L7_uint32 interval)
{
  timer_block_t *timerRecord = L7_NULL;
  L7_RC_t rc;

  SIM_PTS_PRINT("%s user handle %d  port index (%d) interval %d \n",
                __FUNCTION__, userHandle, portIndex, interval);
  osapiSemaTake(ptsSema, L7_WAIT_FOREVER);
  rc = ptsTimerRecordGet(userHandle, portIndex, &timerRecord);

  if (rc == L7_SUCCESS)
  {
    timerRecord->origTimeout = interval;
    timerRecord->currTime = interval;
  }
  osapiSemaGive(ptsSema);
  return L7_SUCCESS;
}

/* Remove static for debug */
L7_RC_t simPtsAction()
{
  L7_uint32 portIndex;
  L7_uint32 user;
  timer_block_t *timerRecord = L7_NULL;

  osapiSemaTake(ptsSema, L7_WAIT_FOREVER);

  for (portIndex = 1; portIndex <= PTS_MAX_PORTS; portIndex++)
  {
    for (user = 0; user < PTS_MAX_USERS; user++)
    {
      timerRecord = &timersList[portIndex][user];
      if (timerRecord->valid == L7_TRUE)
      {
        if (timerRecord->currTime <= PTS_TIMER_TICK)
        {
          users[user].ptsTimerCallback(portIndex);
          timerRecord->currTime = timerRecord->origTimeout;
        }
        else
        {
          timerRecord->currTime -= PTS_TIMER_TICK;
        }

      }
    }
  }
  osapiSemaGive(ptsSema);

  return L7_SUCCESS;

}

L7_RC_t simPts_task()
{
  L7_RC_t rc;

  rc = osapiTaskInitDone(L7_SIM_PTS_TASK_SYNC);

  rc = osapiPeriodicUserTimerRegister(PTS_TIMER_TICK, &periodicTimerServiceHandle);

  for (;;)
  {
    osapiPeriodicUserTimerWait(periodicTimerServiceHandle);
    /* interval get*/
    /* for each user registered*/
    /* walk thru the list of interfaces for which timer is active */
    /* decrement the count*/
    /* if count is zero or less than zero call the expiry function with the interface information

    */
    /*put a msg in the timer queue*/

    rc = simPtsAction();
  }
  return L7_SUCCESS;

}

void simPtsDebugUser(L7_uint32 userId)
{
  L7_uint32 portIndex;
  timer_block_t *timerRecord = L7_NULL;

  if (userId >= PTS_MAX_USERS)
  {
    return;
  }

  for (portIndex = 1; portIndex <= PTS_MAX_PORTS; portIndex++)
  {
    timerRecord = &timersList[portIndex][userId];
    if (timerRecord->valid == L7_TRUE)
    {
      printf("port %d Periodic timer service running\n", portIndex);
    }

  }


}
