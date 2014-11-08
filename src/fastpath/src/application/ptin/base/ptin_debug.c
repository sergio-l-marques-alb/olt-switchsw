/*
 * ptin_debug.c
 * 
 * Implements PTin debug module
 *
 * Created on: 2012/02/17 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#include "ptin_debug.h"
#include "ptin_include.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_msghandler.h"
#include "dtl_ptin.h"
#include <unistd.h>
#include <usmdb_nim_api.h>
#include <string.h>

#include <dapi_db.h>

typedef struct
{
  uint32 number_of_calls;
  uint32 total_runtime;
  uint32 last_runtime;
  uint32 min_runtime;
  uint32 max_runtime;
  uint32 average_runtime;

  L7_uint64 time_ref;
} struct_runtime_t;

static struct_runtime_t proc_runtime[PTIN_PROC_MAX+1];


extern L7_uint32 hpcLocalCardIdGet(int slotNum);


/*************************** 
 * EXTERNAL FUNCTIONS
 ***************************/
 
#define TIMERS_MAX  80

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
  
  if (str!=L7_NULLPTR)
    strncpy(processing_timer[timer_id].timer_string,str,100);
}

void ptin_timer_stop(L7_uint16 timer_id)
{
  L7_uint64 time_now;

  if (timer_id>=TIMERS_MAX)
    return;

  time_now = osapiTimeMicrosecondsGet();

  processing_timer[timer_id].time_start = (L7_uint32) (time_now - processing_timer[timer_id].time_start);  
  processing_timer[timer_id].time_end   = (L7_uint32) ((processing_timer[timer_id].time_end + processing_timer[timer_id].time_start)/2);
}

L7_uint32 ptin_timer_get(L7_uint16 timer_id, char **str)
{
  if (timer_id>=TIMERS_MAX)
    return 0;

  if (str!=L7_NULLPTR)
    *str = processing_timer[timer_id].timer_string;
    
  return ( (L7_uint32) (processing_timer[timer_id].time_end) );
}

/*************************** 
 * DEVSHELL FUNCTIONS
 ***************************/ 

void ptin_debug(void)
{
  printf("PTin debug functions\r\n");
  printf("\r\n");
  printf("  ptin_intf_dump                                            - prints info about all PHY interfaces\r\n");
  printf("  ptin_lag_dump                                             - prints info about all LAG interfaces\r\n");
  printf("  ptin_evc_dump <id>                                        - prints info about eEVCs (use -1 to display all)\r\n");
  printf("  ptin_evc_map                                              - prints EVCs extended indexes mapping\r\n");
  printf("  ptin_evc_which <vlan_int>                                 - prints info about the EVC related to the given internal vlan\r\n");
  printf("  ptin_maclimit_dump                                        - Dumps MAC limit tables (non empty)\r\n");
  printf("\r\n");
  printf("  ptin_evc_clean_all <id>                                   - Clean all profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intf_clean_all <id> <intf_type> <intf_id>        - Clean all profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_allintfs_clean <id>                              - Clean all service profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intf_clean <id> <intf_type> <intf_id>            - Clean service profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_allclients_clean <id>                            - Clean all client profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intfclients_clean <id> <intf_type> <intf_id>     - Clean all client profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_client_clean <id> <intf_type> <intf_id> <cvlan>  - Clean one client profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_clean_counters_enable <1/0>                      - Enable or disable counters remotion during cleanup (default is 1)\r\n");
  printf("  ptin_evc_clean_profiles_enable <1/0>                      - Enable or disable profiles remotion during cleanup (default is 0)\r\n");
  printf("\r\n");
  printf("  ptin_evc_destroy_all                                      - Destroy all EVCs\r\n");
  printf("  ptin_evc_destroy <eEVC_id>                                - Destroy an EVC with all its clients, profiles and counters\r\n");
  printf("  ptin_evc_allclientsflows_remove <id>                      - Remove all clients of an EVC (internal id)\r\n");
  printf("  ptin_evc_intfclientsflows_remove <id> <intfType> <intfId> - Remove all clients of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_client_remove <id> <intf_type> <intf_id> <cvlan> - Remove a client of an EVC (internal id)\r\n");
  printf("  ptin_xlate_dump <stage> [inv]                             - Dump translation entries for ingress (1) or egress (2) stage\r\n");
  printf("\r\n");
  printf("  ptin_debug_trap_packets <port> <OVid> <IVid> <only_drops> - Redirect packets from <port>, with <ovid> and <ivid>, to CPU\r\n");
  printf("  ptin_debug_trap_packets_egress <port> <OVid> <IVid> <drp> - Redirect packets egressing from <port>, with <ovid> and <ivid>, to CPU\r\n");
  printf("  ptin_debug_trap_packets_mirror <dst> <port> <OVid> <IVid> - Redirect packets egressing from <port>, with <ovid> and <ivid>, to <dst> port\r\n");
  printf("  ptin_debug_trap_packets_cancel                            - Cancel redirection packets rule to CPU\r\n");
  printf("  ptin_debug_trap_packets_state                             - Show if some redirection packet rule to CPU is defined\r\n");
  printf("  ptin_debug_trap_packets_dump <enable>                     - Print packets content\r\n");
  printf("\r\n");
  printf("  cpu_intercept_debug_enable <enable>                       - Print all packets intercepted by the processor\r\n");
  printf("  pdu_receive_debug_enable <enable>                         - Print all PDUs received and validated for further processing\r\n");
  printf("  pdu_process_debug_enable <enable>                         - Print all PDUs ready for final processing\r\n");
  printf("\r\n");
  printf("  ptin_stormcontrol_dump                                    - Dump storm control configurations\r\n");
  printf("  ptin_bwpolicer_dump_debug                                 - Dump configured bandwidth policers\r\n");
  printf("  ptin_bwpolicer_flush_debug                                - Flush configured EVC/client policers (at request)\r\n");
  printf("  ptin_fpcounters_dump_debug                                - Dump configured EVC/client counters (at request)\r\n");
  printf("  ptin_fpcounters_flush_debug                               - Flush configured EVC/client counters (at request)\r\n");
  printf("  ptin_vcap_defvid_dump                                     - Flush configured VCAP rules associated to defVID\r\n");
  printf("  hapiBroadDebugPolicyEnable <level>                        - Enable policy management debug messages (minimum level should be 3)\r\n");
  printf("\r\n");                                                    
  printf("  ssm_debug_dump                                            - prints info about SSM operation\r\n");
  printf("  ssm_debug_write <slot> <intf> <ssm_rx> <ssm_tx> <link>    - modifies shared memory contents about SSM\r\n");
  printf("  ptin_reclk_dump                                           - prints info about sync-E recovery clocks\r\n");
  printf("  ptin_reclk_write <primary_port> <backup_port>             - modifies shared memory contents about sync-E recovery clocks\r\n");
  printf("  ptin_dhcp_dump                                            - prints info about DHCP instances\r\n");
  printf("  ptin_igmp_dump                                            - prints info about IGMP instances\r\n");
  printf("  ptin_igmp_assoc_dump                                      - prints info about MC service associations\r\n");
  printf("  ptin_igmp_mfdb_dump                                       - prints MFDB table entries\r\n");
  printf("  ptin_igmp_snoop_dump <id>                                 - prints IGMP snooping entries (use -1 to display all)\r\n");
  printf("  ptin_igmp_proxy_dump                                      - prints IGMP proxy configuration\r\n");
  printf("  ptin_igmp_querier_dump <evc_idx>                          - prints IGMP querier configuration (none/0/-1 for all)\r\n");
  printf("  ptin_igmp_clients_dump                                    - prints info about IMGP device clients\r\n");
  printf("  ptin_igmp_groupclients_dump                               - prints info about IMGP group of clients\r\n");
  printf("  ptin_igmptimer_show                                       - Show IGMP packet processing time data\r\n");
  printf("  ptin_igmptimer_clear                                      - Initializes IGMP packet processing time data\r\n");
  printf("  ptin_timers_clear                                         - Inializes all processing timers\r\n");
  printf("  ptin_timers_show                                          - Show all processing timers contents\r\n");
  printf("  ptin_msg_runtime_init <msg_id>                            - Initialize runtime measurements for msg_id (-1 for all)\r\n");
  printf("  ptin_msg_runtime_show                                     - Show runtime measurements for all messages\r\n");
  printf("\r\n");                                                    
  printf("  ptin_traprules_dump                                       - Dump configured trap (to CPU) rules\r\n");
  printf("  snoopDebugTraceEnable                                     - Enable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceDisable                                    - Disable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceFlagsSetAF <1/2>                           - Activate specific IGMP snoop trace messages\r\n");
  printf("  dsTraceFlagsSet <flags_mask>                              - Enable detailed debug for DHCP snooping (flags_mask=0xff)\r\n");
  printf("  ptin_debug_igmp_enable <enable>                           - Show more debugging logs for the IGMP module\r\n");
  printf("  ptin_debug_igmp_packet_trace_enable <enable>              - Show IGMP Packet Trace\r\n");  
  printf("  ptin_debug_dhcp_enable <enable>                           - Show more debugging logs for the DHCP module\r\n");
  printf("  ptin_debug_xlate_enable <enable>                          - Show more debugging logs for the XLATE module\r\n");
  printf("  ptin_debug_oam_set <enable>                               - Show more debugging logs for the OAM module\r\n");
  printf("  ptin_debug_lacp_set <enable>                              - Show more debugging logs for the LACP module\r\n");
  printf("  ptin_debug_dtl_set <enable>                               - Show more debugging logs for the DTL module\r\n");
  printf("\r\n");
  printf("  ptin_erps_dump <id>                                       - prints info about ERPS (use -1 to display all)\r\n");
  printf("  ptin_intf_boardtype_dump                                  - prints board ids information\r\n");
  printf("  ptin_linkscan_control_global <enable>                     - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("  ptin_intf_linkscan_control <port> <enable>                - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("\r\n");
  printf("  log_help                                                  - displays logger help\r\n");
  printf("  log_sev_set   <ctx_mask> <severity>                       - use log_help for more info\r\n");
  printf("  log_color_set <ctx_mask> <color>                          - use log_help for more info\r\n");
  printf("  l7_logf_enable <enable>             - Enable/disable L7_LOGF traces\r\n");
  printf("\r\n");
  printf("  dump_intf_vp_db                                           - dumps virtual port scratchpad (ifNum, PON, GEMid)\r\n");
  printf("  dump_uplinkprot_traps                                     - info about ports/MEPs with uplink protection port traps triggered\r\n");
  printf("\r\n");
  printf("  mgmd.cli                                                  - Show MGMD help\r\n");
  printf("  ber_help                                                  - Show BER/PRBS help\r\n");
  printf("\r\n");

  fflush(stdout);
}

void ptin_help(void)
{
  ptin_debug();
}

/* Counter of CPU packets */
L7_uint64 hapiBroadReceive_packets_count = 0;

/* Other packets */
L7_uint64 hapiBroadReceice_igmp_count   = 0;
L7_uint64 hapiBroadReceice_mld_count    = 0;
L7_uint64 hapiBroadReceice_dhcpv4_count = 0;
L7_uint64 hapiBroadReceice_dhcpv6_count = 0;
L7_uint64 hapiBroadReceice_pppoe_count  = 0;

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

  fflush(stdout);
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

  fflush(stdout);
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

  fflush(stdout);
}


/**
 * Start time measurement
 * 
 * @author mruas (11/8/2014)
 * 
 * @param instance 
 */
void proc_runtime_start(ptin_proc_instance_t instance)
{
  L7_uint64 time_now;
  struct_runtime_t *proc_runtime_ptr;

  if (instance >= PTIN_PROC_MAX)
    return;

  proc_runtime_ptr = &proc_runtime[instance];

  time_now = osapiTimeMicrosecondsGet();

  proc_runtime_ptr->time_ref = time_now;
}

/**
 * Stop and update time measurement
 * 
 * @author mruas (11/8/2014)
 * 
 * @param instance 
 */
void proc_runtime_stop(ptin_proc_instance_t instance)
{
  L7_uint64 time_now;
  struct_runtime_t *proc_runtime_ptr;

  if (instance >= PTIN_PROC_MAX)
    return;

  proc_runtime_ptr = &proc_runtime[instance];

  time_now = osapiTimeMicrosecondsGet();

  proc_runtime_meter_update(instance, time_now - proc_runtime_ptr->time_ref);
}

/**
 * Update proc runtime meter
 * 
 * @param msg_id : message id
 * @param time_delta : time taken to process message
 */
void proc_runtime_meter_update(ptin_proc_instance_t instance, L7_uint32 time_delta)
{
  struct_runtime_t *proc_runtime_ptr;

  if (instance >= PTIN_PROC_MAX)
    return;

  proc_runtime_ptr = &proc_runtime[instance];

  /* If overflow, reset all data */
  if ( (proc_runtime_ptr->number_of_calls==(L7_uint32)-1) ||
       (proc_runtime_ptr->total_runtime + time_delta < proc_runtime_ptr->total_runtime) )
  {
    proc_runtime_meter_init(instance);
  }

  /* Update database */
  proc_runtime_ptr->number_of_calls++;
  proc_runtime_ptr->last_runtime    = time_delta;
  proc_runtime_ptr->total_runtime  += time_delta;

  if ( (proc_runtime_ptr->number_of_calls)==1 || time_delta<(proc_runtime_ptr->min_runtime) )
    proc_runtime_ptr->min_runtime = time_delta;

  if ( time_delta > proc_runtime_ptr->max_runtime )
    proc_runtime_ptr->max_runtime = time_delta;

  proc_runtime_ptr->average_runtime = proc_runtime_ptr->total_runtime/proc_runtime_ptr->number_of_calls;
}

/**
 * Initializes message runtime meter 
 *  
 * @param msg_id : message id 
 */
void proc_runtime_meter_init(ptin_proc_instance_t instance)
{
  if (instance >= PTIN_PROC_MAX)
  {
    memset(proc_runtime,0x00,sizeof(proc_runtime));
  }
  else
  {
    memset(&proc_runtime[instance],0x00,sizeof(struct_runtime_t));
  }
}

/**
 * Prints meter information
 */
void proc_runtime_meter_print(void)
{
  ptin_proc_instance_t i;

  printf(" --------------------------------------------------------------------------------------------------------- \r\n");
  printf("| Instance   |   #Calls   | Total runtime |  Last runtime |  Min. runtime |  Max. runtime | Avrg. runtime |\r\n");
  printf("|------------|------------|---------------|---------------|---------------|---------------|---------------|\r\n");

  for (i=0; i<PTIN_PROC_MAX; i++)
  {
    if (proc_runtime[i].number_of_calls==0)  continue;

    printf("|   0x%04X   | %10u | %10u us | %10u us | %10u us | %10u us | %10u us |\r\n",
           i,
           proc_runtime[i].number_of_calls,
           proc_runtime[i].total_runtime,
           proc_runtime[i].last_runtime,
           proc_runtime[i].min_runtime,
           proc_runtime[i].max_runtime,
           proc_runtime[i].average_runtime);
  }

  printf(" --------------------------------------------------------------------------------------------------------- \r\n");

  fflush(stdout);
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
      case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        strcpy(speed, "A-NEG");
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_10T:
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
        strcpy(speed, "10T");
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        strcpy(speed, "100T");
        break;

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

      case L7_PORTCTRL_PORTSPEED_AAL5_155:
        strcpy(speed, "AAL5");
        break;

      case L7_PORTCTRL_PORTSPEED_LAG:
        strcpy(speed, "LAG");
        break;

      case L7_PORTCTRL_PORTSPEED_UNKNOWN:
        strcpy(speed, "Unkwn");
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
    if (ptin_intf_port2SlotPort(port, &slot, &sport, L7_NULLPTR) != L7_SUCCESS)
    {
      slot  = 0;
      sport = port;
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

    if (slot == 0)
    {
      sprintf(board_id_str,"local");
    }
    else if (ptin_intf_boardid_get(port, &board_type) == L7_SUCCESS && board_type != 0)
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
      case PTIN_BOARD_TYPE_TOLTU20GR:
        sprintf(board_id_str,"TU20GR");
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
      case PTIN_BOARD_TYPE_TOLT8GR:
        sprintf(board_id_str,"TG8GR");
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
    sprintf(bcm_port_str,"%.7s", hapiSlotMapPtr[port].portName);

    printf("| %-6.6s| %2u/%-2u|  %2u  |  %2u | %2u (%-4.4s)| %-3.3s-%u/%u/%u | %-3.3s | %4.4s | %5.5s | %15llu B %11llu bps | %15llu B %11llu bps |\r\n",
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

  fflush(stdout);

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

  fflush(stdout);

  return;
}

