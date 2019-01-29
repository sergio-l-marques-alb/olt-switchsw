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
 * Create a bw policer
 *  
 * @param usp    
 * @param policer : BW policer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_create(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Destroy a bw policer
 *  
 * @param usp    
 * @param policer : BW policer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_destroy(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Read a field processor entry properties
 *  
 * @param usp   
 * @param policer : BW policer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_get(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Add a new field processor entry
 *  
 * @param usp    
 * @param policer : BW policer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_set(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Remove a field processor entry
 *  
 * @param usp     
 * @param policer : BW policer
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_delete(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Remove all field processor entries
 *  
 * @param usp      
 * @param profile : BW profile 
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_deleteAll(DAPI_USP_t *usp, ptin_bwPolicer_t *policer, DAPI_t *dapi_g);

/**
 * Dump list of bw policers
 */
void bw_policer_list_dump(void);

#endif /* _PTIN_HAPI_FP_BWPOLICER__H */
