/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q.c
*
* @purpose dot1q main file
*
* @component dot1q
*
* @comments none
*
* @create 08/14/2000
*
* @author djohnson
*
* @end
*
**********************************************************************/

#define DOT1Q_STATS

#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "l7_product.h"
#include "nimapi.h"
#include "dot3ad_api.h"
#include "log.h"
#include "statsapi.h"
#include "dtlapi.h"
#include "dot1q_intf.h"
#include "l7_cnfgr_api.h"
#include "dot1q_data.h"
#include "dot1q_control.h"
#include "dot1q_cnfgr.h"
#include "dot1q_cfg.h"
#include "dot1q_debug.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"
#include "mirror_api.h"
#include "dot1x_api.h"
#include "l7_ip_api.h"

/* Externs */

extern dot1qCfgData_t      *dot1qCfg;
extern dot1qVlanCheckNotifyList_t vlanRequestCheckList[L7_LAST_COMPONENT_ID];


/* Local declarations */
dot1qIntfData_t     *dot1qQports  = L7_NULLPTR;
dot1qInfo_t         *dot1qInfo    = L7_NULLPTR;
vlanTreeTables_t       *pVlanTreeHeap = L7_NULLPTR;
dot1q_vlan_dataEntry_t *pVlanDataHeap = L7_NULLPTR;
L7_uint32           *dot1qMapTbl   = L7_NULLPTR;
L7_uint32           *dot1qVlanCfgMapTbl = L7_NULLPTR;
dot1qVidList_t      *pVidList      = L7_NULLPTR;

/*Mask of vlans that are participating for a given port */
L7_VLAN_MASK_t      dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];

/* Vlan intf structures */
dot1qVlanIntf_t     dot1qVlanIntf[DOT1Q_MAX_VLAN_INTF + 1];

/* Mask of valid ports */
NIM_INTF_MASK_t dot1qValidMemberPortsMask;


/*********************************************************************
* @purpose  Register dot1q system counters with the Statistics Manager
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t dot1qStatsSystemCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;
  L7_uint32 listSize_switch = (L7_uint32)(sizeof(dot1q_counters_switch) / sizeof(L7_uint32));
  statsParm_entry_t dot1qCtr;

  dot1qCtr.cKey = 1;
  dot1qCtr.cSize = C32_BITS;
  dot1qCtr.cType = ExternalCtr;
  dot1qCtr.isResettable = L7_FALSE;
  dot1qCtr.pMutlingsParmList = L7_NULL;
  dot1qCtr.pSource = (L7_VOIDFUNCPTR_t) dot1qStatGet;

  /* create the switch counters */
  for (i=0; (i< listSize_switch) && (rc == L7_SUCCESS); i++)
  {
    dot1qCtr.cId = dot1q_counters_switch[i];
    rc = statsCreate(1, (pStatsParm_list_t)(&dot1qCtr));
  }

  return rc;
}

/*********************************************************************
* @purpose  Register dot1q interface counters with the Statistics Manager
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t dot1qStatsIntfCreate(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_uint32 listSize_port = (L7_uint32)(sizeof(dot1q_counters_port) / sizeof(L7_uint32));
  statsParm_entry_t dot1qCtr;

  /* Create counters for interfaces which are valid participants in a VLAN */
  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  rc = L7_SUCCESS;
  dot1qCtr.cSize = C32_BITS;
  dot1qCtr.cType = ExternalCtr;
  dot1qCtr.isResettable = L7_FALSE;
  dot1qCtr.pMutlingsParmList = L7_NULL;
  dot1qCtr.pSource = (L7_VOIDFUNCPTR_t) dot1qStatGet;

  dot1qCtr.cKey = intIfNum;
  for (i=0; (i< listSize_port && rc == L7_SUCCESS);i++)
  {
    dot1qCtr.cId = dot1q_counters_port[i];
    rc = statsCreate(1, (pStatsParm_list_t)(&dot1qCtr));
  }
  /* Invoke USMDB to create its set of DOT1Q user counters */
  rc = usmDbUserStatsCreate(usmDbThisUnitGet(), L7_USMDB_USER_STAT_DOT1Q, intIfNum);

  return rc;
}



/*********************************************************************
* @purpose  Receive interface changes from NIM
*
* @param    event               Any one of L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intIfEvent, NIM_CORRELATOR_t correlator)
{
    DOT1Q_MSG_t msg;
    NIM_EVENT_COMPLETE_INFO_t status;
    L7_RC_t rc;

    status.intIfNum     = intIfNum;
    status.component    = L7_DOT1Q_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.response.reason   = NIM_ERR_RC_UNUSED ;
    status.event        = intIfEvent;
    status.correlator   = correlator;


    /* Do not submit unnecessary messages to the queue */
    if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    {
        status.response.rc = L7_SUCCESS;
        nimEventStatusCallback(status);
        return L7_SUCCESS;
    }

    if (intIfEvent != L7_CREATE &&
        intIfEvent != L7_ATTACH &&
        intIfEvent != L7_DETACH &&
        intIfEvent != L7_DELETE &&
        intIfEvent != L7_ACTIVE &&
        intIfEvent != L7_INACTIVE &&
        intIfEvent != L7_PROBE_SETUP &&
        intIfEvent != L7_PORT_ROUTING_ENABLED &&
        intIfEvent != L7_LAG_ACQUIRE &&
        intIfEvent != L7_DOT1X_ACQUIRE &&
        intIfEvent != L7_PROBE_TEARDOWN &&
        intIfEvent != L7_PORT_ROUTING_DISABLED &&
        intIfEvent != L7_LAG_RELEASE &&
        intIfEvent != L7_DOT1X_RELEASE)
    {
        status.response.rc = L7_SUCCESS;
        nimEventStatusCallback(status);
        return L7_SUCCESS;
    }




    /* Issue the  command to dot1q */

    msg.event = dot1q_intfChange ;
    msg.data.status = status;

    /* Check if Dot1q is ready to process the event or not */
    if (dot1qCnfgrStateCheck() != L7_TRUE)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Received a interface callback while outside the EXECUTE state");
      return L7_FAILURE;
    }

    rc = dot1qIssueCmd(&msg);

    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Process interface change event received from NIM
*
* @param    event               Any one of L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfChangeProcess(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 vlanId = 0;
  L7_uint32 i;
  L7_INTF_TYPES_t intfType;
  L7_uint32 event = status.event;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

 /* Check if Dot1q is ready to process the event or not */
  if (dot1qCnfgrStateCheck() != L7_TRUE)
  {

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Cannot process interface callback while outside the EXECUTE state");
    return L7_FAILURE;
  }

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Invalid intIfNum");
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return rc;
  }

  DOT1Q_TRACE("received int change callback intf (%d, %s) event %d \n", intIfNum, ifName, event);

  switch (event)
  {
    case L7_CREATE:
      /* create interface default config if needed */
      rc = dot1qIntfCreate(intIfNum);

      /* create interface stats */
      dot1qStatsIntfCreate(intIfNum);

      break;

  case L7_ATTACH:
      /* take whats in the config file, apply to the application */
      rc = dot1qApplyIntfConfigData(intIfNum);

      /* apply whats in application to hardware
      rc = dot1qApplyIntfConfigToHW(intIfNum);  */
      break;

    case L7_DETACH:
      (void)dot1qVlanIntfIntfChange(intIfNum, L7_INACTIVE);

      rc = dot1qIntfUnapplyConfigData(intIfNum);

      break;

    case L7_DELETE:
       {
         /*----------------------------------------------------------------------------------------*/
         /*  save the configuration for the next reboot                                            */
         /*----------------------------------------------------------------------------------------*/
         rc = nimGetIntfType(intIfNum,&intfType);
         if (rc != L7_SUCCESS)
         {
            return rc;
         }

         if (intfType == L7_LOGICAL_VLAN_INTF)
         {
           /*Get Vlan ID from intIfNum */
           for (i=1; (int)i <= DOT1Q_MAX_VLAN_INTF; i++)
           {
            if ((dot1qVlanIntf[i].inUse == L7_TRUE) &&
               (dot1qVlanIntf[i].intIfNum == intIfNum))
              {
               vlanId = dot1qVlanIntf[i].vlanId;
               break;
              }
           }

           L7_VLAN_CLRMASKBIT(dot1qCfg->QvlanIntf, vlanId);
           dot1qCfg->hdr.dataChanged = L7_TRUE;

         }

         rc = dot1qIntfDelete(intIfNum);
         break;
       }

    case L7_UP:
      break;

    case L7_DOWN:
      break;

    case L7_ACTIVE:
      rc = dot1qVlanIntfIntfChange(intIfNum, event);
      break;

    case L7_INACTIVE:
      rc = dot1qVlanIntfIntfChange(intIfNum, event);
      break;

  case L7_PROBE_SETUP:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_PORT_ROUTING_ENABLED:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_LAG_ACQUIRE:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
      break;
  case L7_DOT1X_ACQUIRE:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList,L7_DOT1X_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_PROBE_TEARDOWN:
      /* There are issues in the driver such that it is required that the operator
        manually reconfigure the vlans/garp/gmrp on probe teardown. We cannot
      automagically re-enable the vlan configuration. */
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;
  case L7_PORT_ROUTING_DISABLED:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;
  case L7_LAG_RELEASE:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
      break;
  case L7_DOT1X_RELEASE:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList,L7_DOT1X_COMPONENT_ID);
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;

    default:
      break;
  }

  DOT1Q_TRACE("Calling nimStatusCB: rc = %d, reason = %d\n", status.response.rc, status.response.reason);
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase    Activate/Create
*
* @returns  void
*
* @notes    Transfer to dot1q thread
*
* @end
*********************************************************************/
void dot1qIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{

    DOT1Q_MSG_t msg;
    L7_RC_t rc;

    /* Issue the  command to dot1q */
    msg.event = dot1q_nimStartup ;
    msg.data.nimStartup.startupPhase = startupPhase;

    /* Check if dot1q is ready to process the event or not */
    if (dot1qCnfgrStateCheck() != L7_TRUE)
    {
      nimStartupEventDone(L7_DOT1Q_COMPONENT_ID);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
              "Received a interface startup callback while outside the EXECUTE state. Did not process phase %d",
              startupPhase);
      return;
    }

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Failed to issue command for startup phase %d", startupPhase);
    }
    return;
}


/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    Dot1q has no configuration at this point
*
* @end
*********************************************************************/
L7_RC_t dot1qNimCreateStartup(void)
{
  L7_RC_t          rc;
  L7_uint32        intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if (dot1qCnfgrStateCheck() == L7_TRUE)
    {
      /* General snoop interface create */

      if (dot1qIntfIsValid(intIfNum) == L7_TRUE)
      {
        if (dot1qIntfCreate(intIfNum) != L7_SUCCESS)
        {

            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Failed to create an instance for intf %s in dot1q\n", ifName);
        }

        if (dot1qStatsIntfCreate(intIfNum) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Failed to create statistics for intf %s in dot1q\n", ifName);
        }
      }
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_DELETE);
  nimRegisterIntfEvents(L7_DOT1Q_COMPONENT_ID, dot1qInfo->nimStatus.nimEventMask);

  nimStartupEventDone(L7_DOT1Q_COMPONENT_ID);

  dot1qInfo->nimStatus.createStartupDone = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qNimActivateStartup(void)
{
  L7_RC_t          rc;
  L7_uint32        intIfNum;
  L7_INTF_STATES_t intIfState;
  L7_BOOL          isAcquired;
  L7_uint32        activeState;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    intIfState = nimGetIntfState(intIfNum);

    switch (intIfState)
    {
      case L7_INTF_UNINITIALIZED:
      case L7_INTF_CREATING:
      case L7_INTF_CREATED: /* Already receiving this event since startup callback */
      case L7_INTF_DELETING: /* Already receiving this event since startup callback */
      case L7_INTF_ATTACHING:
      case L7_INTF_DETACHING: /* Since interface attach was never processed, nothing to do here */
        break;

      case L7_INTF_ATTACHED:
        if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
        {
            /* Advance to next interface */
            break;
        }
        /* Check if the interface should be acquired */
        isAcquired = dot1qIntfAcquireFlagSet(intIfNum);
        if (isAcquired == L7_FALSE)
        {
            /*
                There is no need to attach the interface configuration if the interface will be acquired.
                Do this after the acquired check. */
            rc = dot1qApplyIntfConfigData(intIfNum);

            if (rc == L7_SUCCESS)
            {
                /*
                Determine if interface is active
                */
                activeState = L7_INACTIVE;
                if (nimGetIntfActiveState(intIfNum, &activeState) == L7_SUCCESS &&
                    activeState == L7_ACTIVE)
                {
                    if (dot1qVlanIntfIntfChange(intIfNum, L7_ACTIVE) != L7_SUCCESS)
                    {
                        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                                "Unable to activate interface intf %s\n", ifName);
                    }
                }
            }
            else
            {
                 L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                         "Unable to apply interface config for intf %s\n", ifName);
            }
        }
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1Q_COMPONENT_ID,
                  "Received unknown NIM intIfState %d", intIfState);
        break;
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_INACTIVE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_DELETE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_DETACH);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_LAG_ACQUIRE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_LAG_RELEASE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_PROBE_TEARDOWN);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_PORT_ROUTING_ENABLED);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_PORT_ROUTING_DISABLED);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_DOT1X_ACQUIRE);
  PORTEVENT_SETMASKBIT(dot1qInfo->nimStatus.nimEventMask, L7_DOT1X_RELEASE);

  nimRegisterIntfEvents(L7_DOT1Q_COMPONENT_ID,dot1qInfo->nimStatus.nimEventMask);

  nimStartupEventDone(L7_DOT1Q_COMPONENT_ID);

  /* Inform cnfgr that protected port has completed it HW updates */
  /* startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP */
  cnfgrApiComponentHwUpdateDone(L7_DOT1Q_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Set the acquired flag for an interface, if needed
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    This routine does not make related changes to config or status
* @notes    This is expected to be done at activate
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfAcquireFlagSet(L7_uint32 intIfNum)
{
  L7_BOOL          isAcquired;
  //L7_uint32        routingEnabled;

  isAcquired = L7_FALSE;

#if 0
  if(mirrorIsActiveProbePort(intIfNum) == L7_TRUE)
  {
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
      isAcquired = L7_TRUE;
  }
#endif
  if (dot3adIsLagActiveMember(intIfNum) == L7_TRUE)
  {
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
      isAcquired = L7_TRUE;
  }
#if 0
  (void) ipMapRtrIntfModeGet(intIfNum, &routingEnabled);
  if (routingEnabled == L7_ENABLE)
  {
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
      isAcquired = L7_TRUE;
  }

  if (dot1xPortIsAuthorized(intIfNum) != L7_SUCCESS)
  {
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList,L7_DOT1X_COMPONENT_ID);
      isAcquired = L7_TRUE;
  }
#endif

  return isAcquired;
}

/*********************************************************************
* @purpose  Allow stats manager to get a dot1q related statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*
* @end
*********************************************************************/
void dot1qStatGet(pCounterValue_entry_t c)
{
  c->status     = L7_SUCCESS;
  c->cSize      = C32_BITS;

  switch (c->cId)
  {
  case L7_PLATFORM_CTR_DYNAMIC_VLAN_COUNT:
    c->cValue.low = dot1qInfo->DynamicVLANCount;
    break;
  case L7_PLATFORM_CTR_MOST_VLAN_COUNT:
    c->cValue.low = dot1qInfo->MostVLANCount;
    break;
  case L7_PLATFORM_CTR_FAILED_VLAN_CREATIONS:
    c->cValue.low = dot1qInfo->FailedVLANCreations;
    break;
  case L7_PLATFORM_CTR_VLAN_DELETION_COUNT:
    c->cValue.low = dot1qInfo->VLANDeletionCount;
    break;
  default:
    c->status = dtlStatsGet(c->cKey, c->cId, c->cSize, &(c->cValue));
  }
}


/*********************************************************************
* @purpose  Given a VLAN ID, get the next VLAN ID that should be considered
*           for internal vlan assignment
*
* @param    current        current VLAN ID
* @param    next           next VLAN ID
*
* @returns  L7_SUCCESS
*           L7_NOT_EXIST if next VLAN ID is the base VLAN ID
*           L7_ERROR if current is out of range
*
* @notes    wraps when the min or max is reached
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanNext(L7_uint32 current, L7_uint32 *next)
{
  DOT1Q_INTERNAL_VLAN_POLICY policy = dot1qInternalVlanPolicyGet();
  L7_uint32 vid;

  if ((current < L7_DOT1Q_MIN_VLAN_ID) || (current > L7_DOT1Q_MAX_VLAN_ID))
    return L7_ERROR;

  if (policy == DOT1Q_INT_VLAN_POLICY_ASCENDING)
  {
    if (current < L7_DOT1Q_MAX_VLAN_ID)
      vid = current + 1;
    else
      vid = L7_DOT1Q_MIN_VLAN_ID;
  }
  else
  {
    if (current > L7_DOT1Q_MIN_VLAN_ID)
      vid = current - 1;
    else
      vid = L7_DOT1Q_MAX_VLAN_ID;
  }
  /* Check if we've wrapped all the way back to the start */
  if (vid == dot1qBaseInternalVlanIdGet())
    return L7_NOT_EXIST;

  *next = vid;
  return L7_SUCCESS;
}
