#ifndef _PTIN_HAPI_L3__H
#define _PTIN_HAPI_L3__H

#include "l7_common.h"
#include "ptin_structs.h"
#include "ptin_globaldefs.h"
#include "dapi_struct.h"
#include "broad_common.h"

/**
 * Add L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l3_host_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data);

/**
 * Remove L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l3_host_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data);

/**
 * Add L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l3_route_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data);

/**
 * Remove L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l3_route_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data);

#endif /* _PTIN_HAPI_L3__H */

