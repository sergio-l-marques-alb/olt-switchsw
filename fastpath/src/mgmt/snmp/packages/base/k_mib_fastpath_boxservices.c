/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_boxservices.c 
*
* @purpose    Box Services agent code 
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
#include "k_private_base.h"
#include "k_mib_fastpath_boxservices_api.h"
#include "usmdb_boxs_api.h"
#include "usmdb_unitmgr_api.h"


boxServicesNotificationsGroup_t *
k_boxServicesNotificationsGroup_get(int serialNum,
							  ContextInfo *contextInfo,
							  int nominator)
{
	return L7_NULL;
}

boxServicesGroup_t *
k_boxServicesGroup_get(int serialNum,
							  ContextInfo *contextInfo,
							  int nominator)
{
  static boxServicesGroup_t boxServicesGroupData;
  L7_uint32 temp;
  L7_BOOL enable;
  L7_uint32 unit_number;
  
  if (usmDbUnitMgrNumberGet(&unit_number) != L7_SUCCESS)
    return NULL;

  switch (nominator)
  {
  case -1:

  case I_boxServicesNormalTempRangeMin:
    if (usmDbBoxsTempRangeGet(unit_number, &boxServicesGroupData.boxServicesNormalTempRangeMin, &temp) == L7_SUCCESS)
      SET_VALID(I_boxServicesNormalTempRangeMin, boxServicesGroupData.valid);
    if (nominator != -1) break;

  case I_boxServicesNormalTempRangeMax:
    if (usmDbBoxsTempRangeGet(unit_number, &temp, &boxServicesGroupData.boxServicesNormalTempRangeMax) == L7_SUCCESS)
      SET_VALID(I_boxServicesNormalTempRangeMax, boxServicesGroupData.valid);
    if (nominator != -1) break;
    
  case I_boxServicesTemperatureTrapEnable:
    if (usmDbBoxsTempStatusTrapEnableGet(unit_number, &enable) == L7_SUCCESS)
    {
        boxServicesGroupData.boxServicesTemperatureTrapEnable = (enable == L7_TRUE) ? D_boxServicesTemperatureTrapEnable_enable : D_boxServicesTemperatureTrapEnable_disable;
        SET_VALID(I_boxServicesTemperatureTrapEnable, boxServicesGroupData.valid);
    }
    if (nominator != -1) break;
    
  case I_boxServicesPSMStateTrapEnable:
    if (usmDbBoxsPowSupplyStatusTrapEnableGet(unit_number, &enable) == L7_SUCCESS)
    {
        boxServicesGroupData.boxServicesPSMStateTrapEnable = (enable == L7_TRUE) ? D_boxServicesPSMStateTrapEnable_enable : D_boxServicesPSMStateTrapEnable_disable;
        SET_VALID(I_boxServicesPSMStateTrapEnable, boxServicesGroupData.valid);
    }
    if (nominator != -1) break;
    
  case I_boxServicesFanStateTrapEnable:
    if (usmDbBoxsFanStatusTrapEnableGet(unit_number, &enable ) == L7_SUCCESS)
    {
        boxServicesGroupData.boxServicesFanStateTrapEnable = (enable == L7_TRUE) ? D_boxServicesFanStateTrapEnable_enable : D_boxServicesFanStateTrapEnable_disable;
        SET_VALID(I_boxServicesFanStateTrapEnable, boxServicesGroupData.valid);
    }
    if (nominator != -1) break;
    
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator >= 0 && !VALID(nominator, boxServicesGroupData.valid))
    return(NULL);
  return(&boxServicesGroupData);
}


#ifdef SETS
int
k_boxServicesGroup_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_boxServicesGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_boxServicesGroup_set(boxServicesGroup_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uint32 tempMin, tempMax;
  L7_uint32 unit_number;
  if (usmDbUnitMgrNumberGet(&unit_number) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* it makes sense setting 2 values at a time */
  if (VALID(I_boxServicesNormalTempRangeMin, data->valid) &&
      VALID(I_boxServicesNormalTempRangeMax, data->valid) &&
      usmDbBoxsTempRangeSet(unit_number, data->boxServicesNormalTempRangeMin, data->boxServicesNormalTempRangeMax) != L7_SUCCESS)
  {
    CLR_VALID(I_boxServicesNormalTempRangeMin, data->valid);
    CLR_VALID(I_boxServicesNormalTempRangeMax, data->valid);
    return COMMIT_FAILED_ERROR;
  } 
  else 
  {
    /* if we need to set only one value from temp. range, we need to read another, and reapply both */
    if (VALID(I_boxServicesNormalTempRangeMin, data->valid) &&
        (usmDbBoxsTempRangeGet(unit_number, &tempMin, &tempMax) != L7_SUCCESS || 
        usmDbBoxsTempRangeSet(unit_number, data->boxServicesNormalTempRangeMin, tempMax) != L7_SUCCESS)
        )
    {
      CLR_VALID(I_boxServicesNormalTempRangeMin, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    
    /* if we need to set only one value from temp. range, we need to read another, and reapply both */
    if (VALID(I_boxServicesNormalTempRangeMax, data->valid) &&
        (usmDbBoxsTempRangeGet(unit_number, &tempMin, &tempMax) != L7_SUCCESS || 
        usmDbBoxsTempRangeSet(unit_number, tempMin, data->boxServicesNormalTempRangeMax) != L7_SUCCESS)
        )
    {
      CLR_VALID(I_boxServicesNormalTempRangeMax, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    
    if (VALID(I_boxServicesTemperatureTrapEnable, data->valid) &&
        snmpBoxsTempStatusTrapEnableSet(unit_number, data->boxServicesTemperatureTrapEnable) != L7_SUCCESS
        )
    {
      CLR_VALID(I_boxServicesTemperatureTrapEnable, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    if (VALID(I_boxServicesPSMStateTrapEnable, data->valid) &&
        snmpBoxsPowSupplyStatusTrapEnableSet(unit_number, data->boxServicesPSMStateTrapEnable) != L7_SUCCESS
        )
    {
      CLR_VALID(I_boxServicesPSMStateTrapEnable, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    
    if (VALID(I_boxServicesFanStateTrapEnable, data->valid) &&
        snmpBoxsFanStatusTrapEnableSet(unit_number, data->boxServicesFanStateTrapEnable) != L7_SUCCESS
        )
    {
      CLR_VALID(I_boxServicesFanStateTrapEnable, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_boxServicesGroup_UNDO
/* add #define SR_boxServicesGroup_UNDO in sitedefs.h to
 * include the undo routine for the boxServicesGroup family.
 */
int
boxServicesGroup_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_boxServicesGroup_UNDO */
  
#endif

boxServicesFansEntry_t *
k_boxServicesFansEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 boxServicesFansIndex)
{
  static boxServicesFansEntry_t boxServicesFansEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 unit_number;
  if (usmDbUnitMgrNumberGet(&unit_number) != L7_SUCCESS)
    return NULL;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

  	boxServicesFansEntryData.boxServicesFanItemType = L7_NULL;
  	boxServicesFansEntryData.boxServicesFanItemState = L7_NULL;
  	boxServicesFansEntryData.boxServicesFanSpeed = L7_NULL;
    boxServicesFansEntryData.boxServicesFanDutyLevel = L7_NULL;
  }

  ZERO_VALID(boxServicesFansEntryData.valid);

  boxServicesFansEntryData.boxServicesFansIndex = boxServicesFansIndex;
  SET_VALID(I_boxServicesFansIndex, boxServicesFansEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpBoxsFanIndexGet(unit_number, boxServicesFansEntryData.boxServicesFansIndex) != L7_SUCCESS) :
       ( (snmpBoxsFanIndexGet(unit_number, boxServicesFansEntryData.boxServicesFansIndex) != L7_SUCCESS) &&
         (snmpBoxsFanIndexNextGet(unit_number,&boxServicesFansEntryData.boxServicesFansIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(boxServicesFansEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_boxServicesFansIndex:
    break;

  case I_boxServicesFanItemType:
    if (snmpBoxsFanTypeGet(unit_number, boxServicesFansEntryData.boxServicesFansIndex, 
                           &boxServicesFansEntryData.boxServicesFanItemType) == L7_SUCCESS)
      SET_VALID(I_boxServicesFanItemType, boxServicesFansEntryData.valid);
    break;

  case I_boxServicesFanItemState:
    if (snmpBoxsFanStateGet(unit_number, 
                            boxServicesFansEntryData.boxServicesFansIndex, 
                           &boxServicesFansEntryData.boxServicesFanItemState) == L7_SUCCESS)
      SET_VALID(I_boxServicesFanItemState, boxServicesFansEntryData.valid);
    break;

  case I_boxServicesFanSpeed:
    if (snmpBoxsFanSpeedGet(unit_number, 
                            boxServicesFansEntryData.boxServicesFansIndex, 
                           &boxServicesFansEntryData.boxServicesFanSpeed) == L7_SUCCESS)
      SET_VALID(I_boxServicesFanSpeed, boxServicesFansEntryData.valid);
	break;

  case I_boxServicesFanDutyLevel:
    if (snmpBoxsFanDutyGet(unit_number, 
                            boxServicesFansEntryData.boxServicesFansIndex, 
                           &boxServicesFansEntryData.boxServicesFanDutyLevel) == L7_SUCCESS)
      SET_VALID(I_boxServicesFanDutyLevel, boxServicesFansEntryData.valid);
    break;
    
  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, boxServicesFansEntryData.valid))
    return(NULL);

  return(&boxServicesFansEntryData);
}


boxServicesPowSuppliesEntry_t *
k_boxServicesPowSuppliesEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 boxServicesPowSupplyIndex)
{
  static boxServicesPowSuppliesEntry_t boxServicesPowSuppliesEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 unit_number;
  if (usmDbUnitMgrNumberGet(&unit_number) != L7_SUCCESS)
    return NULL;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    boxServicesPowSuppliesEntryData.boxServicesPowSupplyItemType = L7_NULL;
    boxServicesPowSuppliesEntryData.boxServicesPowSupplyItemState = L7_NULL;
  }

  ZERO_VALID(boxServicesPowSuppliesEntryData.valid);

  boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex = boxServicesPowSupplyIndex;
  SET_VALID(I_boxServicesPowSupplyIndex, boxServicesPowSuppliesEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpBoxsPowerSupplyIndexGet(unit_number, boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex) != L7_SUCCESS) :
       ( (snmpBoxsPowerSupplyIndexGet(unit_number, boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex) != L7_SUCCESS) &&
         (snmpBoxsPowerSupplyIndexNextGet(unit_number,&boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(boxServicesPowSuppliesEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_boxServicesPowSupplyIndex:
    break;

  case I_boxServicesPowSupplyItemType:
    if (snmpBoxsPowerSupplyTypeGet(unit_number, 
                                   boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex, 
                                   &boxServicesPowSuppliesEntryData.boxServicesPowSupplyItemType ) == L7_SUCCESS)
      SET_VALID(I_boxServicesPowSupplyItemType, boxServicesPowSuppliesEntryData.valid);
    break;

  case I_boxServicesPowSupplyItemState:
    if (snmpBoxsPowerSupplyStateGet(unit_number, 
                                    boxServicesPowSuppliesEntryData.boxServicesPowSupplyIndex, 
                                    &boxServicesPowSuppliesEntryData.boxServicesPowSupplyItemState) == L7_SUCCESS)
      SET_VALID(I_boxServicesPowSupplyItemState, boxServicesPowSuppliesEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, boxServicesPowSuppliesEntryData.valid))
    return(NULL);

  return(&boxServicesPowSuppliesEntryData);
}


boxServicesTempSensorsEntry_t *
k_boxServicesTempSensorsEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 boxServicesTempSensorIndex)
{
	static boxServicesTempSensorsEntry_t boxServicesTempSensorsEntryData;

	static L7_BOOL firstTime = L7_TRUE;
    L7_uint32 unit_number;
    if (usmDbUnitMgrNumberGet(&unit_number) != L7_SUCCESS)
      return NULL;

	if (firstTime == L7_TRUE)
	{
	  firstTime = L7_FALSE;

	  boxServicesTempSensorsEntryData.boxServicesTempSensorType = L7_NULL;
	  boxServicesTempSensorsEntryData.boxServicesTempSensorState = L7_NULL;
	  boxServicesTempSensorsEntryData.boxServicesTempSensorTemperature = L7_NULL;
	}

	ZERO_VALID(boxServicesTempSensorsEntryData.valid);

	boxServicesTempSensorsEntryData.boxServicesTempSensorIndex = boxServicesTempSensorIndex;
	SET_VALID(I_boxServicesTempSensorIndex, boxServicesTempSensorsEntryData.valid);

	if ( (searchType == EXACT) ?
		 (snmpBoxsTempSensorIndexGet(unit_number, boxServicesTempSensorsEntryData.boxServicesTempSensorIndex) != L7_SUCCESS) :
		 ( (snmpBoxsTempSensorIndexGet(unit_number, boxServicesTempSensorsEntryData.boxServicesTempSensorIndex) != L7_SUCCESS) &&
		   (snmpBoxsTempSensorIndexNextGet(unit_number,&boxServicesTempSensorsEntryData.boxServicesTempSensorIndex) != L7_SUCCESS) ) )
	{
	  ZERO_VALID(boxServicesTempSensorsEntryData.valid);
	  return(NULL);
	}

	switch (nominator)
	{
	case -1:
	case I_boxServicesTempSensorIndex:
	  break;

	case I_boxServicesTempSensorType:
	  if (snmpBoxsTempSensorTypeGet(unit_number, 
									 boxServicesTempSensorsEntryData.boxServicesTempSensorIndex, 
									 &boxServicesTempSensorsEntryData.boxServicesTempSensorType) == L7_SUCCESS)
		SET_VALID(I_boxServicesTempSensorType, boxServicesTempSensorsEntryData.valid);
	  break;

	case I_boxServicesTempSensorState:
	  if (snmpBoxsTempSensorStateGet(unit_number, 
									  boxServicesTempSensorsEntryData.boxServicesTempSensorIndex, 
									  &boxServicesTempSensorsEntryData.boxServicesTempSensorState) == L7_SUCCESS)
		SET_VALID(I_boxServicesTempSensorState, boxServicesTempSensorsEntryData.valid);
	  break;

	case I_boxServicesTempSensorTemperature:
	  if (snmpBoxsTempSensorTemperatureGet(unit_number, 
									  boxServicesTempSensorsEntryData.boxServicesTempSensorIndex, 
									  &boxServicesTempSensorsEntryData.boxServicesTempSensorTemperature) == L7_SUCCESS)
		SET_VALID(I_boxServicesTempSensorTemperature, boxServicesTempSensorsEntryData.valid);
	  break;
	  
	default:
	  return(NULL);
	  break;
	}
	
	if (nominator >= 0 && !VALID(nominator, boxServicesTempSensorsEntryData.valid))
	  return(NULL);

	return(&boxServicesTempSensorsEntryData);
}

