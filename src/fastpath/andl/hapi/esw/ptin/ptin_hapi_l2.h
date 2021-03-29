#ifndef _PTIN_HAPI_L2__H
#define _PTIN_HAPI_L2__H

#include "ptin_hapi.h"

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
 * @param bcm_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_inc(bcm_l2_addr_t *bcm_l2_addr);

/**
 * Decrement number of learned MAC addresses
 * 
 * @param bcm_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_dec(bcm_l2_addr_t *bcm_l2_addr);

/**
 * Reset number of learned MAC addresses (Vport level)
 * 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l2intf_maclimit_reset(bcm_gport_t gport);

/**
 * Set maximum number of learned MAC addresses (Vport level)
 * 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l2intf_maclimit_setmax(bcm_gport_t gport, L7_uint8 max_value);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_maclimit_setmax(DAPI_USP_t *ddUsp, L7_uint16 vlan_id, L7_uint32 mac_limit, L7_uint8 action, L7_uint16 send_trap, DAPI_t *dapi_g);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vlan_maclimit_setmax(bcm_vlan_t vlan_id, L7_uint8 max_value);

/**
 * Get the status of the interface (over of within the MAC limit 
 * learned 
 * 
 * @param mac_learned : MAC Learned 
 * @param status      : Status  
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */

extern L7_RC_t ptin_hapi_maclimit_status(DAPI_USP_t *ddUsp, L7_uint32 *mac_learned, L7_uint32 *status, DAPI_t *dapi_g);

/**
 * Configures the information needed to generate alarms
 * 
 * @param gport     : GPort (virtual port)
 * @param intf_id   : Physical port
 * @param outer_vid : VLAN id 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l2intf_maclimit_alarmconfig(bcm_gport_t gport, int bcm_port, L7_uint16 outer_vid, L7_uint port_id, L7_uint type);

/**
 * Get MAC limit state of a particular l2intf
 * 
 * @param l2intf_id    : l2intf_id (virtual port)
 * @param over_limit   : if updated to TRUE is over limit (or 
 *                       error), if FALSE is under_limit
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_l2intf_maclimit_status_get(L7_uint32 l2intf_id, L7_uint8 *over_limit);

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

extern L7_RC_t ptin_hapi_maclimit_trap(L7_uint16 int_f, L7_uint16 physical_port);

#endif //_PTIN_HAPI_L2__H
