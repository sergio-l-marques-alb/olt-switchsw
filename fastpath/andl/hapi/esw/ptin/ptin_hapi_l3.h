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

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_ipmc_add(ptin_dtl_ipmc_addr_t *ptin_ipmc);

/*********************************************************************
* @purpose  Remove a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_ipmc_remove(ptin_dtl_ipmc_addr_t *ptin_ipmc);

/*********************************************************************
* @purpose  Get a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_ipmc_get(ptin_dtl_ipmc_addr_t *ptin_ipmc);

/*********************************************************************
* @purpose  Reset IPMC Table
*
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_ipmc_reset(void);

/*********************************************************************
* @purpose  Init HAPI L3 Intf Id Structures
*
*
* @end
*********************************************************************/
#if 0
extern L7_RC_t ptin_hapi_l3_intf_init(void);
#endif

/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_intf_create (ptin_dtl_l3_intf_t *intf);

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_intf_delete (ptin_dtl_l3_intf_t *intf);

/*********************************************************************
* @purpose  Get an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern L7_RC_t ptin_hapi_l3_intf_get (ptin_dtl_l3_intf_t *intf);

#endif /* _PTIN_HAPI_L3__H */

