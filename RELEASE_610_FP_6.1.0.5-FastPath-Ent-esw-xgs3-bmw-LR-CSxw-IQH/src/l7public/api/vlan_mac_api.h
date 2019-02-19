/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_api.h
*
* @purpose   MAC Vlan  API file
*
* @component vlanMac
*
* @comments
*
* @create    5/20/2005
*
* @author    tsrikanth
*
* @end
*
**********************************************************************/

#ifndef VLAN_MAC_API_H
#define VLAN_MAC_API_H

/* Begin Function Prototypes */

/*********************************************************************
 * @purpose  To assign a MAC to a VLAN.
 *          
 * @param    mac      @b{(input)}    Mac Address
 * @param    vlanId   @b{(input)}    vlan ID
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE  
 *
 * @end
 *********************************************************************/
L7_RC_t vlanMacAdd(L7_enetMacAddr_t mac, L7_uint32 vlanId);

/*********************************************************************
* @purpose  To delete a mac assignment to a VLAN.
*          
* @param    mac      @b{(input)}    Mac Address
* @param    vlanId   @b{(input)}    vlan ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t vlanMacDelete(L7_enetMacAddr_t mac, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Given a MAC address, get the VLAN ID associated with the address
*          
* @param    mac       @b{(input)}    Mac Address
* @param    vlanId    @b{(input)}   vlan ID 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments 
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacGet(L7_enetMacAddr_t mac, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Given a MAC address, get the next mac address-vlan associatio
*          
* @param    mac       @b{(input)}    Mac Address
* @param    *nextMac  @b{(output)}   Mac Address
* @param    *vlanId   @b{(outinput)}   vlan ID associated with next mac
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments Multiple macs can be associated with the same VLAN
* @comments To get the first mac, pass in a  "0" for the mac value
*
* @end
*********************************************************************/
L7_RC_t vlanMacGetNext(L7_enetMacAddr_t mac, L7_enetMacAddr_t *nextMac,
                       L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Checks whether a MAC address is unicast or multicast
*           IPv4 multicast MAC range is 0x0100.5e00.0000 to 0x0100.5e7f.ffff
*
* @param    L7_enetMacAddr_t  mac  @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vlanMacIsMacAddrTypeMulticast(L7_enetMacAddr_t mac);

/* End Function Prototypes */

#endif /* VLAN_MAC_DATA_H */
