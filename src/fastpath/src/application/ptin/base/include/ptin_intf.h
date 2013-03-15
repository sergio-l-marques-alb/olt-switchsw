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
#define PHY_PORT_2500_MBPS            4
#define PHY_PORT_10000_MBPS           5


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
extern L7_RC_t ptin_intf_portExt_set(ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt);

/**
 * Get Port exitension definitions
 * 
 * @param ptin_intf : Interface
 *        mefExt    : MEF Extension parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portExt_get(ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt);

/**
 * Set MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portMAC_set(ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac);

/**
 * Get MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_portMAC_get(ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac);

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
 * Port, LAGs and Interfaces convertion functions
 */ 

/**
 * Get current slot_id for this board
 * 
 * @param slot_id : slot index (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_slot_get(L7_uint8 *slot_id);

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
extern L7_RC_t ptin_intf_intIfNum2SlotPort(L7_uint32 intIfNum, L7_uint16 *slot_ret, L7_uint16 *intf_ret);

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
extern inline L7_RC_t ptin_intf_ptintf2port(ptin_intf_t *ptin_intf, L7_uint32 *ptin_port);

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
extern inline L7_RC_t ptin_intf_ptintf2intIfNum(ptin_intf_t *ptin_intf, L7_uint32 *intIfNum);

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
 * Check if a LAG is created [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @param lag_idx   LAG index [1..PTIN_SYSTEM_N_LAGS]
 * 
 * @return L7_RC_t L7_TRUE/L7_FALSE
 */
extern inline L7_RC_t ptin_intf_lag_exists(L7_uint32 lag_idx);


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
extern L7_RC_t ptin_QoS_intf_config_set(ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos);

/**
 * Read interface properties for QoS
 * 
 * @param intf : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_intf_config_get(ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos);

/**
 * Configures a class of service
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_cos_config_set(ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

/**
 * Reads a class of service QoS configuration
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_QoS_cos_config_get(ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

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

#endif  /* _PTIN_INTERFACE_H */


