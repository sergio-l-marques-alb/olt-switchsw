/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_wdm_cent_tnnl_api.h
*
* @purpose Wireless Data Manager (WDM) USMDB API header
*
* @component WDM
*
* @comments none
*
* @create 01/03/2008
*
* @author jshaw
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_USMDB_WDM_CENT_TNNL_API_H
#define INCLUDE_USMDB_WDM_CENT_TNNL_API_H

/*********************************************************************
*
* @purpose Add or append a VLAN ID to the L2 centralized tunneling list of the Wireless switch.
*
* @param L7_ushort16 vlanId @b{(input)} new VLAN ID to append or add to the 
* @param                                L2 centralized tunneling list
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCentTnnlVlanListEntryAdd(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose Delete an existing VLAN ID from the L2 centralized tunneling list.
*
* @param L7_ushort16 vlanId @b{(input)} Existing VLAN ID in the L2 centralized tunneling 
* @param                                list to be deleted
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCentTnnlVlanListEntryDelete(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose    Purge VLAN IDs from the L2 centralized tunneling list.
*
* @param      none
*
* @returns    L7_SUCCESS
* @retruns    L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCentTnnlVlanListEntryPurge();

/*********************************************************************
*
* @purpose Validate the given VLAN ID is present in the L2 centralized tunneling list or not.
*
* @param L7_ushort16 vlanId @b{(input)} validate the given VLAN ID is existing 
* @param                                in the L2 centralized tunneling list or not
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCentTnnlVlanListEntryGet(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose Get the next available VLAN Id in the L2 centralized tunneling list of the 
* @purpose wireless switch server.
*
* @param L7_ushort16 vlanId @b{(input)} current VLAN ID present in the L2 centralized tunneling list
* @param L7_ushort16 *nextVlanId @b{(output)} location to store the next availabe VLAN ID 
* @param                                      in the L2 centralized tunneling list
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCentTnnlVlanListEntryNextGet(L7_ushort16 vlanId, L7_ushort16 *nextVlanId);

/*********************************************************************
* @purpose  Find a tunnel interface number associated with the tunnel IP
*
* @param    tunnelIpAddr	IP address associated with tunnel
* @param    *intIfNum	Internal interface number associated with tunnel
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWsCentTnnlTunnelByIpAddrFind(L7_IP_ADDR_t tunnelIpAddr, 
                                             L7_uint32 *intIfNum);

#endif /* INCLUDE_USMDB_WDM_CENT_TNNL_API_H */
