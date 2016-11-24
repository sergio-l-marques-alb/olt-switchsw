#ifndef __MAIN__H_
#define __MAIN__H_

#include "globals.h"

extern int canal_buga;

// retirado do ficheiro  u-boot-1.1.x/include/pci_ids.h
#define PCI_VENDOR_ID_BROADCOM          0x14e4

#define SYSTEM_N_PORTS  18
#define SYSTEM_N_LAGS   SYSTEM_N_PORTS
#define SYSTEM_N_INTERF (SYSTEM_N_PORTS+SYSTEM_N_LAGS)
#define CLIENTS_MAX     (128*8)

#define CHMSG_RESET_ALARMS                                  0x200E      // Reset Alarms
#define CHMSG_RESET_DEFAULTS                                0x200F      // Reset defaults

#define CHMSG_IF_GPON_ONU_TEST_TRAFEGO_SWITCH               0x9999      // in st_HWGponOnu                                || out uin#define CHMSG_IF_GPON_ONU_TEST_TRAFEGO_SWITCH               0x9999      // in st_HWGponOnu                                || out uint
#define CHMSG_IF_GPON_ONU_TEST2_TRAFEGO_SWITCH              0x9998      // in st_HWGponOnu                                || out uin#define CHMSG_IF_GPON_ONU_TEST_TRAFEGO_SWITCH               0x9999      // in st_HWGponOnu                                || out uint
#define CHMSG_IF_GPON_ONU_TEST3_TRAFEGO_SWITCH              0x9997      // in st_HWGponOnu                                || out uin#define CHMSG_IF_GPON_ONU_TEST_TRAFEGO_SWITCH               0x9999      // in st_HWGponOnu                                || out uint

#define CHMSG_APPLICATION_IS_ALIVE                          0x9000
#define CHMSG_APPLICATION_RESOURCES                         0x9001
#define CHMSG_IF_GPON_ONU_TEST_SWITCH_SHELL                 0x9002      // in st_HWGponOnu                                || out uin#define CHMSG_IF_GPON_ONU_TEST_TRAFEGO_SWITCH               0x9999
#define CHMSG_IF_GPON_ONU_SWITCH_PORT_CONFIG                0x9003      // in st_HWEthPhyConf
#define CHMSG_IF_GPON_ONU_SWITCH_CROSSLINK_ADD              0x9005      // in st_HWGponSwitchCrossLink
#define CHMSG_IF_GPON_ONU_SWITCH_CROSSLINK_REMOVE           0x9006      // in st_HWGponSwitchCrossLink
#define CHMSG_IF_GPON_ONU_SWITCH_PORT_TYPE_SET              0x9008      // in st_HWVlanDefs
#define CHMSG_IF_GPON_ONU_SWITCH_TRANSLATE_ADD              0x900A      // in st_HWGponSwitchXlate
#define CHMSG_IF_GPON_ONU_SWITCH_TRANSLATE_REMOVE           0x900B      // in st_HWGponSwitchXlate

#define CHMSG_ETH_PORT_COS_CONFIG_SET       0x9010
#define CHMSG_ETH_PORT_COS_CONFIG_GET       0x9011

#define CHMSG_ETH_MAC_TABLE_SHOW            0x9020
#define CHMSG_ETH_MAC_STATIC_TABLE          0x9021
#define CHMSG_ETH_MAC_ENTRY_REMOVE          0x9022
#define CHMSG_ETH_MAC_ENTRY_ADD             0x9023 // in st_switch_mac_operation

#define CHMSG_OLT_GET_PORT_CONF             0x90A1 // in st_HWEthPhyConf
#define CHMSG_OLT_GET_PORT_STATE            0x90A2 // in st_HWEthPhyState
#define CHMSG_OLT_GET_SFP_STATUS            0x90A3 // in st_HWEthSFPstatus
#define CHMSG_OLT_GET_SFP_INFO              0x90A4 // in st_HWSFFInfo

#define CHMSG_ETH_FLOW_READ                 0x9100   // in MSG_HwEthernetMef10EvcBundling
#define CHMSG_ETH_FLOW_ADMIN                0x9101   // in MSG_HwEthernetMef10p1EvcAdmin
#define CHMSG_ETH_FLOW_ADD                  0x9102   // in MSG_HwEthernetMef10EvcBundling
#define CHMSG_ETH_FLOW_REMOVE               0x9103   // in MSG_HwEthernetMef10p1EvcRemove
#define CHMSG_ETH_BW_PROFILE_CREATE         0x910A   // in MSG_HwEthernetProfile
#define CHMSG_ETH_BW_PROFILE_REMOVE         0x910B   // in MSG_HwEthernetProfile
#define CHMSG_ETH_BW_PROFILE_GET            0x910C   // in MSG_HwEthernetProfile

#define CHMSG_ETH_DHCP_PROFILE_GET          0x9110
#define CHMSG_ETH_DHCP_PROFILE_CONFIG       0x9111
#define CHMSG_ETH_DHCP_PROFILE_REMOVE       0x9112
#define CHMSG_ETH_DHCP_ACCESS_NODE_ID_SET   0x9113

//#define CHMSG_ETH_MC_CLIENT_SHOW            0x9400
#define CHMSG_ETH_MC_FLOW_ADD               0x9401
#define CHMSG_ETH_MC_FLOW_REMOVE            0x9402
#define CHMSG_ETH_MC_CLIENT_ADD             0x9404
#define CHMSG_ETH_MC_CLIENT_REMOVE          0x9405
#define CHMSG_ETH_MC_CLIENT_STATS_SHOW      0x940A
#define CHMSG_ETH_MC_PORT_STATS_SHOW        0x940B
#define CHMSG_ETH_MC_CLIENT_STATS_CLEAR     0x940C
#define CHMSG_ETH_MC_PORT_STATS_CLEAR       0x940D
#define CHMSG_ETH_DHCP_CLIENT_STATS_SHOW    0x9410
#define CHMSG_ETH_DHCP_PORT_STATS_SHOW      0x9411
#define CHMSG_ETH_DHCP_CLIENT_STATS_CLEAR   0x9412
#define CHMSG_ETH_DHCP_PORT_STATS_CLEAR     0x9413

#define CHMSG_ETH_DHCP_BIND_TABLE_SHOW      0x9415
#define CHMSG_ETH_DHCP_BIND_TABLE_ADD       0x9416
#define CHMSG_ETH_DHCP_BIND_TABLE_REMOVE    0x9417


#define CHMSG_ETH_MC_STATIC_CHANNEL_ADD     0x9420
#define CHMSG_ETH_MC_STATIC_CHANNEL_DEL     0x9421
#define CHMSG_ETH_MC_ACTIVE_CHANNELS_GET    0x9422
#define CHMSG_ETH_MC_CHANNEL_CLIENTS_GET    0x9423

#define CHMSG_ETH_SNOOP_MODE                0x9501
#define CHMSG_ETH_SNOOP_INTF_MANGMT         0x9503
#define CHMSG_ETH_SNOOP_VLAN_MANGMT         0x9505
#define CHMSG_ETH_SNOOP_QUERIER_MANGMT      0x9507

#define CHMSG_ETH_GET_COUNTERS              0x9201   // in st_HWEthRFC2819Statistics
#define CHMSG_ETH_CLEAR_COUNTERS            0x9202   // in st_HWEthRFC2819Statistics
#define CHMSG_ETH_GET_VLAN_COUNTERS         0x9203   // in st_HWEth_FlowStatistics
#define CHMSG_ETH_GET_FLOW_COUNTERS_ABS     0x9204
#define CHMSG_ETH_GET_FLOW_COUNTERS_DIFF    0x9205
#define CHMSG_ETH_ADD_FLOW_COUNTERS         0x9206
#define CHMSG_ETH_REMOVE_FLOW_COUNTERS      0x9207

#define CHMSG_ETH_LACP_LAG_READ             0x9600
#define CHMSG_ETH_LACP_LAG_CREATE           0x9601
#define CHMSG_ETH_LACP_LAG_DESTROY          0x9602
#define CHMSG_ETH_LACP_LAG_STATUS           0x9603
#define CHMSG_ETH_LACP_ADMINSTATE_SET       0x9604
#define CHMSG_ETH_LACP_ADMINSTATE_GET       0x9605
#define CHMSG_ETH_LACP_STATS_SHOW           0x9610
#define CHMSG_ETH_LACP_STATS_CLEAR          0x9611

#define CHMSH_ETH_CHANGE_STDOUT             0x9300

#define CHMSG_ETH_NTW_CONNECTIVITY_SHOW     0x9F00
#define CHMSG_ETH_NTW_CONNECTIVITY          0x9F01
#define CHMSG_ETH_ROUTE_CONNECTIVITY        0x9F02

#define CCMSG_ETH_SWITCH_SET                0x9A00

#define GPON_LINK_MIN_NUM                 4
#define GPON_LINK_MAX_NUM                 8
#define GPON_DEVICE_MIN_NUM               1
#define GPON_DEVICE_MAX_NUM               2
#define GPON_LINK_PER_DEVICE              4
#define GPON_LINK_MAX_FLOWS               200
#define GPON_ONU_PER_LINK_MIN_NUM         64
#define GPON_ONU_PER_LINK_MAX_NUM         128
#define GPON_US_BW_MAX                    1000 // teoricamente este numero  igual ao numero de t-conts por onu
#define GPON_US_BW_PER_ONU                10 // teoricamente este numero  igual ao numero de t-conts por onu
#define GPON_PROFILE_MAX_NUM              50
#define GPON_PROFILE_TYPE_MAX_NUM         10
#define GPON_ONU_CARDHOLDER_MAX_NUM       10
#define GPON_ONU_TCONT_MAX_NUM            20  // limite teorico256
#define GPON_ONU_GEMPORT_MAX_NUM          4095
#define GPON_ONU_GEMPORT_OFFSET           254
#define GPON_ONU_GEMPORT_INTERVAL_ONU     30
#define GPON_ONU_ALLOCID_OFFSET           1000
#define GPON_ONU_ANIG_MAX_NUM             16
#define GPON_SLOT_PER_ONU_MIN_NUM         1
#define GPON_SLOT_PER_ONU_MAX_NUM         2
#define GPON_ONU_PQUEUE_MAX_NUM           256   // limite pratico = GPON_ONU_TCONT_MAX_NUM  ||   limite teorico 0x0000 a 0xffff 65535-0+1=65536 
#define GPON_ONU_PQUEUE_MAX_INDEX_NUM     65536 // limite teorico 0x0000 a 0xffff 65535-0+1=65536 
#define GPON_ONU_MAX_ETHERNET_PORTS       8

#define GPON_SFP_NUM                      16
#define GPON_XFP_NUM                      2
#define GPON_SFP_XFP_NUM                  ((GPON_SFP_NUM)+(GPON_XFP_NUM))

#define CE_VENDOR_ID_FIELD_SIZE       ( 4 )
#define CE_VENDOR_SPECIFIC_FIELD_SIZE ( 4 )


/* Switch general configuration */
typedef struct {
  int32  err_code;                   /* unused in commands (only in responses */
  uint32 mask;                       /* Mask (32 bits for alignment purposes)
                                            0x01 - Aging Time  */
  int32  aging_time;                 /* Forwarding Database Aging Interval
                                           (10-1000000 */
} __attribute__((packed)) L7_switch_config;

typedef struct _st_HWEthPhyState_Block {
  uint16 Mask;
  uint8  Speed;              // 0x0001  0 - AutoNeg;  1 - 10;    2 - 100; 3 - 1000
  uint8  Duplex;             // 0x0002  0 - Half;     1 - Full
  uint8  Collisions;         // 0x0004  0 - OK;   1 - NOK
  uint8  RxActivity;         // 0x0008  0 - Sem;  1 - Com
  uint8  TxActivity;         // 0x0010  0 - Sem;  1 - Com
  uint8  LinkUp;             // 0x0020  0 - Down; 1 - Up
  uint8  AutoNegComplete;    // 0x0040  0 - incompleted; 1 - completed
  uint8  TxFault;            // 0x0080  0 - ok;   1 - Tx_fault
  uint8  RemoteFault;        // 0x0100  0 - ok;   1 - remote_fault recebido
  uint8  LOS;                // 0x0200  0 - s/ LOS; 1 - em LOS
  uint8  Media;              // 0x0400  0 - electric; 1 - optic
  uint8  MTU_mismatch;       // 0x0800  0 - MTU suported; 1 - MTU not suported
  uint16 Suported_MaxFrame;// 0x1000  1518 to 9600
} __attribute__((packed)) st_HWEthPhyState_Block;

typedef struct _st_HWEthPhyState
{
  uint8  Port;
  st_HWEthPhyState_Block HWEthPhyState_Block;
} __attribute__((packed)) st_HWEthPhyState;


typedef struct _st_HWEthPhyConf_Block {
  uint16  Mask;
  uint8   Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 ? 1000;  4 ? 2500; 5 - 10000
  uint8   Media;              // 0x0002   0 - electric; 1 - optic;   2 - Serdes
  uint8   Duplex;             // 0x0004   0 - Half;     1 - Full
  uint8   LoopBack;           // 0x0008   0 - s/ loop;  1 - Far End; 2 - swap loop; 3 - local oam loop; 4 - remote oam loop
  uint8   FlowControl;        // 0x0010   0 - Off;      1 - Tx;      2 - Rx;        3 - Both
  uint8   PortEnable;         // 0x0020   0 - disabled; 1 - enabled
  uint16  MaxFrame;           // 0x0040
  uint8   VlanAwareness;      // 0x0080   0 - unaware;  1 - aware
  uint8   MacLearning;        // 0x0100   0 - don't learn; 1 - learn
  uint8   AutoMDI;            // 0x0200   0 - disable;     1 - enable
} __attribute__((packed)) st_HWEthPhyConf_Block; // st_MC_Man_PhyConf

// Switch Port PHY configurations
typedef struct _st_HWEthPhyConf {
  uint8   Port;               // 0-7:Pon; 8-15:1/2.5G Eth; 16-17:10G Eth
  st_HWEthPhyConf_Block HWEthPhyConf_Block;
} __attribute__((packed)) st_HWEthPhyConf; // st_MC_Man_PhyConf

// Switch port type definition (MEF)
typedef struct _st_HWVlanDefs {
  uint8   Port;
  uint8   Mask;
  uint16  defVid;                    // 0x01
  uint8   defPrio;                   // 0x02    
  uint8   acceptable_frame_types;    // 0x04
  uint8   ingress_filter;            // 0x08
  uint8   restricted_vlan_reg;       // 0x10
  uint8   vlan_aware;                // 0x20
  uint8   type;                      // 0x40   UNI=1, NNI=2
} __attribute__((packed)) st_HWVlanDefs;




#define MEF10_EVC_UNITYPE_ROOT    0
#define MEF10_EVC_UNITYPE_LEAF    1
#define MEF10_EVC_UNITYPE_UNUSED ((uint8) -1)

//T_EVC_CEVID_MAP
typedef struct {
  uint8    type;       // enum{0 - root, 1 - leaf, -1 - unused}
  uint16   ceVid;      // gamma[1, .., 4094]
} __attribute__((packed)) st_HwEthernetMef10CeVidMap;

typedef struct {
  int err_code;               /*unused in commands (only responses)*/
  uint64 index;                  //index: 0..N_MACs-1
} __attribute__((packed)) MSG_GENERIC_PREFIX_STRUCT;

// Definição de um EVC
typedef struct {
  int err_code;                 /*unused in commands (only responses)*/
  uint64 index;                 //index: 0..N_MACs-1
  uint8                         type;                      // { 0 - p2p, 1 - mp2mp, 2 - rooted mp, 3 - remover }
  uint8                         admin;                     // { 0 - criar, 1 - modificar, 2 - Flush }
  uint8                         bundling;                  // false:true(0:1)
  uint16                        outerTag;                  // gamma [1, .., 4094]
  uint8                         ceVidBitmap[1 << (12-3)];  // mapa de bits para vlans, o acesso a vlan x e feito por ((bitmap[x/8] >> x%8) & 0x01)
  st_HwEthernetMef10CeVidMap    uni[SYSTEM_N_PORTS];       // mapa de pares portos/vlan (vlan apenas interessa quando nao existe bundling)
} __attribute__((packed)) MSG_HwEthernetMef10EvcBundling;

// Definição de um perfil de largura de banda
typedef struct {
  int err_code;               /*unused in commands (only responses)*/
  uint64 index;               //index: bits 63-48:evc_id; 47-32:c_vid; 31-16:s_vid; 15-8:CoS; 7:rx1_tx0; 6:no_profile; 5-0:port;
  uint8                       colourMode;
  uint8                       couplingFlag;
  uint64                      CIR;
  uint64                      CBS;
  uint64                      EIR;
  uint64                      EBS;
} __attribute__((packed)) MSG_HwEthernetProfile;

// DHCP Op82: Set Access Node Identifier
typedef struct {
  int   err_code;             //unused in commands (only responses)
  char  access_node_id[41];
} __attribute__ ((packed)) st_HwEthernetAccessNodeId;

// Definição de um perfil de largura de banda
typedef struct {
  int err_code;               //unused in commands (only responses)
  uint64 index;            //index: bits 63-48:evc_id; 47-32:c_vid; 31-16:s_vid; 15-8:CoS; 7:rx1_tx0; 6:no_profile; 5-0:port;
  char circuitId[64];
  char remoteId[64];
} __attribute__((packed)) st_HwEthernetDhcpOpt82Profile;

// Activação de um EVC
typedef struct {
  int err_code;               /*unused in commands (only responses)*/
  uint64 index;                  //index: 0..N_MACs-1
  uint8                      admin;                     // { 0 - inactivo, 1 - activo }
} __attribute__((packed)) MSG_HwEthernetMef10p1EvcAdmin;

// Remoção de um EVC
typedef struct {
  int err_code;               /*unused in commands (only responses)*/
  uint64 index;                  //index: 0..N_MACs-1
} __attribute__((packed)) MSG_HwEthernetMef10p1EvcRemove;

typedef struct {
  uint16                pvid;       //1-4094    Port VLAN identifier for port-based VLAN
  //not used in our implementation: the VID set for port-and-protocol based VLAN classification (max_entries and the entries, where each entry is a pair VID,PROTOCOL_TEMPLATE_ID)
  uint8    acceptable_frame_types;
  uint8                 ingress_filter;     //accept only frames from VLANs members of this port
  uint8                 restricted_VLAN_reg;
} __attribute__((packed)) _802_1Q_VLAN_MAN_PORT;

// Estruturas de contadores
typedef struct _st_HWEthRFC2819_Statistics_Block
{
  unsigned long long etherStatsDropEvents;               // 0x00000001
  unsigned long long etherStatsOctets;                   // 0x00000002
  unsigned long long etherStatsPkts;                     // 0x00000004
  unsigned long long etherStatsBroadcastPkts;            // 0x00000008
  unsigned long long etherStatsMulticastPkts;            // 0x00000010
  unsigned long long etherStatsCRCAlignErrors;           // 0x00000020
  unsigned long long etherStatsUndersizePkts;            // 0x00000040
  unsigned long long etherStatsOversizePkts;             // 0x00000080
  unsigned long long etherStatsFragments;                // 0x00000100
  unsigned long long etherStatsJabbers;                  // 0x00000200
  unsigned long long etherStatsCollisions;               // 0x00000400
  unsigned long long etherStatsPkts64Octets;             // 0x00000800
  unsigned long long etherStatsPkts65to127Octets;        // 0x00001000
  unsigned long long etherStatsPkts128to255Octets;       // 0x00002000
  unsigned long long etherStatsPkts256to511Octets;       // 0x00004000
  unsigned long long etherStatsPkts512to1023Octets;      // 0x00008000
  unsigned long long etherStatsPkts1024to1518Octets;     // 0x00010000
  unsigned long long etherStatsPkts1519toMaxOctets;      // Proprietary
  unsigned long long Throughput;
} __attribute__((packed)) st_HWEthRFC2819_Statistics_Block; //144

typedef struct _st_HWEthRFC2819_PortStatistics_Block
{
  uint32  RxMask;
  uint32  TxMask;
  st_HWEthRFC2819_Statistics_Block Rx;      // 0x01
  st_HWEthRFC2819_Statistics_Block Tx;      // 0x02
} __attribute__((packed)) st_HWEthRFC2819_PortStatistics_Block; //296

typedef struct _st_HWEthRFC2819Statistics {
  // BYTE Slot;      Não necessário
  // BYTE OutPort;   Não necessário
  // BYTE Inventory; Não necessário
  uint8 Port;
  uint8 Mask;
  st_HWEthRFC2819_PortStatistics_Block PortStatistics;     
} __attribute__((packed)) st_HWEthRFC2819Statistics;

typedef struct _st_HWEth_VlanStatistics_Block
{
  uint64 etherNotRedPackets;
  uint64 etherRedPackets;
} __attribute__((packed)) st_HWEth_VlanStatistics_Block;

typedef struct _st_HWEth_VlanStat
{
  uint16 CVid;
  st_HWEth_VlanStatistics_Block stat;
} __attribute__((packed)) st_HWEth_VlanStat; //296

typedef struct _st_HWEth_VlanStatistics
{
  uint16  FlowId;
  uint16  SVid;
  st_HWEth_VlanStat stat;
} __attribute__((packed)) st_HWEth_VlanStatistics; //296

typedef struct _st_DHCP_Statistics_t
{
  uint64 dhcp_rx_intercepted;
  uint64 dhcp_rx;
  uint64 dhcp_rx_filtered;
  uint64 dhcp_tx_forwarded;
  uint64 dhcp_tx_failed;
  uint64 dhcp_rx_client_requests_without_option82;
  uint64 dhcp_tx_client_requests_with_option82;
  uint64 dhcp_rx_server_replies_with_option82;
  uint64 dhcp_tx_server_replies_without_option82;
  uint64 dhcp_rx_client_pkts_withoutOp82_onTrustedIntf;
  uint64 dhcp_rx_client_pkts_withOp82_onUntrustedIntf;
  uint64 dhcp_rx_server_pkts_withOp82_onUntrustedIntf;
  uint64 dhcp_rx_server_pkts_withoutOp82_onTrustedIntf;
} __attribute__((packed)) st_DHCP_Statistics_t;

typedef struct _st_ClientDhcpStatistics
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: 0..N_MACs-1
  uint16 cvid;
  uint16 port;
  st_DHCP_Statistics_t stats;
} __attribute__((packed)) st_ClientDhcpStatistics;

typedef struct _st_IGMP_Statistics_t
{
  uint64 active_groups;
  uint64 active_clients;
  uint64 igmp_sent;
  uint64 igmp_tx_failed;
  uint64 igmp_intercepted;
  uint64 igmp_dropped;
  uint64 igmp_received_valid;
  uint64 igmp_received_invalid;
  uint64 joins_sent;
  uint64 joins_received_success;
  uint64 joins_received_failed;
  uint64 leaves_sent;
  uint64 leaves_received;
  uint64 membership_report_v3;
  uint64 general_queries_sent;
  uint64 general_queries_received;
  uint64 specific_queries_sent;
  uint64 specific_queries_received;
} __attribute__((packed)) st_IGMP_Statistics_t;

typedef struct _st_ClientIgmpStatistics
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: 0..N_MACs-1
  uint16 cvid;
  uint16 port;
  st_IGMP_Statistics_t stats;
} __attribute__((packed)) st_ClientIgmpStatistics;

typedef struct _st_ClientIgmp
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: 0..N_MACs-1
  uint16 cvid;
} __attribute__((packed)) st_ClientIgmp;


// IGMP Snooping structures

typedef enum {
  PTIN_SNOOP_INTF_DISABLED=0,
  PTIN_SNOOP_INTF_CLIENT,
  PTIN_SNOOP_INTF_ROUTER,
  PTIN_SNOOP_INTF_NONE
} enum_intf_type;

typedef struct _st_snoop_intf_type
{
  uint8 intf;
  enum_intf_type type;
} st_snoop_intf_type;

typedef struct _st_snooping_cfg
{
  uint8        admin;
  uint8        fastleave_admin_mode;
  uint16       group_membership_interval;
  uint16       max_response_time;
  uint16       mrouter_present_expiration_time;
} st_snooping_cfg;

typedef struct _st_snooping_intf
{
  enum_intf_type  intf_type[SYSTEM_N_INTERF];
  st_snooping_cfg intf_config[SYSTEM_N_INTERF];
} st_snooping_intf;

typedef struct _st_snooping_vlan
{
  st_snoop_intf_type  port;
  uint16              snoop_vlan;
  uint16              snoop_vlan_xlate;
  st_snooping_cfg     snoop_vlan_config;
} st_snooping_vlan;

typedef struct _st_snooping_querier_vlan
{
  uint16  querier_vlan;
  uint8   querier_vlan_admin;
  uint8   querier_election_participate_mode;
  uint32  querier_ipaddress;
} st_snooping_querier_vlan;

typedef struct _st_snooping
{
  uint8  snooping_admin;             // default: disabled
  uint8  querier_admin;              // default: disabled
  uint32 querier_ipaddress;          // xx.xx.xx.xx
  uint16 querier_interval;           // 1-1800s: default=60s
  uint16 group_membership_interval;  // 2-3600s: default=260s
  uint8  igmp_version_router;        // default=2
  uint8  igmp_version;               // default=2
  uint8  igmp_packet_cos;            // 0-7: default=5
} st_snooping;

typedef struct _st_MCStaticChannel
{
  int err_code;                     // unused in commands (only responses)
  uint64 index;                     // index: EVCid
  uint32 channelIp;                 // IP do canal a adicionar
  uint32 port_bmp;                  // Bitmap das portas a associar
  uint32 portfwd_bmp;               // Bitmap das portas, onde o canal sera reencaminhado
} __attribute__((packed)) st_MCStaticChannel;

typedef struct _st_MCActiveChannels
{
  int err_code;                     // unused in commands (only responses)
  uint64 index;                     // index: EVCid
  uint16 page_index;                // Indice da mensagem
  uint16 n_pages_total;             // Numero de mensagens para transportar todos os canais
  uint16 n_channels_total;          // Numero total de canais
  uint16 n_channels_msg;            // Numero de canais presentes na mensagem
  uint32 channelsIp_list[1024];     // List da canais (apenas usados os primeiros n_channels_msg canais)
} __attribute__((packed)) st_MCActiveChannels;

typedef struct _st_MCActiveChannelClients
{
  int err_code;                     // unused in commands (only responses)
  uint64 index;                     // index: EVCid
  uint32 channelIp;                 // IP do canal a consultar
  uint16 n_clients;                 // Numero total de clients ligados a este canal
  uint32 clients_list_bmp[128];     // Lista de clients em formato bitmap (4096 bits para 4096 ids possiveis)
} __attribute__((packed)) st_MCActiveChannelClients;

typedef struct _st_LACPLagInfo
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: 0..N_MACs-1
  uint8  admin;                 // DISABLED=0 or ENABLED=1
  uint8  stp_enable;            // DISABLED=0 or ENABLED=1
  uint8  static_enable;         // DISABLED=0 or ENABLED=1
  uint8  loadBalance_mode;      // FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
  uint32 members_pbmp;          // Ports bitmap
} __attribute__((packed)) st_LACPLagInfo;

typedef struct _st_LACPLagStatus
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: 0..N_MACs-1
  uint8  admin;                 // DISABLED=0 or ENABLED=1
  uint8  link_status;           // DOWN=0 or UP=1
  uint8  port_channel_type;     // DYNAMIC=0 or STATIC=1
  uint32 members_pbmp;          // Ports bitmap
  uint32 active_members_pbmp;   // ACTIVE Ports bitmap
} __attribute__((packed)) st_LACPLagStatus;

typedef struct _st_LACPAdminState
{
  int err_code;                 //unused in commands (only responses)
  uint64 index;                 //index: port id (for physical interfaces)
  uint8  state_aggregation;
  uint8  lacp_activity;
  uint8  lacp_timeout;
} __attribute__((packed)) st_LACPAdminState;

typedef struct _st_LACPStats
{
  int err_code;                     // unused in commands (only responses)
  uint64 index;                     // index: Physical interface => 0..7 (internal Eth), 8-15 (External 1G Eth), 16..17 (External 10G eth)
  uint32 LACPdus_rx;                // LACPdu's received
  uint32 LACPdus_tx;                // LACPdu's transmitted
} __attribute__((packed)) st_LACPStats;


typedef struct _st_NtwConnectivity
{
  int err_code;                   // unused in commands (only responses)
  uint32 ipaddr;               // IP Addr: xx.xx.xx.xxh
  uint32 netmask;              // Netmask: xx.xx.xx.xxh
  uint32 defGateway;           // Default gateway: xx.xx.xx.xxh
  uint8  localMacAddr[6];      // Locally Admnistered MAC Address xx:xx:xx:xx:xx:xxh
  uint16 MngmVlanId;           // Management VlanId
  uint8  ntwConfProtocol;      // Network Configuration Protocol: 0:None, 1:Bootp, 2:DHCP
} __attribute__((packed)) st_NtwConnectivity;

typedef struct _st_RouteConnectivity
{
  int err_code;                   // unused in commands (only responses)
  uint32 ipaddr;               // IP Addr: xx.xx.xx.xxh
  uint32 netmask;              // Netmask: xx.xx.xx.xxh
  uint8  routeOp;              // 0: Add default gateway, 1:Add, 2:Remove, 3:Remove all
} __attribute__((packed)) st_RouteConnectivity;

typedef struct _st_QoSConfiguration
{
  int err_code;                     // unused in commands (only responses)
  uint64 index;                     // index:  0..7 (internal Eth), 8-15 (External 1G Eth), 16..17 (External 10G eth), [bit7]=1+lagId (Lag interface), 0xFF: All interfaces

  uint8  mask;                      // Configurations mask

  uint8  trust_mode;                // mask=0x01: 0-None, 1-Untrust markings, 2-802.1p marking, 3: IP-precedence mark; 4-DSCP mark (Default=2)
  uint8  bandwidth_unit;            // mask=0x02: 0: Percentage, 1: Kbps, 2: PPS (Default=0)
  uint32 shaping_rate;              // mask=0x04: 0-100: Default=0 (unlimited)

  struct {                          // Packet priority map
    uint8  mask;                    //   pktpriority map mask (nth bit, tells to configure the nth priority)
    uint32 cos[8];                  //   Mapping: CoS(pcp): Default={0,1,2,3,4,5,6,7}
  } pktprio;                        // mask=0x08: Packet priority map

  struct {                          // CoS configuration
    uint8 mask;                     //   CoS map mask (nth bit, tells to configure the nth CoS)
    struct {                        //   Specific CoS configuration
      uint8  mask;                  //     Specific CoS configuration mask
      uint8  scheduler;             //     mask=0x01: Scheduler type: 0-None, 1-Strict, 2-Weighted (Default=1)
      uint32 min_bandwidth;         //     mask=0x02: Minimum bandwidth (0-100): Default=0 (no guarantee)
      uint32 max_bandwidth;         //     mask=0x04: Maximum bandwidth (0-100): Default=0 (unlimited)
    } cos[8];                       //   Specific CoS configuration (8 queues)
  } cos_config;                     // mask=0x10: CoS configuration
} __attribute__((packed)) st_QoSConfiguration;

// To consult the MAC table
typedef struct st_switch_mac_entry {
  uint16 mac_index;               // MAC index (from 0 to 254)
  uint16 flow_id;                 // EVC id
  uint16 vlan_id;                 // Vlan
  uint8  port;                    // Port
  uint8  mac_address[6];          // MAC address
  uint8  static_address;          // Is static address?
} __attribute__((packed)) st_switch_mac_entry;
typedef struct st_switch_mac_table {
  int err_code;                      // unused in commands (only responses)
  uint64 index;                      // page index
  uint16 mac_table_total_entries;    // Total number of entries in MAC table
  uint16 mac_table_msg_size;         // Number of entries in this message: up to 256
  st_switch_mac_entry mac_table[256];       // MAC table
} __attribute__((packed)) st_switch_mac_table;

// Add (static) entries to the MAC table
typedef struct st_switch_mac_operation {
  int err_code;                   // unused in commands (only responses)
  uint16 flow_id;                 // EVC id
  uint16 vlan_id;                 // Vlan id
  uint8  port;                    // Port
  uint8  mac_address[6];          // MAC address
  uint8  static_address;          // Is static address?
} __attribute__((packed)) st_switch_mac_operation;


// DHCP Binding Entry
typedef struct {
  uint16 entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  uint16 flow_id;                // EVCid
  uint16 service_vlan;           // Service vlan: not used yet
  uint16 client_vlan;            // Client clanId
  uint8  port;                   // Port interface
  uint8  macAddr[6];             // MAC Address
  uint32 ipAddr;                 // IP address
  uint32 remLeave;               // Remaining Leave time in seconds
  uint8  bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} __attribute__((packed)) st_DHCP_bind_entry;

// DHCP Binding Table
typedef struct {
  int err_code;                       // unused in commands (only responses)
  uint64 index;                       // Page index
  uint16 bind_table_total_entries;    // Total entries in Bind table
  uint16 bind_table_msg_size;         // Number of entries in this message: up to 128
  st_DHCP_bind_entry bind_table[128]; // Bind table
} __attribute__((packed)) st_DHCP_bind_table;

// Flow counters
// Statistics
typedef struct {
  uint64 pktTotal;                   // Total packets (Unicast+Multicast+Broadcast)
  uint64 pktUnicast;                 // Unicast packets
  uint64 pktMulticast;               // Multicast packets
  uint64 pktBroadcast;               // Broadcast packets
  uint64 pktDropped;                 // Packets dropped
} st_ptin_flow_port_stats;
typedef struct {
  st_ptin_flow_port_stats pon_rx;    // Packets received at pon interfaces
  st_ptin_flow_port_stats eth_rx;    // PAckets received at eth interfaces
} st_ptin_flow_stats;
typedef struct {
  int              err_code;         // unused in commands (only responses)
  uint64           index;            // EVC id
  uint8            flow_type;        // Flow type
  union {
    uint32 client_vlan;              // Client vlan: if null, is not applicable
    uint32 channel_ip;               // IP channel: if null, is not applicable
  } c_id;                            // client or channel id, if is a UC or MC respectively
  st_ptin_flow_stats  stats;         // Statistics structure
  uint8 countersExist;               // TRUE, if these counters were created
} st_ptin_flow_counters;

// HW Resources
typedef struct
{
  uint16 ffp_available_rules[8];
  struct {
    uint16 ing_stag;
    uint16 ing_dtag;
    uint16 egr_stag;
  } vlanXlate_available_entries;
  struct {
    uint16 igmp;
    uint16 dhcp;
    uint16 bcastLim;
  } vlans_available;
  uint16 flowCounters_available_entries;
} st_ptin_hw_resources;
typedef struct
{
  int err_code;
  st_ptin_hw_resources resources;
} st_msg_ptin_hw_resources;

#endif //_MAIN_H_


