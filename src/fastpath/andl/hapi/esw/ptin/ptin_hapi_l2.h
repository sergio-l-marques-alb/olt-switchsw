#ifndef _PTIN_HAPI_L2__H
#define _PTIN_HAPI_L2__H

//#include "l7_common.h"
//#include "ptin_structs.h"
//#include "ptin_globaldefs.h"
//#include "dapi_struct.h"
//#include "broad_common.h"
#include "ptin_hapi.h"
#include "bcmx/l2.h"

/************************************
 * MAC Learning Control
 ************************************/

/**
 * Init MAC learning data structures
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t ptin_hapi_maclimit_init(void);

/**
 * Increment number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_inc(bcmx_l2_addr_t *bcmx_l2_addr);

/**
 * Decrement number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_dec(bcmx_l2_addr_t *bcmx_l2_addr);

/**
 * Reset number of learned MAC addresses
 * 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vport_maclimit_reset(bcm_gport_t gport);

/**
 * Reset number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vlan_maclimit_reset(bcm_vlan_t vlan_id);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vport_maclimit_setmax(bcm_gport_t gport, L7_uint8 max_value);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_setmax(DAPI_USP_t *ddUsp, L7_uint16 vlan_id, int mac_limit, DAPI_t *dapi_g);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vlan_maclimit_setmax(bcm_vlan_t vlan_id, L7_uint8 max_value);

/**
 * Configures the information needed to generate alarms
 * 
 * @param gport     : GPort (virtual port)
 * @param intf_id   : Physical port
 * @param outer_vid : VLAN id 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vport_maclimit_alarmconfig(bcm_gport_t gport, int bcm_port, L7_uint16 outer_vid);

/**
 * Configures the information needed to generate alarms
 * 
 * @param vlan_id   : VLAN id 
 * @param intf_id   : Physical port
 * @param outer_vid : VLAN id 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_alarmconfig(bcm_vlan_t vlan_id, int bcm_port, L7_uint16 outer_vid);

#endif //_PTIN_HAPI_L2__H
