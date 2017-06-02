

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_rpc_l2.h
*
* @purpose    New bcmx layer that issues commands to a list of units
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

#include "l7_common.h"

#ifndef L7_RPC_L2_H
#define L7_RPC_L2_H

#include "osapi.h"
#include "hpc_hw_api.h"
#include "l7_usl_bcm_l2.h"

/* Type used to pass data to remote units for bulk vlan creation */
#define USL_VLAN_BULK_MSG_TYPE_OFFSET   (0)
#define USL_VLAN_BULK_MSG_ELEM_OFFSET   (USL_MSG_TYPE_OFFSET + sizeof(L7_uint32))
#define USL_VLAN_BULK_MSG_DATA_OFFSET   (USL_VLAN_BULK_MSG_ELEM_OFFSET + sizeof(L7_uint32))
#define USL_VLAN_BULK_MSG_HDR_SIZE      (USL_VLAN_BULK_MSG_DATA_OFFSET)

typedef struct
{
  L7_uint32 vlanId;
} vlanBulkMsgElem_t;


typedef enum
{
  L7_RPC_CMD_TRUNK_CREATE = 1,
  L7_RPC_CMD_TRUNK_DESTROY,
  L7_RPC_CMD_TRUNK_INFO_SET,
  L7_RPC_CMD_TRUNK_PSC_SET
} l7RpcTrunkCmd_t;

typedef enum
{
  L7_RPC_CMD_STG_CREATE = 1,
  L7_RPC_CMD_STG_DESTROY,
  L7_RPC_CMD_STG_VLAN_UPDATE
} l7RpcStgCmd_t;

typedef enum
{
  L7_RPC_CMD_VLAN_CREATE = 1,
  L7_RPC_CMD_VLAN_DELETE,
} l7RpcVlanCmd_t;

/*********************************************************************
* @purpose  Add a L2 MAC address to the HW's forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_bcmx_l2_addr_add(bcmx_l2_addr_t * l2addr, bcmx_lplist_t *port_block,
                               L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid,
                                  L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Set the aging time
*
* @param    ageTime     @{(input)} The aging time in seconds
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_bcmx_l2_age_timer_set(L7_int32 ageTime);

/*********************************************************************
* @purpose  Create a VLAN in the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be created
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_vlan_create(bcm_vlan_t vid,
                              L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_destroy(bcm_vlan_t vid,
                               L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  RPC Client function to set the mcast flood mode for a vlan
*
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_mcast_flood_set(bcm_vlan_t vid,
                                       bcm_vlan_mcast_flood_t mcastFloodMode,
                                       L7_uint32 numChips, L7_uint32 *chips);

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
int l7_rpc_client_vlan_control_flag_update(bcm_vlan_t vid,
                                           L7_uint32  flags,
                                           USL_CMD_t  cmd,
                                           L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid               @{(input)} 
* @param    forwarding_mode   @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode);

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appId   @{(input)} Unique application identifier for 
*                              this stg
* @param    stg      @{(input/output)} Pointer the Spanning Tree Group
* @param    stgFlag  @{(input)} USL Stg flag    
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_stg_create(L7_uint32 appId, bcm_stg_t *pStg,
                                    L7_uint32 stgFlags,
                                    L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Destroy a Spanning-tree Group
*
* @param    appInstId @{(input)} Application instId
* @param    stgId    @{(input)} Hardware-id of spanning-tree group
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_stg_destroy(L7_uint32 appInstId, bcm_stg_t stgId,
                              L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Add/Remove a VLAN to/from a Spanning-tree Group
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
* @param    updateCmd    @{(input)} Add/Remove Vid to Stg
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_stg_vlan_update(L7_uint32 appInstId, bcm_stg_t stg, 
                                  bcm_vlan_t vid, USL_CMD_t updateCmd,
                                  L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Sets hashing mechanism for existing trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_psc_set (L7_uint32 appId, bcm_trunk_t tid,
                                 int psc,
                                 L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    appId        @{(input)}  Application-identifier for trunk
* @param    tid          @{(output)} The Trunk ID
* @param    flags        @{(input)}  Trunk specific flags
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_create(L7_uint32 appId, bcm_trunk_t * tid,
                               L7_uint32 flags, L7_uint32 numChips, 
                               L7_uint32 *chips);

/*********************************************************************
* @purpose  Destroy a TRUNK in the driver
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid,
                                L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                            bcm_trunk_add_info_t * add_info,
                            L7_uint32 numChips, L7_uint32 *chips);


/*********************************************************************
* @purpose  DVLAN Init routing to register the RPC call handler.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
extern void l7_dvlan_rpc_init();

/*********************************************************************
* @purpose VLAN bulk init to register the RPC call handler.
*
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_rpc_init();

/*********************************************************************
* @purpose  Makes the rpc call for a dvlan mode change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
extern int l7_rpc_client_dvlan_mode_set(L7_uint32 mode);

/*********************************************************************
* @purpose  Makes the rpc call for a dvlan translation mode change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable);

/*********************************************************************
* @purpose  Makes the rpc call for a TPID change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_dvlan_tpid_set(L7_ushort16 etherType);

/*********************************************************************
* @purpose  Makes the rpc call for a trunk learn mode change.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
*
* @returns  none
*
* @end
*********************************************************************/

L7_RC_t l7_rpc_client_trunk_learn_mode_set(L7_uint32 appId, 
                                           bcm_trunk_t tgid, L7_BOOL learningLocked);

/*********************************************************************
* @purpose  Make the RPC call to set the protected port group membership
*
* @param    group_list          @{(input)} pointer to list of protected 
*                                          group members
*
* @returns  Defined by Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_protected_group_set(usl_bcm_protected_group_t
                                               *group_list);

/*********************************************************************
* @purpose  Protected group init routine to register the RPC call handler.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
extern L7_RC_t l7_protected_group_rpc_init(void);


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
int l7_rpc_client_vlan_bulk_configure(L7_BOOL bulkCmd,
                                      L7_VLAN_MASK_t *vlanMask, 
                                      L7_ushort16 numVlans,
                                      L7_VLAN_MASK_t *vlanMaskFailure,
                                      L7_uint32 *vlanFailureCount);

/*********************************************************************
* @purpose  Delete a port from protected port group 
*
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/

int l7_rpc_client_protected_group_port_remove(usl_bcm_protected_group_delete_t 
                                                *deleteInfo); 


/*********************************************************************
* @purpose Initialize Trunk RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_trunk_rpc_init(void);


/*********************************************************************
* @purpose Initialize Spanning-tree RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_stg_rpc_init(void);

/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
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
int l7_rpc_client_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, 
                               bcm_vlan_t vid, int prio);

/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
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
int l7_rpc_client_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask);

/*********************************************************************
* @purpose Initialize Spanning-tree RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_ipsubnet_rpc_init(void);


/*********************************************************************
* @purpose Initialize Mac Vlan RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_mac_rpc_init(void);


/*********************************************************************
* @purpose  Delete the entry from HW
*
* @param   mac     @b{(input)} Mac Address
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int l7_rpc_client_vlan_mac_delete(bcm_mac_t mac);


/*********************************************************************
* @purpose  Adds the entry to the HW 
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
int l7_rpc_client_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio);

/*********************************************************************
* @purpose  System db RPC Init to register the RPC call handler.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
extern void l7_system_rpc_init(void);

/*********************************************************************
* @purpose  Makes the rpc call for Flow Control
*
* @param    unit    @{{input}}  L7_ALL_UNITS - to enable on all units
*                               or unit number of the specific unit
* @param    mode - 1 for enable, 0 for disable
*
* @returns
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_flow_control_set(L7_uint32 unit, L7_uint32 mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Makes the rpc call for DOS Control
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  
*
* @end
*********************************************************************/
int l7_rpc_client_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg);

/*********************************************************************
* @purpose  Makes the rpc call for dropmode (ingress or egress)
*
* @param    mode    @{{input}}  enable=1,disable=0
*
* @returns  
*
* @end
*********************************************************************/
int l7_rpc_client_mmu_dropmode_set(L7_uint32 mode);

#endif
