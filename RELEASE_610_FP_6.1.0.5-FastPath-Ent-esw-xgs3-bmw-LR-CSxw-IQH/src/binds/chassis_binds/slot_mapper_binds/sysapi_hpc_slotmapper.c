/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 1999-2007
 *
 **********************************************************************
 *
 * @filename   sysapi_hpc_slotmapper.c
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
 * @author     asingh
 * @end
 *
 **********************************************************************/

/* include files */

#include "l7_common.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_slotmapper.h"

#include "log.h"

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
        L7_int32 *logicalSlot)
{
  *logicalSlot = phySlot;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the physical slot number for a given
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
        L7_int32 *phySlot)
{
  *phySlot = logicalSlot;
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Retrieves the unit number for the module in the given
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
        L7_int32 *unitNum)
{
  *unitNum = 0;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the physical slot number for the given unit 
 *
 * @param    unitNumber  @b{(input)} unit Number 
 *           phySlot     @b{(output)}  Physical Slot Number 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified unit could not be found
 *
 * @notes 
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSPhysicalSlotGet(L7_int32 unitNum,
        L7_int32 *phySlot)
{
  *phySlot = 0;
  return L7_SUCCESS;
}

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
        L7_int32 *phySlot)
{
  *unitNum = 0;
  *phySlot = logicalSlot;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Logical Slot for a given unit number and 
 *           the physical slot numbers 
 *
 * @param    unitNumber      @b{(input)} Unit Number 
 *           phySlot         @b{(input)} Physical Slot Number 
 *           logicalSlot     @b{(output)}  logical Slot Number
 *           
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified unit/slot could not be found
 *
 * @notes 
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapUSLogicalSlotGet(L7_int32 unitNum,
        L7_int32 phySlot,
        L7_int32 *logicalSlot)
{
  *logicalSlot = phySlot;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Module Type and the module index for a give
 *           Physical Slot
 *
 * @param    physicallot     @b{(input)}  Physical Slot Number
 *           moduleType      @b{(output)} Module Type 
 *           moduleIndex     @b{(output)} Module Index 
 *           
 *           
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes 
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPModuleInfoGet(L7_int32 physicalSlot,
        SYSAPI_CARD_TYPE_t *moduleType,
        SYSAPI_CARD_INDEX_t *moduleIndex)
{
  switch(physicalSlot)
  {
	case(L7_LAG_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_LAG;
	  break;
	case(L7_VLAN_SLOT_NUM):
	  *moduleType = SYSAPI_CARD_TYPE_VLAN_ROUTER;
	  break;
	case(L7_CPU_SLOT_NUM):
	  *moduleType = SYSAPI_CARD_TYPE_LOGICAL_CPU;
	  break;
	case(L7_LOOPBACK_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_LOOPBACK;
	  break;
	case(L7_TUNNEL_SLOT_NUM):
	  *moduleType = SYSAPI_CARD_TYPE_TUNNEL;
	  break;
	case(L7_CAPWAP_TUNNEL_SLOT_NUM):
	  *moduleType = SYSAPI_CARD_TYPE_CAPWAP_TUNNEL;
	  break;
	default:
	  *moduleType = SYSAPI_CARD_TYPE_LINE;
	  break;
  }

  *moduleIndex = SYSAPI_CARD_INDEX_INVALID;

  return L7_SUCCESS;
}

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
        L7_int32 *physicalSlot)
{
  *physicalSlot = 0; 
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Internal Slot Number for a given Module Type 
 *           and the module index 
 *
 * @param    moduleType      @b{(input)} Module Type 
 *           moduleIndex     @b{(input)} Module Index 
 *           intSlot         @b{(output)}Internal Slot Number
 *           
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified internal slot could not be found
 *
 * @notes 
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPInternalSlotGet(SYSAPI_CARD_TYPE_t moduleType,
        SYSAPI_CARD_INDEX_t moduleIndex,
        L7_int32 *intSlot)
{
  *intSlot = 0;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Physical Slot Number for a given Internal 
 *           unit and internal slot number 
 *
 * @param    intUnit         @b{(input)} Int Unit 
 *           intSlot         @b{(input)} Int Slot 
 *           intSlot         @b{(output)}Physical Slot Number
 *           
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes 
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapIntUnitSlotToPhySlotGet(L7_uint32 intUnit,
        L7_uint32 intSlot,
        L7_int32 *phySlot)
{
  *phySlot = intSlot;
  return L7_SUCCESS;
}

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
        L7_int32  *logSlot)
{
  *logSlot = intSlot;
  return L7_SUCCESS;
}

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
        L7_int32 *intSlot)
{
  *intSlot = logSlot;
  *intUnit = 0;  /* That's not correct, should be read from hpc */
  return L7_SUCCESS;
}

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
                                         L7_uint32 *pPluggable)
{
  *pPluggable = L7_FALSE;
  return L7_SUCCESS;
}

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
                                         L7_uint32 *pPowerdown)
{
  *pPowerdown = L7_FALSE;
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Return the number of supported slots.
*
* @param    none
*
* @returns  Number 1.
*
* @notes
*
* @end
*
**************************************************************************/
L7_uint32 hpcSlotInfoSupportedSlotNumGet(void)
{
	  return(1);
}

/******************************************************************************
* @purpose  To get the CFM's own slot number
*
* @param    pSlotNum - @b{(input)}  Slot Number
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void hpcCurrentSlotGet(L7_uint32 *pSlotNum)
{
	  *pSlotNum = 0;
}

/******************************************************************************
* @purpose  To initilize the sensor detector and sensor event processor.
*           Detector polls module's state periodically. If there is
*           any change in the status from the previous one, it informs the
*           event processor. The event processor identifies the module event
*           and notifies the registered components.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t hpcSensorHndlrInit(void)
{
  return L7_SUCCESS;
}

