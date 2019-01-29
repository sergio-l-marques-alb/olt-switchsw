/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wdm_wds_ap_api.h
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
#ifndef INCLUDE_WDM_WDS_AP_STATE_API_H
#define INCLUDE_WDM_WDS_AP_STATE_API_H

#include "datatypes.h"
#include "wdm_api.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"

/*********************************************************************
*
* @purpose  Add a WDS AP Status entry to the Database.
*
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
*
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPEntryAdd (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Delete a WDS AP from the database
*
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP Mac Address
* @param    L7_uchar8 groupId @b{(input)} Group Id of the AP
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
L7_RC_t wdmWDSAPEntryDelete (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

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
L7_RC_t wdmWDSAPEntryGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac);

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
L7_RC_t wdmWDSAPEntryNextGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *nextGroupId, L7_enetMacAddr_t *nextAPMac);

/*********************************************************************
*
* @purpose  Get Group ID for the AP MAC if present in the table.
*
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP MAC Address
* @param    L7_uchar8       *groupId @{(output)} WDS AP Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPMacWDSGroupIDGet (L7_enetMacAddr_t apMac, L7_uchar8 *groupId);

/*********************************************************************
*
* @purpose  Check if the AP exist as part of any WDS AP Group.
*
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPMacExists (L7_enetMacAddr_t apMac);

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
L7_RC_t wdmWDSAPSTPPrioritySet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
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
L7_RC_t wdmWDSAPSTPPriorityGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
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
L7_RC_t wdmWDSAPConnectionStatusGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *apConnectionStatus);

/*********************************************************************
*
* @purpose  Set the AP connection Status
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        apConnectionStatus  @b{(input)} Flag Indicating
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
L7_RC_t wdmWDSAPConnectionStatusSet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 apConnectionStatus);

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
L7_RC_t wdmWDSAPSatelliteModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *apSatelliteMode);

/*********************************************************************
*
* @purpose  Set the AP satellite mode
*
* @param    L7_uchar8        groupId    @b{(input)}  Group Id of the AP
* @param    L7_enetMacAddr_t apMac      @b{(input)}  MAC Address of the AP
* @param    L7_uchar8        apSatelliteMode  @b{(input)} Flag Indicating
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
L7_RC_t wdmWDSAPSatelliteModeSet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 apSatelliteMode);

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
L7_RC_t wdmWDSAPSTPRootModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
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
L7_RC_t wdmWDSAPSTPRootPathCostGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                                  L7_uint32 *stpCost);

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
L7_RC_t wdmWDSAPEthernetPortSTPStateGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
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
L7_RC_t wdmWDSAPEthernetPortModeGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
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
L7_RC_t wdmWDSAPEthernetPortLinkStateGet (L7_uchar8 groupId, L7_enetMacAddr_t apMac,
                            L7_uchar8 *ethPortLinkState);

/*********************************************************************
*
* @purpose  Update a AP Provisioning  entry.
*
* @param    L7_enetMacAddr_t      macAddr       @b{(input)} AP mac address
* @param    L7_uchar8             groupId       @b{(input)} WDS group id of AP
*
* @param    L7_uchar8             satelliteMode @b{(input)} satellite mode of
*                                                           the AP
* @param    L7_enetMacAddr_t      rootBridge    @b{(input)} spanning tree root
*                                                                     bridge
* @param    L7_uint32             stpCost @b{(input)} root path cost
*
* @param    L7_uchar8             spanningTreeStatus @b{(input)} spanning tree
*                                                                    status
* @param    L7_uchar8             ethPortMode   @b{(input)} ethernet port mode
*
* @param    L7_uchar8             ethPortLinkState @b{(input)} ethernet port link
*                                                              state
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPStatusEntryUpdate( L7_uchar8        groupId,
                              L7_enetMacAddr_t      apMac,
                              L7_uchar8        satelliteMode,
                              L7_enetMacAddr_t rootBridge,
                              L7_uint32        stpCost,
                              L7_uchar8        spanningTreeStatus,
                              L7_uchar8        ethPortMode,
                              L7_uchar8        ethPortLinkState);
/*********************************************************************
*
* @purpose  Get Group ID for the AP MAC if present in the table.
*
* @param    L7_enetMacAddr_t apMac  @b{(input)} AP MAC Address
* @param    L7_uchar8       *groupId @{(output)} WDS AP Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPMacGroupIDGet (L7_enetMacAddr_t apMac, L7_uchar8 *groupId);

#endif /* INCLUDE_WDM_WDS_AP_STATE_API_H */
