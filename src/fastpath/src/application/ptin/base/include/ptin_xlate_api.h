#ifndef _PTIN_XLATE_API__H
#define _PTIN_XLATE_API__H

#include "datatypes.h"

#define PTIN_XLATE_NOT_DEFINED  0

/* Number of special portgroups */
#define PTIN_XLATE_PORTGROUP_MAX  3

/* Initial value for special group ports */
#define PTIN_XLATE_PORTGROUP_SPECIAL_BASE ((L7_int) PTIN_SYSTEM_N_INTERF)

/* Special group ports */
#define PTIN_XLATE_PORTGROUP_CLIENTS      (PTIN_XLATE_PORTGROUP_SPECIAL_BASE+0)     /* Client ports */
#define PTIN_XLATE_PORTGROUP_ROOTS        (PTIN_XLATE_PORTGROUP_SPECIAL_BASE+1)     /* Root ports (1G) */
#define PTIN_XLATE_PORTGROUP_ROOTS_10G    (PTIN_XLATE_PORTGROUP_SPECIAL_BASE+2)     /* 10G Root ports */

/**
 * Module initialization function
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_init(void);

/**
 * Get portgroup of a specific interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : port group id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_portgroup_get(L7_uint32 intIfNum, L7_uint32 *portgroup);

/**
 * Set portgroup to a specific interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : port group id (PTIN_XLATE_PORTGROUP_INTERFACE to reset)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_portgroup_set(L7_uint32 intIfNum, L7_uint32 portgroup);

/**
 * Reset class ids to all interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_portgroup_reset_all(void);

/**
 * Get ingress translation new vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId );

/**
 * Get egress translation new vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId );

/**
 * Get egress translation new vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_get( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId );

/**
 * Get ingress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId );

/**
 * Get egress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId );

/**
 * Get egress translation original vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_get_originalVlan( L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId );

/**
 * Add ingress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan 
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_add( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );

/**
 * Add egress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_add( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );

/**
 * Add egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_add( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );

/**
 * Delete ingress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_delete( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId );

/**
 * Delete egress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_delete( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId );

/**
 * Delete egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_delete( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId );

/**
 * Deletes all ingress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_delete_all( void );

/**
 * Deletes all egress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_delete_all( void );

/**
 * Deletes all ingress+egress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_delete_all( void );

/**
 * Effectively removes all deleted translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t ptin_xlate_delete_flush( void );

/**
 * Configures PVID. This information is used on egress Action
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t ptin_xlate_PVID_set(L7_uint32 intIfNum, L7_uint16 vlanId);

/**
 * Get PVID
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t ptin_xlate_PVID_get(L7_uint32 intIfNum, L7_uint16 *vlanId);

#endif /* _PTIN_XLATE_API__H */
