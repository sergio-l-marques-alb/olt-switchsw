/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wdm_dev_loc_api.h
*
* @purpose    Wireless Device Location API functions
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

#ifndef INCLUDE_WDM_DEV_LOC_API_H 
#define INCLUDE_WDM_DEV_LOC_API_H 

#include "datatypes.h"
#include "l7_packet.h"
#include "wireless_comm_structs.h"

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
L7_RC_t wdmDevLocBuildingEntryAdd(L7_uint32 bldngNum, L7_char8 *desc);

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
L7_RC_t wdmDevLocBuildingEntryDelete (L7_uint32 bldngNum);

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
L7_RC_t wdmDevLocBuildingEntryAllDelete(void);

/*********************************************************************
*
* @purpose  Check if an entry exists in Building Object table.
*
* @param    L7_uint32  @b{(input)} Building number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBuildingEntryGet(L7_uint32 bldngNum);

/*********************************************************************
*
* @purpose  Check if next building number  exists or not and get value if existing
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
L7_RC_t wdmDevLocBuildingEntryNextGet (L7_uint32 *bldngNum);

/*********************************************************************
*
* @purpose  Set building description.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_char8  *desc    @b{(input)} building Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBuildingDescSet (L7_uint32 bldngNum, L7_char8 *desc);
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
L7_RC_t wdmDevLocBuildingDescGet (L7_uint32 bldngNum, L7_char8 *desc);

/*********************************************************************
*
* @purpose  Add and Update the building floor data.
*
* @param    L7_uint32 bldngNum            @b{(input)} building number
* @param    L7_uint32 flrNum              @b{(input)} floor number
* @param    L7_char8  *flrDesc            @b{(output)} floor Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBldngFlrEntryAddUpdate (L7_uint32 bldngNum,
                                         L7_uint32 flrNum,
                                         L7_char8  *flrDesc);

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
L7_RC_t wdmDevLocBldngFlrEntryDelete(L7_uint32 bldngNum,
                                     L7_uint32 flrNum);

/*********************************************************************
*
* @purpose  Delete all Floor Entries of a given Building.
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBldngFlrEntryDeleteAll(L7_uint32 bldngNum);

/*********************************************************************
*
* @purpose  Check if an entry exists in Building Floor Object table.
*
* @param    L7_uint32  bldngNum @b{(input)} Building number
* @param    L7_uint32  flrNum   @b{(input)} Floor number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBldngFlrEntryGet(L7_uint32 bNum, L7_uint32 fNum);

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
L7_RC_t wdmDevLocBldngFlrEntryNextGet (L7_uint32 bldngNum,
                                       L7_uint32 flrNum,
                                       L7_uint32 *nextBldngNum,
                                       L7_uint32 *nextFlrNum);
/*********************************************************************
*
* @purpose  Set building description.
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_uint32 flrNum   @b{(input)} floor number
* @param    L7_char8  *flrDesc @b{(input)} building Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBldngFlrDescSet (L7_uint32 bldngNum,
                                  L7_uint32 flrNum,
                                  L7_char8 *flrDesc);
/*********************************************************************
*
* @purpose  Get building floor description.
*

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
L7_RC_t wdmDevLocBldngFlrDescGet (L7_uint32 bldngNum,
                                  L7_uint32 flrNum,
                                  L7_char8 *flrDesc);
/*********************************************************************
*
* @purpose  Add and Update the managed Ap Physical location data.
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
L7_RC_t wdmDevLocManagedApLocEntryAddUpdate (L7_uint32 bldngNum,
                                             L7_uint32 flrNum,
                                             L7_enetMacAddr_t apMac);

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
L7_RC_t wdmDevLocManagedApLocEntryDelete(L7_uint32 bldngNum,
                                         L7_uint32 flrNum,
                                         L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Delete all managed APs for given building floor
*
* @param    L7_uint32 bldngNum @b{(input)} building number
* @param    L7_uint32 flrNum   @b{(input)} floor number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocBldgFlrManagedAPEntryDeleteAll(L7_uint32 bldngNum,
                                                L7_uint32 flrNum);

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
L7_RC_t wdmDevLocManagedApLocEntryDeleteAll ();

/*********************************************************************
*
* @purpose  Check if  building number and floor number and AP Mac 
*            exists or not and get value if existing
*
* @param    L7_uint32        bldngNum  @b{(input)} Building number
* @param    L7_uint32        flrNum    @b{(input)} Floor number
* @param    L7_enetMacAddr_t apMac     @b{(input)} AP Mac Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocManagedApLocEntryGet(L7_uint32 bldngNum,
                                      L7_uint32 flrNum,
                                      L7_enetMacAddr_t apMac);
/*********************************************************************
*
* @purpose  Check if next building number and floor number and
*            Ap Mac exists or not and get value if existing
*
* @param    L7_uint32 bldngNum          @b{(input)} building number.
* @param    L7_uint32 flrNum            @b{(input)} floor number.
* @param    L7_enetMacAddr_t apMac      @b{(input)} ap Mac address.
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
L7_RC_t wdmDevLocManagedApLocEntryNextGet (L7_uint32 bldngNum,
                                           L7_uint32 flrNum,
                                           L7_enetMacAddr_t apMac,
                                           L7_uint32 *nextBldngNum,
                                           L7_uint32 *nextFlrNum,
                                           L7_enetMacAddr_t *nextapMac);
/*********************************************************************
*
* @purpose   Set the XY-co-ordinate for Ap Physical Location
*
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
* @param    L7_WDM_MEASUREMENT_SYS_NAME_t measSys @b{(input)} Measurement System
* @param    devLocCoord_t coord   @b{(input)} XY coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocApLocXYCoordSet(L7_uint32 bldngNum,
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
L7_RC_t wdmDevLocApLocXYCoordValidate(L7_WDM_MEASUREMENT_SYS_NAME_t measSys,
                                      L7_int32 x_coord, L7_int32 y_coord);

/*********************************************************************
*
* @purpose   Set the XY-co-ordinate for Managed Ap Physical Location
*
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
* @param    devLocCoord_t coord   @b{(input)} XY coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocManagedApLocXYCoordSet(L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_enetMacAddr_t apMac,
                                        devLocCoord_t coord);
/*********************************************************************
*
* @purpose   Get the XY-co-ordinate for Managed Ap Physical Location
*
*
* @param    L7_uint32         bldngNum @b{(input)} Building number
* @param    L7_uint32         flrNum   @b{(input)} Floor number
* @param    L7_enetMacAddr_t  apMac    @b{(input)} AP Mac Address
* @param    devLocCoord_t *coord  @b{(output)} XY coordinate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDevLocManagedApLocXYCoordGet (L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_enetMacAddr_t apMac,
                                        devLocCoord_t *coord);

/*********************************************************************
*
* @purpose  Check if next Ap Mac exists for given building and floor.
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
L7_RC_t wdmDevLocManagedApEntryNextGet (L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_enetMacAddr_t apMac,
                                        L7_enetMacAddr_t *nextapMac);

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
L7_RC_t wdmDevLocBldngFlrApDelete (L7_uint32 bldngNum,
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
L7_RC_t wdmDevLocBldngApDelete (L7_uint32 bldngNum);

/*********************************************************************
*
* @purpose  Get the Last msg Transmitted time.
*
* @param    L7_uint32 *time @b{(output)} pointer to the time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocStatusLastMsgTxTimeGet(L7_uint32 *time);

/*********************************************************************
*
* @purpose  Set the Last msg Transmitted time.
*
* @param    L7_uint32 *time @b{(input)} time value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocStatusLastMsgTxTimeSet(L7_uint32 time);

/*********************************************************************
*
* @purpose Get the mac address of search target device.
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
L7_RC_t wdmOnDemandLocTrigStatusMacGet(L7_enetMacAddr_t *targetMac);

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
L7_RC_t wdmOnDemandLocTrigStatusDevTypeGet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t *type);

/*********************************************************************
*
* @purpose  Get the Building number to be searched.
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
L7_RC_t wdmOnDemandLocTrigStatusBuildingNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the Floor number to be searched.
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
L7_RC_t wdmOnDemandLocTrigStatusFlrNumGet(L7_uint8 *num);

/*********************************************************************
*
* @purpose  Get the next Building and Floor numbers to be searched.
*
* @param    L7_uint8 bNum @b{(input)} building number .
* @param    L7_uint8 fNum @b{(input)} floor number .
* @param    L7_uint8 *nextBNum @b{(output)}  pointer to the building number.
* @param    L7_uint8 *nextFNnum @b{(output)} pointer to the floor number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusBldgFlrGetNext(L7_uint8 bNum, L7_uint8 fNum, 
                    L7_uint8 *nextBNum, L7_uint8 *nextFNum);

/*********************************************************************
*
* @purpose  Get the radio type (Sentry/Operational) to be used for 
*           search operation.
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
L7_RC_t wdmOnDemandLocTrigStatusManagedApRadioTypeGet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t *type);

/****************************************************************************
*
* @purpose  Get the On-demand-Location triggering status.
*
* @param    L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status @b{(output)} 
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
L7_RC_t wdmOnDemandLocTrigStatusGet(L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the number of Locator APs available for search operation.
*
* @param    L7_uint32 *num @b{(output)} pointer to the number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Number of locator APs depend upon the building/floor/sentry
*           scope specified by the adminitrator.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusLocatorApsNumGet(L7_uint32 *num);

/*********************************************************************
*
* @purpose Set the mac address of search target device.
*
* @param   L7_enetMacAddr_t targetMac @b{(input)} target MAC Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusMacSet(L7_enetMacAddr_t targetMac);

/*********************************************************************
*
* @purpose  Set the type of the device (AP/Client) to be searched.
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
L7_RC_t wdmOnDemandLocTrigStatusDevTypeSet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t type);

/*********************************************************************
*
* @purpose  Set the Building number to be searched in for target device.
*
* @param    L7_uint8 num @b{(input)} building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusBuildingNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Get the next Building and Floor numbers to be searched.
*
* @param    L7_uint8 bNum @b{(input)} building number .
* @param    L7_uint8 fNum @b{(input)} floor number .
* @param    L7_uint8 *nextBNum @b{(output)}  pointer to the building number.
* @param    L7_uint8 *nextFNnum @b{(output)} pointer to the floor number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusBldgFlrGetNext(L7_uint8 bNum, L7_uint8 fNum, 
                    L7_uint8 *nextBNum, L7_uint8 *nextFNum);


/*********************************************************************
*
* @purpose  Set the Floor number to be searched on for target device.
*
* @param    L7_uint8 num @b{(input)} floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusFlrNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the type of the radio (Sentry/Operational) to be used for 
*           search operation.
*
* @param    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type @b{(input)} radio type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusManagedApRadioTypeSet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type);

/*********************************************************************
*
* @purpose  Set the On-demand-Location triggering status.
*
* @param    L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t status @b{(input)} status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusSet(L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t status);

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
L7_RC_t wdmOnDemandLocTrigStatusLocatorApsNumSet(L7_uint32 num);

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
L7_RC_t wdmOnDemandLocTrigStatusLocatorApsFind(L7_uint32 *apNum);

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
L7_RC_t wdmOnDemandLocProcedureInitiate(void);

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
L7_RC_t wdmOnDemandLocProcedureStatusGet(L7_uint8 *cmd);

/*********************************************************************
*
* @purpose  Get the search operation comamnd value.
*
* @param    L7_uint32 *cmd @b{(output)} triggering command.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears command value it after reading.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocProcedureStatusGetClear(L7_uint8 *cmd);

/*********************************************************************
*
* @purpose  Flush the status parameters for the specified floor entry.
*
* @param    L7_uint8 bNum @b{(input)} building number .
* @param    L7_uint8 fNum @b{(input)} floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusParamsReset(L7_uint8 bNum, L7_uint8 fNum);

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
L7_RC_t wdmOnDemandLocFlrStatusGet( L7_uint8 bNum, L7_uint8 fNum,
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
L7_RC_t wdmOnDemandLocFlrStatusSolTypeGet( L7_uint8 bNum, L7_uint8 fNum,
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
L7_RC_t wdmOnDemandLocFlrStatusCoordsGet(L7_uint8 bNum, L7_uint8 fNum, 
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
L7_RC_t wdmOnDemandLocFlrStatusRadiusGet(L7_uint8 bNum, L7_uint8 fNum, 
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
L7_RC_t wdmOnDemandLocFlrStatusSigmaGet(L7_uint8 bNum, L7_uint8 fNum, 
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
L7_RC_t wdmOnDemandLocFlrStatusLocatorApsGet(L7_uint8 bNum, L7_uint8 fNum, 
                                               L7_uint32 *num);

/*********************************************************************
*
* @purpose  Set the number of Aps that detected the device.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 *value @b{(input)} number of Aps.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusLocatorApsSet(L7_uint8 bNum, L7_uint8 fNum, 
                                               L7_uint32 num);

/*********************************************************************
*
* @purpose  Set the search status of the target.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_WDM_ONDEMAND_LOC_FLR_STATUS_t status@b{(input)} 
*                   status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusSet(L7_uint8 bNum, L7_uint8 fNum, 
                               L7_WDM_ONDEMAND_LOC_FLR_STATUS_t status);

/*********************************************************************
*
* @purpose  Set the search solutino type.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_t type@b{(input)} 
*                   status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusSolTypeSet( L7_uint8 bNum, L7_uint8 fNum,
                    L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_t type);

/*********************************************************************
*
* @purpose  Set the target coordinates.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    devLocCoord_t value @b{(input)} coordinates value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusCoordsSet(L7_uint8 bNum, L7_uint8 fNum, 
                                           devLocCoord_t value);

/*********************************************************************
*
* @purpose  Set the radius.
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 value @b{(input)} radius value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusRadiusSet(L7_uint8 bNum, L7_uint8 fNum, 
                                           L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the standard deviation(sigma).
*
* @param    L7_uint8  bNum @b{(input)} building number
* @param    L7_uint8  fNum @b{(input)} floor number
* @param    L7_uint32 value @b{(input)} sigma.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocFlrStatusSigmaSet(L7_uint8 bNum, L7_uint8 fNum, 
                                          L7_int32 value);

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
L7_RC_t wdmOnDemandLocGlobalStatusMacGet(L7_enetMacAddr_t *targetMac);

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
L7_RC_t wdmOnDemandLocGlobalStatusBuildingNumGet(L7_uint8 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusFlrNumGet(L7_uint8 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusDevTypeGet(
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
L7_RC_t wdmOnDemandLocGlobalStatusManagedApRadioTypeGet(
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
L7_RC_t wdmOnDemandLocGlobalStatusGet(L7_WDM_ONDEMAND_LOC_STATUS_t *status);

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
L7_RC_t wdmOnDemandLocGlobalStatusLocatorApsGet(L7_uint32 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusDetectingApsGet(L7_uint32 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusDetectBuildingsGet(L7_uint32 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusDetectFlrsGet(L7_uint32 *num);


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
L7_RC_t wdmOnDemandLocGlobalStatusHighestDetectBuildingGet(L7_uint8 *num);

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
L7_RC_t wdmOnDemandLocGlobalStatusHighestDetectFlrGet(L7_uint8 *num);


/*********************************************************************
*
* @purpose Get the mac address of search target.
*
* @param   L7_enetMacAddr_t targetMac @b{(input)} MAC Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusMacSet(L7_enetMacAddr_t targetMac);

/*********************************************************************
*
* @purpose  Set the Building number, where target device searched.
*
* @param    L7_uint8 num @b{(input)}  building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusBuildingNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the floor number, where target device searched.
*
* @param    L7_uint8 num @b{(input)}  floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusFlrNumSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the search devicde (AP/Client) type.
*
* @param    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t type @b{(input)} 
*                                            device type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusDevTypeSet(
                    L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t type);

/*********************************************************************
*
* @purpose  Set the radio type (Sentry/Operational) used for 
*           device search.
*
* @param    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type @b{(input)} 
*                                            radio type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusManagedApRadioTypeSet(
                    L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t type);

/****************************************************************************
*
* @purpose  Set the On-demand-Location global status.
*
* @param    L7_WDM_ONDEMAND_LOC_STATUS_t status @b{(input)} 
*                    status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*****************************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusSet(L7_WDM_ONDEMAND_LOC_STATUS_t status);

/*********************************************************************
*
* @purpose  Set the number of Locator APs used for device search.
*
* @param    L7_uint32 num @b{(input)}  number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusLocatorApsSet(L7_uint32 num);

/*********************************************************************
*
* @purpose  Set the number of APs that detected the search target.
*
* @param    L7_uint32 num @b{(input)}  number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusDetectingApsSet(L7_uint32 num);

/*********************************************************************
*
* @purpose  Set the Number of Buildings where the managed APs detected
*           the signal.
*
* @param    L7_uint32 num @b{(input)}  building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusDetectBuildingsSet(L7_uint32 num);

/*********************************************************************
*
* @purpose  Set the Number of Floors where the managed APs detected
*           the signal.
*
* @param    L7_uint32 num @b{(input)}  floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusDetectFlrsSet(L7_uint32 num);

/*********************************************************************
*
* @purpose  Set the building num. with detected the signal.
*
* @param    L7_uint8 num @b{(input)}  building number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusHighestDetectBuildingSet(L7_uint8 num);

/*********************************************************************
*
* @purpose  Set the floor num. with detected the signal.
*
* @param    L7_uint8 num @b{(input)}  floor number .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusHighestDetectFlrSet(L7_uint8 num);


/*********************************************************************
*
* @purpose  Get the next Building and Floor number to be searched.
*
* @param    L7_uint8 bNum @b{(input)} building number .
* @param    L7_uint8 fNum @b{(input)} floor number .
* @param    L7_uint8 *nextBNum @b{(output)}  pointer to the building number.
* @param    L7_uint8 *nextFNnum @b{(output)} pointer to the floor number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusBldgFlrGetNext(L7_uint8 bNum, L7_uint8 fNum, 
                    L7_uint8 *nextBNum, L7_uint8 *nextFNum);

/*********************************************************************
*
* @purpose  Get the mac address of next AP on the specified 
*           building and floor.
*
* @param  L7_uint8  bNum @b{(input)} building number.
* @param  L7_uint8  fNum @b{(input)} floor number.
* @param  L7_enetMacAddr_t macAddr @b{(input)} mac address of locator AP.
* @param  L7_enetMacAddr_t *nextMac @b{(output)} mac address of next locator AP.
* @param  L7_uint8  *radiosNum @b{(output)} radios to be used in device search.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next valid AP entry is subject to the 
*           building/floor/sentry/radio type
*           criterion specified in triangulation table. 
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusLocatorApGetNext(L7_uint8 bNum, 
                                                  L7_uint8 fNum, 
                                                  L7_enetMacAddr_t macAddr, 
                                                  L7_enetMacAddr_t *nextMac);

/*********************************************************************
*
* @purpose  Get the mac address of next AP on the specified 
*           building and floor.
*
* @param  L7_uint8  bNum @b{(input)} building number.
* @param  L7_uint8  fNum @b{(input)} floor number.
* @param  L7_enetMacAddr_t macAddr @b{(input)} mac address of locator AP.
* @param  L7_enetMacAddr_t *nextMac @b{(output)} mac address of next locator AP.
* @param  L7_uint8  *radiosNum @b{(output)} radios to be used in device search.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next valid AP entry is subject to the 
*           building/floor/sentry/radio type
*           criterion specified in triangulation table. 
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusLocatorApGetNext(L7_uint8 bNum, 
                                                  L7_uint8 fNum, 
                                                  L7_enetMacAddr_t macAddr, 
                                                  L7_enetMacAddr_t *nextMac,
                                                  L7_uint8 *radiosNum);

/*********************************************************************
*
* @purpose  Initialize all the on demand location status parameters.
*
* @param    void.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocStatusParamsInit(void);

/*********************************************************************
*
* @purpose  Flush on demand location trig. status parameters.
*
* @param    void.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusReset(void);

/*********************************************************************
*
* @purpose  Update the global status parameters from trig. status.
*
* @param    L7_uint8 status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusTrigInfoCopy(void);

/*********************************************************************
*
* @purpose  Update the global status with device location algorithm results.
*
* @param    L7_uint8 status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocGlobalStatusUpdate(L7_uint8 status);

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
L7_RC_t wdmOnDemandLocTrigStatusLocatorApsFindOnly(L7_enetMacAddr_t targetMac,
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
L7_RC_t wdmOnDemandLocTrigStatusLocatorApsFindUpdate(L7_uint32 *apNum);

/*********************************************************************
*
* @purpose  Get the next Building and Floor numbers to be searched.
*
* @param    L7_uint8 bNum @b{(input)} building number .
* @param    L7_uint8 fNum @b{(input)} floor number .
* @param    L7_uint8 *nextBNum @b{(output)}  pointer to the building number.
* @param    L7_uint8 *nextFNnum @b{(output)} pointer to the floor number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmOnDemandLocTrigStatusBldgFlrGetNext1(L7_uint8 bNum, L7_uint8 fNum, 
                    L7_uint8 *nextBNum, L7_uint8 *nextFNum);

/*********************************************************************
*
* @purpose  Add or Update Triangulation status table entry.
*
* @param    L7_enetMacAddr_t           macAddr    @b{(input)} Target mac address
* @param    L7_uint32                  dataType   @b{(input)} location type
* @param    L7_uint32                  status     @b{(input)} location status
* @param    L7_uint32                  deviceType @b{(input)} device type
* @param    L7_uint32                  bldngNum   @b{(input)} building number
* @param    L7_uint32                  flrNum     @b{(input)} floor number
* @param    L7_int32                   coordX     @b{(input)} x-coordinate
* @param    L7_int32                   coordY     @b{(input)} y-coordinate
* @param    L7_uint32                  timestamp  @b{(input)} timestamp
* @param    L7_BOOL                    searchFlag @b{(input)} Flag indicates location
*                                      searched form triangulation tables.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmTriangulationLocStatusEntryAddUpdate (L7_enetMacAddr_t macAddr,
                                                 L7_uint32 dataType,
                                                 L7_uint32 status,
                                                 L7_uint32 deviceType,
                                                 L7_uint32 bldngNum,
                                                 L7_uint32 flrNum,
                                                 L7_int32  coordX,
                                                 L7_int32  coordY,
                                                 L7_uint32 timestamp);

/*********************************************************************
*
* @purpose  Update Triangulation status table entry.
*
* @param    L7_enetMacAddr_t           macAddr    @b{(input)} Target mac address
* @param    L7_uint32                  dataType   @b{(input)} location type
* @param    L7_uint32                  status     @b{(input)} location status
* @param    L7_uint32                  bldngNum   @b{(input)} building number
* @param    L7_uint32                  flrNum     @b{(input)} floor number
* @param    L7_int32                   coordX     @b{(input)} x-coordinate
* @param    L7_int32                   coordY     @b{(input)} y-coordinate
* @param    L7_uint32                  timestamp  @b{(input)} timestamp
* @param    L7_BOOL                    searchFlag @b{(input)} Flag indicates location
*                                      searched form triangulation tables.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function updates an existing entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmTriangulationLocEntryUpdate (L7_enetMacAddr_t macAddr,
                                        L7_uint32 dataType,
                                        L7_uint32 status,
                                        L7_uint32 bldngNum,
                                        L7_uint32 flrNum,
                                        L7_int32  coordX,
                                        L7_int32  coordY,
                                        L7_uint32 timestamp);

/*********************************************************************
*
* @purpose  Delete an Triangulation table entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} Target mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTriangulationLocStatusEntryDelete(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmTriangulationLocStatusEntryGet(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmTriangulationLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
                                              L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the type of data(No loaction/Location).
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} target mac address
* @param    L7_uint32                  type    @b{(output)} data type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmTriangulationLocStatusTypeGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusBldngGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusFlrGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusXYCoordGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusAgeGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationLocStatusDeviceTypeGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationAPLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmTriangulationClientLocStatusEntryNextGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmDevLocFloorApCountGet (L7_uint32 bldngNum, L7_uint32 *flrCount,
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
L7_RC_t wdmDevLocApCountGet (L7_uint32 bldngNum,
                             L7_uint32 flrNum,
                             L7_uint32 *count);

/*********************************************************************
*
* @purpose  * @purpose  Get the next valid Ap Mac for the given inputs.
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
L7_RC_t wdmDevLocManagedApLocEntryDetailedGet (L7_enetMacAddr_t apMac, 
                                        L7_uint8 *bNum,
                                        L7_uint8 *fNum,
                                        devLocCoord_t *coord);

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
L7_RC_t wdmOnDemandLocGlobalStatusTimeGet(L7_uint32 *time);

#endif /* INCLUDE_WDM_DEV_LOC_API_H */
