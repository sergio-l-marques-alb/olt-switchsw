
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  chassis_commdefs.h
*
* @purpose   Common defines, enums and data structures for chassis 
*            specific offering
*
* @component 
*
* @comments  none
*
* @create    9/15/2006
*
* @author    asingh
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef CHASSIS_COMMDEFS_H
#define CHASSIS_COMMDEFS_H

/*-------------------------------------------------------------*/
/* Alarm Manager Enums which are required for UI and Alarm Mgr */
/*-------------------------------------------------------------*/

/* The following enum lists the possible sources for alarm  raise*/
typedef enum
{
  L7_ALARM_SOURCE_ACTIVE_CM,
  L7_ALARM_SOURCE_STANDBY_CM,    
  L7_ALARM_SOURCE_LM0,
  L7_ALARM_SOURCE_LM1,
  L7_ALARM_SOURCE_LM2,
  L7_ALARM_SOURCE_LM3,
  L7_ALARM_SOURCE_PM0,
  L7_ALARM_SOURCE_PM1,
  L7_ALARM_SOURCE_FM0,
  L7_ALARM_SOURCE_FM1,
} L7_ALARM_SOURCE_t;

/* The following enum lists the alarm catergories*/
typedef enum
{  
  L7_ALARM_CATEGORY_EQUIPMENT_FAILURE = 1, /* Equipment failures */ 
  L7_ALARM_CATEGORY_FAN_FAIL_OR_UNPLUGGED, /* Fan failed/unplugged*/
  L7_ALARM_CATEGORY_SOFTWARE_BOOT_FAILURE, /*Board ID mismatch */
  L7_ALARM_CATEGORY_DATA_PATH_INTEGRITY, /*Download failures */
  L7_ALARM_CATEGORY_PROCESSING_ERROR    /* SW processing failures */  
} L7_ALARM_CATEGORY_t;

/* Alarm Attribute - Severity */
typedef enum
{
  L7_ALARM_SEVERITY_UNDETERMINED = 0,
   
  /* The system is usable and is used to provide information/suggestion 
   * to the customer.
   */
  L7_ALARM_SEVERITY_NOTIFICATION = 1,
   
  /* The system is usable but is not functioning in accordance with 
   * specifications, and the error condition has no substantial impact 
   * on the customer’s operation.
   */
  L7_ALARM_SEVERITY_MINOR = 2,

  /* The system is usable, but an essential component of the system is 
   * malfunctioning and substantially impacts the customer’s operation.
   */
  L7_ALARM_SEVERITY_MAJOR = 3,

  /* The system is inoperable and the inability to use the system has a 
   * critical impact on the customer's operation
   */
  L7_ALARM_SEVERITY_CRITICAL = 4
} L7_ALARM_SEVERITY_t;

/* This specify whether the considered alarm is service affecting or 
 * notService Affect. This is used by system. 
 */
typedef enum
{
  L7_ALARM_SERVICE_AFFECTING = 1, 
  L7_ALARM_SERVICE_NOT_AFFECTING = 2,
} L7_ALARM_SERVICE_AFFECT_t;

/* Used by the administrator to enable/disable the reporting of the 
 * considered alarm.
 */
typedef enum
{
  L7_ALARM_REPORT_ENABLED = 1,
  L7_ALARM_REPORT_DISABLED = 2
} L7_ALARM_REPORT_MODE_t;

/* Used by the administrator to enable/disable the logging mode of the 
 * considered alarm.
 */
typedef enum
{
  L7_ALARM_LOGGING_ENABLED = 1,
  L7_ALARM_LOGGING_DISABLED = 2
} L7_ALARM_LOG_MODE_t;

/* This specify whether the specified alarm is active or cleared.*/
typedef enum
{
  L7_ALARM_STATUS_CLEARED = 0,
  L7_ALARM_STATUS_ACTIVE
} L7_ALARM_STATUS_t;

/* This specify when Log tables are FULL, what are the action needs to take. */
typedef enum
{
  /* Future use */
  L7_ALARM_LOG_FULL_WRAP = 1, /* Wrap to first location and overwrite */
  L7_ALARM_LOG_FULL_HALT = 2  /* Halt and return failure */
}L7_ALARM_LOG_FULL_ACTION_t;

/* This defines Def table Ids. If some one adds new Def table, it requires to 
 * add entry in this enum.
 */ 
typedef enum
{
  L7_ALARM_DEFINITION_CHASSIS_TABLE=0   /* Chassis Defintion table */
} L7_ALARM_DEFINITION_TABLE_t;

#endif  /* CHASSIS_COMMDEFS_H */
