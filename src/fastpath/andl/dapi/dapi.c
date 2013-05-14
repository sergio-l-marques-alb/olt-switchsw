/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi.c
*
* @purpose   This file contains the entry points to DAPI
*
* @component dapi
*
* @comments
*
* @create    6/12/2000
*
* @author    smanders
*
* @end
*
**********************************************************************/
#include <string.h>

#include "l7_common.h"
#include "dapi.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "osapi.h"
#include "log.h"

#include "dapi_struct.h"
#include "dapi_debug.h"
#include "dapi_db.h"
#include "platform_config.h"
#include "spm_api.h"

#include "osapi_trace.h"
#include "dapi_trace.h"
#include "hpc_db.h"

/*
 * Globals
 */
DAPI_t *dapi_g;

DAPI_FAMILY_t cmdToFamilyTable[DAPI_NUM_OF_CMDS];


/* Instantiation of the text command families */
DAPI_NAME_CONTAINER_t dapi_family_name_g[DAPI_NUM_OF_FAMILIES] =
{
{DAPI_FAMILY_UNSPECIFIED_NAME},
{DAPI_FAMILY_CONTROL_NAME},
{DAPI_FAMILY_CARD_NAME},
{DAPI_FAMILY_SYSTEM_NAME},
{DAPI_FAMILY_FRAME_NAME},
{DAPI_FAMILY_INTF_MGMT_NAME},
{DAPI_FAMILY_ADDR_MGMT_NAME},
{DAPI_FAMILY_QVLAN_MGMT_NAME},
{DAPI_FAMILY_IPSUBNET_VLAN_MGMT_NAME},
{DAPI_FAMILY_MAC_VLAN_MGMT_NAME},
{DAPI_FAMILY_GARP_MGMT_NAME},
{DAPI_FAMILY_LOGICAL_INTF_MGMT_NAME},
{DAPI_FAMILY_LAG_MGMT_NAME},
{DAPI_FAMILY_ROUTING_MGMT_NAME},
{DAPI_FAMILY_ROUTING_ARP_MGMT_NAME},
{DAPI_FAMILY_ROUTING_INTF_MGMT_NAME},
{DAPI_FAMILY_ROUTING_INTF_MCAST_NAME},
{DAPI_FAMILY_QOS_DIFFSERV_MGMT_NAME},
{DAPI_FAMILY_QOS_ACL_MGMT_NAME},
{DAPI_FAMILY_QOS_COS_MGMT_NAME},
{DAPI_FAMILY_SERVICES_NAME}
};

/* Instantiation of the text command names */
DAPI_NAME_CONTAINER_t dapi_cmd_name_g[DAPI_NUM_OF_CMDS] =
{
{DAPI_CMD_UNSPECIFIED_NAME},

/* system control */
{DAPI_CMD_CONTROL_CALLBACK_REGISTER_NAME},
{DAPI_CMD_CONTROL_DRIVER_START_NAME},
{DAPI_CMD_CONTROL_HW_APPLY_NOTIFY_NAME},
{DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY_NAME},

/* card mgmt */
{DAPI_CMD_CARD_INSERT_NAME},
{DAPI_CMD_CARD_REMOVE_NAME},
{DAPI_CMD_CARD_PLUGIN_NAME},
{DAPI_CMD_CARD_UNPLUG_NAME},


/* system std */
{DAPI_CMD_SYSTEM_MIRRORING_NAME},
{DAPI_CMD_SYSTEM_MIRRORING_MODIFY_NAME},
{DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS_NAME},
{DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS_NAME},
{DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET_NAME},
{DAPI_CMD_SYSTEM_FLOW_CONTROL_NAME},
{DAPI_CMD_SYSTEM_SNOOP_CONFIG_NAME},
{DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE_NAME},
{DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE_NAME},
{DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD_NAME},
{DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE_NAME},
{DAPI_CMD_SYSTEM_DOT1X_CONFIG_NAME},
{DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG_NAME},
{DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY_NAME},
{DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED_NAME},
{DAPI_CMD_INTF_SAMPLE_RATE_NAME},

{DAPI_CMD_INTF_BLINK_SET_NAME},

/* system std - frame handling */
{DAPI_CMD_FRAME_UNSOLICITED_EVENT_NAME},
{DAPI_CMD_FRAME_SEND_NAME},

/* L2 std - interface */
{DAPI_CMD_INTF_UNSOLICITED_EVENT_NAME},
{DAPI_CMD_INTF_STP_STATE_NAME},
{DAPI_CMD_INTF_SPEED_CONFIG_NAME},
{DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG_NAME},
{DAPI_CMD_INTF_LOOPBACK_CONFIG_NAME},
{DAPI_CMD_INTF_ISOLATE_PHY_CONFIG_NAME},
{DAPI_CMD_INTF_STATISTICS_NAME},
{DAPI_CMD_INTF_FLOW_CONTROL_NAME},
{DAPI_CMD_INTF_CABLE_STATUS_GET_NAME},
{DAPI_CMD_INTF_FIBER_DIAG_GET_NAME},
{DAPI_CMD_INTF_DOT1S_STATE_NAME},
{DAPI_CMD_INTF_DOT1X_STATUS_NAME},
{DAPI_CMD_INTF_DOT1X_CLIENT_ADD_NAME},
{DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE_NAME},
{DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT_NAME},
{DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK_NAME},
{DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK_NAME},
{DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG_NAME},
{DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG_NAME},
{DAPI_CMD_INTF_MAC_LOCK_CONFIG_NAME},
{DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG_NAME},
{DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG_NAME},
{DAPI_CMD_INTF_IPSG_CONFIG_NAME},
{DAPI_CMD_INTF_IPSG_STATS_GET_NAME},
{DAPI_CMD_INTF_IPSG_CLIENT_ADD_NAME},
{DAPI_CMD_INTF_IPSG_CLIENT_DELETE_NAME},
{DAPI_CMD_INTF_PFC_CONFIG_NAME},
{DAPI_CMD_INTF_PFC_STATS_GET_NAME},
{DAPI_CMD_INTF_PFC_STATS_CLEAR_NAME},
/*  DOT3AH EFM-OAM*/
{DAPI_CMD_INTF_DOT3AH_CONFIG_ADD_NAME},
{DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE_NAME},
{DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG_NAME},
{DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_NAME},
{DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI_NAME},
{DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE_NAME},

{DAPI_CMD_INTF_LLDP_CONFIG_NAME},
{DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET_NAME},
{DAPI_CMD_INTF_DOT1S_BPDU_FILTERING_NAME},
{DAPI_CMD_INTF_DOT1S_BPDU_GUARD_NAME},
{DAPI_CMD_INTF_DOT1S_BPDU_FLOOD_NAME},
{DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG_NAME},
{DAPI_CMD_INTF_DOSCONTROL_CONFIG_NAME},
{DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG_NAME},
{DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG_NAME},

/* L2 std - FDB */
{DAPI_CMD_ADDR_UNSOLICITED_EVENT_NAME},
{DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD_NAME},
{DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY_NAME},
{DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE_NAME},
{DAPI_CMD_ADDR_MAC_FILTER_ADD_NAME},
{DAPI_CMD_ADDR_MAC_FILTER_DELETE_NAME},
{DAPI_CMD_ADDR_AGING_TIME_NAME},
{DAPI_CMD_ADDR_FLUSH_NAME},
{DAPI_CMD_ADDR_FLUSH_ALL_NAME},
{DAPI_CMD_ADDR_FLUSH_VLAN_NAME},
{DAPI_CMD_ADDR_FLUSH_MAC_NAME},
{DAPI_CMD_ADDR_SYNC_NAME},

/* L2 vlan */
{DAPI_CMD_QVLAN_UNSOLICITED_EVENT_NAME},
{DAPI_CMD_QVLAN_VLAN_CREATE_NAME},
{DAPI_CMD_QVLAN_VLAN_PORT_ADD_NAME},
{DAPI_CMD_QVLAN_VLAN_PORT_DELETE_NAME},
{DAPI_CMD_QVLAN_VLAN_PURGE_NAME},
{DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET_NAME},
{DAPI_CMD_QVLAN_PORT_CONFIG_NAME},
{DAPI_CMD_QVLAN_PBVLAN_CONFIG_NAME},
{DAPI_CMD_QVLAN_PORT_PRIORITY_NAME},
{DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP_NAME},
{DAPI_CMD_QVLAN_VLAN_STATS_GET_NAME},
{DAPI_CMD_QVLAN_MCAST_FLOOD_SET_NAME},
{DAPI_CMD_QVLAN_VLAN_LIST_CREATE_NAME},
{DAPI_CMD_QVLAN_VLAN_LIST_PURGE_NAME},

/* IP Subnet VLANs */
{DAPI_CMD_IPSUBNET_VLAN_CREATE_NAME},
{DAPI_CMD_IPSUBNET_VLAN_DELETE_NAME},

/* MAC VLANs */
{DAPI_CMD_MAC_VLAN_CREATE_NAME},
{DAPI_CMD_MAC_VLAN_DELETE_NAME},

/* PROTECTED_PORT */
{DAPI_CMD_PROTECTED_PORT_ADD_NAME},
{DAPI_CMD_PROTECTED_PORT_DELETE_NAME},

/* L2 GARP */
{DAPI_CMD_GARP_GVRP_NAME},
{DAPI_CMD_GARP_GMRP_NAME},
{DAPI_CMD_GVRP_GMRP_CONFIG_NAME},
{DAPI_CMD_GARP_GROUP_REG_MODIFY_NAME},
{DAPI_CMD_GARP_GROUP_REG_DELETE_NAME},

/* Logical Interface */
{DAPI_CMD_LOGICAL_INTF_CREATE_NAME},
{DAPI_CMD_LOGICAL_INTF_DELETE_NAME},

/* L2 LAG */
{DAPI_CMD_LAG_CREATE_NAME},
{DAPI_CMD_LAG_PORT_ADD_NAME},
{DAPI_CMD_LAG_PORT_DELETE_NAME},
{DAPI_CMD_LAG_DELETE_NAME},
{DAPI_CMD_AD_TRUNK_MODE_SET_NAME},
{DAPI_CMD_LAG_HASHMODE_SET_NAME},
{DAPI_CMD_LAGS_SYNCHRONIZE_NAME},

/* L3 - Routing and routes */
{DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD_NAME},
{DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY_NAME},
{DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_NAME},
{DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL_NAME},
{DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG_NAME},
{DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG_NAME},
{DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG_NAME},
{DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG_NAME},
{DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG_NAME},

/* L3 - ARP */
{DAPI_CMD_ROUTING_ARP_ENTRY_ADD_NAME},
{DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY_NAME},
{DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_NAME},
{DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL_NAME},
{DAPI_CMD_ROUTING_ARP_ENTRY_QUERY_NAME},
{DAPI_CMD_ROUTING_ARP_ENTRY_RESOLVE_NAME},

/* L3 - Interface */
{DAPI_CMD_ROUTING_INTF_ADD_NAME},
{DAPI_CMD_ROUTING_INTF_MODIFY_NAME},
{DAPI_CMD_ROUTING_INTF_DELETE_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG_NAME},
{DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD_NAME},
{DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE_NAME},
{DAPI_CMD_ROUTING_INTF_VRID_ADD_NAME},
{DAPI_CMD_ROUTING_INTF_VRID_DELETE_NAME},
{DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY_NAME},

/* L3 - Multicast */
{DAPI_CMD_ROUTING_INTF_MCAST_ADD_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_DELETE_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_USE_GET_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET_NAME},
{DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT_NAME},

/* Flex QOS - DiffServ */
{DAPI_CMD_QOS_DIFFSERV_INST_ADD_NAME},
{DAPI_CMD_QOS_DIFFSERV_INST_DELETE_NAME},
{DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET_NAME},
{DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET_NAME},

/* Flex QOS - ACL */
{DAPI_CMD_QOS_ACL_ADD_NAME},
{DAPI_CMD_QOS_ACL_DELETE_NAME},
{DAPI_CMD_QOS_ACL_RULE_COUNT_GET_NAME},
{DAPI_CMD_QOS_SYS_ACL_ADD_NAME},
{DAPI_CMD_QOS_SYS_ACL_DELETE_NAME},
{DAPI_CMD_QOS_ACL_RULE_STATUS_SET_NAME},

/* Flex QOS - COS */
{DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP_NAME},
{DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP_NAME},
{DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG_NAME},
{DAPI_CMD_QOS_COS_INTF_CONFIG_NAME},
{DAPI_CMD_QOS_COS_INTF_STATUS_NAME},
{DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG_NAME},
{DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG_NAME},

/* Voip */
{DAPI_CMD_QOS_VOIP_SESSION_NAME},
{DAPI_CMD_QOS_VOIP_PROFILE_NAME},
{DAPI_CMD_QOS_VOIP_STATS_GET_NAME},

/* iSCSI */
{DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD_NAME},
{DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE_NAME},
{DAPI_CMD_QOS_ISCSI_CONNECTION_ADD_NAME},
{DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE_NAME},
{DAPI_CMD_QOS_ISCSI_COUNTER_GET_NAME},
/* Services */
{DAPI_CMD_SERVICES_UNSOLICITED_EVENT_NAME},

/* IPv6 - Routing and routes */
{DAPI_CMD_IPV6_ROUTE_ENTRY_ADD_NAME},
{DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY_NAME},
{DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE_NAME},
{DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG_NAME},
{DAPI_CMD_IPV6_ROUTE_BOOTP_DHCP_CONFIG_NAME},
{DAPI_CMD_IPV6_ROUTE_ICMP_REDIRECTS_CONFIG_NAME},
{DAPI_CMD_IPV6_ROUTE_MCAST_FORWARDING_CONFIG_NAME},
{DAPI_CMD_IPV6_ROUTE_MCAST_IGMP_CONFIG_NAME},

/* IPv6 - HOSTS */
{DAPI_CMD_IPV6_NEIGH_ENTRY_ADD_NAME},
{DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY_NAME},
{DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE_NAME},
{DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY_NAME},
{DAPI_CMD_IPV6_NEIGH_UNSOLICITED_EVENT_NAME},

/* L3 - Interface */
{DAPI_CMD_IPV6_INTF_MCAST_FWD_CONFIG_NAME},
{DAPI_CMD_IPV6_INTF_LOCAL_MCASTADD_NAME},
{DAPI_CMD_IPV6_INTF_LOCAL_MCASTDELETE_NAME},
{DAPI_CMD_IPV6_INTF_ADDR_ADD_NAME},
{DAPI_CMD_IPV6_INTF_ADDR_DELETE_NAME},
{DAPI_CMD_IPV6_INTF_STATS_GET_NAME},

/* L3 - Tunnel */
{DAPI_CMD_TUNNEL_CREATE_NAME},
{DAPI_CMD_TUNNEL_DELETE_NAME},
{DAPI_CMD_TUNNEL_NEXT_HOP_SET_NAME},
{DAPI_CMD_L3_TUNNEL_ADD_NAME},
{DAPI_CMD_L3_TUNNEL_DELETE_NAME},
{DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD_NAME},
{DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE_NAME},
{DAPI_CMD_DEBUG_SHELL_NAME},

/* L2- Voice Vlan */
{DAPI_CMD_VOICE_VLAN_DEVICE_ADD_NAME},
{DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE_NAME},
{DAPI_CMD_VOICE_VLAN_COS_OVERRIDE_NAME},
{DAPI_CMD_VOICE_VLAN_AUTH_NAME},

/* L2 - Tunnel */
{DAPI_CMD_L2_TUNNEL_CREATE_NAME},
{DAPI_CMD_L2_TUNNEL_DELETE_NAME},
{DAPI_CMD_L2_TUNNEL_MODIFY_NAME},
{DAPI_CMD_L2_TUNNEL_VLAN_SET_NAME},
{DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET_NAME},

/* Dot1ad commands */
{DAPI_CMD_L2_DOT1AD_INTF_TYPE_NAME},
{DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION_NAME},
{DAPI_CMD_L2_DOT1AD_INTF_STATS_NAME},
{DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA_NAME},

/* DOT1AD DataTunneling */
{DAPI_CMD_DOT1AD_ADD_NAME},
{DAPI_CMD_DOT1AD_DELETE_NAME},
{DAPI_CMD_DOT1AD_RULE_COUNT_GET_NAME},
{DAPI_CMD_ISDP_INTF_STATUS_SET_NAME},

/* LLPF */
{DAPI_CMD_INTF_LLPF_CONFIG_NAME},

/* PTin added: DAPI */
{DAPI_CMD_PTIN_INIT_NAME},
{DAPI_CMD_PTIN_PORTEXT_NAME},
{DAPI_CMD_PTIN_COUNTERS_READ_NAME},
{DAPI_CMD_PTIN_COUNTERS_CLEAR_NAME},
{DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET_NAME},
{DAPI_CMD_PTIN_VLAN_SETTINGS_NAME},
{DAPI_CMD_PTIN_VLAN_DEFS_NAME},
{DAPI_CMD_PTIN_VLAN_XLATE_NAME},
{DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP_NAME},
{DAPI_CMD_PTIN_VLAN_CROSSCONNECT_NAME},
{DAPI_CMD_PTIN_HW_RESOURCES_NAME},
{DAPI_CMD_PTIN_BW_POLICER_NAME},
{DAPI_CMD_PTIN_FP_COUNTERS_NAME},
{DAPI_CMD_PTIN_PACKET_RATE_LIMIT_NAME},
{DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU_NAME},
{DAPI_CMD_PTIN_PCS_PRBS_NAME},
};

static osapiRWLock_t dapiCardRemovalRWLock;

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
void dapiCardRemovalWriteLockTake (void)
{
  (void)osapiWriteLockTake(dapiCardRemovalRWLock, L7_WAIT_FOREVER);
}

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
void dapiCardRemovalWriteLockGive (void)
{
  (void)osapiWriteLockGive(dapiCardRemovalRWLock);
}

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
void dapiCardRemovalReadLockTake (void)
{
  (void )osapiReadLockTake(dapiCardRemovalRWLock, L7_WAIT_FOREVER);
}

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
void dapiCardRemovalReadLockGive (void)
{
  (void )osapiReadLockGive(dapiCardRemovalRWLock);
}

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
L7_RC_t dapiCtl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data)
{
  L7_RC_t                     result = L7_SUCCESS;
  DAPI_USP_t                  dapiUsp;
  HAPICTLFUNCPTR_t            funcPtr;
  L7_uint32                   delta = 0;
  DAPI_CARD_MGMT_CMD_t       *dapiCmd;

  OSAPI_TRACE_EVENT (L7_TRACE_EVENT_DRIVER_ENTER, 0, 0);
  if (cmd != DAPI_CMD_FRAME_SEND)
    dapiTraceDapiCtl(cmd,usp,L7_TRUE,delta);
  delta = osapiTimeMillisecondsGet();
  dapiCmdProfile(cmd, L7_TRUE);


  /* Check is dapiInit has been called successfully */
  if ((dapi_g == L7_NULLPTR) || (dapi_g->initialized != L7_TRUE))
  {
    result = L7_FAILURE;
  }
  else
  {
    dapiUsp.unit = usp->unit;
    dapiUsp.slot = usp->slot;
    dapiUsp.port = usp->port;

    /* Determine whether the command is interface directed or not */
    switch (cmd)
    {
    case DAPI_CMD_CONTROL_CALLBACK_REGISTER:
      if (dapiControlCallbackRegister(&dapiUsp, cmd, data, dapi_g) != L7_SUCCESS) {
        result = L7_FAILURE;
      }
      break;

    case DAPI_CMD_CONTROL_DRIVER_START:
      if (dapi_g->system->driverStart != L7_NULLPTR)
      {
        result = dapi_g->system->driverStart(data);
      }
      break;

    case DAPI_CMD_DEBUG_SHELL:
      if (dapi_g->system->dbgDrivShell != L7_NULLPTR) {
        dapi_g->system->dbgDrivShell(data);
      }
      break;

    case DAPI_CMD_SYSTEM_MIRRORING:
    case DAPI_CMD_SYSTEM_MIRRORING_PORT_MODIFY:
    case DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS:
    case DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS:
    case DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET:
    case DAPI_CMD_SYSTEM_FLOW_CONTROL:
    case DAPI_CMD_SYSTEM_SNOOP_CONFIG:
    case DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY:
    case DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED:
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE:
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE:
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD:
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE:
    case DAPI_CMD_SYSTEM_DOT1X_CONFIG:
    case DAPI_CMD_LAGS_SYNCHRONIZE:
    case DAPI_CMD_QVLAN_PBVLAN_CONFIG:
    case DAPI_CMD_ADDR_FLUSH_ALL:
    case DAPI_CMD_ADDR_MAC_FILTER_ADD:
    case DAPI_CMD_ADDR_MAC_FILTER_DELETE:
    case DAPI_CMD_ADDR_AGING_TIME:
    case DAPI_CMD_ADDR_SYNC:
    case DAPI_CMD_QVLAN_VLAN_CREATE:
    case DAPI_CMD_QVLAN_VLAN_LIST_CREATE:
    case DAPI_CMD_QVLAN_VLAN_LIST_PURGE:
    case DAPI_CMD_QVLAN_VLAN_PORT_ADD:
    case DAPI_CMD_QVLAN_VLAN_PORT_DELETE:
    case DAPI_CMD_QVLAN_VLAN_PURGE:
    case DAPI_CMD_QVLAN_VLAN_STATS_GET:
    case DAPI_CMD_QVLAN_MCAST_FLOOD_SET:
    case DAPI_CMD_IPSUBNET_VLAN_CREATE:
    case DAPI_CMD_IPSUBNET_VLAN_DELETE:
    case DAPI_CMD_MAC_VLAN_CREATE:
    case DAPI_CMD_MAC_VLAN_DELETE:
    case DAPI_CMD_GVRP_GMRP_CONFIG:
    case DAPI_CMD_GARP_GROUP_REG_MODIFY:
    case DAPI_CMD_GARP_GROUP_REG_DELETE:
    case DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG:
    case DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG:
    case DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG:
    case DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG:
    case DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG:
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD:
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY:
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE:
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL:
    case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL:
    case DAPI_CMD_IPV6_ROUTE_ENTRY_ADD:
    case DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY:
    case DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE:
    case DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG:
    case DAPI_CMD_L3_TUNNEL_ADD:
    case DAPI_CMD_L3_TUNNEL_DELETE:
    case DAPI_CMD_L2_TUNNEL_VLAN_SET:
    case DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET:
    case DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG:
    case DAPI_CMD_PROTECTED_PORT_ADD:
    case DAPI_CMD_QOS_SYS_ACL_ADD:
    case DAPI_CMD_QOS_SYS_ACL_DELETE:
    case DAPI_CMD_QOS_VOIP_SESSION:
    case DAPI_CMD_QOS_VOIP_STATS_GET:
    case DAPI_CMD_INTF_BLINK_SET:
    case DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD:
    case DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE:
    case DAPI_CMD_QOS_ISCSI_CONNECTION_ADD:
    case DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE:
    case DAPI_CMD_QOS_ISCSI_COUNTER_GET:
    case DAPI_CMD_CONTROL_HW_APPLY_NOTIFY:
    case DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY:
    case DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION:
    case DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG:
    case DAPI_CMD_ADDR_FLUSH_VLAN:
    case DAPI_CMD_ADDR_FLUSH_MAC:
      if (dapiCpuUspGet(&dapiUsp) != L7_SUCCESS)
      {
        /*
        ** No CPU Present
        ** No Commands allowed until at the a
        ** logical CPU port has been created
        ** Just ignore command.  It will be
        ** configured at a later time.
        */
        result = L7_SUCCESS;
        break;
      }
      /*
      ** Fall through and treat and any other port
      ** after the USP has been updated to the CPU usp
      */
      /* NO break; */


    case DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG:
    case DAPI_CMD_INTF_DOSCONTROL_CONFIG:
    case DAPI_CMD_INTF_STP_STATE:
    case DAPI_CMD_INTF_SPEED_CONFIG:
    case DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG:
    case DAPI_CMD_INTF_LOOPBACK_CONFIG:
    case DAPI_CMD_INTF_ISOLATE_PHY_CONFIG:
    case DAPI_CMD_INTF_STATISTICS:
    case DAPI_CMD_INTF_FLOW_CONTROL:
    case DAPI_CMD_INTF_CABLE_STATUS_GET:
    case DAPI_CMD_INTF_FIBER_DIAG_GET:
    case DAPI_CMD_INTF_DOT1S_STATE:
    case DAPI_CMD_INTF_DOT1X_STATUS:
    case DAPI_CMD_INTF_DOT1X_CLIENT_ADD:
    case DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE:
    case DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT:
    case DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK:
    case DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK:
    case DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG:
    case DAPI_CMD_INTF_MAC_LOCK_CONFIG:
    case DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET:
    case DAPI_CMD_INTF_DOT1S_BPDU_FILTERING:
    case DAPI_CMD_INTF_DOT1S_BPDU_GUARD:
    case DAPI_CMD_INTF_DOT1S_BPDU_FLOOD:
    case DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG:
    case DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG:
    case DAPI_CMD_INTF_IPSG_CONFIG:
    case DAPI_CMD_INTF_IPSG_STATS_GET:
    case DAPI_CMD_INTF_IPSG_CLIENT_ADD:
    case DAPI_CMD_INTF_IPSG_CLIENT_DELETE:
    case DAPI_CMD_INTF_DOT3AH_CONFIG_ADD:
    case DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE:
    case DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG:
    case DAPI_CMD_INTF_DOT3AH_REM_LB_ADD:
    case DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI:
    case DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE:
    case DAPI_CMD_INTF_LLDP_CONFIG:
    case DAPI_CMD_INTF_SAMPLE_RATE:
    case DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG:
    case DAPI_CMD_INTF_PFC_CONFIG:
    case DAPI_CMD_INTF_PFC_STATS_GET:
    case DAPI_CMD_INTF_PFC_STATS_CLEAR:
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD:
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY:
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE:
    case DAPI_CMD_ADDR_FLUSH:
//  case DAPI_CMD_ADDR_SET_LEARN_MODE:            /* PTin added: MAC learning */
    case DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET:
    case DAPI_CMD_QVLAN_PORT_CONFIG:
    case DAPI_CMD_QVLAN_PORT_PRIORITY:
    case DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP:
    case DAPI_CMD_GARP_GVRP:
    case DAPI_CMD_GARP_GMRP:
    case DAPI_CMD_LOGICAL_INTF_CREATE:
    case DAPI_CMD_LOGICAL_INTF_DELETE:
    case DAPI_CMD_LAG_CREATE:
    case DAPI_CMD_LAG_PORT_ADD:
    case DAPI_CMD_LAG_PORT_DELETE:
    case DAPI_CMD_LAG_DELETE:
    case DAPI_CMD_LAG_HASHMODE_SET:
    case DAPI_CMD_AD_TRUNK_MODE_SET:
    case DAPI_CMD_ROUTING_ARP_ENTRY_ADD:
    case DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY:
    case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE:
    case DAPI_CMD_ROUTING_ARP_ENTRY_QUERY:
    case DAPI_CMD_ROUTING_INTF_ADD:
    case DAPI_CMD_ROUTING_INTF_MODIFY:
    case DAPI_CMD_ROUTING_INTF_DELETE:
    case DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG:
    case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD:
    case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE:
    case DAPI_CMD_ROUTING_INTF_VRID_ADD:
    case DAPI_CMD_ROUTING_INTF_VRID_DELETE:
    case DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET:
    case DAPI_CMD_ROUTING_INTF_MCAST_ADD:
    case DAPI_CMD_ROUTING_INTF_MCAST_DELETE:
    case DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET:
    case DAPI_CMD_ROUTING_INTF_MCAST_USE_GET:
    case DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY:
    case DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD:
    case DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE:
    case DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE:
    case DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET:
    case DAPI_CMD_IPV6_NEIGH_ENTRY_ADD:
    case DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY:
    case DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE:
    case DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY:
    case DAPI_CMD_IPV6_INTF_ADDR_ADD:
    case DAPI_CMD_IPV6_INTF_ADDR_DELETE:
    case DAPI_CMD_IPV6_INTF_LOCAL_MCASTADD:
    case DAPI_CMD_IPV6_INTF_LOCAL_MCASTDELETE:
    case DAPI_CMD_IPV6_INTF_STATS_GET:
    case DAPI_CMD_FRAME_SEND:
    case DAPI_CMD_QOS_DIFFSERV_INST_ADD:
    case DAPI_CMD_QOS_DIFFSERV_INST_DELETE:
    case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET:
    case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET:
    case DAPI_CMD_QOS_ACL_ADD:
    case DAPI_CMD_QOS_ACL_DELETE:
    case DAPI_CMD_QOS_ACL_RULE_COUNT_GET:
    case DAPI_CMD_QOS_ACL_RULE_STATUS_SET:
    case DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP:
    case DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP:
    case DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG:
    case DAPI_CMD_QOS_COS_INTF_CONFIG:
    case DAPI_CMD_QOS_COS_INTF_STATUS:
    case DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG:
    case DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG:
    case DAPI_CMD_QOS_VOIP_PROFILE:
    case DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD:
    case DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE:
    case DAPI_CMD_TUNNEL_CREATE:
    case DAPI_CMD_TUNNEL_DELETE:
    case DAPI_CMD_TUNNEL_NEXT_HOP_SET:
    case DAPI_CMD_PROTECTED_PORT_DELETE:
    case DAPI_CMD_VOICE_VLAN_DEVICE_ADD:
    case DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE:
    case DAPI_CMD_VOICE_VLAN_COS_OVERRIDE:
    case DAPI_CMD_VOICE_VLAN_AUTH:
    case DAPI_CMD_L2_TUNNEL_CREATE:
    case DAPI_CMD_L2_TUNNEL_DELETE:
    case DAPI_CMD_L2_TUNNEL_MODIFY:
    case DAPI_CMD_L2_DOT1AD_INTF_TYPE:
    case DAPI_CMD_L2_DOT1AD_INTF_STATS:
    case DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA:
    case DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG:
    case DAPI_CMD_INTF_LLPF_CONFIG:

    case DAPI_CMD_ISDP_INTF_STATUS_SET:
    case DAPI_CMD_DOT1AD_ADD:
    case DAPI_CMD_DOT1AD_DELETE:
    case DAPI_CMD_DOT1AD_RULE_COUNT_GET:

    /* PTin added: DAPI */
    case DAPI_CMD_PTIN_INIT:
    case DAPI_CMD_PTIN_PORTEXT:
    case DAPI_CMD_PTIN_COUNTERS_READ:
    case DAPI_CMD_PTIN_COUNTERS_CLEAR:
    case DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET:
    case DAPI_CMD_PTIN_VLAN_SETTINGS:
    case DAPI_CMD_PTIN_VLAN_DEFS:
    case DAPI_CMD_PTIN_VLAN_XLATE:
    case DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP:
    case DAPI_CMD_PTIN_VLAN_CROSSCONNECT:
    case DAPI_CMD_PTIN_HW_RESOURCES:
    case DAPI_CMD_PTIN_BW_POLICER:
    case DAPI_CMD_PTIN_FP_COUNTERS:
    case DAPI_CMD_PTIN_PACKET_RATE_LIMIT:
    case DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU:
    case DAPI_CMD_PTIN_PCS_PRBS:
//  case DAPI_CMD_SYSTEM_SNOOP_CONFIG:        /* For dealing with physical interfaces (not CPU)*/
//  case DAPI_CMD_SYSTEM_DHCP_CONFIG:         /* For dealing with physical interfaces (not CPU)*/

      /* This situation applies to all interfaces */
      if ((dapiUsp.unit == -1) && (dapiUsp.slot == -1) && (dapiUsp.port == -1))
      {
        if (dapiCpuUspGet(&dapiUsp) != L7_SUCCESS)
        {
          /* No CPU Present!
           * No Commands allowed until a logical
           * CPU port has been created.
           * Just ignore command.  It will be
           * configured at a later time.
           */
          result = L7_SUCCESS;
          break;
        }

        funcPtr = dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->port[dapiUsp.port]->cmdTable[cmd];
        result  = (*funcPtr)(&dapiUsp, cmd, data, dapi_g);
      }
    /* PTin end */
      else if (isValidSlot(&dapiUsp,dapi_g) != L7_TRUE)
      {
        if (cmd == DAPI_CMD_FRAME_SEND) {
          sysapiNetMbufFree(((DAPI_FRAME_CMD_t*)data)->cmdData.send.frameHdl);
        }
        result = L7_FAILURE;
      }
      /* Check to see if the card is physically present */
      else if (dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_FALSE)
      {
        static L7_uint32 bad_usp_count = 0;

        if (cmd == DAPI_CMD_FRAME_SEND) {
          sysapiNetMbufFree(((DAPI_FRAME_CMD_t*)data)->cmdData.send.frameHdl);
        }
        if ((bad_usp_count % 1000) == 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "Error on command %u: usp %d/%d/%d: Card Not Attached.",
                    cmd, dapiUsp.unit, dapiUsp.slot, dapiUsp.port);
        }
        bad_usp_count++;
        result = L7_SUCCESS;
      }
      else if (isValidUsp(&dapiUsp,dapi_g) == L7_FALSE)
      {
        if (cmd == DAPI_CMD_FRAME_SEND) {
          sysapiNetMbufFree(((DAPI_FRAME_CMD_t*)data)->cmdData.send.frameHdl);
        }
        /* Commands for invalid interfaces are silently ignored.
        */
        result = L7_SUCCESS;
      }
      else
      {
        funcPtr = dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->port[dapiUsp.port]->cmdTable[cmd];
        result  = (*funcPtr)(&dapiUsp, cmd, data, dapi_g);
      }
      break;

    case DAPI_CMD_CARD_INSERT:
      dapiCmd = (DAPI_CARD_MGMT_CMD_t*)data;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardCmdInfo.cardCmd    = dapiCmd->cardCmd;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cachePortConfig        = L7_FALSE;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->pendingAdminModeConfig = L7_FALSE;

      if (dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_FALSE)
      {
        if (dapiCardInsert(&dapiUsp, cmd, data) == L7_FAILURE) {
          result = L7_FAILURE;
        }
        else {
          dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent = L7_TRUE;
        }
      }
      else
      {
        /* Card Already Inserted */
          result = L7_SUCCESS;
        }
      break;

    case DAPI_CMD_CARD_REMOVE:
      dapiCmd = (DAPI_CARD_MGMT_CMD_t*)data;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardCmdInfo.cardCmd    = dapiCmd->cardCmd;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cachePortConfig        = L7_FALSE;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->pendingAdminModeConfig = L7_FALSE;

      if (dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE)
      {
        if (dapiCardRemove(&dapiUsp, cmd, data) == L7_FAILURE) {
          result = L7_FAILURE;
        }
        else {
          dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent = L7_FALSE;
        }
      }
      else
      {
        /* Card Not Present */
          result = L7_SUCCESS;
        }
      break;

    case DAPI_CMD_CARD_PLUGIN:
    case DAPI_CMD_CARD_UNPLUG:

      dapiCmd = (DAPI_CARD_MGMT_CMD_t*)data;
      dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardCmdInfo.cardCmd = dapiCmd->cardCmd;

      if ((dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE) && (dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardType == SYSAPI_CARD_TYPE_LINE))
      {
        funcPtr = dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->port[dapiUsp.port]->cmdTable[cmd];
        result  = (*funcPtr)(&dapiUsp, cmd, data, dapi_g);
      }
      else
      {
        result = L7_FAILURE;
      }
      break;

    default:
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nThe dapi command # %d:%s \nwas left out of the dapiCtl case statement\nPlease add it!\n",
                    cmd,dapi_cmd_name_g[cmd]);
      result = L7_FAILURE;
      break;
    }
  }
  OSAPI_TRACE_EVENT (L7_TRACE_EVENT_DRIVER_EXIT, &result, sizeof(result));
  delta = osapiTimeMillisecondsGet() - delta;
  dapiTraceDapiCtl(cmd,usp,L7_FALSE,delta);
  dapiCmdProfile(cmd, L7_FALSE);

  return result;
}

/*********************************************************************
*
* @purpose Initializes the device driver to receive card inits
*
* @param  stackUnitID   initialization information
*
* @param  cpuBoardID    the identification number of the board
*
* @returns L7_RC_t result
*
* @notes This function can only be called once per boot
*
* @end
*
*********************************************************************/
L7_RC_t dapiInit(L7_ulong32 cardId)
{
  L7_RC_t                       result = L7_SUCCESS;
  void                         *memPtr;
  size_t                        memSize;
  DAPI_USP_t                    usp;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_BASE_CARD_ENTRY_t       *dapiBaseCardInfoPtr;

  dapiTraceInit(DAPI_TRACE_ENTRY_MAX, DAPI_TRACE_ENTRY_SIZE_MAX);
  dapiProfileInit();
  /* Card removal semaphore is used to prevent card removals while device driver
  ** callbacks are in progress.
  */
  if (osapiRWLockCreate(&dapiCardRemovalRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    sysapiPrintf("FAILURE (%s: %d) Code: 0x%08LX\n", __FILE__, __LINE__,
                 (DAPI_ERROR_GENERAL | cardId));
    return result;
  }
  dapiBaseCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  /*
   * Allocate main DAPI structure, dapi_g
   */
  memSize = sizeof(DAPI_t);
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
  if (memPtr == L7_NULLPTR)
  {
    sysapiPrintf("FAILURE (%s: %d) Code: 0x%08LX\n", __FILE__, __LINE__,
                 (DAPI_ERROR_NO_MEM | memSize));
    return result;
  }
  memset(memPtr, 0x00,memSize);
  dapi_g = (DAPI_t *)memPtr;

  /*
   * Initialize Name Database
   */
  memSize = sizeof(DAPI_NAME_t);
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
  if (memPtr == L7_NULLPTR)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
    return result;
  }
  memset(memPtr, 0x00, memSize);
  dapi_g->name = (DAPI_NAME_t *)memPtr;

  dapi_g->name->family = dapi_family_name_g;
  dapi_g->name->cmd    = dapi_cmd_name_g;

  memSize = sizeof(DAPI_FAMILY_t) * DAPI_NUM_OF_CMDS;
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);

  if (memPtr == L7_NULLPTR)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
    return result;
  }

  memset(memPtr, 0x00, memSize);
  dapi_g->name->cmdToFamilyTable = (DAPI_FAMILY_t *)memPtr;

  result = dapiCmdToFamilyTableInit(dapi_g);

  if (result != L7_SUCCESS)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_GENERAL | result);
    return result;
  }

  else {
      result = DAPI_DEBUG_INIT(dapi_g);
      if (result != L7_SUCCESS)
      {
          DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_GENERAL | result);
          return result;
      }
  }


  /*
   * System Allocate
   */
  memSize = sizeof(DAPI_SYSTEM_t);
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);

  if (memPtr == L7_NULLPTR)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
    return result;
  }
  memset(memPtr, 0x00, memSize);
  dapi_g->system = (DAPI_SYSTEM_t *)memPtr;

  /*
   * System Init
   */
  /* Add one to the unit to support unit 0 */
  dapi_g->system->totalNumOfUnits = platUnitTotalMaxPerStackGet() + 1;


  /*
   * Allocate Callbacks
   */
  memSize = sizeof(DAPI_CALLBACK_t) * DAPI_NUM_OF_FAMILIES;
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
  if (memPtr == L7_NULLPTR)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
    return result;
  }
  memset(memPtr, 0x00, memSize);
  dapi_g->system->familyCallback = (DAPI_CALLBACK_t *)memPtr;

  /*
   * Unit Structures Allocate
   */
  memSize = sizeof(DAPI_UNIT_t *) * dapi_g->system->totalNumOfUnits;
  memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
  if (memPtr == L7_NULLPTR)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
    return result;
  }
  memset(memPtr, 0x00, memSize);
  dapi_g->unit = (DAPI_UNIT_t **)memPtr;

  for (usp.unit=0;usp.unit<dapi_g->system->totalNumOfUnits;usp.unit++)
  {
    /*
     * Unit Allocate
     */
    memSize = sizeof(DAPI_UNIT_t);
    memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
    if (memPtr == L7_NULLPTR)
    {
      DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
      return result;
    }
    memset(memPtr, 0x00, memSize);
    dapi_g->unit[usp.unit] = (DAPI_UNIT_t *)memPtr;

    dapi_g->unit[usp.unit]->numOfSlots = platSlotTotalMaxPerUnitGet();

    /*
     * Slot Structures Allocate
     */
    memSize = sizeof(DAPI_SLOT_t *) * dapi_g->unit[usp.unit]->numOfSlots;
    memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
    if (memPtr == L7_NULLPTR)
    {
      DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
      return result;
    }
    memset(memPtr, 0x00, memSize);
    dapi_g->unit[usp.unit]->slot = (DAPI_SLOT_t **)memPtr;

    for (usp.slot=0; usp.slot<dapi_g->unit[usp.unit]->numOfSlots; usp.slot++)
    {
      memSize = sizeof(DAPI_SLOT_t);
      memPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, (L7_uint32)memSize);
      if (memPtr == L7_NULLPTR)
      {
        DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | memSize);
        return result;
      }
      memset(memPtr, 0x00, memSize);
      dapi_g->unit[usp.unit]->slot[usp.slot] = (DAPI_SLOT_t *)memPtr;

      dapi_g->unit[usp.unit]->slot[usp.slot]->cardCmdInfo.cardCmd    = CARD_CMD_LAST;
      dapi_g->unit[usp.unit]->slot[usp.slot]->cachePortConfig        = L7_FALSE;
      dapi_g->unit[usp.unit]->slot[usp.slot]->pendingAdminModeConfig = L7_FALSE;

      if (osapiRWLockCreate(&(dapi_g->unit[usp.unit]->slot[usp.slot]->cardCmdInfo.cardConfigSema), OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
      {
        LOG_ERROR(usp.slot);
      }
    }
  }

  if ((dapiBaseCardInfoPtr->hapiBaseCardInsert(cardId,dapi_g)) == L7_FAILURE)
  {
    DAPI_ERROR_MSG(dapi_g, DAPI_ERROR_NO_MEM | cardId);
    return result;
  }

  dapi_g->initialized = L7_TRUE;

  return result;
}

/*********************************************************************
*
* @purpose Initializes one card at a time
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
L7_RC_t dapiCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data)
{
  static const char            *routine_name = "dapiCardInsert()";
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_CARD_MGMT_CMD_t         *cmdInfo = (DAPI_CARD_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *dapiPortPtr;
  L7_ushort16                   portInfoIndex;
  L7_ushort16                   i;
  DAPI_USP_t                    usp;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardInfoPtr;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(cmdInfo->cmdData.cardInsert.cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  dapiCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  /* Save CardType so it can be recalled from HPC when the driver needs it */
  dapi_g->unit[usp.unit]->slot[usp.slot]->cardType = sysapiHpcCardInfoPtr->type;

  /* Save CardTypeID so it can be recalled from HPC when the driver needs it */
  dapi_g->unit[usp.unit]->slot[usp.slot]->cardId = cmdInfo->cmdData.cardInsert.cardId;

  if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_LOGICAL_CPU)
  {
    if (dapiCpuCardInsert(&usp,cmd,data) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiCpuCardInsert'\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
  }
  else if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_LAG)
  {
    if (dapiLagCardInsert(&usp,cmd,data) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiLagCardInsert'\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
  }
  else if ((dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_VLAN_ROUTER))
  {
    L7_RC_t rc;
    rc = dapiLogicalCardInsert(SYSAPI_CARD_TYPE_VLAN_ROUTER, &usp, cmd, data);
    if (rc == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiLogicalCardInsert' (vlan)\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
  }
  else if ((dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_LOOPBACK))
  {
    L7_RC_t rc;
    rc = dapiLogicalCardInsert(SYSAPI_CARD_TYPE_LOOPBACK, &usp, cmd, data);
    if (rc == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiLogicalCardInsert' (loopbk)\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
  }
  else if ((dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_TUNNEL))
  {
    L7_RC_t rc;
    rc = dapiLogicalCardInsert(SYSAPI_CARD_TYPE_TUNNEL, &usp, cmd, data);
    if (rc == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiLogicalCardInsert' (tunnel)\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
  }
  else if ((dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_CAPWAP_TUNNEL))
  {
    L7_RC_t rc;
    rc = dapiLogicalCardInsert(SYSAPI_CARD_TYPE_CAPWAP_TUNNEL, &usp, cmd, data);
    if (rc == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'dapiLogicalCardInsert' (capwap tunnel)\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }

  }
  else
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = sysapiHpcCardInfoPtr->numOfNiPorts;

    /*
     * Ports
     */
    dapi_g->unit[usp.unit]->slot[usp.slot]->port = (DAPI_PORT_t**)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(DAPI_PORT_t*)*dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot);
    if (dapi_g->unit[usp.unit]->slot[usp.slot]->port == L7_NULLPTR)
    {
      LOG_ERROR (0);
    }

    for (portInfoIndex=0; portInfoIndex < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; portInfoIndex++)
    {
      usp.port = dapiCardInfoPtr->portMap[portInfoIndex].portNum;

      dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] = (DAPI_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(DAPI_PORT_t));
      if (dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] == L7_NULLPTR)
      {
        LOG_ERROR (0);
      }

      dapiPortPtr = DAPI_PORT_GET(&usp,dapi_g);

      /* memory for stats */
      dapiPortPtr->modeparm.physical.stats = (L7_ulong64*)osapiMalloc(L7_DRIVER_COMPONENT_ID, DAPI_STATS_NUM_OF_INTF_ENTRIES * sizeof(L7_ulong64));
      if (dapiPortPtr->modeparm.physical.stats == L7_NULLPTR)
      {
        LOG_ERROR (0);
      }
      else
      {
        memset(dapiPortPtr->modeparm.physical.stats,0x00, (DAPI_STATS_NUM_OF_INTF_ENTRIES * sizeof(L7_ulong64)));
      }

      /* Setup Initial States */
      dapiPortPtr->type                         = sysapiHpcCardInfoPtr->portInfo[portInfoIndex].type;
      dapiPortPtr->connectorType                = sysapiHpcCardInfoPtr->portInfo[portInfoIndex].connectorType;
      dapiPortPtr->phyCapabilities              = sysapiHpcCardInfoPtr->portInfo[portInfoIndex].phyCapabilities;
      dapiPortPtr->modeparm.physical.isLinkUp   = L7_FALSE;
      dapiPortPtr->statsEnable                  = L7_TRUE;
    }
  }

/* Added to initialize all commands in cmdTable to L7_FAILURE */
  for (usp.port=0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp,dapi_g);

    /* initialize the cmdTable so it contains all L7_FAILUREs */
    for (i=0;i<DAPI_NUM_OF_CMDS;i++)
    {
      dapiPortPtr->cmdTable[i] = (HAPICTLFUNCPTR_t)dapiFuncError;
    }
  }

  if ((dapiCardInfoPtr->hapiCardInsert)(&usp, cmd, data, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
#if 0
    /* Card may not get inserted due to a USL error. Don't scare the user with this
    ** message.
    */
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s Card Initialization Failed on unit: %d, slot: %d\n",
                   __FILE__, __LINE__, routine_name, usp.unit, usp.slot);
#endif

    /* Free DAPI memory allocated for the line cards.
    */
    if ((dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_LINE))
    {
      for (portInfoIndex=0; portInfoIndex < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; portInfoIndex++)
      {
        usp.port = dapiCardInfoPtr->portMap[portInfoIndex].portNum;
        dapiPortPtr = DAPI_PORT_GET(&usp,dapi_g);
        osapiFree (L7_DRIVER_COMPONENT_ID,  dapiPortPtr->modeparm.physical.stats);

        osapiFree (L7_DRIVER_COMPONENT_ID, dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]);
      }

      osapiFree (L7_DRIVER_COMPONENT_ID, dapi_g->unit[usp.unit]->slot[usp.slot]->port);
    }
    return result;
  }

  return result;
}

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
L7_RC_t dapiCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data)
{
  static const char            *routine_name = "dapiCardRemove()";
  L7_RC_t                       result = L7_SUCCESS;
  DAPI_USP_t                    usp;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardInfoPtr;

  DAPI_PORT_t           *dapiPortPtr;



  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(dapi_g->unit[usp.unit]->slot[usp.slot]->cardId);

  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  dapiCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  dapiCardRemovalWriteLockTake ();
  /* Invalidate the card before deallocating memory. This insures that callbacks
  ** will not attempt to use hapi structures which have been freed.
  */
  dapi_g->unit[dapiUsp->unit]->slot[dapiUsp->slot]->cardPresent = L7_FALSE;


  /* Note that we don't have to hold the semaphore any longer because
  ** setting cardPresent to L7_FALSE will invalidate the USPs on that card.
  */
  dapiCardRemovalWriteLockGive ();

  /* Uninitialize the hapi driver first */
  if ((dapiCardInfoPtr->hapiCardRemove)(&usp, cmd, data, dapi_g) == L7_FAILURE)
  {
    LOG_ERROR (sysapiHpcCardInfoPtr->cardTypeId);
    result = L7_FAILURE;
  }

  /*
   * Ports
   */
  for (usp.port = 0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    if (IS_SLOT_TYPE_LOGICAL_LAG(&usp, dapi_g) == L7_TRUE)
    {
      osapiSemaDelete(dapiPortPtr->modeparm.lag.lagmembersetSemaID);
    }

    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
    {
      if (dapiPortPtr->modeparm.physical.stats != L7_NULLPTR)
      {
        osapiFree(L7_DRIVER_COMPONENT_ID, dapiPortPtr->modeparm.physical.stats);
      }
    }

    if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_TRUE)
    {
      if (dapiPortPtr->modeparm.cpu.stats != L7_NULLPTR)
      {
        osapiFree(L7_DRIVER_COMPONENT_ID, dapiPortPtr->modeparm.cpu.stats);
      }
      if (dapiPortPtr->modeparm.cpu.statsSema != L7_NULL)
      {
        osapiSemaDelete(dapiPortPtr->modeparm.cpu.statsSema);
        dapiPortPtr->modeparm.cpu.statsSema = L7_NULL;
      }
    }

    if (dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]) {
      osapiFree(L7_DRIVER_COMPONENT_ID, dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]);
    }
  }
  if (dapi_g->unit[usp.unit]->slot[usp.slot]->port) {
    osapiFree(L7_DRIVER_COMPONENT_ID, dapi_g->unit[usp.unit]->slot[usp.slot]->port);
  }
  /* clear values for next card */
  /* Note: cardPresent state transitions are always handled in dapiCtl */
  dapi_g->unit[usp.unit]->slot[usp.slot]->cardId           = 0;
  dapi_g->unit[usp.unit]->slot[usp.slot]->cardType         = SYSAPI_CARD_TYPE_NOT_PRESENT;
  dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = 0;
  dapi_g->unit[usp.unit]->slot[usp.slot]->port             = L7_NULLPTR;
  dapi_g->unit[usp.unit]->slot[usp.slot]->intLevel         = 0;


  return result;
}
/* End Function Declarations */



/*********************************************************************
*
* @purpose Initalizes dapi layer to support logical ports
*
* @param  unitNum     unit number for logical port
* @param  slotNum     slot number for logical port
* @param *dapi_g      pointer to system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiCpuCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data)
{
  static const char            *routine_name = "dapiCpuCardInsert()";
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_USP_t                    usp;
  DAPI_PORT_t                  *dapiPortPtr;
  DAPI_CARD_MGMT_CMD_t         *cmdInfo = (DAPI_CARD_MGMT_CMD_t*)data;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardInfoPtr;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(cmdInfo->cmdData.cardInsert.cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  dapiCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_LOGICAL_CPU)
  {
    /* Only use the slot entries for the number of ports in the case of the cpu interface
    ** The applications can only know about one cpu interface so for the driver to support
    ** more than one, we must of the number of entries in the slot map for the cpu's which
    ** is seperate from the physical port slot maps due to the fact that the cpu's are not
    ** associtated with a slot physically but still have a physical mapping
    */
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = dapiCardInfoPtr->numOfSlotMapEntries;
  }
  else
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->cardId       = (L7_ulong32) NULL;
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = 0;
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s Invalid line card ID\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  dapi_g->unit[usp.unit]->slot[usp.slot]->port = (DAPI_PORT_t**)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(DAPI_PORT_t*)*dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot);
  if (dapi_g->unit[usp.unit]->slot[usp.slot]->port == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'osapiMalloc'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] = (DAPI_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(DAPI_PORT_t));
    if (dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] == L7_NULLPTR)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'osapiMalloc'\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }

    dapiPortPtr = DAPI_PORT_GET(&usp,dapi_g);

    /* only malloc memory for stats on the first CPU interface */
    if (usp.port == 0)
    {
      dapiPortPtr->modeparm.cpu.stats = (L7_ulong64*)osapiMalloc(L7_DRIVER_COMPONENT_ID, DAPI_STATS_NUM_OF_INTF_ENTRIES * sizeof(L7_ulong64));
      if (dapiPortPtr->modeparm.cpu.stats == L7_NULLPTR)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                       "%s %d: In %s call to 'osapiMalloc'\n",
                       __FILE__, __LINE__, routine_name);
        return result;
      }
      else
      {
        memset(dapiPortPtr->modeparm.cpu.stats,0x00,(DAPI_STATS_NUM_OF_INTF_ENTRIES * sizeof(L7_ulong64)));
      }
      dapiPortPtr->modeparm.cpu.statsSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
      if (dapiPortPtr->modeparm.cpu.statsSema == L7_NULL)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                       "%s %d: In %s call to 'osapiSemaMCreate'\n",
                       __FILE__, __LINE__, routine_name);
        return result;
      }
      dapiPortPtr->statsEnable = L7_TRUE;
    }
    dapiPortPtr->type = L7_IANA_OTHER_CPU;
  }

  return result;
}

/*********************************************************************
*
* @purpose Initalizes dapi layer to support logical ports
*
* @param  unitNum     unit number for logical port
* @param  slotNum     slot number for logical port
* @param *dapi_g      pointer to system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiLagCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data)
{
  static const char            *routine_name = "dapiLagCardInsert()";
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_USP_t                    usp;
  DAPI_CARD_MGMT_CMD_t         *cmdInfo = (DAPI_CARD_MGMT_CMD_t*)data;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(cmdInfo->cmdData.cardInsert.cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardId == L7_LOGICAL_CARD_LAG_ID)
  {
    /* Logical are considered physical in the driver */
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot  = L7_MAX_NUM_LAG_INTF;
  }
  else
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->cardId       = (L7_ulong32) NULL;
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = 0;
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s Invalid line card ID\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  dapi_g->unit[usp.unit]->slot[usp.slot]->port = (DAPI_PORT_t**)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                                            sizeof(DAPI_PORT_t*)*dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot);
  if (dapi_g->unit[usp.unit]->slot[usp.slot]->port == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'osapiMalloc'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] = (DAPI_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                                                       sizeof(DAPI_PORT_t));
    if (dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] == L7_NULLPTR)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'osapiMalloc'\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }
    memset(dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port],0x00,sizeof(DAPI_PORT_t));

    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->type = L7_IANA_LAG_DESC;

    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->statsEnable   = L7_FALSE;

    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->modeparm.lag.lagmembersetSemaID = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY,OSAPI_SEM_FULL);
  }

  return result;
}

/*********************************************************************
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
*********************************************************************/
L7_RC_t dapiLogicalCardInsert(SYSAPI_CARD_TYPE_t cardType, DAPI_USP_t *dapiUsp,
                              DAPI_CMD_t cmd, void *data)
{
  static const char *routine_name = "dapiLogicalCardInsert()";
  L7_RC_t result = L7_SUCCESS;
  DAPI_USP_t usp;
  DAPI_CARD_MGMT_CMD_t *cmdInfo = (DAPI_CARD_MGMT_CMD_t*)data;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  L7_uint32 expectedCardId;
  L7_uint32 portsInSlot;
  L7_IANA_INTF_TYPE_t ianaType;
  DAPI_PORT_t **portListPtr;

  switch (cardType)
  {
    case SYSAPI_CARD_TYPE_VLAN_ROUTER:
      expectedCardId = L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID;
      portsInSlot = L7_MAX_NUM_VLAN_INTF;
      ianaType = L7_IANA_L2_VLAN;
      break;

    case SYSAPI_CARD_TYPE_LOOPBACK:
      expectedCardId = L7_LOGICAL_CARD_LOOPBACK_INTF_ID;
      portsInSlot = L7_MAX_NUM_LOOPBACK_INTF;
      ianaType = L7_IANA_SOFTWARE_LOOPBACK;
      break;

    case SYSAPI_CARD_TYPE_TUNNEL:
      expectedCardId = L7_LOGICAL_CARD_TUNNEL_INTF_ID;
      portsInSlot = L7_MAX_NUM_TUNNEL_INTF;
      ianaType = L7_IANA_TUNNEL;
      break;

    case SYSAPI_CARD_TYPE_CAPWAP_TUNNEL:
      expectedCardId = L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID;
      portsInSlot = L7_MAX_NUM_CAPWAP_TUNNEL_INTF;
      ianaType = L7_IANA_CAPWAP_TUNNEL;
      break;

    default:
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                    "%s %d: passed invalid cardType (%u) to %s\n",
                    __FILE__, __LINE__, cardType, routine_name);
      return result;
  }

  sysapiHpcCardInfoPtr =
    sysapiHpcCardDbEntryGet(cmdInfo->cmdData.cardInsert.cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardId == expectedCardId)
  {
    /* Logical are considered physical in the driver */
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = portsInSlot;
  }
  else
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->cardId = (L7_ulong32) NULL;
    dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot = 0;
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s Invalid line card ID\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  portListPtr = (DAPI_PORT_t**)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                           sizeof(DAPI_PORT_t*)*portsInSlot);
  dapi_g->unit[usp.unit]->slot[usp.slot]->port = portListPtr;
  if (portListPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'osapiMalloc'\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  for (usp.port = 0; usp.port < portsInSlot; usp.port++)
  {
    DAPI_PORT_t *portPtr = (DAPI_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                     sizeof(DAPI_PORT_t));
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port] = portPtr;
    if (portPtr == L7_NULLPTR)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "%s %d: In %s call to 'osapiMalloc'\n",
                     __FILE__, __LINE__, routine_name);
      return result;
    }

    memset(portPtr, 0x00, sizeof(DAPI_PORT_t));
    portPtr->type = ianaType;
    portPtr->statsEnable = L7_FALSE;
  }

  return result;
}

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
L7_RC_t dapiCallback(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *cmdInfo)
{
  L7_RC_t       result = L7_SUCCESS;
  DAPI_USP_t          dapiUsp;
  DAPI_FAMILY_t       familyIndex;
  DAPI_CALLBACK_t     cbFuncPtr;

  dapiUsp.unit = usp->unit;
  dapiUsp.slot = usp->slot;
  dapiUsp.port = usp->port;

  if (family == 0)
  {
    familyIndex = dapi_g->name->cmdToFamilyTable[cmd];
  }
  else
  {
    familyIndex = family;
  }

  if (dapi_g->system->familyCallback[familyIndex] != NULL)
  {
    cbFuncPtr = dapi_g->system->familyCallback[familyIndex];
    dapiTraceDapiCallback(event,usp,L7_TRUE,result);
    DAPI_DEBUG_CALLBACK_MSG(&dapiUsp, familyIndex, cmd, event, cmdInfo, dapi_g);
    result  = (*cbFuncPtr)(&dapiUsp, familyIndex, cmd, event, cmdInfo);
    dapiTraceDapiCallback(event,usp,L7_FALSE,result);
    DAPI_DEBUG_CALLBACK_RESPONSE_MSG(&dapiUsp, familyIndex, cmd, event, cmdInfo, result, dapi_g);
  }

  return result;
}

L7_BOOL isValidSlot(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_BOOL result = L7_TRUE;

  /* coverity defects, such as 74797 */
  if (usp->unit < 0)
    return L7_FALSE;

  if (usp->unit >= dapi_g->system->totalNumOfUnits)
  {
    result = L7_FALSE;
    return result;
  }
  else if (usp->slot >= dapi_g->unit[usp->unit]->numOfSlots)
  {
    result = L7_FALSE;
    return result;
  }
  return result;
}

L7_BOOL isValidUsp(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_BOOL result = L7_TRUE;

  if ((usp->unit >= dapi_g->system->totalNumOfUnits) || (usp->unit < 0))
  {
    result = L7_FALSE;
    return result;
  }
  else if ((usp->slot >= dapi_g->unit[usp->unit]->numOfSlots) || (usp->slot < 0))
  {
    result = L7_FALSE;
    return result;
  }
  else if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent != L7_TRUE)
  {
    result = L7_FALSE;
    return result;
  }
  else if ((usp->port >= dapi_g->unit[usp->unit]->slot[usp->slot]->numOfPortsInSlot) || (usp->port < 0))
  {
    result = L7_FALSE;
    return result;
  }
  else if ((dapi_g->unit[usp->unit]->slot[usp->slot]->cardType == SYSAPI_CARD_TYPE_LINE) &&
           (spmFpsPortStackingModeCheck (usp->unit, usp->slot, (usp->port + 1)) == L7_TRUE))
  {
    result = L7_FALSE;
    return result;
  }
  return result;
}

L7_RC_t dapiCtlLagIntf(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data)
{
  DAPI_LAG_ENTRY_t     savedlagmemberset[L7_MAX_MEMBERS_PER_LAG];
  L7_ushort16          entry;
  L7_RC_t        result = L7_FAILURE;
  HAPICTLFUNCPTR_t     funcPtr;
  DAPI_FRAME_CMD_t     *frameCmdData;

  osapiSemaTake(dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->modeparm.lag.lagmembersetSemaID, L7_WAIT_FOREVER);

  memcpy(savedlagmemberset, dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->modeparm.lag.memberSet, L7_MAX_MEMBERS_PER_LAG * sizeof (DAPI_LAG_ENTRY_t));

  osapiSemaGive(dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->modeparm.lag.lagmembersetSemaID);

  /* needs to execute on the first active and inUse physical port */
  if (cmd == DAPI_CMD_FRAME_SEND)
  {
    /* savedlagmemberset = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->modeparm.lag.memberSet; */

    for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
    {
      if (savedlagmemberset[entry].inUse == L7_TRUE)
      {
        funcPtr = dapi_g->unit[savedlagmemberset[entry].usp.unit]->slot[savedlagmemberset[entry].usp.slot]->port[savedlagmemberset[entry].usp.port]->cmdTable[cmd];
        result  = (*funcPtr)(&savedlagmemberset[entry].usp, cmd, data, dapi_g);

        return result;
      }
    }

    if (entry == L7_MAX_MEMBERS_PER_LAG)
    {
      frameCmdData = (DAPI_FRAME_CMD_t*)data;

      sysapiNetMbufFree(frameCmdData->cmdData.send.frameHdl);
      return L7_SUCCESS;
    }
  }
  /* needs to execute on the lag interface itself */
  else if ((cmd == DAPI_CMD_LAG_CREATE)             ||
           (cmd == DAPI_CMD_LAG_PORT_ADD)           ||
           (cmd == DAPI_CMD_LAG_PORT_DELETE)        ||
           (cmd == DAPI_CMD_LAG_DELETE)             ||
           (cmd == DAPI_CMD_QVLAN_VLAN_PORT_ADD)    ||
           (cmd == DAPI_CMD_QVLAN_VLAN_PORT_DELETE) ||
           (cmd == DAPI_CMD_QVLAN_PORT_CONFIG)      ||
           (cmd == DAPI_CMD_QVLAN_PORT_PRIORITY)    ||
           (cmd == DAPI_CMD_INTF_STP_STATE)         ||
           (cmd == DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD) ||
           (cmd == DAPI_CMD_GARP_GVRP)                     ||
           (cmd == DAPI_CMD_GARP_GMRP)                     ||
           (cmd == DAPI_CMD_INTF_DOT1S_STATE)              ||
           (cmd == DAPI_CMD_ADDR_FLUSH)                    ||
           (cmd == DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE) ||
           (cmd == DAPI_CMD_INTF_MAC_LOCK_CONFIG))
  {
    funcPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->cmdTable[cmd];
    result  = (*funcPtr)(usp, cmd, data, dapi_g);
  }
  /* needs to execute on every active and inUse physical member of the lag */
  else
  {
    /*savedlagmemberset = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->modeparm.lag.memberSet;  */

    for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
    {
      if (savedlagmemberset[entry].inUse == L7_TRUE)
      {
        funcPtr = dapi_g->unit[savedlagmemberset[entry].usp.unit]->slot[savedlagmemberset[entry].usp.slot]->port[savedlagmemberset[entry].usp.port]->cmdTable[cmd];
        result  = (*funcPtr)(&savedlagmemberset[entry].usp, cmd, data, dapi_g);
      }
    }
  }
  return result;
}



L7_RC_t dapiCmdToFamilyTableInit(DAPI_t *dapi_g)
{
  L7_ulong32 index;

  for (index=0; index < DAPI_NUM_OF_CMDS; index++)
  {
    if (strncmp(&dapi_g->name->cmd[index].name[9],"CONTROL",7) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_CONTROL;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"CARD",4) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_CARD;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"SYSTEM",6) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_SYSTEM;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"INTF",4) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_INTF_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"LOGICAL_INTF",12) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_LOGICAL_INTF_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"LAG",3) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_LAG_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"AD_TRUNK",8) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_LAG_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"LAG_HASHMODE",7) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_LAG_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"ADDR",4) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_ADDR_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"QVLAN",5) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_QVLAN_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"GARP",4) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_GARP_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"GVRP",4) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_GARP_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"ROUTING_ROUTE",13) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_ROUTING_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"ROUTING_ARP",11) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_ROUTING_ARP_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"ROUTING_INTF",12) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_ROUTING_INTF_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"ROUTING_INTF_MCAST",18) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_ROUTING_INTF_MCAST;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"FRAME",5) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_FRAME;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"QOS_DIFFSERV",12) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_QOS_DIFFSERV_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"QOS_ACL",7) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_QOS_ACL_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9],"QOS_COS",7) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_QOS_COS_MGMT;
    }
    else if (strncmp(&dapi_g->name->cmd[index].name[9], "SERVICES",8) == 0) {
      dapi_g->name->cmdToFamilyTable[index] = DAPI_FAMILY_SERVICES;
    }
    else {
      dapi_g->name->cmdToFamilyTable[index] = L7_NULL;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Initializes Function table so it contains
*
* @param none
*
* @returns L7_ushort16 status
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiFuncError(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
/*static const char *routine_name = "dapiFuncError()";*/

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "DAPI: Command %4.36s not supported in this build\r\n",dapi_g->name->cmd[cmd].name);

  return(L7_SUCCESS);
}

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
                            DAPI_t *dapi_g)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "DAPI: Command %4.36s not supported for usp %u/%u/%u\n",
                dapi_g->name->cmd[cmd].name,
                usp->unit, usp->slot, usp->port);

  return(L7_FAILURE);
}

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
                              DAPI_t *dapi_g)
{
  return(L7_SUCCESS);
}

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
                              DAPI_t *dapi_g)
{
  return(L7_FAILURE);
}

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
                              void *handle)
{
  L7_ushort16 result = L7_SUCCESS;
  DAPI_USP_t usp;
  DAPI_PORT_t *dapiPortPtr;
  DAPI_t *dapi_g = (DAPI_t*)handle;
  int i;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  /*
   * Se the port functions to log a message and return an error by default.
   * Then fill in the individual operations that we do need to support.
   */
  for (usp.port = 0;
       usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp,dapi_g);
    dapiPortPtr->hapiPort = L7_NULLPTR;
    for (i = 0; i < DAPI_NUM_OF_CMDS; i++)
    {
      dapiPortPtr->cmdTable[i] = (HAPICTLFUNCPTR_t)dapiFuncInvalidIntf;
    }

    /*
     * Due to the use of the DAPI_CMD_INTF_SPEED_CONFIG function
     * to get link status, we need to support that function even
     * though most upper-level code is supposed to understand that
     * logical interfaces do not have a "speed" attribute.  We
     * return a failure in this case to indicated that the interface
     * is down and let the uppers layer code handle up/down transitions.
     */
    dapiPortPtr->cmdTable[DAPI_CMD_INTF_SPEED_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnFailure;

    /*
     * This is used as a part of the generic "set admin state"
     * of the interface and the general version of this I saw
     * bugged out with a success for non-physical interfaces.
     * This essentially does the same thing in a more forthright
     * manner.
     */
    dapiPortPtr->cmdTable[DAPI_CMD_INTF_ISOLATE_PHY_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;

    /*
     * This is called from some of the basic NIM interface state
     * management code that really should be rewritten to be
     * more careful about the types of interfaces that it is
     * operating on.
     */
    dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1S_STATE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;

    /*
     * Allow routing interface operations
     */
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_ADD] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MODIFY] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_DELETE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_ADD] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_DELETE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_BOOTP_DHCP_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_ICMP_REDIRECTS_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_MCAST_FORWARDING_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_MCAST_IGMP_CONFIG] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;

    /*
     * Allow NDP operations
     */
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_ADD] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;

    /*
     * Allow tunnel operations
     */
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_CREATE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_DELETE] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_NEXT_HOP_SET] =
      (HAPICTLFUNCPTR_t)dapiFuncReturnSuccess;
  }

  return result;
}

/*********************************************************************
*
* @purpose Registers a callback for a particular family
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dapiControlCallbackRegister(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result          = L7_SUCCESS;
  DAPI_CONTROL_CALLBACK_CMD_t *callbackInfoPtr = (DAPI_CONTROL_CALLBACK_CMD_t*)data;

  dapi_g->system->familyCallback[callbackInfoPtr->family] = callbackInfoPtr->funcPtr;

  return(result);
}

L7_RC_t dapiCpuUspGet(DAPI_USP_t *usp)
{
  usp->unit = 0xFF;
  usp->slot = 0xFF;
  usp->port = 0xFFFF;

  for (usp->unit=0; (usp->unit < dapi_g->system->totalNumOfUnits); usp->unit++)
  {
    for (usp->slot=0; (usp->slot<dapi_g->unit[usp->unit]->numOfSlots); usp->slot++)
    {
      if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_TRUE)
      {
        if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardType == SYSAPI_CARD_TYPE_LOGICAL_CPU)
        {
          usp->port = 0;
          return L7_SUCCESS;
        }
      }
    }
  }
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose Take the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns L7_RC_t
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigReadLockTake(L7_int8 unit, L7_int8 slot)
{
  L7_RC_t result = L7_FAILURE;

  if ((unit >= dapi_g->system->totalNumOfUnits) || (unit < 0))
  {
    result = L7_FAILURE;
  }
  else if ((slot >= dapi_g->unit[unit]->numOfSlots) || (slot < 0))
  {
    result = L7_FAILURE;
  }
  else
  {
    result = osapiReadLockTake(dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardConfigSema,
                               L7_WAIT_FOREVER);
  }

  if (result != L7_SUCCESS)
  {
    LOG_ERROR(slot);
  }

  return result;
}

/*********************************************************************
*
* @purpose Release the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns L7_RC_t
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigReadLockGive(L7_int8 unit, L7_int8 slot)
{
  L7_RC_t result = L7_FAILURE;

  if ((unit >= dapi_g->system->totalNumOfUnits) || (unit < 0))
  {
    result = L7_FAILURE;
  }
  else if ((slot >= dapi_g->unit[unit]->numOfSlots) || (slot < 0))
  {
    result = L7_FAILURE;
  }
  else
  {
    result = osapiReadLockGive(dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardConfigSema);
  }

  if (result != L7_SUCCESS)
  {
    LOG_ERROR(slot);
  }

  return result;
}

/*********************************************************************
*
* @purpose Take the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns L7_RC_t
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigWriteLockTake(L7_int8 unit, L7_int8 slot)
{
  L7_RC_t result = L7_FAILURE;

  if ((unit >= dapi_g->system->totalNumOfUnits) || (unit < 0))
  {
    result = L7_FAILURE;
  }
  else if ((slot >= dapi_g->unit[unit]->numOfSlots) || (slot < 0))
  {
    result = L7_FAILURE;
  }
  else
  {
    result = osapiWriteLockTake(dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardConfigSema,
                                L7_WAIT_FOREVER);
  }

  if (result != L7_SUCCESS)
  {
    LOG_ERROR(slot);
  }

  return result;
}

/*********************************************************************
*
* @purpose Release the DAPI card configuration semaphore
*
* @param   unit: Unit number of the card
* @param   slot: Slot number of the card

* @returns L7_RC_t
*
* @notes   There is a separate semaphore for each card. This semaphore
*          protects the critical section during batch configuration of
*          various ports of the card.
*
* @end
*
*********************************************************************/
L7_RC_t dapiCardConfigWriteLockGive(L7_int8 unit, L7_int8 slot)
{
  L7_RC_t result = L7_FAILURE;

  if ((unit >= dapi_g->system->totalNumOfUnits) || (unit < 0))
  {
    result = L7_FAILURE;
  }
  else if ((slot >= dapi_g->unit[unit]->numOfSlots) || (slot < 0))
  {
    result = L7_FAILURE;
  }
  else
  {
    result = osapiWriteLockGive(dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardConfigSema);
  }

  if (result != L7_SUCCESS)
  {
    LOG_ERROR(slot);
  }

  return result;
}

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
L7_BOOL dapiCardPortConfigCache(L7_int8 unit, L7_int8 slot)
{
  L7_BOOL result = L7_FALSE;

  if ((unit >= dapi_g->system->totalNumOfUnits) || (unit < 0))
  {
    result = L7_FALSE;
  }
  else if ((slot >= dapi_g->unit[unit]->numOfSlots) || (slot < 0))
  {
    result = L7_FALSE;
  }
  else if (dapi_g->unit[unit]->slot[slot]->cardPresent != L7_TRUE)
  {
    result = L7_FALSE;
  }
  else
  {
    if (((dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardCmd == CARD_CMD_PLUGIN_START) ||
        (dapi_g->unit[unit]->slot[slot]->cardCmdInfo.cardCmd == CARD_CMD_UNPLUG_START)) &&
        (dapi_g->unit[unit]->slot[slot]->cachePortConfig == L7_TRUE))
      result = L7_TRUE;
  }

  return result;
}

