#ifndef _PTIN_CROSSCONNECT_API__H
#define _PTIN_CROSSCONNECT_API__H

#include "datatypes.h"

/**
 * Apply or remove CPU from a Vlan ID
 * 
 * @param vlanId : Vlan Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlan_cpu_set(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Add ports to a specific vlan
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlan_port_add(L7_uint32 ptin_port, L7_uint16 vlanId);

/**
 * Remove port from a specific vlan
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlan_port_remove(L7_uint32 ptin_port, L7_uint16 vlanId);

/**
 * Remove port from a specific vlan, and flush mac table
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlan_port_removeFlush(L7_uint32 ptin_port, L7_uint16 vlanId);

/**
 * Remove port from a specific vlan, and flush mac table
 *  
 * @param ptin_port_old : Previous port 
 * @param ptin_port_new : New port 
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlan_port_switch(L7_uint32 ptin_port_old, L7_uint32 ptin_port_new, L7_uint16 vlanId);

/**
 * Create a L3 multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_group_l3_create(L7_int *mcast_group);

/**
 * Create a L2 multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_group_l2_create(L7_int *mcast_group);

/**
 * Create a VLAN multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_group_vlan_create(L7_int *mcast_group);

/**
 * Destroy a multicast group
 * 
 * @param mcast_group : Multicast group id to be destroyed.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_group_destroy(L7_int mcast_group);

/**
 * Associate a multicast group to a vlan
 * 
 * @param vlanId : Vlan id
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlanBridge_multicast_set(L7_uint16 vlanId, L7_int mcast_group);

/**
 * Remove multicast group association to vlan
 * 
 * @param vlanId : Vlan id
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_vlanBridge_multicast_clear(L7_uint16 vlanId, L7_int mcast_group);

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l3_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group, L7_int l3_intf_id);

/**
 * Remove port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l3_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group, L7_int l3_intf_id);

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l2_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group);

/**
 * Remove port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l2_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group);

/**
 * Clean Multicast egress entries
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_egress_clean(L7_int mcast_group);

/**
 * Create Virtual port
 * 
 * @param intIfNum    : interface to be added 
 * @param ext_ovid    : External outer vlan (GEMid)
 * @param ext_ivid    : External inner vlan (UNIVLAN)
 * @param int_ovid    : Internal outer vlan 
 * @param int_ivid    : Internal inner vlan  
 * @param mcast_group : Multicast group id. 
 * @param vport_id    : vport id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_virtual_port_add(L7_uint32 intIfNum,
                                     L7_int ext_ovid, L7_int ext_ivid,
                                     L7_int int_ovid, L7_int int_ivid,
                                     L7_int mcast_group,
                                     L7_int *vport_id,
                                     L7_uint8 macLearnMax);

/**
 * Configure the Maximum Learned MACs foa a Virtual port
 * 
 * @param vport_id    : vport id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_virtual_macLearnMax_set(L7_uint32 intIfNum, L7_int vport_id, L7_uint8 macLearnMax);

/**
 * Remove Virtual port
 * 
 * @param intIfNum      : interface to be removed
 * @param virtual_gport : Virtual port id 
 * @param mcast_group   : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_virtual_port_remove(L7_uint32 intIfNum, L7_int virtual_gport, L7_int mcast_group);

/**
 * Remove Virtual port from vlans info
 * 
 * @param intIfNum    : interface to be removed
 * @param ext_ovid    : External outer vlan 
 * @param ext_ivid    : External inner vlan 
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_virtual_port_remove_from_vlans(L7_uint32 intIfNum, L7_int ext_ovid, L7_int ext_ivid, L7_int mcast_group);

/**
 * Define MAC Learning for a particular Vlan, and its forwarding 
 * vlan 
 * 
 * @param vlanId : Outer Vlan Id
 * @param fwdVlanId : Forward vlan (vlan to use for mac 
 *                  learning)
 * @param macLearn : MAc learning on/off
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_vlan_learn(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_int mcast_group, L7_BOOL macLearn);

/**
 * Use crossconnects, instead of the MAC+Vlan bridging, for a 
 * particular Vlan 
 * 
 * @param vlanId : Vlan id
 * @param crossconnect_apply : use crossconnects
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_enable(L7_uint16 vlanId, L7_BOOL crossconnect_apply, L7_BOOL double_tag);

/**
 * Add a new crossconnect.
 * 
 * @param outerVlanId : outer vlan id
 * @param innerVlanId : inner vlan id
 * @param intIfNum1 : First interface
 * @param intIfNum2 : Second interface
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_add(L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint32 intIfNum1, L7_uint32 intIfNum2);

/**
 * Delete a crossconnect.
 * 
 * @param outerVlanId : outer vlan id
 * @param innerVlanId : inner vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_delete(L7_uint16 outerVlanId, L7_uint16 innerVlanId);

/**
 * Delete all crossconnects.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_delete_all(void);

#endif /* _PTIN_CROSSCONNECT_API__H */
