#ifndef _PTIN_GLOBALDEFS_H
#define _PTIN_GLOBALDEFS_H

#include "l7_platformspecs.h"

/* SDK version */
#define SDK_MAJOR_VERSION   6
#define SDK_MINOR_VERSION   3
#define SDK_REVISION_ID     4
#define SDK_PATCH_ID        0
#define SDK_VERSION_IS      ( ((SDK_MAJOR_VERSION)<<24) | ((SDK_MINOR_VERSION)<<16) | ((SDK_REVISION_ID)<<8) | ((SDK_PATCH_ID)) )
#define SDK_VERSION(major,minor,revis,patch)   ( (((major) & 0xff)<<24) | (((minor) & 0xff)<<16) | (((revis) & 0xff)<<8) | (((patch) & 0xff)) )

/* ************************** */
/* Global Features Activation */

#if PTIN_BOARD_IS_MATRIX
  #define PTIN_ENABLE_ERPS
  #define PTIN_ERPS_EVC
  #define PTIN_ENABLE_DTL0TRAP
#endif  // PTIN_BOARD_IS_MATRIX

/* ************************** */


#if L7_FEAT_LAG_PRECREATE != 0
#error L7_FEAT_LAG_PRECREATE must be set to 0! (feature.h)
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

/* Global to all platforms */
#define PTIN_SYSTEM_MAX_N_FULLSLOTS     18
#define PTIN_SYSTEM_MAX_N_PORTS         64
#define PTIN_SYSTEM_MAX_N_LAGS          PTIN_SYSTEM_MAX_N_PORTS
#define PTIN_SYSTEM_MTU_SIZE            2048
#define PTIN_SYSTEM_N_EXTENDED_EVCS     (1UL << 17)     /* 17 bits will be used by management */

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

#define PTIN_BOARD_IS_PRESENT(board)  ((board) != 0)
#define PTIN_BOARD_IS_UPLINK(board)   ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || ((board)==(PTIN_BOARD_TYPE_TOLTU20GR)) || (board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR))
#define PTIN_BOARD_IS_DOWNLINK(board) ((board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR) || (board)==(PTIN_BOARD_TYPE_TG16G) || (board)==(PTIN_BOARD_TYPE_TA48GE))
#define PTIN_BOARD_IS_OLT360(board)   ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || (board)==(PTIN_BOARD_TYPE_TOLTU20GR) || (board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR))
#define PTIN_BOARD_IS_OLT1T3(board)   ((board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR) || (board)==(PTIN_BOARD_TYPE_TG16G) || (board)==(PTIN_BOARD_TYPE_TA48GE))

#define PTIN_BOARD_LS_CTRL(board)     ((board)==(PTIN_BOARD_TYPE_TOLTU20G) || (board)==(PTIN_BOARD_TYPE_TOLTU20GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TU40G) || (board)==(PTIN_BOARD_TYPE_TU40GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TOLT8G) || (board)==(PTIN_BOARD_TYPE_TOLT8GR) || \
                                       (board)==(PTIN_BOARD_TYPE_TG16G))

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

/* TA48GE */
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
#include "ptin_globaldefs_ta48ge.h"

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
#define PTIN_VLAN_MAX                 (L7_DOT1Q_MAX_VLAN_ID - 1)
#define PTIN_VLAN_INBAND              L7_DOT1Q_MAX_VLAN_ID  /* Reserved VLAN for inBand management */

#define PTIN_EVC_INBAND               0 /* inBand EVC id */

/* Global Macros */
#define PTIN_PORT_IS_VALID(p)         (p < PTIN_SYSTEM_N_INTERF)
#define PTIN_PORT_IS_PON(p)           (p < PTIN_SYSTEM_PON_PORTS)
#define PTIN_PORT_IS_PHYSICAL(p)      (p < PTIN_SYSTEM_N_PORTS)
#define PTIN_PORT_IS_LAG(p)           (p >= PTIN_SYSTEM_N_PORTS && p < PTIN_SYSTEM_N_INTERF)

# define PTIN_VLAN_IS_ELAN(vlanId)        ((vlanId)>=PTIN_SYSTEM_EVC_ELAN_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_ELAN_VLAN_MAX)
# define PTIN_VLAN_IS_ETREE(vlanId)       ((vlanId)>=PTIN_SYSTEM_EVC_ETREE_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_ETREE_VLAN_MAX)

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
# define PTIN_VLAN_IS_QUATTRO_P2P(vlanId) ((vlanId)>=PTIN_SYSTEM_EVC_QUATTRO_P2P_VLAN_MIN && (vlanId)<=PTIN_SYSTEM_EVC_QUATTRO_P2P_VLAN_MAX)
# define PTIN_VLAN_MASK(vlanId)           (PTIN_VLAN_IS_QUATTRO_P2P(vlanId)) ? (PTIN_SYSTEM_EVC_QUATTRO_P2P_VLAN_MASK) : \
                                          ((PTIN_VLAN_IS_ETREE(vlanId)) ? (~((L7_uint16) PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-1) & 0xfff) : 0xfff)
#else
# define PTIN_VLAN_IS_QUATTRO_P2P(vlanId) 0
# define PTIN_VLAN_MASK(vlanId)           ((PTIN_VLAN_IS_ETREE(vlanId)) ? (~((L7_uint16) PTIN_SYSTEM_EVC_VLANS_PER_BLOCK-1) & 0xfff) : 0xfff)
#endif

/* PTin task loop period */
#define PTIN_LOOP_TICK                (1000) /* in milliseconds */

/* Rate Limits */
#define RATE_LIMIT_LACP     64
#define RATE_LIMIT_IGMP     512
#define RATE_LIMIT_DHCP     512
#define RATE_LIMIT_PPPoE    512
#define RATE_LIMIT_BCAST    1024
#define RATE_LIMIT_MCAST    1024
#define RATE_LIMIT_UCUNK    1024
#define RATE_LIMIT_APS      512
#define RATE_LIMIT_CCM      512
#define RATE_LIMIT_IPDTL0   512
#define RATE_LIMIT_QUATTRO  4096
#define RATE_LIMIT_IPDTL0   512

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

#endif /* _PTIN_GLOBALDEFS_H */
