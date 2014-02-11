/**
 * ptin_evc.h 
 *  
 * Implements the EVC configuration and management 
 *
 * Created on: 2011/07/15
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_EVC_H
#define _PTIN_EVC_H

#include "ptin_include.h"
#include "ptin_igmp.h"

/**
 * Initializes EVCs data structures
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_init(void);


/******************************************************** 
 * FOR MESSAGES PROCESSING
 ********************************************************/

/**
 * Gets an EVC configuration
 * 
 * @param evcConf Pointer to the output struct (index field is used as input param)
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Creates or reconfigures an EVC
 * 
 * @param evcConf Pointer to the input struct
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Add port to an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_port_add(L7_uint evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf);

/**
 * Remove port from an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_port_remove(L7_uint evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf);

/**
 * Deletes an EVC
 * 
 * @param evc_id
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_delete(L7_uint evc_ext_id);

/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_destroy(L7_uint evc_ext_id);

/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_destroy_all(void);

/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_p2p_bridge_add(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_p2p_bridge_remove(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Adds a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted  packets
 * @param inner_vlan  : inner vlan of transmitted  packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_flood_vlan_add( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                        L7_uint16 outer_vlan, L7_uint16 inner_vlan );

/**
 * Removes a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted packets
 * @param inner_vlan  : inner vlan of transmitted packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_flood_vlan_remove( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                           L7_uint16 outer_vlan, L7_uint16 inner_vlan );

/**
 * Storm control configurations
 */

/**
 * Initializes Storm Control configurations for all vlans
 * 
 * @return L7_RC_t: L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_init(void);

/**
 * Get storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_get(ptin_stormControl_t *stormControl);

/**
 * Set storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_set(L7_BOOL enable, ptin_stormControl_t *stormControl);

/**
 * Reset storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_reset(ptin_stormControl_t *stormControl);

/**
 * Bandwidth Policers management functions
 */

/**
 * Read data of a bandwidth profile
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile (input and output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_get(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile);

/**
 * Apply a bandwidth profile to an EVC and (optionally) to a 
 * specific client 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_set(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile);

/**
 * Remove a bandwidth profile to an EVC 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_delete(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile);

/**
 * EVC Counters management functions
 */

/**
 * Read RX statistics of a particular EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile 
 * @param stats      : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_get(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats);

/**
 * Add a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_set(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile);

/**
 * Delete a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_delete(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile);


/******************************************************** 
 * FOR PTIN INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Determines if a particular EVC is in use
 * 
 * @param evc_ext_id : EVC extended id
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_in_use(L7_uint32 evc_ext_id);

/**
 * Determines if a particular Port/LAG is being used on any EVC
 * 
 * @param intf_idx PTin interface#
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_intf_in_use(L7_uint intf_idx);

/**
 * Get interface configuration within an EVC
 *  
 * @param evc_ext_id : EVC extended id
 * @param ptin_intf  : PTin interface 
 * @param cfg        : interface configuration 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intfCfg_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_evc_intfCfg_t *cfg);

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct 
 *  
 * @note The returned EVC id will be the extended one 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 internalVlan, ptin_HwEthMef10Evc_t *evcConf);

/**
 * Gets the internal vlan for a particular evc and interface
 * 
 * @param evc_ext_id : EVC extended id 
 * @param ptin_intf  : interface
 * @param intVlan    : Internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intVlan_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 *intVlan);

/**
 * Gets the root vlan (internal) for a particular evc
 * 
 * @param evc_ext_id  : EVC extended id 
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intRootVlan_get(L7_uint32 evc_ext_id, L7_uint16 *intRootVlan);


/**
 * Gets flag options for a particular evc
 * 
 * @param evc_ext_id: EVC extended id 
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_flags_get(L7_uint32 evc_ext_id, L7_uint32 *flags, L7_uint32 *mc_flood);

/**
 * Gets flag options for a particular (internal) OVlan
 * 
 * @param intVlan   : Internal OVlan
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_flags_get_fromIntVlan(L7_uint16 intOVlan, L7_uint32 *flags, L7_uint32 *mc_flood);

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_id+innerVlan. 
 *  
 * @param intIfNum   : FP interface#
 * @param evc_ext_id : EVC extended index
 * @param innerVlan  : Inner vlan
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                      is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get(L7_uint32 intIfNum, L7_uint32 evc_ext_id, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan);

#if 0
/**
 * Validate outer vlan
 * 
 * @param intIfNum : Interface
 * @param extOVlan : external outer vlan
 * @param innerVlan: external inner vlan
 * 
 * @return L7_RC_t : L7_SUCCESS if extOVlan is valid 
 *                   L7_NOT_EXIST if extOVlan does not exist
 *                   L7_FAILURE if other error
 */
extern L7_RC_t ptin_evc_extVlan_validate(L7_uint16 evc_id, ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 innerVlan);
#endif 

/**
 * Get internal vlans, from external vlans and the interface
 * 
 * @author mruas (8/6/2013)
 * 
 * @param intIfNum : Interface
 * @param extOVlan : external outer vlan
 * @param extIVlan : external inner vlan
 * @param intOVlan : internal outer vlan
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_evc_intVlan_get_fromOVlan(ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 extIVlan,
                                              L7_uint16 *intOVlan);

/**
 * Get the list of interfaces associated to a internal vlan
 * 
 * @param intVlan  : Internal vlan 
 * @param type     : Interface type 
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intfType_getList(L7_uint16 intVlan, L7_uint8 type, NIM_INTF_MASK_t *intfList);

/**
 * Get next client, belonging to an EVC
 * 
 * @param evc_id      : evc index
 * @param ptin_intf   : interface
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients (ivid_next==0)
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
extern
L7_RC_t ptin_evc_client_next( L7_uint evc_ext_id, ptin_intf_t *ptin_intf, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next);

/**
 * Configures a root port (unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 * 
 * @param root_intf     Root interface (ptin_intf)
 * @param int_vlan      Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_root_unblock(L7_uint root_intf, L7_uint16 int_vlan);

/**
 * Removes a root port (unstacked EVCs) 
 * 
 * @param root_intf Root interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_root_block(L7_uint root_intf, L7_uint16 int_vlan);

/**
 * Flushes FDB for all int.VLAN associated to this evc_id
 * 
 * @param int_vlan  Root Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_fdbFlushByVlan(L7_uint16 int_vlan);

#if PTIN_IGMP_STATS_IN_EVCS
/**
 * Get a pointer to IGMP stats
 * 
 * @param intVlan      : internal vlan
 * @param intIfNum     : interface
 * @param stats_intf   : pointer to stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_get_fromIntVlan(L7_uint16 intVlan, L7_uint32 intIfNum, ptin_IGMP_Statistics_t **stats_intf);

/**
 * Get a pointer to IGMP stats
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf   : interface
 * @param stats_intf  : Stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stats_intf);

/**
 * Clear IGMP stats of one interface
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf   : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_clear(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf);

/**
 * Clear IGMP stats of all interfaces
 * 
 * @param evc_ext_id  : EVC extended index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_clear_all(L7_uint32 evc_ext_id);
#endif

/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint32 *evc_ext_id);

/**
 * Get the outer+inner external vlan for a specific 
 * interface+outer+inner internal vlan. 
 *  
 * @param intIfNum  : FP interface#
 * @param intOVlan   : Internal outer-vlan 
 * @param intIVlan   : Internal inner-vlan (0 means that there 
 *                     is no inner vlan)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get_fromIntVlan(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan);

/**
 * Gets the root vlan (internal) from the internal vlan
 * 
 * @param intVlan     : Internal vlan
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intRootVlan_get_fromIntVlan(L7_uint16 intVlan, L7_uint16 *intRootVlan);

/**
 * Return EVC type. 
 * 
 * @param evc_id_ext : extended evc id
 * @param evc_type   : EVC type (output)
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_evctype(L7_uint32 evc_id_ext, L7_uint *evc_type);

/**
 * Return EVC type from internal vlan. 
 *  
 * @param intVlan    : Internal outer-vlan 
 * @param evc_type   : evc type (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_evctype_fromIntVlan(L7_uint16 intVlan, L7_uint *evc_type);

/**
 * Check if the EVC related to an internal vlan is stacked. 
 *  
 * @param intVlan    : Internal outer-vlan 
 * @param is_stacked : Is EVC stacked? (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_is_stacked_fromIntVlan(L7_uint16 intVlan, L7_BOOL *is_stacked);

/**
 * Validate interface and vlan belonging to a valid interface 
 * inside a valid EVC 
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_evc_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan);

/**
 * Get next client, belonging to a vlan
 * 
 * @param intVid    : internal vlan
 * @param intIfNum   : intIfNum
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
extern
L7_RC_t ptin_evc_vlan_client_next( L7_uint intVid, L7_uint32 intIfNum, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next);

/**
 * Adds a flow to the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flow_add(ptin_HwEthEvcFlow_t *evcFlow);

/**
 * Removes a flow from the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flow_remove(ptin_HwEthEvcFlow_t *evcFlow);

/**
 * Gets the flooding vlans list
 * 
 * @param intIfNum    : leaf interface
 * @param intVlan     : internal Vlan
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : list of outer vlans
 * @param inner_vlan  : list of inner vlans 
 * @param number_of_vlans : Size of returned lists
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flood_vlan_get( L7_uint32 intIfNum, L7_uint intVlan, L7_uint client_vlan,
                                 L7_uint16 *outer_vlan, L7_uint16 *inner_vlan, L7_uint16 *number_of_vlans );

#endif /* _PTIN_EVC_H */
