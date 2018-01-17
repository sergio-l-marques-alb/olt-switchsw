/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_wds_ap_api.h
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

#ifndef INCLUDE_USMDB_WDM_WDS_AP_STATUS_API_H
#define INCLUDE_USMDB_WDM_WDS_AP_STATUS_API_H

#include "l7_common.h"
#include "wireless_comm_structs.h"

/*********************************************************************
*
* @purpose  Add a WDS AP entry to the Database.
*
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEntryAdd (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Delete a WDS AP from the database
*
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEntryDelete (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Get a WDS AP from the database
*
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEntryGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Get the next WDS AP from the database
*
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
* @param    L7_uchar8 *nextGroupId @b{(outinput)} Next Group Id of the AP
* @param    L7_enetMacAddr_t *nextAPMac  @b{(output)} Next AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEntryNextGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *nextGroupId, L7_enetMacAddr_t *nextAPMac);

/*********************************************************************
*
* @purpose  Set the STP Priority
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        stpPriority  @b{(input)} STP Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPSTPPrioritySet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_ushort16 stpPriority);

/*********************************************************************
*
* @purpose  Get the WDS AP STP Priority
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *stpPriority  @b{(output)} Flag Indicating
*                                                 the STP Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPSTPPriorityGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_ushort16 *stpPriority);

/*********************************************************************
*
* @purpose  Get the AP connection Status
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *apConnectionStatus  @b{(output)} Flag Indicating
*                                                 the AP connection Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPConnectionStatusGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *apConnectionStatus);

/*********************************************************************
*
* @purpose  Get the AP Satellite Mode
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *apConnectionStatus  @b{(output)} Flag Indicating
*                                                 the AP Satellite mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPSatelliteModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *apSatelliteMode);

/*********************************************************************
*
* @purpose  Get the Spanning Tree Root Mode
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *stpRootMode  @b{(output)} Flag Indicating
*                                                 the Spanning Tree Root Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPSTPRootModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *stpRootMode);

/*********************************************************************
*
* @purpose  Get the Root Path Cost
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *stpCost   @b{(output)} Pointer to Root Path Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPSTPRootPathCostGet (L7_uchar8 groupId,
                            L7_enetMacAddr_t apMac, L7_uint32 *stpCost);

/*********************************************************************
*
* @purpose  Get the Ethernet Port Spanning tree state
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *ethPortSTPState  @b{(output)} Pointer to Ethernet
*                             Port STP state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEthernetPortSTPStateGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *ethPortSTPState);

/*********************************************************************
*
* @purpose  Get the Ethernet port mode for the AP
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *ethPortMode  @b{(output)} Pointer to ethernet
*                                          port mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEthernetPortModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *ethPortMode);

/*********************************************************************
*
* @purpose  Get the ethernet port link state
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        *ethPortLinkState  @b{(output)} Pointer to ethernet
*                             port link state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWDSAPEthernetPortLinkStateGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *ethPortLinkState);

#endif /* INCLUDE_USMDB_WDM_WDS_AP_STATUS_API_H */
