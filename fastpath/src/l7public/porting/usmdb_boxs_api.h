/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename  usmdb_boxs_api.h
*
* @purpose   Provide interface to API for usmdb box services
*
* @component mgmt
*
* @comments
*
* @create    01/15/2007
*
* @author    vkozlov
*
* @end
*
**********************************************************************/

#ifndef USMDB_BOXS_API_H
#define USMDB_BOXS_API_H

#include "boxs_api.h"

#define BOXS_MAX_STATE_NAME_LENGTH 32

/*********************************************************************
 * @purpose Set the allowed temperature range for normal operation
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param minTemp        @b{(input)} Minimum value in range 
 * @param maxTemp        @b{(input)} Maximum value in range 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t usmDbBoxsTempRangeSet(L7_uint32 UnitIndex, L7_int32 minTemp, L7_int32 maxTemp);


/*********************************************************************
 * @purpose Get the allowed temperature range for normal operation
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param minTemp        @b{(output)} Minimum value in range 
 * @param maxTemp        @b{(output)} Maximum value in range 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsTempRangeGet(L7_uint32 UnitIndex, L7_int32 *minTemp, L7_int32 *maxTemp);


/*********************************************************************
 * @purpose Enable/disable temperature status trap
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(input)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsTempStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable);



/*********************************************************************
 * @purpose Enable/disable fan status trap
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(input)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsFanStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable);


/*********************************************************************
 * @purpose Enable/disable power supply status trap
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(input)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsPowSupplyStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable);

/*********************************************************************
 * @purpose Enable/disable SFP status trap
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(input)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsSfpStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable);

/*********************************************************************
 * @purpose Enable/disable XFP status trap
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(input)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsXfpStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable);

/*********************************************************************
 * @purpose Get temperature status trap enabled/disabled
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(output)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsTempStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable);


/*********************************************************************
 * @purpose Get fan status trap enabled/disabled
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(output)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsFanStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable);



/*********************************************************************
 * @purpose Get power supply status trap enabled/disabled
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(output)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsPowSupplyStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable);

/*********************************************************************
 * @purpose Get SFP status trap enabled/disabled
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(output)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsSfpStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable);

/*********************************************************************
 * @purpose Get XFP status trap enabled/disabled
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param enable        @b{(output)} L7_ENABLE,  L7_DISABLE
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsXfpStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable);

/*********************************************************************
*
* @purpose Return number of fan in unit
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsNumOfFansGet(L7_uint32 UnitIndex, L7_uint32 *numOfFans);

/*********************************************************************
*
* @purpose Return status of a fan in unit.
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsFanStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 fanNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_ITEM_STATE_t* itemState,
	L7_uint32* fanSpeed,
    L7_uint32* fanDuty);


/*********************************************************************
*
* @purpose Return number of power supplies in unit
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsNumOfPowSuppliesGet(L7_uint32 UnitIndex, L7_uint32 *numOfPowSupplies);


/*********************************************************************
*
* @purpose Return status of a power supply in unit.
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsPowSupplyStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 powSupplyNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_ITEM_STATE_t* itemState);

/*********************************************************************
*
* @purpose Return number of temp. sensors in unit
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsNumOfTempSensorsGet(L7_uint32 UnitIndex, L7_uint32 *numOfTempSemsors);

/*********************************************************************
*
* @purpose Return status of a power supply in unit.
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsTempSensorStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 sensorNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_TEMPSENSOR_STATE_t* sensorState,
	L7_int32* temperature);


/*********************************************************************
 * @purpose Get the general temperature of the switch. If we have several temp sensors,
 *  this will be the highest value (the most dangerous)
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param genTemp        @b{(output)} temperature value
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsGeneralTempGet(L7_uint32 UnitIndex, L7_int32 *genTemp);

/*********************************************************************
 * @purpose Get the general fan speed of the switch. This will be 
 * speed of 1st fan in the system
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param genTemp        @b{(output)} temperature value
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsGeneralFanSpeedGet(L7_uint32 UnitIndex, L7_uint32 *genFanSpeed);

/*********************************************************************
 * @purpose Get the general fan duty level of the switch. This will be 
 * duty level of 1st fan in the system
 *
 * @param UnitIndex  @b{(input)} the unit for this operation
 * @param genTemp        @b{(output)} temperature value
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbBoxsGeneralFanDutyLevelGet(L7_uint32 UnitIndex, L7_uint32 *genDutyLevel);

/*********************************************************************
*
* @purpose Get global status
*
* DESCRIPTION "Current status of the product.This is a rollup for the entire product.
*             The status is intended to give initiative to a snmp monitor to get further data when this status is abnormal.
*             This variable can take the following values:
*             OK
*                If fans and power supplies are functioning and the system did not reboot because of a HW watchdog failure
*                or a SW fatal error condition.
*             Non-critical
*                If at least one power supply is not functional or the system rebooted at least once because of a HW watchdog failure
*                or a SW fatal error condition.
*             Critical
*                If at least one fan is not functional, possibly causing a dangerous warming up of the device."
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *global_status);

/*********************************************************************
*
* @purpose Get last global status
*
* DESCRIPTION "The status before the current status which induced an initiative to issue a global status change trap."
*             This variable can take the following values:
*             OK
*                If fans and power supplies are functioning and the system did not reboot because of a HW watchdog failure
*                or a SW fatal error condition.
*             Non-critical
*                If at least one power supply is not functional or the system rebooted at least once because of a HW watchdog failure
*                or a SW fatal error condition.
*             Critical
*                If at least one fan is not functional, possibly causing a dangerous warming up of the device."
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsLastGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *last_global_status);

/*********************************************************************
*
* @purpose Get last global status
*
* DESCRIPTION "The last time that the product global status has been updated."
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBoxsTimestampGet(L7_uint32 unit_number, L7_uint32 *time_stamp);

/*********************************************************************
 * * @purpose  Get the next index of fan in the system
 * *
 * * @param    unit                      Unit Id
 * * @param  @b{(output)}  *index          Fan index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t usmDbBoxsFanIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);

/*********************************************************************
 * * @purpose  Get the next index of power supply
 * *
 * * @param    unit                      Unit Id
 * * @param    @b{(output)} *index     Power supply index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t usmDbBoxsPowerSupplyIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);

/*********************************************************************
 * * @purpose  Get the next index of temperature sensor
 * *
 * * @param    unit                      Unit Id
 * * @param    @b{(output)} *index     Temperature sensor index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t usmDbBoxsTempSensorIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);



#endif /* USMDB_BOXS_API_H */
