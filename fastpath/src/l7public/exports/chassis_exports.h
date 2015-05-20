/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename chassis_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __CHASSIS_EXPORTS_H_
#define __CHASSIS_EXPORTS_H_


/*-------------------------------------------------------------*/
/* Chassis  Constants                                          */
/*-------------------------------------------------------------*/

/* Power Module and Cooling Unit Constants*/
typedef enum
{
  L7_MODULE_PRESENT = 0,  /* Module is Plugged-in */
  L7_MODULE_HEALTHY,      /* Module is Healthy */
  L7_MODULE_FAILED,       /* Module is faulty  */
  L7_MODULE_NOT_PRESENT   /* Module is not present */
} L7_MODULE_HEALTH_STATUS_t;

typedef enum
{
  L7_MODULE_ALARM_PRESENT = 0, /* Alarm is Present */
  L7_MODULE_ALARM_NOT_PRESENT  /* Alarm is not present */
} L7_MODULE_ALARM_STATUS_t;

typedef enum
{
  L7_MODULE_PARAM_SLOT_NUMBER = 0,   /* Module slot number */
  L7_MODULE_PARAM_HEALTH_STATUS,     /* Module health status */
  L7_MODULE_PARAM_ALARM_STATUS       /* Alarm presence against the module */
} L7_MODULE_DATABASE_PARAMS_t;



/******************** conditional Override *****************************/

#ifdef INCLUDE_CHASSIS_EXPORTS_OVERRIDES
#include "chassis_exports_overrides.h"
#endif

#endif /* __CHASSIS_EXPORTS_H_*/
