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
#include "ptin_cnfgr.h"
#include "ptin_intf.h"
#include "ptin_xlate_api.h"
#include "ptin_igmp.h"
#include "ptin_dhcp.h"
#include "ptin_pppoe.h"
#include "ptin_cfg.h"
#include "dtl_ptin.h"
#include "ipc.h"
#include "nimapi.h"
#include "usmdb_dot3ad_api.h"
#include "ptin_fieldproc.h"

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
  if (port >= PTIN_SYSTEM_N_PONS || port >= 64)
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
      #if ( !PTIN_BOARD_IS_LINECARD )
        #if ( PTIN_BOARD_IS_STANDALONE )
        /* For OLT7-8CH There is only alarms for non PON interfaces */
        if (port>=PTIN_SYSTEM_N_PONS)
        #else
        /* For CXP360G There is only alarms for external LAGs */
        if (interface_is_valid &&
            ptin_intf.intf_type==PTIN_EVC_INTF_LOGICAL && ptin_intf.intf_id<PTIN_SYSTEM_N_LAGS_EXTERNAL)
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
        #if ( !PTIN_BOARD_IS_LINECARD )
          #if ( PTIN_BOARD_IS_STANDALONE )
          #else
          /* For CXP360G boards, only send alarms for external LAGs */
          if (lagIdList[port]<PTIN_SYSTEM_N_LAGS_EXTERNAL)
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
      if (port<PTIN_SYSTEM_N_PONS && interface_is_valid)
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


/**
 * Matrix Commutation process 
 *  
 */
static void monitor_matrix_commutation(void)
{
#if ( PTIN_BOARD==PTIN_BOARD_TOLT8G )

  L7_uint             port, port_border;
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
}

