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
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_ETH
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_UPLINK_INTERF   (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_ETH)
# define PTIN_SYSTEM_N_VPORTS          96

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

#if 1

#define BOARD_CONFIG_MODE ptin_env_board_config_mode_get()

/* AE48GEA - Single mode
Backplane | Virtual ports | Sysintf
----------+---------------+-------- 
        0 |          0-11 |  0-5 , 24-29
        1 |         12-23 |  6-11, 30-35
        2 |         24-35 | 12-17, 36-41
        3 |         36-47 | 18-23, 42-47
        4 |          ---  | Not mapped
        5 |          ---  | Not mapped
        6 |          ---  | Not mapped
        7 |          ---  | Not mapped
----------+---------------+-------- 
*/ 

/* Mapping from sysintf (id) to virtual port index (for AE48GE) */
#define SYSINTF_TO_VPORT_MAP_AE48GEA    \
  { \
    /* Single mode */ \
    {  0,  1,  2,  3,  4,  5,           /* 1G */ \
      12, 13, 14, 15, 16, 17,           /* 1G */ \
      24, 25, 26, 27, 28, 29,           /* 1G */ \
      36, 37, 38, 39, 40, 41,           /* 1G */ \
       6,  7,  8,  9, 10, 11,           /* 10G */ \
      18, 19, 20, 21, 22, 23,           /* 10G */ \
      30, 31, 32, 33, 34, 35,           /* 10G */ \
      42, 43, 44, 45, 46, 47            /* 10G */ \
    }, \
  }

#define BACKPLANE_INTLAG_MAP_AE48GEA \
  { \
    { 48 /*1w*/, 56 /*1p*/ },  /* LAG 0 */ \
    { 49 /*2w*/, 57 /*2p*/ },  /* LAG 1 */ \
    { 50 /*3w*/, 58 /*3p*/ },  /* LAG 2 */ \
    { 51 /*4w*/, 59 /*4p*/ },  /* LAG 3 */ \
    { 52 /*5w*/, 60 /*5p*/ },  /* LAG 4 */ \
    { 53 /*6w*/, 61 /*6p*/ },  /* LAG 5 */ \
    { 54 /*7w*/, 62 /*7p*/ },  /* LAG 6 */ \
    { 55 /*8w*/, 63 /*8p*/ },  /* LAG 7 */ \
  }

#define AE48GEA_SYSINTF_TO_VPORT(sysintf)   sysintf_to_vport_map_ae48gea[0][sysintf]
#define AE48GEA_VPORT_TO_INTLAG(vport)      ((vport)/12)
#define AE48GEA_SYSINTF_TO_INTLAG(sysintf)  AE48GEA_VPORT_TO_INTLAG(AE48GEA_SYSINTF_TO_VPORT(sysintf))
#define AE48GEA_BACKPLANE_INTLAGS_MAX       sizeof(backplane_intlag_map_ae48gea)/sizeof(backplane_intlag_map_ae48gea[0])

extern int sysintf_to_vport_map_ae48gea[][PTIN_SYSTEM_N_ETH];
extern int backplane_intlag_map_ae48gea[8][2];

/* AE48GEv2 - Mode 1
Working side:
Backplane | Virtual ports | Sysintf 
----------+---------------+-------- 
        0 |          0-11 | 36-47
        1 |         12-23 | 24-35
        2 |         24-35 | Empty (Only for AE48GEv2)
        3 |         36-47 | Empty (Only for AE48GEv2)
        4 |         48-59 | Not mapped
        5 |         60-71 | Not mapped
        6 |         72-83 | 12-23
        7 |         84-95 |  0-11
----------+---------------+--------
Protection side:
Backplane | Virtual ports | Sysintf 
----------+---------------+--------
        0 |          0-11 | Empty (Only for AE48GEv2)
        1 |         12-23 | Empty (Only for AE48GEv2)
        2 |         24-35 | 36-47
        3 |         36-47 | 24-35
        4 |         48-59 | Not mapped
        5 |         60-71 | Not mapped
        6 |         72-83 | 12-23
        7 |         84-95 |  0-11
----------+---------------+--------
*/
/* AE48GEv2 - Mode 2
Working side
Backplane | Virtual ports | Sysintf
----------+---------------+--------
        0 |          0-11 | 12-15, 28-31, 44, 46
        1 |         12-23 |  8-11, 24-27, 40, 42
        2 |         24-35 | Empty (Only for AE48GEv2)
        3 |         36-47 | Empty (Only for AE48GEv2)
        4 |         48-59 |  6-7 , 22-23, 38
        5 |         60-71 |  4-5 , 20-21, 36
        6 |         72-83 |  2-3 , 18-19, 34
        7 |         84-95 |  0-1 , 16-17, 32
----------+---------------+-------- 
Protection side:
Backplane | Virtual ports | Sysintf 
----------+---------------+-------- 
        0 |          0-11 | Empty (Only for AE48GEv2)
        1 |         12-23 | Empty (Only for AE48GEv2)
        2 |         24-35 | 12-15, 28-31, 44, 46
        3 |         36-47 |  8-11, 24-27, 40, 42
        4 |         48-59 |  6-7 , 22-23, 38
        5 |         60-71 |  4-5 , 20-21, 36
        6 |         72-83 |  2-3 , 18-19, 34
        7 |         84-95 |  0-1 , 16-17, 32
----------+---------------+-------- 
*/

/* Working Side follows OLT2T4 perspective. For OLT2T2 W and P are inverted */
#define SYSINTF_TO_VPORT_MAP_AE48GEv2 \
  { \
    /* Mode 0 */ \
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, \
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, \
      24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, \
      36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, \
    }, \
    /* Mode 1: for AE48GEv2 ports are mapped in reversed order */ \
    { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, \
      72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, \
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, \
       0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, \
    }, \
    /* Mode 2: for AE48GEv2 ports are mapped in reversed order */ \
    { 84, 85, 72, 73, 60, 61, 48, 49, \
      12, 13, 14, 15,  0,  1,  2,  3, \
      86, 87, 74, 75, 62, 63, 50, 51, \
      16, 17, 18, 19,  4,  5,  6,  7, \
      88, 89, 76, 77, 64, 65, 52, 53, \
      20, 21, 22, 23,  8,  9, 10, 11, \
    }, \
  }

#define BACKPLANE_INTLAG_MAP_AE48GEv2 \
  { \
    { 48 /*1w*/, 58 /*3p*/},  /* LAG 0 */ \
    { 49 /*2w*/, 59 /*4p*/},  /* LAG 1 */ \
    { 50 /*3w*/, 56 /*1p*/},  /* LAG 2 */ \
    { 51 /*4w*/, 57 /*2p*/},  /* LAG 3 */ \
    { 52 /*5w*/, 60 /*5p*/},  /* LAG 4 */ \
    { 53 /*6w*/, 61 /*6p*/},  /* LAG 5 */ \
    { 54 /*7w*/, 62 /*7p*/},  /* LAG 6 */ \
    { 55 /*8w*/, 63 /*8p*/},  /* LAG 7 */ \
  }

/* AE48GEv3 - Mode 1
Working/Protection sides:
Backplane | Virtual ports | Sysintf 
----------+---------------+-------- 
        0 |          0-7  |  0-7
        1 |          8-15 |  8-15
        2 |         16-23 | 16-23
        3 |         24-31 | 24-31
        4 |         32-39 | Not mapped
        5 |         40-47 | Not mapped
        6 |         48-54 | 32-39
        7 |         55-63 | 40-47
----------+---------------+--------
*/
/* AE48GEv3 - Mode 2
Working/Protection sides
Backplane | Virtual ports | Sysintf
----------+---------------+-------- 
        0 |          0-7  |  0-1 ,16-17,32
        1 |          8-15 |  2-3 ,18-19,34
        2 |         16-23 |  4-5 ,20-21,36
        3 |         24-31 |  6-7 ,22-23,38
        4 |         32-39 |  8-9 ,24-25,40
        5 |         40-47 | 10-11,26-27,42
        6 |         48-54 | 12-13,28-29,44
        7 |         55-63 | 14-15,30-31,46
----------+---------------+--------
*/

/* Working Side follows OLT2T4 perspective. For OLT2T2 W and P are inverted */
#define SYSINTF_TO_VPORT_MAP_AE48GEv3 \
  { \
    /* Mode 0 */ \
    {  0,  1,  2,  3,  4,  5,  6,  7, \
       8,  9, 10, 11, 12, 13, 14, 15, \
      16, 17, 18, 19, 20, 21, 22, 23, \
      24, 25, 26, 27, 28, 29, 30, 31, \
      32, 33, 34, 35, 36, 37, 38, 39, \
      40, 41, 42, 43, 44, 45, 46, 47, \
    }, \
    /* Mode 1: for AE48GEv3 ports are mapped in reversed order */ \
    {  0,  1,  2,  3,  4,  5,  6,  7, \
       8,  9, 10, 11, 12, 13, 14, 15, \
      16, 17, 18, 19, 20, 21, 22, 23, \
      24, 25, 26, 27, 28, 29, 30, 31, \
      48, 49, 50, 51, 52, 53, 54, 55, \
      56, 57, 58, 59, 60, 61, 62, 63, \
    }, \
    /* Mode 2: for AE48GEv3 ports are mapped in reversed order */ \
    {  0,  1,  8,  9, 16, 17, 24, 25, 32, 33, 40, 41, 48, 49, 56, 57, \
       2,  3, 10, 11, 18, 19, 26, 27, 34, 35, 42, 43, 50, 51, 58, 59, \
       4,  5, 12, 13, 20, 21, 28, 29, 36, 37, 44, 45, 52, 53, 60, 61, \
    }, \
  }

#define BACKPLANE_INTLAG_MAP_AE48GEv3 \
  { \
    { 48 /*1w*/, 56 /*3p*/},  /* LAG 0 */ \
    { 49 /*2w*/, 57 /*4p*/},  /* LAG 1 */ \
    { 50 /*3w*/, 58 /*1p*/},  /* LAG 2 */ \
    { 51 /*4w*/, 59 /*2p*/},  /* LAG 3 */ \
    { 52 /*5w*/, 60 /*5p*/},  /* LAG 4 */ \
    { 53 /*6w*/, 61 /*6p*/},  /* LAG 5 */ \
    { 54 /*7w*/, 62 /*7p*/},  /* LAG 6 */ \
    { 55 /*8w*/, 63 /*8p*/},  /* LAG 7 */ \
  }

/* Comment the bellow line to use AE48GEv3 */
//#define PTIN_BOARD_AE48GEv2_IN_USE  1

#define AE48GE_SYSINTF_TO_VPORT(sysintf)   sysintf_to_vport_map_ae48ge[BOARD_CONFIG_MODE][sysintf]
#ifdef PTIN_BOARD_AE48GEv2_IN_USE
#define AE48GE_VPORT_TO_INTLAG(vport)      ((vport)/12)
#else
#define AE48GE_VPORT_TO_INTLAG(vport)      ((vport)/8)
#endif
#define AE48GE_SYSINTF_TO_INTLAG(sysintf)  AE48GE_VPORT_TO_INTLAG(AE48GE_SYSINTF_TO_VPORT(sysintf))
#define AE48GE_BACKPLANE_INTLAGS_MAX       sizeof(backplane_intlag_map_ae48ge)/sizeof(backplane_intlag_map_ae48ge[0])

extern int sysintf_to_vport_map_ae48ge[][PTIN_SYSTEM_N_ETH];
extern int backplane_intlag_map_ae48ge[8][2];

#define SYSINTF_TO_VPORT(sysintf)   ((BOARD_CONFIG_MODE == 0) ? AE48GEA_SYSINTF_TO_VPORT(sysintf) : AE48GE_SYSINTF_TO_VPORT(sysintf))
#define VPORT_TO_INTLAG(vport)      ((BOARD_CONFIG_MODE == 0) ? AE48GEA_VPORT_TO_INTLAG(vport) : AE48GE_VPORT_TO_INTLAG(vport))
#define SYSINTF_TO_INTLAG(sysintf)  VPORT_TO_INTLAG(SYSINTF_TO_VPORT(sysintf))
#define BACKPLANE_INTLAGS_MAX       ((BOARD_CONFIG_MODE == 0) ? AE48GEA_BACKPLANE_INTLAGS_MAX : AE48GE_BACKPLANE_INTLAGS_MAX)
#define BACKPLANE_1ST_MEMBER(lag)   ((BOARD_CONFIG_MODE == 0) ? backplane_intlag_map_ae48gea[lag][0] : backplane_intlag_map_ae48ge[lag][0])
#define BACKPLANE_2ND_MEMBER(lag)   ((BOARD_CONFIG_MODE == 0) ? backplane_intlag_map_ae48gea[lag][1] : backplane_intlag_map_ae48ge[lag][1])
#else

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
#endif

#endif /* _PTIN_GLOBALDEFS_AE48GE_H */

