
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_cent_tnnl_api.h
*
* @purpose      Centralized L2 tunneling control function headers
*
* @component    Wireless
*
* @comments     none
*
* @create       12/12/2007
*
* @author       jshaw
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_WDM_CENT_TNNL_API_H
#define INCLUDE_WDM_CENT_TNNL_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"

/*********************************************************************
*
* @purpose Add or append a VLAN ID to the L2 centralized tunnel list of the Wireless
*          Switch server.
*
* @param vlanId @b{(input)} new VLAN ID to append or add to the L2 centralized tunnel
*                           list
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWsCentTnnlVlanListEntryAdd(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose Delete an existing VLAN ID from the L2 centralized tunnel list.
*
* @param vlanId @b{(input)} Existing VLAN ID in the L2 centralized tunnel list
*                           to be deleted
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWsCentTnnlVlanListEntryDelete(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose    Purges VLAN IDs from the L2 centralized tunnel list.
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
L7_RC_t wdmWsCentTnnlVlanListEntryPurge();

/*********************************************************************
*
* @purpose Validate the given VLAN ID is present in the L2 centralized tunnel list
*          or not.
*
* @param vlanId @b{(input)} validate the given VLAN ID is existing in the
*                           L2 centralized tunnel list or not
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWsCentTnnlVlanListEntryGet(L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose Get the next available VLAN Id in the L2 centralized tunnel list of the
*          wireless switch server.
*
* @param vlanId @b{(input)} current VLAN ID present in the L2 centralized tunnel list
* @param nextVlanId @b{(output)} location to store the next availabe VLAN ID
*                                in the L2 centralized tunnel list
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWsCentTnnlVlanListEntryNextGet(L7_ushort16  vlanId,
                                          L7_ushort16 *nextVlanId);

/*********************************************************************
*
* @purpose  Check if the tunneled vlan list has been changed for the WS.
*
* @param    
*
* @returns  L7_TRUE     if the tunneled vlan list has been changed
* @returns  L7_FALSE    if the tunneled vlan list has not been changed
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL wdmWSCentTnnlVlanListChanged(void);

/*********************************************************************
*
* @purpose  Reset the tunnel vlan list changed flag for the Wireless Switch.
*
* @param    
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void wdmWSCentTnnlVlanListChangedReset(void);

/*********************************************************************
*
* @purpose Add all l2 tunnel interfaces to a specified vlan
*
* @param vlanId @b{(input)} vlan to add all l2 tunnels to 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmCentTnnlAllTunnelsToVlanAdd(L7_ushort16 vlanId);

#endif /* INCLUDE_WDM_CENT_TNNL_API_H */
