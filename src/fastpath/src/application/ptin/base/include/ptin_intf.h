#ifndef _PTIN_INTERFACE_H
#define _PTIN_INTERFACE_H

#include "datatypes.h"
#include "ptin_structs.h"

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
extern L7_RC_t ptin_intf_init(void);

/**
 * Init Port exitension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_init(void);

/**
 * Set Port exitension definitions
 * 
 * @param ptin_intf : Interface
 *        mefExt    : MEF Extension parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_set(const ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt);

/**
 * Get Port exitension definitions
 * 
 * @param ptin_intf : Interface
 *        mefExt    : MEF Extension parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_get(const ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt);

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
 * @param portStats Structure to save port counters (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_counters_read(ptin_HWEthRFC2819_PortStatistics_t *portStats);

/**
 * Clear counters
 * 
 * @param portStats portStats->Port must be defined
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_counters_clear(ptin_HWEthRFC2819_PortStatistics_t *portStats);

/**
 * Read counters activity (of physical ports)
 * 
 * @param portActivity Structure to save port counters activity (at the 
 * moment, masks are ignored, therefore all values are read for all ports) 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_intf_counters_activity_get(ptin_HWEth_PortsActivity_t *portActivity);

/*
 * Board management
 */ 

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
 * Converts PTin port mapping (including LAGs) to the FP interface#
 * 
 * @param ptin_port PTin port index
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_port2intIfNum(L7_uint32 ptin_port, L7_uint32 *intIfNum);

/**
 * Converts FP interface# to PTin port mapping (including LAGs)
 * 
 * @param intIfNum  FP intIfNum
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_intIfNum2port(L7_uint32 intIfNum, L7_uint32 *ptin_port);

/**
 * Converts ptin_port index to PTin port type and id
 * 
 * @param ptin_port PTin port index
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_port2ptintf(L7_uint32 ptin_port, ptin_intf_t *ptin_intf);

/**
 * Converts PTin port type and id to ptin_port index
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_ptintf2port(const ptin_intf_t *ptin_intf, L7_uint32 *ptin_port);

/**
 * Converts FP interface# to PTin port type and id
 * 
 * @param intIfNum  FP intIfNum
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_intIfNum2ptintf(L7_uint32 intIfNum, ptin_intf_t *ptin_intf);

/**
 * Converts PTin port type and id to FP interface#
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_ptintf2intIfNum(const ptin_intf_t *ptin_intf, L7_uint32 *intIfNum);

/**
 * Converts LAG index [1..PTIN_SYSTEM_N_LAGS] to FP intIfNum
 * 
 * @param intIfNum  FP intIfNum
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_lag2intIfNum(L7_uint32 lag_idx, L7_uint32 *intIfNum);

/**
 * Convert intIfNum to LAG index
 * 
 * @param intIfNum  FP intIfNum
 * @param lag_idx   LAG index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_intIfNum2lag(L7_uint32 intIfNum, L7_uint32 *lag_idx);

#if PTIN_BOARD_IS_MATRIX
/**
 * Converts Slot to LAG index [0..PTIN_SYSTEM_N_LAGS[
 * 
 * @param slot      slot
 * @param lag_idx   LAG index [0..PTIN_SYSTEM_N_LAGS[ (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern inline L7_RC_t ptin_intf_slot2lagIdx(L7_uint16 slot, L7_uint32 *lag_idx);
#endif

/**
 * Check if a LAG is created [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @param lag_idx   LAG index [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @return L7_RC_t L7_TRUE/L7_FALSE
 */
extern inline L7_RC_t ptin_intf_lag_exists(L7_uint32 lag_idx);


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
 * @param lagInfo Structure that references the LAG to destroy
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_Lag_delete(ptin_LACPLagConfig_t *lagInfo);

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
 * Configures priority mapping to classes of services 
 * 
 * @param intf : interface
 * @param intfQos : priority map
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_intf_config_set(const ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos);

/**
 * Read interface properties for QoS
 * 
 * @param intf : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_intf_config_get(const ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos);

/**
 * Configures a class of service
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_cos_config_set(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

/**
 * Reads a class of service QoS configuration
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_cos_config_get(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

/**
 * Activate PRBS generator/checker
 *  
 * @param intIfNum : Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pcs_prbs_enable(L7_uint32 intIfNum, L7_BOOL enable);

/**
 * Read number of PRBS errors
 *  
 * @param intIfNum : Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pcs_prbs_errors_get(L7_uint32 intIfNum, L7_uint32 *counter);

/**
 * Configure Default VLANs using VCAP rules
 * 
 * @param intIfNum 
 * @param outerVlan 
 * @param innerVlan  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_intf_vcap_defvid(L7_uint32 intIfNum, L7_uint16 outerVlan, L7_uint16 innerVlan);

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
 * @param intIfNum 
 * @param frame_size 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
extern L7_RC_t ptin_intf_frame_oversize_set(L7_uint32 intIfNum, L7_uint32 frame_size);

/**
 * Read Maximum frame size
 * 
 * @param intIfNum 
 * @param frame_size (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
extern L7_RC_t ptin_intf_frame_oversize_get(L7_uint32 intIfNum, L7_uint32 *frame_size);

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
 * read linkscan status
 *  
 * @param intIfNum : Interface
 * @param enable : enable (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkscan_get(L7_uint32 intIfNum, L7_uint8 *enable);

/**
 * Apply linkscan procedure
 *  
 * @param intIfNum : Interface 
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_linkscan_set(L7_uint32 intIfNum, L7_uint8 enable);

/**
 * Apply linkscan procedure
 *  
 * @param intIfNum : Interface
 * @param link : link_status 
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_link_force(L7_uint32 intIfNum, L7_uint8 link, L7_uint8 enable);

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
extern L7_BOOL ptin_intf_is_internal_lag_member(L7_uint32 intIfNum);
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

#endif  /* _PTIN_INTERFACE_H */


