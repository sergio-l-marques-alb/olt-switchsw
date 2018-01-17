#include "ptin_status.h"
#include "dtl_ptin.h"

/**
 * Get temperature sensors data
 * 
 * @param temp_info
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_status_temperature_monitor(ptin_dtl_temperature_monitor_t *temp_info)
{
  return dtlPtinGeneric(L7_ALL_INTERFACES,
                        PTIN_DTL_MSG_TEMPERATURE_MONITOR,
                        DAPI_CMD_GET,
                        sizeof(ptin_dtl_temperature_monitor_t),
                        (void *) temp_info);
}

