#ifndef _PTIN_GLOBALDEFS_OLT78CHB_H
#define _PTIN_GLOBALDEFS_OLT78CHB_H

/** Service association AVL Tree */
#define IGMPASSOC_MULTI_MC_SUPPORTED
#define IGMP_QUERIER_IN_UC_EVC
#define IGMP_DYNAMIC_CLIENTS_SUPPORTED

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_SYSTEM_N_PORTS           18
# define PTIN_SYSTEM_N_PONS            8 
# define PTIN_SYSTEM_N_ETH             0
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PRIO          1    /* MAC Learning priority (station move is allowed over same prio) */
# define PTIN_SYSTEM_UPLINK_PRIO       2    /* MAC Learning priority (station move is allowed over same prio) */

# define PTIN_SYSTEM_PON_PORTS_MASK    0x000000FF
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000FF00
# define PTIN_SYSTEM_10G_PORTS_MASK    0x00030000
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            2048  /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */
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
# define PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX  (PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN + PTIN_SYSTEM_EVC_BITSTREAM_VLAN_BLOCKS*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK - 1)


/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      1
# if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          1024   /*(((~PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK) & 0x3ff)+1)*/
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES               8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP        (L7_MAX_PORT_COUNT + L7_MAX_CPU_SLOTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 2)   /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXONUS                   512   /* 512 ONUs per IGMP instance */
# define PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU        8     /* Settop boxes connected to ONUs */
# define PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF                1024  /* Maximum number of settop-boxes */
# define PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP      5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE        (PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF/(sizeof(L7_uint32)*8)+1)  /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE           1     /* Maximum number of entries in Query queue */
# define PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT 0     /*This feature should be only enabled when this condition is verified !PTIN_BOARD_IS_MATRIX*/

# define PTIN_SYSTEM_N_DHCP_INSTANCES               8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES              8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_DHCP_MAXCLIENTS                512   /* Maximum DHCP clients */
# define PTIN_SYSTEM_PPPOE_MAXCLIENTS               512   /* Maximum PPPoE clients*/

#define SNOOP_PTIN_MGMD_SUPPORT //Comment this line if you want to disable MGMD integration (not supported..)
#define SNOOP_PTIN_MGMD_SUPPORT //Comment this line if you want to disable MGMD integration (not supported..)
#define SNOOP_PTIN_IGMPv3_GLOBAL  1   //Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER  1   //Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY   1   //Change to 0 if you want to disable IGMPv3 Proxy SubModule


# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR             IPC_LOCALHOST_IPADDR
# define IPC_MX_IPADDR                 0xC0A8C801  /* 192.168.200.101 Hardcoded!!! */

#endif /* _PTIN_GLOBALDEFS_OLT78CHB_H */

