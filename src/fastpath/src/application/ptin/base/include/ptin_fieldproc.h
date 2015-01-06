/**
 * ptin_fieldproc.h 
 *  
 * Implements bandwidth profiles and counters at request, using 
 * field processor rules 
 *
 * Created on: 2011/0913 
 * Author: Milton Ruas (milton-r-silva@ptinovacao.pt) 
 *  
 * Notes:
 */

#ifndef _PTIN_FIELDPROC__H
#define _PTIN_FIELDPROC__H

#include "ptin_include.h"

/**
 * BW POLICERS MANAGEMENT
 */

/**
 * Create a new BW policer
 *  
 * @param profile : BW profile 
 * @param policer_id : Policer ID (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_create(ptin_bw_meter_t *meter, L7_int *policer_id);

/**
 * Destroy a BW policer
 *  
 * @param policer_id : Policer ID
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_destroy(L7_int policer_id);

/**
 * Get BW policer
 * 
 * @param profile : BW policer profile 
 * @param meter   : Policer meter 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_meter_t *meter);

/**
 * Add a new BW policer
 * 
 * @param profile : BW policer profile 
 * @param meter   : Policer meter 
 * @param policer_id : Policer id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_meter_t *meter, L7_int policer_id);

/**
 * Remove an existent BW policer
 *  
 * @param profile : BW policer profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_delete(ptin_bw_profile_t *profile);

/**
 * Remove All existent BW policers
 *  
 * @param profile : BW policer profile
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_deleteAll(ptin_bw_profile_t *profile);


/**
 * EVC STATISTICS MANAGEMENT
 */

/**
 * Get EVC Statistics
 * 
 * @param stats   : Statistics data
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_get(ptin_evcStats_counters_t *stats, ptin_evcStats_profile_t *profile);

/**
 * Set a new EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_set(ptin_evcStats_profile_t *profile);

/**
 * Delete an EVC Statistics rule
 * 
 * @param profile : EVC Stats profile
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_delete(ptin_evcStats_profile_t *profile);

/**
 * Remove All existent EVC Statistics rules
 *  
 * @param profile : EVC Stats profile 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_deleteAll(ptin_evcStats_profile_t *profile);

/**
 * Set global enable for IGMP packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmpPkts_global_trap(L7_BOOL enable);

/**
 * Create/remove a rule, to allow IGMP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_igmpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Set global enable for MLD packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_mldPkts_global_trap(L7_BOOL enable);

/**
 * Create/remove a rule, to allow MLD packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_mldPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Create/remove a rule, to allow IGMP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_igmpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Set global enable for DHCP packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcpPkts_global_trap(L7_BOOL enable);

/**
 * Create/remove a rule, to allow DHCP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_dhcpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Set global enable for PPPoE packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoePkts_global_trap(L7_BOOL enable);

/**
 * Create/remove a rule, to allow PPPoE packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_pppoePkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Storm control profile get
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_get(L7_BOOL enable, L7_uint32 intIfNum, L7_uint16 vlanId, L7_uint16 vlanId_mask, ptin_stormControl_t *stormControl);

/**
 * Storm control profile set
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_config(L7_BOOL enable, ptin_stormControl_t *stormControl);

#if 0
/**
 * Apply Rate limit to broadcast traffic
 * 
 * @param enable : enable status
 * @param vlanId : vlan id
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_broadcast_rateLimit(L7_BOOL enable, L7_uint16 vlanId);

/**
 * Apply Rate limit to multicast traffic
 * 
 * @param enable : enable status
 * @param vlanId : vlan id
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_multicast_rateLimit(L7_BOOL enable, L7_uint16 vlanId);
#endif

/**
 * Consult hardware resources
 * 
 * @param resources : available resources
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hw_resources_get(st_ptin_policy_resources *resources);

#endif
