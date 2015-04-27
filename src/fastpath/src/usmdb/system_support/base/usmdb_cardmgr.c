/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_cardmgr.c
*
* @purpose Provides the USMDB interface for Card Manager
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

#include "l7_common.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_unitmgr_api.h"

#include "cardmgr_api.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_slotmapper.h"

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
                              L7_uint32 cardType)
{
    return cmgrSlotCardConfigSet(unit, slot, cardType);
}
                                                        
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
                                L7_uint32 slot)
{
    return cmgrSlotCardConfigClear(unit, slot);
}


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
                                L7_uint32 *cardType)
{
    return cmgrCardInsertedCardTypeGet(unit, slot, cardType);
}

/*****************************************************************
******************************************************************
**
**  The following functions access the Supported-Cards table
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
L7_RC_t usmDbCardSupportedIndexNextGet(L7_uint32 *cardTypeIdIndex)
{
	return sysapiHpcCardDbEntryIndexNextGet (cardTypeIdIndex);
}

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
L7_RC_t usmDbCardSupportedPhysicalIndexNextGet(L7_uint32 *cardTypeIdIndex)
{
	L7_RC_t rc;
	SYSAPI_HPC_CARD_DESCRIPTOR_t * supported_card;

	rc = sysapiHpcCardDbEntryIndexNextGet (cardTypeIdIndex);
    if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	do {
		supported_card = sysapiHpcCardDbEntryByIndexGet (*cardTypeIdIndex);

		if ((supported_card != L7_NULLPTR) && (supported_card->type == SYSAPI_CARD_TYPE_LINE))
		{
			return L7_SUCCESS;
		}

		rc = sysapiHpcCardDbEntryIndexNextGet (cardTypeIdIndex);

	} while (rc == L7_SUCCESS);


	return rc;
}


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
                                         L7_uchar8 *modelID)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   strcpy (modelID, pCardDescriptor->cardModel);
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }

   return(rc);
}

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
                                         L7_uchar8 *modelID)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

     rc = usmDbCardSupportedPhysicalIndexNextGet(cardTypeIdIndex);
	 if (rc != L7_SUCCESS) 
	 {
		 return rc;
	 }

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(*cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   strcpy (modelID, pCardDescriptor->cardModel);
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }

   return(rc);
}

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
                                             L7_uchar8 *cardDescription)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   strcpy (cardDescription, pCardDescriptor->cardDescription);
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }
   return(rc);

}

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
                                             L7_uchar8 *cardDescription)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

     rc = usmDbCardSupportedPhysicalIndexNextGet(cardTypeIdIndex);
     if (rc != L7_SUCCESS) 
     {
	     return rc;
     }

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(*cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   strcpy (cardDescription, pCardDescriptor->cardDescription);
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }
   return(rc);

}

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
                                             L7_uint32 *cardTypeId)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   *cardTypeId = pCardDescriptor->cardTypeId;
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }
   return(rc);

}

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
                                             L7_uint32 *cardTypeId)
{
   L7_RC_t rc;
   SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

     rc = usmDbCardSupportedPhysicalIndexNextGet(cardTypeIdIndex);
     if (rc != L7_SUCCESS) 
     {
	     return rc;
     }

	 if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(*cardTypeIdIndex)) != L7_NULLPTR)
	 {
	   *cardTypeId = pCardDescriptor->cardTypeId;
	   rc = L7_SUCCESS;
	 }
	 else
	 {
	   rc = L7_NOT_EXIST;
	 }
   return(rc);

}


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
L7_RC_t usmDbCardSupportedIndexCheck(L7_uint32 cardTypeIdIndex)
{
  return (sysapiHpcCardDbEntryIndexCheck(cardTypeIdIndex));

}

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
* @purpose Get the next index in the supported-cards table.
*
* @param  L7_uint32   unitIndex 
* @param  L7_uint32   slotIndex 
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
L7_RC_t usmDbSlotSupportedIndexNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex)
{
	return sysapiHpcSlotDbEntryIndexNextGet (unitIndex, slotIndex);
}

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
L7_RC_t usmDbSlotSupportedSlotNumberGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *slotNumber)
{
	return sysapiHpcSlotDbEntrySlotNumberGet (unitIndex, slotIndex, slotNumber);
}

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
L7_RC_t usmDbSlotSupportedSlotNumberNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *slotNumber)
{
	L7_RC_t rc;

	rc = sysapiHpcSlotDbEntryIndexNextGet (unitIndex, slotIndex);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	return sysapiHpcSlotDbEntrySlotNumberGet (*unitIndex, *slotIndex, slotNumber);
}

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
L7_RC_t usmDbSlotSupportedPluggableGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *pluggable)
{
#ifndef L7_CHASSIS
	return sysapiHpcSlotDbEntryPluggableGet (unitIndex, slotIndex, pluggable);
#else
  /* slotIndex here is nothing but physical slot number and unitIndex is 
   * nothing but unit number.
   */
	return sysapiHpcSlotMapSlotPluggableGet (slotIndex, pluggable);
#endif
}

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
L7_RC_t usmDbSlotSupportedPluggableNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *pluggable)
{
  	L7_RC_t rc;

	rc = sysapiHpcSlotDbEntryIndexNextGet (unitIndex, slotIndex);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	return sysapiHpcSlotDbEntryPluggableGet (*unitIndex, *slotIndex, pluggable);
}

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
L7_RC_t usmDbSlotSupportedPowerdownGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *powerdown)
{
#ifndef L7_CHASSIS
	return sysapiHpcSlotDbEntryPowerdownGet (unitIndex, slotIndex, powerdown);
#else
  /* slotIndex here is nothing but physical slot number and unitIndex is 
   * nothing but unit number.
   */
	return sysapiHpcSlotMapSlotPowerdownGet (slotIndex, powerdown);
#endif
}

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
L7_RC_t usmDbSlotSupportedPowerdownNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *powerdown)
{
  	L7_RC_t rc;

	rc = sysapiHpcSlotDbEntryIndexNextGet (unitIndex, slotIndex);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	return sysapiHpcSlotDbEntryPowerdownGet (*unitIndex, *slotIndex, powerdown);
}

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
L7_RC_t usmDbCardInSlotSupportedGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 cardIndex)
{
	return sysapiHpcCardInSlotDbEntryGet (unitIndex, slotIndex, cardIndex);
}

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
L7_RC_t usmDbCardInSlotSupportedNextGet(L7_uint32 *unitIndex, L7_uint32 *slotIndex, L7_uint32 *cardIndex)
{
	return sysapiHpcCardInSlotDbEntryIndexNextGet (unitIndex, slotIndex, cardIndex);
}


/******************************************************************
*******************************************************************
**
** Run-Time table access functions.
**
*******************************************************************
******************************************************************/

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
                                L7_uint32 *cardStatus)
{
    return cmgrCardStatusGet(unit, slot, cardStatus);
}

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
                                L7_uint32 *cardType)
{
    return cmgrCardConfiguredCardTypeGet(unit, slot, cardType);
}

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
                                L7_uint32 *adminMode)
{
    return cmgrSlotConfiguredAdminModeGet(unit, slot, adminMode);
}

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
                                L7_uint32 adminMode)
{
  return cmgrSlotAdminModeSet(unit, slot, adminMode);
}

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
                                L7_uint32 *powerMode)
{
#ifndef L7_CHASSIS
    return cmgrSlotPowerModeGet(unit, slot, powerMode);
#else
    POWER_CONTROL_t source;
    POWER_STATUS_t status;
    L7_RC_t rc = L7_FAILURE;
    if(L7_SUCCESS == (rc = cmgrSlotPowerStatusGet(unit, slot, &status, &source)))
    {
      if(POWER_ON == status)
        *powerMode = L7_ENABLE;
      else if(POWER_OFF == status)
        *powerMode = L7_DISABLE;
    }
    return rc;
#endif
}

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
                                L7_uint32 powerMode)
{
#ifndef L7_CHASSIS
    return cmgrSlotPowerModeSet(unit, slot, powerMode);
#else
  POWER_STATUS_t status;
  POWER_CONTROL_t source = POWER_ADMIN;

  if(powerMode == L7_ENABLE)
  {
    status = POWER_ON;
  }
  else if(powerMode == L7_DISABLE)
  {
    status = POWER_OFF;
  }
  else
    return L7_FAILURE;

  return cmgrSlotPowerStatusSet(unit, slot, status, source);
#endif
}

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
                                L7_BOOL *isFull)
{
    return cmgrSlotIsFullGet(unit, slot, isFull);
}

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
L7_RC_t usmDbSlotFirstGet(L7_uint32 unit, L7_uint32 *slotId)
{
    return cmgrSlotFirstGet(unit, slotId);
}

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
                        L7_uint32 *nextSlot)
{
    return cmgrSlotNextGet(unit, slot, nextSlot);
}

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
L7_RC_t usmDbSlotGet(L7_uint32 unit, L7_uint32 slotId)
{
    return cmgrSlotGet(unit, slotId);
}

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
L7_RC_t usmdbSlotFirstConfigGet(L7_uint32 unit, L7_uint32 *slotId)
{
    return cmgrSlotFirstConfigGet(unit, slotId);
}

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
                        L7_uint32 *nextSlot)
{
    return cmgrSlotConfigNextGet(unit, slot, nextSlot);
}

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
L7_RC_t usmdbSlotConfigGet(L7_uint32 unit, L7_uint32 slotId)
{
    return cmgrSlotConfigGet(unit, slotId);
}

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
L7_RC_t usmdbSlotFirstPhysicalGet(L7_uint32 unit, L7_uint32 *slotId)
{
    return cmgrSlotFirstPhysicalGet(unit, slotId);
}

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
                        L7_uint32 *nextSlot)
{
    return cmgrSlotPhysicalNextGet(unit, slot, nextSlot);
}

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
L7_RC_t usmdbSlotPhysicalGet(L7_uint32 unit, L7_uint32 slotId)
{
    return cmgrSlotPhysicalGet(unit, slotId);
}
 

/*****************************************************************
******************************************************************
**
**  Following are utility functions.
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
L7_RC_t usmDbCardIndexFromIDGet(L7_uint32 card_id, L7_uint32 *cx)
{
  return cmgrCardIndexFromIDGet(card_id, cx);
}

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
L7_RC_t usmDbSlotNumFromIndexGet(L7_uint32 ux, L7_uint32 sx, L7_uint32 *slot_number)
{
  return cmgrSlotNumFromIndexGet(ux, sx, slot_number);
}

/*********************************************************************
* @purpose  Helper routine to get unit index from unit id
*
* @param  unit_id  @b{(input)) the unit id
* @param  ux       @b{(output)) the unit index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitIndexFromIDGet(L7_uint32 unit_id, L7_uint32 *ux)
{
  return cmgrUnitIndexFromIDGet(unit_id, ux);
}

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
L7_RC_t usmDbSlotIndexFromNumGet(L7_uint32 ux, L7_uint32 slot, L7_uint32 *sx)
{
  return cmgrSlotIndexFromNumGet(ux, slot, sx);
}


/*********************************************************************
**********************************************************************
** USMDB Card Manager Debug Functions
**********************************************************************
*********************************************************************/

/***
** SUPPORTED-CARDS Table Test Cases
***/

/***********************************************************
** Debug utility function to display card information for
** specified card index.
***********************************************************/
L7_uint32 usmdbDebugSupCardInfoDisplay (L7_uint32 card_index)
{
    L7_RC_t rc;
	L7_uchar8 model[SYSAPI_HPC_CARD_MODEL_SIZE];
	L7_uchar8 descr[SYSAPI_HPC_CARD_DESCRIPTION_SIZE];
	L7_uint32 card_id;

	   printf("Index: %d\n", card_index);

	   rc = usmDbCardSupportedTypeIdGet (card_index, &card_id);
	   if (rc != L7_SUCCESS) 
	   {
		   return rc;
	   }
	   printf("Card Id: 0x%x\n", card_id);

	   rc = usmDbCardSupportedCardModelIdGet(card_index, model);
	   if (rc != L7_SUCCESS) 
	   {
		   return rc;
	   }
	   printf("Model: %s\n", model);

	   rc = usmDbCardSupportedCardDescriptionGet (card_index, descr);
	   if (rc != L7_SUCCESS) 
	   {
		   return rc;
	   }
	   printf("Description: %s\n", descr);
			  

	return 0;
}

/**************************************************
** Display information about all supported cards.
**************************************************/
L7_uint32 usmdbDebugSupCardsList (void)
{
	L7_uint32 card_index = 0;
	L7_RC_t	  rc;


	/* The card table starts with index 1, so passing 0 as the current index
	** returns the first entry in the supported cards table.
	*/
    rc = usmDbCardSupportedIndexNextGet(&card_index);

	while (rc == L7_SUCCESS) 
	{
		(void) usmdbDebugSupCardInfoDisplay (card_index);
		rc = usmDbCardSupportedIndexNextGet(&card_index);
	}

	return 0;
}

/**************************************************
** Display information about physical supported cards.
**************************************************/
L7_uint32 usmdbDebugSupPhysCardsList (void)
{
	L7_uint32 card_index = 0;
	L7_RC_t	  rc;


	/* The card table starts with index 1, so passing 0 as the current index
	** returns the first entry in the supported cards table.
	*/
    rc = usmDbCardSupportedPhysicalIndexNextGet(&card_index);

	while (rc == L7_SUCCESS) 
	{
		(void) usmdbDebugSupCardInfoDisplay (card_index);
		rc = usmDbCardSupportedPhysicalIndexNextGet(&card_index);
	}

	return 0;
}

/**************************************************
** Test the support-cards get-next functions.
**************************************************/
L7_uint32 usmdbDebugSupCardsNextGetTest (void)
{
    L7_uint32 card_index = 0;
	L7_RC_t	  rc;
	L7_uchar8 model[SYSAPI_HPC_CARD_MODEL_SIZE];
	L7_uchar8 descr[SYSAPI_HPC_CARD_DESCRIPTION_SIZE];
	L7_uint32 card_id;


	/* The card table starts with index 1, so passing 0 as the current index
	** returns the first entry in the supported cards table.
	*/
	card_index = 0;
    rc = usmDbCardSupportedPhysTypeIdNextGet(&card_index, &card_id);

	while (rc == L7_SUCCESS) 
	{
		printf("Index: %d : ", card_index);
		printf("Card Id: 0x%x\n", card_id);

		rc = usmDbCardSupportedPhysTypeIdNextGet(&card_index, &card_id);
	}

 	card_index = 0;
    rc = usmDbCardSupportedPhysModelIdNextGet(&card_index, model);

	while (rc == L7_SUCCESS) 
	{
		printf("Index: %d : ", card_index);
		printf("Model: %s\n", model);

		rc = usmDbCardSupportedPhysModelIdNextGet(&card_index, model);
	}

 	card_index = 0;
    rc = usmDbCardSupportedPhysDescriptionNextGet(&card_index, descr);

	while (rc == L7_SUCCESS) 
	{
		printf("Index: %d : ", card_index);
		printf("Description: %s\n", descr);

		rc = usmDbCardSupportedPhysDescriptionNextGet(&card_index, descr);
	}

	return 0;
}


/**************************************************
** Test the support-slots get-next functions.
**************************************************/
L7_uint32 usmdbDebugSupSlotsNextGetTest (void)
{
    L7_uint32 unit_index;
    L7_uint32 slot_index;
	L7_RC_t	  rc;

	L7_uint32 slot_number;
	L7_uint32 pluggable;
	L7_uint32 powerdown;


	/* The slot table starts with unit_index=1, slot_index= 1, 
	** so passing 0 as the current index
	** returns the first entry in the supported cards table.
	*/
	unit_index = 0;
	slot_index = 0;
    rc = usmDbSlotSupportedSlotNumberNextGet(&unit_index, &slot_index, &slot_number);

	while (rc == L7_SUCCESS) 
	{
		printf("Unit Index: %d, Slot index: %d : ", unit_index, slot_index);
		printf("Slot Number: %d\n", slot_number);

		rc = usmDbSlotSupportedSlotNumberNextGet(&unit_index, &slot_index, &slot_number);
	}


 	unit_index = 0;
	slot_index = 0;
    rc = usmDbSlotSupportedPluggableNextGet(&unit_index, &slot_index, &pluggable);

	while (rc == L7_SUCCESS) 
	{
		printf("Unit Index: %d, Slot index: %d : ", unit_index, slot_index);
		printf("Pluggable Indicator: %d\n", pluggable);

		rc = usmDbSlotSupportedPluggableNextGet(&unit_index, &slot_index, &pluggable);
	}

 	unit_index = 0;
	slot_index = 0;
    rc = usmDbSlotSupportedPowerdownNextGet(&unit_index, &slot_index, &powerdown);

	while (rc == L7_SUCCESS) 
	{
		printf("Unit Index: %d, Slot index: %d : ", unit_index, slot_index);
		printf("Powerdown Indicator: %d\n", powerdown);

		rc = usmDbSlotSupportedPowerdownNextGet(&unit_index, &slot_index, &powerdown);
	}

	return 0;
}

/**************************************************
** Test the supported-card-in-slot get-next functions.
**************************************************/
L7_uint32 usmdbDebugSupCardInSlotNextGetTest (void)
{
    L7_uint32 unit_index;
    L7_uint32 slot_index;
	L7_uint32 card_index;
	L7_RC_t	  rc;


	/* The card-in-slot table starts with unit_index=1, slot_index= 1, card_index = 1. 
	** so passing 0 as the current index
	** returns the first entry in the supported cards table.
	*/
	unit_index = 0;
	slot_index = 0;
	card_index = 0;
    rc = usmDbCardInSlotSupportedNextGet(&unit_index, &slot_index, &card_index);

	while (rc == L7_SUCCESS) 
	{
		printf("Unit Index: %d, Slot index: %d, Card Index: %d : ", unit_index, slot_index, card_index);
		printf("SUPPORTED\n");

		rc = usmDbCardInSlotSupportedNextGet(&unit_index, &slot_index, &card_index);
	}

	return 0;
}

/**************************************************
** Show information about supported units slots
** cards and tell user which cards are supported in
** which slots.
**
** This function provides an example for how 
** WEB/CLI should use the database.
**
**************************************************/
L7_uint32 usmdbDebugSupDbDump (void)
{
	L7_RC_t rc;
	L7_uint32 local_unit_index;
	L7_uint32 local_unit_id;

	L7_uint32 unit_index;
	L7_uint32 unit_id;
	L7_uchar8 unit_model [SYSAPI_HPC_UNIT_MODEL_SIZE];
	L7_uchar8 unit_descr [SYSAPI_HPC_UNIT_DESCRIPTION_SIZE];
	L7_uint32 mgmt_pref;
	L7_uint32 code_load_target_id;

	L7_uint32 ux;
	L7_uint32 slot_index;
 	L7_uint32 slot_number;
	L7_uint32 pluggable;
	L7_uint32 powerdown;

	L7_uint32 ux2;
	L7_uint32 sx;
	L7_uint32 card_index = 0;
	L7_uint32 card_id;


	/* Tell user on which unit type the code is running.
	*/
	rc = usmDbUnitMgrSupportedLocalUnitIndexGet (&local_unit_index);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}
	printf("Local Unit Index: %d\n", local_unit_index);

	rc = usmDbUnitMgrSupportedUnitIdGet (local_unit_index, &local_unit_id);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}
	printf("Local Unit ID: 0x%x\n", local_unit_id);

	/* For each supported unit:
	**	Display information about the unit,
	**  Display information about slots in the unit,
	**  For each slot display which cards are supported in the slot.
	*/


	/* Get the first supported unit.
	*/
  unit_index = 0;
  rc = usmDbUnitMgrSupportedUnitIdNextGet (&unit_index, &unit_id);
  while (rc == L7_SUCCESS) 
  {
	/* Get the rest of the information about this unit.
	*/
	rc =  usmDbUnitMgrSupportedModelGet (unit_index, unit_model);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	rc =  usmDbUnitMgrSupportedDescriptionGet (unit_index, unit_descr);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	rc =  usmDbUnitMgrSupportedMgmtPrefGet (unit_index, &mgmt_pref);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}

	rc =  usmDbUnitMgrSupportedCodeTargetIdGet (unit_index, &code_load_target_id);
	if (rc != L7_SUCCESS) 
	{
		return rc;
	}
	printf("---------------------Unit ---------------------------------------\n");
	printf("Unit Index: %d : ", unit_index);
	printf("Unit ID: 0x%x\n", unit_id);
	printf("Unit Model: %s\n", unit_model);
	printf("Unit Description: %s\n", unit_descr);
	printf("Unit Management Preference: %d\n", mgmt_pref);
	printf("Unit Code Load target ID: 0x%x\n", code_load_target_id);

	/* Get first slot in this unit.
	*/
	ux = unit_index;
	slot_index = 0;
    rc = usmDbSlotSupportedSlotNumberNextGet(&ux, &slot_index, &slot_number);
	while ((rc == L7_SUCCESS) && (ux == unit_index)) 
	{
		/* Get the rest of the information about this slot.
		*/
		rc = usmDbSlotSupportedPluggableGet(ux, slot_index, &pluggable);
		if (rc != L7_SUCCESS) 
		{
			return rc;
		}
		rc = usmDbSlotSupportedPowerdownGet(ux, slot_index, &powerdown);
		if (rc != L7_SUCCESS) 
		{
			return rc;
		}
		printf("   Slot index: %d : ", slot_index);
		printf("   Slot Number: %d\n", slot_number);
		printf("   Pluggable Indicator: %d\n", pluggable);
		printf("   Powerdown Indicator: %d\n", powerdown);


		/* Display which card IDs are supported in this slot.
		*/
		printf("      Supported Card IDs: ");
		card_index = 0;
		ux2 = ux;
		sx = slot_index;
		rc = usmDbCardInSlotSupportedNextGet(&ux2, &sx, &card_index);
		while ((rc == L7_SUCCESS) && (ux2 == ux) && (sx == slot_index)) 
		{
			rc = usmDbCardSupportedTypeIdGet (card_index, &card_id);
			if (rc != L7_SUCCESS) 
			{
				return rc;
			}

			printf("0x%x :", card_id);

			rc = usmDbCardInSlotSupportedNextGet(&ux2, &sx, &card_index);
		}
		printf("\n");

		/* Go to the next slot.
		*/
		rc = usmDbSlotSupportedSlotNumberNextGet(&ux, &slot_index, &slot_number);
	}

	/* Go to the next unit.
	*/
	rc = usmDbUnitMgrSupportedUnitIdNextGet (&unit_index, &unit_id);
  }


	/* Display information about all supported physical cards.
	** Note that the user does not need to know about logical cards.
	*/
	printf("\nDetailed Card Information:\n");
	(void)	usmdbDebugSupPhysCardsList ();

	return 0;
}


/**************************************************
** Invoke all Next-Get debug functions.
** This example shows how SNMP will use the 
** supported unit/slot/card database.
**************************************************/
L7_uint32 usmdbDebugSupNextGetDump (void)
{
	L7_uint32  usmdbDebugSupUnitsNextGetTest (void);

	(void) usmdbDebugSupUnitsNextGetTest ();   /* Unit table */
    (void) usmdbDebugSupSlotsNextGetTest ();  /* Slot Table */
    (void) usmdbDebugSupCardInSlotNextGetTest (); /* Card-In-Slot table */
    (void) usmdbDebugSupCardsNextGetTest ();  /* Card Table */


	return 0;
}
