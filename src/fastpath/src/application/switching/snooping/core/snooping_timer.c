/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_timer.c
*
* @purpose    Contains definitions to start/update/stop timers required
*             by snooping component
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "buff_api.h"
#include "l3_addrdefs.h"
#include "l7utils_inet_addr_api.h"

#include "snooping_util.h"
#include "snooping_debug.h"
#include "snooping_querier.h"
#include "snooping_db.h"
#include "snooping_defs.h"

/* PTin added: IGMP */
#if 1
#include "ptin_igmp.h"
#endif

/*********************************************************************
* @purpose  Process timer event
*
* @param    pSnoopEB    @b{(input)}  Execution Block
*
* @returns  void
*
* @notes    This function is called every timer interval, which is 1 second
*
* @end
*********************************************************************/
void snoopTimerProcess(L7_uint64 timerCBHandle, snoop_eb_t *pSnoopEB)
{
  if (timerCBHandle == L7_NULL)
{
  /* Invoke all the expired timers here */
  appTimerProcess(pSnoopEB->timerCB);
}
  else
  {
    if (timerCBHandle > SNOOP_ENTRY_TIMER_BLOCKS_COUNT)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopTimerProcess: Invalid timer handle received 0X%llx", timerCBHandle);
      return;
    }
    /* Invoke all the expired timers here */
    if (pSnoopEB->snoopEntryTimerCBList[timerCBHandle - 1].pSnoopEntry != L7_NULLPTR)
    {
      appTimerProcess(pSnoopEB->snoopEntryTimerCBList[timerCBHandle - 1].timerCB);
    }
  }
}

/*************************************************************************
* @purpose  Updates the specified timer with specified timer interval
*
* @param    snoopEntry    @b{(input)}  Group Membership Entry
* @param    intIfNum      @b{(input)}  Interface for timer updation
* @param    vlanId        @b{(input)}  VLAN ID for timer updation
* @param    timerType     @b{(input)}  Interface/Timer type
* @param    timerValue    @b{(input)}  New time period for the timer
* @param    pSnoopCB      @b{(input)}  Control Block
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopTimerUpdate(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum,
                      L7_uint32 vlanId, snoop_interface_type_t timerType,
                      L7_uint32 timerValue, snoop_cb_t *pSnoopCB)
{
}
/*************************************************************************
* @purpose  Halts & destroys the specified timer with specified timer
*           and related timer nodes
*
* @param    snoopEntry    @b{(input)}  Group Membership Entry
* @param    intIfNum      @b{(input)}  Interface for timer halt
* @param    vlanId        @b{(input)}  VLAN ID for timer updation
* @param    timerType     @b{(input)}  Interface/Timer type
* @param    pSnoopCB      @b{(input)}  Control Block
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopTimerStop(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum,
                    L7_uint32 vlanId, snoop_interface_type_t timerType,
                    snoop_cb_t *pSnoopCB)
{
}

/*************************************************************************
* @purpose  Starts the specified timer and allocates related timer nodes
*
* @param    snoopEntry    @b{(input)}  Group Membership Entry
* @param    intIfNum      @b{(input)}  Interface for starting the timer
* @param    vlanId        @b{(input)}  VLAN ID for starting the timer
* @param    timerType     @b{(input)}  Interface/Timer type
* @param    pSnoopCB      @b{(input)}  Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t snoopTimerStart(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum,
                          L7_uint32 vlanId, snoop_interface_type_t timerType,
                          snoop_cb_t *pSnoopCB)

{
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Process the multicast router attached interface attached
*           timer expiry event
*
* @param    param    @b{(input)}  Pointer to added mrtr timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopMrtrExpiry(void *param)
{
  snoopMrtrTimerData_t *pTimerData;
  L7_uint64             handle = PTR_TO_UINT64(param);
  snoop_cb_t           *pSnoopCB = L7_NULLPTR;
  snoopOperData_t      *pSnoopOperEntry  = L7_NULLPTR;

  pTimerData = (snoopMrtrTimerData_t *)handleListNodeRetrieve(handle);
  if (pTimerData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoop_mrtr_expiry: Failed to retrieve handle");
    return;
  }

  /* Get Snoop Control Block */
  pSnoopCB = (snoop_cb_t *)pTimerData->cbHandle;

  /* Null out the timer Details */
  pTimerData->mrtrTimer = L7_NULL;
  handleListNodeDelete(pSnoopCB->snoopExec->handle_list, &pTimerData->mrtrTimerHandle);
  if ((pSnoopOperEntry = snoopOperEntryGet(pTimerData->snoopMrtrTimerDataKey.vlanId,
                                           pSnoopCB, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return;
  }

  SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
             "Mrtr Timer Expiry - Vlan %d intIfNum %d", \
             pTimerData->snoopMrtrTimerDataKey.vlanId, \
             pTimerData->snoopMrtrTimerDataKey.intIfNum);

  /* Remove the interface from the snoopEntry only if static mrouter is
     not enabled*/
   if (snoopMcastRtrListRemove(pTimerData->snoopMrtrTimerDataKey.vlanId,
                               pTimerData->snoopMrtrTimerDataKey.intIfNum,
                               SNOOP_DYNAMIC, pSnoopCB) != L7_SUCCESS)
   {
     L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoop_group_membership_expiry: Failed to remove interface from snoop entry");
   }
}

/*************************************************************************
* @purpose  Process the multicast group membership timer expiry event
*
* @param    param    @b{(input)}  Pointer to added group member interface
*                                 timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopGroupMembershipExpiry(void *param)
{
  snoopGrpTimerData_t *pTimerData;
  L7_uint64          handle = PTR_TO_UINT64(param);

  pTimerData = (snoopGrpTimerData_t *)handleListNodeRetrieve(handle);
  if (pTimerData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoop_group_membership_expiry: Failed to retrieve handle");
    return;
  }
}

/*************************************************************************
* @purpose  API to destroy the group membership timer data node
*
* @param    ll_member  @b{(input)}  Linked list node containing the
*                                   timer to be destroyed
*
* @returns  L7_SUCCESS
*
* @comments This is called by SLL library when a node is being deleted
*
* @end
*************************************************************************/
L7_RC_t snoopTimerDataDestroy (L7_sll_member_t *ll_member)
{
  snoopGrpTimerData_t *pTimerData;
  snoop_eb_t       *pSnoopEB;

  pSnoopEB = snoopEBGet();
  pTimerData = (snoopGrpTimerData_t *)ll_member;
  if (pTimerData->grpTimer != L7_NULL)
  {
    /* If timer is running, destroy it! */
    if (snoopTimerDestroy(pTimerData->snoopEntry->timerCB,
                          &pTimerData->grpTimer, &pTimerData->grpTimerHandle)
                          != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoop_timer_data_destroy: Failed to destroy timer");
    }
  }

  bufferPoolFree(pSnoopEB->snoopTimerBufferPoolId, (L7_uchar8 *)pTimerData);
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Helper API to compare two group membership timer nodes  and
*           return the result
*
* @param     p  @b{(input)}  Pointer to Candidate 1 for comparison
* @param     q  @b{(input)}  Pointer to Candidate 2 for comparison
*
* @returns   0   p = q
* @returns  -1   p < q
* @returns  +1   p > q
*
* @comments This is called by SLL library when a nodes are compared
*
* @end
*************************************************************************/
L7_int32 snoopTimerDataCmp(void *p, void *q, L7_uint32 key)
{
  if (((snoopGrpTimerData_t *)p)->intIfNum == ((snoopGrpTimerData_t *)q)->intIfNum)
  {
    return 0;
  }
  else if (((snoopGrpTimerData_t *)p)->intIfNum < ((snoopGrpTimerData_t *)q)->intIfNum)
  {
    return -1;
  }
  else
  {
    return 1;
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
L7_RC_t  snoopTimerDestroy(L7_APP_TMR_CTRL_BLK_t timerCB,
                           L7_APP_TMR_HNDL_t *timer, L7_uint64 *handle)
{
  snoop_eb_t  *pSnoopEB;

  pSnoopEB = snoopEBGet();
  /* Delete the apptimer node */
  (void)appTimerDelete(timerCB, *timer);
  *timer = NULL;

  /* Delete the handle we had created */
  handleListNodeDelete(pSnoopEB->handle_list, handle);
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Returns if the specified timer is active or not
*
* @param    snoopEntry    @b{(input)}  Group Membership Entry
* @param    intIfNum      @b{(input)}  Interface for timer updation
* @param    vlanId        @b{(input)}  VLAN ID for timer updation
* @param    timerType     @b{(input)}  Interface/Timer type
* @param    pSnoopCB      @b{(input)}  Control Block
*
* @returns  L7_TRUE   - Specified timer is active
*           L7_FALSE  - Specified timer is not active
*
* @comments none
*
* @end
*************************************************************************/
L7_BOOL snoop_is_timer_running(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum,
                               L7_uint32 vlanId, snoop_interface_type_t timerType,
                               snoop_cb_t *pSnoopCB)
{
   snoopGrpTimerData_t     timerData, *pTimerData;

   memset(&timerData, 0x00, sizeof(snoopGrpTimerData_t));
   timerData.intIfNum = intIfNum;
   if ((pTimerData = (snoopGrpTimerData_t *)SLLFind(&snoopEntry->ll_timerList,
                                          (void *)&timerData))
                                          == L7_NULLPTR)
   {
      return L7_FALSE;
   }

   if (pTimerData->grpTimer == L7_NULL)
   {
       return L7_FALSE;
   }

   if (pTimerData->timerType == timerType)
   {
       return L7_TRUE;
   }
   return L7_FALSE;
}
/*************************************************************************
* @purpose  Starts the specified querier timer with specified time period
*           and allocates related timer nodes
*
* @param    snoopOperEntry @b{(input)}  entry holding operational info
* @param    timerType      @b{(input)}  Querier timer type
* @param    interval       @b{(input)}  Timer period of the timer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t snoopQuerierTimerStart(snoopOperData_t *snoopOperEntry,
                               snoopQuerierTimerType_t timerType,
                               L7_uint32 interval)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopOperEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerStart: Invalid Data");
    return L7_FAILURE;
  }
  /* Get CB */
  pSnoopCB = (snoop_cb_t *)snoopOperEntry->cbHandle;

  if (timerType == SNOOP_QUERIER_QUERIER_EXPIRY_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer
                                         != L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: Timer already running");
    }

    if ((snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimerHandle
                    = handleListNodeStore(pSnoopCB->snoopExec->handle_list,
                                          (void *)snoopOperEntry)) == 0)
    {
      /* Free the previously allocated bufferpool */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: Could not get the handle node to store the expiry timer data.");
      return L7_FAILURE;
    }

    if ((snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer
          = appTimerAdd(pSnoopCB->snoopExec->timerCB, snoopQuerierExpiry,
                        UINT_TO_PTR(snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimerHandle),
                        interval,
                        "SN-QRE"))
          == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: could not start Querier Expiry timer for vlan %d family %d\n",
              snoopOperEntry->vlanId, pSnoopCB->family);
    }
    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Querier Expiry Timer Start - Vlan %d interval %d", \
                 snoopOperEntry->vlanId, interval);
  }
  else if (timerType == SNOOP_QUERIER_QUERY_INTERVAL_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer
                                         != L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: Timer already running");
    }

    if ((snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimerHandle
                    = handleListNodeStore(pSnoopCB->snoopExec->handle_list,
                                          (void *)snoopOperEntry)) == 0)
    {
      /* Free the previously allocated bufferpool */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: Could not get the handle node to store the expiry timer data.");
      return L7_FAILURE;
    }

    if ((snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer
          = appTimerAdd(pSnoopCB->snoopExec->timerCB, snoopQuerierQueryExpiry,
                        UINT_TO_PTR(snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimerHandle),
                        interval,
                        "SN-QQE")) == L7_NULL
        )
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStart: could not start Query interval for vlan %d family %d",
              snoopOperEntry->vlanId, pSnoopCB->family);
    }
    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Query Interval Timer Start - Vlan %d interval %d", \
                 snoopOperEntry->vlanId, interval);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerStart: Invalid timer");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Updates the specified querier timer with specified time pediod
*
* @param    snoopOperEntry @b{(input)}  entry holding operational info
* @param    timerType      @b{(input)}  Querier timer type
* @param    interval       @b{(input)}  Timer period of the timer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t snoopQuerierTimerUpdate(snoopOperData_t *snoopOperEntry,
                                snoopQuerierTimerType_t timerType,
                                L7_uint32 interval)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopOperEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerUpdate: Invalid Data");
    return L7_FAILURE;
  }
  /* Get CB */
  pSnoopCB = (snoop_cb_t *)snoopOperEntry->cbHandle;
  if (timerType == SNOOP_QUERIER_QUERIER_EXPIRY_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer
                                         == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerUpdate: Querier Expiry Timer not running");
    }

    if (appTimerUpdate(pSnoopCB->snoopExec->timerCB,
                       &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer,
                       (void *)snoopQuerierExpiry,
                       UINT_TO_PTR(snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimerHandle),
                       interval,
                       "snoopQuerierExpiry") != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerUpdate: Failed to update Querier Expiry timer for vlan %d family %d",
              snoopOperEntry->vlanId, pSnoopCB->family);
    }
    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Querier Expiry Timer Update - Vlan %d interval %d", \
                 snoopOperEntry->vlanId, interval);
  }
  else if (timerType == SNOOP_QUERIER_QUERY_INTERVAL_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer
                                         == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerUpdate: Query Interval Timer not running\n");
    }

    if (appTimerUpdate(pSnoopCB->snoopExec->timerCB,
                       &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer,
                      (void *)snoopQuerierQueryExpiry,
                      UINT_TO_PTR(snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimerHandle),
                      interval,
                      "snoopQuerierQueryExpiry") != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerUpdate: Failed to update Query interval for vlan %d family %d\n",
              snoopOperEntry->vlanId, pSnoopCB->family);
    }
    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Query Interval Timer Update - Vlan %d interval %d", \
                 snoopOperEntry->vlanId, interval);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerUpdate: Invalid timer");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Stop the specified querier timer
*
* @param    snoopOperEntry @b{(input)}  entry holding operational info
* @param    timerType      @b{(input)}  Querier timer type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t snoopQuerierTimerStop(snoopOperData_t *snoopOperEntry,
                              snoopQuerierTimerType_t timerType)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopOperEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerStop: Invalid Data");
    return L7_FAILURE;
  }

  /* Get CB */
  pSnoopCB = (snoop_cb_t *)snoopOperEntry->cbHandle;

  if (timerType == SNOOP_QUERIER_QUERIER_EXPIRY_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer == L7_NULL)
    {
      /* timer not running simply return */
      return L7_SUCCESS;
    }

    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Querier Expiry Timer Stop - Vlan %d", \
                 snoopOperEntry->vlanId);

    if (snoopTimerDestroy(pSnoopCB->snoopExec->timerCB,
                          &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer,
                          &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimerHandle)
                          != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStop: Failed to stop Querier Expiry timer for vlan %d family %d",
              snoopOperEntry->vlanId, pSnoopCB->family);
      return L7_FAILURE;
    }
  }
  else if (timerType == SNOOP_QUERIER_QUERY_INTERVAL_TIMER)
  {
    if (snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer == L7_NULL)
    {
      /* timer not running simply return */
      return L7_SUCCESS;
    }
    SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
                "Query Interval Timer Stop - Vlan %d", \
                 snoopOperEntry->vlanId);

    if (snoopTimerDestroy(pSnoopCB->snoopExec->timerCB,
                          &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer,
                          &snoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimerHandle)
                          != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierTimerStop: Failed to stop Query interval timer for vlan %d family %d",
              snoopOperEntry->vlanId, pSnoopCB->family);
      return L7_FAILURE;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierTimerStop: Invalid timer");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Process the querier query interval timer elapse event
*
* @param    param    @b{(input)}  Pointer to other querier query interval
*                                 timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopQuerierExpiry(void *param)
{
  snoopOperData_t *pSnoopOperEntry;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  pSnoopOperEntry = (snoopOperData_t *)handleListNodeRetrieve(PTR_TO_UINT64(param));
  if (pSnoopOperEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierExpiry: Failed to retrieve handle");
    return;
  }

  pSnoopCB = pSnoopOperEntry->cbHandle;

  /* Null out the timer Details */
  pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer
    = L7_NULL;
  /* Delete the handle */
  handleListNodeDelete(pSnoopCB->snoopExec->handle_list,
  &pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimerHandle);

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState ==
      SNOOP_QUERIER_DISABLED)
  {
    pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = 0;
  }
  /* Null out other querier information */
  inetAddressZeroSet(pSnoopCB->family,
                     &pSnoopOperEntry->snoopQuerierInfo.querierAddress);
  pSnoopOperEntry->snoopQuerierInfo.querierVersion = 0;

  SNOOP_TRACE(SNOOP_DEBUG_QUERIER, pSnoopCB->family,
              "Querier Expiry Timer expired, querier address and version zeroed, Vlan %d",
              pSnoopOperEntry->vlanId);

  SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
              "Querier Expiry Timer expired, Vlan %d", \
              pSnoopOperEntry->vlanId);
  /* The other querier expired - Snoop Querier can move to Querier Oper State
     now. It is equivalent to getting an infereior query from network */
  snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierTimerExpiry,
                             pSnoopCB);
}
/*************************************************************************
* @purpose  Process the "other querier expiry" timer elapse event
*
* @param    param    @b{(input)}  Pointer to other querier expiry timer
*                                 handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void snoopQuerierQueryExpiry(void *param)
{
  snoopOperData_t *pSnoopOperEntry;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  pSnoopOperEntry = (snoopOperData_t *)handleListNodeRetrieve(PTR_TO_UINT64(param));
  if (pSnoopOperEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierQueryExpiry: Failed to retrieve handle");
    return;
  }

  pSnoopCB = pSnoopOperEntry->cbHandle;
  /* Null out the last querier since it has expired */
  inetAddressZeroSet(pSnoopCB->family,
                     &pSnoopOperEntry->snoopQuerierInfo.querierAddress);
  pSnoopOperEntry->snoopQuerierInfo.querierVersion = 0;
  SNOOP_TRACE(SNOOP_DEBUG_QUERIER, pSnoopCB->family,
              "Query Interval Timer expired, querier address and version zeroed, Vlan %d",
              pSnoopOperEntry->vlanId);
  /* Null out the timer Details */
  pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer
    = L7_NULL;
  /* Delete the handle */
  handleListNodeDelete(pSnoopCB->snoopExec->handle_list,
  &pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimerHandle);

  SNOOP_TRACE(SNOOP_DEBUG_TIMER, pSnoopCB->family, \
              "Query Interval Timer expired, Vlan %d", \
              pSnoopOperEntry->vlanId);

  snoopQuerierSMProcessEvent(pSnoopOperEntry,
                             snoopQuerierQueryTimerExpiry,
                             pSnoopCB);
}
