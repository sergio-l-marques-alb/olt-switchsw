/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename poe_exports.h
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
* @created 03/26/2008
*
* @author akulkarn
* @end
*
**********************************************************************/

#ifndef __POE_EXPORTS_H_
#define __POE_EXPORTS_H_

/**********************************************************************/
/*************              PoE defines                       *********/
/**********************************************************************/

/* The unit for Power Limit is in WATTS. If the unit on a certain driver controller is required to be in anything other than Watts, 
 * please make that change in UI or the driver controller, not here. DO NOT CHANGE THESE VALUES WITHOUT THE CONSENT OF A POE EXPERT. */

#define L7_POE_PORT_LIMIT_MAX 32000
#define L7_POE_PORT_LIMIT_MIN 3000
#define L7_POE_USAGETHRESHOLD_MIN  1
#define L7_POE_USAGETHRESHOLD_MAX  99

#define POE_TYPE_STRING_LEN      20

/* POE Component Feature List */
typedef enum
{
  L7_POE_FEATURE_ID = 1,                      /* For checking if any unit supports PoE */
  L7_POE_FEATURE_SYSTEM_AUTO_RESET_MODE,
  L7_POE_FEATURE_SYSTEM_LOGICAL_PORT_MAP_MODE,
  L7_POE_FEATURE_SYSTEM_NOTIFICATION_CONTROL,
  L7_POE_FEATURE_PORT_DETECTION_MODE,
  L7_POE_FEATURE_PORT_HIGH_POWER_MODE,
  L7_POE_FEATURE_PORT_VIOLATION_TYPE,
  L7_POE_FEATURE_PORT_DISCONNECT_TYPE,
  L7_POE_FEATURE_ID_TOTAL                     /* total number of enum values */
} L7_POE_FEATURE_IDS_t;

typedef enum
{
  L7_POE_PORT_POWER_PAIRS_INVALID = 0,
  L7_POE_PORT_POWER_PAIRS_SIGNAL = 1, /* Alternative A */
  L7_POE_PORT_POWER_PAIRS_SPARE  = 2  /* Alternative B */

} L7_POE_PORT_POWER_PAIRS_t;

typedef enum
{
  L7_POE_PRIORITY_INVALID = 0,
  L7_POE_PRIORITY_LOW,
  L7_POE_PRIORITY_MEDIUM,
  L7_POE_PRIORITY_HIGH,
  L7_POE_PRIORITY_CRITICAL,

  L7_POE_PRIORITY_COUNT
} L7_POE_PRIORITY_t;

typedef enum
{
  L7_POE_LIMIT_TYPE_INVALID = 0,
  L7_POE_LIMIT_TYPE_DOT3AF,
  L7_POE_LIMIT_TYPE_USER,
  L7_POE_LIMIT_TYPE_NONE,

  L7_POE_LIMIT_TYPE_COUNT
} L7_POE_LIMIT_TYPE_t;

typedef enum
{
  L7_POE_POWER_MGMT_INVALID = 0,
  L7_POE_POWER_MGMT_STATIC,
  L7_POE_POWER_MGMT_DYNAMIC,
  L7_POE_POWER_MGMT_CLASS,
  L7_POE_POWER_MGMT_STATIC_PRI,
  L7_POE_POWER_MGMT_DYNAMIC_PRI,
  L7_POE_POWER_MGMT_NONE,

  L7_POE_POWER_MGMT_COUNT
} L7_POE_POWER_MGMT_t;

typedef enum
{
  L7_POE_DETECTION_INVALID = 0,
  L7_POE_DETECTION_LEGACY,
  L7_POE_DETECTION_4PT_DOT3AF,
  L7_POE_DETECTION_4PT_DOT3AF_LEG,
  L7_POE_DETECTION_2PT_DOT3AF,
  L7_POE_DETECTION_2PT_DOT3AF_LEG,
  L7_POE_DETECTION_NONE,

  L7_POE_DETECTION_COUNT
} L7_POE_DETECTION_t;

typedef enum
{
  L7_POE_PORT_POWER_INVALID = 0,
  L7_POE_PORT_POWER_CLASS0,
  L7_POE_PORT_POWER_CLASS1,
  L7_POE_PORT_POWER_CLASS2,
  L7_POE_PORT_POWER_CLASS3,
  L7_POE_PORT_POWER_CLASS4

} L7_POE_PORT_POWER_CLASS_t;

typedef enum
{
  L7_POE_PORT_DETECTION_STATUS_DISABLED = 0,
  L7_POE_PORT_DETECTION_STATUS_SEARCHING,
  L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER,
  L7_POE_PORT_DETECTION_STATUS_TEST,
  L7_POE_PORT_DETECTION_STATUS_FAULT,
  L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT,
  L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER,
  L7_POE_PORT_DETECTION_STATUS_OVERLOAD
} L7_POE_PORT_DETECTION_STATUS_t;

typedef enum {
  L7_POE_DEVICE_TYPE_PSE = 0 ,
  L7_POE_DEVICE_TYPE_PD,

  L7_POE_DEVICE_TYPE_COUNT
} L7_POE_DEVICE_TYPE_t;

typedef enum {
  L7_POE_PSE_SOURCE_UNKNOWN = 0,
  L7_POE_PSE_SOURCE_PRIMARY,
  L7_POE_PSE_SOURCE_BACKUP,

  L7_POE_PSE_SOURCE_COUNT
} L7_POE_PSE_SOURCE_t;

typedef enum {
  L7_POE_PD_SOURCE_UNKNOWN = 0,
  L7_POE_PD_SOURCE_FROM_PSE,
  L7_POE_PD_SOURCE_LOCAL,
  L7_POE_PD_SOURCE_LOCAL_AND_PSE,

  L7_POE_PD_SOURCE_COUNT
} L7_POE_PD_SOURCE_t;

typedef enum {
  L7_POE_ACK_IGNORE = 0,
  L7_POE_ACK_YES,
  L7_POE_ACK_NO,

  L7_POE_ACK_COUNT
} L7_POE_ACK_t;

typedef enum
{
  L7_POE_ERROR_NONE = 0,
  L7_POE_ERROR_MPS_ABSENT,
  L7_POE_ERROR_SHORT,
  L7_POE_ERROR_OVERLOAD,
  L7_POE_ERROR_POWER_DENIED

} L7_POE_ERROR_TYPE_t;

typedef enum
{
  L7_POE_SYSTEM_OPER_STATUS_ON = 1,
  L7_POE_SYSTEM_OPER_STATUS_OFF,
  L7_POE_SYSTEM_OPER_STATUS_FAULTY

} L7_POE_SYSTEM_OPER_STATUS_t;

typedef enum
{
  L7_POE_PORT_ON = 1,
  L7_POE_PORT_OFF
} L7_POE_TRAP_PORT_CHANGE_t;

typedef enum
{
  L7_POE_THRESHOLD_ABOVE = 1,
  L7_POE_THRESHOLD_BELOW
} L7_POE_TRAP_MAIN_THRESHOLD_t;

typedef enum {
  L7_POE_POWER_DISC_NONE = 0,
  L7_POE_POWER_DISC_AC,
  L7_POE_POWER_DISC_DC,

  L7_POE_POWER_DISC_COUNT
} L7_POE_POWER_DISC_t;

#define LEXICAL_POE_CLASS0          0
#define LEXICAL_POE_CLASS1          1
#define LEXICAL_POE_CLASS2          2
#define LEXICAL_POE_CLASS3          3
#define LEXICAL_POE_CLASS4          4

/******************** conditional Override *****************************/

#ifdef INCLUDE_POE_EXPORTS_OVERRIDES
#include "poe_exports_overrides.h"
#endif

#endif /* __POE_EXPORTS_H_*/
