/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_nv_api.h
*
* @purpose    Wireless Data Manager Network Visualization USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     02/01/06
*
* @author     dfowler
*
* @end
*             
*********************************************************************/
#ifndef INCLUDE_USMDB_WDM_NV_API_H
#define INCLUDE_USMDB_WDM_NV_API_H
#include "wdm_nv_api.h"

/*********************************************************************
*
* @purpose  Set NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVUserDataSet(wdmNVUserData_t *ud);

/*********************************************************************
*
* @purpose  Get NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVUserDataGet(wdmNVUserData_t *ud);

/*********************************************************************
*
* @purpose  Get NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVCheckLocationGet(L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get NV config migration flag
*
* @param    L7_BOOL *flag  @b{(output)} pointer to config migration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVCfgMigrateGet(L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Reset NV config migration flag after message display in NV
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVCfgMigrateReset(void);

/*********************************************************************
*
* @purpose  Get next entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  current graph component index
* @param    L7_uchar8           *next      @b{(output)} next graph component index
* @param    wdmNVGraphCfgData_t *graph     @b{(output)} pointer to graph configuration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, no more entries
*
* @notes  index = 0 returns first graph entry
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVGraphEntryNextGet(L7_uchar8            index,
                                    L7_uchar8           *next,
                                    wdmNVGraphCfgData_t *graph);

/*********************************************************************
*
* @purpose  Add new or update existing entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  graph component index
* @param    wdmNVGraphCfgData_t *graph     @b{(output)} pointer to graph configuration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVGraphEntryAddUpdate(L7_uchar8            index,
                                      wdmNVGraphCfgData_t *graph);

/*********************************************************************
*
* @purpose  Delete an entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  graph component index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVGraphEntryDelete(L7_uchar8 index);

/*********************************************************************
*
* @purpose  Set NV configuration coordinates for this switch.
*
* @param    wdmNVCoordCfgData_t *coord  @b{(input)} pointer to switch coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Valid graphIndexes start at 1, a 0 can be used to indicate no graph
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVWSCoordSet(wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get NV configuration coordinates for this switch.
*
* @param    wdmNVCoordCfgData_t *coord  @b{(output)} pointer to switch coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVWSCoordGet(wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Add or update a NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)} peer switch IP address
* @param    wdmNVCoordCfgData_t *graph  @b{(input)} pointer to peer coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVPeerSwitchEntryAddUpdate(L7_IP_ADDR_t         ip,
                                           wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)}  peer switch IP address
* @param    wdmNVCoordCfgData_t *graph  @b{(output)} pointer to peer coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVPeerSwitchEntryGet(L7_IP_ADDR_t         ip,
                                     wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)}  peer switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVPeerSwitchEntryDelete(L7_IP_ADDR_t ip);

/*********************************************************************
*
* @purpose  Add or update a NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address
* @param    wdmNVCoordCfgData_t *graph    @b{(input)}  pointer to AP coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVAPEntryAddUpdate(L7_enetMacAddr_t     macAddr,
                                   wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address.
* @param    wdmNVCoordCfgData_t *graph    @b{(output)} pointer to AP coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVAPEntryGet(L7_enetMacAddr_t     macAddr,
                             wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVAPEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Add or update an entry in the NV detected client tree list.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)}  detected client MAC address
* @param    wdmNVCoordCfgData_t *coord   @b{(output)} pointer to detected client coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNVDCSEntryAddUpdate(L7_enetMacAddr_t   macAddr,
                                    wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for an detected client.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  detected client address.
* @param    wdmNVCoordCfgData_t *graph    @b{(output)} pointer to detected client coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmNVDCSEntryGet(L7_enetMacAddr_t     macAddr,
                              wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an entry in the NV detected client tree list.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  detected client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNVDCSEntryDelete(L7_enetMacAddr_t   macAddr);

#endif /* INCLUDE_USMDB_WDM_NV_API_H */

