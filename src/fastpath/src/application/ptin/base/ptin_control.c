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
#include "ptin_xlate_api.h"
#include "ptin_evc.h"
#include "ptin_igmp.h"
#include "ptin_dhcp.h"
#include "ptin_pppoe.h"
#include "ptin_acl.h"
#include "ptin_prot_oam_eth.h"
#include "ptin_prot_erps.h"
#include "ptin_hal_erps.h"
#include "ptin_routing.h"
#include "ptin_cfg.h"
#include "dtl_ptin.h"
#include "ipc.h"
#include "nimapi.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "ptin_fieldproc.h"
#include "ptin_msghandler.h"
#include "ptin_msg.h"
#include "ptin_fpga_api.h"

#if ( PTIN_BOARD_IS_STANDALONE )
#include "fw_shm.h"
#endif

#include <usmdb_sim_api.h>

/* PTin module state */
volatile ptin_state_t ptin_state = PTIN_STATE_LOADING;

volatile L7_uint32    ptin_task_msg_id     = (L7_uint32) -1;
volatile void        *ptin_task_msg_buffer = L7_NULLPTR;

#if (PTIN_BOARD_IS_MATRIX)
#define LS_RESETS_MAX   6     /* Maximum tolerabled resets */
#define LS_CREDITS_MAX  10    /* Maximum resets */
typedef struct
{
  L7_int  credits;
  L7_int  resets_counter;
  L7_BOOL monitor_enable;
  L7_uint fcs_threshold;
} struct_linkStatus_monitor_t;

/* Slot monitoring info */
struct_linkStatus_monitor_t ls_monitor_info[PTIN_SYS_SLOTS_MAX+1]={
  {0, 0, L7_FALSE, 0},   /* Dummy: no slot */
  {0, 0, L7_FALSE, 0},
#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  [2 ... (PTIN_SYS_SLOTS_MAX-1)]={10, 0, L7_TRUE , 0},   /* Slot monitor enabled for CXO160G */
#else
  [2 ... (PTIN_SYS_SLOTS_MAX-1)]={10, 0, L7_FALSE, 0},   /* Slot monitor disabled for other SF boards */
#endif
  {0, 0, L7_FALSE, 0}
};

/* Remote link status */
struct_linkStatus_t remote_link_status[PTIN_SYSTEM_N_PORTS];
struct_linkStatus_t local_link_status[PTIN_SYSTEM_N_PORTS];
void *link_status_sem;  /* Semaphore to access this data structure */

#define SLOTRESET_SAVE_LAST_EVENTS  10
typedef struct
{
  L7_uint16 slot_id;
  L7_uint16 board_id;
  L7_uint16 mx_ptin_port;
  struct_linkStatus_t local_linkStatus;
  struct_linkStatus_t remote_linkStatus;
  struct L7_localtime_t time;
} struct_slotReset_record_t;

struct_slotReset_record_t slotReset_record_last[SLOTRESET_SAVE_LAST_EVENTS];
L7_uint16 slotReset_event_index = 0;
#endif

/* Traffic activity bits for external module access */
L7_uint32 ptin_control_port_activity[PTIN_SYSTEM_N_PORTS];  /* maps each phy port */

static L7_uint32 ptin_loop_handle = 0, _10ms_loop_handle=0;  /* periodic timer handle */

static L7_int    linkStatus[PTIN_SYSTEM_N_INTERF];        /* Link status of each interface */
static L7_BOOL   lagActiveMembers[PTIN_SYSTEM_N_PORTS];   /* Port is an active Lag member? */
static L7_uint32 lagIdList[PTIN_SYSTEM_N_PORTS];          /* LAG id that belongs the port */

#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
#if (PHY_RECOVERY_PROCEDURE)
L7_BOOL slots_to_be_reseted[PTIN_SYS_SLOTS_MAX]={L7_FALSE, [1 ... (PTIN_SYS_SLOTS_MAX-2)]=L7_TRUE, L7_FALSE};
#endif
#endif

/* Tells if alarms should be suppressed */
static unsigned char ptin_alarms_suppressed[PTIN_SYSTEM_N_INTERF];

/* Local prototypes */
static void startup_trap_send(void);
static void monitor_throughput(void);
static void monitor_alarms(void);
static void monitor_matrix_commutation(void);

#if (PTIN_BOARD_IS_LINECARD)
static void ptin_control_linkstatus_report(void);
#endif

#if (PTIN_BOARD_IS_MATRIX)
#ifdef PTIN_LINKSCAN_CONTROL
void ptin_control_switchover_monitor(void);
#endif /* PTIN_LINKSCAN_CONTROL */

void ptin_control_linkStatus_monitor(void);
void ptin_control_slot_reset(L7_uint ptin_port, L7_uint slot_id, L7_uint board_id,
                                    struct_linkStatus_t linkStatus[2]);
#endif /* PTIN_BOARD_IS_MATRIX */

static void ptin_control_syncE(void);

/* 10ms task */
void _10msTask(void);

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
  PT_LOG_INFO(LOG_CTX_CONTROL, "PTin task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_ready_sem, L7_WAIT_FOREVER);
  PT_LOG_NOTICE(LOG_CTX_CONTROL, "PTin task will now start!");

  /* System StormControl no more supported */
  /* Initialize storm control */
  rc = ptin_stormControl_init();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing storm control CRASH!");
    PTIN_CRASH();
  }
  PT_LOG_INFO(LOG_CTX_CNFGR, "Storm Control is active with default values.");

  /* Apply configuration on DTL and lower layers */
  if (dtlPtinInit() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin DTL module! CRASH!");
    PTIN_CRASH();
  }

  /* Initialize PTin Interface module data structures
   * Note: ptin_intf_data_init() needs to be invoked ONLY after nim
   * initialization, which can be guaranteed at this stage */
  if (ptin_intf_init() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin Interface module! CRASH!");
    PTIN_CRASH();
  }

  /* Initialize xlate module in application layer */
  if (ptin_xlate_init()!=L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error initializing PTin XLATE module! CRASH!");
    PTIN_CRASH();
  }

  /* By default enable global DHCP trapping */
  if (ptin_dhcp_enable(L7_ENABLE) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error enabling DHCP global trapping! CRASH!");
    PTIN_CRASH();
  }

  /* PPPoE Global enable */
  if (ptin_pppoe_enable(L7_ENABLE) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error enabling PPPoE global trapping! CRASH!");
    PTIN_CRASH();
  }

#if ( PTIN_BOARD_IS_MATRIX )
  /* Configure InBand bridge if this board is CXP360G */
  if (ptin_cfg_inband_bridge_set() != L7_SUCCESS)
  {
    PT_LOG_CRITIC(LOG_CTX_CNFGR, "Error creating InBand bridge!");
  }
#endif

#if ( PTIN_BOARD == PTIN_BOARD_CXO640G || \
      PTIN_BOARD == PTIN_BOARD_TA48GE || PTIN_BOARD == PTIN_BOARD_TG16G || PTIN_BOARD == PTIN_BOARD_TG16GF)
  ptin_cfg_pcap_bridge_set();
#endif

  /* Default EVCs */
  if (ptin_evc_startup() != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create default EVCs!");
    PTIN_CRASH();
  }

  /* Register a period timer */
  if (osapiPeriodicUserTimerRegister(PTIN_LOOP_TICK, &ptin_loop_handle) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error registering period timer! CRASH!");
    PTIN_CRASH();
  }

#if (!PTIN_BOARD_IS_STANDALONE)
  /* Unblock switchover monitor task */
  PT_LOG_INFO(LOG_CTX_CNFGR, "Unblocking switchover monitor task");
  osapiSemaGive(ptin_switchover_sem);
#endif

#if (PTIN_BOARD_IS_MATRIX)
/*Added a 30 seconds delay to prevent abnormal behaviour on the HW L3_IPMC Table of the Standby Matrix. 
  Restored the waiting delay of ptin_control_switchover_monitor() from 30 to 10 seconds.
  This approach is preferable, since this routine is executed before the flush takes place*/
  PT_LOG_NOTICE(LOG_CTX_CNFGR, "Waiting 30 seconds to switch from PTIN_ISLOADING to PTIN_LOADED state...");
  sleep(30);
  PT_LOG_NOTICE(LOG_CTX_CNFGR, "Done.");
#endif

  /* Signal correct initialization */
  ptin_state = PTIN_STATE_READY;

  /* Send startup trap */
  startup_trap_send();

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  /* Create 10ms task */
  if (osapiTaskCreate("10ms task", _10msTask, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_MEDIUM_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create 10ms task!");
    PTIN_CRASH();
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_CNFGR, "10ms task launch OK");
  }

  if (osapiWaitForTaskInit (L7_PTIN_10MS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Unable to initialize 10ms task()\n");
    PTIN_CRASH();
  }
#endif

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

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "Free ptin_ready_sem:%p", ptin_ready_sem);
  osapiSemaGive(ptin_ready_sem);  
  

  /* Loop */
  while (1)
  {
    osapiPeriodicUserTimerWait(ptin_loop_handle);

    /* Monitor throughput */
    monitor_throughput();

    /* Port commutation process for TOLT8G boards */
#if ( PTIN_BOARD != PTIN_BOARD_TA48GE )
    monitor_matrix_commutation();
#endif

    /* Monitor alarms */
    monitor_alarms();

    /* Synchronize recovery clocks */
    ptin_control_syncE();
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
      case CCMSG_PROTECTION_MATRIX_FLUSH_CONFIGURATION_END:
      {
        ptin_msg_protection_matrix_configuration_flush_end();
        break;
      }
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
 * Task for processing 10ms periodicity events
 */
void _10msTask(void)
{
  PT_LOG_NOTICE(LOG_CTX_CONTROL,"10ms Task started");

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  {
   ptin_LACPLagConfig_t lagInfo;

   lagInfo.lagId=            0;
   lagInfo.admin=            1;
   lagInfo.stp_enable=       0;
   lagInfo.static_enable=    1;
   lagInfo.loadBalance_mode= 1;// FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
   //ptin_intf_LagConfig_get(&lagInfo);

   lagInfo.members_pbmp64=   1ULL<<(PTIN_SYSTEM_N_ETH+1) |   1ULL<<PTIN_SYSTEM_N_ETH;
   ptin_intf_Lag_create(&lagInfo);
  }

  PT_LOG_NOTICE(LOG_CTX_CONTROL,"Internal LAG created");

  /* Wait one second, before starting high speed process */
  osapiSleep(1);
#endif

  if (osapiTaskInitDone(L7_PTIN_10MS_TASK_SYNC)!=L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CONTROL, "Error syncing task");
    PTIN_CRASH();
  }

  /* Register a period timer */
  if (osapiPeriodicUserTimerRegister(10, &_10ms_loop_handle) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Error registering period timer! CRASH!");
    PTIN_CRASH();
  }

  //{
  // static unsigned char m[8]={0, 1, 2, 3, 4, 5, 6, 7};
  //    usmDbSwDevCtrlMacAddrTypeSet(0, L7_SYSMAC_LAA);
  //    usmDbSwDevCtrlLocalAdminAddrSet(0, m);
  //}
  sleep(5);     //Allow ANDL/HAPI layer to create the trunk before manipulating it in "monitor_matrix_commutation()"

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "10ms task will now start!");

  //nice(-1);

  /* Loop */
  while (1)
  {
    osapiPeriodicUserTimerWait(_10ms_loop_handle);
    //usleep(10000);

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
    monitor_matrix_commutation();
#endif
  }
}


/**
 * Initialize alarms state
 * 
 */
void ptin_alarms_init(void)
{
  L7_uint32 port;

  /* Initialize alarms state */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    linkStatus[port] = L7_TRUE;
    ptin_alarms_suppressed[port] = L7_FALSE;
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
#if PTIN_BOARD_IS_STANDALONE
  if (port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid port %u", port);
    return L7_FAILURE;
  }

  /* Save new state */
  ptin_alarms_suppressed[port] = state;

  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
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
#if PTIN_BOARD_IS_STANDALONE
  if (port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid port %u", port);
    return L7_FALSE;
  }

  /* Return current state */
  return ptin_alarms_suppressed[port];
#else
  return L7_FALSE;
#endif
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

/* Monitor throughput, and send traps to linecards */
static void monitor_throughput(void)
{
  #if 0
  L7_int  port;
  ptin_HWEth_PortsActivity_t portsActivity = {ports_mask:   0xFFFFFFFF,
                                              activity_mask:0xFFFFFFFF};
  static L7_uint counter=0;
  //L7_uint32 intIfNum, admin, link_state;

  if (dtlPtinCountersActivityGet(&portsActivity) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Error reading counters activity");
    return;
  }

  /* For ports with throughput not null, send a trap message */
  for (port=counter; port<PTIN_SYSTEM_N_PORTS; port+=4)
  {
    //if (ptin_intf_port2intIfNum(port,&intIfNum)!=L7_SUCCESS)
    //  continue;

    //if (nimGetIntfAdminState(intIfNum,&admin)==L7_SUCCESS && admin==L7_ENABLE &&
    //    nimGetIntfLinkState(intIfNum,&link_state)==L7_SUCCESS && link_state==L7_UP)
    {
      if (portsActivity.activity_bmap[port] & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY)
      {
        send_trap_to_linecard(PTIN_EVC_INTF_PHYSICAL, port, TRAP_LINECARD_TRAFFIC_RX, TRAP_ALARM_STATUS_START, 0);
      }
      else
      {
        send_trap_to_linecard(PTIN_EVC_INTF_PHYSICAL, port, TRAP_LINECARD_TRAFFIC_RX, TRAP_ALARM_STATUS_END, 0);
      }
    }
  }

  counter = (counter+1)%4;
  #endif
}

/**
 * Monitor alarms
 */
static void monitor_alarms(void)
{
  L7_uint32 intf, lagIntf;
  L7_uint32 port;
  ptin_intf_t ptin_intf, ptin_intf_lag; 
  L7_uint32 adminState, linkState, link;
  L7_BOOL   interface_is_valid;
  L7_BOOL   isMember, isActiveMember;
  L7_uint32 portActivity_valid = L7_FALSE;
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
  portActivity.ports_mask    = PTIN_SYSTEM_ETH_PORTS_MASK;    /* Only ETH ports */
  portActivity.activity_mask = PTIN_PORTACTIVITY_MASK_RX_ACTIVITY | PTIN_PORTACTIVITY_MASK_TX_ACTIVITY;  /* Get only rx activity */
  if (ptin_intf_counters_activity_get(&portActivity)==L7_SUCCESS)
  {
    portActivity_valid = L7_TRUE;

    /* Update traffic activity bits for external module access */
    memcpy(ptin_control_port_activity, portActivity.activity_bmap, sizeof(L7_uint32)*PTIN_SYSTEM_N_PORTS);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_CONTROL,"Stat Activity get failed!");
    portActivity_valid = L7_FALSE;
  }

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
    if (!interface_is_valid || adminState==L7_DISABLE)
    {
      link = L7_TRUE;       // End of link down
    } else if (nimGetIntfLinkState(intf, &linkState)==L7_SUCCESS)
    {
      link = (linkState==L7_UP);
    } else
    {
      link = L7_FALSE;
    }

  #if ( PTIN_BOARD_IS_STANDALONE )
    if (port<PTIN_SYSTEM_N_PORTS)
    {
      pfw_shm->intf[port].link  = (linkState == L7_UP);
      pfw_shm->intf[port].link |= 
        (((ptin_control_port_activity[port] & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) == PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) << 1) |
        (((ptin_control_port_activity[port] & PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) == PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) << 2);
    }
  #endif

    if (linkStatus[port] != link)
    {
      #if ( PTIN_BOARD_IS_STANDALONE || PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_ACTIVETH )
        #if (PTIN_BOARD_IS_MATRIX)
        /* For CXP360G There is only alarms for external LAGs */
        if (ptin_intf.intf_type==PTIN_EVC_INTF_LOGICAL && ptin_intf.intf_id<PTIN_SYSTEM_N_LAGS_EXTERNAL)
        #else
        /* There is only alarms for non PON interfaces */
        if (port >= PTIN_SYSTEM_N_PONS)
        #endif
        if (!ptin_alarms_is_suppressed(port)) {
          if (send_trap_intf_alarm(ptin_intf.intf_type, ptin_intf.intf_id,
                                 ((!link) ? TRAP_ALARM_LINK_DOWN_START : TRAP_ALARM_LINK_DOWN_END),
                                 TRAP_ALARM_STATUS_EVENT,0) == 0)
          {
            PT_LOG_NOTICE(LOG_CTX_CONTROL,"Alarm sent: port=%u, link=%u", port, link);
          }
        }
        #if (PTIN_BOARD_IS_STANDALONE)
        if ( ((PTIN_SYSTEM_PON_PORTS_MASK >> intf) & 1) || ((PTIN_SYSTEM_BL_INBAND_PORT_MASK >> intf) & 1))
        {          
          if (usmDbIfAdminStateSet(1, intf, L7_ENABLE) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Failed to enable port# %u", intf);           
          }
        }        
        #endif
      #endif
      PT_LOG_INFO(LOG_CTX_CONTROL,"Link state changed: port=%u, link=%u", port, link);
      linkStatus[port]=link;
    }

    // Only send lag active member traps, if interface is physical
    if (port<PTIN_SYSTEM_N_PORTS)
    {
      // Determine if is an active lag member
      if (interface_is_valid &&
          usmDbDot3adIntfIsMemberGet(1,intf,&lagIntf)==L7_SUCCESS && lagIntf!=0 &&
          ptin_intf_intIfNum2ptintf(lagIntf,&ptin_intf_lag)==L7_SUCCESS && ptin_intf_lag.intf_type==PTIN_EVC_INTF_LOGICAL)
      {
        /* This interface is a lag member */
        isMember = L7_TRUE;

        lagIdList[port] = ptin_intf_lag.intf_id;

        /* Is this interface an active lag member? */
        if (adminState==L7_DISABLE)
          isActiveMember = L7_TRUE;
        else if (usmDbDot3adIsActiveMember(1,intf)==L7_SUCCESS)
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
      if (lagActiveMembers[port]!=isActiveMember)
      {
        //if (pfw_shm->intf[port].admin && isMember)
        #if ( PTIN_BOARD_IS_STANDALONE || PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_ACTIVETH )
          #if (PTIN_BOARD_IS_MATRIX)
          /* For CXP360G boards, only send alarms for external LAGs */
          if (lagIdList[port]<PTIN_SYSTEM_N_LAGS_EXTERNAL)
          #else
          /* There is only alarms for non PON interfaces */
          if (port >= PTIN_SYSTEM_N_PONS)
          #endif
          {
            if (send_trap_intf_alarm(PTIN_EVC_INTF_PHYSICAL, port,
                                     ((isActiveMember) ? TRAP_ALARM_LAG_INACTIVE_MEMBER_END : TRAP_ALARM_LAG_INACTIVE_MEMBER_START),
                                     TRAP_ALARM_STATUS_EVENT,
                                     lagIdList[port])==0)
            PT_LOG_NOTICE(LOG_CTX_CONTROL,"Alarm sent: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
          }
        #endif
        lagActiveMembers[port]=isActiveMember;
        PT_LOG_INFO(LOG_CTX_CONTROL,"Active LAG membership changed: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
      }

      /* Led control */
      #if (PTIN_BOARD == PTIN_BOARD_TA48GE)
      /* (only to physical and valid interfaces) */
      if (port<PTIN_SYSTEM_N_ETH && interface_is_valid)
      {
        if (adminState)
        {
          /* Port is enabled */
          if (link)
          {
            /* Link is up */
            //PT_LOG_TRACE(LOG_CTX_CONTROL,"Interface %u have link up",port);

            /* Blink led */
            if ((portActivity_valid) && ((portActivity.ports_mask>>port) & 1) &&
                (portActivity.activity_bmap[port] & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY))
            {
              //PT_LOG_TRACE(LOG_CTX_CONTROL,"Activity on interface %u",port);

              /* Port throughput values are valid, and we have positive throughput */
              ptin_ta48ge_led_control(port, LED_COLOR_GREEN, 0xAA);
            }
            else
            {
              //PT_LOG_TRACE(LOG_CTX_CONTROL,"NO activity on interface %u",port);
              /* No activity, but link is up */
              ptin_ta48ge_led_control(port, LED_COLOR_GREEN, 0xff);
            }
          }
          else
          {
            /* No link, but port is enabled */
            ptin_ta48ge_led_control(port, LED_COLOR_RED, 0xff);
          }
        }
        else
        {
          /* Port is disabled */
          ptin_ta48ge_led_control(port, LED_COLOR_OFF, 0xff);
        }
      }
      #endif
    }
  }
}

/* Number of seconds to wait before send queries */
#define SCHEDULE_MATRIX_SEND_QUERY_WAIT 1

/* Control variable, which will case a query send */
static L7_BOOL matrix_send_queries = L7_FALSE;

/**
 * Schedule Multicast machine reset
 */
void schedule_matrix_query_send(void)
{
  matrix_send_queries = L7_TRUE;
}


/**
 * Matrix Commutation process 
 *  
 */
static void monitor_matrix_commutation(void)
{
#ifdef MAP_CPLD
#if ( PTIN_BOARD==PTIN_BOARD_TOLT8G || PTIN_BOARD==PTIN_BOARD_TA48GE )

  L7_int              cx_work_slot;
  L7_RC_t             rc = L7_SUCCESS;
  static int          cx_work_slot_h = -1;

  #if ( PTIN_BOARD == PTIN_BOARD_TOLT8G )

  ptin_HWEthPhyConf_t phyConf;
  L7_uint             port, port_border;

  cx_work_slot = (cpld_map->reg.slot_matrix >> 4) & 1;

  /* Nothing to do if no change happened */
  if (cx_work_slot == cx_work_slot_h)
  {
    return;
  }

  memset(&phyConf,0x00,sizeof(ptin_HWEthPhyConf_t));
  phyConf.Mask = PTIN_PHYCONF_MASK_PORTEN;

  /* port that delimits working and protection ports */
  port_border = (PTIN_SYSTEM_N_PONS+PTIN_SYSTEM_N_PORTS)/2;

  /* Run all internal ports to change its admin state */
  for (port=PTIN_SYSTEM_N_PONS; port<PTIN_SYSTEM_N_PORTS; port++)
  {
    /* Set port enable */
    if (cx_work_slot)
      phyConf.PortEnable  = (port<port_border) ? L7_TRUE : L7_FALSE;
    else
      phyConf.PortEnable  = (port<port_border) ? L7_FALSE : L7_TRUE;

    phyConf.Port = port;
    if (ptin_intf_PhyConfig_set(&phyConf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL,"Error setting port %u to enable=%u",port,phyConf.PortEnable);
      rc = L7_FAILURE;
    }
  }

  #elif ( PTIN_BOARD == PTIN_BOARD_TA48GE )

  L7_uint32 lag_intf, intIfNum, intIfNum_del;

  rc = ptin_intf_lag2intIfNum(0, &lag_intf);

  /* Cannot retrieve lag intIfNum: reset commutation machine */
  if (rc != L7_SUCCESS)
  {
    cx_work_slot_h = -1;
    //PT_LOG_TRACE(LOG_CTX_CONTROL,"Machine reseted");
    return;
  }

  cx_work_slot = (ptin_fpga_mx_get_matrixactive()==PTIN_SLOT_WORK); //(cpld_map->reg.slot_matrix >> 4) & 1;

  /* Nothing to do if no change happened */
  if (cx_work_slot == cx_work_slot_h)
  {
    return;
  }

  PT_LOG_INFO(LOG_CTX_CONTROL,"Something is going to be done (cx_work_slot_h=%d, cx_work_slot=%d)", cx_work_slot_h, cx_work_slot);
  PT_LOG_INFO(LOG_CTX_CONTROL,"intIfNum of lag 0 is %u", lag_intf);

  if (cx_work_slot)
  {
    PT_LOG_INFO(LOG_CTX_CONTROL,"CX is working");

    if (ptin_intf_port2intIfNum(PTIN_SYSTEM_N_ETH+1, &intIfNum) != L7_SUCCESS ||
        ptin_intf_port2intIfNum(PTIN_SYSTEM_N_ETH, &intIfNum_del) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL,"Failure");
      rc = L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_CONTROL,"CX is protection");

    if (ptin_intf_port2intIfNum(PTIN_SYSTEM_N_ETH, &intIfNum) != L7_SUCCESS ||
        ptin_intf_port2intIfNum(PTIN_SYSTEM_N_ETH+1, &intIfNum_del) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL,"Failure");
      rc = L7_FAILURE;
    }
  }

  /* Only proceed to switchover, if intIfNum values were successfully retrieved */
  if (rc == L7_SUCCESS)
  {
    PT_LOG_INFO(LOG_CTX_CONTROL,"Everyhing is ok... going to apply change (intIfNum_del=%u, intIfNum_new=%u)", intIfNum_del, intIfNum);

    // hashmode: FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
    if (dtlDot3adInternalPortAdd(lag_intf, 1, &intIfNum, 1) != L7_SUCCESS ||
        dtlDot3adInternalPortDelete(lag_intf, 1, &intIfNum_del, 1) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL,"Failure");
      rc = L7_FAILURE;
    }
    else
    {
      PT_LOG_INFO(LOG_CTX_CONTROL,"Success");
    }
  }
  #endif    //#elif ( PTIN_BOARD == PTIN_BOARD_TA48GE )

  /* Any error? */
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL,"Error commuting to %s slot",(cx_work_slot) ? "working" : "protection");
  }
  else
  {
    /* Save new state */
    cx_work_slot_h = cx_work_slot;
    PT_LOG_NOTICE(LOG_CTX_CONTROL,"Success commuting to %s slot",(cx_work_slot) ? "working" : "protection");
  }

  /* if successfull, resend queries */
  if (rc==L7_SUCCESS)
  {
    ptin_igmp_instances_reactivate();
  }
#endif
#endif

#if PTIN_BOARD_IS_MATRIX
  static L7_uint16 schedule_query_tx = 0;   /* Number of seconds to wait before send queries */

  /* Send queries */
  if (schedule_query_tx>0 && (--schedule_query_tx)==0)
  {
    /* Reset Multicast machine */
    ptin_igmp_proxy_reset();
  }

  /* Check if Multicast reset should be executed next time */
  if (matrix_send_queries)
  {
    schedule_query_tx = SCHEDULE_MATRIX_SEND_QUERY_WAIT;
    matrix_send_queries = L7_FALSE;
  }
#endif
}

#if (!PTIN_BOARD_IS_STANDALONE)
/**
 * Task that checks for Matrix Switchovers
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinSwitchoverTask(L7_uint32 numArgs, void *unit)
{
  L7_RC_t rc;

  PT_LOG_NOTICE(LOG_CTX_CONTROL, "ptinSwitchover running!");
  rc = osapiTaskInitDone(L7_PTIN_SWITCHOVER_TASK_SYNC);

#if (PTIN_BOARD_IS_MATRIX)
  /* Initialize semaphore to access rlink status */
  link_status_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (link_status_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create rlink_status_sem semaphore!");
    PTIN_CRASH();
  }
#endif

  /* Wait for a signal indicating that all other modules
   * configurations were executed */
  PT_LOG_INFO(LOG_CTX_CONTROL, "ptinSwitchover task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_switchover_sem, L7_WAIT_FOREVER);
  PT_LOG_NOTICE(LOG_CTX_CONTROL, "ptinSwitchover task will now start!");

  while (1)
  {
#if (PTIN_BOARD_IS_MATRIX)
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
#ifdef PTIN_LINKSCAN_CONTROL
    /* Switchover monitoring */
    ptin_control_switchover_monitor();
#endif
    /* Links monitoring, in case a WarpCore reset is necessary*/
    ptin_control_linkStatus_monitor();
#endif
#endif

#if (PTIN_BOARD_IS_LINECARD)
    /* Monitor links and report them to matrix board */
    ptin_control_linkstatus_report();
#endif

    osapiSleep(10);
  }
}
#endif

#if (PTIN_BOARD_IS_MATRIX)
#ifdef PTIN_LINKSCAN_CONTROL

#ifdef MAP_CPLD
/* List of active interfaces */
static L7_uint8 switchover_intf_active_h[PTIN_SYSTEM_MAX_N_PORTS];
#endif

/**
 * Monitor switchover process (10s period)
 * 
 * @author mruas (11/21/2013)
 * 
 * @return L7_RC_t 
 */
void ptin_control_switchover_monitor(void)
{
  #ifdef MAP_CPLD
  L7_uint8  port;
  L7_uint32 intIfNum;
  //L7_uint8  slot_id;
  L7_uint16 board_id, slot_id;

  L7_uint8 interfaces_active[PTIN_SYSTEM_MAX_N_PORTS];
  static L7_uint8 matrix_is_active_h = 0xFF;    //L7_TRUE;
  L7_uint8 matrix_is_active;

  matrix_is_active = ptin_fpga_mx_is_matrixactive();

  if (ptin_fpga_mx_is_matrixactive() != ptin_fpga_mx_is_matrixactive_rt())
  {
    return;
  }

  /* First time procedure (after switchover) */
  if (ptin_fpga_mx_is_matrixactive() != matrix_is_active_h)
  {
    //if (!matrix_is_active && 0xFF!=matrix_is_active_h) tx_dot3ad_matrix_sync_t();
    
    matrix_is_active_h = matrix_is_active;

    if (linkscan_update_control)
    {
      PT_LOG_INFO(LOG_CTX_CONTROL, "Switchover detected (to active=%d). Waiting 10 seconds...", matrix_is_active);

      osapiSleep(10);

      PT_LOG_INFO(LOG_CTX_CONTROL, "Going to process switchover init (active=%d)", matrix_is_active);

      osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

      /* For active matrix, disable force link up, and enable linkscan, only for uplink ports */
      if (matrix_is_active)
      {
#if 0
        /* Run all slots */
        for (slot_id = PTIN_SYS_LC_SLOT_MIN; slot_id <= PTIN_SYS_LC_SLOT_MAX; slot_id++)
        {
          /* Nothing to do for non uplink boards */
          if (ptin_slot_boardid_get(slot_id, &board_id) != L7_SUCCESS ||
              !PTIN_BOARD_LS_CTRL(board_id) ||
              !PTIN_BOARD_IS_UPLINK(board_id))
            continue;

          /* Disable force link-up, and enable linkscan for uplink boards */
          /* ... except for protected ports */
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to disable force link-up to slot %u", slot_id); 
          ptin_slot_link_force(slot_id, -1, L7_TRUE, L7_DISABLE);
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Goig to enable linkscan to slot %u", slot_id);
          ptin_slot_linkscan_set(slot_id, -1, L7_ENABLE);
          PT_LOG_INFO(LOG_CTX_CONTROL, "Linkscan enabled for slot %u", slot_id);
        }
#endif
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to disable linkscan to all ports");

        /* Enable linkscan for all ports (links will go down) */
        for (port=0; port<ptin_sys_number_of_ports; port++)
        {
          if ( ptin_intf_boardid_get(port, &board_id) != L7_SUCCESS ||
               !PTIN_BOARD_IS_PRESENT(board_id)                     ||
               !PTIN_BOARD_LS_CTRL(board_id)                        ||
               !PTIN_BOARD_IS_UPLINK(board_id)                      ||
               ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS )
          {
            continue;
          }
          
          /* Disable force link-up, and enable linkscan for uplink boards */
          /* ... except for protected ports */
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to disable force link-up to interface %u", intIfNum); 
          ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Goig to enable linkscan to interface %u", intIfNum);
          ptin_intf_linkscan_set(intIfNum, L7_ENABLE);
          PT_LOG_INFO(LOG_CTX_CONTROL, "Linkscan enabled for interface %u", intIfNum);
        }
      }
      /* For passive matrix, reset all ports, and disable linkscan for all of them */
      else
      {
        /* Clear historic values of active interfaces */
        memset(switchover_intf_active_h, 0x00, sizeof(switchover_intf_active_h));

        PT_LOG_TRACE(LOG_CTX_CONTROL, "Goig to force link-down to all ports");

        /* Disable force linkup for all ports */
        for (port=0; port<ptin_sys_number_of_ports; port++)
        {
          /* For passive board, disable force linkup */
          if ( ptin_intf_boardid_get(port, &board_id) == L7_SUCCESS &&
              (!PTIN_BOARD_IS_PRESENT(board_id) || PTIN_BOARD_LS_CTRL(board_id)) &&
               ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS )
          {
            ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);       /* Disable force link-up */
            ptin_intf_link_force(intIfNum, L7_FALSE, 0);            /* Cause link down */
          }
        }

        PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to enable linkscan to all ports");

        /* Enable linkscan for all ports (links will go down) */
        for (port=0; port<ptin_sys_number_of_ports; port++)
        {
          if ( ptin_intf_boardid_get(port, &board_id) == L7_SUCCESS &&
              (!PTIN_BOARD_IS_PRESENT(board_id) || PTIN_BOARD_LS_CTRL(board_id)) &&
               ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS )
            ptin_intf_linkscan_set(intIfNum, L7_ENABLE);
        }

        /* Wait 3 seconds */
        osapiSleep(2);

        PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to disable linkscan to all ports");

        /* Disable linkscan for all ports */
        for (port=0; port<ptin_sys_number_of_ports; port++)
        {
          if ( ptin_intf_boardid_get(port, &board_id) == L7_SUCCESS &&
              (!PTIN_BOARD_IS_PRESENT(board_id) || PTIN_BOARD_LS_CTRL(board_id)) &&
               ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS)
          {
            ptin_intf_linkscan_set(intIfNum, L7_DISABLE);
          }
        }

        PT_LOG_INFO(LOG_CTX_CONTROL, "Linkscan disabled for all ports");
      }

      /* End of procedure */
      osapiSemaGive(ptin_boardaction_sem);
    }
    return;
  }

  /* Do nothing for active matrix */
  if (ptin_fpga_mx_is_matrixactive() && ptin_fpga_mx_is_matrixactive_rt())
  {
    return;
  }

  /* --- Only for Passive matrix --- */

  /* Query active matrix, abot active ports */
  msg_HwIntfInfo_t ports_info;

  memset(interfaces_active, 0x00, sizeof(interfaces_active));
  memset(&ports_info, 0x00, sizeof(msg_HwIntfInfo_t));

  ports_info.slot_id    = (ptin_fpga_board_slot_get() <= PTIN_SYS_MX1_SLOT) ? PTIN_SYS_MX2_SLOT : PTIN_SYS_MX1_SLOT;
  ports_info.generic_id = 0;
  ports_info.generic_id = ptin_sys_number_of_ports;

  if (send_ipc_message(IPC_HW_FASTPATH_PORT,
                       ((ptin_fpga_board_slot_get() <= PTIN_SYS_MX1_SLOT) ? IPC_MX_IPADDR_PROTECTION : IPC_MX_IPADDR_WORKING),
                       CCMSG_HW_INTF_INFO_GET,
                       (char *) &ports_info,
                       (char *) &ports_info,
                       sizeof(msg_HwIntfInfo_t),
                       NULL) < 0)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Failed to send interfaces query!");
    return;
  }
  #if 0
  PT_LOG_TRACE(LOG_CTX_CONTROL, "ptin_board_slotId=%d",  ptin_board_slotId);
  for (port=0; port<ports_info.number_of_ports; port++)
  {
    PT_LOG_TRACE(LOG_CTX_CONTROL, "port=%u: boardId=%u admin=%u link=%u", port,
              ports_info.port[port].board_id, ports_info.port[port].enable, ports_info.port[port].link);
  }
  #endif

  /* Do nothing for active matrix */
  if (ptin_fpga_mx_is_matrixactive())
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "I am active matrix");
    return;
  }

  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Update list of active interfaces */
  for (port=0; port<ports_info.number_of_ports; port++)
  {
    if (ports_info.port[port].board_id != 0)
    {
      if (ports_info.port[port].enable &&
          ports_info.port[port].link)
      {
        interfaces_active[port] = 1;
      }
    }

    /* Get board id */
    if (ptin_intf_port2SlotPort(port, &slot_id, L7_NULLPTR, &board_id) != L7_SUCCESS)
      continue;
    //ptin_intf_boardid_get(port, &board_id);

    /* Save board_id */
    if ( (ports_info.port[port].board_id != 0) && (board_id != ports_info.port[port].board_id) )
    {
      ptin_intf_boardid_set(port, ports_info.port[port].board_id);
      PT_LOG_INFO(LOG_CTX_CONTROL, "Board id %u set for port %u", ports_info.port[port].board_id, port);

    #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
     #if (PHY_RECOVERY_PROCEDURE)
      /* Reset slot? */
      if (slots_to_be_reseted[slot_id])
      {
        L7_RC_t rc;

        /* Only for TG16G boards */
        if (ports_info.port[port].board_id == PTIN_BOARD_TYPE_TG16G)
        {
          PT_LOG_INFO(LOG_CTX_INTF, "Going to reset warpcore of slot %u", slot_id);
          rc = ptin_intf_slot_reset(slot_id, L7_FALSE);
          if (rc == L7_SUCCESS)
          {
            PT_LOG_INFO(LOG_CTX_INTF, "Slot %d reseted", slot_id);
          }
          else if (rc == L7_NOT_EXIST)
          {
            PT_LOG_TRACE(LOG_CTX_INTF, "Nothing done to slot %u", slot_id);
          }
          else
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error reseting slot %u", slot_id);
          }
        }
        /* Clear flag */
        slots_to_be_reseted[slot_id] = L7_FALSE;
      }
     #endif
    #endif
    }
  }

  osapiSemaGive(ptin_boardaction_sem);

  /* Update port settings */
  if (linkscan_update_control)
  {
    /* Do nothing for active matrix */
    if (ptin_fpga_mx_is_matrixactive())
    {
      PT_LOG_ERR(LOG_CTX_CONTROL, "I am active matrix");
      return;
    }

    osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

    /* Update port state */
    for (port = 0; port < PTIN_SYSTEM_MAX_N_PORTS; port++)
    {
      /* Skip unchanged ports */
      if (interfaces_active[port] == switchover_intf_active_h[port])
        continue;

      PT_LOG_INFO(LOG_CTX_CONTROL, "Port %u status changed to %u", port, interfaces_active[port]);

      /* Update new changes */
      switchover_intf_active_h[port] = interfaces_active[port];

      /* --- Passive board --- */

      /* For passive board, update force link states */
      if (ptin_intf_boardid_get(port, &board_id) != L7_SUCCESS || !PTIN_BOARD_LS_CTRL(board_id))
      {
        PT_LOG_TRACE(LOG_CTX_CONTROL, "ptin_port %d is not under linkscan control.", port);
        continue;
      }
      if (ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS) 
      {
        PT_LOG_ERR(LOG_CTX_CONTROL, "Error getting intIfNum from ptin_port %d!", port);
        continue;
      }
      /* Enable/Disable force linkup */
      if (interfaces_active[port])
      {
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Forcing link up to port %u", port);
        if (ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE) != L7_SUCCESS) 
        {
          PT_LOG_ERR(LOG_CTX_CONTROL, "Error setting force link to %u for ptin_port %d!", interfaces_active[port], port);
          continue;
        }
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Forcing link down to port %u", port);
        /* Disable force link-up */
        if (ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_CONTROL, "Error disabling force link-up for ptin_port %d!", interfaces_active[port], port);
          continue;
        }
        /* Cause a linkdown */
        if (ptin_intf_link_force(intIfNum, L7_FALSE, 0) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_CONTROL, "Error causing change link for ptin_port %d!", interfaces_active[port], port);
          continue;
        }
      }
      PT_LOG_INFO(LOG_CTX_CONTROL, "Link forced to %u for port%u", interfaces_active[port], port);
    }
    osapiSemaGive(ptin_boardaction_sem);
  }


  /* Update board id */
  /* When board is removed, board_id is '0' and the force link down process was not triggered */
  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Update list of active interfaces */
  for (port=0; port<ports_info.number_of_ports; port++)
  {
    /* Get board id */
    if (ptin_intf_port2SlotPort(port, &slot_id, L7_NULLPTR, &board_id) != L7_SUCCESS)
      continue;
    //ptin_intf_boardid_get(port, &board_id);

    /* Save board_id */
    if (board_id != ports_info.port[port].board_id )
    {
      ptin_intf_boardid_set(port, ports_info.port[port].board_id);
      PT_LOG_INFO(LOG_CTX_CONTROL, "Board id %u set for port %u", ports_info.port[port].board_id, port);

    #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
     #if (PHY_RECOVERY_PROCEDURE)
      /* Board remove? */
      if (ports_info.port[port].board_id == 0)
      {
        slots_to_be_reseted[slot_id] = L7_TRUE;
        PT_LOG_INFO(LOG_CTX_CONTROL, "Slot %u marked to be reseted ", slot_id);
      }
     #endif
    #endif
    }
  }

  osapiSemaGive(ptin_boardaction_sem);

  #endif
}
#endif /*PTIN_LINKSCAN_CONTROL*/

/**
 * LinkStatus monitor (10s period)
 * 
 * @author mruas (10/1/2015)
 */
void ptin_control_linkStatus_monitor(void)
{
  L7_BOOL slot_in_error;
  L7_int8 credits_to_loose;
  L7_uint32 port, index;
  L7_uint16 board_id, slot_id;
  struct_linkStatus_t linkStatus_copy[2], info;
  static L7_int counter=18;

  /* 60s period */
  if ((--counter) > 0)
  {
    return;
  }
  counter = 6;

  /* Wait for switch to be stable */
  if (ptin_fpga_mx_is_matrixactive() != ptin_fpga_mx_is_matrixactive_rt())
  {
    return;
  }

  /* Do nothing for inactive matrix */
  if (!ptin_fpga_mx_is_matrixactive())
  {
    return;
  }

  /* Update local interfaces status */
  ptin_control_localLinkStatus_update();

  PT_LOG_TRACE(LOG_CTX_CONTROL,"Monitoring linkStatus...");

  /* Run all slots */
  for (slot_id = PTIN_SYS_LC_SLOT_MIN; slot_id <= PTIN_SYS_LC_SLOT_MAX; slot_id++)
  {
    /* If monitor disabled, skip slot */
    if (!ls_monitor_info[slot_id].monitor_enable)
    {
      continue;
    }

    /* Skip non downlink boards */
    if (ptin_slot_boardid_get(slot_id, &board_id)!=L7_SUCCESS || !PTIN_BOARD_IS_DOWNLINK(board_id))
    {
      continue;
    }

    /* Initialize temp vars */
    slot_in_error = L7_FALSE;
    credits_to_loose = 0;

    /* Run all slot ports */
    for (index = 0; index < PTIN_SYS_INTFS_PER_SLOT_MAX; index++)
    {
      /* Get ptin_port */
      if (ptin_intf_slotPort2port(slot_id, index, &port)!=L7_SUCCESS || port>=ptin_sys_number_of_ports)
      {
        continue;
      }

      /* Going to access rlink data */
      osapiSemaTake(link_status_sem, L7_WAIT_FOREVER);
      /* Copy data */
      linkStatus_copy[0] = local_link_status[port];   /* Local links */
      linkStatus_copy[1] = remote_link_status[port];  /* Remote links */
      /* Clear update flags */
      local_link_status[port].updated = L7_FALSE;
      remote_link_status[port].updated = L7_FALSE;
      /* Release semaphore */
      osapiSemaGive(link_status_sem);

      PT_LOG_TRACE(LOG_CTX_CONTROL, "local linkStatus: Port %u -> upd=%u en=%u link=%u stats={tx:%llu rx:%llu er:%llu}",
                port, linkStatus_copy[0].updated, linkStatus_copy[0].enable, linkStatus_copy[0].link, linkStatus_copy[0].tx_packets, linkStatus_copy[0].rx_packets, linkStatus_copy[0].rx_error);
      PT_LOG_TRACE(LOG_CTX_CONTROL, "Remot linkStatus: Port %u -> upd=%u en=%u link=%u stats={tx:%llu rx:%llu er:%llu}",
                port, linkStatus_copy[1].updated, linkStatus_copy[1].enable, linkStatus_copy[1].link, linkStatus_copy[1].tx_packets, linkStatus_copy[1].rx_packets, linkStatus_copy[1].rx_error);

      /* Calculate result information */

      /* Clear info struct to save linkStatus data (combines local and remote data) */
      memset(&info, 0x00, sizeof(info));

      /* Initial values: OK situation */
      info.updated    = 0;  /* To be ORed to*/
      info.enable     = 1;  /* To be ANDed to */
      info.link       = 1;  /* To be ANDed to */
      info.tx_packets = 0;  /* To be sumed to */
      info.rx_packets = 0;  /* To be sumed to */
      info.rx_error   = 0;  /* To be sumed to */
      /* Local data */
      if (linkStatus_copy[0].updated)
      {
        info.updated    = L7_TRUE;
        info.enable     &= linkStatus_copy[0].enable;
        info.link       &= linkStatus_copy[0].link;
        info.tx_packets += linkStatus_copy[0].tx_packets;
        info.rx_packets += linkStatus_copy[0].rx_packets;
        info.rx_error   += linkStatus_copy[0].rx_error;
      }
      /* Remote data */
      if (linkStatus_copy[1].updated)
      {
        info.updated    = L7_TRUE;
        info.enable     &= linkStatus_copy[1].enable;
        info.link       &= linkStatus_copy[1].link;
        info.tx_packets += linkStatus_copy[1].tx_packets;
        info.rx_packets += linkStatus_copy[1].rx_packets;
        info.rx_error   += linkStatus_copy[1].rx_error;
      }

      PT_LOG_TRACE(LOG_CTX_CONTROL, "linkStatus: Port %u -> upd=%u en=%u link=%u stats={tx:%llu rx:%llu er:%llu}",
                port, info.updated, info.enable, info.link, info.tx_packets, info.rx_packets, info.rx_error);

      /* Skip not updated or disabled interfaces */
      if (!info.updated || !info.enable)
      {
        continue;
      }

      /* If link is down, take 5 credits */
      if (!info.link)
      {
        slot_in_error = L7_TRUE;
        credits_to_loose = max(credits_to_loose, 5);
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Port %u has issues: link=%u", port, info.link);
      }
      else if (ls_monitor_info[slot_id].fcs_threshold > 0)
      {
        /* Or have FCS errors (100 errors in 60 seconds), take only 2 credits */
        if (info.rx_error >= ls_monitor_info[slot_id].fcs_threshold)
        {
          slot_in_error = L7_TRUE;
          credits_to_loose = max(credits_to_loose, 2);
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Port %u has issues: tx=%llu rx=%llu er=%llu", port,
                    info.tx_packets, info.rx_packets, info.rx_error);
        }
        #if 0
        /* Have FCS errors, but are very low... keep credits */
        else if (info.rx_error > 10)
        {
          slot_in_error = L7_TRUE;
          credits_to_loose = max(credits_to_loose, 0);
          PT_LOG_TRACE(LOG_CTX_CONTROL, "Port %u has issues: tx=%llu rx=%llu er=%llu", port,
                    info.tx_packets, info.rx_packets, info.rx_error);
        }
        #endif
      }
    }

    if (slot_in_error)
    {
      /* Decrement credits */
      ls_monitor_info[slot_id].credits -= credits_to_loose;
      PT_LOG_TRACE(LOG_CTX_CONTROL, "Slot %u has issues: credits=%u", slot_id, ls_monitor_info[slot_id].credits);

      /* If no credits are assigned to this port, reset slot */
      if (ls_monitor_info[slot_id].credits <= 0)
      {
        PT_LOG_WARN(LOG_CTX_CONTROL, "Credits exhausted: Going to reset slot %u", slot_id);

        /* Reset warpcore */
        ptin_control_slot_reset(port, slot_id, board_id, linkStatus_copy);

        /* One more reset */
        ls_monitor_info[slot_id].resets_counter++;

        /* If we have 6 consecutive WC resets, simply give up! */
        if (ls_monitor_info[slot_id].resets_counter >= LS_RESETS_MAX)
        {
          ls_monitor_info[slot_id].monitor_enable = L7_FALSE;
          ls_monitor_info[slot_id].resets_counter = 0;
          PT_LOG_WARN(LOG_CTX_CONTROL, "Slot %u will never be reset", slot_id);
        }
        /* Credits restored */
        ls_monitor_info[slot_id].credits = LS_CREDITS_MAX;
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Credits restored to slot %u", slot_id);
      }
    }
    /* If no error occured within all ports of a slot, full credits are given */
    else
    {
      if (ls_monitor_info[slot_id].credits < LS_CREDITS_MAX)
        PT_LOG_TRACE(LOG_CTX_CONTROL, "Credits restored to slot %u", slot_id);
      ls_monitor_info[slot_id].credits = LS_CREDITS_MAX;
      ls_monitor_info[slot_id].resets_counter = 0;
    }
  }
}

/**
 * Procedures to be done when resetting a slot
 * 
 * @author mruas (10/2/2015)
 * 
 * @param ptin_port : Port responsible for this reset
 * @param slot_id : Slot associated to this port
 * @param board_id : Board id associated to this slot
 * @param linkStatus : Link status information (Local + Remote)
                                                                */
void ptin_control_slot_reset(L7_uint ptin_port, L7_uint slot_id, L7_uint board_id,
                                    struct_linkStatus_t linkStatus[2])
{
  L7_BOOL force_link;
  struct timespec tm;
  struct L7_localtime_t lt;

  PT_LOG_WARN(LOG_CTX_CONTROL, "Going to reset WC of port %u", ptin_port);

#ifdef PTIN_LINKSCAN_CONTROL
  force_link = (board_id == PTIN_BOARD_TYPE_TG16G);
#else
  force_link = L7_FALSE;
#endif

  /* Reset WarpCore */
  ptin_intf_slot_reset(slot_id, force_link);

  /* Get time */
  clock_gettime(CLOCK_REALTIME, &tm);
  osapiLocalTime(tm.tv_sec, &lt);
  lt.L7_mon  += 1;
  lt.L7_year += 1900;

  /* Save detailed information about this event */
  slotReset_record_last[slotReset_event_index].slot_id           = slot_id;
  slotReset_record_last[slotReset_event_index].board_id          = board_id;
  slotReset_record_last[slotReset_event_index].mx_ptin_port      = ptin_port;
  slotReset_record_last[slotReset_event_index].local_linkStatus  = linkStatus[0];
  slotReset_record_last[slotReset_event_index].remote_linkStatus = linkStatus[1];
  slotReset_record_last[slotReset_event_index].time              = lt;

  PT_LOG_TRACE(LOG_CTX_CONTROL, "Saved record of this event:");
  PT_LOG_TRACE(LOG_CTX_CONTROL, " slot_id      : %u", slotReset_record_last[slotReset_event_index].slot_id);
  PT_LOG_TRACE(LOG_CTX_CONTROL, " board_id     : %u", slotReset_record_last[slotReset_event_index].board_id);
  PT_LOG_TRACE(LOG_CTX_CONTROL, " mx_ptin_port : %u", slotReset_record_last[slotReset_event_index].mx_ptin_port);
  PT_LOG_TRACE(LOG_CTX_CONTROL, " local_linkStatus  : upd=%u en=%u link=%u tx=%llu rx=%llu er=%llu",
            slotReset_record_last[slotReset_event_index].local_linkStatus.updated,
            slotReset_record_last[slotReset_event_index].local_linkStatus.enable,
            slotReset_record_last[slotReset_event_index].local_linkStatus.link,
            slotReset_record_last[slotReset_event_index].local_linkStatus.tx_packets,
            slotReset_record_last[slotReset_event_index].local_linkStatus.rx_packets,
            slotReset_record_last[slotReset_event_index].local_linkStatus.rx_error);
  PT_LOG_TRACE(LOG_CTX_CONTROL, " remote_linkStatus : upd=%u en=%u link=%u tx=%llu rx=%llu er=%llu",
            slotReset_record_last[slotReset_event_index].remote_linkStatus.updated,
            slotReset_record_last[slotReset_event_index].remote_linkStatus.enable,
            slotReset_record_last[slotReset_event_index].remote_linkStatus.link,
            slotReset_record_last[slotReset_event_index].remote_linkStatus.tx_packets,
            slotReset_record_last[slotReset_event_index].remote_linkStatus.rx_packets,
            slotReset_record_last[slotReset_event_index].remote_linkStatus.rx_error);
  PT_LOG_TRACE(LOG_CTX_CONTROL, " time = %u/%u/%u, %u:%u:%u",
            slotReset_record_last[slotReset_event_index].time.L7_mday,
            slotReset_record_last[slotReset_event_index].time.L7_mon,
            slotReset_record_last[slotReset_event_index].time.L7_year,
            slotReset_record_last[slotReset_event_index].time.L7_hour,
            slotReset_record_last[slotReset_event_index].time.L7_min,
            slotReset_record_last[slotReset_event_index].time.L7_sec);

  slotReset_event_index = (slotReset_event_index + 1) % SLOTRESET_SAVE_LAST_EVENTS;
}

/**
 * Reset linkStatus data
 * 
 * @param ptin_port 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_control_linkStatus_reset(L7_uint ptin_port)
{
  /* Validate port info */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  memset(&local_link_status[ptin_port], 0x00, sizeof(struct_linkStatus_t));
  memset(&remote_link_status[ptin_port], 0x00, sizeof(struct_linkStatus_t));

  PT_LOG_TRACE(LOG_CTX_CONTROL, "LinkStatus data reseted");

  return L7_SUCCESS;
}

/**
 * Update local linkStatus data (for all interfaces)
 */
void ptin_control_localLinkStatus_update(void)
{
  L7_uint8  port;
  L7_uint16 board_id;
  ptin_HWEthPhyConf_t local_phyConfig;
  ptin_HWEthRFC2819_PortStatistics_t local_counters;

  PT_LOG_TRACE(LOG_CTX_CONTROL,"Updating local link status...");

  /* Going to access rlink data */
  osapiSemaTake(link_status_sem, L7_WAIT_FOREVER);

  /* Get local ports configuration */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    /* Clear updated flag */
    local_link_status[port].updated = L7_FALSE;

    /* Only consider ports where, there is a downlink board */
    if (ptin_intf_boardid_get(port, &board_id)!=L7_SUCCESS || !PTIN_BOARD_IS_DOWNLINK(board_id))
    {
      osapiSemaGive(link_status_sem);
      continue;
    }

    /* Skip not valid ports, or disabled ones */
    memset(&local_phyConfig, 0x00, sizeof(local_phyConfig));
    local_phyConfig.Port = port;
    local_phyConfig.Mask = 0xffff;
    if (ptin_intf_PhyConfig_get(&local_phyConfig)!=L7_SUCCESS)
    {
      osapiSemaGive(link_status_sem);
      continue;
    }

    /* Do not consider disabled interfaces */
    if (!local_phyConfig.PortEnable)
    {
      osapiSemaGive(link_status_sem);
      continue;
    }

    /* Port is enabled? */
    local_link_status[port].enable = local_phyConfig.PortEnable;

    /* Get local link status */
    local_link_status[port].link = ptin_intf_link_get(port);

    if (local_link_status[port].link)
    {
      /* Get local counters */
      memset(&local_counters, 0x00, sizeof(local_counters));
      local_counters.Port = port;
      local_counters.Mask = 0xff;
      local_counters.RxMask = 0xffff;
      local_counters.TxMask = 0xffff;
      if (ptin_intf_counters_read(&local_counters)==L7_SUCCESS)
      {
        /* local counters */
        local_link_status[port].tx_packets = local_counters.Tx.etherStatsPkts - local_link_status[port].tx_packets;
        local_link_status[port].rx_packets = local_counters.Rx.etherStatsPkts - local_link_status[port].rx_packets;
        local_link_status[port].rx_error   = (local_counters.Rx.etherStatsCRCAlignErrors +
                                              local_counters.Rx.etherStatsFragments +
                                              local_counters.Rx.etherStatsJabbers) -
                                             local_link_status[port].rx_error;
      }
    }

    /* Set updated flag */
    local_link_status[port].updated = L7_TRUE;

    PT_LOG_TRACE(LOG_CTX_CONTROL, "Local linkStatus updated: Port %u -> upd=%u en=%u link=%u stats={tx:%llu rx:%llu er:%llu}",
              port, local_link_status[port].updated, local_link_status[port].enable, local_link_status[port].link,
              local_link_status[port].tx_packets, local_link_status[port].rx_packets, local_link_status[port].rx_error);
  }

  /* Release semaphore */
  osapiSemaGive(link_status_sem);

  PT_LOG_TRACE(LOG_CTX_CONTROL,"Local link status updated!");
}

/**
 * Update remote linkStatus for a particular interface
 * 
 * @param ptin_port 
 * @param info 
 */
void ptin_control_remoteLinkstatus_update(L7_uint32 ptin_port, struct_linkStatus_t *info)
{
  L7_uint16 board_id;

  /* Validate port info */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Invalid port %u", ptin_port);
    return;
  }

  PT_LOG_TRACE(LOG_CTX_CONTROL,"Received linkStatus report from linecard (ptin_port=%u)...", ptin_port);

  /* Going to access rlink data */
  osapiSemaTake(link_status_sem, L7_WAIT_FOREVER);

  /* Clear updated flag */
  remote_link_status[ptin_port].updated = L7_FALSE;

  /* Validate board related to this port: only downlink boards */
  if (ptin_intf_boardid_get(ptin_port, &board_id)!=L7_SUCCESS || !PTIN_BOARD_IS_DOWNLINK(board_id))
  {
    osapiSemaGive(link_status_sem);
    return;
  }

  /* Do not consider disabled interfaces */
  if (!info->enable)
  {
    osapiSemaGive(link_status_sem);
    return;
  }

  /* Update structure (only for enabled interfaces) */
  remote_link_status[ptin_port].enable = info->enable;
  remote_link_status[ptin_port].link   = info->link;

  /* Update stats (only for link up interfaces) */
  if (info->link)
  {
    /* Get counters difference */
    remote_link_status[ptin_port].tx_packets = info->tx_packets - remote_link_status[ptin_port].tx_packets;
    remote_link_status[ptin_port].rx_packets = info->rx_packets - remote_link_status[ptin_port].rx_packets;
    remote_link_status[ptin_port].rx_error   = info->rx_error   - remote_link_status[ptin_port].rx_error;
  }

  /* Set updated flag */
  remote_link_status[ptin_port].updated = L7_TRUE;

  /* Release semaphore */
  osapiSemaGive(link_status_sem);

  PT_LOG_TRACE(LOG_CTX_CONTROL, "Remote linkStatus updated: Port %u -> upd=%u en=%u link=%u stats={tx:%llu rx:%llu er:%llu}",
            ptin_port, remote_link_status[ptin_port].updated, remote_link_status[ptin_port].enable, remote_link_status[ptin_port].link,
            remote_link_status[ptin_port].tx_packets, remote_link_status[ptin_port].rx_packets, remote_link_status[ptin_port].rx_error);
}

/**
 * For dev shell commands 
 * Resets local and remote link status of a port
 * 
 * @param port (-1 for all ports)
 */
void slot_monitor_reset(L7_int port)
{
  L7_uint i;

  /* Going to access rlink data */
  osapiSemaTake(link_status_sem, L7_WAIT_FOREVER);
  for (i = 0; i < ptin_sys_number_of_ports; i++)
  {
    if (port >=0 && port != i)  continue;

    memset(&local_link_status[i] , 0x00, sizeof(struct_linkStatus_t));
    memset(&remote_link_status[i], 0x00, sizeof(struct_linkStatus_t));

    printf("Port %u linkstatus reset!\r\n", i);
  }
  /* Release semaphore */
  osapiSemaGive(link_status_sem);
}

/**
 * Dev shell command 
 * Enable or disable link status monitoring 
 * 
 * @param enable 
 */
void slot_monitor_enable(L7_int slot, L7_BOOL monitor_enable, L7_uint fcs_enable)
{
  L7_uint8  s, p;
  L7_uint32 ptin_port;

  /* Validate slot */
  if (slot >= 0 && (slot < PTIN_SYS_LC_SLOT_MIN || slot > PTIN_SYS_LC_SLOT_MAX))
  {
    printf("Invalid slot %u\r\n", slot);
    return;
  }

  /* Run all slots... */
  for (s = PTIN_SYS_LC_SLOT_MIN; s <= PTIN_SYS_LC_SLOT_MAX; s++)
  {
    /* ... but skip not requested ones */
    if (slot>=0 && slot!=s)  continue;
    
    /* Reset static structures */
    if (!ls_monitor_info[s].monitor_enable && monitor_enable)
    {
      /* Running all slot ports */
      for (p = 0; p < PTIN_SYS_INTFS_PER_SLOT_MAX; p++)
      {
        /* Reset local end remote data */
        if (ptin_intf_slotPort2port(s, p, &ptin_port) == L7_SUCCESS)
        {
          slot_monitor_reset(ptin_port);
        }
      } 
    }

    /* Reset local and remote link data */
    ls_monitor_info[s].monitor_enable = monitor_enable & 1;
    ls_monitor_info[s].fcs_threshold  = fcs_enable;
    ls_monitor_info[s].credits        = LS_CREDITS_MAX;
    ls_monitor_info[s].resets_counter = 0;

    printf("Slot %u: Link status monitoring=%u / FCS control=%u\r\n", s, monitor_enable, fcs_enable);
  }
}

/**
 * Dev shell command 
 * Dump all linkstatus data 
 */
void slot_monitor_dump(void)
{
  L7_uint i, event;

  printf("Records of last Reset events:\r\n");

  for (i=0; i<SLOTRESET_SAVE_LAST_EVENTS; i++)
  {
    event = (slotReset_event_index + i) % SLOTRESET_SAVE_LAST_EVENTS;

    printf("Event n-%d:\r\n", SLOTRESET_SAVE_LAST_EVENTS-i);
    printf("\tslot_id      : %u\r\n", slotReset_record_last[event].slot_id); 
    printf("\tboard_id     : 0x%x\r\n", slotReset_record_last[event].board_id);
    printf("\tmx_ptin_port : %u\r\n", slotReset_record_last[event].mx_ptin_port);
    printf("\tlocal_linkStatus  : upd=%u en=%u link=%u tx=%llu rx=%llu er=%llu\r\n",
           slotReset_record_last[event].local_linkStatus.updated,
           slotReset_record_last[event].local_linkStatus.enable,
           slotReset_record_last[event].local_linkStatus.link,
           slotReset_record_last[event].local_linkStatus.tx_packets,
           slotReset_record_last[event].local_linkStatus.rx_packets,
           slotReset_record_last[event].local_linkStatus.rx_error);
    printf("\tremote_linkStatus : upd=%u en=%u link=%u tx=%llu rx=%llu er=%llu\r\n",
           slotReset_record_last[event].remote_linkStatus.updated,
           slotReset_record_last[event].remote_linkStatus.enable,
           slotReset_record_last[event].remote_linkStatus.link,
           slotReset_record_last[event].remote_linkStatus.tx_packets,
           slotReset_record_last[event].remote_linkStatus.rx_packets,
           slotReset_record_last[event].remote_linkStatus.rx_error);
    printf("\ttime = %u/%u/%u, %u:%u:%u\r\n",
           slotReset_record_last[event].time.L7_mday,
           slotReset_record_last[event].time.L7_mon,
           slotReset_record_last[event].time.L7_year,
           slotReset_record_last[event].time.L7_hour,
           slotReset_record_last[event].time.L7_min,
           slotReset_record_last[event].time.L7_sec);
  }

  printf("\r\nCurrent remote link status:\r\n");
  /* Going to access rlink data */
  osapiSemaTake(link_status_sem, L7_WAIT_FOREVER);
  for (i = 0; i < ptin_sys_number_of_ports; i++)
  {
    if (remote_link_status[i].updated)
    {
      printf("Port %2u: enable=%u link=%u tx_packets=%llu rx_packets=%llu rx_errors=%llu\r\n", i,
              remote_link_status[i].enable,
              remote_link_status[i].link,
              remote_link_status[i].tx_packets,
              remote_link_status[i].rx_packets,
              remote_link_status[i].rx_error);
    }
  }
  /* Release semaphore */
  osapiSemaGive(link_status_sem);

  printf("\r\nSlot monitoring control info:\r\n");
  for (i = PTIN_SYS_LC_SLOT_MIN; i <= PTIN_SYS_LC_SLOT_MAX; i++)
  {
    printf("Slot %2u:  monitor_enable=%u  fcs_threshold=%-5u  credits=%-2u  #resets=%-2u\r\n", i,
            ls_monitor_info[i].monitor_enable,
            ls_monitor_info[i].fcs_threshold,
            ls_monitor_info[i].credits,
            ls_monitor_info[i].resets_counter);
  }
}

#endif /*PTIN_BOARD_IS_MATRIX*/

#if (PTIN_BOARD_IS_LINECARD)
/**
 * This function is applied to linecards, and will report the 
 * matrix board about its local link status 
 * 
 * @author mruas (9/28/2015)
 */
static void ptin_control_linkstatus_report(void)
{
  L7_uint8 active_matrix;
  L7_int  base_port, number_of_ports, port, i;
  ptin_HWEthRFC2819_PortStatistics_t portStats;
  ptin_HWEthPhyConf_t phyConf;
  msg_HwIntfStatus_t  msgLinkStatus;
  static L7_int counter=12;

  /* 60s period */
  if ((--counter) > 0)
  {
    return;
  }
  counter = 6;

  PT_LOG_TRACE(LOG_CTX_CONTROL, "Going to send links report...");

  /* Which matrix is active? */
  active_matrix = ptin_fpga_mx_get_matrixactive();

  /* Determine base port and number of ports */
#if (PTIN_BOARD == PTIN_BOARD_TG16G || PTIN_BOARD == PTIN_BOARD_TG16GF)
  base_port = PTIN_SYSTEM_N_PONS;
  number_of_ports = 4;
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
  base_port = (active_matrix == PTIN_SLOT_WORK) ? (PTIN_SYSTEM_N_ETH+1) : (PTIN_SYSTEM_N_ETH);
  number_of_ports = 1;
#else
  return;
#endif

  /* Init struct */
  memset(&msgLinkStatus, 0x00, sizeof(msgLinkStatus));
  msgLinkStatus.slot_id    = ptin_fpga_board_slot_get();
  msgLinkStatus.generic_id = 0;
  msgLinkStatus.number_of_ports = number_of_ports;

  /* Run all (active) backplane ports */
  for (i = 0; i < number_of_ports; i++)
  {
    port = base_port + i;

    /* Init struct */
    msgLinkStatus.port[i].intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    msgLinkStatus.port[i].intf.intf_id   = port;

    /* Get enable configuration */
    memset(&phyConf, 0x00, sizeof(phyConf));
    phyConf.Port = port;
    phyConf.Mask = 0xffff;
    if (ptin_intf_PhyConfig_get(&phyConf) == L7_SUCCESS)
    {
      msgLinkStatus.port[i].enable = phyConf.PortEnable;
    }

    /* Get link status */
    msgLinkStatus.port[i].link = ptin_intf_link_get(port);

    /* Only read stats, if link is up */
    if (msgLinkStatus.port[i].link)
    {
      /* Get stats */
      memset(&portStats, 0x00, sizeof(portStats));
      portStats.Port = port;
      portStats.Mask = 0xff;
      portStats.RxMask = 0xffff;
      portStats.TxMask = 0xffff;
      if (ptin_intf_counters_read(&portStats) == L7_SUCCESS)
      {
        msgLinkStatus.port[i].tx_packets = portStats.Tx.etherStatsPkts;
        msgLinkStatus.port[i].rx_packets = portStats.Rx.etherStatsPkts;
        msgLinkStatus.port[i].rx_error   = portStats.Rx.etherStatsCRCAlignErrors +
                                            portStats.Rx.etherStatsFragments +
                                            portStats.Rx.etherStatsJabbers;
      }
    }

    PT_LOG_TRACE(LOG_CTX_CONTROL, "Link status: Port %u -> enable=%u link=%u stats={tx:%llu rx:%llu er:%llu}", 
              port, msgLinkStatus.port[i].enable, msgLinkStatus.port[i].link,
              msgLinkStatus.port[i].tx_packets, msgLinkStatus.port[i].rx_packets, msgLinkStatus.port[i].rx_error);
  }

  /* Send report to active matrix */
  if (send_ipc_message(IPC_HW_FP_CTRL_PORT2,
                       ptin_fpga_matrix_ipaddr_get(PTIN_FPGA_ACTIVE_MATRIX),
                       CCMSG_HW_INTF_STATUS,
                       (char *) &msgLinkStatus,
                       L7_NULLPTR,
                       sizeof(msg_HwIntfStatus_t),
                       NULL) < 0)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Failed to send interfaces report!");
    return;
  }

  PT_LOG_TRACE(LOG_CTX_CONTROL, "LinkStatus Report sent");
}
#endif /*PTIN_BOARD_IS_LINECARD*/

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
      PT_LOG_INFO(LOG_CTX_CNFGR, "Event processed: rc=%d", rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_CNFGR, "Error receiving queue messages");
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

  //PT_LOG_INFO(LOG_CTX_CONTROL, "Event received: event=%u, intIfNum=%u",event, intIfNum);

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

  /* Send event to queue */
  /* For CXO, check if slot have an inserted card */
  #if (PTIN_BOARD_IS_MATRIX)
  L7_uint16 slot_id;
  L7_uint16 board_id;

  /* Get slot associated to this interface */
  if (ptin_intf_intIfNum2SlotPort(intIfNum, &slot_id, L7_NULLPTR, &board_id) != L7_SUCCESS || slot_id > PTIN_SYS_SLOTS_MAX)
  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Error getting slot for intIfNum %u", intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  /* Only disable linkscan if slot have an inserted board */
  if (board_id == L7_NULL)
  {
    PT_LOG_WARN(LOG_CTX_CONTROL, "Slot %u (intIfNum %u) not present", slot_id, intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  #endif

  eventMsg.event    = event; 
  eventMsg.intIfNum = intIfNum;
  if (osapiMessageSend(ptin_intf_event_queue, &eventMsg, sizeof(ptinIntfEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)

  {
    PT_LOG_ERR(LOG_CTX_CONTROL, "Error sending message to queue: event=%u, intIfNum=%u",event, intIfNum);
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "Message sent to queue: event=%u, intIfNum=%u",event, intIfNum);
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

  if (eventMsg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  #ifdef PTIN_LINKSCAN_CONTROL
  if ( eventMsg->event == L7_UP ) /* No need to process any other NIM event than these  */
  {
    PT_LOG_INFO(LOG_CTX_CONTROL, "Link up detected at interface intIfNum %u", eventMsg->intIfNum);

    /* Disable linkscan */
    rc = ptin_intf_linkscan_set(eventMsg->intIfNum, L7_DISABLE);

    PT_LOG_INFO(LOG_CTX_CONTROL, "Linkscan disabled: rc=%d, intIfNum=%u", rc, eventMsg->intIfNum);
  }
  #endif

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

  /* Event types to be received */
  nimRegisterIntfEvents(L7_PTIN_COMPONENT_ID, portEvent_mask);

  nimStartupEventDone(L7_PTIN_COMPONENT_ID);
}

/**
 * Routine to be executed periodically, to synchronize recovery 
 * clocks 
 */
static void ptin_control_syncE(void)
{
#if defined (SYNC_SSM_IS_SUPPORTED) && (PTIN_BOARD_IS_STANDALONE)
  static int recovery_clock_h[2] = { -1, -1 };

  L7_int ptin_port_main = -1;
  L7_int ptin_port_bckp = -1;

  /* If shared memory is not defined, do nothing */
  if (pfw_shm == L7_NULLPTR || pfw_shm == &fw_shm)
    return;

  /* Check if values changed, and if they are valid */
  if (pfw_shm->SyncE_Recovery_clock[0] != recovery_clock_h[0] &&
      pfw_shm->SyncE_Recovery_clock[0] < ptin_sys_number_of_ports)
  {
    ptin_port_main = pfw_shm->SyncE_Recovery_clock[0];

    recovery_clock_h[0] = pfw_shm->SyncE_Recovery_clock[0];   /* Save new value */
  }
  if (pfw_shm->SyncE_Recovery_clock[1] != recovery_clock_h[1] &&
      pfw_shm->SyncE_Recovery_clock[1] < ptin_sys_number_of_ports)
  {
    ptin_port_bckp = pfw_shm->SyncE_Recovery_clock[1];

    recovery_clock_h[1] = pfw_shm->SyncE_Recovery_clock[1];   /* Save new value */
  }

  /* If some of the recovery ports are defined, apply configuration */
  if (ptin_port_main >= 0 || ptin_port_bckp >= 0)
  {
    if (ptin_intf_clock_recover_set(ptin_port_main, ptin_port_bckp) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_CONTROL, "Error applying new recovery clocks: main port=%d, backup port=%d", ptin_port_main, ptin_port_bckp);
    }
  }
#endif
}

/**
 * Dump recovery clock references
 */
void ptin_reclk_dump(void)
{
#if defined (SYNC_SSM_IS_SUPPORTED) && (PTIN_BOARD_IS_STANDALONE)
  /* If shared memory is not defined, do nothing */
  if (pfw_shm == L7_NULLPTR || pfw_shm == &fw_shm)
  {
    printf("Shared memory not defined!\r\n");
  }
  else
  {
    printf("Sync-E reference clocks:\r\n");
    printf(" Primary port = %u\r\n", pfw_shm->SyncE_Recovery_clock[0]);
    printf(" Backup  port = %u\r\n", pfw_shm->SyncE_Recovery_clock[1]);
  }
#else
  printf("Platform does not support sync-E!\r\n");
#endif
  fflush(stdout);
}

/**
 * Set recovery clock references
 */
void ptin_reclk_write(L7_int primary, L7_int backup)
{
#if defined (SYNC_SSM_IS_SUPPORTED) && (PTIN_BOARD_IS_STANDALONE)
  /* If shared memory is not defined, do nothing */
  if (pfw_shm == L7_NULLPTR || pfw_shm == &fw_shm)
  {
    printf("Shared memory not defined!\r\n");
  }
  else
  {
    if (primary >= 0 && primary < ptin_sys_number_of_ports)
    {
      pfw_shm->SyncE_Recovery_clock[0] = primary;
      printf("Updated primary recovery clock\r\n");
    }
    if (backup >= 0 && backup < ptin_sys_number_of_ports)
    {
      pfw_shm->SyncE_Recovery_clock[1] = backup;
      printf("Updated backup recovery clock\r\n");
    }
  }
#else
  printf("Platform does not support sync-E!\r\n");
#endif
  fflush(stdout);
}






/* 
#include "../../switching/link_aggregation/core/include/dot3ad_db.h"
extern dot3ad_agg_t dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];

extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1];
extern dot3ad_system_t dot3adSystem;

typedef struct {
    dot3ad_agg_t dot3adAgg[L7_MAX_NUM_LAG_INTF];
    dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
    dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];
    
    L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
    L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1];
    dot3ad_system_t dot3adSystem;
} dot3ad_matrix_sync_t;


int tx_dot3ad_matrix_sync_t(void) {
static dot3ad_matrix_sync_t stat;
//char answer[10];
uint32 ip, len, i;


    memcpy(stat.dot3adAgg, dot3adAgg, sizeof(dot3adAgg));
    memcpy(stat.dot3adPort, dot3adPort, sizeof(dot3adPort));
    memcpy(stat.dot3adOperPort, dot3adOperPort, sizeof(dot3adOperPort));
    
    memcpy(stat.dot3adAggIdx, dot3adAggIdx, sizeof(dot3adAggIdx));
    memcpy(stat.dot3adPortIdx, dot3adPortIdx, sizeof(dot3adPortIdx));
    memcpy(&stat.dot3adSystem, &dot3adSystem, sizeof(dot3adSystem));
    PT_LOG_INFO(LOG_CTX_CONTROL, "sizeof(dot3adAgg)=%u\tsizeof(dot3adPort)=%u\tsizeof(dot3adOperPort)=%u\tsizeof(dot3adAggIdx)=%u\tsizeof(dot3adPortIdx)=%u\tsizeof(dot3adSystem)=%u",
             sizeof(dot3adAgg), sizeof(dot3adPort), sizeof(dot3adOperPort), sizeof(dot3adAggIdx), sizeof(dot3adPortIdx), sizeof(dot3adSystem));

    ip=     ((ptin_board_slotId <= 1) ? IPC_MX_IPADDR_PROTECTION : IPC_MX_IPADDR_WORKING);

    i=      0;
    len=    sizeof(dot3ad_matrix_sync_t);
    if (len>IPCLIB_MAX_MSGSIZE) len=IPCLIB_MAX_MSGSIZE;

    do {
        if (send_ipc_message(IPC_HW_FASTPATH_PORT,
                             ip,
                             CCMSG_ETH_LACP_MATRIXES_SYNC,//CCMSG_HW_INTF_INFO_GET,
                             &((char *) &stat)[i],
                             NULL,//answer,
                             len) < 0) {
            PT_LOG_INFO(LOG_CTX_CONTROL, "Failed syncing matrixes .3ad wise");
            //return 1;
        }

        if (0==i)   {i+=len;      len--;}   //1st message has a different size to align @Rx
        else        i+=len;

        if (i+len>sizeof(dot3ad_matrix_sync_t)) len=sizeof(dot3ad_matrix_sync_t)-i;
    } while (i<sizeof(dot3ad_matrix_sync_t));

    return 0;
}//tx_dot3ad_matrix_sync_t















void rx_dot3ad_matrix_sync_t(char *p, unsigned long dim) {
static dot3ad_matrix_sync_t stat;
static uint32 i=-1UL;
uint32 len;

    len=    sizeof(dot3ad_matrix_sync_t);
    if (len>IPCLIB_MAX_MSGSIZE) len=IPCLIB_MAX_MSGSIZE; //same as in tx_...

    if (dim==len) {
         i=0;      //1st message has this size to align @Rx
         PT_LOG_INFO(LOG_CTX_CONTROL, "rx_dot3ad_matrix_sync_t()\ti=0");
    }
    else PT_LOG_INFO(LOG_CTX_CONTROL, "rx_dot3ad_matrix_sync_t()");

    if (i>sizeof(dot3ad_matrix_sync_t)) return;

    if (dim+i>sizeof(dot3ad_matrix_sync_t)) len=sizeof(dot3ad_matrix_sync_t)-i; //should never happen
    else                                    len=dim;

    memcpy(&((char *) &stat)[i], p, len);

    i+=dim;//len;
    //if (i>sizeof(dot3ad_matrix_sync_t)) return;
    if (i<sizeof(dot3ad_matrix_sync_t)) return;


//    //At this point, copy "stat" to relevant dot3ad variables
//    for (i=0; i<L7_MAX_NUM_LAG_INTF; i++) {
//        memcpy(&dot3adAgg[i].aggMacAddr, &stat.dot3adAgg[i].aggMacAddr,
//               sizeof(dot3ad_agg_t)-sizeof(dot3adAgg[0].next)-sizeof(dot3adAgg[0].prev));
//    }
//    for (i=0; i<L7_MAX_PORT_COUNT + 1; i++) {
//        memcpy(&dot3adPort[i].actorPortNum, &stat.dot3adPort[i].actorPortNum,
//               dot3adPort[0].muxState-dot3adPort[0].actorPortNum+sizeof(dot3adPort[0].muxState));
//    }
//    memcpy(dot3adOperPort, stat.dot3adOperPort, sizeof(dot3adOperPort));
//    memcpy(dot3adAggIdx, stat.dot3adAggIdx, sizeof(dot3adAggIdx));
//    memcpy(&dot3adSystem, &stat.dot3adSystem, sizeof(dot3adSystem));
//
    PT_LOG_INFO(LOG_CTX_CONTROL, "rx_dot3ad_matrix_sync_t()\tEND");
}//rx_dot3ad_matrix_sync_t




int dot3ad_DB_size(void) {
int r, r2;
 r2=sizeof(dot3adAgg);      r=r2;
 printf("sizeof(dot3adAgg)=%d\n\r", r2);
 r2=sizeof(dot3adPort);     r+=r2;
 printf("sizeof(dot3adPort)=%d\n\r", r2);
 r2=sizeof(dot3adOperPort); r+=r2;
 printf("sizeof(dot3adOperPort)=%d\n\r", r2);

 r2=sizeof(dot3adAggIdx); r+=r2;
 printf("sizeof(dot3adAggIdx)=%d\n\r", r2);
 r2=sizeof(dot3adPortIdx); r+=r2;
 printf("sizeof(dot3adPortIdx)=%d\n\r", r2);
 r2=sizeof(dot3adSystem); r+=r2;
 printf("sizeof(dot3adSystem)=%d\n\r", r2);
 return r;
}
*/





















//Goal: active matrix sends its received LACPDUs to standby matrix so as to synchronize it, reducing LACP unavailability when toggling matrixes
#include "../../switching/link_aggregation/core/include/dot3ad_db.h"
#include "../../switching/link_aggregation/core/include/dot3ad_lac.h"
#include "../../switching/link_aggregation/core/include/dot3ad_lacp.h"
#include "../../switching/link_aggregation/core/include/dot3ad_cfg.h"

extern dot3ad_system_t dot3adSystem;
//extern dot3adCfg_t dot3adCfg;

typedef struct {
    L7_uint32           intf;
    L7_enetMacAddr_t    actorSys;
    dot3ad_pdu_t        pdu;
} dot3ad_matrix_sync2_t;


#if (PTIN_BOARD_IS_MATRIX)
static time_t tx_sync_LACPDU[L7_MAX_INTERFACE_COUNT];
static time_t rx_sync_LACPDU[L7_MAX_INTERFACE_COUNT];
#endif



int tx_dot3ad_matrix_sync2_t(L7_uint32 intf, dot3ad_pdu_t *pdu) {
#if (PTIN_BOARD_IS_MATRIX)
static dot3ad_matrix_sync2_t stat;
//char answer[10];
uint32 ip, len, i;

#ifdef MAP_CPLD
    if (!ptin_fpga_mx_is_matrixactive_rt()) return 0;  //It's the active matrix that sends its received LACPDUs to the other; not the other way around
#endif

    {       //rate limit synchronizing LACPDUs between the 2 CXOs
     time_t t;

     t=time(NULL);
     if (tx_sync_LACPDU[intf]==t) return 0;
     tx_sync_LACPDU[intf]=t;
    }

    stat.intf=  intf;
    memcpy(stat.actorSys.addr, dot3adSystem.actorSys.addr, sizeof(dot3adSystem.actorSys));
    memcpy(&stat.pdu, pdu, sizeof(stat.pdu));

    ip=     (ptin_fpga_board_slot_get() <= PTIN_SYS_MX1_SLOT) ? IPC_MX_IPADDR_PROTECTION : IPC_MX_IPADDR_WORKING;

    i=      0;
    len=    sizeof(dot3ad_matrix_sync2_t);
    if (len>IPCLIB_MAX_MSGSIZE) len=IPCLIB_MAX_MSGSIZE;

    do {
        if (send_ipc_message(IPC_HW_FASTPATH_PORT,
                             ip,
                             CCMSG_ETH_LACP_MATRIXES_SYNC2,
                             &((char *) &stat)[i],
                             NULL,//answer,
                             len,
                             NULL) < 0) {
            PT_LOG_TRACE(LOG_CTX_CONTROL, "Failed syncing(2) matrixes .3ad wise");
            //return 1;
        }

        if (0==i)   {i+=len;      len--;}   //1st message has a different size to align @Rx
        else        i+=len;

        if (i+len>sizeof(dot3ad_matrix_sync2_t)) len=sizeof(dot3ad_matrix_sync2_t)-i;
    } while (0); //(i<sizeof(dot3ad_matrix_sync2_t));
#endif
    return 0;
}//tx_dot3ad_matrix_sync2_t

















void rx_dot3ad_matrix_sync2_t(char *pbuf, unsigned long dim) {
#if (PTIN_BOARD_IS_MATRIX)
    dot3ad_port_t *p;
    dot3ad_agg_t *agg;
    dot3ad_matrix_sync2_t *p2;

#ifdef MAP_CPLD
    if (ptin_fpga_mx_is_matrixactive_rt()) return;  //It's the active matrix that sends its received LACPDUs to the other; not the other way around
#endif

    p2= (dot3ad_matrix_sync2_t *) pbuf;

    {       //rate limit synchronizing LACPDUs between the 2 CXOs
     time_t t;

     t=time(NULL);
     if (rx_sync_LACPDU[p2->intf]==t || t==tx_sync_LACPDU[p2->intf]) return;
     rx_sync_LACPDU[p2->intf]=t;
    }

    p = dot3adPortIntfFind(p2->intf);
    if (L7_NULLPTR == p) return;


    /* Drop the received PDU if the port's aggregator is static */
    /* Get the aggregator first */
    agg = dot3adAggKeyFind(p->actorOperPortKey);
    if (L7_NULLPTR == agg) return;

    if (L7_TRUE == agg->isStatic) return;

    memcpy(dot3adSystem.actorSys.addr, p2->actorSys.addr, sizeof(dot3adSystem.actorSys));   //Must use the same actorSys
    //memcpy(dot3adCfg.cfg.dot3adSystem.actorSys.addr, p2->actorSys.addr, sizeof(dot3adSystem.actorSys));   //Must use the same actorSys
    //p->actorOperPortKey=p2->pdu.actorKey;
    dot3adLacpClassifier(lacpPduRx, p, (void *)&p2->pdu);

    PT_LOG_TRACE(LOG_CTX_CONTROL, "rx_dot3ad_matrix_sync2_t()\tEND");
#endif
}//rx_dot3ad_matrix_sync2_t















L7_int get_linkStatus(L7_uint32 port) {return port<PTIN_SYSTEM_N_INTERF? linkStatus[port]: L7_TRUE;}

