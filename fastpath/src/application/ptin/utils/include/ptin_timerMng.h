#ifndef _PTIN_TIMERMNG__H
#define _PTIN_TIMERMNG__H

#include "l7sll_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"

/* Function prototypes */
typedef L7_int32 (*ptin_timerMng_data_compare_fcn_t)(void *p, void *q, L7_uint32 key);  /* Compare function */
typedef void (*ptin_timerMng_expiryCallback_fcn_t)(void *param);                       /* Expiry timer callback */


/* Group of Timers context */
typedef struct
{
  L7_uint   max_timers;          /* Max number of timers */
  L7_sll_t  ll_timerList;        /* Linked list of timer data nodes */

  ptin_timerMng_expiryCallback_fcn_t expiry_callback_fcn; /* Callback associated to each expiry */

} ptin_timerMng_context_t;


/*********************************************************************
* @purpose  Init ptin_timer module
* 
* @param    max_timers: Maximum number of timers
* @param    timer_resolution: 1, 10, 100 or 1000 (ms)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t ptin_timerMng_init(L7_uint16 max_timers, L7_uint16 timer_resolution);

/*********************************************************************
* @purpose  Deinit ptin_timer module
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t ptin_timerMng_deinit(void);

/**
 * Create all the necessary data structure for a new group of 
 * timers
 * 
 * @param context : Data structure pointer 
 * @param expiry_callback_fcn : callback function to be 
 *                              associated to timers expiration
 * @param compare_callback_fcn: Function used to 
 *                                        compare different timers
 * @param max_timers : Maximum number of timers for this context
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_timerMng_context_create(ptin_timerMng_context_t *context,
                                            ptin_timerMng_expiryCallback_fcn_t expiry_callback_fcn,
                                            ptin_timerMng_data_compare_fcn_t compare_callback_fcn,
                                            L7_uint32 max_timers);

/**
 * Destroy all the data structures associated to a group of 
 * timers 
 * 
 * @param context 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_timerMng_context_destroy(ptin_timerMng_context_t *context);

/**
 * Start a new timer
 * 
 * @param context : Group of Timers context
 * @param data    : timer data 
 * @param timeout : timeout 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_timerMng_start (ptin_timerMng_context_t *context, void *data, L7_uint16 timeout);

/**
 * Stops a running timer, andf remove all related resources
 * 
 * @param context : Group of Timers context
 * @param data : timer data 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_timerMng_stop  (ptin_timerMng_context_t *context, void *data);

/**
 * Use this function to get timer data pointer within the expiry
 * callback function 
 * 
 * @param param : param given by the expiry callback
 * 
 * @return void* : pointer to data
 */
extern void *ptin_timerMng_expiryCallback_data_get(void *param);

/**
 * Use this function to get timer data pointer within the 
 * compare callback function 
 * 
 * @param ptr : param given by the compare function
 * 
 * @return void* : pointer to data
 */
extern void *ptin_timerMng_compare_data_get(void *ptr);

#endif /* _PTIN_TIMERMNG__H */

