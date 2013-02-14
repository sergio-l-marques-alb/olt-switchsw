/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi.h
*
* @purpose   This files contains the ALL the necessary prototypes,
*            structures, and #defines for the device driver interface.
*
* @component dapi
*
* @comments
*
* @create    5/9/2000
*
* @author    smanders
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_DAPI_H
#define INCLUDE_DAPI_H

#include "l7_common.h"
#include "acl_api.h"
#include "osapi.h"
#include "cos_exports.h"
#include "doscontrol_exports.h"
#include "dot1x_exports.h"
#include "filter_exports.h"
#include "l7_common_l3.h"
#include "defaultconfig.h"
#include "tlv_api.h"
#include "l7_cos_api.h"
#include "cos_exports.h"
#include "voip_exports.h"
#include "filter_exports.h"
#include "doscontrol_exports.h"
#include "dot1x_exports.h"
#include "policy_exports.h"
#include "dot1ad_api.h"
#include "llpf_exports.h"

#include "l7_cnfgr_api.h"

/*********************************************************************
*
* @structures DAPI_USP_t
*
* @purpose    Type used for identifying a port
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_int8 unit;
  L7_int8 slot;
  L7_short16 port;

} DAPI_USP_t;

/*********************************************************************
*
* @structures DAPI_USP_PROBE_t
*
* @purpose    Type used for identifying mirror port & Probe Type
*
* @comments
*
*********************************************************************/
typedef struct
{
  DAPI_USP_t  srcUsp;
  L7_int32    probeType;
} DAPI_USP_PROBE_t;

/*********************************************************************
*
* @enums      DAPI_NEG_ABILITY_t
*
* @purpose    Defines for port modes
*
* @comments
*
*********************************************************************/
typedef enum
{

  DAPI_NEG_10MB_HD   = (1 << 0),
  DAPI_NEG_10MB_FD   = (1 << 1),
  DAPI_NEG_100MB_HD  = (1 << 2),
  DAPI_NEG_100MB_FD  = (1 << 3),
  DAPI_NEG_1000MB_FD = (1 << 4),
  DAPI_NEG_2500MB_FD = (1 << 5),  /* PTin added (2.5) */
  DAPI_NEG_10GB_FD   = (1 << 6)

} DAPI_NEG_ABILITY_t;
/******************************************************************
* @enums      DAPI_MIRROR_DIRECTION_t
*
* @purpose    Defines for Port Mirroring Direction types
*
* @comments   Make sure this type is in sync L7_MIRROR_DIRECTION_t
******************************************************************/

typedef enum
{
  DAPI_MIRROR_BIDIRECTIONAL = 1,
  DAPI_MIRROR_INGRESS,
  DAPI_MIRROR_EGRESS

} DAPI_MIRROR_DIRECTION_t;
/******************************************************************
* @enums      DAPI_SNOOP_L3_NOTIFY_DIRECTION_t
*
* @purpose    Defines the direction ingress/egress vlan routing
*             port
*
* @comments
******************************************************************/

typedef enum
{
  DAPI_SNOOP_L3_NOTIFY_INGRESS = 1,
  DAPI_SNOOP_L3_NOTIFY_EGRESS

} DAPI_SNOOP_L3_NOTIFY_DIRECTION_t;

typedef enum
{
  CARD_CMD_INSERT = 0,
  CARD_CMD_REMOVE,
  CARD_CMD_PLUGIN_START,
  CARD_CMD_PLUGIN_FINISH,
  CARD_CMD_UNPLUG_START,
  CARD_CMD_UNPLUG_FINISH,
  CARD_CMD_LAST
} DAPI_CARD_CMD_t;

/*********************************************************************
*
* @structures DAPI_CARD_CMD_INFO_t
*
* @purpose    Maintains the last command issued by the CMGR 
*             to DAPI for this card.
*
* @comments
*
*********************************************************************/
typedef struct
{
  DAPI_CARD_CMD_t    cardCmd;
  osapiRWLock_t      cardConfigSema;
} DAPI_CARD_CMD_INFO_t;

/*********************************************************************
*
* @enums      DAPI_FAMILY_t
*
* @purpose    Type used to associate a dapi command or callback to a
*             particular protocol or feature.  Callbacks can be registered
*             for each family through the dapiCtl() interface.
*
* @comments
*
*********************************************************************/
typedef enum
{
  DAPI_FAMILY_UNDEFINED = 0,
  DAPI_FAMILY_CONTROL,
  DAPI_FAMILY_CARD,
  DAPI_FAMILY_SYSTEM,
  DAPI_FAMILY_FRAME,
  DAPI_FAMILY_INTF_MGMT,
  DAPI_FAMILY_ADDR_MGMT,
  DAPI_FAMILY_QVLAN_MGMT,
  DAPI_FAMILY_IPSUBNET_VLAN_MGMT,
  DAPI_FAMILY_MAC_VLAN_MGMT,
  DAPI_FAMILY_GARP_MGMT,
  DAPI_FAMILY_LOGICAL_INTF_MGMT,
  DAPI_FAMILY_LAG_MGMT,
  DAPI_FAMILY_ROUTING_MGMT,
  DAPI_FAMILY_ROUTING_ARP_MGMT,
  DAPI_FAMILY_ROUTING_INTF_MGMT,
  DAPI_FAMILY_ROUTING_INTF_MCAST,
  DAPI_FAMILY_QOS_DIFFSERV_MGMT,
  DAPI_FAMILY_QOS_ACL_MGMT,
  DAPI_FAMILY_QOS_COS_MGMT,
  DAPI_FAMILY_SERVICES,
  DAPI_NUM_OF_FAMILIES

} DAPI_FAMILY_t;

/*********************************************************************
*
* @enums    DAPI_CMD_t
*
* @purpose  Type used to associate a command or feature to a void *data pointer
*
* @comments
*
*********************************************************************/
typedef enum
{
  DAPI_CMD_UNDEFINED = 0,

  /* system control */
  DAPI_CMD_CONTROL_CALLBACK_REGISTER,
  DAPI_CMD_CONTROL_DRIVER_START,
  DAPI_CMD_CONTROL_HW_APPLY_NOTIFY,
  DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY,
  

  /* card mgmt */
  DAPI_CMD_CARD_INSERT,
  DAPI_CMD_CARD_REMOVE,
  DAPI_CMD_CARD_PLUGIN,
  DAPI_CMD_CARD_UNPLUG,

  /* system std */
  DAPI_CMD_SYSTEM_MIRRORING,
  DAPI_CMD_SYSTEM_MIRRORING_PORT_MODIFY,
  DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS,
  DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS,
  DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET,
  DAPI_CMD_SYSTEM_FLOW_CONTROL,
  DAPI_CMD_SYSTEM_SNOOP_CONFIG,
  DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE,
  DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE,
  DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD,
  DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE,
  DAPI_CMD_SYSTEM_DOT1X_CONFIG,
  DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG,
  DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY,
  DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED,
  DAPI_CMD_INTF_SAMPLE_RATE,
  DAPI_CMD_INTF_BLINK_SET,

  /* system std - frame handling */
  DAPI_CMD_FRAME_UNSOLICITED_EVENT,
  DAPI_CMD_FRAME_SEND,

  /* L2 std - interface */
  DAPI_CMD_INTF_UNSOLICITED_EVENT,
  DAPI_CMD_INTF_STP_STATE,
  DAPI_CMD_INTF_SPEED_CONFIG,
  DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG,
  DAPI_CMD_INTF_LOOPBACK_CONFIG,
  DAPI_CMD_INTF_ISOLATE_PHY_CONFIG,
  DAPI_CMD_INTF_STATISTICS,
  DAPI_CMD_INTF_FLOW_CONTROL,
  DAPI_CMD_INTF_CABLE_STATUS_GET,
  DAPI_CMD_INTF_FIBER_DIAG_GET,
  DAPI_CMD_INTF_DOT1S_STATE,
  DAPI_CMD_INTF_DOT1X_STATUS,
  DAPI_CMD_INTF_DOT1X_CLIENT_ADD,
  DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE,
  DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT,
  DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK,
  DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK,
  DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG,
  DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG,
  DAPI_CMD_INTF_MAC_LOCK_CONFIG,
  DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG,
  DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG,
  DAPI_CMD_INTF_IPSG_CONFIG,
  DAPI_CMD_INTF_IPSG_STATS_GET,
  DAPI_CMD_INTF_IPSG_CLIENT_ADD,
  DAPI_CMD_INTF_IPSG_CLIENT_DELETE,
  DAPI_CMD_INTF_PFC_CONFIG,
  DAPI_CMD_INTF_PFC_STATS_GET,
  DAPI_CMD_INTF_PFC_STATS_CLEAR,
  /* DOT3AH (EFM_OAM) */
  DAPI_CMD_INTF_DOT3AH_CONFIG_ADD,
  DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE,
  DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG,
  DAPI_CMD_INTF_DOT3AH_REM_LB_ADD,
  DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI,
  DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE,

  DAPI_CMD_INTF_LLDP_CONFIG,
  DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET,
  DAPI_CMD_INTF_DOT1S_BPDU_FILTERING,
  DAPI_CMD_INTF_DOT1S_BPDU_GUARD,
  DAPI_CMD_INTF_DOT1S_BPDU_FLOOD,
  DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG,
  DAPI_CMD_INTF_DOSCONTROL_CONFIG, 

  /* DVLAN  Multi TPID */
   DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG,

  /* DVLAN default TPID */
   DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG,

  /* L2 std - FDB */
  DAPI_CMD_ADDR_UNSOLICITED_EVENT,
  DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD,
  DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY,
  DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE,
  DAPI_CMD_ADDR_MAC_FILTER_ADD,
  DAPI_CMD_ADDR_MAC_FILTER_DELETE,
  DAPI_CMD_ADDR_AGING_TIME,
  DAPI_CMD_ADDR_FLUSH,
  DAPI_CMD_ADDR_FLUSH_VLAN,
  DAPI_CMD_ADDR_FLUSH_MAC,
  DAPI_CMD_ADDR_FLUSH_ALL,
  DAPI_CMD_ADDR_SYNC,
//DAPI_CMD_ADDR_SET_LEARN_MODE,           /* PTin added: MAC learning */

  /* L2 vlan */
  DAPI_CMD_QVLAN_UNSOLICITED_EVENT,
  DAPI_CMD_QVLAN_VLAN_CREATE,
  DAPI_CMD_QVLAN_VLAN_PORT_ADD,
  DAPI_CMD_QVLAN_VLAN_PORT_DELETE,
  DAPI_CMD_QVLAN_VLAN_PURGE,
  DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET,
  DAPI_CMD_QVLAN_PORT_CONFIG,
  DAPI_CMD_QVLAN_PORT_PRIORITY,
  DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP,
  DAPI_CMD_QVLAN_VLAN_STATS_GET,
  DAPI_CMD_QVLAN_PBVLAN_CONFIG,
  DAPI_CMD_QVLAN_MCAST_FLOOD_SET,
  DAPI_CMD_QVLAN_VLAN_LIST_CREATE,
  DAPI_CMD_QVLAN_VLAN_LIST_PURGE,
    
  /* IP Subnet VLANs */
  DAPI_CMD_IPSUBNET_VLAN_CREATE,
  DAPI_CMD_IPSUBNET_VLAN_DELETE,


  /* MAC VLANs */
  DAPI_CMD_MAC_VLAN_CREATE,
  DAPI_CMD_MAC_VLAN_DELETE,


  /* PROTECTED_PORT */
  DAPI_CMD_PROTECTED_PORT_ADD,
  DAPI_CMD_PROTECTED_PORT_DELETE,

  /* L2 GARP */
  DAPI_CMD_GARP_GVRP,
  DAPI_CMD_GARP_GMRP,
  DAPI_CMD_GVRP_GMRP_CONFIG,
  DAPI_CMD_GARP_GROUP_REG_MODIFY,
  DAPI_CMD_GARP_GROUP_REG_DELETE,

  /* Logical Interface */
  DAPI_CMD_LOGICAL_INTF_CREATE,
  DAPI_CMD_LOGICAL_INTF_DELETE,

  /* L2 LAG */
  DAPI_CMD_LAG_CREATE,
  DAPI_CMD_LAG_PORT_ADD,
  DAPI_CMD_LAG_PORT_DELETE,
  DAPI_CMD_LAG_DELETE,
  DAPI_CMD_AD_TRUNK_MODE_SET,
  DAPI_CMD_LAG_HASHMODE_SET,
  DAPI_CMD_LAGS_SYNCHRONIZE,

  /* L3 - Routing and routes */
  DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD,
  DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY,
  DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE,
  DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL,
  DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG,
  DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG,
  DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG,
  DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG,
  DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG,

  /* L3 - ARP */
  DAPI_CMD_ROUTING_ARP_ENTRY_ADD,
  DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY,
  DAPI_CMD_ROUTING_ARP_ENTRY_DELETE,
  DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL,
  DAPI_CMD_ROUTING_ARP_ENTRY_QUERY,
  DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT,

  /* L3 - Interface */
  DAPI_CMD_ROUTING_INTF_ADD,
  DAPI_CMD_ROUTING_INTF_MODIFY,
  DAPI_CMD_ROUTING_INTF_DELETE,
  DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG,
  DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD,
  DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE,
  DAPI_CMD_ROUTING_INTF_VRID_ADD,
  DAPI_CMD_ROUTING_INTF_VRID_DELETE,
  DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET,
  DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY,

  /* L3 - Multicast */
  DAPI_CMD_ROUTING_INTF_MCAST_ADD,
  DAPI_CMD_ROUTING_INTF_MCAST_DELETE,
  DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET,
  DAPI_CMD_ROUTING_INTF_MCAST_USE_GET,
  DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD,
  DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE,
  DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE,
  DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET,
  DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT,

  /* Flex QOS - DiffServ */
  DAPI_CMD_QOS_DIFFSERV_INST_ADD,
  DAPI_CMD_QOS_DIFFSERV_INST_DELETE,
  DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET,
  DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET,

  /* Flex QOS - ACL */
  DAPI_CMD_QOS_ACL_ADD,
  DAPI_CMD_QOS_ACL_DELETE,
  DAPI_CMD_QOS_ACL_RULE_COUNT_GET,
  DAPI_CMD_QOS_SYS_ACL_ADD,
  DAPI_CMD_QOS_SYS_ACL_DELETE,
  DAPI_CMD_QOS_ACL_RULE_STATUS_SET,

  /* Flex QOS - COS */
  DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP,
  DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP,
  DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG,
  DAPI_CMD_QOS_COS_INTF_CONFIG,
  DAPI_CMD_QOS_COS_INTF_STATUS,
  DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG,
  DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG,

  /* QOS - VOIP */
  DAPI_CMD_QOS_VOIP_SESSION,
  DAPI_CMD_QOS_VOIP_PROFILE,
  DAPI_CMD_QOS_VOIP_STATS_GET,

  /* QOS - ISCSI */
  DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD,
  DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE,
  DAPI_CMD_QOS_ISCSI_CONNECTION_ADD,
  DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE,
  DAPI_CMD_QOS_ISCSI_COUNTER_GET,

  /* Services */
  DAPI_CMD_SERVICES_UNSOLICITED_EVENT,

  /* IPv6 - Routing and routes */
  DAPI_CMD_IPV6_ROUTE_ENTRY_ADD,
  DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY,
  DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE,
  DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG,
  DAPI_CMD_IPV6_ROUTE_BOOTP_DHCP_CONFIG,
  DAPI_CMD_IPV6_ROUTE_ICMP_REDIRECTS_CONFIG,
  DAPI_CMD_IPV6_ROUTE_MCAST_FORWARDING_CONFIG,
  DAPI_CMD_IPV6_ROUTE_MCAST_IGMP_CONFIG,

  /* IPv6 - HOSTS */
  DAPI_CMD_IPV6_NEIGH_ENTRY_ADD,
  DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY,
  DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE,
  DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY,
  DAPI_CMD_IPV6_NEIGH_UNSOLICITED_EVENT,

  /* L3 - Interface */
  DAPI_CMD_IPV6_INTF_MCAST_FWD_CONFIG,
  DAPI_CMD_IPV6_INTF_LOCAL_MCASTADD,
  DAPI_CMD_IPV6_INTF_LOCAL_MCASTDELETE,
  DAPI_CMD_IPV6_INTF_ADDR_ADD,
  DAPI_CMD_IPV6_INTF_ADDR_DELETE,
  DAPI_CMD_IPV6_INTF_STATS_GET,

  /* Tunnels */
  DAPI_CMD_TUNNEL_CREATE,
  DAPI_CMD_TUNNEL_DELETE,
  DAPI_CMD_TUNNEL_NEXT_HOP_SET,
  DAPI_CMD_L3_TUNNEL_ADD,
  DAPI_CMD_L3_TUNNEL_DELETE,
  DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD,
  DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE,
  DAPI_CMD_DEBUG_SHELL,

  /* L2- Voice Vlan */
  DAPI_CMD_VOICE_VLAN_DEVICE_ADD,
  DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE,
  DAPI_CMD_VOICE_VLAN_COS_OVERRIDE,
  DAPI_CMD_VOICE_VLAN_AUTH,

  /* L2 Tunnels */
  DAPI_CMD_L2_TUNNEL_CREATE,
  DAPI_CMD_L2_TUNNEL_DELETE,
  DAPI_CMD_L2_TUNNEL_MODIFY,
  DAPI_CMD_L2_TUNNEL_VLAN_SET,
  DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET,

  /*Dot1ad */
  DAPI_CMD_L2_DOT1AD_INTF_TYPE,
  DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION,
  DAPI_CMD_L2_DOT1AD_INTF_STATS,
  DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA,
  /* DOT1AD DataTunneling */
  DAPI_CMD_DOT1AD_ADD,
  DAPI_CMD_DOT1AD_DELETE,
  DAPI_CMD_DOT1AD_RULE_COUNT_GET,

  /* ISDP */
  DAPI_CMD_ISDP_INTF_STATUS_SET,

  /* LLPF */
  DAPI_CMD_INTF_LLPF_CONFIG,

  /* PTin added: DAPI */
  DAPI_CMD_PTIN_INIT,
  DAPI_CMD_PTIN_PORTEXT,
  DAPI_CMD_PTIN_COUNTERS_READ,
  DAPI_CMD_PTIN_COUNTERS_CLEAR,
  DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET,
  DAPI_CMD_PTIN_VLAN_SETTINGS,
  DAPI_CMD_PTIN_VLAN_DEFS,
  DAPI_CMD_PTIN_VLAN_XLATE,
  DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP,
  DAPI_CMD_PTIN_VLAN_CROSSCONNECT,
  DAPI_CMD_PTIN_HW_RESOURCES,
  DAPI_CMD_PTIN_BW_POLICER,
  DAPI_CMD_PTIN_FP_COUNTERS,
  DAPI_CMD_PTIN_PACKET_RATE_LIMIT,
  DAPI_CMD_PTIN_DHCP_PKTS_TRAP_TO_CPU,
  /* PTin end */

  DAPI_NUM_OF_CMDS

} DAPI_CMD_t;

/*********************************************************************
*
* @enums      DAPI_EVENT_t
*
* @purpose    Type used to associate data to an event callback
*
* @comments
*
*********************************************************************/
typedef enum
{
  DAPI_EVENT_COMMAND_COMPLETE,
  DAPI_EVENT_COMMAND_NOT_COMPLETE,

  /* DAPI_FAMILY_FRAME_RX */
  DAPI_EVENT_FRAME_RX,

  /* DAPI_FAMILY_INTF_MGMT */
  DAPI_EVENT_INTF_CREATE,
  DAPI_EVENT_INTF_DESTROY,
  DAPI_EVENT_INTF_LINK_DOWN,
  DAPI_EVENT_INTF_LINK_UP,
  DAPI_EVENT_INTF_DOT1S_STATE_QUERY,
  DAPI_EVENT_INTF_DOT1S_STATE_SET_RESPONSE,

  /* DAPI_FAMILY_ADDR_MGMT */
  DAPI_EVENT_ADDR_LEARNED_ADDRESS,
  DAPI_EVENT_ADDR_AGED_ADDRESS,
  DAPI_EVENT_ADDR_INTF_MAC_QUERY,
  DAPI_EVENT_ADDR_LOCK_UNKNOWN_ADDRESS,
  DAPI_EVENT_ADDR_DOT1X_UNAUTH,
  DAPI_EVENT_ADDR_SYNC_COMPLETE,

  /* DAPI_FAMILY_QVLAN_MGMT */
  DAPI_EVENT_QVLAN_MEMBER_QUERY,

  /* DAPI_FAMILY_ROUTING_ARP_MGMT */
  DAPI_EVENT_ROUTING_ARP_ENTRY_QUERY,
  DAPI_EVENT_ROUTING_ARP_ENTRY_COLLISION,

  /* DAPI_FAMILY_ROUTING_INTF_MCAST */
  DAPI_EVENT_MCAST_ENTRY_COLLISION,
  DAPI_EVENT_MCAST_TABLE_FULL,

  /* Services */
  DAPI_EVENT_SERVICES_VLAN_TAG_QUERY,

 /* ALWAYS LAST ELEMENT */
  DAPI_NUM_OF_EVENTS

} DAPI_EVENT_t;

/*********************************************************************
*
* @enums      DAPI_CMD_GET_SET_t
*
* @purpose    Type used in commands for desired action, get or set data
*
* @comments
*
*********************************************************************/
typedef enum
{
  DAPI_CMD_IGNORE,
  DAPI_CMD_SET,
  DAPI_CMD_GET,
  DAPI_CMD_CLEAR,
  DAPI_CMD_CLEAR_ALL    /* PTin added */

} DAPI_CMD_GET_SET_t;

/*********************************************************************
*
* @typedef    DAPI_CALLBACK_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef L7_RC_t (*DAPI_CALLBACK_t)(DAPI_USP_t    *usp,
                                   DAPI_FAMILY_t  family,
                                   DAPI_CMD_t     cmd,
                                   DAPI_EVENT_t   event,
                                   void          *eventInfoPtr);

/*********************************************************************
*
* @structures DAPI_CONTROL_CALLBACK_CMD_t
*
* @purpose    This command info registers a callback for a family
*
* @comments
*
*********************************************************************/
typedef struct
{
  DAPI_FAMILY_t      family;
  DAPI_CALLBACK_t    funcPtr;

} DAPI_CONTROL_CALLBACK_CMD_t;

/*********************************************************************
*
* @structures DAPI_CONTROL_GENERIC_CMD_t
*
* @purpose    Command use to pass control information to driver
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      L7_LAST_STARTUP_REASON_t startupReason;
      L7_BOOL                  mgrFailover;
      L7_uint32                lastMgrUnitId;
      L7_enetMacAddr_t         lastMgrKey;
    } startup;

    struct
    {
      L7_CNFGR_HW_APPLY_t phase;
    } hwApplyNotification;

    struct
    {
      L7_uint32 unit;
      L7_BOOL   flag; /* L7_FALSE: Unit is no longer in the stack */
    } unitStatus;

  } cmdData;

} DAPI_CONTROL_GENERIC_CMD_t;

/*********************************************************************
*
* @structures DAPI_UNDEFINED_CMD_t
*
* @purpose    Example of basic DAPI command structure.
*
* @comments   Used for debug only.
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  parms;
    } undefined;

  } cmdData;

} DAPI_UNDEFINED_CMD_t;


/*********************************************************************
*
* @structures DAPI_CARD_MGMT_CMD_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  DAPI_CARD_CMD_t cardCmd;

  union
  {
    struct
    {
      L7_uint32 cardId;
    } cardInsert;

    struct
    {
      void *noParms;
    } cardRemove;
  } cmdData;

} DAPI_CARD_MGMT_CMD_t;

typedef struct
{
  DAPI_CMD_GET_SET_t          getOrSet;
  L7_BOOL                     enable;
  L7_DOSCONTROL_TYPE          dosType;
  L7_int32                    arg;
} DOSCONTROLCONFIG_t;

/*********************************************************************
*
* @structures DAPI_SYSTEM_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_uint32                   numMirrors;
      DAPI_USP_PROBE_t            *mirrorProbeUsp;
      DAPI_USP_t                  probeUsp;
      L7_uint32                   probeType;
      L7_BOOL                     probeSpecified;
    } mirroring;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_BOOL                    add;
      DAPI_USP_t                 mirrorUsp;
      L7_uint32                  probeType;
    } mirroringPortModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddress;
    } systemIpAddress;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } l2FlushAll;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;
    } systemMacAddress;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_STORMCONTROL_TYPE        type;
      L7_BOOL                     enable;
      L7_uint32                   threshold;
      L7_RATE_UNIT_t              unit;
    } broadcastControl;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } flowControl;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_uchar8                   family;
      /* PTin added: IGMP snooping */
      #if 1
      L7_uchar8                   CoS;
      L7_uint16                   vlanId;
      #endif
    } snoopConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   instNumber;
    } dot1sInstanceCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   instNumber;
    } dot1sInstanceDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   instNumber;
      L7_ushort16                 vlanId;
    } dot1sInstanceVlanAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   instNumber;
      L7_ushort16                 vlanId;
    } dot1sInstanceVlanRemove;


    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } dot1xConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32			  protoNum;
      L7_uint32			  protoIndex;
    } pbVlanConfig;

    DOSCONTROLCONFIG_t dosControlConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   ingressSamplingRate;
      L7_uint32                   egressSamplingRate;
      L7_uchar8                   sampleCpuPrio;
      L7_uint32                   RandomSeed;
      L7_BOOL                     direction;
    } sFlowConfig;
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     blink;
    } ledConfig;

  } cmdData;

} DAPI_SYSTEM_CMD_t;

/*********************************************************************
*
* @enums      DAPI_INTF_STATS_t
*
* @purpose
*
* @comments   Common interface HW Stats
*
*********************************************************************/
typedef enum
{
  /* Ingress */
  DAPI_STATS_RECEIVED_BYTES,
  DAPI_STATS_RECEIVED_64_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_1519_TO_1530_BYTE_FRAMES,
  DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES,
  DAPI_STATS_RECEIVED_ERROR_OVERSIZED_FRAMES,
  DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES,
  DAPI_STATS_RECEIVED_ERROR_UNDERSIZED_FRAMES,
  DAPI_STATS_RECEIVED_UNICAST_FRAMES,
  DAPI_STATS_RECEIVED_MULTICAST_FRAMES,
  DAPI_STATS_RECEIVED_BROADCAST_FRAMES,
  DAPI_STATS_RECEIVED_ALIGN_ERROR_FRAMES,
  DAPI_STATS_RECEIVED_FCS_ERROR_FRAMES,
  DAPI_STATS_RECEIVED_OVERRUN_FRAMES,
  DAPI_STATS_RECEIVED_FRAME_TOO_LONG,

  DAPI_STATS_RECEIVED_LOCAL_TRAFFIC_DISCARDS,
  DAPI_STATS_RECEIVED_PAUSE_FRAMES,
  DAPI_STATS_RECEIVED_UNACCEPTABLE_FRAME_TYPE_DISCARDS,
  DAPI_STATS_RECEIVED_INGRESS_FILTER_DISCARDS,
  DAPI_STATS_RECEIVED_VLAN_VIABLE_DISCARDS,
  DAPI_STATS_RECEIVED_MULTICAST_TREE_VIABLE_DISCARDS,
  DAPI_STATS_RECEIVED_RESERVED_ADDRESS_DISCARDS,
  DAPI_STATS_RECEIVED_BROADCAST_MULTICAST_STORM_RECOVERY_DISCARDS,
  DAPI_STATS_RECEIVED_CFI_DISCARDS,
  DAPI_STATS_RECEIVED_UPSTREAM_THRESHOLD_DISCARDS,

  DAPI_STATS_RECEIVED_L3_IN,
  DAPI_STATS_RECEIVED_L3_FORW_DATAGRAMS,
  DAPI_STATS_RECEIVED_L3_FRAG_DISCARDS,
  DAPI_STATS_RECEIVED_L3_IN_HDR_ERRORS,
  DAPI_STATS_RECEIVED_L3_ADDR_ERRORS,
  DAPI_STATS_RECEIVED_L3_DISC_ROUTED_FRAMES,
  DAPI_STATS_RECEIVED_L3_MAC_MCAST_DISCARDS,
  DAPI_STATS_RECEIVED_L3_ARP_TO_CPU,
  DAPI_STATS_RECEIVED_L3_IP_TO_CPU,

  /* Egress */
  DAPI_STATS_TRANSMITTED_BYTES,
  DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_1519_TO_1530_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_UNICAST_FRAMES,
  DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES,
  DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES,
  DAPI_STATS_TRANSMITTED_FCS_ERROR_FRAMES,
  DAPI_STATS_TRANSMITTED_OVERSIZED_FRAMES,

  DAPI_STATS_TRANSMITTED_UNDERRUN_ERROR_FRAMES,
  DAPI_STATS_TRANSMITTED_ONE_COLLISION_FRAMES,
  DAPI_STATS_TRANSMITTED_MULTIPLE_COLLISION_FRAMES,
  DAPI_STATS_TRANSMITTED_EXCESSIVE_COLLISION_FRAMES,
  DAPI_STATS_TRANSMITTED_PAUSE_FRAMES,
  DAPI_STATS_TRANSMITTED_PORT_MEMBERSHIP_DISCARDS,
  DAPI_STATS_TRANSMITTED_VLAN_VIABLE_DISCARDS,
  DAPI_STATS_TRANSMITTED_LATE_COLLISIONS,

  DAPI_STATS_TRANSMITTED_AND_RECEIVED_64_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_65_127_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_128_255_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_256_511_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_512_1023_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_1024_1518_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_1519_1522_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_1523_2047_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_2048_4095_BYTE_FRAMES,
  DAPI_STATS_TRANSMITTED_AND_RECEIVED_4096_9216_BYTE_FRAMES,
  DAPI_STATS_ETHER_STATS_DROP_EVENTS,
  DAPI_STATS_SNMPIFOUTDISCARD_FRAMES,
  DAPI_STATS_SNMPIFINDISCARD_FRAMES,

  /* ipv6 stats */
  DAPI_STATS_IPV6_IN_RECEIVES,
  DAPI_STATS_IPV6_IN_HDR_ERRS,
  DAPI_STATS_IPV6_IN_ADDR_ERRS,
  DAPI_STATS_IPV6_IN_DISCARDS,
  DAPI_STATS_IPV6_OUT_FORW_DATAGRAMS,
  DAPI_STATS_IPV6_OUT_DISCARDS,
  DAPI_STATS_IPV6_IN_MCAST_PKTS,
  DAPI_STATS_IPV6_OUT_MCAST_PKTS,


  DAPI_STATS_NUM_OF_INTF_ENTRIES

} DAPI_INTF_STATS_t;

/*********************************************************************
*
* @enums      DAPI_VLAN_STATS_t
*
* @purpose
*
* @comments   Common VLAN Stats
*
*********************************************************************/
typedef enum
{
  DAPI_STATS_VLAN_IN_FRAMES,
  DAPI_STATS_VLAN_IN_DISCARDS,
  DAPI_STATS_VLAN_POLICING_DISCARDS,
  DAPI_STATS_VLAN_THRESHOLD_DISCARDS,
  DAPI_STATS_VLAN_OUT_FRAMES,

  DAPI_STATS_NUM_OF_VLAN_ENTRIES

} DAPI_VLAN_STATS_t;

/*********************************************************************
*
* @enums      DAPI_PFC_STATS_t
*
* @purpose
*
* @comments   Predefined Priority Flow Control stats
*
*********************************************************************/
typedef enum
{
  DAPI_STATS_PFC_COS0_RX,
  DAPI_STATS_PFC_COS1_RX,
  DAPI_STATS_PFC_COS2_RX,
  DAPI_STATS_PFC_COS3_RX,
  DAPI_STATS_PFC_COS4_RX,
  DAPI_STATS_PFC_COS5_RX,
  DAPI_STATS_PFC_COS6_RX,
  DAPI_STATS_PFC_COS7_RX,
  DAPI_STATS_PFC_RX,
  DAPI_STATS_PFC_TX,

  DAPI_STATS_PFC_ENTRIES
} DAPI_PFC_STATS_t;

/*
 * Enumerations for interface based parameters
 */

typedef enum
{
  DAPI_PORT_STP_BLOCKED,
  DAPI_PORT_STP_LISTENING,
  DAPI_PORT_STP_LEARNING,
  DAPI_PORT_STP_FORWARDING,
  DAPI_PORT_STP_ADMIN_DISABLED,
  DAPI_PORT_STP_NOT_PARTICIPATING

} DAPI_PORT_STP_STATE_t;

typedef enum
{
  DAPI_PORT_DOT1S_DISCARDING = 6,
  DAPI_PORT_DOT1S_LEARNING,
  DAPI_PORT_DOT1S_FORWARDING,
  DAPI_PORT_DOT1S_ADMIN_DISABLED,
  DAPI_PORT_DOT1S_NOT_PARTICIPATING

} DAPI_PORT_DOT1S_STATE_t;

typedef enum
{
  DAPI_PORT_DUPLEX_HALF,
  DAPI_PORT_DUPLEX_FULL,
  DAPI_PORT_DUPLEX_UNKNOWN

} DAPI_PORT_DUPLEX_t;

typedef enum
{
  DAPI_PORT_SPEED_FE_10MBPS,
  DAPI_PORT_SPEED_FE_100MBPS,
  DAPI_PORT_SPEED_GE_1GBPS,
  DAPI_PORT_SPEED_GE_2G5BPS,  /* PTin added (2.5G) */
  DAPI_PORT_SPEED_GE_10GBPS,
  DAPI_PORT_AUTO_NEGOTIATE,
  DAPI_PORT_SPEED_UNKNOWN

} DAPI_PORT_SPEED_t;

/* Port loopback modes. */
typedef enum
{
  DAPI_PORT_LOOPBACK_NONE,
  DAPI_PORT_LOOPBACK_MAC,
  DAPI_PORT_LOOPBACK_PHY,
  DAPI_PORT_LOOPBACK_COUNT

} DAPI_PORT_LOOPBACK_t;
typedef enum
{
  DAPI_PORT_CAPTIVE_PORTAL_ENABLED = 0,
  DAPI_PORT_CAPTIVE_PORTAL_DISABLED,
  DAPI_PORT_CAPTIVE_PORTAL_BLOCKED,
  DAPI_PORT_CAPTIVE_PORTAL_UNBLOCKED
} DAPI_PORT_CAPTIVE_PORTAL_STATE_t;

typedef enum
{
  DAPI_CABLE_TEST_FAIL,     /* Test failed for unknown reason    */
  DAPI_CABLE_NORMAL,        /* Cable is OK                       */
  DAPI_CABLE_OPEN,          /* Cable is not connected on one end */
  DAPI_CABLE_SHORT,         /* Cable is shorted                  */
  DAPI_CABLE_OPENSHORT,
  DAPI_CABLE_CROSSTALK

} DAPI_CABLE_STATUS_t;

/*********************************************************************
*
* @structures DAPI_INTF_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_PORT_STP_STATE_t       state;
    } stpState;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_PORT_SPEED_t           speed;
      DAPI_PORT_DUPLEX_t          duplex;
      L7_BOOL                     isLinkUp;
      L7_BOOL                     isSfpLinkUp;
      L7_BOOL                     isTxPauseAgreed; /* L7_TRUE if autonegotiation agreed on Tx pause with link partner - read-only */
      L7_BOOL                     isRxPauseAgreed; /* L7_TRUE if autonegotiation agreed on Rx pause with link partner - read-only */
    } portSpeedConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_NEG_ABILITY_t          abilityMask;
    } autoNegotiateConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_PORT_LOOPBACK_t        loopMode;
    } portLoopbackConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } portIsolatePhyConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  type;
      L7_BOOL                     enable;
      DAPI_INTF_STATS_t           counterId;
      L7_ulong64                 *valuePtr;
    } statistics;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_CABLE_STATUS_t         cable_status;
      L7_BOOL                     length_known;
      L7_uint32                   shortest_length;
      L7_uint32                   longest_length;
      L7_uint32                   cable_failure_length;

    } cableStatusGet;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   instNumber;
      DAPI_PORT_DOT1S_STATE_t     state;
      L7_uint32                   applicationReference;
      L7_RC_t                     rc;
    } dot1sState;

    struct
    {
      L7_uint32                   instNumber;
      L7_BOOL                     canApplyStateChange;
    } dot1sQuery;

    struct
    {
      void                       *noParms;
    } unsolLinkUp;

    struct
    {
      void                       *noParms;
    } unsolLinkDown;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_ulong32                  thresholdHigh;
      L7_ulong32                  thresholdLow;
    } unsolBroadcastRate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_DOT1X_PORT_STATUS_t    status;
      L7_BOOL                     violationCallbackEnabled;
    } dot1xStatus;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;    /* non-zero value will assign this client to this VLAN */
      L7_tlv_t                   *pTLV;      /* non-NULL ptr will apply this policy for this client */
    } dot1xClientAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
    } dot1xClientRemove;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_BOOL                     *timeout;  /* returns true if client has timed out */
    } dot1xClientTimeout;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;
    } dot1xClientBlock;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;
    } dot1xClientUnblock;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_ushort16                 etherType;
      L7_ushort16                 customerId;
      L7_uint32                   dot1adInterfaceType;
    } doubleVlanTagConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   maxFrameSize;
    } frameSizeConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_uint32                   priority_bmp;
    } pfcConfig;

    struct
    {
      L7_BOOL                    getOrSet;
      L7_uint32                  statCounter;
      DAPI_PFC_STATS_t           statType;
    } pfcStatGet;

    struct 
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL enable;
    }flowControl;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     lock;
    } macLockConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   groupId;
      DAPI_USP_t                  *ports;
    } protectedPortConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enabled;
      L7_BOOL                     trusted;
    } dhcpSnoopingConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     trusted;
    } dynamicArpInspectionConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enabled;
    } ipsgEnable;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     dot3ahRemLbEnabled;
      L7_BOOL                     dot3ahEnabled;

#if 0
      L7_enetMacAddr_t             srcMacAddr;
#endif
    } dot3ahEnable;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                  *droppedPackets;
    } ipsgStats;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ip4Addr;
      L7_in6_addr_t               ip6Addr;
      L7_enetMacAddr_t            macAddr;
    } ipsgClientAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ip4Addr;
      L7_in6_addr_t               ip6Addr;
    } ipsgClientDelete;

 
    struct    
    { 
     DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable; 
   } lldpConfig;

   struct                     
   {                          
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_BOOL                     enable;
   } bpduFiltering;

   struct                     
   {                          
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_BOOL                     enable;
   } bpduGuardMode;

   struct                     
   {                          
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_BOOL                     enable;
   } bpduFlood;

   struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;
    } voiceVlanDeviceAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanId;
    } voiceVlanDeviceRemove;

   struct                     
   {                          
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_BOOL                     enable;
   } voiceVlanCosOverride;

   struct                     
   {                          
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_BOOL                     enable;
   } voiceVlanAuth;

   struct
   {
     DAPI_CMD_GET_SET_t          getOrSet;
     L7_int32                    temperature;
     L7_uint32                   voltage;
     L7_uint32                   current;
     L7_uint32                   powerOut;
     L7_uint32                   powerIn;
     L7_BOOL                     txFault;
     L7_BOOL                     los;
   } cableFiberDiag;


   struct
   {
     DAPI_CMD_GET_SET_t                  getOrSet;
     DAPI_PORT_CAPTIVE_PORTAL_STATE_t    cpState;
   } captivePortalPortState;

    DOSCONTROLCONFIG_t dosControlConfig;

    struct
    {
      DAPI_CMD_GET_SET_t getOrSet;
      L7_BOOL                                    enable;
    } isdpStatus;


    /* LLPF */
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_LLPF_BLOCK_TYPE_t        blockType;
    } llpfConfig;

  } cmdData;

} DAPI_INTF_MGMT_CMD_t;

/*
 * Enumerations for address management based parameters
 */

typedef enum
{
  DAPI_ADDR_FLAG_STATIC          = 0,
  DAPI_ADDR_FLAG_LEARNED         = 1,
  DAPI_ADDR_FLAG_MANAGEMENT      = 2,
  DAPI_ADDR_FLAG_SELF            = 3,
  DAPI_ADDR_FLAG_L3_MANAGEMENT   = 4,

} DAPI_ADDR_FLAG_t;

/*********************************************************************
*
* @structures DAPI_ADDR_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ADDR_FLAG_t            flags;
    } macAddressEntryAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ADDR_FLAG_t            flags;
    } macAddressEntryModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ADDR_FLAG_t            flags;
    } macAddressEntryDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      L7_uint32                   numOfSrcPortEntries;
      DAPI_USP_t                 *srcPorts;
      L7_uint32                   numOfDestPortEntries;
      DAPI_USP_t                 *destPorts;
    } macFilterAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
    } macFilterDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_ulong32                  agingTime;
    } agingTime;

//  /* PTin added: MAC learning */
//  struct
//  {
//    DAPI_CMD_GET_SET_t          getOrSet;
//    L7_BOOL                     learn_enabled;
//  } portAddressSetLearnMode;
//  /* PTin end */

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
    } portAddressFlush;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
    } portAddressFlushVlan;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t                 macAddr;
    } portAddressFlushMac;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      DAPI_ADDR_FLAG_t            flags;
      L7_ushort16                 vlanID;
    } unsolLearnedAddress;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      DAPI_ADDR_FLAG_t            flags;
      L7_ushort16                 vlanID;
    } unsolAgedAddress;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
    } unsolIntfMacQuery;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
    } unsolIntfMacUnknownAddr;

  } cmdData;

} DAPI_ADDR_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_QVLAN_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
    } vlanCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_VLAN_MASK_t              vlanMask;
      L7_ushort16                 numVlans;
      L7_VLAN_MASK_t              *vlanMaskFailure;
      L7_uint32                   *vlanFailureCount;
    } vlanCreateList;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_uint32                   numOfMemberSetEntries;
      DAPI_USP_t                 *operationalMemberSet;
      L7_uint32                   numOfTagSetEntries;
      DAPI_USP_t                 *operationalTagSet;
    } vlanPortAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_uint32                   numOfMemberSetEntries;
      DAPI_USP_t                 *operationalMemberSet;
      L7_uint32                   numOfTagSetEntries;
      DAPI_USP_t                 *operationalTagSet;
    } vlanPortDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
    } vlanPurge;

    struct 
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_VLAN_MASK_t              vlanMask;
      L7_ushort16                 numVlans;
    } vlanPurgeList;

    struct
    {
      L7_BOOL                     getOrSet;
      L7_ushort16                 vlanID;
      DAPI_USP_t                 *portUsp;
      L7_ulong64                 *statsCounter;
      DAPI_VLAN_STATS_t           statsType;
    } vlanStatsGet;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_BOOL                    vlanAddCmd;
      L7_VLAN_MASK_t             operationalMemberSet;
      L7_VLAN_MASK_t             operationalTagSet;
    } portVlanListSet;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_BOOL                     vlanIDModify;
      L7_BOOL                     ingressFilteringEnabled;
      L7_BOOL                     ingressFilteringEnabledModify;
      L7_uint32                   acceptFrameType;
      L7_BOOL                     acceptFrameTypeModify;
      L7_ushort16                 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
      L7_BOOL                     protocolModify;
    } portConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  priority;
    } portPriority;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  priority;
      L7_ulong32                  traffic_class;
    } portPriorityToTcMap;

    struct
    {
      L7_uint32                   numOfEntries;
      L7_ushort16                *vlanID;
      L7_BOOL                    *lagTagged;
    } unsolMemberQuery;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   ipSubnet;
      L7_uint32                   netMask;
      L7_uint32                   vlanId;
    } ipSubnetVlanConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_enetMacAddr_t            mac;
      L7_uint32                   vlanId;
    } macVlanConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   vlanId;
      L7_FILTER_VLAN_FILTER_MODE_t  mode;
    } mcastFloodModeSet;
  } cmdData;

} DAPI_QVLAN_MGMT_CMD_t;


/*
 * Enumerations for GARP based parameters
 */
typedef enum
{
  DAPI_GROUP_REG_STATE_FORWARD,
  DAPI_GROUP_REG_STATE_FILTER,
  DAPI_GROUP_REG_STATE_DYNAMIC

} DAPI_GROUP_REG_STATE_t;

/*********************************************************************
*
* @structures DAPI_GARP_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } portGvrp;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } portGmrp;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     gvrp;
      L7_BOOL                     gmrp;
      L7_BOOL                     enable;
    } gvrpGmrpConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uchar8                   grpMacAddr[L7_MAC_ADDR_LEN];
      L7_ushort16                 vlanId;
      L7_uint32                   numOfPorts;
      DAPI_USP_t                 *ports;
      DAPI_GROUP_REG_STATE_t     *stateInfo;
      L7_BOOL                     isStatic;
    } groupRegModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uchar8                   grpMacAddr[L7_MAC_ADDR_LEN];
      L7_ushort16                 vlanId;
      L7_BOOL                     isStatic;
    } groupRegDelete;

  } cmdData;

} DAPI_GARP_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_LOGICAL_INTF_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_IANA_INTF_TYPE_t         type;
    } logicalIntfCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
    } logicalIntfDelete;

  } cmdData;

} DAPI_LOGICAL_INTF_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_TUNNEL_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_TUNNEL_MODE_t            tunnelMode;
      L7_sockaddr_union_t         localAddr;
      L7_sockaddr_union_t         remoteAddr;
      L7_sockaddr_union_t         nextHopAddr;
      DAPI_USP_t                  nextHopUsp;
    } tunnelCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_sockaddr_union_t         nextHopAddr;
      DAPI_USP_t                  nextHopUsp;
    } tunnelNextHopSet;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
    } tunnelDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_TUNNEL_MODE_t            tunnelMode;
      L7_sockaddr_union_t         localAddr;
      L7_sockaddr_union_t         remoteAddr;
      L7_sockaddr_union_t         nextHopAddr;
      DAPI_USP_t                  nextHopUsp;
    } l3TunnelAddDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_TUNNEL_MODE_t            tunnelMode;
      L7_sockaddr_union_t         localAddr;
      L7_sockaddr_union_t         remoteAddr;
      L7_enetMacAddr_t            macAddr;
    } l3TunnelMacAddressAddDelete;

  } cmdData;

} DAPI_TUNNEL_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_L2_TUNNEL_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   tunnelType;
      L7_sockaddr_union_t         *localIpAddr;
      L7_enetMacAddr_t            *localMacAddr;
      L7_sockaddr_union_t         *remoteIpAddr;
      L7_enetMacAddr_t            *remoteMacAddr;
      L7_ushort16                 vlanID;
      L7_sockaddr_union_t         *nextHopIpAddr;
      DAPI_USP_t                  nextHopUsp;
    } l2TunnelCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   tunnelType;
      L7_sockaddr_union_t         *localIpAddr;
      L7_sockaddr_union_t         *remoteIpAddr;
      L7_enetMacAddr_t            *remoteMacAddr;
      L7_ushort16                 vlanID;
    } l2TunnelDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_BOOL                     addflag;
    } l2TunnelVlanSet;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 udpSrcPort;
      L7_ushort16                 udpDstPort;
    } l2TunnelUdpPortsSet;

  } cmdData;

} DAPI_L2_TUNNEL_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_LAG_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
    } lagCreate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   numOfMembers;
      L7_uint32                   hashMode;
      L7_uint32                   maxFrameSize;
      DAPI_USP_t                 *memberSet;
    } lagPortAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   numOfMembers;
      L7_uint32                   hashMode;
      L7_uint32                   maxFrameSize;
      DAPI_USP_t                 *memberSet;
    } lagPortDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
    } lagDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } portADTrunk;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   hashMode;
    } lagHashMode;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_int32                    timeout;
    } lagsSync;

  } cmdData;

} DAPI_LAG_MGMT_CMD_t;

/*
 * Enumerations for Routing Interface parameters
 */
typedef enum
{
  DAPI_ROUTING_INTF_ENCAPS_TYPE_SNAP,
  DAPI_ROUTING_INTF_ENCAPS_TYPE_ETHERNET

} DAPI_ROUTING_INTF_ENCAPS_TYPE_t;

typedef enum
{
  DAPI_ROUTING_ROUTE_DEFAULT      = 1 << 0,
  DAPI_ROUTING_ROUTE_DISCARD      = 1 << 1

} DAPI_ROUTING_ROUTE_FLAG_t;

typedef enum
{
  DAPI_ROUTING_ARP_UNKNOWN        = 0,
  DAPI_ROUTING_ARP_LOCAL          = 1 << 0,
  DAPI_ROUTING_ARP_DEST_KNOWN     = 1 << 1,
  DAPI_ROUTING_ARP_NET_DIR_BCAST  = 1 << 2,
  DAPI_ROUTING_ARP_UNNUMBERED     = 1 << 3

} DAPI_ROUTING_ARP_FLAG_t;

/*
** Defines flags returned for host entry query.
*/
typedef enum
{
  DAPI_ROUTING_ARP_FLAGS_ZERO     = 0,       /* no flags set */
  DAPI_ROUTING_ARP_ENTRY_IN_SW    = 1 << 0,  /* entry is in software table */
  DAPI_ROUTING_ARP_ENTRY_IN_HW    = 1 << 1,  /* entry is in hardware table */
  DAPI_ROUTING_ARP_ENTRY_ERR_SW   = 1 << 2,  /* error accessing sw table */
  DAPI_ROUTING_ARP_ENTRY_ERR_HW   = 1 << 3,  /* error accessing hw table */
  DAPI_ROUTING_ARP_HIT_SOURCE     = 1 << 4,  /* hardware indicates src hit */
  DAPI_ROUTING_ARP_HIT_DEST       = 1 << 5,  /* hardware indicates dst hit */
} DAPI_ROUTING_ARP_QUERY_FLAG_t;
#define DAPI_ROUTING_ARP_HIT_ANY (DAPI_ROUTING_ARP_HIT_SOURCE|DAPI_ROUTING_ARP_HIT_DEST)

/*
 * Defines a route destination for ECMP. The ipAddr (ip6Addr)
 * parameter is the gateway IP (IPv6) Address.
 */
typedef struct
{
  L7_BOOL                         valid;
  L7_ulong32                      ipAddr;
  L7_in6_addr_t                   ip6Addr;
  DAPI_USP_t                      usp;

} DAPI_ROUTING_ROUTE_DESTINATION_t;

typedef struct
{
  DAPI_ROUTING_ROUTE_DESTINATION_t  equalCostRoute[L7_RT_MAX_EQUAL_COST_ROUTES];

} DAPI_ROUTING_ROUTE_ENTRY_t;

/*
 * Enumerates the methods used for Reverse Path Forwarding checks.
 */
typedef enum
{
  DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH = 0,  /* compare packet incoming interface with IIF in mcast route entry          */
  DAPI_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE   /* look up source IP of received frame in the unicast routing table for IIF */

} DAPI_MCAST_RPF_CHECK_METHOD_t;

/*
 * Enumerates actions taken on L3 multicast Reverse Path Forwarding check failures.
 */
typedef enum
{
  DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP = 0,
  DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU

} DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t;

/*********************************************************************
*
* @structures DAPI_ROUTING_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
      L7_in6_addr_t               ip6Addr;
      L7_in6_addr_t               ip6Mask;
      DAPI_ROUTING_ROUTE_ENTRY_t  route;
      DAPI_ROUTING_ROUTE_FLAG_t   flags;
      L7_ulong32                  count;
    } routeAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
      L7_in6_addr_t               ip6Addr;
      L7_in6_addr_t               ip6Mask;
      DAPI_ROUTING_ROUTE_ENTRY_t  route;
      DAPI_ROUTING_ROUTE_FLAG_t   flags;
      L7_ulong32                  count;
    } routeModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
      L7_in6_addr_t               ip6Addr;
      L7_in6_addr_t               ip6Mask;
      L7_ulong32                  count;
    } routeDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } forwardingConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_ulong32                  bootpDhcpRelayServer;
    } bootpDhcpConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } icmpRedirectsConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_uint32                   family;
    } mcastforwardConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
      L7_uint32                   family;
    } mcastIgmpConfig;

  } cmdData;

} DAPI_ROUTING_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_ROUTING_INTF_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_BOOL                     broadcastCapable;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
      L7_enetMacAddr_t            macAddr;
      DAPI_ROUTING_INTF_ENCAPS_TYPE_t encapsType;
      L7_ulong32                  mtu;
    } rtrIntfAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_BOOL                     broadcastCapable;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
      L7_enetMacAddr_t            macAddr;
      DAPI_ROUTING_INTF_ENCAPS_TYPE_t encapsType;
      L7_ulong32                  mtu;
    } rtrIntfModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vlanID;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
    } rtrIntfDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_in6_addr_t               ip6Addr;
      L7_in6_addr_t               ip6Mask;
      L7_ulong32                  ipAddr;
      L7_ulong32                  ipMask;
    } rtrIntfIpAddrAddDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      union
      {
        L7_ipv6InterfaceStats_t     ipv6;
        /*
         * If this gets used for other L3 interface
         * stats, add them here.
         */
      } stats;
    } rtrL3IntfStatsGet;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } mcastIntfFwdConfig;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
    } localMcastAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
    } localMcastDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } inboundAccessCheck;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_BOOL                     enable;
    } outboundAccessCheck;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vrID;
      L7_uint32                   ipAddr;
    } rtrIntfVRIDAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ushort16                 vrID;
    } rtrIntfVRIDDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t  rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
      DAPI_USP_t                 *outUspList;
      L7_ushort16                 outUspCount;
      L7_BOOL                     outGoingIntfPresent;
      DAPI_USP_t                  ingressPhysicalPort;
    } mcastAdd;

    struct
    {
      DAPI_CMD_GET_SET_t                getOrSet;
      DAPI_SNOOP_L3_NOTIFY_DIRECTION_t  outIntfDirection;
      L7_uchar8                         mcastMacAddr[L7_MAC_ADDR_LEN];
      DAPI_USP_t                       *outUspList;
      L7_ushort16                       outUspCount;
      L7_BOOL                           snoopVlanOperState;
    } mcastModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t  rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
    } mcastDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
      L7_uint32                   packetCount;
      L7_uint32                   byteCount;
      L7_uint32                   wrongIntfCount;          /* number of packets dropped due to RPF fail using this entry */
      L7_BOOL                     packetCountSupported;    /* flag telling whether driver supports the corresponding counter */
      L7_BOOL                     byteCountSupported;
      L7_BOOL                     wrongIntfCountSupported;
    } mcastCountQuery;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
      L7_BOOL                     entryUsed;
    } mcastUseQuery;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     vlanTagging;
      DAPI_USP_t                  outPortUsp;
    } mcastPortAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     vlanTagging;
      DAPI_USP_t                  outPortUsp;
    } mcastPortDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      DAPI_USP_t                  outPortUsp;
      L7_BOOL                     bIsMember;
      L7_BOOL                     vlanTagging;
    } mcastVlanPortMemberUpdate;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ttlVal;
    } ttlMcastVal;

  } cmdData;

} DAPI_ROUTING_INTF_MGMT_CMD_t;


/*********************************************************************
*
* @structures DAPI_ROUTING_ARP_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_in6_addr_t               ip6Addr;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ROUTING_ARP_FLAG_t     flags;
    } arpAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_in6_addr_t               ip6Addr;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ROUTING_ARP_FLAG_t     flags;
    } arpModify;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_ulong32                  ipAddr;
      L7_in6_addr_t               ip6Addr;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ROUTING_ARP_FLAG_t     flags;
    } arpDelete;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet; /* input - get or set data (in) */
      L7_uint32                   numOfEntries; /* number of entries to access (in) */
      L7_ulong32                  ipAddr[L7_IP_ARP_MAX_QUERY]; /* IPv4 addr (in) */
      L7_in6_addr_t               ip6Addr[L7_IP_ARP_MAX_QUERY]; /* IPv6 addr (in) */
      L7_uint32                   lastHitDst[L7_IP_ARP_MAX_QUERY]; /* Last dest hit time (out) */
      L7_uint32                   lastHitSrc[L7_IP_ARP_MAX_QUERY]; /* Last source hit time (out) */
      L7_ushort16                 vlanID[L7_IP_ARP_MAX_QUERY]; /* VLAN ID (ignored?) */
      DAPI_ROUTING_ARP_QUERY_FLAG_t arpQueryFlags[L7_IP_ARP_MAX_QUERY]; /* Entry flags (out) */
    } arpQuery;

    struct
    {
      L7_ulong32                  ipAddr;
      L7_in6_addr_t               ip6Addr;
      L7_enetMacAddr_t            macAddr;
      L7_ushort16                 vlanID;
      DAPI_ROUTING_ARP_FLAG_t     flags;
    } unsolArpResolve;

  } cmdData;

} DAPI_ROUTING_ARP_CMD_t;


/*
 * Enumerate frame types from application
 */
typedef enum
{
  DAPI_FRAME_TYPE_DATA_TO_PORT,
  DAPI_FRAME_TYPE_MCAST_DOMAIN,
  DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT,
  DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN,

  DAPI_NUM_OF_FRAME_TYPES

} DAPI_FRAME_TYPE_t;

/*********************************************************************
*
* @structures DAPI_FRAME_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_netBufHandle             frameHdl;
      DAPI_FRAME_TYPE_t           type;
      L7_ushort16                 vlanID;
      L7_uint32                   priority;
    } send;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_netBufHandle             frameHdl;
      DAPI_FRAME_TYPE_t           type;
      L7_ushort16                 vlanID;
      L7_uint32                   priority;
      L7_ushort16                 innerVlanId;
      L7_uint32                   innerVlanPriority;
    } receive;

  } cmdData;

} DAPI_FRAME_CMD_t;


/*
 * Enumerate QoS parameter values
 */
typedef enum
{
  DAPI_QOS_INTF_DIR_IN,
  DAPI_QOS_INTF_DIR_OUT

} DAPI_QOS_INTF_DIR_t;

typedef enum
{
  DAPI_QOS_DIFFSERV_STAT_IN_OFFERED_OCTETS,
  DAPI_QOS_DIFFSERV_STAT_IN_OFFERED_PACKETS,
  DAPI_QOS_DIFFSERV_STAT_IN_DISCARDED_OCTETS,
  DAPI_QOS_DIFFSERV_STAT_IN_DISCARDED_PACKETS

} DAPI_QOS_DIFFSERV_INTF_STAT_IN_t;

typedef enum
{
  DAPI_QOS_DIFFSERV_STAT_OUT_OFFERED_OCTETS,
  DAPI_QOS_DIFFSERV_STAT_OUT_OFFERED_PACKETS,
  DAPI_QOS_DIFFSERV_STAT_OUT_DISCARDED_OCTETS,
  DAPI_QOS_DIFFSERV_STAT_OUT_DISCARDED_PACKETS,

} DAPI_QOS_DIFFSERV_INTF_STAT_OUT_t;

struct DAPI_QOS_TLV_s
{
  L7_ushort16                  type;
  L7_ushort16                  length;
  /*
   * The 'length' field is followed by a 'value' field, which is as
   * many bytes long as is specified in 'length'
   */

} ;

/*
 * Define COS mapping table structures to use when configuring whole table
 * at once
 */
typedef struct
{
  /* NOTE:  The dot1p mapping table is used for all modes, but contains the
   *        mapped port default priority value for all entries when in any
   *        mode other than 'trust dot1p'.  This table is used in addition
   *        to the IP precedence or DSCP tables in their respective trust
   *        modes to cover the non-IP packet cases.  It is the only table used
   *        in 'untrusted' mode to direct packets to the port default priority
   *        traffic class (i.e. queue).
   */
  L7_uchar8     dot1p_traffic_class[L7_DOT1P_MAX_PRIORITY+1];     /* used for all modes (see note)    */
  L7_uchar8     ip_prec_traffic_class[L7_QOS_COS_MAP_NUM_IPPREC]; /* used for trust IP Prec mode only */
  L7_uchar8     ip_dscp_traffic_class[L7_QOS_COS_MAP_NUM_IPDSCP]; /* used for trust IP DSCP mode only */

} DAPI_QOS_COS_MAP_TABLE_t;

typedef enum
{
  DAPI_QOS_COS_INTF_MODE_UNTRUSTED = 1,     /* do not trust any pkt markings */
  DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P,       /* trust pkt Dot1p (802.1p) value */
  DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC,      /* trust pkt IP Precedence value */
  DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP,       /* trust pkt IP DSCP value */
#if defined(FEAT_METRO_CPE_V1_0)
  DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_IPDSCP, /*  unset trust pkt IP DSCP value */
  DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_DOT1P   /*  unset trust pkt DOT1P value */
#endif

} DAPI_QOS_COS_INTF_MODE_t;

typedef enum
{
  DAPI_QOS_COS_QUEUE_SCHED_TYPE_STRICT = 1, /* strict priority */
  DAPI_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED    /* WRR/WFQ */

} DAPI_QOS_COS_QUEUE_SCHED_TYPE_t;

typedef enum
{
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_NOT_SUPPORTED = 0, /* (see struct def.) */
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP,    /* tail drop */
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_WRED,        /* weighted RED */
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED    /* don't change in this call */

} DAPI_QOS_COS_QUEUE_MGMT_TYPE_t;

typedef struct
{
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_t  dropType;
  L7_uchar8                       minThreshold[L7_MAX_CFG_DROP_PREC_LEVELS+1];
  /* Also queue size for tail-drop */
  L7_uchar8                       maxThreshold[L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8                       dropProb[L7_MAX_CFG_DROP_PREC_LEVELS+1];

} DAPI_QOS_COS_DROP_PARMS_t;

/*********************************************************************
*
* @structures DAPI_QOS_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    /***** DiffServ *****/

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      DAPI_QOS_INTF_DIR_t      ifDirection;
      L7_tlv_t                *pTLV;
      L7_uint32                tlvTotalSize;
    } diffServInstAdd;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      DAPI_QOS_INTF_DIR_t      ifDirection;
      L7_tlv_t                *pTLV;
      L7_uint32                tlvTotalSize;
    } diffServInstDelete;

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      L7_uint32                          instanceKey;
      DAPI_QOS_DIFFSERV_INTF_STAT_IN_t   statistic;
      L7_ulong64                        *value;
    } diffServIntfStatInGet;

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      L7_uint32                          instanceKey;
      DAPI_QOS_DIFFSERV_INTF_STAT_OUT_t  statistic;
      L7_ulong64                        *value;
    } diffServIntfStatOutGet;

    /***** ACL *****/

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      DAPI_QOS_INTF_DIR_t      ifDirection;
      L7_tlv_t                *pTLV;
      L7_uint32                tlvTotalSize;
    } aclInstAdd;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      DAPI_QOS_INTF_DIR_t      ifDirection;
      L7_tlv_t                *pTLV;
      L7_uint32                tlvTotalSize;
    } aclInstDelete;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_uint32                correlator;
      L7_ulong64              *hitCount;
    } aclRuleCountGet;
   
    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_uint32                correlator;
      L7_ushort16              status;
    } aclRuleStatusSet;

    /***** VOIP ****/
    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_BOOL                    val;
      L7_BOOL                    protocol[L7_QOS_VOIP_FEATURE_ID_TOTAL];
      L7_uint32                  guarentedBw;
    } voipProfile;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_BOOL                  val;
      L7_ushort16              srcL4Port;
      L7_ushort16              dstL4Port;
      L7_uint32                protoType;
      L7_uint32                srcIpAddr;
      L7_uint32                dstIpAddr;
    } voipSession;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_uint32                srcIpAddr;
      L7_uint32                dstIpAddr;
      L7_ushort16              srcL4Port;
      L7_ushort16              dstL4Port;
      L7_ulong64               *pHitcount;
    } voipStats;

    /***** ISCSI ****/

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_ushort16                tcpPortNumber;
      L7_uint32                  ipAddress;
      L7_uint32                  traffic_class;
      L7_BOOL                    taggingEnabled;
      L7_QOS_COS_MAP_INTF_MODE_t tagMode;
      L7_uint32                  tagData;
    } iscsiTargetPortAdd;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_ushort16                tcpPortNumber;
      L7_uint32                  ipAddress;
    } iscsiTargetPortDelete;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_uint32                  identifier;
      L7_uint32                  targetIpAddress;
      L7_ushort16                targetTcpPort;
      L7_uint32                  initiatorIpAddress;
      L7_ushort16                initiatorTcpPort;
      L7_uint32                  counterIdentifier;
      L7_uint32                  traffic_class;
      L7_BOOL                    taggingEnabled;
      L7_QOS_COS_MAP_INTF_MODE_t tagMode;
      L7_uint32                  tagData;
    } iscsiConnectionAdd;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_uint32                  identifier;
    } iscsiConnectionRemove;

    struct
    {
      DAPI_CMD_GET_SET_t         getOrSet;
      L7_uint32                  identifier;
      L7_ulong64                 *counter_value;
    } iscsiConnectionCounterGet;

    /***** COS *****/

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      L7_ulong32                         precedence;
      L7_ulong32                         traffic_class;
    } ipPrecedenceToTcMap;

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      L7_ulong32                         dscp;
      L7_ulong32                         traffic_class;
    } ipDscpToTcMap;

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      DAPI_QOS_COS_INTF_MODE_t           mode;
      DAPI_QOS_COS_MAP_TABLE_t           mapTable;
    } intfTrustModeConfig;

    struct
    {
      DAPI_CMD_GET_SET_t              getOrSet;
      L7_ulong32                      intfShapingRate;      /* 0 means no shaping, configured rate */
      DAPI_QOS_COS_QUEUE_MGMT_TYPE_t  queueMgmtTypePerIntf; /* 'not supported' means device uses per-queue mgmt */
      L7_ulong32                      wredDecayExponent;
    } intfConfig;

    struct
    {
      DAPI_CMD_GET_SET_t              getOrSet;
      L7_ulong32                      intfShapingRate;         /* 0 means no shaping, chip's effective rate (kbps) */
      L7_ulong32                      intfShapingBurstSize;    /* 0 means no shaping, chip's effective burst size (kbits) */
    } intfStatus;

    struct
    {
      DAPI_CMD_GET_SET_t              getOrSet;
      L7_ulong32                      minBandwidth[L7_MAX_CFG_QUEUES_PER_PORT];   /* 0% means best-effort (no guarantees) */
      L7_ulong32                      maxBandwidth[L7_MAX_CFG_QUEUES_PER_PORT];   /* 0% means unlimited (no shaping) */
      DAPI_QOS_COS_QUEUE_SCHED_TYPE_t schedulerType[L7_MAX_CFG_QUEUES_PER_PORT];
    } queueSchedConfig;

    struct
    {
      DAPI_CMD_GET_SET_t            getOrSet;
      DAPI_QOS_COS_DROP_PARMS_t     parms[L7_MAX_CFG_QUEUES_PER_PORT];
    } queueDropConfig;

  } cmdData;

} DAPI_QOS_CMD_t;

/*********************************************************************
*
* @structures DAPI_DOT1AD_MGMT_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t getOrSet;
      DOT1AD_INTFERFACE_TYPE_t  intfType;
    } dot1adIntfType;
struct
    {
      DAPI_CMD_GET_SET_t getOrSet;
      L7_BOOL  unconfig;
      L7_ushort16 vlanId;
      L7_uchar8  protocolMac[L7_ENET_MAC_ADDR_LEN];
      L7_uint32  protocolId;
      DOT1AD_TUNNEL_ACTION_t  tunnelAction;
    } dot1adtunnelAction;

struct
    {
      DAPI_CMD_GET_SET_t getOrSet;
      dot1adDebugIntfStats_t *stats;
    } dot1adDebugIntfStats;

struct
    {
      DAPI_CMD_GET_SET_t getOrSet;
      dot1adIntfCfg_t dot1adIntfCfg;
    } dot1adApplyIntfConfigData;

  } cmdData;

} DAPI_DOT1AD_MGMT_CMD_t;

/*********************************************************************
*
* @structures DAPI_DOT1AD_CMD_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_ulong32 appHandle;

  union
  {
    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_tlv_t                *pTLV;
      L7_uint32                tlvTotalSize;
      DAPI_USP_t               evtUsp;
    } dot1adInstAdd;

    struct
    {
      DAPI_CMD_GET_SET_t            getOrSet;
      L7_ushort16                   svid;
      L7_ushort16                   cvid;
      L7_ushort16                   isolatedvid;
      dot1adTlvNniInterfaceList_t   *nniList;
    } dot1adInstDelete;

    struct
    {
      DAPI_CMD_GET_SET_t       getOrSet;
      L7_uint32                correlator;
      L7_ulong64              *hitCount;
    } dot1adRuleCountGet;
  } cmdData;

} DAPI_DOT1AD_CMD_t;


/****************************************************************************************
*
* @purpose  Control entry point to DAPI
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command. For
*                               interface directed commands, this should be the port to
*                               act on. For non-port specific commands, this should be
*                               0.0.0.
* @param    cmd     @b{(input)} command to execute
* @param   *data    @b{(input)} data for the control function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_SUPPORTED
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiCtl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data);

/*********************************************************************
*
* @purpose Take the card removal write lock.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void dapiCardRemovalWriteLockTake (void);

/*********************************************************************
*
* @purpose Take the card removal read lock.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void dapiCardRemovalReadLockTake (void);

/*********************************************************************
*
* @purpose Give up the card removal write lock.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void dapiCardRemovalWriteLockGive (void);

/*********************************************************************
*
* @purpose Give up the card removal read lock.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void dapiCardRemovalReadLockGive (void);

/****************************************************************************************
*
* @purpose Initializes the DAPI and HAPI layer. May also initialize the device driver.
*
* @param    cardId    @b{(input)}  Identification number of the board
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiInit(L7_ulong32 cardId);

/*********************************************************************
*
* @purpose Take the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns DAPI_RESULT result
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigReadLockTake(L7_int8 unit, L7_int8 slot);


/*********************************************************************
*
* @purpose Release the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns DAPI_RESULT result
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigReadLockGive(L7_int8 unit, L7_int8 slot);


/*********************************************************************
*
* @purpose Take the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns DAPI_RESULT result
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigWriteLockTake(L7_int8 unit, L7_int8 slot);


/*********************************************************************
*
* @purpose Release the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns DAPI_RESULT result
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigWriteLockGive(L7_int8 unit, L7_int8 slot);

/*********************************************************************
*
* @purpose Check if the port configuration for this card needs to be cached
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns L7_TRUE: Port config has to be cached
* @returns L7_FALSE: Port config should not be cached
*
* @notes The caller must take the config semaphore for the card before  
*        calling this routine. Port config is cached if card plug-in
*        start or unplug-in start has been issued for the card.
*
* @end
*
*********************************************************************/
L7_BOOL dapiCardPortConfigCache(L7_int8 unit, L7_int8 slot);

#endif /* INCLUDE_DAPI_H */
