/**
 * ptin_msg.h
 *
 * Created on:
 * Author:
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_MSG_H
#define _PTIN_MSG_H

#include "ptin_include.h"
#include "ptin_msghandler.h"

typedef enum
{
  PTIN_MSG_OPER_NONE = 0,
  PTIN_MSG_OPER_INIT,
  PTIN_MSG_OPER_ADD,
  PTIN_MSG_OPER_REMOVE,
  PTIN_MSG_OPER_CLEAR,
  PTIN_MSG_OPER_DESTROY,
  PTIN_MSG_OPER_DEINIT
} ptin_msg_oper_t;

/******************************************************** 
 * EXTERNAL FUNCTIONS PROTOTYPES
 ********************************************************/

/* FastPath Misc Functions ****************************************************/ 
/**
 * Execute drivshell or devshell command string
 * 
 * @param str Input string used to call driv or devshell
 * 
 * @return L7_RC_t Return code L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_ShellCommand_run(L7_char8 *str);

/**
 * Gets general info about FastPath firmware
 * 
 * @param msgFPInfo Pointer to output structure
 * 
 * @return L7_RC_t L7_SUCCESS (always)
 */
extern L7_RC_t ptin_msg_FPInfo_get(msg_FWFastpathInfo *msgFPInfo);


/* Reset Functions ************************************************************/
/**
 * Reset to default configuration 
 *  
 * Actions: 
 *  - EVCs are destroyed (including counter, bw profiles, clientes, etc)
 */
extern void ptin_msg_defaults_reset(L7_char8 mode);

/**
 * Reset Multicast Machine
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_multicast_reset(msg_HwGenReq_t *msg);

/**
 * TYPE B Protection interface switch notification
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_typeBprotIntfSwitchNotify(msg_HwTypeBProtSwitchNotify_t *msg);

/**
 * TYPE B Protection Interface Configuration
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_typeBprotIntfConfig(msg_HwTypeBProtIntfConfig_t *msg);

/**
 * TYPE B Protection Switching
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_typeBprotSwitch(msg_HwTypeBprot_t *msg);

/**
 * Apply linkscan procedure
 * 
 * @param msg : message
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_board_action(msg_HwGenReq_t *msg);

/**
 * Apply linkscan procedure
 * 
 * @param msg : message
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_link_action(msg_HwGenReq_t *msg);

/**
 * Reset alarms state
 *  
 */
extern void ptin_msg_alarms_reset(void);

/* Resources ******************************************************************/ 
/**
 * Read hardware resources
 * 
 * @param msgResources : structure with the availanle resources 
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_msg_hw_resources_get(msg_ptin_policy_resources *msgResources);

/* Physical Interfaces Functions **********************************************/ 
/**
 * Set physical port configuration
 * 
 * @param msgPhyConf Structure with the configuration to be set
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyConfig_set(msg_HWEthPhyConf_t *msgPhyConf);

/**
 * Get physical port configuration
 * 
 * @param msgPhyConf Structure to save port configuration (Port 
 * field MUST be set; Output mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyConfig_get(msg_HWEthPhyConf_t *msgPhyConf);

/**
 * Get physical port state
 * 
 * @param msgPhyState Structure to save port state (Port 
 * field MUST be set; Outut mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyState_get(msg_HWEthPhyState_t *msgPhyState);

/**
 * Get physical port activity
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyActivity_get(msg_HWEthPhyActivity_t *msgPhyAct);

/**
 * Get physical port state
 * 
 * @param msgPhyState Structure to save port state (Port 
 * field MUST be set; Outut mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyStatus_get(msg_HWEthPhyStatus_t *msgPhyStatus);

/* Counters Manipulation Functions ********************************************/ 
/**
 * Read PHY counters
 * 
 * @param msgPortStats : Array of stats (one for each port) 
 * @param msgRequest   : Array of requests (one for each port) 
 * @param numElems     : Number of elements to be read
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyCounters_read(msg_HwGenReq_t *msgRequest, msg_HWEthRFC2819_PortStatistics_t *msgPortStats, L7_uint nElems);

/**
 * Clear PHY counters
 * 
 * @param portStats portStats.Port must defined the port#
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_PhyCounters_clear(msg_HWEthRFC2819_PortStatistics_t *msgPortStats);

/* Slot mode configuration ****************************************************/

/**
 * Get all interfaces info
 * 
 * @param intf_info 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_intfInfo_get(msg_HwIntfInfo_t *intf_info);

/**
 * Get slot mode configuration
 * 
 * @param slotMode 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_slotMode_get(msg_slotModeCfg_t *slotMode);

/**
 * Validate slot mode configuration
 * 
 * @param slotMode 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_slotMode_validate(msg_slotModeCfg_t *slotMode);

/**
 * Apply new slot mode configuration by rebboting application
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_slotMode_apply(void);


/* Port Type Functions ********************************************************/ 

/**
 * Define Port Type settings
 * 
 * @param mefExt : Mef Extension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_portExt_set(msg_HWPortExt_t *mefExt, L7_uint nElems);

/**
 * Read Port Type settings
 * 
 * @param mefExt : Mef Extension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_portExt_get(msg_HWPortExt_t *mefExt, L7_uint *nElems);

/**
 * Set MAC address to interfaces
 * 
 * @param portMac : MAC definitions 
 * @param nElems  : Number of MACs to be attributed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_portMAC_set(msg_HWPortMac_t *portMac, L7_uint nElems);

/**
 * Get MAC addresses for the given interfaces
 * 
 * @param portMac : MAC definitions 
 * @param nElems  : Number of MACs to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_portMAC_get(msg_HWPortMac_t *portMac, L7_uint *nElems);

/* CoS Functions *************************************************************/ 
/**
 * Get CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_CoS_get(msg_QoSConfiguration_t *qos_config);

/**
 * Redefine CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_CoS_set(msg_QoSConfiguration_t *qos_config);


/* LAGs Manipulation Functions ************************************************/ 
/**
 * Gets one or all LAGs info
 * 
 * @param lagInfo Pointer to the output structure (or array)
 * @param nElems  Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_Lag_get(msg_LACPLagInfo_t *lagInfo, L7_uint *nElems);

/**
 * Creates a LAG
 * 
 * @param lagInfo Pointer to the structure with LAG info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_Lag_create(msg_LACPLagInfo_t *lagInfo);

/**
 * Deletes a LAG
 * 
 * @param lagInfo Structure that references the LAG to destroy
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_Lag_destroy(msg_LACPLagInfo_t *lagInfo);

/**
 * Gets one or all LAGs status
 * 
 * @param lagStatus Pointer to the output structure (or array)
 * @param nElems    Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_LagStatus_get(msg_LACPLagStatus_t *lagStatus, L7_uint *nElems);

/**
 * Sets one or all Ports LACP Admin State
 * 
 * @param adminState Pointer to the input structure (or array)
 * @param nElems     Number of elements in the array of structures
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_LACPAdminState_set(msg_LACPAdminState_t *adminState, L7_uint nElems);

/**
 * Gets one or all Ports LACP Admin State
 * 
 * @param adminState Pointer to the output structure (or array)
 * @param nElems     Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_LACPAdminState_get(msg_LACPAdminState_t *adminState, L7_uint *nElems);

/**
 * Gets one or all Ports LACP statistics info
 * 
 * @param lagStats Pointer to the output structure (or array)
 * @param nElems   Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_LACPStats_get(msg_LACPStats_t *lagStats, L7_uint *nElems);

/**
 * Clears one or all Ports LACP statistics info
 * 
 * @param lagStats Structure that references the Port# to clear
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_LACPStats_clear(msg_LACPStats_t *lagStats);

/* L2 Manipulation functions **************************************************/

/**
 * Sets L2 aging time
 * 
 * @param switch_config Pointer to configuration structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_l2_switch_config_set(msg_switch_config_t *switch_config);

/**
 * Gets L2 aging time
 * 
 * @param switch_config Pointer to configuration structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_l2_switch_config_get(msg_switch_config_t *switch_config);

/**
 * Shows L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_l2_macTable_get(msg_switch_mac_table_t *mac_table, int struct1or2);

/**
 * Remove an address from the L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_l2_macTable_remove(msg_switch_mac_table_t *mac_table);

/**
 * Add an address to the L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_l2_macTable_add(msg_switch_mac_table_t *mac_table);

/* EVCs Manipulation Functions ************************************************/ 
/**
 * Gets an EVC configuration
 * 
 * @param msgEvcConf Pointer to the output struct (index field is used as input param)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVC_get(msg_HwEthMef10Evc_t *msgEvcConf);

/**
 * Creates or reconfigures an EVC
 * 
 * @param msgEvcConf Pointer to the input struct
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVC_create(msg_HwEthMef10Evc_t *msgEvcConf);

/**
 * Deletes an EVC
 * 
 * @param msgEvcConf Pointer to the input struct (index field must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVC_delete(msg_HwEthMef10EvcRemove_t *msgEvcConf, L7_uint16 n_structs);

/**
 * Add/remove port to/from an EVC
 * 
 * @param msgEvcPort : Pointer to the input struct 
 * @param n_size     : Number of structures 
 * @param oper       : Operation type
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_evc_port(msg_HWevcPort_t *msgEvcPort, L7_uint16 n_size, ptin_msg_oper_t oper);

/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param msgEvcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVCBridge_add(msg_HwEthEvcBridge_t *msgEvcBridge);

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface
 * 
 * @param msgEvcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVCBridge_remove(msg_HwEthEvcBridge_t *msgEvcBridge);

/**
 * Adds an flow to an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVCFlow_add(msg_HwEthEvcFlow_t *msgEvcFlow);

/**
 * Removes a flow from an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EVCFlow_remove(msg_HwEthEvcFlow_t *msgEvcFlow);

/**
 * Adds a flooding vlan applied to an EVC
 * 
 * @param msgEvcFlood : Flooding vlan info 
 * @param n_clients   : Number of vlans to be added
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EvcFloodVlan_add(msg_HwEthEvcFloodVlan_t *msgEvcFlood, L7_uint n_clients);

/**
 * Removes a flooding vlan applied to an EVC
 * 
 * @param msgEvcFlood : Flooding vlan info 
 * @param n_clients   : Number of vlans to be removed
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_EvcFloodVlan_remove(msg_HwEthEvcFloodVlan_t *msgEvcFlood, L7_uint n_clients);


/* Bandwidth profiles **************************************/

/**
 * Get data of an existent bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_bwProfile_get(msg_HwEthBwProfile_t *msgBwProfile);

/**
 * Add a new bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_bwProfile_set(msg_HwEthBwProfile_t *msgBwProfile, unsigned int msgId);

/**
 * Remove an existent bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_bwProfile_delete(msg_HwEthBwProfile_t *msgBwProfile, unsigned int msgId);

/**
 * Get data of an existent storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_stormControl_get(msg_HwEthStormControl_t *msgStormControl);

/**
 * Configure storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_stormControl_set(msg_HwEthStormControl_t *msgStormControl);

/**
 * Reset storm control profile
 * 
 * @param msgStormControl : 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_stormControl_reset(msg_HwEthStormControl_t *msgStormControl);

/**
 * Clear storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_stormControl_clear(msg_HwEthStormControl_t *msgStormControl);

/* EVC Statistics *********************************************************/

/**
 * Get Traffic Statistics of a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_evcStats_get(msg_evcStats_t *msg_evcStats);

/**
 * Allocate statistics counting for a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_evcStats_set(msg_evcStats_t *msg_evcStats);

/**
 * Deallocate statistics counting for a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_evcStats_delete(msg_evcStats_t *msg_evcStats);

/* Network Connectivity (inBand) Functions ************************************/
/**
 * Gets Network Connectivity configuration
 * 
 * @param msgNtwConn Pointer to the output data (mask defines what to read)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_ntw_connectivity_get(msg_NtwConnectivity_t *msgNtwConn);

/**
 * Sets Network Connectivity configuration
 * 
 * @param msgNtwConn Pointer to the config data (mask defines what to set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_ntw_connectivity_set(msg_NtwConnectivity_t *msgNtwConn);

/* DHCP Management Functions **************************************************/

/**
 * Reconfigure a global DHCP EVC
 *
 * @param dhcpEvcInfo: DHCP EVC info
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_evc_reconf(msg_DhcpEvcReconf_t *dhcpEvcInfo);

/**
 * Set DHCP circuit-id global data
 *
 * @param profile: DHCP profile
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_circuitid_set(msg_AccessNodeCircuitId_t *circuitid);

/**
 * Get DHCP circuit-id global data
 *
 * @param profile: DHCP profile
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_circuitid_get(msg_AccessNodeCircuitId_t *circuitid);

/**
 * Get DHCP profile data
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_profile_get(msg_HwEthernetDhcpOpt82Profile_t *profile);

/**
 * Add a new DHCP profile
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_profile_add(msg_HwEthernetDhcpOpt82Profile_t *profile, L7_uint n_clients);

/**
 * Remove DHCP profile
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_profile_remove(msg_HwEthernetDhcpOpt82Profile_t *profile, L7_uint n_clients);

/**
 * Get DHCP counters at a specific client
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_clientStats_get(msg_DhcpClientStatistics_t *dhcp_stats);

/**
 * Clear DHCP counters at a specific client
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_clientStats_clear(msg_DhcpClientStatistics_t *dhcp_stats);

/**
 * Get DHCP counters at a specific interface
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_intfStats_get(msg_DhcpClientStatistics_t *dhcp_stats);

/**
 * Clear DHCP counters at a specific interface
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_intfStats_clear(msg_DhcpClientStatistics_t *dhcp_stats);

/**
 * Get DHCP bind table
 *
 * @param table: bind table entries
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCPv4v6_bindTable_get(msg_DHCP_bind_table_request_t *input, msg_DHCPv4v6_bind_table_t *output);

/**
 * Remove a DHCP bind table entry
 * 
 * @param table: bind table entries
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_DHCP_bindTable_remove(msg_DHCPv4v6_bind_table_t *table);

/* IPSG Management Functions **************************************************/

/**
 * Configure IP Source Guard on Ptin Port
 * 
 * @param msgIpsgVerifySource Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_verify_source_set(msg_IPSG_set_t* msgIpsgVerifySource);

/**
 * Configure an IP Source Guard  static entry
 * 
 * @param msg_IPSG_static_entry_t Structure with config 
 *                                parameters
 * @param n_msg : number of structs 
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_static_entry_set(msg_IPSG_static_entry_t* msgIpsgStaticEntry, L7_uint16 n_msg);

/**
 * Get IP Source Guard binding table
 *
 * @param table: bind table entries
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_binding_table_get(msg_ipsg_binding_table_request_t *input, msg_ipsg_binding_table_response_t *output);

/* IGMP Management Functions **************************************************/

/**
* @purpose Set the IGMP Admission Control 
*          Configuration
*  
* @param  msg_IgmpAdmissionControl : Structure with config 
*                                  parameters
*
* @return L7_RC_t L7_SUCCESS/L7_FAILURE
*
* @notes This routine will support configuring the admission 
*        control parameters on the interface, on the evc id, and
*        on the igmp client
*/
L7_RC_t ptin_msg_igmp_admission_control_set(msg_IgmpAdmissionControl_t *igmpAdmissionControl);

/**
 * Applies IGMP Proxy configuration
 * 
 * @param msgIgmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_proxy_set(msg_IgmpProxyCfg_t *msgIgmpProxy);

/**
 * Gets IGMP Proxy configuration
 * 
 * @param msgIgmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_proxy_get(msg_IgmpProxyCfg_t *msgIgmpProxy);

/**
 * Creates/updates an IGMP instance (interfaces/VLANs)
 *
 * @param msgIgmpIntf Structure with config parameters
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_instance_add(msg_IgmpMultcastUnicastLink_t *msgIgmpInst);

/**
 * Deletes an IGMP instance (interfaces/VLANs)
 *
 * @param msgIgmpIntf Structure with config parameters (router VLAN must be set)
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_instance_remove(msg_IgmpMultcastUnicastLink_t *msgIgmpInst);

/**
 * Add a Multicast client to a MC EVC
 *
 * @param McastClient : Multicast client info 
 * @param n_clients   : Number of clients 
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_client_add(msg_IgmpClient_t *McastClient, L7_uint16 n_clients);

/**
 * Remove a Multicast client from a MC EVC
 *
 * @param McastClient : Multicast client info 
 * @param n_clients   : Number of clients 
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_igmp_client_delete(msg_IgmpClient_t *McastClient, L7_uint16 n_clients);

/**
 * Get Client IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_clientStats_get(msg_IgmpClientStatistics_t *igmp_stats);

/**
 * Clear Client IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_clientStats_clear(msg_IgmpClientStatistics_t *igmp_stats, uint16 n_clients);

/**
 * Get interface IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_intfStats_get(msg_IgmpClientStatistics_t *igmp_stats);

/**
 * Clear interface IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_intfStats_clear(msg_IgmpClientStatistics_t *igmp_stats, uint16 n_clients);

/**
 * Get list of channels contained in the white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_ChannelAssoc_get(msg_MCAssocChannel_t *channel_list, L7_uint16 *n_channels);

/**
 * Add channels to White list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_ChannelAssoc_add(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels);

/**
 * Remove channels to white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_ChannelAssoc_remove(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels);

/**
 * Remove all channels to white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_ChannelAssoc_remove_all(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels);

/**
 * Add a static group channel to MFDB table
 * 
 * @param channel : static group channel
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_staticChannel_add(msg_MCStaticChannel_t *channel, L7_uint16 n_channels);

/**
 * Remove a static group channel from MFDB table
 * 
 * @param channel : static group channel
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_channel_remove(msg_MCStaticChannel_t *channel, L7_uint16 n_channels);

/**
 * Consult list of multicast channels
 * 
 * @param channel_list : list of multicast channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_channelList_get(msg_MCActiveChannelsRequest_t *inputPtr, msg_MCActiveChannelsReply_t *outputPtr, L7_uint16 *numberOfChannels);

/**
 * Process Snoop Sync Request Message
 * 
 * @param msg_SnoopSyncRequest_t : 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_snoop_sync_request(msg_SnoopSyncRequest_t *snoopSyncRequest);

/**
 * Process Snoop Sync Reply Message
 * 
 * @param msg_SnoopSyncReply_t : 
 * @param numberOfSnoopEntries :  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_snoop_sync_reply(msg_SnoopSyncReply_t *snoopSyncReply, L7_uint32 numberOfSnoopEntries);

/**
 * Consult list of clients of a particular multicast channel
 * 
 * @param client_list : list of client vlans
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_clientList_get(msg_MCActiveChannelClients_t *client_list);

/**
 * Remove all static group channel from MFDB table
 * 
 * @param channel : static group channel
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_channel_remove_all(msg_MCStaticChannel_t *channel, L7_uint16 n_channels);

/**
 * Uplink protection command
 * 
 * @param cmd : command array
 * @param n : number of commands
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_uplink_protection_cmd(msg_uplinkProtCmd *cmd, L7_int n);

/**
 * Sync MGMD open ports
 * 
 * @param port_sync_data : MGMD port to sync
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_mgmd_sync_ports(msg_HwMgmdPortSync *port_sync_data);

/**
 * Enable PRBS tx/rx
 * 
 * @param msg : PRBS configuration
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_pcs_prbs_enable(msg_ptin_pcs_prbs *msg, L7_int n_msg);

/**
 * Read PRBS errors
 * 
 * @param msg : PRBS configuration
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_pcs_prbs_status(msg_ptin_pcs_prbs *msg, L7_int n_msg);

/**
 * Enable PRBS tx/rx
 * 
 * @param msg : PRBS configuration
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_prbs_enable(msg_ptin_prbs_enable *msg, L7_int n_msg);

/**
 * Read PRBS errors
 * 
 * @param msg_in : PRBS input
 * @param msg_out : PRBS results 
 * @param n_msg : Number of structures 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_prbs_status(msg_ptin_prbs_request *msg_in, msg_ptin_prbs_status *msg_out, L7_int *n_msg);

/**
 * Used to create a new MEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_wr_MEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i);

/**
 * Used to remove a MEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_del_MEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i);

/**
 * Used to create a new RMEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_wr_RMEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i);

/**
 * Used to remove a RMEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_del_RMEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i);

/**
 * Used to dump MEPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_dump_MEPs(ipc_msg *inbuff, ipc_msg *outbuff);

/**
 * Used to dump MPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_dump_MEs(ipc_msg *inbuff, ipc_msg *outbuff);

/**
 * Used to dump LUT MEPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
extern L7_RC_t ptin_msg_dump_LUT_MEPs(ipc_msg *inbuff, ipc_msg *outbuff);


/****************************************************************************** 
 * ERPS Configuration
 ******************************************************************************/

/**
 * ERPS Configuration
 * 
 * @author joaom (6/4/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_erps_set(msg_erps_t *ptr);

/**
 * ERPS removal
 * 
 * @author joaom (6/22/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_erps_del(msg_erps_t *msgErpsConf);

/**
 * ERPS Reconfiguration
 * 
 * @author joaom (6/22/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_erps_config(msg_erps_t *msgErpsConf);

/**
 * ERPS status
 * 
 * @author joaom (6/24/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_erps_status(msg_erps_status_t *msgErpsStatus);

/**
 * ERPS status
 * 
 * @author joaom (6/24/2013)
 * 
 * @param ptr 
 * @param n
 * 
 * @return L7_RC_t 
 */
extern int ptin_msg_erps_status_next(msg_erps_status_t *msgErpsStatus, L7_int *n);

/**
 * ERPS Commands
 * 
 * @author joaom (7/12/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_msg_erps_cmd(msg_erps_cmd_t *msgErpsCmd);


/****************************************************************************** 
 * ACL Configuration
 ******************************************************************************/

/**
 * ACL Rule Configuration
 * 
 * @param msgAcl : Pointer to beginning of data
 * @param msgId : operation 
 * @param msgDim : Dimension of data 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_rule_config(void *msgAcl, L7_uint msgId, L7_uint msgDim);

/**
 * ACL Enable/Disable
 * 
 * @author joaom (11/01/2013)
 * 
 * @param msgAcl : Pointer to data 
 * @param msgId : Operation 
 * @param n_msg : Number of structs
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_enable(msg_apply_acl_t *msgAcl, L7_uint msgId, L7_uint n_msg);


extern int msg_wr_802_1x_Genrc(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i);
//#define msg_wr_802_1x_AdminMode     msg_wr_802_1x_Genrc
//#define msg_wr_802_1x_TraceMode     msg_wr_802_1x_Genrc
//#define msg_wr_802_1x_VlanAssgnMode msg_wr_802_1x_Genrc
//#define msg_wr_802_1x_MonMode       msg_wr_802_1x_Genrc
//#define msg_wr_802_1x_DynVlanMode   msg_wr_802_1x_Genrc

extern int msg_wr_802_1x_Genrc2(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i);

extern int msg_wr_802_1x_AuthServ(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i);

/**
 * This routine is a place holder to trigger events that require 
 * the protection matrix  to be in the state of end 
 * of flush configuration 
 * 
 */
extern void ptin_msg_protection_matrix_configuration_flush_end(void);

/****************************************************************************** 
 * Routing
 ******************************************************************************/

/**
 * Create new routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_create(msg_RoutingIntf* data);

/**
 * Modify an existing routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_modify(msg_RoutingIntf* data);

/**
 * Remove an existing routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_remove(msg_RoutingIntf* data);

/**
 * Get ARP table.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_arptable_get(msg_RoutingArpTableRequest* inBuffer, msg_RoutingArpTableResponse* outBuffer, L7_uint32* readEntries);

/**
 * Delete ARP entry.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_arpentry_purge(msg_RoutingArpEntryPurge* data);

/**
 * Get route table.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param maxEntries
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_routetable_get(msg_RoutingRouteTableRequest* inBuffer, msg_RoutingRouteTableResponse* outBuffer, L7_uint32 maxEntries, L7_uint32* readEntries);

/**
 * Configure a static route.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_staticroute_add(msg_RoutingStaticRoute* data);

/**
 * Delete an existing static route.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_staticroute_delete(msg_RoutingStaticRoute* data);

/**
 * Start a ping request.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_create(msg_RoutingPingSessionCreate* data);

/**
 * Get ping session status.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_query(msg_RoutingPingSessionQuery* data);

/**
 * Free existing ping session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_free(msg_RoutingPingSessionFree* data);

/**
 * Start a traceroute session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_create(msg_RoutingTracertSessionCreate* data);

/**
 * Get traceroute session status.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_query(msg_RoutingTracertSessionQuery* data);

/**
 * Get current hops of a given traceroute session.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param maxEntries
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_gethops(msg_RoutingTracertSessionHopsRequest* inBuffer, msg_RoutingTracertSessionHopsResponse* outBuffer, L7_uint32 maxEntries, L7_uint32* readEntries);

/**
 * Free existing traceroute session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_free(msg_RoutingTracertSessionFree* data);


#endif /* _PTIN_MSG_H */

