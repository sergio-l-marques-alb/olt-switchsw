/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_boxservices_api.c 
*
* @purpose    Provide API functions for box services component objects
*
* @component  SNMP
*
* @comments
*
* @create    02/22/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/
#include "commdefs.h"


#include <k_private_base.h>
#include <usmdb_boxs_api.h>
#include "usmdb_boxs_api.h"


/**************************************************************************************************************
  Fans
**************************************************************************************************************/

/*********************************************************************
 * * @purpose  Get the next index of fan in the system
 * *
 * * @param    unit                      Unit Id
 * * @param  @b{(output)}  *index					 Fan index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index)
{
  L7_uint32 tempIndex;
  L7_uint32 numOfFans;

  tempIndex = *index;
  if (usmDbBoxsNumOfFansGet(UnitIndex, &numOfFans) == L7_SUCCESS)
  {
    if (++tempIndex < numOfFans)
    {
      *index = tempIndex;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get the current index of fan in the system
 * *
 * * @param    unit                      Unit Id
 * * @param    index					 Fan index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanIndexGet(L7_uint32 UnitIndex, L7_uint32 index)
{
	L7_uint32 numOfFans;
	if (usmDbBoxsNumOfFansGet(UnitIndex, &numOfFans) == L7_SUCCESS)
	{
	  if (index < numOfFans)
		  return L7_SUCCESS;
	}
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get the fan type
 * *
 * * @param    unit                      Unit Id
 * * @param    index					 Fan index
 * * @param    @b{(output)} *fanType					 Fan type 
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanType)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;
	L7_uint32 fanSpeed, fanDuty;


  if (usmDbBoxsFanStatusGet(UnitIndex, index, &itemType, &itemState,&fanSpeed, &fanDuty) == L7_SUCCESS)
  {
    *fanType = itemType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
 * * @purpose  Get the fan state
 * *
 * * @param    unit                      Unit Id
 * * @param    index					 Fan index
 * * @param    @b{(output)} *fanState    Fan state 
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanState)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;
	L7_uint32 fanSpeed, fanDuty;

  if (usmDbBoxsFanStatusGet(UnitIndex, index, &itemType, &itemState,&fanSpeed,&fanDuty) == L7_SUCCESS)
  {
    *fanState = itemState;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get the fan speed
 * *
 * * @param    unit                      Unit Id
 * * @param    index					 Fan index
 * * @param    @b{(output)} *fanSpeed    Fan speed
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanSpeedGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanSpeed)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;
    L7_uint32 fanDuty;

  return usmDbBoxsFanStatusGet(UnitIndex, index, &itemType, &itemState,fanSpeed, &fanDuty);
}

/*********************************************************************
 * * @purpose  Get the fan duty level
 * *
 * * @param    unit                      Unit Id
 * * @param    index					 Fan index
 * * @param    @b{(output)} *fanDuty    Fan speed
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsFanDutyGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanDuty)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;
    L7_uint32 fanSpeed;

  return usmDbBoxsFanStatusGet(UnitIndex, index, &itemType, &itemState,&fanSpeed, fanDuty);
}

/**************************************************************************************************************
  Power supplies
**************************************************************************************************************/

/*********************************************************************
 * * @purpose  Get the next index of power supply
 * *
 * * @param    unit                      Unit Id
 * * @param    @b{(output)} *index		 Power supply index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsPowerSupplyIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index)
{
  L7_uint32 tempIndex;
  L7_uint32 numOfFans;

  tempIndex = *index;
  if (usmDbBoxsNumOfPowSuppliesGet(UnitIndex, &numOfFans) == L7_SUCCESS)
  {
    if (++tempIndex < numOfFans)
    {
      *index = tempIndex;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get (verify) the index of power supply
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Power supply index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsPowerSupplyIndexGet(L7_uint32 UnitIndex, L7_uint32 index)
{
	L7_uint32 numOfFans;
	if (usmDbBoxsNumOfPowSuppliesGet(UnitIndex, &numOfFans) == L7_SUCCESS)
	{
	  if (index < numOfFans)
		  return L7_SUCCESS;
	}
  return L7_FAILURE;
}


/*********************************************************************
 * * @purpose  Get the type of power supply
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Power supply index
 * * @param    @b{(output)} *powSupplyType		 Power supply type
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsPowerSupplyTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *powSupplyType)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;

  if (usmDbBoxsPowSupplyStatusGet(UnitIndex, index, &itemType, &itemState) == L7_SUCCESS)
  {
    *powSupplyType = itemType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
 * * @purpose  Get the state of power supply
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Power supply index
 * * @param    @b{(output)} *powSupplyState		 Power supply state
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsPowerSupplyStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *powSupplyState)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
	HPC_BOXS_ITEM_STATE_t itemState;

  if (usmDbBoxsPowSupplyStatusGet(UnitIndex, index, &itemType, &itemState) == L7_SUCCESS)
  {
    *powSupplyState = itemState;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/**************************************************************************************************************
  Temperature sensors
**************************************************************************************************************/


/*********************************************************************
 * * @purpose  Get the next index of temperature sensor
 * *
 * * @param    unit                      Unit Id
 * * @param    @b{(output)} *index		 Temperature sensor index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsTempSensorIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index)
{
  L7_uint32 tempIndex;
  L7_uint32 numOfTempSensors;

  tempIndex = *index;
  if (usmDbBoxsNumOfTempSensorsGet(UnitIndex, &numOfTempSensors) == L7_SUCCESS)
  {
    if (++tempIndex < numOfTempSensors)
    {
      *index = tempIndex;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get (verify) the index of temperature sensor 
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Temperature sensor index
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsTempSensorIndexGet(L7_uint32 UnitIndex, L7_uint32 index)
{
	L7_uint32 numOfTempSensors;
	if (usmDbBoxsNumOfTempSensorsGet(UnitIndex, &numOfTempSensors) == L7_SUCCESS)
	{
	  if (index < numOfTempSensors)
		  return L7_SUCCESS;
	}
  return L7_FAILURE;
}


/*********************************************************************
 * * @purpose  Get the type of temperature sensor 
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Temperature sensor index
 * * @param    @b{(output)} *tempSensorType		 Temperature sensor type
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsTempSensorTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorType)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
  HPC_BOXS_TEMPSENSOR_STATE_t sensorState;
  L7_uint32 tempValue;

  if (usmDbBoxsTempSensorStatusGet(UnitIndex, index, &itemType, &sensorState, &tempValue) == L7_SUCCESS)
  {
    *tempSensorType = itemType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get the state of temperature sensor 
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Temperature sensor index
 * * @param    @b{(output)} *tempSensorState		 Temperature sensor state
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsTempSensorStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorState)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
  HPC_BOXS_TEMPSENSOR_STATE_t sensorState;
  L7_uint32 tempValue;

  if (usmDbBoxsTempSensorStatusGet(UnitIndex, index, &itemType, &sensorState, &tempValue) == L7_SUCCESS)
  {
    *tempSensorState = sensorState;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Get the temperature value  of temperature sensor 
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Temperature sensor index
 * * @param    @b{(output)} *tempSensorTemperature		 Temperature sensor temperature value 
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t
snmpBoxsTempSensorTemperatureGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorTemperature)
{
  HPC_BOXS_ITEM_TYPE_t itemType;
  HPC_BOXS_TEMPSENSOR_STATE_t sensorState;
  L7_uint32 tempValue;

  if (usmDbBoxsTempSensorStatusGet(UnitIndex, index, &itemType, &sensorState, &tempValue) == L7_SUCCESS)
  {
    *tempSensorTemperature = tempValue;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Enable or disable power supply status trap
 * *
 * * @param    unit          Unit Id
 * * @param    @b{(output)} enable	1 enable 2 disable
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/

L7_RC_t snmpBoxsPowSupplyStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable)
{
  L7_BOOL bEnable = L7_FALSE;

  switch ( enable )
  {
  case D_boxServicesPSMStateTrapEnable_enable: 
    bEnable = L7_TRUE;
    break;
  case D_boxServicesPSMStateTrapEnable_disable: 
    bEnable = L7_FALSE;
    break;
  default:
    return L7_FAILURE;
  }
  return usmDbBoxsPowSupplyStatusTrapEnableSet(UnitIndex, bEnable);
}



/*********************************************************************
 * * @purpose  Enable or disable temperature change trap
 * *
 * * @param    unit          Unit Id
 * * @param    @b{(output)} enable	1 enable 2 disable
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t snmpBoxsTempStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable)
{
  L7_BOOL bEnable = L7_FALSE;

  switch ( enable )
  {
  case D_boxServicesTemperatureTrapEnable_enable: 
    bEnable = L7_TRUE;
    break;
  case D_boxServicesTemperatureTrapEnable_disable: 
    bEnable = L7_FALSE;
    break;
  default:
    return L7_FAILURE;
  }
  return usmDbBoxsTempStatusTrapEnableSet(UnitIndex, bEnable);
}

/*********************************************************************
 * * @purpose  Enable or disable fan status trap
 * *
 * * @param    unit          Unit Id
 * * @param    index		 Temperature sensor index
 * * @param    @b{(output)} enable	1 enable 2 disable
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * *
 * * @notes   none
 * * @end
 * *********************************************************************/
L7_RC_t snmpBoxsFanStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable)
{
  L7_BOOL bEnable = L7_FALSE;

  switch ( enable )
  {
  case D_boxServicesFanStateTrapEnable_enable: 
    bEnable = L7_TRUE;
    break;
  case D_boxServicesFanStateTrapEnable_disable: 
    bEnable = L7_FALSE;
    break;
  default:
    return L7_FAILURE;
  }
  return usmDbBoxsFanStatusTrapEnableSet(UnitIndex, bEnable);
}

