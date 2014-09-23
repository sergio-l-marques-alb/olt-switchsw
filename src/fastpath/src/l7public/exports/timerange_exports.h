/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange_exports.h
*
* @purpose  Defines constants and  definitions that are shared by Management 
*           and the application
*
* @component
*
* @comments
*
*
* @Notes
*
* @created 11/09/2009
*
* @author Siva Mannem
* @end
*
**********************************************************************/
#ifndef __TIMERANGE_EXPORTS_H_
#define __TIMERANGE_EXPORTS_H_

#include "defaultconfig.h"
 
#define L7_TIMERANGE_MAX_NUM                     100
#define L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE   10

/* range of time range entry numbers */
#define L7_TIMERANGE_MIN_RULE_NUM                1
#define L7_TIMERANGE_MAX_RULE_NUM                L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE

#define L7_TIMERANGE_MIN_INDEX                   1
#define L7_TIMERANGE_MAX_INDEX                   (L7_TIMERANGE_MIN_INDEX + L7_TIMERANGE_MAX_NUM - 1)

#define L7_TIMERANGE_NAME_LEN_MIN                 1   /* does not include trailing '\0' */
#define L7_TIMERANGE_NAME_LEN_MAX                31   /* does not include trailing '\0' */

#define TIMERANGE_MSG_COUNT                     (L7_TIMERANGE_MAX_NUM * \
                                                 L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE) 

#define TIMERANGE_BASE_TIMEOUT                   (1 * 1000)

#define L7_TIMERANGE_MIN_YEAR                     1993
#define L7_TIMERANGE_MAX_YEAR                     2035

/*Status of the time range*/

typedef enum {
  TIMERANGE_STATUS_ACTIVE = 1,
  TIMERANGE_STATUS_INACTIVE,
  TIMERANGE_STATUS_NON_EXISTENT,
  TIMERANGE_STATUS_LAST 
} timeRangeStatus_t;

/*Time range event notification types*/

typedef enum {
  TIMERANGE_EVENT_START = 1,
  TIMERANGE_EVENT_END,
  TIMERANGE_EVENT_DELETE,
  TIMERANGE_EVENT_LAST
} timeRangeEvent_t;

/*Time range entry types*/
typedef enum {
  TIMERANGE_ABSOLUTE_ENTRY = 0,
  TIMERANGE_PERIODIC_ENTRY,
  TIMERANGE_ENTRY_TYPE_TOTAL   /* total number of entry types */
} timeRangeEntryType_t;

/*Time range entry fields*/
typedef enum {
  TIMERANGE_ENTRY_TYPE = 1,
  TIMERANGE_ENTRY_START_TIME,
  TIMERANGE_ENTRY_END_TIME,
  TIMERANGE_ENTRY_FIELDS_TOTAL
} timeRangeEntryFields_t;

/*Time range entry's absolute date*/
typedef struct timeRangeEntryabsoluteDate_s {
  L7_uchar8  day;     /*day  : 1-31  */
  L7_uchar8  month;   /*month: 1-12  */
  L7_uint32  year;    /*year*/
} timeRangeEntryabsoluteDate_t;

/*Time range entry's periodic day or days*/
typedef struct timeRangeEntryPeriodicDate_s {
  L7_uchar8 dayMask;
} timeRangeEntryPeriodicDay_t;

typedef union timerangeEntryDateParms_s {
  /* absolute entry date */
  timeRangeEntryabsoluteDate_t  absoluteDate;

  /* Periodic entry day or days*/
  timeRangeEntryPeriodicDay_t   daysOfTheWeek;  

} timeRangeEntryDateParms_t;

/*Time range entry's date and time*/
typedef struct timerangeEntryDateAndTimeParms_s {
  timeRangeEntryDateParms_t  date;  
  L7_uchar8                  hour;
  L7_uchar8                  minute;
} timeRangeEntryDateAndTimeParms_t;

#ifdef INCLUDE_TIMERANGE_EXPORTS_OVERRIDES
#include "timerange_exports_overrides.h"
#endif

#endif
