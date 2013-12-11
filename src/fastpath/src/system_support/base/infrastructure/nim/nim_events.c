/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_events.c
*
* @purpose    The functions that handle events from other components
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#define L7_MAC_ENET_BCAST
#include "l7_common.h"
#include "nim_data.h"
#include "nim_util.h"
#include "nim_events.h"
#include "nim_cnfgr.h"
#include "nim_outcalls.h"
#include "cfg_change_api.h"
#include "simapi.h"   /* for usmDbStpSpanningTreeModeGet */
#include "trapapi.h"  /* for trapMgrLinkUpLogTrap */
#include "nim_config.h"
#include "cardmgr_api.h"
#include "l7_packet.h"
#include "event_manager.h"
#include "nim_sid.h"
#include "nim_trace.h"
#include "nim_startup.h"
#include "cnfgr_sid.h"
#include "nim_exports.h"

#include "logger.h"

static nimCorrelatorData_t  correlatorTable = { 0};
static NIM_CORRELATOR_t     *correlatorInUse = 0;

/* The NIM timeout is 600 seconds. The timeout needs to be long
** because when routing is enabled an interface event can trigger a long operation,
** such as clearing the ARP cache.
*/
#define NIM_EVENT_TIMEOUT 600
#define MASK_STRING_LENGTH  (((L7_LAST_COMPONENT_ID/32)+1)*15)

/* semaphore for the event transaction creation */
static void *nimEventSema = L7_NULLPTR;

static L7_uint32 maxCorrelators = 0;
static L7_uint32 maxCorrelatorsInUse = 0;
static L7_uint32 lastCorrelatorTaken = 0;

/* data and structures for status queue */
static void *pNimEventStatusQueue;

typedef struct {
  L7_RC_t                 response;
  NIM_CORRELATOR_t        correlator;
  L7_PORT_EVENTS_t        event;
  L7_uint32               intIfNum;
} NIM_EVENT_STATUS_MSG_t;

/* Local prototypes */
void nimEventAttachPostProcess(NIM_EVENT_NOTIFY_INFO_t eventInfo);
extern L7_char8 *nimDebugCompStringGet(L7_COMPONENT_IDS_t cid);

static L7_uint32 default_nim_timeout = NIM_EVENT_TIMEOUT*1000;
int nimDebugTimeoutSet(L7_uint32 timeout)
{
  default_nim_timeout = timeout;

  return 0;
}

/* PTin added: ptin_intf
   Check if nim message queue is empty */
L7_BOOL nimIntfRequestsDone(void)
{
  L7_uint32 num;

  if (osapiMsgQueueGetNumMsgs(nimCtlBlk_g->nimMsgQueue, &num) != L7_SUCCESS)
    return L7_FALSE;

  return (num == 0);
}
/* PTin end */

/*********************************************************************
* @purpose  go through registered users and notify them of interface changes.
*
* @param    correlator    The correlator to match with the request
* @param    eventInfo     The event, intf, component, callback func
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
void nimDoNotify(NIM_CORRELATOR_t correlator, NIM_EVENT_NOTIFY_INFO_t eventInfo)
{
  L7_RC_t   rc;
  L7_int32 i;
  L7_PORT_EVENTS_t event  = eventInfo.event;
  L7_uint32 intIfNum      = eventInfo.intIfNum, now;
  NIM_NOTIFY_CB_INFO_t cbData;
  L7_uchar8 maskString[MASK_STRING_LENGTH]; /* number of chars per mask index */
  NIM_EVENT_STATUS_MSG_t msg;

  /* Set to L7_TRUE when one or more clients want to receive this event. */
  L7_BOOL sendEvent = L7_FALSE;

  now = osapiUpTimeRaw();
  if (nimPhaseStatusCheck() != L7_TRUE) return;

  /* create the transaction */
  osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

  /* start with no responses received */
  memset(correlatorTable.remainingMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));
  memset(correlatorTable.failedMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));
  memset(maskString,0,sizeof(maskString));

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((nimCtlBlk_g->nimNotifyList[i].registrar_ID) &&
      (PORTEVENT_ISMASKBITSET(nimCtlBlk_g->nimNotifyList[i].notify_mask,event)))
    {
      correlatorTable.remainingMask[i/32] |= (1 << i % 32);
      sendEvent = L7_TRUE;
    }
  }

  if (sendEvent)
  {
      correlatorTable.time = now + default_nim_timeout;
      correlatorTable.correlator = correlator;
      correlatorTable.inUse = L7_TRUE;
      memcpy(&correlatorTable.requestData,&eventInfo,sizeof(NIM_EVENT_NOTIFY_INFO_t));

      /* assume success */
      correlatorTable.response = L7_SUCCESS;

      osapiSemaGive(nimEventSema);
      /* notify the components */
      for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
      {
        if ((nimCtlBlk_g->nimNotifyList[i].registrar_ID) &&
          (PORTEVENT_ISMASKBITSET(nimCtlBlk_g->nimNotifyList[i].notify_mask,event)))
        {
          nimTracePortEventComp(i,event,intIfNum,L7_TRUE,correlator);
          nimProfilePortEventComp(i,event,intIfNum,L7_TRUE);
          rc = (*nimCtlBlk_g->nimNotifyList[i].notify_intf_change)(intIfNum, event,correlator);
        }
      }

      /* wait for message with a timeout */
      rc = osapiMessageReceive(pNimEventStatusQueue, &msg, sizeof(NIM_EVENT_STATUS_MSG_t),default_nim_timeout);

      osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

      cbData.handle           = correlatorTable.correlator;

      if (rc != L7_SUCCESS)
      {
        cbData.response.rc      = L7_FAILURE;
        cbData.response.reason  = NIM_ERR_RC_TIMEOUT;

        for (i=(L7_LAST_COMPONENT_ID / 32); i >= 0 ;i--)
        {
          if ((MASK_STRING_LENGTH - strlen(maskString)) >= 15)
          {
            sprintf(maskString,"%s 0x%.8x ",maskString,correlatorTable.remainingMask[i]);
          }
        }

        LOG_FATAL(LOG_CTX_MISC,"NIM: Timeout event(%d), intIfNum(%d) remainingMask = %s (rc=%d)",
                      correlatorTable.requestData.event,
                      correlatorTable.requestData.intIfNum,maskString,
                      rc);

        NIM_LOG_ERROR("NIM: Timeout event(%d), intIfNum(%d) remainingMask = %s\n",
                      correlatorTable.requestData.event,
                      correlatorTable.requestData.intIfNum,maskString);
      }
      else
      {
        /* tally complete */
        cbData.response.rc  = correlatorTable.response;
      }
  }
  else
  {
      cbData.response.rc = L7_SUCCESS;
  }

  /* delete the correlator for the next event */
  if (nimEventCorrelatorDelete(correlatorTable.correlator) != L7_SUCCESS)
  {
    NIM_LOG_ERROR("NIM: Error deleting the event correlator(%d)\n", correlatorTable.correlator);
  }

  correlatorTable.inUse = L7_FALSE;

  (void)osapiSemaGive(nimEventSema);
  /* change the state of intf if necessary, do any processing, and callback the generator */
  nimEventPostProcessor(eventInfo,cbData);

}

/*********************************************************************
* @purpose  Send message to nim to Notifies registered routines of interface changes.
*
* @param    intIfNum    internal interface number
* @param    event       all port events,
*                       (@b{  L7_CREATE,
*                             L7_DELETE,
*                             L7_PORT_DISABLE,
*                             L7_PORT_ENABLE,
*                             L7_UP,
*                             L7_DOWN,
*                             L7_ENABLE,
*                             L7_DISABLE,
*                             L7_DIAG_DISABLE,
*                             L7_FORWARDING,
*                             L7_NOT_FORWARDING,
*                             L7_CREATE,
*                             L7_DELETE,
*                             L7_ACQUIRE,
*                             L7_RELEASE,
*                             L7_SPEED_CHANGE,
*                             L7_LAG_CFG_CREATE,
*                             L7_LAG_CFG_MEMBER_CHANGE,
*                             L7_LAG_CFG_REMOVE,
*                             L7_LAG_CFG_END,
*                             L7_PROBE_SETUP,
*                             L7_PROBE_TEARDOWN,
*                             L7_SET_INTF_SPEED,
*                             L7_SET_MTU_SIZE,
*                             L7_VRRP_TO_MASTER or
*                             L7_VRRP_FROM_MASTER})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimNotifyIntfChange(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_RC_t     returnVal =  L7_FAILURE;
  NIM_HANDLE_t           handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  L7_RC_t rc;
  L7_uint32 ifIndex;
  L7_char8 buf[128];

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);

  eventInfo.component     = L7_NIM_COMPONENT_ID;
  eventInfo.event         = event;
  eventInfo.intIfNum      = intIfNum;
  eventInfo.pCbFunc       = L7_NULLPTR;

  /* don't need to keep the handle around */
  if ((returnVal = nimEventIntfNotify(eventInfo,&handle)) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: Failed to send PORT EVENT on NIM_QUEUE\n");
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Failed to send PORT EVENT on NIM_QUEUE (intIfNum=%u)", intIfNum);
  }
  else
  {
    rc = nimGetIntfIfIndex(intIfNum, &ifIndex);
    sprintf(buf, "IfIndex: %d", ifIndex);
    EM_LOG((SYSEVENT_EVENT|event), buf);
    returnVal = L7_SUCCESS;
  }

  return(returnVal);
}

void nimEventCmgrDebugCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  if (retVal.response.rc != L7_SUCCESS)
  {
    if (retVal.response.reason == NIM_ERR_RC_TIMEOUT)
    {
      NIM_LOG_MSG("NIM: Timeout Cmgr event occured for handle(%d)\n",retVal.handle);
    }
    else
    {
      NIM_LOG_MSG("NIM: Failed Cmgr event notify for handle(%d)\n",retVal.handle);
    }
  }
}

void nimEventDtlDebugCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  if (retVal.response.rc != L7_SUCCESS)
  {
    if (retVal.response.reason == NIM_ERR_RC_TIMEOUT)
    {
      NIM_LOG_MSG("NIM: Timeout DTL event occured for handle(%d)\n",retVal.handle);
    }
    else
    {
      NIM_LOG_MSG("NIM: Failed DTL event notify for handle(%d)\n",retVal.handle);
    }
  }
}

/*********************************************************************
* @purpose  Register a routine to be called when a link state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    *notify        pointer to a routine to be invoked for link state
*                          changes.  Each routine has the following parameters:
*                          (internal interface number, event(L7_UP, L7_DOWN,
*                           etc.)).
* @param    *startup_notify @b{(input)} pointer to a routine to be invoked at startup.
*                          Each routine has the following parameters:
*                          (startup_phase(NIM_INTERFACE_CREATE_STARTUP,
*                                         NIM_INTERFACE_ACTIVATE_STARTUP)).
* @param    priority       @b{(input)} priority of the startup notification.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimRegisterIntfChange(L7_COMPONENT_IDS_t registrar_ID,
                              L7_RC_t (*notify)(L7_uint32 intIfNum,L7_uint32 event,NIM_CORRELATOR_t correlator),
                              StartupNotifyFcn startupFcn,
                              L7_uint32 priority)
{
  L7_RC_t rc = L7_FAILURE;

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    NIM_LOG_MSG("NIM registrar_ID %ud greater then NIM_USER_LAST\n", registrar_ID);
  }
  else if (nimCtlBlk_g == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimCtlBlk_g uninitialized\n");

  }
  else if (nimCtlBlk_g->nimNotifyList == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimNotifyList not initialized\n");
  }
  else
  {
    nimCtlBlk_g->nimNotifyList[registrar_ID].registrar_ID = registrar_ID;
    nimCtlBlk_g->nimNotifyList[registrar_ID].notify_intf_change = notify;

    if (startupFcn != L7_NULLPTR)
    {
      nimStartUpCreate(registrar_ID, priority, startupFcn);
    }
    else {
      LOG_ERROR(registrar_ID);
    }
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*******************************************************************************
* @purpose  To allow components to register only for port events that it processes
*
* @param    registrar_ID     @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)
* @param    registeredEvents @b{(input)} Bit mask of port events that component requests
*                              notification
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t nimRegisterIntfEvents(L7_COMPONENT_IDS_t registrar_ID,
                              PORTEVENT_MASK_t   registeredEvents)
{
  L7_RC_t rc = L7_FAILURE;

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    NIM_LOG_MSG("NIM registrar_ID %ud greater then NIM_USER_LAST\n", registrar_ID);
  }
  else if (nimCtlBlk_g == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimCtlBlk_g uninitialized\n");

  }
  else if (nimCtlBlk_g->nimNotifyList == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimNotifyList not initialized\n");
  }
  else
  {
    osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);
    nimCtlBlk_g->nimNotifyList[registrar_ID].notify_mask = registeredEvents;
    osapiSemaGive(nimEventSema);
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  De-Register a routine to be called when a link state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimDeRegisterIntfChange(L7_COMPONENT_IDS_t registrar_ID)
{
  L7_RC_t rc = L7_FAILURE;

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    NIM_LOG_MSG("NIM registrar_ID %ud greater then NIM_USER_LAST\n", registrar_ID);
  }
  else if (nimCtlBlk_g == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimCtlBlk_g uninitialized\n");

  }
  else if (nimCtlBlk_g->nimNotifyList == L7_NULLPTR)
  {
    NIM_LOG_ERROR("NIM: nimNotifyList not initialized\n");
  }
  else
  {
    nimCtlBlk_g->nimNotifyList[registrar_ID].registrar_ID = L7_NULL;
    nimCtlBlk_g->nimNotifyList[registrar_ID].notify_intf_change = L7_NULLPTR;
    memset(&nimCtlBlk_g->nimNotifyList[registrar_ID].notify_mask,
           0x0, sizeof(PORTEVENT_MASK_t));
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*********************************************************************
* @purpose  Notify all recepients of nim notifications of the link up event
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine is called only once when the system initialization is complete.
*           This event was suppressed when the system comes up and is only
*           propagated when all the components have initialized.
*
* @end
*
*********************************************************************/
L7_RC_t nimNotifyLinkUp()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 result;
  L7_uint32 i;

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);
  for (i = 1; i <= platIntfTotalMaxCountGet() ; i++)
  {
    result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, i);
    if ((result != L7_NIM_UNUSED_PARAMETER) && (nimCtlBlk_g->nimPorts[i].sysIntfType != L7_CPU_INTF))
    {
      rc = nimNotifyIntfChange(i,L7_UP);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Callback routine for DTL to notify NIM of Interface Events
*
*
* @param    usp           internal interface number
* @param    event         all port events,
* @param    dapiIntMgmt   data sent with the callback
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimDtlIntfChangeCallback(nimUSP_t *usp, L7_uint32 event, void * dapiIntmgmt)
{
  nimUSP_t      nimUsp;
  L7_uint32     intIfNum = 0;
  NIM_HANDLE_t           handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    return;
  }

  /* fill the nim config usp */
  nimUsp.unit = usp->unit;
  nimUsp.slot = usp->slot;
  nimUsp.port = usp->port;

  if (nimGetIntIfNumFromUSP(&nimUsp, &intIfNum) != L7_SUCCESS)
  {
    /* interface does not exist */
    NIM_LOG_MSG("NIM: Failed to find interface at unit %d slot %d port %hu for event(%d)\n",
                usp->unit,usp->slot,usp->port,event);
    return;
  }

  NIM_CRIT_SEC_WRITE_ENTER();

  switch (event)
  {
    case L7_UP:
      NIM_INTF_SETMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
      break;
    case L7_DOWN:
      NIM_INTF_CLRMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
      break;
    default:
      break;
  }

  NIM_CRIT_SEC_WRITE_EXIT();

  eventInfo.component     = L7_DTL_COMPONENT_ID;
  eventInfo.event         = event;
  eventInfo.intIfNum      = intIfNum;
  eventInfo.pCbFunc       = L7_NULLPTR;

  /* don't need to keep the handle around */
  if (nimEventIntfNotify(eventInfo,&handle) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: Failed to send CMGR PORT EVENT on NIM_QUEUE\n");
  }

  return;

}

/*********************************************************************
* @purpose  Notifies registered routines of interface changes.
*
* @param    correlator    The correlator to match with the request
* @param    eventInfo     The event, intf, component, callback func
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimNotifyUserOfIntfChange(NIM_CORRELATOR_t correlator, NIM_EVENT_NOTIFY_INFO_t eventInfo)
{
  L7_uint32   result;
  L7_uint32   macroPort;
  nimUSP_t    usp;
  L7_INTF_STATES_t  state;
  NIM_NOTIFY_CB_INFO_t status;
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   intIfNum = eventInfo.intIfNum;
  L7_PORT_EVENTS_t  event = eventInfo.event;
  L7_BOOL performCallback = L7_FALSE;
#if L7_FEAT_DUAL_PHY_COMBO
  L7_uint32   negoCapabilities;
#endif
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  memset(ifName, 0, sizeof(ifName));
  state = nimUtilIntfStateGet(eventInfo.intIfNum);

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc =  (L7_ERROR);
    performCallback = L7_TRUE;
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    NIM_LOG_MSG("Component %s generated interface event %s (%d) for interface %s (%u). "
                "Interface manager not ready to receive interface events.",
                nimDebugCompStringGet(eventInfo.component), nimGetIntfEvent(event),
                event, ifName, intIfNum);
  }
  else if (state == L7_INTF_UNINITIALIZED)
  {
    rc = L7_SUCCESS;
    performCallback = L7_TRUE;

    if ((event != L7_DOWN) &&
        (event != L7_PORT_DISABLE) &&
        (event != L7_NOT_FORWARDING) &&
        (event != L7_INACTIVE))
    {
      /* Not a INTF teardown event, LOG it */
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      NIM_LOG_MSG("Component %s generated interface event %s (%d) for interface %s (%u). "
                  "This interface is uninitialized.",
                  nimDebugCompStringGet(eventInfo.component), nimGetIntfEvent(event),
                  event, ifName, intIfNum);
    }
    else
    {
      /* A teardown event, trace it (This can happen during normal operation */
      nimTraceEventError(eventInfo.component,event,intIfNum,NIM_ERR_EVENT_INTF_DNE);
    }
  }
  else if ((rc = nimGetUnitSlotPort(intIfNum,&usp)) != L7_SUCCESS)
  {
    rc = L7_ERROR;
    performCallback = L7_TRUE;
  }
  else
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (event < L7_LAST_PORT_EVENT)
      {
        switch (event)
        {
          case L7_ATTACH:
            if (nimUtilIntfStateGet(eventInfo.intIfNum) != L7_INTF_CREATED)
            {
              NIM_LOG_MSG("NIM: L7_ATTACH out of order for Interface %s\n", ifName);
              rc = L7_FAILURE;
              performCallback = L7_TRUE;
            }
            else if (nimIntfConfigApply(eventInfo.intIfNum) != L7_SUCCESS)
            {
              NIM_LOG_MSG("NIM: Failed to ATTACH Interface %s\n", ifName);
              rc = L7_FAILURE;
              performCallback = L7_TRUE;
            }
            else
            {
              rc = nimUtilIntfStateSet(eventInfo.intIfNum,L7_INTF_ATTACHING);
              nimDoNotify(correlator,eventInfo);;
            }
            break;
          case L7_DETACH:
            if (nimUtilIntfStateGet(eventInfo.intIfNum) != L7_INTF_ATTACHED)
            {
              NIM_LOG_MSG("NIM: L7_DETACH out of order for Interface %s\n", ifName);
              rc = L7_FAILURE;
              performCallback = L7_TRUE;
            }
            else
            {
              rc = nimUtilIntfStateSet(eventInfo.intIfNum,L7_INTF_DETACHING);

              NIM_CRIT_SEC_WRITE_ENTER();

              nimCtlBlk_g->nimPorts[intIfNum].linkChangeTime = osapiUpTimeRaw();

              NIM_INTF_CLRMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
              NIM_INTF_CLRMASKBIT(nimCtlBlk_g->forwardStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);

              NIM_CRIT_SEC_WRITE_EXIT();

              nimDoNotify(correlator,eventInfo);;
            }
            break;
          case L7_DELETE:
            if (nimUtilIntfStateGet(eventInfo.intIfNum) != L7_INTF_CREATED)
            {
              NIM_LOG_MSG("NIM: L7_DELETE out of order for Interface %s\n", ifName);
              rc = L7_FAILURE;
              performCallback = L7_TRUE;
            }
            else
            {
              rc = nimUtilIntfStateSet(eventInfo.intIfNum,L7_INTF_DELETING);
              nimDoNotify(correlator,eventInfo);;
            }
            break;

          case  L7_UP:

            NIM_CRIT_SEC_WRITE_ENTER();

            nimCtlBlk_g->nimPorts[intIfNum].linkChangeTime = osapiUpTimeRaw();
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
#if L7_FEAT_DUAL_PHY_COMBO
            if(dtlIntfNegotiationCapabilitiesGet(intIfNum, &negoCapabilities) == L7_SUCCESS)
            {
              nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities = negoCapabilities;
            }
#endif

            NIM_CRIT_SEC_WRITE_EXIT();

            nimDoNotify(correlator,eventInfo);;

            if (nimIsMacroPort(intIfNum))
            {
              if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.adminState == L7_ENABLE)
              {

                /* Hooks for interactions with other components */
                nimNotifyUserOfIntfChangeOutcall(intIfNum, event);

                trapMgrLinkUpLogTrap(intIfNum);
              }
            }
            else
            {
              macroPort = nimCtlBlk_g->nimPorts[intIfNum].operInfo.macroPort.macroPort;

              if (nimCtlBlk_g->nimPorts[macroPort].configPort.cfgInfo.adminState == L7_ENABLE)
              {
                /* Do I issue a dtl call */
                if (trapMgrLinkUpLogTrap(intIfNum) != L7_SUCCESS)
                {
                  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                          "NIM: failed to send a TRAP message\n");
                }
              }
            }
            break;

          case L7_DOWN:
            NIM_CRIT_SEC_WRITE_ENTER();

            nimCtlBlk_g->nimPorts[intIfNum].linkChangeTime = osapiUpTimeRaw();
            /* check if the port is in FWD state */
            result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->forwardStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
#if L7_FEAT_DUAL_PHY_COMBO
            if(dtlIntfNegotiationCapabilitiesGet(intIfNum, &negoCapabilities) == L7_SUCCESS)
            {
              nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities = negoCapabilities;
            }
#endif

            NIM_CRIT_SEC_WRITE_EXIT();


            nimDoNotify(correlator,eventInfo);

            if (trapMgrLinkDownLogTrap(intIfNum) != L7_SUCCESS)
            {
              LOG_FATAL(LOG_CTX_MISC,"FATAL ERROR: intfIfNum=%u",intIfNum);

              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                      "NIM: failed to send a TRAP message\n");
            }
            /* Customer trap */
            if (trapMgrLinkFailureLogTrap(intIfNum) != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                      "NIM: failed to send a TRAP message\n");
            }

            /* if the Macro port and in FWD state */
            if ((nimIsMacroPort(intIfNum)) &&
                ((result) || (nimCtlBlk_g->nimPorts[intIfNum].sysIntfType == L7_LAG_INTF)))
            {
              /* Hooks for interactions with other components */
              nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            }
            break;


          case L7_FORWARDING:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->forwardStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
            nimDoNotify(correlator,eventInfo);

            /* Hooks for interactions with other components */
            nimNotifyUserOfIntfChangeOutcall(intIfNum, event);

            break;

          case L7_NOT_FORWARDING:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->forwardStateMask, nimCtlBlk_g->nimPorts[intIfNum].runTimeMaskId);
            nimDoNotify(correlator,eventInfo);

            /* Hooks for interactions with other components */
            nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            break;

          case L7_PORT_DISABLE:
            nimDoNotify(correlator,eventInfo);
            if (nimIsMacroPort(intIfNum))
            {
              result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, intIfNum);

              if (result)
              {
                /* Hooks for interactions with other components */
                nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
              }
            }
            break;

          case L7_PORT_ENABLE:
            nimDoNotify(correlator,eventInfo);

            if (nimIsMacroPort(intIfNum))
            {
              /* only send a trap if it's already up and being enabled */
              result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, intIfNum);

              if (result)
              {
                trapMgrLinkUpLogTrap(intIfNum);

                /* Hooks for interactions with other components */
                nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
              }
            }
            break;


          case L7_LAG_ACQUIRE:
            nimDoNotify(correlator,eventInfo);

            /* Hooks for interactions with other components */
            nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            break;

          case L7_LAG_RELEASE:
            nimDoNotify(correlator,eventInfo);

            /* only send a trap if it's already up and being enabled */
            result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, intIfNum);

            /* release implies port is now a macro port */
            if (result)
            {
              trapMgrLinkUpLogTrap(intIfNum);

              /* Hooks for interactions with other components */
              nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            }
            break;

          case L7_PORT_ROUTING_ENABLED:
          case L7_PORT_ROUTING_DISABLED:
          case L7_PROBE_TEARDOWN:
          case L7_DELETE_PENDING:
          case L7_SPEED_CHANGE:
          case L7_LAG_CFG_CREATE:
          case L7_LAG_CFG_MEMBER_CHANGE:
          case L7_LAG_CFG_REMOVE:
          case L7_LAG_CFG_END:
          case L7_PORT_STATS_RESET:
            nimDoNotify(correlator,eventInfo);
            break;

          case L7_PROBE_SETUP:
            nimDoNotify(correlator,eventInfo);
            /* Hooks for interactions with other components */
            nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            break;

          case L7_DOT1X_PORT_AUTHORIZED:
          case L7_DOT1X_PORT_UNAUTHORIZED:
            /* Hooks for interactions with other components */
            /*
             * need to delete the correlator and do the callback since
             * this event is not propogated to other components
             */
            performCallback = L7_TRUE;
            rc = L7_SUCCESS;
            nimNotifyUserOfIntfChangeOutcall(intIfNum, event);
            break;

          case L7_DOT1X_ACQUIRE:
            nimDoNotify(correlator,eventInfo);
            break;

          case L7_DOT1X_RELEASE:
            nimDoNotify(correlator,eventInfo);
            break;

          default:
            nimDoNotify(correlator,eventInfo);
            break;
        }
      }
      else
      {
        rc = (L7_FAILURE);
      }
    }
  }

  nimProfilePortEvent(eventInfo.component,eventInfo.event,eventInfo.intIfNum,L7_FALSE);
  if (performCallback == L7_TRUE)
  {
    /* delete the correlator for the next event */
    if (nimEventCorrelatorDelete(correlator) != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Error deleting the event correlator(%d)\n", correlator);
    }

    nimTracePortEvent(eventInfo.component,eventInfo.event,eventInfo.intIfNum,L7_FALSE,correlator);

    if (eventInfo.pCbFunc != L7_NULLPTR)
    {
      status.event    = eventInfo.event;
      status.intIfNum = eventInfo.intIfNum;
      status.response.rc = rc;
      status.handle   = correlator;

      eventInfo.pCbFunc(status);
    }
  }
  return(rc);
}


/*********************************************************************
* @purpose  callback function for interface to mac addr translation
*
* @param    intfNum   Internal Interface Number
* @param    dtlEvent  DTL_EVENT_t DTL_EVENT_ADDR_INTF_MAC_QUERY
* @param    data      pointer to mac address
* @param    numOfElements pointer to the number of mac address in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    here the number of mac address requested is always 1 as it is
*           a translation of only 1 interface number
*
* @end
*
*********************************************************************/
L7_RC_t nimMacAddrQueryCallback(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, void *data, L7_uint32 *numOfElements)
{
  L7_RC_t rc = L7_FAILURE;

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);
  rc = nimGetIntfAddress(intfNum,L7_NULL,data);
  *(numOfElements) = 1;
  return(rc);

}

/*********************************************************************
* @purpose  NIM Task
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimTask()
{
  nimPdu_t nmpdu;
  L7_RC_t rc;
  L7_CNFGR_CMD_DATA_t pCmdData;

  if (osapiTaskInitDone(L7_NIM_TASK_SYNC) != L7_SUCCESS)
  {
    NIM_LOG_ERROR("NIM: Task failed to int\n");
  }

  do
  {
    if (nimCtlBlk_g->nimMsgQueue == L7_NULLPTR)
      continue;

    /* initialize the message to zero */
    memset(&nmpdu,0x00,sizeof(nimPdu_t));

    rc = osapiMessageReceive(nimCtlBlk_g->nimMsgQueue, &nmpdu,
                             sizeof(nimPdu_t), L7_WAIT_FOREVER);
    if (rc == L7_SUCCESS)
    {
      switch (nmpdu.msgType)
      {
        case CNFGR_MSG:

          pCmdData = nmpdu.data.pCmdData;
          nimRecvCnfgrCommand( &pCmdData );
          break;

        case NIM_MSG:
          LOG_INFO(LOG_CTX_PTIN_INTF, "Going to call nimNotifyUserOfIntfChange: intIfNum=%u", nmpdu.data.message.eventInfo.intIfNum);
          if (nimNotifyUserOfIntfChange(nmpdu.data.message.correlator,nmpdu.data.message.eventInfo) != L7_SUCCESS)
          {
            LOG_FATAL(LOG_CTX_MISC,"NIM: Failed to notify users of interface change: event=%u, intIfNum=%u",
                      nmpdu.data.message.eventInfo.event, nmpdu.data.message.eventInfo.intIfNum);

            NIM_LOG_MSG("NIM: Failed to notify users of interface change\n");
          }
          break;

        case START_MSG:

          (nmpdu.data.nimStartMsg.startupFunction)(nmpdu.data.nimStartMsg.phase);
          /* Wait until startup has completed */
          nimStartupEventWait();
          break;

        default:
          NIM_LOG_MSG("nimTask(): invalid message type:%d. %s:%d\n",
                      nmpdu.msgType, __FILE__, __LINE__);
          break;
      }
    }
  } while (1);
  return;
}

/*********************************************************************
*
* @purpose  Status callback from components to NIM for PORT EVENT Notifications
*
* @param    status        @b{(output)}  Status from the component
*
* @returns  void
*
* @notes    At the conclusion of processing a PORT Event, each component must
*           must call this function with the correlator, intf, status, and
*           the component ID
*
* @end
*********************************************************************/
void nimEventStatusCallback(NIM_EVENT_COMPLETE_INFO_t status)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_BOOL   done = L7_FALSE;
  NIM_EVENT_STATUS_MSG_t msg = { 0 };

  LOG_INFO(LOG_CTX_PTIN_INTF, "nimEventStatusCallback: Event=%u, intIfNum=%u", status.event, status.intIfNum);

  osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

  nimTracePortEventComp(status.component,status.event,status.intIfNum,L7_FALSE,status.correlator);
  nimProfilePortEventComp(status.component,status.event,status.intIfNum,L7_FALSE);

  /* take the correlator semaphore and tally the response */
  if ((rc = nimEventTally(status,&done)) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: Error in the tally routine\n");
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Error in the tally routine: Event=%u, intIfNum=%u", status.event, status.intIfNum);
  }
  else if (done == L7_TRUE)
  {
    /* Send a message when the api is ready */
    correlatorTable.inUse = L7_FALSE;

    msg.correlator = status.correlator;
    msg.event      = status.event;
    msg.intIfNum   = status.intIfNum;
    msg.response   = correlatorTable.response;

    LOG_INFO(LOG_CTX_PTIN_INTF, "Sending event: Event=%u, intIfNum=%u", status.event, status.intIfNum);

    rc = osapiMessageSend(pNimEventStatusQueue,
                          &msg,
                          sizeof(msg),
                          L7_NO_WAIT,
                          L7_MSG_PRIORITY_NORM);

    if (rc != L7_SUCCESS)
    {
      NIM_LOG_MSG("failed to put status on queue");
      LOG_ERR(LOG_CTX_PTIN_INTF, "failed to put status on queue: Event=%u, intIfNum=%u", status.event, status.intIfNum);
    }

  }
  else
  {
    /* no work to do */
  }

  osapiSemaGive(nimEventSema);

  return;
}


/*********************************************************************
*
* @purpose  Notify all interested components of an Interface Change event
*
* @param    eventInfo     @b{(input)}   The event information
* @param    pHandle       @b{(output)}  A handle that identifies this request
*
* @returns  L7_SUCCESS    Event was accepted
* @returns  L7_FAILURE    Event was not accepted
*
* @notes    If the caller is interested in being notified at when the event is
*           completed, they must put a callback function in the cbInfo.pCbFunc
*
* @end
*********************************************************************/
L7_RC_t nimEventIntfNotify(NIM_EVENT_NOTIFY_INFO_t eventInfo, NIM_HANDLE_t *pHandle)
{
  L7_RC_t rc = L7_SUCCESS;
  nimPdu_t  pdu;
  NIM_CORRELATOR_t *correlator;
  correlator = pHandle;

  LOG_INFO(LOG_CTX_PTIN_INTF, "Event=%u: intIfNum=%u", eventInfo.event, eventInfo.intIfNum);

  /* validate the data */
  if (pHandle == L7_NULL)
  {
    rc = L7_FAILURE;
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error: intIfNum=%u", eventInfo.intIfNum);
  }
  else if (eventInfo.component >= L7_LAST_COMPONENT_ID)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: Component(%d) out of range in nimEventIntfNotify\n",eventInfo.component);
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Component(%d) out of range in nimEventIntfNotify (intIfNum=%u)",eventInfo.component, eventInfo.intIfNum);
  }
  else if (eventInfo.event >= L7_LAST_PORT_EVENT)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: Event(%d) out of range in nimEventIntfNotify\n",eventInfo.event);
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Event(%d) out of range in nimEventIntfNotify (intIfNum=%u)",eventInfo.event, eventInfo.intIfNum);
  }
  else if ((rc = nimEventCorrelatorCreate(correlator)) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: Failed to get a correlator in nimNotify\n");
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Failed to get a correlator in nimNotify (intIfNum=%u)", eventInfo.intIfNum);
    rc = L7_FAILURE;
  }
  else
  {
    /* turn off the port */
    if (eventInfo.event == L7_DETACH)
    {
    if (nimIsMacroPort(eventInfo.intIfNum))
    {
          rc = dtlIntfAdminStateSet(eventInfo.intIfNum, L7_FALSE);
    }
    }

    /* pack the message */
    pdu.msgType = NIM_MSG;
    pdu.data.message.correlator = *correlator;
    memcpy((void*)&pdu.data.message.eventInfo,&eventInfo,sizeof(NIM_EVENT_NOTIFY_INFO_t));

    /* send the message to NIM_QUEUE */
    if (osapiMessageSend( nimCtlBlk_g->nimMsgQueue, (void *)&pdu,
                          (L7_uint32)sizeof(nimPdu_t), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM ) == L7_ERROR)
    {
      NIM_LOG_MSG("NIM: failed to send message to NIM message Queue.\n");
      LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: failed to send message to NIM message Queue (intIfNum=%u)", eventInfo.intIfNum);
      nimTraceEventError(eventInfo.component,eventInfo.event,eventInfo.intIfNum,NIM_ERR_LAST);
      rc = L7_FAILURE;
    }
    else
    {
      nimTracePortEvent(eventInfo.component,eventInfo.event,eventInfo.intIfNum,L7_TRUE,*pHandle);
      nimProfilePortEvent(eventInfo.component,eventInfo.event,eventInfo.intIfNum,L7_TRUE);
      rc = L7_SUCCESS;
    }
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Create a correlator for the event
*
* @param    correlator        @b{(output)}  A returned correlator
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t nimEventCorrelatorCreate(NIM_CORRELATOR_t *correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 index;


  if (correlator == L7_NULL)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

    index = lastCorrelatorTaken;
    index++; /* start at the next correlator */

    for (; index < maxCorrelators; index++)
    {
      if (correlatorInUse[index] == L7_FALSE )
      {
        correlatorInUse[index] = L7_TRUE;
        break;
      }
    }

    if (index == maxCorrelators)
    {
      /*
       * We must not have found a correlator
       * start at the beginning
       */
      index = 1;

      for (; index < lastCorrelatorTaken ; index++)
      {
        if (correlatorInUse[index] == L7_FALSE )
        {
          correlatorInUse[index] = L7_TRUE;
          break;
        }
      }
    } /* index == maxCorrelators */

    if ((index == lastCorrelatorTaken) || (index == maxCorrelators))
    {
      /* No correlator available at this time */
      *correlator = 0;
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;
      *correlator = index;
      lastCorrelatorTaken = index;
    }

    osapiSemaGive(nimEventSema);
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Delete a correlator for the event
*
* @param    correlator        @b{(input)}  The correlator to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t nimEventCorrelatorDelete(NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

  if (correlator < maxCorrelators)
  {
    correlatorInUse[correlator] = L7_FALSE;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(nimEventSema);

  return(rc);
}

/*********************************************************************
* @purpose  Initialize the Event Handler Resources
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimEventHdlrInit()
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {
    maxCorrelators = nimSidMsgCountGet() * 2;

    if ((nimEventSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: failed to create the event semaphore\n");
      rc = L7_FAILURE;
      break;
    }

    correlatorInUse = osapiMalloc (L7_NIM_COMPONENT_ID, maxCorrelators * sizeof (NIM_CORRELATOR_t));

    (void)memset((void*)correlatorInUse,0,sizeof(NIM_CORRELATOR_t)*maxCorrelators);

    (void)memset((void*)&correlatorTable,0,sizeof(nimCorrelatorData_t));

    correlatorTable.remainingMask = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));

    if (correlatorTable.remainingMask == L7_NULL)
    {
      NIM_LOG_ERROR("NIM: unable to alloc memory for correlator table.\n");
      rc = L7_FAILURE;
    }
    else
    {
      memset((void*)correlatorTable.remainingMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));
    }

    correlatorTable.failedMask = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));

    if (correlatorTable.failedMask == L7_NULL)
    {
      NIM_LOG_ERROR("NIM: unable to alloc memory for correlator table.\n");
      rc = L7_FAILURE;
    }
    else
    {
      memset((void*)correlatorTable.failedMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));
    }

    maxCorrelatorsInUse = 0;

    lastCorrelatorTaken = 0;

    /* create queue for receiving responses from sync messages */
    pNimEventStatusQueue = osapiMsgQueueCreate("NIM EVENT RESPONSE QUEUE", 1, sizeof(NIM_EVENT_STATUS_MSG_t));

    if (pNimEventStatusQueue == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: unable to create the status quueue.\n");
      rc = L7_ERROR;
      break;
    }

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Clear the data to a reset state
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    No data is freed
*
* @end
*
*********************************************************************/
L7_RC_t nimEventHdlrDataClear()
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {
    osapiSemaTake(nimEventSema,L7_WAIT_FOREVER);

    (void)memset((void*)correlatorInUse,0,sizeof(NIM_CORRELATOR_t)*maxCorrelators);


    correlatorTable.correlator = 0;
    correlatorTable.inUse = L7_FALSE;
    (void)memset((void*)&correlatorTable.requestData,0,sizeof(NIM_EVENT_NOTIFY_INFO_t));
    correlatorTable.response = L7_SUCCESS;
    correlatorTable.time = 0;

    memset((void*)correlatorTable.remainingMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));
    memset((void*)correlatorTable.failedMask,0,sizeof(L7_uint32) * ((L7_LAST_COMPONENT_ID/32) +1));

    maxCorrelatorsInUse = 0;

    lastCorrelatorTaken = 0;

  } while ( 0 );

  osapiSemaGive(nimEventSema);

  return rc;
}

/*********************************************************************
* @purpose  Remove the Event Handler Resources
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
void nimEventHdlrFini()
{
  if (nimEventSema != L7_NULLPTR)
  {
    (void)osapiSemaDelete(nimEventSema);

    if (correlatorTable.remainingMask != L7_NULL)
    {
      (void)osapiFree(L7_NIM_COMPONENT_ID, correlatorTable.remainingMask);
    }

    osapiFree (L7_NIM_COMPONENT_ID, correlatorInUse);
    (void)memset((void*)&correlatorTable,0,sizeof(nimCorrelatorData_t));
  }
}

/*********************************************************************
* @purpose  Remove the Event Handler Resources
*
* @param    status    @b{(input)}   The returned value from the caller
*
* @param    complete  @b{(output)}  Boolean value indicating whether event is complete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimEventTally(NIM_EVENT_COMPLETE_INFO_t status,L7_BOOL *complete)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mask = 0;
  L7_uint32 intIndex,bitIndex;
  L7_uint32 index;
  L7_uint32 maxComps = 0;

  intIndex = status.component/32;
  bitIndex = status.component%32;
  maxComps = (L7_LAST_COMPONENT_ID / 32) + 1;

  if (status.correlator != correlatorTable.correlator)
  {
    NIM_LOG_ERROR("NIM: Unexpected status callback on correlator(%d), event(%d), intIf(%d)\n",
                  status.correlator,status.event,status.intIfNum);
    LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Unexpected status callback on correlator(%d), event(%d), intIf(%d)",
             status.correlator,status.event,status.intIfNum);
    rc = L7_FAILURE;
  }
  else
  {
    correlatorTable.remainingMask[intIndex] &= ~(1 << bitIndex);

    for (index = 0;index < maxComps;index++)
    {
      mask |= correlatorTable.remainingMask[index];
    }

    *complete = (mask == 0)?L7_TRUE:L7_FALSE;

    if (mask != 0)
    {
      LOG_INFO(LOG_CTX_PTIN_INTF, "Not complete: mask=0x%08x", mask);
    }

    if (status.response.rc != L7_SUCCESS)
    {
      correlatorTable.response = status.response.rc;
      correlatorTable.failedMask[intIndex] |= (1 << bitIndex);
      NIM_LOG_ERROR("NIM: Component(%d) failed on event(%d) for intIfNum(%d)\n",
                    status.component,status.event,status.intIfNum);
      LOG_ERR(LOG_CTX_PTIN_INTF, "NIM: Component(%d) failed on event(%d) for intIfNum(%d)",
               status.component,status.event,status.intIfNum);
    }
  }

  return(rc);
}


/*********************************************************************
* @purpose  Post Processor for Events
*
* @param    eventInfo    @b{(input)}   The event information as assigned by the generator
* @param    status       @b{(output)}  The status of the event by either Tally or Timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
void nimEventPostProcessor(NIM_EVENT_NOTIFY_INFO_t eventInfo, NIM_NOTIFY_CB_INFO_t status)
{
  L7_INTF_STATES_t currState, nextState;

  NIM_CRIT_SEC_WRITE_ENTER();

  currState = nimUtilIntfStateGet(eventInfo.intIfNum);

  if (status.response.rc == L7_SUCCESS)
  {
    switch (eventInfo.event)
    {
      case L7_CREATE:
        if (nimUtilIntfNextStateGet(currState,L7_CREATE_COMPLETE,&nextState) == L7_SUCCESS)
        {
          nimUtilIntfStateSet(eventInfo.intIfNum,nextState);
        }

        NIM_INTF_SETMASKBIT(nimCtlBlk_g->createdMask,eventInfo.intIfNum);

        /* set the appropriate bit interface specific mask */
        switch (nimCtlBlk_g->nimPorts[eventInfo.intIfNum].sysIntfType)
        {
          case L7_PHYSICAL_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->physicalIntfMask,eventInfo.intIfNum);
            break;
          case L7_CPU_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->cpuIntfMask,eventInfo.intIfNum);
            break;
          case L7_LAG_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->lagIntfMask,eventInfo.intIfNum);
            break;
          case L7_LOGICAL_VLAN_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->vlanIntfMask,eventInfo.intIfNum);
            break;
          case L7_CAPWAP_TUNNEL_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->l2tnnlIntfMask,eventInfo.intIfNum);
            break;
          /* PTin added: virtual ports */
          case L7_VLAN_PORT_INTF:
            NIM_INTF_SETMASKBIT(nimCtlBlk_g->vlanportIntfMask,eventInfo.intIfNum);
            break;
          default:
            break;
        }

        /* shouldn't increment for pseudo interfaces like tunnels... */
        nimCtlBlk_g->ifNumber++;

        NIM_CRIT_SEC_WRITE_EXIT();
        break;

      case L7_DETACH:
        if (nimUtilIntfNextStateGet(currState,L7_DETACH_COMPLETE,&nextState) == L7_SUCCESS)
        {
          nimUtilIntfStateSet(eventInfo.intIfNum,nextState);
        }

        NIM_INTF_CLRMASKBIT(nimCtlBlk_g->presentMask,eventInfo.intIfNum);

        NIM_CRIT_SEC_WRITE_EXIT();
        break;

      case L7_ATTACH:
        if (nimUtilIntfNextStateGet(currState,L7_ATTACH_COMPLETE,&nextState) == L7_SUCCESS)
        {
          nimUtilIntfStateSet(eventInfo.intIfNum,nextState);
        }

        NIM_INTF_SETMASKBIT(nimCtlBlk_g->presentMask,eventInfo.intIfNum);

        NIM_CRIT_SEC_WRITE_EXIT();

        /* More work needed for the ATTACH process */
        nimEventAttachPostProcess(eventInfo);

        break;
      case L7_DELETE:
        if (nimUtilIntfNextStateGet(currState,L7_DELETE_COMPLETE,&nextState) == L7_SUCCESS)
        {
          nimUtilIntfStateSet(eventInfo.intIfNum,nextState);
        }


        NIM_INTF_CLRMASKBIT(nimCtlBlk_g->createdMask,eventInfo.intIfNum);

        /* set the appropriate bit interface specific mask */
        switch (nimCtlBlk_g->nimPorts[eventInfo.intIfNum].sysIntfType)
        {
          case L7_PHYSICAL_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->physicalIntfMask,eventInfo.intIfNum);
            break;
          case L7_CPU_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->cpuIntfMask,eventInfo.intIfNum);
            break;
          case L7_LAG_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->lagIntfMask,eventInfo.intIfNum);
            break;
          case L7_LOGICAL_VLAN_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->vlanIntfMask,eventInfo.intIfNum);
            break;
          case L7_CAPWAP_TUNNEL_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->l2tnnlIntfMask,eventInfo.intIfNum);
            break;
          /* PTin added: virtual ports */
          case L7_VLAN_PORT_INTF:
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->vlanportIntfMask,eventInfo.intIfNum);
            break;
          default:
            break;
        }

        /* shouldn't decrement for pseudo interfaces like tunnels... */
        nimCtlBlk_g->ifNumber--;

        NIM_CRIT_SEC_WRITE_EXIT();

        /* Now we can successfully delete the interface */
        nimDeleteInterface(eventInfo.intIfNum);

        break;

      default:
        NIM_CRIT_SEC_WRITE_EXIT();
        break;
    }
  }
  else
  {
    NIM_CRIT_SEC_WRITE_EXIT();

    NIM_LOG_ERROR("NIM: Failed event(%d), intIfNum(%d)\n",
                  eventInfo.event, eventInfo.intIfNum);
  }

  /* notify the event generator if there was a callback assigned */
  if (eventInfo.pCbFunc != L7_NULLPTR)
  {
    status.event    = eventInfo.event;
    status.intIfNum = eventInfo.intIfNum;
    eventInfo.pCbFunc(status);
  }

  nimTracePortEvent(eventInfo.component,eventInfo.event,eventInfo.intIfNum,L7_FALSE,status.handle);
}

/*********************************************************************
* @purpose  Request to create an interface
*
* @param    *pRequest       @b{(input)}   pointer to nimIntfCreateRequest_t structure
* @param    *pOutput         @b{(output)}  The returned data for this create request
*
*
* @returns  one of L7_RC_t enum values
*
* @notes  This routine is used to request the creation of an interface.
*         All interfaces must be created using this mechanism.
*
*
*         *pIntfIdInfo - This should contain sufficient information to uniquely
*                       identify the interface in the system.
*
*         *pIntfDescr  - This is a pointer to static information that describes this
*                       interface. This information MUST be completely specified
*                       with the create request.
*                       based on FD_NIM_DEFAULT* values.
*
*         *pDefaultCfg - This is a pointer to the information that would be stored
*                       in this particular port's configuration file if configuration
*                       were reset to default values.
*
*                       If this pointer is null, NIM presumes default configuration
*                       based on FD_NIM_DEFAULT* values.
*
*                       Note that all ports of the same type should have the same
*                       default configuration, although this methodology gives a
*                       system integrator flexibility on different devices.
*
*                       The default configuration will be applied on a subsequent
*                       L7_ATTACH command if no non-volatile configuration exists for
*                       this interface.
*
*
*
* @end
*********************************************************************/
L7_RC_t   nimIntfCreate(nimIntfCreateRequest_t *pRequest, nimIntfCreateOutput_t *pOutput)
{
  nimUSP_t  usp;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 *intIfNum;
  L7_uint32 port;
  nimConfigIdTreeData_t  configIdInfo;
  nimConfigID_t          *intfIdInfo;  /* unique interface specification info */
  nimIntfDescr_t         *intfDescr;   /* unique interface descriptor  info */

  intIfNum = pOutput->intIfNum;
  intfIdInfo = pRequest->pIntfIdInfo;
  intfDescr = pRequest->pIntfDescr;
  *intIfNum = 0;

  NIM_CRIT_SEC_WRITE_ENTER();

  do
  {
    /* No interfaces can be created until we are in Execute */
    if ( nimPhaseStatusCheck() != L7_TRUE)
    {
      rc = L7_FAILURE;
      break;
    }
    else if ((pRequest == L7_NULLPTR) || (pOutput == L7_NULLPTR) ||
             (pRequest->pIntfDescr == L7_NULLPTR) ||
             (pRequest->pIntfIdInfo == L7_NULLPTR))
    {
      NIM_LOG_ERROR("NIM: Null data in call to nimIntfCreate\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      rc = L7_SUCCESS;
    }

    memcpy((void *)&usp,&pRequest->pIntfIdInfo->configSpecifier.usp,sizeof(nimUSP_t));

    switch (pRequest->pIntfIdInfo->type)
    {
      case L7_PHYSICAL_INTF:
      case L7_CPU_INTF:
      case L7_STACK_INTF:
        break;

      case L7_LAG_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in intfId must be 1 based and it's range needs to be
         * determined as 1 - platIntfLagIntfMaxCountGet()
         */

        /*
         * Need to perform a LAG create for DTL
         * the LAG U and S need to be determined via CMGR
         */
        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotLagSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.dot3adIntf;
        break;

      case L7_LOGICAL_VLAN_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in intfId must be 1 based and it's range needs to be
         * determined as 1 - platIntfVlanIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotVlanSlotNumGet ();

        if (nimPortInstanceNumGet(*pRequest->pIntfIdInfo, &port) == L7_SUCCESS)
          usp.port = port;
        else
          usp.port = platIntfMaxCountGet() + 1;  /* Set to an invalid value */
        break;

      case L7_LOOPBACK_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in loopbackId is 0 based and it's range needs to be
         * determined as 1 - platIntfLoopbackIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotLoopbackSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.loopbackId + 1;
        break;

      case L7_TUNNEL_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in tunnelId is 0 based and it's range needs to be
         * determined as 1 - platIntfTunnelIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotTunnelSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.tunnelId + 1;
        break;

      case L7_WIRELESS_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in tunnelId is 0 based and it's range needs to be
         * determined as 1 - platIntfTunnelIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotwirelessNetSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.wirelessNetId;
        break;

      case L7_CAPWAP_TUNNEL_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in tunnelId is 0 based and it's range needs to be
         * determined as 1 - platIntfTunnelIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotL2TunnelSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.l2tunnelId;
        break;

      /* PTin added: virtual ports */
      case L7_VLAN_PORT_INTF:
        /*
         * get the u and s from cmgr, port is the id of the interface
         * The passed in tunnelId is 0 based and it's range needs to be
         * determined as 1 - platIntfTunnelIntfMaxCountGet()
         */

        usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;
        usp.slot = (L7_uchar8)platSlotVlanPortSlotNumGet ();
        usp.port = pRequest->pIntfIdInfo->configSpecifier.vlanportId;
        break;

      default:
        NIM_LOG_MSG("NIM: Unknown interface type in nimIntfCreate\n");
        rc = L7_FAILURE;
        break;
    }

    /* if we have not gotten here successfully, return */
    if (rc != L7_SUCCESS) break;

    L7_NIM_IFDESCRINFO_SET(intfIdInfo, intfDescr->ianaType,
                                intfDescr->ifName,
                                intfDescr->ifDescr,
                                intfDescr->ifLongName);

    NIM_CRIT_SEC_WRITE_EXIT();

    /* quick check to see if the interface is created */
    if ((rc = nimGetIntIfNumFromUSP(&usp,intIfNum)) != L7_ERROR)
    {
      NIM_LOG_MSG("NIM: Interface already created, %d.%d.%d\n",usp.unit,usp.slot,usp.port);
      rc = L7_ERROR;
    }
    else if (pRequest->pIntfIdInfo->type >= L7_MAX_INTF_TYPE_VALUE)
    {
      NIM_LOG_MSG("Invalid Interface Type in create request\n");
      rc = L7_FAILURE;
    }
    else if (nimNumberOfInterfaceExceeded(pRequest->pIntfIdInfo->type) == L7_TRUE)
    {
      NIM_LOG_MSG("NIM: Number of interface of type(%d) exceeded during create\n",pRequest->pIntfIdInfo->type);
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;
    }

    NIM_CRIT_SEC_WRITE_ENTER();

    if (rc != L7_SUCCESS)
    {
        /* Note that we must break out of the loop while holding the write lock.
        */
        break;
    }

    if (nimIntIfNumCreate(*pRequest->pIntfIdInfo,intIfNum) != L7_SUCCESS)
    {
      NIM_LOG_MSG("NIM: Failed to create the internal interface number\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset((void*)&nimCtlBlk_g->nimPorts[*intIfNum],0,sizeof(nimIntf_t));
    }

    /* set the fast lookup for USP to intIfNum */
    if (nimUnitSlotPortToIntfNumSet(&usp,*intIfNum) != L7_SUCCESS)
    {
      NIM_LOG_MSG("NIM: Failed to set the mapping of USP to intIfNum fast lookup\n");
      rc = L7_FAILURE;

      break;
    }
    else
    {
      /* set the intIfNums of the interface */
      nimCtlBlk_g->nimPorts[*intIfNum].intfNo = *intIfNum;

      nimCtlBlk_g->nimPorts[*intIfNum].runTimeMaskId = *intIfNum;

      NIM_CONFIG_ID_COPY(&nimCtlBlk_g->nimPorts[*intIfNum].configInterfaceId,pRequest->pIntfIdInfo);

      nimCtlBlk_g->nimPorts[*intIfNum].sysIntfType = pRequest->pIntfIdInfo->type;

      nimIfIndexCreate(usp,pRequest->pIntfIdInfo->type,&nimCtlBlk_g->nimPorts[*intIfNum].ifIndex,*intIfNum);

      nimCtlBlk_g->nimPorts[*intIfNum].usp = usp;

      /* copy the interface characteristics from the caller's request */
      memcpy(&nimCtlBlk_g->nimPorts[*intIfNum].operInfo,pRequest->pIntfDescr,sizeof(nimIntfDescr_t));

      /* remaining items in the interface are: present, resetTime, linkChangeTime, currentLoopbackState, intfState */

      /* Get the default config for the interface */
      if (pRequest->pDefaultCfg == L7_NULLPTR)
      {
        /* no defaultConfig was supplied by the caller; therefore, use NIMs default for the type */
        nimConfigDefaultGet(pRequest->pIntfDescr,&nimCtlBlk_g->nimPorts[*intIfNum].defaultCfg);
      }
      else
      {
        /* use the callers supplied default config */
        memcpy(&nimCtlBlk_g->nimPorts[*intIfNum].defaultCfg,pRequest->pDefaultCfg,sizeof(nimIntfConfig_t));
        /* This field not set by caller */
        nimCtlBlk_g->nimPorts[*intIfNum].defaultCfg.mgmtAdminState = FD_NIM_ADMIN_STATE;
      }

      NIM_INTF_CLRMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[*intIfNum].runTimeMaskId);

      nimCtlBlk_g->nimPorts[*intIfNum].present = L7_TRUE;


      /* check to see if we have a saved config for the interface */
      if ((rc = nimConfigSet(&nimCtlBlk_g->nimPorts[*intIfNum],NIM_CFG_VER_CURRENT)) != L7_SUCCESS)
      {
        NIM_LOG_MSG("NIM: Failed to set the config for interface\n");
        rc = L7_FAILURE;
      }
      else if ((rc = nimUtilIntfStateSet(*intIfNum,L7_INTF_CREATING)) != L7_SUCCESS)
      {
        NIM_LOG_MSG("NIM: Failed to set intf state to L7_INTF_CREATING\n");
        rc = L7_FAILURE;
      }
      else
      {
        rc = L7_SUCCESS;
      }
    }

  } while ( 0 );

  if (rc != L7_SUCCESS)
  {
    if ((*intIfNum != 0) && (rc != L7_ERROR))
    {
      /* rewind the data for the create */
      (void)nimUtilIntfStateSet(*intIfNum,L7_INTF_UNINITIALIZED);
      nimIntIfNumDelete(*intIfNum);
      nimUnitSlotPortToIntfNumSet(&usp,0);
      memset((void*)&nimCtlBlk_g->nimPorts[*intIfNum],0,sizeof(nimIntf_t));


      memset(&configIdInfo, 0 , sizeof( configIdInfo));
      NIM_CONFIG_ID_COPY(&configIdInfo.configId, pRequest->pIntfIdInfo);
      configIdInfo.intIfNum = *intIfNum;
      (void)nimConfigIdTreeEntryDelete( &configIdInfo);
    }
    NIM_CRIT_SEC_WRITE_EXIT();
  }
  else
  {
    if (nimCtlBlk_g->nimHighestIntfNumber < *intIfNum)
    {
      nimCtlBlk_g->nimHighestIntfNumber = *intIfNum;
    }
    /* give back the sema before calling out to the other components */
    NIM_CRIT_SEC_WRITE_EXIT();

    /* Counters must be created outside of the critical section
    ** in order to avoid deadlocks with the stats semaphore.
    */
    if (nimInterfaceCounters(*intIfNum,L7_TRUE) != L7_SUCCESS)
    {
      nimCtlBlk_g->nimPorts[*intIfNum].present = L7_FALSE;

      NIM_LOG_MSG("NIM: Create error for Physical Interface \n");
      LOG_EVENT(*intIfNum);
      rc = (L7_FAILURE);
    }

    if (rc != L7_SUCCESS)
    {
      NIM_LOG_MSG("NIM: Failed to notify the components of L7_CREATE event\n");

      NIM_CRIT_SEC_WRITE_ENTER();

      /* rewind the data for the create */
      (void)nimUtilIntfStateSet(*intIfNum,L7_INTF_UNINITIALIZED);
      nimIntIfNumDelete(*intIfNum);
      nimUnitSlotPortToIntfNumSet(&usp,0);
      memset((void*)&nimCtlBlk_g->nimPorts[*intIfNum],0,sizeof(nimIntf_t));


      memset(&configIdInfo, 0 , sizeof( configIdInfo));
      NIM_CONFIG_ID_COPY(&configIdInfo.configId, pRequest->pIntfIdInfo);
      configIdInfo.intIfNum = *intIfNum;
      (void)nimConfigIdTreeEntryDelete( &configIdInfo);

      NIM_CRIT_SEC_WRITE_EXIT();
    }
    else
    {

      NIM_CRIT_SEC_WRITE_ENTER();

      nimCtlBlk_g->numberOfInterfacesByType[pRequest->pIntfIdInfo->type]++;

      nimCtlBlk_g->nimNumberOfPortsPerUnit[(L7_uint32)usp.unit]++;

      NIM_CRIT_SEC_WRITE_EXIT();
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Notify the system application layer of changes in interfaces
*
* @param    unit        unit number (stack unit)
* @param    slot        slot number
* @param    port        port number
* @param    cardType    a card descriptor as given to Card Manager from HPC
* @param    event       an event designator such as L7_ENABLE
* @param    interfaceType   the type of interface such as PHYSICAL
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t   nimCmgrNewIntfChangeCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                                       L7_uint32 cardType,L7_PORT_EVENTS_t event,
                                       SYSAPI_HPC_PORT_DESCRIPTOR_t *portData)
{
  L7_RC_t                rc = L7_FAILURE;
  nimIntfCreateRequest_t pRequest;
  L7_uint32   intIfNum;
  NIM_HANDLE_t  handle;
  NIM_INTF_CREATE_INFO_t eventInfo;

  nimConfigID_t          pIntfIdInfo;  /* unique interface specification info */
  nimIntfDescr_t         pIntfDescr;   /* unique interface descriptor  info */
  nimIntfCreateOutput_t  output;

  /* Initialize data structures */

  memset((void *)&pIntfIdInfo,0,sizeof(nimConfigID_t));
  memset((void *)&eventInfo,0,sizeof(NIM_INTF_CREATE_INFO_t));
  memset((void *)&pIntfDescr,0,sizeof(nimIntfDescr_t));
  memset((void *)&pRequest,0,sizeof(nimIntfCreateRequest_t));

  output.handle =   &handle;
  output.intIfNum = &intIfNum;

  eventInfo.component = L7_CARDMGR_COMPONENT_ID;
  eventInfo.pCbFunc   = nimEventCmgrDebugCallback;

  /* setup the config ID */
  pIntfIdInfo.configSpecifier.usp.unit = unit;
  pIntfIdInfo.configSpecifier.usp.slot = slot;
  pIntfIdInfo.configSpecifier.usp.port = port;

  /* setup the request block pointers */
  pRequest.pDefaultCfg  = L7_NULLPTR;
  pRequest.pIntfDescr   = &pIntfDescr;
  pRequest.pIntfIdInfo  = &pIntfIdInfo;
  pRequest.pCreateInfo  = &eventInfo;

  switch (portData->type)
  {
    case  L7_IANA_OTHER_CPU:
      pIntfIdInfo.type = L7_CPU_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
               " CPU Interface for Unit:", unit, "Slot:", slot, "Port:", port);

        sprintf (pIntfDescr.ifName, "%s %d/%d/%d",
               "CPU Interface: ", unit, slot, port);
      }
      else
      {
        sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s %d %s %d",
                   " CPU Interface for", "Slot:", slot, "Port:", port);

        sprintf (pIntfDescr.ifName, "%s %d/%d",
                   "CPU Interface: ", slot, port);
      }
      break;

    case L7_IANA_LAG_DESC:
      pIntfIdInfo.type = L7_LAG_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {

          (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
          (void)sprintf (pIntfDescr.ifName, "%d/%d",
                     slot, port);
      }
      break;

    case  L7_IANA_ETHERNET:
    case  L7_IANA_FAST_ETHERNET:
    case  L7_IANA_FAST_ETHERNET_FX:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                         "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                         slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                     (L7_char8 *)(pIntfDescr.ifDescr),
                     IANA_FAST_ETHERNET_DESC);



      break;

    case  L7_IANA_GIGABIT_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                   (L7_char8 *)(pIntfDescr.ifDescr),
                   IANA_GIGABIT_ETHERNET_DESC);


      break;

    /* PTin added: Speed 2.5G */
    case  L7_IANA_2G5_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                   (L7_char8 *)(pIntfDescr.ifDescr),
                   IANA_2G5_ETHERNET_DESC);


      break;
    /* PTin end */

    case  L7_IANA_10G_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                     (L7_char8 *)(pIntfDescr.ifDescr),
                     IANA_10G_ETHERNET_DESC);


      break;

    /* PTin added: Speed 40G */
    case  L7_IANA_40G_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                   (L7_char8 *)(pIntfDescr.ifDescr),
                   IANA_40G_ETHERNET_DESC);

      break;

    /* PTin added: Speed 100G */
    case  L7_IANA_100G_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                     "Unit:", unit, "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                     unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);


        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                   (L7_char8 *)(pIntfDescr.ifDescr),
                   IANA_100G_ETHERNET_DESC);

      break;

    /* PTin end */

    case L7_IANA_L2_VLAN:
      pIntfIdInfo.type = L7_LOGICAL_VLAN_INTF;
      break;
    default:
      NIM_LOG_MSG("NIM: Unknown interface type\n");
      return(L7_FAILURE);
  }

  if ((rc = cmgrIfaceMacGet((L7_INTF_TYPES_t)pIntfIdInfo.type,(L7_uint32)unit,(L7_uint32)slot,port,pIntfDescr.macAddr.addr, pIntfDescr.l3MacAddr.addr)) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: unable to retrieve the MAC address for %d.%d.%d\n",(L7_uint32)unit,(L7_uint32)slot,port);
    rc = L7_FAILURE;
    return(rc);
  }

  pIntfDescr.configurable   = L7_TRUE;
  pIntfDescr.settableParms  = L7_INTF_PARM_ADMINSTATE | L7_INTF_PARM_MTU |
                              L7_INTF_PARM_MACADDR | L7_INTF_PARM_LINKTRAP |
                              L7_INTF_PARM_LOOPBACKMODE |
                              L7_INTF_PARM_MACROPORT | L7_INTF_PARM_ENCAPTYPE;
  switch (pIntfIdInfo.type)
  {
    case L7_PHYSICAL_INTF:
      pIntfDescr.settableParms |= L7_INTF_PARM_AUTONEG | L7_INTF_PARM_SPEED |
                                  L7_INTF_PARM_FRAMESIZE;

      if ((portData->type == L7_IANA_10G_ETHERNET) &&
          (portData->phyCapabilities & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_TRUE)
      {
        pIntfDescr.settableParms &= ~L7_INTF_PARM_AUTONEG;
      }

      break;

    case L7_LAG_INTF:
      pIntfDescr.settableParms |= L7_INTF_PARM_FRAMESIZE;
      break;

    default:
      /* do nothing */
      break;
  }

  pIntfDescr.connectorType  = portData->connectorType;
  pIntfDescr.defaultSpeed   =  portData->defaultSpeed;
  pIntfDescr.frameSize.largestFrameSize = 1500;
  pIntfDescr.ianaType       = portData->type;
  pIntfDescr.internal       = L7_FALSE;
  pIntfDescr.phyCapability  = portData->phyCapabilities;
  memset((void*)&pIntfDescr.macroPort,0,sizeof(nimMacroPort_t));
  memcpy (&pIntfDescr.bcastMacAddr,  &L7_ENET_BCAST_MAC_ADDR, 6);


  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: Attempted event (%d), on USP %d.%d.%hu before phase 3\n",event,unit,slot,port);
    return(rc);
  }

  if ((unit < 1) || (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) || (port < 1))
  {
    NIM_LOG_MSG("NIM: attempted event (%d) with invalid USP, %d.%d.%hu\n",event,unit,slot,port);
    return(rc);
  }

  if (event == L7_CREATE)
  {
    if (nimIntfCreate(&pRequest,&output) == L7_SUCCESS)
    {
      NIM_LOG_MSG("Success in create\n");
    }
    else
    {
      NIM_LOG_MSG("Failed in create\n");
    }
  }
  else
  {
    NIM_LOG_MSG("NIM: BAD event for my test\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Post Processor for ATTACH events
*
* @param    eventInfo    @b{(input)}   The event information as assigned by the generator
*
* @returns  none
*
* @notes    none
*
* @end
*
*********************************************************************/
void nimEventAttachPostProcess(NIM_EVENT_NOTIFY_INFO_t eventInfo)
{
  L7_BOOL                 isLinkUp;
  NIM_EVENT_NOTIFY_INFO_t attachEventInfo;
  NIM_HANDLE_t            handle;
  L7_RC_t                 rc = L7_SUCCESS;

  if ((rc = dtlIntfLinkStateGet(eventInfo.intIfNum, &isLinkUp)) == L7_SUCCESS)
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    if (isLinkUp == L7_TRUE)
    {
      NIM_INTF_SETMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[eventInfo.intIfNum].runTimeMaskId);

      NIM_CRIT_SEC_WRITE_EXIT();

      /*
       * don't need to keep the handle around
       * need to notify the rest of the system of the link state
       */
      attachEventInfo.event         = L7_UP;
      attachEventInfo.component     = L7_NIM_COMPONENT_ID;
      attachEventInfo.intIfNum      = eventInfo.intIfNum;
      attachEventInfo.pCbFunc       = L7_NULLPTR;

      if (nimEventIntfNotify(attachEventInfo,&handle) != L7_SUCCESS)
      {
        NIM_LOG_MSG("NIM: Failed to send LINK UP on queue\n");
        rc = L7_FAILURE;
      }
    }
    else
    {
      NIM_INTF_CLRMASKBIT(nimCtlBlk_g->linkStateMask, nimCtlBlk_g->nimPorts[eventInfo.intIfNum].runTimeMaskId);

      NIM_CRIT_SEC_WRITE_EXIT();
    }

  }
  else
    rc = L7_FAILURE;

}


void nimDebugCompsLeft()
{
  int i,j,mask;
  L7_uint32 compId;
  printf("Components: ");

  for (i = 0; i < (L7_LAST_COMPONENT_ID / 32) ; i++)
  {
    mask = correlatorTable.remainingMask[i];
    for (j = 0; j < 32; j++)
    {
      if (mask & (1 << j))
      {
        compId = (i * 32) + j;
        printf("%s (%d),", cnfgrSidComponentNameGet(compId), compId);
      }
    }
  }
}


