#ifndef _PTIN_HAPI_FP_COUNTERS__H
#define _PTIN_HAPI_FP_COUNTERS__H

#include "ptin_hapi.h"

/******************************************** 
 * FUNCTIONS FOR EXTERNAL USAGE
 ********************************************/

/**
 * Initialize all database
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t hapi_ptin_fpCounters_init(void);

/**
 * Read the list of counters associated to a policy
 * 
 * @param stats : Statistics data
 * @param counter : Pointer to Counter policy
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_get(ptin_evcStats_counters_t *stats, ptin_evcStats_policy_t *counter);

/**
 * Add a new counters policy
 * 
 * @param profile : Policy reference data
 * @param counter : Pointer to Counter policy
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_set(ptin_evcStats_profile_t *profile, ptin_evcStats_policy_t **counter, DAPI_t *dapi_g);

/**
 * Remove a counters policy
 *
 * @param counter : Pointer to Counter policy
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_delete(ptin_evcStats_policy_t *counter);

/**
 * Remove all counters policies
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_deleteAll(void);

#endif /* _PTIN_HAPI_FP_COUNTERS__H */
