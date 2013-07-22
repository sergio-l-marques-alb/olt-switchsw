/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
*
* @filename   boxs.c
*
* @purpose    Component code for box services
*
* @component
*
* @comments   Handles box services information
*
* @create     01/15/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/

#include "commdefs.h"
#include <string.h>
#include "l7_common.h"
#include "sysapi_hpc.h"
#include "boxs.h"
#include "boxs_debug.h"
#include "osapi.h"
#include "registry.h"
#include "unitmgr_api.h"
#include "usmdb_sim_api.h"
#include "trapapi.h"
#include "bxs_exports.h"


#include "boxs_cfg.h"
#include "boxs_api.h"

void *boxsRspQ = L7_NULLPTR;
void *boxsReqQ = L7_NULLPTR;

extern void *boxsSemaphore;
extern boxsCfg_t * boxsCfg;

L7_uint32 L7_boxs_Req_task_id = 0;
L7_uint32 L7_boxs_Rsp_task_id = 0;

boxsData_t gBoxsData[L7_ALL_UNITS];    /* index zero not used */
boxsGlobalStatus_t globalStatus[L7_ALL_UNITS];
boxsGlobalStatus_t lastGlobalStatus[L7_ALL_UNITS];


/* This callback function will be called only once,
  indicating condition that data is available after initialization*/
void (*boxsDataAvailableCallback)(void) = L7_NULLPTR;


#define DEBUG_BOXS 0


/*********************************************************************
*
* @purpose Initialize the  box services component.
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t boxsInitialize(void)
{
  L7_uint32 i ;

  memset(gBoxsData, 0, sizeof(boxsData_t));

  for (i = 0; i < L7_ALL_UNITS; i++)
  {
    globalStatus[i]     = BOXS_GLOBAL_STATUS_OK;
    lastGlobalStatus[i] = BOXS_GLOBAL_STATUS_OK;
  }

  boxsSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (boxsSemaphore == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Failed to Create Box Services Semaphore.\n");
    return L7_FAILURE;
  }

  /* Only poll for status if we support either fans, powersupplies, temperature, etc. */
  if (cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_FAN_STATUS_FEATURE_ID) ||
      cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_PWR_SUPPLY_STATUS_FEATURE_ID) ||
      cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_TEMPERATURE_FEATURE_ID) ||
      cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_TEMPERATURE_STATUS_FEATURE_ID) ||
      cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_SFP_STATUS_FEATURE_ID) ||
      cnfgrIsFeaturePresent(L7_BOX_SERVICES_COMPONENT_ID, L7_BXS_XFP_STATUS_FEATURE_ID))
  {
    boxsRspQ = (void *)osapiMsgQueueCreate("boxsRespQ", BOXS_MSG_COUNT, BOXS_MSG_SIZE);
    if (boxsRspQ == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Failed to Create Box Services Response Queue.\n");
      return L7_FAILURE;
    }

    boxsReqQ = (void *)osapiMsgQueueCreate("boxsReqQ", BOXS_MSG_COUNT, BOXS_MSG_SIZE);
    if (boxsReqQ == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Failed to Create Box Services Request Queue.\n");
      return L7_FAILURE;
    }

    L7_boxs_Req_task_id = osapiTaskCreate("boxs Req",
                                          boxsReqTask, 0, 0,
                                          L7_DEFAULT_STACK_SIZE,
                                          L7_DEFAULT_TASK_PRIORITY,
                                          L7_DEFAULT_TASK_SLICE);

    if (L7_boxs_Req_task_id == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Failed to Create Box Services Request Task.\n");
      return L7_FAILURE;
    }

    L7_boxs_Rsp_task_id = osapiTaskCreate("boxs Resp",
                                          boxsRspTask, 0, 0,
                                          L7_DEFAULT_STACK_SIZE,
                                          L7_DEFAULT_TASK_PRIORITY,
                                          L7_DEFAULT_TASK_SLICE);

    if (L7_boxs_Rsp_task_id == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Failed to Create Box Services Response Task.\n");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Removes anything allocated during initialization
*
* @returns L7_SUCCESS  if all uninitialization is done
* @returns L7_FAILURE  if something fails during uninitialization
*
* @end
*
*********************************************************************/
void boxsUninitialize(void)
{
  if (L7_boxs_Req_task_id != 0)
  {
    osapiTaskDelete(L7_boxs_Req_task_id);
  }

  if (L7_boxs_Rsp_task_id != 0)
  {
    osapiTaskDelete(L7_boxs_Rsp_task_id);
  }

  if (boxsRspQ != L7_NULLPTR)
  {
    osapiMsgQueueDelete(boxsRspQ);
  }

  if (boxsReqQ != L7_NULLPTR)
  {
    osapiMsgQueueDelete(boxsReqQ);
  }

  if (boxsSemaphore != L7_NULL)
  {
    (void)osapiSemaDelete(boxsSemaphore);
  }
}

/*********************************************************************
* @purpose  Receives messages from HPC
*
* @param    src_key {(input)}  Key (mac-address) of the unit that sent the msg
* @param    msg     {(input)}  The buffer holding the message
* @param    msg_len {(input)}  The buffer length of the message
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t boxsHPCReceiveCallback(L7_enetMacAddr_t src_key,
                               boxsMsg_t *msg, L7_uint32 length)
{
  boxsMsg_t toQ;

  memcpy(&toQ, msg, sizeof(boxsMsg_t));

  if (msg->hdr.msgType == L7_BOXS_REQUEST)
  {
    osapiMessageSend(boxsReqQ, &toQ, BOXS_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }

  else if (msg->hdr.msgType == L7_BOXS_RESPONSE)
  {
    osapiMessageSend(boxsRspQ, &toQ, BOXS_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read the local boxs data via HPC API
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t boxsLocalDataRead(boxsData_t *boxsData)
{
  L7_uint32 fanCount = 0;
  L7_uint32 powerSupplyCount = 0;
  L7_uint32 tempSensorCount = 0;
  L7_uint32 sfpCount = 0;
  L7_uint32 xfpCount = 0;
  L7_uint32 i=0;
  L7_RC_t rc = L7_SUCCESS;

  memset(boxsData, 0, sizeof(boxsData_t));

  /* reading fans count */
  fanCount = sysapiHpcLocalFanCountGet();
  if (fanCount > L7_MAX_FANS_PER_UNIT)
  {
    /* Need to increase L7_MAX_FANS_PER_UNIT */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Fan status data truncated.");
  }

  boxsData->NumofFans = fanCount;

  /* reading all fans status */
  for (i = 0; i < min(fanCount, L7_MAX_FANS_PER_UNIT); i++)
  {
      if ((rc = sysapiHpcLocalFanDataGet(i, &boxsData->FanStatusData[i])) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Error %d on call to sysapiHpcLocalFanDataGet(%d)\n",rc, i );
          return L7_FAILURE;
      }
  }

  /* reading power supplies count */
  powerSupplyCount = sysapiHpcLocalPowerSupplyCountGet();
  if (powerSupplyCount > L7_MAX_PW_MODS_PER_UNIT)
  {
    /* Need to increase L7_MAX_PW_MODS_PER_UNIT */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Power supply data truncated.");
  }

  boxsData->NumofPwMods = powerSupplyCount;

  /* reading power supplies status */
  for (i = 0; i < min(powerSupplyCount, L7_MAX_PW_MODS_PER_UNIT); i++)
  {
      if ((rc = sysapiHpcLocalPowerSupplyDataGet(i, &boxsData->PwModStatusData[i])) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Error %d on call to sysapiHpcLocalPowerSupplyDataGet(%d)\n",rc, i );
          return L7_FAILURE;
      }
  }

  /* reading temperature sensors count */
  tempSensorCount = sysapiHpcLocalTempSensorCountGet();
  if (tempSensorCount > L7_MAX_TEMP_SENSORS_PER_UNIT)
  {
    /* Need to increase L7_MAX_TEMP_SENSORS_PER_UNIT */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Temperature sensor data truncated.");
  }

  boxsData->NumofTempSensors = tempSensorCount;

  /* reading temperature sensors status */
  for (i = 0; i < min(tempSensorCount, L7_MAX_TEMP_SENSORS_PER_UNIT); i++)
  {
      if ((rc = sysapiHpcLocalTempSensorDataGet(i, &boxsData->TempSensorStatusData[i])) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Error %d on call to sysapiHpcLocalPowerSupplyDataGet(%d)\n",rc, i );
          return L7_FAILURE;
      }
  }

  /* reading SFP count */
  sfpCount = sysapiHpcLocalSfpCountGet();
  if (sfpCount > L7_MAX_SFPS_PER_UNIT)
  {
    /* Need to increase L7_MAX_SFPS_PER_UNIT */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "SFP data truncated.");
  }

  boxsData->NumofSFPs = sfpCount;

  /* reading SFP status */
  for (i = 0; i < min(sfpCount, L7_MAX_SFPS_PER_UNIT); i++)
  {
      if ((rc = sysapiHpcLocalSfpDataGet(i, &boxsData->sfpStatusData[i])) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Error %d on call to sysapiHpcLocalSfpDataGet(%d)\n",rc, i );
          return L7_FAILURE;
      }
  }

  /* reading XFP count */
  xfpCount = sysapiHpcLocalXfpCountGet();
  if (xfpCount > L7_MAX_XFPS_PER_UNIT)
  {
    /* Need to increase L7_MAX_XFPS_PER_UNIT */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "XFP data truncated.");
  }

  boxsData->NumofXFPs = xfpCount;

  /* reading XFP status */
  for (i = 0; i < min(xfpCount, L7_MAX_XFPS_PER_UNIT); i++)
  {
      if ((rc = sysapiHpcLocalXfpDataGet(i, &boxsData->xfpStatusData[i])) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "Error %d on call to sysapiHpcLocalXfpDataGet(%d)\n",rc, i );
          return L7_FAILURE;
      }
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if temperature is within normal range
*
* @param    tempValue temperature
*
* @returns  L7_TRUE, L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL boxsTemperatureIsWithinRange(L7_int32 tempValue)
{
    return  (tempValue >= boxsCfg->cfg.globalCfgData.minTemp) &&
        (tempValue <= boxsCfg->cfg.globalCfgData.maxTemp);
}

/*********************************************************************
* @purpose  Check if temperature is below normal range
*
* @param    tempValue temperature
*
* @returns  L7_TRUE, L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL boxsTemperatureIsBelowRange(L7_int32 tempValue)
{
    return tempValue < boxsCfg->cfg.globalCfgData.minTemp;
}

/*********************************************************************
* @purpose  Check if temperature is above normal range
*
* @param    tempValue temperature
*
* @returns  L7_TRUE, L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL boxsTemperatureIsAboveRange(L7_int32 tempValue)
{
    return tempValue > boxsCfg->cfg.globalCfgData.maxTemp;
}

/*********************************************************************
* @purpose  Check if temperature is near lower bound of normal range
*
* @param    tempValue temperature
*
* @returns  L7_TRUE, L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL boxsTemperatureIsAroundLowerBoundary(L7_int32 tempValue)
{
    L7_int32 configMinTemp = boxsCfg->cfg.globalCfgData.minTemp;
    return  (tempValue >= (configMinTemp - (BOXS_TEMPERATURE_MARGIN_OUTSIDE) ))
        && (tempValue <= (configMinTemp + (BOXS_TEMPERATURE_MARGIN_INSIDE) ));
}

/*********************************************************************
* @purpose  Check if temperature is near upper bound of normal range
*
* @param    tempValue temperature
*
* @returns  L7_TRUE, L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL boxsTemperatureIsAroundUpperBoundary(L7_int32 tempValue)
{
    L7_int32 configMaxTemp = boxsCfg->cfg.globalCfgData.maxTemp;
    return  (tempValue >= (configMaxTemp - (BOXS_TEMPERATURE_MARGIN_INSIDE) ))
        && (tempValue <= (configMaxTemp + (BOXS_TEMPERATURE_MARGIN_OUTSIDE) ));
}

/*********************************************************************
* @purpose  Process temperature change event and sends a trap,
*           if necessary
*
* @param    unitNum         Unit number
* @param    itemNum         Temp sensor number
* @param    trapEvent       Temperature change event to be processed
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsIssueTemperatureChangeEvent(L7_uint32 unitNum, L7_uint32 itemNum, BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t trapEvent)
{
#if DEBUG_BOXS
    L7_char8 tempEventName[32];
#endif
    L7_RC_t rc = L7_FAILURE;

    if  (boxsCfg->cfg.globalCfgData.trapTempStatusEnabled != L7_ENABLE)
        return;

#if DEBUG_BOXS
    switch(trapEvent)
    {
    case BOXS_EVENT_TEMP_ABOVE_THRESHOLD:
        osapiStrncpySafe(tempEventName, "above threshold", sizeof(tempEventName));
        break;
    case BOXS_EVENT_TEMP_BELOW_THRESHOLD:
        osapiStrncpySafe(tempEventName, "below threshold", sizeof(tempEventName));
        break;
    case BOXS_EVENT_TEMP_WITHIN_NORMAL_RANGE:
        osapiStrncpySafe(tempEventName, "within range threshold", sizeof(tempEventName));
        break;
    default:
        osapiStrncpySafe(tempEventName, "<unknown>", sizeof(tempEventName));
        break;
    }
#endif
    /* Fastpath trap */
    if ( (rc = trapMgrTemperatureChange(unitNum, itemNum, trapEvent)) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send temperature change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
    /* Customer trap */
    if (trapEvent == BOXS_EVENT_TEMP_ABOVE_THRESHOLD)
    {
      if ( (rc = trapMgrEnvMonTemperatureRisingAlarmLogTrap(unitNum)) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                  "Failed to send temperature rising trap, errcode %d, unit %d item %d event %d\n",
                         rc,  unitNum, itemNum, trapEvent);
      }
    }
}

/*********************************************************************
* @purpose  Process fan status change event and sends a trap,
*           if necessary
*
* @param    unitNum         Unit number
* @param    itemNum         Temp sensor number
* @param    trapEvent       Fan status change event to be processed
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsIssueFanStateChangeEvent(L7_uint32 unitNum, L7_uint32 itemNum, BOXS_ITEM_EVENT_t trapEvent)
{
#if DEBUG_BOXS
    L7_char8 tempEventName[32];
#endif
    L7_RC_t rc = L7_FAILURE;

    if  (boxsCfg->cfg.globalCfgData.trapFanStatusEnabled != L7_ENABLE)
        return;

#if DEBUG_BOXS
    switch(trapEvent)
    {
    case BOXS_EVENT_INSERTION:
        osapiStrncpySafe(tempEventName, "insertion", sizeof(tempEventName));
        break;
    case BOXS_EVENT_REMOVAL:
        osapiStrncpySafe(tempEventName, "removal", sizeof(tempEventName));
        break;
    case BOXS_EVENT_BECOME_OPERATIONAL:
        osapiStrncpySafe(tempEventName, "operational", sizeof(tempEventName));
        break;
    case BOXS_EVENT_FAILURE:
        osapiStrncpySafe(tempEventName, "failure", sizeof(tempEventName));
        break;
    default:
        osapiStrncpySafe(tempEventName, "<unknown>", sizeof(tempEventName));
        break;
    }
#endif
    /* sending trap */
    /* Fastpath trap */
    if ( (rc = trapMgrFanStateChange(unitNum, itemNum, trapEvent)) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send fan state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
    /* Customer trap */
    if ( (rc = trapMgrEnvMonFanStateChangeLogTrap(unitNum, itemNum, (trapEvent == BOXS_EVENT_FAILURE) ? L7_FALSE : L7_TRUE)) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send fan state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
}

/*********************************************************************
* @purpose  Process power supply status change event and sends a trap,
*           if necessary
*
* @param    unitNum         Unit number
* @param    itemNum         Temp sensor number
* @param    trapEvent       Power supply status change event to be processed
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsIssuePowerSupplyStateChangeEvent(L7_uint32 unitNum, L7_uint32 itemNum, BOXS_ITEM_EVENT_t trapEvent)
{
#if DEBUG_BOXS
    L7_char8 tempEventName[32];
#endif
    L7_RC_t rc = L7_FAILURE;

    if  (boxsCfg->cfg.globalCfgData.trapPowSupplyStatusEnabled != L7_ENABLE)
        return;

#if DEBUG_BOXS
    switch(trapEvent)
    {
    case BOXS_EVENT_INSERTION:
        osapiStrncpySafe(tempEventName, "insertion", sizeof(tempEventName));
        break;
    case BOXS_EVENT_REMOVAL:
        osapiStrncpySafe(tempEventName, "removal", sizeof(tempEventName));
        break;
    case BOXS_EVENT_BECOME_OPERATIONAL:
        osapiStrncpySafe(tempEventName, "operational", sizeof(tempEventName));
        break;
    case BOXS_EVENT_FAILURE:
        osapiStrncpySafe(tempEventName, "failure", sizeof(tempEventName));
        break;
    default:
        osapiStrncpySafe(tempEventName, "<unknown>", sizeof(tempEventName));
        break;
    }
#endif

    /* sending trap */
    /* Fastpath trap */
    if ((rc = trapMgrPowSupplyStateChange(unitNum, itemNum, trapEvent)) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send power supply state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
    /* Customer trap */
    if ((rc = trapMgrEnvMonPowerSupplyStateChangeLogTrap(unitNum, itemNum, (trapEvent == BOXS_EVENT_FAILURE) ? L7_FALSE : L7_TRUE)) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send power supply state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
}

/*********************************************************************
* @purpose  Process SFP status change event and sends a trap,
*           if necessary
*
* @param    unitNum         Unit number
* @param    itemNum         SFP number
* @param    trapEvent       SFP status change event to be processed
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsIssueSFPStateChangeEvent(L7_uint32 unitNum, L7_uint32 itemNum, L7_uint32 port, BOXS_ITEM_EVENT_t trapEvent)
{
    L7_RC_t rc = L7_FAILURE;

    if  (boxsCfg->cfg.globalCfgData.trapSfpStatusEnabled != L7_ENABLE)
        return;

    /* sending trap */
    if ((rc = trapMgrSFPInsertionRemovalTrap(unitNum, port, ((trapEvent == BOXS_EVENT_INSERTION) ? 1 : 0))) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send SFP state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
}

/*********************************************************************
* @purpose  Process XFP status change event and sends a trap,
*           if necessary
*
* @param    unitNum         Unit number
* @param    itemNum         XFP number
* @param    trapEvent       XFP status change event to be processed
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsIssueXFPStateChangeEvent(L7_uint32 unitNum, L7_uint32 itemNum, L7_uint32 port, BOXS_ITEM_EVENT_t trapEvent)
{
    L7_RC_t rc = L7_FAILURE;

    if  (boxsCfg->cfg.globalCfgData.trapXfpStatusEnabled != L7_ENABLE)
        return;

    /* sending trap */
    if ((rc = trapMgrXFPInsertionRemovalTrap(unitNum, port, ((trapEvent == BOXS_EVENT_INSERTION) ? 1 : 0))) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Failed to send SFP+/XFP state change trap, errcode %d, unit %d item %d event %d\n",
                       rc,  unitNum, itemNum, trapEvent);
    }
}


/*********************************************************************
* @purpose  Analyse temperature sensor status change and returns
*           correspinding status change event, if any
*
* @param    currentData     Current sensor status data
* @param    newData         New  sensor status data
* @param    lastReportedEvent @b{(output)}      Last event reported.
*
*           Actual only if funcion returns valid event, this means that trap need to be sent
*           and last reported event needs to be saved (for checking temp. range boundary margin conditions)
*
* @returns  Temperature cross range event
*
* @notes    none
*
* @end
*********************************************************************/
BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t boxsProcessTemperatureChange(
    HPC_TEMP_SENSOR_DATA_t* currentData,
    HPC_TEMP_SENSOR_DATA_t* newData,
    BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t lastReportedEvent)
{
    BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t trapEvent = BOXS_EVENT_TEMP_NONE;
  L7_int32 newTemperature = newData->temperature;

    /* checking all movements */
    /* move 'below' from 'within' or 'above' or 'none' */
    if (boxsTemperatureIsBelowRange(newTemperature) && (lastReportedEvent == BOXS_EVENT_TEMP_WITHIN_NORMAL_RANGE ||
                                                      lastReportedEvent == BOXS_EVENT_TEMP_ABOVE_THRESHOLD ||
                                                      lastReportedEvent == BOXS_EVENT_TEMP_NONE))
    {
        /* check that new temperature value is not within lower margins  */
        if(!boxsTemperatureIsAroundLowerBoundary(newTemperature))
    {
            trapEvent = BOXS_EVENT_TEMP_BELOW_THRESHOLD;
    }
    }

    /* move 'above' from 'within' or 'below' or 'none' */
    if (boxsTemperatureIsAboveRange(newTemperature) && (lastReportedEvent == BOXS_EVENT_TEMP_WITHIN_NORMAL_RANGE ||
                                                      lastReportedEvent == BOXS_EVENT_TEMP_BELOW_THRESHOLD ||
                                                      lastReportedEvent == BOXS_EVENT_TEMP_NONE))
    {
    /* check that new temperature value is not within upper margins  */
        if(!boxsTemperatureIsAroundUpperBoundary(newTemperature))
    {
            trapEvent = BOXS_EVENT_TEMP_ABOVE_THRESHOLD;
    }
    }

    /* move 'within' from 'above' or 'below' ignore 'none' */
    if (boxsTemperatureIsWithinRange(newTemperature) && (lastReportedEvent == BOXS_EVENT_TEMP_ABOVE_THRESHOLD ||
                                                       lastReportedEvent == BOXS_EVENT_TEMP_BELOW_THRESHOLD))
    {
    /* check that new temperature value is not within upper or lower margins  */
        if(!boxsTemperatureIsAroundUpperBoundary(newTemperature) &&
       !boxsTemperatureIsAroundLowerBoundary(newTemperature))
    {
            trapEvent = BOXS_EVENT_TEMP_WITHIN_NORMAL_RANGE;
    }
    }

    return trapEvent;
}



/*********************************************************************
* @purpose  Analyse item (fan or power supply) status change and returns
*           correspinding status change event, if any
*
* @param    currentState    Current statue of item
* @param    newState        New  statue of item
* @param    item            Type of actual item being processed (fan or power supply)
*
* @returns  Event ID of item status change
*
* @notes    none
*
* @end
*********************************************************************/

BOXS_ITEM_EVENT_t boxsProcessItemState(HPC_BOXS_ITEM_STATE_t currentState, HPC_BOXS_ITEM_STATE_t newState, HPC_ITEM_t item)
{
    BOXS_ITEM_EVENT_t ret;
    ret = BOXS_EVENT_NONE;

#if DEBUG_BOXS
    if ( (currentState == HPC_ITEMSTATE_NONE) ||
         (newState == HPC_ITEMSTATE_NONE) )
    {
        sysapiPrintf("boxsProcessItemState: error currentstate %d newstate %d\n",currentState , newState);
    }
#endif
    if (currentState != newState)
    {
        if ( ((currentState == HPC_ITEMSTATE_OPERATIONAL) ||
              (currentState == HPC_ITEMSTATE_FAILED )) &&
             (newState == HPC_ITEMSTATE_NOT_PRESENT))
        {
          /* removal */
          ret = BOXS_EVENT_REMOVAL;
        } else if ( ((newState == HPC_ITEMSTATE_OPERATIONAL) ||
              (newState == HPC_ITEMSTATE_FAILED )) &&
             (currentState == HPC_ITEMSTATE_NOT_PRESENT))
        {
            /* insertion */
            ret = BOXS_EVENT_INSERTION;
        } else if ( (newState == HPC_ITEMSTATE_OPERATIONAL) &&
             (currentState == HPC_ITEMSTATE_FAILED) )
        {
            /* becoming operational */
            ret = BOXS_EVENT_BECOME_OPERATIONAL;
        } else if ( (newState == HPC_ITEMSTATE_FAILED) &&
             (currentState == HPC_ITEMSTATE_OPERATIONAL) )
        {
            /* failure */
            ret = BOXS_EVENT_FAILURE;
        }
    }
    return ret;
}


/*********************************************************************
* @purpose  Processing obtained box data (via HPC or read locally)
*
* @param    unitNum             Unit number
* @param    responseboxsData    Prt to box status data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsDataProcess(L7_uint32 unitNum, boxsData_t *responseboxsData)
{
  L7_uint32 i;
  BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t temperatureEvent;
  BOXS_ITEM_EVENT_t fanEvent;
  BOXS_ITEM_EVENT_t powSupplyEvent;
  BOXS_ITEM_EVENT_t sfpEvent;
  BOXS_ITEM_EVENT_t xfpEvent;
  static L7_BOOL bBoxsDataAvailable = L7_FALSE;

  lastGlobalStatus[unitNum] = globalStatus[unitNum];
  globalStatus[unitNum] = BOXS_GLOBAL_STATUS_OK;

  for (i = 0; i < min(responseboxsData->NumofPwMods, L7_MAX_PW_MODS_PER_UNIT); i++)
  {
    if (responseboxsData->PwModStatusData[i].itemState == HPC_ITEMSTATE_FAILED)
    {
      globalStatus[unitNum] = BOXS_GLOBAL_STATUS_NONCRITICAL;
      break;
    }
  }

  for (i = 0; i < min(responseboxsData->NumofFans, L7_MAX_FANS_PER_UNIT); i++)
  {
    if (responseboxsData->FanStatusData[i].itemState == HPC_ITEMSTATE_FAILED)
    {
      globalStatus[unitNum] = BOXS_GLOBAL_STATUS_CRITICAL;
      break;
    }
  }

  if (gBoxsData[unitNum].NumofFans == responseboxsData->NumofFans)
  {
    for (i = 0; i < min(gBoxsData[unitNum].NumofFans, L7_MAX_FANS_PER_UNIT); i++)
    {
      if ( ( fanEvent = boxsProcessItemState(
                  gBoxsData[unitNum].FanStatusData[i].itemState,
                  responseboxsData->FanStatusData[i].itemState,
                  HPC_ITEM_FAN)) != BOXS_EVENT_NONE )
      {
            boxsIssueFanStateChangeEvent(unitNum, i, fanEvent);
      }
    }
  }
  else
  {
      /* TODO this can happen, if slave (non-local) stack unit was replaced with another HW box ??
      This can be probably handled by HW recovery*/
  }

  if (gBoxsData[unitNum].NumofPwMods == responseboxsData->NumofPwMods)
  {
    for (i = 0; i < min(gBoxsData[unitNum].NumofPwMods, L7_MAX_PW_MODS_PER_UNIT); i++)
    {
        if ( ( powSupplyEvent = boxsProcessItemState(
            gBoxsData[unitNum].PwModStatusData[i].itemState,
            responseboxsData->PwModStatusData[i].itemState,
            HPC_ITEM_POWERSUPPLY)) != BOXS_EVENT_NONE )
        {
            boxsIssuePowerSupplyStateChangeEvent(unitNum, i, powSupplyEvent);
        }
    }
  }
  else
  {
     /* TODO this can happen, if slave (non-local) stack unit was replaced with another HW box ??*/
  }


  if (gBoxsData[unitNum].NumofTempSensors == responseboxsData->NumofTempSensors)
  {
    for (i = 0; i < min(gBoxsData[unitNum].NumofTempSensors, L7_MAX_TEMP_SENSORS_PER_UNIT); i++)
    {
        if ( (temperatureEvent =  boxsProcessTemperatureChange(
            &gBoxsData[unitNum].TempSensorStatusData[i],
            &responseboxsData->TempSensorStatusData[i],
            gBoxsData[unitNum].TempSensorLastReportedEvent[i]) ) != BOXS_EVENT_TEMP_NONE)
        {
        responseboxsData->TempSensorLastReportedEvent[i] = temperatureEvent;
            boxsIssueTemperatureChangeEvent(unitNum, i, temperatureEvent);
        }
      else
      {
        /* Update the last reported event */
        responseboxsData->TempSensorLastReportedEvent[i] = gBoxsData[unitNum].TempSensorLastReportedEvent[i];
      }
    }
  }
  else
  {
     /* TODO this can happen, if slave (non-local) stack unit was replaced with another HW box ??*/
  }

  if (gBoxsData[unitNum].NumofSFPs == responseboxsData->NumofSFPs)
  {
    for (i = 0; i < min(gBoxsData[unitNum].NumofSFPs, L7_MAX_SFPS_PER_UNIT); i++)
    {
        if ( (sfpEvent =  boxsProcessItemState(
            gBoxsData[unitNum].sfpStatusData[i].itemState,
            responseboxsData->sfpStatusData[i].itemState,
            HPC_ITEM_SFP) ) != BOXS_EVENT_NONE)
        {
            boxsIssueSFPStateChangeEvent(unitNum, i, responseboxsData->sfpStatusData[i].port, sfpEvent);
        }
    }
  }
  else
  {
     /* TODO this can happen, if slave (non-local) stack unit was replaced with another HW box ??*/
  }

  if (gBoxsData[unitNum].NumofXFPs == responseboxsData->NumofXFPs)
  {
    for (i = 0; i < min(gBoxsData[unitNum].NumofXFPs, L7_MAX_XFPS_PER_UNIT); i++)
    {
        if ( (xfpEvent =  boxsProcessItemState(
            gBoxsData[unitNum].xfpStatusData[i].itemState,
            responseboxsData->xfpStatusData[i].itemState,
            HPC_ITEM_XFP) ) != BOXS_EVENT_NONE)
        {
            boxsIssueXFPStateChangeEvent(unitNum, i, responseboxsData->xfpStatusData[i].port, xfpEvent);
        }
    }
  }
  else
  {
     /* TODO this can happen, if slave (non-local) stack unit was replaced with another HW box ??*/
  }

  /* save current status */
  memcpy(&gBoxsData[unitNum], responseboxsData, sizeof(boxsData_t));
  /* notifying that data is already available */
  if ( (bBoxsDataAvailable == L7_FALSE) &&  (boxsDataAvailableCallback != L7_NULLPTR) )
  {
    bBoxsDataAvailable = L7_TRUE;
    boxsDataAvailableCallback();
  }

  gBoxsData[unitNum].timestamp = osapiUpTimeRaw();

}

/*********************************************************************
* @purpose  Handles requests for Box Information
*
* @param    void
*
* @returns  void
*
* @notes    offloads HPC
*
* @end
*********************************************************************/
void boxsRspTask(void)
{
  boxsMsg_t msg, resp;
  L7_RC_t rc = L7_FAILURE;

  if (boxsReqQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "boxs Req Task: Failed to create");
    return;
  }

  while (1)
  {
    memset(&msg, 0, BOXS_MSG_SIZE);
    memset(&resp, 0, BOXS_MSG_SIZE);

    if ( (rc = osapiMessageReceive(boxsReqQ, &msg, BOXS_MSG_SIZE, L7_WAIT_FOREVER)) == L7_SUCCESS)
    {
        resp.hdr.boxsStatus = boxsLocalDataRead(&resp.boxsData);

        if (unitMgrNumberGet(&resp.hdr.unitNum) == L7_SUCCESS)
        {
          resp.hdr.msgType = L7_BOXS_RESPONSE;
          sysapiHpcMessageSend(L7_BOX_SERVICES_COMPONENT_ID, msg.hdr.unitNum, BOXS_MSG_SIZE, (L7_uchar8 *)&resp);
        }
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "boxsRspTask failed to receive message, rc = %d\n", rc );
        osapiSleep(10);
    }
  }
}

/*********************************************************************
* @purpose  Requests Box Information;
*           the puspose if this task is processing box services data periodically,
*           taking into account that our switch can become unit master and slave
*           box services data is read locally (if we're stack mananger),
*           or read from slaves via HPC
*
* @param    void
*
* @returns  void
*
* @notes    offloads HPC
*
* @end
*********************************************************************/
void boxsReqTask(void)
{
  L7_RC_t rc;
  L7_RC_t rc1, rc2;
  boxsMsg_t msg, resp;
  unitMgrStatus_t status;
  L7_uint32 unitNum, u, msgCnt;
  boxsData_t tempboxsData;

  if (boxsRspQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID, "boxs Req Task: Failed to create, no Response Queue");
    return;
  }

  osapiSleep(10);

  while (1)
  {
    osapiSleep(2);

    /*Following code means that we're waiting for becoming a management unit*/
    {
        /* only unitMgr request status */
        rc1 = unitMgrNumberGet(&u);
        rc2 = unitMgrMgrNumberGet(&unitNum);
        if ((rc1 != L7_SUCCESS) || (rc2 != L7_SUCCESS) || (u != unitNum))
        {
          /* not unitMgr */
          osapiSleep(10);
          continue;
        }
    }

    /* flush response queue, found possibility of left over responses lingering */
    while (osapiMessageReceive(boxsRspQ, &resp, BOXS_MSG_SIZE, L7_NO_WAIT) == L7_SUCCESS);

    /* some initialization, preparing ... */
    msgCnt = 0;
    /* build request */
    msg.hdr.unitNum = unitNum;
    msg.hdr.msgType = L7_BOXS_REQUEST;

    /* now we became a mgmt unit and obtaining box services data for the whole stack  --
      locally for our unit, via HPC from all the rest */
    {
        rc = unitMgrStackMemberGetFirst(&u);

        while (rc == L7_SUCCESS)
        {
          /* ignore preconfigured units */
          unitMgrUnitStatusGet(u, &status);
          if (status != L7_UNITMGR_UNIT_NOT_PRESENT)
          {
            if (u != unitNum) /* only use hpc for remote units */
            {
              sysapiHpcMessageSend(L7_BOX_SERVICES_COMPONENT_ID, u, BOXS_MSG_SIZE, (L7_uchar8 *)&msg);
              msgCnt++;
            }
            else
            {
              boxsLocalDataRead(&tempboxsData);
              boxsDataProcess(unitNum, &tempboxsData);
            }
          }
          rc = unitMgrStackMemberGetNext(u, &u);
        }

        /* now waiting for response from all other units and processing replies */
        while (msgCnt)
        {
          if (osapiMessageReceive(boxsRspQ, &resp, BOXS_MSG_SIZE, BOXS_CLIENT_TIMEOUT * 1000) != L7_SUCCESS)
          {
            msgCnt--;
            continue;
          }
          msgCnt--;
          /* process response */
          if (resp.hdr.boxsStatus == L7_SUCCESS)
          {
            boxsDataProcess(resp.hdr.unitNum, &resp.boxsData);
          }
        }

    }
  }
}

/*********************************************************************
* @purpose  Debug display of current box status data
*
* @notes    none
*
* @end
*********************************************************************/

void boxsDebugStatusGet()
{
  boxsData_t boxsData;
  int       index;

  boxsLocalDataRead(&boxsData);

  sysapiPrintf("-----------------------------\n");
  sysapiPrintf("Timestamp:            %d\n", boxsData.timestamp);
  sysapiPrintf("Number of fans:       %d\n", boxsData.NumofFans);
  sysapiPrintf("Fan status:           \n");
  for (index = 0; index < min(boxsData.NumofFans, L7_MAX_FANS_PER_UNIT); index++)
  {
    sysapiPrintf("[%d] type %d state %d fanspeed %d fanDuty %d\n",
           index,
           boxsData.FanStatusData[index].itemType,
           boxsData.FanStatusData[index].itemState,
           boxsData.FanStatusData[index].fanSpeed,
           boxsData.FanStatusData[index].fanDuty);
  }
  sysapiPrintf("\n");
  sysapiPrintf("Number of power mods: %d\n", boxsData.NumofPwMods);
  sysapiPrintf("Power Module status:  \n");
  for (index = 0; index < min(boxsData.NumofPwMods, L7_MAX_PW_MODS_PER_UNIT); index++)
  {
      sysapiPrintf("[%d] type %d state %d\n",
             index,
             boxsData.PwModStatusData[index].itemType,
             boxsData.PwModStatusData[index].itemState);
  }
  sysapiPrintf("\n");
  sysapiPrintf("Number of temp sensors: %d\n", boxsData.NumofTempSensors);
  sysapiPrintf("Temp sensor status:  \n");
  for (index = 0; index < min(boxsData.NumofTempSensors, L7_MAX_TEMP_SENSORS_PER_UNIT); index++)
  {
      sysapiPrintf("[%d] type %d state %d temperature %d\n",
             index,
             boxsData.TempSensorStatusData[index].itemType,
             boxsData.TempSensorStatusData[index].tempSensorState,
             boxsData.TempSensorStatusData[index].temperature);
  }
  sysapiPrintf("\n");
  sysapiPrintf("Number of SFPs: %d\n", boxsData.NumofSFPs);
  sysapiPrintf("SFP status:  \n");
  for (index = 0; index < min(boxsData.NumofSFPs, L7_MAX_SFPS_PER_UNIT); index++)
  {
      sysapiPrintf("[%d] port %d type %d state %d\n",
             index,
             boxsData.sfpStatusData[index].port,
             boxsData.sfpStatusData[index].itemType,
             boxsData.sfpStatusData[index].itemState);
  }
  sysapiPrintf("\n");
  sysapiPrintf("Number of XFPs: %d\n", boxsData.NumofXFPs);
  sysapiPrintf("XFP status:  \n");
  for (index = 0; index < min(boxsData.NumofXFPs, L7_MAX_XFPS_PER_UNIT); index++)
  {
      sysapiPrintf("[%d] port %d type %d state %d\n",
             index,
             boxsData.xfpStatusData[index].port,
             boxsData.xfpStatusData[index].itemType,
             boxsData.xfpStatusData[index].itemState);
  }
}


