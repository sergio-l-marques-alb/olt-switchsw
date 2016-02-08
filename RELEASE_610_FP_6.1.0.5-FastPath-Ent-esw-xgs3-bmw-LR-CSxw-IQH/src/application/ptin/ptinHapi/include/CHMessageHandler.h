/*
 * CHMessageHandler.h
 *
 *  Created on: 2010/04/09
 *      Author: Andre Brizido
 */

#ifndef CHMESSAGEHANDLER_H_
#define CHMESSAGEHANDLER_H_

// ==============================================
// Identificação das mensagens
// ==============================================

#define CCMSG_DEBUG_SHOW                    0x0100
#define CCMSG_DEBUG_CONFIG                  0x0101
#define CCMSG_RESET_ALARMS                  0x200E
#define CCMSG_RESET_DEFAULTS                0x200F
#define CCMSG_BOARD_SHOW                    0x2010
                                            
#define CCMSG_APPLICATION_IS_ALIVE          0x9000
#define CCMSG_APPLICATION_RESOURCES         0x9001
#define CCMSG_ETH_SHELL_CMD                 0x9002
#define CCMSG_ETH_SET_PHY_CONFIG            0x9003
#define CCMSG_ETH_GET_PHY_CONFIG            0x90A1
#define CCMSG_ETH_GET_PHY_STATE             0x90A2
                                            
#define CCMSG_ETH_PORT_TYPE_SET             0x9008
                                            
#define CCMSG_ETH_PORT_COS_CONFIG_SET       0x9010
#define CCMSG_ETH_PORT_COS_CONFIG_GET       0x9011
                                            
#define CCMSG_ETH_MAC_DYNAMIC_TABLE         0x9020
#define CCMSG_ETH_MAC_STATIC_TABLE          0x9021
#define CCMSG_ETH_MAC_ENTRY_REMOVE          0x9022
#define CCMSG_ETH_MAC_ENTRY_ADD             0x9023

#define CCMSG_ETH_IGMP_CHANNEL_WLIST_GET    0x906D  // struct msg_MCChannelWList_t
#define CCMSG_ETH_IGMP_CHANNEL_WLIST_ADD    0x906E  // struct msg_MCChannelWList_t
#define CCMSG_ETH_IGMP_CHANNEL_WLIST_REMOVE 0x906F  // struct msg_MCChannelWList_t


#define CCMSG_ETH_GET_COUNTERS              0x9201
#define CCMSG_ETH_CLEAR_COUNTERS            0x9202
#define CCMSG_ETH_GET_VLAN_COUNTERS         0x9203
#define CCMSG_ETH_GET_FLOW_COUNTERS_ABS     0x9204
#define CCMSG_ETH_GET_FLOW_COUNTERS_DIFF    0x9205
#define CCMSG_ETH_ADD_FLOW_COUNTERS         0x9206
#define CCMSG_ETH_REMOVE_FLOW_COUNTERS      0x9207
                                            
/* Deprecated */
#define CCMSG_ETH_MC_FLOW_ADD               0x9401
#define CCMSG_ETH_MC_FLOW_REMOVE            0x9402
#define CCMSG_ETH_MC_CLIENT_ADD             0x9404
#define CCMSG_ETH_MC_CLIENT_REMOVE          0x9405
#define CCMSG_ETH_MC_CLIENT_STATS_SHOW      0x940A
#define CCMSG_ETH_MC_PORT_STATS_SHOW        0x940B
#define CCMSG_ETH_MC_CLIENT_STATS_CLEAR     0x940C
#define CCMSG_ETH_MC_PORT_STATS_CLEAR       0x940D
#define CCMSG_ETH_DHCP_CLIENT_STATS_SHOW    0x9410
#define CCMSG_ETH_DHCP_PORT_STATS_SHOW      0x9411
#define CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR   0x9412
#define CCMSG_ETH_DHCP_PORT_STATS_CLEAR     0x9413
#define CCMSG_ETH_DHCP_BIND_TABLE_SHOW      0x9415
#define CCMSG_ETH_DHCP_BIND_TABLE_ADD       0x9416
#define CCMSG_ETH_DHCP_BIND_TABLE_REMOVE    0x9417

#define CCMSG_ETH_MC_STATIC_CHANNEL_ADD     0x9420
#define CCMSG_ETH_MC_STATIC_CHANNEL_DEL     0x9421
#define CCMSG_ETH_MC_ACTIVE_CHANNELS_GET    0x9422    /* Obtem a lista de canais MC */
#define CCMSG_ETH_MC_CHANNEL_CLIENTS_GET    0x9423    

#define CCMSG_ETH_EVC_READ                  0x9100
#define CCMSG_ETH_EVC_CREATE                0x9102
#define CCMSG_ETH_EVC_DESTROY               0x9103
#define CCMSG_ETH_PROFILE_SET               0x910A
#define CCMSG_ETH_PROFILE_REMOVE            0x910B
#define CCMSG_ETH_PROFILE_GET               0x910C
                                            
#define CCMSG_ETH_DHCP_PROFILE_GET          0x9110
#define CCMSG_ETH_DHCP_PROFILE_CONFIG       0x9111
#define CCMSG_ETH_DHCP_PROFILE_REMOVE       0x9112
#define CCMSG_ETH_DHCP_ACCESS_NODE_ID_SET   0x9113
                                            
#define CCMSG_ETH_SNOOP_MODE                0x9501
/* Deprecated */
#define CCMSG_ETH_SNOOP_INTF_MANGMT         0x9503
#define CCMSG_ETH_SNOOP_VLAN_MANGMT         0x9505
#define CCMSG_ETH_SNOOP_QUERIER_MANGMT      0x9507
                                            
#define CCMSG_ETH_LACP_LAG_SHOW             0x9600
#define CCMSG_ETH_LACP_LAG_CREATE           0x9601
#define CCMSG_ETH_LACP_LAG_DESTROY          0x9602
#define CCMSG_ETH_LACP_LAG_STATUS           0x9603
#define CCMSG_ETH_LACP_ADMINSTATE_SET       0x9604
#define CCMSG_ETH_LACP_ADMINSTATE_GET       0x9605
#define CCMSG_ETH_LACP_STATS_SHOW           0x9610
#define CCMSG_ETH_LACP_STATS_CLEAR          0x9611
                                            
#define CCMSG_ETH_CHANGE_STDOUT             0x9300

#define CCMSG_ETH_NTW_CONNECTIVITY_SHOW     0x9F00
#define CCMSG_ETH_NTW_CONNECTIVITY          0x9F01
#define CCMSG_ETH_ROUTE_CONNECTIVITY        0x9F02

#define CCMSG_ETH_SWITCH_SET                0x9A00    /* Aging time */


// ==================================================================
// Handler de Mensagens recebidas das aplicacoes Cliente
// ==================================================================

int global_var_trap_sent;
int global_var_system_ready;

int CHMessageHandler (ipc_msg *inbuffer, ipc_msg *outbuffer);

#endif /* CHMESSAGEHANDLER_H_ */
