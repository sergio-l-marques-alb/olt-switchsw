/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_slotmapper.c
*
* @purpose slot_mapper functions
*
* @component slot mapper stub functions for non-chassis platforms
* 
* @comments none
*
* @create 10/23/2003
*
* @author djohnson
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "usmdb_slotmapper.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_chassis.h"

/*********************************************************************
* @purpose  To Convert external unit,slot to internal unit,slot
*
* @param    extUnit        
* @param    extSlot
* @param    intUnit
* @param    intSlot
*
* @returns  L7_SUCCESS
*
* @notes    stub code to work for non chassis build  
*       
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapLogicalSlotToIntUnitSlot(L7_uint32 extUnit, L7_uint32 extSlot,
                                      L7_uint32 *intUnit, L7_uint32 *intSlot)
{
  *intSlot = extSlot;
  *intUnit = extUnit;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To Convert external unit,slot to internal unit,slot
*
* @param    extUnit
* @param    extSlot
* @param    intUnit
* @param    intSlot
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapPhysicalSlotToLogicalSlotGet(L7_uint32 intUnit, L7_uint32 intSlot,
                                          L7_uint32 *extUnit, L7_uint32 *extSlot)
{
  *extUnit = intUnit;
  *extSlot = intSlot;
   return L7_SUCCESS;
}

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
L7_RC_t  usmDbSlotMapUSPhysicalSlotGet(L7_uint32 unit , L7_uint32 *physicalSlot)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  get the first physicalSlot number in chassis
*
*
* @param    physicalSlot @b{{output}} the physical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSlotMapChassisSlotFirstGet (L7_uint32 *physicalSlot)
{
  return L7_FAILURE;
}

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
                                     L7_uint32 *moduleIndex)
{
  return L7_FAILURE;
}

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
                                  L7_uint32 *unit)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  get the next physicalSlot number in chassis
*
* @param    physicalSlot @b{{input}} the physical slot for this operation
* @param    nextPhysicalSlot @b{{output}} the physical slot for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if no physicalSlot slot could not be found after
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbSlotMapChassisSlotNextGet (L7_uint32 physicalSlot,
                                 L7_uint32 *nextphysicalSlot)
{
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  It Gets the status of all LEDs in the system.
*
* @param    ledStatus   Led Status Array.  LED status information.
*
* @returns  L7_SUCCESS  Successful getting LED status.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t usmDbChassisLedStatusGet(L7_CHASSIS_LED_STATS_t *ledStatus)
{
  return L7_FAILURE;
}
