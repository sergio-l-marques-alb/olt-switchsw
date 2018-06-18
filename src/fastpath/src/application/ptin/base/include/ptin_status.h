#ifndef _PTIN_STATUS__H
#define _PTIN_STATUS__H

#include "ptin_structs.h"

/**
 * Get temperature sensors data
 * 
 * @param temp_info 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_status_temperature_monitor(ptin_dtl_temperature_monitor_t *temp_info);

#endif /* _PTIN_STATUS__H */

