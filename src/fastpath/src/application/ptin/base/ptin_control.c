/*
 * ptin_cfg.h
 * 
 * Implements PTin module control
 *
 * Created on: 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#include <unistd.h>
#include "ptin_include.h"
#include "ptin_control.h"
#include "ptin_cnfgr.h"
#include "ptin_intf.h"
#include "ptin_cfg.h"
#include "dtl_ptin.h"
#include "ipc.h"
#include "nimapi.h"
#include "usmdb_dot3ad_api.h"
#include "dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "ptin_msghandler.h"
#include "ptin_msg.h"
#include "ptin_fpga_api.h"

#include "fw_shm.h"

#include <usmdb_sim_api.h>

/* PTin module state */
volatile ptin_state_t ptin_state = PTIN_STATE_LOADING;

volatile L7_uint32    ptin_task_msg_id     = (L7_uint32) -1;
volatile void        *ptin_task_msg_buffer = L7_NULLPTR;

/* Traffic activity bits for external module access */
L7_uint32 ptin_control_port_activity[PTIN_SYSTEM_N_PORTS];  /* maps each phy port */

static L7_uint32 ptin_loop_handle = 0;  /* periodic timer handle */

/* Link status of each interf (instant+history) */
L7_uint32 linkStatus_alarm[L7_ALL_INTERFACES/32+1];

static L7_int    linkStatus_history[PTIN_SYSTEM_N_INTERF];        /* Link status of each interface */
static L7_BOOL   lagActiveMembers[PTIN_SYSTEM_N_PORTS];   /* Port is an active Lag member? */
static L7_uint32 lagIdList[PTIN_SYSTEM_N_PORTS];          /* LAG id that belongs the port */

/* Tells if alarms should be suppressed */
static unsigned char ptin_alarms_suppressed[PTIN_SYSTEM_N_INTERF];

L7_BOOL ptin_control_debug = L7_FALSE;
void ptin_control_debug_set(L7_BOOL enable)
{
  ptin_control_debug = enable;
}

/* Local prototypes */
static void startup_trap_send(void);
static void monitor_alarms(void);

/* Task for processing messages */
void ptin_control_task_process(void);

/******************************** 
 * Interface events 
 ********************************/

/* Interface events */
typedef struct
{
  L7_uint32 intIfNum;
  L7_uint32 event;
  L7_uint32 type;
} ptinIntfEventMsg_t;

/* Queue to manage interface events */
#define PTIN_INTF_EVENT_QUEUE_MAX 100
#define PTIN_INTF_EVENT_QUEUE_MSG_SIZE  sizeof(ptinIntfEventMsg_t)
void *ptin_intf_event_queue = L7_NULLPTR;

static L7_RC_t ptinIntfUpdate(ptinIntfEventMsg_t *eventMsg);


/**
 * Task that runs part of the PTin initialization and further periodic 
 * processing (alarms check) 
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinTask(L7_uint32 numArgs, void *unit)
{
  L7_RC_t rc;

  rc = osapiTaskInitDone(L7_PTIN_TASK_SYNC);

  /* Wait for a signal indicating that all other modules
   * configurations were executed */
  osapiSleep(10);
  PT_LOG_NOTICE(LOG_CTX_CONTROL, "PTin task will now start!");

  /* Initialize PTin Interface module data structures */
  if (ptin_intf_pre_init() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin Interface module! CRASH!");
    PTIN_CRASH();
  }

  /* Apply configuration on DTL and lower layers */
  if (dtlPtinInit() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin DTL module! CRASH!");
    PTIN_CRASH();
  }

  /* Note: ptin_intf_data_init() needs to be invoked ONLY after nim
   * initialization, which can be guaranteed at this stage */
  if (ptin_intf_post_init() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin Interface module! CRASH!");
    PTIN_CRASH();
  }

  /* Register a period timer */
  if (osapiPeriodicUserTimerRegister(PTIN_LOOP_TICK, &ptin_loop_handle) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error registering period timer! CRASH!");
    PTIN_CRASH();
  }

  /* Unblock switchover monitor task */
  PT_LOG_INFO(LOG_CTX_CNFGR, "Unblocking switchover monitor task");
  //osapiSemaGive(ptin_switchover_sem);

  /* Signal correct initialization */
  ptin_state = PTIN_STATE_READY;

  /* Send startup trap */
  startup_trap_send();

  /* Create task to proceed to reset defaults */
  if (osapiTaskCreate("ptin_control_task_process task", ptin_control_task_process, 0, 0,
                      L7_DEFAULT_STACK_SIZE*10,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_control_task_process task!");
    PTIN_CRASH();
  }
  PT_LOG_INFO(LOG_CTX_CNFGR, "ptin_control_task_process task launch OK");

  /* register callback with NIM for L7_UPs and L7_DOWNs */
  if (nimRegisterIntfChange(L7_PTIN_COMPONENT_ID,
                            ptinIntfChangeCallback,
                            ptinIntfStartupCallback, NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CONTROL, "Failed to register events reception!");
    PTIN_CRASH();
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "ptinIntfChangeCallback registered!");
  }

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "Free ptin_ready_sem:%p", ptin_ready_sem);
  osapiSemaGive(ptin_ready_sem);  
  
  /* Loop */
  while (1)
  {
    osapiPeriodicUserTimerWait(ptin_loop_handle);

    /* Monitor alarms */
    monitor_alarms();
  }
}

/**
 * Task for processing messages that may take a longer time 
 * (>IPC_LIB_TIME_OUT) 
 */
void ptin_control_task_process(void)
{
  PT_LOG_NOTICE(LOG_CTX_CONTROL,"ptin_control_task_process started");

  /* Loop */
  while (1)
  {
    /* Lock Busy State */
    PT_LOG_INFO(LOG_CTX_CONTROL,"Going to take ptin_busy_sem:%p",ptin_busy_sem);
    osapiSemaTake(ptin_busy_sem, L7_WAIT_FOREVER);
    ptin_state = PTIN_STATE_BUSY;
    PT_LOG_INFO(LOG_CTX_CONTROL,"Going to perform task :0x%x.",ptin_task_msg_id);

    #if 0
    if (ptin_task_msg_id == (L7_uint32) -1)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL,"Invalid Parameters: ptin_msg_id:%u ptin_msg_ptr_buffer=%p", ptin_task_msg_id, ptin_task_msg_buffer);
      osapiSemaGive(ptin_ready_sem);
      continue;
    }
    #endif

    switch (ptin_task_msg_id)
    {      
      case CCMSG_DEFAULTS_RESET:
      {
        ptin_msg_defaults_reset((msg_HwGenReq_t*) ptin_task_msg_buffer);
        break;
      }
#if 0
      case CCMSG_PROTECTION_MATRIX_FLUSH_CONFIGURATION_END:
      {
        ptin_msg_protection_matrix_configuration_flush_end();
        break;
      }
#endif
      default:
      PT_LOG_WARN(LOG_CTX_CONTROL,"Message Id 0x%x Not Supported!", ptin_task_msg_id);
      break;
    }
    
    /* Restore Ready State */
    ptin_state = PTIN_STATE_READY;   
    PT_LOG_INFO(LOG_CTX_CONTROL,"Task done:0x%x", ptin_task_msg_id);

    /*Clear Global Variables*/
    ptin_task_msg_id = (L7_uint32) -1; 
    ptin_task_msg_buffer = L7_NULLPTR; 
    
    /* Unlock Ready State */
    osapiSemaGive(ptin_ready_sem);
  }
}


/**
 * Initialize alarms state
 * 
 */
void ptin_alarms_init(void)
{
  L7_uint32 port;

  /* Clear alarms */
  memset(linkStatus_alarm, 0x00, sizeof(linkStatus_alarm));

  /* Initialize alarms state */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    linkStatus_history[port] = L7_TRUE;
    ptin_alarms_suppressed[port] = L7_TRUE;

    /* Unsupress alarms, only for these conditions: */
    if (port < PTIN_SYSTEM_N_ETH)
    {
      ptin_alarms_suppressed[port] = L7_FALSE;
    }
  }

  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++)
    lagActiveMembers[port] = L7_TRUE;
}

/**
 * Change suppressed alarm state
 * 
 * @author mruas (8/12/2015)
 * 
 * @param port : ptin_port format
 * @param state : TRUE or FALSE
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_alarms_suppress(L7_uint32 port, L7_BOOL state)
{
  if (port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid port %u", port);
    return L7_FAILURE;
  }

  /* Save new state */
  ptin_alarms_suppressed[port] = state;

  return L7_SUCCESS;
}

/**
 * Get suppressed alarm state
 * 
 * @param port : ptin_port format
 * 
 * @return L7_BOOL : TRUE or FALSE
 */
L7_BOOL ptin_alarms_is_suppressed(L7_uint32 port)
{
  if (port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid port %u", port);
    return L7_FALSE;
  }

  /* Return current state */
  return ptin_alarms_suppressed[port];
}


static void startup_trap_send(void)
{
  /* Send a trap because Fastpath is ready to be flushed with configs */
  while (1)
  {
    if (send_trap(IPC_CHMSG_TRAP_PORT, TRAP_ARRANQUE, -1) < 0) {
      PT_LOG_ERR(LOG_CTX_CONTROL, "Failed to send Startup Trap");
      sleep(1);
    }
    else {
      PT_LOG_NOTICE(LOG_CTX_CONTROL, "Startup Trap successfully sent");
      break;
    }
  }
}

/**
 * Monitor alarms
 */
static void monitor_alarms(void)
{
  L7_uint32 intf, port;
  ptin_intf_t ptin_intf; 
  L7_uint32 adminState, linkState, link;
  L7_BOOL   interface_is_valid;
  L7_uint32 portActivity_valid;
  ptin_HWEth_PortsActivity_t portActivity;

  static L7_BOOL   first_time=L7_TRUE;

  /* Initializations */
  if (first_time)
  {
    /* Initialize alarms state */
    ptin_alarms_init();

    memset(lagIdList,0xff,sizeof(lagIdList));

    first_time = L7_FALSE;
  }

  /* Get RX activity for all ports */
  memset(&portActivity, 0x00, sizeof(portActivity));
  portActivity_valid = L7_FALSE;

  /* Run all ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    /* Skip not valid ports */
    if (port >= ptin_sys_number_of_ports && port < PTIN_SYSTEM_N_PORTS)
      continue;

    if (ptin_intf_port2intIfNum(port,&intf)!=L7_SUCCESS ||
        ptin_intf_port2ptintf(port,&ptin_intf)!=L7_SUCCESS ||
        nimGetIntfAdminState(intf,&adminState)!=L7_SUCCESS)
    {
      intf = 0;
      ptin_intf.intf_type = 0;
      ptin_intf.intf_id   = 0;
      adminState = L7_DISABLE;

      ptin_intf_port2ptintf(port,&ptin_intf);

      interface_is_valid = L7_FALSE;
    }
    else
    {
      interface_is_valid = L7_TRUE;
    }

    /* Initialize Link-state */
    linkState = L7_DOWN;

    // Get Link status
    if ((!interface_is_valid) || (adminState == L7_DISABLE))
    {
      link = L7_TRUE;       // End of link down
    }
    /* Did happen a link-down in the last second? This alarm will contain that information */
    else if ( ((linkStatus_alarm[intf/32]>>(intf%32)) & 1) )
    {
      link = L7_FALSE;
      PT_LOG_DEBUG(LOG_CTX_INTF  ,"Alarm-down detected: port=%u, intIfNum=%u", port, intf);
    }
    /* Very current link status */
    else if (nimGetIntfLinkState(intf, &linkState) == L7_SUCCESS)
    {
      link = (linkState == L7_UP);
    }
    else
    {
      link = L7_FALSE;
    }

    /* Clear Link alarm */
    linkStatus_alarm[intf/32] &= ~((L7_uint32) 1 << (intf%32));

    if (linkStatus_history[port] != link)
    {
      if (!ptin_alarms_is_suppressed(port))
      {
        if (send_trap_intf_alarm(ptin_intf.intf_type, ptin_intf.intf_id,
                               ((!link) ? TRAP_ALARM_LINK_DOWN_START : TRAP_ALARM_LINK_DOWN_END),
                               TRAP_ALARM_STATUS_EVENT,0) == 0)
        {
          PT_LOG_NOTICE(LOG_CTX_INTF  ,"Alarm sent: port=%u, link=%u", port, link);
          PT_LOG_NOTICE(LOG_CTX_EVENTS,"Alarm sent: port=%u, link=%u", port, link);
        }
      }

      PT_LOG_INFO(LOG_CTX_INTF  ,"Link state changed: port=%u, link=%u", port, link);
      PT_LOG_INFO(LOG_CTX_EVENTS,"Link state changed: port=%u, link=%u", port, link);
      linkStatus_history[port] = link;
    }

    // Only send lag active member traps, if interface is physical
    if (port<PTIN_SYSTEM_N_PORTS)
    {
      L7_uint32 lag_intIfNum = 0;
      L7_uint32 lag_port;
      ptin_intf_t ptin_intf_lag;
      L7_BOOL   isMember, isActiveMember;

      // Determine if is an active lag member
      if (interface_is_valid &&
          (usmDbDot3adIntfIsMemberGet(1, intf, &lag_intIfNum) == L7_SUCCESS && lag_intIfNum != 0) &&
          (ptin_intf_intIfNum2ptintf(lag_intIfNum, &ptin_intf_lag) == L7_SUCCESS && ptin_intf_lag.intf_type == PTIN_EVC_INTF_LOGICAL))
      {
        /* This interface is a lag member */
        isMember = L7_TRUE;

        lagIdList[port] = ptin_intf_lag.intf_id;

        /* Is this interface an active lag member? */
        if (adminState==L7_DISABLE)
          isActiveMember = L7_TRUE;
        else if ((usmDbDot3adIsActiveMember(1,intf)==L7_SUCCESS) && (link == L7_TRUE))
          isActiveMember = L7_TRUE;
        else
          isActiveMember = L7_FALSE;
      }
      else
      {
        isMember = L7_FALSE;
        isActiveMember = L7_TRUE;
      }

      // Check if there is a change in the active member state
      if (lagActiveMembers[port] != isActiveMember)
      {
        if (ptin_intf_intIfNum2port(lag_intIfNum, &lag_port) == L7_SUCCESS &&
            !ptin_alarms_is_suppressed(lag_port))
        {
          if (send_trap_intf_alarm(PTIN_EVC_INTF_PHYSICAL, port,
                                   ((isActiveMember) ? TRAP_ALARM_LAG_INACTIVE_MEMBER_END : TRAP_ALARM_LAG_INACTIVE_MEMBER_START),
                                   TRAP_ALARM_STATUS_EVENT,
                                   lagIdList[port])==0)
          {
            PT_LOG_NOTICE(LOG_CTX_INTF  ,"Alarm sent: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
            PT_LOG_NOTICE(LOG_CTX_EVENTS,"Alarm sent: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
          }
          else
          {
            PT_LOG_ERR(LOG_CTX_INTF  ,"Error sending alarm: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
          }
        }
        lagActiveMembers[port]=isActiveMember;
        PT_LOG_INFO(LOG_CTX_INTF  ,"Active LAG membership changed: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
        PT_LOG_INFO(LOG_CTX_EVENTS,"Active LAG membership changed: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
      }
    }
  }
}



/**
 * Task that checks for Interface changes
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinIntfTask(L7_uint32 numArgs, void *unit)
{
  L7_RC_t rc;
  ptinIntfEventMsg_t eventMsg;

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "PTinIntf running!");

  rc = osapiTaskInitDone(L7_PTIN_INTF_TASK_SYNC);

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "PTinIntf task will now start!");

  /* Queue that will process timer events */
  ptin_intf_event_queue = (void *) osapiMsgQueueCreate("ptin_intf_event_queue",
                                                       PTIN_INTF_EVENT_QUEUE_MAX, PTIN_INTF_EVENT_QUEUE_MSG_SIZE);
  if (ptin_intf_event_queue == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CONTROL,"ptin_intf_event_queue creation error.");
    PTIN_CRASH();
  }
  PT_LOG_INFO(LOG_CTX_CONTROL,"ptin_intf_event_queue created.");

  /* register callback with NIM for L7_UPs and L7_DOWNs */
  if ((rc = nimRegisterIntfChange(L7_PTIN_COMPONENT_ID,
                                  ptinIntfChangeCallback,
                                  ptinIntfStartupCallback, NIM_STARTUP_PRIO_DEFAULT)) != L7_SUCCESS)
  {
    PT_LOG_INFO(LOG_CTX_CONTROL,"Failed to register events");
    PTIN_CRASH();
  }
  PT_LOG_INFO(LOG_CTX_CONTROL,"Events registered successfully");

  #if 0
  /* Wait for a signal indicating that all other modules
   * configurations were executed */
  PT_LOG_INFO(LOG_CTX_CONTROL, "PTinIntf task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_ready_sem, L7_WAIT_FOREVER);
  PT_LOG_NOTICE(LOG_CTX_CONTROL, "PTinIntf task will now start!");
  #endif

  while (1)
  {
    /* Check if queue is valid */
    if (ptin_intf_event_queue == L7_NULLPTR)
    {
      osapiSleep(1);
      continue;
    }

    if (osapiMessageReceive(ptin_intf_event_queue, &eventMsg, sizeof(ptinIntfEventMsg_t), L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      /* Process interface events: only a maximum of 10 per loop */
      rc = ptinIntfUpdate(&eventMsg);
      PT_LOG_DEBUG(LOG_CTX_CONTROL, "Event processed: rc=%d", rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_CONTROL, "Error receiving queue messages");
    }
  }
}

/*********************************************************************
* @purpose  Update the current state of a given interface.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptinIntfChangeCallback(L7_uint32 intIfNum,
                               L7_uint32 event,
                               NIM_CORRELATOR_t correlator)
{
  NIM_EVENT_COMPLETE_INFO_t status;
  ptinIntfEventMsg_t eventMsg;

  status.intIfNum     = intIfNum;
  status.component    = L7_PTIN_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  PT_LOG_DEBUG(LOG_CTX_CONTROL, "Event received: event=%u, intIfNum=%u", event, intIfNum);

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    PT_LOG_ERR(LOG_CTX_CONTROL,
            "DHCP snooping received an interface change callback for event %s"
            " on interface %s during invalid initialization phase.",
            nimGetIntfEvent(event), ifName);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  /* Other events than those, should be ignored */
  if (event != L7_UP &&
      event != L7_DOWN &&
      event != L7_LAG_ACTIVE_MEMBER_ADDED &&
      event != L7_LAG_ACTIVE_MEMBER_REMOVED)
  {
    PT_LOG_DEBUG(LOG_CTX_CONTROL, "Error: event=%u, intIfNum=%u", event, intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  eventMsg.event    = event; 
  eventMsg.intIfNum = intIfNum;
  if (osapiMessageSend(ptin_intf_event_queue, &eventMsg, sizeof(ptinIntfEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)

  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Error sending message to queue: event=%u, intIfNum=%u",event, intIfNum);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_CONTROL, "Message sent to queue: event=%u, intIfNum=%u",event, intIfNum);
  }

  nimEventStatusCallback(status);

  return L7_SUCCESS;
}

/**
 * Process interface events
 * 
 * @param eventMsg 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptinIntfUpdate(ptinIntfEventMsg_t *eventMsg)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_INTF_TYPES_t intf_type;
  L7_uint32 lag_intIfNum;

  if (eventMsg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get interface type */
  if (nimGetIntfType(eventMsg->intIfNum, &intf_type) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if ( eventMsg->event == L7_UP ) /* No need to process any other NIM event than these  */
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "Link up detected at interface intIfNum %u", eventMsg->intIfNum);
    //rc = uplinkProtEventProcess(eventMsg->intIfNum, eventMsg->event);
  }
  else if ( eventMsg->event == L7_DOWN )
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "Link down detected at interface intIfNum %u", eventMsg->intIfNum);
    //rc = uplinkProtEventProcess(eventMsg->intIfNum, eventMsg->event);
  }
  else if ( eventMsg->event == L7_LAG_ACTIVE_MEMBER_ADDED || eventMsg->event == L7_LAG_ACTIVE_MEMBER_REMOVED )
  {
    if (intf_type == L7_LAG_INTF)
    {
      L7_uint32 activeMembers;

      lag_intIfNum = eventMsg->intIfNum;

      /* If no active members remain, do nothing... a link-down event will come later */
      if (dot3adLagNumActiveMembersGet(lag_intIfNum, &activeMembers) != L7_SUCCESS || activeMembers == 0)
      {
        PT_LOG_TRACE(LOG_CTX_INTF, "No active members... not doing nothing.");
        return L7_SUCCESS;
      }
    }
    else if (intf_type == L7_PHYSICAL_INTF)
    {
      if (usmDbDot3adIntfIsMemberGet(0, eventMsg->intIfNum, &lag_intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error obtainging LAG information");
        return L7_FAILURE;
      }
    }

    PT_LOG_INFO(LOG_CTX_INTF, "LAG active members addition/remotion (%u) detected at interface intIfNum %u (LAG intIfNum is %u)",
                eventMsg->event, eventMsg->intIfNum, lag_intIfNum);

    //rc = uplinkProtEventProcess(lag_intIfNum, eventMsg->event);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_CONTROL, "Unknown event detected at interface intIfNum %u", eventMsg->intIfNum);
  }

  return rc;
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
void ptinIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  PORTEVENT_MASK_t portEvent_mask;

  PT_LOG_INFO(LOG_CTX_CONTROL, "Startup executed");

  memset(&portEvent_mask, 0x00, sizeof(portEvent_mask));

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_UP);
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_DOWN);
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_LAG_ACTIVE_MEMBER_ADDED);
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_LAG_ACTIVE_MEMBER_REMOVED);

  /* Event types to be received */
  nimRegisterIntfEvents(L7_PTIN_COMPONENT_ID, portEvent_mask);

  nimStartupEventDone(L7_PTIN_COMPONENT_ID);
}

void reboot_task(void)
{
  PT_LOG_NOTICE(LOG_CTX_CONTROL, "Started thread to reboot system. Waiting 7 seconds...");

  osapiSleep(7);

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "Rebooting system...");

  system("sync");
  osapiSleep(3);
  system("reboot");

  /* Loop */
  while (1)
  {
    osapiSleep(1);
  }
}

/**
 * Reboot system
 * 
 * @author mruas (13/10/17)
 * 
 * @param void 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_control_reboot(void)
{
  PT_LOG_INFO(LOG_CTX_CONTROL, "Creating reboot task...");
  if (osapiTaskCreate("reboot_task", reboot_task, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_MEDIUM_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Failed to create reboot task... going to crash intentionally!");
    PTIN_CRASH();
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "Reboot task launch OK");
  }

  return L7_SUCCESS;
}






L7_int get_linkStatus(L7_uint32 port) {return port<PTIN_SYSTEM_N_INTERF? linkStatus_history[port]: L7_TRUE;}

