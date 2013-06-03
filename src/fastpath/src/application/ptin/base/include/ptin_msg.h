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
extern void ptin_msg_defaults_reset(void);

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
extern L7_RC_t ptin_msg_l2_macTable_get(msg_switch_mac_table_t *mac_table);

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
extern L7_RC_t ptin_msg_EVC_delete(msg_HwEthMef10Evc_t *msgEvcConf);

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
extern L7_RC_t ptin_msg_bwProfile_set(msg_HwEthBwProfile_t *msgBwProfile);

/**
 * Remove an existent bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_bwProfile_delete(msg_HwEthBwProfile_t *msgBwProfile);

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

/* IGMP Management Functions **************************************************/
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
 * Add a static group channel to MFDB table
 * 
 * @param channel : static group channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_staticChannel_add(msg_MCStaticChannel_t *channel);

/**
 * Remove a static group channel from MFDB table
 * 
 * @param channel : static group channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_channel_remove(msg_MCStaticChannel_t *channel);

/**
 * Consult list of multicast channels
 * 
 * @param channel_list : list of multicast channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_channelList_get(msg_MCActiveChannels_t *channel_list);

/**
 * Consult list of clients of a particular multicast channel
 * 
 * @param client_list : list of client vlans
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_IGMP_clientList_get(msg_MCActiveChannelClients_t *client_list);

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

#endif /* _PTIN_MSG_H */
