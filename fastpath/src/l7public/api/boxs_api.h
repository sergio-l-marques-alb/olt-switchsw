/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 1999-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename   boxs_api.h
*
* @purpose    Box services support
*
* @component  
*
* @comments
*
* @create     01/15/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/

#ifndef BOXS_API_H
#define BOXS_API_H
#include "sysapi_hpc.h"

typedef enum 
{
  BOXS_GLOBAL_STATUS_OK,
  BOXS_GLOBAL_STATUS_NONCRITICAL,
  BOXS_GLOBAL_STATUS_CRITICAL
} boxsGlobalStatus_t;

/*********************************************************************
*
* @purpose Set allowed min. temperature for normal functioning
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiAllowedMinTempSet(L7_int32 minTemp);


/*********************************************************************
*
* @purpose Set allowed max. temperature for normal functioning
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiAllowedMaxTempSet(L7_int32 maxTemp);


/*********************************************************************
*
* @purpose Set allowed temperature range for normal functioning
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiAllowedTempRangeSet(L7_int32 minTemp, L7_int32 maxTemp);

/*********************************************************************
*
* @purpose Get allowed temperature range for normal functioning
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/

L7_RC_t boxsApiAllowedTempRangeGet(L7_int32* minTemp, L7_int32* maxTemp);



/*********************************************************************
*
* @purpose Enable/disable temperature status trap
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiTempStatusTrapEnableSet(L7_BOOL enable);


/*********************************************************************
*
* @purpose Enable/disable fan status trap
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiFanStatusTrapEnableSet(L7_BOOL enable);


/*********************************************************************
*
* @purpose Enable/disable power supply status trap
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/


L7_RC_t boxsApiPowSupplyStatusTrapEnableSet(L7_BOOL enable);

/*********************************************************************
*
* @purpose Enable/disable SFP status trap
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiSfpStatusTrapEnableSet(L7_BOOL enable);

/*********************************************************************
*
* @purpose Enable/disable XFP status trap
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiXfpStatusTrapEnableSet(L7_BOOL enable);

/*********************************************************************
*
* @purpose Get temperature status trap enabled/disabled
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/

L7_RC_t boxsApiTempStatusTrapEnableGet(L7_BOOL *enable);


/*********************************************************************
*
* @purpose Get fan status trap enabled/disabled
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/

L7_RC_t boxsApiFanStatusTrapEnableGet(L7_BOOL *enable);

/*********************************************************************
*
* @purpose Get power supply status trap enabled/disabled
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/

L7_RC_t boxsApiPowSupplyStatusTrapEnableGet(L7_BOOL *enable);

/*********************************************************************
*
* @purpose Get SFP status trap enabled/disabled
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiSfpStatusTrapEnableGet(L7_BOOL *enable);

/*********************************************************************
*
* @purpose Get XFP status trap enabled/disabled
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiXfpStatusTrapEnableGet(L7_BOOL *enable);

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
L7_RC_t boxsApiNumOfFansGet(L7_uint32 UnitIndex, L7_uint32 *numOfFans);


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
L7_RC_t boxsApiFanStatusGet(
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
L7_RC_t boxsApiNumOfPowSuppliesGet(L7_uint32 UnitIndex, L7_uint32 *numOfPowSupplies);


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
L7_RC_t boxsApiPowSupplyStatusGet(
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
L7_RC_t boxsApiNumOfTempSensorsGet(L7_uint32 UnitIndex, L7_uint32 *numOfTempSemsors);


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
L7_RC_t boxsApiTempSensorStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 sensorNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_TEMPSENSOR_STATE_t* sensorState,
	L7_int32* temperature);

/*********************************************************************
*
* @purpose Register the 'Box Services data available' callback
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/

void boxsApiRegisterDataAvailableCallback(void (*callback)(void));


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
L7_RC_t boxsApiGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *global_status);

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
L7_RC_t boxsApiLastGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *last_global_status);

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
L7_RC_t boxsApiTimestampGet(L7_uint32 unit_number, L7_uint32 *time_stamp);

#endif /* BOXS_API_H */ 
