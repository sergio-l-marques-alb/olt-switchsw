/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 1999-2007
 *
 **********************************************************************
 *
 * @filename   sysapi_hpc_slotmapper.h
 *
 * @purpose    APIs for the Slot Mapper module for chassis.
 *             Slot mapper APIs are used mostly for translation of
 *             chassis slot/port notation from stacking
 *             unit/slot/port notation and vice-versa. They are also
 *             used to get general infomation about slots.
 *
 * @component  Slot Mapper
 *
 * @comments
 *
 * @create     07/25/2005
 *
 * @author     bviswanath
 * @end
 *
 **********************************************************************/

#ifndef SLOTMAPPPER_API_H
#define SLOTMAPPPER_API_H

#include "sysapi_hpc.h"

/*  A slot information, */

typedef struct _slot_info_t
{
  L7_int32    physicalSlotNum;
  L7_int32    logicalSlotNum;
  L7_int32    unitNum;     /* Internal Stacking Unit Number    */
  L7_int32    intSlot;     /* Internal Stacking/CM Slot number */
  SYSAPI_CARD_TYPE_t   moduleType;
  SYSAPI_CARD_INDEX_t  moduleIndex;
  L7_BOOL     pluggable;   /* L7_TRUE - card can be plugged into the slot. */
  L7_BOOL     power_down;  /* L7_TRUE - Slot can be powered down.          */

} slotmapSlotInfo_t;

/* all API prototypes */

/*********************************************************************
 * @purpose  Retrieves the logical slot number for a given
 *           Physical Slot Number
 *
 * @param    phySlot     @b{(input)}  Physical Slot Number
 *           logicalSlot @b{(output)} logical Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapPLLogicalSlotGet(L7_int32 phySlot,
                                         L7_int32 *logicalSlot);

/*********************************************************************
 * @purpose  Retrieves the Physical slot number for a given
 *           Logical slot Number
 *
 * @param    logicalSlot @b{(input)} logical Slot Number
 *           phySlot     @b{(output)}  Physical Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified logical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapPLPhysicalSlotGet(L7_int32 logicalSlot,
                                          L7_int32 *phySlot);

/*********************************************************************
 * @purpose  Retrieves the Unit number for the module in the given
 *           Physical Slot
 *
 * @param    phySlot     @b{(input)}  Physical Slot Number
 *           unitNumber  @b{(output)} Unit Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSUnitNumGet(L7_int32 phySlot,
                                     L7_int32 *unitNum);

/*********************************************************************
 * @purpose  Retrieves the Physical slot number for the given Unit
 *
 * @param    unitNumber  @b{(input)}  unit Number
 *           phySlot     @b{(output)} Physical Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified unit could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSPhysicalSlotGet(L7_int32 unitNumber,
                                          L7_int32 *phySlot);

/*********************************************************************
 * @purpose  Retrieves the unit number and the physical slot numbers
 *            for the module in the given logical Slot
 *
 * @param    logicalSlot     @b{(input)}  logical Slot Number
 *           unitNumber      @b{(output)} Unit Number
 *           phySlor         @b{(output)} Physical Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified Logical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSUnitSlotGet(L7_int32 logicalSlot,
                                      L7_int32 *unitNum,
                                      L7_int32 *phySlot);

/*********************************************************************
 * @purpose  Retrieves the Logical Slot for a given Unit number and
 *           the Physical slot numbers
 *
 * @param    unitNumber      @b{(input)}  Unit Number
 *           phySlot         @b{(input)}  Physical Slot Number
 *           logicalSlot     @b{(output)} logical Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified unit/slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSLogicalSlotGet(L7_int32 unitNum,
                                         L7_int32 phySlot,
                                         L7_int32 *logicalSlot);

/*********************************************************************
 * @purpose  Retrieves the Module Type and the module index for a given
 *           Physical Slot
 *
 * @param    physicallot     @b{(input)}  Physical Slot Number
 *           moduleType      @b{(output)} Module Type
 *           moduleIndex     @b{(output)} Module Index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPModuleInfoGet(L7_int32 physicalSlot,
                                        SYSAPI_CARD_TYPE_t  *moduleType,
                                        SYSAPI_CARD_INDEX_t *moduleIndex);

/*********************************************************************
 * @purpose  Retrieves the Internal Slot Number for a given Module Type
 *           and the module index
 *
 * @param    moduleType      @b{(input)} Module Type
 *           moduleIndex     @b{(input)} Module Index
 *           intSlot         @b{(output)}Internal Slot Number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified internal slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPInternalSlotGet(SYSAPI_CARD_TYPE_t moduleType,
                                          SYSAPI_CARD_INDEX_t moduleIndex,
                                          L7_int32 *intSlot);

/*********************************************************************
* @purpose  Retrieves the Physical Slot Number for a given Internal
*           Unit and internal slot number
*
* @param    intUnit         @b{(input)} Int Unit
*           intSlot         @b{(input)} Int Slot
*           intSlot         @b{(output)}Physical Slot Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if specified physical slot could not be found
*
* @notes
*  @end
*********************************************************************/
L7_RC_t sysapiHpcSlotMapIntUnitSlotToPhySlotGet(L7_uint32 intUnit,
                                                L7_uint32 intSlot,
                                                L7_int32 *phySlot);

/*********************************************************************
 * @purpose  Retrieves the Physical Slot Number for a given Module Type
 *           and the module index
 *
 * @param    moduleType      @b{(input)} Module Type
 *           moduleIndex     @b{(input)} Module Index
 *           physicalSlot    @b{(output)}  Physical Slot Number
 *
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPPhysicalSlotGet(SYSAPI_CARD_TYPE_t moduleType,
                                          SYSAPI_CARD_INDEX_t moduleIndex,
                                          L7_int32 *physicalSlot);

/*********************************************************************
 * @purpose  Retrieves the Internal Unit/Slot Number for the given
 *           logical slot number
 *
 * @param    logSlot         @b{(input)} logical Slot
 *           intUnit         @b{(output)}Internal Unit
 *           intSlot         @b{(output)}Internal Slot
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSIntUnitSlotGet(L7_int32 logSlot,
                                         L7_int32 *intUnit,
                                         L7_int32 *intSlot);

/*********************************************************************
 * @purpose  Retrieves the Logical Slot Number for the given
 *           internal Unit/Slot number
 *
 * @param    intUnit         @b{(input)} Internal Unit
 *           intSlot         @b{(input)} Internal Slot
 *           logSlot         @b{(output)}Logical Slot
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSIntToLogSlotGet(L7_int32 intSlot,
                                          L7_int32  intUnit,
                                          L7_int32  *logSlot);

/*********************************************************************
 * @purpose  Retrieves the First Physical Slot Number
 *
 * @param    logicalSlot    @b{(input)} Location to fill the first slot
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if no Physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapFirstSlotGet(L7_int32 *physicalSlot);

/*********************************************************************
 * @purpose  Retrieves the Next physicalSlot Slot Number after the given slot
 *
 * @param    currentSlot    @b{(input)} current slot
 *           nextSlot       @b{(input)} Location to fill the slot
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if no physicalSlot slot could not be found after
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapNextSlotGet(L7_int32 firstSlot, L7_int32 *nextSlot);

/**************************************************************************
* @purpose  Return the number of supported slots.
*
* @param    none
*
* @returns  Number of supported slots.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcSlotInfoSupportedSlotNumGet(void);

/*********************************************************************
 * @purpose  Retrieves the pluggable capability of a given slot
 *
 * @param    slotNum    @b{(input)}  slot number
 *           pPluggable @b{(output)} Stores whether a slot accepts
 *                                   a module that is pluggable
 *
 * @returns  L7_SUCCESS on returning the pluggable status of a
 *                      slot successfully
 * @returns  L7_FAILURE Otherwise
 *
 * @notes    none
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapSlotPluggableGet(L7_uint32 slotNum,
                                         L7_uint32 *pPluggable);

/*********************************************************************
 * @purpose  Retrieves the power down capability of a given slot
 *
 * @param    slotNum    @b{(input)}  slot number
 *           pPluggable @b{(output)} Stores whether a slot can be
 *                                   power down or not
 *
 * @returns  L7_SUCCESS on returning the power down capability of a
 *                      given slot successfully
 * @returns  L7_FAILURE Otherwise
 *
 * @notes    none
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapSlotPowerdownGet(L7_uint32 slotNum,
                                         L7_uint32 *pPowerdown);

/**************************************************************************
* @purpose  Returns the slot info for a given slot number.
*
* @param    slot      @b{(input)}  Slot Number
*
* @returns  Pointer to the slot info structure, if success.
* @returns  L7_NULL, if wrong slot number is specified
*
* @end
*
*************************************************************************/
slotmapSlotInfo_t *hpcSlotInfoGet(L7_uint32 slot);

#endif /* SLOTMAPPER_API_H */
