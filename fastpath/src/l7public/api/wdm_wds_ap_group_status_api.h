/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wdm_wds_ap_group_state_api.h
*
* @purpose    Wireless Data Manager WDS Link API header
*
* @component  WDM
*
* @comments   none
*
* @create     5/18/2009
*
* @author     gauravk
*
* @end
*
*********************************************************************/
#ifndef INCLUDE_WDM_WDS_AP_GROUP_STATE_API_H
#define INCLUDE_WDM_WDS_AP_GROUP_STATE_API_H

#include "datatypes.h"
#include "wdm_api.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"

/*********************************************************************
*
* @purpose  Add an entry to the WDS AP Group Status database.
*
* @param    L7_char8 groupId @b{(input)} WDS Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupStatusEntryAdd(L7_uchar8 groupId);

/*********************************************************************
*
* @purpose  Delete an entry from the WDS AP Group Status database.
*
* @param    L7_uint32 groupId @b{(input)} WDS Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupStatusEntryDelete(L7_uint32 groupId);

/*********************************************************************
*
* @purpose  Get the number of configured APs in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numConfiguredAPs  @b{(output)} Pointer to
*                             number of configured APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumConfiguredAPsGet (L7_uchar8 groupId,
                            L7_uchar8 *numConfiguredAPs);

/*********************************************************************
*
* @purpose  Get the number of configured APs in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numConnectedAPs  @b{(output)} Pointer to
*                             number of configured APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumConnectedAPsGet (L7_uchar8 groupId,
                            L7_uchar8 *numConnectedAPs);

/*********************************************************************
*
* @purpose  Get the number of root APs in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numRootAPs  @b{(output)} Pointer to
*                             number of root APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumRootAPsGet (L7_uchar8 groupId,
                            L7_uchar8 *numRootAPs);

/*********************************************************************
*
* @purpose  Get the number of satellite APs in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numSatelliteAPs  @b{(output)} Pointer to
*                             number of satellite APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumSatelliteAPsGet (L7_uchar8 groupId,
                            L7_uchar8 *numSatelliteAPs);

/*********************************************************************
*
* @purpose  Get the root device type
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *rootDeviceType  @b{(output)} Pointer to
*                             root device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupRootDeviceTypeGet (L7_uchar8 groupId,
                            L7_uchar8 *rootDeviceType);

/*********************************************************************
*
* @purpose  Get the number of configured WDS link in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numConfiguredWDSLinks  @b{(output)} Pointer to
*                             number of configured WDS link.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumConfiguredWDSLinkGet (L7_uchar8 groupId,
                            L7_uchar8 *numConfiguredWDSLinks);

/*********************************************************************
*
* @purpose  Get the number of detected WDS link in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numDetectedWDSLinks  @b{(output)} Pointer to
*                             number of number of detected WDS link.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumDetectedWDSLinkGet (L7_uchar8 groupId,
                            L7_uchar8 *numDetectedWDSLinks);

/*********************************************************************
*
* @purpose  Get the number of blocked WDS link in WDS AP group
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *numBlockedWDSLinks  @b{(output)} Pointer to
*                             number of blocked WDS link.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupNumBlockedWDSLinkGet (L7_uchar8 groupId,
                            L7_uchar8 *numBlockedWDSLinks);

/*********************************************************************
*
* @purpose  Get the Spanning tree root bridge
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_uchar8        *rootBridge  @b{(output)} Pointer to
*                             the root bridge.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPGroupRootBridgeGet (L7_uchar8 groupId,
                            L7_enetMacAddr_t  *rootBridge);
#endif
