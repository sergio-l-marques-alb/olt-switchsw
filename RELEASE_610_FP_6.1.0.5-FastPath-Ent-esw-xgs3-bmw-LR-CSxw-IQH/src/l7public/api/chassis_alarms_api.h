/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
*@filename    chassis_alarm_api.h
*
*@purpose     Externs for Chassis Alarms
*
*@component   Chassis Alarms
*
*@comments    none
*
*@create      05/10/2005
*
*@author      ytramanjaneyulu
*@end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef CHASSIS_ALARM_API_H
#define CHASSIS_ALARM_API_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_cnfgr_api.h"

typedef enum
{
    L7_ALARM_DEF_CHASSIS_PARAM_TYPE = 0,    /* Chassis alarm Type */
    L7_ALARM_DEF_CHASSIS_PARAM_SOURCE,    /* Chassis alarm Source */
    L7_ALARM_DEF_CHASSIS_PARAM_CATEGORY,    /* Chassis alarm Category */
    L7_ALARM_DEF_CHASSIS_PARAM_SEVERITY,       /* Chassis alarm Severity */
    L7_ALARM_DEF_CHASSIS_PARAM_SERVICE,    /* Chassis alarm Service affects */
    L7_ALARM_DEF_CHASSIS_PARAM_REPORT_MODE,    /* Chassis alarm Report mode */
    L7_ALARM_DEF_CHASSIS_PARAM_LOG_MODE,       /* Chassis alarm Log mode */
    L7_ALARM_DEF_CHASSIS_PARAM_DESC,    /* Chassis alarm Description */
}L7_ALARM_DEF_CHASSIS_TABLE_PARAMS_t;


/*********************************************************************
*
* @purpose  Handles messages from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request 
*                                   from the configurator
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
extern void chassisAlarmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif



