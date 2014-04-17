/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @purpose   This files contains prototypes, structures, and defines
*            for the dapi component
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
#ifndef INCLUDE_DAPI_STRUCT_H
#define INCLUDE_DAPI_STRUCT_H

#include <stdlib.h>

#include "l7_common.h"
#include "dapi.h"
#include "sysapi_hpc.h"

/* Family Names for debugging and printing to screen */
#define DAPI_FAMILY_UNSPECIFIED_NAME                      "DAPI_FAMILY_UNSPECIFIED"
#define DAPI_FAMILY_CONTROL_NAME                          "DAPI_FAMILY_CONTROL"
#define DAPI_FAMILY_CARD_NAME                             "DAPI_FAMILY_CARD"
#define DAPI_FAMILY_SYSTEM_NAME                           "DAPI_FAMILY_SYSTEM"
#define DAPI_FAMILY_FRAME_NAME                            "DAPI_FAMILY_FRAME"
#define DAPI_FAMILY_INTF_MGMT_NAME                        "DAPI_FAMILY_INTF_MGMT"
#define DAPI_FAMILY_ADDR_MGMT_NAME                        "DAPI_FAMILY_ADDR_MGMT"
#define DAPI_FAMILY_QVLAN_MGMT_NAME                       "DAPI_FAMILY_QVLAN_MGMT"
#define DAPI_FAMILY_IPSUBNET_VLAN_MGMT_NAME               "DAPI_FAMILY_IPSUBNET_VLAN_MGMT"
#define DAPI_FAMILY_MAC_VLAN_MGMT_NAME                    "DAPI_FAMILY_MAC_VLAN_MGMT"
#define DAPI_FAMILY_GARP_MGMT_NAME                        "DAPI_FAMILY_GARP_MGMT"
#define DAPI_FAMILY_LOGICAL_INTF_MGMT_NAME                "DAPI_FAMILY_LOGICAL_INTF_MGMT"
#define DAPI_FAMILY_LAG_MGMT_NAME                         "DAPI_FAMILY_LAG_MGMT"
#define DAPI_FAMILY_ROUTING_MGMT_NAME                     "DAPI_FAMILY_ROUTING_MGMT"
#define DAPI_FAMILY_ROUTING_ARP_MGMT_NAME                 "DAPI_FAMILY_ROUTING_ARP_MGMT"
#define DAPI_FAMILY_ROUTING_INTF_MGMT_NAME                "DAPI_FAMILY_ROUTING_INTF_MGMT"
#define DAPI_FAMILY_ROUTING_INTF_MCAST_NAME               "DAPI_FAMILY_ROUTING_INTF_MCAST"
#define DAPI_FAMILY_QOS_DIFFSERV_MGMT_NAME                "DAPI_FAMILY_QOS_DIFFSERV_MGMT"
#define DAPI_FAMILY_QOS_ACL_MGMT_NAME                     "DAPI_FAMILY_QOS_ACL_MGMT"
#define DAPI_FAMILY_QOS_COS_MGMT_NAME                     "DAPI_FAMILY_QOS_COS_MGMT"
#define DAPI_FAMILY_IPSG_MGMT_NAME                        "DAPI_FAMILY_IPSG_MGMT"
#define DAPI_FAMILY_SERVICES_NAME                         "DAPI_FAMILY_SERVICES"
#define DAPI_FAMILY_LAST_NAME                             "DAPI_FAMILY_LAST"

/* Command Names for debugging and printing to screen */

  /* card mgmt */
#define DAPI_CMD_CARD_INSERT_NAME                         "DAPI_CMD_CARD_INSERT"
#define DAPI_CMD_CARD_REMOVE_NAME                         "DAPI_CMD_CARD_REMOVE"
#define DAPI_CMD_CARD_PLUGIN_NAME                         "DAPI_CMD_CARD_PLUGIN"
#define DAPI_CMD_CARD_UNPLUG_NAME                         "DAPI_CMD_CARD_UNPLUG"

  /* system std */
#define DAPI_CMD_UNSPECIFIED_NAME                         "DAPI_CMD_UNSPECIFIED"
#define DAPI_CMD_CONTROL_CALLBACK_REGISTER_NAME           "DAPI_CMD_CONTROL_CALLBACK_REGISTER"
#define DAPI_CMD_CONTROL_DRIVER_START_NAME                "DAPI_CMD_CONTROL_DRIVER_START"
#define DAPI_CMD_CONTROL_HW_APPLY_NOTIFY_NAME             "DAPI_CMD_CONTROL_HW_APPLY_NOTIFY"
#define DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY_NAME          "DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY"
#define DAPI_CMD_SYSTEM_MIRRORING_NAME                    "DAPI_CMD_SYSTEM_MIRRORING"
#define DAPI_CMD_SYSTEM_MIRRORING_MODIFY_NAME             "DAPI_CMD_SYSTEM_MIRRORING_MODIFY"
#define DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS_NAME            "DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS"
#define DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS_NAME           "DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS"
#define DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET_NAME   "DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET"
#define DAPI_CMD_SYSTEM_FLOW_CONTROL_NAME                 "DAPI_CMD_SYSTEM_FLOW_CONTROL"
#define DAPI_CMD_SYSTEM_SNOOP_CONFIG_NAME                 "DAPI_CMD_SYSTEM_SNOOP_CONFIG"
#define DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE_NAME        "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE"
#define DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE_NAME        "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE"
#define DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD_NAME      "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD"
#define DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE_NAME   "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE"
#define DAPI_CMD_SYSTEM_DOT1X_CONFIG_NAME                 "DAPI_CMD_SYSTEM_DOT1X_CONFIG"
#define DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG_NAME            "DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG"
#define DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY_NAME          "DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY"
#define DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED_NAME           "DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED"
#define DAPI_CMD_INTF_SAMPLE_RATE_NAME                    "DAPI_CMD_INTF_SAMPLE_RATE"

#define DAPI_CMD_INTF_BLINK_SET_NAME                      "DAPI_CMD_INTF_BLINK_SET"

  /* L2 std - interface */
#define DAPI_CMD_INTF_UNSOLICITED_EVENT_NAME              "DAPI_CMD_INTF_UNSOLICITED_EVENT"
#define DAPI_CMD_INTF_STP_STATE_NAME                      "DAPI_CMD_INTF_STP_STATE"
#define DAPI_CMD_INTF_SPEED_CONFIG_NAME                   "DAPI_CMD_INTF_SPEED_CONFIG"
#define DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG_NAME          "DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG"
#define DAPI_CMD_INTF_LOOPBACK_CONFIG_NAME                "DAPI_CMD_INTF_LOOPBACK_CONFIG"
#define DAPI_CMD_INTF_ISOLATE_PHY_CONFIG_NAME             "DAPI_CMD_INTF_ISOLATE_PHY_CONFIG"
#define DAPI_CMD_INTF_STATISTICS_NAME                     "DAPI_CMD_INTF_STATISTICS"
#define DAPI_CMD_INTF_FLOW_CONTROL_NAME                   "DAPI_CMD_INTF_FLOW_CONTROL"
#define DAPI_CMD_INTF_CABLE_STATUS_GET_NAME               "DAPI_CMD_INTF_CABLE_STATUS_GET"
#define DAPI_CMD_INTF_FIBER_DIAG_GET_NAME                 "DAPI_CMD_INTF_FIBER_DIAG_GET"
#define DAPI_CMD_INTF_DOT1S_STATE_NAME                    "DAPI_CMD_INTF_DOT1S_STATE"
#define DAPI_CMD_INTF_DOT1X_STATUS_NAME                   "DAPI_CMD_INTF_DOT1X_STATUS"
#define DAPI_CMD_INTF_DOT1X_CLIENT_ADD_NAME               "DAPI_CMD_INTF_DOT1X_CLIENT_ADD"
#define DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE_NAME            "DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE"
#define DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT_NAME           "DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT"
#define DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK_NAME             "DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK"
#define DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK_NAME           "DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK"
#define DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG_NAME          "DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG"
#define DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG_NAME  "DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG"
#define DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG_NAME          "DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG"
#define DAPI_CMD_INTF_MAC_LOCK_CONFIG_NAME                "DAPI_CMD_INTF_MAC_LOCK_CONFIG"
#define DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG_NAME           "DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG"
#define DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG_NAME  "DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG"
#define DAPI_CMD_INTF_IPSG_CONFIG_NAME                    "DAPI_CMD_INTF_IPSG_CONFIG"
#define DAPI_CMD_INTF_IPSG_STATS_GET_NAME                 "DAPI_CMD_INTF_IPSG_STATS_GET"
#define DAPI_CMD_INTF_IPSG_CLIENT_ADD_NAME                "DAPI_CMD_INTF_IPSG_CLIENT_ADD"
#define DAPI_CMD_INTF_IPSG_CLIENT_DELETE_NAME             "DAPI_CMD_INTF_IPSG_CLIENT_DELETE"
#define DAPI_CMD_INTF_PFC_CONFIG_NAME                     "DAPI_CMD_INTF_PFC_CONFIG"
#define DAPI_CMD_INTF_PFC_STATS_GET_NAME                  "DAPI_CMD_INTF_PFC_STATS_GET"
#define DAPI_CMD_INTF_PFC_STATS_CLEAR_NAME                "DAPI_CMD_INTF_PFC_STATS_CLEAR"
#define DAPI_CMD_INTF_DOT3AH_CONFIG_ADD_NAME              "DAPI_CMD_INTF_DOT3AH_CONFIG_ADD"
#define DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE_NAME           "DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE"
#define DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG_NAME           "DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG"
#define DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_NAME              "DAPI_CMD_INTF_DOT3AH_REM_LB_ADD"
#define DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI_NAME          "DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI"
#define DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE_NAME           "DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE"
#define DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG_NAME   "DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG"

#define DAPI_CMD_INTF_LLDP_CONFIG_NAME                    "DAPI_CMD_INTF_LLDP_CONFIG"
#define DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET_NAME     "DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET"
#define DAPI_CMD_INTF_DOT1S_BPDU_FILTERING_NAME           "DAPI_CMD_INTF_DOT1S_BPDU_FILTERING"
#define DAPI_CMD_INTF_DOT1S_BPDU_GUARD_NAME               "DAPI_CMD_INTF_DOT1S_BPDU_GUARD"
#define DAPI_CMD_INTF_DOT1S_BPDU_FLOOD_NAME               "DAPI_CMD_INTF_DOT1S_BPDU_FLOOD"
#define DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG_NAME          "DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG"
#define DAPI_CMD_INTF_DOSCONTROL_CONFIG_NAME              "DAPI_CMD_INTF_DOSCONTROL_CONFIG"

  /* L2 std - FDB */
#define DAPI_CMD_ADDR_UNSOLICITED_EVENT_NAME              "DAPI_CMD_ADDR_UNSOLICITED_EVENT"
#define DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD_NAME          "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD"
#define DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY_NAME       "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY"
#define DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE_NAME       "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE"
#define DAPI_CMD_ADDR_MAC_FILTER_ADD_NAME                 "DAPI_CMD_ADDR_MAC_FILTER_ADD"
#define DAPI_CMD_ADDR_MAC_FILTER_DELETE_NAME              "DAPI_CMD_ADDR_MAC_FILTER_DELETE"
#define DAPI_CMD_ADDR_AGING_TIME_NAME                     "DAPI_CMD_ADDR_AGING_TIME"
#define DAPI_CMD_ADDR_FLUSH_NAME                          "DAPI_CMD_ADDR_FLUSH"
#define DAPI_CMD_ADDR_FLUSH_VLAN_NAME                          "DAPI_CMD_ADDR_FLUSH_VLAN"
#define DAPI_CMD_ADDR_FLUSH_MAC_NAME                          "DAPI_CMD_ADDR_FLUSH_MAC"
#define DAPI_CMD_ADDR_FLUSH_ALL_NAME                      "DAPI_CMD_ADDR_FLUSH_ALL"
#define DAPI_CMD_ADDR_SYNC_NAME                           "DAPI_CMD_ADDR_SYNC"

  /* L2 vlan */
#define DAPI_CMD_QVLAN_UNSOLICITED_EVENT_NAME             "DAPI_CMD_QVLAN_UNSOLICITED_EVENT"
#define DAPI_CMD_QVLAN_VLAN_CREATE_NAME                   "DAPI_CMD_QVLAN_VLAN_CREATE"
#define DAPI_CMD_QVLAN_VLAN_PORT_ADD_NAME                 "DAPI_CMD_QVLAN_VLAN_PORT_ADD"
#define DAPI_CMD_QVLAN_VLAN_PORT_DELETE_NAME              "DAPI_CMD_QVLAN_VLAN_PORT_DELETE"
#define DAPI_CMD_QVLAN_VLAN_PURGE_NAME                    "DAPI_CMD_QVLAN_VLAN_PURGE"
#define DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET_NAME            "DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET"
#define DAPI_CMD_QVLAN_PORT_CONFIG_NAME                   "DAPI_CMD_QVLAN_PORT_CONFIG"
#define DAPI_CMD_QVLAN_PBVLAN_CONFIG_NAME                 "DAPI_CMD_QVLAN_PBVLAN_CONFIG"                 
#define DAPI_CMD_QVLAN_PORT_PRIORITY_NAME                 "DAPI_CMD_QVLAN_PORT_PRIORITY"
#define DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP_NAME       "DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP"
#define DAPI_CMD_QVLAN_VLAN_LIST_CREATE_NAME              "DAPI_CMD_QVLAN_VLAN_LIST_CREATE"                 
#define DAPI_CMD_QVLAN_VLAN_LIST_PURGE_NAME               "DAPI_CMD_QVLAN_VLAN_LIST_PURGE"                 
#define DAPI_CMD_QVLAN_VLAN_STATS_GET_NAME                "DAPI_CMD_QVLAN_VLAN_STATS_GET"
#define DAPI_CMD_QVLAN_MCAST_FLOOD_SET_NAME               "DAPI_CMD_QVLAN_MCAST_FLOOD_SET"

  /* IP Subnet VLANs */
#define DAPI_CMD_IPSUBNET_VLAN_CREATE_NAME                "DAPI_CMD_IPSUBNET_VLAN_CREATE"
#define DAPI_CMD_IPSUBNET_VLAN_DELETE_NAME                "DAPI_CMD_IPSUBNET_VLAN_DELETE"

 /* MAC VLANs */
#define DAPI_CMD_MAC_VLAN_CREATE_NAME                     "DAPI_CMD_MAC_VLAN_CREATE"
#define DAPI_CMD_MAC_VLAN_DELETE_NAME                     "DAPI_CMD_MAC_VLAN_DELETE"

  /* PROTECTED_PORT */
#define DAPI_CMD_PROTECTED_PORT_ADD_NAME                  "DAPI_CMD_PROTECTED_PORT_ADD"
#define DAPI_CMD_PROTECTED_PORT_DELETE_NAME               "DAPI_CMD_PROTECTED_PORT_DELETE"

  /* L2 GARP */
#define DAPI_CMD_GARP_GVRP_NAME                           "DAPI_CMD_GARP_GVRP"
#define DAPI_CMD_GARP_GMRP_NAME                           "DAPI_CMD_GARP_GMRP"
#define DAPI_CMD_GVRP_GMRP_CONFIG_NAME                    "DAPI_CMD_GVRP_GMRP_CONFIG"
#define DAPI_CMD_GARP_GROUP_REG_MODIFY_NAME               "DAPI_CMD_GARP_GROUP_REG_MODIFY"
#define DAPI_CMD_GARP_GROUP_REG_DELETE_NAME               "DAPI_CMD_GARP_GROUP_REG_DELETE"

  /* Logical Interface */
#define DAPI_CMD_LOGICAL_INTF_CREATE_NAME                 "DAPI_CMD_LOGICAL_INTF_CREATE"
#define DAPI_CMD_LOGICAL_INTF_DELETE_NAME                 "DAPI_CMD_LOGICAL_INTF_DELETE"

  /* L2 LAG */
#define DAPI_CMD_LAG_CREATE_NAME                          "DAPI_CMD_LAG_CREATE"
#define DAPI_CMD_LAG_PORT_ADD_NAME                        "DAPI_CMD_LAG_PORT_ADD"
#define DAPI_CMD_LAG_PORT_DELETE_NAME                     "DAPI_CMD_LAG_PORT_DELETE"
/* PTin added */
#define DAPI_CMD_INTERNAL_LAG_PORT_ADD_NAME                        "DAPI_CMD_INTERNAL_LAG_PORT_ADD"
#define DAPI_CMD_INTERNAL_LAG_PORT_DELETE_NAME                     "DAPI_CMD_INTERNAL_LAG_PORT_DELETE"
/* End of PTin added */
#define DAPI_CMD_LAG_DELETE_NAME                          "DAPI_CMD_LAG_DELETE"
#define DAPI_CMD_AD_TRUNK_MODE_SET_NAME                   "DAPI_CMD_AD_TRUNK_MODE_SET"
#define DAPI_CMD_LAG_HASHMODE_SET_NAME                    "DAPI_CMD_LAG_HASHMODE_SET"
#define DAPI_CMD_LAGS_SYNCHRONIZE_NAME                    "DAPI_CMD_LAGS_SYNCHRONIZE"

  /* L3 - Routing and routes */
#define DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD_NAME             "DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD"
#define DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY_NAME          "DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY"
#define DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_NAME          "DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE"
#define DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL_NAME      "DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL"
#define DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG_NAME     "DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG"
#define DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG_NAME     "DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG"
#define DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG_NAME "DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG"
#define DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG_NAME "DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG"
#define DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG_NAME     "DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG"
#define DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY_NAME     "DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY"
#define DAPI_CMD_ROUTING_INTF_ADDR_ADD_NAME               "DAPI_CMD_ROUTING_INTF_ADDR_ADD"
#define  DAPI_CMD_ROUTING_INTF_ADDR_DELETE_NAME           "DAPI_CMD_ROUTING_INTF_ADDR_DELETE"
  /* L3 - ARP */
#define DAPI_CMD_ROUTING_ARP_ENTRY_ADD_NAME               "DAPI_CMD_ROUTING_ARP_ENTRY_ADD"
#define DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY_NAME            "DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY"
#define DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_NAME            "DAPI_CMD_ROUTING_ARP_ENTRY_DELETE"
#define DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL_NAME        "DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL"
#define DAPI_CMD_ROUTING_ARP_ENTRY_QUERY_NAME             "DAPI_CMD_ROUTING_ARP_ENTRY_QUERY"
#define DAPI_CMD_ROUTING_ARP_ENTRY_RESOLVE_NAME           "DAPI_CMD_ROUTING_ARP_ENTRY_RESOLVE"

  /* L3 - Interface */
#define DAPI_CMD_ROUTING_INTF_ADD_NAME                    "DAPI_CMD_ROUTING_INTF_ADD"
#define DAPI_CMD_ROUTING_INTF_MODIFY_NAME                 "DAPI_CMD_ROUTING_INTF_MODIFY"
#define DAPI_CMD_ROUTING_INTF_DELETE_NAME                 "DAPI_CMD_ROUTING_INTF_DELETE"
#define DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG_NAME       "DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG"
#define DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD_NAME         "DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD"
#define DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE_NAME      "DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE"
#define DAPI_CMD_ROUTING_INTF_VRID_ADD_NAME               "DAPI_CMD_ROUTING_INTF_VRID_ADD"
#define DAPI_CMD_ROUTING_INTF_VRID_DELETE_NAME            "DAPI_CMD_ROUTING_INTF_VRID_DELETE"
#define DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET_NAME          "DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET"

  /* L3 - Multicast */
#define DAPI_CMD_ROUTING_INTF_MCAST_ADD_NAME              "DAPI_CMD_ROUTING_INTF_MCAST_ADD"
#define DAPI_CMD_ROUTING_INTF_MCAST_DELETE_NAME           "DAPI_CMD_ROUTING_INTF_MCAST_DELETE"
#define DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET_NAME     "DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET"
#define DAPI_CMD_ROUTING_INTF_MCAST_USE_GET_NAME          "DAPI_CMD_ROUTING_INTF_MCAST_USE_GET"
#define DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD_NAME         "DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD"
#define DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE_NAME      "DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE"
#define DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE_NAME "DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE"
#define DAPI_CMD_ROUTING_INTF_MCAST_SEND_UNKNL3_CPU_NAME  "DAPI_CMD_ROUTING_INTF_MCAST_SEND_UNKNL3_CPU"
#define DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET_NAME          "DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET"
#define DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT_NAME "DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT"

  /* system std - frame handling */
#define DAPI_CMD_FRAME_UNSOLICITED_EVENT_NAME             "DAPI_CMD_FRAME_UNSOLICITED_EVENT"
#define DAPI_CMD_FRAME_SEND_NAME                          "DAPI_CMD_FRAME_SEND"

  /* Flex - QOS */
#define  DAPI_CMD_QOS_DIFFSERV_INST_ADD_NAME              "DAPI_CMD_QOS_DIFFSERV_INST_ADD"
#define  DAPI_CMD_QOS_DIFFSERV_INST_DELETE_NAME           "DAPI_CMD_QOS_DIFFSERV_INST_DELETE"
#define  DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET_NAME      "DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET"
#define  DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET_NAME     "DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET"

  /* ACL */
#define  DAPI_CMD_QOS_ACL_ADD_NAME                        "DAPI_CMD_QOS_ACL_ADD"
#define  DAPI_CMD_QOS_ACL_DELETE_NAME                     "DAPI_CMD_QOS_ACL_DELETE"
#define  DAPI_CMD_QOS_ACL_RULE_COUNT_GET_NAME             "DAPI_CMD_QOS_ACL_RULE_COUNT_GET"
#define  DAPI_CMD_QOS_SYS_ACL_ADD_NAME                    "DAPI_CMD_QOS_SYS_ACL_ADD"
#define  DAPI_CMD_QOS_SYS_ACL_DELETE_NAME                 "DAPI_CMD_QOS_SYS_ACL_DELETE"
#define  DAPI_CMD_QOS_ACL_RULE_STATUS_SET_NAME            "DAPI_CMD_QOS_ACL_RULE_STATUS_SET"

  /* COS */
#define  DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP_NAME    "DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP"
#define  DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP_NAME          "DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP"
#define  DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG_NAME     "DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG"
#define  DAPI_CMD_QOS_COS_INTF_CONFIG_NAME                "DAPI_CMD_QOS_COS_INTF_CONFIG"
#define  DAPI_CMD_QOS_COS_INTF_STATUS_NAME                "DAPI_CMD_QOS_COS_INTF_STATUS"
#define  DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG_NAME         "DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG"
#define  DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG_NAME          "DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG"

/*VOIP*/
#define  DAPI_CMD_QOS_VOIP_SESSION_NAME                    "DAPI_CMD_QOS_VOIP_SESSION"
#define  DAPI_CMD_QOS_VOIP_PROFILE_NAME                    "DAPI_CMD_QOS_VOIP_PRFILE"
#define  DAPI_CMD_QOS_VOIP_STATS_GET_NAME                  "DAPI_CMD_QOS_VOIP_STATS_GET"

  /* ISCSI */
#define  DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD_NAME           "DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD"
#define  DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE_NAME        "DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE"
#define  DAPI_CMD_QOS_ISCSI_CONNECTION_ADD_NAME            "DAPI_CMD_QOS_ISCSI_CONNECTION_ADD"
#define  DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE_NAME         "DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE"
#define  DAPI_CMD_QOS_ISCSI_COUNTER_GET_NAME               "DAPI_CMD_QOS_ISCSI_COUNTER_GET"

/* Service */
#define  DAPI_CMD_SERVICES_UNSOLICITED_EVENT_NAME         "DAPI_CMD_SERVICES_UNSOLICITED_EVENT"

  /* IPv6 - Routing and routes */
#define  DAPI_CMD_IPV6_ROUTE_ENTRY_ADD_NAME               "DAPI_CMD_IPV6_ROUTE_ENTRY_ADD"
#define  DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY_NAME            "DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY"
#define  DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE_NAME            "DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE"
#define  DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG_NAME       "DAPI_CMD_IPV6_FORWARDING_CONFIG"
#define  DAPI_CMD_IPV6_ROUTE_BOOTP_DHCP_CONFIG_NAME       "DAPI_CMD_IPV6_BOOTP_DHCP_CONFIG"
#define  DAPI_CMD_IPV6_ROUTE_ICMP_REDIRECTS_CONFIG_NAME   "DAPI_CMD_IPV6_ICMP_REDIRECTS_CONFIG"
#define  DAPI_CMD_IPV6_ROUTE_MCAST_FORWARDING_CONFIG_NAME "DAPI_CMD_IPV6_MCAST_FOWARDING_CONFIG"
#define  DAPI_CMD_IPV6_ROUTE_MCAST_IGMP_CONFIG_NAME       "DAPI_CMD_IPV6_ROUTE_MCAST_IGMP_CONFIG"

  /* IPv6 - HOSTS */
#define  DAPI_CMD_IPV6_NEIGH_ENTRY_ADD_NAME               "DAPI_CMD_IPV6_NEIGH_ENTRY_ADD"
#define  DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY_NAME            "DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY"
#define  DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE_NAME            "DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE"
#define  DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY_NAME             "DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY"
#define  DAPI_CMD_IPV6_NEIGH_UNSOLICITED_EVENT_NAME       "DAPI_CMD_IPV6_NEIGH_UNSOLICITED_EVENT"

  /* L3 - Interface */
#define  DAPI_CMD_IPV6_INTF_MCAST_FWD_CONFIG_NAME         "DAPI_CMD_IPV6_INTF_MCAST_FWD_CONFIG"
#define  DAPI_CMD_IPV6_INTF_LOCAL_MCASTADD_NAME           "DAPI_CMD_IPV6_INTF_LOCAL_MCASTADD"
#define  DAPI_CMD_IPV6_INTF_LOCAL_MCASTDELETE_NAME        "DAPI_CMD_IPV6_INTF_LOCAL_MCASTDELETE"
#define  DAPI_CMD_IPV6_INTF_ADDR_ADD_NAME                 "DAPI_CMD_IPV6_INTF_ADDR_ADD"
#define  DAPI_CMD_IPV6_INTF_ADDR_DELETE_NAME              "DAPI_CMD_IPV6_INTF_ADDR_DELETE"
#define  DAPI_CMD_IPV6_INTF_STATS_GET_NAME                "DAPI_CMD_IPV6_INTF_STATS_GET"

  /* L3 - Tunnel */
#define  DAPI_CMD_TUNNEL_CREATE_NAME                      "DAPI_CMD_TUNNEL_CREATE"
#define  DAPI_CMD_TUNNEL_DELETE_NAME                      "DAPI_CMD_TUNNEL_DELETE"
#define  DAPI_CMD_TUNNEL_NEXT_HOP_SET_NAME                "DAPI_CMD_TUNNEL_NEXT_HOP_SET"
#define  DAPI_CMD_L3_TUNNEL_ADD_NAME                      "DAPI_CMD_L3_TUNNEL_ADD"
#define  DAPI_CMD_L3_TUNNEL_DELETE_NAME                   "DAPI_CMD_L3_TUNNEL_DELETE"
#define  DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD_NAME          "DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD"
#define  DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE_NAME       "DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE"
#define  DAPI_CMD_DEBUG_SHELL_NAME                        "DAPI_CMD_DEBUG_SHELL"
/*voice vlan commands*/
#define  DAPI_CMD_VOICE_VLAN_DEVICE_ADD_NAME              "DAPI_CMD_VOICE_VLAN_DEVICE_ADD"
#define  DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE_NAME           "DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE"
#define  DAPI_CMD_VOICE_VLAN_COS_OVERRIDE_NAME            "DAPI_CMD_VOICE_VLAN_COS_OVERRIDE"
#define  DAPI_CMD_VOICE_VLAN_AUTH_NAME                    "DAPI_CMD_VOICE_VLAN_AUTH"         

  /* L2 - Tunnel */
#define  DAPI_CMD_L2_TUNNEL_CREATE_NAME                   "DAPI_CMD_L2_TUNNEL_CREATE"
#define  DAPI_CMD_L2_TUNNEL_DELETE_NAME                   "DAPI_CMD_L2_TUNNEL_DELETE"
#define  DAPI_CMD_L2_TUNNEL_MODIFY_NAME                   "DAPI_CMD_L2_TUNNEL_MODIFY"
#define  DAPI_CMD_L2_TUNNEL_VLAN_SET_NAME                 "DAPI_CMD_L2_TUNNEL_VLAN_SET"
#define  DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET_NAME            "DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET"

  /* IP Source Guard */
#define  DAPI_CMD_IPSG_INTF_CONFIG_NAME                   "DAPI_CMD_IPSG_INTF_CONFIG"
#define  DAPI_CMD_IPSG_ADDRESS_ADD_NAME                   "DAPI_CMD_IPSG_ADDRESS_ADD"
#define  DAPI_CMD_IPSG_ADDRESS_DELETE_NAME                "DAPI_CMD_IPSG_ADDRESS_DELETE"

  /* Dot1ad */
#define  DAPI_CMD_L2_DOT1AD_INTF_TYPE_NAME              "DAPI_CMD_L2_DOT1AD_INTF_TYPE"
#define  DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION_NAME              "DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION"
#define  DAPI_CMD_L2_DOT1AD_INTF_STATS_NAME              "DAPI_CMD_L2_DOT1AD_INTF_STATS"
#define  DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA_NAME              "DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA"

  /* DOT1AD DataTunneling */
#define  DAPI_CMD_DOT1AD_ADD_NAME                        "DAPI_CMD_DOT1AD_ADD"
#define  DAPI_CMD_DOT1AD_DELETE_NAME                     "DAPI_CMD_DOT1AD_DELETE"
#define  DAPI_CMD_DOT1AD_RULE_COUNT_GET_NAME             "DAPI_CMD_DOT1AD_RULE_COUNT_GET"

  /* ISDP */
#define  DAPI_CMD_ISDP_INTF_STATUS_SET_NAME              "DAPI_CMD_ISDP_INTF_STATUS_SET"


  /* LLPF */
#define  DAPI_CMD_INTF_LLPF_CONFIG_NAME      "DAPI_CMD_INTF_LLPF_CONFIG"

/* PTIN added: DAPI */
#define  DAPI_CMD_PTIN_INIT_NAME                         "DAPI_CMD_PTIN_INIT"
#define  DAPI_CMD_PTIN_PORTEXT_NAME                      "DAPI_CMD_PTIN_PORTEXT"
#define  DAPI_CMD_PTIN_COUNTERS_READ_NAME                "DAPI_CMD_PTIN_COUNTERS_READ"
#define  DAPI_CMD_PTIN_COUNTERS_CLEAR_NAME               "DAPI_CMD_PTIN_COUNTERS_CLEAR"
#define  DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET_NAME        "DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET"
#define  DAPI_CMD_PTIN_VLAN_SETTINGS_NAME                "DAPI_CMD_PTIN_VLAN_SETTINGS"
#define  DAPI_CMD_PTIN_VLAN_DEFS_NAME                    "DAPI_CMD_PTIN_VLAN_DEFS"
#define  DAPI_CMD_PTIN_VLAN_XLATE_NAME                   "DAPI_CMD_PTIN_VLAN_XLATE"
#define  DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP_NAME  "DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP"
#define  DAPI_CMD_PTIN_VLAN_CROSSCONNECT_NAME            "DAPI_CMD_PTIN_VLAN_CROSSCONNECT"
#define  DAPI_CMD_PTIN_VLAN_MULTICAST_NAME               "DAPI_CMD_PTIN_VLAN_MULTICAST"
#define  DAPI_CMD_PTIN_MULTICAST_EGRESS_PORT_NAME        "DAPI_CMD_PTIN_MULTICAST_EGRESS_PORT"
#define  DAPI_CMD_PTIN_VIRTUAL_PORT_NAME                 "DAPI_CMD_PTIN_VIRTUAL_PORT"
#define  DAPI_CMD_PTIN_HW_RESOURCES_NAME                 "DAPI_CMD_PTIN_HW_RESOURCES"
#define  DAPI_CMD_PTIN_BW_POLICER_NAME                   "DAPI_CMD_PTIN_BW_POLICER"
#define  DAPI_CMD_PTIN_FP_COUNTERS_NAME                  "DAPI_CMD_PTIN_FP_COUNTERS"
#define  DAPI_CMD_PTIN_STORM_CONTROL_NAME                "DAPI_CMD_PTIN_STORM_CONTROL"
#define  DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU_NAME          "DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU"
#define  DAPI_CMD_PTIN_PCS_PRBS_NAME                     "DAPI_CMD_PTIN_PCS_PRBS"
#define  DAPI_CMD_PTIN_SLOT_MODE_NAME                    "DAPI_CMD_PTIN_SLOT_MODE"
#define  DAPI_CMD_PTIN_HW_PROCEDURE_NAME                 "DAPI_CMD_PTIN_HW_PROCEDURE"
#define  DAPI_CMD_PTIN_L3_NAME                           "DAPI_CMD_PTIN_L3"
/* PTin end */

#define DAPI_PORT_GET(_pDapiUsp,_dapi_g)     (_dapi_g->unit[(_pDapiUsp)->unit]->slot[(_pDapiUsp)->slot]->port[(_pDapiUsp)->port])
#define HAPI_PORT_GET(_pHapiUsp,_dapi_g)     (_dapi_g->unit[(_pHapiUsp)->unit]->slot[(_pHapiUsp)->slot]->port[(_pHapiUsp)->port]->hapiPort)

#define IS_SLOT_TYPE_NOT_PRESENT(_usp,_dapi_g)       ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_NOT_PRESENT)
#define IS_SLOT_TYPE_PHYSICAL(_usp,_dapi_g)          ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_LINE)
#define IS_SLOT_TYPE_LOGICAL_LAG(_usp,_dapi_g)       ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_LAG)
#define IS_SLOT_TYPE_LOGICAL_VLAN(_usp,_dapi_g)      ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_VLAN_ROUTER)
#define IS_SLOT_TYPE_CAPWAP_TUNNEL(_usp,_dapi_g)      ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_CAPWAP_TUNNEL)
#define IS_SLOT_TYPE_CPU(_usp,_dapi_g)               ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType == SYSAPI_CARD_TYPE_CPU)
#define SLOT_TYPE_GET(_usp, _dapi_g)                 ((_dapi_g)->unit[(_usp)->unit]->slot[(_usp)->slot]->cardType)

#define IS_PORT_TYPE_PHYSICAL(_pDapiPortPtr) (((_pDapiPortPtr)->type == L7_IANA_ETHERNET)         || \
                                              ((_pDapiPortPtr)->type == L7_IANA_FAST_ETHERNET)    || \
                                              ((_pDapiPortPtr)->type == L7_IANA_FAST_ETHERNET_FX) || \
                                              ((_pDapiPortPtr)->type == L7_IANA_2G5_ETHERNET)     || /* PTin added: Speed 2.5G */ \
                                              ((_pDapiPortPtr)->type == L7_IANA_10G_ETHERNET)     || \
                                              ((_pDapiPortPtr)->type == L7_IANA_40G_ETHERNET)     || /* PTin added: Speed 40G */ \
                                              ((_pDapiPortPtr)->type == L7_IANA_100G_ETHERNET)    || /* PTin added: Speed 100G */ \
                                              ((_pDapiPortPtr)->type == L7_IANA_GIGABIT_ETHERNET))

#define IS_PORT_TYPE_LOGICAL_LAG(_pDapiPortPtr)       ((_pDapiPortPtr)->type == L7_IANA_LAG_DESC)
#define IS_PORT_TYPE_LOGICAL_VLAN(_pDapiPortPtr)      ((_pDapiPortPtr)->type == L7_IANA_L2_VLAN)
#define IS_PORT_TYPE_CPU(_pDapiPortPtr)               ((_pDapiPortPtr)->type == L7_IANA_OTHER_CPU)
#define IS_PORT_TYPE_LOOPBACK(_pDapiPortPtr)          ((_pDapiPortPtr)->type == L7_IANA_SOFTWARE_LOOPBACK)
#define IS_PORT_TYPE_TUNNEL(_pDapiPortPtr)            ((_pDapiPortPtr)->type == L7_IANA_TUNNEL)
#define IS_PORT_TYPE_CAPWAP_TUNNEL(_pDapiPortPtr)     ((_pDapiPortPtr)->type == L7_IANA_CAPWAP_TUNNEL)
#define IS_PORT_TYPE_VLAN_PORT(_pDapiPortPtr)         ((_pDapiPortPtr)->type == L7_IANA_VLAN_PORT)  /* PTin added: virtual ports */

#define CPU_USP_GET(_usp)                             dapiCpuUspGet((_usp))

#define IS_USP_TYPE_TUNNEL(_usp)    ((_usp)->unit == L7_LOGICAL_UNIT && \
                                     (_usp)->slot == L7_TUNNEL_SLOT_NUM)


typedef L7_RC_t (*HAPICTLFUNCPTR_t)(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, void *dapi_g);

typedef struct
{
  L7_BOOL     inUse;
  DAPI_USP_t  usp;
} DAPI_LAG_ENTRY_t;

typedef enum
{
  DEPRECATED_LINK_DOWN,
  DEPRECATED_LINK_UP
} DEPRECATED_DAPI_LINK_t;

typedef enum
{
  DAPI_PHY_TASK_POLL,
  DAPI_PHY_TASK_INTR
} DAPI_PHY_TASK_TYPE_t;

typedef enum line_stat_val_e
{
    statIfInOctets,
    statIfInUcastPkts,
    statIfInNUcastPkts,
    statIfInDiscards,
    statIfOutOctets,
    statIfOutUcastPkts,
    statIfOutNUcastPkts,

    statCount
} line_stat_val_t;

/* DAPI Port Structure */
typedef struct
{
  L7_IANA_INTF_TYPE_t   type;
  L7_PHY_CAPABILITIES_t phyCapabilities;
  L7_CONNECTOR_TYPES_t  connectorType;

  L7_BOOL               isLogicalIntfCreated;
  L7_BOOL               statsEnable;

  L7_BOOL                 isMirrored;
  DAPI_MIRROR_DIRECTION_t mirrorType;
  /* NOTE - an attempt to keep the physical and router structures in sync must be made! */
  union
  {
    struct
    {
      L7_ulong64                     *stats;
      L7_BOOL                         isLinkUp;
      L7_BOOL                         routerIntfEnabled;    /* not valid if physical interface */
      L7_ulong32                      ipAddr;
      L7_ulong32                      ipMask;
      L7_uchar8                       macAddr[6];
      L7_ulong32                      mtu;
      DAPI_ROUTING_INTF_ENCAPS_TYPE_t encapsType;
      L7_ushort16                     pvid;
      L7_BOOL                         bCastCapable;
    } physical;

    struct
    {
      L7_ulong64                     *stats;                /* not used in the router instantiation */
      L7_BOOL                         isLinkUp;
      L7_BOOL                         routerIntfEnabled;    /* Valid if routing interface */
      L7_ulong32                      ipAddr;
      L7_ulong32                      ipMask;
      L7_uchar8                       macAddr[6];
      L7_ulong32                      mtu;
      DAPI_ROUTING_INTF_ENCAPS_TYPE_t encapsType;
      L7_ushort16                     vlanID;
      L7_BOOL                         bCastCapable;
    } router;

    struct
    {
      L7_ulong64                     *stats;
      void                           *statsSema;
    } cpu;

    struct
    {
      void                           *lagmembersetSemaID;
      DAPI_LAG_ENTRY_t                memberSet[L7_MAX_MEMBERS_PER_LAG];
    } lag;
    struct
    {
      L7_ulong64                     *stats;
    } l2tunnel;
  } modeparm;

  HAPICTLFUNCPTR_t  cmdTable[DAPI_NUM_OF_CMDS];
  L7_ulong32        message_level[DAPI_NUM_OF_CMDS];

  /* pointer to hapi hw specific port struct */
  void             *hapiPort;

} DAPI_PORT_t;

/* DAPI Slot Structure */
typedef struct
{
  L7_ulong32             cardId;
  SYSAPI_CARD_TYPE_t     cardType;
  L7_BOOL                cardPresent;

  L7_ulong32             intLevel;


  L7_BOOL                cachePortConfig;
  L7_BOOL                pendingAdminModeConfig;
  DAPI_CARD_CMD_INFO_t   cardCmdInfo;

  DAPI_PORT_t            **port;

  void                   *hapiSlot;
  L7_ushort16             numOfPortsInSlot;

} DAPI_SLOT_t;

/* DAPI Unit Structure */
typedef struct
{
  DAPI_SLOT_t   **slot;

  L7_ushort16     numOfSlots;

  void           *hapiUnit;

} DAPI_UNIT_t;

/* DAPI System Structure */
typedef struct
{
  DAPI_CALLBACK_t  *familyCallback;
  L7_ushort16       totalNumOfUnits;
  L7_ulong64       *vlanStats[L7_MAX_VLAN_PER_BRIDGE];
  void             *hapiSystem;

  /* Stored data for non interface related info */
  L7_enetMacAddr_t systemMacAddr;
  L7_BOOL          broadcastControlEnable;
  L7_BOOL          multicastControlEnable;
  L7_BOOL          unicastControlEnable;
  L7_BOOL          flowControlEnable;
  L7_BOOL          igmpSnoopingEnable;
  L7_BOOL          mldSnoopingEnable;
  L7_BOOL          gvrpConfigEnabled;
  L7_BOOL          gmrpConfigEnabled;
  L7_BOOL          dot1sZeroCreated;
  L7_BOOL          routeForwardConfigEnable;
  L7_BOOL          ipv6ForwardConfigEnable;
  L7_BOOL          mcastEnable; /* IP Multicast enable/disable */
  L7_BOOL          igmpEnable;
  L7_BOOL          mldEnable; /* Igmp equivalent for IPv6 */
  L7_BOOL          icmpRedirEnable;
  L7_BOOL          dvlanEnable;

  /*Mirroring*/
  L7_BOOL          mirrorEnable;
  L7_uint32        mirroredPortSet[L7_MAX_PORT_COUNT + 1];
  DAPI_USP_t       probeUsp;

  /* Wireless */
  L7_ushort16 wlan_capwap_data_udp_dst_port;
  L7_ushort16 wlan_capwap_data_udp_src_port;

  /* Debug command handler */
  L7_RC_t          (*dbgDrivShell)(void *str);

  /* Driver start command handler */
  L7_RC_t          (*driverStart) (void *data);

} DAPI_SYSTEM_t;

/* DAPI Name Container Structure */
typedef struct
{
  const L7_char8 *name;

} DAPI_NAME_CONTAINER_t;

/* DAPI Name Structure for both families and commands */
typedef struct
{
  DAPI_NAME_CONTAINER_t *family;
  DAPI_NAME_CONTAINER_t *cmd;
  DAPI_FAMILY_t         *cmdToFamilyTable;

} DAPI_NAME_t;

/* The driver object */
typedef struct
{
  DAPI_SYSTEM_t       *system;
  DAPI_UNIT_t        **unit;
  DAPI_NAME_t         *name;
  L7_BOOL              initialized;
  L7_ushort16          message_level;

} DAPI_t;

typedef enum
{
  DAPI_USP_SHRINK,
  DAPI_USP_EXPAND

} DAPI_USP_CONVERT_CMD_t;

/*
 * These two prototypes exist here because they must be defined after DAPI_t
 * is defined. Rightfully they should exist in the BSP area.
 */

/*
 * These two prototypes exist here because they must be defined after DAPI_t
 * is defined. Rightfully they should exist in the BSP area.
 */

/****************************************************************************************
*
* @purpose Control entry point from HAPI back to application
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the callback. For
*                               interface directed callbacks, this should be the port
*                               requesting the action. For non-port specific callbacks,
*                               this should be 0.0.0.
* @param    family  @b{(input)} The feature group
* @param    cmd     @b{(input)} The callback command to execute
* @param   *cmdInfo @b{(input)} data for the callback function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiCallback(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *cmdInfo);

/****************************************************************************************
*
* @purpose  Determines usp validity in the system.
*
* @param   *usp           @b{(input)} The unit, slot, port that is to be tested
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_BOOL isValidUsp(DAPI_USP_t *usp, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Determines unit, slot validity in the system.
*
* @param   *usp           @b{(input)} The unit, slot that is to be tested
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_BOOL  isValidSlot(DAPI_USP_t *usp, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Retrieves the CPU's usp
*
* @param   *usp           @b{(output)} The unit, slot, port of the CPU interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiCpuUspGet(DAPI_USP_t *usp);

/****************************************************************************************
*
* @purpose  Registers the family for each DAPI command
*
* @param   *data    @b{(input)} Data for the control function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiCmdToFamilyTableInit(DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Initializes a DAPI logical (CPU) card
*
* @param    dapiUsp     @b{(input)}  unit, slot, port(NA)
* @param    cmd         @b{(input)}  command - should be a card insert
* @param   *data        @b{(input)}  data for the card creation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiCpuCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data);

/****************************************************************************************
*
* @purpose  Initializes a DAPI logical (LAG) card
*
* @param    dapiUsp     @b{(input)}  unit, slot, port(NA)
* @param    cmd         @b{(input)}  command - should be a card insert
* @param   *data        @b{(input)}  data for the card creation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
****************************************************************************************/
L7_RC_t dapiLagCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data);

/****************************************************************************************
*
* @purpose  Initializes a DAPI logical card
*
* @param    cardType    @b{(input)}  type of card
* @param    dapiUsp     @b{(input)}  unit, slot, port(NA)
* @param    cmd         @b{(input)}  command - should be a card Insert
* @param   *data        @b{(input)}  data for the card creation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This can be used as a base function to commonize several of
*           the other dapi*CardInsert functions
*
* @end
*
****************************************************************************************/
L7_RC_t dapiLogicalCardInsert(SYSAPI_CARD_TYPE_t cardType, DAPI_USP_t *dapiUsp,
                              DAPI_CMD_t cmd, void *data);

/****************************************************************************************
*
* @purpose  Control entry point to DAPI
*
* @param   *usp     @b{(input)} The unit, slot, port designation of the LAG to which this
*                               command is directed.
* @param    cmd     @b{(input)} Command to execute
* @param   *data    @b{(input)} Data for the control function
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
L7_RC_t dapiCtlLagIntf(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data);

/*********************************************************************
*
* @purpose  Provide an error path for uninstantiated functions
*
* @param   *usp           @b{(input)} The USP of the port that is to be set
* @param    cmd           @b{(input)} Command
* @param   *data          @b{(input)} Command data
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dapiFuncError(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initializes Function table so it logs a messages indicating
*          that the operation is invalid for this type of interface,
*          and returns an error.
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiFuncInvalidIntf(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data,
                            DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initializes Function table so that the function does
*          nothing and returns L7_SUCCESS.
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiFuncReturnSuccess(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data,
                              DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initializes Function table so that the function does
*          nothing and returns L7_FAILURE.
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiFuncReturnFailure(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data,
                              DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Generic HAPI-level logical card insert function
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns L7_SUCCESS
*
* @notes This is a stand-in function for the hapi-level card insert
*        function, for logical interface cards that require no
*        real hardware support.  This can be overridden for a
*        specific hardware platform.
*
* @end
*
*********************************************************************/
L7_RC_t dapiGenericCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data,
                              void *handle);

/*********************************************************************
*
* @purpose  Registers a callback for a particular family
*
* @param   *usp           @b{(input)} The USP of the port that is to be set
* @param    cmd           @b{(input)} Command
* @param   *data          @b{(input)} Command data
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dapiControlCallbackRegister(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*******************************************************************************
*
* @purpose  Get the DAPI_PORT_t of an associated usp
*
* @param    *b      @b(input) Pointer to the global driver handle
* @param    *pUsp   @b(input) Pointer to a usp
*
* @returns  *DAPI_PORT_t      Pointer to the associated DAPI_PORT_t structure
*
* @comments
*
* @end
*
*******************************************************************************/
#define GET_DAPI_PORT(b,usp) \
  ((DAPI_PORT_t*)(b)->unit[(usp)->unit]->slot[(usp)->slot]->port[(usp)->port])

/*********************************************************************
*
* @purpose Initializes one card at a time
*
* @param   *usp         @b{(input)}  unit, slot, port(na)
* @param    cmd         @b{(input)}  command - card insert
* @param   *data        @b{(input)}  data for the command
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardInsert(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data);

/*********************************************************************
*
* @purpose Un-initializes one card at a time
*
* @param  unitNum    unit number of this card
* @param  slotNum    slot number of this card
* @param  cardId identification number of the line card
* @param  intLevel   the interrupt level to register this card
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data);

#endif /* INCLUDE_DAPI_STRUCT_H */
