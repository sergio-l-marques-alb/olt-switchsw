#ifndef _PTIN_GLOBALDEFS_H
#define _PTIN_GLOBALDEFS_H

#include "l7_platformspecs.h"

/* ************************** */
/* Global Features Activation */

#if PTIN_BOARD_IS_MATRIX
  #define PTIN_ENABLE_ERPS
#endif  // PTIN_BOARD_IS_MATRIX

/* ************************** */


#if L7_FEAT_LAG_PRECREATE != 0
#error L7_FEAT_LAG_PRECREATE must be set to 0! (feature.h)
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

/* Global to all platforms */
#define PTIN_SYSTEM_MAX_N_FULLSLOTS   18
#define PTIN_SYSTEM_MAX_N_PORTS       36
#define PTIN_SYSTEM_MAX_N_LAGS        PTIN_SYSTEM_MAX_N_PORTS
#define PTIN_SYSTEM_MTU_SIZE          2048

#define PTIN_IS_PORT_PON(p)           ((((unsigned long long)1 << p) & PTIN_SYSTEM_PON_PORTS_MASK) != 0)
#define PTIN_IS_PORT_ETH(p)           ((((unsigned long long)1 << p) & PTIN_SYSTEM_ETH_PORTS_MASK) != 0)
#define PTIN_IS_PORT_10G(p)           ((((unsigned long long)1 << p) & PTIN_SYSTEM_10G_PORTS_MASK) != 0)

#define PTIN_TPID_OUTER_DEFAULT       0x8100
//#define PTIN_TPID_INNER_DEFAULT       0x8100 NOT USED!

#define PTIN_CPLD_MAP_SIZE            0x00FF
#define PTIN_FPGA_MAP_SIZE            0xFFFF
#define FPGA_ID0_VAL                  ((FPGA_ID >> 8) & 0xFF)
#define FPGA_ID1_VAL                  (FPGA_ID & 0xFF)
#define CPLD_ID0_VAL                  ((CPLD_ID >> 8) & 0xFF)
#define CPLD_ID1_VAL                  (CPLD_ID & 0xFF)

#define PTIN_INBAND_BRIDGE_SCRIPT     "/bin/sh /usr/local/ptin/scripts/startBridge.sh"  /* Only applicable to CXP360G */

#define PTIN_SYSTEM_GROUP_VLANS  1

/* OLT7-8CH */
#if (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_SYSTEM_N_PORTS           18
# define PTIN_SYSTEM_N_PONS            8 
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

# define PTIN_SYSTEM_N_EVCS            65    /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK    32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS    4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS  (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN       PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX      (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN     (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX     (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP      L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */
# define PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP    5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE   PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8)     /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE         100   /* Maximum number of entries in Query queue */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_FPGA
# define MAP_CPLD
# define FPGA_BASE_ADDR                0xFF200000
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
//# define CPLD_ID                       0x0849
//# define CPLD_ID0_REG                  0x0005
//# define CPLD_ID1_REG                  0x0004
//# define CPLD_VER_REG                  0x0006
//# define CPLD_HW_ID_REG                0x0008
//# define CPLD_CHASSIS_ID_REG           0x0006
//# define CPLD_SLOT_ID_REG              0x0050

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

extern volatile st_cpld_map_t *cpld_map;

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

extern volatile st_fpga_map_t *fpga_map;

# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR             IPC_LOCALHOST_IPADDR


/* TOLT8G */
#elif (PTIN_BOARD == PTIN_BOARD_TOLT8G)

/** Service association AVL Tree */
//#define IGMPASSOC_MULTI_MC_SUPPORTED
//#define IGMP_QUERIER_IN_UC_EVC

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_SYSTEM_N_PORTS           12
# define PTIN_SYSTEM_N_PONS            8 
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x000000FF
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x00000000
# define PTIN_SYSTEM_10G_PORTS_MASK    0x00000F00
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            65    /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK         32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS         4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS       (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN            PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX           (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN          (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX          (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP      L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */
# define PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP    5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE   PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8)     /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE         100   /* Maximum number of entries in Query queue */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE 512 

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_FPGA
# define MAP_CPLD
# define FPGA_BASE_ADDR                0xFF200000
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1194
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

    L7_uint8  empty2[0x1E - 0x10];
    L7_uint8  slot_matrix;      /* 0x1e (1 byte) */
    L7_uint8  empty4[PTIN_CPLD_MAP_SIZE - 0x1f];
  } reg;
} st_cpld_map_t;

extern volatile st_cpld_map_t *cpld_map;

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

extern volatile st_fpga_map_t *fpga_map;

//# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR             0xC0A8C865  /* 192.168.200.101 Hardcoded!!! */


/* TG16G */
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)

/** Service association AVL Tree */
//#define IGMPASSOC_MULTI_MC_SUPPORTED
//#define IGMP_QUERIER_IN_UC_EVC

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_SYSTEM_N_PORTS           20
# define PTIN_SYSTEM_N_PONS            16 
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   0
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000FFFF
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x00000000
# define PTIN_SYSTEM_10G_PORTS_MASK    0x000F0000
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            65    /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK    32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS    4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS  (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN       PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX      (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN     (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX     (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP      L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */
# define PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP    5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE   PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8)     /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE         100   /* Maximum number of entries in Query queue */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE 512

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_FPGA
# define MAP_CPLD
# define FPGA_BASE_ADDR                0xFF200000
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

    L7_uint8  empty2[0x1E - 0x10];
    L7_uint8  slot_matrix;      /* 0x1e (1 byte) */
    L7_uint8  empty4[PTIN_CPLD_MAP_SIZE - 0x1f];
  } reg;
} st_cpld_map_t;

extern volatile st_cpld_map_t *cpld_map;

/* FPGA map registers */
# define FPGA_ID                       0x1256
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
    L7_uint8  empty1[0xF061 - 0x0003];
    L7_uint8  tx_disable;           /* 0xF061 (1 byte)  */
    L7_uint8  empty2[0xF080 - 0xF062];
    L7_uint8  extPhy_reset;         /* 0xF080 (1 byte)  */
    L7_uint8  empty3[PTIN_FPGA_MAP_SIZE - 0xF081];
  } reg;
} st_fpga_map_t;

extern volatile st_fpga_map_t *fpga_map;

//# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR             0xC0A8C865  /* 192.168.200.101 Hardcoded!!! */


/* OLT360 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED

#define PTIN_PTP_PORT               41  /* PTP interface port nr (xe40) */

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 2

# define PTIN_PORTMAP_SLOT_WORK { \
   2,  1, 16, 15, 14, 13,  8,  7, \
   6,  5, 20, 19, 18, 17, 12, 11, \
  10,  9, 32, 31, 30, 29, 24, 23, \
  22, 21, 35, 36, 34, 33, 28, 27, \
  26, 25, 40, 39, 41 /* PTP */,   \
   3,  4, 37, 38  \
}
# define PTIN_PORTMAP_SLOT_PROT { \
   9, 10, 11, 12, 17, 18, 19, 20, \
   5,  6,  7,  8, 13, 14, 15, 16, \
   1,  2, 39, 40, 25, 26, 27, 28, \
  33, 34, 36, 35, 21, 22, 23, 24, \
  29, 30, 31, 32, 41 /* PTP */,   \
   3,  4, 37, 38  \
}
# define PTIN_SLOT_WORK                0
# define PTIN_SLOT_PROT                1

# define PTIN_SYSTEM_N_PORTS           (36+1)
# define PTIN_SYSTEM_N_PONS            0 
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   ((PTIN_SYSTEM_N_PORTS-1)/2-1)
# define PTIN_SYSTEM_N_LAGS            (PTIN_SYSTEM_N_PORTS-1)
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0x0000001FFFFFFFFFULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            129   /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK    32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS    4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS  (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN       PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX      (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN     (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX     (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP      L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */
# define PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP    5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE   PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8)     /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE         100   /* Maximum number of entries in Query queue */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE 512

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_CPLD
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1165
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
    L7_uint8  empty3[PTIN_CPLD_MAP_SIZE - 0x10];
  } reg;
} st_cpld_map_t;

extern volatile st_cpld_map_t *cpld_map;

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0xffff - 0x0000];
  } reg;
} st_fpga_map_t;

extern volatile st_fpga_map_t *fpga_map;

//# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
# define IPC_SERVER_IPADDR             IPC_SERVER_IPADDR_WORKING  /* Default ip address */


/* OLT1T3 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G_V1 || PTIN_BOARD == PTIN_BOARD_CXO640G)

/* If SSM is not supported, comment this line */
#define SYNC_SSM_IS_SUPPORTED
#define PTIN_PTP_PORT     56  /* PTP interface port nr (xe56) */


# define PTIN_PHY_PREEMPHASIS_DEFAULT         0xBF00 /* main=48 post=15 */
# define PTIN_PHY_PREEMPHASIS_FARTHEST_SLOTS  0xCEC0 /* main=44 post=19 */
# define PTIN_PHY_PREEMPHASIS_NEAREST_SLOTS   0xB720 /* main=50 post=13 */

#define PTIN_SYS_LC_SLOT_MIN        2
#define PTIN_SYS_LC_SLOT_MAX        19
#define PTIN_SYS_SLOTS_MAX          20
#define PTIN_SYS_INTFS_PER_SLOT_MAX 4

/* Slot/port TO to interface map */
/* CXO640G v1 */
# define PTIN_SLOTPORT_TO_INTF_MAP  \
          { { -1, -1, -1, -1 }, \
            { -1, -1, -1, -1 }, \
            { -1, -1,  0,  1 }, \
            { -1, -1,  2,  3 }, \
            {  4,  5,  6,  7 }, \
            {  8,  9, 10, 11 }, \
            { -1, -1, 12, 13 }, \
            { -1, -1, 14, 15 }, \
            { 16, 17, 18, 19 }, \
            { 20, 21, 22, 23 }, \
            { 24, 25, 26, 27 }, \
            { 28, 29, 30, 31 }, \
            { 32, 33, 34, 35 }, \
            { 36, 37, 38, 39 }, \
            { -1, -1, 40, 41 }, \
            { -1, -1, 42, 43 }, \
            { 44, 45, 46, 47 }, \
            { 48, 49, 50, 51 }, \
            { -1, -1, 52, 53 }, \
            { -1, -1, 54, 55 } }
# define PTIN_SLOTPORT_TO_INTF_MAP_PROT  \
          { { -1, -1, -1, -1 }, \
            { -1, -1, -1, -1 }, \
            { -1, -1, 54, 55 }, \
            { -1, -1, 52, 53 }, \
            { 48, 49, 50, 51 }, \
            { 44, 45, 46, 47 }, \
            { -1, -1, 42, 43 }, \
            { -1, -1, 40, 41 }, \
            { 36, 37, 38, 39 }, \
            { 32, 33, 34, 35 }, \
            { 28, 29, 30, 31 }, \
            { 24, 25, 26, 27 }, \
            { 20, 21, 22, 23 }, \
            { 16, 17, 18, 19 }, \
            { -1, -1, 14, 15 }, \
            { -1, -1, 12, 13 }, \
            {  8,  9, 10, 11 }, \
            {  4,  5,  6,  7 }, \
            { -1, -1,  2,  3 }, \
            { -1, -1,  0,  1 } }

/* Interface to slot map */
/* CXO640G v1 */
# define PTIN_INTF_TO_SLOT_MAP  \
          {  2,  2,  3,  3, \
             4,  4,  4,  4, \
             5,  5,  5,  5, \
             6,  6,  7,  7, \
             8,  8,  8,  8, \
             9,  9,  9,  9, \
            10, 10, 10, 10, \
            11, 11, 11, 11, \
            12, 12, 12, 12, \
            13, 13, 13, 13, \
            14, 14, 15, 15, \
            16, 16, 16, 16, \
            17, 17, 17, 17, \
            18, 18, 19, 19, \
            -1, -1, -1, -1, \
            -1, -1, -1, -1  }

/* Interface to port map */
# define PTIN_INTF_TO_PORT_MAP  \
          {  0,  1,  0,  1, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  0,  1, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  0,  1, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  0,  1, \
            -1, -1, -1, -1, \
            -1, -1, -1, -1  }

/* CXO640G v1 */
# define PTIN_PORTMAP_SLOT_WORK { \
   1,  2,  3,  4,  5,  6,  7,  8, \
   9, 10, 11, 12, 13, 14, 15, 16, \
  17, 18, 19, 20, 21, 22, 23, 24, \
  25, 26, 27, 28, 29, 30, 31, 32, \
  33, 34, 35, 36, 37, 38, 39, 40, \
  41, 42, 43, 44, 45, 46, 47, 48, \
  49, 50, 51, 52, 53, 54, 55, 56, \
  57, 58, 59, 60, 61, 62, 63, 64, \
}
# define PTIN_PORTMAP_SLOT_PROT { \
  55, 56, 53, 54, 49, 50, 51, 52, \
  45, 46, 47, 48, 43, 44, 41, 42, \
  37, 38, 39, 40, 33, 34, 35, 36, \
  29, 30, 31, 32, 25, 26, 27, 28, \
  21, 22, 23, 24, 17, 18, 19, 20, \
  15, 16, 13, 14,  9, 10, 11, 12, \
   5,  6,  7,  8,  3,  4,  1,  2, \
  57, 58, 59, 60, 61, 62, 63, 64, \
}

# define PTIN_SLOT_WORK                0
# define PTIN_SLOT_PROT                1

# define PTIN_SYSTEM_N_PORTS           64
# define PTIN_SYSTEM_N_PONS            0 
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   (PTIN_SYSTEM_N_PORTS/2-1)
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0xFFFFFFFFFFFFFFFFULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            129   /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK    32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS    4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS  (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN       PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX      (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN     (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX     (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXINTERFACES_PER_GROUP      L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF /* Maximum nr of interfaces per multicast group */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */
# define PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP    5     /* Maximum number of sources per multicast/interface group */
# define PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE   PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8)     /* Maximum number of clientes per source */
# define PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE         100   /* Maximum number of entries in Query queue */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE 512

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
    L7_uint8  empty3[PTIN_CPLD_MAP_SIZE - 0x10];
  } reg;
} st_cpld_map_t;

extern volatile st_cpld_map_t *cpld_map;

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0xffff - 0x0000];
  } reg;
} st_fpga_map_t;

extern volatile st_fpga_map_t *fpga_map;

//# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
# define IPC_SERVER_IPADDR             IPC_SERVER_IPADDR_WORKING  /* Default ip address */

/* OLT1T3 Matrix card V2 */
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G_V2)

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

/* Slot/port TO to interface map */
/* CXO640G v2: slots 2,3,18,19 a 20G e o resto a 40G */
# define PTIN_SLOTPORT_TO_INTF_MAP  \
          { { -1, -1, -1, -1 }, \
            { -1, -1, -1, -1 }, \
            { -1, -1,  0,  1 }, \
            { -1, -1,  2,  3 }, \
            {  4,  5,  6,  7 }, \
            {  8,  9, 10, 11 }, \
            { 12, 13, 14, 15 }, \
            { 16, 17, 18, 19 }, \
            { 20, 21, 22, 23 }, \
            { 24, 25, 26, 27 }, \
            { 28, 29, 30, 31 }, \
            { 32, 33, 34, 35 }, \
            { 36, 37, 38, 39 }, \
            { 40, 41, 42, 43 }, \
            { 44, 45, 46, 47 }, \
            { 48, 48, 50, 51 }, \
            { 52, 53, 54, 55 }, \
            { 56, 57, 58, 59 }, \
            { -1, -1, 60, 61 }, \
            { -1, -1, 62, 63 } }
# define PTIN_SLOTPORT_TO_INTF_MAP_PROT  \
          { { -1, -1, -1, -1 }, \
            { -1, -1, -1, -1 }, \
            { -1, -1, 62, 63 }, \
            { -1, -1, 60, 61 }, \
            { 56, 57, 58, 59 }, \
            { 52, 53, 54, 55 }, \
            { 48, 49, 50, 51 }, \
            { 44, 45, 46, 47 }, \
            { 40, 41, 42, 43 }, \
            { 36, 37, 38, 39 }, \
            { 32, 33, 34, 35 }, \
            { 28, 29, 30, 31 }, \
            { 24, 25, 26, 27 }, \
            { 20, 21, 22, 23 }, \
            { 16, 17, 18, 19 }, \
            { 12, 13, 14, 15 }, \
            {  8,  9, 10, 11 }, \
            {  4,  5,  6,  7 }, \
            { -1, -1,  2,  3 }, \
            { -1, -1,  0,  1 } }

/* Interface to slot map */
/* CXO640G v2: slots 2,3,18,19 a 20G e o resto a 40G */
# define PTIN_INTF_TO_SLOT_MAP  \
          {  2,  2,  3,  3, \
             4,  4,  4,  4, \
             5,  5,  5,  5, \
             6,  6,  6,  6, \
             7,  7,  7,  7, \
             8,  8,  8,  8, \
             9,  9,  9,  9, \
            10, 10, 10, 10, \
            11, 11, 11, 11, \
            12, 12, 12, 12, \
            13, 13, 13, 13, \
            14, 14, 14, 14, \
            15, 15, 15, 15, \
            16, 16, 16, 16, \
            17, 17, 17, 17, \
            18, 18, 19, 19  }

/* Interface to port map */
# define PTIN_INTF_TO_PORT_MAP  \
          {  0,  1,  0,  1, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  2,  3, \
             0,  1,  0,  1  }

/* CXO640G v2: slots 2,3,18,19 a 20G e o resto a 40G */
# define PTIN_PORTMAP_SLOT_WORK { \
   1,  2,  3,  4,  5,  6,  7,  8, \
   9, 10, 11, 12, 13, 14, 15, 16, \
  17, 18, 19, 20, 21, 22, 23, 24, \
  25, 26, 27, 28, 29, 30, 31, 32, \
  33, 34, 35, 36, 37, 38, 39, 40, \
  41, 42, 43, 44, 45, 46, 47, 48, \
  49, 50, 51, 52, 53, 54, 55, 56, \
  57, 58, 59, 60, 61, 62, 63, 64, \
}
# define PTIN_PORTMAP_SLOT_PROT { \
  63, 64, 61, 62, 57, 58, 59, 60, \
  53, 54, 55, 56, 49, 50, 51, 52, \
  45, 46, 47, 48, 41, 42, 43, 44, \
  37, 38, 39, 40, 33, 34, 35, 36, \
  29, 30, 31, 32, 25, 26, 27, 28, \
  21, 22, 23, 24, 17, 18, 19, 20, \
  13, 14, 15, 16,  9, 10, 11, 12, \
   5,  6,  7,  8,  3,  4,  1,  2, \
}

# define PTIN_SLOT_WORK                0
# define PTIN_SLOT_PROT                1

# define PTIN_SYSTEM_N_PORTS           64
# define PTIN_SYSTEM_N_PONS            0 
# define PTIN_SYSTEM_N_LAGS_EXTERNAL   (PTIN_SYSTEM_N_PORTS/2-1)
# define PTIN_SYSTEM_N_LAGS            PTIN_SYSTEM_N_PORTS
# define PTIN_SYSTEM_N_PORTS_AND_LAGS  max(PTIN_SYSTEM_N_PORTS, PTIN_SYSTEM_N_LAGS)
# define PTIN_SYSTEM_N_INTERF          (PTIN_SYSTEM_N_PORTS + PTIN_SYSTEM_N_LAGS)

# define PTIN_SYSTEM_PON_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_ETH_PORTS_MASK    0x0000000000000000ULL
# define PTIN_SYSTEM_10G_PORTS_MASK    0xFFFFFFFFFFFFFFFFULL
# define PTIN_SYSTEM_PORTS_MASK        (PTIN_SYSTEM_PON_PORTS_MASK | PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_10G_PORTS_MASK)

# define PTIN_SYSTEM_N_EVCS            129   /* Maximum nr of EVCs allowed in this equipment */
# define PTIN_SYSTEM_N_CLIENTS         1024  /* Maximum nr of clients allowed in this equipment */

# define PTIN_SYSTEM_MAX_BW_POLICERS   1024  /* Maximum number of BW policer */
# define PTIN_SYSTEM_MAX_COUNTERS      128   /* Maximum number of Multicast probes */

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_SYSTEM_EVC_VLANS_PER_BLOCK    32    /* Number of vlans for each unstacked service (Must be power of 2) */
# define PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS    4     /* Number of vlan blocks for stacked services */
# define PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS  (4096/PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2P_VLAN_MIN       PTIN_VLAN_MIN
# define PTIN_SYSTEM_EVC_P2P_VLAN_MAX      (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2P_VLAN_BLOCKS-1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MIN     (PTIN_SYSTEM_EVC_P2P_VLAN_MAX+1)
# define PTIN_SYSTEM_EVC_P2MP_VLAN_MAX     (PTIN_SYSTEM_EVC_VLANS_PER_BLOCK*PTIN_SYSTEM_EVC_P2MP_VLAN_BLOCKS-1)
#endif

# define PTIN_SYSTEM_N_IGMP_INSTANCES             8     /* Maximum nr of IGMP instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE 512   /* 512 clients per IGMP instance */

# define PTIN_SYSTEM_N_DHCP_INSTANCES             8     /* Maximum nr of DHCP instances */
# define PTIN_SYSTEM_N_PPPOE_INSTANCES            8     /* Maximum nr of PPPoE instances */
# define PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE 512   /* 512 clients per DHCP instance */

# define PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE 512

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_CPLD
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1259
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
    L7_uint8  empty3[PTIN_CPLD_MAP_SIZE - 0x10];
  } reg;
} st_cpld_map_t;

extern volatile st_cpld_map_t *cpld_map;

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0xffff - 0x0000];
  } reg;
} st_fpga_map_t;

extern volatile st_fpga_map_t *fpga_map;

//# define IPC_LOCALHOST_IPADDR          0x7F000001  /* 127.0.0.1 */
# define IPC_SERVER_IPADDR_WORKING     0xC0A8C865  /* 192.168.200.101: Working Matrix */
# define IPC_SERVER_IPADDR_PROTECTION  0xC0A8C866  /* 192.168.200.102: Protection Matrix */
# define IPC_SERVER_IPADDR             IPC_SERVER_IPADDR_WORKING  /* Default ip address */

#endif



#define PTIN_VLAN_MIN                 L7_DOT1Q_MIN_VLAN_ID_CREATE
#define PTIN_VLAN_MAX                 (L7_DOT1Q_MAX_VLAN_ID - 1)
#define PTIN_VLAN_INBAND              L7_DOT1Q_MAX_VLAN_ID  /* Reserved VLAN for inBand management */

#define PTIN_EVC_INBAND               0 /* inBand EVC id */

/* Global Macros */
#define PTIN_PORT_IS_VALID(p)         (p < PTIN_SYSTEM_N_INTERF)
#define PTIN_PORT_IS_PON(p)           (p < PTIN_SYSTEM_PON_PORTS)
#define PTIN_PORT_IS_PHYSICAL(p)      (p < PTIN_SYSTEM_N_PORTS)
#define PTIN_PORT_IS_LAG(p)           (p >= PTIN_SYSTEM_N_PORTS && p < PTIN_SYSTEM_N_INTERF)

#if (PTIN_SYSTEM_GROUP_VLANS)
# define PTIN_VLAN_IS_STACKED(vlanId)    ((vlanId)>=PTIN_SYSTEM_EVC_P2P_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_P2P_VLAN_MAX)
# define PTIN_VLAN_IS_UNSTACKED(vlanId)  ((vlanId)>=PTIN_SYSTEM_EVC_P2MP_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_P2MP_VLAN_MAX)
# define PTIN_VLAN_MASK(vlanId)           (PTIN_VLAN_IS_UNSTACKED(vlanId)) ? (~((L7_uint16) PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-1) & 0xfff) : 0xfff;
#endif

/* PTin task loop period */
#define PTIN_LOOP_TICK                (1000) /* in milliseconds */

/* Rate Limits */
#define RATE_LIMIT_LACP     64
#define RATE_LIMIT_IGMP     512
#define RATE_LIMIT_DHCP     512
#define RATE_LIMIT_PPPoE    512
#define RATE_LIMIT_BCAST    256
#define RATE_LIMIT_APS      512
#define RATE_LIMIT_CCM      512

/* PTin module states */
typedef enum {
  PTIN_LOADED = 0,
  PTIN_ISLOADING,
  PTIN_CRASHED,
} ptin_state_t;

/* PTin module state */
extern volatile ptin_state_t ptin_state;

#define PTIN_CRASH()  {       \
  ptin_state = PTIN_CRASHED;  \
  volatile int i;             \
  for(;;i++) sleep(60);       \
}

#define PTIN_T_RETRY_IPC      5   /* sec */

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

#endif /* _PTIN_GLOBALDEFS_H */
