/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   sim.c
*
* @purpose    System Interface Manager Main File
*
* @component  sim
*
* @comments
*
* @create     08/21/2000
*
* @author     bmutz
*
* @end
*
**********************************************************************/

#include <string.h>
#include <time.h>
#include "l7_common.h"
#include "usmdb_timezone_api.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "l7_cnfgr_api.h"
#include "l7_ip6_api.h"
#include "nimapi.h"
#include "simapi.h"
#include "bspapi.h"
#include "sim.h"
#include "sysapi.h"
#include "l7_product.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "dtlapi.h"
#include "fdb_api.h"
#include "dot1s_api.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"
#include "ipstk_api.h"
#include "osapi_support.h"
#include "dhcp_bootp_api.h"
#include "dhcp6c_api.h"

#if L7_FEAT_DNI8541_BLADESERVER
#include "bspapi_blade.h"
#endif

simCfgData_t      simCfgData;
simTransferInfo_t simTransferInfo;
L7_uint32         systemCurrentConfigMode;
L7_uint32         servPortCurrentConfigMode;
L7_BOOL           transferInProgress;
L7_BOOL           suspendMgmtAccess;
void             *transferContext;
simAddrChangeNotifyList_t simAddrChangeNotifyList[SIM_ADDR_CHANGE_REGISTRATIONS_MAX];

#define          NUM_IPV6_ADDRS_FROM_STACK       L7_RTR6_MAX_INTF_ADDRS*2

extern simRouteStorage_t  * servPortRoutingProtRoute;
extern simRouteStorage_t  * netPortRoutingProtRoute;

static transferCompletionHandlerFnPtr transferCompletionHandler = L7_NULLPTR;

static void simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_t  addrType,
                                     L7_uchar8               addrFamily);
/*********************************************************************
* @purpose  Get the Unit's system name
*
* @param    *name  @b{(output)} system name, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemName(L7_char8 *name)
{
  memcpy(name, simCfgData.systemName, L7_SYS_SIZE);
}

/*********************************************************************
* @purpose  Sets the Unit's System Name
*
* @param    *name  @b{(input)} system name, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemName(L7_char8 *name)
{
  memset(( void * )&simCfgData.systemName, 0, L7_SYS_SIZE);

  if (strlen(name) < L7_SYS_SIZE)
    strcpy( simCfgData.systemName, name );
  else
    strncpy( simCfgData.systemName, name, (L7_SYS_SIZE-1) );

  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
*
* @purpose  Set the time zone minutes parameter
*
* @param    minutes       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneOffsetSet(L7_int32 minutes)
{
  if (minutes < (0 - (L7_TIMEZONE_HOURS_MAX * L7_MINS_PER_HOUR)) || minutes > (L7_TIMEZONE_HOURS_MAX * L7_MINS_PER_HOUR))
  {
    return L7_FAILURE;
  }

  simCfgData.systemTimeZone.minutes = minutes;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the time zone acronym parameter
*
* @param    acronym       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneAcronymSet(L7_char8 *acronym)
{
  if (strlen(acronym) > L7_TIMEZONE_ACRONYM_LEN)
    return L7_FAILURE;

  osapiStrncpy(simCfgData.systemTimeZone.tzName, acronym, sizeof(simCfgData.systemTimeZone.tzName));
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the time zone minutes parameter
*
* @param    minutes       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneOffsetGet(L7_int32 *minutes)
{
  *minutes = simCfgData.systemTimeZone.minutes;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the time zone acronym parameter
*
* @param    acronym       minutes in timezone structure.
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simTimeZoneAcronymGet(L7_char8 *acronym)
{
  osapiStrncpy(acronym, simCfgData.systemTimeZone.tzName, sizeof(simCfgData.systemTimeZone.tzName));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the summer-time mode
*
* @param  ct(output)summer-time mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStModeGet(L7_int32 *mode)
{
  if (mode != NULL)
  {
    *mode = simCfgData.summerTime.mode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Set the summer-time mode
*
* @param  ct(input)summer-time mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStModeSet(L7_int32 mode)
{
  if (mode == L7_SUMMER_TIME_RECURRING_EU || mode == L7_SUMMER_TIME_RECURRING_USA)
  {
    simCfgData.summerTime.mode = L7_SUMMER_TIME_RECURRING;
  }
  else
  {
    simCfgData.summerTime.mode = mode;
  }
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  if (mode == L7_NO_SUMMER_TIME)
  {
    memset(&simCfgData.summerTime.recurring,    0, sizeof(simCfgData.summerTime.recurring));
    memset(&simCfgData.summerTime.nonrecurring, 0, sizeof(simCfgData.summerTime.nonrecurring));
    simCfgData.summerTime.stTimeInSeconds = 0;
    simCfgData.summerTime.endTimeInSeconds = 0;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get non-recurring summertime parameters.
*
* @param    L7_uint32     UnitIndex unit number
* @param    L7_uint32     parameter flag to be set by caller to get corresponding
*                         summer-time parameter.
* @param    void *        data, summer-time field to be returned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStNonRecurringGet(L7_int32 parameter, void *data)
{
  if (data != NULL)
  {
    switch (parameter)
    {
      case L7_START_MONTH:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.start_month;
        return L7_SUCCESS;

      case L7_START_DAY:
        *(L7_uint32 *)data =  simCfgData.summerTime.nonrecurring.start_date;
        return L7_SUCCESS;

      case L7_START_YEAR:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.start_year;
        return L7_SUCCESS;

      case L7_START_HOUR:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.start_time / L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_START_MINUTE:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.start_time % L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_END_MONTH:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.end_month;
        return L7_SUCCESS;

      case L7_END_DAY:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.end_date;
        return L7_SUCCESS;

      case L7_END_YEAR:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.end_year;
        return L7_SUCCESS;

      case L7_END_HOUR:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.end_time / L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_END_MINUTE:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.end_time % L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_OFFSET_MINUTES:
        *(L7_uint32 *)data = simCfgData.summerTime.nonrecurring.offset_minutes;
        return L7_SUCCESS;

      case L7_ZONE_ACRONYM:
        strcpy(data, simCfgData.summerTime.nonrecurring.zone);
        return L7_SUCCESS;

      default:
        return L7_FAILURE;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get summertime recurring parameters
*
* @param    paramter  Indicates flag, for which the field in recurring
*                     summertime needs to get
* @param    data      field in summertime structure.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simStRecurringGet(L7_int32 parameter, void *data)
{
  if (data != NULL)
  {
    switch (parameter)
    {
      case L7_START_MONTH:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.start_month;
        return L7_SUCCESS;

      case L7_START_DAY:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.start_day;
        return L7_SUCCESS;

      case L7_START_WEEK:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.start_week;
        return L7_SUCCESS;

      case L7_START_HOUR:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.start_time / L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_START_MINUTE:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.start_time % L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_END_MONTH:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.end_month;
        return L7_SUCCESS;

      case L7_END_DAY:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.end_day;
        return L7_SUCCESS;

      case L7_END_WEEK:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.end_week;
        return L7_SUCCESS;

      case L7_END_HOUR:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.end_time / L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_END_MINUTE:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.end_time % L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_OFFSET_MINUTES:
        *(L7_uint32 *)data = simCfgData.summerTime.recurring.offset_minutes;
        return L7_SUCCESS;

      case L7_ZONE_ACRONYM:

        strcpy(data, simCfgData.summerTime.recurring.zone);
        return L7_SUCCESS;

      default:
        return L7_FAILURE;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the  recurring summer-time parameters
*
*
* @returns   L7_SUCCESS
*            L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t simStRecurringSet(L7_int32 parameter, void *ptr)
{
  L7_uint32  data;

  if ((ptr != NULL) &&(simCfgData.summerTime.mode == L7_SUMMER_TIME_RECURRING))
  {
    switch (parameter)
    {
      case L7_START_WEEK:

        data= *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.start_week = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_START_DAY:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.start_day = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_START_MONTH:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.start_month = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_START_HOUR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.start_time = data * L7_MINS_PER_HOUR;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_START_MINUTE:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.start_time += data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_END_WEEK:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.end_week = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_END_DAY:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.end_day = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_END_MONTH:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.end_month = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_END_HOUR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.end_time = data * L7_MINS_PER_HOUR;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_END_MINUTE:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.end_time += data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      case L7_OFFSET_MINUTES:
        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.recurring.offset_minutes = data;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;


      case L7_ZONE_ACRONYM:

        strcpy(simCfgData.summerTime.recurring.zone, (L7_char8 *)ptr);
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        return L7_SUCCESS;

      default:
        return L7_FAILURE;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the  non recurring summer-time parameters
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t simStNonRecurringSet(L7_int32 parameter, void *ptr)
{
  L7_uint32  data;

  if (ptr != NULL && simCfgData.summerTime.mode == L7_SUMMER_TIME_NON_RECURRING)
  {
    switch (parameter)
    {
      case L7_START_DAY:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.start_date = data;
        return L7_SUCCESS;

      case L7_START_MONTH:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.start_month = data;
        return L7_SUCCESS;

      case L7_START_YEAR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.start_year = data;
        return L7_SUCCESS;

      case L7_START_HOUR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.start_time = data * L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_START_MINUTE:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.start_time += data;
        return L7_SUCCESS;

      case L7_END_DAY:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.end_date = data;
        return L7_SUCCESS;

      case L7_END_MONTH:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.end_month = data;
        return L7_SUCCESS;

      case L7_END_YEAR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.end_year = data;
        return L7_SUCCESS;

      case L7_END_HOUR:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.end_time = data * L7_MINS_PER_HOUR;
        return L7_SUCCESS;

      case L7_END_MINUTE:

        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.end_time += data;
        return L7_SUCCESS;

      case L7_OFFSET_MINUTES:
        data = *(L7_uint32 *)ptr;
        simCfgData.summerTime.nonrecurring.offset_minutes = data;
        return L7_SUCCESS;

      case L7_ZONE_ACRONYM:
        strcpy(simCfgData.summerTime.nonrecurring.zone, (L7_char8 *)ptr);
        return L7_SUCCESS;

      default:
        return L7_FAILURE;
    }
  }

  return L7_FAILURE;
}

/**********************************************************************
* @purpose  know whether the summer-time is in effect or not.
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL simSummerTimeIsInEffect()
{
  L7_uint32 minutes;
  time_t timenow = osapiUTCTimeNow();
  struct tm *time_ptr;
  L7_uint32 start_day = 0;  /* actual day of nth day */
  L7_int32 start_cmp = 0;
  L7_int32 end_cmp = 0;

  /*Adjustment for time-zone */
  simTimeZoneOffsetGet(&minutes);
  timenow += (minutes*60);

  time_ptr = gmtime(&timenow);

  /* if recurring, take the month, week, day-of-week and time to determine start/end points */
  if (simCfgData.summerTime.mode == L7_SUMMER_TIME_RECURRING)
  {
    start_day = simFindDayOfWeek(time_ptr->tm_mday,
                                 time_ptr->tm_wday,
                                 simCfgData.summerTime.recurring.start_week-1,
                                 simCfgData.summerTime.recurring.start_day-1);

    start_cmp = simCompareDateAndTime(0, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                      0,
                                      simCfgData.summerTime.recurring.start_month,
                                      start_day,
                                      simCfgData.summerTime.recurring.start_time);

    end_cmp = simCompareDateAndTime(0, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                    0,
                                    simCfgData.summerTime.recurring.end_month,
                                    start_day,
                                    simCfgData.summerTime.recurring.end_time);

    /* determine if northern or southern hemisphere */
    if (simCfgData.summerTime.recurring.start_month < simCfgData.summerTime.recurring.end_month ?
        (start_cmp <= 0 && end_cmp > 0) : ((start_cmp >= 0 && end_cmp > 0) || (start_cmp <= 0 && end_cmp < 0)))
    {
        return L7_TRUE;
    }
  }
  /* if not recurring, take the month, day and time to determine start/end points */
  else if (simCfgData.summerTime.mode == L7_SUMMER_TIME_NON_RECURRING)
  {
    start_cmp = simCompareDateAndTime(time_ptr->tm_year+1900, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                      simCfgData.summerTime.nonrecurring.start_year,
                                      simCfgData.summerTime.nonrecurring.start_month,
                                      simCfgData.summerTime.nonrecurring.start_date,
                                      simCfgData.summerTime.nonrecurring.start_time);

    end_cmp = simCompareDateAndTime(time_ptr->tm_year+1900, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                    simCfgData.summerTime.nonrecurring.end_year,
                                    simCfgData.summerTime.nonrecurring.end_month,
                                    simCfgData.summerTime.nonrecurring.end_date,
                                    simCfgData.summerTime.nonrecurring.end_time);

    if (start_cmp <= 0 && end_cmp > 0)
    {
        return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/**********************************************************************
* @purpose  calculates the ordinal day of the month
*
* @returns
*
* @end
*********************************************************************/
L7_uint32 simFindDayOfWeek(L7_uint32 mday, L7_uint32 wday, L7_uint32 ordinal, L7_uint32 day)
{
  /* calculate what day of the week the first day of the month is */
  L7_uint32 first_day_of_month = 7-((mday - wday - 1)%7);

  L7_uint32 counter = 0;

  /* start with first day of month and search for n-th day */
  L7_uint32 check_date;

  for (check_date = 1; check_date <= 31; check_date++)
  {
    first_day_of_month = first_day_of_month % 7;

    if (first_day_of_month == day) {
      counter++;

      if (counter == ordinal + 1) {
        return check_date;
      }
    }
    first_day_of_month++;
  }
  return 0;
}

/**********************************************************************
* @purpose  compare two split-apart dates
*
* @returns  >0 if b is greater
*           <0 if a is greater
*            0 if both are equal
*
* @end
*********************************************************************/
L7_int32 simCompareDateAndTime(L7_uint32 year_a, L7_uint32 month_a, L7_uint32 day_a, L7_uint32 minute_a,
                               L7_uint32 year_b, L7_uint32 month_b, L7_uint32 day_b, L7_uint32 minute_b)
{

  if (year_a != year_b)
  {
    return year_b - year_a;
  }
  if (month_a != month_b)
  {
    return month_b - month_a;
  }
  if (day_a != day_b)
  {
    return day_b - day_a;
  }
  if (minute_a != minute_b)
  {
    return minute_b - minute_a;
  }
  return 0;
}

/**********************************************************************
* @purpose  Adjusts current time for timezone and summer time
*
* @returns  Adjusted time
*
*
* @end
*********************************************************************/
L7_uint32 simAdjustedTimeGet()
{
  L7_uint32 minutes;
  L7_clocktime ct;

  osapiUTCTimeGet(&ct);

  /*Adjustment for time-zone */
  simTimeZoneOffsetGet(&minutes);
  ct.seconds += (minutes*60);

  /*Adjustment for day-light saving */
  if (simSummerTimeIsInEffect() == L7_TRUE) {
    if (simCfgData.summerTime.mode == L7_SUMMER_TIME_RECURRING)
    {
      ct.seconds += simCfgData.summerTime.recurring.offset_minutes * 60;
    }
    else
    {
      ct.seconds += simCfgData.summerTime.nonrecurring.offset_minutes * 60;
    }
  }

  return ct.seconds;
}

/*********************************************************************
* @purpose  Get the Unit's System Location
*
* @param    *location  @b{(output)} system location, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
********************************************************************/
void simGetSystemLocation(L7_char8 *location)
{
  memcpy(location, simCfgData.systemLocation, L7_SYS_SIZE);
}

/*********************************************************************
* @purpose  Sets the Unit's System Location
*
* @param    *location  @b{(input)} system location, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemLocation(L7_char8 *location)
{
  memset(( void * )&simCfgData.systemLocation, 0, L7_SYS_SIZE);

  if (strlen(location) < L7_SYS_SIZE)
    strcpy( simCfgData.systemLocation, location );
  else
    strncpy( simCfgData.systemLocation, location, (L7_SYS_SIZE-1) );

  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Get the Unit's System Contact
*
* @param    *contact  @b{(output)} System Contact, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemContact(L7_char8 *contact)
{
  memcpy(contact, simCfgData.systemContact, L7_SYS_SIZE);
}

/*********************************************************************
* @purpose  Sets the Unit's system Contact
*
* @param    *contact  @b{(input)} System Contact, length L7_SYS_SIZE
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemContact(L7_char8 *contact)
{
  memset(( void * )&simCfgData.systemContact, 0, L7_SYS_SIZE);

  if (strlen(contact) < L7_SYS_SIZE)
    strcpy( simCfgData.systemContact, contact );
  else
    strncpy( simCfgData.systemContact, contact, (L7_SYS_SIZE-1) );

  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}


/*********************************************************************
* @purpose  Get the Unit's System Object ID
*
* @param    *sysOID   @b{(output)} System Object
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemOID(L7_uchar8 *sysOID)
{
  sysapiHpcLocalSystemOidGet(sysOID);
}


/*********************************************************************
* @purpose  Get the value of the number of services primarily
*           offered by this entity.
*
* @param    *val  @b{(output)} Number of services
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemServices(L7_int32 *val)
{
  *val = FD_SIM_DEFAULT_SYSTEM_SERVICES;
  return;
}

/*********************************************************************
* @purpose  Get the Unit's System IP Address
*
* @param    none
*
* @returns  ipAddr   System's IP Address
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPAddr(void)
{
  return simCfgData.systemIpAddress;
}

/*********************************************************************
* @purpose  Sets the Unit's System IP Address
*
* @param    ipAddr   @b{(input)} System IP Address
* @param    force   Force address change.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSystemIPAddr(L7_uint32 ipAddr, L7_BOOL force)
{
  L7_RC_t rc = L7_SUCCESS;
  if ((ipAddr != simCfgData.systemIpAddress) || (force == L7_TRUE))
  {
    simCfgData.systemIpAddress = ipAddr;
    if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_NONE)
    {
      simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }

    sysapiConfigSwitchIp(ipAddr, simCfgData.systemNetMask);
    rc = dtlIPAddrSystemSet(ipAddr);
    if (!ipAddr)
    {
      osapiCleanupIf(L7_DTL_PORT_IF);
    }
    simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_NETWORK, L7_AF_INET);
  }
  return rc;
}

/*********************************************************************
* @purpose  Sets the Unit's System IP Address with Netmask
*
* @param    ipAddr    @b{(input)} System IP Address
* @param    netMask   @b{(input)} System IP Address
* @param    force     Force address change.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSystemIPMaskAddr(L7_uint32 ipAddr, L7_uint32 netMask, L7_BOOL force)
{
  L7_RC_t rc = L7_SUCCESS;

  if ((ipAddr != simCfgData.systemIpAddress) || (force == L7_TRUE))
  {
    simCfgData.systemIpAddress = ipAddr;
    if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_NONE)
    {
      simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    if (netMask != simCfgData.systemNetMask)
    {
      simCfgData.systemNetMask = netMask;
      if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_NONE)
      {
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
      }
    }

    sysapiConfigSwitchIp(ipAddr, netMask);
    rc = dtlIPAddrSystemSet(ipAddr);
    if (!ipAddr)
    {
      osapiCleanupIf(L7_DTL_PORT_IF);
    }
    simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_NETWORK, L7_AF_INET);
  }
  return rc;
}


/*********************************************************************
* @purpose  Get the Unit's System NetMask
*
* @param    none
*
* @returns  netMask   System's NetMask
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPNetMask(void)
{
  return simCfgData.systemNetMask;
}

/*********************************************************************
* @purpose  Sets the Unit's System NetMask
*
* @param    netMask   @b{(input)} System NetMask
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemIPNetMask(L7_uint32 netMask)
{
  if (netMask != simCfgData.systemNetMask)
  {
    if (simCfgData.systemIpAddress != L7_NULL)
    {
      sysapiConfigSwitchIp(simCfgData.systemIpAddress, netMask);
    }
    simCfgData.systemNetMask = netMask;
    if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_NONE)
    {
      simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
  }
}

/*********************************************************************
* @purpose  Get the Unit's System Gateway
*
* @param    none
*
* @returns  gateway  System's Gateway
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPGateway(void)
{
  return simCfgData.systemGateway;
}

/*********************************************************************
* @purpose  Sets the default gateway associated with the network port.
*
* @param    newGateway  @b{(input)} new System Gateway
* @param    force       @b{(input)} set command even if data has not changed
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemIPGateway(L7_uint32 newGateway, L7_uint32 force)
{
    if (newGateway != simCfgData.systemGateway)
    {
      simCfgData.systemGateway = newGateway;
      if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_NONE)
      {
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
      }
    ipstkDefGwUpdate();
  }
  else if (force == L7_TRUE)
  {
    ipstkDefGwUpdate();
  }
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/**************************************************************************
 * @purpose  Activates/Deactivates one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    enable_flag    @b{(input)}   enable=L7_TRUE, disable=L7_FALSE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
static L7_RC_t simActivateSystemIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                L7_uint32 ip6PrefixLen,
                                L7_uint32 enable_flag)
{
  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (enable_flag)
  {
    if (sysapiConfigSystemIPV6PrefixAdd(ip6Addr, ip6PrefixLen)
        != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    (void)sysapiConfigSystemIPV6PrefixRemove(ip6Addr, ip6PrefixLen);
  }

  return L7_SUCCESS;
}
#endif /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */


/*********************************************************************
* @purpose  Get the Unit's System (Network Port) IPV6 Admin maode
*
* @param    none
*
* @returns  adminMode    L7_ENABLE or L7_DISABLE
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPV6AdminMode(void)
{
  return simCfgData.systemIpv6AdminMode;
}

/*********************************************************************
* @purpose  Sets the Unit's System (Network Port) IPV6 Admin Mode
*
* @param    adminMode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSystemIPV6AdminMode(L7_uint32 adminMode)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uchar8 *ifName = L7_DTL_PORT_IF;
  L7_uchar8 qName[L7_DTL_PORT_IF_LEN +2];
  L7_uint32 i = 0;

  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  sprintf(qName,"%s%d",ifName,0);

  switch (adminMode)
  {
    case L7_ENABLE:
      if (osapiIfIpv6Enable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* Re-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateSystemIPV6Prefix(&simCfgData.systemIpv6Intfs[i].ip6Addr,
                                          simCfgData.systemIpv6Intfs[i].ip6PrefixLen,
                                          L7_TRUE);
      }

      /* Re-activate configured IPv6 gateway */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
      {
        (void)sysapiConfigIPV6Gateway(ifName, L7_NULL, &simCfgData.systemIpv6Gateway);
      }

      /* Enable DHCPv6 operationally on the network port if config-enabled */
      if((simCfgData.systemIpv6AdminMode != adminMode) &&
         (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_DHCP);
      }

      break;

    case L7_DISABLE:
      /* De-activate configured IPv6 gateway */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
      {
        (void)sysapiConfigIPV6Gateway(ifName, &simCfgData.systemIpv6Gateway, L7_NULL);
      }

      /* De-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateSystemIPV6Prefix(&simCfgData.systemIpv6Intfs[i].ip6Addr,
                                          simCfgData.systemIpv6Intfs[i].ip6PrefixLen,
                                          L7_FALSE);
      }

      /* Disable DHCPv6 operationally on the network port if config-enabled */
      if((simCfgData.systemIpv6AdminMode != adminMode) &&
         (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NONE);
      }

      if (osapiIfIpv6Disable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      break;

    default:
      return L7_FAILURE;
      break;
  }

  if (simCfgData.systemIpv6AdminMode != adminMode)
  {
    simCfgData.systemIpv6AdminMode = adminMode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on network port interface
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetSystemIPV6Addrs( L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_RC_t rc = L7_FAILURE;
  L7_in6_prefix_t ip6Addr[NUM_IPV6_ADDRS_FROM_STACK];
  L7_uint32 i, j, count, totalcount = 0;
#endif

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  count = *acount;
#endif

  *acount = 0;
  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if(simCfgData.systemIpv6AdminMode == L7_DISABLE)
    {
      *acount = 0;
      return L7_SUCCESS;
    }
    rc = osapiIfIpv6AddrsGet(L7_DTL_PORT_IF, ip6Addr,&count);
    if ((rc != L7_SUCCESS) || (count > NUM_IPV6_ADDRS_FROM_STACK))
    {
      *acount = 0;
      return L7_FAILURE;
    }
    if(count > 0)
    {
       /* First get the link local address configured in the stack */
       for(i = 0; i < count; i++)
       {
         if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
           break;
         else
           continue;
       }
       memcpy(&addrs[totalcount], &ip6Addr[i], sizeof(L7_in6_prefix_t));
       totalcount++;

       /* Next get the manually configured global addresses */
       for(i = 0; i < L7_RTR6_MAX_INTF_ADDRS; i++)
       {
         if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
         {
           continue;
         }
         memcpy(&addrs[totalcount].in6Addr, &simCfgData.systemIpv6Intfs[i].ip6Addr,
                    sizeof(L7_in6_addr_t));
         addrs[totalcount].in6PrefixLen =
                            simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
         totalcount++;
       }

       /* Next get either the autoconfigured or dhcpv6 client assigned global ipv6 addresses */
       if((simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
          (simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE))
       {
         for(i = 0; i < count; i++)
         {
           if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
           {
             continue;
           }
           for(j = 0; j < L7_RTR6_MAX_INTF_ADDRS; j++)
           {
             if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[j].ip6Addr))
             {
               continue;
             }
             if(memcmp(&simCfgData.systemIpv6Intfs[j].ip6Addr, &ip6Addr[i], sizeof(L7_in6_addr_t)) == 0)
             {
               break;
             }
           }
           if(j == L7_RTR6_MAX_INTF_ADDRS)
           {
             memcpy(&addrs[totalcount].in6Addr, &ip6Addr[i], sizeof(L7_in6_prefix_t));
             totalcount++;
           }
         }
       }
       *acount = totalcount;
     }
     return L7_SUCCESS;
#else
    *acount = 0;
    return L7_SUCCESS;
#endif
}

/**************************************************************************
 * @purpose  Get next of the Unit's network port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetNextSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 *ip6PrefixLen,
                                  L7_uint32 *eui_flag)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, foundIndex = -1;

  if (ip6Addr == L7_NULLPTR || ip6PrefixLen == L7_NULLPTR || eui_flag == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(ip6Addr))
  {
    /* This indicates to get the first entry */
    foundIndex = 0;
  }

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
      continue;
    }

    if (foundIndex != -1)
    {
      /* This is the "next" entry, so copy and return */
      memcpy(ip6Addr, &simCfgData.systemIpv6Intfs[i].ip6Addr, sizeof(L7_in6_addr_t));
      *ip6PrefixLen = simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
      *eui_flag = simCfgData.systemIpv6Intfs[i].eui_flag;
      return L7_SUCCESS;
    }

    if ((memcmp(&simCfgData.systemIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.systemIpv6Intfs[i].ip6PrefixLen == *ip6PrefixLen) &&
        (simCfgData.systemIpv6Intfs[i].eui_flag == *eui_flag))
    {
        /* Found the "previous" entry */
        foundIndex = i;
    }
  }

#endif

  /* "next" entry not found */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  mask interface ID portions of an address, leaving net num
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static L7_uchar8 ip6MaskByte[8] = {0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe};
L7_RC_t simIP6MaskApply(L7_in6_addr_t *ip6Addr, L7_uint32 prefix_len)
{
    int i;

    if(prefix_len > 128) return L7_FAILURE;

    /* mask by octet */
    for(i = 0; i < 16;i++)
    {
       if(prefix_len < 8){
          /* mask some bits */
          ip6Addr->in6.addr8[i] &= ip6MaskByte[prefix_len];
          break;
       }
       prefix_len -= 8;
    }
    /* rest of bytes are 0 */
    for(i++; i < 16;i++)
    {
       ip6Addr->in6.addr8[i] = 0;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check to see if the ip6 address conflicts with that of
*           the service port.
*
* @param    ip6Address      IP6 Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with service port ip6 addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simServPortIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr,
                                            L7_uint32 ip6PrefixLen)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i = 0, count = NUM_IPV6_ADDRS_FROM_STACK;
  L7_uint32 pref_min;
  L7_in6_addr_t a1,a2;
  L7_in6_prefix_t ip6AddrFromStack[NUM_IPV6_ADDRS_FROM_STACK];
  L7_RC_t rc = L7_FAILURE;

  /* Check for conflict with service port config */
  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      continue;
    }
    pref_min = (ip6PrefixLen < simCfgData.servPortIpv6Intfs[i].ip6PrefixLen)?
                    ip6PrefixLen:simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
    a1 = *ip6Addr;
    simIP6MaskApply(&a1,pref_min);
    a2 = simCfgData.servPortIpv6Intfs[i].ip6Addr;
    simIP6MaskApply(&a2,pref_min);
    if(memcmp(&a1,&a2, 16) == 0)
      return L7_FAILURE;
  }

  rc =  osapiIfIpv6AddrsGet(bspapiServicePortNameGet(), ip6AddrFromStack,&count);
  if ((rc != L7_SUCCESS) || (count > NUM_IPV6_ADDRS_FROM_STACK))
  {
    return L7_FAILURE;
  }

  /* Check for conflict with autoconfigured or dhcpv6 learnt ipv6 addresses
   * on service ports */
  if((simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
     (simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE))
  {
    for(i = 0; i < count; i++)
    {
      if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6AddrFromStack[i].in6Addr))
      {
        continue;
      }
      /* compare against the complete 128 bit address in the stack */
      if(memcmp(ip6Addr, &ip6AddrFromStack[i], sizeof(L7_in6_addr_t)) == 0)
      {
        return L7_FAILURE;
      }
    }
  }
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check to see if the ip6 address conflicts with that of
*           the network port.
*
* @param    ip6Address      IP6 Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with network port ip6 addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simSystemIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr,
                                          L7_uint32 ip6PrefixLen)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i = 0, count = NUM_IPV6_ADDRS_FROM_STACK;
  L7_uint32 pref_min;
  L7_in6_addr_t a1,a2;
  L7_in6_prefix_t ip6AddrFromStack[NUM_IPV6_ADDRS_FROM_STACK];
  L7_RC_t rc = L7_FAILURE;

  /* Check for conflict with network port config */
  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
      continue;
    }
    pref_min = (ip6PrefixLen < simCfgData.systemIpv6Intfs[i].ip6PrefixLen)?
                    ip6PrefixLen:simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
    a1 = *ip6Addr;
    simIP6MaskApply(&a1,pref_min);
    a2 = simCfgData.systemIpv6Intfs[i].ip6Addr;
    simIP6MaskApply(&a2,pref_min);
    if(memcmp(&a1,&a2, 16) == 0)
      return L7_FAILURE;
  }

  rc =  osapiIfIpv6AddrsGet(L7_DTL_PORT_IF, ip6AddrFromStack,&count);
  if ((rc != L7_SUCCESS) || (count > NUM_IPV6_ADDRS_FROM_STACK))
  {
    return L7_FAILURE;
  }

  /* Check for conflict with autoconfigured or dhcpv6 learnt ipv6 addresses
   * on service ports */
  if((simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
     (simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE))
  {
    for(i = 0; i < count; i++)
    {
      if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6AddrFromStack[i].in6Addr))
      {
        continue;
      }
      /* compare against the complete 128 bit address in the stack */
      if(memcmp(ip6Addr, &ip6AddrFromStack[i], sizeof(L7_in6_addr_t)) == 0)
      {
        return L7_FAILURE;
      }
    }
  }
#endif
  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Get the auto-configured link-local address of the management port
 *
 * @param    ip6Addr        @b{(input)}   IPv6 address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    This function updates the ip6Addr argument with the
 *              autoconfigured eui-64 format link local address of
 *              the management port (service port or network port),
 *              irrespective of the ipv6 state of the mgmt interface.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetSystemLinkLocalIPV6Addr (L7_in6_addr_t *ip6Addr)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(mac);
  }
  else
  {
    simGetSystemIPLocalAdminMac(mac);
  }
  memset(ip6Addr, 0, sizeof(L7_in6_addr_t));

  ip6Addr->in6.addr8[0] = 0xfe;
  ip6Addr->in6.addr8[1] = 0x80;
  ip6Addr->in6.addr8[8] = mac[0] ^ 0x02;
  ip6Addr->in6.addr8[9] = mac[1];
  ip6Addr->in6.addr8[10] = mac[2];
  ip6Addr->in6.addr8[11] = 0xff;
  ip6Addr->in6.addr8[12] = 0xfe;
  ip6Addr->in6.addr8[13] = mac[3];
  ip6Addr->in6.addr8[14] = mac[4];
  ip6Addr->in6.addr8[15] = mac[5];

#endif

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Sets one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t simSetSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                L7_uint32 eui_flag, L7_uint32 force)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, emptyIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&tempIp6Addr, 0, sizeof(tempIp6Addr));
  memcpy(&tempIp6Addr, ip6Addr, sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
    {
      return L7_FAILURE;
    }
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(mac);
    }
    else
    {
      simGetSystemIPLocalAdminMac(mac);
    }
    tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
    tempIp6Addr.in6.addr8[9] = mac[1];
    tempIp6Addr.in6.addr8[10] = mac[2];
    tempIp6Addr.in6.addr8[11] = 0xff;
    tempIp6Addr.in6.addr8[12] = 0xfe;
    tempIp6Addr.in6.addr8[13] = mac[3];
    tempIp6Addr.in6.addr8[14] = mac[4];
    tempIp6Addr.in6.addr8[15] = mac[5];

    memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
  }

  if (force == L7_FALSE)
  {
    /* Check for conflict with service port config */
    if(SERVICE_PORT_PRESENT)
    {
      if((simServPortIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen))
                             != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }

    /* Check for conflict with network port config */
    if((simSystemIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen)) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
    /* Check IPv6 prefix for conflicts with routing interface */
    if (ip6MapRtrIntfAddressConflict(0, 0, ip6Addr, ip6PrefixLen, 0)
               != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
#endif
#endif

    /* find a empty slot for network port */
    for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
    {
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
      {
        if (emptyIndex == -1)
            emptyIndex = i;
        break;
      }
    }

    if (emptyIndex == -1)
    {
      /* No empty slots found to store the IPv6 prefix! */
      return L7_FAILURE;
    }
  }

  if (force == L7_FALSE)
  {
    /* Save our new IPv6 prefix in the config */
    memcpy(&simCfgData.systemIpv6Intfs[emptyIndex].ip6Addr, ip6Addr,
           sizeof(L7_in6_addr_t));
    simCfgData.systemIpv6Intfs[emptyIndex].ip6PrefixLen = ip6PrefixLen;
    simCfgData.systemIpv6Intfs[emptyIndex].eui_flag = eui_flag;

    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  (void)simActivateSystemIPV6Prefix(ip6Addr, ip6PrefixLen,  L7_TRUE);
#endif

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Removes one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t simDeleteSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                   L7_uint32 eui_flag)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, foundIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&tempIp6Addr,0,sizeof(tempIp6Addr));
  memset(mac, 0, L7_MAC_ADDR_LEN);
  memcpy(&tempIp6Addr,ip6Addr,sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
      return L7_FAILURE;

    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(mac);
    }
    else
    {
      simGetSystemIPLocalAdminMac(mac);
    }

    tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
    tempIp6Addr.in6.addr8[9] = mac[1];
    tempIp6Addr.in6.addr8[10] = mac[2];
    tempIp6Addr.in6.addr8[11] = 0xff;
    tempIp6Addr.in6.addr8[12] = 0xfe;
    tempIp6Addr.in6.addr8[13] = mac[3];
    tempIp6Addr.in6.addr8[14] = mac[4];
    tempIp6Addr.in6.addr8[15] = mac[5];

    memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
  }

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
        continue;
    }

    if ((memcmp(&simCfgData.systemIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.systemIpv6Intfs[i].ip6PrefixLen == ip6PrefixLen))
    {
      /* Found the address, so just break. */
        foundIndex = i;
        break;
    }
  }

  if (foundIndex == -1)
  {
    /* Didn't find the IPv6 prefix! */
    return L7_FAILURE;
  }

  (void)simActivateSystemIPV6Prefix(ip6Addr, ip6PrefixLen, L7_FALSE);

  /* Remove the IPv6 prefix from the config */
  memset(&simCfgData.systemIpv6Intfs[foundIndex].ip6Addr, 0, sizeof(L7_in6_addr_t));
  simCfgData.systemIpv6Intfs[foundIndex].ip6PrefixLen = 0;
  simCfgData.systemIpv6Intfs[foundIndex].eui_flag = 0;

  simCfgData.cfgHdr.dataChanged = L7_TRUE;

#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on network port interface
*
* @param    addrs   pointer to default router array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetSystemIPV6DefaultRouters( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    return osapiIfIpv6DefaultRoutersGet(L7_DTL_PORT_IF, addrs,acount);
#else
    *acount = 0;
    return L7_SUCCESS;
#endif
}

/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on network port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetSystemIPV6Gateway( L7_in6_addr_t *gateway)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
  {
    return L7_FAILURE;
  }
  else
  {
    memcpy(gateway, &simCfgData.systemIpv6Gateway, sizeof(L7_in6_addr_t));
    return L7_SUCCESS;
  }

#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on network port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simSetSystemIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force)
{

  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if (L7_IP6_IS_ADDR_UNSPECIFIED(gateway))
  {
    /* We are attempting to delete the existing gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
    {
      /* Remove the existing gateway */
      if (sysapiConfigIPV6Gateway(L7_DTL_PORT_IF, &simCfgData.systemIpv6Gateway,
                                  L7_NULL) == L7_SUCCESS)
      {
        if (force == L7_FALSE)
        {
          memset(&simCfgData.systemIpv6Gateway, 0, sizeof(L7_in6_addr_t));
          simCfgData.cfgHdr.dataChanged = L7_TRUE;
        }
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* We are attempting to add a new gateway */
    if (sysapiConfigIPV6Gateway(L7_DTL_PORT_IF, L7_NULL, gateway)
                        == L7_SUCCESS)
    {
      if (force == L7_FALSE)
      {
        memcpy(&simCfgData.systemIpv6Gateway, gateway, sizeof(L7_in6_addr_t));
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }

#endif

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the Unit's System Burned in Mac Address
*
* @param    *sysBIA   @b{(output)} pointer to system burned in mac address
*                              Length L7_MAC_ADDR_LEN
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemIPBurnedInMac(L7_uchar8 *sysBIA)
{
  memcpy(sysBIA, simCfgData.systemBIA, L7_MAC_ADDR_LEN);
}

/*********************************************************************
* @purpose  Get the Unit's Service Port's Burned in Mac Address
*
* @param    *servPortBIA   @b{(output)} pointer to service port burned in mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetServicePortBurnedInMac(L7_uchar8 *servPortBIA)
{
#if L7_FEAT_DNI8541_BLADESERVER
  bspapiIomServicePortMacAddrGet(servPortBIA);
#else
  simGetSystemIPBurnedInMac(servPortBIA);
  /* Service port burned in mac is obtained by adding 1 to the
     system burned in mac */
  servPortBIA[5] +=1;
#endif
}

/*********************************************************************
* @purpose  Get the Unit's System Locally Administered Mac Address
*
* @param    *sysLAA   @b{(output)} pointer to system local admin mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simGetSystemIPLocalAdminMac(L7_uchar8 *sysLAA)
{
  memcpy(sysLAA, simCfgData.systemLAA, L7_MAC_ADDR_LEN);
}

/*********************************************************************
* @purpose  Sets the Unit's System Locally Administered Mac Address
*
* @param    *sysLAA   @b{(input)} pointer to system local admin mac address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemIPLocalAdminMac(L7_uchar8 *sysLAA)
{
  L7_uint32 vlanId;

  vlanId = simMgmtVlanIdGet();
  if (simGetSystemIPMacType() == L7_SYSMAC_LAA)
  {
    if (memcmp(sysLAA, simCfgData.systemLAA, L7_MAC_ADDR_LEN))
    {
      fdbSysMacDelEntry(simCfgData.systemLAA, vlanId, 1, DTL_MAC_TYPE_LOCAL_ADMIN);
      osapiUpdateNetworkMac(sysLAA);
    }
    else
      return;
  }

  memcpy(simCfgData.systemLAA, sysLAA, L7_MAC_ADDR_LEN);
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Get the Unit's System Mac Address Type
*
* @param    none
*
* @returns  sysMacType   System Mac Address Type
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPMacType(void)
{
  return(simCfgData.systemMacType);
}

/*********************************************************************
* @purpose  Sets the Unit's System Mac Address Type
*
* @param    type   @b{(input)} System Mac Address Type
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemIPMacType(L7_uint32 type)
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint32 unit;
  L7_uint32 vlanId;

  unit = usmDbThisUnitGet();
  vlanId = simMgmtVlanIdGet();

  if (type != simCfgData.systemMacType)
  {
    if (simCfgData.systemMacType == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(mac);
      fdbSysMacDelEntry(mac, vlanId, 1, DTL_MAC_TYPE_BURNED_IN);
      simGetSystemIPLocalAdminMac(mac);
      osapiUpdateNetworkMac(mac);
    }
    else
    {
      simGetSystemIPLocalAdminMac(mac);
      fdbSysMacDelEntry(mac, vlanId, 1, DTL_MAC_TYPE_LOCAL_ADMIN);
      simGetSystemIPBurnedInMac(mac);
      osapiUpdateNetworkMac(mac);
    }

    simCfgData.systemMacType = type;

      /* IEEE 802.1s Support for Multiple Spanning Tree */

      /* Notify Spanning Tree of the Mac address type change. */
      dot1sMacAddrChangeNotify();


    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
}



/*********************************************************************
* @purpose  Get the Unit's service port IP Address
*
* @param    none
*
* @returns  ipAddr   service port IP Address
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPAddr(void)
{
  return simCfgData.servPortIpAddress;
}

/*********************************************************************
* @purpose  Sets the Unit's service port IP Address
*
* @param    ipAddr   @b{(input)} service port IP Address
* @param    force    @b{(input)} issue set command even if data has not changed
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortIPAddr(L7_uint32 ipAddr, L7_uint32 force)
{
  L7_uint32 rc;
  if (ipAddr != simCfgData.servPortIpAddress || force == L7_TRUE)
  {
    rc = sysapiConfigServicePortIp(ipAddr, simCfgData.servPortNetMask);

    if (ipAddr != simCfgData.servPortIpAddress)
    {
      simCfgData.servPortIpAddress = ipAddr;
      if (simGetServPortConfigMode() == L7_SYSCONFIG_MODE_NONE)
      {
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_SERVICEPORT, L7_AF_INET);
      }
#if L7_FEAT_DNI8541_BLADESERVER
      bspapiIomIpConfigChanged();
#endif
    }
  }

  if (!ipAddr)
  {
    /* Remove IP address and local route from interface in stack. */
    osapiCleanupIf(bspapiServicePortNameGet());
  }
  return;
}

/*********************************************************************
* @purpose  Get the Unit's service port NetMask
*
* @param    none
*
* @returns  netMask   service port NetMask
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPNetMask(void)
{
  return simCfgData.servPortNetMask;
}

/*********************************************************************
* @purpose  Sets the Unit's service port NetMask
*
* @param    netMask   @b{(input)} Service port NetMask
* @param    force     @b{(input)} issue set command even if data has not changed
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortIPNetMask(L7_uint32 netMask, L7_uint32 force)
{
  L7_uint32 rc;
  if (netMask != simCfgData.servPortNetMask || force == L7_TRUE)
  {
    if (simCfgData.servPortIpAddress != L7_NULL)
    {
      rc = sysapiConfigServicePortIp(simCfgData.servPortIpAddress, netMask);
    }

    if (netMask != simCfgData.servPortNetMask)
    {
      simCfgData.servPortNetMask = netMask;
      if (simGetServPortConfigMode() == L7_SYSCONFIG_MODE_NONE)
      {
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
        simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_SERVICEPORT, L7_AF_INET);
      }
#if L7_FEAT_DNI8541_BLADESERVER
      bspapiIomIpConfigChanged();
#endif
    }
  }
}

/*********************************************************************
* @purpose  Get the Unit's service port Gateway
*
* @param    none
*
* @returns  gateway  service port Gateway
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPGateway(void)
{
  return simCfgData.servPortGateway;
}

/*********************************************************************
* @purpose  Sets the Unit's service port Gateway
*
* @param    newGateway  @b{(input)} new service port Gateway
* @param    force       @b{(input)} issue set command even if data has not changed
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortIPGateway(L7_uint32 newGateway, L7_uint32 force)
{
  if (newGateway != simCfgData.servPortGateway)
  {
    simCfgData.servPortGateway = newGateway;
    if (simGetServPortConfigMode() == L7_SYSCONFIG_MODE_NONE)
    {
      simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    ipstkDefGwUpdate();
#if L7_FEAT_DNI8541_BLADESERVER
    bspapiIomIpConfigChanged();
#endif
  }
  else if (force == L7_TRUE)
  {
    ipstkDefGwUpdate();
  }
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/**************************************************************************
 * @purpose  Activates/Deactivates one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    enable_flag    @b{(input)}   enable=L7_TRUE, disable=L7_FALSE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
static L7_RC_t simActivateServPortIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                L7_uint32 ip6PrefixLen,
                                L7_uint32 enable_flag)
{

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (enable_flag)
  {
    if (sysapiConfigServicePortIPV6PrefixAdd(ip6Addr, ip6PrefixLen)
        != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    (void)sysapiConfigServicePortIPV6PrefixRemove(ip6Addr, ip6PrefixLen);
  }

  return L7_SUCCESS;
}
#endif /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */

/*********************************************************************
* @purpose  Get the Unit's Service Port IPV6 Admin maode
*
* @param    none
*
* @returns  adminMode    L7_ENABLE or L7_DISABLE
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPV6AdminMode(void)
{
  return simCfgData.servPortIpv6AdminMode;
}

/*********************************************************************
* @purpose  Sets the Unit's Service Port IPV6 Admin Mode
*
* @param    adminMode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetServPortIPV6AdminMode(L7_uint32 adminMode)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uchar8 *ifName = bspapiServicePortNameGet();
  L7_uchar8 qName[32];
  L7_uint32 i = 0;

  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  sprintf(qName,"%s%d",ifName,0);

  switch (adminMode)
  {
    case L7_ENABLE:
      if (osapiIfIpv6Enable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* Re-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateServPortIPV6Prefix(&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                                            simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
                                            L7_TRUE);
      }

      /* Re-activate configured IPv6 gateway */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
      {
        (void)sysapiConfigIPV6Gateway(ifName, L7_NULL, &simCfgData.servPortIpv6Gateway);
      }

      /* Enable DHCPv6 operationally on the service port if config-enabled */
      if((simCfgData.servPortIpv6AdminMode != adminMode) &&
         (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_DHCP);
      }

      break;

    case L7_DISABLE:
      /* De-activate configured IPv6 gateway */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
      {
        (void)sysapiConfigIPV6Gateway(ifName, &simCfgData.servPortIpv6Gateway, L7_NULL);
      }

      /* De-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateServPortIPV6Prefix(&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                                            simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
                                            L7_FALSE);
      }

      /* Disable DHCPv6 operationally on the service port if config-enabled */
      if((simCfgData.servPortIpv6AdminMode != adminMode) &&
         (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_NONE);
      }

      if (osapiIfIpv6Disable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      break;

    default:
      return L7_FAILURE;
      break;
  }

  if (simCfgData.servPortIpv6AdminMode != adminMode)
  {
    simCfgData.servPortIpv6AdminMode = adminMode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on service port interface
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6Addrs( L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_RC_t rc = L7_FAILURE;
  L7_in6_prefix_t ip6Addr[NUM_IPV6_ADDRS_FROM_STACK];
  L7_uint32 i, j, count, totalcount = 0;
#endif

  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  count = *acount;
#endif

  *acount = 0;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if(simCfgData.servPortIpv6AdminMode == L7_DISABLE)
    {
      *acount = 0;
      return L7_SUCCESS;
    }
    rc =  osapiIfIpv6AddrsGet(bspapiServicePortNameGet(), ip6Addr,&count);
    if ((rc != L7_SUCCESS) || (count > NUM_IPV6_ADDRS_FROM_STACK))
    {
      *acount = 0;
      return L7_FAILURE;
    }

    if(count > 0)
    {
       /* First get the link local address configured in the stack */
       for(i = 0; i < count; i++)
       {
         if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
           break;
         else
           continue;
       }
       memcpy(&addrs[totalcount], &ip6Addr[i], sizeof(L7_in6_prefix_t));
       totalcount++;

       /* Next get the manually configured global addresses */
       for(i = 0; i < L7_RTR6_MAX_INTF_ADDRS; i++)
       {
         if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
         {
           continue;
         }
         memcpy(&addrs[totalcount].in6Addr, &simCfgData.servPortIpv6Intfs[i].ip6Addr,
                    sizeof(L7_in6_addr_t));
         addrs[totalcount].in6PrefixLen =
                             simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
          totalcount++;
       }

       /* Next get either the autoconfigured or dhcpv6 client assigned global ipv6 addresses */
       if((simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
          (simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE))
       {
         for(i = 0; i < count; i++)
         {
           if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
           {
             continue;
           }
           for(j = 0; j < L7_RTR6_MAX_INTF_ADDRS; j++)
           {
             if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[j].ip6Addr))
             {
               continue;
             }
             if(memcmp(&simCfgData.servPortIpv6Intfs[j].ip6Addr, &ip6Addr[i], sizeof(L7_in6_addr_t)) == 0)
             {
               break;
             }
           }
           if(j == L7_RTR6_MAX_INTF_ADDRS)
           {
             memcpy(&addrs[totalcount].in6Addr, &ip6Addr[i], sizeof(L7_in6_prefix_t));
             totalcount++;
           }
         }
       }
       *acount = totalcount;
     }
     return L7_SUCCESS;
#else
    *acount = 0;
    return L7_SUCCESS;
#endif

}

/**************************************************************************
 * @purpose  Get next of the Unit's service port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetNextServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 *ip6PrefixLen,
                                  L7_uint32 *eui_flag)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, foundIndex = -1;

  if (ip6Addr == L7_NULLPTR || ip6PrefixLen == L7_NULLPTR || eui_flag == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(ip6Addr))
  {
    /* This indicates to get the first entry */
    foundIndex = 0;
  }

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      continue;
    }

    if (foundIndex != -1)
    {
      /* This is the "next" entry, so copy and return */
      memcpy(ip6Addr, &simCfgData.servPortIpv6Intfs[i].ip6Addr, sizeof(L7_in6_addr_t));
      *ip6PrefixLen = simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
      *eui_flag = simCfgData.servPortIpv6Intfs[i].eui_flag;
      return L7_SUCCESS;
    }

    if ((memcmp(&simCfgData.servPortIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.servPortIpv6Intfs[i].ip6PrefixLen == *ip6PrefixLen) &&
        (simCfgData.servPortIpv6Intfs[i].eui_flag == *eui_flag))
    {
        /* Found the "previous" entry */
        foundIndex = i;
    }
  }

#endif

  /* "next" entry not found */
  return L7_FAILURE;
}

/**************************************************************************
 * @purpose  Sets one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t simSetServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                  L7_uint32 eui_flag, L7_uint32 force)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, emptyIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  memset(&tempIp6Addr,0,sizeof(tempIp6Addr));
  memcpy(&tempIp6Addr,ip6Addr,sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
       return L7_FAILURE;

    simGetServicePortBurnedInMac(mac);

    tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
    tempIp6Addr.in6.addr8[9] = mac[1];
    tempIp6Addr.in6.addr8[10] = mac[2];
    tempIp6Addr.in6.addr8[11] = 0xff;
    tempIp6Addr.in6.addr8[12] = 0xfe;
    tempIp6Addr.in6.addr8[13] = mac[3];
    tempIp6Addr.in6.addr8[14] = mac[4];
    tempIp6Addr.in6.addr8[15] = mac[5];

    memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
  }

  if (force == L7_FALSE)
  {
    /* Check for conflict with network port config */
    if((simSystemIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen)) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* Check for conflict with service port config */
   if((simServPortIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen)) != L7_SUCCESS)
   {
     return L7_FAILURE;
   }


#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
    /* Check IPv6 prefix for conflicts with routing interface */
    if (ip6MapRtrIntfAddressConflict(0, 0, ip6Addr, ip6PrefixLen, 0)
               != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
#endif
#endif

    for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
    {
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
      {
        if (emptyIndex == -1)
            emptyIndex = i;
        break;
      }
    }

    if (emptyIndex == -1)
    {
      /* No empty slots found to store the IPv6 prefix! */
      return L7_FAILURE;
    }
  }

  if (force == L7_FALSE)
  {
    /* Save our new IPv6 prefix in the config */
    memcpy(&simCfgData.servPortIpv6Intfs[emptyIndex].ip6Addr, ip6Addr,
           sizeof(L7_in6_addr_t));
    simCfgData.servPortIpv6Intfs[emptyIndex].ip6PrefixLen = ip6PrefixLen;
    simCfgData.servPortIpv6Intfs[emptyIndex].eui_flag = eui_flag;

    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  (void)simActivateServPortIPV6Prefix(ip6Addr, ip6PrefixLen, L7_TRUE);

#endif

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Removes one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t simDeleteServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                     L7_uint32 eui_flag)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_int32 i = 0, foundIndex = -1;
  L7_in6_addr_t tempIp6Addr;
   L7_uchar8 mac[L7_MAC_ADDR_LEN];

   memset(&tempIp6Addr, 0, sizeof(tempIp6Addr));
   memcpy(&tempIp6Addr, ip6Addr, sizeof(tempIp6Addr));

   if (eui_flag == L7_TRUE)
   {
     if(ip6PrefixLen != 64)
       return L7_FAILURE;

     simGetServicePortBurnedInMac(mac);

     tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
     tempIp6Addr.in6.addr8[9] = mac[1];
     tempIp6Addr.in6.addr8[10] = mac[2];
     tempIp6Addr.in6.addr8[11] = 0xff;
     tempIp6Addr.in6.addr8[12] = 0xfe;
     tempIp6Addr.in6.addr8[13] = mac[3];
     tempIp6Addr.in6.addr8[14] = mac[4];
     tempIp6Addr.in6.addr8[15] = mac[5];
     memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
   }


  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
        continue;
    }

    if ((memcmp(&simCfgData.servPortIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.servPortIpv6Intfs[i].ip6PrefixLen == ip6PrefixLen))
    {
       /* Found the address, so just break. */
        foundIndex = i;
        break;
    }
  }

  if (foundIndex == -1)
  {
    /* Didn't find the IPv6 prefix! */
    return L7_FAILURE;
  }

  (void) simActivateServPortIPV6Prefix(ip6Addr, ip6PrefixLen, L7_FALSE);

  /* Remove the IPv6 prefix from the config */
  memset(&simCfgData.servPortIpv6Intfs[foundIndex].ip6Addr, 0, sizeof(L7_in6_addr_t));
  simCfgData.servPortIpv6Intfs[foundIndex].ip6PrefixLen = 0;
  simCfgData.servPortIpv6Intfs[foundIndex].eui_flag = 0;

  simCfgData.cfgHdr.dataChanged = L7_TRUE;
#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on service port interface
*
* @param    addrs   pointer to default router array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6DefaultRouters( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    return osapiIfIpv6DefaultRoutersGet(bspapiServicePortNameGet(), addrs,acount);
#else
    *acount = 0;
    return L7_SUCCESS;
#endif
}

/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on service port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6Gateway( L7_in6_addr_t *gateway)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
  {
    return L7_FAILURE;
  }
  else
  {
    memcpy(gateway, &simCfgData.servPortIpv6Gateway, sizeof(L7_in6_addr_t));
    return L7_SUCCESS;
  }

#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on service port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simSetServPortIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if (L7_IP6_IS_ADDR_UNSPECIFIED(gateway))
  {
    /* We are attempting to delete the existing gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
    {
      /* Remove the existing gateway */
      if (sysapiConfigIPV6Gateway(bspapiServicePortNameGet(), &simCfgData.servPortIpv6Gateway,
                                  L7_NULL) == L7_SUCCESS)
      {
        if (force == L7_FALSE)
        {
          memset(&simCfgData.servPortIpv6Gateway, 0, sizeof(L7_in6_addr_t));
          simCfgData.cfgHdr.dataChanged = L7_TRUE;
        }
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* We are attempting to add a new gateway */
    if (sysapiConfigIPV6Gateway(bspapiServicePortNameGet(), L7_NULL, gateway)
                        == L7_SUCCESS)
    {
      if (force == L7_FALSE)
      {
        memcpy(&simCfgData.servPortIpv6Gateway, gateway, sizeof(L7_in6_addr_t));
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }

#endif

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  get management port NDisc info from stack
*
* @param    ifName            management interface name
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t simGetNextMgmtPortIPV6Ndp( L7_uchar8 *ifName,
                               L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    ipstkIpv6NetToMediaEntry_t vars;
    L7_uint32 ifIndex;
    L7_BOOL icarry;

    /* increment index ala snmp */
    if(!L7_IP6_IS_ADDR_UNSPECIFIED(ipv6NetAddress))
    {
        L7_IP6_ADDR_INCREMENT(ipv6NetAddress,icarry);
        if(icarry == L7_TRUE)
            return L7_FAILURE;
    }

    /* get interface index */
    if(ipstkStackIfIndexGet(ifName, &ifIndex) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    vars.ipv6IfIndex = ifIndex;
    vars.ipv6NetToMediaNetAddress = *ipv6NetAddress;


    if(osapiIpv6NetToMediaEntryGet(L7_MATCH_GETNEXT, L7_TRUE, &vars) != L7_SUCCESS){
        return L7_FAILURE;
    }
    if(vars.ipv6IfIndex != ifIndex){
        return L7_FAILURE;
    }
    if(vars.ipv6NetToMediaPhysAddress.len > *ipv6PhysAddrLen){
        return L7_FAILURE;
    }
    *ipv6NetAddress = vars.ipv6NetToMediaNetAddress;
    *ipv6PhysAddrLen = vars.ipv6NetToMediaPhysAddress.len;
    memcpy(ipv6PhysAddress, vars.ipv6NetToMediaPhysAddress.addr, *ipv6PhysAddrLen);
    *ipv6Type = vars.ipv6IfNetToMediaType;
    *ipv6State = vars.ipv6IfNetToMediaState;
    *ipv6LastUpdated = vars.ipv6IfNetToMediaLastUpdated;
    *ipv6IsRtr = vars.ipv6IfNetToMediaIsRouter;


    return L7_SUCCESS;
#else
    return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  get service port NDisc info
*
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simGetServPortIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
    L7_uchar8 *ifName = bspapiServicePortNameGet();
    L7_uchar8 qName[32];

    sprintf(qName,"%s%d",ifName,0);
    return simGetNextMgmtPortIPV6Ndp(qName, ipv6NetAddress, ipv6PhysAddrLen,
                                     ipv6PhysAddress, ipv6Type, ipv6State,
                                     ipv6LastUpdated, ipv6IsRtr);
}

/*********************************************************************
* @purpose  get network port NDisc info
*
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simGetSystemIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
    L7_uchar8 *ifName = L7_DTL_PORT_IF;
    L7_uchar8 qName[L7_DTL_PORT_IF_LEN +2];

    sprintf(qName,"%s%d",ifName,0);
    return simGetNextMgmtPortIPV6Ndp(qName, ipv6NetAddress, ipv6PhysAddrLen,
                                     ipv6PhysAddress, ipv6Type, ipv6State,
                                     ipv6LastUpdated, ipv6IsRtr);
}

/*********************************************************************
* @purpose  Get the Unit's System Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemConfigMode(void)
{
  return(simCfgData.systemConfigMode);
}

/*********************************************************************
* @purpose  Get the Unit's System IPv6 Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPv6ConfigMode(void)
{
  return(simCfgData.systemIPv6ConfigMode);
}

/*********************************************************************
* @purpose  Get the Unit's System Current Config Mode
*
* @param    none
*
* @returns  mode  System Current Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemCurrentConfigMode(void)
{
  return(systemCurrentConfigMode);
}

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simSystemIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  *val = simCfgData.systemIPv6AddrAutoConfig;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the ipv6 address autoconfiguration is enabled on the service port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simSystemIPv6AddrAutoConfigSet(L7_uint32 val)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_char8 ifname[32];

  if((val == L7_ENABLE) &&
     ((simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
      (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
      (simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE)))
  {
    /* Don't allow ipv6 address autoconfiguration on network port if
     * (a) the dhcpv6 protocol is enabled on the network port (or)
     * (b) the dhcpv6 protocol is enabled on the service port (or)
     * (c) the ipv6 address autoconfiguration is enabled on the service port */
    return L7_FAILURE;
  }

  if(val != simCfgData.systemIPv6AddrAutoConfig)
  {
    simCfgData.systemIPv6AddrAutoConfig = val;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    sprintf(ifname, "%s%d", L7_DTL_PORT_IF, 0);
    osapiIfIpv6AddrAutoConfigSet(ifname, simCfgData.systemIPv6AddrAutoConfig);

    /* If ipv6 mode is enabled on network port, disable and re-enable ipv6 mode to -
     *
     * (a) flush already autoconfigured ipv6 addresses (while disabling autoconfig mode)
     * (b) trigger the stack to send router solicitation packet out to be able
     *     to quickly facilitate autoconfiguration of ipv6 addresses (while enabling autoconfig mode)
     */
    if(simCfgData.systemIpv6AdminMode == L7_ENABLE)
    {
      if (osapiIfIpv6Disable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to disable ipv6 mode on Network Port while changing ipv6 autoconfig mode.\n");
      }
      if (osapiIfIpv6Enable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to enable ipv6 mode on Network Port while changing ipv6 autoconfig mode.\n");
      }
    }
  }
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Unit's System Config Mode
*
* @param    mode  @b{(input)} System Config Mode
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemConfigMode(L7_uint32 mode)
{
  L7_RC_t rc;

  switch (mode)
  {
  case L7_SYSCONFIG_MODE_NONE:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_NONE);
    break;
  case L7_SYSCONFIG_MODE_BOOTP:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_BOOTP);
    break;
  case L7_SYSCONFIG_MODE_DHCP:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_DHCP);
    break;
  default:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_NONE);
    break;

  }
  if (rc == L7_SUCCESS)
  {
    if (simGetSystemConfigMode() != mode)
    {
      /* Clear off the current IP information */
      simSetSystemIPGateway(0, L7_FALSE);
      simSetSystemIPAddr(0, L7_FALSE);
      simSetSystemIPNetMask(0);
    }

    simCfgData.systemConfigMode = mode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

  }
}

/*********************************************************************
* @purpose  Sets the Unit's System IPv6 Config Mode
*
* @param    mode  @b{(input)} System Config Mode
*
* @returns L7_FAILURE    When any of this is true
*                        - the ipv6 address autoconfiguration is enabled on the network port
*                        - the ipv6 address autoconfiguration is enabled on the service port
*                        - the dhcpv6 protocol is enabled on the service port
*          L7_SUCCESS    otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSystemIPv6ConfigMode(L7_uint32 mode)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if((mode == L7_SYSCONFIG_MODE_DHCP) &&
     ((simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE) ||
      (simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE) ||
      (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)))
  {
    /* Don't allow DHCPv6 client mode to be set on network port if
     * (a) the ipv6 address autoconfiguration is enabled on the network port (or)
     * (b) the ipv6 address autoconfiguration is enabled on the service port (or)
     * (b) the dhcpv6 protocol is enabled on the service port */
    return L7_FAILURE;
  }

  if(mode != simCfgData.systemIPv6ConfigMode)
  {
    simCfgData.systemIPv6ConfigMode = mode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    if (simCfgData.systemIpv6AdminMode == L7_ENABLE)
    {
      if (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_DHCP);
      }
      else if(simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_NONE)
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NONE);
      }
    }
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortConfigMode(void)
{
  return(simCfgData.servPortConfigMode);
}

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPv6ConfigMode(void)
{
  return(simCfgData.servPortIPv6ConfigMode);
}

/*********************************************************************
* @purpose  Get the Unit's Service Port Admin state
*
* @param    none
*
* @returns  mode  Service Port admin state
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortAdminState(void)
{
  return(simCfgData.servPortAdminState);
}

/*********************************************************************
* @purpose  Sets the Unit's Service Port Admin State
*
* @param    state  @b{(input)} Service Port Admin State
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortAdminState(L7_uint32 state)
{
  char ifname[20];
  L7_RC_t rc;

  if (usmDbComponentPresentCheck(0, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    sprintf(ifname, "%s0", bspapiServicePortNameGet());
    if (L7_ENABLE == state)
    {
      rc = ipstkMgmtIfUp(ifname);
    }
    else
    {
      rc = ipstkMgmtIfDown(ifname);
    }
    if (simCfgData.servPortAdminState != state)
    {
      simCfgData.servPortAdminState = state;
      simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
  }

}

/*********************************************************************
* @purpose  Get the Unit's Service Port Link state
*
* @param    none
*
* @returns  mode  Service Port link state
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortLinkState(void)
{
  char ifname[20];
  L7_BOOL linkState;

  sprintf(ifname, "%s0", bspapiServicePortNameGet());
  if (ipstkIfIsUp(ifname, &linkState) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return linkState;
}

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simServPortIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  *val = simCfgData.servPortIPv6AddrAutoConfig;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the ipv6 address autoconfiguration is enabled on the network port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simServPortIPv6AddrAutoConfigSet(L7_uint32 val)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uchar8 ifname[32];

  osapiSnprintf(ifname, sizeof(ifname), "%s%d", bspapiServicePortNameGet(),0);

  if((val == L7_ENABLE) &&
     ((simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
      (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
      (simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE)))
  {
    /* Don't allow ipv6 address autoconfiguration on service port if
     * (a) the dhcpv6 protocol is enabled on the service port (or)
     * (b) the dhcpv6 protocol is enabled on the network port (or)
     * (c) the ipv6 address autoconfiguration is enabled on the network port */
    return L7_FAILURE;
  }

  if(val != simCfgData.servPortIPv6AddrAutoConfig)
  {
    simCfgData.servPortIPv6AddrAutoConfig = val;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    osapiIfIpv6AddrAutoConfigSet(ifname, simCfgData.servPortIPv6AddrAutoConfig);

    /* If ipv6 mode is enabled on service port, disable and re-enable ipv6 mode to -
     *
     * (a) flush already autoconfigured ipv6 addresses (while disabling autoconfig mode)
     * (b) trigger the stack to send router solicitation packet out to be able
     *     to quickly facilitate autoconfiguration of ipv6 addresses (while enabling autoconfig mode)
     */
    if(simCfgData.servPortIpv6AdminMode == L7_ENABLE)
    {
      if (osapiIfIpv6Disable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to disable ipv6 mode on Service Port while changing ipv6 autoconfig mode.\n");
      }
      if (osapiIfIpv6Enable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to enable ipv6 mode on Service Port while changing ipv6 autoconfig mode.\n");
      }
    }
  }
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Current Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortCurrentConfigMode(void)
{
  return(servPortCurrentConfigMode);
}

/*********************************************************************
* @purpose  Sets the Unit's Service Port Config Mode
*
* @param    mode  @b{(input)} Service Port Config Mode
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortConfigMode(L7_uint32 mode)
{
  simCfgData.servPortConfigMode = mode;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Sets the Unit's Service Port Config Mode
*
* @param    mode  @b{(input)} Service Port Config Mode
*
* @returns L7_FAILURE    When any of this is true
*                        - the ipv6 address autoconfiguration is enabled on the service port
*                        - the ipv6 address autoconfiguration is enabled on the network port
*                        - the dhcpv6 protocol is enabled on the network port
*          L7_SUCCESS    otherwise
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetServPortIPv6ConfigMode(L7_uint32 mode)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if((mode == L7_SYSCONFIG_MODE_DHCP) &&
     ((simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE) ||
      (simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE) ||
      (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)))
  {
   /* Don't allow DHCPv6 client mode to be set on service port if
     * (a) the ipv6 address autoconfiguration is enabled on the service port (or)
     * (b) the ipv6 address autoconfiguration is enabled on the network port (or)
     * (b) the dhcpv6 protocol is enabled on the network port */
    return L7_FAILURE;
  }

  if(mode != simCfgData.servPortIPv6ConfigMode)
  {
    simCfgData.servPortIPv6ConfigMode = mode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    if (simCfgData.servPortIpv6AdminMode == L7_ENABLE)
    {
      if (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_DHCP);
      }
      else if(simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_NONE)
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_NONE);
      }
    }
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's Serial Port Baudrate
*
* @param    none
*
* @returns  baudRate Unit's Serial Port Baudrate
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSerialBaudRate(void)
{
  return(simCfgData.systemBaudRate);
}

/*********************************************************************
* @purpose  Sets the Unit's Serial Port Baudrate
*
* @param    baudRate   @b{(input)} Unit's Serial Port Baudrate
*
* @returns  L7_SUCCES
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSerialBaudRate(L7_uint32 baudRate)
{
  L7_RC_t rc = L7_FAILURE;

  rc = osapiBaudRateChange(baudRate);
  if ( rc == L7_SUCCESS )
  {
    simCfgData.systemBaudRate = baudRate;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the Unit's Serial Port TimeOut
*
* @param    none
*
* @returns  timeOut  Unit's Serial Port TimeOut
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSerialTimeOut(void)
{
  return(simCfgData.serialPortTimeOut);
}

/*********************************************************************
* @purpose  Sets the Unit's Serial Port TimeOut
*
* @param    timeOut    @b{(input)} Unit's Serial Port TimeOut
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSerialTimeOut(L7_uint32 timeOut)
{
  simCfgData.serialPortTimeOut = timeOut;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
}


/*********************************************************************
* @purpose  Sets the Serial Port Parameter
*
* @param    parm        @b{(input)} serial port parameter to be set
* @param    newValue    @b{(input)} new value of port parameter
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSerialPortParm(L7_uint32 parm, L7_uint32 newValue)
{
  switch (parm)
  {
  case SYSAPISERIALPORT_CHARSIZE:
    simCfgData.serialPortCharSize = newValue;
    break;

  case SYSAPISERIALPORT_FLOWCONTROL:
    simCfgData.serialPortFlowControl = newValue;
    break;

  case SYSAPISERIALPORT_PARITYTYPE:
    simCfgData.serialPortParityType = newValue;
    break;

  case SYSAPISERIALPORT_STOPBITS:
    simCfgData.serialPortStopBits = newValue;
    break;
  }

  simCfgData.cfgHdr.dataChanged = L7_TRUE;

}


/*********************************************************************
* @purpose  Get this unit number
*
* @param    none
*
* @returns  unitID      The stack ID for this unit
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetThisUnit()
{
  return(simCfgData.systemUnitID);
}

/*********************************************************************
*
* @purpose  Get the transfer mode
*
* @param    none
*
* @returns  System transfer mode of type enum L7_TRANSFER_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simGetTransferMode(void)
{
  return(simTransferInfo.systemTransferMode);
}

/*********************************************************************
*
* @purpose  Set the transfer mode
*
* @param    val   @b{(input)} Transfer mode of type enum L7_TRANSFER_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferMode(L7_uint32 val)
{
  simTransferInfo.systemTransferMode = val;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Tftp server's Ip Address type
*
* @param    val @b{(output)} tftp server ip address type
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferServerAddressType(L7_uint32 *val)
{
  *val =  simTransferInfo.systemTransferServerIp.family;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the Tftp server's Ip Address type
*
* @param    val @b{(input)} tftp server ip address type
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferServerAddressType(L7_uint32 val)
{
  simTransferInfo.systemTransferServerIp.family = (L7_uchar8)val;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Ip Address of the Tftp Server
*
* @param    val @b{(output)} tftp server ip address
*
* @returns  Ip Address of tftp server
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferServerIp(L7_uchar8 *val)
{
  L7_RC_t rc = L7_SUCCESS;

  if(L7_AF_INET == simTransferInfo.systemTransferServerIp.family)
  {
    memset(val, 0, sizeof(simTransferInfo.systemTransferServerIp.addr.ipv4));
    memcpy(val, &(simTransferInfo.systemTransferServerIp.addr.ipv4),
                sizeof(simTransferInfo.systemTransferServerIp.addr.ipv4));
  }
  else if(L7_AF_INET6 == simTransferInfo.systemTransferServerIp.family)
  {
    memset(val, 0, sizeof(simTransferInfo.systemTransferServerIp.addr.ipv6));
    memcpy(val, &(simTransferInfo.systemTransferServerIp.addr.ipv6),
                sizeof(simTransferInfo.systemTransferServerIp.addr.ipv6));
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set the Ip Address of the Tftp Server
*
* @param    val  @b{(input)} tftp server Ip Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferServerIp(L7_uchar8 *val)
{
  L7_RC_t rc = L7_SUCCESS;

  memset( (L7_uchar8 *)(&(simTransferInfo.systemTransferServerIp.addr)), 0,
          sizeof(simTransferInfo.systemTransferServerIp.addr) );

  if(L7_AF_INET == simTransferInfo.systemTransferServerIp.family)
  {
    memcpy(&(simTransferInfo.systemTransferServerIp.addr.ipv4), val,
           sizeof(simTransferInfo.systemTransferServerIp.addr.ipv4));
  }
  else if(L7_AF_INET6 == simTransferInfo.systemTransferServerIp.family)
  {
    memcpy(&(simTransferInfo.systemTransferServerIp.addr.ipv6), val,
           sizeof(simTransferInfo.systemTransferServerIp.addr.ipv6));
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the transfer file path for the local file
*
* @param    *buf  @b{(output)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFilePathLocal(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemTransferFilePathLocal);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Set the transfer file path for the local file
*
* @param    *buf  @b{(input)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFilePathLocal(L7_char8 *buf)
{
  memset( simTransferInfo.systemTransferFilePathLocal, 0, L7_MAX_FILEPATH+1 );

  if (strlen(buf) <= L7_MAX_FILEPATH)
    strcpy( simTransferInfo.systemTransferFilePathLocal, buf );
  else
    strncpy( simTransferInfo.systemTransferFilePathLocal, buf, (L7_MAX_FILEPATH) );

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the transfer file name for the local file
*
* @param    *buf  @b{(output)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFileNameLocal(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemTransferFileNameLocal);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the transfer file name for the local file
*
* @param    *buf  @b{(input)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFileNameLocal(L7_char8 *buf)
{
  memset( simTransferInfo.systemTransferFileNameLocal, 0, L7_MAX_FILENAME+1 );

  if (strlen(buf) <= L7_MAX_FILENAME)
    strcpy( simTransferInfo.systemTransferFileNameLocal, buf );
  else
    strncpy( simTransferInfo.systemTransferFileNameLocal, buf, (L7_MAX_FILENAME) );

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get the transfer file path for the remote file
*
* @param    *buf  @b{(output)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFilePathRemote(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemTransferFilePathRemote);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Set the transfer file path for the remote file
*
* @param    *buf  @b{(input)} transfer file Path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFilePathRemote(L7_char8 *buf)
{
  memset( simTransferInfo.systemTransferFilePathRemote, 0, L7_MAX_FILEPATH+1 );

  if (strlen(buf) <= L7_MAX_FILEPATH)
    strcpy( simTransferInfo.systemTransferFilePathRemote, buf );
  else
    strncpy( simTransferInfo.systemTransferFilePathRemote, buf, (L7_MAX_FILEPATH) );

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the transfer file name for the remote file
*
* @param    *buf  @b{(output)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFileNameRemote(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemTransferFileNameRemote);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the transfer file name for the remote file
*
* @param    *buf  @b{(input)} transfer file Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFileNameRemote(L7_char8 *buf)
{
  memset( simTransferInfo.systemTransferFileNameRemote, 0, L7_MAX_FILENAME+1 );

  if (strlen(buf) <= L7_MAX_FILENAME)
    strcpy( simTransferInfo.systemTransferFileNameRemote, buf );
  else
    strncpy( simTransferInfo.systemTransferFileNameRemote, buf, (L7_MAX_FILENAME) );

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the unit number for the transfer
*
* @param    *unit  @b{(output)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferUnitNumber(L7_uint32 *unit)
{
  *unit = simCfgData.systemTransferUnitNumber;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the unit number for the transfer
*
* @param    unit  @b{(input)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferUnitNumber(L7_uint32 unit)
{

  simCfgData.systemTransferUnitNumber = unit;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get the transfer upload file type
*
* @param    none
*
* @returns  Upload file type of enum L7_FILE_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simGetTransferUploadFileType(void)
{
  return(simTransferInfo.systemUploadFileType);
}


/*********************************************************************
*
* @purpose  Set the transfer upload file type
*
* @param    val  @b{(input)} Transfer upload file of type enum L7_FILE_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferUploadFileType(L7_uint32 val)
{
  simTransferInfo.systemUploadFileType = val;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get the transfer download file type
*
* @param    none
*
* @returns  Download file type of enum L7_FILE_TYPES_t
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simGetTransferDownloadFileType(void)
{
  return(simTransferInfo.systemDownloadFileType);
}

/*********************************************************************
*
* @purpose  Set the transfer download file type
*
* @param    val  @b{(input)} transfer download file of type enum L7_FILE_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferDownloadFileType(L7_uint32 val)
{
  simTransferInfo.systemDownloadFileType = val;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Get the status of the transfer
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL simTransferInProgressGet()
{
  return transferInProgress;
}

/*********************************************************************
*
* @purpose  Set the status of the transfer
*
* @param    val   @b{(input)} The context for the transfer of type L7_BOOL
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
void simTransferInProgressSet(L7_BOOL val)
{
  transferInProgress = val;
  return;
}

/*********************************************************************
*
* @purpose  Gets the context of the transfer
*
* @param    none
*
* @returns  pointer of the context for this transfer
*
* @comments
*
* @end
*
*********************************************************************/
void* simTransferContextGet()
{
  return transferContext;
}

/*********************************************************************
*
* @purpose  Set the trasfer context
*
* @param    *context @b{(input)} The context of the transfer
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
void simTransferContextSet(void *context)
{
  transferContext = context;
  return;
}

/*********************************************************************
*
* @purpose  Get the management VLAN ID
*
* @param    none
*
* @returns  management VLAN ID
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simMgmtVlanIdGet()
{
  if (simCfgData.systemMgmtVlanId)
  {
      return (simCfgData.systemMgmtVlanId);
  }
  return FD_SIM_DEFAULT_MGMT_VLAN_ID;
}

/*********************************************************************
*
* @purpose  Set the management VLAN ID
*
* @param    mgmtVlanId   @b{(input)} management vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments Assumes valid VLAN ID
*
* @end
*
*********************************************************************/
L7_RC_t simMgmtVlanIdSet(L7_uint32 mgmtVlanId)
{

  L7_uchar8 mac[6];
  L7_uint32 vlanId;
  L7_uint32 intIfNum;
  L7_RC_t   rc,rc1 = L7_FAILURE;
  DTL_MAC_TYPE_t mac_type;
  L7_uint32 ageTimeOld = FD_FDB_DEFAULT_AGING_TIMEOUT,fid;

  vlanId = simCfgData.systemMgmtVlanId;
  if ((mgmtVlanId < L7_DOT1Q_MIN_VLAN_ID) || (mgmtVlanId > L7_DOT1Q_MAX_VLAN_ID))
  {
    return L7_FAILURE;
  }

  /* Do not bother to change configuration if 
     the new value is equal to the old value */
  if (vlanId  == mgmtVlanId)
  {
      return L7_SUCCESS;
  }

  /*  Apply the configuration changes */

  rc =  nimFirstValidIntfNumberByType(L7_CPU_INTF, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(mac);
      mac_type = DTL_MAC_TYPE_BURNED_IN;
    }
    else
    {
      simGetSystemIPLocalAdminMac(mac);
      mac_type = DTL_MAC_TYPE_LOCAL_ADMIN;
    }

    rc = fdbSysMacAddEntry(mac, mgmtVlanId, intIfNum,
                          L7_FDB_ADDR_FLAG_MANAGEMENT);

    /* Get the age time of the old vlan */
    if((rc1 = fdbFdbIDGet(FD_SIM_DEFAULT_MGMT_VLAN_ID, &fid)) == L7_SUCCESS) 
    {
      ageTimeOld = fdbGetAddressAgingTimeOut(FD_SIM_DEFAULT_MGMT_VLAN_ID);
    }

    if (rc != L7_SUCCESS)
    {
      LOG_MSG("Error: SIM - could not add management MAC.\n");
    }
    else
    {
      rc = fdbSysMacDelEntry(mac, vlanId, intIfNum, mac_type);
      if (rc != L7_SUCCESS)
      {
        LOG_MSG("Error: SIM - could not delete management MAC.\n");
      }
      /* Set the age time for the new management vlan id. This is purely to update the config structure 
       * for the new managemnet vlan id, as the hardware has the same age time for all vlans.
       */
      if (rc1 == L7_SUCCESS)
      {
         (void) fdbSetAddressAgingTimeOut(ageTimeOld, mgmtVlanId);
      }
    }
    /* There is a change in mgmtVlanId */
    if (vlanId != mgmtVlanId)
    {
      if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_DHCP)
      {
        l7_dhcp_release();
        simSetSystemIPGateway(L7_NULL, L7_FALSE);
        simSetSystemIPAddr(L7_NULL, L7_FALSE);
        simSetSystemIPNetMask(L7_NULL);
        simSetSystemConfigMode(L7_SYSCONFIG_MODE_DHCP);
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    /*  Save the configuration changes */
    simCfgData.systemMgmtVlanId = mgmtVlanId;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the management Port
*
* @param    none
*
* @returns  management VLAN ID
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simMgmtPortGet(void)
{
  return (simCfgData.systemMgmtPortNum);
}

/*********************************************************************
*
* @purpose  Set the management Port
*
* @param    interface  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments Assumes valid VLAN ID
*
* @end
*
*********************************************************************/
L7_RC_t simMgmtPortSet(L7_uint32 interface)
{
  simCfgData.systemMgmtPortNum = interface;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To check if the route formed with the arguments is the same
*           as the local route formed by the serv port or network port
*           ip address and mask.
*
* @param    ipAddr    The network of the route
* @param    netmask   Netmask of the route
* @param    gateway   Gateway of the route
* @param    routeCmd  Route command of the route which is add, delete or modify
*
* @returns  L7_SUCCESS  The route is not the same
*           L7_FAILURE  The route is the same
*
* @notes    If the route is the same, the arguements are stored
*
* @end
*********************************************************************/
L7_RC_t simRouteModifyCheck(L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway,
                            L7_uint32 routeCmd)
{
  L7_IP_ADDR_t servPortIpAddr, netPortIpAddr;
  L7_IP_MASK_t servPortMask, netPortMask;
  L7_BOOL routeMatch;
  simRouteStorage_t * pStorage;
  L7_uint32 storageLen;


  /* Check for the service port and network port
   * ip address and mask. If the routing protocol route being modified
   * is same as the local route formed by the service port or network port
   * ip address and mask, it can corrupt the kernel route/arp tables. So
   * do not modify the routing protocol route.
   */
  servPortIpAddr = simGetServPortIPAddr();
  servPortMask = simGetServPortIPNetMask();
  netPortIpAddr = simGetSystemIPAddr();
  netPortMask = simGetSystemIPNetMask();
  routeMatch = L7_FALSE;
  pStorage = L7_NULLPTR;
  storageLen = 0;

  if (servPortIpAddr != L7_NULL_IP_ADDR)
  {
    if ((ipAddr & netmask)  == (servPortIpAddr & servPortMask))
    {
      pStorage =  servPortRoutingProtRoute;
      storageLen = sizeof(simRouteStorage_t);
      routeMatch = L7_TRUE;
    }
  }
  if ((routeMatch == L7_FALSE) &&(netPortIpAddr != L7_NULL_IP_ADDR))
  {
    if ((ipAddr & netmask)  == (netPortIpAddr & netPortMask))
    {
      pStorage =  netPortRoutingProtRoute;
      storageLen = sizeof(simRouteStorage_t);
      routeMatch = L7_TRUE;
    }
  }

  switch (routeCmd)
  {
  case SIM_ROUTE_MODIFY:
  case SIM_ROUTE_ADD:
    if (routeMatch == L7_TRUE)
    {
      pStorage->ipAddr = ipAddr;
      pStorage->netMask = netmask;
      pStorage->gateway = gateway;
      break;
    }
    break;
  case SIM_ROUTE_DELETE:
    if (routeMatch == L7_TRUE)
    {
      memset(pStorage, 0, storageLen);
      break;
    }
    break;
  default:
    break;
  }

  if (routeMatch == L7_TRUE)
    return L7_FAILURE;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Notify the address change event
*
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_t  addrType,
                                     L7_uchar8               addrFamily)
{
  L7_uint32 idx;

  for (idx = 0; idx < SIM_ADDR_CHANGE_REGISTRATIONS_MAX; idx++)
  {
    if (simAddrChangeNotifyList[idx].addrChangeCB != L7_NULLPTR &&
        simAddrChangeNotifyList[idx].addrType == addrType &&
        simAddrChangeNotifyList[idx].addrFamily == addrFamily)
    {
      if ((*simAddrChangeNotifyList[idx].addrChangeCB)(simAddrChangeNotifyList[idx].registrar_ID,
                                                     simAddrChangeNotifyList[idx].addrType,
                                                     simAddrChangeNotifyList[idx].addrFamily)
                                                     != L7_SUCCESS)
      {
        LOG_MSG("simAddrChangeEventNotify: Failed to notif registrar_ID %d addrType %d addrFamily %d",
                simAddrChangeNotifyList[idx].registrar_ID, simAddrChangeNotifyList[idx].addrType,
                simAddrChangeNotifyList[idx].addrFamily);
      }
    }/* End of match check */
  }/* End of search loop */
}
/*********************************************************************
* @purpose  Register a routine to be called when a system address
*           changes
*
* @param    registrar_ID  Routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
* @param    *notify       pointer to a routine to be invoked for address
*                         changes.  Each routine has the following parameters:
*                         registrar_ID, addrType, addrFamily
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simAddrChangeEventRegister(L7_COMPONENT_IDS_t      registrar_ID,
                                   SIM_ADDR_CHANGE_TYPE_t  addrType,
                                   L7_uchar8               addrFamily,
                                   SIM_ADDR_CHANGE_CB      addrChangeCB)
{
  L7_uint32 idx;
  L7_RC_t   rc = L7_FAILURE;

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("simAddrChangeEventRegister: Invalid registrat ID %d", registrar_ID);
  }
  else if (addrFamily != L7_AF_INET && addrFamily != L7_AF_INET6)
  {
    LOG_MSG("simAddrChangeEventRegister: Invalid address family");
  }
#if !defined(L7_IPV6_MGMT_PACKAGE) && !defined(L7_IPV6_PACKAGE)
  else if (addrFamily == L7_AF_INET6)
  {
    LOG_MSG("simAddrChangeEventRegister: Invalid address family");
  }
#endif
  else if (addrType >= SIM_ADDR_CHANGE_TYPE_TOTAL)
  {
    LOG_MSG("simAddrChangeEventRegister: Invalid address type");
  }
  else
  {
    for (idx = 0; idx < SIM_ADDR_CHANGE_REGISTRATIONS_MAX; idx++)
    {
      /* Found an empty slot ? */
      if (simAddrChangeNotifyList[idx].addrChangeCB == L7_NULLPTR)
      {
        simAddrChangeNotifyList[idx].addrChangeCB   = addrChangeCB;
        simAddrChangeNotifyList[idx].addrType       = addrType;
        simAddrChangeNotifyList[idx].addrFamily     = addrFamily;
        simAddrChangeNotifyList[idx].registrar_ID   = registrar_ID;
        rc = L7_SUCCESS;
        break;
      }
    } /*End of loop to search for empty slot */
  }
  return rc;
}
/*********************************************************************
* @purpose  Deregister a routine to be called when system address
*           changes
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simAddrChangeEventDeRegister(L7_COMPONENT_IDS_t      registrar_ID,
                                     SIM_ADDR_CHANGE_TYPE_t  addrType,
                                     L7_uchar8               addrFamily)
{
  L7_uint32 idx;
  L7_RC_t   rc = L7_FAILURE;

  for (idx = 0; idx < SIM_ADDR_CHANGE_REGISTRATIONS_MAX; idx++)
  {
    /* Found an empty slot ? */
    if (simAddrChangeNotifyList[idx].addrChangeCB != L7_NULLPTR &&
        simAddrChangeNotifyList[idx].addrType == addrType &&
        simAddrChangeNotifyList[idx].addrFamily == addrFamily &&
        simAddrChangeNotifyList[idx].registrar_ID == registrar_ID)
    {
      simAddrChangeNotifyList[idx].addrChangeCB   = L7_NULLPTR;
      rc = L7_SUCCESS;
    }
  } /*End of loop to search for empty slot */
  return rc;
}

/*********************************************************************
* @purpose  Get the Unit's system mac address which is currently in use.
*
* @param    *name  @b{(output)} system mac, length L7_ENET_MAC_ADDR_LEN
*
* @returns	L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simGetSystemMac(L7_uchar8 *mac)
{
  if(L7_NULLPTR == mac )
  {
    return L7_FAILURE;
  }

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    /* Burned in base MAC Address BIA */
    simGetSystemIPBurnedInMac(mac);
  }
  else
  {
    /* Locally Administrated base MAC Address */
    simGetSystemIPLocalAdminMac(mac);
  }

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Gets bytes transferred for the file
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferBytesCompletedGet(L7_uint32 *bytes)
{
  *bytes = simTransferInfo.systemBytesTransferred;
}

/*********************************************************************
 *
 * @purpose Sets bytes transferred for the file
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferBytesCompletedSet(L7_uint32 bytes)
{
  simTransferInfo.systemBytesTransferred = bytes;
}

/*********************************************************************
 *
 * @purpose Gets completion status for transfer
 *
 * @param completed
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferCompletionStatusGet(L7_BOOL *completed)
{
  *completed = simTransferInfo.systemTransferComplete;
}

/*********************************************************************
 *
 * @purpose Sets completion status for transfer
 *
 * @param completed
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferCompletionStatusSet(L7_BOOL completed)
{
  simTransferInfo.systemTransferComplete = completed;
}

/*********************************************************************
 *
 * @purpose Gets the direction of the transfer
 *
 * @param direction
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferDirectionGet(L7_uint32 *direction)
{
  *direction = simTransferInfo.systemTransferDirection;
}

/*********************************************************************
 *
 * @purpose Sets the direction of the transfer
 *
 * @param direction
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void simTransferDirectionSet(L7_uint32 direction)
{
  simTransferInfo.systemTransferDirection = direction;
}

/*********************************************************************
*
* @purpose Set the history buffer size for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t simSerialHistoryBufferSizeSet(L7_uint32 historyBufferSize)
{
  if (historyBufferSize > HISTORYBUFFER_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  simCfgData.serialHistoryBufferSize = historyBufferSize;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSerialHistoryBufferSizeGet(L7_uint32 *bufferSize)
{
  if (bufferSize == L7_NULL)
  {
    return L7_FAILURE;
  }

  *bufferSize = simCfgData.serialHistoryBufferSize;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  serial history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSerialHistoryBufferEnableSet(L7_uint32 enable)
{
  if ((enable != L7_ENABLE) && (enable != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  simCfgData.serialHistoryBufferEnable = enable;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  serial history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSerialHistoryBufferEnableGet(L7_uint32 *enable)
{
  if (enable == L7_NULL)
  {
    return L7_FAILURE;
  }

  *enable = simCfgData.serialHistoryBufferEnable;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the history buffer size for telnet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t simTelnetHistoryBufferSizeSet(L7_uint32 historyBufferSize)
{
  if (historyBufferSize > HISTORYBUFFER_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  simCfgData.telnetHistoryBufferSize = historyBufferSize;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simTelnetHistoryBufferSizeGet(L7_uint32 *bufferSize)
{
  if (bufferSize == L7_NULL)
  {
    return L7_FAILURE;
  }

  *bufferSize = simCfgData.telnetHistoryBufferSize;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Telnet history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simTelnetHistoryBufferEnableSet(L7_uint32 enable)
{
  if ((enable != L7_ENABLE) && (enable != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  simCfgData.telnetHistoryBufferEnable = enable;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Telnet history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simTelnetHistoryBufferEnableGet(L7_uint32 *enable)
{
  if (enable == L7_NULL)
  {
    return L7_FAILURE;
  }

  *enable = simCfgData.telnetHistoryBufferEnable;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the Timeout for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t simSshHistoryBufferSizeSet(L7_uint32 historyBufferSize)
{
  if (historyBufferSize > HISTORYBUFFER_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  simCfgData.sshHistoryBufferSize = historyBufferSize;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSshHistoryBufferSizeGet(L7_uint32 *bufferSize)
{
  if (bufferSize == L7_NULL)
  {
    return L7_FAILURE;
  }

  *bufferSize = simCfgData.sshHistoryBufferSize;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Ssh history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSshHistoryBufferEnableSet(L7_uint32 enable)
{
  if ((enable != L7_ENABLE) && (enable != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  simCfgData.sshHistoryBufferEnable = enable;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Ssh history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t simSshHistoryBufferEnableGet(L7_uint32 *enable)
{
  if (enable == L7_NULL)
  {
    return L7_FAILURE;
  }

  *enable = simCfgData.sshHistoryBufferEnable;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the username for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FTP Password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFTPUserName(L7_char8 *buf)
{
  memset(simTransferInfo.systemFTPServerUserName, 0, L7_MAX_FILEPATH+1);

  if (strlen(buf) <= L7_MAX_FILEPATH)
  {
    strcpy(simTransferInfo.systemFTPServerUserName, buf);
  }
  else
  {
    strncpy(simTransferInfo.systemFTPServerUserName, buf, (L7_MAX_FILEPATH));
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the username for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FTP Username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFTPUserName(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemFTPServerUserName);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the password for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) FPAssword for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferFTPPassword(L7_char8 *buf)
{
  memset(simTransferInfo.systemFTPServerPassword, 0, L7_MAX_FILEPATH+1);

  if (strlen(buf) <= L7_MAX_FILEPATH)
  {
    strcpy(simTransferInfo.systemFTPServerPassword, buf);
  }
  else
  {
    strncpy(simTransferInfo.systemFTPServerPassword, buf, (L7_MAX_FILEPATH));
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the password for FTP Server
*
* @param L7_uchar8 *buf      @b((output)) Password for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t simGetTransferFTPPassword(L7_char8 *buf)
{
  strcpy(buf, simTransferInfo.systemFTPServerPassword);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the Notify function to be called upon Transfer complete
*
* @param    void  (*notify)(L7_TRANSFER_STATUS_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSetTransferCompleteNotification(void (*notifyFuncPtr)(L7_TRANSFER_STATUS_t status))
{
  transferCompletionHandler = notifyFuncPtr;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the Notify function to be called upon Transfer complete
*
* @param    void  (*notify)(L7_TRANSFER_STATUS_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
void simTransferCompleteNotification(L7_TRANSFER_STATUS_t status)
{
  if (transferCompletionHandler != L7_NULLPTR)
  {
      (void)(*transferCompletionHandler)(status);
      transferCompletionHandler = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose  Get the status of Mgmt Access
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL simTransferMgmtAccessGet()
{
  return suspendMgmtAccess;
}

/*********************************************************************
*
* @purpose  Set the status of Mgmt Access
*
* @param    val   @b{(input)} The context for the transfer of type L7_BOOL
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
void simTransferMgmtAccessSet(L7_BOOL val)
{
  suspendMgmtAccess = val;
  return;
}
