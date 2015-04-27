/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename commdefs.h
*
* @purpose The purpose of this file is to have a central location for
*          common #defines to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 7/27/2000
*
* @author paulq
* @end
*
**********************************************************************/

#ifndef INCLUDE_COMMDEFS
#define INCLUDE_COMMDEFS

#include "l7_resources.h"
#include "commdefs_pkg.h"


#define L7_USMDB_UNIT_ZERO 0

#define L7_SERIAL_COM_ATTR 0        /* PTin added: serial port */

/*--------------------------------------*/
/*  Commonly Used Constants             */
/*--------------------------------------*/
#define L7_NULL 0
#define L7_NULLPTR  ((void *)0)

#define L7_EOS      ((unsigned char)'\0')   /* ASCIIZ end-of-string character */

#define L7_ENABLE           1
#define L7_DISABLE          0
#define L7_DIAG_DISABLE     2

#define L7_ON   1
#define L7_OFF  0

#define L7_YES  1
#define L7_NO   0

#define L7_OK           0
#define L7_NOT_EXISTS   2
#define L7_NO_WAIT      0
#define L7_WAIT_FOREVER (-1)
#define L7_ALLONES      (-1)
#define L7_MSG_PRIORITY_NORM  0
#define L7_MSG_PRIORITY_HI    1

#define L7_MATCH_EXACT     1
#define L7_MATCH_GETNEXT   2
#define L7_MATCH_FREE      3


#ifndef min
  #define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
  #define max(x, y) (((x) < (y)) ? (y) : (x))
#endif

/* Admin mode */
typedef enum
{
  L7_ADMIN_MODE_DISABLE = L7_DISABLE,
  L7_ADMIN_MODE_ENABLE = L7_ENABLE
} L7_ADMIN_MODE_t;

/* When adding a new component id to this table, make sure to add
** the component below other existing components, and before the
** L7_LAST_COMPONENT_ID entry.  Insertion of components into this
** list will cause binary configuration migration to fail.
*/

typedef enum
{
  L7_FIRST_COMPONENT_ID = 0,       /* range checking */
  L7_SIM_COMPONENT_ID =1,
  L7_NIM_COMPONENT_ID,
  L7_TRAPMGR_COMPONENT_ID,
  L7_RESERVED_3_COMPONENT_ID,
    /* Obsolete component removed, must have placeholder for config migration */
  L7_CNFGR_COMPONENT_ID,
  L7_DRIVER_COMPONENT_ID,
  L7_POLICY_COMPONENT_ID,
  L7_DOT1Q_COMPONENT_ID,
  L7_PBVLAN_COMPONENT_ID,
  L7_DOT3AD_COMPONENT_ID,
  L7_STATSMGR_COMPONENT_ID,

#if L7_FEAT_STACKING_K_COMPAT
  L7_RESERVED_1_COMPONENT_ID,
#endif

    /* Obsolete component removed, must have placeholder for config migration */
  L7_LTS_COMPONENT_ID,
  L7_UNUSED1_COMPONENT_ID,
  L7_USMDB_COMPONENT_ID,
  L7_LAYER3_COMPONENT_ID,
  L7_CUSTOM_COMPONENT_ID,
  L7_FDB_COMPONENT_ID,
  L7_GARP_COMPONENT_ID,
  L7_SNMP_COMPONENT_ID,
  L7_DTL_COMPONENT_ID,
  L7_USER_MGR_COMPONENT_ID,
  L7_CLI_WEB_COMPONENT_ID,
  L7_CMD_LOGGER_COMPONENT_ID,
  L7_IP_MAP_COMPONENT_ID,
  L7_RTR_DISC_COMPONENT_ID,
  L7_OSPF_MAP_COMPONENT_ID,
  L7_RIP_MAP_COMPONENT_ID,
  L7_FLEX_MGMD_MAP_COMPONENT_ID,
  L7_FLEX_MCAST_MAP_COMPONENT_ID,
  L7_FLEX_PIMDM_MAP_COMPONENT_ID,
  L7_FLEX_DVMRP_MAP_COMPONENT_ID,
  L7_FLEX_PIMSM_MAP_COMPONENT_ID,
  L7_VRRP_MAP_COMPONENT_ID,
  L7_FLEX_PACKAGES_ID,
  L7_FLEX_MPLS_MAP_COMPONENT_ID,
  L7_FLEX_BGP_MAP_COMPONENT_ID,
  L7_FLEX_QOS_ACL_COMPONENT_ID,
  L7_FLEX_QOS_COS_COMPONENT_ID,
  L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
  L7_PORT_MIRROR_COMPONENT_ID,
  L7_MFDB_COMPONENT_ID,
  L7_FILTER_COMPONENT_ID,
  L7_DHCP_RELAY_COMPONENT_ID,
  L7_SNOOPING_COMPONENT_ID,
  L7_GMRP_COMPONENT_ID,
  L7_SERVICE_PORT_PRESENT_ID,
  L7_CABLE_TEST_COMPONENT_ID,
  L7_DOT1S_COMPONENT_ID,
  L7_RADIUS_COMPONENT_ID,
  L7_TACACS_COMPONENT_ID,
  L7_DOT1X_COMPONENT_ID,
  L7_DHCPS_MAP_COMPONENT_ID,
  L7_DVLANTAG_COMPONENT_ID,
  L7_IPV6_PROVISIONING_COMPONENT_ID,
  L7_FLEX_OPENSSL_COMPONENT_ID,
  L7_FLEX_SSLT_COMPONENT_ID,
  L7_FLEX_SSHD_COMPONENT_ID,
  L7_FLEX_SSHC_COMPONENT_ID,
  L7_DOT1P_COMPONENT_ID,
  L7_UNITMGR_COMPONENT_ID,
  L7_CARDMGR_COMPONENT_ID,
  L7_FLEX_STACKING_COMPONENT_ID,
  L7_EDB_COMPONENT_ID,
  L7_CDA_COMPONENT_ID,
  L7_FFTP_COMPONENT_ID,
  L7_OSAPI_COMPONENT_ID,
  L7_POE_COMPONENT_ID,
  L7_CONFIG_SCRIPT_COMPONENT_ID,
  L7_LOG_COMPONENT_ID,
  L7_SNTP_COMPONENT_ID,
  L7_TELNET_MAP_COMPONENT_ID,
  L7_PORT_MACLOCK_COMPONENT_ID,
  L7_DHCP_FILTERING_COMPONENT_ID,
  L7_DOSCONTROL_COMPONENT_ID,
L7_SERVICES_COMPONENT_ID,
  L7_ALARMMGR_COMPONENT_ID,
  L7_POWER_UNIT_MGR_COMPONENT_ID,
  L7_FAN_UNIT_MGR_COMPONENT_ID,
  L7_FLEX_OSPFV3_MAP_COMPONENT_ID,
  L7_FLEX_IPV6_MAP_COMPONENT_ID,
  L7_RLIM_COMPONENT_ID,
  L7_FLEX_DHCP6S_MAP_COMPONENT_ID,
  L7_LLDP_COMPONENT_ID,
  L7_VLAN_IPSUBNET_COMPONENT_ID,
  L7_VLAN_MAC_COMPONENT_ID,
 L7_PROTECTED_PORT_COMPONENT_ID,
  L7_CHASSIS_ALARM_COMPONENT_ID,
  L7_DNS_CLIENT_COMPONENT_ID,
  L7_FLEX_WIRELESS_COMPONENT_ID,
  L7_BSP_COMPONENT_ID,
  L7_FLEX_IPV6_MGMT_COMPONENT_ID,
  L7_MFC_COMPONENT_ID,
 L7_VOICE_VLAN_COMPONENT_ID,
 L7_DHCP_SNOOPING_COMPONENT_ID,
 PTIN_PPPOE_COMPONENT_ID,              /* PTin added */
 L7_DAI_COMPONENT_ID,
 L7_FLEX_QOS_VOIP_COMPONENT_ID,
 L7_SFLOW_COMPONENT_ID,
 L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
 L7_ISDP_COMPONENT_ID,
  L7_MGMT_ACAL_COMPONENT_ID,
  L7_BOX_SERVICES_COMPONENT_ID,
  L7_LINK_DEPENDENCY_COMPONENT_ID,
  L7_FLEX_QOS_ISCSI_COMPONENT_ID,
  L7_RMON_COMPONENT_ID,
 L7_FLEX_METRO_DOT1AD_COMPONENT_ID,
 L7_DOT3AH_COMPONENT_ID,
  L7_DOT1AG_COMPONENT_ID,
 L7_TR069_COMPONENT_ID,
 L7_AUTO_INSTALL_COMPONENT_ID,
 L7_DHCP_CLIENT_COMPONENT_ID,
 L7_BOOTP_COMPONENT_ID,
  L7_CP_WIO_COMPONENT_ID,
  L7_DHCP6C_COMPONENT_ID,
  L7_CKPT_COMPONENT_ID,
  L7_ARP_MAP_COMPONENT_ID,
  L7_PFC_COMPONENT_ID,  
 L7_LLPF_COMPONENT_ID,
 L7_DOT1X_AUTH_SERV_COMPONENT_ID,
  L7_TIMERANGES_COMPONENT_ID,

  L7_PTIN_COMPONENT_ID,          /* PTin added: ptin module */

  L7_LAST_COMPONENT_ID,              /* range checking */
  /* This MUST be after the LAST_COMPONENT_ID */
  L7_ALL_COMPONENTS = 0xffff
} L7_COMPONENT_IDS_t;

/* LOOK HERE:  Make sure your mnemonic and name strings are SHORTER than these limits! */
#define L7_COMPONENT_MNEMONIC_MAX_LEN   10      /* includes end-of-string char */
#define L7_COMPONENT_NAME_MAX_LEN       40      /* includes end-of-string char */

/* Maximum interface name length */
#define L7_INTF_NAME_MAX_LEN 32

typedef enum
{
  L7_DOT1Q_TASK_SYNC=1,
  L7_DOT1Q_TIMER_TASK_SYNC,
  L7_PBVLAN_TASK_SYNC,
  L7_IPMAP_TIMER_TASK_SYNC,
  L7_IPMAP_FORWARDING_TASK_SYNC,
  L7_IPMAP_PROCESSING_TASK_SYNC,
  L7_IPMAP_ARP_REISSUE_TASK_SYNC,
  L7_IPMAP_ARP_CALLBACK_TASK_SYNC,
  L7_RTR_DISC_PROCESSING_TASK_SYNC,
  L7_OSPF_TASK_SYNC,
  L7_RIP_TASK_SYNC,
  L7_RIP_PROCESSING_TASK_SYNC,
  L7_PIMDM_TASK_SYNC,
  L7_PIMSM_TASK_SYNC,
  L7_PIMSM_PROTO_TASK_SYNC,
  L7_DVMRP_TASK_SYNC,
  L7_NIM_TASK_SYNC,
  L7_LAG_TASK_SYNC,
  L7_DOT3AD_CORE_TASK_SYNC,
  L7_DOT3AD_TIMER_TASK_SYNC,
  L7_DOT3AD_HELPER_TASK_SYNC,
  L7_SIM_PTS_TASK_SYNC,
  L7_DOT3AD_CORE_TX_TASK_SYNC,
  L7_MPLS_TASK_SYNC,
  L7_VRRP_TASK_SYNC,
  L7_DHCP_RELAY_TASK_SYNC,
  L7_IGMP_TASK_SYNC,
  L7_IGMP_SNOOPING_TASK_SYNC,
  L7_DOT1S_TASK_SYNC,
  L7_DOT1S_TIMER_TASK_SYNC,
  L7_DOT1S_HELPER_TASK_SYNC,
  L7_DOT1S_TRANSPORT_TASK_SYNC,
  L7_RADIUS_TASK_SYNC,
  L7_RADIUS_RX_TASK_SYNC,
  L7_RADIUS_TIMER_TASK_SYNC,
  L7_RADIUS_CLUSTER_TASK_SYNC,
  L7_TACACS_TASK_SYNC,
  L7_TACACS_RX_TASK_SYNC,
  L7_DOT1X_TASK_SYNC,
  L7_DOT1X_TIMER_TASK_SYNC,
  L7_SSLT_TASK_SYNC,
  L7_SSHD_TASK_SYNC,
  L7_MGMD_MAP_TASK_SYNC,
  L7_MCAST_MAP_TASK_SYNC,
  L7_PKT_RCVR_TASK_SYNC,
  L7_DVMRP_MAP_TASK_SYNC,
  L7_PIMDM_MAP_TASK_SYNC,
  L7_PIMSM_MAP_TASK_SYNC,
  L7_DHCPS_TASK_SYNC,
  L7_DHCPS_PROC_TASK_SYNC,
  L7_SNTP_TASK_SYNC,
  L7_SNTP_CFG_TASK_SYNC,
  L7_LOG_CFG_TASK_SYNC,
  L7_TELNET_TASK_SYNC,
  L7_TELNET_PROC_TASK_SYNC,
  L7_PORT_MACLOCK_TASK_SYNC,
  L7_QOS_ACL_LOG_TASK_SYNC,
  L7_QOS_ACL_EVENT_TASK_SYNC,
  L7_QOS_ACL_CLUSTER_TASK_SYNC,
  L7_QOS_DIFFSERV_CLUSTER_TASK_SYNC,
#ifdef L7_OSPFV3
  L7_OSPFV3_TASK_SYNC,
#endif
  L7_IP6MAP_PROCESSING_TASK_SYNC,
  L7_IP6MAP_LOCAL_DATA_TASK_SYNC,
  L7_IP6MAP_EXC_DATA_TASK_SYNC,
  L7_IP6MAP_NDISC_TASK_SYNC,
  L7_IP6MAP_RADVD_TASK_SYNC,
  L7_DHCP6S_TASK_SYNC,
  L7_DHCP6S_PROC_TASK_SYNC,
  L7_WIRELESS_CONFIG_TASK_SYNC,
  L7_WIRELESS_WDM_TASK_SYNC,
  L7_WIRELESS_PEER_TXRX_TASK_SYNC,
  L7_WIRELESS_DISCOVER_TASK_SYNC,
  L7_WIRELESS_RF_SCAN_TASK_SYNC,
  L7_WIRELESS_AP_CONF_TASK_SYNC,
  L7_WIRELESS_AUTH_TASK_SYNC,
  L7_WIRELESS_MUTUAL_AUTH_TASK_SYNC,
  L7_WIRELESS_CLIENTASSOCLB_TASK_SYNC,
  L7_WIRELESS_AP_STATS_TASK_SYNC,
  L7_WIRELESS_AP_TSPEC_TASK_SYNC,
  L7_WIRELESS_AP_UPD_TASK_SYNC,
  L7_WIRELESS_AP_RESET_TASK_SYNC,
  L7_WIRELESS_CONFIG_PUSH_TASK_SYNC,
  L7_WIRELESS_WIDS_TASK_SYNC,
  L7_WIRELESS_CLIENT_THREAT_MITIGATION_TASK_SYNC,
  L7_WIRELESS_DEV_LOC_TASK_SYNC,
  L7_DHCP_FILTERING_TASK_SYNC,
  L7_GARP_TIMER_TASK_SYNC,
  L7_LLDP_TASK_SYNC,
  L7_CHANNEL_POWER_TASK_SYNC,
  L7_DNS_CLIENT_TASK_SYNC,
  L7_DNS_CLIENT_RX_TASK_SYNC,
  L7_OSAPI_TIMER_TASK_SYNC,
  L7_CAPTIVE_PORTAL_TASK_SYNC,
  L7_DOS_TASK_SYNC, 
  L7_VOIP_TASK_SYNC,  
  L7_ISDP_TASK_SYNC,
  L7_VOIP_TIMER_TASK_SYNC,
  L7_POE_TASK_SYNC,  
  L7_SFLOW_TASK_SYNC,
  L7_MGMT_ACAL_TASK_SYNC,
  L7_BOX_SERVICES_TASK_SYNC,
  L7_ISCSI_TASK_SYNC,
  L7_TR069_TASK_SYNC,
  L7_AUTO_INSTALL_TASK_SYNC,
  L7_DOT1AD_TASK_SYNC,
  L7_DOT1AG_TASK_SYNC,
  L7_USL_WORKER_TASK_SYNC,
  L7_USL_CONTROL_TASK_SYNC,
  L7_STACK_MGR_TASK_SYNC,
  L7_WIRELESS_CLIENTASSOCMSG_TASK_SYNC,
  L7_TIMERANGE_TASK_SYNC,
  L7_DHCP_CLIENT_TASK_SYNC,
  L7_PTIN_TASK_SYNC,            /* PTin added: ptin task */
  L7_PTIN_INTF_TASK_SYNC,       /* PTin added: ptin task */
  L7_PTIN_SWITCHOVER_TASK_SYNC, /* PTin added: ptin task */
  L7_PTIN_IGMP_TASK_SYNC,       /* PTin added: ptin task */
  L7_PTIN_PACKET_TASK_SYNC,     /* PTin added: ptin task */
  L7_SSM_RX_TASK_SYNC,          /* PTin added: ssm task  */
  L7_SSM_TX_TASK_SYNC,          /* PTin added: ssm task  */
  L7_BER_TX_SYNC,               /* PTin added: BER task  */
  L7_BER_RX_SYNC,               /* PTin added: BER task  */
  L7_PTIN_ERPS_TASK_SYNC,       /* PTin added: ptin task */
  L7_PTIN_APS_PACKET_TASK_SYNC, /* PTin added: ptin task */
  L7_PTIN_OAM_ETH_TASK_SYNC,    /* PTin added: ptin task */
  L7_PTIN_IPDTL0_TASK_SYNC,     /* PTin added: ptin task */
  L7_PTIN_10MS_TASK_SYNC,       /* PTin added: 10ms task */
  L7_TASK_SYNC_LAST
} L7_TASK_SYNCS_t;

/*********************************************************************
*
* PORT (OR INTERFACE) SECTION
*
*********************************************************************/

/*--------------------------------------*/
/*  Port (or Interface) Events          */
/*--------------------------------------*/

typedef enum
{
  L7_PORT_DISABLE = 0,
  L7_PORT_ENABLE,
  L7_PORT_INSERT,
  L7_DOWN,
  L7_UP,
  L7_INACTIVE,
  L7_ACTIVE,
  L7_FORWARDING,
  L7_NOT_FORWARDING,
  L7_CREATE,
  L7_CREATE_COMPLETE,
  L7_DELETE_PENDING,
  L7_DELETE,
  L7_DELETE_COMPLETE,
  L7_LAG_ACQUIRE,
  L7_LAG_RELEASE,
  L7_SPEED_CHANGE,
  L7_LAG_CFG_CREATE,
  L7_LAG_CFG_MEMBER_CHANGE,
  L7_LAG_CFG_REMOVE,
  L7_LAG_CFG_END,
  L7_PROBE_SETUP,
  L7_PROBE_TEARDOWN,
  L7_SET_INTF_SPEED,                                                      /* Affects Spanning Tree when path cost dynamically calculated */
  L7_SET_MTU_SIZE,                                                        /* Affects IP layers */
  L7_PORT_ROUTING_ENABLED,
  L7_PORT_ROUTING_DISABLED,
  L7_PORT_BRIDGING_ENABLED,
  L7_PORT_BRIDGING_DISABLED,
  L7_VRRP_TO_MASTER,
  L7_VRRP_FROM_MASTER,
  L7_DOT1X_PORT_AUTHORIZED,
  L7_DOT1X_PORT_UNAUTHORIZED,
  L7_ATTACH,
  L7_ATTACH_COMPLETE,
  L7_DETACH,
  L7_DETACH_COMPLETE,
  L7_DOT1X_ACQUIRE,
  L7_DOT1X_RELEASE,
  L7_PORT_STATS_RESET,
  L7_LAST_PORT_EVENT

} L7_PORT_EVENTS_t;


/*--------------------------------------*/
/*  Interface Acquire Actions           */
/*--------------------------------------*/

typedef enum
{
  L7_INTF_ACQ_ACTION_NONE = 0,
  L7_INTF_ACQ_ACTION_ACQUIRE,
  L7_INTF_ACQ_ACTION_RELEASE

} L7_INTF_ACQUIRE_ACTIONS_t;


/*--------------------------------------*/
/*  INTERFACE STATES                    */
/*--------------------------------------*/
typedef enum
{

  L7_INTF_UNINITIALIZED = 0,  /* The interface doesn't exist */
  L7_INTF_CREATING,           /* An L7_CREATE event is in process */
  L7_INTF_CREATED,            /* The L7_CREATE event has been processed by all components */
  L7_INTF_ATTACHING,          /* An L7_ATTACH event is in process */
  L7_INTF_ATTACHED,           /* The L7_ATTACH event has been processed by all components */
  L7_INTF_DETACHING,          /* An L7_DETACH event is in process, next state is L7_INTF_CREATED */
  L7_INTF_DELETING,           /* An L7_DELETE event is in process, next state is L7_INTF_UNINITIALIZED */

} L7_INTF_STATES_t;

/****************************************
*
*  LVL7 Interfaces Types
*
*  If a value is removed from this enum,
*  replace it with L7_UNUSED<N>_INTF to
*  preserve the existing values which are
*  used in config.
*
*****************************************/
typedef enum
{
/* Physical Interfaces */
  L7_PHYSICAL_INTF = 3,
  L7_STACK_INTF,
  L7_CPU_INTF,
  L7_MAX_PHYS_INTF_VALUE,
/* Logical Interfaces */
  L7_LAG_INTF,
  L7_UNUSED1_INTF,
  L7_LOGICAL_VLAN_INTF,
  L7_LOOPBACK_INTF,
  L7_TUNNEL_INTF,
  L7_WIRELESS_INTF,
  L7_CAPWAP_TUNNEL_INTF,
  L7_VLAN_PORT_INTF,      /* PTin added: Virtual ports */
  L7_MAX_INTF_TYPE_VALUE

}L7_INTF_TYPES_t;

/****************************************
*
*  LVL7 Interfaces Parameter Types
*
*****************************************/
typedef enum
{
  L7_INTF_PARM_ADMINSTATE       = 0x1,
  L7_INTF_PARM_SPEED            = 0x2,
  L7_INTF_PARM_AUTONEG          = 0x4,
  L7_INTF_PARM_MTU              = 0x8,
  L7_INTF_PARM_FRAMESIZE        = 0x10,
  L7_INTF_PARM_MACADDR          = 0x20,
  L7_INTF_PARM_LINKTRAP         = 0x40,
  L7_INTF_PARM_LOOPBACKMODE     = 0x80,
  L7_INTF_PARM_MACROPORT        = 0x100,
  L7_INTF_PARM_ENCAPTYPE        = 0x200,
  L7_INTF_PARM_CONNECTOR        = 0x400,
  L7_INTF_PARM_IFDESC           = 0x800,
  L7_INTF_PARM_CP_CAP           = 0x1000,
} L7_INTF_PARM_TYPES_t;

/*--------------------------------------*/
/*  IANAifTypes INTERFACE TYPES         */
/*--------------------------------------*/

/* From IANAifType-MIB */

/* Purposely unused in Stingray are:
        iso88023Csmacd
        ipSwitch
        ipForward
        fibreChannel
        l3ipVlan
        l3ipxVlan
        hyperchannel
*/
typedef enum
{
  L7_IANA_OTHER            = 1,       /* other (unknown)    */
#define IANA_OTHER_DESC             "Unknown Port Type"

  L7_IANA_OTHER_CPU        = 1,       /* other CPU COUNTERS */
#define IANA_CPU_DESC               " "

  L7_IANA_OTHER_SERV_PORT  = 1,       /* other SERVICE PORT */

  L7_IANA_ETHERNET         = 6,       /* ethernetCsmacd     */
#define IANA_ETHERNET_DESC           "ethernet Csmacd"

  L7_IANA_SOFTWARE_LOOPBACK = 24,     /* software loopback  */
#define IANA_SOFTWARE_LOOPBACK_DESC  "software loopback"

  L7_IANA_AAL5             = 49,      /* AAL5 over ATM      */
#define IANA_AAL5_DESC               "AAL5 over ATM"

  L7_IANA_PROP_VIRTUAL     = 53,      /* proprietary virtual/internal */
#define IANA_PROP_VIRTUAL_DESC       "proprietary virtual/internal"

  L7_IANA_FAST_ETHERNET    = 62,      /* 100BaseT           */
#define IANA_FAST_ETHERNET_DESC     "10/100 Copper - Level"

  L7_IANA_FAST_ETHERNET_FX = 69,      /* 100BaseFX          */
#define IANA_FAST_ETHERNET_FX_DESC  "100 Fiber - Level"

  L7_IANA_GIGABIT_ETHERNET = 117,     /* Gigabit Ethernet   */
#define IANA_GIGABIT_ETHERNET_DESC  "Gigabit - Level"

  L7_IANA_TUNNEL = 131,               /* tunnel   */
#define IANA_TUNNEL_DESC            "tunnel"

/* PTin NOTE:
 * the assigned values for the IANA 2.5G and 10G fields DO NOT follow
 * the standard! Those are arbitrary values BUT they do not influence the
 * correct SNMP operation. The IANA standard states that ethernetCsmacd
 * (6) should be used for ALL ethernet interfaces, independ of its speed.
 * This is achieved on the following API (line 6575)
 * src/mgmt/snmp/packages/switching/common/k_mib_fastpathswitching_api.h
 */

/* PTin added: 2.5G */
  L7_IANA_2G5_ETHERNET  = 251,        /* 2.5G Ethernet */
#define IANA_2G5_ETHERNET_DESC        "2.5G - Level"
/* PTin end */

/* PTin added: 40G */
  L7_IANA_40G_ETHERNET  = 252,        /* 40G Ethernet */
#define IANA_40G_ETHERNET_DESC        "40G - Level"
/* PTin end */

/* PTin added: 100G */
  L7_IANA_100G_ETHERNET = 253,        /* 100G Ethernet */
#define IANA_100G_ETHERNET_DESC       "100G - Level"
/* PTin end */

  L7_IANA_10G_ETHERNET  = 198,        /* 10G Ethernet       */
#define IANA_10G_ETHERNET_DESC        "10G - Level"

  L7_IANA_L2_VLAN       = 135,        /* 802.1Q VLAN        */
#define IANA_L2_VLAN_DESC             "802.1Q VLAN"

  L7_IANA_LAG_DESC      = 161,        /* Link Aggregation   */
#define IANA_LAG_DESC                 "Link Aggregate"

  L7_IANA_LOGICAL_DESC  = 162,        /* need to find out this ??? */
#define IANA_LOGICAL_PORT_DESC        "Logical Interface"

  L7_IANA_CAPWAP_TUNNEL = 245,        /* CAPWAP tunnel  value is IANA TBD */
#define IANA_CAPWAP_TUNNEL_DESC       "CAPWAP tunnel"

/* PTin added: virtual ports */
  L7_IANA_VLAN_PORT     = 246         /* VLAN PORT */
#define IANA_VLAN_PORT_DESC           "VLAN_PORT interface"

}L7_IANA_INTF_TYPE_t;

#define IANA_STACK_DESC
#define IANA_VLAN_DESC               "VLAN Routing Interface"

/* LLD_NOTE (trunking): Find IANA for trunking */

/*---------------------------*/
/* Physical Interface Types  */
/*---------------------------*/

typedef enum
{
  L7_ETHERNET = 0,
  L7_FRAME_RELAY,
  L7_ATM
}L7_PHY_INTF_TYPE_t;

/* This constant is to only be used when configuring a command to apply to all interfaces. */
#define L7_ALL_INTERFACES ((L7_uint32)(L7_MAX_INTERFACE_COUNT + 1))

/* The default port speed to be assumed when the speed cannot be successfully
   returned by NIM for reasons such as the link is down, in that case we assume
   a speed of 100 full duplex */
#define L7_PORTCTRL_PORTSPEED_DEFAULT 200

/* Indicates starting Interface No. of 1-based wireless networks. And,
   it should be noted that it is not the min. no. of network interfaces */
#define L7_MIN_NUM_WIRELESS_INTF 1

/*--------------------------------------*/
/*  Port characteristics                */
/*--------------------------------------*/

typedef enum
{
  L7_PORT_NORMAL = 1,
  L7_TRUNK_MEMBER,
  L7_MIRRORED_PORT,
  L7_PROBE_PORT,
  L7_L2TUNNEL_PORT
} L7_SPECIAL_PORT_TYPE_t;

/* Common define for port control settings */

typedef enum
{
  L7_PORTCTRL_PORTSPEED_AUTO_NEG   = 1,
  L7_PORTCTRL_PORTSPEED_HALF_100TX,
  L7_PORTCTRL_PORTSPEED_FULL_100TX,
  L7_PORTCTRL_PORTSPEED_HALF_10T,
  L7_PORTCTRL_PORTSPEED_FULL_10T,
  L7_PORTCTRL_PORTSPEED_FULL_100FX,
  L7_PORTCTRL_PORTSPEED_FULL_1000SX,
  L7_PORTCTRL_PORTSPEED_FULL_10GSX,
  L7_PORTCTRL_PORTSPEED_FULL_40G_KR4,     /* PTin added: Speed 40G */
  L7_PORTCTRL_PORTSPEED_FULL_100G_BKP,    /* PTin added: Speed 100G */
  L7_PORTCTRL_PORTSPEED_AAL5_155,
  L7_PORTCTRL_PORTSPEED_FULL_2P5FX = 128, /* PTin added: Speed 2.5G */
  L7_PORTCTRL_PORTSPEED_LAG,
  L7_PORTCTRL_PORTSPEED_UNKNOWN,
  L7_PORT_CAPABILITY_DUPLEX_HALF = 0x100,
  L7_PORT_CAPABILITY_DUPLEX_FULL = 0x200,
  L7_PORT_CAPABILITY_DUPLEX_AUTO = 0x400,
  L7_PORT_CAPABILITY_SPEED_10    = 0x800,
  L7_PORT_CAPABILITY_SPEED_100   = 0x1000,
  L7_PORT_CAPABILITY_SPEED_1000  = 0x2000,
  L7_PORT_CAPABILITY_SPEED_2500  = 0x10000, /* PTin added: Speed 2.5G */
  L7_PORT_CAPABILITY_SPEED_10G   = 0x4000,
  L7_PORT_CAPABILITY_SPEED_40G   = 0x20000, /* PTin added: Speed 40G */
  L7_PORT_CAPABILITY_SPEED_100G  = 0x40000, /* PTin added: Speed 100G */
  L7_PORT_CAPABILITY_SPEED_AUTO  = 0x8000,
} L7_PORT_SPEEDS_t;


/* bit masks for phy port, speed and auto_neg capabilities */
typedef enum
{
  L7_PHY_CAP_PORTSPEED_AUTO_NEG   = 0x1,
  L7_PHY_CAP_PORTSPEED_HALF_10    = 0x2,
  L7_PHY_CAP_PORTSPEED_FULL_10    = 0x4,
  L7_PHY_CAP_PORTSPEED_HALF_100   = 0x8,
  L7_PHY_CAP_PORTSPEED_FULL_100   = 0x10,
  L7_PHY_CAP_PORTSPEED_HALF_1000  = 0x20,
  L7_PHY_CAP_PORTSPEED_FULL_1000  = 0x40,
  L7_PHY_CAP_PORTSPEED_FULL_2500  = 0x2000, /* PTin added: Speed 2.5G */
  L7_PHY_CAP_PORTSPEED_FULL_10G   = 0x200,
  L7_PHY_CAP_PORTSPEED_FULL_40G   = 0x4000, /* PTin added: Speed 40G */
  L7_PHY_CAP_PORTSPEED_FULL_100G  = 0x8000, /* PTin added: Speed 100G */
  L7_PHY_CAP_PORTSPEED_SFP        = 0x80,
  L7_PHY_CAP_PORTSPEED_SFP_DETECT = 0x400,
  L7_PHY_CAP_POE_PSE              = 0x100,
  L7_PHY_CAP_POE_PD               = 0x800,
  L7_PHY_CAP_INTERNAL             = 0x1000  /* Indicates a blade switch internal port */
} L7_PHY_CAPABILITIES_t;

/* bit masks for auto-negotiation capabilities. */
typedef enum
{
  L7_PORT_NEGO_CAPABILITY_HALF_10   = (1 << 0),
  L7_PORT_NEGO_CAPABILITY_FULL_10   = (1 << 1),
  L7_PORT_NEGO_CAPABILITY_HALF_100  = (1 << 2),
  L7_PORT_NEGO_CAPABILITY_FULL_100  = (1 << 3),
  L7_PORT_NEGO_CAPABILITY_FULL_1000 = (1 << 4),
  L7_PORT_NEGO_CAPABILITY_FULL_2500 = (1 << 5),   /* PTin added: Speed 2.5G */
  L7_PORT_NEGO_CAPABILITY_FULL_10G  = (1 << 6),
  L7_PORT_NEGO_CAPABILITY_FULL_40G  = (1 << 9),   /* PTin added: Speed 40G */
  L7_PORT_NEGO_CAPABILITY_FULL_100G = (1 << 10),  /* PTin added: Speed 100G */
  L7_PORT_NEGO_CAPABILITY_ALL       = (1 << 8),
} L7_PORT_NEGO_CAPABILITIES_t;

typedef enum
{
  L7_CONNECTOR_NONE = 0,
  L7_RJ45,
  L7_MTRJ,
  L7_SCDUP,
  L7_XAUI,
  L7_XLAUI,         /* PTin added: Speed 40G */
  L7_BACKPLANE_KR   /* PTin added: Speed 10G/40G/100G */
} L7_CONNECTOR_TYPES_t;


#define L7_PORT_ENET_ENCAP_MIN_MTU    576
#define L7_PORT_ENET_ENCAP_MAX_MTU   1500
#define L7_PORT_ENET_ENCAP_DEF_MTU   1500
#define L7_PORT_SNAP_ENCAP_MAX_MTU   1492

typedef enum
{
  L7_CABLE_UNTESTED,
  L7_CABLE_TEST_FAIL,   /* Test failed for unknown reason */
  L7_CABLE_NORMAL,      /* Cable is OK */
  L7_CABLE_OPEN,        /* Cable is not connected on one end */
  L7_CABLE_SHORT,       /* Cable is shorted */
  L7_CABLE_OPENSHORT,
  L7_CABLE_CROSSTALK,
  L7_CABLE_NOCABLE

} L7_CABLE_STATUS_t;

typedef enum
{
  PORT_MEDIUM_COPPER = 1,
  PORT_MEDIUM_FIBER,
  PORT_MEDIUM_BACKPLANE /* PTin added: Speed 10G/40G/100G */
} L7_PORT_CABLE_MEDIUM_t;

typedef enum
{
L7_STATUS_UP=1,
L7_STATUS_DOWN=2,
L7_STATUS_TESTING=3
}L7_OPER_STATUS_t;



/* Loopback Test */
typedef enum
{
    L7_PORT_LOOPBACK_NONE = 0,
    L7_PORT_LOOPBACK_MAC,
    L7_PORT_LOOPBACK_PHY,
    L7_PORT_LOOPBACK_COUNT
} L7_PORT_LOOPBACK_t;


typedef enum
{
  L7_RATE_UNIT_NONE = 0,
  L7_RATE_UNIT_PERCENT,                 /* rate is percentage (of link speed) */
  L7_RATE_UNIT_KBPS,                    /* rate is specified in kbps */
  L7_RATE_UNIT_PPS,                     /* rate is specified in pps */
  L7_RATE_UNIT_TOTAL                    /* total number of unit types defined - this must be last */
} L7_RATE_UNIT_t;


#endif /* INCLUDE_COMMDEFS */
