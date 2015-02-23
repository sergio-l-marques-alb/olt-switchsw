#ifndef _PTIN_GLOBALDEFS_TA48GE_H
#define _PTIN_GLOBALDEFS_TA48GE_H

/* Allows direct control over port add/remove of a LAG (shortcut to message queues) */
#define LAG_DIRECT_CONTROL_FEATURE  1

//#define __Y1731_802_1ag_OAM_ETH__
//#define __APS_AND_CCM_COMMON_FILTER__     //can't define this; most likely we aren't being allowed to add BROAD_FIELD_IP6_DST to group existent qualifiers
//#define COMMON_APS_CCM_CALLBACKS__ETYPE_REG
//#define USING_SDK_OAM_FP_CREATE

#define IGMPASSOC_MULTI_MC_SUPPORTED
/* Querier will be active for Multicast services */
#define IGMP_QUERIER_IN_UC_EVC
#define IGMP_DYNAMIC_CLIENTS_SUPPORTED

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2
#define PTIN_SYS_MX1_SLOT           (PTIN_SYS_LC_SLOT_MIN-1)
#define PTIN_SYS_MX2_SLOT           (PTIN_SYS_LC_SLOT_MAX+1)

# define PTIN_SLOT_WORK                0
# define PTIN_SLOT_PROT                1

# define PTIN_SYSTEM_N_PORTS           52
# define PTIN_SYSTEM_N_PONS            0
# define PTIN_SYSTEM_N_ETH             48
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000FFFFFFFFFFFFULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0x000F000000000000ULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            4002  /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         8192  /* 1 client per interface */
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

# define PTIN_SYSTEM_EVC_MACLRN_VLAN_BLOCKS       8     /* Number of vlan blocks for CPU services with MAC learning */

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
# define PTIN_SYSTEM_EVC_BITSTR_NOMACL_VLAN_MIN  (PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_BITSTR_NOMACL_VLAN_MAX   PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX

/* ELAN Vlans with and without MAC learning */
# define PTIN_SYSTEM_EVC_MCAST_MACLRN_VLAN_MIN    PTIN_SYSTEM_EVC_MCAST_VLAN_MIN
# define PTIN_SYSTEM_EVC_MCAST_MACLRN_VLAN_MAX   (/*PTIN_SYSTEM_EVC_MCAST_UNKUC_VLAN_MIN +*/ PTIN_SYSTEM_EVC_MACLRN_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_MCAST_MACLRN_VLAN_MASK  (~(PTIN_SYSTEM_EVC_MCAST_MACLRN_VLAN_MAX /*- PTIN_SYSTEM_EVC_MCAST_UNKUC_VLAN_MIN*/) & 0xfff)

# define PTIN_SYSTEM_EVC_MCAST_NOMACL_VLAN_MIN   (PTIN_SYSTEM_EVC_MCAST_MACLRN_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_MCAST_NOMACL_VLAN_MAX    PTIN_SYSTEM_EVC_MCAST_VLAN_MAX

# define PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MIN    PTIN_SYSTEM_EVC_BCAST_VLAN_MIN
# define PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MAX   (PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MIN + PTIN_SYSTEM_EVC_MACLRN_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MASK  (~(PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MAX - PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MIN) & 0xfff)

# define PTIN_SYSTEM_EVC_BCAST_NOMACL_VLAN_MIN   (PTIN_SYSTEM_EVC_BCAST_MACLRN_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_BCAST_NOMACL_VLAN_MAX    PTIN_SYSTEM_EVC_BCAST_VLAN_MAX

# define PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MIN   PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN
# define PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MAX  (PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MIN + PTIN_SYSTEM_EVC_MACLRN_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)
# define PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MASK (~(PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MAX - PTIN_SYSTEM_EVC_BITSTR_MACLRN_VLAN_MIN) & 0xfff)


/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      1
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
# define PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF          1     /* 1 client per frontal port */
# define PTIN_SYSTEM_IGMP_MAXONUS                   (PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*PTIN_SYSTEM_N_INTERF)
# define PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU        128     /* Settop boxes connected to ONUs */
# define PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF       PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU /*PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*/
# define PTIN_SYSTEM_IGMP_MAXCLIENTS                max (PTIN_SYSTEM_IGMP_MAXONUS, PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF*PTIN_SYSTEM_N_INTERF)
# define PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP      5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE        (PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF/(sizeof(L7_uint32)*8)+1)  /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE           1     /* Maximum number of entries in Query queue */
# define PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT 1     /*This feature should be only enabled when this condition is verified !PTIN_BOARD_IS_MATRIX*/

# define PTIN_SYSTEM_N_DHCP_INSTANCES               32    /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES              32    /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE   8192  /* 1 client per DHCP instance */
# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE  8192  /* 1 client per PPPoE instance */

#define SNOOP_PTIN_MGMD_SUPPORT //Comment this line if you want to disable MGMD integration (not supported..)
#define SNOOP_PTIN_IGMPv3_GLOBAL 1//Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER 1//Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY 1//Change to 0 if you want to disable IGMPv3 Proxy SubModule

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_FPGA
# define MAP_CPLD
# define FPGA_BASE_ADDR                0xF8000000
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1255
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_BUILD_REG                0x0003
# define CPLD_HW_ID_REG                0x0008
# define CPLD_HW_VER_REG               0x0009
# define CPLD_CHASSIS_ID_REG           0x000C
# define CPLD_CHASSIS_VER_REG          0x000D
# define CPLD_SLOT_ID_REG              0x000F
# define CPLD_SLOT_MATRIX_REG          0x001E

typedef union
{
  L7_uint8 map[PTIN_CPLD_MAP_SIZE];
  struct
  {
    L7_uint16 id;               /* 0x00 (2 bytes) */
    L7_uint8  ver;              /* 0x02 (1 byte)  */
    L7_uint8  build;            /* 0x03 (1 byte)  */
    L7_uint8  empty1[0x08 - 0x04];
    L7_uint8  hw_id;            /* 0x08 (1 byte)  */
    L7_uint8  hw_ver;           /* 0x09 (1 byte)  */
    L7_uint8  bom_ver;          /* 0x0A (1 byte)  */
    L7_uint8  pb_id;            /* 0x0B (1 byte)  */
    L7_uint8  chassis_id;       /* 0x0C (1 byte)  */
    L7_uint8  chassis_ver;      /* 0x0D (1 byte)  */
    L7_uint8  chassis_bom_ver;  /* 0x0E (1 byte)  */
    L7_uint8  slot_id;          /* 0x0F (1 byte)  */
    L7_uint8  empty3[0x18 - 0x10];
    L7_uint8  mx_get_active;    /* 0x18 (1 byte)  */
    L7_uint8  empty4[0x1E - 0x19];
    L7_uint8  slot_matrix;      /* 0x1E (1 byte)  */
    L7_uint8  empty5[PTIN_CPLD_MAP_SIZE - 0x1F];
  } reg;
} st_cpld_map_t;

#ifdef MAP_CPLD
extern volatile st_cpld_map_t *cpld_map;
#endif

/* FPGA map registers */
# define FPGA_ID                       0x1295
# define FPGA_ID0_REG                  0x0000
# define FPGA_ID1_REG                  0x0001
# define FPGA_VER_REG                  0x0002
# define FPGA_TXDISABLE_REG            0xF061
# define FPGA_EXTPHY_RESET             0xF080

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  id0;                  /* 0x0000 (1 bytes) */
    L7_uint8  id1;                  /* 0x0001 (1 bytes) */
    L7_uint8  ver;                  /* 0x0002 (1 byte)  */
    L7_uint8  empty1[0x2000-0x0003];
    L7_uint8  led_blink_pairports[0x20];   /* 0x2000..0x201F */
    L7_uint8  led_color_pairports[0x20];   /* 0x2020..0x203F */
    L7_uint8  empty2[0x2100-0x2040];
    L7_uint8  led_blink_oddports[0x20];  /* 0x2100..0x211F */
    L7_uint8  led_color_oddports[0x20];  /* 0x2120..0x213F */
    L7_uint8  empty3[0xF080 - 0x2140];
    L7_uint8  extPhy_reset;         /* 0xF080 (1 byte)  */
    L7_uint8  empty4[0xF140 - 0xF081];
    L7_uint8  sfp_los_port_0to7;        /* 0xF140 */
    L7_uint8  empty5[0xF140 - 0xF081];
    L7_uint8  sfp_los_port_8to15;       /* 0xF150 */
    L7_uint8  empty6[0xF160 - 0xF151];
    L7_uint8  sfp_los_port_16to23;      /* 0xF160 */
    L7_uint8  empty7[0xF170 - 0xF161];
    L7_uint8  sfp_los_port_24to31;      /* 0xF170 */
    L7_uint8  empty8[0xF180 - 0xF171];
    L7_uint8  sfp_los_port_32to39;      /* 0xF180 */
    L7_uint8  empty9[0xF190 - 0xF181];
    L7_uint8  sfp_los_port_40to47;      /* 0xF190 */
    L7_uint8  empty10[0xF1D0 - 0xF191];
    L7_uint8  sfp_txdisable[6];     /* 0xF1D0..0xF1D5 */
    L7_uint8  empty11[PTIN_FPGA_MAP_SIZE - 0xF1D6];
  } reg;
} st_fpga_map_t;

#ifdef MAP_FPGA
extern volatile st_fpga_map_t *fpga_map;
#endif

# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
#ifdef MAP_CPLD
/* Depends of active matrix */
# define IPC_SERVER_IPADDR             ((cpld_map->reg.slot_matrix & 1) ? IPC_SERVER_IPADDR_WORKING : IPC_SERVER_IPADDR_PROTECTION)
#else
# define IPC_SERVER_IPADDR             0xC0A8C865  /* 192.168.200.101 Hardcoded!!! */
#endif

// MX IP address
# define IPC_MX_IPADDR_WORKING      0xC0A8C801  /* 192.168.200.1: Working Matrix */
# define IPC_MX_IPADDR_PROTECTION   0xC0A8C802  /* 192.168.200.2: Protection Matrix */
#ifdef MAP_CPLD
# define IPC_MX_IPADDR              ((cpld_map->reg.slot_matrix & 1) ? IPC_MX_IPADDR_WORKING : IPC_MX_IPADDR_PROTECTION)
#else
# define IPC_MX_IPADDR              IPC_MX_IPADDR_WORKING  /* Default ip address */
#endif

#endif /* _PTIN_GLOBALDEFS_TA48GE_H */

