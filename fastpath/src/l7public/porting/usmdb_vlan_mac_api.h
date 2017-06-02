/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename  usmdb_vlan_ipsubnet.c
 *
 * @purpose   Mac Vlan USMDB APIs
 *
 * @component vlanMac
 *
 * @comments
 *
 * @create   05/24/2005 
 *
 * @author    ryadagiri
 * @end
 *
 **********************************************************************/

#ifndef USMDB_VLAN_MAC_API_H
#define USMDB_VLAN_MAC_API_H

/* Begin Function Declarations: usmdb_vlan_mac_api.h */

/********************************************************************
 *
 * @purpose  To add a mac to a VLAN.
 *
 * @param    unitIndex  @b{(input)} Unit Index
 * @param    vlanId     @b{(input)} an existing VLAN id
 * @param    mac        @b{(input)} IP mac
 *
 * @returns  L7_SUCCESS, if mac is created for the VLAN
 * @returns  L7_FAILURE, if input parameters are invalid
 * @returns  L7_ERROR, if the VLAN is already in use
 *
 * @comments none
 *
 * @end
 *
 *******************************************************************/
L7_RC_t usmDbVlanMacAdd(L7_uint32 unitIndex, L7_enetMacAddr_t mac, 
                        L7_uint32 vlanId);

/********************************************************************
 *
 * @purpose  To delete a mac from a VLAN.
 *
 * @param    unitIndex  @b{(input)} Unit Index
 * @param    vlanId     @b{(input)}  an existing VLAN id
 * @param    mac        @b{(input)}  IP mac
 *
 * @returns  L7_SUCCESS, if mac is deleted from the VLAN
 * @returns  L7_FAILURE, if unable to delete (invalid input parameters)
 * @returns  L7_ERROR, if the specified VLAN is not in use
 *
 * @comments none
 *
 * @end
 *
 *******************************************************************/
L7_RC_t usmDbVlanMacDelete(L7_uint32 unitIndex, L7_enetMacAddr_t mac,
                           L7_uint32 vlanId);

/*******************************************************************
 *
 * @purpose  To get the details of the entry
 *
 * @param    unitIndex   @b{(input)} Unit Index
 * @param    mac         @b{(input)} macAddress
 * @param    vlanID      @b{(input)} VLAN id
 *
 * @returns  L7_SUCCESS, if the entry is found
 * @returns  L7_FAILURE, if the entry is not found
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbVlanMacGet(L7_uint32 unitIndex, L7_enetMacAddr_t mac, 
                        L7_uint32 *vlanId);

/*********************************************************************
 *
 * @purpose  Given a VLAN id, get the details of the next created VLAN.
 *
 * @param    unitIndex    @b{(input)} Unit Index
 * @param    mac          @b{(input)}  created mac
 * @param    *nextmac     @b{(output)} next created mac
 * @param    *nextvlanID  @b{(output)} vlan id, next
 *
 * @returns  L7_SUCCESS, if the next created VLAN is found
 * @returns  L7_FAILURE, if invalid vlanID or next VLAN is not found
 *
 * @comments The details of the next created VLAN will be returned.
 *
 * @end
 *
 **********************************************************************/
L7_RC_t usmDbVlanMacGetNext(L7_uint32 unitIndex, L7_enetMacAddr_t mac, 
                            L7_enetMacAddr_t *nextmac, L7_uint32 *nextvlanId);

/* End Function Prototypes */
#endif /* USMDB_VLAN_MAC_API_H */
