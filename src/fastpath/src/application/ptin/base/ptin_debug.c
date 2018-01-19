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
#include "ptin_msg.h"
#include "usmdb_telnet_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include <unistd.h>
#include <usmdb_nim_api.h>
#include <string.h>
#include <ctype.h>

#include <dapi_db.h>

typedef struct
{
  uint32 number_of_calls;
  uint64 total_runtime;
  uint64 last_runtime;
  uint64 min_runtime;
  uint64 max_runtime;
  uint64 average_runtime;

  L7_uint64 time_ref;
} struct_runtime_t;

static struct_runtime_t proc_runtime[PTIN_PROC_MAX+1];


extern L7_uint32 hpcLocalCardIdGet(int slotNum);


/*************************** 
 * EXTERNAL FUNCTIONS
 ***************************/
 
#define TIMERS_MAX  90

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
  printf("  ptin_intf_dump                                                  - prints info about all PHY interfaces\r\n");
  printf("  ptin_lag_dump                                                   - prints info about all LAG interfaces\r\n");
  printf("  ptin_intf_shaper_max_dump                                       - prints the max rate for all interfaces\r\n");
  printf("  ptin_evc_dump <id>                                              - prints info about eEVCs (use -1 to display all)\r\n");
  printf("  ptin_evc_map                                                    - prints EVCs extended indexes mapping\r\n");
  printf("  ptin_evc_which <vlan_int>                                       - prints info about the EVC related to the given internal vlan\r\n");
  printf("  ptin_l2_maclimit_dump                                           - Dumps MAC limit tables (non empty)\r\n");
  printf("  ptin_l2_maclimit_status                                         - Dumps MAC limit tables per interface (non empty)\r\n");
  printf("  ptin_l2_maclimit_config <sys> <ifType> <ifId> <vid> <lmt>       - Configures L2 MAC Limit on system/ per VLAN / per Port\r\n");
  printf("  slot_monitor_enable <slot> <enable> <fcs_threshold>             - Enables or disables slot monitor reset\r\n");
  printf("  slot_monitor_dump                                               - Dump link status information\r\n");
  printf("  slot_monitor_reset <port>                                       - Reset link status data (local and remote)\r\n");
  printf("  ptin_debug_example <intIfNum> <oper> <param1> <param2>          - Generic DTL processor example\r\n");
  printf("  osapiShowTasks                                                  - Show all existent tasks\r\n");
  printf("  ptin_process_cpu                                                - Show CPU utilization by each thread\r\n");
  printf("  ptin_process_threshold                                          - Show CPU thresholds\r\n");
  printf("  sysapiCpuUtilTableDump <show_tasks>                             - Show CPU calculation results for all tasks\r\n");
  printf("  cliTelnetAdminModeSet <port>\r\n");
  printf("  webAccessAdminModeSet <port>\r\n");
  printf("\r\n");                                                          
  printf("  ptin_evc_clean_all <id>                                         - Clean all profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intf_clean_all <id> <intf_type> <intf_id>              - Clean all profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_allintfs_clean <id>                                    - Clean all service profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intf_clean <id> <intf_type> <intf_id>                  - Clean service profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_allclients_clean <id>                                  - Clean all client profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_intfclients_clean <id> <intf_type> <intf_id>           - Clean all client profiles and counters of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_client_clean <id> <intf_type> <intf_id> <cvlan>        - Clean one client profiles and counters of an EVC (internal id)\r\n");
  printf("  ptin_evc_clean_counters_enable <1/0>                            - Enable or disable counters remotion during cleanup (default is 1)\r\n");
  printf("  ptin_evc_clean_profiles_enable <1/0>                            - Enable or disable profiles remotion during cleanup (default is 0)\r\n");
  printf("\r\n");                                                          
  printf("  ptin_evc_destroy_all                                            - Destroy all EVCs\r\n");
  printf("  ptin_evc_destroy <eEVC_id>                                      - Destroy an EVC with all its clients, profiles and counters\r\n");
  printf("  ptin_evc_allclientsflows_remove <id>                            - Remove all clients of an EVC (internal id)\r\n");
  printf("  ptin_evc_intfclientsflows_remove <id> <intfType> <intfId>       - Remove all clients of one interface of an EVC (internal id)\r\n");
  printf("  ptin_evc_client_remove <id> <intf_type> <intf_id> <cvlan>       - Remove a client of an EVC (internal id)\r\n");
  printf("\r\n");                                                          
  printf("  ptin_pvid_dump                                                  - Dump default vlans for all ports\r\n");
  printf("  ptin_xlate_dump <stage> [inv]                                   - Dump translation entries for ingress (1) or egress (2) stage\r\n");
  printf("  dump_intf_vp_db                                                 - dumps virtual port scratchpad (ifNum, PON, GEMid)\r\n");
  printf("  dump_uplinkprot_traps                                           - info about ports/MEPs with uplink protection port traps triggered\r\n");
  printf("  ptin_erps_dump <id>                                             - prints info about ERPS (use -1 to display all)\r\n");
  printf("  ptin_intf_boardtype_dump                                        - prints board ids information\r\n");
  printf("  ptin_linkscan_control_global <enable>                           - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("  ptin_intf_linkscan_control <port> <enable>                      - Enable/disable globally linkscan control (only for boards with this feature)\r\n");
  printf("\r\n");                                                          
  printf("  ssm_debug_dump                                                  - prints info about SSM operation\r\n");
  printf("  ssm_debug_write <slot> <intf> <ssm_rx> <ssm_tx> <link>          - modifies shared memory contents about SSM\r\n");
  printf("  ptin_reclk_dump                                                 - prints info about sync-E recovery clocks\r\n");
  printf("  ptin_reclk_write <primary_port> <backup_port>                   - modifies shared memory contents about sync-E recovery clocks\r\n");
  printf("  ptin_dhcp_dump <show_clients>                                   - prints info about DHCP instances\r\n");
  printf("  ptin_dhcpClients_dump                                           - prints info about DHCP clients\r\n");
  printf("  ptin_pppoe_dump <show_clients>                                  - prints info about PPPoE instances\r\n");
  printf("  ptin_pppoeClients_dump                                          - prints info about PPPoE clients\r\n");
  printf("  ptin_igmp_dump                                                  - prints info about IGMP instances\r\n");
  printf("  ptin_igmp_channels_dump                                         - prints info about Group List\r\n");
  printf("  snoop_l3_channels_dump                                          - prints info about L3 Channel Entries\r\n");
  printf("  snoop_l3_channels_intf_mask_dump                                - prints info about L3 Channel Interface Bitmap\r\n");
//printf("  ptin_igmp_mfdb_dump                                             - prints MFDB table entries\r\n");
//printf("  ptin_igmp_snoop_dump <id>                                       - prints IGMP snooping entries (use -1 to display all)\r\n");
  printf("  ptin_igmp_proxy_dump                                            - prints IGMP proxy configuration\r\n");
  printf("  ptin_igmp_querier_dump <evc_idx>                                - prints IGMP querier configuration (none/0/-1 for all)\r\n");
  printf("  ptin_igmp_device_clients_dump                                   - prints info about IMGP device clients\r\n");
  printf("  ptin_igmp_group_clients_dump                                    - prints info about IMGP group of clients\r\n");
  printf("  ptin_igmptimer_show                                             - Show IGMP packet processing time data\r\n");
  printf("  ptin_igmptimer_clear                                            - Initializes IGMP packet processing time data\r\n");
  printf("  dai_settings_get                                                - Get DAI configurations\r\n");
  printf("  daiAcl_table_dump                                               - Dump ARP ACL table\r\n");
  printf("  ptin_mirror_dump                                                - Dump Mirror sessions \r\n");
  printf("\r\n");                                                          
  printf("  snoopDebugTraceEnable                                           - Enable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceDisable                                          - Disable IGMP snoop trace debugging\r\n");
  printf("  snoopDebugTraceFlagsSetAF <1/2>                                 - Activate specific IGMP snoop trace messages\r\n");
  printf("  dsTraceFlagsSet <flags_mask>                                    - Enable detailed debug for DHCP snooping (flags_mask=0xff)\r\n");
  printf("  ptin_debug_igmp_packet_trace_enable <enable>                    - Show IGMP Packet Trace\r\n");    
  printf("  dtlNetPtinDebugEnable 0xf                                       - Print all packets transmited from processor\r\n");
  printf("  ptin_timers_clear                                               - Inializes all processing timers\r\n");
  printf("  ptin_timers_show                                                - Show all processing timers contents\r\n");
  printf("  ptin_msg_runtime_init <msg_id>                                  - Initialize runtime measurements for msg_id (-1 for all)\r\n");
  printf("  ptin_msg_runtime_show                                           - Show runtime measurements for all messages\r\n");
  printf("\r\n");                                                          
  printf("  cpu_intercept_debug_enable <debug_level>                        - Print all packets intercepted by the processor (1:stdout, 0xX0:LOGs)\r\n");
  printf("  pdu_receive_debug_enable <enable>                               - Print all PDUs received and validated for further processing\r\n");
  printf("  pdu_process_debug_enable <enable>                               - Print all PDUs ready for final processing\r\n");
  printf("  cpu_tx_debug_enable <debug_level>                               - Print all packets transmited from processor (1:stdout, 0xX0:LOGs)\r\n");
  printf("  ptin_debug_trap_packets_dump <enable>                           - Print received packets content\r\n");
  printf("  ptin_debug_tx_packets_dump <enable>                             - Print transmited packets content (from CPU)\r\n");
  printf("  dtlFamilyCallbacks_getAddr                                      - Get DTL Family Callbacks addresses (to be compared with familyCallbackListDebugShow)\r\n");
  printf("  familyCallbackListDebugShow                                     - Show list of packet processing callbacks related to familyCallback's\r\n");
  printf("  sysNetInterceptDebugShow                                        - Show list of packet processing callbacks related to sysnetPduHooks\r\n");
  printf("  sysNetNotifyListDebugShow                                       - Show list of packet processing callbacks related to sysnetNotifyList\r\n");
  printf("\r\n");                                                          
  printf("  ptin_debug_trap_packets <port> <OVid> <IVid> <only_drops>       - Redirect packets from <port>, with <ovid> and <ivid>, to CPU\r\n");
  printf("  ptin_debug_trap_packets_tx <src_port> <tx_port> <OVid> <IVid>   - Redirect packets egressing from <port>, with <ovid> and <ivid>, to CPU\r\n");
  printf("  ptin_debug_trap_packets_cancel                                  - Cancel redirection packets rule to CPU\r\n");
  printf("  ptin_debug_trap_packets_state                                   - Show if some redirection packet rule to CPU is defined\r\n");
  printf("\r\n");                                                          
  printf("  ptin_traprules_dump                                             - Dump configured trap (to CPU) rules\r\n");
  printf("  ptin_stormcontrol_dump                                          - Dump storm control configurations\r\n");
  printf("  ptin_intf_stormcontrol_dump                                     - Dump storm control's interface configurations\r\n");
  printf("  ptin_bwpolicer_dump_debug                                       - Dump configured bandwidth policers\r\n");
  printf("  ptin_bwpolicer_flush_debug                                      - Flush configured EVC/client policers (at request)\r\n");
  printf("  ptin_fpcounters_dump_debug                                      - Dump configured EVC/client counters (at request)\r\n");
  printf("  ptin_fpcounters_flush_debug                                     - Flush configured EVC/client counters (at request)\r\n");
  printf("  ptin_debug_intf_cos_policer_set   <intf_type> <intf_id> <cos> <cir> <eir> <cbs> <ebs> - Create Interface/COS policer\r\n");
  printf("  ptin_debug_intf_cos_policer_clear <intf_type> <intf_id> <cos>   - Remove Interface/COS policer\r\n");
  printf("  ptin_hapi_qos_dump                                              - Dump VLAN-QoS rules\r\n");
  printf("  ptin_vcap_defvid_dump                                           - Flush configured VCAP rules associated to defVID\r\n");
  printf("  ptin_aclDbDump_all                                              - Flush configured ACL \r\n");
  printf("  hapiBroadReconfigTrap <packet_type> <reenable>                  - Reconfigure IGMP=1/MLD=2/DHCP=3/PPPoE=4/APS=5/IPDTL0=7/MEP=8/MIP=9 trap rules\r\n");
  printf("  hapiBroadReconfigTrapMeter <packet_type> <cir> <cbs>            - Reconfigure meters for IGMP=1/MLD=2/DHCP=3/PPPoE=4/APS=5/IPDTL0=7/MEP=8/MIP=9 trap rules\r\n");
  printf("  hapiBroadReconfigInbandMeter <cir> <cbs>                        - Reconfigure meter for the INBAND trap rule\r\n");
  printf("  hapi_ptin_stormControl_cpu_set <enable> <cir> <cbs>             - Reconfigure egress meter for CPU packets\r\n");  
  printf("  hapiBroadDebugPolicyEnable <level>                              - Enable policy management debug messages (minimum level should be 3)\r\n");
  printf("\r\n");
#if (PTIN_BOARD_IS_STANDALONE)
  printf("  ptin_ptp_oam_prtvid_dump                                        - Dump FPGA PRT VID table (just OLT1T0-AC)\r\n");
  printf("  ptin_hapi_ptp_dump                                              - Dump FPGA PTP table (just OLT1T0-AC)\r\n");
  printf("  ptin_hapi_oam_dump                                              - Dump FPGA OAM table (just OLT1T0-AC)\r\n");
#endif
  printf("\r\n");
  printf("Multicast Admission Control\r\n");
  printf("  ptin_igmp_admission_control_port_dump_active                    - Dump Admission Control Parameters of Port\r\n");
  printf("  ptin_igmp_admission_control_multicast_service_dump_active       - Dump Admission Control Parameters of Multicast Service\r\n");
  printf("  ptin_igmp_group_clients_dump                                     - Dump Admission Control Parameters of Service with IGMP Flag\r\n");
  printf("  ptin_igmp_admission_control_port_reset_allocation               - Reset Admission Control Parameters of Port\r\n");
  printf("  ptin_igmp_admission_control_multicast_service_reset_allocation  - Reset Admission Control Parameters of Multicast Service\r\n");
  printf("  ptin_igmp_admission_control_group_clients_reset_allocation      - Reset Admission Control Parameters of Service with IGMP Flag\r\n");
  printf("  ptin_igmp_admission_control_multicast_service_reset_allocation  - Reset All Admission Control Parameters\r\n"); 
  printf("\r\n");
  printf("Multicast Channel Packages\r\n");
  printf("  ptin_igmp_multicast_package_add    <packageId>                                                                                    \r\n");
  printf("  ptin_igmp_multicast_package_remove <packageId>                                                                                    \r\n");
  printf("  ptin_igmp_multicast_package_dump   <packageId>                                                                                    \r\n");
  printf("  ptin_igmp_multicast_service_add    <ptinPort> <onuId> <serviceId>                                                                 \r\n");
  printf("  ptin_igmp_multicast_service_remove <ptinPort> <onuId> <serviceId>                                                                 \r\n");
  printf("  ptin_igmp_multicast_service_dump   <ptinPort> <onuId> <serviceId>                                                                 \r\n");
  printf("  ptin_igmp_multicast_service_dump_active                                                                                           \r\n");  
  printf("  ptin_igmp_debug_multicast_package_channels_add    <packageId> <serviceId> <groupAddr> <groupMask> <sourceAddr> <sourceMask>       \r\n");
  printf("  ptin_igmp_debug_multicast_package_channels_remove <packageId> <serviceId> <groupAddr> <groupMask> <sourceAddr> <sourceMask>       \r\n");
  printf("  ptin_igmp_debug_multicast_client_package_add    <packageId> <ptinPort> <groupClientId>                                            \r\n");
  printf("  ptin_igmp_debug_multicast_client_package_remove <packageId> <ptinPort> <groupClientId>                                            \r\n");
  printf("\r\n");
  printf("Developer Debug Level\r\n");
  printf("  ptin_debug_evc_enable <enable>                                  - Show more debugging logs for the EVC module\r\n");
  printf("  ptin_debug_igmp_enable <enable>                                 - Show more debugging logs for the IGMP module\r\n");  
  printf("  ptin_debug_dhcp_enable <enable>                                 - Show more debugging logs for the DHCP module\r\n");
  printf("  ptin_debug_pppoe_enable <enable>                                - Show more debugging logs for the PPPoE module\r\n");
  printf("  ptin_debug_dai_enable <enable>                                  - Show more debugging logs for the DAI module\r\n");
  printf("  ptin_debug_xlate_enable <enable>                                - Show more debugging logs for the XLATE module\r\n");
  printf("  ptin_debug_oam_set <enable>                                     - Show more debugging logs for the OAM module\r\n");
  printf("  ptin_debug_lacp_set <enable>                                    - Show more debugging logs for the LACP module\r\n");
  printf("  ptin_debug_dtl_set <enable>                                     - Show more debugging logs for the DTL module\r\n");
  printf("  ptin_debug_hapi_l2_enable <enable>                              - Show more debugging logs for the HAPI L2 module\r\n");
  printf("  ptin_debug_opensaf_enable <enable>                              - Show more debugging logs for the OPENSAF module\r\n");
#if (PTIN_BOARD == PTIN_BOARD_TG16GF || PTIN_BOARD == PTIN_BOARD_OLT1T0F || PTIN_BOARD == PTIN_BOARD_OLT1T0 || PTIN_BOARD == PTIN_BOARD_TT04SXG)
  printf("  cpld_spi_read_debug <addr>                                      - Read CPLD register using SPI\r\n");
  printf("  cpld_spi_write_debug <addr> <value>                             - Write CPLD register using SPI\r\n");
  printf("  set_debug_APS_CCM_pktTimer <0/1>                                - (De)activate rx APS/CCM packets' timing measurements\r\n");
#endif

#ifdef NGPON2_SUPPORTED 
  printf("  ptin_intf_NGPON2_groups_dump                                    - Dump all NGPON2 groups \r\n");
  printf("  remove_all_offlineEvc                                           - Remove all the offline EVC's \r\n");
  printf("  dump_all_offlineEvc                                             - Dump all the offline EVC's \r\n");
#endif

  printf("\r\n");
  printf("  logger_help                                                     - displays logger help\r\n");
  printf("  logger_sev_set   <ctx_mask> <severity>                          - use logger_help for more info\r\n");
  printf("  logger_color_set <ctx_mask> <color>                             - use logger_help for more info\r\n");
  printf("  l7_logf_enable <enable>                                         - Enable/disable L7_LOGF traces\r\n");
  printf("\r\n");                                                          
  printf("  mgmd.cli                                                        - Show MGMD help\r\n");
  printf("  ber_help                                                        - Show BER/PRBS help\r\n");
  printf("\r\n");
  printf("  ipc_msg_bytes_debug_enable (0/1)                                - Dump IPC MSGs' bytes\r\n");
  printf("\r\n");

  fflush(stdout);
}

void ptin_help(void)
{
  ptin_debug();
}

void cliTelnetAdminModeSet(L7_int port)
{
  if (port == FD_TELNET_PORT_NUM)
  {
    usmDbAgentTelnetAdminModeSet(1, L7_ENABLE);
    printf("CLI Telnet enabled\r\n");
  }
  else
  {
    usmDbAgentTelnetAdminModeSet(1, L7_DISABLE);
    if (port == 0)
    {
      printf("CLI Telnet disabled\r\n");
    }
  }
  fflush(stdout);
}

void webAccessAdminModeSet(L7_int port)
{
  if (port == FD_HTTP_PORT_NUM)
  {
    usmDbSwDevCtrlWebMgmtModeSet(1, L7_ENABLE);
    printf("WEB Access enabled\r\n");
  }
  else
  {
    usmDbSwDevCtrlWebMgmtModeSet(1, L7_DISABLE);
    if (port == 0)
    {
      printf("WEB Access disabled\r\n");
    }
  }
  fflush(stdout);
}

/**
 * Example to use Generic DTL processor
 * 
 * @param intIfNum 
 * @param operation 
 * @param param1 
 * @param param2 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_debug_example(L7_uint32 intIfNum, L7_uint operation, L7_uint param1, L7_uint param2)
{
  ptin_dtl_example_t example;
  L7_RC_t rc;

  PT_LOG_INFO(LOG_CTX_MISC, "Executing generic DTL processor: intIfNum=%u operation=%u param1=%u param2=%u sizeof(ptin_dtl_example_t)=%u",
           intIfNum, operation, param1, param2, sizeof(ptin_dtl_example_t));

  memset(&example, 0x00, sizeof(example));
  example.param1 = param1;
  example.param2 = param2;

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_EXAMPLE, operation, sizeof(ptin_dtl_example_t), (void *) &example);

  PT_LOG_INFO(LOG_CTX_DTL, "Result rc=%u", rc);

  return rc;
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

unsigned char debug_APS_CCM_pktTimer = 0;
void set_debug_APS_CCM_pktTimer(unsigned char v) {debug_APS_CCM_pktTimer=v;}

void ptin_igmptimer_clear(void)
{
  L7_uint64 time_now = osapiTimeMillisecondsGet();

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
  L7_uint32 n_packets, n_packets_ok, n_packets_er;
  L7_uint64 time_delta;

  //time_delta = debug_pktTimer.time_end - debug_pktTimer.time_start;
  time_delta = osapiTimeMillisecondsDiff(debug_pktTimer.time_end, debug_pktTimer.time_start);

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
  printf(" Time taken for processing (ms) = %llu\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %llu\r\n",time_delta/n_packets);
  printf(" Mean number of packets processed in 1 second = %llu\r\n",(n_packets*1000)/time_delta);
  printf("\nFor successfully processed packets:\r\n");
  printf(" Number of packets processed    = %u\r\n",n_packets_ok);
  printf(" Time taken for processing (ms) = %llu\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %llu\r\n",time_delta/n_packets_ok);
  printf(" Mean number of packets processed in 1 second = %llu\r\n",(n_packets_ok*1000)/time_delta);
  printf("\nFor non successfully processed packets:\r\n");
  printf(" Number of packets processed    = %u\r\n",n_packets_er);
  printf(" Time taken for processing (ms) = %llu\r\n",time_delta);
  printf(" Mean time for processing one packet (ms)     = %llu\r\n",time_delta/n_packets_er);
  printf(" Mean number of packets processed in 1 second = %llu\r\n",(n_packets_er*1000)/time_delta);

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
  static char _1st_time=1;

  if (_1st_time) {
      for (time_now=0; time_now<PTIN_PROC_MAX; time_now++) proc_runtime_meter_init(time_now);
      _1st_time=0;
  }

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

  if (proc_runtime_ptr->time_ref != (L7_uint64)-1 &&
      proc_runtime_ptr->time_ref <= time_now)
  {
    proc_runtime_meter_update(instance, time_now - proc_runtime_ptr->time_ref);
  }

  proc_runtime_ptr->time_ref = (L7_uint64)-1;
}

/**
 * Update proc runtime meter
 * 
 * @param msg_id : message id
 * @param time_delta : time taken to process message
 */
void proc_runtime_meter_update(ptin_proc_instance_t instance, L7_uint64 time_delta)
{
  struct_runtime_t *proc_runtime_ptr;

  if (instance >= PTIN_PROC_MAX)
    return;

  proc_runtime_ptr = &proc_runtime[instance];

  /* If overflow, reset all data */
  if ( (proc_runtime_ptr->number_of_calls == (L7_uint32)-1) ||
       (proc_runtime_ptr->total_runtime + time_delta < proc_runtime_ptr->total_runtime) )
  {
    proc_runtime_meter_init(instance);
  }

  /* Update database */
  proc_runtime_ptr->number_of_calls++;
  proc_runtime_ptr->last_runtime    = time_delta;
  proc_runtime_ptr->total_runtime  += (L7_uint64) time_delta;

  if ( /*1==proc_runtime_ptr->number_of_calls || 0==proc_runtime_ptr->min_runtime ||*/ time_delta<(proc_runtime_ptr->min_runtime) )
    proc_runtime_ptr->min_runtime = time_delta;

  if ( time_delta > proc_runtime_ptr->max_runtime )
    proc_runtime_ptr->max_runtime = time_delta;

  proc_runtime_ptr->average_runtime = //(L7_uint32)
                                       (proc_runtime_ptr->total_runtime/proc_runtime_ptr->number_of_calls);
}

/**
 * Initializes message runtime meter 
 *  
 * @param msg_id : message id 
 */
void proc_runtime_meter_init(ptin_proc_instance_t instance)
{
  L7_uint i;

  if (instance < PTIN_PROC_MAX)
  {
    memset(&proc_runtime[instance], 0x00 ,sizeof(struct_runtime_t));
    proc_runtime[instance].time_ref = (L7_uint64)-1;
    proc_runtime[instance].min_runtime = -1;
  }
  else
  {
    for (i = 0; i < PTIN_PROC_MAX; i++)
    {
      memset(&proc_runtime[instance], 0x00 ,sizeof(struct_runtime_t));
      proc_runtime[instance].time_ref = (L7_uint64)-1;
      proc_runtime[instance].min_runtime = -1;
    }
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

    printf("|   0x%04X   | %10u | %10llu us | %10llu us | %10llu us | %10llu us | %10llu us |\r\n",
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
  L7_uint32 lagIntfNum = -1;
  L7_uint32 speed_mode;
#if (!PTIN_BOARD_IS_DNX)
  L7_uint32 frameOversize;
#endif
  L7_uint32 frameMax;
  L7_char8  speed[8];
  L7_char8  bcm_port_str[8];
  L7_char8  board_id_str[8];
  L7_int32  bcm_port;
  L7_uint   admin;
  L7_uint   link;
#if (!PTIN_BOARD_IS_DNX)
  ptin_HWPortExt_t  portExt;
#endif
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr = dapiCardPtr->slotMap;
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------+------------------------+------------------------+\r\n");
  printf("| Board | Slot | Port | IfN | bcm_port | MEF Ext.* | Ena | Link | Speed | FOvr/FMax |   RX:  bytes       bps |   TX:  bytes       bps |\r\n");
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------+------------------------+------------------------+\r\n");
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    if (!KERNEL_NODE_IS("OLT1T0-AC"))
    {
      /* Skip FPGA port for non OLT1T0-AC systems */
      if (port == PTIN_PORT_FPGA)
      {
        continue;
      }
    }
    #endif

    /* Get intIfNum ID */
    ptin_intf_port2intIfNum(port, &intIfNum);

    /* Admin state */
    if (usmDbIfAdminStateGet(1, intIfNum, &admin) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get admin state of port# %u", port);
      continue;
    }

    if (nimGetLagIntfNum(intIfNum, &lagIntfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get lagIntfNum from port# %u", port);
    }

    /* Speed */
    if (usmDbIfSpeedGet(1, intIfNum, &speed_mode) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get speed of port# %u", port);
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
      case L7_PORTCTRL_PORTSPEED_FULL_40G:
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

    /* Get Oversize packets limit */
    if (usmDbIfConfigMaxFrameSizeGet(intIfNum, &frameMax) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get max MTU of port# %d", port);
      continue;
    }
#if (!PTIN_BOARD_IS_DNX)
    if (ptin_intf_frame_oversize_get(intIfNum, &frameOversize) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get Oversize frame limit of port# %d", port);
      continue;
    }
#endif

    /* Link State */
    if (nimGetIntfLinkState(intIfNum, &link) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get link state of port# %d", port);
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

  #if (!PTIN_BOARD_IS_DNX)
    /* Apply configuration */
    if (dtlPtinL2PortExtGet(intIfNum, &portExt) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error getting MEF Ext of port# %d", port);
      continue;
    }
  #endif

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
      case PTIN_BOARD_TYPE_TG16GF:
        sprintf(board_id_str,"TG16GF");
        break;
      case PTIN_BOARD_TYPE_TOLT8G:
        sprintf(board_id_str,"TOLT8G");
        break;
      case PTIN_BOARD_TYPE_TOLT8GR:
        sprintf(board_id_str,"TOLT8GR");
        break;
      case PTIN_BOARD_TYPE_TA48GE:
        sprintf(board_id_str,"TA48GE");
        break;
      case PTIN_BOARD_TYPE_TA48GED:
        sprintf(board_id_str,"TA48GED");
        break;
      case PTIN_BOARD_TYPE_TT04SXG:
        sprintf(board_id_str,"TT04SXG");
        break;
      case PTIN_BOARD_TYPE_TT08SXG:
        sprintf(board_id_str,"TT08SXG");
        break;
      case PTIN_BOARD_TYPE_TU100G:
        sprintf(board_id_str,"TU100G");
        break;
      case PTIN_BOARD_TYPE_TA12XGE:
        sprintf(board_id_str,"TA12XG");
        break;
      default:
        sprintf(board_id_str," 0x%02x", board_type);
        break;
      }
    }
    else
  #endif
    {
      sprintf(board_id_str,"  ---  ");
    }

    /* Switch port: ge/xe (indexes changed according to the board) */
    sprintf(bcm_port_str,"%.7s", hapiSlotMapPtr[port].portName);

  #if (PTIN_BOARD_IS_DNX)
    printf("|%-7.7s| %2u/%-2u|  %2u  |  %2u | %2u (%-4.4s)|    ---    | %-3.3s | %4.4s | %5.5s |     %5u |%s%12llu %9llu%s|%s%12llu %9llu%s|\r\n",
           board_id_str, slot, sport,
           port,
           intIfNum,
           bcm_port, bcm_port_str,
           admin ? "Ena" : "Dis",
           link == L7_UP ? " Up " : "Down",
           speed,
           frameMax,
           (portStats.Rx.etherStatsOctets >= 1000000000000ULL) ? "*" : " ", portStats.Rx.etherStatsOctets % 1000000000000ULL,
           portStats.Rx.Throughput / ((portStats.Rx.Throughput>=1000000000ULL) ? 1000 : 1), (portStats.Rx.Throughput>=1000000000ULL) ? "K" : " ",
           (portStats.Tx.etherStatsOctets >= 1000000000000ULL) ? "*" : " ", portStats.Tx.etherStatsOctets % 1000000000000ULL,
           portStats.Tx.Throughput / ((portStats.Tx.Throughput>=1000000000ULL) ? 1000 : 1), (portStats.Tx.Throughput>=1000000000ULL) ? "K" : " ");
  #else
    printf("|%-7.7s| %2u/%-2u|  %2u  |  %2u/%-3d | %2u (%-4.4s)| %-3.3s-%u/%u/%u | %-3.3s | %4.4s | %5.5s |%5u/%-5u|%s%12llu %9llu%s|%s%12llu %9llu%s|\r\n",
           board_id_str, slot, sport,
           port,
           intIfNum,
           intIfNum==lagIntfNum?-1:lagIntfNum,
           bcm_port, bcm_port_str,
           (portExt.egress_type == PTIN_PORT_EGRESS_TYPE_ISOLATED) ? "ISO" : ((portExt.egress_type == PTIN_PORT_EGRESS_TYPE_COMMUNITY) ? "COM" : "PRO"),
           portExt.macLearn_stationMove_enable, portExt.macLearn_stationMove_samePrio, portExt.macLearn_stationMove_prio,
           admin ? "Ena" : "Dis",
           link == L7_UP ? " Up " : "Down",
           speed,
           frameOversize, frameMax,
           (portStats.Rx.etherStatsOctets >= 1000000000000ULL) ? "*" : " ", portStats.Rx.etherStatsOctets % 1000000000000ULL,
           portStats.Rx.Throughput / ((portStats.Rx.Throughput>=1000000000ULL) ? 1000 : 1), (portStats.Rx.Throughput>=1000000000ULL) ? "K" : " ",
           (portStats.Tx.etherStatsOctets >= 1000000000000ULL) ? "*" : " ", portStats.Tx.etherStatsOctets % 1000000000000ULL,
           portStats.Tx.Throughput / ((portStats.Tx.Throughput>=1000000000ULL) ? 1000 : 1), (portStats.Tx.Throughput>=1000000000ULL) ? "K" : " ");
    #endif
  }
  printf("+-------+------+------+-----+----------+-----------+-----+------+-------+-----------+------------------------+------------------------+\r\n");
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

#define ESC_CHAR     0x1b
#define L7_BUFFSIZE  16*1024
L7_char8 buffer_threads[L7_BUFFSIZE];

inline void print_string_hex(char *str, L7_uint16 max, L7_BOOL print_all, L7_BOOL char_identify)
{
  L7_uint16 i;

  if (str == L7_NULLPTR)
  {
    return;
  }

  for (i = 0; i < max && (print_all || str[i] != '\0'); i++)
  {
    printf("%02x", str[i]);
    if (char_identify)
    {
      if (str[i]=='\0')
        printf("Z");
      else if (str[i] == '\r')
        printf("R");
      else if (str[i] == '\n')
        printf("N");
      else if (str[i] == '\e')
        printf("E");
      else if (str[i]=='\t' || str[i]==' ')
        printf("_");
      else if (isdigit(str[i]))
        printf("=");
      else if (isalpha(str[i]))
        printf("a");
      else if (ispunct(str[i]))
        printf(".");
      else if (isgraph(str[i]))
        printf("#");
      else if (iscntrl(str[i]))
        printf("$");
      else if (str[i] < ' ' || (unsigned char) str[i] >= 127)
        printf("!");
      else
        printf("?");
    }
    printf(" ");
  }
  printf("\r\n");
}

/**
 * Show CPU utilization for each thread
 * 
 * @param hex 
 * @param print_all 
 * @param char_identify 
 */
void ptin_process_cpu(void)
{
  L7_char8   printBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8   *cpuUtil = "CPU Utilization:";
  L7_uint32  cpuUtilLen = osapiStrnlen(cpuUtil, L7_CLI_MAX_STRING_LENGTH);

  memset(buffer_threads, 0x00, sizeof(buffer_threads));

  if(usmdbCpuUtilizationStringGet(1, buffer_threads, L7_BUFFSIZE) != L7_SUCCESS)
  {
    printf("Error getting CPU utilization!\r\n");
    return;
  }

  L7_char8 *start;
  L7_char8 *end;

  start = buffer_threads;
  end   = buffer_threads;
  buffer_threads[L7_BUFFSIZE-1] = 0; /* For safety */

  do
  {
    while ( ( isprint((int)*end ) ) ||
             ( ESC_CHAR == *end  ) )
    {
       end++;
    }
    *end = '\0';

    if (osapiStrncmp(start, cpuUtil, cpuUtilLen) == L7_NULL)
    {
      osapiSnprintf(printBuf, L7_CLI_MAX_STRING_LENGTH,"\r\n %s\r\n\r\n", start);
      printf("%s",printBuf);
    }
    else
    {
      osapiSnprintf(printBuf, L7_CLI_MAX_STRING_LENGTH,"%s\r\n", start);
      printf("%s",printBuf);
    }

    end++;               /* Skip over zero */
    while ( ( iscntrl((int)*end ) ) &&
             ( *end != ESC_CHAR ) &&
             ( *end != '\0' )    &&
             ( end - buffer_threads < L7_BUFFSIZE  ) )
    {
       end++;
    }
    start = end;
  } while ( ( start - buffer_threads < L7_BUFFSIZE ) && ( start[1] != '\0' ) );

  printf("\r\nDone!\r\n");
}

/**
 * Show CPU thresholds
 */
void ptin_process_threshold(void)
{
  L7_uint32  risingThr = 0, risingInterval = 0;
  L7_uint32  fallingThr = 0, fallingInterval = 0;
  L7_uint32  memoryLowWatermark = 0;

  (void) usmdbCpuUtilMonitorParamGet(1, 
                                    SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM,
                                    &risingThr);
  (void) usmdbCpuUtilMonitorParamGet(1, 
                                    SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM,
                                    &risingInterval);
  (void) usmdbCpuUtilMonitorParamGet(1, 
                                    SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM,
                                    &fallingThr);
  (void) usmdbCpuUtilMonitorParamGet(1, 
                                    SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM,
                                    &fallingInterval);
  (void) usmdbCpuFreeMemoryThresholdGet(1, &memoryLowWatermark);

  printf("CPU Utilization Monitoring Parameters\r\n");
  printf("Rising Threshold : %d %%\r\n", risingThr);
  printf("Rising Interval  : %d secs\r\n", risingInterval);
  printf("Falling Threshold: %d %%\r\n", fallingThr);
  printf("Falling Interval : %d secs\r\n", fallingInterval);
  printf("\r\n");
  printf("CPU Free Memory Monitoring Threshold: %d KB\r\n", memoryLowWatermark);
}



/**
 * ptin_debug_l2_maclimit_config
 * 
 * @param system 
 * @param intf_type 
 * @param intf_id 
 * @param vid 
 * @param limit 
 */
void ptin_l2_maclimit_config(L7_uint8 system, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 limit, L7_uint8 action, L7_uint8 send_trap)
{
  msg_l2_maclimit_config_t maclimit;

  memset(&maclimit, 0, sizeof(msg_l2_maclimit_config_t));

  maclimit.slotId =         1;

  system          = 0;
  maclimit.system = system;

  if ((intf_type != (L7_uint8)-1) && (intf_id!=(L7_uint8)-1))
  {
    maclimit.intf.intf_type = intf_type;
    maclimit.intf.intf_id =   intf_id;
  }
  if(limit !=0)
  {
    maclimit.limit = limit;
    maclimit.mask |= L2_MACLIMIT_MASK_LIMIT;
  }

  maclimit.action     = action;
  maclimit.send_trap  = send_trap;
  maclimit.mask      |= L2_MACLIMIT_MASK_ACTION;
  maclimit.mask      |= L2_MACLIMIT_MASK_SEND_TRAP ;      

  ptin_msg_l2_maclimit_config(&maclimit);
}

/**
 * ptin_debug_l2_maclimit_config
 * 
 * @param intf_type 
 * @param intf_id 
 */
void ptin_l2_maclimit_status(L7_uint8 intf_type, L7_uint8 intf_id)
{

  msg_l2_maclimit_status_t status;

  memset(&status, 0, sizeof(msg_l2_maclimit_status_t));

  status.slotId =         1;

  if ((intf_type != (L7_uint8)-1) && (intf_id!=(L7_uint8)-1))
  {
    status.intf.intf_type = intf_type;
    status.intf.intf_id =   intf_id;
  }

  ptin_msg_l2_maclimit_status(&status);

}

void ptin_sys_mac_set(L7_uint8 macAddr0, L7_uint8 macAddr1, L7_uint8 macAddr2, L7_uint8 macAddr3, L7_uint8 macAddr4, L7_uint8 macAddr5)
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  mac[0] = macAddr0;
  mac[1] = macAddr1;
  mac[2] = macAddr2;
  mac[3] = macAddr3;
  mac[4] = macAddr4;
  mac[5] = macAddr5;

  printf("Applying MAC address %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  usmDbSwDevCtrlLocalAdminAddrSet(1, mac);
}

