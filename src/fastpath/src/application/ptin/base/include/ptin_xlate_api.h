#ifndef _PTIN_XLATE_API__H
#define _PTIN_XLATE_API__H

#include "datatypes.h"
#include "ptin_structs.h"

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
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                       L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId);

/**
 * Get egress translation new vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                      L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId );

#if 0
/**
 * Get egress translation new vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_get( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                                L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId );
#endif

/**
 * Get ingress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned)  
 * @param newOuterVlanId : new OUTER vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                                    L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );

/**
 * Get egress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned)  
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                                   L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );

#if 0
/**
 * Get egress translation original vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned)  
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_get_originalVlan( L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                                             L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId );
#endif

/**
 * Add ingress translation entry
 *  
 * @param intf_vlan: intf/VLAN input 
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * @param newOuterPrio : new outer prio (-1 to not be used)
 * @param newInnerPrio : new inner prio (-1 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_ingress_add( ptin_HwEthMef10Intf_t *intf_vlan,
                                       L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId,
                                       L7_int newOuterPrio, L7_int newInnerPrio  );

/**
 * Add egress translation entry
 *  
 * @param intf_vlan: intf/VLAN input  
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * @param newOuterPrio : new outer prio (-1 to not be used)
 * @param newInnerPrio : new inner prio (-1 to not be used)
 *  
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_add( ptin_HwEthMef10Intf_t *intf_vlan,
                                      L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId,
                                      L7_int newOuterPrio, L7_int newInnerPrio );

#if 0
/**
 * Add egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param remove_VLANs : flag to remove VLANs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_xlate_egress_portgroup_add( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                                L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId, L7_BOOL remove_VLANs );
#endif
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

#if 0
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
#endif

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


/**
 * Get all the outer vlan from port 
 * 
 * @param stage : PTIN_XLATE_STAGE_ALL, PTIN_XLATE_STAGE_INGRESS
 *             or PTIN_XLATE_STAGE_EGRESS
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t xlate_outer_vlan_replicate_Dstport(L7_uint32 operation, L7_uint32 ptin_port_src , L7_uint32 ptin_port_dst);

#endif /* _PTIN_XLATE_API__H */
