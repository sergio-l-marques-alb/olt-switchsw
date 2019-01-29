/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  usmdb_vlan_ipsubnet.c
*
* @purpose   
*
* @component vlanIpSubnet 
*
* @comments
*
* @create    05/20/2005
*
* @author    ryadagiri
*
* @end
*
**********************************************************************/

#ifndef USMDB_VLAN_IPSUBNET_API_H
#define USMDB_VLAN_IPSUBNET_API_H

/* Begin Function Prototypes */

/**********************************************************************
*
* @purpose  To add a subnet to a VLAN.
*
* @param    unitIndex  @b{(input)} Unit Index
* @param    vlanId     @b{(input)} an existing VLAN id
* @param    subnet     @b{(input)} IP subnet
* @param    netmask    @b{(input)} network mask
*
* @returns  L7_SUCCESS, if subnet is created for the VLAN
* @returns  L7_FAILURE, if input parameters are invalid
* @returns  L7_ERROR, if the VLAN is already in use
*
* @comments none
*
* @end
*
***********************************************************************/
L7_RC_t usmDbVlanIpSubnetSubnetAdd(L7_uint32 unitIndex, L7_uint32 subnet, 
                                   L7_uint32 netmask, L7_uint32 vlanId);

/**********************************************************************
*
* @purpose  To delete a subnet from a VLAN.
*
* @param    unitIndex  @b{(input)} Unit Index
* @param    vlanId     @b{(input)}  an existing VLAN id
* @param    subnet     @b{(input)}  IP subnet
* @param    netmask    @b{(input)}  network mask
*
* @returns  L7_SUCCESS, if subnet is deleted from the VLAN
* @returns  L7_FAILURE, if unable to delete (invalid input parameters)
* @returns  L7_ERROR, if the specified VLAN is not in use
*
* @comments none
*
* @end
*
***********************************************************************/
L7_RC_t usmDbVlanIpSubnetSubnetDelete(L7_uint32 unitIndex, L7_uint32 subnet, 
                                      L7_uint32 netmask, L7_uint32 vlanId);

/**********************************************************************
*
* @purpose  To get the details of the first created group.
*
* @param    unitIndex  @b{(input)} Unit Index
* @param    *vlanId     @b{(output)} next created VLAN id
* @param    *subnet     @b{(output)} next created subnet
* @param    *netmask    @b{(output)} next created netmask
*
* @returns  L7_SUCCESS, if the next created VLAN is found
* @returns  L7_FAILURE, if next VLAN is not found
*
* @comments The '*nextvlanID' is zero (0) when trying to find the very
*           first created group.
*
* @end
*
***********************************************************************/
L7_RC_t usmDbVlanIpSubnetSubnetGet(L7_uint32 unitIndex, L7_uint32 subnet, 
                                   L7_uint32 netmask, L7_uint32 *vlanId);

/**********************************************************************
*
* @purpose  Given a VLAN id, get the details of the next created VLAN.
*
* @param    unitIndex  @b{(input)} Unit Index
* @param    unitIndex    @b{(input)} UnitIndex
* @param    subnet       @b{(input)} subnet
* @param    netmask      @b{(input)} netmask
* @param    *nextSubnet  @b{(output)} next created subnet
* @param    *nextnetmask @b{(output)} next created netmask
* @param    *nextvlanID  @b{(output)} vlan id, next
*
* @returns  L7_SUCCESS, if the next created VLAN is found
* @returns  L7_FAILURE, if invalid vlanID or next VLAN is not found
*
* @comments The details of the next created VLAN will be returned.
*
* @end
*
***********************************************************************/
L7_RC_t usmDbVlanIpSubnetSubnetGetNext(L7_uint32 unitIndex, L7_uint32 subnet, 
                                       L7_uint32 netmask,L7_uint32 *nextsubnet, 
                                       L7_uint32 *nextnetmask, L7_uint32 *nextvlanId);

/*********************************************************************
* @purpose  To Check for the subnet conflict
*
* @param    unitIndex   @b((input)) Unit Index
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL usmDbVlanIpSubnetCfgConflictCheck(L7_uint32 unitIndex, L7_uint32 subnet,
                                         L7_uint32 netmask, L7_uint32 vlanId);

/* End Function Prototypes */

#endif /* USMDB_VLAN_IPSUBNET_API_H */
