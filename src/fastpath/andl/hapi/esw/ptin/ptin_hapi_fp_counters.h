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
 * @param usp  
 * @param stats : Statistics data
 * @param profile : Policy reference data
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_get(DAPI_USP_t *usp, ptin_evcStats_counters_t *stats, ptin_evcStats_profile_t *profile, DAPI_t *dapi_g);

/**
 * Add a new counters policy
 *  
 * @param usp  
 * @param profile : Policy reference data 
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_set(DAPI_USP_t *usp, ptin_evcStats_profile_t *profile, DAPI_t *dapi_g);

/**
 * Remove a counters policy
 *  
 * @param usp  
 * @param profile : Policy reference data
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_delete(DAPI_USP_t *usp, ptin_evcStats_profile_t *profile, DAPI_t *dapi_g);

/**
 * Remove all counters policies
 *
 * @param usp  
 * @param profile : Policy reference data
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_fpCounters_deleteAll(DAPI_USP_t *usp, ptin_evcStats_profile_t *profile);

#endif /* _PTIN_HAPI_FP_COUNTERS__H */
