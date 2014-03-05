#ifndef _PTIN_HAPI_BRIDGE__H
#define _PTIN_HAPI_BRIDGE__H

#include "ptin_hapi.h"
#include "bcmx/l2.h"

/**
 * Initialize bridge module
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_init(void);

/**
 * Get available resources for cross-connections
 * 
 * @param crossconnects: pointer to variable where will contain 
 *               the number of cross-connections.
 * @return L7_RC_t: always L7_SUCCESS
 */
extern L7_RC_t ptin_hapi_bridge_free_resources(L7_uint16 *crossconnects);

/**
 * Define vlan related parameters, for bridging purposes
 * 
 * @param vlanId :    vlan to be configured
 * @param fwdVlanId : forward vlan to be used when learning MAC 
 *                    addresses
 * @param cross_connects_apply: Use cross-connects to this vlan?
 * @param mac_learning_apply:   Apply mac learning to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_vlan_mode_set(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_BOOL cross_connects_apply, L7_BOOL mac_learning_apply, L7_int mc_group);

/**
 * Define forward vlanId for a specific vlan for bridging 
 * purposes 
 * 
 * @param vlanId :    vlan to be configured
 * @param fwdVlanId : forward vlan to be used when learning MAC 
 *                    addresses
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_vlan_mode_fwdVlan_set(L7_uint16 vlanId, L7_uint16 fwdVlanId);

/**
 * Define MC group for a specific vlan for bridging 
 * 
 * @param vlanId :    vlan to be configured
 * @param mcast_group : MC group id (if invalid, create a new 
 *                    one, and return it)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridgeVlan_multicast_set(L7_uint16 vlanId, L7_int *mcast_group);

/**
 * Define the outer tpid of a specific vlan, for bridging 
 * purposes 
 * 
 * @param vlanId :    vlan to be configured
 * @param outer_tpid : TPID of the outer tag
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_vlan_mode_outerTpId_set(L7_uint16 vlanId, L7_uint16 outer_tpid);

/**
 * (Dis)Enable MAC learning for a specific vlan
 * 
 * @param vlanId :    vlan to be configured
 * @param mac_learning_apply:   Apply mac learning to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_vlan_mode_macLearn_set(L7_uint16 vlanId, L7_BOOL mac_learning_apply);

/**
 * Change bridging vlan mode (cross-connects enabler)
 * 
 * @param vlanId :    vlan to be configured
 * @param cross_connects_apply: Use cross-connects to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_vlan_mode_crossconnect_set(L7_uint16 vlanId, L7_BOOL cross_connects_apply, L7_BOOL double_tag);

/**
 * Add a cross-connection between two ports
 * 
 * @param outerVlanId: outer vlan to look for
 * @param innerVlanId: inner vlan to look for (0 to not use)
 * @param dapiPort1: First port
 * @param dapiPort2: Second port 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_hapi_bridge_crossconnect_add(L7_uint16 outerVlanId, L7_uint16 innerVlanId, ptin_dapi_port_t *dapiPort1, ptin_dapi_port_t *dapiPort2);

/**
 * Delete a cross-connection
 * 
 * @param outerVlanId: Outer vlan to look for
 * @param innerVlanId: Inner vlan to look for
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_crossconnect_delete(L7_uint16 outerVlanId, L7_uint16 innerVlanId);

/**
 * Deletes all configured cross-connections
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridge_crossconnect_delete_all(void);

/**
 * Create Virtual port
 * 
 * @param dapiPort      : PON port
 * @param match_ovid    : external outer vlan (GEMid)
 * @param match_ivid    : external inner vlan (UNIVLAN)
 * @param egress_ovid   : outer vlan inside switch
 * @param egress_ivid   : inner vlan inside switch 
 * @param mcast_group   : mc group (-1 to create) 
 * @param virtual_gport : vport id (to be returned) 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_vp_create(ptin_dapi_port_t *dapiPort,
                                   L7_uint16 match_ovid, L7_uint16 match_ivid, L7_uint16 egress_ovid, L7_uint16 egress_ivid,
                                   L7_int *mcast_group,
                                   L7_int *virtual_gport);

/**
 * Remove virtual port
 * 
 * @param dapiPort      : PON port
 * @param match_ovid    : external Outer vlan (GEMid)
 * @param match_ivid    : external inner vlan (UNIVLAN) 
 * @param virtual_gport : vport id 
 * @param mcast_group   : multicast group
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vp_remove(ptin_dapi_port_t *dapiPort,
                            L7_uint16 match_ovid, L7_uint16 match_ivid,
                            L7_int virtual_gport,
                            L7_int mcast_group);

/**
 * Add port to egress multicast group
 * 
 * @param mcast_group : MC group id (-1 to create)
 * @param dapiPort    : Port to be added 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_multicast_egress_port_add(L7_int *mcast_group, ptin_dapi_port_t *dapiPort);

/**
 * Remove port from egress multicast group
 * 
 * @param mcast_group : MC group id (must be valid) 
 * @param dapiPort    : Port to be removed
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_multicast_egress_port_remove(L7_int mcast_group, ptin_dapi_port_t *dapiPort);

/**
 * Remove all entries associated to a MC group
 * 
 * @param mcast_group : MC group id (must be valid) 
 * @param destroy_mcgroup : Destroy MC group 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_multicast_egress_clean(L7_int mcast_group, L7_BOOL destroy_mcgroup);

/**
 * Define MC group for a specific vlan for bridging 
 * 
 * @param vlanId      : vlan to be configured
 * @param mcast_group : MC group id (-1 to create)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridgeVlan_multicast_set(L7_uint16 vlanId, L7_int *mcast_group);

/**
 * Rempove MC group for a specific vlan for bridging 
 * 
 * @param vlanId          : vlan to be configured
 * @param mcast_group     : MC group id 
 * @param destroy_mcgroup : Destroy MC group
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_bridgeVlan_multicast_reset(L7_uint16 vlanId, L7_int mcast_group, L7_BOOL destroy_mcgroup);

#if 1
/************************************
 * MAC Learning Control
 ************************************/

/**
 * Init MAC learning data structures
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t ptin_hapi_macaddr_init(void);

/**
 * Increment number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_macaddr_inc(bcmx_l2_addr_t *bcmx_l2_addr);

/**
 * Decrement number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_macaddr_dec(bcmx_l2_addr_t *bcmx_l2_addr);

/**
 * Reset number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_macaddr_reset(bcm_vlan_t vlan_id, bcm_gport_t gport);

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_macaddr_setmax(bcm_vlan_t vlan_id, bcm_gport_t gport, L7_uint8 max_value);
#endif

#endif /* _PTIN_HAPI_BRIDGE__H */
