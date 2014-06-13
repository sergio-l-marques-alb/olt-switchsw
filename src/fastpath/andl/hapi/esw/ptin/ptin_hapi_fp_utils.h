#ifndef _PTIN_HAPI_FP_UTILS__H
#define _PTIN_HAPI_FP_UTILS__H

#include "ptin_hapi.h"

/*********************************
 * TYPEDEFS
 *********************************/

/* Descriptor for a FP policies database */
typedef struct {
  void *database_base;                      /* Pointer to database */

  L7_uint16 database_num_elems;             /* Number of elements of database */
  L7_uint16 database_elem_sizeof;           /* Sizeof of each element */

  L7_int database_index_first_free;         /* First free element in database */

  void (*policy_clear_data)(void *policy);                          /* Function to clear element */
  L7_BOOL (*policy_inUse)(void *policy);                            /* Function to check if element is in use */
  L7_BOOL (*policy_compare)(void *profile, const void *policy);     /* Function to compare data */
  L7_BOOL (*policy_check_conflicts)(void *profile, const void *policy, int stage);  /* Function to compare data */
} ptin_hapi_database_t;

/*********************************
 * MACROS FOR MANAGING DATABASE
 *********************************/

/* Check if database pointer is valid */
#define FP_POLICY_VALID_PTR(ptr,db)   ( ( (ptr) != L7_NULLPTR ) && \
                                        ( (L7_uint32) ((char *) (ptr)) >= (L7_uint32) ((char *) (db)->database_base) ) && \
                                        ( (L7_uint32) ((char *) (ptr)) <= (L7_uint32) ((char *) (db)->database_base) + ((db)->database_num_elems-1)*(db)->database_elem_sizeof) )


/**
 * Consult Switch available resources
 * 
 * @param resources : struct describing system resources
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_policy_resources_get(st_ptin_policy_resources *resources);


/**********************************
 * FUNCTIONS FOR MANAGING DATABASE
 **********************************/

/**
 * Clear element in database (it will be marked as free)
 * 
 * @param elem : Pointer to element in database
 * @param db : Database descriptor
 */
extern void ptin_hapi_policy_clear(void *elem, ptin_hapi_database_t *db);

/**
 * Find next used element in database
 * 
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
extern void *ptin_hapi_policy_next(void *base_ptr, ptin_hapi_database_t *db);

/**
 * Find element in database with profile data
 * 
 * @param profile : Reference data to search for
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
extern void *ptin_hapi_policy_find(void *profile, void *base_ptr, ptin_hapi_database_t *db);

/**
 * Find element in database with conflict
 * 
 * @param profile : Reference data to search for
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor 
 * @param state: ingress or egress 
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
extern void *ptin_hapi_policy_check_conflicts(void *profile, void *base_ptr, ptin_hapi_database_t *db, int stage);

/**
 * Find first free element in database
 * 
 * @param db : database descript
 * 
 * @return void* : Pointer to first free element (L7_NULLPTR if 
 *         no free elements are available)
 */
extern void *ptin_hapi_policy_find_free(ptin_hapi_database_t *db);

#endif /* _PTIN_HAPI_FP_UTILS__H */
