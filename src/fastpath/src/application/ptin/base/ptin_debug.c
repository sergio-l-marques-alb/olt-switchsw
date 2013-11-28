/*
 * ptin_debug.c
 * 
 * Implements PTin debug module
 *
 * Created on: 2012/02/17 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

//#include "ptin_debug.h"
#include "ptin_include.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_msghandler.h"
#include "dtl_ptin.h"
#include <unistd.h>
#include <usmdb_nim_api.h>
#include <string.h>

#include <dapi_db.h>
extern L7_uint32 hpcLocalCardIdGet(int slotNum);


/*************************** 
 * EXTERNAL FUNCTIONS
 ***************************/
 
#define TIMERS_MAX  40

/* Used for processing timing measure */
typedef struct
{
  L7_uint64 time_start;
  L7_uint64 time_end;
  char timer_string[101];
} ptin_debug_timer_t;

ptin_debug_timer_t processing_timer[TIMERS_MAX];


void ptin_timer_start(L7_uint16 timer_id, char *str)
{
  L7_uint64 time_now;

  if (timer_id>=TIMERS_MAX)
    return;

  time_now = osapiTimeMicrosecondsGet();

  processing_timer[timer_id].time_start = time_now;
  processing_timer[timer_id].time_end   = time_now;

  if (str!=L7_NULLPTR)
    strncpy(processing_timer[timer_id].timer_string,str,100);
}

void ptin_timer_stop(L7_uint16 timer_id)
{
  L7_uint64 time_now;

  if (timer_id>=TIMERS_MAX)
    return;

  time_now = osapiTimeMicrosecondsGet();

  processing_timer[timer_id].time_end   = time_now;
}

L7_uint32 ptin_timer_get(L7_uint16 timer_id, char **str)
{
  if (timer_id>=TIMERS_MAX)
    return 0;

  if (str!=L7_NULLPTR)
    *str = processing_timer[timer_id].timer_string;
    
  return ( (L7_uint32) (processing_timer[timer_id].time_end - processing_timer[timer_id].time_start) );
}

/*************************** 
 * DEVSHELL FUNCTIONS
 ***************************/ 

void ptin_debug(void)
{
  printf("PTin debug functions\r\n");
  printf("\r\n");
  printf("  ptin_intf_dump            - prints info about all PHY interfaces\r\n");
  printf("  ptin_lag_dump             - prints info about all LAG interfaces\r\n");
  printf("  ptin_evc_dump <id>        - prints info about EVCs (use -1 to display all)\r\n");
  printf("  ptin_evc_map              - prints EVCs extended indexes mapping\r\n");
  printf("  ptin_evc_which <vlan_int> - prints info about the EVC related to the given internal vlan\r\n");
  printf("  ptin_flow_dump <evc_id> <port_type> <port_id>             - Dumps all configured flows");
  printf("\r\n");
  printf("  ptin_evc_clean_all <id>                                   - Clean all profiles and counters of an EVC\r\n");
  printf("  ptin_evc_intf_clean_all <id> <intf_type> <intf_id>        - Clean all profiles and counters of one interface of an EVC\r\n");
  printf("  ptin_evc_allintfs_clean <id>                              - Clean all service profiles and counters of an EVC\r\n");
  printf("  ptin_evc_intf_clean <id> <intf_type> <intf_id>            - Clean service profiles and counters of one interface of an EVC\r\n");
  printf("  ptin_evc_allclients_clean <id>                            - Clean all client profiles and counters of an EVC\r\n");
  printf("  ptin_evc_intfclients_clean <id> <intf_type> <intf_id>     - Clean all client profiles and counters of one interface of an EVC\r\n");
  printf("  ptin_evc_client_clean <id> <intf_type> <intf_id> <cvlan>  - Clean one client profiles and counters of an EVC\r\n");
  printf("  ptin_evc_clean_counters_enable <1/0>                      - Enable or disable counters remotion during cleanup (default is 1)\r\n");
  printf("  ptin_evc_clean_profiles_enable <1/0>                      - Enable or disable profiles remotion during cleanup (default is 0)\r\n");
  printf("\r\n");
  printf("  ptin_evc_destroy <id>                                     - Destroy an EVC with all its clients, profiles and counters\r\n");
  printf("  ptin_evc_allclients_remove <id>                           - Remove all clients of an EVC\r\n");
  printf("  ptin_evc_intfclients_remove <id> <intf_type> <intf_id>    - Remove all clients of one interface of an EVC\r\n");
  printf("  ptin_evc_client_remove <id> <intf_type> <intf_id> <cvlan> - Remove a client of an EVC\r\n");
  printf("\r\n");
  printf("  ptin_debug_trap_packets <port> <OVid> <IVid> <only_drops> - Redirect packets from <port>, with <ovid> and <ivid>, to CPU\r\n");
  printf("  ptin_debug_trap_packets_cancel                            - Cancel redirection packets rule to CPU\r\n");
  printf("  ptin_debug_trap_packets_state                             - Show if some redirection packet rule to CPU is defined\r\n");
  printf("  ptin_debug_trap_packets_dump <enable>                     - Print packets content\r\n");
  printf("\r\n");
  printf("  cpu_intercept_debug_enable <enable>                       - Print all packets intercepted by the processor\r\n");
  printf("  pdu_receive_debug_enable <enable>                         - Print all PDUs received and validated for further processing\r\n");
  printf("  pdu_process_debug_enable <enable>                         - Print all PDUs ready for final processing\r\n");
  printf("\r\n");
  printf("  ptin_bwpolicer_dump_debug           - Dump configured bandwidth policers\r\n");
  printf("  ptin_fpcounters_dump_debug          - Dump configured EVC/client counters (at request)\r\n");
  printf("  hapiBroadDebugPolicyEnable <level>  - Enable policy management debug messages (minimum level should be 3)\r\n");
  printf("\r\n");
  printf("  ssm_debug_dump                      - prints info about SSM operation\r\n");
  printf("  ptin_dhcp_dump                      - prints info about DHCP instances\r\n");
  printf("  ptin_igmp_dump                      - prints info about IGMP instances\r\n");
  printf("  ptin_igmp_clients_dump              - prints info about IMGP clients\r\n");
  printf("  ptin_igmp_assoc_dump                - prints info about MC service associations\r\n");
  printf("  ptin_igmp_mfdb_dump                 - prints MFDB table entries\r\n");
  printf("  ptin_igmp_snoop_dump <id>           - prints IGMP snooping entries (use -1 to display all)\r\n");
  printf("  ptin_igmp_proxy_dump                - prints IGMP proxy configuration\r\n");
  printf("  ptin_igmp_querier_dump <evc_idx>    - prints IGMP querier configuration (none/0/-1 for all)\r\n");
  printf("  ptin_igmptimer_show                 - Show IGMP packet processing time data\r\n");
  printf("  ptin_igmptimer_clear                - Initializes IGMP packet processing time data\r\n");
  printf("  ptin_timers_clear                   - Inializes all processing timers\r\n");
  printf("  ptin_timers_show                    - Show all processing timers contents\r\n");
  printf("  ptin_msg_runtime_init <msg_id>      - Initialize runtime measurements for msg_id (-1 for all)\r\n");
  printf("  ptin_msg_runtime_show               - Show runtime measurements for all messages\r\n");
  printf("\r\n");
  printf("  snoopDebugTraceEnable               - Enable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceDisable              - Disable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceFlagsSetAF <1/2>     - Activate specific IGMP snoop trace messages\r\n");
  printf("  dsTraceFlagsSet <flags_mask>        - Enable detailed debug for DHCP snooping (flags_mask=0xff)\r\n");
  printf("  ptin_debug_igmp_enable <enable>     - Show more debugging logs for the IGMP module\r\n");
  printf("  ptin_debug_dhcp_enable <enable>     - Show more debugging logs for the DHCP module\r\n");
  printf("  ptin_debug_xlate_enable <enable>    - Show more debugging logs for the XLATE module\r\n");
  printf("\r\n");
  printf("  ptin_erps_dump <id>                         - prints info about ERPS (use -1 to display all)\r\n");
  printf("  ptin_intf_boardtype_dump                    - prints board ids information\r\n");
  printf("  ptin_linkscan_control_global <enable>       - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("  ptin_intf_linkscan_control <port> <enable>  - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("\r\n");
  printf("  log_help                            - displays logger help\r\n");
  printf("  log_sev_set   <ctx_mask> <severity> - use log_help for more info\r\n");
  printf("  log_color_set <ctx_mask> <color>    - use log_help for more info\r\n");
  printf("\r\n");
}

void ptin_help(void)
{
  ptin_debug();
}

/* Measure of packet processing time */
ptin_debug_pktTimer_t debug_pktTimer;

void ptin_igmptimer_clear(void)
{
  L7_uint32 time_now = osapiTimeMillisecondsGet();

  debug_pktTimer.first_pkt  = L7_FALSE;
  debug_pktTimer.time_start = time_now;
  debug_pktTimer.time_end   = time_now;
  debug_pktTimer.pktOk_counter = 0;
  debug_pktTimer.pktEr_counter = 0;

  debug_pktTimer.pkt_cpu_counter=0;
  debug_pktTimer.pkt_cpu2_counter=0;
  debug_pktTimer.pkt_dtl_counter=0;
  debug_pktTimer.pkt_dtl2_counter=0;
  debug_pktTimer.pkt_intercept_counter=0;
}

void ptin_igmptimer_show(void)
{
  L7_uint32 time_delta, n_packets, n_packets_ok, n_packets_er;

  time_delta = debug_pktTimer.time_end - debug_pktTimer.time_start;

  n_packets_ok  = debug_pktTimer.pktOk_counter;
  n_packets_er  = debug_pktTimer.pktEr_counter;
  n_packets     = n_packets_ok + n_packets_er;

  printf("IGMP Packet Timer calculator\r\n");
  printf("Received first packet? %s\r\n",(debug_pktTimer.first_pkt ? "Yes" : "No"));
  printf("\npkt_cpu_counter=%u,%u\r\n",debug_pktTimer.pkt_cpu_counter,debug_pktTimer.pkt_cpu2_counter);
  printf("\npkt_dtl_counter=%u,%u\r\n",debug_pktTimer.pkt_dtl_counter,debug_pktTimer.pkt_dtl2_counter);
  printf("\npkt_int_counter=%u\r\n",debug_pktTimer.pkt_intercept_counter);
  printf("\nFor all processed packets:\r\n");
  printf(" Number of packets processed    = %u\r\n",n_packets);
  printf(" Time taken for processing (ms) = %u\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %u\r\n",time_delta/n_packets);
  printf(" Mean number of packets processed in 1 second = %u\r\n",(n_packets*1000)/time_delta);
  printf("\nFor successfully processed packets:\r\n");
  printf(" Number of packets processed    = %u\r\n",n_packets_ok);
  printf(" Time taken for processing (ms) = %u\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %u\r\n",time_delta/n_packets_ok);
  printf(" Mean number of packets processed in 1 second = %u\r\n",(n_packets_ok*1000)/time_delta);
  printf("\nFor non successfully processed packets:\r\n");
  printf(" Number of packets processed    = %u\r\n",n_packets_er);
  printf(" Time taken for processing (ms) = %u\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %u\r\n",time_delta/n_packets_er);
  printf(" Mean number of packets processed in 1 second = %u\r\n",(n_packets_er*1000)/time_delta);
}

void ptin_timers_clear(void)
{
  memset(processing_timer,0x00,sizeof(processing_timer));

  printf("Timers initialized!\r\n");

  #if 0
  ptin_timer_start(0);
  ptin_timer_stop(0);

  ptin_timer_start(1);
  printf("Testing...\r\n");
  ptin_timer_stop(1);

  ptin_timer_start(2);
  usleep(10);
  ptin_timer_stop(2);

  ptin_timer_start(3);
  usleep(100);
  ptin_timer_stop(3);

  ptin_timer_start(4);
  usleep(1000);
  ptin_timer_stop(4);

  ptin_timer_start(5);
  usleep(10000);
  ptin_timer_stop(5);

  ptin_timer_start(6);
  usleep(100000);
  ptin_timer_stop(6);

  ptin_timer_start(7);
  usleep(1000000);
  ptin_timer_stop(7);

  ptin_timer_start(8);
  sleep(1);
  ptin_timer_stop(8);

  ptin_timers_show();
  #endif

  //printf("sysconf(_SC_CLK_TCK)=%lu\r\n",sysconf(_SC_CLK_TCK));
}

void ptin_timers_show(void)
{
  L7_uint i;
  L7_uint32 delta;
  char *str;

  printf("Processing Time:\r\n");

  for (i=0; i<TIMERS_MAX; i++)
  {
    delta=ptin_timer_get(i,&str);
    printf(" Timer#%2u: %6u us : %s\r\n",i,delta,str);
  }
}

/**
 * Initialize runtime measurements for msg_id
 * 
 * @author mruas (7/5/2012)
 * 
 * @param msg_id : message id (-1 for all)
 */
void ptin_msg_runtime_init(L7_uint msg_id)
{
  CHMessage_runtime_meter_init(msg_id);
}

/**
 * Show all measurements data
 */
void ptin_msg_runtime_show(void)
{
  CHMessage_runtime_meter_print();
}

/**
 * Prints a table with several info about all interfaces state
 * 
 * @author alex (2/17/2012)
 */
void ptin_intf_dump(void)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;

  L7_uint   port;
  L7_uint16 slot, sport;
  L7_uint32 intIfNum = -1;
  L7_uint32 speed_mode;
  L7_char8  speed[8];
  L7_char8  bcm_port_str[8];
  L7_char8  board_id_str[8];
  L7_int32  bcm_port;
  L7_uint   admin;
  L7_uint   link;
  ptin_HWPortExt_t  portExt;
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr = dapiCardPtr->slotMap;
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------------------------------+-----------------------------------+\r\n");
  printf("| Board | Slot | Port | IfN | bcm_port | MEF Ext.* | Ena | Link | Speed |                 RX                |                 TX                |\r\n");
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------------------------------+-----------------------------------+\r\n");
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    /* Get intIfNum ID */
    ptin_intf_port2intIfNum(port, &intIfNum);

    /* Admin state */
    if (usmDbIfAdminStateGet(1, intIfNum, &admin) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get admin state of port# %u", port);
      continue;
    }

    /* Speed */
    if (usmDbIfSpeedGet(1, intIfNum, &speed_mode) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get speed of port# %u", port);
      continue;
    }
    switch (speed_mode)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        strcpy(speed, "100M");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        strcpy(speed, "1G");
        break;

      /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        strcpy(speed, "2.5G");
        break;

      /* PTin added: Speed 10G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        strcpy(speed, "10G");
        break;

      /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
        strcpy(speed, "40G");
        break;

      /* PTin added: Speed 100G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
        strcpy(speed, "100G");
        break;

      default:
        strcpy(speed, "???");
        break;
    }

    /* Link State */
    if (nimGetIntfLinkState(intIfNum, &link) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get link state of port# %d", port);
      continue;
    }

    /* Counters */
    portStats.Port = port;
    ptin_intf_counters_read(&portStats);

    /* Get slot and port id */
    slot = sport = 0;
#if (PTIN_BOARD_IS_MATRIX)
    if (ptin_intf_port2SlotPort(port, &slot, &sport, L7_NULLPTR)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get slot and port ids for port# %d", port);
      continue;
    }
#endif

    /* Apply configuration */
    if (dtlPtinL2PortExtGet(intIfNum, &portExt) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error getting MEF Ext of port# %d", port);
      continue;
    }

    /* bcm_port_t */
    bcm_port = hapiSlotMapPtr[port].bcm_port;

#if (PTIN_BOARD_IS_MATRIX)
    L7_uint16 board_type;

    if (ptin_intf_boardtype_get(port, &board_type) == L7_SUCCESS && board_type != 0)
    {
      switch (board_type)
      {
      case PTIN_BOARD_TYPE_TU40G:
        sprintf(board_id_str,"TU40G");
        break;
      case PTIN_BOARD_TYPE_TU40GR:
        sprintf(board_id_str,"TU40GR");
        break;
      case PTIN_BOARD_TYPE_TOLTU20G:
        sprintf(board_id_str,"TU20G");
        break;
      case PTIN_BOARD_TYPE_TR32R:
        sprintf(board_id_str,"TR32R");
        break;
      case PTIN_BOARD_TYPE_TG16G:
        sprintf(board_id_str,"TG16G");
        break;
      case PTIN_BOARD_TYPE_TOLT8G:
        sprintf(board_id_str,"TG8G");
        break;
      case PTIN_BOARD_TYPE_TA48GE:
        sprintf(board_id_str,"TA48G");
        break;
      default:
        sprintf(board_id_str," 0x%02x", board_type);
        break;
      }
    }
    else
#endif
    {
      sprintf(board_id_str," ---");
    }

    /* Switch port: ge/xe (indexes changed according to the board) */
#if (PTIN_BOARD_IS_MATRIX)
    sprintf(bcm_port_str, "xe%u", bcm_port - 1);
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
    sprintf(bcm_port_str, "%2.2s%u",
            (speed_mode==L7_PORTCTRL_PORTSPEED_FULL_10GSX || speed_mode==L7_PORTCTRL_PORTSPEED_FULL_40G_KR4) ? "xe" : "ge",
            (1<<port) & PTIN_SYSTEM_10G_PORTS_MASK ? bcm_port - 54 : bcm_port - 1);
#else
    sprintf(bcm_port_str, "%2.2s%u",
            speed_mode == L7_PORTCTRL_PORTSPEED_FULL_10GSX ? "xe" : "ge",
            (1<<port) & PTIN_SYSTEM_10G_PORTS_MASK ? bcm_port - 26 : bcm_port - 30);
#endif

    printf("| %-6.6s| %2u/%u |  %2u  |  %2u | %2u (%-4.4s)| %-3.3s-%u/%u/%u | %-3.3s | %4.4s | %5.5s | %15llu B %11llu bps | %15llu B %11llu bps |\r\n",
           board_id_str, slot, sport,
           port,
           intIfNum,
           bcm_port, bcm_port_str,
           (portExt.egress_type == PTIN_PORT_EGRESS_TYPE_ISOLATED) ? "ISO" : ((portExt.egress_type == PTIN_PORT_EGRESS_TYPE_COMMUNITY) ? "COM" : "PRO"),
           portExt.macLearn_stationMove_enable, portExt.macLearn_stationMove_samePrio, portExt.macLearn_stationMove_prio,
           admin ? "Ena" : "Dis",
           link == L7_UP ? " Up " : "Down",
           speed,
           portStats.Rx.etherStatsOctets,
           portStats.Rx.Throughput,
           portStats.Tx.etherStatsOctets,
           portStats.Tx.Throughput);
  }
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------------------------------+-----------------------------------+\r\n");
  printf("MEF Ext: MEF Extension attributes -> Port Type - MAC move enable / MAC move with same prio enable / MAC move prio\r\n");
  return;
}


/**
 * Prints a table with info about LAGs
 * 
 * @author alex (2/17/2012)
 */
void ptin_lag_dump(void)
{
  ptin_LACPLagConfig_t  lagConf;
  ptin_LACPLagStatus_t  lagStatus;
  ptin_LACPAdminState_t lagState;
  ptin_LACPStats_t      lagStats;
  L7_uint               i;
  L7_uint32             intIfNum;

  const char *lbal[9] = {
    "Invalid!!!",
    "SA_VLAN",
    "DA_VLAN",
    "SDA_VLAN",
    "SIP_SPORT",
    "DIP_DPORT",
    "SDIP_DPORT",
    "ENHANCED",
    "Invalid!!!",
  };

  printf("+----+------+----------+--------+------------+--------------+--------------+------+------------+------------+\r\n");
  printf("| Id | Port | IntIfNum |  Type  | Load Balan |   Members    | Active Membr | Link | LACPdu RX  | LACPdu TX  |\r\n");
  printf("+----+------+----------+--------+------------+--------------+--------------+------+------------+------------+\r\n");
  for (i=0; i<PTIN_SYSTEM_N_LAGS; i++)
  {
    /* Config */
    if (!ptin_intf_lag_exists(i))
    {
      printf("| %2u |  %2u  |    -     |   -    |      -     |      -       |      -       |  -   |     -      |     -      |\r\n",
             i, PTIN_SYSTEM_N_PORTS + i);
      continue;
    }

    /* Get intIfNum ID */
    intIfNum = 255;
    ptin_intf_lag2intIfNum(i, &intIfNum);

    lagConf.lagId = i;
    ptin_intf_LagConfig_get(&lagConf);

    /* Status */
    lagStatus.lagId = i;
    ptin_intf_LagStatus_get(&lagStatus);

    /* LACP State */
    lagState.port = i;
    ptin_intf_LACPAdminState_get(&lagState);

    /* LACP Stats */
    lagStats.port = i;
    ptin_intf_LACPStats_get(&lagStats);

    printf("| %2u |  %2u  |    %2u    | %-6.6s | %-10.10s | 0x%010llX | 0x%010llX | %4.4s | %10u | %10u |\r\n",
           i,
           PTIN_SYSTEM_N_PORTS + i,
           intIfNum,
           lagConf.static_enable ? "Static" : "Dynam.",
           lbal[lagConf.loadBalance_mode],
           lagStatus.members_pbmp64,
           lagStatus.active_members_pbmp64,
           lagStatus.link_status ? " Up " : "Down",
           lagStats.LACPdus_rx,
           lagStats.LACPdus_tx);
  }
  printf("+----+------+----------+--------+------------+--------------+--------------+------+------------+------------+\r\n");

  return;
}

