#ifndef _PTIN_QOS__H
#define _PTIN_QOS__H

#include "datatypes.h"
#include "ptin_structs.h"

/**
 * Configure Shaping
 * 
 * @author mruas (05/01/21)
 * 
 * @param ptin_port 
 * @param tc 
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t
ptin_qos_shaper_set(L7_uint32 ptin_port, L7_int tc,
                    L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size);

#endif /* #define _PTIN_QOS__H */
