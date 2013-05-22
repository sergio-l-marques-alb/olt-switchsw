/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename default_configurator.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 09/15/2000
*
* @author bmutz
* @end
*
**********************************************************************/

#ifndef INCLUDE_DEFAULT_CNFGR
#define INCLUDE_DEFAULT_CNFGR

#include "cnfgr.h"

/* Start of CONFIGURATOR SIM Component's Factory Defaults */

#define FD_CNFGR_SIM_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_SIM_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

#ifdef _L7_OS_LINUX_
#define FD_CNFGR_SIM_DEFAULT_TRANSFER_TASK_PRI     L7_TASK_PRIORITY_LEVEL(L7_MEDIUM_TASK_PRIORITY)
#else
#define FD_CNFGR_SIM_DEFAULT_TRANSFER_TASK_PRI     L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#endif

#define FD_CNFGR_SIM_DEFAULT_UTIL_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_SNMP_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_STATS_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_PHY_TASK_PRI          L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_DAPI_TX_TASK_PRI      L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_DAPI_RX_TASK_PRI      L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SIM_DEFAULT_TIMER_TICK_TASK_PRI   L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* End of CONFIGURATOR SIM Component's Factory Defaults */

/* Start of CONFIGURATOR NIM Component's Factory Defaults */

/* Set allowable ranges for interface number assignments based on the
   type of interface */

#define FD_CNFGR_NIM_MIN_PHYS_INTF_NUM       1
#define FD_CNFGR_NIM_MAX_PHYS_INTF_NUM       (L7_MAX_PORT_COUNT)

#define FD_CNFGR_NIM_MIN_CPU_INTF_NUM        (FD_CNFGR_NIM_MAX_PHYS_INTF_NUM + 1)
#define FD_CNFGR_NIM_MAX_CPU_INTF_NUM        (FD_CNFGR_NIM_MIN_CPU_INTF_NUM + L7_MAX_CPU_SLOTS_PER_UNIT-1)

#define FD_CNFGR_NIM_MIN_LAG_INTF_NUM        (FD_CNFGR_NIM_MAX_CPU_INTF_NUM + 1)
#define FD_CNFGR_NIM_MAX_LAG_INTF_NUM        (FD_CNFGR_NIM_MIN_LAG_INTF_NUM + L7_MAX_NUM_LAG_INTF-1)

#define FD_CNFGR_NIM_MIN_VLAN_INTF_NUM     (FD_CNFGR_NIM_MAX_LAG_INTF_NUM + 1 )
#define FD_CNFGR_NIM_MAX_VLAN_INTF_NUM     (FD_CNFGR_NIM_MIN_VLAN_INTF_NUM + L7_MAX_NUM_ROUTER_INTF-1)


#define FD_CNFGR_NIM_MIN_LOGICAL_INTF_NUM    (FD_CNFGR_NIM_MAX_VLAN_INTF_NUM + 1 )
#define FD_CNFGR_NIM_MAX_LOGICAL_INTF_NUM    (FD_CNFGR_NIM_MIN_LOGICAL_INTF_NUM + L7_MAX_NUM_VLAN_INTF-1)


#define FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM  (FD_CNFGR_NIM_MAX_LOGICAL_INTF_NUM + 1)
#define FD_CNFGR_NIM_MAX_SERV_PORT_INTF_NUM  (FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM + SERVICE_PORT_PRESENT-1)

/* End of CONFIGURATOR NIM Component's Factory Defaults */

/* Start of CONFIGURATOR DOT1Q Component's Factory Defaults */

#define FD_CNFGR_DOT1Q_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_DOT1Q_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_DOT1Q_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_DOT1Q_DEFAULT_TIMER_TASK_PRI  L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_DOT1Q_DEFAULT_GARP_TASK_PRI   L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* End of CONFIGURATOR DOT1Q Component's Factory Defaults */

/* Unit Manager constants controlling copy of running config to backup unit. */

/* Minimum number of seconds between copies of running config to backup */
#define L7_UNITMGR_CONFIG_COPY_HOLDDOWN   120 

/* When a change to running config occurs, wait this number of seconds 
 * before copying running config to backup. If there are additional 
 * configuration changes, they'll be included in a single copy. */
#define L7_UNITMGR_CONFIG_COPY_DELAY       30


/* Start of CONFIGURATOR FDB Component's Factory Defaults */

#define FD_CNFGR_FDB_DEFAULT_STACK_SIZE      L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_FDB_DEFAULT_TASK_SLICE      L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_FDB_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define FD_CNFGR_FDB_DEFAULT_FID      0

/* End of CONFIGURATOR FDB Component's Factory Defaults */

/* Start of CONFIGURATOR SNMP Component's Factory Defaults */
#define FD_CNFGR_SNMP_DEFAULT_STACK_SIZE      L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_SNMP_DEFAULT_TASK_SLICE      L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_SNMP_DEFAULT_MSG_SIZE        100

#define FD_CNFGR_SNMP_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define L7_8021Q_DEFAULT_PRIORITY   0
#define L7_VLAN_TAG_DEFAULT_PRIORITY_MASK   0x0000
#define L7_VLAN_TAG_DEFAULT_CFI_MASK        0x0000
/* End of CONFIGURATOR SNMP Component's Factory Defaults */

/* Start of CONFIGURATOR CLI_WEB Component's Factory Defaults */
#define FD_CNFGR_CLI_WEB_DEFAULT_STACK_SIZE      L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_CLI_WEB_DEFAULT_TASK_SLICE      L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_CLI_WEB_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
/* End of CONFIGURATOR CLI_WEB Component's Factory Defaults */

/* Start of CONFIGURATOR Sysnet Factory Defaults */
#define FD_CNFGR_SYSNET_MAX_REGISTRATIONS   15
/* End of CONFIGURATOR Sysnet Factory Defaults */

/* Start of CONFIGURATOR Sysapi Factory Defaults */
#define FD_CNFGR_SYSAPI_MAX_FILE_ENTRIES    SYSAPI_MAX_FILE_ENTRIES
#define FD_CNFGR_SYSAPI_CONFIG_FILENAME     SYSAPI_CONFIG_FILENAME
/* End of CONFIGURATOR Sysapi Factory Defaults */

/* Start of CONFIGURATOR DTL Factory Defaults */

#define FD_CNFGR_DTL_DEFAULT_STACK_SIZE      L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_DTL_DEFAULT_TASK_SLICE      L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_DTL_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* End of CONFIGURATOR DTL Factory Defaults */

/* Start of CONFIGURATOR PBVLAN Factory Defaults */

#define FD_CNFGR_PBVLAN_DEFAULT_STACK_SIZE      L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_PBVLAN_DEFAULT_TASK_SLICE      L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_PBVLAN_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* End of CONFIGURATOR PBVLAN Factory Defaults */

/* Start of CONFIGURATOR IGMP Snooping Factory Defaults */

#define FD_CNFGR_IGMP_SNOOPING_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_IGMP_SNOOPING_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_IGMP_SNOOPING_DEFAULT_TASK_PRI       L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* End of CONFIGURATOR IGMP Snooping Factory Defaults */

/* Start of CONFIGURATOR RADIUS Client Factory Defaults */

#define FD_CNFGR_RADIUS_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_RADIUS_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_RADIUS_DEFAULT_TASK_PRI       L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)


/* End of CONFIGURATOR RADIUS Client Factory Defaults */


/* Start of CONFIGURATOR TACACS+ Client Factory Defaults */

#define FD_CNFGR_TACACS_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_TACACS_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_TACACS_DEFAULT_TASK_PRI       L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* Start of CONFIGURATOR TACACS+ Client Factory Defaults */


/* Start of CONFIGURATOR DOT1X Factory Defaults */

#define FD_CNFGR_DOT1X_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_DOT1X_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

#define FD_CNFGR_DOT1X_DEFAULT_TASK_PRI       L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)



#define max(x, y) (((x) < (y)) ? (y) : (x))

/* End of CONFIGURATOR DOT1X Factory Defaults */

/* Start of CONFIGURATOR SSLT Client Factory Defaults */

#define FD_CNFGR_SSLT_MSG_COUNT 64

/* End of CONFIGURATOR SSLT Client Factory Defaults */

/* Start of CONFIGURATOR SSHD Client Factory Defaults */

#define FD_CNFGR_SSHD_MSG_COUNT 64

/* End of CONFIGURATOR SSHD Client Factory Defaults */

/* Start of User Manager Factory Defaults */

#define FD_MAX_APL_COUNT    10  /* max number of configured APLs */

/* End of User Manager Factory Defaults */

/* Start of POLICY definitions */
#define L7_POLICY_INTF_MAX_COUNT (L7_MAX_PORT_COUNT + 1)
/* End of POLICY definitions */

/* Start of FILTER definitions */
#define L7_FILTER_INTF_MAX_COUNT  (L7_FILTER_MAX_INTF + 1)
/* End of FILTER definitions */


/* Start of SNOOPING definitions */
#define L7_IGMP_SNOOPING_MAX_INTF (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + L7_MAX_NUM_CAPWAP_TUNNEL_INTF + 1)
/* End of SNOOPING definitions */

/* Start of Port MAC Locking definitions */
#define L7_PML_MAX_INTF (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
/* End of Port MAC Locking  definitions */

/* Start of DHCP Filtering definitions */
#define L7_DHCP_FILTERING_MAX_INTF  (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
/* End of DHCP Filtering definitions */

#define L7_DHCP_SNOOP_MAX_BINDINGS L7_MAX_FDB_MAC_ENTRIES
#define L7_IPSG_MAX_STATIC_BINDINGS       1024 /*It should be equallent to HAPI_BROAD_IPSG_TREE_HEAP_SIZE */ 

/* Start of VRRP definitions */
#define L7_VRRP_MAX_VIRT_ROUTERS   20
#define L7_VRRP_MAX_INTERFACES     L7_IPMAP_INTF_MAX_COUNT
#define L7_VRRP_MAX_TRACK_INTF_PER_VR 20
#define L7_VRRP_MAX_TRACK_RT_PER_VR 20                                        
/* End of VRRP definitions */

/* Start of ACL defines */
#define L7_ACL_INTF_MAX_COUNT (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + L7_MAX_NUM_VLAN_INTF + 1)
#define FD_CNFGR_ACL_MAX_CALLBACK_REGISTRATIONS       10
/* End of ACL defines */

/* Start of COS defines */
#define L7_COS_INTF_MAX_COUNT      (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
/* End of COS defines */

/* Start of DiffServ defines */
#define L7_DIFFSERV_INTF_MAX_COUNT (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
#define FD_CNFGR_DIFFSERV_MAX_CALLBACK_REGISTRATIONS  10
/* End of DiffServ defines */

/* Start of 802.1AB definitions */
#define L7_LLDP_INTF_MAX_COUNT            (L7_MAX_PORT_COUNT + 1)  /* physical ports only */
/* End of 802.1AB definitions */

/* Start of DNS client platform defines */
#define L7_DNS_CONCURRENT_REQUESTS        16
#define L7_DNS_SEARCH_LIST_ENTRIES        6 /* this is the default domain list
                                               used when no domain search list
                                               entries are found */
#define L7_DNS_STATIC_HOST_ENTRIES        64
#define L7_DNS_CACHE_ENTRIES              128
#define L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES 32 /* this is number of search list
                                                entries*/

/* End of DNS Client */
/* Start of Async Event Handler defines */
#define L7_ASYNC_EVENT_MAX_HANDLERS         4 /*L7_LAST_COMPONENT_ID*/
#define L7_ASYNC_EVENT_MAX_STORED_DATA      64
/* End of Async Event Handler defines */

/* Start of dot1x defines */
#define L7_DOT1X_INTF_MAX_COUNT (L7_MAX_PORT_COUNT + 1)
/* End of dot1x defines */

/* Start of dot1s defines */
#define L7_DOT1S_MAX_INTERFACE_COUNT           (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
#define L7_DOT1S_DEFAULT_DOT1S_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define L7_DOT1S_DEFAULT_DOT1S_TIMER_TASK_PRI  L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
/* End of dot1s defines */

/* Start of isdp defines */
#define L7_ISDP_DEFAULT_ISDP_TASK_PRI          L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define L7_ISDP_INTF_MAX_COUNT                 (L7_MAX_PORT_COUNT + 1)
#define L7_ISDP_MAX_NEIGHBORS                  (6 * L7_ISDP_INTF_MAX_COUNT)
#define L7_ISDP_MAX_IPADDRESSES                (12 * L7_ISDP_INTF_MAX_COUNT)
/* End of isdp defines */

#define L7_VOICE_VLAN_INTF_MAX_COUNT (L7_MAX_PORT_COUNT + 1)
/* Start of CONFIGURATOR DHCPS Component's Factory Defaults */

#define FD_CNFGR_DHCPS_DEFAULT_STACK_SIZE       L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_DHCPS_DEFAULT_TASK_SLICE       L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_DHCPS_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define DHCPS_PROC_TASK "DHCP Server Processing Task"
#define DHCPS_PROC_QUEUE "DHCP Server Processing Queue"

/* End of DHCPS Server Factory Defaults */

#define DHCP6C_CTL_QUEUE      "DHCPv6 Client Ctl Queue"
#define DHCP6C_CTL_MSG_COUNT  3

#define DHCP_SNOOP_TASK "DHCP snoop"
#define DHCP_SNOOP_EVENT_QUEUE "DHCP snp evnt q"

/* Could get a VLAN change event for all vlans in the system*/
#define DHCP_SNOOP_EVENTQ_MSG_COUNT  L7_MAX_VLANS
#define DHCP_SNOOP_PACKET_QUEUE "DHCP snp pkt q"
#define DHCP_SNOOP_PACKETQ_MSG_COUNT  200

/* Start of Configurator for DOT3AH */
#define DOT3AH_TASK "DOT3AH Task"
#define DOT3AH_EVENT_QUEUE "DOT3AH evnt Q"
#define DOT3AH_PDU_QUEUE "DOT3AH PDU Q"
/* End of Configurator for DOT3AH */

/* Start of CONFIGURATOR Dynamic ARP Inspection Component's Factory Defaults */
#define FD_CNFGR_DAI_DEFAULT_STACK_SIZE       L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_DAI_DEFAULT_TASK_SLICE       L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_DAI_DEFAULT_TASK_PRIORITY    L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define DAI_TASK                    "Dynamic ARP Inspection"
#define DAI_EVENT_QUEUE             "DAI Evnt Q"
/* Could get a VLAN change event for all vlans in the system */
#define DAI_EVENTQ_MSG_COUNT        L7_MAX_VLANS
#define DAI_PACKET_QUEUE            "DAI Pkt Q"
#define DAI_PACKETQ_MSG_COUNT       200

/* End of Dynamic ARP Inspection Factory Defaults */

#define WIO_TASK "CP Wired If"
#define WIO_EVENT_QUEUE "CPWIO Event Q"
#define WIO_QUEUE_MSG_COUNT  (3 * L7_MAX_PORT_COUNT)
#define WIO_FRAME_QUEUE "CPWIO Frame Q"
#define WIO_FRAME_QUEUE_MSG_COUNT  50

/* Start of CONFIGURATOR LOG Server Component's Factory Defaults */
#define FD_CNFGR_LOG_DEFAULT_STACK_SIZE       L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_LOG_DEFAULT_TASK_SLICE       L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_LOG_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define LOG_PROC_TASK "LOG Server Processing Task"
#define LOG_PROC_QUEUE "LOG Server Processing Queue"
/* End of LOG Client Factory Defaults */

/* Start of SNTP Client Factory Defaults */
#define FD_CNFGR_SNTP_DEFAULT_STACK_SIZE       L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_SNTP_DEFAULT_TASK_SLICE       L7_DEFAULT_TASK_SLICE
#define FD_CNFGR_SNTP_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define FD_CNFGR_SNTP_MSG_COUNT                8
/* End of SNTP Factory Defaults */

/* Start of Protected Port Defaults */
#define L7_PROTECTED_PORT_GROUP_NAME_SIZE_MIN     1
#define L7_PROTECTED_PORT_GROUP_NAME_SIZE         L7_MAX_FILENAME + 1
#define FD_PROTECTED_PORT_GROUP_NAME ""
/* End of Protected Port Defaults */

/* Start of Wireless Factory Defaults */
#define FD_CNFGR_WIRELESS_CONFIG_MSG_COUNT 64
#define FD_CNFGR_WIRELESS_WDM_MSG_COUNT    8

#define FD_CNFGR_WIRELESS_AP_CLTQOS_PROCESS_INTERVAL      3     /* seconds */
#define FD_CNFGR_WIRELESS_AP_CLTQOS_CHANGE_LIST_DEPTH     128   /* max simultaneous notify events */

/* End of Wireless Factory Defaults */

/* Start of DOS Control definitions */
#define L7_DOS_MAX_INTF (L7_MAX_PORT_COUNT +  1)
/* End of DOS Control definitions */


/* Start of Time Range definitions */
#define FD_CNFGR_TIMERANGE_DEFAULT_STACK_SIZE        L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_TIMERANGE_DEFAULT_TASK_PRIORITY     L7_DEFAULT_TASK_PRIORITY
#define FD_CNFGR_TIMERANGE_DEFAULT_TASK_SLICE        L7_DEFAULT_TASK_SLICE
#define TIMERANGE_PROC_TASK                          "TimeRange Processing Task"
#define TIMERANGE_PROC_QUEUE                         "TimeRange Processing Queue"
/* End of Time Range definitions */

 /* interval to save dhcp config if dirty.
   0 implies saving is disabled */
#define L7_DHCPS_CFG_SAVE_INTERVAL_SECS    0

/* Start of VOIP definitions */
#define L7_VOIP_MAX_INTF      (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)  
/* End of VOIP   definitions */

/* Start of LLPF definitions */
#define L7_LLPF_MAX_INTF      (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)
/* End of LLPF   definitions */

#endif /* INCLUDE_DEFAULT_CNFGR */

