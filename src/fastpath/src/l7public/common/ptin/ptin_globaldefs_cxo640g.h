#ifndef _PTIN_GLOBALDEFS_CXO640G_H
#define _PTIN_GLOBALDEFS_CXO640G_H

#define IGMP_DYNAMIC_CLIENTS_SUPPORTED

/* To manage linkscan, uncomment this line */
#define PTIN_LINKSCAN_CONTROL

/* Allows direct control over port add/remove of a LAG (shortcut to message queues) */
#define LAG_DIRECT_CONTROL_FEATURE  1

#define __Y1731_802_1ag_OAM_ETH__
//#define __APS_AND_CCM_COMMON_FILTER__     //can't define this; most likely we aren't being allowed to add BROAD_FIELD_IP6_DST to group existent qualifiers
#define COMMON_APS_CCM_CALLBACKS__ETYPE_REG

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED
#define PTIN_PTP_PORT     0   /* PTP interface port nr (xe56) */ /* 10G port */


# define PTIN_PHY_PREEMPHASIS_DEFAULT         0xBF00 /* main=48 post=15 */
# define PTIN_PHY_PREEMPHASIS_FARTHEST_SLOTS  0xCEC0 /* main=44 post=19 */
# define PTIN_PHY_PREEMPHASIS_NEAREST_SLOTS   0xB720 /* main=50 post=13 */

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 4
#define PTIN_SYS_MX1_SLOT           (PTIN_SYS_LC_SLOT_MIN-1)
#define PTIN_SYS_MX2_SLOT           (PTIN_SYS_LC_SLOT_MAX+1)

# define PTIN_SLOT_WORK                0
# define PTIN_SLOT_PROT                1

# define PTIN_SYSTEM_N_PORTS           64
# define PTIN_SYSTEM_N_PONS            0 
# define PTIN_SYSTEM_N_ETH             0
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   (PTIN_SYSTEM_N_PORTS/2-1)
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_UPLINK_INTERF   PTIN_SYSTEM_N_PORTS

# define PTIN_SYSTEM_INTERNAL_LAGID_BASE    18
# define PTIN_SYSTEM_PROTECTION_LAGID_BASE  40

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0xFFFFFFFFFFFFFFFFULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            256   /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         8192  /* Maximum nr of clients allowed in this equipment */
# define PTIN_SYSTEM_N_FLOWS_MAX       8192  /* Maximum nr of flows (total) */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK          32    /* Number of vlans for each P2MP service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_CPU_MCAST_VLAN_BLOCKS    16    /* Number of vlan blocks for CPU-Multicast services */
# define PTIN_SYSTEM_EVC_CPU_BCAST_VLAN_BLOCKS    16    /* Number of vlan blocks for CPU-Broadcast services */
# define PTIN_SYSTEM_EVC_CPU_VLAN_BLOCKS          (PTIN_SYSTEM_EVC_CPU_BCAST_VLAN_BLOCKS + PTIN_SYSTEM_EVC_CPU_MCAST_VLAN_BLOCKS)
# define PTIN_SYSTEM_EVC_BITSTREAM_VLAN_BLOCKS    32    /* Number of vlan blocks for Bitstream services */
# define PTIN_SYSTEM_EVC_ELAN_VLAN_BLOCKS         (PTIN_SYSTEM_EVC_BITSTREAM_VLAN_BLOCKS + PTIN_SYSTEM_EVC_CPU_VLAN_BLOCKS)
/* No E-TREEs */
#if 0
# define PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_BLOCKS    16
# define PTIN_SYSTEM_EVC_ETREE_BS_VLAN_BLOCKS     15    /* Number of vlan blocks for E-Tree Bitstream services */
# define PTIN_SYSTEM_EVC_ETREE_VLAN_BLOCKS        (PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_BLOCKS + PTIN_SYSTEM_EVC_ETREE_BS_VLAN_BLOCKS)
#endif

/* ELAN Vlans */
# define PTIN_SYSTEM_EVC_ELAN_VLAN_MIN     PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_ELAN_VLAN_MAX    (/*PTIN_SYSTEM_EVC_ELAN_VLAN_MIN +*/ PTIN_SYSTEM_EVC_ELAN_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)

# define PTIN_SYSTEM_EVC_CPU_VLAN_MIN      PTIN_SYSTEM_EVC_ELAN_VLAN_MIN
# define PTIN_SYSTEM_EVC_CPU_VLAN_MAX     (/*PTIN_SYSTEM_EVC_CPU_VLAN_MIN +*/ PTIN_SYSTEM_EVC_CPU_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_CPU_VLAN_MASK    (~(PTIN_SYSTEM_EVC_CPU_VLAN_MAX /*- PTIN_SYSTEM_EVC_CPU_VLAN_MIN*/) & 0xfff)

# define PTIN_SYSTEM_EVC_MCAST_VLAN_MIN    PTIN_SYSTEM_EVC_CPU_VLAN_MIN
# define PTIN_SYSTEM_EVC_MCAST_VLAN_MAX   (/*PTIN_SYSTEM_EVC_CPU_MCAST_VLAN_MIN +*/ PTIN_SYSTEM_EVC_CPU_MCAST_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_MCAST_VLAN_MASK  (~(PTIN_SYSTEM_EVC_MCAST_VLAN_MAX /*- PTIN_SYSTEM_EVC_MCAST_VLAN_MIN*/) & 0xfff)

# define PTIN_SYSTEM_EVC_BCAST_VLAN_MIN   (PTIN_SYSTEM_EVC_MCAST_VLAN_MAX + 1)
# define PTIN_SYSTEM_EVC_BCAST_VLAN_MAX   (PTIN_SYSTEM_EVC_BCAST_VLAN_MIN + PTIN_SYSTEM_EVC_CPU_BCAST_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_BCAST_VLAN_MASK  (~(PTIN_SYSTEM_EVC_BCAST_VLAN_MAX - PTIN_SYSTEM_EVC_BCAST_VLAN_MIN) & 0xfff)

# define PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN  (PTIN_SYSTEM_EVC_CPU_VLAN_MAX + 1)
# define PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX  (PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN + PTIN_SYSTEM_EVC_BITSTREAM_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1 - PTIN_RESERVED_VLANS)


/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      0
# if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          1024   /*(((~PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK) & 0x3ff)+1)*/
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

/* No E-TREEs */
#if 0
/* E-Tree Vlans */
# define PTIN_SYSTEM_EVC_ETREE_VLAN_MIN        3072
# define PTIN_SYSTEM_EVC_ETREE_VLAN_MAX       (PTIN_SYSTEM_EVC_ETREE_VLAN_MIN + PTIN_SYSTEM_EVC_ETREE_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)

# define PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MIN    PTIN_SYSTEM_EVC_ETREE_VLAN_MIN
# define PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MAX   (PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MIN + PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MASK  (~(PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MAX - PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MIN) & 0xfff)

# define PTIN_SYSTEM_EVC_ETREE_BS_VLAN_MIN    (PTIN_SYSTEM_EVC_ETREE_CPU_VLAN_MAX + 1)
# define PTIN_SYSTEM_EVC_ETREE_BS_VLAN_MAX    (PTIN_SYSTEM_EVC_ETREE_BS_VLAN_MIN + PTIN_SYSTEM_EVC_ETREE_BS_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES               40    /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP        (L7_MAX_PORT_COUNT + L7_MAX_CPU_SLOTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 2)   /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF          1
# define PTIN_SYSTEM_IGMP_MAXONUS                   (PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*PTIN_SYSTEM_N_INTERF)  /* 20 clients per IGMP instance (20 slots) */
# define PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU        1     /* Settop boxes connected to ONUs */
# define PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF       PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF
# define PTIN_SYSTEM_IGMP_MAXCLIENTS                PTIN_SYSTEM_IGMP_MAXONUS
# define PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP      5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE        (PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF/(sizeof(L7_uint32)*8)+1)  /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE           1     /* Maximum number of entries in Query queue */
# define PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT 0     /*This feature should be only enabled when this condition is verified !PTIN_BOARD_IS_MATRIX*/
# define PTIN_SYSTEM_IGMP_MAXPACKAGES               256   /*Max Multicast Packages Per Card*/    
# define PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD      1     /*Support L3 Multicast Forwarding*/
# define PTIN_SYSTEM_IGMP_L3_MULTICAST_MAX_ENTRIES  4096  /*Max Number of L3 Multicast Entries*/
# define PTIN_SYSTEM_IGMP_GROUP_LIST_MAX_ENTRIES    0 /*Max Number of Group List Entries*/

# define PTIN_SYSTEM_N_DHCP_INSTANCES               1     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES              1     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_DHCP_MAXCLIENTS                2     /* Maximum DHCP clients */
# define PTIN_SYSTEM_PPPOE_MAXCLIENTS               2     /* Maximum PPPoE clients */

#define SNOOP_PTIN_MGMD_SUPPORT //Comment this line if you want to disable MGMD integration (not supported..)
#define SNOOP_PTIN_IGMPv3_GLOBAL 1//Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER 1//Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY 1//Change to 0 if you want to disable IGMPv3 Proxy SubModule

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_CPLD
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID_CXO640G_V1            0x1259
# define CPLD_ID                       0x1324
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_HW_ID_REG                0x0008
# define CPLD_CHASSIS_ID_REG           0x000C
# define CPLD_SLOT_ID_REG              0x000F

typedef union
{
  L7_uint8 map[PTIN_CPLD_MAP_SIZE];
  struct
  {
    L7_uint16 id;               /* 0x00 (2 bytes) */
    L7_uint8  ver;              /* 0x02 (1 byte)  */
    L7_uint8  empty0[0x08 - 0x03];
    L7_uint8  hw_id;            /* 0x08 (1 byte)  */
    L7_uint8  hw_ver;           /* 0x09 (1 byte)  */
    L7_uint8  empty1[0x0C - 0x0A];
    L7_uint8  chassis_id;       /* 0x0C (1 byte)  */
    L7_uint8  empty2[0x0F - 0x0D];
    L7_uint8  slot_id;          /* 0x0F (1 byte)  */
    L7_uint8  empty3[0x18 - 0x10];
    L7_uint8  mx_get_active;    /* 0x18 (1 byte)  */
    L7_uint8  empty4[0x1E - 0x19];
    L7_uint8  mx_is_active;     /* 0x1E (1 byte)  */
    L7_uint8  empty5[PTIN_CPLD_MAP_SIZE - 0x1F];
  } reg;
} st_cpld_map_t;

#ifdef MAP_CPLD
extern volatile st_cpld_map_t *cpld_map;
#endif

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0xffff - 0x0000];
  } reg;
} st_fpga_map_t;

#ifdef MAP_FPGA
extern volatile st_fpga_map_t *fpga_map;
#endif

/* Slot/port map: only for matrix board */
extern int ptin_sys_slotport_to_intf_map[PTIN_SYS_SLOTS_MAX+1][PTIN_SYS_INTFS_PER_SLOT_MAX];
extern int ptin_sys_intf_to_slot_map[PTIN_SYSTEM_N_PORTS];
extern int ptin_sys_intf_to_port_map[PTIN_SYSTEM_N_PORTS];

# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
#ifdef MAP_CPLD
# define IPC_SERVER_IPADDR             ((cpld_map->reg.slot_id == 0) ? IPC_SERVER_IPADDR_WORKING : IPC_SERVER_IPADDR_PROTECTION)
#else
# define IPC_SERVER_IPADDR             IPC_SERVER_IPADDR_WORKING  /* Default ip address */
#endif

// MX IP address
# define IPC_MX_IPADDR_WORKING      0xC0A8C801  /* 192.168.200.1: Working Matrix */
# define IPC_MX_IPADDR_PROTECTION   0xC0A8C802  /* 192.168.200.2: Protection Matrix */
#ifdef MAP_CPLD
# define IPC_MX_IPADDR              ((cpld_map->reg.slot_id == 0) ? IPC_MX_IPADDR_WORKING : IPC_MX_IPADDR_PROTECTION)
#else
# define IPC_MX_IPADDR              IPC_MX_IPADDR_WORKING  /* Default ip address */
#endif

#endif /* _PTIN_GLOBALDEFS_CXO640G_H */

