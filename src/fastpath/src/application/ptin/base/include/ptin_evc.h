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
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Creates or reconfigures an EVC
 * 
 * @param evcConf Pointer to the input struct
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Deletes an EVC
 * 
 * @param evc_idx
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_delete(L7_uint evc_idx);

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
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_stacked_bridge_add(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_stacked_bridge_remove(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Bandwidth Policers management functions
 */

/**
 * Read data of a bandwidth profile
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile (input and output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_get(L7_uint evc_idx, ptin_bw_profile_t *profile);

/**
 * Apply a bandwidth profile to an EVC and (optionally) to a 
 * specific client 
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_set(L7_uint evc_idx, ptin_bw_profile_t *profile);

/**
 * Remove a bandwidth profile to an EVC 
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_delete(L7_uint evc_idx, ptin_bw_profile_t *profile);

/**
 * EVC Counters management functions
 */

/**
 * Read RX statistics of a particular EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile 
 * @param stats : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_get(L7_uint evc_idx, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats);

/**
 * Add a rule to make packets counting of a specific EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_set(L7_uint evc_idx, ptin_evcStats_profile_t *profile);

/**
 * Delete a rule to make packets counting of a specific EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_delete(L7_uint evc_idx, ptin_evcStats_profile_t *profile);


/******************************************************** 
 * FOR PTIN INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Determines if a particular EVC is in use
 * 
 * @param evc_idx : EVC id
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_in_use(L7_uint evc_idx);

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
 * @param evc_idx   : EVC index
 * @param ptin_intf : PTin interface 
 * @param cfg       : interface configuration 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intfCfg_get(L7_uint evc_idx, ptin_intf_t *ptin_intf, ptin_evc_intfCfg_t *cfg);

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 internalVlan, ptin_HwEthMef10Evc_t *evcConf);

/**
 * Gets the root vlan (internal) for a particular evc
 * 
 * @param evc_id      : EVC id 
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_intRootVlan(L7_uint16 evc_id, L7_uint16 *intRootVlan);

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_idx+innerVlan. 
 *  
 * @param intIfNum  : FP interface#
 * @param evc_idx   : EVC index
 * @param innerVlan : Inner vlan
 * @param extOVlan  : External outer-vlan 
 * @param extIVlan  : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get(L7_uint32 intIfNum, L7_uint16 evc_idx, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan);

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


/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_id       : EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint16 *evc_id);

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

#endif /* _PTIN_EVC_H */
