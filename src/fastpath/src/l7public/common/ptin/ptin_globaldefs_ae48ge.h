#ifndef _PTIN_GLOBALDEFS_AE48GE_H
#define _PTIN_GLOBALDEFS_AE48GE_H

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED
#define SHMEM_IS_IN_USE

/* Allows direct control over port add/remove of a LAG (shortcut to message queues) */
#define LAG_DIRECT_CONTROL_FEATURE  1

#define PHY_RECOVERY_PROCEDURE  1

//#define __Y1731_802_1ag_OAM_ETH__

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

# define PTIN_SYSTEM_N_PORTS           64
# define PTIN_SYSTEM_N_PONS            0
# define PTIN_SYSTEM_N_ETH             48
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_UPLINK_INTERF   (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_ETH)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000FFFFFFFFFFFFULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0x000F000000000000ULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

/* Internal VLAN for the first EVC */
#define PTIN_SYSTEM_BASE_INTERNAL_VLAN  100

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
# define PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX  (PTIN_VLAN_MAX)

#define PTIN_EVC_INBAND_SUPPORT           0

/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      0
# if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          1024   /*(((~PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK) & 0x3ff)+1)*/
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES                  40    /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP           (L7_MAX_PORT_COUNT + L7_MAX_CPU_SLOTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 2)   /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXINTERFACES                PTIN_SYSTEM_N_UPLINK_INTERF                                                 /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF             1     /* 1 client per frontal port */
# define PTIN_SYSTEM_IGMP_MAXONUS                      (PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*PTIN_SYSTEM_N_INTERF)
# define PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU           128     /* Settop boxes connected to ONUs */
# define PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF          PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU /*PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF*/
# define PTIN_SYSTEM_IGMP_MAXCLIENTS                   max (PTIN_SYSTEM_IGMP_MAXONUS, PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF*PTIN_SYSTEM_N_INTERF)
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

#define SNOOP_PTIN_MGMD_SUPPORT //Comment this line if you want to disable MGMD integration (not supported..)
#define SNOOP_PTIN_IGMPv3_GLOBAL 1//Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER 1//Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY 1//Change to 0 if you want to disable IGMPv3 Proxy SubModule


// IPC NETWORK IP address
#define PTIN_IPC_SUBNET_ID             0xC0A8C800  /* 192.168.200.0 Subnet Id*/
#define PTIN_IPC_IF_NAME               "eth0"

# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
/* Depends of active matrix */
# define IPC_SERVER_IPADDR             ((CPLD_SLOT_MATRIX_GET() & 1) ? IPC_SERVER_IPADDR_WORKING : IPC_SERVER_IPADDR_PROTECTION)

// MX IP address
# define IPC_MX_IPADDR_WORKING      0xC0A8C801  /* 192.168.200.1: Working Matrix */
# define IPC_MX_IPADDR_PROTECTION   0xC0A8C802  /* 192.168.200.2: Protection Matrix */
# define IPC_MX_IPADDR              ((CPLD_SLOT_MATRIX_GET() & 1) ? IPC_MX_IPADDR_WORKING : IPC_MX_IPADDR_PROTECTION)

/* Mapping from sysintf (id) to virtual port index (for AE48GE) */
#define SYSINTF_TO_VPORT_MAP_AE48GE     \
  {                                     \
    {  0,  1,  2,  3,  4,  5,           /* 1G */ \
      12, 13, 14, 15, 16, 17,           /* 1G */ \
      24, 25, 26, 27, 28, 29,           /* 1G */ \
      36, 37, 38, 39, 40, 41,           /* 1G */ \
       6,  7,  8,  9, 10, 11,           /* 10G */ \
      18, 19, 20, 21, 22, 23,           /* 10G */ \
      30, 31, 32, 33, 34, 35,           /* 10G */ \
      42, 43, 44, 45, 46, 47            /* 10G */ \
    },                                  \
    {  0,  1,  2,  3,  4,  5,  6,  7,   /* 1G */ \
       8,  9, 10, 11, 12, 13, 14, 15,   /* 1G */ \
      16, 17, 18, 19, 20, 21, 22, 23,   /* 1G */ \
      24, 25, 26, 27, 28, 29, 30, 31,   /* 1G */ \
      32, 33, 34, 35, 36, 37, 38, 39,   /* 1G */ \
      40, 41, 42, 43, 44, 45, 46, 47    /* 1G */ \
    },                                  \
    {  0,  1,  6,  7, 12, 13, 18, 19,   /* 1G */ \
      24, 25, 30, 31, 36, 37, 42, 43,   /* 1G */ \
       2,  3,  8,  9, 14, 15, 20, 21,   /* 10G/1G */ \
      26, 27, 32, 33, 38, 39, 44, 45,   /* 10G/1G */ \
       4,  5, 10, 11, 16, 17, 22, 23,   /* 10G */ \
      28, 29, 34, 35, 40, 41, 46, 47    /* 10G */ \
    }                                   \
  }

#define VPORT_TO_INTLAG_MAP_AE48GE      \
  {                                     \
    {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,   \
       5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,   \
       6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,   \
       7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7    \
    },                                  \
    {  0,  0,  0,  0,  0,  0,  0,  0,   \
       1,  1,  1,  1,  1,  1,  1,  1,   \
       4,  4,  4,  4,  4,  4,  4,  4,   \
       5,  5,  5,  5,  5,  5,  5,  5,   \
       6,  6,  6,  6,  6,  6,  6,  6,   \
       7,  7,  7,  7,  7,  7,  7,  7    \
    },                                  \
    {  0,  0,  0,  0,  0,  0,           \
       1,  1,  1,  1,  1,  1,           \
       2,  2,  2,  2,  2,  2,           \
       3,  3,  3,  3,  3,  3,           \
       4,  4,  4,  4,  4,  4,           \
       5,  5,  5,  5,  5,  5,           \
       6,  6,  6,  6,  6,  6,           \
       7,  7,  7,  7,  7,  7            \
    }                                   \
  }

#define SYSINTF_TO_VPORT(sysintf)   sysintf_to_vport_map_ae48ge[ptin_env_board_config_mode_get()][sysintf]
#define VPORT_TO_INTLAG(vport)      vport_to_intlag_map_ae48ge[ptin_env_board_config_mode_get()][vport]
#define SYSINTF_TO_INTLAG(sysintf)  VPORT_TO_INTLAG(SYSINTF_TO_VPORT(sysintf))
extern int sysintf_to_vport_map_ae48ge[][PTIN_SYSTEM_N_ETH];
extern int vport_to_intlag_map_ae48ge[][PTIN_SYSTEM_N_ETH];

#endif /* _PTIN_GLOBALDEFS_AE48GE_H */

