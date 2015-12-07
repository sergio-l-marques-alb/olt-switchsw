/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename dot1s_ih.c
*
* @purpose Multiple Spanning tree Interface Handler
*
* @component dot1s
*
* @comments none
*
* @create 10/14/2002
*
* @author skalyanam
*
* @end
*             
**********************************************************************/

#include "dot1s_include.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "l7_ip_api.h"
#include "dot1s_nsf.h"
#include "osapi_trace.h"

static L7_ushort16 VIDList[L7_MAX_VLANS + 1];
extern dot1sDeregister_t dot1sDeregister;
extern dot1sCnfgrState_t dot1sCnfgrState;
extern dot1sCfg_t *dot1sCfg;
/* dot1s/driver async task parameters */
extern dot1sStateCirBuf_t *dot1sCB;
extern void * dot1sStateSetSema;
extern osapiTimerDescr_t        *dot1sStateSetTimer;
static L7_uint32 dot1sCBHead = 0;
static L7_uint32 dot1sCBTail = 0;

extern L7_INTF_MASK_t portCommonChangeMask;
extern L7_INTF_MASK_t *portInstInfoChangeMask;
static L7_uint32  dot1s_num_port_events_pending = 0;
static void * port_event_mutex = 0;
extern void *dot1sTaskSyncSema;

/*********************************************************************
* @purpose  Return the number of port events issued by dot1s 
*           that are pending in the system.
*
* @param    none
*
* @returns  none
* 
* @comments none
*       
* @end
*********************************************************************/
L7_uint32 dot1sIhNimEventPendingGet (void)
{
  return dot1s_num_port_events_pending;
}

/*********************************************************************
* @purpose  Callback from NIM indicating that port event processing 
*           is done.
*
* @param    none
*
* @returns  none
* 
* @comments none
*       
* @end
*********************************************************************/
static void dot1sIhNimCallback (NIM_NOTIFY_CB_INFO_t retVal)
{
  osapiSemaTake (port_event_mutex, L7_WAIT_FOREVER);
  dot1s_num_port_events_pending--;
  osapiSemaGive (port_event_mutex);
}


/*********************************************************************
* @purpose  Set the state of the port for display in usmdb
*
* @param    none
*
* @returns  none
* 
* @comments none
*       
* @end
*********************************************************************/
static void dot1sPortStateDisplaySet(DOT1S_PORT_COMMON_t *p, L7_uint32 state)
{
  L7_uint32 instIndex;
  L7_INTF_MASK_t intfMask;

  memset(&intfMask, 0, sizeof(intfMask));
  for (instIndex=0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    p->portInstInfo[instIndex].portState = state;
    L7_INTF_SETMASKBIT(intfMask, p->portNum);
  }
  if (dot1sNsfFuncTable.dot1sCallCheckpointService)
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,"%s: Calling ckpt ",
                     __FUNCTION__);
    dot1sNsfFuncTable.dot1sCallCheckpointService(&intfMask,instIndex);
  }
}



L7_RC_t dot1sPortDisabledAction(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32 instIndex;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_MISC, "IntIfNum %u: Applying action - dot1sBridge->Mode=%u p->notParticipating=%u",
           p->portNum, dot1sBridge->Mode, p->notParticipating);

  if (dot1sBridge->Mode == L7_ENABLE)
  {
    if (p->notParticipating == L7_TRUE)
    {
      /* notParticipating */
      /* set port state to disabled */
      for (instIndex=0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
      { 
         dot1sFlush(p->portNum, instIndex);
      }
      dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_DISABLED);                      
    }
    else                                                            
    {
      /* port is Participating (ie; not notParticipating) */
      rc = dot1sSwitchPortDisableSet(p->portNum);
    }
  }
  else /* MSTP is Disabled */
  {
    dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_DISABLED);
    dot1sFlush(p->portNum, L7_DOT1S_MSTID_ALL);
  }         

  return rc;
}

L7_RC_t dot1sPortEnabledAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_MISC, "IntIfNum %u: Applying action - dot1sBridge->Mode=%u p->notParticipating=%u",
           p->portNum, dot1sBridge->Mode, p->notParticipating);

  /* is MSTP Enabled */
  if (dot1sBridge->Mode == L7_ENABLE)
  {
    if (p->notParticipating == L7_TRUE)
    {
      /* notParticipating */
      /* set port state to manual forwarding */
      dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_MANUAL_FWD);
    }
    else                                                            
    {
      /* port is Participating (ie; not notParticipating) */
      /* set port state to discarding */
      rc = dot1sSwitchPortEnableSet(p->portNum);
    }
  }
  else /* MSTP is Disabled */
  {

    dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_MANUAL_FWD);
    dot1sPortStateDisplaySet(p, L7_DOT1S_MANUAL_FWD);
  }
  return rc;
}
/*********************************************************************
* @purpose  Notifies the system of a particular event regarding an 
* @purpose  interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    intIfEvent @b{(input)} one of L7_PORT_EVENTS_t
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* 
* @comments this notification is sent to NIM which in turn passes it 
*           to all the registered routines
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhNotifySystem(L7_uint32 intIfNum, L7_uint32 intIfEvent)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_HANDLE_t           handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_NIM,"Notify system intf %d Event %s", 
                  intIfNum, nimGetIntfEvent(intIfEvent)); 

  if (port_event_mutex == 0)
  {
    port_event_mutex = osapiSemaMCreate (OSAPI_SEM_Q_FIFO);
  }

  osapiSemaTake (port_event_mutex, L7_WAIT_FOREVER);
  dot1s_num_port_events_pending++;
  osapiSemaGive (port_event_mutex);

  eventInfo.component     = L7_DOT1S_COMPONENT_ID;
  eventInfo.pCbFunc       = dot1sIhNimCallback;
  eventInfo.event         = intIfEvent;
  eventInfo.intIfNum      = intIfNum;

  rc = nimEventIntfNotify(eventInfo,&handle);

  return rc;
}

/*********************************************************************
* @purpose  Handles events generated by NIM
*
* @param    intIfNum @b{(input)} interface number
* @param    intIfEvent @b{(input)} interface event 
* @param    @b{(output)}
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 intIfEvent,NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_EVENT_COMPLETE_INFO_t status;    

  status.intIfNum     = intIfNum;
  status.component    = L7_DOT1S_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.response.reason = NIM_ERR_RC_UNUSED;
  status.event        = intIfEvent;
  status.correlator   = correlator;

  if (dot1sDeregister.dot1sIntfChangeCallback == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sIntfCallback is deregistered\n");
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }
  if (!(DOT1S_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Received a interface callback while outside the EXECUTE state");
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }

  rc = dot1sIssueCmd(dot1sIntfChange, intIfNum, L7_NULL, /*&intIfEvent*/&status);

  return rc;
}

/*********************************************************************
* @purpose  Handles startup notifications generated by NIM
*
* @param    startup_phase @b{(input)} The create or Activate Phase
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
void dot1sIntfStartupCallBack(NIM_STARTUP_PHASE_t startup_phase)
{

  if (dot1sDeregister.dot1sIntfChangeCallback == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sIntfCallback is deregistered\n");
    return ;
  }

  if (!(DOT1S_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Received a interface callback while outside the EXECUTE state");
    return;
    
  }
  dot1sIssueCmd(dot1sIntfStartup, L7_NULL, L7_NULL, &startup_phase);

  return;
}
/*********************************************************************
* @purpose  Process Link state changes   
*
* @param    intIfnum @b{(input)} interface number
* @param    intIfEvent @b{(input)} interface event
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_uint32 dot1sIhProcessIntfChange(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status)
{
  L7_uint32 currAcquired, prevAcquired;
  L7_uint32 adminState, instIndex;
  L7_RC_t rc;
#ifdef OBSELETE
  L7_INTF_TYPES_t type;
#endif
  L7_uint32 intIfEvent;
  L7_uint32 prevLagAcquired, prevIpMapAcquired;
  DOT1S_PORT_COMMON_t *p;

  if (dot1sIsValidIntf(intIfNum) == L7_FALSE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  intIfEvent = status.event;

  PT_LOG_TRACE(LOG_CTX_MISC, "Interface change detected for intIfNum %u: event=%u", intIfNum, intIfEvent);

  if (nimCheckIfNumber(intIfNum) == L7_SUCCESS)
  {
    rc = nimGetIntfAdminState(intIfNum, &adminState);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Invalid intIfNum");
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  p = dot1sIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID, "Cannot find dot1s Interface handle for interface(%s) \n", ifName);
    rc = L7_FAILURE;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_MISC, "IntIfNum %u: Event %u - adminState=%u, portEnabled=%u portLinkState=%u ignoreLinkStateChanges=%u", 
           intIfNum, intIfEvent, adminState, p->portEnabled, p->portLinkState, p->ignoreLinkStateChanges);

  switch (intIfEvent)
  {
    case L7_CREATE:
      rc = dot1sIntfCreate(intIfNum);
      if (rc != L7_SUCCESS)
      {
          p->notParticipating = L7_TRUE;
      }
      break;

    case (L7_DELETE):
      rc = dot1sIntfDelete(intIfNum);
      break;

    case (L7_ATTACH):
      dot1sApplyIntfConfigData(intIfNum);
      break;

    case (L7_UP):
      /* if port already known as up then nothing to do */
      if (p->portLinkState != L7_UP)
      {
        p->portLinkState = L7_UP;
        if (p->portEnabled == L7_TRUE)
        {
          p->portUpEnabledNum = p->portNum;
        }

        if (adminState == L7_ENABLE)
        {
          /* are link transition being ignored */
          if (p->ignoreLinkStateChanges == L7_FALSE)
          { 
            dot1sPortEnabledAction(p);
          }/*end if (dot1sPort[intIfNum].....*/
        }/*end if (adminState == L7_ENABLE)*/
      }/*end if (dot1sPort[intIfNum].portLinkState != L7_UP)*/
      break;

  case(L7_PORT_ENABLE):
      /* Clear rate Limit counters */
      p->TickerCount = 0;
      p->currSnapshotTicker = 0;
      memset(&p->currRxCount[0], 0, (sizeof(L7_uint32)*RATE_DETECT_BUCKET_SIZE));

      /* If the port is already enabled do not do anything */
      if (p->portNum == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "%s (%d) portnum %d \n", __FUNCTION__, __LINE__, p->portNum);
        break;
      }
      p->diagnosticDisable = L7_FALSE;
      L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);
      
      if (p->portEnabled != L7_TRUE)
      {
        if (p->portLinkState == L7_UP)
        {
          /* are link transition being ignored */
          if (p->ignoreLinkStateChanges == L7_FALSE)
          { 
            dot1sPortEnabledAction(p);                                                                 
          }/*end if (dot1sPort[intIfNum]....*/
        }/*end if (dot1sPort[intIfNum].portLinkState == L7_UP)*/
      }/*end if (dot1sPort[intIfNum].portEnabled != L7_TRUE) */
      break;
    
    case (L7_DETACH):
      dot1sUnApplyIntfConfigData(intIfNum);
      /* Fall thru*/
    case (L7_DOWN):
      /* if port already known as down then nothing to do */
      if (p->portLinkState == L7_UP)
      {
        p->portLinkState = L7_DOWN;
        p->portUpEnabledNum = 0;

        if (adminState == L7_ENABLE || p->portEnabled == L7_TRUE)
        {
          /* are link transition being ignored */
          if (p->ignoreLinkStateChanges == L7_FALSE)
          { 
            dot1sPortDisabledAction(p);
            /* is MSTP Enabled */
            if (dot1sBridge->Mode != L7_ENABLE)
            {
              p->lastNotificationSent = L7_LAST_PORT_EVENT;
            }                                                                 
          }/*end if (dot1sPort[intIfNum]....*/
        }/*end if (adminState == L7_ENABLE)*/
      }/*end if (dot1sPort[intIfNum].portLinkState == L7_UP)*/
      break; 

    case(L7_PORT_DISABLE):
      /* if port already known as down then nothing to do */
      if (p->portLinkState == L7_UP)
      {
        dot1sPortDisabledAction(p);
      }/*end if (dot1sPort[intIfNum].portLinkState == L7_UP)*/
      break;

    case(L7_LAG_ACQUIRE):
      
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(p->acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* If the interface was not previously acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree 
       */
      if (prevAcquired == L7_FALSE)
      {
        dot1sIhAcquire(p);
      }
      break;
    
    case(L7_LAG_RELEASE):
    /* Was the bit set in the first place. Proceed with the IhRelease only
       * if it was previously acquired and now is getting released
       */
    prevLagAcquired = COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_DOT3AD_COMPONENT_ID);

    if (prevLagAcquired != L7_FALSE)
    {
    
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(p->acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree 
       */
    
      if (currAcquired == L7_FALSE)
      {
        dot1sIhRelease(p);
      }
    }

      break;

    case(L7_PORT_ROUTING_ENABLED):
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, prevAcquired);
      /* check if this interface has been previously acquired by ip map component*/
      prevIpMapAcquired =  COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_IP_MAP_COMPONENT_ID);
      /* if prevIpMapAcquired is true then do not do anything thus preventing 
       * unneccessary events to be generated
       */
      if (prevIpMapAcquired == 0)
      {
        /* update aquired list */
        COMPONENT_ACQ_SETMASKBIT(p->acquiredList, L7_IP_MAP_COMPONENT_ID);

        /* If the interface was not previously acquired by some component, cause
         * the acquisition to be reflected in the state of the spanning tree 
         */
        if (prevAcquired == L7_FALSE)
        {
          dot1sIhAcquire(p);
        }
        
      }
      break;

    case(L7_PORT_ROUTING_DISABLED):
      /* make sure that ip map had this interface previously acquired */
      prevIpMapAcquired = 1;
      prevIpMapAcquired =  COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_IP_MAP_COMPONENT_ID);

      if (prevIpMapAcquired != 0)
      {
      
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(p->acquiredList, L7_IP_MAP_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree 
       */
      if (currAcquired == L7_FALSE)
      {
        dot1sIhRelease(p);
      }
      }
      break;

    case(L7_PROBE_SETUP):
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(p->acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* If the interface is not previously acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree.
       */
      if (prevAcquired == L7_FALSE)
      {
        dot1sIhAcquire(p);
      }

      break;

    case(L7_PROBE_TEARDOWN):
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(p->acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(p->acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree.
       */
      if (currAcquired == L7_FALSE)
      {
        dot1sIhRelease(p);
      }
      break;

    case(L7_SPEED_CHANGE):

      /* Port Path Cost by default is linked to the link speed */
      rc = dot1sCalcPortPathCost(intIfNum);

        for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
        {
          if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
          {
            p->portInstInfo[instIndex].reselect = L7_TRUE;
            p->portInstInfo[instIndex].selected = L7_FALSE;
            /* generate reselect event */
            rc = dot1sStateMachineClassifier(prsReselect, p, instIndex , L7_NULL, L7_NULLPTR);
          }
        }

      break;

    case L7_FORWARDING:
    case L7_NOT_FORWARDING:
      if (intIfEvent != p->lastNotificationSent)
      {
        p->lastNotificationSent = intIfEvent;
      } 
      break;

    default:
      break;
  }/*end switch (intIfEvent)*/                      

  status.response.rc = L7_SUCCESS;
  nimEventStatusCallback(status);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process startup notifications generated by NIM
*
* @param    startup_phase @b{(input)} The create or Activate Phase
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sProcessIntfStartupCallBack(NIM_STARTUP_PHASE_t startup_phase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  PORTEVENT_MASK_t nimEventMask;
  DOT1S_PORT_COMMON_t *p;
  L7_uint64 startTime, endTime;

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_NIM,"Startup callback phase %s\n", 
                  (startup_phase == NIM_INTERFACE_CREATE_STARTUP) ? 
                  "CREATE" : "ACTIVATE");

  startTime = osapiTimeMillisecondsGet64();
  switch (startup_phase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:
      rc = nimFirstValidIntfNumber(&intIfNum);
      
      while (rc == L7_SUCCESS)
      {
        p = dot1sIntfFind(intIfNum);
        if (p != L7_NULLPTR)
        {
          if (dot1sIntfCreate(intIfNum) != L7_SUCCESS)
          {
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

              L7_LOGF(L7_LOG_SEVERITY_ERROR,L7_DOT1S_COMPONENT_ID, 
                      "Error in creating Intf %s ", ifName);
              p->notParticipating = L7_TRUE;
          }
        }
        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }
      memset(&nimEventMask, 0, sizeof(PORTEVENT_MASK_t));
      PORTEVENT_SETMASKBIT(nimEventMask, L7_CREATE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_DELETE);
      nimRegisterIntfEvents(L7_DOT1S_COMPONENT_ID, nimEventMask);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      rc = dot1sIntfActivateStartup();
      
       /* Now ask NIM to send any future changes for these event types */
      memset(&nimEventMask, 0, sizeof(PORTEVENT_MASK_t));
      PORTEVENT_SETMASKBIT(nimEventMask, L7_CREATE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_DELETE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_ATTACH);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_DETACH);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PORT_ENABLE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PORT_DISABLE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_UP);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_DOWN);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_SPEED_CHANGE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_LAG_ACQUIRE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_LAG_RELEASE);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PORT_ROUTING_ENABLED);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PORT_ROUTING_DISABLED);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PROBE_SETUP);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_PROBE_TEARDOWN);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_FORWARDING);
      PORTEVENT_SETMASKBIT(nimEventMask, L7_NOT_FORWARDING);
      nimRegisterIntfEvents(L7_DOT1S_COMPONENT_ID, nimEventMask);
      break;
      
    default:
      break;
  }

  endTime = osapiTimeMillisecondsGet64();
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_NIM,"startup callback done in %d msecs \n", 
                  (endTime - startTime));
  nimStartupEventDone(L7_DOT1S_COMPONENT_ID);
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The specified interface has been "acquired" by another 
* @purpose  component.  
*       
* @param    intIfnum @b{(input)} port changing state
* @param    @b{(output)}
*
* @returns  
*
* @comments This routine should be called only if the interface is 
*           not already acquired by a component.
*           Calling this routine will cause the internal port mode 
*           to be reflected as Disabled. This internal port mode
*           has no reflection on the operational state of the port. 
*           Users wishing to read the operational state of the port 
*           must obtain this information from NIM, not Spanning Tree.
*
* @end
*********************************************************************/
void dot1sIhAcquire(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc;
  L7_uint32 linkState;

  /* Don't react to link state changes in LAG member ports. */
  if (COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_DOT3AD_COMPONENT_ID))
    p->ignoreLinkStateChanges = L7_TRUE;


  dot1sUnApplyIntfConfigData(p->portNum);

    /* is MSTP Enabled */
  if (dot1sBridge->Mode == L7_ENABLE)
  {                                                                       
    /* Tell MSTP */
    rc = dot1sSwitchPortDisableSet(p->portNum);
  }
  else /* MSTP is disabled */
  {
    /*If link is up, move the state to forwarding*/
    rc = nimGetIntfLinkState(p->portNum, &linkState);
    if (linkState == L7_UP)
    {
      if (p->ignoreLinkStateChanges != L7_TRUE)
      {
        /* set port state to manual forwarding */
        dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, 
                            L7_DOT1S_MANUAL_FWD);
      }
      dot1sPortStateDisplaySet(p, L7_DOT1S_MANUAL_FWD);
    }
  }

  p->notParticipating = L7_TRUE;
  /* disable flooding on the port */
  dot1sCommonPortBpduFloodModeSet(p->portNum,L7_NULL,L7_FALSE);
}

/*********************************************************************
* @purpose  The specified interface has been "released" by another component.  
*           Its acquisition means that it should no longer participate in Spanning 
*           Tree.
*       
* @param    intIfnum @b{(input)} port changing state
*
* @returns  
*
* @comments This routine should be called only if the the last acquiring 
*           component has released this interface.
*
* @end
*********************************************************************/
void dot1sIhRelease(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc;
  DOT1S_PORT_COMMON_CFG_t *pCfg;
  L7_uint32 adminState;
  
  /* This interface has been "released" by another component.  Its 
   * release means that it **may** be eligible to participate in 
   * Spanning Tree. It is eligible as long as no other component is 
   * also holding a "lock" on the interface. It is the responsibility 
   * of the calling routine to ensure that no other component holds a
   * lock on the interface.
   */

  if (p->portAdminMode == L7_ENABLE)
  {
    p->notParticipating = L7_FALSE;
  }
  else
  {
    p->notParticipating = L7_TRUE;
  }

  /* Do not ignore the link state changes of the port now that it 
       * is no longer acquired.
       */
  if (!COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_DOT3AD_COMPONENT_ID))
    p->ignoreLinkStateChanges = L7_FALSE;

  dot1sApplyIntfConfigData(p->portNum);

  /* Reflect the port state */
  if (p->portLinkState == L7_UP)
  {
    /* is MSTP Enabled */
    if (dot1sBridge->Mode == L7_ENABLE)
    {
      if (p->notParticipating == L7_TRUE)
      {
        /* notParticipating */
        /* set port state to manual forwarding */
        dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_MANUAL_FWD);
      }
      else                                                            
      {
        /* port is Participating (ie; not notParticipating) */
        /* set port state to discarding */
        rc = nimGetIntfAdminState(p->portNum, &adminState);
        if (rc == L7_SUCCESS && adminState == L7_ENABLE)
        {
        rc = dot1sSwitchPortEnableSet(p->portNum);
      }
      }
    }
    else /* MSTP is Disabled */
    {
      dot1sPortStateDisplaySet(p, L7_DOT1S_MANUAL_FWD);

    }                                                                 
  }/*end if (dot1sPort[intIfNum].portLinkState == L7_UP)*/
  else
  {
    if (p->portLinkState == L7_DOWN)
    {
      /* is MSTP Enabled */
      if (dot1sBridge->Mode == L7_ENABLE)
      {
        if (p->notParticipating == L7_TRUE)
        {
          /* notParticipating */
          /* set port state to disabled */
          dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, p->portNum, L7_DOT1S_DISABLED);
        }
        else                                                            
        {
          /* port is Participating (ie; not notParticipating) */
          rc = dot1sSwitchPortDisableSet(p->portNum);
        }
      }
      else /* MSTP is Disabled */
      {
        dot1sPortStateDisplaySet(p, L7_DOT1S_DISABLED);
      }                                                                 
    }/*end if (dot1sPort[intIfNum].portLinkState == L7_DOWN)*/
  }/*end else if (dot1sPort[intIfNum].portLinkState == L7_UP)*/

  if (dot1sMapIntfIsConfigurable(p->portNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->bpduFloodMode != L7_DOT1S_BPDUFLOOD_ACTION_AUTO)
    {
      dot1sCommonPortBpduFloodModeSet(p->portNum,L7_NULL,pCfg->bpduFloodMode);
    }
    else if (pCfg->bpduFloodMode == L7_DOT1S_BPDUFLOOD_ACTION_AUTO)
    {
      dot1sCommonPortBpduFloodModeSet(p->portNum,L7_NULL,!(dot1sBridge->Mode));
    }
  }
}

/*********************************************************************
* @purpose  Go through registered Dot1s users and notify them of 
*           interface changes in a specific instance.
*
* @param    mstID @b{(input)} MSTP instance
* @param    intIfNum @b{(input)} interface number
* @param    intIfEvent @b{(input)} interface event
* @param    @b{(output)}
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
void dot1sIhDoNotify(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 intIfEvent)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_BOOL systemNotify;

  switch (intIfEvent)
  {
    case   L7_DOT1S_EVENT_FORWARDING:
    case L7_DOT1S_EVENT_NOT_FORWARDING:
      /* This is already taken care of now when calling
         So any call that comes with FWD/NOT FWD let it through
      */
      systemNotify = L7_TRUE;
      break;
    case L7_DOT1S_EVENT_INSTANCE_CREATE:
    case L7_DOT1S_EVENT_INSTANCE_DELETE:
      /* User configuration - notify rest of system*/
      systemNotify = L7_TRUE;
      break;
    case L7_DOT1S_EVENT_TOPOLOGY_CHANGE:
      /* We need to wait till the system is completely ready*/
      systemNotify = dot1sCnfgrAppIsReady();
      break;
    default:
      systemNotify = L7_TRUE;
      break;
     
  }

  if (systemNotify == L7_TRUE)
  {
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (dot1sNotifyList[i].registrar_ID)
    {
      rc = (*dot1sNotifyList[i].notify_intf_change)(mstID, intIfNum, intIfEvent);
    }
  }
}
}

/*********************************************************************
* @purpose  Go through registered Dot1s users and notify them of an 
*           interface change in every valid instance.
*
* @param    intIfNum @b{(input)} interface number
* @param    intIfEvent @b{(input)} interface event
* @param    @b{(output)}
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
void dot1sIhDoNotifyAllInstances(L7_uint32 intIfNum, L7_uint32 intIfEvent)
{
  L7_uint32 inst, instNumber;
  L7_RC_t rc;

  for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES; inst++)
  {
    if ((rc = dot1sInstNumFind(inst, &instNumber)) == L7_SUCCESS)
    {
      dot1sIhDoNotify(instNumber, intIfNum, intIfEvent);
    }
  }
}

/*********************************************************************
* @purpose  Gets the dataRate for a particular interface
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    dataRate @b{(output)} pointer to the dataRate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfSpeedDataRateGet(L7_uint32 intIfNum, L7_uint32 *dataRate)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfSpeedDataRate(intIfNum, dataRate);

  return rc;
}

/*********************************************************************
* @purpose  Checks if an interface is full duplex
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    @b{(output)}
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments    
*       
* @end
*********************************************************************/
L7_BOOL dot1sIhIsIntfSpeedFullDuplex(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;
  
  rc = nimIsIntfSpeedFullDuplex(intIfNum);

  return rc;
}

/*********************************************************************
* @purpose  Gets the link speed of the interface 
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    speed @b{(output)} pointer to speed as in L7_PORT_SPEEDS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfSpeedGet(L7_uint32 intIfNum, L7_uint32 *speed)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfSpeed(intIfNum, speed);

  return rc;
}

/*********************************************************************
* @purpose  Gets the link state of the interface
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    state @b{(output)} L7_UP or L7_DOWN
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfLinkStateGet(L7_uint32 intIfNum, L7_uint32 *state)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfLinkState(intIfNum, state);

  return rc;
}

/*********************************************************************
* @purpose  Gets the MAC Address associated with an interface
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    addrType @b{(input)} address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured 
*                       MAC Address
* @param    macAddr @b{(output)} pointer to mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfAddrGet(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfAddress(intIfNum, addrType, macAddr);

  return rc;
}

/*********************************************************************
* @purpose  Retrieves the unit slot and port of a given interface
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    usp @b{(output)} pointer to the usp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfUspGet(L7_uint32 intIfNum, DOT1S_USP_t *usp)
{
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t nimUsp;

  rc = nimGetUnitSlotPort(intIfNum, &nimUsp);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  usp->unit = nimUsp.unit;
  usp->slot = nimUsp.slot;
  usp->port = nimUsp.port;

  return rc;
}

/*********************************************************************
* @purpose  Retrieves auto negotiation status of a given interface
*           
* @param    intIfNum @b{(input)} internal interface number
* @param    autoNegoStatus @b{(output)} pointer to the status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhIntfAutoNegStatusGet(L7_uint32 intIfNum, L7_uint32 *autoNegoStatus)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfAutoNegAdminStatus(intIfNum, autoNegoStatus);

  return rc;
}

/*********************************************************************
* @purpose  return the highest assigned interface number 
*          
* @param    @b{(input)}
* @param    highestIntfNumber @b{(output)} pointer to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIhHighestIntfNumGet(L7_uint32 *highestIntfNumber)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetHighestIntfNumber(highestIntfNumber);

  return rc;
}

/*********************************************************************
* @purpose  process the activate callback from NIM 
*          
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfActivateStartup()
{
  L7_RC_t rc;
  L7_uint32 intIfNum;


  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_STATUS,
                  "DOT1S Reconciliation IF ACTIVATE begin"); 
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_NIM,"Nim activate startup restart type %s\n", 
                  (dot1sIsRestartTypeWarm() == L7_TRUE) ? 
                  "WARM" : "COLD");
  OSAPI_TRACE_EVENT(L7_TRACE_EVENT_DOT1S_STARTUP_ACTIVATE_START, L7_NULLPTR, 0);

  dot1sAppState = DOT1S_NSF_SO_BEGIN;

  if ((dot1sIsRestartTypeWarm() == L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sNsfActivateStartupBegin))
  {
    dot1sNsfFuncTable.dot1sNsfActivateStartupBegin();
  }


  rc = nimFirstValidIntfNumber(&intIfNum);
      
  while (rc == L7_SUCCESS)
  {

    if (dot1sIsValidIntf(intIfNum) == L7_TRUE)
    {
      dot1sIntfActivate(intIfNum);
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }


  dot1sAppState = (dot1sIsRestartTypeWarm() == L7_TRUE) ? 
         DOT1S_NSF_SO_IF_ACT : DOT1S_READY;

  dot1sStartupTime.activateDone = osapiTimeMillisecondsGet64();
  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE_STATUS,
                  "DOT1S Reconciliation IF ACTIVATE complete"); 
  OSAPI_TRACE_EVENT(L7_TRACE_EVENT_DOT1S_STARTUP_ACTIVATE_END, L7_NULLPTR, 0);

  
  if (dot1sNsfFuncTable.dot1sCheckGlobalReconciled)
  {
    dot1sNsfFuncTable.dot1sCheckGlobalReconciled(!(dot1sIsRestartTypeWarm()));
  }
  else
  {
    cnfgrApiComponentHwUpdateDone(L7_DOT1S_COMPONENT_ID, 
                                  L7_CNFGR_HW_APPLY_CONFIG);
    dot1sAppState = DOT1S_READY;
  }
  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  process the cold start of the interface 
*          
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfActivate(L7_uint32 intIfNum)
{
  L7_uint32 adminState;
  L7_BOOL probePort, routingEnabled = L7_FALSE, lagEnabled;
  L7_RC_t rc= L7_SUCCESS;
  DOT1S_PORT_COMMON_t *p = L7_NULLPTR;
  L7_COMPONENT_IDS_t acqCompId = L7_FIRST_COMPONENT_ID;
  L7_uint32 linkState;
  L7_INTF_STATES_t intIfState;

  do
  {

    intIfState = nimGetIntfState(intIfNum);
    if (intIfState == L7_INTF_ATTACHED)
    {
      dot1sApplyIntfConfigData(intIfNum);
    }

    /* Gather information about the state of the interface*/
    rc = nimGetIntfLinkState(intIfNum,&linkState);
    if (rc != L7_SUCCESS)
    {
      break;
    }
    rc = nimGetIntfAdminState(intIfNum, &adminState);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    /* Check if the interface is acquired */
    probePort = mirrorIsActiveProbePort(intIfNum);
    if (probePort)
    {
      acqCompId = L7_PORT_MIRROR_COMPONENT_ID;
    }
    else
    {
      lagEnabled = dot3adIsLagActiveMember(intIfNum);
      if (lagEnabled)
      {
        acqCompId = L7_DOT3AD_COMPONENT_ID;
      }
      else
      {
        rc  = ipMapRtrIntfModeGet(intIfNum, &routingEnabled);
        if (rc == L7_SUCCESS)
        {
          if (routingEnabled)
          {
            acqCompId = L7_IP_MAP_COMPONENT_ID;
          }
        }

      }  /* not LagEnabled */
    }/* not probe port*/

    if (!dot1sIsValidIntf(intIfNum))
    {
      /* 
       * If this is warm restart and the port is up let the system know 
       * that the port is forwarding. For cold restarts NIM will issue this
       * 
       */
      if ((dot1sIsRestartTypeWarm() == L7_TRUE) &&
          (linkState == L7_UP) && (adminState == L7_ENABLE))
      {
        if (dot1sNsfFuncTable.dot1sUpdateIntfReconciled)
        {
          dot1sNsfFuncTable.dot1sUpdateIntfReconciled(intIfNum);
        }
        nimNotifyIntfChange(intIfNum, L7_FORWARDING);
      }
    }


    p = dot1sIntfFind(intIfNum);
    if (p == L7_NULLPTR)
    {
      break;
    }
    p->portLinkState = linkState;
    /* Act on the information */
    if (acqCompId != L7_FIRST_COMPONENT_ID)
    {
      if (dot1sNsfFuncTable.dot1sUpdateIntfReconciled)
      {
        dot1sNsfFuncTable.dot1sUpdateIntfReconciled(intIfNum);
      }
      COMPONENT_ACQ_SETMASKBIT(p->acquiredList, acqCompId);
      dot1sIhAcquire(p);
    }
    else
    {
      p->ignoreLinkStateChanges = L7_FALSE;
      if ((p->portLinkState == L7_UP) && (adminState == L7_ENABLE))
      {
       /* Initialize the port if this is cold restart or if the port 
        * is not meant to run spanning tree
       */ 
        if ((dot1sIsRestartTypeWarm() != L7_TRUE) ||
            (dot1sBridge->Mode != L7_ENABLE) ||
            (p->notParticipating == L7_TRUE))
        {
          if (dot1sNsfFuncTable.dot1sUpdateIntfReconciled)
          {
            dot1sNsfFuncTable.dot1sUpdateIntfReconciled(intIfNum);
          }
          rc = dot1sPortEnabledAction(p);
        }
        else
        {
          if (dot1sNsfFuncTable.dot1sIntfReconcile)
          {
            dot1sNsfFuncTable.dot1sIntfReconcile(p);
          }

        }

      }
      else
      {
        if (dot1sNsfFuncTable.dot1sUpdateIntfReconciled)
        {
          dot1sNsfFuncTable.dot1sUpdateIntfReconciled(intIfNum);
        }
  
        if ((dot1sBridge->Mode == L7_ENABLE) &&
            (p->notParticipating != L7_TRUE))
        {
          rc = dot1sPortDisabledAction(p);
        }
      }
    }

    rc = dot1sCalcPortPathCost(intIfNum);
  
    
  }

  while (L7_FALSE);

  return rc;
}


/*********************************************************************
* @purpose  Set the dot1s forwarding state of an interface in all
*           instances.
*
* @param    intIfNum @b{(input)} the interface number
* @param    state @b{(input)} the dot1s State
* @param    @b{(output)}
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
void dot1sIhPortStateAllSet(L7_uint32 intIfNum, L7_uint32 state)
{
  L7_uint32 inst, instNumber;
  L7_RC_t rc;


  for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES; inst++)
  {
    if ((rc = dot1sInstNumFind(inst, &instNumber)) == L7_SUCCESS)
    {
      dot1sIhStateSet(instNumber, intIfNum, state);
    }
  }
}
/*********************************************************************
* @purpose  Causes the switch to change state
*
* @param    mstID    @b((input)) user assigned instance number  
* @param    intIfNum @b{(input)} interface to change the state of
* @param    state    @b{(input)} state to change to
*
* @returns  
*
* @comments     
*       
* @end
*********************************************************************/
void dot1sIhSetPortState(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 state)
{
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_uint32 instIndex = 0;

  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
          "nimIntfQuery failed for intf %s ", ifName);
    return;/* should never get here*/
  }
  /* only talk to the hardware when the hardware is valid */
  if ((nimQueryData.data.state != L7_INTF_ATTACHING) &&
      (nimQueryData.data.state != L7_INTF_DETACHING) &&
      (nimQueryData.data.state != L7_INTF_ATTACHED))
  {
    return;
  }

  (void )dot1sInstIndexFind(mstID,&instIndex); 
  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_CHANGE, instIndex))
  {
    DOT1S_DEBUG_MSG("Setting Port(%d) instance(%d) to %s at %lld\n", 
       intIfNum, instIndex,dot1sStateStringGet(state),
                      osapiTimeMillisecondsGet64()); 
  }
  if (state != L7_DOT1S_DISABLED)
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "State machine setting Port(%d) instance(%d) to %s", 
                   intIfNum, instIndex, dot1sStateStringGet(state));
  }

  switch (state)
  {
  case L7_DOT1S_DISABLED:

    if (mstID == L7_DOT1S_MSTID_ALL)
    {
      /* This state change is applicable to all instances.
       */
      dot1sIhPortStateAllSet(intIfNum, state);
    }
    else
    {
      dot1sIhStateSet(mstID, intIfNum, state);
    }

    break;

  case L7_DOT1S_LEARNING:
    /* Set to Learning for this instance only.
     */
    dot1sIhStateSet(mstID, intIfNum, state);
    break;

  case L7_DOT1S_FORWARDING:
    /* Set to Forwarding for this instance only.
     */
    dot1sIhStateSet(mstID, intIfNum, state);
    break;

  case L7_DOT1S_DISCARDING:

    if (mstID == L7_DOT1S_MSTID_ALL)
    {
      /* This state change is applicable to all instances.
       */
      dot1sIhPortStateAllSet(intIfNum, state);
    }
    else
    {
      /* This state change is only applicable to one instance.
       */
      dot1sIhStateSet(mstID, intIfNum, state);
    }
    break;

  case L7_DOT1S_MANUAL_FWD:
    /* forwarding when spanning tree is off for the port */
    /* This state change is applicable to all instances.
     */
    if (mstID == L7_DOT1S_MSTID_ALL)
    {
      dot1sIhPortStateAllSet(intIfNum, state);
    }
    else
    {
      /* This state change is only applicable to one instance.
       */
      dot1sIhStateSet(mstID, intIfNum, state);
    }
    break;

  default:
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "\r\ndot1sIhSetPortState: Invalid MSTP state!\n");
    break;
  }
}


/*********************************************************************
* @purpose  Set the port to diagnostically disabled state 
*
* @param    intIfNum @b{(input)} interface to change the state of
*
* @returns  
*
* @comments     
*       
* @end
*********************************************************************/
L7_RC_t dot1sDiagDisablePort(L7_uint32 errIntIfNum)
{
  DOT1S_PORT_COMMON_t *errorPort = dot1sIntfFind(errIntIfNum);
  nimUSP_t usp;

  if (errorPort == L7_NULLPTR)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(errIntIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Cannot find intf(%s) to diagnostically disable", ifName);
    return L7_FAILURE;
  }

  if (errorPort->diagnosticDisable == L7_FALSE)
  {
    (void)nimSetIntfAdminState(errIntIfNum, L7_DIAG_DISABLE);
    
    errorPort->diagnosticDisable = L7_TRUE;

    L7_INTF_SETMASKBIT(portCommonChangeMask, errIntIfNum);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,
                    "%s: Calling ckpt for intf %d ",
                    __FUNCTION__, errIntIfNum);

    (void)nimGetUnitSlotPort(errIntIfNum, &usp);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Diagnostically disabling interface %d/%d/%d\n", 
            usp.unit, usp.slot,usp.port);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Causes the switch to change state by telling DTL 
*
* @param    mstID    @b((input)) user assigned instance number  
* @param    intIfNum @b{(input)} interface to change the state of
* @param    state    @b{(input)} state to change to
*
* @returns  
*
* @comments     
*       
* @end
*********************************************************************/
void dot1sIhStateSet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 state)
{
  L7_uint32 localMPC;
  L7_RC_t rc;
  L7_uint32 instIndex;
  DOT1S_PORT_COMMON_t *p;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  PT_LOG_TRACE(LOG_CTX_MISC, "IntIfNum %u: mstID=%u state=%u", intIfNum, mstID, state);

  p = dot1sIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID, 
          "Cannot find dot1s Interface handle for interface(%s) ", ifName);
    return ;
  }

  rc = dot1sInstIndexFind(mstID,  &instIndex);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Unknown instance %d\n",  mstID);
    return;
  }
  p->portInstInfo[instIndex].stateChangeInProgress++;

  /* conditionally Block here */
  if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)))
  {
    /* Check with reconciliation logic if we need to make a call
       Success notification indicates that everything is in sync 
       we do not need to call the dtl
    */
    if (dot1sNsfFuncTable.dot1sNsfReconEventFunc(intIfNum,instIndex, 
                        dot1s_recon_event_state_change_begin) == L7_SUCCESS)
    {
      dot1sStateSetAction(intIfNum,mstID,state);
      return;
    }
  }

  (void)osapiSemaTake(dot1sStateSetSema,  L7_WAIT_FOREVER);
  if (dot1sStateSetTimer == L7_NULLPTR)
  {
   /* Start a new osapi timer */
   osapiTimerAdd(dot1sStateSetTimerExpired, intIfNum, state, 
                 DOT1S_STATE_SET_TIMEOUT, &dot1sStateSetTimer);
  }
  else
  {
   /*Timer is already running so no need to to do anything*/
  }

  if (dot1sIsStateCBFull() == L7_TRUE)
  {
    /* Too many messages in ther circular buffer 
     * disabling this port and logging a message
     */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
        "dot1sCB is full intf %s, instance ID %d state %d", ifName, mstID, state);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                    "dot1sCB is full intIfNum %d, instance ID %d state %d",
                     intIfNum, mstID, state);

    dot1sDiagDisablePort(intIfNum);
    
    (void)osapiSemaGive(dot1sStateSetSema);
    return;
  }


  
  localMPC = dot1sCBTail;
  dot1sCB[dot1sCBTail].intIfNum = (L7_ushort16)intIfNum;
  dot1sCB[dot1sCBTail].valid = L7_TRUE;
  dot1sCBTail = ((dot1sCBTail + 1) % DOT1S_STATE_SET_CB_SIZE);

  rc = dtlDot1sStateSet(mstID, intIfNum, state, localMPC);
  (void)osapiSemaGive(dot1sStateSetSema);

  if (rc != L7_SUCCESS)
  {
    
    /* We will not attempt to clean up dot1sCBTail as this will involve taking the sema
     * which could lead to curruption of the variable as another task (nim or dot1s 
     * depending on which one has invoked the current call)  hence we will allow the 
     * timer to expire and take the clean up action on the expiry of the timer.
     * we will ofcource diagnostically disable this port as soon as we receive a 
     * failure. Typically this involves an inability to put htis msg on the dapi/hapi Q
     * In the sync case (no Q in the driver) the clean up has already happened as the 
     * callback is invoked before returning from the hapi function
     */

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "dtlDot1sStateSet failed intf %s, instance ID %d state %d",
            ifName, mstID, state);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                      "dtlDot1sStateSet failed intIfNum %d, "
                      "instance ID %d state %d\n", intIfNum, mstID, state);
    dot1sDiagDisablePort(intIfNum);
  }
  
  return;
}
/*********************************************************************
* @purpose  Notify of a dot1s instance create
*
* @param    instNumber @b{(input)} instance number
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhDoNotifyInstanceCreate(L7_uint32 instNumber)
{
  L7_RC_t rc;

  /* Notify Dot1s Registered users */
  dot1sIhDoNotify(instNumber, L7_NULL, L7_DOT1S_EVENT_INSTANCE_CREATE);

  /* Notify DTL */
  rc = dtlDot1sInstanceCreate(instNumber);

  return rc;
}

/*********************************************************************
* @purpose  Notify of a dot1s instance delete
*
* @param    instNumber @b{(input)} instance number
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*       
* @end
*********************************************************************/
L7_RC_t dot1sIhDoNotifyInstanceDelete(L7_uint32 instNumber)
{
  L7_RC_t rc;
  L7_uint32 numVlan;
  L7_uint32 i;

  /* Notify Dot1s Registered users */
  dot1sIhDoNotify(instNumber, L7_NULL, L7_DOT1S_EVENT_INSTANCE_DELETE);

  /* Notify DTL */
  rc = dtlDot1sInstanceDelete(instNumber);

  /* notify DTL of the Vlan list changes for the CIST */
  bzero((char *)&VIDList, (L7_int32)sizeof(VIDList));
  rc = dot1sMstiVIDList(DOT1S_CIST_ID, VIDList, &numVlan);

  for (i = 0; i < numVlan; i++)
  {
    rc = dtlDot1sInstVlanIdAdd(DOT1S_CIST_ID, VIDList[i]);
  }

  return rc;
}



/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL dot1sMapIntfIsConfigurable(L7_uint32 intIfNum, DOT1S_PORT_COMMON_CFG_t **pCfg)
{
  nimConfigID_t configId;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  
  if (!(DOT1S_IS_READY))
    return L7_FALSE;


  /* Check boundary conditions */
  if (portIndex == L7_NULL)
    return L7_FALSE;


  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if ((nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS) &&
      (NIM_CONFIG_ID_IS_EQUAL(&configId, 
         &(dot1sCfg->cfg.dot1sCfgPort[portIndex].configId)) == L7_TRUE))
    {
    *pCfg = &dot1sCfg->cfg.dot1sCfgPort[portIndex];
    return L7_TRUE;
  }


      /* if we get here, either we have a table management error between dot1sCfg and dot1sMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
          "Error accessing DOT1S config data for interface %s  portIndex %d in %s.\n",
          ifName, portIndex, __FUNCTION__);
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dot1sIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  DOT1S_PORT_COMMON_CFG_t *pCfg;
  L7_uint32 portIndex; 

  if (dot1sIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;
  
  portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);
  if (portIndex == L7_NULL)
    return L7_FAILURE;


  pCfg = &dot1sCfg->cfg.dot1sCfgPort[portIndex];
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      dot1sPortDefaultConfigDataBuild (pCfg);

  dot1sIntfCfgToPortCopy(pCfg);

  
  /* apply the configuration */
  dot1sCommonPortBpduFilterModeSet(intIfNum,L7_NULL,pCfg->bpduFilterMode);
  if (pCfg->bpduFloodMode == L7_DOT1S_BPDUFLOOD_ACTION_AUTO)
  {
    dot1sCommonPortBpduFloodModeSet(intIfNum,L7_NULL,!(dot1sBridge->Mode));
  }
  else
  {
    dot1sCommonPortBpduFloodModeSet(intIfNum,L7_NULL,pCfg->bpduFloodMode);
  }



  /* Register with stats manager*/
  (void)dot1sStatsCreate(intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To apply interface properties to h/w, now that h/w is available
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dot1sApplyIntfConfigData(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  DOT1S_PORT_COMMON_t     *pPort = L7_NULLPTR;
  DOT1S_BRIDGE_t *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pPort = dot1sIntfFind(intIfNum);

  if (pPort != L7_NULLPTR)
  {
    /* call the bpdu filter api to set the filter mode */
    if (pPort->adminEdge == L7_TRUE )
    {
      if(bridge->bpduFilter == L7_TRUE ||
         pPort->bpduFilter == L7_TRUE)
      {
        if (dtlDot1sBpduFilterSet(intIfNum,L7_TRUE) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                  "Failed to set BPDU Filter value for %d\n",intIfNum);
          return L7_FAILURE;
        }
      }
  
      if(bridge->bpduGuard == L7_TRUE)
      {
        if (dtlDot1sBpduGuardSet(intIfNum,L7_TRUE) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                  "Failed to set BPDU Guard for %d\n", intIfNum);
          return L7_FAILURE;
        }
      }
    }

    if (dtlDot1sBpduFloodSet(intIfNum, pPort->bpduFlood) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
              "Failed to set BPDU Flood value %d for %d\n", 
              pPort->bpduFlood,intIfNum);
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To remove interface properties to h/w,now that h/w is not available
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The intent here is to put the interface in default state
*
* @end
*
*********************************************************************/
L7_RC_t dot1sUnApplyIntfConfigData(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  DOT1S_PORT_COMMON_t     *pPort = L7_NULLPTR;
  DOT1S_BRIDGE_t *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pPort = dot1sIntfFind(intIfNum);

  if (pPort != L7_NULLPTR)
  {
    /* If the conditions match we must have set the bpdu filter and bpdu guard
       reset that variable.
    */
    if (pPort->adminEdge == L7_TRUE )
    {
      if(bridge->bpduFilter == L7_TRUE ||
         pPort->bpduFilter == L7_TRUE)
      {
        if (dtlDot1sBpduFilterSet(intIfNum,L7_FALSE) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                  "Failed to set BPDU Filter value for %d\n",intIfNum);
          return L7_FAILURE;
        }
      }
  
      if(bridge->bpduGuard == L7_TRUE)
      {
        if (dtlDot1sBpduGuardSet(intIfNum,L7_FALSE) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                  "Failed to set BPDU Guard for %d\n", intIfNum);
          return L7_FAILURE;
        }
      }
    }

    if (dtlDot1sBpduFloodSet(intIfNum, L7_FALSE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
              "Failed to set BPDU Flood value %d for %d\n", 
              pPort->bpduFlood,intIfNum);
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dot1sIntfDelete(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL brc;
  DOT1S_PORT_COMMON_CFG_t *pCfg;
  L7_uint32 portIndex;

  if ((brc = dot1sMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
  {
   (void)dot1sSwitchPortAdminModeDisableSet(intIfNum);
  }

  if (brc == L7_TRUE)
  {

    portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);
    if (portIndex == L7_NULL)
      return L7_FAILURE;

    memset((void *)&pCfg->configId, 0x00, (L7_uint32)sizeof(nimConfigID_t));
      memset((void *)pCfg, 0x00, (L7_uint32)sizeof(DOT1S_PORT_COMMON_CFG_t));
      rc = L7_SUCCESS;
  }

  return rc;
}
/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dot1sIntfDetach(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_CFG_t *pCfg;

  if (dot1sMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /*
    if (snoopIntfModeApply(intIfNum, FD_IGMP_SNOOPING_INTF_MODE) != L7_SUCCESS)
      return L7_FAILURE;
      */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in dot1s
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dot1sIntfCfgToPortCopy(DOT1S_PORT_COMMON_CFG_t *pPortCfg)
{
  DOT1S_PORT_COMMON_t *pPort;
  L7_uint32           intIfNum, instIndex, priInstId;
  L7_RC_t rc;

  if (nimIntIfFromConfigIDGet(&(pPortCfg->configId), &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  pPort = dot1sIntfFind(intIfNum);

  if (pPort == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /* Initialize with the defaults then replace the configured fields
   * with their configured values.
   */
  rc = dot1sPortDefaultPopulate(pPort, intIfNum);

  pPort->portAdminMode = pPortCfg->portAdminMode;

  /* notParticipating must be kept in sync with Admin Mode
   * up until the time of an IhAcquire or IhRelease
   */
  if (pPort->portAdminMode == L7_ENABLE)
  {
    pPort->notParticipating = L7_FALSE;
  }
  else
  {
    pPort->notParticipating = L7_TRUE;
  }

  pPort->portNum = intIfNum;
  pPort->adminEdge = pPortCfg->adminEdge;
  pPort->autoEdge = pPortCfg->autoEdge;
  pPort->restrictedRole = pPortCfg->restrictedRole;
  pPort->loopGuard = pPortCfg->loopGuard;  
  pPort->restrictedTcn = pPortCfg->restrictedTcn;

  for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    pPort->portInstInfo[instIndex].ExternalPortPathCost = pPortCfg->portInstInfo[instIndex].ExternalPortPathCost;
    pPort->portInstInfo[instIndex].InternalPortPathCost = pPortCfg->portInstInfo[instIndex].InternalPortPathCost;
    pPort->portInstInfo[instIndex].autoInternalPortPathCost = pPortCfg->portInstInfo[instIndex].autoInternalPortPathCost;
    pPort->portInstInfo[instIndex].autoExternalPortPathCost = pPortCfg->portInstInfo[instIndex].autoExternalPortPathCost;
    pPort->portInstInfo[instIndex].portId = ((pPortCfg->portInstInfo[instIndex].portPriority << 8) | intIfNum);
    /* msti */
    if (instIndex != DOT1S_CIST_INDEX)
    {
      priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.regRootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId.priInstId = (L7_ushort16)priInstId;
    }
    else /* cist */
    {
      priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

      pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.rootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.regRootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.rootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.regRootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.rootId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
      pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.regRootId.priInstId = (L7_ushort16)priInstId;
    }
  }/*endfor instIndex*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if this port is Forwarding in any MSTP instance other than the one supplied
*           
*          
* @param    intIfNum @b{(input)} interface number
*           instIndex @b{(input)} instance Index
*
* @returns  L7_SUCCESS if forwarding in any instance or Nim Manual Forwarding
* @returns  L7_FAILURE if not forwarding in all instances
*
* @comments Use dot1sMstiPortStateGet if the state of a port in a specific 
*           instance is required. 
*
* @end
*********************************************************************/
L7_BOOL dot1sIsPortForwarding(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_uint32 index, state;
  DOT1S_PORT_COMMON_t *pPort;

  /* Only return port forwarding state information if MSTP is enabled.
   */
  if (dot1sModeGet() == L7_ENABLE)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      for (index = 0; index <= L7_MAX_MULTIPLE_STP_INSTANCES; index++)
      {
        if (index == instIndex)
        {
              continue;
        }

        if (dot1sInstanceMap[index].inUse == L7_TRUE)
        {
          if(pPort->portInstInfo[index].portState == L7_DOT1S_FORWARDING ||
             pPort->portInstInfo[index].portState == L7_DOT1S_MANUAL_FWD)
          {
            return(L7_TRUE);
          }
        }
      }
    }
  }
  else /* MSTP is disabled, return NIM Manual Forwarding status */
  {
    if (nimGetIntfActiveState(intIfNum, &state) == L7_SUCCESS)
    {
      if (state == L7_ACTIVE)
      {
        return(L7_TRUE);
      }
    }
  }

  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Check if this port is Discarding in any MSTP instance other than the one supplied
*           
*          
* @param    intIfNum @b{(input)} interface number
*           instIndex @b{(input)} instance Index
*
* @returns  L7_SUCCESS if forwarding in any instance or Nim Manual Forwarding
* @returns  L7_FAILURE if not forwarding in all instances
*
* @comments 
*           
*
* @end
*********************************************************************/

L7_BOOL dot1sIsPortDiscarding(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_uint32 index, state;
  DOT1S_PORT_COMMON_t *pPort;

  /* Only return port forwarding state information if MSTP is enabled.
   */

  if (dot1sModeGet() == L7_ENABLE)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      for (index = 0; index <= L7_MAX_MULTIPLE_STP_INSTANCES; index++)
      {
        if (index == instIndex)
        {
              continue;
        }

        if (dot1sInstanceMap[index].inUse == L7_TRUE)
        {
          if(pPort->portInstInfo[index].portState != L7_DOT1S_DISCARDING)
          {
            return(L7_FALSE);
          }
        }
      }
    }
  }
  else /* MSTP is disabled, return NIM Manual Forwarding status */
  {
    if (nimGetIntfActiveState(intIfNum, &state) == L7_SUCCESS)
    {
      if (state == L7_ACTIVE)
      {
  return(L7_FALSE);
      }
    }
  }

  return(L7_TRUE);
}
/*********************************************************************
* @purpose  The dot1s state set timer has expired disable the affected interface
*           
*          
* @param    intIfNum @b{(input)} interface number
*           state    @b{(input)} MSTP state
*
* @returns  
* @returns  
*
* @comments We are in the osapi Timer context keep processing to a minimum
*            The critical section with in the semaphore protection must be
*            short
*           
*
* @end
*********************************************************************/
void dot1sStateSetTimerExpired(L7_uint32 intIfNum, L7_uint32 state)
{
  dot1sIssueCmd(dot1sStateSetError, intIfNum, L7_NULL, L7_NULLPTR);
}

/*********************************************************************
* @purpose  The dot1s state set timer has expired disable the affected interface
*           
*          
* @param    intIfNum @b{(input)} interface number
*           state    @b{(input)} MSTP state
*
* @returns  
* @returns  
*
* @comments We are in the osapi Timer context keep processing to a minimum
*            The critical section with in the semaphore protection must be
*            short
*           
*
* @end
*********************************************************************/
void dot1sStateSetTimerExpiredProcess(L7_uint32 intIfNum)
{
  L7_uint32 CBIntIfNum = 0;
  nimUSP_t usp;

  (void)osapiSemaTake(dot1sStateSetSema, L7_WAIT_FOREVER);
 
  dot1sStateSetTimer = L7_NULLPTR;

  if (dot1sIsStateCBEmpty() != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    CBIntIfNum = (L7_uint32)dot1sCB[dot1sCBHead].intIfNum;
    nimGetIntfName(CBIntIfNum, L7_SYSNAME, ifName);

    /*Clean up dot1sCB */
    dot1sStateCBDeQueue();

    dot1sDiagDisablePort(CBIntIfNum);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Dot1s State Set timer (%d msec) expired for interface %s\n",
            DOT1S_STATE_SET_TIMEOUT, ifName);
  }
  else
  {
    (void)nimGetUnitSlotPort(intIfNum, &usp);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
                    "Received callback when CB is empty interface %d/%d/%d"
                    "Head%d Tail %d", 
                    usp.unit, usp.slot,usp.port,
                    dot1sCBHead, dot1sCBTail);
  }

  dot1sStateSetTimerStart();

  (void)osapiSemaGive(dot1sStateSetSema);


  return;
}

/*********************************************************************
* @purpose  
*           
*          
* @param    intIfNum     @b{(input)} interface number
*            instance     @b{(input)} interface number
*           state        @b{(input)} MSTP state
*
* @returns  
* @returns  
*
* @comments 
*            
*            
*           
*
* @end
*********************************************************************/
L7_RC_t dot1sStateSetAction(L7_uint32 intIfNum, 
                              L7_uint32 instance, 
                              L7_uint32 state)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 instIndex;
  L7_RC_t rc;
  
  p = dot1sIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Cannot find dot1s Interface handle for interface(%s) \n", ifName);
    return L7_FAILURE;
  }

  
  if (dot1sBridge->Mode == L7_ENABLE)
  {
    dot1sStateSetProtocolAction(intIfNum, instance, state, L7_FALSE, L7_FALSE);
  }
  else
  {
    /* If mstpDeviceMode is false then it does not matter if the port has mstp enabled
     * NIM has made this set call so do the neccessary actions that NIM would do
     */
    dot1sStateSetNimAction(intIfNum, state);
  }
  rc = dot1sInstIndexFind(instance,  &instIndex);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Instance(%d) not found, message expected during instance deletion \n",  instance);
    return L7_FAILURE;
  }
  p->portInstInfo[instIndex].stateChangeInProgress--;
   

  if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)))
  {
    dot1sNsfFuncTable.dot1sNsfReconEventFunc(intIfNum, instIndex,
                                        dot1s_recon_event_update);
    return L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  
*           
*          
* @param    intIfNum 	@b{(input)} interface number
*			instance 	@b{(input)} interface number
*           state    	@b{(input)} MSTP state
*			stateStatus @b{(input)} interface number
*
* @returns  
* @returns  
*
* @comments 
*			
*			
*           
*
* @end
*********************************************************************/
L7_RC_t dot1sStateSetCallback(L7_uint32 intIfNum, 
                              L7_uint32 instance, 
                              L7_uint32 state, 
                              dot1s_stateCB_t stateStatus)
{

  if (dot1sDeregister.dot1sStateSetResponseCallback == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "dot1sStateSetCallback is deregistered\n");
    return L7_FAILURE;
  }

  if (!(DOT1S_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Received a interface callback while outside the EXECUTE state");
    return L7_FAILURE;
  }

  return dot1sIssueCmd(dot1sStateChangeDone, intIfNum, instance, &stateStatus);

}
/*********************************************************************
* @purpose  
*           
*          
* @param    intIfNum     @b{(input)} interface number
*			instance 	@b{(input)} interface number
*           state    	@b{(input)} MSTP state
*			stateStatus @b{(input)} interface number
*
* @returns  
* @returns  
*
* @comments 
*			
*			
*           
*
* @end
*********************************************************************/
L7_RC_t dot1sStateSetCallbackProcess(L7_uint32 intIfNum, 
                              L7_uint32 instance, 
                              dot1s_stateCB_t *stateStatus)
{
  L7_uint32 errIntIfNum = L7_NULL;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 instIndex;


  if (stateStatus->state != L7_DOT1S_DISABLED)
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                   "%s: intf %d inst %d state %s status %d", __FUNCTION__,
                   intIfNum, instance,
                   dot1sStateStringGet(stateStatus->state), stateStatus->rc);
  }


  if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)))
  {
    rc = dot1sInstIndexFind(instance,  &instIndex);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
    dot1sNsfFuncTable.dot1sNsfReconEventFunc(intIfNum, instIndex,
                                        dot1s_recon_event_state_change_finish);
    return L7_SUCCESS;
  }
  else
  {
    errIntIfNum = dot1sStateSetBookkeeping(intIfNum, stateStatus);
  }
  /* We have now taken care of the bookkeeping if all was good i.e. the final errIntfNum 
   * was L7_NULL, i.e. the content of stateStatus was alright we can now advertize this
   * and call the relevant routine based on whether mstp is enabled on system and port.
   */
  if (errIntIfNum == L7_NULL)
  {
    rc = dot1sStateSetAction(intIfNum, instance, stateStatus->state);
  }
  
  return rc;
}

/*********************************************************************
* @purpose  Perform neccessary calculations for the circular buffer and diag
*			disable a interface(s) if need be           
*          
* @param    intIfNum 	@b{(input)} interface number
*			stateStatus @b{(input)} interface number
*
* @returns  errIntfNum @{(output)} internal interface number
* @returns  
*
* @comments if errIntIfNum is L7_NULL then the contents of stateStatus
*			is valid and additional action can be performed, ie.
*			either dot1s or nim can take further action on the interface			
*			listed in the stateStatus           
*
* @end
*********************************************************************/
L7_uint32 dot1sStateSetBookkeeping(L7_uint32 intIfNum, 
                                   dot1s_stateCB_t *stateStatus)
{
  L7_uint32 errIntIfNum = L7_NULL;
  nimUSP_t usp;
  L7_INTF_MASK_t mask;
  
  memset(&mask, 0, sizeof(mask));
  /* Bookkeeping */
  (void)osapiSemaTake(dot1sStateSetSema,  L7_WAIT_FOREVER);
  if (dot1sStateSetTimer != L7_NULLPTR)
  {
    (void)osapiTimerFree(dot1sStateSetTimer);
    dot1sStateSetTimer = L7_NULLPTR;
  }
  
  /* Only process if we are actually waiting on something*/
  if (dot1sIsStateCBEmpty() != L7_TRUE)
  {
    /* mpc is what we expect*/
    if (stateStatus->mpc == dot1sCBHead )
    {
      /* apparently last call failed*/
      if (stateStatus->rc != L7_SUCCESS)
      {
        errIntIfNum = (L7_uint32)dot1sCB[dot1sCBHead].intIfNum;
      }
      dot1sStateCBDeQueue();
    }
    else /*misordered interface*/
    {
      
      if (dot1sCB[stateStatus->mpc].valid == L7_TRUE)
      {
        /* Must disable all the interfaces which are inbetween */
        do
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 errIfName[L7_NIM_IFNAME_SIZE + 1];
          
          errIntIfNum = (L7_uint32)dot1sCB[dot1sCBHead].intIfNum;
    
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          nimGetIntfName(errIntIfNum, L7_SYSNAME, errIfName);

          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "dot1sStateSetCallback misordered set expected handle %d intf %s "
            "received handle %d intf %s", dot1sCBHead, errIfName,
                  stateStatus->mpc, ifName);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_RECONCILE,
                  "dot1sStateSetCallback misordered set expected handle %d "
                  "intf %d received handle %d intf %d", 
                  dot1sCBHead, errIntIfNum, stateStatus->mpc, intIfNum);

          dot1sDiagDisablePort(errIntIfNum);

          dot1sStateCBDeQueue();
    
        } while (dot1sCBHead != stateStatus->mpc);
        /* Now check if the current interface had a good return code */
        if (stateStatus->rc != L7_SUCCESS)
        {
          errIntIfNum = (L7_uint32)dot1sCB[dot1sCBHead].intIfNum;
        }
        else
        {
          errIntIfNum = L7_NULL;
        }
    
        dot1sStateCBDeQueue();
        
      }
    }
  }/* Queue is not empty*/
  else
  {
    (void)nimGetUnitSlotPort(intIfNum, &usp);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
                  "Received callback when CB is empty interface %d/%d/%d"
                  "Head%d Tail %d", 
                  usp.unit, usp.slot,usp.port, dot1sCBHead, dot1sCBTail);
  }

  /* Check if we need to restart the timer */
  dot1sStateSetTimerStart();

  if (errIntIfNum != L7_NULL)
  {
    L7_uchar8 errIfName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(errIntIfNum, L7_SYSNAME, errIfName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Intf %s spanning state %s set failed with error code %d",
            errIfName, dot1sStateStringGet(stateStatus->state),
            stateStatus->rc);
    dot1sDiagDisablePort(errIntIfNum);
  }

  (void)osapiSemaGive(dot1sStateSetSema);
  return errIntIfNum;
}
/*********************************************************************
* @purpose  
*           
*          
* @param    intIfNum     @b{(input)} interface number
*			instance 	@b{(input)} interface number
*           state    	@b{(input)} MSTP state
*			reconInProgress @b{(input)} : true if reconciliation is in progress
*
* @returns  
* @returns  
*
* @comments This function when called with reconInProgress as TRUE 
*			should not interact with the state machine but only notify 
*           the rest of the system with the relevant state.
*			
*           
*
* @end
*********************************************************************/
void dot1sStateSetProtocolAction(L7_uint32 intIfNum, L7_uint32 instance, 
                                 L7_uint32 state, L7_BOOL reconInProgress,
                                 L7_BOOL forceNotify)
{
  DOT1S_PORT_COMMON_t *p;
  L7_BOOL allDiscard = L7_FALSE;
  L7_BOOL anyForward = L7_TRUE;
  L7_uint32 instIndex;
  L7_RC_t rc;
  L7_INTF_MASK_t intfMask;
  L7_BOOL notifySystem = L7_TRUE;

  p = dot1sIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return;
  }
  rc = dot1sInstIndexFind(instance,&instIndex);
  if (rc != L7_SUCCESS)
  {
    return;
  }


  if (reconInProgress == L7_FALSE)
  {
    /* If the reconciliation is in progress we need to notify the checkpointed
       state always otherwise only if the interface is reconciled 
    */
    if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)))
    {
      notifySystem = L7_FALSE;
    }

    if ((p->loopInconsistent == L7_TRUE) && (state != L7_DOT1S_DISCARDING))
    {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /*We should not get here . The state machine should handle loop inconsistent*/
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
                "Cannot set state(%d) for port (%s) in instance(%d) "
                "to anything other than discarding for loop inconsistent \n",
              state, ifName, instance);
      return;
    }  
  
    memset(&intfMask, 0, sizeof(intfMask));
  
  
    if (!COMPONENT_ACQ_ISMASKBITSET(p->acquiredList, L7_DOT3AD_COMPONENT_ID))
    {
      /* For usmdb purposes and protocol refelect the state */
      p->portInstInfo[instIndex].portState = state;
    }
  
    if (dot1sNsfFuncTable.dot1sCallCheckpointService)
    {
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,"%s:Calling ckpt for intf %d",
                      __FUNCTION__, intIfNum);
      L7_INTF_SETMASKBIT(intfMask, intIfNum);
      dot1sNsfFuncTable.dot1sCallCheckpointService(&intfMask,instIndex); 
    }
    
  }

  switch (state)
  {
    case L7_DOT1S_DISCARDING:

      if (notifySystem == L7_TRUE)
      {
        /* Tell dot1s registered users for this instance */
        dot1sIhDoNotify(instance, intIfNum, L7_DOT1S_EVENT_NOT_FORWARDING);
        /* Tell NIM to tell every other interested party */
        
        allDiscard = dot1sIsPortDiscarding(intIfNum,instIndex);
  
        if (forceNotify == L7_TRUE ||
            (allDiscard == L7_TRUE && 
            (p->lastNotificationSent == L7_FORWARDING ||
             p->lastNotificationSent == L7_LAST_PORT_EVENT)))
        {
          rc = dot1sIhNotifySystem(intIfNum, L7_NOT_FORWARDING);
          p->lastNotificationSent = L7_NOT_FORWARDING;
        }
      }
      if (reconInProgress == L7_FALSE)
      {
        p->portInstInfo[instIndex].learning = L7_FALSE;
        p->portInstInfo[instIndex].forwarding = L7_FALSE;
        /* Call the pst state machines generate event routine */
        rc = dot1sPstGenerateEvents(p, instIndex);
      }
      break;

    case L7_DOT1S_LEARNING:
      if (reconInProgress == L7_FALSE)
      {
      p->portInstInfo[instIndex].learning = L7_TRUE;
      /* Call the pst state machines generate event routine */
      rc = dot1sPstGenerateEvents(p, instIndex);
      }
      break;

    case L7_DOT1S_FORWARDING:

      if (notifySystem == L7_TRUE)
      {
        anyForward = dot1sIsPortForwarding(intIfNum,instIndex);
        if (forceNotify == L7_TRUE ||
            (anyForward != L7_TRUE && 
            (p->lastNotificationSent == L7_NOT_FORWARDING || 
             p->lastNotificationSent == L7_LAST_PORT_EVENT)))
        {
          rc = dot1sIhNotifySystem(intIfNum, L7_FORWARDING);
          p->lastNotificationSent = L7_FORWARDING;
        }
        dot1sIhDoNotify(instance, intIfNum, L7_DOT1S_EVENT_FORWARDING);
      }

      if (reconInProgress == L7_FALSE)
      {
        p->portInstInfo[instIndex].forwarding = L7_TRUE;
        /* Call the pst state machines generate event routine */
        rc = dot1sPstGenerateEvents(p, instIndex);
      }
      break;

    case L7_DOT1S_DISABLED:
      dot1sIhDoNotify(instance, intIfNum, L7_DOT1S_EVENT_NOT_FORWARDING);
      if (p->lastNotificationSent == L7_FORWARDING ||
          p->lastNotificationSent == L7_LAST_PORT_EVENT)
      {
        rc = dot1sIhNotifySystem(intIfNum, L7_NOT_FORWARDING);
        p->lastNotificationSent = L7_NOT_FORWARDING;
      }
      break;

    case L7_DOT1S_MANUAL_FWD: 
      if (p->lastNotificationSent == L7_NOT_FORWARDING || 
          p->lastNotificationSent == L7_LAST_PORT_EVENT)
      { 
        rc = dot1sIhNotifySystem(intIfNum, L7_FORWARDING);
        p->lastNotificationSent = L7_FORWARDING;
      }
      break;

    case L7_DOT1S_NOT_PARTICIPATE:
    default:
      /*There is no protocol action to be performed*/
      break;
  }

  return;
}

/*********************************************************************
* @purpose  
*           
*          
* @param    intIfNum     @b{(input)} interface number
*			instance 	@b{(input)} interface number
*           state    	@b{(input)} MSTP state
*			stateStatus @b{(input)} interface number
*
* @returns  
* @returns  
*
* @comments 
*			
*			
*           
*
* @end
*********************************************************************/
void dot1sStateSetNimAction(L7_uint32 intIfNum, L7_uint32 state)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  p = dot1sIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return;
  }

  switch (state)
  {
    case L7_DOT1S_DISABLED:
      if (p->lastNotificationSent == L7_FORWARDING || p->lastNotificationSent == L7_LAST_PORT_EVENT)
      {
        rc = dot1sIhNotifySystem(intIfNum, L7_NOT_FORWARDING);
        p->lastNotificationSent = L7_NOT_FORWARDING;
      }
      break;

    case L7_DOT1S_MANUAL_FWD: 
      if (p->lastNotificationSent == L7_NOT_FORWARDING || p->lastNotificationSent == L7_LAST_PORT_EVENT)
      {
        rc = dot1sIhNotifySystem(intIfNum, L7_FORWARDING);
        p->lastNotificationSent = L7_FORWARDING;
      }
      break;

    default:
      /*With MSTP disbaled we should not get any other notifications*/
      break;
  }


  return;
}
/*********************************************************************
* @purpose  This routine deterimines whether the stateset timer needs 
* 			to restart or not and adds the timer in osapiTimer
*           
*          
* @param    none
*
* @returns  none
*
* @comments Examines the MPC and NEC to determine whether there are any pending events
*  			that need to be acknoledged. Assumes that NEC has already been incremented by 1
* 			Assumes that NEC and MPC are unit32s.
*			Does not protect from more that 0xffffffff pending events. i.e. assumes that 
*           such a rollover has not occured. If such is the case then the system is in 
* 			a un recoverable state. 
*			
*			
* @end
*********************************************************************/
void dot1sStateSetTimerStart()
{
  /* We will not start the timer if Queue is not empty */
  if (dot1sIsStateCBEmpty() == L7_FALSE)
  {
    /* Another set is pending*/
    if (dot1sStateSetTimer == L7_NULLPTR)
    {
      /* Start a new osapi timer */
      osapiTimerAdd(dot1sStateSetTimerExpired, 
                    (L7_uint32)dot1sCB[dot1sCBHead].intIfNum, 
                    L7_NULL, DOT1S_STATE_SET_TIMEOUT, &dot1sStateSetTimer);
    }
    return;
  }
  return;
}

void dot1sDebugCB(L7_uint32 printCB)
{
  L7_uint32 head, tail;
  /* Explicitly not taking the dot1sStateSetSema semaphore so that 
   * we do not affect the actual processing when we are printing out the 
   * entire CB buffer
   */ 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot1sCBHead: %d\n", dot1sCBHead);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot1sCBTail: %d\n", dot1sCBTail);
  if (printCB != 0)
  {
    head = dot1sCBHead;
    tail = dot1sCBTail;
    while (head != tail)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "CB index %d  valid %d contains intIfNum %d\n", 
                    head, dot1sCB[head].valid, dot1sCB[head].intIfNum);
      head = ((head + 1)%DOT1S_STATE_SET_CB_SIZE);
    }
  }
  return;
}

void dot1sTestCBTimer(L7_uint32 nec, L7_uint32 mpc)
{
  /* We will not start the timer if NEC is greater than MPC by exactly 1 */
  if (((nec > mpc) && (nec - mpc == 1)) ||
      ((nec == 0) && (mpc == 0xFFFFFFFF)))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Will not start timer\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Will start timer\n");
    return;
  }
  return;


}
/*********************************************************************
* @purpose  Check if the state callback queue is Full
*           
*          
* @param    none
*
* @returns  L7_TRUE/L7_FALSE
*
* @comments This function assumes that dot1sStateSetSema is taken
*            
*            
* @end
*********************************************************************/
L7_BOOL dot1sIsStateCBFull()
{
  return (((dot1sCBTail +1) % DOT1S_STATE_SET_CB_SIZE) == dot1sCBHead);

}
/*********************************************************************
* @purpose  Check if the state callback queue is Empty
*           
*          
* @param    none
*
* @returns  L7_TRUE/L7_FALSE
*
* @comments This function assumes that dot1sStateSetSema is taken
*            
*            
* @end
*********************************************************************/
L7_BOOL dot1sIsStateCBEmpty()
{
  return (dot1sCBTail == dot1sCBHead);

}

/*********************************************************************
* @purpose  Remove the first element from the queue
*           
*          
* @param    none
*
* @returns  The new head of the queue
*
* @comments This function assumes that dot1sStateSetSema is taken
*            
*            
* @end
*********************************************************************/
L7_uint32 dot1sStateCBDeQueue()
{
  dot1sCB[dot1sCBHead].intIfNum = L7_NULL;
  dot1sCB[dot1sCBHead].valid = L7_FALSE;
  dot1sCBHead = ((dot1sCBHead+1)%DOT1S_STATE_SET_CB_SIZE);

  return dot1sCBHead;
}
