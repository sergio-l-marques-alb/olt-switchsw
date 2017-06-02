/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* Name: cnfgr_hw_tally.c
*
* Purpose: Tally responses from components indicating that the components
*          have set their hardware state and are ready for USL to
*          reconcile the hardware after a warm restart.
*
* Component: Configurator (cnfgr)
*
* Comments:  Each component indicates in cnfgrComponentList[] initialization
*            the hardware reconciliation phases it intends to participate in.
*            When the component has completed its hardware updates for a given
*            phase, it calls cnfgrApiComponentHwUpdateDone() to say it's done.
*            When all components in a given phase are done, Configurator tells
*            USL that is can reconcile the hardware tables included in that
*            phase.
*
* Created by: rrice 11/4/2008
*
*********************************************************************/


#include "cnfgr_include.h"
#include "log.h"
#include "component_mask.h"
#include "osapi.h"
#include "osapi_trace.h"
#include  "cardmgr_api.h"

typedef struct hwUpdatePhaseTally_s
{
  /* Bit set for each component expected to respond for each hw update phase. */
  COMPONENT_MASK_t pendingMask;

  /* L7_TRUE if all components in a given phase have already reported completion. */
  L7_BOOL hwUpdatesDone;

  /* Timer for each phase. If any component in a given phase does not respond
   * before the timer expires, Configurator initiates the hardware reconcilation
   * without further delay. */
  osapiTimerDescr_t *phaseTimer;

} hwUpdatePhaseTally_t;

static hwUpdatePhaseTally_t hwUpdateTally[L7_CNFGR_HW_APPLY_NUM_PHASES];

/* Semaphore synchronizing access to pending masks */
static void *pendingMaskLock = L7_NULLPTR;

/* How long to wait, in seconds, for all components to report for each phase */
static const L7_uint32 L2_HW_UPDATE_TIMEOUT = 120;
static const L7_uint32 L3_HW_UPDATE_TIMEOUT = 120;
static const L7_uint32 MCAST_HW_UPDATE_TIMEOUT = 180;    

static L7_uchar8 *phaseNames[L7_CNFGR_HW_APPLY_NUM_PHASES] =
{
  "L2", "L3", "IP MCAST"
};

static void cnfgrHwTallyTimerExpired(L7_uint32 phase, L7_uint32 unused);
static L7_uint32 cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_t phase);
static void cnfgrHwUpdatePendingSet(L7_COMPONENT_IDS_t cid,
                                    L7_CNFGR_HW_APPLY_t phase);


/*********************************************************************
* @purpose  Given a hardware update phase value, convert to an index
*           into the tally array.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
*
* @end
*********************************************************************/
static L7_uint32 cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_t phase)
{
  L7_uint32 i = 0;

  phase = phase >> 1;
  while (phase)
  {
    i++;
    phase = phase >> 1;
  }
  return i;
}

/*********************************************************************
* @purpose  Set a component's bit in the pending mask for a given phase.
*
* @param    cid    @b{(input)}  component ID
* @param    phase  @b{(input)}  hardware reconciliation phase
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
static void cnfgrHwUpdatePendingSet(L7_COMPONENT_IDS_t cid,
                                    L7_CNFGR_HW_APPLY_t phase)
{
  L7_uint32 i = cnfgrHwUpdatePhaseToIndex(phase);
  COMPONENT_SETMASKBIT(hwUpdateTally[i].pendingMask, cid);
}

/*********************************************************************
* @purpose  Initialize the pending mask for each hw reconciliation phase.
*           This has to be done whenever the system goes through p3 init.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwUpdatePendingMasksInit(void)
{
  L7_COMPONENT_IDS_t cid;
  L7_RC_t rc;
  CNFGR_CR_HANDLE_t  crHandle;
  CNFGR_CR_COMPONENT_t  *pComponent;
  L7_uint32 i;
  L7_uint32 p = 1;

  if (osapiSemaTake(pendingMaskLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CNFGR_COMPONENT_ID,
           "Failed to take pending mask lock");
    return L7_FAILURE;
  }

  /* If any timers are still set, cancel them */
  for (i = 0; i < L7_CNFGR_HW_APPLY_NUM_PHASES; i++)
  {
    if (hwUpdateTally[i].phaseTimer)
    {
      osapiTimerFree(hwUpdateTally[i].phaseTimer);
      hwUpdateTally[i].phaseTimer = NULL;
    }
  }

  memset((void *)&hwUpdateTally, 0, sizeof(hwUpdateTally));

  for (cid = L7_FIRST_COMPONENT_ID + 1; cid < L7_LAST_COMPONENT_ID; cid++)
  {
    rc = cnfgrCrComponentTake(cid, &crHandle, &pComponent);
    if (rc == L7_SUCCESS)
    {
      if ((pComponent != L7_NULLPTR) && (crHandle != L7_NULLPTR))
      {
        if (pComponent->present == L7_TRUE)
        {
          p = 1;
          for (i = 0; i < L7_CNFGR_HW_APPLY_NUM_PHASES; i++)
          {
            if (pComponent->hwApplyPhases & p)
            {
              cnfgrHwUpdatePendingSet(cid, (L7_CNFGR_HW_APPLY_t) p);
            }
            p = p << 1;
          }
        }
        (void)cnfgrCrComponentGive(crHandle);
      }
    }
  }

  /* Set timers in case someone doesn't report as expected */
  i = cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_CONFIG);
  osapiTimerAdd(cnfgrHwTallyTimerExpired, L7_CNFGR_HW_APPLY_CONFIG, 0, L2_HW_UPDATE_TIMEOUT * 1000,
                &hwUpdateTally[i].phaseTimer);

  i = cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_L3);
  osapiTimerAdd(cnfgrHwTallyTimerExpired, L7_CNFGR_HW_APPLY_L3, 0, L3_HW_UPDATE_TIMEOUT * 1000,
                &hwUpdateTally[i].phaseTimer);

  i = cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_IPMCAST);
  osapiTimerAdd(cnfgrHwTallyTimerExpired, L7_CNFGR_HW_APPLY_IPMCAST, 0, MCAST_HW_UPDATE_TIMEOUT * 1000,
                &hwUpdateTally[i].phaseTimer);

  osapiSemaGive(pendingMaskLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Allocate resources needed to tally hardware update complete reports.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    done once at init time.
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwUpdateTallyInitialize(void)
{
  memset((void *)&hwUpdateTally, 0, sizeof(hwUpdateTally));

  pendingMaskLock = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (pendingMaskLock == NULL)
  {
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Free resources.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwTallyFini(void)
{
  osapiSemaDelete(pendingMaskLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Take actions required when a hw update phase completes.
*
* @param    phase  @b{(input)}  hardware reconciliation phase
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwTallyDone(L7_CNFGR_HW_APPLY_t phase)
{
  L7_uint32 i = cnfgrHwUpdatePhaseToIndex(phase);
  L7_RC_t   rc;

  hwUpdateTally[i].hwUpdatesDone = L7_TRUE;

 /* Notify CM about hwApply event which in turn will notify the driver. */
  rc = cmgrHwApplyNotify(phase);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
            "%s hardware notification to CM failed.", phaseNames[i]);
  }

  /* Cancel timer */
  osapiTimerFree(hwUpdateTally[i].phaseTimer);
  hwUpdateTally[i].phaseTimer = NULL;

  if (phase == L7_CNFGR_HW_APPLY_CONFIG)
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_HW_UPDATE_GENERIC_DONE,
                      (L7_uchar8 *)L7_NULLPTR, 0 );
  else if (phase == L7_CNFGR_HW_APPLY_L3)
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_HW_UPDATE_L3_DONE,
                      (L7_uchar8 *)L7_NULLPTR, 0 );
  else if (phase == L7_CNFGR_HW_APPLY_IPMCAST)
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_HW_UPDATE_IPMCAST_DONE,
                      (L7_uchar8 *)L7_NULLPTR, 0 );

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API that components call to indicate they have finished their
*           hardware updates following a system restart.
*
* @param    cid    @b{(input)}  caller's component ID
* @param    phase  @b{(input)}  hardware reconciliation phase
*
* @returns  L7_SUCCESS
*           L7_FAILURE if cid or phase are invalid or if component reports
*                      unexpectedly
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentHwUpdateDone(L7_COMPONENT_IDS_t cid,
                                      L7_CNFGR_HW_APPLY_t phase)
{
  L7_char8 compName[L7_COMPONENT_NAME_MAX_LEN];
  L7_uint32 keepWaiting;
  L7_uint32 i;

  if ((cid <= L7_FIRST_COMPONENT_ID) || (cid >= L7_LAST_COMPONENT_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CNFGR_COMPONENT_ID,
            "Invalid component %u reports hardware update complete for phase %u",
            cid, phase);
    return L7_FAILURE;
  }

  if (cnfgrApiComponentNameGet(cid, compName) != L7_SUCCESS)
  {
    osapiStrncpySafe (compName, "Unknown", sizeof (compName));
  };

  if ((phase != L7_CNFGR_HW_APPLY_CONFIG) &&
      (phase != L7_CNFGR_HW_APPLY_L3) &&
      (phase != L7_CNFGR_HW_APPLY_IPMCAST))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, cid,
            "%s reported hardware update complete for invalid phase %u",
            compName, phase);
    return L7_FAILURE;
  }

  i = cnfgrHwUpdatePhaseToIndex(phase);

  if (osapiSemaTake(pendingMaskLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CNFGR_COMPONENT_ID,
           "Failed to take pending mask lock");
    return L7_FAILURE;
  }

  if (!COMPONENT_ISMASKBITSET(hwUpdateTally[i].pendingMask, cid))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, cid,
            "%s reported unexpectedly for %s hardware reconciliation",
            compName, phaseNames[i]);
    osapiSemaGive(pendingMaskLock);
    return L7_FAILURE;
  }

  COMPONENT_CLRMASKBIT(hwUpdateTally[i].pendingMask, cid);
  COMPONENT_NONZEROMASK(hwUpdateTally[i].pendingMask, keepWaiting);
  if (!keepWaiting)
  {
    cnfgrHwTallyDone(phase);

    /* A build may not have L3 or IP mcast. In such a build, no component
     * will report to trigger hw reconciliation for those phases. I assume
     * there will always be components reporting for the L2 phase. So
     * check if the L3 or mcast masks are 0 here, and if so, report to USL. */
    if (phase == L7_CNFGR_HW_APPLY_CONFIG)
    {
      L7_uint32 p;

      p = cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_L3);
      if (!hwUpdateTally[p].hwUpdatesDone)
      {
        COMPONENT_NONZEROMASK(hwUpdateTally[p].pendingMask, keepWaiting);
        if (!keepWaiting)
        {
          cnfgrHwTallyDone(L7_CNFGR_HW_APPLY_L3);
        }
      }

      p = cnfgrHwUpdatePhaseToIndex(L7_CNFGR_HW_APPLY_IPMCAST);
      if (!hwUpdateTally[p].hwUpdatesDone)
      {
        COMPONENT_NONZEROMASK(hwUpdateTally[p].pendingMask, keepWaiting);
        if (!keepWaiting)
        {
          cnfgrHwTallyDone(L7_CNFGR_HW_APPLY_IPMCAST);
        }
      }
    }
  }

  osapiSemaGive(pendingMaskLock);
  return L7_SUCCESS;
}

static void cnfgrHwTallyTimerExpired(L7_uint32 phase, L7_uint32 unused)
{
  L7_uint32 i = cnfgrHwUpdatePhaseToIndex(phase);
  L7_uchar8 compStr[512];
  L7_COMPONENT_IDS_t cid;
  L7_char8 compName[L7_COMPONENT_NAME_MAX_LEN];
  L7_BOOL firstName = L7_TRUE;
  L7_RC_t rc;

  compStr[0] = '\0';

  if (osapiSemaTake(pendingMaskLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CNFGR_COMPONENT_ID,
           "Failed to take pending mask lock");
    return;
  }

  if (!hwUpdateTally[i].hwUpdatesDone)
  {
    for (cid = L7_FIRST_COMPONENT_ID + 1; cid < L7_LAST_COMPONENT_ID; cid++)
    {
      if (COMPONENT_ISMASKBITSET(hwUpdateTally[i].pendingMask, cid))
      {
        if (cnfgrApiComponentNameGet(cid, compName) != L7_SUCCESS)
        {
          osapiStrncpySafe (compName, "Unknown", sizeof (compName));
        };

        if (!firstName)
        {
          osapiStrncat(compStr, ", ", 3);
        }
        firstName = L7_FALSE;
        osapiStrncat(compStr, compName, L7_COMPONENT_NAME_MAX_LEN);
      }
    }

    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_CNFGR_COMPONENT_ID,
            "%s hardware update tally timed out. Proceeding with hardware reconciliation.",
            phaseNames[i]);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_CNFGR_COMPONENT_ID,
            "The following components did not report: %s", compStr);

    hwUpdateTally[i].hwUpdatesDone = L7_TRUE;
    memset(&hwUpdateTally[i].pendingMask, 0, sizeof(COMPONENT_MASK_t));

    /* Notify CM about hwApply event which in turn will notify the driver. */
    rc = cmgrHwApplyNotify(phase);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
              "%s hardware notification to CM failed.", phaseNames[i]);
    }

  }
  hwUpdateTally[i].phaseTimer = NULL;

  osapiSemaGive(pendingMaskLock);
}


/*********************************************************************
* @purpose  Get the Cnfgr Hw Tally timeout value for the specified phase
*
*
* @param    phase    {(input)}   Hw Tally phase
* @param    timeout  {(output)}  Timeout val (secs)
*
* @returns  L7_RC_t
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwTallyTimeoutGet(L7_CNFGR_HW_APPLY_t phase, L7_uint32 *timeout)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (phase)
  {
    case L7_CNFGR_HW_APPLY_CONFIG:
      *timeout = L2_HW_UPDATE_TIMEOUT;
      break;

    case L7_CNFGR_HW_APPLY_L3:
      *timeout = L3_HW_UPDATE_TIMEOUT;
      break;

    case L7_CNFGR_HW_APPLY_IPMCAST:
      *timeout = MCAST_HW_UPDATE_TIMEOUT;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Print a list of components whose hw updates are not done
*           for each reconciliation phase.
*
* @param    void
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void cnfgrHwUpdateTallyShow(void)
{
  L7_COMPONENT_IDS_t cid;
  L7_char8 compName[L7_COMPONENT_NAME_MAX_LEN];
  L7_uint32 i;

  if (osapiSemaTake(pendingMaskLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    printf("Failed to take pending mask lock");
    return;
  }

  for (i = 0; i < L7_CNFGR_HW_APPLY_NUM_PHASES; i++)
  {
    if (hwUpdateTally[i].hwUpdatesDone)
    {
      printf("\n%s hardware reconciliation initiated", phaseNames[i]);
    }
    else
    {
      printf("\nWaiting on the following components for %s hardware reconciliation:",
             phaseNames[i]);

      for (cid = L7_FIRST_COMPONENT_ID + 1; cid < L7_LAST_COMPONENT_ID; cid++)
      {
        if (COMPONENT_ISMASKBITSET(hwUpdateTally[i].pendingMask, cid))
        {
          if (cnfgrApiComponentNameGet(cid, compName) != L7_SUCCESS)
          {
            osapiStrncpySafe (compName, "Unknown", sizeof (compName));
          };
          printf("\n%s (%u)", compName, cid);
        }
      }
    }
  }

  osapiSemaGive(pendingMaskLock);
}


