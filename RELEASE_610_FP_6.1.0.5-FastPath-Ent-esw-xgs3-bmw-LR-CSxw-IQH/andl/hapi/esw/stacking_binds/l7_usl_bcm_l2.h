/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.h
*
* @purpose    Stubs to Handle synchronization responsibilities for Address,Trunk,VLAN, STG
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#ifndef L7_USL_BCM_L2_H
#define L7_USL_BCM_L2_H

#include "l7_common.h"
#include "bcmx/trunk.h"
#include "bcmx/l2.h"
#include "bcmx/mcast.h"
#include "bcmx/vlan.h"
#include "bcmx/stg.h"

/*********************************************************************
* @purpose  Initialize the Layer 2 tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
extern L7_RC_t usl_l2_init();

/*********************************************************************
* @purpose  Release all resources allocated during usl_l2_init()
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were released
* @returns  L7_ERROR   - if any resourses were not released 
*
* @notes    
*       
* @end
*********************************************************************/
extern L7_RC_t usl_l2_fini();

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified trunk.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_trunk (bcm_trunk_t tgid, L7_uint32 reactivateLearning);  /* PTin added */

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified wlan port.
*
* @param    port - BCMX wlan vp identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_wlan_port (bcmx_lport_t port);

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified lport.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_port (bcmx_lport_t lport, L7_uint32 reactivateLearning);     /* PTin added */

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for all lport.
*
* @param    none.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_all (L7_uint32 reactivateLearning);         /* PTin added */

/*********************************************************************
* @purpose  Flush dynamic MAC addresses matching specified vlan.
*
* @param    vid - BCMX VLAN identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_vlan (bcm_vlan_t vid);

/*********************************************************************
* @purpose  Add a L2 MAC address to the HW's forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l2_addr_add(bcmx_l2_addr_t * l2addr,bcmx_lplist_t *port_block);

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l2_addr_remove(bcm_mac_t mac_addr, bcm_vlan_t vid);

/*********************************************************************
* @purpose  Set the aging time
*
* @param    ageTime     @{(input)} The aging time in seconds
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l2_age_timer_set(L7_int32 ageTime);

/*********************************************************************
* @purpose  Create a VLAN in the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_vlan_create(bcm_vlan_t vid);

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
extern int usl_bcmx_vlan_destroy(bcm_vlan_t vid);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    pStg          @{(input)} the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_create(bcm_stg_t *pStg);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_destroy(bcm_stg_t stg);


/*********************************************************************
* @purpose  Get the existing list of vlans for this spanning tree group
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vlanList     @{(input)} the list of vlans belonging to STG
* @param    vlanCount    @{(input)} Count of Vlans associated to STG
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_stg_vlan_list_get(bcm_stg_t stgId, bcm_vlan_t **vlanList, int *vlanCount);

/*********************************************************************
* @purpose  Destroy the vlan list obtained through list_get
*
* @param    vlanList     @{(input)} the list of vlans
* @param    vlanCount    @{(input)} Count of Vlans
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_stg_vlan_list_destroy(bcm_vlan_t *list, int count);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_vlan_add(bcm_stg_t stg,bcm_vlan_t vid);

/*********************************************************************
* @purpose  Remove an association between a Spanning Tree group and a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_vlan_remove(bcm_stg_t stg,bcm_vlan_t vid);

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_trunk_create(bcm_trunk_t * tid);

/*********************************************************************
* @purpose  Set hashing mechanism for existing trunk.
*
* @param    tid          @{(output)} The Trunk ID
* @param    psc          @{(output)} The hashing mechanism.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_trunk_psc_set (bcm_trunk_t tid, int psc);

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_trunk_destroy(bcm_trunk_t tid);

/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_trunk_set(bcm_trunk_t tid,bcmx_trunk_add_info_t * add_info);

/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param    mcaddr       @{(input)} The multicast address, and flags to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_addr_add(bcmx_mcast_addr_t * mcaddr);

/*********************************************************************
* @purpose  Remove a L2 Multicast address from the HW's database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_addr_remove( bcm_mac_t mac, bcm_vlan_t vid);

/*********************************************************************
* @purpose  Add a port to a L2 Multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    port         @{(input)} The port to be added
* @param    mcaddr       @{(input)} the MAC address
* @param    allrtr       @{(input)} the MAC address
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_join( bcm_mac_t mac,
                                bcm_vlan_t vid,
                                bcmx_lport_t port,
                                bcmx_mcast_addr_t * mcaddr,
                                bcmx_lplist_t * allrtr);

/*********************************************************************
* @purpose  Add a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_join_ports(bcmx_mcast_addr_t *mcaddr);
extern int usl_bcmx_multicast_join_ports (bcmx_l2_addr_t *mcaddr, bcmx_lplist_t addPorts);

/*********************************************************************
* @purpose  Remove a port from a L2 Multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    port         @{(input)} The port to be removed 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_leave ( bcm_mac_t mac, bcm_vlan_t vid , bcmx_lport_t port);

/*********************************************************************
* @purpose  Create a multicast group
*
* @param    flags       @{(input)} flags for type of group
* @param    group       @{(output)} The multicast index
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_multicast_create(uint32 flags, bcm_multicast_t *group);

/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param    mcaddr       @{(input)} The multicast address, and flags to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_multicast_addr_add(bcmx_l2_addr_t * mcaddr);

/*********************************************************************
* @purpose  Remove a L2 Multicast address from the HW's database
*
* @param    mcaddr          @{(input)} the MAC address entry
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_multicast_addr_remove(bcmx_l2_addr_t * mcaddr);

/*********************************************************************
* @purpose  Add a port to a L2 Multicast address 
*
* @param    group          @{(input)} the mcast group id
* @param    port           @{(input)} the gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_multicast_egress_add(bcmx_l2_addr_t *l2addr,
                                         bcm_gport_t port);

/*********************************************************************
* @purpose  Remove a port from a L2 Multicast address 
*
* @param    group          @{(input)} the mcast group id
* @param    port           @{(input)} the gport
* @param    encap_id       @{(input)} the encap id
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_multicast_egress_delete(bcmx_l2_addr_t *l2addr, 
                                       bcm_gport_t port);

/*********************************************************************
* @purpose  Remove a list of ports from a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_leave_ports (bcmx_mcast_addr_t *mcaddr);
extern int usl_bcmx_multicast_leave_ports (bcmx_l2_addr_t *mcaddr, bcmx_lplist_t removePorts);

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid);

/*********************************************************************
* @purpose  Sets the DVLAN MODE for all the ports 
*
* @param    mode     
* @param    ethertype
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_mode_set(L7_uint32 mode,L7_ushort16 ethertype);

/*********************************************************************
* @purpose  Used to indicate to all units whether learns are allowed
*           on the specified trunk.
*
* @param    
* @param    
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_trunk_learn_mode_set(L7_uint32 tgid, L7_BOOL learningLocked);

/*********************************************************************
* @purpose  Create a protected port group 
*
* @param    protectedGroupList    @{(input)}  List of protected port
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
extern int usl_bcmx_protected_group_set(bcmx_lplist_t protectedGroupList);

/*********************************************************************
* @purpose  Create/Delete a list of vlans in the HW
*
* @param    bulkCmd          @{(input)} L7_TRUE: Create the Vlans
*                                       L7_FALSE: Delete the Vlans
* @param    vlanMask         @{(input)} Pointer to mask of vlan-ids to be created
* @param    numVlans         @{(input)} number of vlans to be created
* @param    vlanMaskFailure  @{(output)} Pointer to mask populated by usl with 
*                                       failed vlan-ids
* @param    vlanFailureCount @{(output)} Pointer to integer to pass the number 
*                                        of vlans that could not be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_vlan_bulk_configure(L7_BOOL bulkCmd,
                                        L7_VLAN_MASK_t *vlanMask, 
                                        L7_ushort16 numVlans,
                                        L7_VLAN_MASK_t *vlanMaskFailure,
                                        L7_uint32 *vlanFailureCount);

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    mode    @{{input}}  enable=1,disable=0
* @param    mac     @{{input}}  mac address of switch
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_flow_control_set(L7_uint32 unit, L7_uint32 mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Sets the DVLAN Translation for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable);

/*********************************************************************
* @purpose  Sets the Default TPID for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_dvlan_default_tpid_set(L7_ushort16 etherType);

#endif
