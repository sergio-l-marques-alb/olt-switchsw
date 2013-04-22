/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_wds_ap_group_status_api.h
*
* @purpose    Wireless Data Manager WDS Link USMDB API header
*
* @component  USMDB
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

#ifndef INCLUDE_USMDB_WDM_WDS_AP_GROUP_STATUS_API_H
#define INCLUDE_USMDB_WDM_WDS_AP_GROUP_STATUS_API_H

#include "l7_common.h"
#include "wireless_comm_structs.h"

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
L7_RC_t usmDbWdmWDSAPGroupNumConfiguredAPsGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumConnectedAPsGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumRootAPsGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumSatelliteAPsGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupRootDeviceTypeGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumConfiguredWDSLinkGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumDetectedWDSLinkGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupNumBlockedWDSLinkGet (L7_uchar8 groupId,
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
L7_RC_t usmDbWdmWDSAPGroupRootBridgeGet (L7_uchar8 groupId,
                            L7_enetMacAddr_t  *rootBridge);

#endif  /* INCLUDE_USMDB_WDM_WDS_AP_GROUP_STATUS_API_H */
