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
extern L7_RC_t ptin_crossconnect_vlan_learn(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_BOOL macLearn);

/**
 * Use crossconnects, instead of the MAC+Vlan bridging, for a 
 * particular Vlan 
 * 
 * @param vlanId : Vlan id
 * @param crossconnect_apply : use crossconnects
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_crossconnect_enable(L7_uint16 vlanId, L7_BOOL crossconnect_apply);

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
