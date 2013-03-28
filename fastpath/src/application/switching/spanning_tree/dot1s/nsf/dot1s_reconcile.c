/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_reconcile.c
* @purpose     802.1s Multiple Spanning Tree reconcile NSF data
* @component   dot1s
* @comments    none
* @create      4/12/09
* @author      akulkarn
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "dot1s_db.h"
#include "dot1s_cnfgr.h"
#include "default_cnfgr.h"
#include "dot1s_nsf.h"
#include "dot1s_api.h"
#include "dot1s_control.h"
#include "dot1s_ih.h"
#include "osapi.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "l7_ip_api.h"
#include "dot1s_debug.h"
#include "osapi_trace.h"

dot1s_intf_recon_data_t *port_recon_data;
dot1s_nsf_recon_states_t dot1sReconPhase = DOT1S_RECON_NA;
osapiTimerDescr_t        *dot1sReconTimer;
extern void *dot1sTaskSyncSema;
static L7_INTF_MASK_t dot1sPduWaitingMask;

void dot1sNsfDebugReconStatus();

extern L7_uint32 dot1sPortIndexFromIntfNumGet(L7_uint32 intIfNum);

dot1s_intf_recon_data_t *dot1sIntfReconDataFind(L7_uint32 intIfNum)
{
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  if (portIndex != L7_NULL )
  {
    return &port_recon_data[portIndex];
  }

  return L7_NULLPTR;
}

char* dot1sNsfReconEventGet(dot1s_recon_intf_events_t reconEvent)
{
  switch (reconEvent)
  {
    case dot1s_recon_event_pdu_rx : return "PDU_RX";
    case dot1s_recon_event_state_change_begin : return "State Change begin";
    case dot1s_recon_event_state_change_finish : return "State Change finish";
    case dot1s_recon_event_flush : return "Flush";
    case dot1s_recon_event_update : return "Update";
    default : return "Unknown";
  }
}

char *dot1sNsfReconStateGet(dot1s_nsf_recon_states_t reconState)
{
  switch (reconState)
  {
    case DOT1S_RECON_NA : return "DOT1S_RECON_NA";
    case DOT1S_NSF_SO_BEGIN : return "DOT1S_NSF_SO_BEGIN";
    case DOT1S_NSF_SO_IF_ACT : return "DOT1S_NSF_SO_IF_ACT";
    case DOT1S_NSF_EXT_RECON : return "DOT1S_NSF_EXT_RECON";
    case DOT1S_NSF_STATE_SYNC : return "DOT1S_NSF_STATE_SYNC";
    case DOT1S_READY : return "DOT1S_READY";
    default : return "Unknown";
  }
}


/*********************************************************************
* @purpose  Determine if the system notification is needed
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    state  @b{(input)}
*
* @returns  L7_SUCCESS
*
* @notes    This function is called when reconciliation detects the ckpted
*           state is different from steady state. We have already notified
*           the ckpted state to the system. Now do we need to notify the
*           state?
* @end
*********************************************************************/
L7_BOOL dot1sNsfIsNotifyNeeded(dot1s_nsf_port_checkpoint_data_t *ckptPort,
                               L7_uint32 newState)
{
  L7_uint32 index;

  for (index = 0; index <= L7_MAX_MULTIPLE_STP_INSTANCES; index++)
  {

    if (dot1sInstanceMap[index].inUse == L7_TRUE)
    {
      if (ckptPort->instInfo[index].portState == newState)
      {
        return(L7_FALSE);
      }
    }
  }

  return L7_TRUE;

}

/*********************************************************************
* @purpose  Is the port going to issue tcPropTree
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    state  @b{(input)}
*
* @returns  L7_SUCCESS
*
* @notes    The dot1s reconciliation is not complete until a port goes
*           to detected state in the topology state machine. This function
*           tells the reconciliation logic whether or not wait for that
*           step before declaring the port as reconciled.
*
* @end
*********************************************************************/
L7_BOOL dot1sIsFlushExpected(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32 instIndex;

  /* If the port is edge port it is not going to send flush*/
  if (p->operEdge == L7_TRUE)
  {
    return L7_FALSE;
  }

  for (instIndex = 0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
    {
      if (p->portInstInfo[instIndex].forward == L7_TRUE)
      {
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}
/*********************************************************************
* @purpose  handle failure to reconcile
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    state  @b{(input)}
*
* @returns  L7_SUCCESS
*
* @notes    This function is only called for interfaces which are enabled for spanning tree
*           This function will not restart the state machines but tell the driver the
*           application state instead of the check pointed state
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfReconcileFailed()
{
  DOT1S_PORT_COMMON_t *p;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  L7_uint32 instIndex, mstID;
  L7_RC_t rc = L7_SUCCESS;
  dot1s_intf_recon_data_t *port_recon;


  /*    walk through the ports to identify the notification to the hardware and system.
       update the notification if needed. Tell the cnfgr  dot1s reconciliation is done
       update app state , delete timer
    */
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
          " DOT1S Reconciliation failed at %d: resuming state machines ",
          osapiUpTimeMillisecondsGet());


  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    ckptPort = dot1sIntfCkptDataFind(p->portNum);
    port_recon = dot1sIntfReconDataFind(p->portNum );
    /* For each instance check the state */
    port_recon->intfReconciled = L7_TRUE;

    for (instIndex = 0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
    {
      /* If ckpointed state is available that is the one we have check pointed
         If the current state differs from the check pointed state
         call the driver with new state
         notify the system

         update application state

       */
      if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,"%s: port %d inst %d state %d ",
                        __FUNCTION__, p->portNum, instIndex,
                        p->portInstInfo[instIndex].portState);

        if (p->portInstInfo[instIndex].portState !=
            ckptPort->instInfo[instIndex].portState)
        {
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "State does not match ckpointed %d ",
                          ckptPort->instInfo[instIndex].portState);

          if ((rc = dot1sInstNumFind(instIndex, &mstID)) == L7_SUCCESS)
          {
            dot1sIhSetPortState(mstID, p->portNum,
                                p->portInstInfo[instIndex].portState);
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                    "Cannot find inst num");
            return rc;
          }

        }
      }/* if instance is in use*/

    } /* for instance*/

    p = dot1sIntfFindNext(p->portNum);

  }/* while*/

  return rc;

}

void dot1sReconcileTimerCallback(L7_uint32 intf, L7_uint32 state)
{
  dot1sIssueCmd(dot1sReconError, intf, L7_NULL, L7_NULLPTR);
}
/*********************************************************************
* @purpose  Handle the timer callback
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    state  @b{(input)}
*
* @returns  L7_SUCCESS
*
* @notes    Note we do not use the params but need it for the osapiTimer callback
*
* @end
*********************************************************************/
void dot1sReconcileTimerExpiredProcess()
{
  DOT1S_PORT_COMMON_t *p;
  dot1s_intf_recon_data_t *port_recon ;
  L7_uint32 expiredTime;

  expiredTime = osapiUpTimeMillisecondsGet();

  /* Timer has expired indicates that we haven't received PDUs on our interfaces
     This means that the information that we have been acting on so far could be stale.
     Restart the application
     notify cnfgr with recon phase.

  */

  if (dot1sAppState == DOT1S_NSF_EXT_RECON)
  {
    /* The timer expired before the code got a chance to free it*/
    return;
  }



  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
          "DOT1S Reconciliation Timer expired at %d: restarting state machines",
          expiredTime );
  p = dot1sIntfFindFirst();

  while (p != L7_NULLPTR)
  {
    /* Try to find out why we could not reconcile in the given timer
       So as to give the user meaningful debug.
    */
    port_recon = dot1sIntfReconDataFind(p->portNum);
    if ((port_recon != L7_NULLPTR) &&
        (port_recon->intfReconciled != L7_TRUE))
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
              "Port %d Not reconciled : reconciliation state %s %s on PDU ",
              p->portNum, dot1sNsfReconStateGet(port_recon->intReconState),
              (port_recon->pduExpected) ? "Waiting" : "not waiting");
    }

    if (p->portEnabled == L7_TRUE)
    {
      dot1sUpdateIntfReconciled(p->portNum);
      dot1sPortStateMachineInit(p, L7_TRUE);
    }
    p = dot1sIntfFindNext(p->portNum);
  }
  dot1sAppState = DOT1S_READY;
  /*
   dot1sSwitchModeEnableSet(L7_TRUE);
   */
  cnfgrApiComponentHwUpdateDone(L7_DOT1S_COMPONENT_ID,
                                L7_CNFGR_HW_APPLY_CONFIG);

}


/*********************************************************************
* @purpose  Reconcile the interface data with configured values
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dot1sIsIntfReconciled(L7_uint32 intIfNum)
{

  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);
  if (port_recon == L7_NULLPTR)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Intf %s cannot get recon data ", ifName);
    return L7_FALSE;
  }
  return port_recon->intfReconciled;
}

/*********************************************************************
* @purpose  Update the intf state to reconciled
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void dot1sUpdateIntfReconciled(L7_uint32 intIfNum)
{
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);

  if (port_recon == L7_NULLPTR)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Intf %s cannot get recon data ", ifName);
    return;
  }

  port_recon->intfReconciled = L7_TRUE;
  port_recon->pduExpected = L7_FALSE;
  port_recon->intReconState = DOT1S_READY;
}

/*********************************************************************
* @purpose  check if all the interfaces are reconciled
*
* @param    coldRestart  @b{(input)} if the restart type is cold
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dot1sCheckGlobalReconciled(L7_BOOL coldRestart)
{

  L7_BOOL globalReconcile = L7_TRUE;
  L7_BOOL ext_recon = L7_TRUE;
  dot1s_nsf_recon_states_t oldAppState;
  L7_uint32 intIfNum;
  dot1s_intf_recon_data_t *port_recon;
  L7_RC_t rc;

  if ((dot1sCnfgrAppIsReady() == L7_TRUE) &&
      (coldRestart != L7_TRUE))
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                    "%s(%d) ", __FUNCTION__, __LINE__);
    return;
  }

  rc = nimFirstValidIntfNumber(&intIfNum);

  /* Check if all the interface are reconciled */
  if (coldRestart != L7_TRUE)
  {
    while (rc == L7_SUCCESS)
    {
      if (dot1sIsValidIntf(intIfNum) == L7_TRUE)
      {

        port_recon = dot1sIntfReconDataFind(intIfNum);
        if ((port_recon != L7_NULLPTR) &&
            (port_recon->intfReconciled != L7_TRUE))
        {
          /* The intf will be reconciled once its state catches up
             with check pointed state.That has not happened for this interface.
             Have the roles atleast synced yet?
          */
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "%s:intIfNum %d not yet synced",
                          __FUNCTION__, intIfNum);
          globalReconcile = L7_FALSE;
          if ((port_recon->intReconState != DOT1S_NSF_EXT_RECON) &&
              (port_recon->intReconState != DOT1S_NSF_STATE_SYNC))
          {
            DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                            "Ext recon false for port %d recon state %s",
                            intIfNum,
                            dot1sNsfReconStateGet(port_recon->intReconState));
            ext_recon = L7_FALSE;
            break;
          }

        }
      }

      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }
  }


  oldAppState = dot1sAppState;

  if ((ext_recon == L7_TRUE && oldAppState == DOT1S_NSF_SO_IF_ACT) ||
      (coldRestart == L7_TRUE))
  {
    /* Notify the system that we are in sync.*/
    dot1sStartupTime.extRecon = osapiUpTimeMillisecondsGet();
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_STATUS,
                    "DOT1S External Reconciliation complete");
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_DOT1S_EXTERNAL_RECON_END, L7_NULLPTR, 0);
    dot1sAppState = DOT1S_NSF_EXT_RECON;
    /* Notify the cnfgr of recon complete*/
    osapiTimerFree(dot1sReconTimer);
    dot1sReconTimer = L7_NULLPTR;
    cnfgrApiComponentHwUpdateDone(L7_DOT1S_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);

  }

  /* All the interfaces have reconciled. Reconciliation is complete*/
  if (globalReconcile == L7_TRUE)
  {
    dot1sStartupTime.reconComplete = osapiUpTimeMillisecondsGet();
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_STATUS,
                    "DOT1S Reconciliation complete"); 
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_DOT1S_GLOBAL_RECON_END, L7_NULLPTR, 0);
    dot1sAppState = DOT1S_READY;
    /* After recon is done make sure we update the latest and greates state
       to the backup.
    */
    if (dot1sBackupMgrElected == L7_TRUE)
    {
      dot1sNsfCheckpointPrepare();
      rc = ckptMgrCheckpointStart(L7_DOT1S_COMPONENT_ID);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "Call to checkpoint service to start failed(%d)",rc);
      }
    }
  }

}

/*********************************************************************
* @purpose  check if the interface is reconciled
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
dot1s_nsf_recon_states_t dot1sCheckIntfReconciled(L7_uint32 intIfNum)
{
  L7_uint32 i;
  DOT1S_PORT_ROLE_t ckptRole, currRole;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  DOT1S_PORT_COMMON_t *p;
  L7_BOOL roleReconciled = L7_TRUE, stateReconciled = L7_TRUE;
  L7_BOOL intfReconPause = L7_FALSE;
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);
  L7_BOOL reconFailed = L7_FALSE;
  L7_uint32 ckptState, currState, steadyState;
  dot1s_nsf_recon_states_t isIntfReconciled = DOT1S_RECON_NA;
  L7_BOOL result; /* used to store the result of maskis non zero*/



  if (port_recon == L7_NULLPTR)
  {
    return isIntfReconciled;
  }

  if ((port_recon->pduExpected == L7_TRUE) ||
      (port_recon->intReconState == DOT1S_RECON_NA) ||
      (port_recon->intReconState == DOT1S_NSF_SO_BEGIN))
  {
    /* Nothing to look at until we receive and process the pdu
       we have been waiting for or the timer expires.
    */

    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                    "port(%d) not ready PDU expected(%d) recon State %d ",
                    intIfNum, port_recon->pduExpected,
                    port_recon->intReconState);
    return isIntfReconciled;
  }


  p = dot1sIntfFind(intIfNum);
  ckptPort = dot1sIntfCkptDataFind(intIfNum);

  if (p == L7_NULLPTR || ckptPort == L7_NULLPTR)
  {
    return isIntfReconciled;
  }

  if (port_recon->intfReconciled == L7_TRUE)
  {
    return DOT1S_READY;
  }
  /* For all the instances, go through the roles and determine
     if they agree with whatever checkpointed data we have.
     (They may not be same e.g. Alternate ->Root)

  */
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,"%s on intf %d ",
                  __FUNCTION__, p->portNum);

  for (i = 0; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE)
    {
      ckptRole = ckptPort->instInfo[i].portRole;
      currRole = p->portInstInfo[i].role;
      ckptState = ckptPort->instInfo[i].portState;
      currState = p->portInstInfo[i].portState;
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,"Port %d inst(%d)"
                      "ckpt role(%d) curr Role(%d) ckptState(%d) currState(%d)",
                      p->portNum,i,ckptRole,currRole,ckptState,currState); 

      if (ckptRole != currRole)
      {
        /* REgion boundary has changed*/
        if (((currRole == ROLE_MASTER) &&
             (ckptRole != ROLE_ALTERNATE)) ||
            (ckptRole == ROLE_MASTER))
        {
          /* Error in reconciliation */
          roleReconciled = L7_FALSE;
          reconFailed = L7_TRUE;
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,"%s(%d):role not in "
                          "agreement port %d inst %d ", __FUNCTION__,
                          __LINE__, p->portNum, i);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "Curr Role %d ckpt role %d ", currRole, ckptRole);
          break;
        }

        /* the port mode has changed*/
        if ((currRole == ROLE_DISABLED) ||
            (ckptRole == ROLE_DISABLED))
        {
          /* Error in reconciliation */
          roleReconciled = L7_FALSE;
          reconFailed = L7_TRUE;
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "%s(%d):role not in agreement port %d inst %d ",
                          __FUNCTION__, __LINE__, p->portNum, i);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "Curr Role %d ckpt role %d ", currRole, ckptRole);
          break;
        }

        /* Received inferior information
         Note we have received PDU*/
        if ((currRole == ROLE_DESIGNATED) &&
            ((ckptRole == ROLE_ROOT) ||
             (ckptRole == ROLE_ALTERNATE) ||
             (ckptRole == ROLE_BACKUP)))
        {
          roleReconciled = L7_FALSE;
          reconFailed = L7_TRUE;
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "%s(%d):role not in agreement port %d inst %d ",
                          __FUNCTION__, __LINE__, p->portNum, i);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "Curr Role %d ckpt role %d ", currRole, ckptRole);
          break;
        }

        /* New information is available*/
        if ((ckptRole == ROLE_DESIGNATED) &&
            ((currRole == ROLE_ROOT) ||
             (currRole == ROLE_ALTERNATE) ||
             (currRole == ROLE_BACKUP)))
        {
          roleReconciled = L7_FALSE;
          reconFailed = L7_TRUE;
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "%s(%d):role not in agreement port %d inst %d ",
                          __FUNCTION__, __LINE__, p->portNum, i);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "Curr Role %d ckpt role %d ", currRole, ckptRole);
          break;
        }


      }/* roles not same*/

      L7_INTF_NONZEROMASK(dot1sPduWaitingMask,result)
      /* If we have received all the PDU we have been waiting for and still the
         steady state is different from the ckpointed state, we need to tell
         H/W and the rest of the system the new state.
      */
      if ((currRole != ckptRole) &&
          (result == L7_FALSE))
      {
        /* Find out what the steady state for this role should be*/
        if ((currRole == ROLE_ROOT) ||
            (currRole == ROLE_DESIGNATED) ||
            (currRole == ROLE_MASTER))
        {
          steadyState = L7_DOT1S_FORWARDING;
        }
        else
        {
          steadyState = L7_DOT1S_DISCARDING;
        }
      }
      else
      {
        /* Go with whatever we have so far */
        steadyState = ckptState;
        if (currRole != ckptRole)
        {
          /* We have not received all the pdus yet
             But the ckpted roles do not agree so wait until we rx all pdus
          */
          intfReconPause = L7_TRUE;
        }
      }
      /* Has state reconciled for each instance? */
      stateReconciled = (stateReconciled && (steadyState == currState));

      if (stateReconciled == L7_TRUE)
      {
        if ((p->portInstInfo[i].portState != ckptState) &&
            (port_recon->intReconState == DOT1S_NSF_SO_IF_ACT))
        {
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                          "Setting Port %d inst %d steady state %s ",
                          p->portNum, i, dot1sStateStringGet(currState) );
          dot1sNsfStateSet(intIfNum,i, currState, L7_FALSE);
          /* Store the new state as ckpted; need this for notifications*/
          ckptPort->instInfo[i].portState = currState;
        }
      }

    }/* Instance present*/
  }


  if (roleReconciled != L7_TRUE && stateReconciled != L7_TRUE)
  {
    /*  If we have received all the pdus..
       Some new network event has made our checkpointed data obsolete
     */
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,"reconcile failed on intf %d ",
                    p->portNum);
    dot1sNsfReconcileFailed();
    return DOT1S_RECON_NA;

  }


  /* The state machines have caught up for all the instances on this port*/
  if ((roleReconciled == L7_TRUE) &&
      (port_recon->intReconState == DOT1S_NSF_SO_IF_ACT) &&
      (intfReconPause == L7_FALSE))
  {
    /* Forwarding port will be reconciled when flush is complete*/
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                    "role reconciled for %d  moving port to EXT RECON",
                    p->portNum);
    port_recon->intReconState = DOT1S_NSF_EXT_RECON;
  }

  if ((stateReconciled == L7_TRUE) &&
      (port_recon->intReconState == DOT1S_NSF_EXT_RECON))
  {
    /* The roles are in sync but the states will come later*/
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                    "State  reconciled for %d moving port to state recon",
                    p->portNum);
    port_recon->intReconState = DOT1S_NSF_STATE_SYNC;
  }

  return port_recon->intReconState;
}

/*********************************************************************
* @purpose  check if the port is expecting any state changes/flush to finish
*
* @param    intIfNum  @b{(input)}   internal intf of the port
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL isPortInstStateFlushDone(L7_uint32 intIfNum)
{
  L7_uint32 i;
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);

  for (i = 0; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE)
    {
      if ((port_recon->instInfo[i].numStatesWait != L7_NULL) ||
          (port_recon->instInfo[i].waitingOnFlush == L7_TRUE))
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                        "Port %d Waiting on %d states %s for inst %d", intIfNum,
                        port_recon->instInfo[i].numStatesWait,
                        (port_recon->instInfo[i].waitingOnFlush == L7_TRUE) ?
                        " and Flush":"",i);

        return L7_FALSE;
      }
    }
  }

  return L7_TRUE;

}

/*********************************************************************
* @purpose  Function to handle reconciliation events like PDU reception etc.
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfReconEventFunc(L7_uint32 intIfNum, L7_uint32 instIndex,
                               dot1s_recon_intf_events_t reconEvent)
{
  DOT1S_PORT_COMMON_t *p = dot1sIntfFind(intIfNum);
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);
  dot1s_nsf_recon_states_t intfReconState = DOT1S_RECON_NA;
  L7_BOOL flushDone;

  if ((port_recon == L7_NULLPTR) ||
      (p == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if ((dot1sAppState == DOT1S_READY) ||
      (dot1sAppState == DOT1S_RECON_NA ))
  {
    return L7_SUCCESS;
  }

  if ((port_recon->intReconState == DOT1S_RECON_NA) ||
      (port_recon->intReconState == DOT1S_NSF_SO_BEGIN))
  {
    return L7_SUCCESS;
  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                  "%s: %s on INTF %d inst %d ", __FUNCTION__,
                  dot1sNsfReconEventGet(reconEvent), intIfNum, instIndex);
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                  "Port Pdu expected %s recon state %s",
                  (port_recon->pduExpected) ?"L7_TRUE" : "L7_FALSE",
                  dot1sNsfReconStateGet(port_recon->intReconState));
  switch (reconEvent)
  {
    case dot1s_recon_event_pdu_rx:
      if (port_recon->intReconState == DOT1S_READY)
      {
        return L7_SUCCESS;
      }
      L7_INTF_CLRMASKBIT(dot1sPduWaitingMask, intIfNum);
      port_recon->pduExpected = L7_FALSE;
      intfReconState = dot1sCheckIntfReconciled(intIfNum);
      if ((intfReconState == DOT1S_NSF_STATE_SYNC) &&
          (isPortInstStateFlushDone(intIfNum) == L7_TRUE))
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                        "intf %d reconciled ", intIfNum);
        port_recon->intReconState = DOT1S_READY;
        port_recon->intfReconciled = L7_TRUE;
      }

      break;
    case dot1s_recon_event_flush:
      if ((port_recon->intReconState != DOT1S_NSF_EXT_RECON) &&
          (port_recon->intReconState != DOT1S_NSF_STATE_SYNC) &&
          (port_recon->intReconState != DOT1S_NSF_SO_IF_ACT))
      {
        return L7_SUCCESS;
      }

      /* Process for all instances if the port is now edge port*/
      if (instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES)
      {
        port_recon->instInfo[instIndex].waitingOnFlush = L7_FALSE;
      }
      else if (instIndex == L7_DOT1S_MSTID_ALL)
      {
        for (instIndex=0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES;instIndex++)
        {
          port_recon->instInfo[instIndex].waitingOnFlush = L7_FALSE;
        }

      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                 "Invalid instance information received (%d) \n", instIndex);
        return L7_FAILURE;
      }

      intfReconState = dot1sCheckIntfReconciled(intIfNum);
      flushDone = isPortInstStateFlushDone(intIfNum);
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                     "is intfReconState %d recon state %d flushdone %d",
                      intfReconState, port_recon->intReconState, flushDone);
      if ((intfReconState == DOT1S_NSF_STATE_SYNC) && 
          (flushDone == L7_TRUE))
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                        "intf %d reconciled ", intIfNum);
        port_recon->intReconState = DOT1S_READY;
        port_recon->intfReconciled = L7_TRUE;
        intfReconState = DOT1S_NSF_STATE_SYNC;
      }
      break;
    case dot1s_recon_event_state_change_begin:
      if (port_recon->intReconState == DOT1S_NSF_STATE_SYNC)
      {
        /* We have received a state change call after syncing the state
           Update the intf as reconciled and return error.
        */
        dot1sUpdateIntfReconciled(intIfNum);
        return L7_ERROR;
      }

      break;
    case dot1s_recon_event_state_change_finish:

      port_recon->instInfo[instIndex].numStatesWait--;
      /* Fall thru*/
    case dot1s_recon_event_update:
      if ((port_recon->intReconState != DOT1S_NSF_EXT_RECON) &&
          (port_recon->intReconState != DOT1S_NSF_STATE_SYNC))
      {
        return L7_SUCCESS;
      }
      intfReconState = dot1sCheckIntfReconciled(intIfNum);
      flushDone = isPortInstStateFlushDone(intIfNum);
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                     "is intfReconState %d recon state %d flushdone %d",
                      intfReconState, port_recon->intReconState, flushDone);
      if ((intfReconState == DOT1S_NSF_STATE_SYNC) &&
          (flushDone == L7_TRUE))
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                        "intf %d reconciled ", intIfNum);
        port_recon->intReconState = DOT1S_READY;
        port_recon->intfReconciled = L7_TRUE;
      }
      break;
  }


  if ((intfReconState == DOT1S_NSF_EXT_RECON) ||
      (intfReconState == DOT1S_NSF_STATE_SYNC))
  {
    dot1sCheckGlobalReconciled(L7_FALSE);
  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_EVENTS,
                  "%s: Exit port recon state  %d ",
                  __FUNCTION__, port_recon->intReconState);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Prepare the reconcilation data for warm start
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfWarmRestartPrepare()
{
  L7_uint32 portIndex;

  /* Block notification channels
  1. Nim
  2. Driver.
  3. Helpers.
  */
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_STATUS,
                  "DOT1S Reconciliation begin");
  OSAPI_TRACE_EVENT(L7_TRACE_EVENT_DOT1S_EXTERNAL_RECON_START, L7_NULLPTR, 0);
  dot1sAppState = DOT1S_NSF_SO_BEGIN;

  for (portIndex = 1; portIndex < L7_DOT1S_MAX_INTERFACE_COUNT; portIndex++)
  {
    port_recon_data[portIndex].intfReconciled = L7_FALSE;
    port_recon_data[portIndex].intReconState = DOT1S_NSF_SO_BEGIN;

    memset(port_recon_data[portIndex].instInfo, 0,
           sizeof(port_recon_data[portIndex].instInfo));

  }

  memset(&dot1sPduWaitingMask, 0, sizeof(dot1sPduWaitingMask));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Perform any initialization tasks before activate begins
*
* @param
*
* @returns  None
*
* @notes  The timer could not be initialized in phase 3 init as
*         it would expire early if a large time is spent in config apply
*
* @end
*********************************************************************/
void dot1sNsfActivateStartupBegin()
{
  /* Add a timer that waits for given time for reception of BPDUS*/
  osapiTimerAdd(dot1sReconcileTimerCallback, L7_NULL, L7_NULL,
                DOT1S_RECON_TIMEOUT_MS, &dot1sReconTimer);


}
/*********************************************************************
* @purpose  Reconcile the interface data with configured values
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfReconcile(DOT1S_PORT_COMMON_t *p)
{
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 instIndex;
  DOT1S_PORT_ROLE_t ckptRole;
  DOT1S_BRIDGE_t *bridge;
  L7_uint32 ckptState;
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(p->portNum);

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "Attempt to reconcile intf %d ", p->portNum);
  bridge = dot1sBridgeParmsFind();
  port_recon->intfReconciled = L7_FALSE;
  ckptPort = dot1sIntfCkptDataFind(p->portNum);
  if (ckptPort == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Error: Cannot find check pointed data for port %d ", p->portNum);
    return L7_FAILURE;
  }
  rc = dot1sSwitchPortEnableSet(p->portNum);
  p->sendRSTP = ckptPort->common.sendRSTP;
  if (ckptPort->common.diagnosticDisable == L7_TRUE )
  {
    dot1sUpdateIntfReconciled(p->portNum);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "Intf %d diagnostically disabled, reconciling", p->portNum);
    (void)nimSetIntfAdminState(p->portNum, L7_DIAG_DISABLE);
    p->diagnosticDisable = L7_TRUE;
    /*dot1sIhDisablePort(p->portNum);*/
    /* set the state to discarding*/
  }

  else if (ckptPort->common.loopInconsistent == L7_TRUE)
  {
    /* If the port is configured for loop guard*/
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "Intf %d loopInconsistent, reconciling", p->portNum);
    dot1sUpdateIntfReconciled(p->portNum);
    if (p->loopGuard == L7_TRUE)
    {
      dot1sLoopInconsistentSet(p, DOT1S_CIST_INDEX, L7_TRUE);
    }
    else
    {
      /* There is some inconsistency here.
       * The ckpointed data says loopInconsistent but loopguard is not
       * configured.
      */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
              "Mismatch between checkpointed data and config for port %d ",
              p->portNum);
    }

  }
  else if (ckptPort->common.bpduGuardEffect == L7_TRUE)
  {
    dot1sUpdateIntfReconciled(p->portNum);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "Intf %d bpduGuardEffect is set, reconciling", p->portNum);

    if (p->adminEdge && bridge->bpduGuard)
    {
      p->bpduGuardEffect = L7_TRUE;
      dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_DISCARDING);
    }
  }

  if (port_recon->intfReconciled != L7_TRUE)
  {
    port_recon->pduExpected = L7_FALSE;
    port_recon->intReconState = DOT1S_NSF_SO_IF_ACT;

    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "Moving intf %d to DOT1S_NSF_SO_IF_ACT ", p->portNum);
    /* Now for the port Instance reconciliation*/
    for (instIndex = 0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
    {

      if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
      {
        ckptRole = ckptPort->instInfo[instIndex].portRole;
        ckptState = ckptPort->instInfo[instIndex].portState;
        if ((ckptRole == ROLE_ROOT) ||
            (ckptRole == ROLE_ALTERNATE) ||
            (ckptRole == ROLE_BACKUP))
        {
          port_recon->pduExpected = L7_TRUE;
          L7_INTF_SETMASKBIT(dot1sPduWaitingMask,p->portNum);
        }
        /* Notify the ckpointed state of the interface to the driver*/
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                        "Setting the checkpointed state (%s) "
                        "for intf %d instance %d  ",
                        dot1sStateStringGet(ckptState), p->portNum, instIndex);

        rc = dot1sNsfStateSet(p->portNum,instIndex, ckptState, L7_TRUE);
      }
    }
  } /* if port is not reconciled */


  dot1sCheckIntfReconciled(p->portNum);

  return rc;

}

/*********************************************************************
* @purpose  set the state of the port in the driver
*
* @param    intIfNum  @b{(input)}   internal intf of the port
* @param    instIndex @b{(input)}  the inst index for which the is to be set
* @param    state     @b{(input)}   the state of the port
* @param    isCkpted  @b{(input)}   If the state is same as ckpted
*
* @returns  L7_SUCCESS
*
* @notes    The state change called by this function is as determined
*           by the reconciliation logic.
*           If the state is steady state i.e. diff from ckpted state
*           then the state machine has already processed the pdu and the state
*           change and hence we do not need to wait on flush.
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfStateSet(L7_uint32 intIfNum, L7_uint32 instIndex,
                         L7_uint32 state, L7_BOOL isCkpted)
{
  L7_RC_t rc;
  L7_uint32 mstID;
  dot1s_intf_recon_data_t *port_recon = dot1sIntfReconDataFind(intIfNum);
  DOT1S_PORT_COMMON_t *p = dot1sIntfFind(intIfNum);
  dot1s_inst_recon_data_t *port_recon_inst;
  L7_BOOL forceNotify = L7_FALSE;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;

  ckptPort = dot1sIntfCkptDataFind(intIfNum);

  if ((p == L7_NULLPTR) || (port_recon == L7_NULLPTR) ||
      (ckptPort == L7_NULLPTR) ||
      (dot1sInstNumFind(instIndex, &mstID) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  port_recon_inst = &port_recon->instInfo[instIndex];

  rc = dtlDot1sStateSet(mstID, intIfNum, state, 0);

  if ((isCkpted == L7_FALSE) ||
      (p->operEdge == L7_TRUE))
  {

    if (p->operEdge == L7_TRUE &&
        instIndex == DOT1S_CIST_INDEX)
    {
      /* For edge ports; by the time we come here the state machine has
         already progressed, so the application thinks the port is forwarding
         and hence will not send a notification.
         So we force a notification to the rest of the system here
       */
      forceNotify = L7_TRUE;
    }
    else
    {
      /* Check ckpted data to determine if we need to notify system*/
      forceNotify = dot1sNsfIsNotifyNeeded(ckptPort, state);
    }
  }
  dot1sStateSetProtocolAction(intIfNum, mstID, state, L7_TRUE, forceNotify);
  port_recon_inst->numStatesWait++;
  if (port_recon->pduExpected != L7_TRUE)
  {
    port_recon_inst->waitingOnFlush = ((state == L7_DOT1S_FORWARDING) &&
                                       (p->operEdge != L7_TRUE) &&
                                       (isCkpted == L7_TRUE));

  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                 "Port %d to wait on State(%d)%s", intIfNum,
                 port_recon_inst->numStatesWait,
                 (port_recon_inst->waitingOnFlush == L7_TRUE) ?
                 " and Flush":"" );
  return rc;
}

/*********************************************************************
* @purpose  Debug function to display reconciliation status
*
* @param
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void dot1sNsfDebugReconStatus()
{
  L7_uint32 intIfNum;
  dot1s_intf_recon_data_t *port_recon;
  L7_BOOL intfDisplay;
  L7_RC_t rc;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1S Application state %s \n",
                dot1sNsfReconStateGet(dot1sAppState));
  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    port_recon = dot1sIntfReconDataFind(intIfNum);
    if (port_recon == L7_NULLPTR)
    {
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      continue;
    }
    intfDisplay = L7_FALSE;
    if (port_recon->pduExpected)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"PDU expected ");
      intfDisplay = L7_TRUE;
    }
    if (port_recon->intfReconciled != L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"Intf not reconciled ");
      intfDisplay = L7_TRUE;
    }
    if (port_recon->intReconState != DOT1S_READY)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"Intf Recon State %s ",
             dot1sNsfReconStateGet(port_recon->intReconState));
      intfDisplay = L7_TRUE;
    }

    if (intfDisplay == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "for intIfNum %d \n", intIfNum);
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  if (dot1sAppState == DOT1S_READY)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"Activate time T+ %d ms\n",
                (dot1sStartupTime.activateDone - dot1sStartupTime.phase3Begin));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"Ext recon time T+ %d ms\n",
                (dot1sStartupTime.extRecon - dot1sStartupTime.phase3Begin));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"Recon complete T+ %d ms\n",
               (dot1sStartupTime.reconComplete - dot1sStartupTime.phase3Begin));
  }

}

