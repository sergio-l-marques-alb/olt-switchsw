#ifndef _PTIN_GLOBALDEFS_OLT1T0_H
#define _PTIN_GLOBALDEFS_OLT1T0_H

//#define PTIN_LINKFAULTS_IGNORE

#define UPLNK_PROT_DISABLE_JUST_TX

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED

#define __Y1731_802_1ag_OAM_ETH__
#define __MxP_FILTER__INSTEAD_OF__APS_AND_CCM__
#define APS_CCM_hapiBroadReceive_2_callback_SHORTCUT

#define PHY_RECOVERY_PROCEDURE  1
/** Service association AVL Tree */
#define IGMPASSOC_MULTI_MC_SUPPORTED
#define IGMP_QUERIER_IN_UC_EVC
#define IGMP_DYNAMIC_CLIENTS_SUPPORTED

/* Define for the ring configuration support */
#define ONE_MULTICAST_VLAN_RING_SUPPORT 1

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

#define PTIN_SYS_LC_SLOT_MIN        1
#define PTIN_SYS_LC_SLOT_MAX        1
#define PTIN_SYS_SLOTS_MAX          1
#define PTIN_SYS_INTFS_PER_SLOT_MAX 4

# define PTIN_SYSTEM_N_PONS            8 
# define PTIN_SYSTEM_N_ETH             8
# define PTIN_SYSTEM_N_UPLINK          PTIN_SYSTEM_N_ETH
# define PTIN_SYSTEM_N_INTERNAL        2
# define PTIN_SYSTEM_N_PORTS           (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_UPLINK + PTIN_SYSTEM_N_INTERNAL)
# define PTIN_SYSTEM_N_PORTS_PHYSICAL  PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PONS_PHYSICAL   PTIN_SYSTEM_N_PONS
# define PTIN_SYSTEM_N_ETH_PHYSICAL    PTIN_SYSTEM_N_ETH
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS_PHYSICAL
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_CLIENT_PORTS    (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_ETH)

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
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 2 - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES                  40    /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP           (L7_MAX_PORT_COUNT + L7_MAX_CPU_SLOTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 2)   /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_IGMP_MAXINTERFACES                PTIN_SYSTEM_N_CLIENT_PORTS                                                 /* Maximum nr of interfaces per multicast group */
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

/* The following constants will allow L2intf range separation, in order to allow
   specific QoS configurations for each range. */
#define L2INTF_ID_MAX   PTIN_SYSTEM_N_CLIENTS

#define SNOOP_PTIN_IGMPv3_GLOBAL  1   //Change to 0 if you want to globally disable IGMPv3 Module
#define SNOOP_PTIN_IGMPv3_ROUTER  1   //Change to 0 if you want to disable  IGMPv3 Router SubModule
#define SNOOP_PTIN_IGMPv3_PROXY   1   //Change to 0 if you want to disable IGMPv3 Proxy SubModule


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

