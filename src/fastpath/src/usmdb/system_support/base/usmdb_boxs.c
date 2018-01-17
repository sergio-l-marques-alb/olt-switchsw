/*********************************************************************

*

* (C) Copyright Broadcom Corporation 2001-2007

*

**********************************************************************

*

* @filename  usmdb_boxs.c

*

* @purpose   Provide interface to box services API's

*

* @component unitmgr

*

* @comments

*

* @create    01/15/2008

*

* @author    vkozlov

*

* @end

*

**********************************************************************/

#include "commdefs.h"





#include <l7_common.h>

#include <stdio.h>





#include <stdlib.h>

#include <string.h>

#include <osapi.h>



#include "usmdb_boxs_api.h"

#include "boxs_api.h"



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

L7_RC_t usmDbBoxsTempRangeSet(L7_uint32 UnitIndex, L7_int32 minTemp, L7_int32 maxTemp)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiAllowedTempRangeSet(minTemp, maxTemp);

  return rc;

}





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

L7_RC_t usmDbBoxsTempRangeGet(L7_uint32 UnitIndex, L7_int32 *minTemp, L7_int32 *maxTemp)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiAllowedTempRangeGet(minTemp, maxTemp);

  return rc;

}







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

L7_RC_t usmDbBoxsTempStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiTempStatusTrapEnableSet(enable);

  return rc;

}







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

L7_RC_t usmDbBoxsFanStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiFanStatusTrapEnableSet(enable);

  return rc;

}





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

L7_RC_t usmDbBoxsPowSupplyStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiPowSupplyStatusTrapEnableSet(enable);

  return rc;

}



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

L7_RC_t usmDbBoxsSfpStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiSfpStatusTrapEnableSet(enable);

  return rc;

}



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

L7_RC_t usmDbBoxsXfpStatusTrapEnableSet(L7_uint32 UnitIndex, L7_BOOL enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiXfpStatusTrapEnableSet(enable);

  return rc;

}





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

L7_RC_t usmDbBoxsTempStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiTempStatusTrapEnableGet(enable);

  return rc;

}





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

L7_RC_t usmDbBoxsFanStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiFanStatusTrapEnableGet(enable);

  return rc;

}





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

L7_RC_t usmDbBoxsPowSupplyStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiPowSupplyStatusTrapEnableGet(enable);

  return rc;

}



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

L7_RC_t usmDbBoxsSfpStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiSfpStatusTrapEnableGet(enable);

  return rc;

}



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

L7_RC_t usmDbBoxsXfpStatusTrapEnableGet(L7_uint32 UnitIndex, L7_BOOL* enable)

{

  L7_RC_t rc;

  (void)UnitIndex;

  rc = boxsApiXfpStatusTrapEnableGet(enable);

  return rc;

}





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

L7_RC_t usmDbBoxsNumOfFansGet(L7_uint32 UnitIndex, L7_uint32 *numOfFans)

{

	return boxsApiNumOfFansGet(UnitIndex, numOfFans);

}





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

    L7_uint32* fanDuty)

{

	return boxsApiFanStatusGet(UnitIndex, fanNum, itemType, itemState, fanSpeed, fanDuty);

}



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

L7_RC_t usmDbBoxsNumOfPowSuppliesGet(L7_uint32 UnitIndex, L7_uint32 *numOfPowSupplies)

{

	return boxsApiNumOfPowSuppliesGet(UnitIndex, numOfPowSupplies);

}



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

	HPC_BOXS_ITEM_STATE_t* itemState)

{

	return boxsApiPowSupplyStatusGet(UnitIndex, powSupplyNum, itemType, itemState);

}



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

L7_RC_t usmDbBoxsNumOfTempSensorsGet(L7_uint32 UnitIndex, L7_uint32 *numOfTempSemsors)

{

	return boxsApiNumOfTempSensorsGet(UnitIndex, numOfTempSemsors);

}



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

	L7_int32* temperature)

{

	return boxsApiTempSensorStatusGet(UnitIndex, sensorNum, itemType, sensorState, temperature);

}







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

L7_RC_t usmDbBoxsGeneralTempGet(L7_uint32 UnitIndex, L7_int32 *genTemp)

{

  L7_uint32 numOfTempSemsors;

  L7_uint32 i;

  L7_RC_t rc = L7_FAILURE;

  HPC_BOXS_ITEM_TYPE_t itemType;

  HPC_BOXS_TEMPSENSOR_STATE_t sensorState;

  L7_int32 temperature, minTemp = 0;

  

  if (boxsApiNumOfTempSensorsGet(UnitIndex, &numOfTempSemsors) == L7_SUCCESS)

  {

    for (i=0;i<numOfTempSemsors;i++)

    {

      if ( (boxsApiTempSensorStatusGet(UnitIndex, i, &itemType, &sensorState,&temperature) == L7_SUCCESS) &&

          (sensorState == HPC_TEMPSENSOR_STATE_NORMAL ||

           sensorState == HPC_TEMPSENSOR_STATE_WARNING ||

           sensorState == HPC_TEMPSENSOR_STATE_CRITICAL ||

           sensorState == HPC_TEMPSENSOR_STATE_SHUTDOWN) 

           )

      {

        if (rc != L7_SUCCESS)

        {

            minTemp = temperature;

            rc = L7_SUCCESS;

        }

        else

        {

          if(temperature < minTemp)

            minTemp = temperature;

        }

      }

    }

  }

  if (rc == L7_SUCCESS)

  {

    *genTemp = minTemp;

  }

  return rc;

}





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

L7_RC_t usmDbBoxsGeneralFanSpeedGet(L7_uint32 UnitIndex, L7_uint32 *genFanSpeed)

{

  L7_RC_t rc = L7_FAILURE;

  L7_uint32 numOfFans,i;



  HPC_BOXS_ITEM_TYPE_t itemType; 

  HPC_BOXS_ITEM_STATE_t itemState;

  L7_uint32 fanSpeed, fanDuty;

  

  if (boxsApiNumOfFansGet(UnitIndex, &numOfFans) == L7_SUCCESS)

  {

    for (i=0;i<numOfFans;i++)

    {

      if( (boxsApiFanStatusGet(UnitIndex, i, &itemType, &itemState,&fanSpeed, &fanDuty) == L7_SUCCESS) &&

          (itemState == HPC_ITEMSTATE_OPERATIONAL) )

      {

        *genFanSpeed = fanSpeed;

        rc = L7_SUCCESS;

        break;

      }

    }

  }

  return rc;

}



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

L7_RC_t usmDbBoxsGeneralFanDutyLevelGet(L7_uint32 UnitIndex, L7_uint32 *genDutyLevel)

{

  L7_RC_t rc = L7_FAILURE;

  L7_uint32 numOfFans,i;



  HPC_BOXS_ITEM_TYPE_t itemType; 

  HPC_BOXS_ITEM_STATE_t itemState;

  L7_uint32 fanSpeed, fanDuty;

  

  if (boxsApiNumOfFansGet(UnitIndex, &numOfFans) == L7_SUCCESS)

  {

    for (i=0;i<numOfFans;i++)

    {

      if( (boxsApiFanStatusGet(UnitIndex, i, &itemType, &itemState,&fanSpeed, &fanDuty) == L7_SUCCESS) &&

          (itemState == HPC_ITEMSTATE_OPERATIONAL) )

      {

        *genDutyLevel = fanDuty;

        rc = L7_SUCCESS;

        break;

      }

    }

  }

  return rc;

}



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

L7_RC_t usmDbBoxsGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *global_status)

{

  return boxsApiGlobalStatusGet(unit_number, global_status);

}



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

L7_RC_t usmDbBoxsLastGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *last_global_status)

{

  return boxsApiLastGlobalStatusGet(unit_number, last_global_status);

}



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

L7_RC_t usmDbBoxsTimestampGet(L7_uint32 unit_number, L7_uint32 *time_stamp)

{

  return boxsApiTimestampGet(unit_number, time_stamp);

}



