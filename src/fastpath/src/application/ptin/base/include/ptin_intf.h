#ifndef _PTIN_INTERFACE_H
#define _PTIN_INTERFACE_H

#include "datatypes.h"
#include "ptin_structs.h"
#include "ptin_utils.h"

#define PHY_MAX_MAXFRAME              2048

#define PHY_PORT_MEDIA_ELECTRIC       0
#define PHY_PORT_MEDIA_OPTICAL        1
#define PHY_PORT_MEDIA_INTERNAL       2

#define PHY_PORT_AUTONEG              0
#define PHY_PORT_10_MBPS              1
#define PHY_PORT_100_MBPS             2
#define PHY_PORT_1000_MBPS            3
#define PHY_PORT_2500_MBPS            4   /* PTin added: Speed 2.5G */
#define PHY_PORT_10_GBPS              5   /* PTin added: Speed 10G */
#define PHY_PORT_1000AN_GBPS          6   /* PTin added: Speed 1G with/ Autoneg */
#define PHY_PORT_40_GBPS              7   /* PTin added: Speed 40G */
#define PHY_PORT_100_GBPS             8   /* PTin added: Speed 100G */

#define PTIN_PORT_ALL       PTIN_SYSTEM_N_INTERF
#define PTIN_PORT_INVALID   ((L7_uint32)-1)

#define PTIN_PORT_IS_VALID(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_INTERF)

/* Macros to check interface phy-type */
#define PTIN_PORT_IS_PON(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_PONS)
#define PTIN_PORT_IS_FRONT_ETH(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_ETH)
#define PTIN_PORT_IS_FRONT(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_PONS || (ptin_port) < PTIN_SYSTEM_N_ETH)
#define PTIN_PORT_IS_INTERNAL(ptin_port) \
    ((ptin_port) >= PTIN_SYSTEM_N_PONS && (ptin_port) >= PTIN_SYSTEM_N_ETH && (ptin_port) < PTIN_SYSTEM_N_PORTS)

/* The following Macros are used to distinguish PON from XGSPON ports */
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
 #define PTIN_PORT_IS_PON_GPON_TYPE(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_PONS/2)
 #define PTIN_PORT_IS_PON_XGSPON_TYPE(ptin_port) \
    ((ptin_port) >= PTIN_SYSTEM_N_PONS/2 && (ptin_port) < PTIN_SYSTEM_N_PONS)
#else
 #define PTIN_PORT_IS_PON_GPON_TYPE(ptin_port)   PTIN_PORT_IS_PON(ptin_port)
 #define PTIN_PORT_IS_PON_XGSPON_TYPE(ptin_port) L7_FALSE
#endif

/* Macros to check interface type */
#define PTIN_PORT_IS_PHY(ptin_port) \
    ((ptin_port) < PTIN_SYSTEM_N_PORTS)
#define PTIN_PORT_IS_LAG(ptin_port) \
    ((ptin_port) >= PTIN_SYSTEM_N_PORTS && (ptin_port) < (PTIN_SYSTEM_N_PORTS+PTIN_SYSTEM_N_LAGS))
#define PTIN_PORT_IS_OTHER_TYPE(ptin_port) \
    ((ptin_port) >= (PTIN_SYSTEM_N_PORTS+PTIN_SYSTEM_N_LAGS) && (ptin_port) < PTIN_SYSTEM_N_INTERF)

/*Bitmap Macro Handlers*/
#define UCHAR8_BITSIZE  (sizeof(L7_uchar8)*8)

typedef struct {
    bmp_cell_t value[PTIN_SYSTEM_N_INTERF/8 + 1];
} ptin_port_bmp_t;

#define PTINPORT_BITMAP_SET(ptin_port_bmp,  ptin_port) \
    BITMAP_SET((ptin_port_bmp).value, ptin_port)

#define PTINPORT_BITMAP_CLEAR(ptin_port_bmp,  ptin_port) \
    BITMAP_CLEAR((ptin_port_bmp).value, ptin_port)

#define PTINPORT_BITMAP_CLEARALL(ptin_port_bmp) \
    BITMAP_CLEARALL((ptin_port_bmp).value)

#define PTINPORT_BITMAP_IS_SET(ptin_port_bmp,  ptin_port) \
    BITMAP_IS_SET((ptin_port_bmp).value, ptin_port)

#define PTINPORT_BITMAP_IS_WORD_SET(ptin_port_bmp, ptin_port) \
    (ptin_port_bmp.value[((ptin_port)/UCHAR8_BITSIZE)] == 0 ? 0 : 1)

#define BITMAP_IS_CLEAR(ptin_port_bmp,  ptin_port) \
    BITMAP_IS_CLEAR((ptin_port_bmp).value, ptin_port)

#define BITMAP_IS_CLEARALL(ptin_port_bmp) \
    BITMAP_IS_CLEARALL((ptin_port_bmp).value)


#ifdef PORT_VIRTUALIZATION_N_1
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
 #if defined (PORT_VIRTUALIZATION_4_1) /*ASPEN 4:1*/
/* Please check
   https://jira.ptin.corppt.com/secure/attachment/620082/screenshot-1.png
   https://jira.ptin.corppt.com/browse/OLTSWITCH-1371
*/
  #define PTIN_CARD_MAX_N_MODES          2 /* 0 - MPM, 1- GPON mode*/
  #define PORT_VIRTUALIZATION_VID_N_SETS 4

  static const
  L7_uint32 phy2vport[PTIN_CARD_MAX_N_MODES][PTIN_SYSTEM_N_PONS_PHYSICAL][PORT_VIRTUALIZATION_VID_N_SETS] = {
     {{ 16,   0,  17,   1},
      {-1U, -1U, -1U, -1U},
      { 18,   2,  19,   3},
      {-1U, -1U, -1U, -1U},
      { 20,   4,  21,   5},
      {-1U, -1U, -1U, -1U},
      { 22,   6,  23,   7},
      {-1U, -1U, -1U, -1U},
      { 24,   8,  25,   9},
      {-1U, -1U, -1U, -1U},
      { 26,  10,  27,  11},
      {-1U, -1U, -1U, -1U},
      { 28,  12,  29,  13},
      {-1U, -1U, -1U, -1U},
      { 30,  14,  31,  15},
      {-1U, -1U, -1U, -1U}}, 

     {{ 0,  -1U,  1,  16}, 
      {-1U, -1U, -1U, 17},     
      { 2,  -1U,  3,  18},   
      {-1U, -1U, -1U, 19},     
      { 4,  -1U,  5,  20},   
      {-1U, -1U, -1U, 21},     
      { 6,  -1U,  7,  22},   
      {-1U, -1U, -1U, 23},     
      { 8,  -1U,  9,  24},   
      {-1U, -1U, -1U, 25},     
      { 10, -1U,  11, 26},     
      {-1U, -1U, -1U, 27},     
      { 12, -1U,  13, 28},     
      {-1U, -1U, -1U, 29},     
      { 14, -1U,  15, 30},          
      {-1U, -1U, -1U, 31}},   

  };

 #elif defined (PORT_VIRTUALIZATION_2_1) /*ASPEN 2:1*/
/* Please check
   https://jira.ptin.corppt.com/secure/attachment/620085/screenshot-2.png
   https://jira.ptin.corppt.com/browse/OLTSWITCH-1371
*/

  #define PTIN_CARD_MAX_N_MODES          2/* 0 - MPM, 1- GPON mode*/
  #define PORT_VIRTUALIZATION_VID_N_SETS 4

  static const
  L7_uint32 phy2vport[PTIN_CARD_MAX_N_MODES][PTIN_SYSTEM_N_PONS_PHYSICAL][PORT_VIRTUALIZATION_VID_N_SETS] = {
     {{ 16,   0, -1U, -1U},     
      {-1U, -1U,  17,   1},     
      { 18,   2, -1U, -1U},     
      {-1U, -1U,  19,   3},     
      { 20,   4, -1U, -1U},     
      {-1U, -1U,  21,   5},     
      { 22,   6, -1U, -1U},     
      {-1U, -1U,  23,   7},     
      { 24,   8, -1U, -1U},     
      {-1U, -1U,  25,   9},     
      { 26,  10, -1U, -1U},     
      {-1U, -1U,  27,  11},     
      { 28,  12, -1U, -1U},     
      {-1U, -1U,  29,  13},     
      { 30,  14, -1U, -1U},     
      {-1U, -1U,  31,  15}}, 

     {{ 0,  -1U, -1U, 16},    
      { 1,  -1U, -1U, 17},    
      { 2,  -1U, -1U, 18},    
      { 3,  -1U, -1U, 19},    
      { 4,  -1U, -1U, 20},    
      { 5,  -1U, -1U, 21},    
      { 6,  -1U, -1U, 22},    
      { 7,  -1U, -1U, 23},    
      { 8,  -1U, -1U, 24},    
      { 9,  -1U, -1U, 25},    
      { 10, -1U, -1U, 26},    
      { 11, -1U, -1U, 27},    
      { 12, -1U, -1U, 28},    
      { 13, -1U, -1U, 29},    
      { 14, -1U, -1U, 30},    
      { 15, -1U, -1U, 31}},   
  };

 #else /*defined (PORT_VIRTUALIZATION_?_1)*/
  #error "Port virtualization mode not defined!"
 #endif /* defined (PORT_VIRTUALIZATION_?_1) */

#else /* Other boards */
 #error "Port virtualization not supported!"
#endif

#else /*PORT_VIRTUALIZATION_N_1*/
 /* Default */
 #define PORT_VIRTUALIZATION_VID_N_SETS 1
#endif /*PORT_VIRTUALIZATION_N_1*/


extern L7_BOOL linkscan_update_control;

/**
 * Initializes the ptin_intf module (structures) and several interfaces 
 * related configurations.
 *  
 * NOTE: This function must be invoked ONLY after fastpath initialization. 
 * During Init phase1/2/3 stages, the interfaces MAY NOT be initialized! 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_pre_init(void);
extern L7_RC_t ptin_intf_post_init(void);

/**
 * Init Port exitension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_init(void);

/**
 * Set Port exitension definitions
 * 
 * @param ptin_port : Interface
 *        mefExt    : MEF Extension parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_set(L7_uint32 ptin_port, ptin_HWPortExt_t *mefExt);

/**
 * Get Port exitension definitions
 * 
 * @param ptin_port : Interface
 *        mefExt    : MEF Extension parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_get(L7_uint32 ptin_port, ptin_HWPortExt_t *mefExt);

/**
 * Set MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portMAC_set(const ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac);

/**
 * Get MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portMAC_get(const ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac);

/**
 * Configure a physical interface
 * 
 * @param phyConf Structure with port configuration
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_PhyConfig_set(ptin_HWEthPhyConf_t *phyConf);

/**
 * Get physical interface configuration (read values from buffered config)
 * 
 * @param phyConf Structure to save port configuration (Port member 
 * must be set with the respective port; mask is ignored!)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_PhyConfig_get(ptin_HWEthPhyConf_t *phyConf);

/**
 * Get physical interface state (read values from switch)
 * 
 * @param phyConf Structure to save port configuration (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_PhyState_read(ptin_HWEthPhyState_t *phyState);

/**
 * Read counter of a specific physical interface
 *  
 * @param ptin_port 
 * @param portStats Structure to save port counters (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_counters_read(L7_uint ptin_port, ptin_HWEthRFC2819_PortStatistics_t *portStats);

/**
 * Clear counters
 * 
 * @param ptin_port
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_counters_clear(L7_uint ptin_port);

/**
 * Read counters activity (of physical ports)
 *  
 * @param ptin_port 
 * @param portActivity Structure to save port counters activity (at the 
 * moment, masks are ignored, therefore all values are read for all ports) 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_counters_activity_get(L7_uint32 ptin_port, ptin_HWEth_PortsActivity_t *portActivity);

/*
 * Board management
 */ 

/**
 * Get board id for a particular interface
 * 
 * @author mruas (07/09/17)
 * 
 * @param ptin_port 
 * 
 * @return L7_int : board_id or -1 if error
 */
extern L7_int ptin_intf_board(L7_int ptin_port);

/**
 * Get board id for a particular slot
 * 
 * @author mruas (07/09/17)
 * 
 * @param slot_id 
 * 
 * @return L7_int : board_id or -1 if error
 */
extern L7_int ptin_slot_board(L7_int slot_id);

/**
 * Get board id for a particular interface
 *  
 * @param ptin_port
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_boardid_get(L7_int ptin_port, L7_uint16 *board_id);

/**
 * Set board id for a particular interface (will override 
 * board_id of other interfaces, if they belong to the same 
 * slot) 
 *  
 * @param ptin_port
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_boardid_set(L7_int ptin_port, L7_uint16 board_id);

/**
 * Get board id for a particular slot
 *  
 * @param slot_id
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_slot_boardid_get(L7_int slot_id, L7_uint16 *board_id);

/**
 * Set board id
 *  
 * @param slot_id
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_slot_boardtype_set(L7_int slot_id, L7_uint16 board_id);

/*
 * Port, LAGs and Interfaces convertion functions
 */ 

/**
 * Get all interface formats
 * 
 * @param intf 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_any_format(ptin_intf_any_format_t *intf);

/**
 * Direct function to convert ptin_intf_t to ptin_port
 * 
 * @author mruas (12/11/20)
 * 
 * @param port_type (in) : ptin_intf_t.intf_type
 * @param port_id (in) : ptin_intf_t.intf_id 
 * 
 * @return L7_uint32 : ptin_port
 */
extern L7_uint32 ptintf2port(L7_uint8 intf_type, L7_uint8 intf_id);

/**
 * Direct function to convert intIfNum to ptin_port
 * 
 * @author mruas (12/11/20)
 * 
 * @param intIfNum 
 * @param vlan_gem 
 * 
 * @return L7_uint32 : ptin_port
 */
extern L7_uint32 intIfNum2port(L7_uint32 intIfNum, L7_uint16 vlan_gem);

/**
 * Direct function to convert ptin_port to intIfNum
 * 
 * @author mruas (12/11/20)
 * 
 * @param ptin_port
 * @param vlan_gem 
 * 
 * @return L7_uint32 : intIfNum
 */
extern L7_uint32 port2intIfNum(L7_uint32 ptin_port);

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get current slot_id for this board
 * 
 * @param slot_id : slot index (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_slot_get(L7_uint8 *slot_id);
#endif

/**
 * Get slot and port location in the system, from the ptin_port
 * 
 * @author mruas (3/14/2013)
 * 
 * @param ptin_port
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_port2SlotPort(L7_uint32 ptin_port, L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type);

/**
 * Get the ptin_port from the slot and port location in the 
 * system. 
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param ptin_port (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_slotPort2port(L7_uint16 slot, L7_uint16 port, L7_uint32 *ptin_port_ret);

/**
 * Get slot and port location in the system, from the ptin_intf
 * 
 * @author mruas (3/14/2013)
 * 
 * @param ptin_intf
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_ptintf2SlotPort(const ptin_intf_t *ptin_intf, L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type);

/**
 * Get slot and port location in the system, from the ptin_intf
 * 
 * @author mruas (3/14/2013)
 * 
 * @param intf_type 
 * @param intf_id
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_typeId2SlotPort(L7_uint8 ptin_type, L7_uint8 intf_id,
                                         L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type);

/**
 * Get the ptin_intf from the slot and port location in the 
 * system. 
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param ptin_intf (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_slotPort2ptintf(L7_uint16 slot, L7_uint16 port, ptin_intf_t *ptin_intf);

/**
 * Get slot and port location in the system, from the intIfNum
 * 
 * @author mruas (3/14/2013)
 * 
 * @param intIfNum 
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_intIfNum2SlotPort(L7_uint32 intIfNum, L7_uint16 *slot_ret, L7_uint16 *intf_ret, L7_uint16 *board_type);

/**
 * Get the intIfNum from the slot and port location in the 
 * system.
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param intIfNum (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_slotPort2IntIfNum(L7_uint16 slot, L7_uint16 intf, L7_uint32 *intIfNum_ret);

/**
 * Convert a ptin_port bitmap to NIM_INTF_MASK_t type
 * 
 * @author mruas (16/11/20)
 * 
 * @param ptin_port_bmp (in)
 * @param portMask (out)
 */
extern void ptin_intf_portbmp2intIfNumMask(ptin_port_bmp_t *ptin_port_bmp, NIM_INTF_MASK_t *portMask);

/**
 * Converts PTin port mapping (including LAGs) to the FP interface#
 * 
 * @param ptin_port PTin port index
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_port2intIfNum(L7_uint32 ptin_port, L7_uint32 *intIfNum);

/**
 * Convert ptin_port to the pair intIfNum + queueSet
 * 
 * @author mruas (11/01/21)
 * 
 * @param ptin_port 
 * @param intIfNum 
 * @param queueSet : l7_cosq_set_t
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_port2intIfNum_queueSet(L7_uint32 ptin_port, L7_uint32 *intIfNum, l7_cosq_set_t *queueSet);

/**
 * Converts FP interface# to PTin port mapping (including LAGs)
 * 
 * @param intIfNum (in) : FP intIfNum 
 * @param virtual_vid (in) : Virtualized VLAN
 * @param ptin_port (out) : PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
#define INVALID_SWITCH_VID 0
/* BCM_VLAN_INVALID DOT1Q_VLAN_INTF_INVALID L7_ACL_INVALID_VLAN_ID */
extern L7_RC_t ptin_intf_intIfNum2port(L7_uint32 intIfNum, L7_uint16 virtual_vid,
                                       L7_uint32 *ptin_port);

/**
 * Convert virtual VID to GEM VID
 * 
 * @author mruas (10/12/20)
 * 
 * @param virtual_vid (in)
 * @param gem_vid (out)
 * 
 * @return L7_RC_t 
 */
extern 
L7_RC_t ptin_intf_virtualVid2GemVid(L7_uint16 virtual_vid, L7_uint16 *gem_vid);

/**
 * From the virtualized ptin_port and GEM-VLAN id, obtain the 
 * physical intIfNum and the Virtualized VLAN with an offset 
 * added (4096/#VirtualPorts_per_PhyPort) 
 * 
 * @author mruas (26/11/20)
 * 
 * @param ptin_port (in) : Virtualized port
 * @param gem_vid (in) : GEM VLAN id 
 * @param virtual_vid (out): Virtualized GEM-VID
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern 
L7_RC_t ptin_intf_portGem2virtualVid(L7_uint32 ptin_port, L7_uint16 gem_vid, L7_uint16 *virtual_vid);

/**
 * Converts ptin_port index to LAG index
 * 
 * @param ptin_port PTin port index
 * @param lag_idx   LAG index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_port2lag(L7_uint32 ptin_port, L7_uint32 *lag_idx);

/**
 * Converts LAG index to ptin_port
 *  
 * @param lag_idx   LAG index 
 * @param ptin_port PTin port index 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_lag2port(L7_uint32 lag_idx, L7_uint32 *ptin_port);

/**
 * Converts ptin_port index to PTin port type and id
 * 
 * @param ptin_port PTin port index
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_port2ptintf(L7_uint32 ptin_port, ptin_intf_t *ptin_intf);

/**
 * Converts PTin port type and id to ptin_port index
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_ptintf2port(const ptin_intf_t *ptin_intf, L7_uint32 *ptin_port);

/**
 * Converts PTin port type and id to ptin_port index
 * 
 * @param intf_type PTin port type (0 is physical and 1 is LAG) 
 * @param intf_id   PTin port id
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_typeId2port(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 *ptin_port);

/**
 * Converts ptin_port index to port type and id
 * 
 * @param ptin_port PTin port index
 * @param intf_type PTin port type (out)
 * @param intf_id   PTin port id (out)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_port2typeId(L7_uint32 ptin_port, L7_uint8 *intf_type, L7_uint8 *intf_id);

/**
 * Converts FP interface# to PTin port type and id
 * 
 * @param intIfNum  FP intIfNum
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_intIfNum2ptintf(L7_uint32 intIfNum, ptin_intf_t *ptin_intf);

/**
 * Converts PTin port type and id to FP interface#
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_ptintf2intIfNum(const ptin_intf_t *ptin_intf, L7_uint32 *intIfNum);

/**
 * Converts PTin port type and id to FP interface#
 * 
 * @param intf_type PTin port type (0 is physical and 1 is LAG) 
 * @param intf_id   PTin port id
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_typeId2intIfNum(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 *intIfNum);

/**
 * Converts LAG index [1..PTIN_SYSTEM_N_LAGS] to FP intIfNum
 * 
 * @param intIfNum  FP intIfNum
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_lag2intIfNum(L7_uint32 lag_idx, L7_uint32 *intIfNum);

/**
 * Convert intIfNum to LAG index
 * 
 * @param intIfNum  FP intIfNum
 * @param lag_idx   LAG index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_intIfNum2lag(L7_uint32 intIfNum, L7_uint32 *lag_idx);

#if PTIN_BOARD_IS_MATRIX
/**
 * Converts Slot to LAG index [0..PTIN_SYSTEM_N_LAGS[
 * 
 * @param slot      slot
 * @param lag_idx   LAG index [0..PTIN_SYSTEM_N_LAGS[ (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_slot2lagIdx(L7_uint16 slot, L7_uint32 *lag_idx);
#endif

/**
 * Check if a LAG is created [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @param lag_idx   LAG index [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @return L7_RC_t L7_TRUE/L7_FALSE
 */
extern L7_RC_t ptin_intf_lag_exists(L7_uint32 lag_idx);


/**
 * Check if a particular port is used for uplink protection
 * 
 * @author mruas (5/2/2014)
 * 
 * @param ptin_port 
 * 
 * @return L7_BOOL 
 */
extern L7_BOOL ptin_intf_is_uplinkProtection(L7_uint32 ptin_port);

/**
 * Check if a protection port is active
 * 
 * @author mruas (5/28/2014)
 * 
 * @param ptin_port 
 * 
 * @return L7_BOOL 
 */
extern L7_BOOL ptin_intf_is_uplinkProtectionActive(L7_uint32 ptin_port);

/*
 * LAGs Manipulation Functions
 */ 
/**
 * Gets one or all LAGs info
 * 
 * @param lagInfo Pointer to the output structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LagConfig_get(ptin_LACPLagConfig_t *lagInfo);

/**
 * Creates a LAG
 * 
 * @param lagInfo Pointer to the structure with LAG info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_Lag_create(ptin_LACPLagConfig_t *lagInfo);

/**
 * Deletes a LAG
 * 
 * @param lag_idx LAG Identifier
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_Lag_delete(L7_uint32 lag_idx);

/**
 * Deletes all LAGs
 * 
 * @param 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_Lag_delete_all(void);

/**
 * Gets a LAG status
 * 
 * @param lagStatus Pointer to the output structure (or array)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LagStatus_get(ptin_LACPLagStatus_t *lagStatus);

/**
 * Sets a Port LACP Admin State
 * 
 * @param adminState Pointer to the input structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LACPAdminState_set(ptin_LACPAdminState_t *adminState);

/**
 * Gets a Port LACP Admin State
 * 
 * @param adminState Pointer to the output structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LACPAdminState_get(ptin_LACPAdminState_t *adminState);

/**
 * Gets a Port LACP statistics info
 * 
 * @param lagStats Pointer to the output structure (or array)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LACPStats_get(ptin_LACPStats_t *lagStats);

/**
 * Clears a Port LACP statistics info
 * 
 * @param lagStats Structure that references the Port# stats to clear
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_LACPStats_clear(ptin_LACPStats_t *lagStats);

/**
 * Read stormcontrol data for Broadcast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param rate_burst  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
extern
L7_RC_t ptin_intf_bcast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units);

/**
 * Read stormcontrol data for Multicast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param burst_size  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
extern 
L7_RC_t ptin_intf_mcast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units);

/**
 * Read stormcontrol data for Multicast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param burst_size  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
extern 
L7_RC_t ptin_intf_mcast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units);

/**
 * Configure stormcontrol for Broadcast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
extern
L7_RC_t ptin_intf_bcast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units);

/**
 * Configure stormcontrol for Multicast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
extern
L7_RC_t ptin_intf_mcast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units);

/**
 * Configure stormcontrol for unknown Unicast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
extern
L7_RC_t ptin_intf_ucast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units);

/**
 * Activate PRBS generator/checker
 *  
 * @param ptin_port: Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pcs_prbs_enable(L7_uint32 ptin_port, L7_BOOL enable);

/**
 * Read number of PRBS errors
 *  
 * @param ptin_port: Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pcs_prbs_errors_get(L7_uint32 ptin_port, L7_uint32 *counter);

/**
 * Configure Default VLANs using VCAP rules
 * 
 * @param ptin_port 
 * @param outerVlan 
 * @param innerVlan  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_vcap_defvid(L7_uint32 ptin_port, L7_uint16 outerVlan, L7_uint16 innerVlan);

/**
 * Configure clock recovery references
 * 
 * @param ptin_port_main : main port
 * @param ptin_port_bckp : backup port
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_clock_recover_set(L7_int ptin_port_main, L7_int ptin_port_bckp);

/**
 * Configure Maximum frame size
 * 
 * @param ptin_port 
 * @param frame_size 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
extern L7_RC_t ptin_intf_frame_oversize_set(L7_uint32 ptin_port, L7_uint32 frame_size);

/**
 * Apply linkfaults enable procedure
 *  
 * @param ptin_port : Interface
 * @param local_enable : Local faults processing enable 
 * @param remote_enable : Remote faults processing enable 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkfaults_enable(L7_uint32 ptin_port, L7_BOOL local_enable, L7_BOOL remote_enable);

/**
 * Read Maximum frame size
 * 
 * @param ptin_port 
 * @param frame_size (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
extern L7_RC_t ptin_intf_frame_oversize_get(L7_uint32 ptin_port, L7_uint32 *frame_size);

/**
 * Enable or disable linkscan control for a particular port
 * 
 * @param port 
 * @param enable 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkscan_control(L7_uint port, L7_BOOL enable);

/**
 * Reset warpcore associated to a specific slot 
 * 
 * @param slot_id 
 * @param force_linkup 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_slot_reset(L7_int slot_id, L7_BOOL force_linkup);

/**
 * read linkscan status
 *  
 * @param ptin_port : Interface
 * @param enable : enable (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkscan_get(L7_uint32 ptin_port, L7_uint8 *enable);

/**
 * Apply linkscan procedure
 *  
 * @param ptin_port : Interface 
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkscan_set(L7_uint32 ptin_port, L7_uint8 enable);

/**
 * Apply linkscan procedure
 *  
 * @param ptin_port : Interface
 * @param link : link_status 
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_link_force(L7_uint32 ptin_port, L7_uint8 link, L7_uint8 enable);

/**
 * Apply linkscan procedure
 *  
 * @param slot_id : slot id 
 * @param slot_port : slot port index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_slot_linkscan_set(L7_int slot_id, L7_int slot_port, L7_uint8 enable);

/**
 * Force link to all slot ports
 *  
 * @param slot_id : slot id 
 * @param link : link status  
 * @param slot_port : slot port index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_slot_link_force(L7_int slot_id, L7_int slot_port, L7_uint8 link, L7_uint8 enable);

/**
 * Procedure for board insertion
 * 
 * @param slot_id 
 * @param board_id 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_slot_action_insert(L7_uint16 slot_id, L7_uint16 board_id);

/**
 * Procedure for board removal
 * 
 * @author mruas (5/28/2014)
 * 
 * @param slot_id 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_slot_action_remove(L7_uint16 slot_id);

/**
 * Get the current slot mode list
 *  
 * @param slotmodes 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_slotMode_get(L7_uint32 *slotmodes);

/**
 * Validate a slot mode list
 *  
 * @param slotmodes 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_slotMode_validate(L7_uint32 *slotmodes);

/**
 * Get interface status
 * 
 * @param ptin_intf : interface (input)
 * @param enable    : admin state (out)
 * @param link      : link state (out)
 * @param board_type: board_id (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_info_get(const ptin_intf_t *ptin_intf, L7_uint16 *enable, L7_uint16 *link, L7_uint16 *board_type);

/**
 * Get LOS state of a given ptin_port 
 *  
 * @param ptin_intf : interface (input)
 * @param los       : LOS state (out)
 * 
 * @return L7_RC_t : L7_TRUE / L7_FALSE 
 * 
 * @author joaom (11/6/2014)
 */
extern L7_BOOL ptin_intf_los_get(L7_uint32 ptin_port);

/**
 * Get Link Down of a given ptin_port 
 *  
 * @param ptin_intf : interface (input)
 * @param link      : link state (out)
 * 
 * @return L7_RC_t : L7_TRUE / L7_FALSE 
 * 
 * @author joaom (11/6/2014)
 */
extern L7_BOOL ptin_intf_link_get(L7_uint32 ptin_port);

/**
 * Protection command
 * 
 * @param slot : board slot
 * @param port : board port
 * @param cmd : command
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */

/**
 * Get the maximum bandwidth associated to a interface (physical
 * or LAG) 
 * 
 * @param ptin_port 
 * @param bandwidth : bandwidth in Kbps 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_ERROR
 */
extern L7_RC_t ptin_intf_max_bandwidth(L7_uint32 ptin_port, L7_uint32 *bandwidth);

/**
 * Get the AVAILABLE bandwidth of an interface (physical or LAG)
 * 
 * @param ptin_port 
 * @param bandwidth : bandwidth in Kbps 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_ERROR
 */
extern L7_RC_t ptin_intf_active_bandwidth(L7_uint32 ptin_port, L7_uint32 *bandwidth);

#ifdef NGPON2_SUPPORTED
/**
 * PTIN_INTF NGPON2 Add Group 
 *  
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 *  
 *  
 * @author Hugo Araujo: hugo-f-araujo@telecom.pt 
 */
extern L7_RC_t ptin_intf_NGPON2_add_group(ptin_NGPON2group_t *group_info);


/**
 * PTIN_INTF NGPON2 Remove Group 
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 *  
 * @author Hugo Araujo: hugo-f-araujo@telecom.pt  
 */
extern L7_RC_t ptin_intf_NGPON2_rem_group(ptin_NGPON2group_t *group_info);

/**
 * Check if a NGPON2 group already exists 
 * 
 * @param group_idx       
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */ 
extern L7_RC_t ptin_intf_NGPON2_group_exists(L7_uint8 group_idx);



/**
 * PTIN_INTF NGPON2 Add Group Port
 *  
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 *  
 *  
 * @author Hugo Araujo: hugo-f-araujo@telecom.pt 
 */
extern L7_RC_t ptin_intf_NGPON2_add_group_port(ptin_NGPON2group_t *group_info);


/**
 * PTIN_INTF NGPON2 Remove Group Port
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 *  
 * @author Hugo Araujo: hugo-f-araujo@telecom.pt  
 */
extern L7_RC_t ptin_intf_NGPON2_rem_group_port(ptin_NGPON2group_t *group_info);

/**
 * PTIN_INTF NGPON2 clear 
 * 
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */

L7_RC_t ptin_intf_NGPON2_clear();


/**
 * PTIN_INTF NGPON2 check intf
 *  
 * @brief check if a physical port belongs to a NGPON2 group 
 *  
 */

L7_RC_t ptin_intf_NGPON2_group_check(L7_uint8 intf_index, L7_uint8 *group_index);

/**
 * PTIN_INTF get NGPON2 group info 
 * 
 * @param group_info      : Pointer to struct with group info 
 * @param group_index     : NGPON2 group index                  
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
extern L7_RC_t get_NGPON2_group_info(ptin_NGPON2_groups_t *NGPON2_GROUP, L7_uint8 group_index);

extern L7_RC_t set_NGPON2_group_info(ptin_NGPON2_groups_t *group_info, L7_uint8 group_index);

#endif /*NGPON2_SUPPORTED*/


extern L7_RC_t ptin_intf_protection_cmd(L7_uint slot, L7_uint port, L7_uint cmd);
extern L7_RC_t ptin_intf_protection_cmd_planC(L7_uint slot, L7_uint port, L7_uint cmd);
extern L7_RC_t ptin_intf_protection_cmd_planD(L7_uint slot_old, L7_uint port_old, L7_uint slot_new, L7_uint port_new);

/**
 The following 2 functions identify, respectively
 @application/DTL level and @ANDL/hapi level the (static)
             interfaces constituing the internal (stati) LAG
             between TA48 and CXO/matrix.
 A LAG is a P2P structure. That's not exactly what we have, but
 TA48 "sees" a LAG with an (in future 2) interface to CXO slot1
 and another (interface) to CXO slot20. Every circuit/EVC
 enclosing CXOs uses this LAG. Manipulation of this LAG is done
 exclusively by TA48's matrix protection mechanism (so, no LACP
 nor any other one whatsoever).
 */
extern L7_BOOL ptin_intf_is_internal_lag_member(L7_uint32 ptin_port);
extern int dapi_usp_is_internal_lag_member(DAPI_USP_t *dusp);

#if (PTIN_BOARD == PTIN_BOARD_TA48GE)
#define LED_COLOR_OFF     0
#define LED_COLOR_GREEN   1
#define LED_COLOR_RED     2
#define LED_COLOR_YELLOW  3

/**
 * Control board leds
 * 
 * @param port 
 * @param color 
 * @param blink 
 */
extern void ptin_ta48ge_led_control(L7_uint32 port, L7_uint8 color, L7_uint8 blink);

/**
 * Control laser txdisable
 * 
 * @param port 
 * @param state 
 */
extern void ptin_ta48ge_txdisable_control(L7_uint32 port, L7_uint8 state);

#endif

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Restore DAI Setting for Internal Interfaces
 *  
 * @param none
 *  
 * @return none
 */
extern void ptin_intf_dai_restore_defaults(void);
#endif

#endif  /* _PTIN_INTERFACE_H */


