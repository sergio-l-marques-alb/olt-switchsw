
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.h
*
* @purpose    USL BCM API Implementation
*
* @component  HAPI
*
* @comments   none
*
* @create     1/11/2009
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifndef L7_USL_BCM_L2_H
#define L7_USL_BCM_L2_H

#include "l7_usl_bcm.h"
#include "doscontrol_exports.h"

#include "bcm/types.h"
#include "bcm/trunk.h"
#include "bcm/vlan.h"

/* PTin added: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
#include "bcm_int/common/trunk.h"
#endif

/* Number of bytes in mask */
#define L7_CAPWAP_INDICES   ((L7_MAX_NUM_CAPWAP_TUNNEL_INTF) / (sizeof(L7_uchar8) * 8) + 1)
#define L7_CAPWAP_PORT_MAX_MASK_BIT   L7_MAX_NUM_CAPWAP_TUNNEL_INTF

/* structure definition for the vlan Mask : bitmask for all CAPWAP tunnel ports */
/* Interface storage */
typedef struct L7_WLAN_PBMP
{
  L7_uchar8   value[L7_CAPWAP_INDICES];
} L7_WLAN_PBMP_t;

/* This macro sets a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_WLAN_PBMP_SET(_pbmp,_port) \
            ((_pbmp).value[((_port) / 8)] |=  (1 << ((_port) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_WLAN_PBMP_CLEAR(_pbmp,_port) \
            ((_pbmp).value[((_port) / 8)] &=  (~(1 << ((_port) % 8) )))

/* This macro returns a non-zero value if the port is a member of the specified VLAN.
*/
#define BROAD_WLAN_PBMP_IS_SET(_pbmp,_port) \
            (_pbmp.value[(_port) / 8] & (1 << ((_port) % 8) ))


/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define BROAD_WLAN_MASKOREQ(_j, _k) {                               \
        L7_uint32 _x;                                               \
                                                                    \
        for (_x = 0; _x < L7_CAPWAP_INDICES; _x++) {                \
                (_j).value[_x] |= (_k).value[_x];                   \
        }                                                           \
}

/* MASKANDEQINV turns on the bits in mask j that are on in both mask j
   and the bitwise-inverse of mask k. */
#define BROAD_WLAN_MASKANDEQINV(_j, _k) {                            \
        L7_uint32 _x;                                                \
                                                                     \
        for (_x = 0; _x < L7_CAPWAP_INDICES; _x++) {                 \
                _j.value[_x] &= ~(_k.value[_x]);                     \
        }                                                            \
}

#define BROAD_WLAN_PBMP_ITER(_pbmp, _port) \
	for ((_port) = 0; (_port) < L7_CAPWAP_PORT_MAX_MASK_BIT; (_port)++) \
		if (BROAD_WLAN_PBMP_IS_SET((_pbmp), (_port)))

#define BROAD_WLAN_GPORT_TO_ID   BCM_GPORT_WLAN_PORT_ID_GET

#define BROAD_WLAN_ID_TO_GPORT   BCM_GPORT_WLAN_PORT_ID_SET


typedef struct usl_bcm_mcast_addr_s {
    bcm_mac_t           mac;                                     /* 802.3 MAC address */
    bcm_vlan_t          vid;                                     /* VLAN identifier */
    bcm_cos_t           cos_dst;                                 /* COS based on dst addr */
    bcm_pbmp_t          mod_pbmp[L7_MOD_MAX];                    /* Port bitmap */
    bcm_pbmp_t          mod_ubmp[L7_MOD_MAX];                    /* Untag port bitmap */
    L7_WLAN_PBMP_t      wlan_pbmp;
    uint32              l2mc_index;                              /* XGS: l2mc index */
} usl_bcm_mcast_addr_t;

/* Type used to store IpSubnet Vlan Entry*/
typedef struct
{
  L7_IP_ADDR_t ipSubnet;
  L7_IP_MASK_t netMask;
  L7_uint32    vlanId;
  int          prio;
} usl_bcm_vlan_ipsubnet_t;

/* Type used to store Mac Vlan Entry*/
typedef struct
{
  bcm_mac_t        mac;
  L7_uint32        vlanId;
  int              prio;
} usl_bcm_vlan_mac_t;

typedef struct
{
  L7_uint32  groupId;
  bcm_pbmp_t mod_pbmp[L7_MOD_MAX];
} usl_bcm_protected_group_t;

typedef struct
{
  L7_uint32  groupId;
  L7_uint32  modid;
  L7_int32   bcmPort;
} usl_bcm_protected_group_delete_t;


#define USL_BCM_PROTECTED_GROUP_INVALID_ID L7_PROTECTED_PORT_MAX_GROUPS

#define USL_BCM_STG_CREATE_WITH_ID   (1 << 0)
#define USL_BCM_TRUNK_CREATE_WITH_ID (1 << 1)
#define USL_BCM_WLAN_CREATE_WITH_ID (1 << 2)

/*********************************************************************
*
* @purpose  USL bcm function to remove a port from protected group
*
* @param   deleteInfo  @b{(input)}   Deleted protected port information
*
* @returns Defined by Broadcom driver
* @notes   none
*
* @end
*
*********************************************************************/
int usl_bcm_protected_group_port_remove(usl_bcm_protected_group_delete_t *deleteInfo);

/*********************************************************************
*
* @purpose  USL bcm function to configure a group of protected ports
*
* @param   groupInfo  {(input)}   Pointer to list of protected ports
* @param   hwCommit   {(input)}   Commit the data to hardware
*
* @returns Defined by Broadcom driver
* @notes   none
*
* @end
*
*********************************************************************/
int usl_bcm_protected_group_set(usl_bcm_protected_group_t *groupInfo);

/*********************************************************************
* @purpose  Create or delete a vlan
*
* @params   vid    {(input)} vlan-id
* @params   create {(input)} L7_TRUE: Create the vlan
*                            L7_FALSE: Delete the vlan
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_update(bcm_vlan_t vid, L7_BOOL create);


int usl_bcm_vlan_cpu_port_update(bcm_vlan_t vlan);

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
int usl_bcm_vlan_mcast_flood_set(bcm_vlan_t vid, 
                                 bcm_vlan_mcast_flood_t mcastFloodMode);

/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} USL_CMD_ADD: Add the specified flags
*                                                to existing flags
*                                   USL_CMD_REMOVE: Remove the flags
*                                                   from the existing flags
*                                   USL_CMD_SET: Override the existing flags
*                                                with the specified flags.
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcm_vlan_control_flag_update(bcm_vlan_t vid, L7_uint32 flags,
                                     USL_CMD_t cmd);

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    vid                    @{(input)} 
* @param    forwarding_mode        @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcm_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode);

/*********************************************************************
* @purpose  Sets hashing mechanism for existing trunk
*
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_trunk_psc_set(L7_uint32 appId, bcm_trunk_t tid, int psc);

/*********************************************************************
* @purpose  Create a trunk
*
* @param    appId   @{(input)} Application identifier for the trunk
* @param    flags   @{(input)}   Trunk flags 
* @param    tid     @{(input/output)} Tid
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_create(L7_uint32 appId, L7_uint32 flags, bcm_trunk_t *tid);

/*********************************************************************
* @purpose  Delete a trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid);

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
int usl_bcm_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                      bcm_trunk_add_info_t * addInfo);

/*********************************************************************
* @purpose  USL BCM API to create Stg in hardware
*
*
* @param appId @{(input)} Unique application identifier for 
*                         this stg
* @param stg @{(input/output)} Pointer the Spanning Tree Group
* @param flags @{(input)} USL_BCM stg flag
*
*
* @param    flags       
* @returns  Defined by the Broadcom driver
*
* @end
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_stg_create(L7_uint32 appId, L7_uint32 flags, L7_int32 *stg);

/*********************************************************************
* @purpose Destroy a spanning-tree group
*
* @param    appInstId  @{(input)} Application Inst Id
* @param    stg        @{(input)} Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_stg_destroy(L7_uint32 appInstId, bcm_stg_t stgId);

/*********************************************************************
* @purpose  Add/Remove a VLAN to/from a Spanning-tree Group
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
* @param    cmd          @{(input)} Add or Remove vlan
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_stg_vlan_update(L7_uint32 appInstId,
                            bcm_stg_t stgId, 
                            bcm_vlan_t vid, 
                            USL_CMD_t cmd);

/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
*
* @param    ipSubnetData @{(input)} IP Subnet vlan data
*
* @returns  BCM Return code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_ip4_add(usl_bcm_vlan_ipsubnet_t *ipSubnetData);

/*********************************************************************
* @purpose  Deletes the Ip Subnet Vlan entry from the HW table
*
* @param    ipSubnetData @{(input)} IP Subnet vlan data
*
* @returns  BCM Return code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_ip4_delete(usl_bcm_vlan_ipsubnet_t *ipSubnetData);

/*********************************************************************
* @purpose  Adds the Vlan mac entry to the HW 

* @param   macData  @b{(input)} Mac vlan data
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_mac_add(usl_bcm_vlan_mac_t *macData);

/*********************************************************************
* @purpose  Deletes the Vlan mac entry from the HW 

* @param   macData  @b{(input)} Mac vlan data
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_mac_delete(usl_bcm_vlan_mac_t *macData);

/*********************************************************************
* @purpose  USL BCM API to add L2 multicast addresses
*
* @param    mcAddr  @{(input)} Mcast Group Info
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int usl_bcm_mcast_addr_add(usl_bcm_mcast_addr_t *mcAddr);


/*********************************************************************
* @purpose  USL BCM function to remove L2 multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int usl_bcm_mcast_addr_remove(usl_bcm_mcast_addr_t *mcAddr);


/*********************************************************************
* @purpose Handle L2MC Address Port Add/Remove commands
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to egress object information
* @param    buf_len - buffer length
* @param    status - return code 
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_mcast_ports_add_remove(usl_bcm_mcast_addr_t *mcAddr,
                                   USL_CMD_t     updateCmd);

/*********************************************************************
* @purpose  Handle L2MC Address Port Add/Remove commands
*
* @param    unit         - bcm unit
* @param    port         - bcm port
* @param    l2mc_index[] - array of L2MC indices
* @param    l2mc_index   - count of L2MC indices 
* @param    updateCmd    - add/remove
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_port_mcast_groups_update(int unit, bcm_port_t port, L7_uint32 *l2mc_index, L7_uint32 l2mc_index_count, USL_CMD_t updateCmd);

/*********************************************************************
* @purpose  Set bit in the uslTgidMask
*
* @param    
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void usl_trunk_learn_mode_set(L7_uint32 tgid, L7_uint32 learningLocked);


/*********************************************************************
* @purpose  Set the learning mode for trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    learnMode    @{(input)} 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_learn_mode_set(L7_uint32 appId, bcm_trunk_t tid, 
                                 L7_BOOL learnMode);

/*********************************************************************
* @purpose Set the flow control mode
*
* @param    mode {(input)} flow-control mode
* @param    mac  {(input)} System mac
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_flow_control_set(L7_uint32 mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose Set the DOS control mode
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg);

/*********************************************************************
* @purpose  Custom RPC function to handle modifications to the
*           source interface when processing RPF failures.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_enable_set (L7_uint32 enable);

/*********************************************************************
* @purpose  Sets the DVLAN mode for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_mode_set(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the DVLAN TPID for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_tpid_set(L7_ushort16 tpid);

/*********************************************************************
* @purpose  Sets the DVLAN translation for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable);


/*********************************************************************
* @purpose  Custom RPC function to handle setting the dropmode (ingress/egress)
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_mmu_dropmode_set (L7_uint32 mode);
#endif
