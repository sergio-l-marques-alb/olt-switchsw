#ifndef _PTIN_HAPI_FP_BWPOLICER__H
#define _PTIN_HAPI_FP_BWPOLICER__H

#include "ptin_hapi.h"

/******************************************** 
 * FUNCTIONS FOR EXTERNAL USAGE
 ********************************************/

/**
 * Initialize all database
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t hapi_ptin_bwPolicer_init(void);

/**
 * Read a field processor entry properties
 * 
 * @param profile : BW profile (returned data)
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_policy_t *policer);

/**
 * Add a new field processor entry
 * 
 * @param profile : BW profile
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_policy_t **policer, DAPI_t *dapi_g);

/**
 * Remove a field processor entry
 * 
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_delete(ptin_bw_policy_t *policer);

/**
 * Remove all field processor entries
 * 
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_deleteAll(void);

/**
 * Dump list of bw policers
 */
void bw_policer_list_dump(void);

#endif /* _PTIN_HAPI_FP_BWPOLICER__H */
