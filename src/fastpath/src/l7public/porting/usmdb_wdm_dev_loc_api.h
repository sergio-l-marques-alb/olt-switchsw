/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_dev_loc_api.h
*
* @purpose    Wireless Device Location USMDB API functions
*
* @component  wireless
*
* @comments   none
*
* @create     06/10/2009
*
* @author     syed moin ahmed
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_USMDB_WDM_DEV_LOC_H 
#define INCLUDE_USMDB_WDM_DEV_LOC_H 

#include "wdm_dev_loc_api.h"

/*********************************************************************
*
* @purpose  Add an entry to Building Table.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_char8 *desc     @b{(input)} building description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingEntryAdd (L7_uint32 bldngNum,
                                        L7_char8 *desc);

/*********************************************************************
*
* @purpose  Check if an entry exists in Building Object table.
*
* @param    L7_uint32  bldngNum @b{(input)} Building number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingEntryGet(L7_uint32 bldngNum);
/*********************************************************************
*
* @purpose  Add and Update the building floor data.
*
* @param    L7_uint32 bldngNum            @b{(input)} building number
* @param    L7_uint32 flrNum              @b{(input)} floor number
* @param    L7_char8  *flrDesc            @b{(input)} floor Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrEntryAdd (L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_char8 *flrDesc);

/*********************************************************************
*
* @purpose  Add and Update the managed AP Physical location data.
*
* @param    L7_uint32 bldngNum            @b{(input)} building number
* @param    L7_uint32 flrNum              @b{(input)} floor number
* @param    L7_enetMacAddr_t  apMac       @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryAdd (L7_uint32 bldngNum,
                                            L7_uint32 flrNum,
                                            L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Check if building number and floor number and AP Mac exists or not
*             and get value if existing.
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryGet(L7_uint32 bldngNum,
                                           L7_uint32 flrNum,
                                           L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose   Set the XY-co-ordinate for Managed Ap Physical Location
*
* @param    L7_uint32  bldngNum        @b{(input)} Building number
* @param    L7_uint32  flrNum          @b{(input)} Floor number
* @param    L7_enetMacAddr_t apMac     @b{(input)} AP Mac Address
* @param    wdmDevLocXYCoord_t coord   @b{(input)} XY coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocXYCoordSet(L7_uint32 bldngNum,
                                             L7_uint32 flrNum,
                                             L7_enetMacAddr_t apMac,
                                             L7_WDM_MEASUREMENT_SYS_NAME_t measSys,
                                             devLocCoord_t coord);

/*********************************************************************
*
* @purpose   Validate XY-co-ordinate for AP Physical Location
*
* @param    L7_WDM_MEASUREMENT_SYS_NAME_t measSys @b{(input)} Measurement System
* @param    L7_int32  x-coord @b{(input)} X coordinate
* @param    L7_int32  y-coord @b{(input)} Y coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocXYCoordValidate(L7_WDM_MEASUREMENT_SYS_NAME_t measSys,
                                                  L7_int32 x_coord, L7_int32 y_coord);

/*********************************************************************
*
* @purpose  Get building description.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_char8  *desc    @b{(output)} building Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingDescGet (L7_uint32 bldngNum, L7_char8 *desc);

/*********************************************************************
*
* @purpose  Check if next building number exists or not,
*           and get value if existing.
*
* @param    L7_uint32 *bldngNum @b{(input)} building number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingEntryNextGet (L7_uint32 *bldngNum);

/*********************************************************************
*
* @purpose  Delete an entry in the Building Table.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingEntryDelete (L7_uint32 bldngNum);

/*********************************************************************
*
* @purpose  Delete all entries from the Building Table.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingEntryAllDelete(void);

/*********************************************************************
*
* @purpose  Delete an Building floor Entry
*
* @param    L7_uint32 bldngNum            @b{(input)} building number
* @param    L7_uint32 flrNum              @b{(input)} floor number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrEntryDelete (L7_uint32 bldngNum,
                                           L7_uint32 flrNum);

/*********************************************************************
*
* @purpose  Delete all floor Entries of given building.
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrEntryDeleteAll(L7_uint32 bldngNum);

/*********************************************************************
*
* @purpose  Delete an Managed Ap Physical Location Entry
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryDelete(L7_uint32 bldngNum,
                                              L7_uint32 flrNum,
                                              L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Check if building number and floor number and AP Mac exists or not
*             and get value if existing.
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryGet(L7_uint32 bldngNum,
                                           L7_uint32 flrNum,
                                           L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Check if an entry exists in Building Floor Object table.
*
* @param    L7_uint32 bldngNum  @b{(input)} Building number
* @param    L7_uint32 flrNum    @b{(input)} Floor number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrEntryGet(L7_uint32 bldngNum, L7_uint32 flrNum);

/*********************************************************************
*
* @purpose  Check if next building number and floor number exists or not
*           and get value if existing
*
* @param    L7_uint32 bldngNum      @b{(input)} building number.
* @param    L7_uint32 flrNum        @b{(input)} floor number.
* @param    L7_uint32 *nextBldngNum @b{(output)} next building number.
* @param    L7_uint32 *nextFlrNum   @b{(output)} next floor number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrEntryNextGet (L7_uint32 bldngNum,
                                            L7_uint32 flrNum,
                                            L7_uint32 *nextBldngNum,
                                            L7_uint32 *nextFlrNum);

/*********************************************************************
*
* @purpose  Get building floor description.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_uint32 flrNum   @b{(input)} floor number
* @param    L7_char8  *flrDesc @b{(output)} building floor Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrDescGet (L7_uint32 bldngNum,
                                       L7_uint32 flrNum,
                                       L7_char8 *flrDesc);

/*********************************************************************
*
* @purpose  Set building description.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_char8  *desc    @b{(input)} building description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBuildingDescSet (L7_uint32 bldngNum, L7_char8 *desc);

/*********************************************************************
*
* @purpose  Set building description.
*
* @param    L7_uint32 bldngNum  @b{(input)} building number
* @param    L7_uint32 flrNum    @b{(input)} floor number
* @param    L7_char8  *flrDesc  @b{(input)} building Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrDescSet (L7_uint32 bldngNum,
                                       L7_uint32 flrNum,
                                       L7_char8 *flrDesc);

/*********************************************************************
*
* @purpose  Check if next building number and floor number and Ap Mac
*            exists or not and get value if existing
*
* @param    L7_uint32 bldngNum          @b{(input)} building number.
* @param    L7_uint32 flrNum            @b{(input)} floor number.
* @param    L7_enetMacAddr_t apMac      @b{(input)} AP Mac.
* @param    L7_uint32 *nextBldngNum     @b{(output)} next  building number.
* @param    L7_uint32 *nextFlrNum       @b{(output)} next floor number.
* @param    L7_enetMacAddr_t *nextapMac @b{(output)} next ap Mac address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryNextGet (L7_uint32 bldngNum,
                                                L7_uint32 flrNum,
                                                L7_enetMacAddr_t apMac,
                                                L7_uint32 *nextBldngNum,
                                                L7_uint32 *nextFlrNum,
                                                L7_enetMacAddr_t *nextapMac);

/*********************************************************************
*
* @purpose   Get the XY-co-ordinate for Managed Ap Physical Location
*
* @param    L7_uint32        bldngNum @b{(input)} Building number
* @param    L7_uint32        flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t apMac    @b{(input)} AP Mac Address
* @param    wdmDevLocXYCoord_t *coord @b{(output)} XY coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocXYCoordGet (L7_uint32 bldngNum,
                                              L7_uint32 flrNum,
                                              L7_enetMacAddr_t apMac,
                                              devLocCoord_t *coord);

/*********************************************************************
*
* @purpose  Delete all Managed AP Physical Location Table Entries.
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApLocEntryDeleteAll ();

/*********************************************************************
*
* @purpose  Delete all Managed AP's depending on the building number
*           floor number.
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngFlrApDelete (L7_uint32 bldngNum,
                                        L7_uint32 flrNum);

/*********************************************************************
*
* @purpose  Delete all Managed AP's depending on the building number
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocBldngApDelete (L7_uint32 bldngNum);
/*********************************************************************
*
* @purpose Get the mac address of search target device.
*
* @param   L7_enetMacAddr_t *targetMac @b{(input)} pointer to the AP MAC Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusMacGet(L7_enetMacAddr_t *targetMac);

/*********************************************************************
*
* @purpose  Get the type of the device (AP/Client) to be searched.
*
* @param    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t *type @b{(input)} 
*                                           pointer to the device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusDevTypeGet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t *type);

/*********************************************************************
*
* @purpose  Find the number of Locator APs, given the trigger inputs.
*
* @param    L7_enetMacAddr_t targetMac @b{(input)} macaddress of the target.
* @param    L7_uint8 targetType @b{(input)} type of the target (Ap/Client).
* @param    L7_uint8 bNum @b{(input)} building number.
* @param    L7_uint8 fNum @b{(input)} floor number.
* @param    L7_uint8 radioType @b{(input)} radio type
*                                  (Sentry/Both operational & sentry type).
* @param    L7_uint32 *apNum @b{(input)} pointer to the number of locator APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API doesn't update the trigger status table.  All the trigger
*           inputs are supplied for it to just calculate and give back th number.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusLocatorApsFindOnly(L7_enetMacAddr_t targetMac,
                                                    L7_uint8 targetType,
                                                    L7_uint8 bNum,
                                                    L7_uint8 fNum,
                                                    L7_uint8 radioType,
                                                    L7_uint32 *apNum);

/*********************************************************************
*
* @purpose  Set the number of Locator APs available for search operation.
*
* @param    L7_uint32 num @b{(input)} number of locator APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Number of locator APs depends upon the building/floor/sentry
*           scope specified by the adminitrator.  This shall be calculated 
*           by caller of this function in application scope.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusLocatorApsFindUpdate(L7_uint32 *apNum);



/*********************************************************************
*
* @purpose  Get the Building number to be searched in for target device.
*
* @param    L7_uint8 *num @b{(input)} pointer to the building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusBuildingNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the Floor number to be searched on for target device.
*
* @param    L7_uint8 *num @b{(input)} pointer to the floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusFlrNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the type of the radio (Sentry/Operational) to be used for 
*           search operation.
*
* @param    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t *type @b{(input)} 
*                                           pointer to the radio type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusManagedApRadioTypeGet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t *type);

/****************************************************************************
*
* @purpose  Get the On-demand-Location triggering status.
*
* @param    L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status @b{(input)} 
*                   pointer to the status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*****************************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusGet(
                    L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status);

/*********************************************************************
*
* @purpose Set the mac address of search target device.
*
* @param   L7_enetMacAddr_t targetMac @b{(output)} AP MAC Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusMacSet(L7_enetMacAddr_t targetMac);

/*********************************************************************
*
* @purpose  Get the type of the device (AP/Client) to be searched.
*
* @param    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t type @b{(input)} device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusDevTypeSet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t type);

/*********************************************************************
*
* @purpose  Set the Building number to be searched in for target device.
*
* @param    L7_uint8 num @b{(output)} building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusBuildingNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the Floor number to be searched on for target device.
*
* @param    L7_uint8 *num @b{(output)} pointer to the floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusFlrNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the type of the radio (Sentry/Operational) to be used for 
*           search operation.
*
* @param    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type @b{(output)} radio type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocTrigStatusManagedApRadioTypeSet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type);

/*********************************************************************
*
* @purpose  Trigger the On-demand-Location Procedure.
*
* @param    none.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocProcedureInitiate(void);

/*********************************************************************
*
* @purpose  Get the search operation comamnd value.
*
* @param    L7_uint32  *cmd @b{(output)} triggering command.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Always returns L7_DISABLE.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocProcedureStatusGet(L7_uint8 *cmd);

/*********************************************************************
*
* @purpose  Get the search status of the target.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_WDM_ONDEMAND_LOC_FLR_STATUS_t *status@b{(output)} 
*                   pointer to the status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusGet( L7_uint8 bNum, L7_uint8 fNum,
                    L7_WDM_ONDEMAND_LOC_FLR_STATUS_t *status);


/*********************************************************************
*
* @purpose  Get the search solutino type.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_t *type@b{(output)} 
*                   pointer to the status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusSolTypeGet( L7_uint8 bNum, L7_uint8 fNum,
                    L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_t *type);

/*********************************************************************
*
* @purpose  Get the target coordinates.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    devLocCoord_t *value @b{(output)} pointer to the coordinates.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusCoordsGet(L7_uint8 bNum, L7_uint8 fNum, 
                                           devLocCoord_t *value);

/*********************************************************************
*
* @purpose  Get the radius value.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 *value @b{(output)} pointer to the radius.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusRadiusGet(L7_uint8 bNum, L7_uint8 fNum, 
                                           L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the standard deviation(sigma).
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 *value @b{(output)} pointer to the sigma.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusSigmaGet(L7_uint8 bNum, L7_uint8 fNum, 
                                          L7_int32 *value);

/*********************************************************************
*
* @purpose  Get the number of Aps that detected the device.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 *value @b{(output)} pointer to the number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocFlrStatusLocatorApsGet(L7_uint8 bNum, L7_uint8 fNum, 
                                               L7_uint32 *num);

/*********************************************************************
*
* @purpose  Get the Building number, where target device searched.
*
* @param    L7_uint8 *num @b{(output)} pointer to the building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusBuildingNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the floor number, where target device searched.
*
* @param    L7_uint8 *num @b{(output)} pointer to the floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusFlrNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the search devicde (AP/Client) type.
*
* @param    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t *type @b{(output)} 
*                                           pointer to the device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusDevTypeGet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t *type);

/*********************************************************************
*
* @purpose  Get the radio type (Sentry/Operational) used for 
*           device search.
*
* @param    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t *type @b{(output)} 
*                                           pointer to the radio type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusManagedApRadioTypeGet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t *type);

/****************************************************************************
*
* @purpose  Get the On-demand-Location global status.
*
* @param    L7_WDM_ONDEMAND_LOC_STATUS_t *status @b{(output)} 
*                   pointer to the status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*****************************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusGet(
                    L7_WDM_ONDEMAND_LOC_STATUS_t *status);

/*********************************************************************
*
* @purpose Get the mac address of search target.
*
* @param   L7_enetMacAddr_t *targetMac @b{(output)} pointer to the MAC Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusMacGet(L7_enetMacAddr_t *targetMac);

/*********************************************************************
*
* @purpose  Get the number of Locator APs used for device search.
*
* @param    L7_uint32 *num @b{(output)} pointer to the number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusLocatorApsGet(L7_uint32 *num);

/*********************************************************************
*
* @purpose  Get the number of APs that detected the search target.
*
* @param    L7_uint32 *num @b{(output)} pointer to the number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusDetectingApsGet(L7_uint32 *num);

/*********************************************************************
*
* @purpose  Get the Number of Buildings where the managed APs detected
*           the signal.
*
* @param    L7_uint32 *num @b{(output)} pointer to the building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusDetectBuildingsGet(L7_uint32 *num);

/*********************************************************************
*
* @purpose  Get the Number of Floors where the managed APs detected
*           the signal.
*
* @param    L7_uint32 *num @b{(output)} pointer to the floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusDetectFlrsGet(L7_uint32 *num);


/*********************************************************************
*
* @purpose  Get the building num. with highest detected the signal.
*
* @param    L7_uint8 *num @b{(output)} pointer to the building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusHighestDetectBuildingGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the floor num. with highest detected the signal.
*
* @param    L7_uint8 *num @b{(output)} pointer to the floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusHighestDetectFlrGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Verify presence of a triangulation status table entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} target mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next triangulation status table entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} target mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
                                                   L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the type of data(No location/Location).
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} target mac address
* @param    L7_uint32                  *type    @b{(output)} data type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusTypeGet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 *type);

/*********************************************************************
*
* @purpose  Get status for Triangulation Status table entry.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} target mac address
* @param    L7_uint32                  *status  @b{(output)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusGet(L7_enetMacAddr_t macAddr,
                                          L7_uint32 *status);

/*********************************************************************
*
* @purpose  Get building number for Triangulation Status table entry.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} target mac address
* @param    L7_uint32                  *bldngNum @b{(output)} building number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusBldngGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *bldngNum);

/*********************************************************************
*
* @purpose  Get floor number for Triangulation Status table entry.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} target mac address
* @param    L7_uint32                  *flrNum   @b{(output)} floor number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusFlrGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 *flrNum);

/*********************************************************************
*
* @purpose  Get xy coordinate for Triangulation Status table entry.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} target mac address
* @param    devLocCoord_t              *coord    @b{(output)} xy-coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusXYCoordGet(L7_enetMacAddr_t macAddr,
                                                 devLocCoord_t *coord);

/*********************************************************************
*
* @purpose  Get time since last successful location computation.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} target mac address
* @param    L7_uint32                  *age      @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusAgeGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 *age);

/*********************************************************************
*
* @purpose  Get the device type (AP/Client).
*
* @param    L7_enetMacAddr_t     macAddr     @b{(input)} target mac address
* @param    L7_uint32            *deviceType @b{(output)} device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationLocStatusDeviceTypeGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *deviceType);
/*********************************************************************
*
* @purpose  Get the mac address of next AP triangulation status table entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} target mac address
* @param    L7_enetMacAddr_t *nextMacAddr   @b{(output)} pointer to next mac address
* @param    L7_uchar8        *devStatus     @b{(output)} pointer to device status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationAPLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
                                                    L7_enetMacAddr_t *nextMacAddr,
                                                    L7_uchar8 *devStatus);

/*********************************************************************
*
* @purpose  Get the mac address of next client triangulation status table entry
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} target mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
* @param    L7_uchar8        *devStatus     @b{(output)} pointer to device status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTriangulationClientLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
                                                        L7_enetMacAddr_t *nextMacAddr,
                                                        L7_uchar8 *devStatus);
/*********************************************************************
*
* @purpose  Get the number of configured floor and locator AP entries 
*           on the specified building.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_uint32 *count   @b{(output)} pointer to count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocFloorApCountGet (L7_uint32 bldngNum, L7_uint32 *flrCount,
                                       L7_uint32 *apCount);

/*********************************************************************
*
* @purpose  Get the number Locator AP entries configured on 
*           specified building-floor.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_uint32 flrNum   @b{(input)} floor number
* @param    L7_uint32 *count   @b{(output)} pointer to count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocApCountGet (L7_uint32 bldngNum,
                                  L7_uint32 flrNum,
                                  L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the On-Demand-Location detected response for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address.
* @param    L7_int32 *signal @b{(output)} received signal strength in Dbm.
* @param    L7_uint8  *band @b{(output)} detected band.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedApOnDemandLocReportGet(L7_enetMacAddr_t macAddr,
                                         L7_int32 *signal,
                                         L7_uint8 *band);

/*********************************************************************
*
* @purpose  @purpose  Get the next valid Ap Mac for the given inputs.
*
* @param    L7_uint32 bldngNum          @b{(input)} building number.
* @param    L7_uint32 flrNum            @b{(input)} floor number.
* @param    L7_enetMacAddr_t apMac      @b{(input)} AP Mac.
* @param    L7_enetMacAddr_t *nextapMac @b{(output)} next ap Mac address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDevLocManagedApEntryNextGet (L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_enetMacAddr_t apMac,
                                        L7_enetMacAddr_t *nextapMac);

/*********************************************************************
*
* @purpose  Get the (lapsed)Time since device location triggered.
*
* @param    L7_uint32 *time. @b{(output)} pointer to time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOnDemandLocGlobalStatusTimeGet(L7_uint32 *time);


#endif /* INCLUDE_USMDB_WDM_DEV_LOC_H  */

