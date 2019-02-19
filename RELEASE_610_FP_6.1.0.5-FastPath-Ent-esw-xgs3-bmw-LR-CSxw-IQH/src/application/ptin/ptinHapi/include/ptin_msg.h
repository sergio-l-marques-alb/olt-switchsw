#ifndef _PTIN_MSG__H
#define _PTIN_MSG__H

#include "datatypes.h"
#include "dtlinclude.h"

#include "ptin/globaldefs.h"

// Resources availability
L7_RC_t ptin_msg_HwResources(st_msg_ptin_hw_resources *rsc);

// In-band connectivity

extern L7_RC_t ptin_msg_showNtwConnectivity(L7_NtwConnectivity *connection);

extern L7_RC_t ptin_msg_ntwConnectivity(L7_NtwConnectivity *connection);

extern L7_RC_t ptin_msg_routeConnectivity(L7_RouteConnectivity *connection);

// DHCP, option 82

extern L7_RC_t ptin_msg_dhcpOpt82Get(L7_HwEthernetDhcpOpt82Profile *profile);

extern L7_RC_t ptin_msg_dhcpOpt82AccessNodeUpdate(L7_HwEthernetAccessNodeId *accessnode_msg);

extern L7_RC_t ptin_msg_dhcpOpt82Config(L7_HwEthernetDhcpOpt82Profile *profile);

extern L7_RC_t ptin_msg_dhcpOpt82Remove(L7_HwEthernetDhcpOpt82Profile *profile);

extern L7_RC_t ptin_msg_dhcp_bindtable_get(L7_DHCP_bind_table *ptr);

extern L7_RC_t ptin_msg_dhcp_bindtable_remove(L7_DHCP_bind_table *ptr);

// LACP configurations

extern L7_RC_t ptin_msg_LAGGet(L7_LACPLagInfo *lag_info, L7_uint8 *nLags);

extern L7_RC_t ptin_msg_LAGCreate(L7_LACPLagInfo *lag_info);

extern L7_RC_t ptin_msg_LAGDestroy(L7_LACPLagInfo *lag_info);

extern L7_RC_t ptin_msg_LAGStatus(L7_LACPLagStatus *lag_status, L7_uint8 *nLags);

extern L7_RC_t ptin_msg_LAGAdminStateSet( L7_LACPAdminState *adminState, L7_uint8 nStructs );

extern L7_RC_t ptin_msg_LAGAdminStateGet( L7_int port, L7_LACPAdminState *adminState, L7_uint8 *nStructs );

extern L7_RC_t ptin_msg_LACPduStat_get( L7_int port, L7_LACPStats *stat, L7_uint8 *nStructs );

extern L7_RC_t ptin_msg_LACPduStat_clr( L7_LACPStats *stat );

// IGMP snooping configurations

extern L7_RC_t ptin_msg_snooping_querier_config(st_snooping *snoop);

extern L7_RC_t ptin_msg_snooping_intf_config_single(L7_uint8 port, enum_intf_type type, st_snooping_cfg *cfg);

extern L7_RC_t ptin_msg_snooping_intf_config(st_snooping_intf *snoop);

extern L7_RC_t ptin_msg_snooping_vlan_config(st_snooping_vlan *snoop, L7_uint16 number_of_vlans);

extern L7_RC_t ptin_msg_snooping_querier_vlan_config(st_snooping_querier_vlan *snoop, L7_uint16 number_of_vlans);

extern L7_RC_t ptin_msg_snooping_querier_vlan_getconfig(st_snooping_querier_vlan *snoop, L7_uint16 number_of_vlans);

// IGMP statistics management

extern L7_RC_t ptin_msg_IgmpFlowAdd(L7_ClientIgmp *flow, L7_uint16 number_of_clients );

extern L7_RC_t ptin_msg_IgmpFlowRemove(L7_ClientIgmp *flow, L7_uint16 number_of_clients );

extern L7_RC_t ptin_msg_IgmpClientAdd(L7_ClientIgmp *client, L7_uint16 number_of_clients );

extern L7_RC_t ptin_msg_IgmpClientRemove(L7_ClientIgmp *client, L7_uint16 number_of_clients );

extern L7_RC_t ptin_msg_IgmpClientStatsGet(L7_ClientIgmpStatistics *client);

extern L7_RC_t ptin_msg_IgmpClientStatsClear(L7_ClientIgmpStatistics *client);

extern L7_RC_t ptin_msg_IgmpPortStatsGet(L7_ClientIgmpStatistics *stats);

extern L7_RC_t ptin_msg_IgmpPortStatsClear(L7_ClientIgmpStatistics *stats);

#ifdef IGMP_WHITELIST_FEATURE
extern L7_RC_t ptin_msg_IGMP_WList_add(msg_MCChannelWList_t *channel_list, L7_uint16 n_channels);

extern L7_RC_t ptin_msg_IGMP_WList_remove(msg_MCChannelWList_t *channel_list, L7_uint16 n_channels);
#endif

extern L7_RC_t ptin_msg_MCStaticChannel_add(L7_MCStaticChannel *channel, L7_uint8 nStructs);

extern L7_RC_t ptin_msg_MCStaticChannel_del(L7_MCStaticChannel *channel, L7_uint8 nStructs);

extern L7_RC_t ptin_msg_MCActiveChannelsList_get(L7_MCActiveChannels *channels);

extern L7_RC_t ptin_msg_MCChannelClientsList_get(L7_MCActiveChannelClients *clients);

// DHCP statistics

extern L7_RC_t ptin_msg_DhcpClientStatsGet(L7_ClientDhcpStatistics *client);

extern L7_RC_t ptin_msg_DhcpClientStatsClear(L7_ClientDhcpStatistics *client);

extern L7_RC_t ptin_msg_DhcpPortStatsGet(L7_ClientDhcpStatistics *stats);

extern L7_RC_t ptin_msg_DhcpPortStatsClear(L7_ClientDhcpStatistics *stats);

// EVC's management

extern L7_RC_t ptin_msg_EVCread(L7_HwEthernetMef10EvcBundling *evcCfg);

extern L7_RC_t ptin_msg_EVCcreate(L7_HwEthernetMef10EvcBundling *evcCfg);

extern L7_RC_t ptin_msg_EVCdestroy(L7_HwEthernetMef10p1EvcRemove *evcCfg);


// BW profiles management

extern L7_RC_t ptin_msg_GetProfile(L7_HwEthernetProfile *dapiCmd, L7_uint16 *nProfiles);

extern L7_RC_t ptin_msg_SetProfile(L7_HwEthernetProfile *dapiCmd);

extern L7_RC_t ptin_msg_RemoveProfile(L7_HwEthernetProfile *dapiCmd);


// PHY configuration ans state management

extern L7_RC_t ptin_msg_setPhyConfig(L7_HWEthPhyConf *dapiCmd);

extern L7_RC_t ptin_msg_getPhyConfig(L7_HWEthPhyConf *phy_conf);

extern L7_RC_t ptin_msg_getPhyState(L7_HWEthPhyState *phy_state);

extern L7_RC_t ptin_msg_mac_table_get(L7_switch_mac_table *ptr, L7_BOOL static_entry);

extern L7_RC_t ptin_msg_mac_table_entry_remove(L7_switch_mac_table *ptr);

extern L7_RC_t ptin_msg_mac_table_entry_add(L7_switch_mac_operation *ptr);

// Interface and vlan counters management

extern L7_RC_t ptin_msg_getCounters(L7_HWEthRFC2819_PortStatistics *stat);

extern L7_RC_t ptin_msg_getVlanCounters(L7_HWEth_VlanStatistics *stat, L7_uint16 *nClients);

extern L7_RC_t ptin_msg_clearCounters(L7_HWEthRFC2819_PortStatistics *stat);

extern L7_RC_t ptin_msg_getFlowCounters(st_ptin_flow_counters *ptr);

extern L7_RC_t ptin_msg_addFlowCounters(st_ptin_flow_counters *ptr);

extern L7_RC_t ptin_msg_removeFlowCounters(st_ptin_flow_counters *ptr);

// QoS processing

extern L7_RC_t ptin_msg_qos_config_set(L7_QoSConfiguration *qos, L7_uint8 n_cells);

extern L7_RC_t ptin_msg_qos_config_get(L7_QoSConfiguration *qos, L7_uint8 *n_cells);

// Defaults

extern L7_RC_t ptin_msg_reset_alarms(void);

extern L7_RC_t ptin_msg_set_defaults(L7_uint32 param);

// More

extern L7_RC_t ptin_msg_Shell_Command(L7_char8 *str);

#endif // _PTIN_MSG__H
