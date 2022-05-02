#ifndef _PTIN_GLOBALDEFS_TC16SXG_H
#define _PTIN_GLOBALDEFS_TC16SXG_H

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED

/* Allows direct control over port add/remove of a LAG (shortcut to message queues) */
#define LAG_DIRECT_CONTROL_FEATURE  1

/* Activate this flag to select several ports using Inport+InterfaceClassId qualifiers, instead of Inports */
#define ICAP_INTERFACES_SELECTION_BY_CLASSPORT

/* Info about ASPEN devices */
#define TC16SXG_ASPEN_BRIDGE_SCRIPT "/bin/sh /usr/local/ptin/scripts/startAspenBridge.sh"  /* Only applicable to TC16SXG */

/* 
  ASPEN N:1 modes (4:1 or 2:1)
  In these modes, MAC GPON ASPEN muxes several PONs in a SWITCH<=>ASPEN
  physical interface
*/
#define PORT_VIRTUALIZATION_N_1     /* Port virtualization enabled */
#define PORT_VIRTUALIZATION_2_1     /* Port virtualization mode */

#define PTIN_MODE_MPM  0
#define PTIN_MODE_GPON 1

/* Special EVCs for this equipment */
#define PTIN_ASPEN2CPU_A_EVC         (PTIN_SYSTEM_EXT_EVCS_MGMT+0)     /* MACGPON-CPU EVC */
#define PTIN_ASPEN2CPU_B_EVC         (PTIN_SYSTEM_EXT_EVCS_MGMT+1)     /* MACGPON-CPU EVC */
/* Special VLANs for this equipment */
#define PTIN_ASPEN2CPU_A_VLAN           2044
#define PTIN_ASPEN2CPU_B_VLAN           2045
#define PTIN_ASPEN2CPU_A_VLAN_EXT       100
#define PTIN_ASPEN2CPU_B_VLAN_EXT       100
#define PTIN_ASPEN2CPU_A_SMAC           { 0x00, 0x10, 0x18, 0x00, 0x00, 0x00 }
#define PTIN_ASPEN2CPU_B_SMAC           { 0x00, 0x10, 0x18, 0x00, 0x00, 0x01 }
#define PTIN_ASPEN2CPU_A_INTIFNUM       1
#define PTIN_ASPEN2CPU_B_INTIFNUM       9

#define RATE_LIMIT_ASPEN    4096
#define BUCKET_SIZE_ASPEN   1024


/* Special ports for this equipment */
#define PTIN_PORT_CPU                 (PTIN_SYSTEM_N_PORTS-1)


/** Service association AVL Tree */
#define IGMPASSOC_MULTI_MC_SUPPORTED
#define IGMP_QUERIER_IN_UC_EVC
#define IGMP_DYNAMIC_CLIENTS_SUPPORTED
#define IGMP_SMART_MC_EVC_SUPPORTED /* SFR service model.  Creation/removal of L3 Interfaces on MC-IPTV Services */

/* TAP settings */
#define PTIN_PHY_LC2CXO_1T1W_PRE    0
#define PTIN_PHY_LC2CXO_1T1W_MAIN   50
#define PTIN_PHY_LC2CXO_1T1W_POST   10
#define PTIN_PHY_LC2CXO_1T1P_PRE    PTIN_PHY_LC2CXO_1T1W_PRE
#define PTIN_PHY_LC2CXO_1T1P_MAIN   PTIN_PHY_LC2CXO_1T1W_MAIN
#define PTIN_PHY_LC2CXO_1T1P_POST   PTIN_PHY_LC2CXO_1T1W_POST

#define PTIN_PHY_LC2CXO_1T3W_PRE    0
#define PTIN_PHY_LC2CXO_1T3W_MAIN   50
#define PTIN_PHY_LC2CXO_1T3W_POST   10
#define PTIN_PHY_LC2CXO_1T3P_PRE    PTIN_PHY_LC2CXO_1T3W_PRE
#define PTIN_PHY_LC2CXO_1T3P_MAIN   PTIN_PHY_LC2CXO_1T3W_MAIN
#define PTIN_PHY_LC2CXO_1T3P_POST   PTIN_PHY_LC2CXO_1T3W_POST

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2
#define PTIN_SYS_MX1_SLOT           (PTIN_SYS_LC_SLOT_MIN-1)
#define PTIN_SYS_MX2_SLOT           (PTIN_SYS_LC_SLOT_MAX+1)
# define PTIN_SLOT_WORK             0
# define PTIN_SLOT_PROT             1

# define PTIN_SYSTEM_N_PONS            32   /* (Trident3-X3) */
# define PTIN_SYSTEM_N_ETH             0
# define PTIN_SYSTEM_N_UPLINK          16
# define PTIN_SYSTEM_N_INTERNAL        1
# define PTIN_SYSTEM_N_PORTS           (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_UPLINK + PTIN_SYSTEM_N_INTERNAL)
# define PTIN_SYSTEM_N_PONS_PHYSICAL   16
# define PTIN_SYSTEM_N_ETH_PHYSICAL    0
# define PTIN_SYSTEM_N_PORTS_PHYSICAL  (PTIN_SYSTEM_N_PONS_PHYSICAL + PTIN_SYSTEM_N_UPLINK + PTIN_SYSTEM_N_INTERNAL)
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS_PHYSICAL
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_CLIENT_PORTS    (PTIN_SYSTEM_N_PONS + PTIN_SYSTEM_N_ETH)

/* These are switch-physical ports (not ptin_ports): only to be used at HAPI layer */
# define PTIN_SYSTEM_PON_PORTS_MASK    ((1ULL<<PTIN_SYSTEM_N_PONS_PHYSICAL)-1)   /*0xFFFFFFFF*/
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x00000000
# define PTIN_SYSTEM_10G_PORTS_MASK    (0xFFFFULL<<PTIN_SYSTEM_N_PONS_PHYSICAL) /* (Trident3-X3) FIXME 33? 49?*/
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            4002  /* Maximum nr of EVCs allowed in this equipment */
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

#define PTIN_EVC_INBAND_SUPPORT           0

/* For QUATTRO P2P services */
# define PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      1
# if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
#  define PTIN_SYSTEM_EVC_QUATTRO_VLANS          2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN       2048
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX      (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN + PTIN_SYSTEM_EVC_QUATTRO_VLANS - 2 - 1)
#  define PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK     (~(PTIN_SYSTEM_EVC_QUATTRO_VLANS-1) & 0xfff) /*0x0c00*/
# endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES                  80     /* Maximum nr of IGMP instances */
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
# define PTIN_SYSTEM_IGMP_EVC_MC_OFFSET                600   /* on TC16SXG due to virtualization exists one EVC for GPON MC and other for XGS. 
                                                                they have a offset of 600. The constant is use because channels on AVL have only one entry*/
                                                      
# define PTIN_SYSTEM_N_DHCP_INSTANCES                  32     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES                 32     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_DHCP_MAXCLIENTS                   8192  /* Maximum DHCP clients */
# define PTIN_SYSTEM_PPPOE_MAXCLIENTS                  8192  /* Maximum PPPoE clients */

/* The following constants will allow L2intf range separation, in order to allow
   specific QoS configurations for each range.
   For TC16SXG board, 2 ranges will be defined with the first applied to GPON ports,
   and the second for XGSPON ports */
#define L2INTF_ID_MAX           PTIN_SYSTEM_N_CLIENTS
#define L2INTF_QUEUES_NUMBER    2

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

#endif /* _PTIN_GLOBALDEFS_TC16SXG_H */

