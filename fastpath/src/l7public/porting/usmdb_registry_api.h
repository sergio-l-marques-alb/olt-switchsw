/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\umdb\public\usmdb_registry_api.h
*
* @purpose externs for the registry section of the USMDB layer
*
* @component unitmgr
*
* @comments tba
*
* @create 11/30/2000
*
* @author tgaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_REGISTRY_API_H
#define USMDB_REGISTRY_API_H
 
/*********************************************************************
*
* @purpose Get the serial number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevInfoSerialNumGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the service Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevInfoServiceTagGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the Asset Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevInfoAssetTagGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the Asset Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevInfoAssetTagSet(L7_uint32 UnitIndex, L7_char8 *at);

/*********************************************************************
*
* @purpose Get the description of the machine.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSystemDescrGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the type of the machine.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMachineTypeGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the base card id
*          
* @param UnitIndex  the unit for this operation
* @param *cardID    ptr to card id  
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes This api is used primarily by the web code to uniquely 
*        identify all boxes so that the correct base java applet
*        image can be selected.  The card id's must be unique for
*        every box/manufacturer combination.
*        Box ID's are defined in the platform specific hpc_db.h 
*        and reproduced for all platforms in web.h.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbBaseCardIDGet(L7_uint32 UnitIndex, L7_uint32 *cardID);

/*********************************************************************
*
* @purpose Get the box front panel id
*          
* @param UnitIndex      the unit for this operation
* @param *frontPanelID  ptr to front panel id  
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes This api is used primarily by the web code to uniquely 
*        identify all boxes so that the correct base java applet
*        image can be selected.  The front panel id's must be unique for
*        every box/manufacturer combination.
*        Box ID's are defined in the platform specific hpc_db.h 
*        and reproduced in web.h for only those platforms that need another id to
*        further distinguish very similar boxes.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFrontPanelIDGet(L7_uint32 UnitIndex, L7_uint32 *frontPanelID);

/*********************************************************************
*
* @purpose Get the machine model
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMachineModelGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the FRU number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFRUNumGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the part number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbPartNumGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the maintenance level.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMaintLevelGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the manufacturer.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbManufacturerGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the software version.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwVersionGet(L7_uint32 UnitIndex, L7_char8 *buf);
/*********************************************************************
*
* @purpose Get the hardware version.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbHwVersionGet(L7_uint32 UnitIndex, L7_char8 *buf);
/*********************************************************************
*
* @purpose Get the operating system.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbOperSysGet(L7_uint32 UnitIndex, L7_char8 *buf); 

/*********************************************************************
*
* @purpose Get the NPD - Network Processing Device Version
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbNPDGet(L7_uint32 UnitIndex, L7_char8 *buf); 

/*********************************************************************
*
* @purpose Get information on which slots are populated.
*          
* @param UnitIndex  the unit for this operation
* @param *slotPop   array of length L7_MAX_SLOTS_PER_UNIT
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes The slotPop[] array will be filled in with L7_TRUE or L7_FALSE
* @notes slotPop[n] = L7_TRUE if slot n is populated with a feature card
* @notes slotPop[n] = L7_FALSE if slot n is not populated 
* @notes The slot numbers are 0-based, ranging from 0 to L7_MAX_SLOTS_PER_UNIT-1
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSlotsPopulatedGet(L7_uint32 UnitIndex, L7_uint32 *slotPop);

/*********************************************************************
*
* @purpose Get the number of ports in the specified slot
*          
* @param UnitIndex  the unit for this operation
* @param slot       slot number (0 to L7_MAX_SLOTS_PER_UNIT-1)
* @param *val       number of ports in the specified slot
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
* @returns L7_NOT_EXISTS, if the specified slot is not populated
*
* @notes The slotPop[] array will be filled in with L7_TRUE or L7_FALSE
* @notes slotPop[n] = L7_TRUE if slot n is populated with a feature card
* @notes slotPop[n] = L7_FALSE if slot n is not populated 
* @notes The slot numbers are 0-based, ranging from 0 to L7_MAX_SLOTS_PER_UNIT-1
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbPortsPerSlotGet(L7_uint32 UnitIndex, L7_uint32 slot, L7_uint32 *val);

#endif
