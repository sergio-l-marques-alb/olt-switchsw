/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_api.h
*
* @purpose   IP Subnet Vlan  API file
*
* @component vlanIpSubnet
*
* @comments 
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/


#ifndef VLAN_IPSUBNET_API_H
#define VLAN_IPSUBNET_API_H

/* Begin Function Prototypes */

/*********************************************************************
 * @purpose  To assign a subnet to a VLAN.
 *          
 * @param    subnet      @b{(input)} IP Subnet
 * @param    netmask     @b{(input)} network mask 
 * @param    vlanId      @b{(input)} vlan ID
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE  
 *
 * @end
 *********************************************************************/
L7_RC_t vlanIpSubnetSubnetAdd(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, L7_uint32 vlanId);

/*********************************************************************
* @purpose  To delete a subnet assignment to a VLAN.
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask 
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetDelete(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, L7_uint32 vlanId);

/*********************************************************************
* @purpose  To get the  subnet-vlan association in the list 
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    *nextSubnet @b{(output)}    IP Subnet
* @param    *nextMask   @b{(output)}    network mask
* @param    *vlanId     @b{(output)}  vlan ID associated with next subnet
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments Multiple subnets can be associated with the same VLAN
* @comments To get the first subnet, pass in a  "0" for the subnet value
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetGetNext(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, 
                                  L7_IP_ADDR_t *nextSubnet, L7_IP_ADDR_t *nextMask, L7_uint32 *vlanId);

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
* @comments Multiple subnets can be associated with the same VLAN
* @comments To get the first subnet, pass in a  "0" for the subnet value
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetGet(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, 
                                   L7_uint32 *vlanId);

/*********************************************************************
* @purpose  To Check for the subnet conflict
*
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
L7_BOOL vlanIpSubnetCfgConflictCheck(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId);
/* End Function Prototypes */

#endif /* VLAN_IPSUBNET_API_H */
