
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_main.c
*
* @purpose   Dynamic ARP Inspection main task
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create 08/20/2007
*
* @author Kiran Kumar Kella
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "trapapi.h"

#include "dai_cfg.h"
#include "dai_util.h"
#include "dai_cnfgr.h"

#include "dot1q_api.h"
#include "fdb_api.h"
#include "dhcp_snooping_api.h"
#include "l7_ip_api.h"
/* #include "l3end_api.h" */
#include "l7_vrrp_api.h"
#include "usmdb_util_api.h"

#include "logger.h"
#include "ptin_evc.h"
#include "ptin_intf.h"

#define DAI_TIMER_EVENT_INTERVAL      1000

extern daiCfgData_t     *daiCfgData;
extern daiCnfgrState_t  daiCnfgrState;
extern osapiRWLock_t    daiCfgRWLock;

extern L7_uchar8 *daiInitStateNames[];

extern void             *daiMsgQSema;
extern void             *dai_Event_Queue;
extern void             *dai_Packet_Queue;

L7_BOOL ptin_debug_dai_snooping = 0;

void ptin_debug_dai_enable(L7_BOOL enable)
{
  ptin_debug_dai_snooping = enable;
}

static L7_uchar8 nullIPAddr[L7_IP_ADDR_LEN] = {0,0,0,0};
static L7_uchar8 bcstIPAddr[L7_IP_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff};
static L7_uchar8 bcstMacAddr[L7_ENET_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};

static osapiTimerDescr_t   *daiTimer             = L7_NULLPTR;

extern daiInfo_t       *daiInfo;

/* Array of per interface working data. Indexed on internal interface number */
extern daiIntfInfo_t *daiIntfInfo;

/* Array of per vlan working data. Indexed on router interface number */
extern daiVlanInfo_t *daiVlanInfo;

/* API to get the ARP Acl entry given the ACL name */
extern arpAclCfg_t* _arpAclEntryGet(L7_uchar8 *aclName);

/* PTin Added - Routing support */
extern L7_uint16 ptin_ipdtl0_dtl0Vid_get(L7_uint16 dtl0Vid);

#if 0
/* Not used as vlan events are now a mask and it is not appropriate to have a array */
static L7_uchar8 *vlanEvents[] = {"Invalid", "ADD", "DELETE PENDING", "DELETE", "ADD PORT",
                                  "DELETE PORT", "START TAGGING PORT", "STOP TAGGING PORT",
                                  "INITIALIZED", "RESTORE"};
#endif
L7_uchar8 *arpMsgTypeNames[] = {"UNKNOWN", "REQUEST", "RESPONSE"};

L7_RC_t daiVlanEventProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);

static void daiTimerCallback(void);

/*********************************************************************
* @purpose  Main function for the Dynamic ARP Inspection. Read incoming
*           messages (events and ARP packets) and process accordingly.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void daiTask(void)
{
  daiEventMsg_t eventMsg;
  daiFrameMsg_t frameMsg;

  /* Loop forever, processing incoming messages */
  while (L7_TRUE)
  {
    /* Since we are reading from multiple queues, we cannot wait forever
     * on the message receive from each queue. Use a semaphore to indicate
     * whether any queue has data. Wait until data is available. */
    if (osapiSemaTake(daiMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
             "Failure taking DAI queue semaphore.");
      continue;
    }
    memset(&eventMsg, 0, sizeof(daiEventMsg_t));
    memset(&frameMsg, 0, sizeof(daiFrameMsg_t));

    /* Always try to read from the event queue first. */
    if (osapiMessageReceive(dai_Event_Queue, &eventMsg,
                            sizeof(daiEventMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      /* Configuration actions are all done on the UI thread.
       * Synchronize that processing with event processing by
       * taking the same semaphore here. */
      switch (eventMsg.msgType)
      {
        case DAI_VLAN_EVENT:
          if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            daiVlanEventProcess(&eventMsg.daiMsgData.vlanEvent.vlanData,
                               eventMsg.daiMsgData.vlanEvent.intIfNum,
                               eventMsg.daiMsgData.vlanEvent.vlanEventType);
            osapiWriteLockGive(daiCfgRWLock);
          }
          break;

        case DAI_CNFGR_INIT:
          if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            daiCnfgrHandle(&eventMsg.daiMsgData.cmdData);
            osapiWriteLockGive(daiCfgRWLock);
          }
          break;

        case DAI_TIMER_EVENT:
          if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            daiRateLimitCheck();
            osapiWriteLockGive(daiCfgRWLock);
          }
          break;
        case DAI_INTF_EVENT:
           if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
           {
             daiIntfChangeProcess(eventMsg.daiMsgData.intfEvent.intIfNum,
                                  eventMsg.daiMsgData.intfEvent.event,
                                  eventMsg.daiMsgData.intfEvent.correlator);
             osapiWriteLockGive(daiCfgRWLock);
           }
          break;
        case DAI_NIM_STARTUP:
          if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.daiMsgData.nimStartupEvent.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
            {
              daiNimCreateStartup();
            }
            else if (eventMsg.daiMsgData.nimStartupEvent.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
            {
              daiNimActivateStartup();
            }
            else
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
                      "Unknown NIM startup event %d", eventMsg.daiMsgData.nimStartupEvent.startupPhase);
            }
            osapiWriteLockGive(daiCfgRWLock);
          }
          break;

        default:
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
                 "Unknown event message type received in the DAI thread.");
          break;
      }
    }

    /* If the event queue was empty, see if an ARP packet has arrived. */
    else if (osapiMessageReceive(dai_Packet_Queue, &frameMsg,
                                 sizeof(daiFrameMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        daiFrameProcess(frameMsg.rxIntf, frameMsg.vlanId, frameMsg.innerVlanId,
                        frameMsg.frameBuf, frameMsg.dataLen);
        osapiWriteLockGive(daiCfgRWLock);
      }
    }

    /* Someone gave the msg queue semaphore, but didn't put a message on
     * either queue. Count it. */
    else
    {
      daiInfo->debugStats.msgRxError++;
    }
  } /* end while true */

  return;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void daiIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  daiEventMsg_t   msg;
  L7_RC_t rc;

  if (daiCfgData->daiTraceFlags & DAI_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DAI_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DAI_MAX_TRACE_LEN,
                  "DAI received NIM %s startup event",
                  startupPhase == NIM_INTERFACE_CREATE_STARTUP ? "CREATE" : "ACTIVATE");
    daiTraceWrite(traceBuf);
  }

  if (daiCnfgrState != DAI_PHASE_EXECUTE)
  {
    nimStartupEventDone(L7_DAI_COMPONENT_ID);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
            "Received NIM startup %d callback while in init state %s",
            startupPhase, daiInitStateNames[daiCnfgrState]);
    return;
  }

  memset(&msg, 0, sizeof(daiEventMsg_t));
  msg.msgType = DAI_NIM_STARTUP;
  msg.daiMsgData.nimStartupEvent.startupPhase = startupPhase;

  rc = osapiMessageSend(dai_Event_Queue, &msg, sizeof(daiEventMsg_t),
                        L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
  if(rc == L7_SUCCESS)
  {
    osapiSemaGive(daiMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DAI_COMPONENT_ID,
            "Failed to send NIM startup phase %d to DAI thread",
            startupPhase);
  }
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    DAI has no configuration at this point.
*
* @end
*********************************************************************/
L7_RC_t daiNimCreateStartup(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (daiIntfIsValid(intIfNum))
    {
      daiIntfCreate(intIfNum);
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_DELETE);

  nimRegisterIntfEvents(L7_DAI_COMPONENT_ID, daiInfo->nimEventMask);

  nimStartupEventDone(L7_DAI_COMPONENT_ID);

  if (daiCfgData->daiTraceFlags & DAI_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DAI_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DAI_MAX_TRACE_LEN,
                  "DAI has processed NIM create startup callback");
    daiTraceWrite(traceBuf);
  }

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
L7_RC_t daiNimActivateStartup(void)
{
  L7_RC_t          rc;
  L7_uint32        intIfNum;
  L7_INTF_STATES_t intIfState;

  if (daiCfgData->daiTraceFlags & DAI_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DAI_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DAI_MAX_TRACE_LEN,
                  "DAI beginning NIM activate startup");
    daiTraceWrite(traceBuf);
  }

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (daiIntfIsValid(intIfNum))
    {
      intIfState = nimGetIntfState(intIfNum);

      switch (intIfState)
      {
        case L7_INTF_ATTACHED:
          daiIntfAttach(intIfNum);
          break;

        default:
          break;
      }
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_DELETE);
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(daiInfo->nimEventMask, L7_DETACH);

  nimRegisterIntfEvents(L7_DAI_COMPONENT_ID, daiInfo->nimEventMask);

  nimStartupEventDone(L7_DAI_COMPONENT_ID);

  if (daiCfgData->daiTraceFlags & DAI_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DAI_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DAI_MAX_TRACE_LEN,
                  "DAI completed processing NIM activate startup");
    daiTraceWrite(traceBuf);
  }

  /* Tell Configurator that hardware update by DAI component is done */
  cnfgrApiComponentHwUpdateDone(L7_DAI_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);

  daiInfo->l2HwUpdateComplete = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  event      event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Enqueues the interface event onto the processing queue.
*           The processing task handles the event through
*           daiIntfChangeProcess()
*
* @end
*
*********************************************************************/
L7_RC_t daiIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  daiEventMsg_t msg;

  if (  (event != L7_CREATE) &&
         (event != L7_DELETE)&&
         (event != L7_ATTACH)&&
         (event != L7_DETACH)

       ) /* No need to process any other NIM event than these */
  {
    status.intIfNum     = intIfNum;
    status.response.rc  = L7_SUCCESS;
    status.event        = event;
    status.component    = L7_DAI_COMPONENT_ID;
    status.correlator   = correlator;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(daiEventMsg_t) );

  msg.daiMsgData.intfEvent.intIfNum = intIfNum;
  msg.daiMsgData.intfEvent.event = event;
  msg.daiMsgData.intfEvent.correlator = correlator;
  msg.msgType    = DAI_INTF_EVENT;

  if (dai_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
           "DAI event queue is NULL when processing Interface change.");
    return L7_FAILURE;
  }

  if ( (rc = osapiMessageSend(dai_Event_Queue, &msg, sizeof(daiEventMsg_t),
                        L7_NO_WAIT,
                        L7_MSG_PRIORITY_NORM )) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DAI_COMPONENT_ID,
              "DAI: NIM event Enqueue failed.\n");
      daiInfo->debugStats.eventMsgTxError++;
  }

  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(daiMsgQSema);
  }
  return rc;
}




/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanData     @b{(input)}   VLAN ID or Vlan Mask
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    process on own thread
*
* @end
*********************************************************************/
L7_RC_t daiVlanChangeCallback(dot1qNotifyData_t *vlanDataCopy, L7_uint32 intIfNum,
                              L7_uint32 event)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  daiEventMsg_t msg;
  L7_uint32 vid = 0;
  L7_uint32 vlanId;
  L7_uint32 numVlans = 0;
  L7_uint32 tempNumVlans, vlanIdTemp = 0;
  dot1qNotifyData_t vlanData;

  if (dai_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
           "DAI event queue is NULL when processing VLAN change.");
    return L7_FAILURE;
  }

  /* Ignore events types we don't care about.  */
  if ((event != VLAN_DELETE_PENDING_NOTIFY) &&
      (event != VLAN_ADD_PORT_NOTIFY) &&
      (event != VLAN_DELETE_PORT_NOTIFY))
  {
    return L7_SUCCESS;
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Ignore interfaces we don't care about. */
  if (intIfNum && !daiIntfIsValid(intIfNum))
  {
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }
  memcpy(&vlanData, vlanDataCopy, sizeof(dot1qNotifyData_t));
  tempNumVlans = vlanData.numVlans;

  for (vid = 1; vid<=L7_VLAN_MAX_MASK_BIT; vid++)
  {
    if (vlanData.numVlans == 1)
    {
      vlanId = vlanData.data.vlanId;
      /* For any continue, we will break out */
      vid = L7_VLAN_MAX_MASK_BIT + 1;
    }
    else
    {
      if (L7_VLAN_ISMASKBITSET(vlanData.data.vlanMask,vid))
      {
         vlanId = vid;
      }
      else
      {
         if (numVlans == tempNumVlans)
        {
          /* Already taken care of all the bits in the mask so break out of for loop */
          break;
        }
        else
        {
          /* Vlan is not set check for the next bit since there are more bits that are set*/
          continue;
        }
      }
    }
    /* Ignore events for VLANs where DAI is not enabled. */
    if (!_daiVlanEnableGet(vlanId))
    {
       if (vlanData.numVlans == 1)
       {
         osapiReadLockGive(daiCfgRWLock);
         return L7_SUCCESS;
       }
       else
       {
         /*Reset the bit in the mask and continue */
         L7_VLAN_CLRMASKBIT(vlanData.data.vlanMask, vid);
         tempNumVlans--;
         continue;
       }
    }
    else
    {
      /* reserve a VLAN ID to check there exists only one DAI enabled VLANID */
      vlanIdTemp = vlanId;
    }

    /* Only trace VLAN events we care about */
    if (daiCfgData->daiTraceFlags & DAI_TRACE_VLAN_EVENTS)
    {
      L7_uchar8 daiTrace[DAI_MAX_TRACE_LEN];
      osapiSnprintf(daiTrace, DAI_MAX_TRACE_LEN,
                    "DAI received %d event for VLAN %u ",
                    event, vlanId);
      if (intIfNum)
      {
        L7_uchar8 intfStr[64];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiSnprintf(intfStr, 64, "and interface %s.", ifName);
        strncat(daiTrace, intfStr, 64);
      }
      daiTraceWrite(daiTrace);
    }

    /* Have to process the VLAN delete pending on the dot1q thread. If we
     * wait to process on own thread, dot1q won't be able to tell us which
     * ports were members of the VLAN because the VLAN will be gone already! */
    if (event == VLAN_DELETE_PENDING_NOTIFY)
    {
      if (_daiVlanEnableGet(vlanId))
      {
        /* If DAI is enabled on deleted VLAN */
        if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
        {
          for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
          {
            if (L7_INTF_ISMASKBITSET(portMask, i))
            {
              L7_BOOL intfIsInspected = daiIntfIsInspected(i);

              daiIntfInfo[i].daiNumVlansEnabled--;
              if (intfIsInspected && !daiIntfIsInspected(i))
              {
                daiIntfDisable(i);
              }
            }
          }
        }
      }
    }
    numVlans++;
  }

  vlanData.numVlans = tempNumVlans;
  if (tempNumVlans == 1)
  {
    /* If only one vlan Id with DAI enabled is found,
     * then move that vlanId to the appropriate place holder
     * in the vlanData structure. */
    vlanData.data.vlanId = vlanIdTemp;
  }

  if (event != VLAN_DELETE_PENDING_NOTIFY && vlanData.numVlans > 0)
  {

    /* process event on our thread */
    memset( (void *)&msg, 0, sizeof(daiEventMsg_t) );
    msg.msgType = DAI_VLAN_EVENT;
    memcpy(&msg.daiMsgData.vlanEvent.vlanData, &vlanData, sizeof(dot1qNotifyData_t));
    msg.daiMsgData.vlanEvent.intIfNum = intIfNum;
    msg.daiMsgData.vlanEvent.vlanEventType = event;

    rc = osapiMessageSend(dai_Event_Queue, &msg, sizeof(daiEventMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc == L7_SUCCESS)
    {
      osapiSemaGive(daiMsgQSema);
    }
    else
    {
      daiInfo->debugStats.eventMsgTxError++;
    }

  }
  osapiReadLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Process VLAN event caught in daiVlanChangeCallback().
*
* @param    vlanId     @b{(input)}   VLAN ID
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called on DAI thread context.
*
* @end
*********************************************************************/
L7_RC_t daiVlanEventProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 i, vlanId = 0, numVlans = 0;

  /* Remember port state before VLAN event */
  L7_BOOL intfIsInspected = daiIntfIsInspected(intIfNum);

  for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
  {
    if (vlanData->numVlans == 1)
    {
      vlanId = vlanData->data.vlanId;
      /* For any continue, we will break out */
      i = L7_VLAN_MAX_MASK_BIT + 1;
    }
    else
    {
      if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i))
      {
        vlanId = i;
      }
      else
      {
        if (numVlans == vlanData->numVlans)
        {
          /* Already taken care of all the bits in the mask so break out of for loop */
          break;
        }
        else
        {
          /* Vlan is not set check for the next bit since there are more bits that are set*/
          continue;
        }
      }
    }

    switch (event)
    {
      case VLAN_ADD_PORT_NOTIFY:
        if (_daiVlanEnableGet(vlanId))
        {
          /* DAI enabled on this VLAN. Increment VLAN count on port. */
          daiIntfInfo[intIfNum].daiNumVlansEnabled++;
          if (!intfIsInspected && daiIntfIsInspected(intIfNum))
          {
            /* Adding port to the VLAN enabled DAI on the port. */
            daiIntfEnable(intIfNum);
          }
        }
        break;

      case VLAN_DELETE_PORT_NOTIFY:
        if (_daiVlanEnableGet(vlanId))
        {
          if (daiIntfInfo[intIfNum].daiNumVlansEnabled != 0)
          {
            daiIntfInfo[intIfNum].daiNumVlansEnabled--;
            if (intfIsInspected && !daiIntfIsInspected(intIfNum))
            {
              /* Removing port from VLAN disabled DAI on VLAN. */
              daiIntfDisable(intIfNum);
            }
          }
          else
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
                    "DAI VLAN count on interface %s is bad.", ifName);
          }
        }
        break;

      case VLAN_DELETE_PENDING_NOTIFY:
        /* handled in callback */
        break;

      default:
        /* do nothing */
        break;
    }
    numVlans++;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Intf event caught in daiIntfChangeCallback().
*
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   Intf event (see L7_PORT for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called on DAI thread context.
*
* @end
*********************************************************************/
L7_RC_t daiIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
   L7_RC_t rc = L7_SUCCESS;
   NIM_EVENT_COMPLETE_INFO_t status;

   status.intIfNum     = intIfNum;
   status.component    = L7_DAI_COMPONENT_ID;
   status.response.rc  = L7_SUCCESS;
   status.event        = event;
   status.correlator   = correlator;

  /* Ignore interfaces we don't care about. */
  if (intIfNum && !daiIntfIsValid(intIfNum))
  {
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  if (DAI_IS_READY)
  {
    switch (event)
    {
      case L7_CREATE:
        daiIntfCreate(intIfNum);
    break;

     case L7_DELETE:
       rc  = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
       break;

     case L7_ATTACH:
       daiIntfAttach(intIfNum);
       break;

     case L7_DETACH:
       daiIntfTrustApply(intIfNum, FD_DAI_TRUST);
       break;

     default:
       rc  = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
       break;
    }
  }
  else
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DAI_COMPONENT_ID,
             "%s: %d: daiIntfChangeProcess(): Received a interface callback while outside the EXECUTE state", __FILE__, __LINE__);
        rc = L7_FAILURE;

  }
  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;

}


/*********************************************************************
* @purpose  Start the DAI timer
*
* @param    none
*
* @returns  L7_SUCCESS
*           L7_FAILURE if timer already running
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t daiTimerStart(void)
{
  osapiTimerAdd((void *)daiTimerCallback, L7_NULL, L7_NULL,
                DAI_TIMER_EVENT_INTERVAL, &daiTimer);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset the received packet count at the start of
*           new 1 second interval on each interface.
*           And count the consecutive intervals with rate_limit hit
*
* @param    none
*
* @returns  void
*
* @notes    This function gets called every 1 second
*
* @end
*********************************************************************/
void daiRateLimitCheck(void)
{
  L7_uint32 i;

  for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    if (daiCfgData->intfCfg[i].rate_limit != L7_DAI_RATE_LIMIT_NONE)
    {
      /* Above rate limit */
      if (daiIntfInfo[i].pktRxCount >= (L7_uint32)daiCfgData->intfCfg[i].rate_limit)
      {
        daiIntfInfo[i].consecutiveInterval++;

        /* If blocking packets, reset counter to maximum value */
        if (daiIntfInfo[i].blockedInterval > 0)
        {
          daiIntfInfo[i].blockedInterval = daiCfgData->intfCfg[i].burst_interval*2;
        }
      }
      /* Below rate limit */
      else
      {
        daiIntfInfo[i].consecutiveInterval = 0;

        /* Decrement blocking interval */
        if (daiIntfInfo[i].blockedInterval > 0)
        {
          daiIntfInfo[i].blockedInterval--;

          if (daiIntfInfo[i].blockedInterval == 0)
          {
            if (ptin_debug_dai_snooping)
              LOG_DEBUG(LOG_CTX_DAI, "ARP rate limiter unblocked for intIfNum %u.", i);
          }
        }
      }
    }
    else
    {
      daiIntfInfo[i].consecutiveInterval = 0;
      daiIntfInfo[i].blockedInterval = 0;
    }

    daiIntfInfo[i].pktRxCount = 0;
  }

  /* reschedule the timer */
  osapiTimerAdd((void *)daiTimerCallback, L7_NULL, L7_NULL,
                DAI_TIMER_EVENT_INTERVAL, &daiTimer);
}

/*********************************************************************
* @purpose  Check if the configured rate limit is exceeded on this
*           interface
*
* @param    pduInfo  @b{(input)} pointer to pdu info structure
*                                which stores intIfNum and vlanId
*
* @returns  L7_TRUE  - pkt rate > rate limit
*                     (or) the interface is down
*
* @returns  L7_FALSE  - pkt rate < rate limit
*                       or if the rate limit is none
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL rateLimitFilter(sysnet_pdu_info_t *pduInfo)
{
  L7_uint32 linkState;

  /* We shall directly ask NIM in this intercept function to know the
   * exact status of the interface in the h/w. This will ensure,
   * we drop those packets received when the interface is
   * being made error-disabled */
  if(! ((nimGetIntfLinkState(pduInfo->intIfNum, &linkState) == L7_SUCCESS) &&
        (linkState == L7_UP)) )
  {
    return L7_TRUE;
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Enforce the rate limit check on the packets received on this interface */
  if(daiCfgData->intfCfg[pduInfo->intIfNum].rate_limit != L7_DAI_RATE_LIMIT_NONE)
  {
    daiIntfInfo[pduInfo->intIfNum].pktRxCount++;

    /* PTin added: DAI */
    #if 1
    if (daiIntfInfo[pduInfo->intIfNum].blockedInterval > 0)
    {
      /* Drop packet */
      osapiReadLockGive(daiCfgRWLock);
      return L7_TRUE;
    }
    #endif

    if((daiIntfInfo[pduInfo->intIfNum].consecutiveInterval >=
        daiCfgData->intfCfg[pduInfo->intIfNum].burst_interval) &&
        (daiIntfInfo[pduInfo->intIfNum].pktRxCount >=
        (L7_uint32)daiCfgData->intfCfg[pduInfo->intIfNum].rate_limit))
    {
      /* Error disable this interface by calling NIM API.
       * User has to explicitly do "no shutdown"
       * to bring the interface up */
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pduInfo->intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DAI_COMPONENT_ID,
              "DAI Interface %s diagnostically-disabled!! Rate Limit %d pps with burst interval %d hit",
              ifName, daiCfgData->intfCfg[pduInfo->intIfNum].rate_limit,
              daiCfgData->intfCfg[pduInfo->intIfNum].burst_interval);

      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DAI_COMPONENT_ID,
              "User has to bring the interface %s up explicitly",ifName);

      /* PTin removed: DAI - Do not disable interface... only drop packet */
      #if 0
      daiIntfInfo[pduInfo->intIfNum].pktRxCount = 0;
      /* Disable the i/f in h/w */
      nimSetIntfAdminState(pduInfo->intIfNum, L7_DIAG_DISABLE);
      #endif
      daiIntfInfo[pduInfo->intIfNum].blockedInterval = daiCfgData->intfCfg[pduInfo->intIfNum].burst_interval*2;

      /* Raise a trap for the error disabling event */
      trapMgrDaiIntfErrorDisabledTrap(pduInfo->intIfNum);

      osapiReadLockGive(daiCfgRWLock);
      return L7_TRUE;
    }
  }

  osapiReadLockGive(daiCfgRWLock);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Queue an ARP packet for processing on our own thread
*
* @purpose  Apply the additional SRC-MAC, DST-MAC, IP checks on the
*           ARP packets in the sysnet callback itself and drop such
*           packets. We don't want to queue such packets to daiTask
*           as they would eat up netbuffers unnecessarily.
*
* @param    data      @b{(input)} ethernet frame
* @param    vlanId    @b{(input)} Vlan Id
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE    if frame is filtered
*           L7_FALSE   if frame is not filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL macIpAddrFilter(L7_uchar8 *data, L7_uint32 vlanId, L7_uint32 intIfNum)
{
  L7_uint32 offset, spa, tpa;
  L7_enetHeader_t *eth_header;
  L7_ether_arp_t *arp_pkt;

  offset = sysNetDataOffsetGet(data);
  eth_header = (L7_enetHeader_t *)data;
  arp_pkt = (L7_ether_arp_t *)(data + offset);

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  /* Drop packets if Ethernet Source MAC doesn't match sender MAC in
   * the ARP body of ARP requests/responses */
  if(daiCfgData->daiVerifySMac)
  {
    if(memcmp(&(eth_header->src),arp_pkt->arp_sha,L7_ENET_MAC_ADDR_LEN) != 0)
    {
      daiLogAndDropPacket(data, vlanId, intIfNum, SOURCE_MAC_FAILURE);
      osapiReadLockGive(daiCfgRWLock);
      return L7_TRUE;
    }
  }

  /* Drop packets if Ethernet Destination MAC doesn't match target MAC in
   * the ARP body of ARP responses */
  if(daiCfgData->daiVerifyDMac)
  {
    if((arp_pkt->arp_op == L7_ARPOP_REPLY) &&
       (memcmp(&(eth_header->dest), arp_pkt->arp_tha, L7_ENET_MAC_ADDR_LEN) != 0))
    {
      daiLogAndDropPacket(data, vlanId, intIfNum, DEST_MAC_FAILURE);
      osapiReadLockGive(daiCfgRWLock);
      return L7_TRUE;
    }
  }

  /* Drop packets if Sender IP addresses in ARP requests/responses or
   * target IP addresses in ARP responses are invalid/unexpected IP addresses.
   * Such addresses include 0.0.0.0, 255.255.255.255, IP multicast addresses,
   * Class E address (240.0.0.0/4) and Loopback addresses (127.0.0.0/8) */
  spa = GET_IP_FROM_PKT(arp_pkt->arp_spa);
  tpa = GET_IP_FROM_PKT(arp_pkt->arp_tpa);

  if(daiCfgData->daiVerifyIP)
  {
    if(((memcmp(arp_pkt->arp_spa, nullIPAddr, L7_IP_ADDR_LEN) == 0) ||
        (memcmp(arp_pkt->arp_spa, bcstIPAddr, L7_IP_ADDR_LEN) == 0) ||
        ((spa & L7_CLASS_E_ADDR_MASK) == L7_CLASS_E_ADDR_MASK) ||
        ((spa & L7_IP_LOOPBACK_ADDR_NETWORK) == L7_IP_LOOPBACK_ADDR_NETWORK) ||
    (L7_IP4_IN_MULTICAST(spa))) ||

       ((arp_pkt->arp_op == L7_ARPOP_REPLY) &&
        ((memcmp(arp_pkt->arp_tpa, nullIPAddr, L7_IP_ADDR_LEN) == 0) ||
         (memcmp(arp_pkt->arp_tpa, bcstIPAddr, L7_IP_ADDR_LEN) == 0) ||
         ((tpa & L7_CLASS_E_ADDR_MASK) == L7_CLASS_E_ADDR_MASK) ||
         ((tpa & L7_IP_LOOPBACK_ADDR_NETWORK) == L7_IP_LOOPBACK_ADDR_NETWORK) ||
         (L7_IP4_IN_MULTICAST(tpa)))))
    {
      daiLogAndDropPacket(data, vlanId, intIfNum, IP_VALID_FAILURE);
      osapiReadLockGive(daiCfgRWLock);
      return L7_TRUE;
    }
  }

  osapiReadLockGive(daiCfgRWLock);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Intercept incoming ARP packets.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if packet to be forwarded
*           SYSNET_PDU_RC_IGNORED   if packet should continue normal processing
*           SYSNET_PDU_RC_DISCARD   if packet falls through the cracks
*           SYSNET_PDU_RC_COPIED    if broadcast destination MAC
*
* @notes    This function is executed on the DTL thread.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t daiArpRecv(L7_uint32 hookId,
                           L7_netBufHandle bufHandle,
                           sysnet_pdu_info_t *pduInfo,
                           L7_FUNCPTR_t continueFunc)
{
  L7_uint32 rc, len, routingEnabled = L7_DISABLE;
  L7_uchar8 *data;
  L7_uint32 vlanId   = pduInfo->vlanId, vlanIntf;
  L7_uint32 intIfNum = pduInfo->intIfNum, intfType;

  if (ptin_debug_dai_snooping)
    LOG_DEBUG(LOG_CTX_DAI, "Received ARP packet from intIfNum %u, vlan %u", intIfNum, vlanId);

  daiInfo->debugStats.pktsIntercepted++;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  {
    L7_uchar8 daiTrace[DAI_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    L7_ushort16    ethHdrLen = sysNetDataOffsetGet(data);
    L7_ether_arp_t *arp_pkt  = (L7_ether_arp_t*)(data + ethHdrLen);

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if (daiCfgData->daiTraceFlags & DAI_TRACE_FRAME_RX)
    {
      osapiSnprintf(daiTrace, DAI_MAX_TRACE_LEN,
                    "DAI Received ARP %s on interface %s in VLAN %u.",
                    arpMsgTypeNames[arp_pkt->arp_op], ifName, vlanId);
      daiTraceWrite(daiTrace);
    }

    if (daiCfgData->daiTraceFlags & DAI_TRACE_FRAME_RX_DETAIL)
    {
      osapiSnprintf(daiTrace, DAI_MAX_TRACE_LEN,
                    "Detailed Pkt: DAI Received ARP %s on interface %s in VLAN %u.",
                    arpMsgTypeNames[arp_pkt->arp_op], ifName, vlanId);
      daiLogEthernetHeader((L7_enetHeader_t*) data, DAI_TRACE_CONSOLE);
      daiLogArpPacket(data, vlanId, intIfNum, DAI_TRACE_CONSOLE);
    }

    if (! daiInfo->l2HwUpdateComplete)
    {
      /* ARP packet received before control plane is ready. Drop such packet */
      daiInfo->debugStats.msgsDroppedControlPlaneNotReady++;
      return SYSNET_PDU_RC_DISCARD;
    }
  }

  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_DAI, "intIfNum %u is not ready", intIfNum);
    return SYSNET_PDU_RC_IGNORED;
  }

  if (intfType == L7_PHYSICAL_INTF)
  {
    /* If the packet came up to CPU on a port based routing interface,
     * give it to ARP appln, as DAI is not supported on
     * port-based routing interfaces */
    if(ipMapRtrIntfModeGet(intIfNum, &routingEnabled) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_DAI, "intIfNum %u is not ready", intIfNum);
      return SYSNET_PDU_RC_IGNORED;
    }
    if(routingEnabled == L7_ENABLE)
    {
      daiInfo->debugStats.pktsOnPortRoutingIf++;
      daiInfo->debugStats.pktsToArpAppln++;
      if (ptin_debug_dai_snooping)
        LOG_DEBUG(LOG_CTX_DAI, "intIfNum %u is a routing port", intIfNum);
      return SYSNET_PDU_RC_IGNORED;
    }
  }

  /* If the ARP Packet is received on a trusted (configured in h/w)
   * port/lag incoming interface */
  if (! ((daiIntfIsInspected(intIfNum) == L7_TRUE) &&
         (_daiIntfTrustGet(intIfNum)   == L7_FALSE)) )
  {
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "intIfNum %u is trusted", intIfNum);

    /* If the packet came up to CPU on a Routing VLAN on a trusted
     * port/lag interface, give it to ARP appln */
    if((dot1qVlanIntfVlanIdToIntIfNum(vlanId, &vlanIntf) == L7_SUCCESS) &&
       (ipMapRtrIntfModeGet(vlanIntf, &routingEnabled) == L7_SUCCESS) &&
       (routingEnabled == L7_ENABLE))
    {
      daiInfo->debugStats.pktsOnVlanRoutingIf++;
      daiInfo->debugStats.pktsToArpAppln++;
      if (ptin_debug_dai_snooping)
        LOG_DEBUG(LOG_CTX_DAI, "intIfNum %u is a routing port", intIfNum);
      return SYSNET_PDU_RC_IGNORED;
    }

    /* If the packet came up to CPU on a switching VLAN on a trusted
     * port, give it to system directly */
    daiInfo->debugStats.pktsOnVlanNonRoutingIf++;

    /* Forward only packets received on mgmt vlan to CPU */
    if(pduInfo->vlanId == simMgmtVlanIdGet())
    {
      daiInfo->debugStats.pktsToSystem++;
      if (ptin_debug_dai_snooping)
        LOG_DEBUG(LOG_CTX_DAI, "VLAN %u is a management vlan", pduInfo->vlanId);
      return SYSNET_PDU_RC_IGNORED;
    }

    /* Packets received on non-mgmt and on non-routing vlans are not handled.
     * They are not meant for us if received on a trusted port */
    daiInfo->debugStats.pktsNotHandled++;
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "Packet not handled");
    return SYSNET_PDU_RC_IGNORED;
  }

  /****************************************************************/
  /*  If we reach here, it means the ARP packet is received       */
  /*  on an untrusted port. The ingress Vlan can either be enabled*/
  /*  or disabled for DAI.                                        */
  /****************************************************************/

  /* Check if the incoming pkt rate on the untrusted interface is
   * greater than the configured rate limit over a burst interval */
  if(rateLimitFilter(pduInfo) == L7_TRUE)
  {
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "Packet dropped because of rate limiter");
    return SYSNET_PDU_RC_DISCARD;
  }

  /* Perform additional mac,ip checks */
  if(macIpAddrFilter(data, vlanId, intIfNum) == L7_TRUE)
  {
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "Packet dropped because of invalid MAC/IP address");
    return SYSNET_PDU_RC_DISCARD;
  }

  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);

  /* Queue the copy of daiTask that shall do the rest of the
   * DAI processing (ARP ACL + DHCP Snooping database checks etc...) */
  if (ptin_debug_dai_snooping)
    LOG_DEBUG(LOG_CTX_DAI, "Queuing packet");
  rc = daiPacketQueue(data, len, pduInfo);
  if(rc == L7_SUCCESS)
  {
    /* We consumed the netbuffer, so free it */
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return SYSNET_PDU_RC_CONSUMED;
  }
 
  LOG_ERR(LOG_CTX_DAI, "Error queuing packet");

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Queue an ARP packet for processing on our own thread
*
* @param    ethHeader    @b{(input)} ethernet frame
* @param    dataLen      @b{(input)} length of ethernet frame
* @param    pduInfo      @b{(input)} pointer to pdu info structure
*                                    which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS if successfully queued
*           L7_FAILURE if queuing failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiPacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                       sysnet_pdu_info_t *pduInfo)
{
    daiFrameMsg_t daiFrameMsg;

    if(dataLen > DAI_PKT_ALLOWED_MAX_SIZE)
    {
      /* ARP packets size would be less than 64 bytes */
      daiInfo->debugStats.pktsInvalidLength++;
      dataLen = DAI_PKT_ALLOWED_MAX_SIZE;
    }

    memcpy(&daiFrameMsg.frameBuf, ethHeader, dataLen);

    daiFrameMsg.dataLen     = dataLen;
    daiFrameMsg.rxIntf      = pduInfo->intIfNum;
    daiFrameMsg.vlanId      = pduInfo->vlanId;
    daiFrameMsg.innerVlanId = pduInfo->innerVlanId;

    if (osapiMessageSend(dai_Packet_Queue, &daiFrameMsg, sizeof(daiFrameMsg_t), L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
      daiInfo->debugStats.pktsQueued++;
      osapiSemaGive(daiMsgQSema);
    }
    else
    {
      daiInfo->debugStats.frameMsgTxError++;
      return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Post TIMER event to daiTask in timeout
*
* @param    void
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void daiTimerCallback(void)
{
  daiEventMsg_t msg;
  L7_RC_t rc;

  if (dai_Event_Queue == L7_NULLPTR)
    return;

  /* process event on our thread */
  memset((void *)&msg, 0, sizeof(daiEventMsg_t) );
  msg.msgType = DAI_TIMER_EVENT;

  rc = osapiMessageSend(dai_Event_Queue, &msg, sizeof(daiEventMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(daiMsgQSema);
  }
  else
  {
    daiInfo->debugStats.eventMsgTxError++;
  }
}

/*********************************************************************
* @purpose  Hook the netbuffer to dtl ARP Receive routine
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    dataLen  @b{(input)} frame length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dtlArpPacketHook(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_netBufHandle bufHandle = L7_NULL;
  sysnet_pdu_info_t pduInfo;
  L7_uchar8        *dataStart;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if(bufHandle == L7_NULL)
  {
    daiInfo->debugStats.daiMbufFailures++;
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart,frame,dataLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLen);

  pduInfo.intIfNum = intIfNum;
  pduInfo.vlanId   = vlanId;

  daiInfo->debugStats.pktsToSystem++;
  if(dtlARPProtoRecv(bufHandle, &pduInfo) != L7_SUCCESS)
  {
    /* If frame is not consumed, free the buffer */
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }
}

/*********************************************************************
* @purpose  Hook the netbuffer to IPMAP for further processing
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    dataLen  @b{(input)} frame length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiIpMapPacketHook(L7_uint32 intIfNum, L7_ushort16 vlanId,
                     L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_netBufHandle bufHandle = L7_NULL;
  L7_uchar8        *dataStart;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if(bufHandle == L7_NULL)
  {
    daiInfo->debugStats.daiMbufFailures++;
    return;
  }

  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_DAI);
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart,frame,dataLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLen);

  daiInfo->debugStats.pktsToArpAppln++;
  if(ipMapArpRecvQueue(bufHandle, vlanId, intIfNum) != L7_SUCCESS)
  {
    /* If frame is not consumed by IPMAP, free the buffer */
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }
}

/*********************************************************************
* @purpose  Process an ARP packet on DAI task
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    dataLen     @b{(input)} frame length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                     L7_uchar8 *frame, L7_uint32 dataLen)
{
  daiFilterAction_t filterRc = DAI_FILTER_NONE;

  if (ptin_debug_dai_snooping)
    LOG_DEBUG(LOG_CTX_DAI, "Processing packet from intIfNum %u, VLAN %u", intIfNum, vlanId);

  /* Perform the validation checks on the queued ARP packets
   * that are received on untrusted ports, if the ingress VLAN
   * is enabled for DAI */
  if (_daiVlanEnableGet(vlanId))
  {
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "Received ARP with valid VLAN %u", vlanId);

    /* Filter ARP packets based on ARP ACL rules */
    filterRc = daiFrameARPAclFilter(intIfNum, vlanId, frame, dataLen);
    if(filterRc == DAI_FILTER_FAIL)
    {
      if (ptin_debug_dai_snooping)
        LOG_DEBUG(LOG_CTX_DAI, "ARP packet dropped after ACL validation");
      return;
    }
    else if(filterRc == DAI_FILTER_NONE)
    {
      /* Filter ARP packets based on match in DHCP Snooping bindings database */
      if (daiFrameDHCPSnoopingDbFilter(intIfNum, vlanId,
                                     frame, dataLen) == L7_TRUE)
      {
        if (ptin_debug_dai_snooping)
          LOG_DEBUG(LOG_CTX_DAI, "ARP packet dropped after DHCP snooping validation");
        return;
      }
    }
    daiInfo->debugStats.pktsValidated++;
    /* If we reach here, the ARP packet is verified to be a genuine one. */
  }
  else
  {
    if (ptin_debug_dai_snooping)
      LOG_DEBUG(LOG_CTX_DAI, "VLAN %u is not enabled", vlanId);
  }

  if (ptin_debug_dai_snooping)
    LOG_DEBUG(LOG_CTX_DAI, "ARP packet is going to be forwarded");

  /* (a) We shall flood this packet to other ports in this VLAN
   * (b) Give it to IPMAP if the ingress VLAN is a Routing VLAN */
  daiFrameForward(intIfNum, vlanId, innerVlanId, frame, dataLen);
}

/*********************************************************************
* @purpose  Apply ARP ACL rules on the received ARP packet
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  DAI_FILTER_NONE  if the packet is neither passed nor failed
*                            it means we can apply next filter on the packet
*           DAI_FILTER_PASS  if the packet is passed
*           DAI_FILTER_FAIL  if the packet is failed
*
* @notes    none
*
* @end
*********************************************************************/
daiFilterAction_t daiFrameARPAclFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                                       L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_uint32    /*i, ip_address,*/ senderIpAddr;
  //L7_uchar8    mac_address[L7_ENET_MAC_ADDR_LEN];
  L7_ushort16 ethHdrLen = sysNetDataOffsetGet(frame);
  L7_ether_arp_t *arp_pkt = (L7_ether_arp_t*)(frame + ethHdrLen);

  L7_uchar8 *aclName = daiCfgData->aclName[vlanId];
  arpAclCfg_t *acl = L7_NULL;

  if(osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX) == 0)
  {
    /* No ARP ACLs configured for check on this VLAN */
    return DAI_FILTER_NONE;
  }

  if((acl = _arpAclEntryGet(aclName)) == L7_NULL)
  {
    /* Configured ARP ACL doesn't exist */
    return DAI_FILTER_NONE;
  }

  senderIpAddr = GET_IP_FROM_PKT(arp_pkt->arp_spa);

  /* PTin modified: DAI */
  #if 1
  if (_ptin_arpAclRuleFindIndex(acl, senderIpAddr, arp_pkt->arp_sha) >= 0)
  {
    /* Match found, don't filter the packet */
    daiVlanInfo[vlanId].stats.aclPermits++;
    return DAI_FILTER_PASS;
  }
  #else
  /* If matching rule found, pass the packet */
  for(i = 0; i < L7_ARP_ACL_RULES_MAX; i++)
  {
    ip_address  = acl->rules[i].senderHostIp;
    memcpy(mac_address, acl->rules[i].senderHostMac, L7_ENET_MAC_ADDR_LEN);

    /* Reached end of the rules in this acl */
    if(ip_address == 0)
      break;

    /* Check the validity of Sender Address in ARP request/response */
    if(senderIpAddr == ip_address)
    {
      if(memcmp(arp_pkt->arp_sha, mac_address,
                L7_ENET_MAC_ADDR_LEN) == 0)
      {
        /* Match found, don't filter the packet */
        daiVlanInfo[vlanId].stats.aclPermits++;
        return DAI_FILTER_PASS;
      }
    }
  }
  #endif

  /* No matching rule exists in the ARP ACL for the ip address in ARP packet (or)
   * No rules at all configured in this ARP ACL.
   * If static flag is configured for this vlan, implicit deny is meant
   * to be explicit deny. So, drop such packet */
  if(_daiVlanStaticFlagGet(vlanId) == L7_TRUE)
  {
    daiLogAndDropPacket(frame, vlanId, intIfNum, ACL_MATCH_FAILURE);
    return DAI_FILTER_FAIL;
  }

  return DAI_FILTER_NONE;
}

/*********************************************************************
* @purpose  Check if a matching DHCP Snooping binding entry exists
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE  if frame is filtered by DHCP snooping (or)
*                    if DHCP snooping component doesn't exist
*           L7_FALSE if frame is not filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL daiFrameDHCPSnoopingDbFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                                     L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_uint32                 senderIpAddr;
  dhcpSnoopBinding_t        dsBinding;
  L7_ushort16 ethHdrLen   = sysNetDataOffsetGet(frame);
  L7_ether_arp_t *arp_pkt = (L7_ether_arp_t*)(frame + ethHdrLen);

  /* Filter ARP request / ARP response if the sender-IP + sender-MAC pair
   * is not present in the DHCP snooping bindings database */
  senderIpAddr = GET_IP_FROM_PKT(arp_pkt->arp_spa);
  memcpy(dsBinding.key.macAddr, arp_pkt->arp_sha, L7_ENET_MAC_ADDR_LEN);

  if(dsFuncTable.dsBindingGet == L7_NULL)
  {
    /* DHCP Snooping component doesn't seem to be present.
     * We need to drop the packets in this case, since we
     * depend on the validation of ARP packets on DHCP Snooping
     * binding entries when we reach here. */
    return L7_TRUE;
  }

  if(dsFuncTable.dsBindingGet(&dsBinding) != L7_SUCCESS)
  {
    /* No entry exists for the sender MAC. Filter the frame */
    daiLogAndDropPacket(frame, vlanId, intIfNum, DHCP_SNOOP_DB_MATCH_FAILURE);
    return L7_TRUE;
  }
  else
  {
    if((senderIpAddr == dsBinding.ipAddr) &&
       (vlanId       == dsBinding.vlanId) &&
       (intIfNum     == dsBinding.intIfNum))
    {
      /* Match found for the tuple {IP, MAC, VLAN, Port} */
      daiVlanInfo[vlanId].stats.dhcpPermits++;
      return L7_FALSE;
    }
    else
    {
      /* Entry exists for the MAC, but the tuple is mismatched.
       * Filter the frame */
      daiLogAndDropPacket(frame, vlanId, intIfNum, DHCP_SNOOP_DB_MATCH_FAILURE);
      return L7_TRUE;
    }
  }

  /* It shouldn't reach here */
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Forward a valid ARP packet.
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t daiFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                        L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_uint32 routingEnabled, rtrIntf, offset;
  L7_enetHeader_t *eth_header = (L7_enetHeader_t *)frame;
  L7_ether_arp_t *arp_pkt;

  L7_uchar8 systemMac[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 vMac[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 vidMac[L7_FDB_KEY_SIZE], vrId;
  dot1dTpFdbData_t fdbEntry;

  offset = sysNetDataOffsetGet(frame);
  arp_pkt = (L7_ether_arp_t *)(frame + offset);

  /* If this function is called, it means
   * (a) the packet is received on an untrusted(configured in h/w) port and
   *
   * (i) received VALID ARP packet on DAI enabled ingress VLAN (or)
   * (ii) received INVALID/VALID ARP packet on DAI disabled ingress VLAN
   */

  daiVlanInfo[vlanId].stats.forwarded++;

  /* In a Routing VLAN */
  if((dot1qVlanIntfVlanIdToIntIfNum(vlanId, &rtrIntf) == L7_SUCCESS) &&
     (ipMapRtrIntfModeGet(rtrIntf, &routingEnabled) == L7_SUCCESS) &&
     (routingEnabled == L7_ENABLE))
  {
    daiInfo->debugStats.pktsOnVlanRoutingIf++;
    if(arp_pkt->arp_op == L7_ARPOP_REQUEST)
    {
      /* Give a copy  of this ARP Request to ARP Appln */
      daiIpMapPacketHook(intIfNum, vlanId, frame, frameLen);
    }
    else
    {
      /* ARP Response destined to local interface MAC address or
       * destined to VRRP MAC address (for which if we are VRRP master)
       * should be given to ARP Appln */

      if(arp_pkt->arp_op == L7_ARPOP_REPLY)
      {
        if (nimGetIntfL3MacAddress(intIfNum, 0, intfMac) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
                 "Failure to get Interface MAC address for %s",ifName);
          return L7_FAILURE;
        }

        memset(vMac, 0, L7_ENET_MAC_ADDR_LEN);
        if((memcmp(intfMac, eth_header->dest.addr, L7_ENET_MAC_ADDR_LEN) == 0) ||
           (vrrpFuncTable.vrrpMapGetVMac &&
            (vrrpFuncTable.vrrpMapGetVMac(arp_pkt->arp_tpa, (L7_uchar8 *)vMac, &vrId) == L7_SUCCESS) &&
            (memcmp(eth_header->dest.addr, vMac, L7_ENET_MAC_ADDR_LEN) == 0)))
        {
          /* This is ARP Response sent to me. Give it to ARP Appln
           * and return from here */
          daiIpMapPacketHook(intIfNum, vlanId, frame, frameLen);
          return L7_SUCCESS;
        }
        /* If the DA MAC is broadcast address in ARP reply, give a copy to ARP Appln.
         * It can be the case of Gratuitous ARP reply sent by someone */
        if(memcmp(eth_header->dest.addr, bcstMacAddr, L7_ENET_MAC_ADDR_LEN) == 0)
        {
          /* Give a copy  of this ARP Request to ARP Appln */
          daiIpMapPacketHook(intIfNum, vlanId, frame, frameLen);
        }
      }
    }
  }
  else
  {
    daiInfo->debugStats.pktsOnVlanNonRoutingIf++;

    /* Forward only packets received on mgmt vlan to CPU */
    if(vlanId == simMgmtVlanIdGet())
    {
      if(arp_pkt->arp_op == L7_ARPOP_REQUEST)
      {
        /* ARP Request to bcast DA, give a copy of it to OS */
        dtlArpPacketHook(intIfNum, vlanId, frame, frameLen);
      }
      else
      {
        /* If the ARP Response is destined to our System MAC,
         * give it to OS and return from here */
        if(simGetSystemIPMacType() == L7_SYSMAC_BIA)
          simGetSystemIPBurnedInMac(systemMac);
        else
          simGetSystemIPLocalAdminMac(systemMac);

        if(arp_pkt->arp_op == L7_ARPOP_REPLY)
        {
          if(memcmp(systemMac, eth_header->dest.addr, 3 /*L7_ENET_MAC_ADDR_LEN*/) == 0)
          {
            dtlArpPacketHook(intIfNum, vlanId, frame, frameLen);
            return L7_SUCCESS;
          }
          /* If the DA MAC is broadcast address in ARP reply, give a copy to the system.
           * It can be the case of Gratuitous ARP reply sent by someone on the mgmt vlan */
          if(memcmp(eth_header->dest.addr, bcstMacAddr, L7_ENET_MAC_ADDR_LEN) == 0)
          {
            dtlArpPacketHook(intIfNum, vlanId, frame, frameLen);
          }
        }
      }
    }
  }

  /* If we reach here, it means the ARP packet has
   * to be L2 switched (unicast or flood) in the VLAN
   */

  /* Unicast the valid ARP Request/Reply packet to its destination
   * as is done in the h/w and return from here if passed */

  /* combine mac and vlanid to get an 8-byte vidMac address */
  memset(vidMac, 0, L7_FDB_KEY_SIZE);
  (void)usmDbEntryVidMacCombine(vlanId, eth_header->dest.addr, vidMac);

  if (ptin_debug_dai_snooping)
    LOG_TRACE(LOG_CTX_DAI,"VidMAC=%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x",
              vidMac[0], vidMac[1], vidMac[2], vidMac[3], vidMac[4], vidMac[5], vidMac[6], vidMac[7]);

  memset(&fdbEntry, 0, sizeof(fdbEntry));
  if(L7_SUCCESS == fdbFind(vidMac, L7_MATCH_EXACT, &fdbEntry))
  {
    return daiFrameUnicast(fdbEntry.dot1dTpFdbPort, vlanId, innerVlanId, fdbEntry.dot1dTpFdbVirtualPort, frame, frameLen);
  }

  /* If Destination mac is not found in FDB table, flood the ARP Request/Reply
   * packet to other ports in the VLAN as is done in the h/w */

  return daiFrameFlood(intIfNum, vlanId, innerVlanId, frame, frameLen);
}

/***********************************************************************
* @purpose Unicast ARP Response packet to its destination
*
* @param    outgoingIf   @b{(input)} outgoing interface number
* @param    vlanId       @b{(input)} VLAN ID
* @param    innerVlanId  @b{(input)} Inner VLAN ID
* @param    vport_id     @b{(input)} Virtual Port id
* @param    frame        @b{(input)} ethernet frame
* @param    frameLen     @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*
* @end
*
***********************************************************************/
L7_RC_t daiFrameUnicast(L7_uint32 outgoingIf, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                        L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_INTF_MASK_t portMask;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_INTF_TYPES_t   sysIntfType = 0;

  if (ptin_debug_dai_snooping)
    LOG_TRACE(LOG_CTX_DAI, "intIfNum=%u, vlanId=%u, innerVlanId=%u, vport_id=%u", outgoingIf, vlanId, innerVlanId, vport_id);

  nimGetIntfName(outgoingIf, L7_SYSNAME, ifName);

  /* Get interface type */
  nimGetIntfType(outgoingIf, &sysIntfType);

  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    /* Do not evaluate port, if it is a virtual port */
    if (sysIntfType == L7_VLAN_PORT_INTF || L7_INTF_ISMASKBITSET(portMask, outgoingIf))
    {
      if (daiFrameSend(outgoingIf, vlanId, innerVlanId, vport_id, frame, frameLen) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
             "Failure to unicast ARP Reply on interface %s in VLAN %d",
             ifName, vlanId);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
             "daiFrameUnicast: Outgoing interface %s is not member of VLAN %d",
             ifName, vlanId);
    }
  }

  daiInfo->debugStats.pktTxFailures++;
  return rc;
}

/***********************************************************************
* @purpose Flood ARP packets to all ports on this VLAN
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on ports(trusted and untrusted) in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t daiFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                      L7_uchar8 *frame, L7_ushort16 frameLen)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i, activeState = L7_INACTIVE;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_dai_snooping)
    LOG_TRACE(LOG_CTX_DAI, "intIfNum=%u, vlanId=%u, innerVlanId=%u", intIfNum, vlanId, innerVlanId);

  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        /* Don't flood back on the incoming interface */
        if(i != intIfNum)
        {
          if((nimGetIntfActiveState(i, &activeState) == L7_SUCCESS) &&
             (activeState == L7_ACTIVE))
          {
            /* Send on an interface that is link up and in forwarding state */
            if (daiFrameSend(i, vlanId, innerVlanId, 0, frame, frameLen) != L7_SUCCESS)
            {
              daiInfo->debugStats.pktTxFailures++;
              rc = L7_FAILURE;
            }
          }
        }
      }
    }
  }

  return rc;
}

/***********************************************************************
* @purpose Send an ARP packet on a given interface
*
* @param    intIfNum    @b{(input)} outgoing interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    vport_id    @b{(input)} Virtual Port id
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t daiFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                    L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  /* PTin added: DAI */
  #if 1
  L7_uint16           vlanId_list[16][2], number_of_vlans=0, i;
  L7_uint16           extOVlan = vlanId;
  L7_uint16           extIVlan = 0;
  #endif
  L7_RC_t rc = L7_SUCCESS;

  /* If outgoing interface is CPU interface, don't send it */
  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_CPU_INTF))
  {
    return L7_SUCCESS;
  }

  {
    L7_uchar8 daiTrace[DAI_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if (daiCfgData->daiTraceFlags & DAI_TRACE_FRAME_TX)
    {
      osapiSnprintf(daiTrace, DAI_MAX_TRACE_LEN,
                    "DAI forwarding %u byte frame on interface %s in VLAN %u.",
                    frameLen, ifName, vlanId);
      daiTraceWrite(daiTrace);
    }

    if (daiCfgData->daiTraceFlags & DAI_TRACE_FRAME_TX_DETAIL)
    {
      osapiSnprintf(daiTrace, DAI_MAX_TRACE_LEN,
                    "Detailed Pkt: DAI forwarding %u byte frame on interface %s in VLAN %u.",
                    frameLen, ifName, vlanId);
      daiLogEthernetHeader((L7_enetHeader_t*) frame, DAI_TRACE_CONSOLE);
      daiLogArpPacket(frame, vlanId, intIfNum, DAI_TRACE_CONSOLE);
    }
  }

  if (ptin_debug_dai_snooping)
    LOG_TRACE(LOG_CTX_DAI, "intIfNum=%u, vlanId=%u, innerVlanId=%u, vport_id=%u", intIfNum, vlanId, innerVlanId, vport_id);

  /* QUATTRO service? */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (vport_id != 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlanVPort(vlanId, vport_id, &intIfNum, &vlanId_list[0][0], &vlanId_list[0][1]) != L7_SUCCESS)
    {
      if (ptin_debug_dai_snooping)
        LOG_ERR(LOG_CTX_DAI, "Error obtaining Ext. VLANs for VLANs %u and VPort %u", vlanId, vport_id);
      return L7_FAILURE;
    }
    number_of_vlans = 1;
  }
  /* Quattro VLAN, but no vport? (flooding) */
  else if (ptin_evc_is_quattro_fromIntVlan(vlanId) && !ptin_evc_intf_isRoot(vlanId, intIfNum))
  {
    ptin_HwEthEvcFlow_t vport_flow;

    /* Get list of vlans (outer+inner) to be flooded */
    for (memset(&vport_flow, 0x00, sizeof(vport_id));
         ptin_evc_vlan_client_next(vlanId, intIfNum, &vport_flow, &vport_flow) == L7_SUCCESS && number_of_vlans < 16;
         number_of_vlans++)
    {
      vlanId_list[number_of_vlans][0] = vport_flow.uni_ovid;
      vlanId_list[number_of_vlans][1] = vport_flow.uni_ivid;
    }
  }
  /* Regular service */
  else
#endif
  {
    L7_BOOL   is_stacked;
    L7_uint8  port_type;

    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, vlanId, innerVlanId, &vlanId_list[0][0], &vlanId_list[0][1]) != L7_SUCCESS ||
        ptin_evc_check_is_stacked_fromIntVlan(vlanId, &is_stacked) != L7_SUCCESS ||
        ptin_evc_intf_type_get(vlanId, intIfNum, &port_type) != L7_SUCCESS)
    {
      if (ptin_debug_dai_snooping)
        LOG_ERR(LOG_CTX_DAI, "Error obtaining UNI VLANs from IntIfNum %u, VLANs %u+%u", intIfNum, vlanId, innerVlanId);
      return L7_FAILURE;
    }
    /* No inner VLAN for root interfaces of unstacked services */
    if (!is_stacked && port_type == PTIN_EVC_INTF_ROOT)
    {
      vlanId_list[0][1] = 0;
    }
    /* Only one VLAN */
    number_of_vlans = 1; 
  }

  if (ptin_debug_dai_snooping)
    LOG_TRACE(LOG_CTX_DAI, "number_of_vlans=%u", number_of_vlans);

  /* Transmit for all VLANs */
  for (i = 0; i < number_of_vlans; i++)
  {
    extOVlan = vlanId_list[i][0];
    extIVlan = vlanId_list[i][1];

    if (ptin_debug_dai_snooping)
      LOG_TRACE(LOG_CTX_DAI, "Going to transmit to intIfNum %u, with VLANs %u+%u", intIfNum, extOVlan, extIVlan);

    SYSAPI_NET_MBUF_GET(bufHandle);
    if (bufHandle == L7_NULL)
    {
      /* Don't bother logging this. mbuf alloc failures happen occasionally. */
      daiInfo->debugStats.daiMbufFailures++;
      return L7_FAILURE;
    }

    if (osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x8100 &&
        osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x88A8 &&
        osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x9100)
    {
      memmove(&frame[16], &frame[12], frameLen);
      frame[12] = 0x81;
      frame[13] = 0x00;
      frame[14] = (vlanId>>8) & 0xff;
      frame[15] = vlanId & 0xff;

      frameLen += 4;

      if (ptin_debug_dai_snooping)
        LOG_TRACE(LOG_CTX_DAI, "Added outer VLAN (%u)", vlanId);
    }

    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      frame[14] &= 0xf0;
      frame[14] |= ((extOVlan>>8) & 0x0f);
      frame[15]  = extOVlan & 0xff;
      //vlanId = extOVlan;
      if (ptin_debug_dai_snooping)
        LOG_TRACE(LOG_CTX_DAI, "Replaced outer VLAN (%u)", extOVlan);
    }
    /* Add inner vlan when there exists, and if vlan belongs to a stacked EVC */
    if (extIVlan!=0)
    {
      //for (i=frameLen-1; i>=16; i--)  frame[i+4] = frame[i];
            /* No inner tag? */
      if (osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x8100 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x88A8 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x9100)
      {
        memmove(&frame[20],&frame[16],frameLen);
        frame[16] = 0x81;
        frame[17] = 0x00;
        frameLen += 4;
      }
      frame[18] = (frame[14] & 0xe0) | ((extIVlan>>8) & 0x0f);
      frame[19] = extIVlan & 0xff;
      //innerVlanId = extIVlan;
      if (ptin_debug_dai_snooping)
        LOG_TRACE(LOG_CTX_DAI, "Added inner VLAN (%u)", extIVlan);
    }

    if (ptin_debug_dai_snooping)
      LOG_TRACE(LOG_CTX_DAI, "Going to transmit packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, extOVlan, extIVlan);

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    memcpy(dataStart, frame, frameLen);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLen);

    rc = dtlIpBufSend(intIfNum, vlanId, bufHandle);

    if (rc != L7_SUCCESS)
    {
      break;
    }
  }

  return rc;
}

