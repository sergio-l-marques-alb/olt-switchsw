#ifndef _PTIN_DEBUG_H
#define _PTIN_DEBUG_H

#include "ptin_include.h"

typedef enum
{
  PTIN_PROC_ERPS_INSTANCE=0,
  PTIN_PROC_MAX
} ptin_proc_instance_t;


/**
 * Example to use Generic DTL processor
 * 
 * @param intIfNum 
 * @param operation 
 * @param param1 
 * @param param2 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_debug_example(L7_uint32 intIfNum, L7_uint operation, L7_uint param1, L7_uint param2);

/**
 * Start time measurement
 * 
 * @param instance 
 */
extern void proc_runtime_start(ptin_proc_instance_t instance);

/**
 * Stop and update time measurement
 * 
 * @param instance 
 */
void proc_runtime_stop(ptin_proc_instance_t instance);

/**
 * Update proc runtime meter
 * 
 * @param msg_id : message id
 * @param time_delta : time taken to process message
 */
extern void proc_runtime_meter_update(ptin_proc_instance_t instance, L7_uint32 time_delta);

/**
 * Initializes proc runtime meter 
 *  
 * @param msg_id : message id 
 */
extern void proc_runtime_meter_init(ptin_proc_instance_t instance);

/**
 * Prints meter information
 */
extern void proc_runtime_meter_print(void);



extern void ptin_timer_start(L7_uint16 timer_id, char *str);

extern void ptin_timer_stop(L7_uint16 timer_id);

#endif
