/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_slotmapper.c
*
* @purpose USMDB Code slot mapper
*
* @component slotmapper
*
* @comments none
*
* @create 22/07/05
*
* @author p.muralikrishna
*
* @end
*             
**********************************************************************/
#ifndef USMDB_SLOTMAPPER_H
#define USMDB_SLOTMAPPER_H

#include "l7_common.h"
#include "sysapi_hpc_slotmapper.h"

/*********************************************************************
* @purpose  To Convert user entered unit,slot to internal unit,
*           internal slot
*
* @param    extUnit @b{{input}} the user entered unit for this operation
* @param    logicalSlot @b{{input}} the user entered slot for this operation
* @param    intUnit @b{{output}} the internal unit for this operation
* @param    intSlot @b{{output}} the internal slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    user allways enters unit as 1 and logical slot for physical
*           cards
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapLogicalSlotToIntUnitSlot(L7_uint32 extUnit,  L7_uint32 logicalSlot,
                                      L7_uint32 *intUnit, L7_uint32 *intSlot);

/*********************************************************************
* @purpose  To Convert external unit,slot to internal unit,slot
*
* @param    extUnit @b{{input}} the user entered unit for this operation
* @param    logicalSlot @b{{input}} the user entered slot for this operation
* @param    intUnit @b{{output}} the internal unit for this operation
* @param    physicalSlot @b{{output}} the physical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapPhysicalSlotToLogicalSlotGet(L7_uint32 intUnit, 
                                          L7_uint32 logicalSlot,
                                          L7_uint32 *extUnit, 
                                          L7_uint32 *physicalSlot);
/*********************************************************************
* @purpose  To Convert unit to physical slot
*
* @param    unit    @b{{input}} the internal unit for this operation
* @param    physicalSlot @b{{output}} the physical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  usmDbSlotMapUSPhysicalSlotGet(L7_uint32 unit , L7_uint32 *physicalSlot);

/*********************************************************************
* @purpose  To convert physical slot to logical slot 
*
* @param    physicalSlot @b{{input}} the physical slot for this operation
* @param    logicalSlot @b{{output}} the logical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapPLLogicalSlotGet(L7_uint32 physicalSlot,
                                     L7_uint32 *logicalSlot);
/*********************************************************************
* @purpose  Get the moduletype and module index from physical slot
*
* @param    physicalSlot @b{{input}} the user entered unit for this operation
* @param    moduleType   @b{{output}} the user entered slot for this operation
* @param    moduleIndex  @b{{output}} the internal unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapMPModuleInfoGet (L7_uint32 physicalSlot,
                                     L7_uint32 *moduleType,
                                     L7_uint32 *moduleIndex);
/*********************************************************************
* @purpose  Get the physical slot from moduletype and moduleindex 
*
* @param    moduleType @b{{input}} moduletype present in the slot
* @param    moduleIndex @b{{input}} module index present in the slot
* @param    physicalSlot @b{{output}} the physical slot slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapMPPhysicalSlotGet(L7_uint32 moduleType,
                                      L7_uint32 moduleIndex,
                                      L7_uint32 *physicalSlot);
/*********************************************************************
* @purpose  Get the logical slot from unit and physicalSlot 
*
* @param    unit @b{{input}} unit number used in this opration
* @param    physicalSlot @b{{input}} physical slot position 
* @param    logicalSlot @b{{output}} logical slot 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/ 
L7_RC_t usmDbSlotMapUSLogicalSlotGet(L7_uint32 unit,
                                     L7_uint32 physicalSlot,
                                     L7_uint32 *logicalSlot);
/*********************************************************************
* @purpose  To Convert external unit,slot to internal unit,slot
*
* @param    logicalSlot @b{{input}} the user entered slot for this operation
* @param    unit @b{{output}} the internal unit for this operation
* @param    physicalSlot @b{{output}} the internal slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapUSUnitSlotGet (L7_uint32 logicalSlot,
                                   L7_uint32 *unit,
                                   L7_uint32 *physicalSlot);

/*********************************************************************
* @purpose  Retrieves the unit number for the module in a slot identified 
*           by the given physical slot number

* @param    physicalSlot @b{{input}} the internal unit for this operation
* @param    unit @b{{output}} the internal slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapUSUnitNumGet (L7_uint32 physicalSlot,
				  L7_uint32 *unit);

/*********************************************************************
* @purpose  Retrieves the physical slot number for a given logical slot number
*
* 
* @param    logicalSlot @b{{input}} the logical  slot for this operation
* @param    physicalSlot @b{{output}} the physical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapPLPhysicalSlotGet(L7_uint32 logicalSlot,
                                      L7_uint32 *physicalSlot);


/*********************************************************************
* @purpose  Retrieves the Internal Unit/Slot Number for the given
*           logical slot number
*
* @param    logicalSlot        @b{(input)} logical Slot
*           intUnit            @b{(output)}Internal Unit
*           intSlot            @b{(output)}Internal Slot
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapUSIntUnitSlotGet(L7_uint32 logicalSlot,
                                     L7_uint32 *intUnit,
                                     L7_uint32 *intSlot);

/*********************************************************************
* @purpose  Retrieves the Logical Slot Number for the given
*           internal Unit/Slot number
*
* @param    intUnit         @b{(input)} Internal Unit
*           intSlot         @b{(input)} Internal Slot
*           logicalSlot     @b{(output)}Logical Slot
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapUSIntToLogSlotGet(L7_uint32 intUnit,
                                      L7_uint32 intSlot,
                                      L7_uint32 *logicalSlot);

/*********************************************************************
* @purpose  Give a chassis slot, get the next physical chassis slot
*
* @param    physicalSlot @b{{input}} physical  slot in chassis
* @param    nextphysicalSlot @b{{output}} next slot in chassis 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE         No more slots are accessible
*
* @notes  Can also give FM or PM slot
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapChassisSlotNextGet (L7_uint32 physicalSlot,
                                 L7_uint32 *nextphysicalSlot);
/*********************************************************************

* @purpose  To Get the first External Slot in a chassis
*
* @param    physicalSlot @b{{output}} the first slot in chassis
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     Can also give a FM or PM slot
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapChassisSlotFirstGet (L7_uint32 *physicalSlot);

#endif
