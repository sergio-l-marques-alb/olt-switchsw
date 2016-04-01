#ifndef _PTIN_GLOBALDEFS_OLT1T0_H
#define _PTIN_GLOBALDEFS_OLT1T0_H

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED

#define __Y1731_802_1ag_OAM_ETH__
//#define __APS_AND_CCM_COMMON_FILTER__     //can't define this; most likely we aren't being allowed to add BROAD_FIELD_IP6_DST to group existent qualifiers
#define __CCM_FILTER__MEANS__MEP_FILTER__
#define COMMON_APS_CCM_CALLBACKS__ETYPE_REG
//#define USING_SDK_OAM_FP_CREATE

#define PHY_RECOVERY_PROCEDURE  1
/** Service association AVL Tree */
#define IGMPASSOC_MULTI_MC_SUPPORTED
#define IGMP_QUERIER_IN_UC_EVC
#define IGMP_DYNAMIC_CLIENTS_SUPPORTED

/* Special EVCs for this equipment */
#define PTIN_EVC_BL2CPU         (PTIN_SYSTEM_EXT_EVCS_MGMT+0)     /* MACGPON-CPU EVC */
#define PTIN_EVC_FPGA2CPU       (PTIN_SYSTEM_EXT_EVCS_MGMT+1)     /* FPGA-CPU EVC */
#define PTIN_EVC_FPGA2PORTS_MIN (PTIN_SYSTEM_EXT_EVCS_MGMT+2+0)   /* Front ports - FPGA EVC (one for each port) */
#define PTIN_EVC_FPGA2PORTS_MAX (PTIN_SYSTEM_EXT_EVCS_MGMT+2+31)

/* Special VLANs for this equipment */
#define PTIN_VLAN_FPGA2PORT_MIN       2016
#define PTIN_VLAN_FPGA2PORT_MAX       2031
#define PTIN_VLAN_FPGA2CPU            2045
#define PTIN_VLAN_BL2CPU              2046
#define PTIN_VLAN_BL2CPU_EXT          400

/* Special ports for this equipment */
#define PTIN_PORT_FPGA                (PTIN_SYSTEM_N_PORTS-2)
#define PTIN_PORT_CPU                 (PTIN_SYSTEM_N_PORTS-1)

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_SYSTEM_N_PORTS           18
# define PTIN_SYSTEM_N_PONS            8 
# define PTIN_SYSTEM_N_ETH             8
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_UPLINK_INTERF   (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_ETH)

# define PTIN_SYSTEM_PROTECTION_LAGID_BASE  10

/* Allows direct control over port add/remove of a LAG (shortcut to message queues) */
#define LAG_DIRECT_CONTROL_FEATURE  1

# define PTIN_SYSTEM_PON_PRIO          1    /* MAC Learning priority (station move is allowed over same prio) */
# define PTIN_SYSTEM_UPLINK_PRIO       2    /* MAC Learning priority (station move is allowed over same prio) */

# define PTIN_SYSTEM_PON_PORTS_MASK         0x000000FF
# define PTIN_SYSTEM_ETH_PORTS_MASK         0x0000FF00
# define PTIN_SYSTEM_10G_PORTS_MASK         0x0000F000
# define PTIN_SYSTEM_BL_INBAND_PORT_MASK    0x00010000
# define PTIN_SYSTEM_PORTS_MASK             (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            4065  /* Maximum nr of EVCs allowed in this equipment */
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

#define PTIN_EVC_INBAND_SUPPORT           1

/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      1
# if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          1024   /*(((~PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK) & 0x3ff)+1)*/
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES                  40    /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP           (L7_MAX_PORT_COUNT + L7_MAX_CPU_SLOTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 2)   /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXINTERFACES                PTIN_SYSTEM_N_UPLINK_INTERF                                                 /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF             128   /* 128 ONUs per port */
# define PTIN_SYSTEM_IGMP_MAXONUS                      (PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*PTIN_SYSTEM_N_INTERF)
# define PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU           8     /* Settop boxes connected to ONUs */
# define PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF          PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF
# define PTIN_SYSTEM_IGMP_MAXCLIENTS                   PTIN_SYSTEM_IGMP_MAXONUS
# define PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP         5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE           (PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF/(sizeof(L7_uint32)*8)+1)  /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE              1     /* Maximum number of entries in Query queue */
# define PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT    1     /*This feature should be only enabled when this condition is verified !PTIN_BOARD_IS_MATRIX*/
# define PTIN_SYSTEM_IGMP_MAXPACKAGES                  256   /*Max Multicast Packages Per Card*/    
# define PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES  32768 /*Max Packages Channel Entries */   
# define PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD         1     /*Support L3 Multicast Forwarding*/
# define PTIN_SYSTEM_IGMP_L3_MULTICAST_MAX_ENTRIES     4096  /*Max Number of L3 Multicast Entries*/
# define PTIN_SYSTEM_IGMP_GROUP_LIST_MAX_ENTRIES       16384 /*Max Number of Group List Entries*/
                                                      
# define PTIN_SYSTEM_N_DHCP_INSTANCES                  32    /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES                 32    /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_DHCP_MAXCLIENTS                   8192  /* Maximum DHCP clients */
# define PTIN_SYSTEM_PPPOE_MAXCLIENTS                  8192  /* Maximum PPPoE clients */

#define SNOOP_PTIN_IGMPv3_GLOBAL  1   //Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER  1   //Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY   1   //Change to 0 if you want to disable IGMPv3 Proxy SubModule

/* FPGA AND CPLD BASE ADDRESS */
//# define MAP_FPGA
//# define MAP_CPLD
# define FPGA_BASE_ADDR                0xFF200000
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1195
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_HW_ID_REG                0x0008
# define CPLD_CHASSIS_ID_REG           0x0006
# define CPLD_SLOT_ID_REG              0x0050

typedef union
{
  L7_uint8 map[PTIN_CPLD_MAP_SIZE];
  struct
  {
    L7_uint16 id;               /* 0x00 (2 bytes) */
    L7_uint8  ver;              /* 0x02 (1 byte)  */
    L7_uint8  empty0[0x06 - 0x03];
    L7_uint8  chassis_id;       /* 0x06 (1 byte)  */
    L7_uint8  empty1[0x08 - 0x07];
    L7_uint8  hw_id;            /* 0x08 (1 byte)  */
    L7_uint8  hw_ver;           /* 0x09 (1 byte)  */
    L7_uint8  empty2[0x1E - 0x0A];
    L7_uint8  slot_matrix;      /* 0x1e (1 byte) */
    L7_uint8  empty3[0x50 - 0x1F];
    L7_uint8  slot_id;          /* 0x50 (1 byte)  */
    L7_uint8  empty4[PTIN_CPLD_MAP_SIZE - 0x51];
  } reg;
} st_cpld_map_t;

#ifdef MAP_CPLD
extern volatile st_cpld_map_t *cpld_map;
#endif

/* FPGA map registers */
# define FPGA_ID                       0x1112
# define FPGA_ID0_REG                  0x0005 /* Address is swapped! */
# define FPGA_ID1_REG                  0x0004 /* Address is swapped! */
# define FPGA_VER_REG                  0x0006
# define FPGA_TXDISABLE_REG            0xF061
# define FPGA_EXTPHY_RESET             0xF080

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0x0004 - 0x0000];
    L7_uint8  id1;                  /* 0x0004 (1 bytes) */
    L7_uint8  id0;                  /* 0x0005 (1 bytes) */
    L7_uint8  ver;                  /* 0x0006 (1 byte)  */
    L7_uint8  empty1[0xF061 - 0x0007];
    L7_uint8  tx_disable;           /* 0xF061 (1 byte)  */
    L7_uint8  empty2[0xF080 - 0xF062];
    L7_uint8  extPhy_reset;         /* 0xF080 (1 byte)  */
    L7_uint8  empty3[PTIN_FPGA_MAP_SIZE - 0xF081];
  } reg;
} st_fpga_map_t;

#ifdef MAP_FPGA
extern volatile st_fpga_map_t *fpga_map;
#endif

// IPC NETWORK IP address
#define PTIN_IPC_SUBNET_ID             0xC0A8C800  /* 192.168.200.0 Subnet Id*/
#define PTIN_IPC_IF_NAME               "eth1:1"

# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR             IPC_LOCALHOST_IPADDR
# define IPC_MX_IPADDR                 0xC0A8C801  /* 192.168.200.101 Hardcoded!!! */

// MX IP address
# define IPC_MX_IPADDR_WORKING      0xC0A8C801  /* 192.168.200.1: Working Matrix */
# define IPC_MX_IPADDR_PROTECTION   0xC0A8C802  /* 192.168.200.2: Protection Matrix */

#endif /* _PTIN_GLOBALDEFS_OLT1T0_H */

