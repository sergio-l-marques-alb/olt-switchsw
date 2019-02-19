/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_cardmgr_api.h
*
* @purpose Usmdb externs for Card Manager
*
* @component Card Manager
*
* @comments none
*
* @create 01/9/2003
*
* @author kmcdowell
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef USMDB_CARD_MANAGER_API_H
#define USMDB_CARD_MANAGER_API_H

#include "l7_common.h"
#include "sysapi_hpc.h"

/*********************************************************************
*
* @purpose Configure a card in a slot
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot
* @param  L7_uint32   cardType    Configured Card Type
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This configures a card with the given card type in a slot.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotCardConfigSet(L7_uint32 unit, 
                              L7_uint32 slot, 
                              L7_uint32 cardType);
                                                        
/*********************************************************************
*
* @purpose Clear configured card information from a slot
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit and slot does not exist
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This clears the configured card information from a slot.  
*        If a card is physically located in the slot, it will use
*        the default card values.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotCardConfigClear(L7_uint32 unit, 
                                L7_uint32 slot);


/*********************************************************************
*
* @purpose Get inserted card type
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32   *cardType  Inserted Card Type
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist or there is no
*                          card currently plugged into that slot
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the card type of the card that is physically
*        plugged in to this slot.  If no card is plugged in this slot,
*         L7_NOT_EXIST will be returned.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardInsertedCardTypeGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 *cardType);

/*****************************************************************
******************************************************************
**
**  The following functions access the Supported-Cards Database
**	The Supported-Cards table contains physical and logical cards.
**
******************************************************************
*****************************************************************/

/*********************************************************************
*
* @purpose Get the next index in the supported-cards table.
*
* @param  L7_uint32   cardTypeIdIndex   Pointer to current index
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedIndexNextGet(L7_uint32 *cardTypeIdIndex);

/*********************************************************************
*
* @purpose Get the next index of a physical card in 
*		   the supported-cards table.
*
* @param  L7_uint32   cardTypeIdIndex   Pointer to current index
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedPhysicalIndexNextGet(L7_uint32 *cardTypeIdIndex);

/*********************************************************************
*
* @purpose Get model identifier for specified card index.
*
* @param  L7_uint32    cardTypeIdIndex  Card Index
* @param  L7_uchar8    modelId    Pointer to Card Model ID
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedCardModelIdGet(L7_uint32 cardTypeIdIndex, 
                                         L7_uchar8 *modelID);

/*********************************************************************
*
* @purpose Get model identifier for the next card index.
*
* @param  L7_uint32    cardTypeIdIndex  Card Index
* @param  L7_uchar8    modelId    Pointer to Card Model ID
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes MIB walks should only show physical cards.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedPhysModelIdNextGet(L7_uint32 *cardTypeIdIndex, 
                                         L7_uchar8 *modelID);

/*********************************************************************
*
* @purpose Get card description
*
* @param  L7_uint32    cardTypeIdIndex   Card database index
* @param  L7_uchar8    cardDescription   Pointer to Card Description
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedCardDescriptionGet(L7_uint32 cardTypeIdIndex, 
                                             L7_uchar8 *cardDescription);

/*********************************************************************
*
* @purpose Get next card description
*
* @param  L7_uint32    cardTypeIdIndex   Card database index
* @param  L7_uchar8    cardDescription   Pointer to Card Description
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes MIB walks should only show physical cards.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedPhysDescriptionNextGet(L7_uint32 *cardTypeIdIndex, 
                                             L7_uchar8 *cardDescription);

/*********************************************************************
*
* @purpose Get card type
*
* @param  cardTypeIdIndex   Card database index
* @param  cardTypeId   		Pointer to Card Type Identifier
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedTypeIdGet(L7_uint32 cardTypeIdIndex, 
                                             L7_uint32 *cardTypeId);

/*********************************************************************
*
* @purpose Get next card type
*
* @param  cardTypeIdIndex   Card database index
* @param  cardTypeId   		Pointer to Card Type Identifier
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card type
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes MIB walks should only show physical cards.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardSupportedPhysTypeIdNextGet(L7_uint32 *cardTypeIdIndex, 
                                             L7_uint32 *cardTypeId);

/**************************************************************************
*
* @purpose  Check validity of cardTypeIdIndex value
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t usmDbCardSupportedIndexCheck(L7_uint32 cardTypeIdIndex);

/*****************************************************************
******************************************************************
**
**  The following functions access the Supported-Slots table
**	The Supported-Slot table contains information about physical
**  slots in the specified unit.
**
******************************************************************
*****************************************************************/

/*********************************************************************
*
* @purpose Get the slot number of the specified slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   slotNumber 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedSlotNumberGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *slotNumber);

/*********************************************************************
*
* @purpose Get the slot number of the next slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   slotNumber 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedSlotNumberNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *slotNumber);

/*********************************************************************
*
* @purpose Get the pluggable indicator of the specified slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   pluggable 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedPluggableGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *pluggable);

/*********************************************************************
*
* @purpose Get the pluggable indicator of the next slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   pluggable 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedPluggableNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *pluggable);

/*********************************************************************
*
* @purpose Get the powerdown indicator of the specified slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   powerdown 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedPowerdownGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *powerdown);

/*********************************************************************
*
* @purpose Get the powerdown indicator of the next slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   powerdown 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotSupportedPowerdownNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *powerdown);


/*****************************************************************
******************************************************************
**
**  The following functions access the Supported-Cards-In-Slot table
**	The Supported-Cards-In-Slot-Slot table contains information 
**  about which cards are supported in which slots.
**
******************************************************************
*****************************************************************/

/*********************************************************************
*
* @purpose Indicate whether specified card is supported in the
*		   specified unit and slot.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   cardIndex 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardInSlotSupportedGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 cardIndex);

/*********************************************************************
*
* @purpose Get the next supported unit/slot/card index.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
* @param  L7_uint32   cardIndex 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if no card supported
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardInSlotSupportedNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *cardIndex);


/*********************************************************************
*
* @purpose Get inserted card status
*
* @param  L7_uint32     unit       Unit ID of the unit
* @param  L7_uint32     slot       Slot ID of the slot port
* @param  L7_uint32    *cardStatus  Inserted Card Status
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist, or no card is
*                          currently plubbed into that slot
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the card status of the card that is physically
*        plugged in to this slot.  If no card is plugged in this slot,
*         L7_NOT_EXIST will be returned.
*
* @end
*
*********************************************************************/

L7_RC_t usmDbCardStatusGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 *cardStatus);

/*********************************************************************
*
* @purpose Get configured card type
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32    *cardType  Configured Card Type
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist or no card is
*                          configured in that slot
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the card type of the card that is configured
*         in to this slot.  If no card is configured in this slot,
*         L7_NOT_EXIST will be returned.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCardConfiguredCardTypeGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 *cardType);

/*********************************************************************
*
* @purpose Get slot administrative mode
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32   *adminMode  Configured Administrative mode
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
*                          configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the administrative mode of the slot.  If this
*        slot is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotAdminModeGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 *adminMode);

/*********************************************************************
*
* @purpose Set slot administrative mode
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32    adminMode  Configured Administrative mode
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This sets the administrative mode of the slot.    
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotAdminModeSet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 adminMode);

/*********************************************************************
*
* @purpose Get slot power mode
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32   *powerMode  Configured Power mode
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
*                          configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the power mode of the slot.  If this
*        slot is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotPowerModeGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 *powerMode);

/*********************************************************************
*
* @purpose Set slot power mode
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_uint32    powerMode  Power mode to set
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
*                          configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This configures the power mode of the slot.  If this
*        slot is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotPowerModeSet(L7_uint32 unit, L7_uint32 slot, 
                                L7_uint32 powerMode);

/*********************************************************************
*
* @purpose Retrieve slot full/empty status
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot port
* @param  L7_BOOL     *isFull     True if slot is full; false otherwise
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
*                          configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns true if the slot is full.  Returns false if the
*        slot is empty.  If this slot is not configured, L7_NOT_EXIST 
*        will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotIsFullGet(L7_uint32 unit, L7_uint32 slot, 
                                L7_BOOL *isFull);

/*********************************************************************
*
* @purpose Retrieve first slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    *slot      Slot ID of the first slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the first slot in this unit.  If this unit is 
*        not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotFirstGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
*
* @purpose Retrieve the next slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot 
* @param  L7_uint32    *nextSlot      Slot ID of the next slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the slot following the input slot in this unit.  
*        If this unit is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotNextGet(L7_uint32 unit, L7_uint32 slot, 
                        L7_uint32 *nextSlot);

/*********************************************************************
*
* @purpose Retrieve the slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot      Slot ID of the  slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit and slot does not exist,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns L7_SUCCESS if the slot exists in this unit.  
*        If this unit/slot combination is not configured, 
*        L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotGet(L7_uint32 unit, L7_uint32 slotId);


/*********************************************************************
*
* @purpose Retrieve first configured slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    *slot      Slot ID of the first slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the first configured slot in this unit.  If this unit is 
*        not configured, L7_NOT_EXIST will be returned.  
*                                            
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotFirstConfigGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
*
* @purpose Retrieve the next configured slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot 
* @param  L7_uint32    *nextSlot      Slot ID of the next slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns configured the slot following the input slot 
*        in this unit.  
*        If this unit is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotConfigNextGet(L7_uint32 unit, L7_uint32 slot, 
                        L7_uint32 *nextSlot);

/*********************************************************************
*
* @purpose Retrieve the configured slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot      Slot ID of the  slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit and slot does not exist,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns L7_SUCCESS if the slot exists in this unit.  
*        If this unit/slot combination is not configured, 
*        L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotConfigGet(L7_uint32 unit, L7_uint32 slotId);


/*********************************************************************
*
* @purpose Retrieve first physically inserted slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    *slot      Slot ID of the first slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the first slot in this unit.  If this unit is 
*        not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotFirstPhysicalGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
*
* @purpose Retrieve the next physical slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot       Slot ID of the slot 
* @param  L7_uint32    *nextSlot      Slot ID of the next slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns the slot following the input slot in this unit.  
*        If this unit is not configured, L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotPhysicalNextGet(L7_uint32 unit, L7_uint32 slot, 
                        L7_uint32 *nextSlot);

/*********************************************************************
*
* @purpose Retrieve the physical slot in the unit
*
* @param  L7_uint32    unit       Unit ID of the unit
* @param  L7_uint32    slot      Slot ID of the  slot 
*
* @returns L7_SUCCESS,     if success
* @returns L7_NOT_EXIST,   if that unit and slot does not exist,
*                          or is not configured 
* @returns L7_ERROR,       if parameters are invalid or other error
*
* @notes This returns L7_SUCCESS if the slot exists in this unit.  
*        If this unit/slot combination is not configured, 
*        L7_NOT_EXIST will be returned.  
*
* @end
*
*********************************************************************/
L7_RC_t usmdbSlotPhysicalGet(L7_uint32 unit, L7_uint32 slotId);


/*****************************************************************
******************************************************************
**
**  Following are utility function APIs.
**
******************************************************************
*****************************************************************/

/*********************************************************************
* @purpose  Helper routine to get card index from card id
*
* @param  card_id  @b{(input)) the card id
* @param  cx       @b{(output)) the card index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbCardIndexFromIDGet(L7_uint32 card_id, L7_uint32 *cx);

/*********************************************************************
* @purpose  Helper routine to get slot number from unit-slot index
*
* @param  ux        @b{(input)) the unit index
* @param  sx        @b{(input)) the slot index
* @param  slot_num  @b{(output)) the slot number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotNumFromIndexGet(L7_uint32 ux, L7_uint32 sx, L7_uint32 *slot_number);

/*********************************************************************
* @purpose  Helper routine to get unit index from unit id
*
* @param  unit_id  @b{(input)) the unit id
* @param  cx       @b{(output)) the unit index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitIndexFromIDGet(L7_uint32 unit_id, L7_uint32 *ux);

/*********************************************************************
* @purpose  Helper routine to get slot index from slot number 
*           for a given unit index.
*
* @param  ux    @b{(input)) the unit index
* @param  slot  @b{(input)) the slot number
* @param  sx    @b{(output)) the slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotIndexFromNumGet(L7_uint32 ux, L7_uint32 slot, L7_uint32 *sx);

#endif /* USMDB_CARD_MANAGER_API_H */
