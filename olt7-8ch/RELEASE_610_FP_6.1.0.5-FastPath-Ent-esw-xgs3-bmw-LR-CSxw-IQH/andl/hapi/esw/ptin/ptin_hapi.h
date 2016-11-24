#ifndef _PTIN_HAPI__H
#define _PTIN_HAPI__H

#define SYSTEM_N_GPON_PORTS 8
#define SYSTEM_N_PORTS  18

#include <sal/types.h>
#include "dapi.h"
#include "addrmap.h"

// ====== FLOW COUNTERS ==============

// Maximum number of flow counters
#define FLOW_COUNTERS_MAX   16

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
  PTIN_FLOW_TYPE_NONE,
} enum_ptin_flow_type;

// Flow directions
typedef enum ptin_flow_direction {
  PTIN_PORTS_PON_RX=0,
  PTIN_PORTS_ETH_RX,
  PTIN_PORTS_MAX
} enum_ptin_flow_direction;

// Operations
typedef enum {
  PTIN_ADD_FLOW_COUNTERS=0,
  PTIN_REMOVE_FLOW_COUNTERS,
  PTIN_READ_FLOW_ABS_COUNTERS,
  PTIN_READ_FLOW_DIFF_COUNTERS
} enum_ptin_fcounters_oper;

// Statistics
typedef struct {
  uint64 pktTotal;
  uint64 pktUnicast;
  uint64 pktMulticast;
  uint64 pktBroadcast;
  uint64 pktDropped;
} st_ptin_flow_port_stats;
typedef struct {
  st_ptin_flow_port_stats pon_rx;
  st_ptin_flow_port_stats eth_rx;
} st_ptin_flow_stats;

// List of ports and vlans
typedef struct {
  uint32 port_bmp;
  uint16 vlan;
} st_ptin_port_vlan;

// Final structure
typedef struct {
  enum_ptin_fcounters_oper    oper;
  uint16                      flow_id;
  enum_ptin_flow_type         flow_type;
  st_ptin_port_vlan           pon_ports;
  st_ptin_port_vlan           eth_ports;
  uint32                      client_channel;
  st_ptin_flow_stats          stats;
} st_ptin_fcounters_oper;

// Broadcast traffic limiter structure
typedef struct {
  enum_ptin_operation oper;
  uint16              flow_id;
  st_ptin_port_vlan   pon_ports;
  st_ptin_port_vlan   eth_ports;
} st_ptin_bcastpkt_limit_oper;

// General purpose structure to pass arguments
typedef struct {
  uint32 admin;
  uint32 index;
  uint32 vlanId;
  uint32 srcPorts_bmp;
  DAPI_USP_t dstPort;
} st_ptin_args;

// Flow counters structure
typedef struct {
  uint16              flow_id;
  enum_ptin_flow_type flow_type;
  uint32              client_channel;
  BROAD_POLICY_t      policy_id; //[PTIN_FLOW_TYPE_MAX][PTIN_PORTS_MAX];
  st_ptin_flow_stats  stats_abs;
  st_ptin_flow_stats  stats_diff;
} st_ptin_fcounters;

// List of flow counters
extern st_ptin_fcounters ptin_fcounters[FLOW_COUNTERS_MAX];

// ===========================================

//#define PHY_MAX_MAXFRAME              9608
//
//#define PHY_PORT_MEDIA_ELECTRIC       0
//#define PHY_PORT_MEDIA_OPTICAL        1
//#define PHY_PORT_MEDIA_INTERNAL       2
//
//#define PHY_PORT_AUTONEG              0
//#define PHY_PORT_10_MBPS              1
//#define PHY_PORT_100_MBPS             2
//#define PHY_PORT_1000_MBPS            3
//#define PHY_PORT_2500_MBPS            4
//#define PHY_PORT_10000_MBPS           5
//
////T_EVC_CEVID_MAP
//typedef struct {
//  uint8    type;       // enum{0 - root, 1 - leaf, -1 - unused}
//  uint16   ceVid;      // gamma[1, .., 4094]
//} __attribute__ ((packed)) st_HwEthernetMef10CeVidMap;
//

typedef struct {
  uint16 vid_value;
  uint16 vid_mask;
} st_vid;

typedef struct {
  uint32 port_bmp;
  uint8  port_output;
  st_vid stag;
  st_vid ctag;
  uint32 cir;
  uint32 cbs;
  uint32 eir;
  uint32 ebs;
} st_bw_profile;


//#define MEF10_EVC_UNITYPE_ROOT    0
//#define MEF10_EVC_UNITYPE_LEAF    1
//#define MEF10_EVC_UNITYPE_UNUSED ((uint8) -1)
//
//// Definição de um EVC
//typedef struct {
//  int err_code;                 //unused in commands (only responses)
//  uint64 index;                 //index: 0..N_MACs-1
//  uint8                         type;                      // { 0 - p2p, 1 - mp2mp, 2 - rooted mp, 3 - remover }
//  uint8                         admin;                     // { 0 - criar, 1 - modificar, 2 - Flush }
//  uint8                         bundling;                  // false:true(0:1)
//  uint16                        outerTag;                  // gamma [1, .., 4094]
//  uint8                         ceVidBitmap[1 << (12-3)];  // mapa de bits para vlans, o acesso a vlan x e feito por ((bitmap[x/8] >> x%8) & 0x01)
//  st_HwEthernetMef10CeVidMap    uni[SYSTEM_N_PORTS];       // mapa de pares portos/vlan (vlan apenas interessa quando nao existe bundling)
//} __attribute__ ((packed)) MSG_HwEthernetMef10EvcBundling;
//
//// Remoção de um EVC
//typedef struct {
//  int err_code;               //unused in commands (only responses)
//  uint64 index;                  //index: 0..N_MACs-1
//} __attribute__ ((packed)) MSG_HwEthernetMef10p1EvcRemove;
//
//// Definição de um perfil de largura de banda
//typedef struct {
//  int err_code;               //unused in commands (only responses)
//  uint64 index;               //index: bits 63-48:evc_id; 47-32:c_vid; 31-16:s_vid; 15-8:CoS; 7:rx1_tx0; 6:no_profile; 5-0:port;
//  uint8                       colourMode;
//  uint8                       couplingFlag;
//  uint64                      CIR;
//  uint64                      CBS;
//  uint64                      EIR;
//  uint64                      EBS;
//} __attribute__ ((packed)) MSG_HwEthernetProfile;
//
//
//// Switch Port PHY configurations
//typedef struct _st_HWEthPhyConf {
//  uint8   Port;               // 0-7:Pon; 8-15:1/2.5G Eth; 16-17:10G Eth
//  uint16  Mask;
//  uint8   Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 ? 1000;  4 ? 2500; 5 - 10000
//  uint8   Media;              // 0x0002   0 - electric; 1 - optic;   2 - Serdes
//  uint8   Duplex;             // 0x0004   0 - Half;     1 - Full
//  uint8   LoopBack;           // 0x0008   0 - s/ loop;  1 - Far End; 2 - swap loop; 3 - local oam loop; 4 - remote oam loop
//  uint8   FlowControl;        // 0x0010   0 - Off;      1 - Tx;      2 - Rx;        3 - Both
//  uint8   PortEnable;         // 0x0020   0 - disabled; 1 - enabled
//  uint16  MaxFrame;           // 0x0040
//  uint8   VlanAwareness;      // 0x0080   0 - unaware;  1 - aware
//  uint8   MacLearning;        // 0x0100   0 - don't learn; 1 - learn
//  uint8   AutoMDI;            // 0x0200   0 - disable;     1 - enable
//} __attribute__ ((packed)) st_HWEthPhyConf; // st_MC_Man_PhyConf
//
//
//typedef struct _st_HWEthPhyState {
//  uint8  Port;
//  uint16 Mask;
//  uint8  Speed;              // 0x0001  0 - AutoNeg;  1 - 10;    2 - 100; 3 - 1000
//  uint8  Duplex;             // 0x0002  0 - Half;     1 - Full
//  uint8  Collisions;         // 0x0004  0 - OK;   1 - NOK
//  uint8  RxActivity;         // 0x0008  0 - Sem;  1 - Com
//  uint8  TxActivity;         // 0x0010  0 - Sem;  1 - Com
//  uint8  LinkUp;             // 0x0020  0 - Down; 1 - Up
//  uint8  AutoNegComplete;    // 0x0040  0 - incompleted; 1 - completed
//  uint8  TxFault;            // 0x0080  0 - ok;   1 - Tx_fault
//  uint8  RemoteFault;        // 0x0100  0 - ok;   1 - remote_fault recebido
//  uint8  LOS;                // 0x0200  0 - s/ LOS; 1 - em LOS
//  uint8  Media;              // 0x0400  0 - electric; 1 - optic
//  uint8  MTU_mismatch;       // 0x0800  0 - MTU suported; 1 - MTU not suported
//  uint16 Suported_MaxFrame;// 0x1000  1518 to 9600
//} __attribute__ ((packed)) st_HWEthPhyState;

/*
Bit 8: Rx Dropped packets
Bit 7: Rx Undersized packets 
Bit 6: Rx Oversized packets 
Bit 5: Rx Fragments 
Bit 4: Rx Jabbers 
Bit 3: CRC errors 
Bit 2: Tx Collisions 
Bit 1: Tx activity 
Bit 0: Rx activity
*/
#define BIT_RX_DROPPACKETS      8
#define BIT_RX_UNDERSIZEPACKETS 7
#define BIT_RX_OVERSIZEPACKETS  6
#define BIT_RX_FRAGMENTS        5
#define BIT_RX_JABBERS          4
#define BIT_RX_CRC_ERRORS       3
#define BIT_TX_COLLISIONS       2
#define BIT_TX_ACTIVITY         1
#define BIT_RX_ACTIVITY         0

typedef struct _st_HWEth_Statistics_State
{
  uint32 status_mask;
  uint32 status_value[SYSTEM_N_PORTS];
} st_HWEth_Statistics_State;

typedef struct _st_HWEth_VlanStatistics_Block
{
  L7_uint64 etherNotRedPackets;
  L7_uint64 etherRedPackets;
} __attribute__ ((packed)) st_HWEth_VlanStatistics_Block;

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
} __attribute__ ((packed)) st_HWEthRFC2819_Statistics_Block; //144

typedef struct _st_HWEthRFC2819_PortStatistics
{
  uint8   Port;
  uint8   Mask;
  uint32  RxMask;
  uint32  TxMask;
  st_HWEthRFC2819_Statistics_Block Rx;      // 0x01
  st_HWEthRFC2819_Statistics_Block Tx;      // 0x02
} __attribute__ ((packed)) st_HWEthRFC2819_PortStatistics; //296



#define VLAN_XLATE_OPER_NONE              0
#define VLAN_XLATE_OPER_GET_INGRESS_ST    1
#define VLAN_XLATE_OPER_GET_INGRESS_DT    2
#define VLAN_XLATE_OPER_GET_EGRESS_ST     3
#define VLAN_XLATE_OPER_ADD_INGRESS_ST    4
#define VLAN_XLATE_OPER_ADD_INGRESS_DT    5
#define VLAN_XLATE_OPER_ADD_EGRESS_ST     6
#define VLAN_XLATE_OPER_DEL_INGRESS_ST    7
#define VLAN_XLATE_OPER_DEL_INGRESS_DT    8
#define VLAN_XLATE_OPER_DEL_EGRESS_ST     9
#define VLAN_XLATE_OPER_DELALL_INGRESS_ST 10
#define VLAN_XLATE_OPER_DELALL_INGRESS_DT 11
#define VLAN_XLATE_OPER_DELALL_EGRESS_ST  12
#define VLAN_XLATE_OPER_DELALL_INGRESS    13
#define VLAN_XLATE_OPER_DELALL_EGRESS     14
#define VLAN_XLATE_OPER_DELETE_ALL        15

#define FPENTRY_OPER_NONE         0
#define FPENTRY_OPER_READ         1
#define FPENTRY_OPER_CREATE       2
#define FPENTRY_OPER_DESTROY      3
#define FPENTRY_OPER_COUNTERS     4

typedef struct _st_vlan_translation
{
  int     operation;
  int     port;
  uint16  vlan_old;
  uint16 *vlan_new;
} st_vlan_translation;

typedef struct  {
  uint8  inUse;         // This entry is in use
  int    entry_index;   // Entry index: in initialized as sequential and must not be rewritten
  int    fpentry_id;    // FP rule id
  uint32 port_bmp;      // Source ports bitmap
  uint8  port_output;   // Port redirection
  st_vid stag;          // SVlan
  st_vid ctag;          // CVlan
  uint16 policer_id;    // Profile policer id
  uint8  counter;       // Number of creations
} struct_fpentry_data;

typedef struct _st_fpentry_oper
{
  int                           operation;
  st_HWEth_VlanStatistics_Block *counters;
  st_bw_profile                 *profile;
  struct_fpentry_data           *fpentry_ptr;
} st_fpentry_oper;

typedef struct _st_ptin_vlan_defs
{
  int create_vlan_1;
  int include_cpu_intf;
} st_ptin_vlan_defs;
extern st_ptin_vlan_defs ptin_vlan_defs;


// Resources structures
typedef enum {
  ptin_vlan_xlate_ingress_stag = 0,
  ptin_vlan_xlate_ingress_dtag,
  ptin_vlan_xlate_egress_stag
} enum_ptin_vlan_xlate_entrytype;

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
// End of resources structures

/* FPGA map registers */
#define FPGA_ID1_REG        0x0004
#define FPGA_ID0_REG        0x0005
#define FPGA_TXDISABLE_REG  0xF061
#define FPGA_EXTPHY_RESET   0xF080

/* FPGA map pointer */
typedef struct {
    uint8 registo[0xFFFF];
} __attribute__ ((packed)) st_fpga_map;
extern volatile st_fpga_map *fpga_map;


extern int hapi_ptin_get_available_rules(void);

extern int hapi_ptin_get_available_vlanXlate_entries(enum_ptin_vlan_xlate_entrytype entryType);

extern L7_RC_t hapi_ptin_get_hw_resources(st_ptin_hw_resources *resources);


extern L7_RC_t hapi_ptin_vlan_defs(int create_vlan_1, int include_cpu_intf);

extern L7_RC_t hapi_ptin_vlan_translate(int operation, int port, uint16 vlan_old, uint16 *vlan_new);

extern L7_RC_t hapi_ptin_fpentry_read(st_bw_profile *profile, struct_fpentry_data *fpentry_ptr);

extern L7_RC_t hapi_ptin_fpentry_counters(st_HWEth_VlanStatistics_Block *counters, struct_fpentry_data *fpentry_ptr);

extern L7_RC_t hapi_ptin_fpentry_create(st_bw_profile *profile, struct_fpentry_data **fpentry_ptr);

extern L7_RC_t hapi_ptin_fpentry_destroy(struct_fpentry_data *fpentry_id);

extern L7_RC_t hapi_ptin_flow_init(void);

extern int hapi_ptin_get_realPhysicalPort(int port);

//extern L7_RC_t hapi_ptin_flow_create(uint16 flow_id, st_HwEthernetMef10CeVidMap *port, DAPI_USP_t usp[SYSTEM_N_PORTS]);
//
//extern L7_RC_t hapi_ptin_flow_kill(uint16 flow_id);
//
//extern L7_RC_t hapi_ptin_flow_reset(void);
//
//extern L7_RC_t hapi_ptin_flow_set_profile(uint16 flow_id, uint8 port, st_vid *stag, st_vid *ctag, st_bw_profile *bw_profile);
//
//extern L7_RC_t hapi_ptin_flow_consult(uint16 flow_id);
//

extern L7_RC_t hapi_ptin_phy_config_init(void);

extern L7_RC_t hapi_ptin_phy_set(uint8 port, uint8 tx_disable);

//extern L7_RC_t hapi_ptin_phy_config_set(uint8 port, uint8 enable, uint8 speed, uint8 autoneg, uint8 full_duplex, uint16 frame_max, uint8 lb);
//
//extern L7_RC_t hapi_ptin_phy_config_get(uint8 port, uint8 *enable, uint8 *speed, uint8 *autoneg, uint8 *full_duplex, uint16 *frame_max, uint8 *lb);
//
//extern L7_RC_t hapi_ptin_phy_state_get(uint8 port, uint8 *linkup, uint8 *autoneg_complete);

extern L7_RC_t hapi_ptin_readCounters(uint8 port, st_HWEthRFC2819_Statistics_Block *Rx, st_HWEthRFC2819_Statistics_Block *Tx);

extern L7_RC_t hapi_ptin_clearCounters(uint8 port);


// Flow counters

extern uint16 hapiBroadPtinFlowCounters_numberOfAvailableEntries(void);

extern L7_RC_t hapi_ptin_flow_counters_add(uint16 flow_id, enum_ptin_flow_type flow_type,
                                       uint32 pon_port_bmp, uint32 eth_port_bmp, uint16 pon_vlan, uint16 eth_vlan,
                                       uint32 client_channel);

extern L7_RC_t hapi_ptin_flow_counters_remove(uint16 flow_id, uint32 client_channel);

extern L7_RC_t hapi_ptin_flow_counters_update(void);

extern L7_RC_t hapi_ptin_flow_absCounters_read(uint16 flow_id, uint32 client_channel, st_ptin_flow_stats *stats);

extern L7_RC_t hapi_ptin_flow_diffCounters_read(uint16 flow_id, uint32 client_channel, st_ptin_flow_stats *stats);

// Add broadcast rate limiter to a UNICAST flow
// Inputs:
//  flow_id       >> EVC id
//  pon_port_bmp  >> pon ports bitmap
//  eth_port_bmp  >> eth ports bitmap
//  pon_vlan      >> pon vlan
//  eth_vlan      >> eth vlan
// Returns:
//  0 = Success
//  other = failure
//
extern L7_RC_t hapi_ptin_flow_bcastpack_limit(DAPI_USP_t *usp, uint8 enable, uint16 vlanId, DAPI_t *dapi_g);

extern L7_RC_t hapi_ptin_flow_bitstream_upFwd(DAPI_USP_t *usp, uint8 admin, uint16 vlanId, DAPI_t *dapi_g);

extern L7_RC_t hapi_ptin_flow_bitstream_lagRecheck(DAPI_USP_t *usp, uint8 admin, DAPI_t *dapi_g);

extern uint16 hapiBroadPtinBCastLim_numberOfAvailableVlans(void);

#endif
