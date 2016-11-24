#ifndef _DTL_PTIN__H
#define _DTL_PTIN__H

#include "l7_common.h"
#include "defaultconfig.h"
#include "dapi.h"

#define L7_SYSTEM_N_PORTS   18
#define L7_SYSTEM_PON_PORTS 8
#define L7_SYSTEM_N_LAGS    L7_SYSTEM_N_PORTS
#define L7_SYSTEM_N_INTERF  (L7_SYSTEM_N_PORTS+L7_SYSTEM_N_LAGS)

typedef struct  {
  unsigned int    SlotIndex;                // (not used)
  unsigned int    BoardPresent;             // 1 \u2013 indica que o fastPath esta\u2019 iniciado.   
  unsigned int    BoardNInterfaces;         // (not used)                         
  unsigned int    BoardVersion;             // (not used)              
  unsigned int    BoardMode;                // (not used) 
  char            BoardSerialNumber[20];    // (Versao do fastPath)                   
  unsigned int    FpgaFixoID;               // (not used)                
  unsigned int    FpgaFixoVersion;          // (not used)                     
  unsigned int    FpgaFlexID;               // (not used)                
  unsigned int    FpgaFlexVersion;          // (not used)                    
  unsigned int    PCBVersion;               // (not used)
} L7_HwCardInfo;

typedef enum {
  PTIN_SNOOP_INTF_DISABLED=0,
  PTIN_SNOOP_INTF_CLIENT,
  PTIN_SNOOP_INTF_ROUTER,
  PTIN_SNOOP_INTF_NONE
} enum_intf_type;

typedef struct _st_snoop_intf_type
{
  L7_uint8 intf;
  enum_intf_type type;
} st_snoop_intf_type;

typedef struct _st_snooping_cfg
{
  L7_uint8        admin;
  L7_uint8        fastleave_admin_mode;
  L7_uint16       group_membership_interval;
  L7_uint16       max_response_time;
  L7_uint16       mrouter_present_expiration_time;
} st_snooping_cfg;

typedef struct _st_snooping_intf
{
  enum_intf_type  intf_type[L7_SYSTEM_N_INTERF];
  st_snooping_cfg intf_config[L7_SYSTEM_N_INTERF];
} st_snooping_intf;

typedef struct _st_snooping_vlan
{
  st_snoop_intf_type  port;
  L7_uint16           snoop_vlan;
  L7_uint16           snoop_vlan_xlate;
  st_snooping_cfg     snoop_vlan_config;
} st_snooping_vlan;

typedef struct _st_snooping_mrouter_vlan
{
  L7_uint8        admin;
  L7_uint8        intf;
  L7_uint16       vlan;
} st_snooping_mrouter_vlan;

typedef struct _st_snooping_querier_vlan
{
  L7_uint16 querier_vlan;
  L7_uint8  querier_vlan_admin;
  L7_uint8  querier_election_participate_mode;
  L7_uint32 querier_ipaddress;
} st_snooping_querier_vlan;

typedef struct _st_snooping
{
  L7_uint8  snooping_admin;             // default: disabled
  L7_uint8  querier_admin;              // default: disabled
  L7_uint32 querier_ipaddress;          // xx.xx.xx.xx
  L7_uint16 querier_interval;           // 1-1800s: default=60s
  L7_uint16 group_membership_interval;  // 2-3600s: default=260s
  L7_uint8  igmp_version_router;        // default=2
  L7_uint8  igmp_version;               // default=2
  L7_uint8  igmp_packet_cos;            // 0-7: default=5
} st_snooping;

typedef struct {
  int err_code;               //unused in commands (only responses)
  L7_uint64 index;                  //index: 0..N_MACs-1
} __attribute__ ((packed)) L7_MsgIndex;

//T_EVC_CEVID_MAP
typedef struct L7_HwEthernetMef10CeVidMap {
  L7_uint8    type;       // enum{0 - root, 1 - leaf, -1 - unused}
  L7_uint16   ceVid;      // gamma[1, .., 4094]
} __attribute__ ((packed)) L7_HwEthernetMef10CeVidMap;

// Definição de um EVC
typedef struct {
  int err_code;                 //unused in commands (only responses)
  L7_uint64 index;                 //index: 0..N_MACs-1
  L7_uint8                         type;                      // { 0 - p2p, 1 - mp2mp, 2 - rooted mp, 3 - remover }
  L7_uint8                         admin;                     // { 0 - criar, 1 - modificar, 2 - Flush }
  L7_uint8                         bundling;                  // false:true(0:1)
  L7_uint16                        outerTag;                  // gamma [1, .., 4094]
  L7_uint8                         ceVidBitmap[1 << (12-3)];  // mapa de bits para vlans, o acesso a vlan x e feito por ((bitmap[x/8] >> x%8) & 0x01)
  L7_HwEthernetMef10CeVidMap       uni[L7_SYSTEM_N_PORTS];       // mapa de pares portos/vlan (vlan apenas interessa quando nao existe bundling)
} __attribute__((packed)) L7_HwEthernetMef10EvcBundling;

// Remoção de um EVC
typedef struct {
  int err_code;               //unused in commands (only responses)
  L7_uint64 index;                  //index: 0..N_MACs-1
} __attribute__ ((packed)) L7_HwEthernetMef10p1EvcRemove;

// Definição de um perfil de largura de banda
typedef struct {
  int err_code;               //unused in commands (only responses)
  L7_uint64 index;               //index: bits 63-48:evc_id; 47-32:c_vid; 31-16:s_vid; 15-8:ponPort(0-7); 7-0:ethPort(8...);
  L7_uint8                       colourMode;
  L7_uint8                       couplingFlag;
  L7_uint64                      CIR;
  L7_uint64                      CBS;
  L7_uint64                      EIR;
  L7_uint64                      EBS;
} __attribute__ ((packed)) L7_HwEthernetProfile;

// DHCP Op82: Set Access Node Identifier
typedef struct {
  int err_code;             //unused in commands (only responses)
  L7_char8     access_node_id[FD_DS_MAX_ACCESS_NODE_ID_STRING];
  L7_char8     L2Type[FD_DS_MAX_L2_TYPE_STRING];  //    ETH, ATM, PON...
  L7_char8     slot[FD_DS_MAX_SLOT_STRING];       //    composto por Rack/Shelf/Slot [22/33/1]
} __attribute__ ((packed)) L7_HwEthernetAccessNodeId;

// Definição de um perfil de largura de banda
typedef struct {
  int err_code;               //unused in commands (only responses)
  L7_uint64 index;               //index: bits 63-48:evc_id; 47-32:c_vid; 31-16:s_vid; 7-0:port;
  L7_char8                       circuitId[FD_DS_MAX_REMOTE_ID_STRING];
  L7_char8                       remoteId[FD_DS_MAX_REMOTE_ID_STRING];
} __attribute__ ((packed)) L7_HwEthernetDhcpOpt82Profile;


// Switch Port PHY configurations
typedef struct _L7_HWEthPhyConf {
  L7_uint8   Port;               // 0-7:Pon; 8-15:1/2.5G Eth; 16-17:10G Eth
  L7_uint16  Mask;
  L7_uint8   Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 ? 1000;  4 ? 2500; 5 - 10000
  L7_uint8   Media;              // 0x0002   0 - electric; 1 - optic;   2 - Serdes
  L7_uint8   Duplex;             // 0x0004   0 - Half;     1 - Full
  L7_uint8   LoopBack;           // 0x0008   0 - s/ loop;  1 - Far End; 2 - swap loop; 3 - local oam loop; 4 - remote oam loop
  L7_uint8   FlowControl;        // 0x0010   0 - Off;      1 - Tx;      2 - Rx;        3 - Both
  L7_uint8   PortEnable;         // 0x0020   0 - disabled; 1 - enabled
  L7_uint16  MaxFrame;           // 0x0040
  L7_uint8   VlanAwareness;      // 0x0080   0 - unaware;  1 - aware
  L7_uint8   MacLearning;        // 0x0100   0 - don't learn; 1 - learn
  L7_uint8   AutoMDI;            // 0x0200   0 - disable;     1 - enable
} __attribute__ ((packed)) L7_HWEthPhyConf; // st_MC_Man_PhyConf

typedef struct _st_HWEthPhyState {
  L7_uint8  Port;
  L7_uint16 Mask;
  L7_uint8  Speed;              // 0x0001  0 - AutoNeg;  1 - 10;    2 - 100; 3 - 1000
  L7_uint8  Duplex;             // 0x0002  0 - Half;     1 - Full
  L7_uint8  Collisions;         // 0x0004  0 - OK;   1 - NOK
  L7_uint8  RxActivity;         // 0x0008  0 - Sem;  1 - Com
  L7_uint8  TxActivity;         // 0x0010  0 - Sem;  1 - Com
  L7_uint8  LinkUp;             // 0x0020  0 - Down; 1 - Up
  L7_uint8  AutoNegComplete;    // 0x0040  0 - incompleted; 1 - completed
  L7_uint8  TxFault;            // 0x0080  0 - ok;   1 - Tx_fault
  L7_uint8  RemoteFault;        // 0x0100  0 - ok;   1 - remote_fault recebido
  L7_uint8  LOS;                // 0x0200  0 - s/ LOS; 1 - em LOS
  L7_uint8  Media;              // 0x0400  0 - electric; 1 - optic
  L7_uint8  MTU_mismatch;       // 0x0800  0 - MTU suported; 1 - MTU not suported
  L7_uint16 Suported_MaxFrame;// 0x1000  1518 to 9600
} __attribute__((packed)) L7_HWEthPhyState;

typedef struct _L7_HWEth_VlanStatistics_Block
{
  L7_uint64 etherNotRedPackets;
  L7_uint64 etherRedPackets;
} __attribute__ ((packed)) L7_HWEth_VlanStatistics_Block;

typedef struct _L7_HWEth_VlanStat
{
  L7_uint16 CVid;
  L7_HWEth_VlanStatistics_Block stat;
} __attribute__ ((packed)) L7_HWEth_VlanStat; //296

typedef struct _L7_HWEth_VlanStatistics
{
  L7_uint16  FlowId;
  L7_uint16  SVid;
  L7_HWEth_VlanStat stat;
} __attribute__ ((packed)) L7_HWEth_VlanStatistics; //296

typedef struct _L7_HWEth_Statistics_State
{
  L7_uint32 status_mask;
  L7_uint32 status_value[L7_SYSTEM_N_PORTS];
} L7_HWEth_Statistics_State;


// Estruturas de contadores
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
  L7_uint64 etherStatsPkts1519toMaxOctets;      // Proprietary
  L7_uint64 Throughput;
} __attribute__ ((packed)) L7_HWEthRFC2819_Statistics_Block; //144

typedef struct _L7_HWEthRFC2819_PortStatistics
{
  L7_uint8   Port;
  L7_uint8   Mask;
  L7_uint32  RxMask;
  L7_uint32  TxMask;
  L7_HWEthRFC2819_Statistics_Block Rx;      // 0x01
  L7_HWEthRFC2819_Statistics_Block Tx;      // 0x02
} __attribute__ ((packed)) L7_HWEthRFC2819_PortStatistics; //296


typedef struct _L7_DHCP_Statistics_t
{
  L7_uint64 dhcp_rx_intercepted;
  L7_uint64 dhcp_rx;
  L7_uint64 dhcp_rx_filtered;
  L7_uint64 dhcp_tx_forwarded;
  L7_uint64 dhcp_tx_failed;
  L7_uint64 dhcp_rx_client_requests_without_option82;
  L7_uint64 dhcp_tx_client_requests_with_option82;
  L7_uint64 dhcp_rx_server_replies_with_option82;
  L7_uint64 dhcp_tx_server_replies_without_option82;
  L7_uint64 dhcp_rx_client_pkts_withoutOp82_onTrustedIntf;
  L7_uint64 dhcp_rx_client_pkts_withOp82_onUntrustedIntf;
  L7_uint64 dhcp_rx_server_pkts_withOp82_onUntrustedIntf;
  L7_uint64 dhcp_rx_server_pkts_withoutOp82_onTrustedIntf;
} __attribute__ ((packed)) L7_DHCP_Statistics_t;

typedef struct _L7_ClientDhcpStatistics
{
  int err_code;                 //unused in commands (only responses)
  L7_uint64 index;                 //index: 0..N_MACs-1
  L7_uint16 cvid;
  L7_uint16 port;
  L7_DHCP_Statistics_t stats;
} __attribute__ ((packed)) L7_ClientDhcpStatistics;


typedef struct _L7_IGMP_Statistics_t
{
  L7_uint64 active_groups;
  L7_uint64 active_clients;
  L7_uint64 igmp_sent;
  L7_uint64 igmp_tx_failed;
  L7_uint64 igmp_intercepted;
  L7_uint64 igmp_dropped;
  L7_uint64 igmp_received_valid;
  L7_uint64 igmp_received_invalid;
  L7_uint64 joins_sent;
  L7_uint64 joins_received_success;
  L7_uint64 joins_received_failed;
  L7_uint64 leaves_sent;
  L7_uint64 leaves_received;
  L7_uint64 membership_report_v3;
  L7_uint64 general_queries_sent;
  L7_uint64 general_queries_received;
  L7_uint64 specific_queries_sent;
  L7_uint64 specific_queries_received;
} __attribute__ ((packed)) L7_IGMP_Statistics_t;


typedef struct _L7_ClientIgmp
{
  int err_code;                 //unused in commands (only responses)
  L7_uint64 index;                 //index: 0..N_MACs-1
  L7_uint16 cvid;
} __attribute__ ((packed)) L7_ClientIgmp;

typedef struct _L7_ClientIgmpStatistics
{
  int err_code;                 //unused in commands (only responses)
  L7_uint64 index;                 //index: 0..N_MACs-1
  L7_uint16 cvid;
  L7_uint16 port;
  L7_IGMP_Statistics_t stats;
} __attribute__ ((packed)) L7_ClientIgmpStatistics;

typedef struct _L7_MCStaticChannel
{
  int err_code;                     // unused in commands (only responses)
  L7_uint64 index;                  // index: EVCid
  L7_uint32 channelIp;              // IP do canal a adicionar
  L7_uint32 port_bmp;               // Bitmap das portas a associar
  L7_uint32 portfwd_bmp;            // Bitmap das portas, onde o canal sera reencaminhado
} __attribute__ ((packed)) L7_MCStaticChannel;

typedef struct _L7_MCActiveChannels
{
  int err_code;                     // unused in commands (only responses)
  L7_uint64 index;                  // index: EVCid
  L7_uint16 page_index;             // Indice da mensagem
  L7_uint16 n_pages_total;          // Numero de mensagens para transportar todos os canais
  L7_uint16 n_channels_total;       // Numero total de canais
  L7_uint16 n_channels_msg;         // Numero de canais presentes na mensagem
//struct {
//  L7_uint32 channelIp;
//  L7_uint16 number_of_clients
//} channelsIp_list[1000];
  L7_uint32 channelsIp_list[1024];  // List da canais (apenas usados os primeiros n_channels_msg canais)
} __attribute__ ((packed)) L7_MCActiveChannels;

typedef struct _L7_MCActiveChannelClients
{
  int err_code;                     // unused in commands (only responses)
  L7_uint64 index;                  // index: EVCid
  L7_uint32 channelIp;              // IP do canal a consultar
  L7_uint16 n_clients;              // Numero total de clients ligados a este canal
  L7_uint32 clients_list_bmp[128];  // Lista de clients em formato bitmap (4096 bits para 4096 ids possiveis)
} __attribute__ ((packed)) L7_MCActiveChannelClients;

/* To add or remove a channel from the white list */
// Messages CCMSG_ETH_IGMP_CHANNEL_WLIST_GET or CCMSG_ETH_IGMP_CHANNEL_WLIST_ADD and CCMSG_ETH_IGMP_CHANNEL_WLIST_REMOVE
typedef struct _msg_MCChannelWList_t
{
  int         err_code;             // unused in commands (only responses)
  L7_uint64   entry_idx;            // Entry index: only for readings
  L7_uint16   evc_idx;              // Multicast EVC index
  L7_uint32   grpAddr;              // IP do canal a adicionar/remover
  L7_uint8    grpAddrmask;          // MAscara do canal em numero de bits (LSB)
} __attribute__((packed)) msg_MCChannelWList_t;


typedef struct _L7_LACPLagInfo
{
  int err_code;                 //unused in commands (only responses)
  L7_uint64 index;              //index: 0..N_MACs-1
  L7_uint8  admin;              // DISABLED=0 or ENABLED=1
  L7_uint8  stp_enable;         // DISABLED=0 or ENABLED=1
  L7_uint8  static_enable;      // DISABLED=0 or ENABLED=1
  L7_uint8  loadBalance_mode;   // FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
  L7_uint32 members_pbmp;       // Ports bitmap
} __attribute__ ((packed)) L7_LACPLagInfo;

typedef struct _L7_LACPLagStatus
{
  int err_code;                   //unused in commands (only responses)
  L7_uint64 index;                //index: 0..N_MACs-1
  L7_uint8  admin;                // DISABLED=0 or ENABLED=1
  L7_uint8  link_status;          // DOWN=0 or UP=1
  L7_uint8  port_channel_type;    // DYNAMIC=0 or STATIC=1
  L7_uint32 members_pbmp;         // Ports bitmap
  L7_uint32 active_members_pbmp;  // Ports bitmap
} __attribute__ ((packed)) L7_LACPLagStatus;

typedef struct _L7_LACPAdminState
{
  int err_code;                   //unused in commands (only responses)
  L7_uint64 index;                //index: port id (for physical interfaces)
  L7_uint8  state_aggregation;
  L7_uint8  lacp_activity;
  L7_uint8  lacp_timeout;
} __attribute__ ((packed)) L7_LACPAdminState;

typedef struct _L7_LACPStats
{
  int err_code;                     // unused in commands (only responses)
  L7_uint64 index;                  // index: Physical interface => 0..7 (internal Eth), 8-15 (External 1G Eth), 16..17 (External 10G eth)
  L7_uint32 LACPdus_rx;             // LACPdu's received
  L7_uint32 LACPdus_tx;             // LACPdu's transmitted
} __attribute__ ((packed)) L7_LACPStats;

typedef struct _L7_NtwConnectivity
{
  int err_code;                   // unused in commands (only responses)
  L7_uint32 ipaddr;               // IP Addr: xx.xx.xx.xxh
  L7_uint32 netmask;              // Netmask: xx.xx.xx.xxh
  L7_uint32 defGateway;           // Default gateway: xx.xx.xx.xxh
  L7_uint8  localMacAddr[6];      // Locally Admnistered MAC Address xx:xx:xx:xx:xx:xxh
  L7_uint16 MngmVlanId;           // Management VlanId
  L7_uint8  ntwConfProtocol;      // Network Configuration Protocol: 0:None, 1:Bootp, 2:DHCP
} __attribute__ ((packed)) L7_NtwConnectivity;

typedef struct _L7_RouteConnectivity
{
  int err_code;                   // unused in commands (only responses)
  L7_uint32 ipaddr;               // IP Addr: xx.xx.xx.xxh
  L7_uint32 netmask;              // Netmask: xx.xx.xx.xxh
  L7_uint8  routeOp;              // 0: Add default gateway, 1:Add, 2:Remove, 3:Remove all
} __attribute__ ((packed)) L7_RouteConnectivity;

typedef struct _L7_QoSConfiguration
{
  int err_code;                     // unused in commands (only responses)
  L7_uint64 index;                  // index:  0..7 (internal Eth), 8-15 (External 1G Eth), 16..17 (External 10G eth), [bit7]=1+lagId (Lag interface), 0xFF: All interfaces

  L7_uint8  mask;                   // Configurations mask

  L7_uint8  trust_mode;             // mask=0x01: 0-None, 1-Untrust markings, 2-802.1p marking, 3: IP-precedence mark; 4-DSCP mark (Default=2)
  L7_uint8  bandwidth_unit;         // mask=0x02: 0: Percentage, 1: Kbps, 2: PPS (Default=0)
  L7_uint32 shaping_rate;           // mask=0x04: 0-100: Default=0 (unlimited)

  struct {                          // Packet priority map
    L7_uint8  mask;                 //   pktpriority map mask (nth bit, tells to configure the nth priority)
    L7_uint32 cos[8];               //   Mapping: CoS(pcp): Default={0,1,2,3,4,5,6,7}
  } pktprio;                        // mask=0x08: Packet priority map

  struct {                          // CoS configuration
    L7_uint8 mask;                  //   CoS map mask (nth bit, tells to configure the nth CoS)
    struct {                        //   Specific CoS configuration
      L7_uint8  mask;               //     Specific CoS configuration mask
      L7_uint8  scheduler;          //     mask=0x01: Scheduler type: 0-None, 1-Strict, 2-Weighted (Default=1)
      L7_uint32 min_bandwidth;      //     mask=0x02: Minimum bandwidth (0-100): Default=0 (no guarantee)
      L7_uint32 max_bandwidth;      //     mask=0x04: Maximum bandwidth (0-100): Default=0 (unlimited)
    } cos[8];                       //   Specific CoS configuration (8 queues)
  } cos_config;                     // mask=0x10: CoS configuration
} __attribute__ ((packed)) L7_QoSConfiguration;

// DHCP Binding Entry
typedef struct {
  L7_uint16 entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint16 flow_id;                // EVCid
  L7_uint16 service_vlan;           // Service vlan: not used yet
  L7_uint16 client_vlan;            // Client clanId
  L7_uint8  port;                   // Port interface
  L7_uint8  macAddr[6];             // MAC Address
  L7_uint32 ipAddr;                 // IP address
  L7_uint32 remLeave;               // Remaining Leave time in seconds
  L7_uint8  bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} __attribute__ ((packed)) L7_DHCP_bind_entry;
// DHCP Binding Table
typedef struct {
  int err_code;                         // unused in commands (only responses)
  L7_uint64 index;                      // Page index
  L7_uint16 bind_table_total_entries;   // Total entries in Bind table
  L7_uint16 bind_table_msg_size;        // Number of entries in this message: up to 128
  L7_DHCP_bind_entry bind_table[128];   // Bind table
} __attribute__ ((packed)) L7_DHCP_bind_table;

// To consult the MAC table
typedef struct L7_switch_mac_entry {
  L7_uint16 mac_index;               // MAC index (from 0 to 254)
  L7_uint16 flow_id;                 // EVC id
  L7_uint16 vlan_id;                 // Vlan id
  L7_uint8  port;                    // Port
  L7_uint8  mac_address[6];          // MAC address
  L7_uint8  static_address;          // Is static address?
} __attribute__ ((packed)) L7_switch_mac_entry;
typedef struct L7_switch_mac_table {
  L7_int err_code;                      // unused in commands (only responses)
  L7_uint64 index;                      // page index
  L7_uint16 mac_table_total_entries;    // Total number of entries in MAC table
  L7_uint16 mac_table_msg_size;         // Number of entries in this message: up to 256
  L7_switch_mac_entry mac_table[256];       // MAC table
} __attribute__ ((packed)) L7_switch_mac_table;

// Add (static) entries to the MAC table
typedef struct L7_switch_mac_operation {
  L7_int err_code;                   // unused in commands (only responses)
  L7_uint16 flow_id;                 // EVC id
  L7_uint16 vlan_id;                 // Vlan id
  L7_uint8  port;                    // Port
  L7_uint8  mac_address[6];          // MAC address
  L7_uint8  static_address;          // Is static address?
} __attribute__ ((packed)) L7_switch_mac_operation;

/* Switch general configuration */
typedef struct {
  L7_int32  err_code;                   /* unused in commands (only in responses */
  L7_uint32 mask;                       /* Mask (32 bits for alignment purposes)
                                            0x01 - Aging Time  */
  L7_int32  aging_time;                 /* Forwarding Database Aging Interval
                                           (10-1000000 */
} __attribute__ ((packed)) L7_switch_config;

// Flow counters
// Statistics
typedef struct {
  L7_uint64 pktTotal;                   // Total packets (Unicast+Multicast+Broadcast)
  L7_uint64 pktUnicast;                 // Unicast packets
  L7_uint64 pktMulticast;               // Multicast packets
  L7_uint64 pktBroadcast;               // Broadcast packets
  L7_uint64 pktDropped;                 // Packets dropped
} st_ptin_flow_port_stats;
typedef struct {
  st_ptin_flow_port_stats pon_rx;       // Packets received at pon interfaces
  st_ptin_flow_port_stats eth_rx;       // PAckets received at eth interfaces
} st_ptin_flow_stats;

typedef struct {
  L7_int              err_code;         // unused in commands (only responses)
  L7_uint64           index;            // EVC id
  L7_uint8            flow_type;        // Flow type
  union {
    L7_uint32 client_vlan;              // Client vlan: if null, is not applicable
    L7_uint32 channel_ip;               // IP channel: if null, is not applicable
  } c_id;                               // client or channel id, if is a UC or MC respectively
  st_ptin_flow_stats  stats;            // Statistics structure
  L7_uint8 countersExist;               // TRUE, if these counters were created
} st_ptin_flow_counters;

typedef struct {
  L7_uint16 vid_value;
  L7_uint16 vid_mask;
} L7_vid;

typedef struct {
  L7_uint32 port_bmp;
  L7_uint8  port_output;
  L7_vid    stag;
  L7_vid    ctag;
  L7_uint32 cir;
  L7_uint32 cbs;
  L7_uint32 eir;
  L7_uint32 ebs;
} L7_bw_profile;

#define L7_VLAN_XLATE_OPER_NONE              0
#define L7_VLAN_XLATE_OPER_GET_INGRESS_ST    1
#define L7_VLAN_XLATE_OPER_GET_INGRESS_DT    2
#define L7_VLAN_XLATE_OPER_GET_EGRESS_ST     3
#define L7_VLAN_XLATE_OPER_ADD_INGRESS_ST    4
#define L7_VLAN_XLATE_OPER_ADD_INGRESS_DT    5
#define L7_VLAN_XLATE_OPER_ADD_EGRESS_ST     6
#define L7_VLAN_XLATE_OPER_DEL_INGRESS_ST    7
#define L7_VLAN_XLATE_OPER_DEL_INGRESS_DT    8
#define L7_VLAN_XLATE_OPER_DEL_EGRESS_ST     9
#define L7_VLAN_XLATE_OPER_DELALL_INGRESS_ST 10
#define L7_VLAN_XLATE_OPER_DELALL_INGRESS_DT 11
#define L7_VLAN_XLATE_OPER_DELALL_EGRESS_ST  12
#define L7_VLAN_XLATE_OPER_DELALL_INGRESS    13
#define L7_VLAN_XLATE_OPER_DELALL_EGRESS     14
#define L7_VLAN_XLATE_OPER_DELETE_ALL        15

#define L7_FPENTRY_OPER_NONE         0
#define L7_FPENTRY_OPER_READ         1
#define L7_FPENTRY_OPER_CREATE       2
#define L7_FPENTRY_OPER_DESTROY      3
#define L7_FPENTRY_OPER_COUNTERS     4

typedef struct _L7_vlan_translation
{
  L7_int     operation;
  L7_int     port;
  L7_uint16  vlan_old;
  L7_uint16 *vlan_new;
} L7_vlan_translation;

typedef struct  {
  L7_uint8  inUse;        /* Is this entry in use? */
  L7_int    entry_index;  /* Entry index */
  L7_int    fpentry_id;   /* FFP entry Id */
  L7_uint32 port_bmp;     /* Ports bitmap */
  L7_uint8  port_output;  /* Output port */
  L7_vid    stag;         /* Internal VlanId */
  L7_vid    ctag;         /* Client Vlan */
  L7_uint16 policer_id;   /* Policer Id */
  L7_uint8  counter;
} L7_fpentry_data;

typedef struct _L7_fpentry_oper
{
  L7_int                        operation;
  L7_HWEth_VlanStatistics_Block *counters;
  L7_bw_profile                 *profile;
  L7_fpentry_data               *fpentry_ptr;
} L7_fpentry_oper;

typedef struct _L7_ptin_vlan_defs
{
  L7_int create_vlan_1;
  L7_int include_cpu_intf;
} L7_vlan_defs;


// ====== FLOW COUNTERS ==============

// Universal operations
typedef enum ptin_operation
{
  PTIN_OPER_NONE=0,
  PTIN_OPER_GET,
  PTIN_OPER_ADD,
  PTIN_OPER_REMOVE
} enum_ptin_operation;

// Flow Types
typedef enum ptin_flow_type {
  PTIN_FLOW_TYPE_UNICAST=0,
  PTIN_FLOW_TYPE_BROADCAST,
  PTIN_FLOW_TYPE_MULTICAST,
  PTIN_FLOW_TYPE_DROPPED,
  PTIN_FLOW_TYPE_MAX,
  PTIN_FLOW_TYPE_NONE
} enum_ptin_flow_type;

// Operations
typedef enum {
  PTIN_ADD_FLOW_COUNTERS=0,
  PTIN_REMOVE_FLOW_COUNTERS,
  PTIN_READ_FLOW_ABS_COUNTERS,
  PTIN_READ_FLOW_DIFF_COUNTERS
} enum_ptin_fcounters_oper;

// List of ports and vlans
typedef struct {
  L7_uint32 port_bmp;
  L7_uint16 vlan;
} st_ptin_port_vlan;

// Final structure
typedef struct {
  enum_ptin_fcounters_oper    oper;
  L7_uint16                   flow_id;
  enum_ptin_flow_type         flow_type;
  st_ptin_port_vlan           pon_ports;
  st_ptin_port_vlan           eth_ports;
  L7_uint32                   client_channel;
  st_ptin_flow_stats          stats;
} st_ptin_fcounters_oper;

// General purpose structure to pass arguments
typedef struct {
  L7_uint32  admin;
  L7_uint32  index;
  L7_uint32  vlanId;
  L7_uint32  srcPorts_bmp;
  DAPI_USP_t dstPort;
} st_ptin_args;

// ===========================================

// HW Resources
typedef struct
{
  L7_uint16 ffp_available_rules[8];
  struct {
    L7_uint16 ing_stag;
    L7_uint16 ing_dtag;
    L7_uint16 egr_stag;
  } vlanXlate_available_entries;
  struct {
    L7_uint16 igmp;
    L7_uint16 dhcp;
    L7_uint16 bcastLim;
  } vlans_available;
  L7_uint16 flowCounters_available_entries;
} st_ptin_hw_resources;

// HW Resources
typedef struct
{
  L7_int               err_code;
  st_ptin_hw_resources resources;
} st_msg_ptin_hw_resources;


extern L7_RC_t dtlPtinInit(void);

extern L7_RC_t dtlPtinPhySet(L7_uint32 intIfNum, L7_uint8 tx_disable);

extern L7_RC_t dtlPtinHwResources(st_ptin_hw_resources *resources);

extern L7_RC_t dtlPtinVlanDefs( L7_vlan_defs *data );

extern L7_RC_t dtlPtinVlanXlate( L7_vlan_translation *data );

extern L7_RC_t dtlPtinFPentry( L7_fpentry_oper *data );

extern L7_RC_t dtlPtinFlowCounters( st_ptin_fcounters_oper *data );

extern L7_RC_t dtlPtinBcastPktLimit( L7_uint32 admin, L7_uint32 intIfNum, L7_uint32 vlanId );

extern L7_RC_t dtlPtinBitStreamUpFwd(L7_uint32 intIfNum, L7_uint32 admin, L7_uint16 index, L7_uint32 vlanId);

extern L7_RC_t dtlPtinBitStreamLagRecheck(L7_uint32 intIfNum, L7_uint32 admin);

extern L7_RC_t dtlPtinGetBoardState(L7_HwCardInfo *data);

extern L7_RC_t dtlPtinGetCounters(L7_HWEthRFC2819_PortStatistics *data);

extern L7_RC_t dtlPtinClearCounters(L7_HWEthRFC2819_PortStatistics *data);

extern L7_RC_t dtlPtinGetCountersState(L7_HWEth_Statistics_State *data);

#endif
