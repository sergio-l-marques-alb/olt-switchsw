#ifndef _TRMI_H_
#define _TRMI_H_

//MESSAGE CODES*********************************************************************************
#define CCMSG_APPLICATION_IS_ALIVE          0x9000  // (To be used with fp.cli)
#define CCMSG_BOARD_SHOW                    0x9001  // struct msg_FWFastpathInfo
#define CCMSG_APP_CHANGE_STDOUT             0x91F0  // (To be used with fastpath.cli)
#define CCMSG_APP_LOGGER_OUTPUT             0x91F2  // (To be used with fastpath.cli)

#define CCMSG_ETH_PHY_CONFIG_SET            0x9010  // struct msg_HWEthPhyConf_t
#define CCMSG_ETH_PHY_CONFIG_GET            0x9011  // struct msg_HWEthPhyConf_t
#define CCMSG_ETH_PHY_STATE_GET             0x9012  // struct msg_HWEthPhyState_t

#define CCMSG_ETH_PHY_COUNTERS_GET          0x9013  // struct msg_HWEthRFC2819_PortStatistics_t
#define CCMSG_ETH_PHY_COUNTERS_CLEAR        0x9014  // struct msg_HWEthRFC2819_PortStatistics_t

#define CCMSG_ETH_EVC_ADD                   0x9031  // struct msg_HwEthMef10Evc_t
#define CCMSG_ETH_EVC_REMOVE                0x9032  // struct msg_HwEthMef10EvcRemove_t
#define CCMSG_ETH_EVC_PORT_ADD              0x903C  // struct msg_HWevcPort_t
#define CCMSG_ETH_EVC_PORT_REMOVE           0x903D  // struct msg_HWevcPort_t

//ESSENTIAL DEFINES*****************************************************************************

// retirado do ficheiro  u-boot-1.1.x/include/pci_ids.h
#define PCI_VENDOR_ID_BROADCOM          0x14e4
#define PTIN_SYSTEM_MAX_N_PORTS   64
#define PTIN_SYSTEM_MAX_N_LAGS    PTIN_SYSTEM_MAX_N_PORTS
#define PTIN_SYSTEM_N_PORTS       18
#define PTIN_SYSTEM_N_LAGS        PTIN_SYSTEM_N_PORTS
#define PTIN_SYSTEM_N_INTERF      (PTIN_SYSTEM_N_PORTS+PTIN_SYSTEM_N_LAGS)

//MESSAGE STRUCTS*******************************************************************************

/* Generic structs */
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

/* Struct used for reference valuue */
#define MSG_ID_DEF_TYPE     0       /* Considered to be evc id */
#define MSG_ID_EVC_TYPE     1       /* Use evc id */
#define MSG_ID_NNIVID_TYPE  2       /* Use NNI vlan */
typedef struct
{
  L7_uint8  id_type;                /* Reference id type: 1->evc id; 2->NNI SVid */
  union
  {
    L7_uint32   evc_id;             /* EVC ID: 0xffff to use NNI_STAG */
    L7_uint32   nni_vid;            /* NNI_STAG (to be used, when evc id id 0xffff) */
  } __attribute__((packed)) id_val;
} __attribute__((packed)) msg_id_t;

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
  L7_uint16 inner_vid;    // Inner VLAN id [1..4094]
  L7_uint8  pcp;          // Packet's priority (to be valid should have bit 4 = 1)
  L7_uint16 ethertype;    // Packet's ethertype
} __attribute__((packed)) msg_HwEthMef10Intf_t;

/* EVC config */
typedef struct {
  L7_uint8  SlotId;
  L7_uint32 id;           // EVC Id [1..PTIN_SYSTEM_N_EVCS]                                 /* EVC id */
  L7_uint32 flags;        // 0x000001 - Bundling
                          // 0x000002 - All to One
                          // 0x000004 - Stacked
                          // 0x000008 - Mac Learning
                          // 0x000010 - cpu Trapping
                          // 0x000080 - IPSG protocol
                          // 0x000100 - DHCP protocol
                          // 0x000200 - IGMP protocol
                          // 0x000400 - PPPOE protocol
                          // 0x001000 - DHCPv6 protocol
                          // 0x002000 - MLD protocol
                          // 0x010000 - P2P EVC
                          // 0x020000 - QUATTRO EVC
  L7_uint8  type;         // (not used) { 0 - p2p, 1 - mp2mp, 2 - rooted mp }
  L7_uint8  mc_flood;     // MC flood type {0-All, 1-Unknown, 2-None} (PTin custom field)
  
  L7_uint8  n_intf;       // Number of interfaces present on intf array
  msg_HwEthMef10Intf_t intf[PTIN_SYSTEM_MAX_N_PORTS];

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
  L7_uint32 evcId;        // EVC Id

  msg_HwEthMef10Intf_t intf;
} __attribute__((packed)) msg_HWevcPort_t;



#endif //_TRMI_H_

