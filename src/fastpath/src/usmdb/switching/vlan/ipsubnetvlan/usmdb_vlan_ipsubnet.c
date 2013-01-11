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
 * @component IPSUBNET VLAN
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_vlan_ipsubnet_api.h"
#include "osapi.h"

#include "vlan_ipsubnet_api.h"

/* Begin Function Declarations: usmdb_vlan_ipsubnet_api.h */

/**********************************************************************
 *
 * @purpose  To add a subnet to a VLAN.
 *
 * @param    unitIndex  @b((input)) Unit Index
 * @param    subnet     @b((input)) IP subnet
 * @param    netmask    @b((input)) network mask
 * @param    vlanId     @b((input)) an existing VLAN id
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
                                   L7_uint32 netmask, L7_uint32 vlanId)
{
  return vlanIpSubnetSubnetAdd(subnet, netmask, vlanId);
}

/**********************************************************************
 *
 * @purpose  To delete a subnet from a VLAN.
 *
 * @param    unitIndex  @b((input)) Unit Index
 * @param    subnet     @b((input))  IP subnet
 * @param    netmask    @b((input))  network mask
 * @param    vlanId     @b((input))  an existing VLAN id
 *
 * @returns  L7_SUCCESS, if subnet is deleted from the VLAN
 * @returns  L7_FAILURE, if unable to delete (invalid input parameters)
 * @returns  L7_ERROR, if the specified VLAN is not in use
 *
 * @comments none
 *
 * @end
 *
*********************************************************************/

/*********************************************************************
* @purpose  To get the  subnet-vlan association in the list 
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}  vlan ID associated with next subnet
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @end
*********************************************************************/
L7_RC_t usmDbVlanIpSubnetSubnetDelete(L7_uint32 unitIndex, L7_uint32 subnet,
                                      L7_uint32 netmask, L7_uint32 vlanId)
{
  return vlanIpSubnetSubnetDelete(subnet, netmask,vlanId);
}

/**********************************************************************
 *
 * @purpose  To get the VLAN ID associated with the specified subnet
 *
 * @param    unitIndex  @b((input)) Unit Index
 * @param    *subnet    @b((input)) next created subnet
 * @param    *netmask   @b((input)) next created netmask
 * @param    *vlanID    @b((input)) next created VLAN id
 *
 * @returns  L7_SUCCESS, if the next created VLAN is found
 * @returns  L7_FAILURE, if next VLAN is not found
 *
 * @comments The '*nextvlanID' is zero (0) when trying to find the very
 *           first created group.
 *
 * @end
 *
 **********************************************************************
 */
L7_RC_t usmDbVlanIpSubnetSubnetGet(L7_uint32 unitIndex, L7_uint32 subnet,
                                   L7_uint32 netmask, L7_uint32 *vlanId)
{
  return vlanIpSubnetSubnetGet(subnet, netmask, vlanId);
}

/**********************************************************************
 *
 * @purpose  Given a subnet, get the details of the next created VLAN.
 *
 * @param    unitIndex   @b((input)) Unit Index
 * @param    subnet      @b{(input)}    unit Index 
 * @param    netmask     @b{(input)}    network mask
 * @param    *nextSubnet @b{(output)}    IP Subnet
 * @param    *nextMask   @b{(output)}    network mask
 * @param    *nextvlanId @b{(output)}  vlan ID associated with next subnet
 *
 * @returns  L7_SUCCESS, if the next created VLAN is found
 * @returns  L7_FAILURE, if invalid vlanID or next VLAN is not found
 *
 * @comments The details of the next created VLAN will be returned.
 *
 * @end
 *
 **********************************************************************
 */
L7_RC_t usmDbVlanIpSubnetSubnetGetNext(L7_uint32 unitIndex, L7_uint32 subnet,
                                       L7_uint32 netmask, L7_uint32 *nextsubnet,
                                  L7_uint32 *nextnetmask, L7_uint32 *nextvlanId)
{
  return vlanIpSubnetSubnetGetNext(subnet, netmask, nextsubnet, nextnetmask,
      nextvlanId);
}

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
                                         L7_uint32 netmask, L7_uint32 vlanId)
{
  return vlanIpSubnetCfgConflictCheck(subnet, netmask, vlanId);
}

/* End Function Declarations */
