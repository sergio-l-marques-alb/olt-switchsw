
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_powerunitmgr_api.h
*
* @purpose Usmdb externs for Power Unit Manager
*
* @component Power Unit Manager
*
* @comments none
*
* @create 08/17/2005
*
* @author msiva
* 
* @end
*
******************************************************************************/

#ifndef USMDB_POWER_UNIT_MANAGER_API_H
#define USMDB_POWER_UNIT_MANAGER_API_H

#include "chassis_alarmmgr_api.h"
#include "l7_common.h"
#include "sysapi_hpc.h"


/******************************************************************************
*
* @purpose Get the handle to first entry in the power module database.
*
* @param  L7_uint32 pHandle    Pointer to a handle used to identify 
*                              a particular power module database
*
* @returns L7_SUCCESS,     if success
* @returns L7_FAILURE,     if parameters are invalid or other error
*
* @end
* 
******************************************************************************/
L7_RC_t usmDbPowerModuleStatusFirstGet(L7_uint32 *pHandle);

/******************************************************************************
*
* @purpose Get the handle to next entry in the power module database.
*
* @param  L7_uint32 handle       Unique identifier to identify a database
*                                for a particular power module
* @param  L7_uint32 *pNextHandle Pointer to next handle.
*
* @returns L7_SUCCESS,     if success
* @returns L7_FAILURE,     if parameters are invalid or other error
* @returns L7_NOT_EXIST,   if we reached the end of the module database.
*
* @end
* 
******************************************************************************/
L7_RC_t usmDbPowerModuleStatusNextGet(L7_uint32 handle, 
                                      L7_uint32 *pNextHandle);

/******************************************************************************
*
* @purpose Get the handle to a power module database entry,
*          identified by the slot number.
*
* @param  L7_uint32 slotNuml     Slot number of a power module
* @param  L7_uint32 *pHandle     Pointer to a handle.
*
* @returns L7_SUCCESS,     if success
* @returns L7_FAILURE,     if parameters are invalid or other error
*
* @end
*
******************************************************************************/
L7_RC_t usmDbPowerModuleStatusSlotGet(L7_uint32 slotNum, 
                                      L7_uint32 *pHandle);

/******************************************************************************
*
* @purpose Get the slot number of a power module identified by the handle
*
* @param  L7_uint32 handle     Unique identifier to identify a database
*                              for a particular power module
* @param  pSlotNum *pSlotNum   Pointer to Slot number
*
* @returns L7_SUCCESS,         if success
* @returns L7_FAILURE,         if parameters are invalid or other error
*
* @end
* 
******************************************************************************/
L7_RC_t usmDbPowerModuleSlotNumGet(L7_uint32 handle, L7_uint32 *pSlotNum);

/******************************************************************************
*
* @purpose Get the health status of a power module identified by the handle.
*
* @param  L7_uint32 handle     Unique identifier to identify a database
*                              for a particular power module
* @param  L7_MODULE_HEALTH_STATUS_t *pHealthStatus   Pointer to health 
*                                                    status of a module
*
* @returns L7_SUCCESS,         if success
* @returns L7_FAILURE,         if parameters are invalid or other error
*
* @end
* 
******************************************************************************/
L7_RC_t usmDbPowerModuleHealthStatusGet(L7_uint32 handle, 
                                    L7_MODULE_HEALTH_STATUS_t *pHealthStatus);

/******************************************************************************
*
* @purpose Get the alarm status of a power module identified by the handle.
*
* @param  L7_uint32 handle     Unique identifier to identify a database
*                              for a particular power module
* @param  L7_MODULE_ALARM_STATUS_t *pAlarmStatus  Pointer to 
*                                                 alarm status of a module
*
* @returns L7_SUCCESS,         if success
* @returns L7_FAILURE,         if parameters are invalid or other error
*
* @end
* 
******************************************************************************/
L7_RC_t usmDbPowerModuleAlarmStatusGet(L7_uint32 handle, 
                                       L7_MODULE_ALARM_STATUS_t *pAlarmStatus);

#endif /* USMDB_POWER_UNIT_MANAGER_API_H */

