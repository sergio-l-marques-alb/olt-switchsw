
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2008
*
**********************************************************************
*
* @filename timezone_exports.h
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
* @created 07/29/2008
*
* @author cpverne    
* @end
*
**********************************************************************/

#ifndef __TIMEZONE_EXPORTS_H_
#define __TIMEZONE_EXPORTS_H_

/******************************************************************/
/*************       Start TIMEZONE types and defines *********/
/******************************************************************/

#define L7_HOURS_OFFSET_MIN     -12
#define L7_HOURS_OFFSET_MAX     13
#define L7_MINUTES_OFFSET_MIN   0
#define L7_MINUTES_OFFSET_MAX   59
#define L7_ZONE_LEN_MIN         0
#define L7_ZONE_LEN_MAX         5
#define L7_OFFSET_MIN           1
#define L7_OFFSET_MAX           1440
#define L7_YEAR_START           2000
#define L7_YEAR_END             2097
#define L7_FORM_SUMMER_TIME_DATE      0
#define L7_FORM_SUMMER_TIME_RECURRING 1

typedef enum
{
  L7_LOCATION_USA = 1,
  L7_LOCATION_EU,
  L7_LOCATION_NONE
} L7_SUMMER_TIME_LOCATION_t;

typedef enum
{
  L7_WEEK_1 = 1,
  L7_WEEK_2,
  L7_WEEK_3,
  L7_WEEK_4,
  L7_WEEK_5
} L7_SUMMER_TIME_WEEK_t;

typedef enum
{
  L7_DAY_SUN = 1,
  L7_DAY_MON,
  L7_DAY_TUE,
  L7_DAY_WED,
  L7_DAY_THU,
  L7_DAY_FRI,
  L7_DAY_SAT
} L7_SUMMER_TIME_DAY_t;

typedef enum
{
  L7_JAN = 1,
  L7_FEB,
  L7_MAR,
  L7_APR,
  L7_MAY,
  L7_JUN,
  L7_JUL,
  L7_AUG,
  L7_SEP,
  L7_OCT,
  L7_NOV,
  L7_DEC
} L7_SUMMER_TIME_MONTH_t;


#define L7_TIME_SOURCE_NONE             0
#define L7_TIME_SOURCE_SNTP             1

#define L7_MINS_PER_HOUR                60

#define L7_TIMEZONE_ACRONYM_LEN         4
#define L7_TIMEZONE_HOURS_MIN           (-12)
#define L7_TIMEZONE_HOURS_MAX           13
#define L7_TIMEZONE_MINS_MIN            0
#define L7_TIMEZONE_MINS_MAX            59

#define L7_TIMEZONE_UHOURS_MIN          0
#define L7_TIMEZONE_UHOURS_MAX          23

#define L7_START_SUMMER_TIME_YEAR       2000
#define L7_END_SUMMER_TIME_YEAR         2097
#define L7_DEFAULT_SUMMERTIME_OFFSET    (60)
#define L7_SUMMER_TIME_OFFSET_MIN       1
#define L7_SUMMER_TIME_OFFSET_MAX       1440

#define L7_SUMMER_TIME_DAY              1

#define L7_SUMMER_TIME_USA_START_MONTH  3
#define L7_SUMMER_TIME_USA_START_WEEK   2
#define L7_SUMMERTIME_USA_HOUR          2

#define L7_SUMMER_TIME_USA_END_MONTH    11
#define L7_SUMMER_TIME_USA_END_WEEK     1

#define L7_SUMMER_TIME_EU_START_MONTH   3
#define L7_SUMMER_TIME_EU_START_WEEK    5
#define L7_SUMMERTIME_EU_START_HOUR     2

#define L7_SUMMER_TIME_EU_END_MONTH     10
#define L7_SUMMER_TIME_EU_END_WEEK      5
#define L7_SUMMERTIME_EU_END_HOUR       3

#define L7_NO_SUMMER_TIME               0
#define L7_SUMMER_TIME_RECURRING        1
#define L7_SUMMER_TIME_RECURRING_EU     2
#define L7_SUMMER_TIME_RECURRING_USA    3
#define L7_SUMMER_TIME_NON_RECURRING    4

#define L7_MONTHS_IN_YEAR               12
#define L7_DAYS_IN_MONTH                31
#define L7_LEAP_YEAR_DAY                29
#define L7_MONTH_FEB                    2

#define L7_LEAP_YEAR_CHECK(x)        ((((x % 4) == 0) && ((x % 100) != 0)) || ((x % 400) == 0)) ? 1 : 0

#define L7_HOURS_OFFSET                 1
#define L7_MINUTES_OFFSET               2
#define L7_ZONE_ACRONYM                 3
#define L7_START_DAY                    4
#define L7_START_MONTH                  5
#define L7_START_YEAR                   6
#define L7_START_WEEK                   7
#define L7_START_HOUR                   8
#define L7_START_MINUTE                 9
#define L7_END_DAY                      10
#define L7_END_MONTH                    11
#define L7_END_WEEK                     12
#define L7_END_YEAR                     13
#define L7_END_HOUR                     14
#define L7_END_MINUTE                   15
#define L7_OFFSET_MINUTES               16
#define L7_MODE                         17
#define L7_START_TIME                   18
#define L7_END_TIME                     19

#define L7_WEEK_CNT_MIN                 1
#define L7_WEEK_CNT_MAX                 5

#define L7_WEEK_DAY_CNT_MIN             1
#define L7_WEEK_DAY_CNT_MAX             7

#define L7_MONTH_CNT_MIN                1
#define L7_MONTH_CNT_MAX                12

/******************** conditional Override *****************************/

#ifdef INCLUDE_TIMEZONE_EXPORTS_OVERRIDES
#include "timezone_exports_overrides.h"
#endif

#endif /* __TIMEZONE_EXPORTS_H_*/
