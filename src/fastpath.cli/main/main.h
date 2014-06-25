#ifndef __MAIN__H_
#define __MAIN__H_

#include "globals.h"

extern int canal_buga;

// retirado do ficheiro  u-boot-1.1.x/include/pci_ids.h
#define PCI_VENDOR_ID_BROADCOM          0x14e4

#define PTIN_SYSTEM_MAX_N_PORTS   64
#define PTIN_SYSTEM_MAX_N_LAGS    PTIN_SYSTEM_MAX_N_PORTS
#define PTIN_SYSTEM_N_PORTS       18
#define PTIN_SYSTEM_N_LAGS        PTIN_SYSTEM_N_PORTS
#define PTIN_SYSTEM_N_INTERF      (PTIN_SYSTEM_N_PORTS+PTIN_SYSTEM_N_LAGS)

#ifndef min
  #define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
  #define max(x, y) (((x) < (y)) ? (y) : (x))
#endif

/*****************************************************************************
 * IPC Messages codes 
 *****************************************************************************/
#define CCMSG_APP_CHANGE_STDOUT             0x91F0  // (To be used with fastpath.cli)
#define CCMSG_APP_SHELL_CMD_RUN             0x91F1  // (To be used with fastpath.cli)
#define CCMSG_APP_LOGGER_OUTPUT             0x91F2  // (To be used with fastpath.cli)

#define CCMSG_APPLICATION_IS_ALIVE          0x9000  // (To be used with fastpath.cli)
#define CCMSG_BOARD_SHOW                    0x9001  // struct msg_FWFastpathInfo
#define CCMSG_ALARMS_RESET                  0x9002  // No struct
#define CCMSG_DEFAULTS_RESET                0x9003  // No struct
#define CCMSG_APPLICATION_RESOURCES         0x9004  // struct msg_ptin_policy_resources
#define CCMSG_MULTICAST_MACHINE_RESET       0x9005  // struct msg_HwGenReq_t
#define CCMSG_HW_BOARD_ACTION               0x9007	// struct msg_HwGenReq_t
#define CCMSG_HW_LINK_ACTION                0x9008	// struct msg_HwGenReq_t

#define CCMSG_ETH_PHY_CONFIG_SET            0x9010  // struct msg_HWEthPhyConf_t
#define CCMSG_ETH_PHY_CONFIG_GET            0x9011  // struct msg_HWEthPhyConf_t
#define CCMSG_ETH_PHY_STATE_GET             0x9012  // struct msg_HWEthPhyState_t
#define CCMSG_ETH_PHY_COUNTERS_GET          0x9013  // struct msg_HWEthRFC2819_PortStatistics_t
#define CCMSG_ETH_PHY_COUNTERS_CLEAR        0x9014  // struct msg_HWEthRFC2819_PortStatistics_t

#define CCMSG_ETH_PORT_EXT_SET              0x9015  // struct msg_HWPortExt_t
#define CCMSG_ETH_PORT_EXT_GET              0x9016  // struct msg_HWPortExt_t
#define CCMSG_ETH_PORT_MAC_SET              0x9017  // struct msg_HWPortExt_t
#define CCMSG_ETH_PORT_MAC_GET              0x9018  // struct msg_HWPortExt_t

#define CCMSG_ETH_LACP_LAG_GET              0x9020  // struct msg_LACPLagInfo_t
#define CCMSG_ETH_LACP_LAG_ADD              0x9021  // struct msg_LACPLagInfo_t
#define CCMSG_ETH_LACP_LAG_REMOVE           0x9022  // struct msg_LACPLagInfo_t
#define CCMSG_ETH_LACP_LAG_STATUS_GET       0x9023  // struct msg_LACPLagStatus_t
#define CCMSG_ETH_LACP_ADMINSTATE_SET       0x9024  // struct msg_LACPAdminState_t
#define CCMSG_ETH_LACP_ADMINSTATE_GET       0x9025  // struct msg_LACPAdminState_t
#define CCMSG_ETH_LACP_STATS_GET            0x9026  // struct msg_LACPStats_t
#define CCMSG_ETH_LACP_STATS_CLEAR          0x9027  // struct msg_LACPStats_t

#define CCMSG_ETH_EVC_GET                   0x9030  // struct msg_HwEthMef10Evc_t
#define CCMSG_ETH_EVC_ADD                   0x9031  // struct msg_HwEthMef10Evc_t
#define CCMSG_ETH_EVC_REMOVE                0x9032  // struct msg_HwEthMef10EvcRemove_t
#define CCMSG_ETH_EVC_BRIDGE_ADD            0x9033  // struct msg_HwEthEvcBridge_t
#define CCMSG_ETH_EVC_BRIDGE_REMOVE         0x9034  // struct msg_HwEthEvcBridge_t
#define CCMSG_ETH_EVC_FLOW_GET              0x9039  // struct msg_HwEthEvcFlow_t
#define CCMSG_ETH_EVC_FLOW_ADD              0x903A  // struct msg_HwEthEvcFlow_t
#define CCMSG_ETH_EVC_FLOW_REMOVE           0x903B  // struct msg_HwEthEvcFlow_t
#define CCMSG_ETH_EVC_PORT_ADD              0x903C  // struct msg_HWevcPort_t
#define CCMSG_ETH_EVC_PORT_REMOVE           0x903D  // struct msg_HWevcPort_t

#define CCMSG_ETH_EVC_COUNTERS_GET          0x9040  // struct msg_evcStats_t
#define CCMSG_ETH_EVC_COUNTERS_ADD          0x9041  // struct msg_evcStats_t
#define CCMSG_ETH_EVC_COUNTERS_REMOVE       0x9042  // struct msg_evcStats_t

#define CCMSG_ETH_BW_PROFILE_SET            0x9050  // struct msg_HwEthBwProfileData_t
#define CCMSG_ETH_BW_PROFILE_DELETE         0x9051  // struct msg_HwEthBwProfileData_t
#define CCMSG_ETH_BW_PROFILE_GET            0x9052  // struct msg_HwEthBwProfileData_t

#define CCMSG_ETH_STORM_CONTROL_GET         0x9054  // struct msg_HwEthStormControl_t
#define CCMSG_ETH_STORM_CONTROL_RESET       0x9055  // struct msg_HwEthStormControl_t
#define CCMSG_ETH_STORM_CONTROL_SET         0x9056  // struct msg_HwEthStormControl_t
#define CCMSG_ETH_STORM_CONTROL_CLEAR       0x9057  // struct msg_HwEthStormControl_t

#define CCMSG_ETH_NTW_CONNECTIVITY_GET      0x9060  // struct msg_NtwConnectivity_t
#define CCMSG_ETH_NTW_CONNECTIVITY_SET      0x9061  // struct msg_NtwConnectivity_t

/* To be tested */

/* To be implemented */

#define CCMSG_ETH_IGMP_CLIENT_STATS_GET     0x9077  // struct msg_IgmpClientStatistics_t
#define CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR   0x9078  // struct msg_IgmpClientStatistics_t
#define CCMSG_ETH_IGMP_INTF_STATS_GET       0x9079  // struct msg_IgmpClientStatistics_t
#define CCMSG_ETH_IGMP_INTF_STATS_CLEAR     0x907A  // struct msg_IgmpClientStatistics_t

#define CCMSG_ETH_IGMP_CHANNEL_ASSOC_GET    0x906A  // struct msg_MCAssocChannel_t
#define CCMSG_ETH_IGMP_CHANNEL_ASSOC_ADD    0x906B  // struct msg_MCAssocChannel_t
#define CCMSG_ETH_IGMP_CHANNEL_ASSOC_REMOVE 0x906C  // struct msg_MCAssocChannel_t

#define CCMSG_ETH_IGMP_STATIC_GROUP_ADD     0x907B  // struct msg_MCStaticChannel_t
#define CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE  0x907C  // struct msg_MCStaticChannel_t
#define CCMSG_ETH_IGMP_GROUPS_GET           0x907D  // struct msg_MCActiveChannels_t
#define CCMSG_ETH_IGMP_CLIENT_GROUPS_GET    0x907E  // struct msg_MCActiveChannelClients_t

#define CCMSG_ETH_PORT_COS_GET              0x9090  // struct msg_QoSConfiguration_t
#define CCMSG_ETH_PORT_COS_SET              0x9091  // struct msg_QoSConfiguration_t

#define CCMSG_ETH_SWITCH_CONFIG_GET         0x90A0  // struct msg_switch_config_t
#define CCMSG_ETH_SWITCH_CONFIG_SET         0x90A1  // struct msg_switch_config_t
#define CCMSG_ETH_MAC_TABLE_SHOW            0x90A2  // struct msg_switch_mac_table_t
#define CCMSG_ETH_MAC_ENTRY_REMOVE          0x90A3  // struct msg_switch_mac_table_t
#define CCMSG_ETH_MAC_ENTRY_ADD             0x90A4  // struct msg_switch_mac_table_t

#define CCMSG_ETH_DHCP_PROFILE_GET          0x90C0  // struct msg_HwEthernetDhcpOpt82Profile_t
#define CCMSG_ETH_DHCP_PROFILE_ADD          0x90C1  // struct msg_HwEthernetDhcpOpt82Profile_t
#define CCMSG_ETH_DHCP_PROFILE_REMOVE       0x90C2  // struct msg_HwEthernetDhcpOpt82Profile_t
#define CCMSG_ETH_DHCP_CLIENT_STATS_GET     0x90C3  // struct msg_ClientDhcpStatistics_t
#define CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR   0x90C4  // struct msg_ClientDhcpStatistics_t
#define CCMSG_ETH_DHCP_INTF_STATS_GET       0x90C5  // struct msg_ClientDhcpStatistics_t
#define CCMSG_ETH_DHCP_INTF_STATS_CLEAR     0x90C6  // struct msg_ClientDhcpStatistics_t
#define CCMSG_ETH_DHCP_BIND_TABLE_GET       0x90C7  // struct msg_DHCP_bind_table_t
#define CCMSG_ETH_DHCP_BIND_TABLE_REMOVE    0x90C8  // struct msg_DHCP_bind_table_t

#define CCMSG_ETH_IPSG_ENABLE               0x90D0  // struct msg_IPSG_enable_t
#define CCMSG_ETH_IPSG_STATIC_ENTRY         0x90D1  // struct msg_IPSG_static_entry_t
#define CCMSG_ETH_IPSG_BINDING_TABLE_GET    0x90D2  // struct msg_IPSG_binding_table_request_t

#define CCMSG_ETH_IGMP_PROXY_GET            0x9070  // struct msg_IgmpProxyCfg_t
#define CCMSG_ETH_IGMP_PROXY_SET            0x9071  // struct msg_IgmpProxyCfg_t
#define CCMSG_ETH_IGMP_ENTRY_ADD            0x9073  // struct msg_IgmpMultcastUnicastLink_t
#define CCMSG_ETH_IGMP_ENTRY_REMOVE         0x9074  // struct msg_IgmpMultcastUnicastLink_t
#define CCMSG_ETH_IGMP_CLIENT_ADD           0x9075  // struct msg_IgmpClient_t
#define CCMSG_ETH_IGMP_CLIENT_REMOVE        0x9076  // struct msg_IgmpClient_t

#define CCMSG_ETH_PCS_PRBS_ENABLE           0x9080  // Enable PRBS tx/rx
#define CCMSG_ETH_PCS_PRBS_STATUS           0x9081  // PRBS lock and number of errors

#define CHMSG_ETH_UPLINK_COMMAND            0x9116  // Uplink protection command from Mx (fw control): struct msg_uplinkProtCmd

/* Routing */
#define CCMSG_ROUTING_INTF_CREATE             0x9151  // msg_RoutingIntfCreate
#define CCMSG_ROUTING_INTF_MODIFY             0x9152  // msg_RoutingIntfModify
#define CCMSG_ROUTING_INTF_REMOVE             0x9153  // msg_RoutingIntfRemove
#define CCMSG_ROUTING_ARPTABLE_GET            0x9154  // msg_RoutingArpTableRequest / msg_RoutingArpTableResponse
#define CCMSG_ROUTING_ARPENTRY_PURGE          0x9155  // msg_RoutingArpEntryPurge
#define CCMSG_ROUTING_ROUTETABLE_GET          0x9156  // msg_RoutingRouteTableRequest / msg_RoutingRouteTableResponse
#define CCMSG_ROUTING_STATICROUTE_ADD         0x9157  // msg_RoutingStaticRoute
#define CCMSG_ROUTING_STATICROUTE_DELETE      0x9158  // msg_RoutingStaticRoute
#define CCMSG_ROUTING_PINGSESSION_CREATE      0x9159  // msg_RoutingPingSessionCreate
#define CCMSG_ROUTING_PINGSESSION_QUERY       0x915A  // msg_RoutingPingSessionQuery
#define CCMSG_ROUTING_PINGSESSION_FREE        0x915B  // msg_RoutingPingSessionFree
#define CCMSG_ROUTING_TRACERTSESSION_CREATE   0x915C  // msg_RoutingTracertSessionCreate
#define CCMSG_ROUTING_TRACERTSESSION_QUERY    0x915D  // msg_RoutingTracertSessionQuery
#define CCMSG_ROUTING_TRACERTSESSION_GETHOPS  0x915E  // msg_RoutingTracertSessionHopsRequest / msg_RoutingTracertSessionHopsResponse
#define CCMSG_ROUTING_TRACERTSESSION_FREE     0x915F  // msg_RoutingTracertSessionFree

#define CCMSG_SLOT_MAP_MODE_GET             0x91E0  // struct msg_slotModeCfg_t
#define CCMSG_SLOT_MAP_MODE_VALIDATE        0x91E1  // struct msg_slotModeCfg_t
#define CCMSG_SLOT_MAP_MODE_APPLY           0x91E2  // struct msg_slotModeCfg_t


/* Fastpath typedefs */
typedef uint8    L7_uint8;
typedef uint16   L7_uint16;
typedef uint32   L7_uint32;
typedef int32    L7_int32;
typedef uint64   L7_uint64;


/*****************************************************************************
 * Structures exchanged on the messages
 * NOTE: By default they are 'packed' to avoid empty gaps
 *****************************************************************************/

typedef struct {
  L7_uint8  slot_id;    // Slot ID
  L7_uint8  generic_id; // ID used for any purpose (context of each message)
  L7_uint8  type;       // Field used for any purpose
  L7_uint8  param;      // Generic 8 bit mask
} __attribute__((packed)) msg_HwGenReq_t;

typedef struct {
  L7_uint8  intf_type;  // Interface type: { 0-Physical, 1-Logical (LAG) }
  L7_uint8  intf_id;    // Interface Id# (phy ports / LAGs)
} __attribute__((packed)) msg_HwEthInterface_t;

typedef struct L7_in_addr_s
{
    L7_uint32   s_addr;    /* 32 bit IPv4 address in network byte order */
} __attribute__((packed)) msg_in_addr_t;

#define PTIN_AF_INET    0
#define PTIN_AF_INET6   1
typedef struct chmessage_ip_addr_s {
   L7_uint8       family;     /* IP traffic type: 0=IPv4; 1:IPv6 */
   union
   {
      L7_uint32   ipv4;       /* 32 bit IPv4 address in network byte order */
      L7_uint8    ipv6[16];   /* 128 bit IPv6 address in network byte order */
   }__attribute__((packed)) addr;
}__attribute__((packed)) chmessage_ip_addr_t;

/* Client identification */
#define MSG_CLIENT_OVLAN_MASK 0x01
#define MSG_CLIENT_IVLAN_MASK 0x02
#define MSG_CLIENT_INTF_MASK  0x04
typedef struct
{
  L7_uint8 mask;
  L7_uint16 outer_vlan;       /* [mask=0x01] Outer vlan */
  L7_uint16 inner_vlan;       /* [mask=0x02] Inner vlan */
  msg_HwEthInterface_t intf;  /* [mask=0x04] interface  */
} __attribute__((packed)) msg_client_info_t;

/* Structure used on response to CCMSG_DEBUG_SHOW message */
/* Deprecated */
#if 0
#define TTY_MAX_LEN         100
typedef struct {
  L7_uint32  mask;
  L7_uint32  traceoutput;
  L7_uint32  tracemode;
  L7_uint32  tracemask;
  L7_char8   tty[TTY_MAX_LEN];
} __attribute__((packed)) msg_Debug_t;
#endif

/* Misc structs ***************************************************************/

// Board show
// Message CCMSG_BOARD_SHOW
typedef struct  {
  L7_uint8     SlotIndex;                // slot id
  L7_uint32    BoardPresent;             // 1 - means FastPath is loaded
  L7_uint32    BoardNInterfaces;         // (not used)                         
  L7_uint32    BoardVersion;             // (not used)              
  L7_uint32    BoardMode;                // (not used) 
  L7_uint8     BoardSerialNumber[20];    // (FastPath version)                   
  L7_uint32    FpgaFixoID;               // (not used)                
  L7_uint32    FpgaFixoVersion;          // (not used)                     
  L7_uint32    FpgaFlexID;               // (not used)                
  L7_uint32    FpgaFlexVersion;          // (not used)                    
  L7_uint32    PCBVersion;               // (not used)
} __attribute__((packed)) msg_FWFastpathInfo;

/***************************************************** 
 * PORT CONFIGURATIONS
 ****************************************************/

/* Switch Port PHY configuration */
// Messages CCMSG_ETH_PHY_CONFIG_SET and CCMSG_ETH_PHY_CONFIG_GET
typedef struct {
  L7_uint8  SlotId;
  L7_uint8  Port;               // 0-7:Pon; 8-15:1/2.5G Eth; 16-17:10G Eth
  L7_uint16 Mask;               
  L7_uint8  Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 - 1000;  4 - 2500; 5 - 10000
  L7_uint8  Media;              // 0x0002   0 - electric; 1 - optic;   2 - Serdes
  L7_uint8  Duplex;             // 0x0004   0 - Half;     1 - Full
  L7_uint8  LoopBack;           // 0x0008   0 - s/ loop;  1 - Far End; 2 - swap loop; 3 - local oam loop; 4 - remote oam loop
  L7_uint8  FlowControl;        // 0x0010   0 - Off;      1 - Tx;      2 - Rx;        3 - Both
  L7_uint8  PortEnable;         // 0x0020   0 - disabled; 1 - enabled
  L7_uint16 MaxFrame;           // 0x0040
  L7_uint8  VlanAwareness;      // 0x0080   0 - unaware;  1 - aware
  L7_uint8  MacLearning;        // 0x0100   0 - don't learn; 1 - learn
                                // NOTE: now holds MAC Learning Priority [0..3] (0-lowest, 3-highest)
  L7_uint8  AutoMDI;            // 0x0200   0 - disable;     1 - enable
} __attribute__((packed)) msg_HWEthPhyConf_t;

/* Switch Port PHY state */
// Message CCMSG_ETH_PHY_STATE_GET
typedef struct {
  L7_uint8  SlotId;
  L7_uint8  Port;
  L7_uint16 Mask;
  L7_uint8  Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 - 1000;  4 - 2500; 5 - 10000
  L7_uint8  Duplex;             // 0x0002   0 - Half;     1 - Full
  L7_uint8  Collisions;         // 0x0004   0 - OK;       1 - NOK
  L7_uint8  RxActivity;         // 0x0008   0 - Sem;      1 - Com
  L7_uint8  TxActivity;         // 0x0010   0 - Sem;      1 - Com
  L7_uint8  LinkUp;             // 0x0020   0 - Down;     1 - Up
  L7_uint8  AutoNegComplete;    // 0x0040   0 - incompleted; 1 - completed
  L7_uint8  TxFault;            // 0x0080   0 - ok;       1 - Tx_fault
  L7_uint8  RemoteFault;        // 0x0100   0 - ok;       1 - remote_fault recebido
  L7_uint8  LOS;                // 0x0200   0 - s/ LOS;   1 - em LOS
  L7_uint8  Media;              // 0x0400   0 - electric; 1 - optic
  L7_uint8  MTU_mismatch;       // 0x0800   0 - MTU suported; 1 - MTU not suported
  L7_uint16 Supported_MaxFrame; // 0x1000  1518 to 9600
} __attribute__((packed)) msg_HWEthPhyState_t;

/* Switch Port Counters structures */
// Message CCMSG_ETH_PHY_COUNTERS_GET and CCMSG_ETH_PHY_COUNTERS_CLEAR
typedef struct _L7_HWEthRFC2819_Statistics_Block
{
  L7_uint64 etherStatsDropEvents;               // 0x00000001
  L7_uint64 etherStatsOctets;                   // 0x00000002
  L7_uint64 etherStatsPkts;                     // 0x00000004
  L7_uint64 etherStatsBroadcastPkts;            // 0x00000008
  L7_uint64 etherStatsMulticastPkts;            // 0x00000010
  L7_uint64 etherStatsCRCAlignErrors;           // 0x00000020
  L7_uint64 etherStatsUndersizePkts;            // 0x00000040
  L7_uint64 etherStatsOversizePkts;             // 0x00000080
  L7_uint64 etherStatsFragments;                // 0x00000100
  L7_uint64 etherStatsJabbers;                  // 0x00000200
  L7_uint64 etherStatsCollisions;               // 0x00000400
  L7_uint64 etherStatsPkts64Octets;             // 0x00000800
  L7_uint64 etherStatsPkts65to127Octets;        // 0x00001000
  L7_uint64 etherStatsPkts128to255Octets;       // 0x00002000
  L7_uint64 etherStatsPkts256to511Octets;       // 0x00004000
  L7_uint64 etherStatsPkts512to1023Octets;      // 0x00008000
  L7_uint64 etherStatsPkts1024to1518Octets;     // 0x00010000
  L7_uint64 etherStatsPkts1519toMaxOctets;      // Proprietary (0x00020000)
  L7_uint64 Throughput;                         // Proprietary (0x00040000)
} __attribute__((packed)) msg_HWEthRFC2819_StatisticsBlock_t; //144
typedef struct {
  L7_uint8  SlotId;
  L7_uint8  Port;
  L7_uint8  Mask;
  L7_uint32 RxMask;
  L7_uint32 TxMask;
  msg_HWEthRFC2819_StatisticsBlock_t Rx;        // 0x01
  msg_HWEthRFC2819_StatisticsBlock_t Tx;        // 0x02
} __attribute__((packed)) msg_HWEthRFC2819_PortStatistics_t;

/* MEF extension definitions for a specific port */
// Messages CCMSG_ETH_PORT_EXT_SET and CCMSG_ETH_PORT_EXT_GET
#define MSG_HWPORTEXT_MASK_DEFVID                         0x0001
#define MSG_HWPORTEXT_MASK_DEFPRIO                        0x0002
#define MSG_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES         0x0004
#define MSG_HWPORTEXT_MASK_INGRESS_FILTER                 0x0008
#define MSG_HWPORTEXT_MASK_RESTRICTED_VLAN_REG            0x0010
#define MSG_HWPORTEXT_MASK_VLAN_AWARE                     0x0020
#define MSG_HWPORTEXT_MASK_TYPE                           0x0040
#define MSG_HWPORTEXT_MASK_DOUBLETAG                      0x0100
#define MSG_HWPORTEXT_MASK_OUTER_TPID                     0x0200
#define MSG_HWPORTEXT_MASK_INNER_TPID                     0x0400
#define MSG_HWPORTEXT_MASK_EGRESS_TYPE                    0x0800
#define MSG_HWPORTEXT_MASK_MACLEARN_ENABLE                0x1000
#define MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE    0x2000
#define MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO      0x4000
#define MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO  0x8000
typedef struct
{
  L7_uint8              SlotId;
  msg_HwEthInterface_t  intf;                           /* Interface id: can be physical or logical */
  L7_uint16             Mask;                           /* Configuration mask */
  L7_uint16             defVid;                         // [Mask=0x0001] (only physical interfaces)
  L7_uint8              defPrio;                        // [Mask=0x0002] (only physical interfaces)
  L7_uint8              acceptable_frame_types;         // [Mask=0x0004] (only physical interfaces)
  L7_uint8              ingress_filter;                 // [Mask=0x0008] (only physical interfaces)
  L7_uint8              restricted_vlan_reg;            // [Mask=0x0010] (only physical interfaces)
  L7_uint8              vlan_aware;                     // [Mask=0x0020] (only physical interfaces)
  L7_uint8              type;                           // [Mask=0x0040] UNI=1, NNI=2 (only physical interfaces)
  L7_uint8              doubletag;                      // [Mask=0x0100] (only physical interfaces)
  L7_uint16             outer_tpid;                     // [Mask=0x0200] (only physical interfaces)
  L7_uint16             inner_tpid;                     // [Mask=0x0400] (only physical interfaces)
  L7_uint8              egress_type;                    // [Mask=0x0800] PROMISCUOUS=0 / COMMUNITY=1 / ISOLATED=2
  L7_uint8              macLearn_enable;                // [Mask=0x1000] Enable Mac Learning (only physical interfaces)
  L7_uint8              macLearn_stationMove_enable;    // [Mask=0x2000] Mac Station Move Enable (physical/LAG interfaces)
  L7_uint8              macLearn_stationMove_prio;      // [Mask=0x4000] Mac Station Move Priority: 1-2 (physical/LAG interfaces)
  L7_uint8              macLearn_stationMove_samePrio;  // [Mask=0x8000] Enable Station Move between same priority ports (physical/LAG interfaces)
} __attribute__((packed)) msg_HWPortExt_t;

typedef struct
{
  L7_uint8              SlotId;
  msg_HwEthInterface_t  intf;                           /* Interface id: for the present it will be only physical */
  L7_uint16             Mask;                           /* Mask */
  L7_uint8              macAddr[6];                     /* [Mask=0x0001]: MAC address */
} __attribute__((packed)) msg_HWPortMac_t;

/***************************************************** 
 * QoS messages
 ****************************************************/

/* QoS Configuration */ 
// Messages CCMSG_ETH_PORT_COS_GET and CCMSG_ETH_PORT_COS_SET
#define MSG_QOS_CONFIGURATION_TRUSTMODE_MASK         0x01
#define MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK     0x02
#define MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK       0x04
#define MSG_QOS_CONFIGURATION_PACKETPRIO_MASK        0x08
#define MSG_QOS_CONFIGURATION_PACKETPRIO_COS_MASK      0xff
#define MSG_QOS_CONFIGURATION_QOSCONF_MASK           0x10
#define MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK   0x01
#define MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK      0x02
#define MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK      0x04

#define MSG_QOS_CONFIGURATION_PRIORITY_MAP_MASK    ( MSG_QOS_CONFIGURATION_TRUSTMODE_MASK | \
                                                     MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK | \
                                                     FIGURATION_SHAPINGRATE_MASK | \
                                                     MSG_QOS_CONMSG_QOS_CONFIGURATION_PACKETPRIO_MASK )
#define MSG_QOS_CONFIGURATION_COS                  ( MSG_QOS_CONFIGURATION_QOSCONF_MASK )

typedef struct
{
  L7_uint8 SlotId;                      // Slot id
  msg_HwEthInterface_t intf;            // Interface

  L7_uint8  mask;                       // Configurations mask

  L7_uint8  trust_mode;                 // mask=0x01: 0-None, 1-Untrust markings, 2-802.1p marking, 3: IP-precedence mark; 4-DSCP mark (Default=2)
  L7_uint8  bandwidth_unit;             // mask=0x02: 0: Percentage, 1: Kbps, 2: PPS (Default=0)
  L7_uint32 shaping_rate;               // mask=0x04: 0-100: Default=0 (unlimited)

  struct {                              // Packet priority map
    L7_uint8  mask;                     //   pktpriority map mask (nth bit, tells to configure the nth priority)
    L7_uint32 cos[8];                   //   Mapping: CoS(pcp): Default={0,1,2,3,4,5,6,7}
  } __attribute__((packed)) pktprio;    // mask=0x08: Packet priority map

  struct {                              // CoS configuration
    L7_uint8 mask;                      //   CoS map mask (nth bit, tells to configure the nth CoS)
    struct {                            //   Specific CoS configuration
      L7_uint8  mask;                   //     Specific CoS configuration mask
      L7_uint8  scheduler;              //     mask=0x01: Scheduler type: 0-None, 1-Strict, 2-Weighted (Default=1)
      L7_uint32 min_bandwidth;          //     mask=0x02: Minimum bandwidth (0-100): Default=0 (no guarantee)
      L7_uint32 max_bandwidth;          //     mask=0x04: Maximum bandwidth (0-100): Default=0 (unlimited)
    } __attribute__((packed)) cos[8];   //   Specific CoS configuration (8 queues)
  } __attribute__((packed)) cos_config; // mask=0x10: CoS configuration
} __attribute__((packed)) msg_QoSConfiguration_t;

/***************************************************** 
 * L2 Table messages
 ****************************************************/

/* L2 MACs */ 
// Messages CCMSG_ETH_MAC_TABLE_SHOW and CCMSG_ETH_MAC_TABLE_REMOVE
#define MSG_CMDGET_MAC_TABLE_MAXENTRIES 256
typedef struct {
    L7_uint8              slotId;         /* Slot Id */
    L7_uint32             startEntryId;   /* Starting Entry id (starts from 0) */
    L7_uint32             numEntries;     /* Maximum number of entries to be read */
} __attribute__((packed)) msg_switch_mac_intro_t;

typedef struct {
    L7_uint32             evcId;          /* EVC id that belongs this MAC entry (if no EVC is associated its value will be 0xffff) */
    L7_uint16             vlanId;         /* Vlan associated to this MAC entry */
    L7_uint8              addr[6];        /* MAC address */
    msg_HwEthInterface_t  intf;           /* Interface attached to this entry */
    L7_uint16             gem_id;         /* When intf's a PON, GEMid identifies the ONU*/
    L7_uint8              static_entry;   /* Is this entry static? */
} __attribute__((packed)) msg_switch_mac_entry;

typedef struct {
    msg_switch_mac_intro_t  intro;        /* Intro struct  used for requests will also be used for replies */
    msg_switch_mac_entry    entry[MSG_CMDGET_MAC_TABLE_MAXENTRIES]; /* List of entries */
} __attribute__((packed)) msg_switch_mac_table_t;
#if 0
typedef struct {
  L7_uint16 mac_index;                // MAC index (from 0 to 254)
  L7_uint16 flow_id;                  // EVC id
  L7_uint16 vlan_id;                  // service Vlan
  msg_HwEthInterface_t intf;           // Interface
  L7_uint8  mac_address[6];           // MAC address
  L7_uint8  static_address;           // Is static address?
} __attribute__ ((packed)) st_switch_mac_entry;
typedef struct {
  L7_uint8 SlotId;                    // slot
  uint16   page;                      // page index
  L7_uint16 mac_table_total_entries;    // Total number of entries in MAC table
  L7_uint16 mac_table_msg_size;         // Number of entries in this message: up to 256
  st_switch_mac_entry mac_table[256];       // MAC table
} __attribute__ ((packed)) msg_switch_mac_table_t;
#endif

/* Switch general configuration */
// Messages CCMSG_ETH_SWITCH_CONFIG_GET and CCMSG_ETH_SWITCH_CONFIG_SET
typedef struct {
  L7_uint8  SlotId;                   // slot
  L7_uint32 mask;                     // Mask (32 bits for alignment purposes)
                                          // 0x0001 - Aging Time
  L7_uint32 aging_time;               // [0x0001] Forwarding Database Aging Interval    (10-1000000) */
} __attribute__((packed)) msg_switch_config_t;


/***************************************************** 
 * LAGs messages
 ****************************************************/

/* LAGs Configuration */
// Messages CCMSG_ETH_LACP_LAG_GET, CCMSG_ETH_LACP_LAG_ADD and CCMSG_ETH_LACP_LAG_REMOVE
typedef struct {
  L7_uint8  SlotId;               //
  L7_uint8  id;                   // [0..PTIN_SYSTEM_N_LAGS-1]
  L7_uint8  admin;                // DISABLED=0 or ENABLED=1
  L7_uint8  stp_enable;           // DISABLED=0 or ENABLED=1
  L7_uint8  static_enable;        // DISABLED=0 or ENABLED=1
  L7_uint8  loadBalance_mode;     // FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
  L7_uint32 members_pbmp;         // Ports bitmap (0  .. 31)
  L7_uint32 members_pbmp2;        // Ports bitmap (32 .. 63)
} __attribute__((packed)) msg_LACPLagInfo_t;

/* LAGs Status */
// Message CCMSG_ETH_LACP_LAG_STATUS_GET
typedef struct {
  L7_uint8  SlotId;                 //
  L7_uint8  id;                     // [0..PTIN_SYSTEM_N_LAGS-1]
  L7_uint8  admin;                  // DISABLED=0 or ENABLED=1
  L7_uint8  link_status;            // DOWN=0 or UP=1
  L7_uint8  port_channel_type;      // DYNAMIC=0 or STATIC=1
  L7_uint32 members_pbmp1;          // Ports bitmap (0  .. 31)
  L7_uint32 members_pbmp2;          // Ports bitmap (32 .. 63)
  L7_uint32 active_members_pbmp1;   // Ports bitmap (0  .. 31)
  L7_uint32 active_members_pbmp2;   // Ports bitmap (32 .. 63)
} __attribute__((packed)) msg_LACPLagStatus_t;

/* LAGs Admin state */
// Messages CCMSG_ETH_LACP_ADMINSTATE_SET and CCMSG_ETH_LACP_ADMINSTATE_GET
typedef struct {
  L7_uint8  SlotId;              //
  L7_uint8  id;                  // index: physical interface #
  L7_uint8  state_aggregation;   // Enable=1, Disable=0
  L7_uint8  lacp_activity;       //
  L7_uint8  lacp_timeout;        // Short=1, Long=0
} __attribute__((packed)) msg_LACPAdminState_t;

/* LACPs statistics */
// Messages CCMSG_ETH_LACP_STATS_GET and CCMSG_ETH_LACP_STATS_CLEAR
typedef struct {
  L7_uint8  SlotId;              //
  L7_uint8  id;                  // index: physical interface #
  L7_uint32 LACPdus_rx;          // LACPdu's received
  L7_uint32 LACPdus_tx;          // LACPdu's transmitted
} __attribute__((packed)) msg_LACPStats_t;

/***************************************************** 
 * Routing messages
 ****************************************************/

// Message CCMSG_ROUTING_INTF_CREATE
#define CCMSG_ROUTING_INTF_TYPE_UPLINK        1
#define CCMSG_ROUTING_INTF_TYPE_LOOPBACK      2
typedef struct
{
   L7_uint16            slotId;
   L7_uint8             type;
   msg_HwEthInterface_t routingIntf;
   msg_HwEthInterface_t physicalIntf;
   L7_uint16            routingVlanId;
   L7_uint32            evcId;
   L7_uint32            ipAddress;
   L7_uint32            subnetMask;
} __attribute__((packed)) msg_RoutingIntfCreate;

// Message CCMSG_ROUTING_INTF_MODIFY
typedef struct
{
   L7_uint16            slotId;
   msg_HwEthInterface_t routingIntf;
   L7_uint32            ipAddress;
   L7_uint32            subnetMask;
} __attribute__((packed)) msg_RoutingIntfModify;

// Message CCMSG_ROUTING_INTF_REMOVE
typedef struct
{
   L7_uint16            slotId;
   msg_HwEthInterface_t routingIntf;
} __attribute__((packed)) msg_RoutingIntfRemove;

// Message CCMSG_ROUTING_ARPTABLE_GET
#define CCMSG_ROUTING_ARPTABLE_TYPE_STATIC   1
#define CCMSG_ROUTING_ARPTABLE_TYPE_DYNAMIC  2
#define CCMSG_ROUTING_ARPTABLE_TYPE_LOCAL    3
#define CCMSG_ROUTING_ARPTABLE_TYPE_GATEWAY  4
typedef struct
{
   L7_uint16            slotId;
   msg_HwEthInterface_t intf;
   L7_uint32            lastIndex;
} __attribute__((packed)) msg_RoutingArpTableRequest;
typedef struct
{
   L7_uint16            index;
   msg_HwEthInterface_t intf;
   L7_uint8             type;
   L7_uint32            age;
   L7_uint32            ipAddr;
   L7_uint8             macAddr[6];
} __attribute__((packed)) msg_RoutingArpTableResponse;

// Message CCMSG_ROUTING_ARPENTRY_PURGE
typedef struct
{
   L7_uint16            slotId;
   msg_HwEthInterface_t intf;
   L7_int32             ipAddr;
} __attribute__((packed)) msg_RoutingArpEntryPurge;

// Message CCMSG_ROUTING_ROUTETABLE_GET
#define CCMSG_ROUTING_ROUTETABLE_PROTOCOL_LOCAL   1
#define CCMSG_ROUTING_ROUTETABLE_PROTOCOL_STATIC  2
#define CCMSG_ROUTING_ROUTETABLE_PROTOCOL_MPLS    3
#define CCMSG_ROUTING_ROUTETABLE_PROTOCOL_RIP     16
typedef struct
{
   L7_uint16            slotId;
   msg_HwEthInterface_t intf;
   L7_uint32            lastIndex;
} __attribute__((packed)) msg_RoutingRouteTableRequest;
typedef struct
{
   L7_uint16            index;
   msg_HwEthInterface_t intf;
   L7_uint8             protocol;
   struct
   {
      L7_uint32 days;
      L7_uint32 hours;
      L7_uint32 minutes;
      L7_uint32 seconds;
   }                    updateTime;
   L7_uint32            ipAddr;
   L7_uint32            subnetMask;
   L7_uint32            preference;
   L7_uint32            metric;
} __attribute__((packed)) msg_RoutingRouteTableResponse;

//CCMSG_ROUTING_STATICROUTE_ADD / CCMSG_ROUTING_STATICROUTE_DELETE
typedef struct
{
  L7_uint16 slotId;
  L7_uint32 dstIpAddr; 
  L7_uint32 subnetMask;
  L7_uint32 nextHopRtr;
  L7_uint8  pref;
  L7_uint8  isNullRoute;
} __attribute__((packed)) msg_RoutingStaticRoute;

// Message CCMSG_ROUTING_PINGSESSION_CREATE
typedef struct
{
   L7_uint16 slotId;
   L7_uint16 sessionIdx;
   L7_uint32 dstIpAddr; 
   L7_uint16 probeCount; 
   L7_uint16 probeSize; 
   L7_uint16 probeInterval;
} __attribute__((packed)) msg_RoutingPingSessionCreate;

// Message CCMSG_ROUTING_PINGSESSION_QUERY
typedef struct
{
   L7_uint16 slotId;
   L7_uint16 sessionIdx;
   L7_uint8  isRunning;
   L7_uint16 probeSent;
   L7_uint16 probeSucc;
   L7_uint16 probeFail;
   L7_uint32 minRtt; 
   L7_uint32 maxRtt; 
   L7_uint32 avgRtt; 
} __attribute__((packed)) msg_RoutingPingSessionQuery;

// Message CCMSG_ROUTING_PINGSESSION_FREE
typedef struct
{
   L7_uint16 slotId;
   L7_uint16 sessionIdx;
} __attribute__((packed)) msg_RoutingPingSessionFree;

//CCMSG_ROUTING_TRACERTSESSION_CREATE
typedef struct
{
  L7_uint16 slotId;
  L7_uint16 sessionIdx;
  L7_uint32 dstIpAddr; 
  L7_uint16 probePerHop; 
  L7_uint16 probeSize; 
  L7_uint32 probeInterval;
  L7_uint8  dontFrag;
  L7_uint16 port;
  L7_uint16 maxTtl;
  L7_uint16 initTtl;
  L7_uint16 maxFail;
} __attribute__((packed)) msg_RoutingTracertSessionCreate;

//CCMSG_ROUTING_TRACERTSESSION_QUERY
typedef struct
{
  L7_uint16 slotId;
  L7_uint16 sessionIdx;
  L7_uint8  isRunning;
  L7_uint16 currTtl;
  L7_uint16 currHopCount;
  L7_uint16 currProbeCount;
  L7_uint16 testAttempt; 
  L7_uint16 testSuccess; 
} __attribute__((packed)) msg_RoutingTracertSessionQuery;

//CCMSG_ROUTING_TRACERTSESSION_GETHOPS
typedef struct
{
  L7_uint16 slotId;
  L7_uint16 sessionIdx;
  L7_uint16 lastIndex;
} __attribute__((packed)) msg_RoutingTracertSessionHopsRequest;
typedef struct
{
  L7_uint16 hopIdx;
  L7_uint16 ttl;
  L7_uint32 ipAddr;
  L7_uint32 minRtt;
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_uint16 probeSent;
  L7_uint16 probeRecv;
} __attribute__((packed)) msg_RoutingTracertSessionHopsResponse;

//CCMSG_ROUTING_TRACERTSESSION_FREE
typedef struct
{
  L7_uint16 slotId;
  L7_uint16 sessionIdx;
} __attribute__((packed)) msg_RoutingTracertSessionFree;


/***************************************************** 
 * EVCs messages
 ****************************************************/

/* EVC details per interface */
typedef struct {
  L7_uint8  intf_type;    // Interface type: { 0-Physical, 1-Logical (LAG) }
  L7_uint8  intf_id;      // Interface Id# (phy ports / LAGs)
  L7_uint16 outer_vid;    // Outer VLAN id [1..4094]
  L7_uint16 inner_vid;    // Inner VLAN id [1..4094]
} __attribute__((packed)) msg_HwEthIntf_t;

/* EVC details per interface */
// Messages CCMSG_ETH_EVC_GET, CCMSG_ETH_EVC_ADD and CCMSG_ETH_EVC_REMOVE
typedef struct {
  L7_uint8  intf_type;    // Interface type: { 0-Physical, 1-Logical (LAG) }
  L7_uint8  intf_id;      // Interface Id# (phy ports / LAGs)
  L7_uint8  mef_type;     // { 0 - root, 1 - leaf }
  L7_uint16 vid;          // Outer VLAN id [1..4094]
  L7_uint16 inner_vid;    // Inner vlan
} __attribute__((packed)) msg_HwEthMef10Intf_t;

/* EVC config */
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 id;           // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint32 flags;        // 0x0001 - Bundling      (not implemented)
                          // 0x0002 - All to One    (not implemented)
                          // 0x0004 - Stacked       (PTin custom field)
                          // 0x0008 - Mac Learning  (PTin custom field)
                          // 0x0010 - cpu Trapping  (PTin custom field)
                          // 0x0100 - DHCP protocol (PTin custom field)
  L7_uint8  type;         // (not used) { 0 - p2p, 1 - mp2mp, 2 - rooted mp }
  L7_uint8  mc_flood;     // MC flood type {0-All, 1-Unknown, 2-None} (PTin custom field)
  L7_uint8  ce_vid_bmp[(1<<12)/(sizeof(L7_uint8)*8)];   // VLANs mapping (ONLY for bundling) ((bmp[i/8] >> i%8) & 0x01)
  
  L7_uint8  n_intf;       // Number of interfaces present on intf array
  msg_HwEthMef10Intf_t intf[max(PTIN_SYSTEM_MAX_N_PORTS, PTIN_SYSTEM_MAX_N_LAGS)];

  /* NOTE: the size of the intf array is limited to either the maximum number of
   * physical or LAG interfaces because if one interface is being used as physical
   * port, it cannot be used in a LAG, and vice-versa. So it means that we will
   * have at most the max(phy,lag) interfaces combination in an EVC. */

  /* IMPORTANT: interfaces must be in sequence (phy+lags) */

} __attribute__((packed)) msg_HwEthMef10Evc_t;

/* EVC Remove */
// Messages CCMSG_ETH_EVC_REMOVE
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 id;           // EVC Id [1..PTIN_SYSTEM_N_EVCS]
} __attribute__((packed)) msg_HwEthMef10EvcRemove_t;

/* EVC port add/remove */
// Messages CCMSG_ETH_EVC_PORT_ADD and CCMSG_ETH_EVC_PORT_REMOVE
typedef struct
{
  L7_uint8  slotId;
  L7_uint32 evcId;

  msg_HwEthMef10Intf_t intf;
} __attribute__((packed)) msg_HWevcPort_t;

/* EVC stacked bridge */
// Messages CCMSG_ETH_EVC_BRIDGE_ADD and CCMSG_ETH_EVC_BRIDGE_REMOVE
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 evcId;        // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint16 inn_vlan;     // Client VLAN (inner tag)

  /* Client interface (root is already known by the EVC) */
  msg_HwEthMef10Intf_t intf;// VID represents the new outer VLAN (Vs')
} __attribute__((packed)) msg_HwEthEvcBridge_t;

/* EVC flow */
// Messages CCMSG_ETH_EVC_FLOW_ADD and CCMSG_ETH_EVC_FLOW_REMOVE
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 evcId;  // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint32 flags;  // Flags:  0x000100 - DHCP protocol (PTin custom field)
                            // 0x000200 - IGMP protocol (PTin custom field)
                            // 0x000400 - PPPOE protocol (PTin custom field)
  /* Flow information */
  L7_uint16 nni_cvlan;  // NNI inner vlan
  msg_HwEthIntf_t intf; // outer vlan is the GEM id
  L7_uint8  macLearnMax;  // Maximum number of Learned MAC addresses
} __attribute__((packed)) msg_HwEthEvcFlow_t;

/***************************************************** 
 * BW Profiles messages
 ****************************************************/

/* Bandwidth profile applied to an EVC(+interface/client) */
// Messages CCMSG_ETH_BW_PROFILE_GET, CCMSG_ETH_BW_PROFILE_SET and CCMSG_ETH_BW_PROFILE_DELETE
typedef struct {
  L7_uint64 cir;        // Commited Information rate (bps)
  L7_uint64 cbs;        // Commited Burst Size (bytes)
  L7_uint64 eir;        // Excess Information Rate (bps)
  L7_uint64 ebs;        // Excess Burst Size (bytes)
} __attribute__((packed)) msg_HwEthBwProfileData_t;

#define MSG_HWETH_BWPROFILE_MASK_NONE     0x00
#define MSG_HWETH_BWPROFILE_MASK_SVLAN    0x01
#define MSG_HWETH_BWPROFILE_MASK_CVLAN    0x02
#define MSG_HWETH_BWPROFILE_MASK_INTF_SRC 0x04
#define MSG_HWETH_BWPROFILE_MASK_INTF_DST 0x08
#define MSG_HWETH_BWPROFILE_MASK_PROFILE  0x80

typedef struct {
  L7_uint8  SlotId;
  L7_uint32 evcId;                    // EVC index    /* XXX */
  L7_uint8  mask;                     // Configuration mask 
  L7_uint16 service_vlan;             // [mask=0x01] External Outer Vlan
  L7_uint16 client_vlan;              // [mask=0x02] Client (Inner) Vlan
  msg_HwEthInterface_t intf_src;      // [mask=0x04] Source Interface to apply profile
  msg_HwEthInterface_t intf_dst;      // [mask=0x08] Destination Interface to apply profile
  msg_HwEthBwProfileData_t profile;   // [mask=0x80] Profile to be applied
} __attribute__((packed)) msg_HwEthBwProfile_t;

/* Storm control */
// Messages CCMSG_ETH_STORM_CONTROL_GET, CCMSG_ETH_STORM_CONTROL_SET and CCMSG_ETH_STORM_CONTROL_CLEAR
// Rate limit structure
#define MSG_STORMCONTROL_MASK_BCAST   0x0001
#define MSG_STORMCONTROL_MASK_MCAST   0x0002
#define MSG_STORMCONTROL_MASK_UCUNK   0x0004
#define MSG_STORMCONTROL_MASK_ALL     0x00ff

typedef struct {
  L7_uint8  SlotId;
  L7_uint32 id;                 /* ID */
  L7_uint16 flags;              /* Control flags: 0x0000 */
  L7_uint16 mask;               /* Mask */
  L7_uint32 bcast_rate;         /* [mask=0x0001] in bps */
  L7_uint32 mcast_rate;         /* [mask=0x0002] in bps */
  L7_uint32 ucast_unknown_rate; /* [mask=0x0004] in bps */
} __attribute__((packed)) msg_HwEthStormControl_t;

/***************************************************** 
 * EVC counters messages
 ****************************************************/

// Statistics data
// Messages CCMSG_ETH_EVC_COUNTERS_GET, CCMSG_ETH_EVC_COUNTERS_ADD and CCMSG_ETH_EVC_COUNTERS_REMOVE
typedef struct {
  L7_uint32 pktTotal;                   // Total packets (Unicast+Multicast+Broadcast)
  L7_uint32 pktUnicast;                 // Unicast packets
  L7_uint32 pktMulticast;               // Multicast packets
  L7_uint32 pktBroadcast;               // Broadcast packets
  L7_uint32 pktDropped;                 // Packets dropped
} __attribute__((packed)) msg_counters_t;

#define MSG_EVC_COUNTERS_MASK_STATS_RX 0x01
#define MSG_EVC_COUNTERS_MASK_STATS_TX 0x02

typedef struct {
  L7_uint8 mask_stat;         // Máscara de contadores
  msg_counters_t  rx;         // [mask_stat=0x01] Estrutura de estatísticas de recepção (RX)
  msg_counters_t  tx;         // [mask_stat=0x02] Estrutura de estatísticas de transmissão (TX)
} __attribute__((packed)) msg_evcStats_counters_t;

#define MSG_EVC_COUNTERS_MASK_NONE     0x00
#define MSG_EVC_COUNTERS_MASK_INTF     0x01
#define MSG_EVC_COUNTERS_MASK_SVLAN    0x02
#define MSG_EVC_COUNTERS_MASK_CVLAN    0x04
#define MSG_EVC_COUNTERS_MASK_CHANNEL  0x08
#define MSG_EVC_COUNTERS_MASK_STATS    0x80

typedef struct {
  L7_uint8             SlotId;       // Indice da slot
  L7_uint32            evc_id;       // Indice do Serviço do OLT  /* XXX */
  L7_uint8             mask;         // Máscara de campos a utilizar
  msg_HwEthInterface_t intf;         // [mask=0x01] Interface
  L7_uint16            service_vlan; // [mask=0x02] (reservado para uso futuro)
  L7_uint16            client_vlan;  // [mask=0x04] Inner Vlan que identifica o ONT (apenas para contadores ao ONT)
  L7_uint32            channel_ip;   // [mask=0x08] Destination IP que identifica o canal (contadores ao canal)
  msg_evcStats_counters_t stats;     // [mask=0x80] Estatísticas
} __attribute__((packed)) msg_evcStats_t;

/***************************************************** 
 * Inband connectivity messages
 ****************************************************/

/* Network Connectivity (inBand) structs */
// Messages CCMSG_ETH_NTW_CONNECTIVITY_GET and CCMSG_ETH_NTW_CONNECTIVITY_SET
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 mask;
  L7_uint32 ipaddr;                         // 0x0002: IP Addr: xx.xx.xx.xxh
  L7_uint32 netmask;                        // 0x0004: Netmask: xx.xx.xx.xxh
  L7_uint32 gateway;                        // 0x0008: Default gateway: xx.xx.xx.xxh
  L7_uint16 mgmtVlanId;                     // 0x0010: Management VlanId
  L7_uint8  n_intf;       // Number of interfaces present on intf array
  msg_HwEthInterface_t intf[PTIN_SYSTEM_MAX_N_PORTS];
} __attribute__((packed)) msg_NtwConnectivity_t;

/***************************************************** 
 * DHCP option 82 configuration messages
 ****************************************************/

#define MSG_INTERFACE_MASK    0x01
#define MSG_CLIENT_MASK       0x02

/* DHCP Profile */
// Messages CCMSG_ETH_DHCP_PROFILE_GET, CCMSG_ETH_DHCP_PROFILE_ADD and CCMSG_ETH_DHCP_PROFILE_REMOVE
typedef struct {
  L7_uint8  SlotId;               /* slot id */
  L7_uint32 evc_id;               /* evc id */    /* XXX */
  L7_uint8              mask;     /* Mask of fields to be considered */
  msg_HwEthInterface_t  intf;     /* [mask=0x01] Interface */
  msg_client_info_t     client;   /* [mask=0x02] Client reference */
  L7_uint16             options;  /* [mask=0x04] Options to be active: 0x01=Option82; 0x02=Option 37; 0x04=Option18 */
  char circuitId[64];             /* [mask=0x10] Circuit id */
  char remoteId[64];              /* [mask=0x20] Remote id */
} __attribute__((packed)) msg_HwEthernetDhcpOpt82Profile_t;

/* DHCP Statistics */ 
// Messages CCMSG_ETH_DHCP_CLIENT_STATS_GET, CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR,
//          CCMSG_ETH_DHCP_INTF_STATS_GET and CCMSG_ETH_DHCP_INTF_STATS_CLEAR
typedef struct _st_DHCP_Statistics_t
{
  uint32 dhcp_rx_intercepted;
  uint32 dhcp_rx;
  uint32 dhcp_rx_filtered;
  uint32 dhcp_tx_forwarded;
  uint32 dhcp_tx_failed;
  uint32 dhcp_rx_client_requests_without_options;
//uint32 dhcp_tx_client_requests_without_options;
  uint32 dhcp_tx_client_requests_with_option82;
  uint32 dhcp_tx_client_requests_with_option37;
  uint32 dhcp_tx_client_requests_with_option18;
  uint32 dhcp_rx_server_replies_with_option82;
  uint32 dhcp_rx_server_replies_with_option37;
  uint32 dhcp_rx_server_replies_with_option18;
//uint32 dhcp_rx_server_replies_without_options;
  uint32 dhcp_tx_server_replies_without_options;
  uint32 dhcp_rx_client_pkts_onTrustedIntf;
  uint32 dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  uint32 dhcp_rx_server_pkts_onUntrustedIntf;
  uint32 garbage;
} __attribute__ ((packed)) msg_DHCP_Statistics_t;

typedef struct _st_ClientDhcpStatistics
{
  L7_uint8  SlotId;               /* slot */
  L7_uint32 evc_id;               /* evc id */    /* XXX */
  L7_uint8              mask;     /* Mask of fields to be considered */
  msg_HwEthInterface_t  intf;     /* [mask=0x01] Interface */
  msg_client_info_t     client;   /* [mask=0x02] Client reference */
  msg_DHCP_Statistics_t stats;    /* statistics */
} __attribute__((packed)) msg_DhcpClientStatistics_t;

/* DHCP Binding Table */
// Messages CCMSG_ETH_DHCP_BIND_TABLE_GET and CCMSG_ETH_DHCP_BIND_TABLE_REMOVE
typedef struct {
  L7_uint16             entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint32             evc_idx;                // EVCid    /* XXX */
  L7_uint16             outer_vlan;             // Service vlan: not used yet
  L7_uint16             inner_vlan;             // Client clanId
  msg_HwEthInterface_t  intf;                   // Interface
  L7_uint8              macAddr[6];             // MAC Address
  msg_in_addr_t         ipAddr;                 // IP address
  L7_uint32             remLeave;               // Remaining Leave time in seconds
  L7_uint8              bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} __attribute__((packed)) msg_DHCP_bind_entry;

typedef struct {
  L7_uint16             entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint32             evc_idx;                // EVCid    /* XXX */
  L7_uint16             outer_vlan;             // Service vlan: not used yet
  L7_uint16             inner_vlan;             // Client clanId
  msg_HwEthInterface_t  intf;                   // Interface
  L7_uint8              macAddr[6];             // MAC Address
  chmessage_ip_addr_t   ipAddr;                 // IP address
  L7_uint32             remLeave;               // Remaining Leave time in seconds
  L7_uint8              bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} __attribute__((packed)) msg_DHCP4v6_bind_entry;

typedef struct {
  uint8  SlotId;                         // slot
  uint16 page;                           // Page index
  L7_uint16 bind_table_total_entries;    // Total entries in Bind table
  L7_uint16 bind_table_msg_size;         // Number of entries in this message: up to 128
  msg_DHCP_bind_entry bind_table[128];   // Bind table
} __attribute__((packed)) msg_DHCP_bind_table_t;

typedef struct {
  uint8  SlotId;                         // slot
  uint16 page;                           // Page index
  L7_uint16 bind_table_total_entries;    // Total entries in Bind table
  L7_uint16 bind_table_msg_size;         // Number of entries in this message: up to 128
  msg_DHCP4v6_bind_entry bind_table[128];// Bind table
} __attribute__((packed)) msg_DHCPv4v6_bind_table_t;


/***************************************************** 
 * IGMP PROXY
 ****************************************************/

/* IGMP Querier's config structure */
typedef struct {
  L7_uint16 mask;                               /* PTIN_IGMP_QUERIER_MASK_xxxx */
  L7_uint8  flags;                              /* [0x01] - AUTO_GMI
                                                 * [0x02] - AUTO_OQPI
                                                 * [0x04] - AUTO_SQI
                                                 * [0x08] - AUTO_SQC
                                                 * [0x10] - AUTO_LMQC
                                                 * [0x20] - AUTO_OHPT */
  L7_uint8  version;                            /* [Mask: 0x0001] Defines maximum working version */
  L7_uint8  robustness;                         /* [Mask: 0x0002] */
  L7_uint16 query_interval;                     /* [Mask: 0x0004] */
  L7_uint16 query_response_interval;            /* [Mask: 0x0008] */
  L7_uint16 group_membership_interval;          /* [Mask: 0x0010] */
  L7_uint16 other_querier_present_interval;     /* [Mask: 0x0020] */
  L7_uint16 startup_query_interval;             /* [Mask: 0x0040] */
  L7_uint16 startup_query_count;                /* [Mask: 0x0080] */
  L7_uint16 last_member_query_interval;         /* [Mask: 0x0100] */
  L7_uint16 last_member_query_count;            /* [Mask: 0x0200] */
  L7_uint16 older_host_present_timeout;         /* [Mask: 0x0400] */
} __attribute__((packed)) msg_IgmpV3QuerierCfg_t;

/* IGMP Host's config structure */
typedef struct {
  L7_uint8  mask;                               /* PTIN_IGMP_HOST_MASK_xxxx */
  L7_uint8  flags;                              /* [0x01] - AUTO_OQPT */
  L7_uint8  version;                            /* [Mask: 0x01] Defines maximum working version */
  L7_uint8  robustness;                         /* [Mask: 0x02] */
  L7_uint16 unsolicited_report_interval;        /* [Mask: 0x04] */
  L7_uint16 older_querier_present_timeout;      /* [Mask: 0x08] */

} __attribute__((packed)) msg_IgmpV3HostCfg_t;

/* IGMP Proxy config structure */

// Messages CCMSG_ETH_IGMP_PROXY_GET and CCMSG_ETH_IGMP_PROXY_SET
typedef struct {
  L7_uint8      SlotId;
  L7_uint8      mask;                           /* PTIN_IGMP_PROXY_MASK_xxxx */

  L7_uint8      admin;                          /* [Mask: 0x01] (Global admin for both host and querier) */
  L7_uint8      version;                        /* [Mask: 0x02] (defines maximum working version - overrides query/host version) */
  msg_in_addr_t ipv4_addr;                      /* [Mask: 0x04] (Proxy IP (for both host and querier)) */
  L7_uint8      igmp_cos;                       /* [Mask: 0x08] [1..7] */
  L7_uint8      fast_leave;                     /* [Mask: 0x10] TRUE/FALSE */

  msg_IgmpV3QuerierCfg_t querier;              /* [Mask: 0x20] */
  msg_IgmpV3HostCfg_t    host;                 /* [Mask: 0x40] */

} __attribute__((packed)) msg_IgmpProxyCfg_t;

/* IGMP EVC Multicast/Unicast link config structure */
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 multicastEvcId;
  L7_uint32 unicastEvcId;
} __attribute__((packed)) msg_IgmpMultcastUnicastLink_t;

/* IGMP Client add/remove struct */
// Messages CCMSG_ETH_IGMP_CLIENT_ADD, CCMSG_ETH_IGMP_CLIENT_REMOVE
typedef struct {
  L7_uint8             SlotId;
  L7_uint32            mcEvcId;                 /* Multicast EVC Id */
  msg_client_info_t client;                /* Client identification */
} __attribute__((packed)) msg_IgmpClient_t;

/***************************************************** 
 * IGMP STATISTICS
 ****************************************************/

/* IGMP Client Statistics struct */
// Messages CCMSG_ETH_IGMP_CLIENT_STATS_GET, CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR,
//          CCMSG_ETH_IGMP_INTF_STATS_GET and CCMSG_ETH_IGMP_INTF_STATS_CLEAR
#if 0
typedef struct _st_IGMP_Statistics_t
{
  uint32 active_groups;
  uint32 active_clients;
  uint32 igmp_sent;
  uint32 igmp_tx_failed;
  uint32 igmp_intercepted;
  uint32 igmp_dropped;
  uint32 igmp_received_valid;
  uint32 igmp_received_invalid;
  uint32 joins_sent;
  uint32 joins_received_success;
  uint32 joins_received_failed;
  uint32 leaves_sent;
  uint32 leaves_received;
  uint32 membership_report_v3;
  uint32 general_queries_sent;
  uint32 general_queries_received;
  uint32 specific_queries_sent;
  uint32 specific_queries_received;
} __attribute__((packed)) msg_IGMP_Statistics_t;
#else
//New IGMP statictics Structure
typedef struct _st_IGMP_Statistics_t
{
  uint32 active_groups;
  uint32 active_clients;

  uint32 igmp_tx;  
  uint32 igmp_total_rx;  
  uint32 igmp_valid_rx;
  uint32 igmp_invalid_rx;
  uint32 igmp_dropped_rx;

  struct
  {  
  uint32 join_tx;
  uint32 join_valid_rx;
  uint32 join_invalid_rx;

  uint32 leave_tx;
  uint32 leave_valid_rx;
  } __attribute__((packed)) HWIgmpv2Statistics;

  struct
  {
    uint32 membership_report_tx;
    uint32 membership_report_valid_rx;  
    uint32 membership_report_invalid_rx; 
    
    struct
    {
      uint32 allow_tx;
      uint32 allow_valid_rx;
      uint32 allow_invalid_rx;
      uint32 block_tx;
      uint32 block_valid_rx;
      uint32 block_invalid_rx;
      uint32 is_include_tx;
      uint32 is_include_valid_rx;  
      uint32 is_include_invalid_rx;  
      uint32 is_exclude_tx;
      uint32 is_exclude_valid_rx;
      uint32 is_exclude_invalid_rx;
      uint32 to_include_tx;
      uint32 to_include_valid_rx;
      uint32 to_include_invalid_rx;
      uint32 to_exclude_tx;
      uint32 to_exclude_valid_rx;
      uint32 to_exclude_invalid_rx;
    } __attribute__((packed)) HWGroupRecordStatistics;
  } __attribute__((packed)) HWIgmpv3Statistics;

  struct
  {
    uint32 general_query_tx;
    uint32 general_query_valid_rx;

    uint32 group_query_tx;  
    uint32 group_query_valid_rx;

    uint32 source_query_tx;
    uint32 source_query_valid_rx;
  } __attribute__((packed)) HWQueryStatistics;

} __attribute__((packed)) msg_IGMP_Statistics_t;
#endif

#define MSG_INTERFACE_MASK    0x01
#define MSG_CLIENT_MASK  0x02

typedef struct _st_ClientIgmpStatistics
{
  L7_uint8 SlotId;
  uint32 mcEvcId;                 /* XXX */
  L7_uint8               mask;    /* Mask of fields to be considered */
  msg_HwEthInterface_t   intf;    /* [mask=0x01] Interface */
  msg_client_info_t client;  /* [mask=0x02] Client reference */
  msg_IGMP_Statistics_t stats;    /* Statistics to be returned (for reading messages) */
} __attribute__((packed)) msg_IgmpClientStatistics_t;

//typedef struct {
//  L7_uint8             SlodId;
//  L7_uint32            mcEvcId;                 /* Multicast EVC Id */
//  L7_uint16            client_vlan;             /* Client VLAN */
//  st_HwEthInterface_t intf;                    /* Interface on which client is attached */
//  msg_IgmpStatistics_t stats;                   /* Statistics */
//} __attribute__((packed)) msg_IgmpClientStatistics_t;

/***************************************************** 
 * STATIC MULTICAST CHANNELS
 ****************************************************/

/* To add or remove a channel associated to a MC service */
// Messages CCMSG_ETH_IGMP_CHANNEL_ASSOC_GET or CCMSG_ETH_IGMP_CHANNEL_ASSOC_ADD and CCMSG_ETH_IGMP_CHANNEL_ASSOC_REMOVE
typedef struct _msg_MCAssocChannel_t
{
  L7_uint8  SlotId;                     // slot
  L7_uint32 evcid_mc;                   // index: EVCid (MC)    /* XXX */
  L7_uint16 entry_idx;                  // Entry index: only for readings
  chmessage_ip_addr_t channel_dstIp;    // IP do canal a adicionar/remover
  L7_uint8            channel_dstmask;  // MAscara do canal em numero de bits (LSB)
  chmessage_ip_addr_t channel_srcIp;    // IP source 
  L7_uint8            channel_srcmask;  // MAscara do IP source em numero de bits (LSB)
} __attribute__((packed)) msg_MCAssocChannel_t;

/* To add or remove a static channel */
// Messages CCMSG_ETH_IGMP_STATIC_GROUP_ADD and CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE
typedef struct _msg_MCStaticChannel_t
{
  L7_uint8  SlotId;                    // slot
  L7_uint32 evc_id;                    // index: EVCid      /* L7_uint32 */
  msg_in_addr_t channelIp;             // IP of the Multicast Group
  msg_in_addr_t sourceIp;             //  IP of the Multicast Source
} __attribute__((packed)) msg_MCStaticChannel_t;

/* To List all channels */
// Message CCMSG_ETH_IGMP_GROUPS_GET
#define MSG_MCACTIVECHANNELS_CHANNELS_MAX       1024
typedef struct
{
   L7_uint8          SlotId;                                                  
   L7_uint32         evc_id;        /* XXX */
   msg_client_info_t client;       // Client information                                                
   L7_uint16         entryId;
} __attribute__((packed)) msg_MCActiveChannelsRequest_t;

typedef struct
{
   L7_uint16    entryId;
   L7_uint8     chType;
   L7_uint32    srcIP;
   L7_uint32    chIP;
} __attribute__((packed)) msg_MCActiveChannelsReply_t;

/* To list all clients of a channel */
// Message CCMSG_ETH_IGMP_CLIENT_GROUPS_GET
#define MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX  512
typedef struct _st_MCActiveChannelClients
{
  uint8  SlotId;                       // slot
  uint32 evc_id;                       // index: EVCid    /* XXX */
  msg_in_addr_t channelIp;             // IP do canal a consultar
  msg_in_addr_t sourceIp;              // IP source
  L7_uint16 page_index;                // Indice da mensagem
  L7_uint16 n_pages_total;             // Numero de mensagens para transportar todos os canais
  L7_uint16 n_clients_total;           // Numero total de canais
  L7_uint16 n_clients_msg;             // Numero de canais presentes na mensagem
  struct
  {
    L7_uint8             mask;
    L7_uint16            outer_vlan;       /* [mask=0x01] Outer vlan */
    L7_uint16            inner_vlan;       /* [mask=0x02] Inner vlan */
    msg_HwEthInterface_t intf;             /* [mask=0x04] interface  */  
  } __attribute__((packed))  clients_list[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX]; // List of clients
} __attribute__((packed)) msg_MCActiveChannelClients_t;

typedef struct
{
  L7_uint8             lock;
  L7_uint32            rxErrors;
} __attribute__((packed)) msg_rxStatus_t;

typedef struct
{
  L7_uint8             SlotId;
  msg_HwEthInterface_t intf;
  L7_uint8             enable;
  msg_rxStatus_t       rxStatus;
} __attribute__((packed)) msg_ptin_pcs_prbs;


/***************************************************** 
 * SLOT MODE CONFIGURATION
 ****************************************************/

/* Slot mode configuration */
// Message CCMSG_SLOT_MAP_MODE_GET, CCMSG_SLOT_MAP_MODE_VALIDATE
#define MSG_SLOTMODECFG_NSLOTS  20
typedef struct
{
  L7_uint8 SlotId;           // Slot id: Slot de destino para a mensagem
  struct {                   // List de port modes para o backplane:
    L7_uint8 slot_config;        // 1 para configurar a respectiva slot; 0, para ignorar este elemento
    L7_uint8 slot_index;         // Índice da slot a configurar
    L7_uint8 slot_mode;          // Modo da slot
  } slot_list[MSG_SLOTMODECFG_NSLOTS];
} __attribute__((packed)) msg_slotModeCfg_t;

/***************************************************** 
 * INTERNAL MESSAGES
 ****************************************************/

// HW Resources
// Message CCMSG_APPLICATION_RESOURCES

#define PTIN_POLICY_MAX_STAGES 3
#define PTIN_POLICY_MAX_GROUPS 16

typedef struct
{
  L7_uint16 rules;
  L7_uint16 counters;
  L7_uint16 meters;
  L7_uint16 slice_width;
} __attribute__((packed))msg_ptin_group_param;

typedef struct
{
  uint8 inUse;
  int   group_id;
  msg_ptin_group_param total;
  msg_ptin_group_param free;
  msg_ptin_group_param count;
} __attribute__((packed))msg_ptin_ffp_rules;

/* Policy Resources */
typedef struct
{
  uint8  SlotId;                       // slot
  msg_ptin_ffp_rules cap[PTIN_POLICY_MAX_GROUPS][PTIN_POLICY_MAX_STAGES];
  struct {
    L7_uint16 ing_stag;
    L7_uint16 ing_dtag;
    L7_uint16 egr_stag;
  } __attribute__((packed))vlanXlate_available_entries;
  struct {
    L7_uint16 igmp;
    L7_uint16 dhcp;
    L7_uint16 bcastLim;
  } __attribute__((packed))vlans_available;
  L7_uint16 crossconnexs_available_entries;
  L7_uint16 flowCounters_available_entries;
  L7_uint16 flowProfiles_available_entries;
} __attribute__((packed))msg_ptin_policy_resources;

/**************************************************************************** 
 * UPLINK PROTECTION
 ****************************************************************************/

/* Protection command received from MX control fw */
// Message CHMSG_ETH_UPLINK_COMMAND
typedef struct
{
  uint8 slotId;
  uint8 port;                 // Port index
  uint8 protCmd;              // Protection command: bit0-Port mode (1:active;0:inactive) / bit1-Command type (1:forced;0:normal)
} __attribute__ ((packed)) msg_uplinkProtCmd;


/**************************************************************************** 
 * IP Source Guard
 ****************************************************************************/

typedef enum
{
  IPSG_DISABLE       = 0,
  IPSG_ENABLE        = 1
} IPSG_ENABLE_t;

typedef enum
{  
  IPSG_ACTION_REMOVE    = 0,
  IPSG_ACTION_ADD       = 1
} IPSG_ACTION_t;

typedef enum
{
  IPSG_EVC_ID     = 1,
  IPSG_ROOT_VLAN  = 2,
  IPSG_ID_ALL     = 3
} IPSG_ID_TYPE_t;

typedef enum
{  
  IPSG_STATIC_BINDING       = 0,
  IPSG_DYNAMIC_BINDING      = 1
} IPSG_BINDING_TYPE_t;

/* Message to enable/disable IP Source Guard (IPSG) on a given Interface  */
typedef struct {  
  L7_uint8              slotId;           //Slot ID
  msg_HwEthInterface_t  intf;             //Interface   
  L7_uint8              enable;           //1=True | 0=False  
} __attribute__((packed)) msg_IPSG_set_t;

/* Message to add/remove IP Source Guard (IPSG) on a given Interface  */
typedef struct {
  L7_uint8              slotId;           // Slot ID
  L7_uint8              idType;           // idType=1 [evcId]; idType=2 [rootVlan]       
  L7_uint32             id;               // idType=1 [evcId]; idType=2 [rootVlan]   
  L7_uint32             evc_idx;          // eEVCid      
  msg_HwEthInterface_t  intf;             // Interface
  L7_uint8              action;           // IPSG_ACTION_t
  L7_uint8              macAddr[6];       // MAC Address  
  chmessage_ip_addr_t   ipAddr;           // IP address  
} __attribute__((packed)) msg_IPSG_static_entry_t;

typedef struct {  
  L7_uint16             entry_index;
} __attribute__((packed)) msg_ipsg_binding_table_request_t;

typedef struct {
  L7_uint16             entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint32             evc_idx;                // EVCid                          
  L7_uint16             outer_vlan;             // Service vlan: not used yet
  L7_uint16             inner_vlan;             // Client vlanId
  msg_HwEthInterface_t  intf;                   // Interface
  L7_uint8              macAddr[6];             // MAC Address
  chmessage_ip_addr_t   ipAddr;                 // IP address  
  L7_uint8              bindingType;            // Binding type: 0=Static, 1=Dynamic
  L7_uint8              hwStatus;                // HW Status: 0=Disabled, 1=Enabled
} __attribute__((packed)) msg_IPSG_binding_entry;

/* IPSG Binding table IPv6 compatible */
typedef struct {
  uint8                   slotId;                         // slot
  uint16                  page;                           // Page index
  L7_uint16               binding_table_total_entries;    // Total entries in Bind table
  L7_uint16               binding_table_msg_size;         // Number of entries in this message: up to 128
  msg_IPSG_binding_entry  binding_table[128];                // Binding table
} __attribute__((packed)) msg_ipsg_binding_table_response_t;

#endif //_MAIN_H_

