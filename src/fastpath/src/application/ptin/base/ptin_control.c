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
#include "ptin_cfg.h"
#include "dtl_ptin.h"
#include "ipc.h"
#include "nimapi.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "ptin_fieldproc.h"
#include "ptin_msghandler.h"

#if ( PTIN_BOARD_IS_STANDALONE )
#include "fw_shm.h"
#endif

/* PTin module state */
volatile ptin_state_t ptin_state = PTIN_ISLOADING;

static L7_uint32 ptin_loop_handle = 0;  /* periodic timer handle */

static L7_int    linkStatus[PTIN_SYSTEM_N_INTERF];        /* Link status of each interface */
static L7_BOOL   lagActiveMembers[PTIN_SYSTEM_N_PORTS];   /* Port is an active Lag member? */
static L7_uint32 lagIdList[PTIN_SYSTEM_N_PORTS];          /* LAG id that belongs the port */

/* Local prototypes */
static void startup_trap_send(void);
static void monitor_throughput(void);
static void monitor_alarms(void);
static void monitor_matrix_commutation(void);

#if (PTIN_BOARD_IS_MATRIX)
#ifdef PTIN_LINKSCAN_CONTROL
void ptin_control_switchover_monitor(void);
#endif
#endif

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
  LOG_INFO(LOG_CTX_PTIN_CONTROL, "PTin task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_ready_sem, L7_WAIT_FOREVER);
  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "PTin task will now start!");

  /* Initialize PTin Interface module data structures
   * Note: ptin_intf_data_init() needs to be invoked ONLY after nim
   * initialization, which can be guaranteed at this stage */
  if (ptin_intf_init() != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error initializing PTin Interface module! CRASH!");
    PTIN_CRASH();
  }

  /* Initialize xlate module in application layer */
  if (ptin_xlate_init()!=L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error initializing PTin XLATE module! CRASH!");
    PTIN_CRASH();
  }

  /* Apply configuration on DTL and lower layers */
  if (dtlPtinInit() != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error initializing PTin DTL module! CRASH!");
    PTIN_CRASH();
  }

  /* Load IGMP default config */
  rc = ptin_igmp_proxy_defaultcfg_load();
  if (rc != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error load IGMP default config! CRASH!");
    PTIN_CRASH();
  }

  /* By default enable global DHCP trapping */
  if (ptin_dhcp_enable(L7_ENABLE) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error enabling DHCP global trapping! CRASH!");
    PTIN_CRASH();
  }

  /* PPPoE Global enable */
  if (ptin_pppoe_enable(L7_ENABLE) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error enabling PPPoE global trapping! CRASH!");
    PTIN_CRASH();
  }

  /* Initialize storm control */
  rc = ptin_stormControl_init();
  if (rc != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error initializing storm control CRASH!");
    PTIN_CRASH();
  }
  LOG_INFO(LOG_CTX_PTIN_CNFGR, "Storm Control is active with default values.");

#if ( PTIN_BOARD_IS_MATRIX )
  /* Configure InBand bridge if this board is CXP360G */
  if (ptin_cfg_inband_bridge_set() != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_PTIN_CNFGR, "Error creating InBand bridge!");
  }
#endif

  /* Register a period timer */
  if (osapiPeriodicUserTimerRegister(PTIN_LOOP_TICK, &ptin_loop_handle) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Error registering period timer! CRASH!");
    PTIN_CRASH();
  }

  /* Signal correct initialization */
  ptin_state = PTIN_LOADED;

  /* Send startup trap */
  startup_trap_send();

  /* Loop */
  while (1) {
//    LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "PTin task is Sleeping (%us)...", PTIN_LOOP_TICK/1000);
    osapiPeriodicUserTimerWait(ptin_loop_handle);

    /* Monitor throughput */
    monitor_throughput();

    /* Port commutation process for TOLT8G boards */
    monitor_matrix_commutation();

    /* Monitor alarms */
    monitor_alarms();
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
    linkStatus[port] = L7_TRUE;
  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++)
    lagActiveMembers[port] = L7_TRUE;
}


static void startup_trap_send(void)
{
  /* Send a trap because Fastpath is ready to be flushed with configs */
  while (1)
  {
    if (send_trap(IPC_CHMSG_TRAP_PORT, TRAP_ARRANQUE, -1) < 0) {
      LOG_ERR(LOG_CTX_PTIN_CONTROL, "Failed to send Startup Trap");
      sleep(1);
    }
    else {
      LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "Startup Trap successfully sent");
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
    LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error reading counters activity");
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

#if (PTIN_BOARD == PTIN_BOARD_TA48GE)
#define LED_COLOR_OFF     0
#define LED_COLOR_GREEN   1
#define LED_COLOR_RED     2
#define LED_COLOR_YELLOW  3

void ptin_ta48ge_led_control(L7_uint32 port, L7_uint8 color, L7_uint8 blink)
{
  #ifdef MAP_FPGA
  if (port >= PTIN_SYSTEM_N_ETH || port >= 64)
    return;

  if (port%2==0)  /* Pair ports */
  {
    fpga_map->reg.led_color_pairports[port/2] = color;
    fpga_map->reg.led_blink_pairports[port/2] = blink;
  }
  else
  {
    fpga_map->reg.led_color_oddports[(port-1)/2] = color;
    fpga_map->reg.led_blink_oddports[(port-1)/2] = blink;
  }
  #endif
}
#endif

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
  portActivity.ports_mask    = PTIN_SYSTEM_ETH_PORTS_MASK;          /* Only ETH ports */
  portActivity.activity_mask = PTIN_PORTACTIVITY_MASK_RX_ACTIVITY;  /* Get only rx activity */
  if (ptin_intf_counters_activity_get(&portActivity)==L7_SUCCESS)
  {
    portActivity_valid = L7_TRUE;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_CONTROL,"Stat Activity get failed!");
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

      interface_is_valid = L7_FALSE;
    }
    else
    {
      interface_is_valid = L7_TRUE;
    }

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
    if (port<PTIN_SYSTEM_N_PORTS)  pfw_shm->intf[port].link = link;
    #endif

    if (linkStatus[port] != link)
    {
      #if ( PTIN_BOARD_IS_STANDALONE || PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_ACTIVETH )
        #if (PTIN_BOARD_IS_MATRIX)
        /* For CXP360G There is only alarms for external LAGs */
        if (interface_is_valid &&
            ptin_intf.intf_type==PTIN_EVC_INTF_LOGICAL && ptin_intf.intf_id<PTIN_SYSTEM_N_LAGS_EXTERNAL)
        #else
        /* There is only alarms for non PON interfaces */
        if (port >= PTIN_SYSTEM_N_PONS)
        #endif
        {
          if (send_trap_intf_alarm(ptin_intf.intf_type, ptin_intf.intf_id,
                                 ((!link) ? TRAP_ALARM_LINK_DOWN_START : TRAP_ALARM_LINK_DOWN_END),
                                 TRAP_ALARM_STATUS_EVENT,0) == 0)
          LOG_NOTICE(LOG_CTX_PTIN_CONTROL,"Alarm sent: port=%u, link=%u", port, link);
        }
      #endif
      LOG_INFO(LOG_CTX_PTIN_CONTROL,"Link state changed: port=%u, link=%u", port, link);
      linkStatus[port]=link;
    }

    #if ( PTIN_BOARD_IS_STANDALONE )
    if (port<PTIN_SYSTEM_N_PORTS)  pfw_shm->intf[port].counter_state = 0; //counters_state.status_value[port];
    #endif

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
            LOG_NOTICE(LOG_CTX_PTIN_CONTROL,"Alarm sent: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
          }
        #endif
        lagActiveMembers[port]=isActiveMember;
        LOG_INFO(LOG_CTX_PTIN_CONTROL,"Active LAG membership changed: port=%u, activeMember=%u (lag_id=%u)",port,isActiveMember,lagIdList[port]);
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
            //LOG_TRACE(LOG_CTX_PTIN_CONTROL,"Interface %u have link up",port);

            /* Blink led */
            if ((portActivity_valid) && ((portActivity.ports_mask>>port) & 1) &&
                (portActivity.activity_bmap[port] & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY))
            {
              //LOG_TRACE(LOG_CTX_PTIN_CONTROL,"Activity on interface %u",port);

              /* Port throughput values are valid, and we have positive throughput */
              ptin_ta48ge_led_control(port, LED_COLOR_GREEN, 0xAA);
            }
            else
            {
              //LOG_TRACE(LOG_CTX_PTIN_CONTROL,"NO activity on interface %u",port);
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
  ptin_HWEthPhyConf_t phyConf;
  L7_RC_t             rc = L7_SUCCESS;
  static int          cx_work_slot_h = -1;

  memset(&phyConf,0x00,sizeof(ptin_HWEthPhyConf_t));
  phyConf.Mask = PTIN_PHYCONF_MASK_PORTEN;

  cx_work_slot = (L7_int) cpld_map->reg.slot_matrix & 1;

  /* Nothing to do if no change happened */
  if (cx_work_slot == cx_work_slot_h)
  {
    return;
  }

  #if ( PTIN_BOARD == PTIN_BOARD_TOLT8G )
  L7_uint             port, port_border;

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
      LOG_ERR(LOG_CTX_PTIN_CONTROL,"Error setting port %u to enable=%u",port,phyConf.PortEnable);
      rc = L7_FAILURE;
    }
  }
  #elif ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  L7_uint             port;

  /* Run all internal ports to change its admin state */
  for (port = PTIN_SYSTEM_N_ETH; port < PTIN_SYSTEM_N_PORTS; port++)
  {
    /* Set port enable */
    if (cx_work_slot)
      phyConf.PortEnable  = (port==(PTIN_SYSTEM_N_ETH+1)) ? L7_TRUE : L7_FALSE;  /* Only port 1 will be active */
    else
      phyConf.PortEnable  = (port==(PTIN_SYSTEM_N_ETH+0)) ? L7_TRUE : L7_FALSE;  /* Only port 0 will be active */

    phyConf.Port = port;
    if (ptin_intf_PhyConfig_set(&phyConf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_CONTROL,"Error setting port %u to enable=%u",port,phyConf.PortEnable);
      rc = L7_FAILURE;
    }
  }
  #endif

  /* Any error? */
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_CONTROL,"Error commuting to %s slot",(cx_work_slot) ? "working" : "protection");
  }
  else
  {
    /* Save new state */
    cx_work_slot_h = cx_work_slot;
    LOG_NOTICE(LOG_CTX_PTIN_CONTROL,"Success commuting to %s slot",(cx_work_slot) ? "working" : "protection");
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

#if (PTIN_BOARD_IS_MATRIX)
#ifdef PTIN_LINKSCAN_CONTROL

/* List of active interfaces */
static L7_uint8 switchover_intf_active_h[PTIN_SYSTEM_MAX_N_PORTS];

/**
 * Task that checks for Matrix Switchovers
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinSwitchoverTask(L7_uint32 numArgs, void *unit)
{
  L7_RC_t rc;

  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "ptinSwitchover running!");
  rc = osapiTaskInitDone(L7_PTIN_SWITCHOVER_TASK_SYNC);

  /* Wait for a signal indicating that all other modules
   * configurations were executed */
  LOG_INFO(LOG_CTX_PTIN_CONTROL, "ptinSwitchover task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_switchover_sem, L7_WAIT_FOREVER);
  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "ptinSwitchover task will now start!");

  while (1)
  {
    if (linkscan_update_control)
      ptin_control_switchover_monitor();

    osapiSleep(10);
  }
}

/**
 * Monitor switchover process
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
  L7_uint8  slot_id;
  L7_uint16 board_id;

  L7_uint8 interfaces_active[PTIN_SYSTEM_MAX_N_PORTS];
  static L7_uint8 matrix_is_active_h = L7_TRUE;
  L7_uint8 matrix_is_active;

  matrix_is_active = cpld_map->reg.mx_is_active;

  /* First time procedure (after switchover) */
  if (cpld_map->reg.mx_is_active != matrix_is_active_h)
  {
    matrix_is_active_h = matrix_is_active;
    
    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Switchover detected (to active=%d)", matrix_is_active);

    osapiSleep(10);

    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Goig to process switchover init (active=%d)", matrix_is_active);

    osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

    /* For active matrix, disable force link up, and enable linkscan, only for uplink ports */
    if (matrix_is_active)
    {
      /* Run all slots */
      for (slot_id = PTIN_SYS_LC_SLOT_MIN; slot_id <= PTIN_SYS_LC_SLOT_MAX; slot_id++)
      {
        /* Nothing to do for non uplink boards */
        if (ptin_slot_boardtype_get(slot_id, &board_id)!=L7_SUCCESS ||
            !PTIN_BOARD_IS_UPLINK(board_id))
          continue;

        /* Disable force link-up, and enable linkscan for uplink boards */
        ptin_slot_link_force(slot_id, -1, L7_TRUE, L7_DISABLE);
        ptin_slot_linkscan_set(slot_id, -1, L7_ENABLE);
        LOG_INFO(LOG_CTX_PTIN_CONTROL, "Linkscan enabled for slot %u", slot_id);
      }
    }
    /* For passive matrix, reset all ports, and disable linkscan for all of them */
    else
    {
      /* Clear historic values of active interfaces */
      memset(switchover_intf_active_h, 0x00, sizeof(switchover_intf_active_h));

      /* Disable force linkup for all ports */
      for (port=0; port<ptin_sys_number_of_ports; port++)
      {
        /* For passive board, disable force linkup */
        if (ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS)
        {
          ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);       /* Disable force link-up */
          ptin_intf_link_force(intIfNum, L7_FALSE, 0);            /* Cause link down */
        }
      }

      /* Enable linkscan for all ports (links will go down) */
      for (port=0; port<ptin_sys_number_of_ports; port++)
      {
        if (ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS)
          ptin_intf_linkscan_set(intIfNum, L7_ENABLE);
      }

      /* Wait 3 seconds */
      osapiSleep(2);

      /* Disable linkscan for all ports */
      for (port=0; port<ptin_sys_number_of_ports; port++)
      {
        if (ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS)
          ptin_intf_linkscan_set(intIfNum, L7_DISABLE);
      }

      LOG_INFO(LOG_CTX_PTIN_CONTROL, "Linkscan disabled for all ports");
    }

    /* End of procedure */
    osapiSemaGive(ptin_boardaction_sem);
    return;
  }

  /* Do nothing for active matrix */
  if (cpld_map->reg.mx_is_active)
  {
    return;
  }

  /* --- Only for Passive matrix --- */

  /* Query active matrix, abot active ports */
  msg_HwIntfInfo_t ports_info;

  memset(interfaces_active, 0x00, sizeof(interfaces_active));
  memset(&ports_info, 0x00, sizeof(msg_HwIntfInfo_t));

  ports_info.slot_id    = (ptin_board_slotId <= 1) ? 20 : 1;
  ports_info.generic_id = 0;
  ports_info.generic_id = ptin_sys_number_of_ports;

  LOG_INFO(LOG_CTX_PTIN_CONTROL, "quering for active interfaces");

  if (send_ipc_message(IPC_HW_FASTPATH_PORT,
                       ((ptin_board_slotId <= 1) ? IPC_MX_IPADDR_PROTECTION : IPC_MX_IPADDR_WORKING),
                       CCMSG_HW_INTF_INFO_GET,
                       (char *) &ports_info,
                       (char *) &ports_info,
                       sizeof(msg_HwIntfInfo_t)) < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_CONTROL, "Failed to send interfaces query!");
    return;
  }
  #if 0
  LOG_TRACE(LOG_CTX_PTIN_CONTROL, "ptin_board_slotId=%d",  ptin_board_slotId);
  for (port=0; port<ports_info.number_of_ports; port++)
  {
    LOG_TRACE(LOG_CTX_PTIN_CONTROL, "port=%u: boardId=%u admin=%u link=%u", port,
              ports_info.port[port].board_id, ports_info.port[port].enable, ports_info.port[port].link);
  }
  #endif

  /* Do nothing for active matrix */
  if (cpld_map->reg.mx_is_active)
  {
    return;
  }

  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  printf("Other board active ports: { ");
  for (port=0; port<ports_info.number_of_ports; port++)
  {
    if (ports_info.port[port].board_id != 0)
    {
      if (ports_info.port[port].enable &&
          ports_info.port[port].link)
      {
        interfaces_active[port] = 1;
        printf("%2u ", port);
      }
    }
    /* Save board_id */
    ptin_intf_boardtype_get(port, &board_id);
    if (board_id != ports_info.port[port].board_id )
    {
      ptin_intf_boardtype_set(port, ports_info.port[port].board_id);
      LOG_INFO(LOG_CTX_PTIN_CONTROL, "Board id %u set for port %u", ports_info.port[port].board_id, port);
    }
  }
  printf("}\r\n");

  /* Do nothing for active matrix */
  if (cpld_map->reg.mx_is_active)
  {
    osapiSemaGive(ptin_boardaction_sem);
    return;
  }

  LOG_INFO(LOG_CTX_PTIN_CONTROL, "Updating Port status");

  /* Update port state */
  for (port = 0; port < PTIN_SYSTEM_MAX_N_PORTS; port++)
  {
    /* Skip unchanged ports */
    if (interfaces_active[port] == switchover_intf_active_h[port])
      continue;

    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Port %u status changed to %u", port, interfaces_active[port]);

    /* Update new changes */
    switchover_intf_active_h[port] = interfaces_active[port];

    /* --- Passive board --- */

    /* For passive board, update force link states */
    if (ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error getting intIfNum from ptin_port %d!", port);
      continue;
    }
    /* Enable/Disable force linkup */
    if (interfaces_active[port])
    {
      if (ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE) != L7_SUCCESS) 
      {
        LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error setting force link to %u for ptin_port %d!", interfaces_active[port], port);
        continue;
      }
    }
    else
    {
      /* Disable force link-up */
      if (ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error disabling force link-up for ptin_port %d!", interfaces_active[port], port);
        continue;
      }
      /* Cause a linkdown */
      if (ptin_intf_link_force(intIfNum, L7_FALSE, 0) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error causing change link for ptin_port %d!", interfaces_active[port], port);
        continue;
      }
    }
    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Link forced to %u for port%u", interfaces_active[port], port);
  }

  osapiSemaGive(ptin_boardaction_sem);
  #endif
}
#endif
#endif

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

  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "PTinIntf running!");

  rc = osapiTaskInitDone(L7_PTIN_INTF_TASK_SYNC);

  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "PTinIntf task will now start!");

  #if 0
  /* Wait for a signal indicating that all other modules
   * configurations were executed */
  LOG_INFO(LOG_CTX_PTIN_CONTROL, "PTinIntf task waiting for other modules to boot up...");
  rc = osapiSemaTake(ptin_ready_sem, L7_WAIT_FOREVER);
  LOG_NOTICE(LOG_CTX_PTIN_CONTROL, "PTinIntf task will now start!");
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
      LOG_INFO(LOG_CTX_PTIN_CNFGR, "Event processed: rc=%d", rc);
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_CNFGR, "Error receiving queue messages");
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

  //LOG_INFO(LOG_CTX_PTIN_CONTROL, "Event received: event=%u, intIfNum=%u",event, intIfNum);

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    LOG_ERR(LOG_CTX_PTIN_CONTROL,
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
    LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error getting slot for intIfNum %u", intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  /* Only disable linkscan if slot have an inserted board */
  if (board_id == L7_NULL)
  {
    LOG_WARNING(LOG_CTX_PTIN_CONTROL, "Slot %u (intIfNum %u) not present", slot_id, intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  #endif

  eventMsg.event    = event; 
  eventMsg.intIfNum = intIfNum;
  if (osapiMessageSend(ptin_intf_event_queue, &eventMsg, sizeof(ptinIntfEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)

  {
    LOG_ERR(LOG_CTX_PTIN_CONTROL, "Error sending message to queue: event=%u, intIfNum=%u",event, intIfNum);
  }
  else
  {
    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Message sent to queue: event=%u, intIfNum=%u",event, intIfNum);
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
    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Link up detected at interface intIfNum %u", eventMsg->intIfNum);

    /* Disable linkscan */
    rc = ptin_intf_linkscan_set(eventMsg->intIfNum, L7_DISABLE);

    LOG_INFO(LOG_CTX_PTIN_CONTROL, "Linkscan disabled: rc=%d, intIfNum=%u", rc, eventMsg->intIfNum);
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

  LOG_INFO(LOG_CTX_PTIN_CONTROL, "Startup executed");

  memset(&portEvent_mask, 0x00, sizeof(portEvent_mask));

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_UP);
  PORTEVENT_SETMASKBIT(portEvent_mask, L7_DOWN);

  /* Event types to be received */
  nimRegisterIntfEvents(L7_PTIN_COMPONENT_ID, portEvent_mask);

  nimStartupEventDone(L7_PTIN_COMPONENT_ID);
}

