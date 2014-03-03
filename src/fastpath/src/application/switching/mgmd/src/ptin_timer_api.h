/**
 * ptin_timer_api.h 
 *  
 * Provides a timer API
 *  
 * Created on: 2013/10/16 
 * Author:     Daniel Figueira
 */

#ifndef _PTIN_TIMER_API_H
#define _PTIN_TIMER_API_H

#include "ptin_mgmd_defs.h"

typedef enum
{
  PTIN_MGMD_TIMER_1MSEC   = 1,    /* Timer tick granularity of 1 millisecond    */
  PTIN_MGMD_TIMER_10MSEC  = 10,   /* Timer tick granularity of 10 milliseconds  */
  PTIN_MGMD_TIMER_100MSEC = 100,  /* Timer tick granularity of 100 milliseconds */
  PTIN_MGMD_TIMER_1SEC    = 1000  /* Timer tick granularity of 1 second         */
} L7_TIMER_GRAN_t;

typedef void* PTIN_MGMD_TIMER_t;
typedef void* PTIN_MGMD_TIMER_CB_t;


/**
 * Create a new controlBlock that will manage timer ticks in an 
 * independent thread. 
 * 
 * @param[in]  tickGranularity      : Timer granularity  
 * @param[in]  numTimers            : Max number of timers on the controlBlock
 * @param[in]  timerStackThreadSize : Stack size for callback functions. To disable threads, set to '0'
 * @param[out] controlBlock         : Pointer to the new controlBlock
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_createCB(L7_TIMER_GRAN_t tickGranularity, uint32 numTimers, uint32 timerStackThreadSize, PTIN_MGMD_TIMER_CB_t *controlBlock);


/**
 * Initialize a new timer, managed by the given controlBlock, 
 * which will invoke the funcPtr callback upon expiring. 
 * 
 * @param[in]  controlBlock : ControlBlock responsible for this timer's management
 * @param[out] timerPtr     : Pointer to the new timer
 * @param[in]  funcPtr      : Callback to be invoked upon timer's expiral
 * 
 * @return RC_t 
 *  
 * @note: Timer's countdown MUST NOT start to decrement until ptin_timer_start is called.
 */
RC_t ptin_mgmd_timer_init(PTIN_MGMD_TIMER_CB_t controlBlock, PTIN_MGMD_TIMER_t *timerPtr, void * (*funcPtr)(void* param));


/**
 * Delete the given timer.
 * 
 * @param[in] timerPtr : Pointer to the timer
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_deinit(PTIN_MGMD_TIMER_t timerPtr);


/**
 * Start the given timer, which will expire after timeout, 
 * calling the registered funcPtr with param. 
 * 
 * @param[in] timerPtr : Pointer to the timer
 * @param[in] timeout  : Timer's timeout (expressed accordingly to the controlBlock granularity)
 * @param[in] param    : Callback argument
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_start(PTIN_MGMD_TIMER_t timerPtr, uint32 timeout, void *param);


/**
 * Stop the given timer.
 * 
 * @param[in] timerPtr : Pointer to the timer
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_stop(PTIN_MGMD_TIMER_t timerPtr);


/**
 * Return how much time the given timer has before expiring.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return uint32 
 */
uint32 ptin_mgmd_timer_timeLeft(PTIN_MGMD_TIMER_t timerPtr);


/**
 * Check if the given timer is running.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return BOOL 
 */
BOOL ptin_mgmd_timer_isRunning(PTIN_MGMD_TIMER_t timerPtr);


/**
 * Check if the given timer exists.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return BOOL 
 */
BOOL ptin_mgmd_timer_exist(PTIN_MGMD_TIMER_t timerPtr);

#endif //_L7_TIMER_API_H
