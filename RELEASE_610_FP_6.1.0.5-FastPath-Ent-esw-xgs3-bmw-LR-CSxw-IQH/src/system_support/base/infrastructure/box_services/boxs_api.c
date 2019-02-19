/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
*
* @filename   boxs_api.c
*
* @purpose    Component code for box services
*
* @component  
*
* @comments   Box services api
*
* @create     1/15/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/
#include "commdefs.h"

#include <string.h>
#include "l7_common.h"
#include "boxs.h"
#include "boxs_api.h"
#include "boxs_debug.h"
#include "unitmgr_api.h"
#include "boxs_cfg.h"

extern boxsData_t gBoxsData[L7_ALL_UNITS]; 
extern boxsGlobalStatus_t globalStatus[L7_ALL_UNITS];
extern boxsGlobalStatus_t lastGlobalStatus[L7_ALL_UNITS];
extern void *boxsSemaphore;

extern boxsCfg_t * boxsCfg;

extern void (*boxsDataAvailableCallback)(void);


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


L7_RC_t boxsApiAllowedMinTempSet(L7_int32 minTemp)
{

  L7_RC_t rc = L7_FAILURE;
  if (  (minTemp <= BOXS_TEMP_RANGE_START) || (minTemp >= BOXS_TEMP_RANGE_END) )
  {
    LOG_MSG("invalid value (%d)\n", minTemp);
    rc = L7_FAILURE;
    return rc;
  }
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.minTemp != minTemp)
  {
    if (minTemp > boxsCfg->cfg.globalCfgData.maxTemp)
    {
	  LOG_MSG("invalid value (%d)\n",minTemp);
      rc = L7_FAILURE;
    }
    else 
    {
      boxsCfg->cfg.globalCfgData.minTemp = minTemp;
      boxsCfg->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
    }
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}


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


L7_RC_t boxsApiAllowedMaxTempSet(L7_int32 maxTemp)
{

  L7_RC_t rc = L7_FAILURE;
  if (  (maxTemp <= BOXS_TEMP_RANGE_START) || (maxTemp >= BOXS_TEMP_RANGE_END) )
  {
    LOG_MSG( "invalid value %d\n",maxTemp);
    rc = L7_FAILURE;
    return rc;
  }
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.maxTemp != maxTemp)
  {
    if (maxTemp < boxsCfg->cfg.globalCfgData.minTemp)
    {
		LOG_MSG("invalid value (%d)\n",maxTemp);
      rc = L7_FAILURE;
    }
    else 
    {
      boxsCfg->cfg.globalCfgData.maxTemp = maxTemp;
      boxsCfg->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
    }
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}



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


L7_RC_t boxsApiAllowedTempRangeSet(L7_int32 minTemp, L7_int32 maxTemp)
{

  L7_RC_t rc = L7_FAILURE;
  if (  (minTemp < BOXS_TEMP_RANGE_START) || 
	(minTemp > BOXS_TEMP_RANGE_END)  || 
	(maxTemp < BOXS_TEMP_RANGE_START) || 
 	(maxTemp > BOXS_TEMP_RANGE_END) )
  {
	  LOG_MSG("invalid value(s) minTemp %d, maxTemp %d\n",minTemp, maxTemp);
    rc = L7_FAILURE;
    return rc;
  }
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if ( (boxsCfg->cfg.globalCfgData.maxTemp != maxTemp) || (boxsCfg->cfg.globalCfgData.minTemp != minTemp) )
  {
    if (maxTemp < minTemp)
    {
		LOG_MSG( "invalid value(s) minTemp %d, maxTemp %d\n",minTemp, maxTemp);
      rc = L7_FAILURE;
    }
    else 
    {
      boxsCfg->cfg.globalCfgData.maxTemp = maxTemp;
      boxsCfg->cfg.globalCfgData.minTemp = minTemp;
      boxsCfg->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
    }
  } else rc = L7_SUCCESS;
  osapiSemaGive(boxsSemaphore);
  return rc;
}


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

L7_RC_t boxsApiAllowedTempRangeGet(L7_int32* minTemp, L7_int32* maxTemp)
{
  if (boxsCfg == L7_NULLPTR)
  {
    LOG_MSG( "NULL pointer %p\n",boxsCfg);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
     
  *minTemp = boxsCfg->cfg.globalCfgData.minTemp;
  *maxTemp = boxsCfg->cfg.globalCfgData.maxTemp;

  return L7_SUCCESS;
}


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


L7_RC_t boxsApiTempStatusTrapEnableSet(L7_BOOL enable)
{

  L7_RC_t rc = L7_FAILURE;
  if ( (enable != L7_ENABLE) && (enable != L7_DISABLE) )
  {
	  LOG_MSG( "invalid value %d\n",enable);
      rc = L7_FAILURE;
      return rc;
  }

  rc = L7_SUCCESS;
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.trapTempStatusEnabled != enable)
  {
      boxsCfg->cfg.globalCfgData.trapTempStatusEnabled = enable;
      boxsCfg->hdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}



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


L7_RC_t boxsApiFanStatusTrapEnableSet(L7_BOOL enable)
{

  L7_RC_t rc = L7_FAILURE;
  if ( (enable != L7_ENABLE) && (enable != L7_DISABLE) )
  {
	  LOG_MSG("invalid value %d\n",enable);
      rc = L7_FAILURE;
      return rc;
  }

  rc = L7_SUCCESS;
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.trapFanStatusEnabled != enable)
  {
      boxsCfg->cfg.globalCfgData.trapFanStatusEnabled = enable;
      boxsCfg->hdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}


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


L7_RC_t boxsApiPowSupplyStatusTrapEnableSet(L7_BOOL enable)
{

  L7_RC_t rc = L7_FAILURE;
  if ( (enable != L7_ENABLE) && (enable != L7_DISABLE) )
  {
	  LOG_MSG( "invalid value %d\n",enable);
      rc = L7_FAILURE;
      return rc;
  }

  rc = L7_SUCCESS;
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.trapPowSupplyStatusEnabled != enable)
  {
      boxsCfg->cfg.globalCfgData.trapPowSupplyStatusEnabled = enable;
      boxsCfg->hdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}

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
L7_RC_t boxsApiSfpStatusTrapEnableSet(L7_BOOL enable)
{
  L7_RC_t rc = L7_FAILURE;
  if ( (enable != L7_ENABLE) && (enable != L7_DISABLE) )
  {
	  LOG_MSG( "invalid value %d\n",enable);
      rc = L7_FAILURE;
      return rc;
  }

  rc = L7_SUCCESS;
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.trapSfpStatusEnabled != enable)
  {
      boxsCfg->cfg.globalCfgData.trapSfpStatusEnabled = enable;
      boxsCfg->hdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}

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
L7_RC_t boxsApiXfpStatusTrapEnableSet(L7_BOOL enable)
{
  L7_RC_t rc = L7_FAILURE;
  if ( (enable != L7_ENABLE) && (enable != L7_DISABLE) )
  {
	  LOG_MSG( "invalid value %d\n",enable);
      rc = L7_FAILURE;
      return rc;
  }

  rc = L7_SUCCESS;
  osapiSemaTake(boxsSemaphore, L7_WAIT_FOREVER);
  if (boxsCfg->cfg.globalCfgData.trapXfpStatusEnabled != enable)
  {
      boxsCfg->cfg.globalCfgData.trapXfpStatusEnabled = enable;
      boxsCfg->hdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(boxsSemaphore);
  return rc;
}

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

L7_RC_t boxsApiTempStatusTrapEnableGet(L7_BOOL *enable)
{
  if (boxsCfg == L7_NULLPTR)
  {
	LOG_MSG( "invalid value %d\n",enable);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
  *enable = boxsCfg->cfg.globalCfgData.trapTempStatusEnabled;

  return L7_SUCCESS;
}


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

L7_RC_t boxsApiFanStatusTrapEnableGet(L7_BOOL *enable)
{
  if (boxsCfg == L7_NULLPTR)
  {
	  LOG_MSG( "invalid value %d\n",enable);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
  *enable = boxsCfg->cfg.globalCfgData.trapFanStatusEnabled;

  return L7_SUCCESS;
}


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

L7_RC_t boxsApiPowSupplyStatusTrapEnableGet(L7_BOOL *enable)
{
  if (boxsCfg == L7_NULLPTR)
  {
	LOG_MSG( "box services configuration: null pointer\n",enable);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
  *enable = boxsCfg->cfg.globalCfgData.trapPowSupplyStatusEnabled;

  return L7_SUCCESS;
}

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
L7_RC_t boxsApiSfpStatusTrapEnableGet(L7_BOOL *enable)
{
  if (boxsCfg == L7_NULLPTR)
  {
	LOG_MSG( "box services configuration: null pointer\n",enable);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
  *enable = boxsCfg->cfg.globalCfgData.trapSfpStatusEnabled;

  return L7_SUCCESS;
}

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
L7_RC_t boxsApiXfpStatusTrapEnableGet(L7_BOOL *enable)
{
  if (boxsCfg == L7_NULLPTR)
  {
	LOG_MSG( "box services configuration: null pointer\n",enable);
    return L7_FAILURE;
  }
  /* we don't need any sync for read operation */
  *enable = boxsCfg->cfg.globalCfgData.trapXfpStatusEnabled;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Return number of fan in unt.
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiNumOfFansGet(L7_uint32 unit_number, L7_uint32 *numOfFans)
{ 
  unitMgrStatus_t status;

  *numOfFans = 0;

  unitMgrUnitStatusGet(unit_number, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  *numOfFans = gBoxsData[unit_number].NumofFans;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Return status of a fan in unt.
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @end
*
*********************************************************************/
L7_RC_t boxsApiFanStatusGet(L7_uint32 UnitIndex, L7_uint32 fanNum, HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_ITEM_STATE_t* itemState, L7_uint32* fanSpeed, L7_uint32* fanDuty)
{ 
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(UnitIndex, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  if (fanNum >= min(gBoxsData[UnitIndex].NumofFans, L7_MAX_FANS_PER_UNIT))
  {
    return L7_FAILURE;
  }
  else
  {
	  *itemType = gBoxsData[UnitIndex].FanStatusData[fanNum].itemType;
	  *itemState = gBoxsData[UnitIndex].FanStatusData[fanNum].itemState;
	  *fanSpeed = gBoxsData[UnitIndex].FanStatusData[fanNum].fanSpeed;
      *fanDuty = gBoxsData[UnitIndex].FanStatusData[fanNum].fanDuty;
  }
  return L7_SUCCESS;
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
L7_RC_t boxsApiNumOfPowSuppliesGet(L7_uint32 UnitIndex, L7_uint32 *numOfPowSupplies)
{
	unitMgrStatus_t status;

    *numOfPowSupplies = 0;

	unitMgrUnitStatusGet(UnitIndex, &status);
	if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
	  return L7_FAILURE;

	*numOfPowSupplies = gBoxsData[UnitIndex].NumofPwMods;
	return L7_SUCCESS;
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
L7_RC_t boxsApiPowSupplyStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 powSupplyNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_ITEM_STATE_t* itemState)
{
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(UnitIndex, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  if (powSupplyNum >= min(gBoxsData[UnitIndex].NumofPwMods, L7_MAX_PW_MODS_PER_UNIT))
  {
    return L7_FAILURE;
  }
  else
  {
	  *itemType = gBoxsData[UnitIndex].PwModStatusData[powSupplyNum].itemType;
	  *itemState = gBoxsData[UnitIndex].PwModStatusData[powSupplyNum].itemState;
  }
  return L7_SUCCESS;
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
L7_RC_t boxsApiNumOfTempSensorsGet(L7_uint32 UnitIndex, L7_uint32 *numOfTempSemsors)
{
	unitMgrStatus_t status;

    *numOfTempSemsors = 0;

	unitMgrUnitStatusGet(UnitIndex, &status);
	if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
	  return L7_FAILURE;

	*numOfTempSemsors = gBoxsData[UnitIndex].NumofTempSensors;
	return L7_SUCCESS;
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
L7_RC_t boxsApiTempSensorStatusGet(
	L7_uint32 UnitIndex, 
	L7_uint32 sensorNum, 
	HPC_BOXS_ITEM_TYPE_t* itemType, 
	HPC_BOXS_TEMPSENSOR_STATE_t* sensorState,
	L7_int32* temperature)
{
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(UnitIndex, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  if (sensorNum >= min(gBoxsData[UnitIndex].NumofTempSensors, L7_MAX_TEMP_SENSORS_PER_UNIT))
  {
    return L7_FAILURE;
  }
  else
  {
	  *itemType = gBoxsData[UnitIndex].TempSensorStatusData[sensorNum].itemType;
	  *sensorState = gBoxsData[UnitIndex].TempSensorStatusData[sensorNum].tempSensorState;
	  *temperature = gBoxsData[UnitIndex].TempSensorStatusData[sensorNum].temperature;
  }
  return L7_SUCCESS;
}




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

void boxsApiRegisterDataAvailableCallback(void (*callback)(void))
{
  boxsDataAvailableCallback = callback;
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
L7_RC_t boxsApiGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *global_status)
{
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(unit_number, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  *global_status = globalStatus[unit_number];

  return L7_SUCCESS;
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
L7_RC_t boxsApiLastGlobalStatusGet(L7_uint32 unit_number, boxsGlobalStatus_t *last_global_status)
{
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(unit_number, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  *last_global_status = lastGlobalStatus[unit_number];

  return L7_SUCCESS;
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
L7_RC_t boxsApiTimestampGet(L7_uint32 unit_number, L7_uint32 *time_stamp)
{
  unitMgrStatus_t status;

  unitMgrUnitStatusGet(unit_number, &status);
  if (status == L7_UNITMGR_UNIT_NOT_PRESENT)
    return L7_FAILURE;

  *time_stamp = gBoxsData[unit_number].timestamp;
  return L7_SUCCESS;
}

