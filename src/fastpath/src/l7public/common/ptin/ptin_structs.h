#ifndef _PTIN_STRUCTS_H
#define _PTIN_STRUCTS_H

#include "ptin_globaldefs.h"
#include "commdefs.h"
#include "dapi.h"

/* Used for packet processing timing measure */
typedef struct
{
  L7_BOOL   first_pkt;
  L7_uint32 time_start;
  L7_uint32 time_end;
  L7_uint32 pktOk_counter;
  L7_uint32 pktEr_counter;
  L7_uint32 pkt_cpu_counter;
  L7_uint32 pkt_cpu2_counter;
  L7_uint32 pkt_dtl_counter;
  L7_uint32 pkt_dtl2_counter;
  L7_uint32 pkt_intercept_counter;
} ptin_debug_pktTimer_t;


/* General structs */
/* PTin IP Address */
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

/* PTin Interface */
typedef struct {
  L7_uint8  intf_type;                          /* Interface type: { 0-Physical, 1-Logical (LAG) } */
  L7_uint8  intf_id;                            /* Interface Id# (phy ports / LAGs) */
} ptin_intf_t;

/* Slo modes */
typedef struct
{
  DAPI_CMD_GET_SET_t  operation;
  L7_uint32 slotMode[PTIN_SYS_SLOTS_MAX];       /* List of slot modes */
} ptin_slotmode_t;


/* Switch Port PHY configurations *********************************************/
#define PTIN_PHYCONF_MASK_SPEED         0x0001
#define PTIN_PHYCONF_MASK_MEDIA         0x0002
#define PTIN_PHYCONF_MASK_DUPLEX        0x0004
#define PTIN_PHYCONF_MASK_LOOPBACK      0x0008
#define PTIN_PHYCONF_MASK_PORTEN        0x0020
#define PTIN_PHYCONF_MASK_MAXFRAME      0x0040

typedef enum
{
  HW_ETHERNET_STATUS_MASK_SPEED100_BIT   = 0x0001,                                 // aplicavel apenas se nao for SPEED1000
  HW_ETHERNET_STATUS_MASK_TX_BIT         = 0x0002,                                 //
  HW_ETHERNET_STATUS_MASK_RX_BIT         = 0x0004,                                 //
  HW_ETHERNET_STATUS_MASK_COLLISION_BIT  = 0x0008,                                 //
  HW_ETHERNET_STATUS_MASK_LINK_BIT       = 0x0010,                                 // obtido via (64/66b)
  HW_ETHERNET_STATUS_MASK_AUTONEG_BIT    = 0x0020,                                 //
  HW_ETHERNET_STATUS_MASK_FULLDUPLEX_BIT = 0x0040,                                 //
  HW_ETHERNET_STATUS_MASK_SPEED1000_BIT  = 0x0080,                                 //
  HW_ETHERNET_STATUS_MASK_MEDIAX_BIT     = 0x0100,                                 //
  HW_ETHERNET_STATUS_MASK_HIGH_BER_BIT   = 0x0200,                                 // obtido via (64/66b)
  HW_ETHERNET_STATUS_MASK_SPEED10G_BIT   = 0x0400,                                 // 
  HW_ETHERNET_STATUS_MASK_LOS            = 0x8000,                                 // 

  HW_ETHERNET_STATUS_MASK_PWRDWN_LOOP    = (1<<31),
} HW_ETHERNET_STATUS_MASK;


typedef struct {
  L7_uint8   Port;              // 0-7:Pon; 8-15:1/2.5G Eth; 16-17:10G Eth
  L7_uint16  Mask;
  L7_uint8   Speed;             // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 ? 1000;  4 ? 2500; 5 - 10000
  L7_uint8   Media;             // 0x0002   0 - electric; 1 - optic;   2 - Serdes
  L7_uint8   Duplex;            // 0x0004   0 - Half;     1 - Full
  L7_uint8   LoopBack;          // 0x0008   0 - s/ loop;  1 - Far End; 2 - swap loop; 3 - local oam loop; 4 - remote oam loop
  L7_uint8   PortEnable;        // 0x0020   0 - disabled; 1 - enabled
  L7_uint16  MaxFrame;          // 0x0040
} ptin_HWEthPhyConf_t;

/* Switch Port PHY state */
#define PTIN_PHYSTATE_MASK_SPEED        0x0001
#define PTIN_PHYSTATE_MASK_DUPLEX       0x0002
#define PTIN_PHYSTATE_MASK_LINKUP       0x0020
#define PTIN_PHYSTATE_MASK_AUTONEG      0x0040

typedef struct {
  L7_uint8  Port;
  L7_uint16 Mask;
  L7_uint8  Speed;              // 0x0001   0 - AutoNeg;  1 - 10;      2 - 100;       3 - 1000;  4 - 2500; 5 - 10000
  L7_uint8  Duplex;             // 0x0002   0 - Half;     1 - Full
  L7_uint8  LinkUp;             // 0x0020   0 - Down;     1 - Up
  L7_uint8  AutoNegComplete;    // 0x0040   0 - incompleted; 1 - completed
} ptin_HWEthPhyState_t;

/* MEF extension definitions for a specific port */
#define PTIN_HWPORTEXT_MASK_DEFVID                        0x0001
#define PTIN_HWPORTEXT_MASK_DEFPRIO                       0x0002
#define PTIN_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES        0x0004
#define PTIN_HWPORTEXT_MASK_INGRESS_FILTER                0x0008
#define PTIN_HWPORTEXT_MASK_RESTRICTED_VLAN_REG           0x0010
#define PTIN_HWPORTEXT_MASK_VLAN_AWARE                    0x0020
#define PTIN_HWPORTEXT_MASK_TYPE                          0x0040
#define PTIN_HWPORTEXT_MASK_DOUBLETAG                     0x0100
#define PTIN_HWPORTEXT_MASK_OUTER_TPID                    0x0200
#define PTIN_HWPORTEXT_MASK_INNER_TPID                    0x0400
#define PTIN_HWPORTEXT_MASK_EGRESS_TYPE                   0x0800
#define PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE               0x1000
#define PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE   0x2000
#define PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO     0x4000
#define PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO 0x8000

#define PTIN_PORT_EGRESS_TYPE_PROMISCUOUS   0
#define PTIN_PORT_EGRESS_TYPE_COMMUNITY     1
#define PTIN_PORT_EGRESS_TYPE_ISOLATED      2

typedef struct
{
  DAPI_CMD_GET_SET_t  operation;
  L7_uint16 Mask;
  L7_uint16 defVid;                         // 0x0001
  L7_uint8  defPrio;                        // 0x0002
  L7_uint8  acceptable_frame_types;         // 0x0004
  L7_uint8  ingress_filter;                 // 0x0008
  L7_uint8  restricted_vlan_reg;            // 0x0010
  L7_uint8  vlan_aware;                     // 0x0020
  L7_uint8  type;                           // 0x0040   UNI=1, NNI=2
  L7_uint8  doubletag;                      // 0x0100
  L7_uint16 outer_tpid;                     // 0x0200
  L7_uint16 inner_tpid;                     // 0x0400
  L7_uint8  egress_type;                    // 0x0800   PROMISCUOUS=0, COMMUNITY=1, ISOLATED=2
  L7_uint8  macLearn_enable;                // 0x1000
  L7_uint8  macLearn_stationMove_enable;    // 0x2000
  L7_uint8  macLearn_stationMove_prio;      // 0x4000
  L7_uint8  macLearn_stationMove_samePrio;  // 0x8000
} ptin_HWPortExt_t;

/* MAC address attributions */
#define PTIN_HWPORTMAC_MASK_MACADDR         0x0001
typedef struct
{
  DAPI_CMD_GET_SET_t operation;
  L7_uint16 Mask;
  L7_uint8  macAddr[L7_MAC_ADDR_LEN];       // 0x0001
} ptin_HWPortMac_t;

/* Counters Structures ********************************************************/
#define PTIN_RFC2819_MASK_DROPEVENTS            0x00000001
#define PTIN_RFC2819_MASK_OCTETS                0x00000002
#define PTIN_RFC2819_MASK_PKTS                  0x00000004
#define PTIN_RFC2819_MASK_BROADCASTPKTS         0x00000008
#define PTIN_RFC2819_MASK_MULTICASTPKTS         0x00000010
#define PTIN_RFC2819_MASK_CRCALIGNERRORS        0x00000020
#define PTIN_RFC2819_MASK_UNDERSIZEPKTS         0x00000040
#define PTIN_RFC2819_MASK_OVERSIZEPKTS          0x00000080
#define PTIN_RFC2819_MASK_FRAGMENTS             0x00000100
#define PTIN_RFC2819_MASK_JABBERS               0x00000200
#define PTIN_RFC2819_MASK_COLLISIONS            0x00000400
#define PTIN_RFC2819_MASK_PKTS64OCTETS          0x00000800
#define PTIN_RFC2819_MASK_PKTS65TO127OCTETS     0x00001000
#define PTIN_RFC2819_MASK_PKTS128TO255OCTETS    0x00002000
#define PTIN_RFC2819_MASK_PKTS256TO511OCTETS    0x00004000
#define PTIN_RFC2819_MASK_PKTS512TO1023OCTETS   0x00008000
#define PTIN_RFC2819_MASK_PKTS1024TO1518OCTETS  0x00010000
#define PTIN_RFC2819_MASK_PKTS1519TOMAXOCTETS   0x00020000
#define PTIN_RFC2819_MASK_THROUGHPUT            0x00040000

typedef struct {
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
} ptin_HWEthRFC2819_StatisticsBlock_t; //144

typedef struct {
  L7_uint8   Port;
  L7_uint8   Mask;
  L7_uint32  RxMask;
  L7_uint32  TxMask;
  ptin_HWEthRFC2819_StatisticsBlock_t Rx;       // 0x01
  ptin_HWEthRFC2819_StatisticsBlock_t Tx;       // 0x02
} ptin_HWEthRFC2819_PortStatistics_t; //296

/* Mask for ptin_HWEth_PortActivity_t */
#define PTIN_PORTACTIVITY_MASK_RX_ACTIVITY          0x0001
#define PTIN_PORTACTIVITY_MASK_TX_ACTIVITY          0x0002
#define PTIN_PORTACTIVITY_MASK_TX_COLLISIONS        0x0004
#define PTIN_PORTACTIVITY_MASK_RX_CRC_ERRORS        0x0008
#define PTIN_PORTACTIVITY_MASK_RX_JABBERS           0x0010
#define PTIN_PORTACTIVITY_MASK_RX_FRAGMENTS         0x0020
#define PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS   0x0040
#define PTIN_PORTACTIVITY_MASK_RX_UNDERSIZEPACKETS  0x0080
#define PTIN_PORTACTIVITY_MASK_RX_DROPPACKETS       0x0100

typedef struct
{
  L7_uint64 ports_mask;     /* Indicates which array indexes are valid */
  L7_uint64 activity_mask;  /* Masks bitmap contents:
                             *   0x0001: Rx activity
                             *   0x0002: Tx activity 
                             *   0x0004: Tx Collisions 
                             *   0x0008: CRC errors 
                             *   0x0010: Rx Jabbers 
                             *   0x0020: Rx Fragments 
                             *   0x0040: Rx Oversized packets 
                             *   0x0080: Rx Undersized packets 
                             *   0x0100: Rx Dropped packets */
  L7_uint32 activity_bmap[PTIN_SYSTEM_N_PORTS]; /* maps each phy port */
} ptin_HWEth_PortsActivity_t;


/* LAGs structs ***************************************************************/
typedef struct {
  L7_uint32 lagId;                // LAG nr. [0..PTIN_SYSTEM_N_LAGS[
  L7_uint8  admin;                // DISABLED=0 or ENABLED=1
  L7_uint8  stp_enable;           // DISABLED=0 or ENABLED=1
  L7_uint8  static_enable;        // DISABLED=0 or ENABLED=1
  L7_uint8  loadBalance_mode;     // FIRST=0, SA_VLAN=1, DA_VLAN=2, SDA_VLAN=3, SIP_SPORT=4, DIP_DPORT=5, SDIP_DPORT=6
  L7_uint64 members_pbmp64;       // Ports bitmap
} ptin_LACPLagConfig_t;

typedef struct {
  L7_uint32 lagId;                // LAG nr. [0..PTIN_SYSTEM_N_LAGS[
  L7_uint8  admin;                // DISABLED=0 or ENABLED=1
  L7_uint8  link_status;          // DOWN=0 or UP=1
  L7_uint8  port_channel_type;    // DYNAMIC=0 or STATIC=1
  L7_uint64 members_pbmp64;       // Ports bitmap
  L7_uint64 active_members_pbmp64;// Ports bitmap
} ptin_LACPLagStatus_t;

typedef struct {
  L7_uint32 port;                 // Port nr. [0..PTIN_SYSTEM_N_PORTS[
  L7_uint8  state_aggregation;
  L7_uint8  lacp_activity;
  L7_uint8  lacp_timeout;
} ptin_LACPAdminState_t;

typedef struct {
  L7_uint32 port;                 // Port nr. [0..PTIN_SYSTEM_N_PORTS[
  L7_uint32 LACPdus_rx;           // LACPdu's received
  L7_uint32 LACPdus_tx;           // LACPdu's transmitted
} ptin_LACPStats_t;


/* Xlate structs **************************************************************/

/* Vlan translation stage */
typedef enum
{
  PTIN_XLATE_STAGE_NONE = 0,
  PTIN_XLATE_STAGE_INGRESS,
  PTIN_XLATE_STAGE_EGRESS,
  PTIN_XLATE_STAGE_ALL
} ptin_vlanXlate_stage_enum;

/* Vlan translation operation */
typedef enum
{
  PTIN_XLATE_ACTION_NONE = 0,
  PTIN_XLATE_ACTION_ADD,
  PTIN_XLATE_ACTION_REPLACE,
  PTIN_XLATE_ACTION_DELETE
} ptin_vlanXlate_action_enum;

/* Structure used to configure translation entries */
typedef struct {
  DAPI_CMD_GET_SET_t oper;                  // Operation: DAPI_CMD_SET add, DAPI_CMD_CLEAR remove, and DAPI_CMD_GET reads.
  L7_uint32                   portgroup;    // Group of ports
  ptin_vlanXlate_stage_enum   stage;        // Translation stage
  ptin_vlanXlate_action_enum  outerAction;  // Action associated to outer vlan
  ptin_vlanXlate_action_enum  innerAction;  // Action associated to outer vlan
  L7_uint16 outerVlan;                      // Outer vlan id [1..4095]
  L7_uint16 innerVlan;                      // Inner vlan id [0..4095]: 0 means to not use this field
  L7_uint16 outerVlan_new;                  // New outer vlan [1..4095]
  L7_uint16 innerVlan_new;                  // New inner vlan [0..4095]: 0 means to not translate this field
} ptin_vlanXlate_t;


#define PTIN_XLATE_PORTGROUP_INTERFACE  0   // Use this class id to make reference to the physical port

/* Structure used to set the classIds (for egress translations) */
typedef struct
{
  DAPI_CMD_GET_SET_t oper;          // Operation: DAPI_CMD_SET set, and DAPI_CMD_GET reads.
  L7_int class_id;                  // Class id: positive value
} ptin_vlanXlate_classId_t;

/* Struct used ti configure vlan parameters via DTL */
typedef struct
{
  L7_uint16 vlanId;
  L7_BOOL cpu_include;
} ptin_vlan_mode_t;

/* Struct used to configure vlan mode via DTL */
typedef struct
{
  L7_uint16 vlanId;                 // Vlan which will be configured
  L7_BOOL   double_tag;             // Use double tag
    #define PTIN_BRIDGE_VLAN_MODE_MASK_NONE           0x00
  L7_uint8  mask;                   // Mask of configurations (see below)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_FWDVLAN        0x01
  L7_uint16 fwdVlanId;              // Forwrd vlan used for MAC larning (only applicable to mask=0x01)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_OTPID          0x02
  L7_uint16 outer_tpid;             // Outer TPID (only applicable to mask=0x02)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_ITPID          0x04
  L7_uint16 inner_tpid;             // Inner TPID (only applicable to mask=0x04)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_LEARN_EN       0x08
  L7_BOOL   learn_enable;           // Enable MAC learning to this vlan (only applicable to mask=0x08)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_CROSSCONN_EN   0x10
  L7_BOOL   cross_connects_enable;  // Enable cross-connections for this vlan (only applicable to mask=0x10)
    #define PTIN_BRIDGE_VLAN_MODE_MASK_MC_GROUP       0x20
  L7_int    multicast_group;        // Associate a multicast group
} ptin_bridge_vlan_mode_t;

/* Multicast management with vlans and ports */
typedef struct
{
  DAPI_CMD_GET_SET_t oper;    /* Operation */
  L7_int  vlanId;             /* Vlan Id (-1 to be applied on egress ports) */
  L7_int  multicast_group;    /* Multicast group id (-1 to be created) */
  L7_BOOL destroy_on_clear;   /* Destroy MC group, if oper is CLEAR */
} ptin_bridge_vlan_multicast_t;

/* Virtual ports management */
typedef struct
{
  DAPI_CMD_GET_SET_t oper;
  L7_int ext_ovid;
  L7_int ext_ivid;
  L7_int int_ovid;
  L7_int int_ivid;
  L7_int virtual_gport;
  L7_int multicast_group;
} ptin_vport_t;

/* Struct used to manipulate cross connects via DTL */
typedef struct
{
  DAPI_CMD_GET_SET_t oper;          // Operation: DAPI_CMD_SET add, DAPI_CMD_CLEAR remove, DAPI_CMD_CLEAR_ALL removes all, and DAPI_CMD_GET reads.
  DAPI_USP_t dstUsp;                // Destination port (source goes through direct usp)
  L7_uint16 outerVlanId;            // Outer vlan id to look for
  L7_uint16 innerVlanId;            // Inner vlan id to look for (0 to not be used)
} ptin_bridge_crossconnect_t;


/* EVCs structs ***************************************************************/
/* EVC details per interface */
#define PTIN_EVC_INTF_PHYSICAL      0
#define PTIN_EVC_INTF_LOGICAL       1
#define PTIN_EVC_INTF_ROOT          0
#define PTIN_EVC_INTF_LEAF          1

typedef struct {
  L7_uint8  intf_id;      // Interface Id# (phy ports / LAGs)
  L7_uint8  intf_type;    // Interface type: { 0-Physical, 1-Logical (LAG) }
  L7_uint8  mef_type;     // { 0 - root, 1 - leaf }
  L7_uint16 vid;          // Outer VLAN id [1..4094]
  L7_uint16 vid_inner;    // Inner vlan associated to this interface
} ptin_HwEthMef10Intf_t;

/* EVC config */
#define PTIN_EVC_MASK_BUNDLING          0x00000001
#define PTIN_EVC_MASK_ALL2ONE           0x00000002
#define PTIN_EVC_MASK_STACKED           0x00000004
#define PTIN_EVC_MASK_MACLEARNING       0x00000008
#define PTIN_EVC_MASK_CPU_TRAPPING      0x00000010
#define PTIN_EVC_MASK_MC_FLOOD_ALL      0x00000020    /* Added */
#define PTIN_EVC_MASK_DHCP_PROTOCOL     0x00000100
#define PTIN_EVC_MASK_IGMP_PROTOCOL     0x00000200
#define PTIN_EVC_MASK_PPPOE_PROTOCOL    0x00000400
#define PTIN_EVC_MASK_SERV_STD          0x00000000

#define PTIN_EVC_MASK_P2P               0x00010000
#define PTIN_EVC_MASK_QUATTRO           0x00020000
#define PTIN_EVC_MASK_TYPE              (PTIN_EVC_MASK_P2P | PTIN_EVC_MASK_QUATTRO)

/* This bit will tell EVC if should use several vlans to gurantee port isolation */
#define PTIN_EVC_MASK_ETREE             0x00100000

/* EVC type */
#define PTIN_EVC_TYPE_STD_P2MP          0x0
#define PTIN_EVC_TYPE_STD_P2P           0x1
#define PTIN_EVC_TYPE_QUATTRO_P2MP      0x2
#define PTIN_EVC_TYPE_QUATTRO_P2P       0x3

#define PTIN_EVC_MC_FLOOD_ALL       0
#define PTIN_EVC_MC_FLOOD_UNKNOWN   1
#define PTIN_EVC_MC_FLOOD_NONE      2

#define PTIN_EVC_PORT_FORWARDING    0
#define PTIN_EVC_PORT_BLOCKING      1


typedef struct {
  L7_uint32 index;        // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint32 flags;        // 0x0001 - Bundling      (not implemented)
                          // 0x0002 - All to One    (not implemented)
                          // 0x0004 - Stacked       (PTin custom field)
                          // 0x0008 - Mac Learning  (PTin custom field)
                          // 0x0100 - DHCP Protocol (PTin custom field)
  L7_uint8  type;         // (not used) { 0 - p2p, 1 - mp2mp, 2 - rooted mp }
  L7_uint8  evc_type;     // EVC type: STD_P2MP, STD_P2P, QUATTRO_P2MP, QUATTRO_P2P
  L7_uint8  mc_flood;     // MC flood type {0-All, 1-Unknown, 2-None} (PTin custom field)
  L7_uint8  ce_vid_bmp[(1<<12)/(sizeof(L7_uint8)*8)];   // VLANs mapping (ONLY for bundling) ((bmp[i/8] >> i%8) & 0x01)
  
  L7_uint16 n_clients;    // Number of attached clients

  L7_uint8  n_intf;       // Number of interfaces present on intf array
  ptin_HwEthMef10Intf_t intf[PTIN_SYSTEM_MAX_N_PORTS];

  /* NOTE: the size of the intf array is limited to either the maximum number of
   * physical or LAG interfaces because if one interface is being used as physical
   * port, it cannot be used in a LAG, and vice-versa. So it means that we will
   * have at most the max(phy,lag) interfaces combination in an EVC. */

} ptin_HwEthMef10Evc_t;

/* EVC Interface configuration */
typedef struct {
  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */
  L7_uint8   type;          /* 0:root; 1:leaf;*/
  L7_uint16  out_vlan;      /* Outer VLAN:
                             *  Root: S-VLAN
                             *  Leaf: S' (ONLY applicable to unstacked services)
                             *        (on unstacked services we allow a S->S'
                             *         xlate per leaf port) */
  L7_uint16  inner_vlan;    /* Inner vlan associated to interface */

  L7_uint16  int_vlan;      /* Internal VLAN:
                             *  stacked   - NOT APPLICABLE
                             *  unstacked - one internal VLAN per interface */
} ptin_evc_intfCfg_t;

/* EVC stacked bridge */
typedef struct {
  L7_uint32 index;        // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint16 inn_vlan;     // Client VLAN (inner tag)

  /* Client interface (root is already known by the EVC) */
  ptin_HwEthMef10Intf_t intf; // VID represents the new outer VLAN (Vs')
} ptin_HwEthEvcBridge_t;

/* EVC stacked bridge */
typedef struct {
  L7_uint32 evc_idx;      // EVC Id [1..PTIN_SYSTEM_N_EVCS]
  L7_uint32 flags;        // Protocol flags

  L7_uint16 int_ivid;     // C-VLAN tagged in the upstream flows (inside the switch)

  /* Client interface (root is already known by the EVC) */
  ptin_intf_t ptin_intf;  // PON interface
  L7_uint16   uni_ovid;   // GEM id
  L7_uint16   uni_ivid;   // UNI cvlan
} ptin_HwEthEvcFlow_t;

/* Client identification */

#define PTIN_CLIENT_MASK_FIELD_INTF       0x01
#define PTIN_CLIENT_MASK_FIELD_OUTERVLAN  0x02
#define PTIN_CLIENT_MASK_FIELD_INNERVLAN  0x04
#define PTIN_CLIENT_MASK_FIELD_IPADDR     0x08
#define PTIN_CLIENT_MASK_FIELD_MACADDR    0x10

typedef struct {
  L7_uint8    mask;                     /* Mask of fields to identify the client */
  ptin_intf_t ptin_intf;                /* [mask=0x01] PTin intf, which is attached */
  L7_uint16   outerVlan;                /* [mask=0x02] Outer Vlan  */
  L7_uint16   innerVlan;                /* [mask=0x04] Inner Vlan */
  L7_uint32   ipv4_addr;                /* [mask=0x08] IP address */
  L7_uchar8   macAddr[L7_MAC_ADDR_LEN]; /* [mask=0x10] Source MAC */
} ptin_client_id_t;

typedef struct {                    /* Mask values used here come from the variable 'mask' in the struct msg_AccessNodeCircuitId_t */
  L7_uint16   onuid;                /* [mask=0x0040] ONU ID */
  L7_uint8    slot;             	/* [mask=0x0080] Slot */
  L7_uint16   port;                 /* [mask=0x0100] Slot Port*/
  L7_uint16   q_vid;                /* [mask=0x0200] VLAN ID on U interface */
  L7_uint16   c_vid;                /* [mask=0x0400] C-VLAN on U interface */
} ptin_clientCircuitId_t;

/* Resources ******************************************************************/

#define PTIN_POLICY_MAX_GROUPS  16
#define PTIN_POLICY_MAX_STAGES  3

typedef struct
{
  L7_uint16 rules;
  L7_uint16 counters;
  L7_uint16 meters;
  L7_uint16 slice_width;
} st_ptin_group_param;

typedef struct
{
  L7_uint8 inUse;
  L7_int   group_id;
  st_ptin_group_param total;
  st_ptin_group_param free;
  st_ptin_group_param count;
} st_ptin_ffp_rules;

/* Policy Resources */
typedef struct
{
  st_ptin_ffp_rules cap[PTIN_POLICY_MAX_GROUPS][PTIN_POLICY_MAX_STAGES];
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
  L7_uint16 crossconnexs_available_entries;
  L7_uint16 flowCounters_available_entries;
  L7_uint16 flowProfiles_available_entries;
} st_ptin_policy_resources;


/* BW Profiles structs ********************************************************/
/* Bandwidth profile applied to an EVC(+interface/client) */
typedef struct {
  L7_uint32 cir;                        // Commited Information Rate
  L7_uint32 cbs;                        // Commited Burst Size
  L7_uint32 eir;                        // Excess Information Rate
  L7_uint32 ebs;                        // Excess Burst Size
} ptin_bw_meter_t;

/* Structure describing the bw policer rule */
typedef struct {
  L7_uint8   inUse;                     // This entry is in use (TRUE to be in use)
  DAPI_USP_t ddUsp_src;                 // Source interface (have precedence over port_bmp_src)
  DAPI_USP_t ddUsp_dst;                 // Destination interface (have precedence over port_bmp_dst)
  L7_uint16  outer_vlan_in;             // SVlan (0 value means no appliance)
  L7_uint16  outer_vlan_out;            // SVlan (0 value means no appliance)
  L7_uint16  inner_vlan_in;             // CVlan (0 value means no appliance)
  L7_uint16  inner_vlan_out;            // CVlan (0 value means no appliance)
  ptin_bw_meter_t meter;                // Meter data
  L7_int     policy_id;                 // Policer rule id (must be a positive value)
} ptin_bw_policy_t;

typedef struct {
  DAPI_USP_t ddUsp_src;                 // Source interface (have precedence over port_bmp_src)
  DAPI_USP_t ddUsp_dst;                 // Destination interface (have precedence over port_bmp_dst)
  L7_uint16  outer_vlan_in;             // SVlan (0 value means no appliance)               
  L7_uint16  outer_vlan_out;            // SVlan (0 value means no appliance)
  L7_uint16  inner_vlan_in;             // CVlan (0 value means no appliance)               
  L7_uint16  inner_vlan_out;            // CVlan (0 value means no appliance)
  ptin_bw_meter_t meter;                // Meter data
} ptin_bw_profile_t;

typedef struct {
  L7_int             operation;         // Operation: DAPI_CMD_GET / DAPI_CMD_SET / DAPI_CMD_CLEAR / DAPI_CMD_CLEAR_ALL
  ptin_bw_profile_t  profile;           // Profile data
  ptin_bw_policy_t  *policy_ptr;        // Policy pointer
} ptin_bwPolicer_t;

/* Storm control */
#define PTIN_STORMCONTROL_FLAGS_BCAST_SET 0x0001
#define PTIN_STORMCONTROL_FLAGS_MCAST_SET 0x0002
#define PTIN_STORMCONTROL_FLAGS_UCUNK_SET 0x0004
typedef struct {
  L7_uint16 flags;              /* Control flags: 0x0000 */
  L7_uint32 bcast_rate;         /* [flags=0x0001] in bps */
  L7_uint32 mcast_rate;         /* [flags=0x0002] in bps */
  L7_uint32 ucunk_rate;         /* [flags=0x0004] in bps */
} ptin_stormControl_t;

/* EVC statistics */

typedef enum {
  PTIN_PACKETS_TYPE_FIRST=0,
  PTIN_PACKETS_TYPE_UNICAST=PTIN_PACKETS_TYPE_FIRST,
  PTIN_PACKETS_TYPE_BROADCAST,
  PTIN_PACKETS_TYPE_MULTICAST,
  PTIN_PACKETS_TYPE_DROPPED,
  PTIN_PACKETS_TYPE_MAX,
  PTIN_PACKETS_TYPE_NONE
} ptin_packets_type_t;

// Policy data
#define PTIN_POLICY_STAGE_COUNT 3
typedef struct {
  L7_uint8   inUse;                     // is in use?
  DAPI_USP_t ddUsp_src;                 // Interface
  DAPI_USP_t ddUsp_dst;                 // Interface
  L7_uint16  outer_vlan_in;             // Outer vlan
  L7_uint16  outer_vlan_out;            // Outer vlan
  L7_uint16  inner_vlan_in;             // Inner vlan
  L7_uint16  inner_vlan_out;            // Inner vlan
  L7_uint32  dip;                       // Destination IP (v4)
  L7_int     policy_id[PTIN_POLICY_STAGE_COUNT];                       // Policer rule id (must be a positive value)
  L7_int     rule_id[PTIN_POLICY_STAGE_COUNT][PTIN_PACKETS_TYPE_MAX];  // Rule ids for each packet type
} ptin_evcStats_policy_t;

// Policy data
typedef struct {
  DAPI_USP_t ddUsp_src;                 // Interface
  DAPI_USP_t ddUsp_dst;                 // Interface
  L7_uint16  outer_vlan_in;             // Outer vlan
  L7_uint16  outer_vlan_out;            // Outer vlan
  L7_uint16  inner_vlan_in;             // Inner vlan
  L7_uint16  inner_vlan_out;            // Inner vlan
  L7_uint32  dst_ip;                    // Destination IP (v4)
} ptin_evcStats_profile_t;

// Counter fields
typedef struct {
  L7_uint32 pktTotal;                   // Total packets (Unicast+Multicast+Broadcast)
  L7_uint32 pktUnicast;                 // Unicast packets
  L7_uint32 pktMulticast;               // Multicast packets
  L7_uint32 pktBroadcast;               // Broadcast packets
  L7_uint32 pktDropped;                 // Packets dropped
} ptin_counters_t;

#define PTIN_EVCSTATS_COUNTERS_MASK_RX  0x01
#define PTIN_EVCSTATS_COUNTERS_MASK_TX  0x02

// Statistics data
typedef struct {
  L7_uint8 mask;                        // Mask
  ptin_counters_t rx;                   // [mask=0x01] RX counters
  ptin_counters_t tx;                   // [mask=0x02] TX counters
} ptin_evcStats_counters_t;

// Global structure
typedef struct {
  L7_int    operation;                  // Operation: DAPI_CMD_GET / DAPI_CMD_SET / DAPI_CMD_CLEAR / DAPI_CMD_CLEAR_ALL
  ptin_evcStats_profile_t   profile;    // EVC stat profile
  ptin_evcStats_counters_t  counters;   // Counters data
  ptin_evcStats_policy_t   *policy_ptr; // Policy pointer
} ptin_evcStats_t;

// Rate limit structure
#define PACKET_RATE_LIMIT_BROADCAST 0x0001
#define PACKET_RATE_LIMIT_MULTICAST 0x0002
typedef struct {
  L7_int    operation;                  // Operation: DAPI_CMD_GET / DAPI_CMD_SET / DAPI_CMD_CLEAR / DAPI_CMD_CLEAR_ALL
  L7_uint16 vlanId;
  L7_uint16 trafficType;
  L7_uint32 rate;
} ptin_pktRateLimit_t;

/* Network Connectivity (inBand) structs **************************************/
#define PTIN_NTWCONN_MASK_INTF          0x00000001
#define PTIN_NTWCONN_MASK_IPADDR        0x00000002
#define PTIN_NTWCONN_MASK_NETMASK       0x00000004
#define PTIN_NTWCONN_MASK_GATEWAY       0x00000008
#define PTIN_NTWCONN_MASK_VLAN          0x00000010
//#define PTIN_NTWCONN_MASK_MACADDR       0x00000020
//#define PTIN_NTWCONN_MASK_PROTOCOL      0x00000040

typedef struct {
  L7_uint32 mask;
  L7_uint32 ipaddr;                         // 0x0002: IP Addr: xx.xx.xx.xxh
  L7_uint32 netmask;                        // 0x0004: Netmask: xx.xx.xx.xxh
  L7_uint32 gateway;                        // 0x0008: Default gateway: xx.xx.xx.xxh
  L7_uint16 mgmtVlanId;                     // 0x0010: Management VlanId
//  L7_uint8  localMacAddr[L7_MAC_ADDR_LEN];  // 0x0020: Locally Admnistered MAC Address xx:xx:xx:xx:xx:xxh
//  L7_uint8  ntwConfProtocol;                // 0x0040: Network Configuration Protocol: 1:None, 2:Bootp, 3:DHCP
  L7_uint8    n_intf;                       // Number of interfaces present on intf array
  ptin_intf_t intf[PTIN_SYSTEM_MAX_N_PORTS]; // 0x0001: Interface Id# (phy ports / LAGs)
                                                  //         Interface type: { 0-Physical, 1-Logical (LAG) }
} ptin_NtwConnectivity_t;


/* DHCP */

/* DHCP Binding Table */
typedef struct {
  L7_uint16             entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint16             evc_idx;                // EVCid
  ptin_intf_t           ptin_intf;              // Interface
  L7_uint16             outer_vlan;             // Outer vlan
  L7_uint16             inner_vlan;             // Inner vlan
  L7_uint8              macAddr[6];             // MAC Address
  L7_in_addr_t          ipAddr;                 // IP address
  L7_uint32             remLeave;               // Remaining Leave time in seconds
  L7_uint8              bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} ptin_DHCP_bind_entry;

/* DHCP Binding Table - IPv6 Compatible */
typedef struct {
  L7_uint16             entry_index;            // Entry index (from 0 to bind_table_total_entries-1)
  L7_uint16             evc_idx;                // EVCid
  L7_uint16             outer_vlan;             // Service vlan: not used yet
  L7_uint16             inner_vlan;             // Client clanId
  ptin_intf_t           ptin_intf;              // Interface
  L7_uint8              macAddr[6];             // MAC Address
  chmessage_ip_addr_t   ipAddr;                 // IP address
  L7_uint32             remLeave;               // Remaining Leave time in seconds
  L7_uint8              bindingType;            // Binding type: 0=Tentative, 1=Static, 2=Dynamic
} ptin_DHCPv4v6_bind_entry;


/* L2 Table */

typedef struct {
  L7_uint32   entryId;
  L7_uint16   evcId;          /* EVC id that belongs this MAC entry (if no EVC is associated its value will be 0xffff) */
  L7_uint16   vlanId;         /* Vlan associated to this MAC entry */
  L7_uint8    addr[6];        /* MAC address */
  ptin_intf_t intf;           /* Interface attached to this entry */
  L7_uint8    static_entry;   /* Is this entry static? */
} ptin_switch_mac_entry;

/* IGMP proxy structs *********************************************************/
/* Querier's config structure */
#define PTIN_IGMP_QUERIER_MASK_RV         0x0001
#define PTIN_IGMP_QUERIER_MASK_QI         0x0002
#define PTIN_IGMP_QUERIER_MASK_QRI        0x0004
#define PTIN_IGMP_QUERIER_MASK_GMI        0x0008
#define PTIN_IGMP_QUERIER_MASK_OQPI       0x0010
#define PTIN_IGMP_QUERIER_MASK_SQI        0x0020
#define PTIN_IGMP_QUERIER_MASK_SQC        0x0040
#define PTIN_IGMP_QUERIER_MASK_LMQI       0x0080
#define PTIN_IGMP_QUERIER_MASK_LMQC       0x0100
#define PTIN_IGMP_QUERIER_MASK_OHPT       0x0200

#define PTIN_IGMP_QUERIER_MASK_AUTO_GMI   0x0001
#define PTIN_IGMP_QUERIER_MASK_AUTO_OQPI  0x0002
#define PTIN_IGMP_QUERIER_MASK_AUTO_SQI   0x0004
#define PTIN_IGMP_QUERIER_MASK_AUTO_SQC   0x0008
#define PTIN_IGMP_QUERIER_MASK_AUTO_LMQC  0x0010
#define PTIN_IGMP_QUERIER_MASK_AUTO_OHPT  0x0020

typedef struct {
  L7_uint16 mask;                               /* PTIN_IGMP_QUERIER_MASK_xxxx */
  L7_uint16 flags;                              /* [0x0001] - AUTO_GMI
                                                 * [0x0002] - AUTO_OQPI
                                                 * [0x0004] - AUTO_SQI
                                                 * [0x0008] - AUTO_SQC
                                                 * [0x0010] - AUTO_LMQC
                                                 * [0x0020] - AUTO_OHPT */
  L7_uint8  robustness;                         /* [Mask: 0x0001] */
  L7_uint16 query_interval;                     /* [Mask: 0x0002] */
  L7_uint16 query_response_interval;            /* [Mask: 0x0004] */
  L7_uint16 group_membership_interval;          /* [Mask: 0x0008] */
  L7_uint16 other_querier_present_interval;     /* [Mask: 0x0010] */
  L7_uint16 startup_query_interval;             /* [Mask: 0x0020] */
  L7_uint16 startup_query_count;                /* [Mask: 0x0040] */
  L7_uint16 last_member_query_interval;         /* [Mask: 0x0080] */
  L7_uint16 last_member_query_count;            /* [Mask: 0x0100] */
  L7_uint16 older_host_present_timeout;         /* [Mask: 0x0200] */
} ptin_IgmpV3QuerierCfg_t;

/* Host's config structure */
#define PTIN_IGMP_HOST_MASK_RV            0x0001
#define PTIN_IGMP_HOST_MASK_URI           0x0002
#define PTIN_IGMP_HOST_MASK_OQPT          0x0004
#define PTIN_IGMP_HOST_MASK_MRPR          0x0008

#define PTIN_IGMP_HOST_MASK_AUTO_OQPT     0x0001

typedef struct {
  L7_uint8  mask;                               /* PTIN_IGMP_HOST_MASK_xxxx */
  L7_uint8  flags;                              /* [0x01] - AUTO_OQPT */
  L7_uint8  robustness;                         /* [Mask: 0x01] */
  L7_uint16 unsolicited_report_interval;        /* [Mask: 0x02] */
  L7_uint16 older_querier_present_timeout;      /* [Mask: 0x04] */
  L7_uint8  max_records_per_report;             /* [Mask: 0x08] */
} ptin_IgmpV3HostCfg_t;

/* Proxy's config structure */
#define PTIN_IGMP_PROXY_MASK_ADMIN           0x0001
#define PTIN_IGMP_PROXY_MASK_NETWORKVERSION  0x0002
#define PTIN_IGMP_PROXY_MASK_CLIENTVERSION   0x0004
#define PTIN_IGMP_PROXY_MASK_IPV4            0x0008
#define PTIN_IGMP_PROXY_MASK_COS             0x0010
#define PTIN_IGMP_PROXY_MASK_FASTLEAVE       0x0020
#define PTIN_IGMP_PROXY_MASK_QUERIER         0x0040
#define PTIN_IGMP_PROXY_MASK_HOST            0x0080

typedef struct {
  L7_uint8      mask;                           /* PTIN_IGMP_PROXY_MASK_xxxx */

  L7_uint8      admin;                          /* Mask: 0x01 (Global admin for both host and querier) */
  L7_uint8      networkVersion;                 /* Mask: 0x02 (defines maximum working version - overrides query/host version) */
  L7_uint8      clientVersion;                  /* Mask: 0x04 (defines maximum working version - overrides query/host version) */
  L7_in_addr_t  ipv4_addr;                      /* Mask: 0x08 (Proxy IP (for both host and querier)) */
  L7_uint8      igmp_cos;                       /* Mask: 0x10 [1..7] */
  L7_uint8      fast_leave;                     /* Mask: 0x20 TRUE/FALSE */

  ptin_IgmpV3QuerierCfg_t querier;              /* Mask: 0x40 */
  ptin_IgmpV3HostCfg_t    host;                 /* Mask: 0x80 */

} ptin_IgmpProxyCfg_t;

typedef struct
{
   L7_inet_addr_t groupAddr;
   L7_inet_addr_t sourceAddr;
   L7_uint8 static_type;      // 0: Dynamic; 1: Static
} ptin_igmpChannelInfo_t;

/* QoS mapping */ 
#define PTIN_QOS_INTF_TRUSTMODE_MASK        0x01
#define PTIN_QOS_INTF_BANDWIDTHUNIT_MASK    0x02
#define PTIN_QOS_INTF_SHAPINGRATE_MASK      0x04
#define PTIN_QOS_INTF_WRED_DECAY_EXP_MASK   0x08
#define PTIN_QOS_INTF_PACKETPRIO_MASK       0x10
#define PTIN_QOS_INTF_PACKETPRIO_COS_MASK     0xff
typedef struct
{
  L7_uint8    mask;                     // Configurations mask
  L7_uint8    trust_mode;               // [mask=0x01] 0-None, 1-Untrust markings, 2-802.1p marking, 3: IP-precedence mark; 4-DSCP mark (Default=2)
  L7_uint8    bandwidth_unit;           // [mask=0x02] 0: Percentage, 1: Kbps, 2: PPS (Default=0)
  L7_uint32   shaping_rate;             // [mask=0x04] 0-100: Default=0 (unlimited)
  L7_uint8    wred_decay_exponent;      // [mask=0x08] WRED Decay Exponent: 0-15
  struct {                              // Packet priority map
    L7_uint8  mask;                     //   pktpriority map mask (nth bit, tells to configure the nth priority)
    L7_uint32 cos[8];                   //   Mapping: CoS(pcp): Default={0,1,2,3,4,5,6,7}
  } pktprio;                            // [mask=0x08] Packet priority map
} ptin_QoS_intf_t;

/* QoS scheduler and bandwidths */
#define PTIN_QOS_COS_SCHEDULER_MASK       0x01
#define PTIN_QOS_COS_QUEUE_MANGM_MASK     0x02
#define PTIN_QOS_COS_WRED_THRESH_MIN_MASK 0x04
#define PTIN_QOS_COS_WRED_THRESH_MAX_MASK 0x08
#define PTIN_QOS_COS_BW_MIN_MASK          0x10
#define PTIN_QOS_COS_BW_MAX_MASK          0x20
typedef struct
{
  L7_uint8    mask;                   // Mask
  L7_uint8    scheduler_type;         // [mask=0x01] Scheduler type: 0-None, 1-Strict, 2-Weighted (Default=1)
  L7_uint8    queue_management_type;  // [mask=0x02] Queue management type: 0-taildrop, 1-wred
  L7_uint8    wred_min_threshold;     // [mask=0x04] WRED minimum threshold: 0-100
  L7_uint8    wred_max_threshold;     // [mask=0x08] WRED maximum threshold: 0-100
  L7_uint32   min_bandwidth;          // [mask=0x10] Minimum bandwidth (0-100): Default=0 (no guarantee)
  L7_uint32   max_bandwidth;          // [mask=0x20] Maximum bandwidth (0-100): Default=0 (unlimited)
} ptin_QoS_cos_t;



/***************************************************************************** 
 * ERPS Configuration
 *****************************************************************************/

typedef enum {
   ERPS_PORTROLE_NONRPL       = 0,
   ERPS_PORTROLE_RPL          = 1,
   ERPS_PORTROLE_RPLNEIGHBOUR = 2,
   ERPS_PORTROLE_RPLNEXTNEIGH = 3,
} ERPS_PORTROLE;

typedef struct {
   unsigned char slot;
   unsigned char type;
   unsigned char idx;
} ptin_erpsPort_t;

typedef enum
{
   ERPS_CONF_MASK_BIT_RINGID        = 0x0001, //[RC]
   ERPS_CONF_MASK_BIT_ISOPENRING    = 0x0002, //[RW]
   ERPS_CONF_MASK_BIT_CONTROLVID    = 0x0004, //[RC]
   ERPS_CONF_MASK_BIT_MEGLEVEL      = 0x0008, //[RC]
   ERPS_CONF_MASK_BIT_PORT0         = 0x0010, //[RC]
   ERPS_CONF_MASK_BIT_PORT1         = 0x0020, //[RC]
   ERPS_CONF_MASK_BIT_PORT0ROLE     = 0x0040, //[RC]
   ERPS_CONF_MASK_BIT_PORT1ROLE     = 0x0080, //[RC]
   ERPS_CONF_MASK_BIT_PORT0CFMIDX   = 0x0100, //[RW]
   ERPS_CONF_MASK_BIT_PORT1CFMIDX   = 0x0200, //[RW]
   ERPS_CONF_MASK_BIT_REVERTIVE     = 0x0400, //[RW]
   ERPS_CONF_MASK_BIT_GUARDTIMER    = 0x0800, //[RW]
   ERPS_CONF_MASK_BIT_HOLDOFFTIMER  = 0x1000, //[RW]
   ERPS_CONF_MASK_BIT_WAITTORESTORE = 0x2000, //[RW]
   ERPS_CONF_MASK_BIT_VIDBMP        = 0x4000, //[RW]
} ERPS_CONF_MASK;

#define HW_ERPS_MASK_FULL 0x7FFF

#endif /* _PTIN_STRUCTS_H */

