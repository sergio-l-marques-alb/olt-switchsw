/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_bcmx_l2.c
*
* @purpose    USL_BCMX L2 API header file
*
* @component  USL
*
* @comments   none
*
* @create     11/21/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_BCMX_L2_H
#define L7_USL_BCMX_L2_H

#include "l7_usl_bcm_l2.h"

#include "bcm/vlan.h"
#include "bcmx/l2.h"

/* PTin added: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
#include "bcm_int/common/trunk.h"
#endif

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
int usl_bcmx_l2_addr_remove_by_trunk (bcm_trunk_t tgid);

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
int usl_bcmx_l2_addr_remove_by_port (bcmx_lport_t lport);

/*********************************************************************
* @purpose  Flush specific dynamic MAC addresses.
*
* @param    mac - BCMX mac identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_mac (bcm_mac_t mac);

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
int usl_bcmx_l2_addr_remove_all ();

/*********************************************************************
* @purpose Synchronize all the dynamic L2 entries w/ the FDB application
*
* @param    none.
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_sync();

/*********************************************************************
* @purpose  Flush dynamic MAC addresses matching specified vlan.
*
* @param    vid - BCM VLAN identifier.
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
* @purpose  Add a L2 MAC address to the USL forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_l2_learned_addr_add(bcmx_l2_addr_t * l2addr ,bcmx_lplist_t *port_block);

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
extern int usl_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid);

/*********************************************************************
* @purpose  Remove a L2 MAC address from USL's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_l2_learned_addr_remove(bcm_mac_t mac_addr, bcm_vlan_t vid);

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
int usl_bcmx_vlan_create(bcm_vlan_t vid);

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_destroy(bcm_vlan_t vid);

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
 int usl_bcmx_vlan_bulk_configure(L7_BOOL bulkCmd,
                                  L7_VLAN_MASK_t *vlanMask, 
                                  L7_ushort16 numVlans,
                                  L7_VLAN_MASK_t *vlanMaskFailure,
                                  L7_uint32 *vlanFailureCount);

/*********************************************************************
* @purpose  Set the mcast flood mode for a vlan
*
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_mcast_flood_set(bcm_vlan_t vid, 
                                  bcm_vlan_mcast_flood_t mcastFloodMode);


/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_control_flag_update(bcm_vlan_t vid, L7_uint32 flags, 
                                      USL_CMD_t cmd);

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    vid               @{(input)} 
* @param    forwarding_mode   @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode);

/*********************************************************************
* @purpose  Set hashing mechanism for existing trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_psc_set (L7_uint32 appId, bcm_trunk_t tid, int psc);

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    appId        @{(input)}  Unique application identifier for
*                                    trunk
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_create(L7_uint32 appId, bcm_trunk_t * tid);

/*********************************************************************
* @purpose  Delete a trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid);

/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                       bcm_trunk_add_info_t * add_info);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appId        @{(input)} Unique application identifier for 
*                                   this stg
* @param    stg          @{(input)} Pointer the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_create(L7_uint32 appId, bcm_stg_t *pStg);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_destroy(L7_uint32 appInstId, bcm_stg_t stg);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_vlan_add(L7_uint32 appInstId, bcm_stg_t stg, 
                                 bcm_vlan_t vid);

/*********************************************************************
* @purpose  Remove an association between a Spanning Tree group and a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_stg_vlan_remove(L7_uint32 appInstId, bcm_stg_t stg, 
                                    bcm_vlan_t vid);

/*********************************************************************
* @purpose  Destroy a list returned by usl_stg_vlan_list_destroy.
*
* @param
*
* @returns  BCM_E_NONE
*
* @end
*********************************************************************/
extern int usl_stg_vlan_list_destroy(bcm_vlan_t *vlanList, int vlanCount);


/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param   mcAddr @{(input)} Multicast address to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_addr_add(usl_bcm_mcast_addr_t *mcAddr);

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
extern int usl_bcmx_mcast_addr_remove(usl_bcm_mcast_addr_t *mcAddr);

/*********************************************************************
* @purpose  Add a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_join_ports(usl_bcm_mcast_addr_t *mcaddr);

/*********************************************************************
* @purpose  Remove a list of ports from a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_leave_ports (usl_bcm_mcast_addr_t *mcaddr);

/*********************************************************************
* @purpose  Add a port to a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_port_join_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count);

/*********************************************************************
* @purpose  Remove a port from a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mcast_port_leave_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count);

/*********************************************************************
* @purpose  sets the port to the uslipsubnetvlan classfiertable
*
* @param   port    @b{(input)} Ethernet Port
* @param   type    @b{(input)} type field
* @param   arg     @b{(input)} arg field
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
int usl_ip_bcmx_vlan_control_port_set(bcmx_lport_t port,
                                      bcm_vlan_control_port_t type, int arg);

/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, bcm_vlan_t vid, int prio);

/*********************************************************************
* @purpose  Deletes the entry from the uslipsubnetvlan table
*
* @param   ipaddr     @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask);

/*********************************************************************
* @purpose  sets the port to the uslvlanmac classfiertable 
*
* @param   port    @b{(input)} Ethernet Port 
* @param   type    @b{(input)} type field 
* @param   arg     @b{(input)} arg field 
*
* @returns  bcm_error_t 
*
* @end
*
*********************************************************************/
int usl_mac_bcmx_vlan_control_port_set(bcmx_lport_t port,
                                       bcm_vlan_control_port_t type, int arg);

/*********************************************************************
* @purpose  Adds the entry to the uslvlanmac table 
*
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio);

/*********************************************************************
* @purpose  Adds the entry to the uslvlanmac table 
*
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_mac_delete(bcm_mac_t mac);

/*********************************************************************
* @purpose  Sets the DVLAN MODE for all the ports 
*
* @param    mode    @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_dvlan_mode_set(L7_uint32 mode);

/*********************************************************************
* @purpose  Used to indicate to all units whether learns are allowed
*           on the specified trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_learn_mode_set(L7_uint32 appId, bcm_trunk_t tgid, 
                                  L7_BOOL learningLocked);

/*********************************************************************
* @purpose  Create a protected port group 
*
* @param    groupInfo  @{(input)}  Protected group info
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
int usl_bcmx_protected_group_set(usl_bcm_protected_group_t *groupInfo);

/*********************************************************************
* @purpose  Delete a port from protected port group 
*
* @param    lport                 @{(input)}  port to be removed
* @param    groupId               @{(input)}  GroupId of the protected port
* @param    protectedGroupList    @{(input)}  List of protected port
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
int usl_bcmx_protected_group_port_remove(bcmx_lport_t lport, L7_uint32 groupId);

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    unit    @{{input}}  L7_ALL_UNITS - to enable on all units
*                               or unit number of the specific unit
* @param    mode    @{{input}}  enable=1,disable=0
* @param    mac     @{{input}}  mac address of switch
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_flow_control_set(L7_uint32 unit, L7_uint32 mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Enable/Disable DOS control in the system
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg);

/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_enable(int enabled);

/*********************************************************************
* @purpose  Sets the DVLAN Translation for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_dvlan_translation_set(L7_BOOL direction, 
                                          L7_BOOL enable);

/*********************************************************************
* @purpose  Sets the Default TPID for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_dvlan_default_tpid_set(L7_ushort16 ethertype);

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    mode    @{{input}}  enable=1,disable=0
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_mmu_dropmode_set(L7_uint32 mode);

#endif /* L7_USL_BCMX_L2_H */
