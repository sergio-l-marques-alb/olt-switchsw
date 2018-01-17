/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007-2006
*
**********************************************************************
* @filename  sFlow_timer.c
*
* @purpose    Contains definitions to APIs used to interract with
*             timers
*
* @component sFlow
*
* @comments
*
* @create    29-Nov-2007
*
* @author    drajendra
*
* @end
*
**********************************************************************/
#include "datatypes.h"

#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_debug.h"
extern SFLOW_agent_t agent;
static L7_RC_t  sFlowTimerDestroy(L7_APP_TMR_HNDL_t *timer, L7_uint32 *handle);
/*************************************************************************
* @purpose  Starts the specified poll timer with specified time period
*           and allocates related timer nodes
*
* @param    pPoller        @b{(input)}  reference to poller object
* @param    interval       @b{(input)}  Timer period of the timer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t sFlowPollerTimerStart(SFLOW_poller_t *pPoller, L7_uint32 interval)
{
  if (pPoller->timerData.ctrPollTimer != L7_NULL)
  {
    if (appTimerUpdate(agent.timerCB, &pPoller->timerData.ctrPollTimer,
                       (void *)sFlowPollerTimerExpiry,
                       (void *)pPoller->timerData.ctrPollTimerHandle, interval,
                        "sFlowPollerTimerExpiry")
                       != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
              "sFlowPollerTimerStart: Timer could not be updated for dsIndex %u instance %u interval %u\n",
             pPoller->key.dsIndex, pPoller->key.sFlowInstance, interval);
    }
    return L7_SUCCESS;
  }
  if ((pPoller->timerData.ctrPollTimerHandle
                  = handleListNodeStore(agent.handle_list, (void *)pPoller)) == 0)
  {
    /* Free the previously allocated bufferpool */
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SFLOW_COMPONENT_ID,
         "sFlowPollerTimerStart: Could not get the handle node to store the expiry timer data."
         " Timer list is full and no more new timer creations are possible.");
    return L7_FAILURE;
  }

  if ((pPoller->timerData.ctrPollTimer
        = appTimerAdd(agent.timerCB, sFlowPollerTimerExpiry,
                      (void *)pPoller->timerData.ctrPollTimerHandle,
                       interval, "sFlowPollerTimerExpiry"))
        == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowPollerTimerStart: Could not start poller timer for dsIndex %u instance %u\n",
             pPoller->key.dsIndex, pPoller->key.sFlowInstance);
    return L7_FAILURE;
  }
  SFLOW_TRACE(SFLOW_DEBUG_TIMER, "Poller dsIndex %u instance %u interval %u timer started!",
              pPoller->key.dsIndex, pPoller->key.sFlowInstance, interval);
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  stops the specified poll timer with specified time period
*           andd deletes related timer nodes
*
* @param    pPoller        @b{(input)}  reference to poller object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t sFlowPollerTimerStop(SFLOW_poller_t *pPoller)
{
  if (pPoller->timerData.ctrPollTimer == L7_NULL)
  {
    /* If timer is not running return success */
    return L7_SUCCESS;
  }
  if (sFlowTimerDestroy(&pPoller->timerData.ctrPollTimer,
                        &pPoller->timerData.ctrPollTimerHandle)
                        != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowPollerTimerStop: Failed to stop poller timer for dsIndex %u instance %u\n",
             pPoller->key.dsIndex, pPoller->key.sFlowInstance);
    return L7_FAILURE;
  }
  SFLOW_TRACE(SFLOW_DEBUG_TIMER, "Poller dsIndex %u instance %u timer stopped!",
              pPoller->key.dsIndex, pPoller->key.sFlowInstance);
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Process the poller timer expiry event
*
* @param    param    @b{(input)}  Pointer to poller timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void sFlowPollerTimerExpiry(void *param)
{
  SFLOW_poller_t *pPoller;

  pPoller = (SFLOW_poller_t *)handleListNodeRetrieve((L7_uint32)param);
  if (pPoller == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowPollerTimerExpiry: Failed to retrieve handle \n");
    return;
  }

  SFLOW_TRACE(SFLOW_DEBUG_TIMER, "Poller dsIndex %u instance %u timer expired!",
              pPoller->key.dsIndex, pPoller->key.sFlowInstance);

  /* Null out the timer Details */
  pPoller->timerData.ctrPollTimer = L7_NULL;
  /* Delete the handle */
  handleListNodeDelete(agent.handle_list, &pPoller->timerData.ctrPollTimerHandle);

  /* collect counters for this poller object */
  sFlowCounterProcess(pPoller);

  /* Renable the poller object */
  if (sFlowPollerOperModeSet(pPoller, L7_ENABLE) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
            "sFlowPollerTimerExpiry: Failed to restart poller timer for dsIndex %u\n",
            pPoller->key.dsIndex);
  }
}
/*************************************************************************
* @purpose  Helper API to delete the specifed timer node, timer handle
*
* @param    timer  @b{(input)}  Pointer to appTimer node
* @param    handle @b{(input)}  Pointer to appTimer handle handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
static L7_RC_t  sFlowTimerDestroy(L7_APP_TMR_HNDL_t *timer, L7_uint32 *handle)
{
  /* Delete the apptimer node */
  (void)appTimerDelete(agent.timerCB, *timer);
  *timer = NULL;

  /* Delete the handle we had created */
  handleListNodeDelete(agent.handle_list, handle);
  return L7_SUCCESS;
}

