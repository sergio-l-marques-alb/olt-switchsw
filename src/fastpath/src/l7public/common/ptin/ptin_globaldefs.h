#ifndef _PTIN_GLOBALDEFS_H
#define _PTIN_GLOBALDEFS_H

#include "l7_platformspecs.h"

#define LOG_OUTPUT_FILE_DEFAULT   "/var/log/switchdrvr.log"
#define LOG_OUTPUT_FILE_DEFAULT2  "/var/log/switchdrvr_sdk.log"

/* ************************** */
/* Global Features Activation */

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_STANDALONE)
  #define PTIN_ENABLE_ERPS
  #define PTIN_ERPS_EVC
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_STANDALONE || PTIN_BOARD_IS_LINECARD)
  #define PTIN_ENABLE_DTL0TRAP
#endif

/* ************************** */

#define PTIN_SYS_OLT1T3_SLOTS_MAX   20
#define PTIN_SYS_OLT1T1_SLOTS_MAX   5

#if L7_FEAT_LAG_PRECREATE != 0
#error L7_FEAT_LAG_PRECREATE must be set to 0! (feature.h)
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

/* Global to all platforms */
#define PTIN_SYSTEM_MAX_N_FULLSLOTS     20
#define PTIN_SYSTEM_MAX_N_PORTS         64
#define PTIN_SYSTEM_MAX_N_LAGS          PTIN_SYSTEM_MAX_N_PORTS
#define PTIN_SYSTEM_ETH_MTU_SIZE        9600
#define PTIN_SYSTEM_PON_MTU_SIZE        2048
#define PTIN_SYSTEM_N_EXTENDED_EVCS     ((1UL << 17) + 1)     /* 17 bits will be used by management */

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

#define PTIN_INBAND_BRIDGE_SCRIPT     "/bin/sh /usr/local/ptin/scripts/startBridge.sh"  /* Only applicable to CXP640G */
#define PTIN_PCAP_BRIDGE_SCRIPT       "/bin/sh /usr/local/ptin/scripts/startPcapBridge.sh"  /* Applicable to CXP640G and Line Cards */

#define PTIN_SYSTEM_GROUP_VLANS  1

/* Number of physical interfaces */
extern int ptin_sys_number_of_ports;

#define PTIN_BOARD_TYPE_NONE      0
#define PTIN_BOARD_TYPE_TU40G     0x21
#define PTIN_BOARD_TYPE_TU40GR    0x2D
#define PTIN_BOARD_TYPE_TOLTU20G  0x18
#define PTIN_BOARD_TYPE_TOLTU20GR 0x0C
#define PTIN_BOARD_TYPE_TR32R     0x2E
#define PTIN_BOARD_TYPE_TG16G     0x1C
#define PTIN_BOARD_TYPE_TOLT8G    0x19
#define PTIN_BOARD_TYPE_TOLT8GR   0x0E
#define PTIN_BOARD_TYPE_TA48GE    0x28
#define PTIN_BOARD_TYPE_TA48GED   0x59
#define PTIN_BOARD_TYPE_CXO160G   0x39
#define PTIN_BOARD_TYPE_CXO640G   0x33

#define PTIN_BOARD_IS_PRESENT(board)  ((board) != 0)
#define PTIN_BOARD_IS_UPLINK(board)   ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || ((board)==(PTIN_BOARD_TYPE_TOLTU20GR)) || (board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR) || (board)==(PTIN_BOARD_TYPE_CXO160G))
#define PTIN_BOARD_IS_DOWNLINK(board) ((board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR) || (board)==(PTIN_BOARD_TYPE_TG16G) || (board)==(PTIN_BOARD_TYPE_TA48GE) || (board)==(PTIN_BOARD_TYPE_TA48GED))
#define PTIN_BOARD_IS_OLT360(board)   ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || (board)==(PTIN_BOARD_TYPE_TOLTU20GR) || (board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR))
#define PTIN_BOARD_IS_OLT1T3(board)   ((board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR) || (board)==(PTIN_BOARD_TYPE_TG16G) || (board)==(PTIN_BOARD_TYPE_TA48GE) || (board)==(PTIN_BOARD_TYPE_TA48GED))

#define PTIN_BOARD_LS_CTRL(board)     ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || (board)==(PTIN_BOARD_TYPE_TOLTU20GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TG16G) || (board)==(PTIN_BOARD_TYPE_CXO160G) || (board)==(PTIN_BOARD_TYPE_CXO640G))

/* OLT10 */
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
#include "ptin_globaldefs_olt1t0.h"

/* OLT7-8CH */
#elif (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
#include "ptin_globaldefs_olt7-8ch-b.h"

/* TOLT8G */
#elif (PTIN_BOARD == PTIN_BOARD_TOLT8G)
#include "ptin_globaldefs_tolt8g.h"

/* TG16G */
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
#include "ptin_globaldefs_tg16g.h"

/* TG4G */
#elif (PTIN_BOARD == PTIN_BOARD_TG4G)
#include "ptin_globaldefs_tg4g.h"

/* TA48GE */
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
#include "ptin_globaldefs_ta48ge.h"

/* CXO160G */
#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)
#include "ptin_globaldefs_cxo160g.h"

/* OLT360 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
#include "ptin_globaldefs_cxo360g.h"

/* OLT1T3 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
#include "ptin_globaldefs_cxo640g.h"

#endif




#if PTIN_BOARD_IS_MATRIX
#ifdef MAP_CPLD
  #define is_matrix_protection() (cpld_map->reg.slot_id != 0)   /* To know if we are in protection matrix */
  #define matrix_board_version() ((cpld_map->reg.id==CPLD_ID_CXO640G_V1) ? 1 : 2)
#else
  #define is_matrix_protection() 0
  #define matrix_board_version() 2
#endif
#endif  // PTIN_BOARD_IS_MATRIX


#define PTIN_VLAN_MIN                 L7_DOT1Q_MIN_VLAN_ID_CREATE
#define PTIN_VLAN_MAX                 L7_DOT1Q_MAX_VLAN_ID
#define PTIN_RESERVED_VLAN_MIN        2016
#define PTIN_RESERVED_VLAN_MAX        2047
#define PTIN_RESERVED_VLANS           (PTIN_RESERVED_VLAN_MAX - PTIN_RESERVED_VLAN_MIN + 1)
#define PTIN_VLAN_PCAP_EXT            2048  /* L7_DOT1Q_MAX_VLAN_ID - Reserved VLAN for packet capture. Only used on Linux interfaces (dtl0 and eth0) */
#define PTIN_VLAN_INBAND              2047  /* L7_DOT1Q_MAX_VLAN_ID - Reserved VLAN for inBand management */
#define PTIN_VLAN_BL2CPU              2046  /* (L7_DOT1Q_MAX_VLAN_ID - 1) */
#define PTIN_VLAN_BL2CPU_EXT          400

#define PTIN_EVC_INBAND               0 /* inBand EVC id */
#define PTIN_EVC_BL2CPU               (PTIN_SYSTEM_N_EXTENDED_EVCS - 1)

/* Global Macros */
#define PTIN_PORT_IS_VALID(p)         (p < PTIN_SYSTEM_N_INTERF)
#define PTIN_PORT_IS_PON(p)           (p < PTIN_SYSTEM_PON_PORTS)
#define PTIN_PORT_IS_PHYSICAL(p)      (p < PTIN_SYSTEM_N_PORTS && p < ptin_sys_number_of_ports)
#define PTIN_PORT_IS_LAG(p)           (p >= PTIN_SYSTEM_N_PORTS && p < PTIN_SYSTEM_N_INTERF)

# define PTIN_VLAN_IS_ELAN(vlanId)        ((vlanId)>=PTIN_SYSTEM_EVC_ELAN_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_ELAN_VLAN_MAX)
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
# define PTIN_VLAN_IS_QUATTRO(vlanId) ((vlanId)>=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX)
# define PTIN_VLAN_MASK(vlanId)       ((PTIN_VLAN_IS_QUATTRO(vlanId)) ? (PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK) : 0xfff)
#else
# define PTIN_VLAN_IS_QUATTRO(vlanId) 0
# define PTIN_VLAN_MASK(vlanId)       0xfff
#endif

/* PTin task loop period */
#define PTIN_LOOP_TICK                (1000) /* in milliseconds */

/* Rate Limits */
#define RATE_LIMIT_LACP     64
#define RATE_LIMIT_IGMP     256
#define RATE_LIMIT_DHCP     256
#define RATE_LIMIT_PPPoE    256
#define RATE_LIMIT_APS      256
#define RATE_LIMIT_CCM      256
#define RATE_LIMIT_IPDTL0   256
#define RATE_LIMIT_QUATTRO  1024
#define RATE_LIMIT_INBAND   4096
#define RATE_LIMIT_PCAP     1024    /* 1 Mbps by default */
#define RATE_LIMIT_BCAST    1024    /* 1 Mbps by default */
#define RATE_LIMIT_MCAST    1024    /* 1 Mbps by default */
#define RATE_LIMIT_UCUNK    1024    /* 1 Mbps by default */
#define RATE_LIMIT_CPU      1024    /* 1 Mbps by default */

/* CoS assigned to trapped packets */
#define CPU_TRAPPED_PACKETS_COS_DEFAULT 8     /* For IGMP/DHCP/PPPoE/APS/... */
#define CPU_TRAPPED_PACKETS_COS_INBAND  9     /* For Inband packets */
#define CPU_TRAPPED_PACKETS_COS_HIPRIO  10    /* High priority */
#define CPU_TRAPPED_PACKETS_COS_PCAP    11    /* For Packet Capture */

/* PTin module states */
typedef enum {
  PTIN_STATE_READY   = 0,
  PTIN_STATE_BUSY    = 1,
  PTIN_STATE_LOADING = 2,
  PTIN_STATE_CRASHED = 3,
  PTIN_STATE_LAST
} ptin_state_t;

/* PTin module state */
extern volatile ptin_state_t ptin_state;

extern volatile void        *ptin_task_msg_buffer;

extern volatile L7_uint32    ptin_task_msg_id;

#define PTIN_CRASH()  {       \
  ptin_state = PTIN_STATE_CRASHED;  \
  volatile int i;             \
  for(;;i++) sleep(60);       \
}

#define PTIN_T_RETRY_IPC      5   /* sec */

#endif /* _PTIN_GLOBALDEFS_H */
