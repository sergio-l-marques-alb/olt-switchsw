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
 * Get BW policer
 * 
 * @param profile : BW profile
 * @param policer : BW policer pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_policy_t *policer);

/**
 * Add a new BW policer
 *  
 * @param intIfNum : Interface to apply profile 
 * @param profile : BW profile
 * @param policer : BW policer pointer address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_policy_t **policer);

/**
 * Remove an existent BW policer
 *  
 * @param policer : BW policer pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_delete(ptin_bw_policy_t *policer);

/**
 * Remove All existent BW policers
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_deleteAll(void);


/**
 * EVC STATISTICS MANAGEMENT
 */

/**
 * Get EVC Statistics
 * 
 * @param stats   : Statistics data
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_get(ptin_evcStats_counters_t *stats, ptin_evcStats_policy_t *policy);

/**
 * Set a new EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_set(ptin_evcStats_profile_t *profile, ptin_evcStats_policy_t **policy);

/**
 * Delete an EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_delete(ptin_evcStats_policy_t *policy);

/**
 * Remove All existent EVC Statistics rules
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evcStats_deleteAll(void);

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
extern L7_RC_t ptin_stormControl_get(ptin_stormControl_t *stormControl);

/**
 * Storm control profile set
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_set(ptin_stormControl_t *stormControl);

/**
 * Storm control profile clear
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_clear(ptin_stormControl_t *stormControl);

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

/**
 * Consult hardware resources
 * 
 * @param resources : available resources
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hw_resources_get(st_ptin_policy_resources *resources);

#endif
