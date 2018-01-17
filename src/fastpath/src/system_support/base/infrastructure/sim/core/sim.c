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
#include "trapapi.h"
#include "dhcp_client_api.h"
#if L7_FEAT_DNI8541_BLADESERVER
#include "bspapi_blade.h"
#endif

#include "ptin_globaldefs.h"
simCfgData_t      simCfgData;
simTransferInfo_t simTransferInfo;
L7_BOOL           transferInProgress;
L7_BOOL           suspendMgmtAccess;
void             *transferContext;
static L7_uint32 systemUpTimeDelta=0;
L7_char8         systemBIA[L7_MAC_ADDR_LEN];
L7_uint32      __ipcIpAddr = 0x00;


void* simImageSemaphore = L7_NULLPTR;


simAddrChangeNotifyList_t simAddrChangeNotifyList[SIM_ADDR_CHANGE_REGISTRATIONS_MAX];

#define          NUM_IPV6_ADDRS_FROM_STACK       L7_RTR6_MAX_INTF_ADDRS*2
#define          ADDR_CONFLICT_DETECT_TIMEOUT    15 /* 15 seconds */

/* As mentioned by iana (http://www.iana.org/assignments/port-numbers),
 * the well known ports are in the range 0 - 1023
 * the registered ports are in the range 1024 - 49151
 * the dynamic and/or private ports are in the range 49152 - 65535
 * We chose a random number 49160 in the private ports range and
 * assign it for address conflict detection purpose */
#define          ADDR_CONFLICT_DETECT_PORT       49160
#define          ADDR_CONFLICT_LOOPBACK_ADDR     0x7F000001

extern simRouteStorage_t  * servPortRoutingProtRoute;
extern simRouteStorage_t  * netPortRoutingProtRoute;
extern simOperInfo_t      * simOperInfo;

static transferCompletionHandlerFnPtr transferCompletionHandler = L7_NULLPTR;

/*********************************************************************
* @purpose  Set the Unit's system IP Address
*
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t __simSetIpcIpAddr(void)
{
  L7_RC_t   rc;
  
  rc = ipstkIfAddrGet(PTIN_IPC_IF_NAME, &__ipcIpAddr);
  if (rc != L7_SUCCESS)
  {
    /*Error Ocurred*/
    return rc;
  }  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's IPC IP Address
*
*
* @returns  ipAddr
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetIpcIpAddr(void)
{
  if (__ipcIpAddr == 0x00)
  {
    if (__simSetIpcIpAddr() != L7_SUCCESS)
    {
      return 0x00;
    }
  }
  return __ipcIpAddr;
}

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

        osapiStrncpy(simCfgData.summerTime.recurring.zone, (L7_char8 *)ptr,
                     sizeof(simCfgData.summerTime.recurring.zone));
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
        osapiStrncpy(simCfgData.summerTime.nonrecurring.zone, (L7_char8 *)ptr,
                     sizeof(simCfgData.summerTime.nonrecurring.zone));
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
  L7_uint32 dayOfWeek = 0;  /* actual day of nth day */
  L7_int32 start_cmp = 0;
  L7_int32 end_cmp = 0;

  /*Adjustment for time-zone */
  simTimeZoneOffsetGet(&minutes);
  timenow += (minutes*60);

  time_ptr = gmtime(&timenow);

  /* if recurring, take the month, week, day-of-week and time to determine start/end points */
  if (simCfgData.summerTime.mode == L7_SUMMER_TIME_RECURRING)
  {
    dayOfWeek = simFindDayOfWeek(time_ptr->tm_mday,
                                 time_ptr->tm_wday,
                                 simCfgData.summerTime.recurring.start_week-1,
                                 simCfgData.summerTime.recurring.start_day-1);

    start_cmp = simCompareDateAndTime(0, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                      0,
                                      simCfgData.summerTime.recurring.start_month,
                                      dayOfWeek,
                                      simCfgData.summerTime.recurring.start_time);

    dayOfWeek = simFindDayOfWeek(time_ptr->tm_mday,
                                 time_ptr->tm_wday,
                                 simCfgData.summerTime.recurring.end_week-1,
                                 simCfgData.summerTime.recurring.end_day-1);

    /* Adjust end_time by the offset to properly identify when to switch back.  Can't use current adjusted time. */
    end_cmp = simCompareDateAndTime(0, time_ptr->tm_mon+1, time_ptr->tm_mday, time_ptr->tm_hour * 60 + time_ptr->tm_min,
                                    0,
                                    simCfgData.summerTime.recurring.end_month,
                                    dayOfWeek,
                                    simCfgData.summerTime.recurring.end_time - simCfgData.summerTime.recurring.offset_minutes);

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
* @param    prefixCheckOnly Flag to indicate check the prefix part only
*                           or the complete address for conflict
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with service port ip6 addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simServPortIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr,
                                            L7_uint32 ip6PrefixLen,
                                            L7_BOOL prefixCheckOnly)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i = 0, count = NUM_IPV6_ADDRS_FROM_STACK;
  L7_uint32 pref_min;
  L7_in6_addr_t a1,a2;
  L7_in6_prefix_t ip6AddrFromStack[NUM_IPV6_ADDRS_FROM_STACK];
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE];

  /* Check for conflict with service port config */
  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      continue;
    }
    if(prefixCheckOnly)
    {
    pref_min = (ip6PrefixLen < simCfgData.servPortIpv6Intfs[i].ip6PrefixLen)?
                    ip6PrefixLen:simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
    }
    else
    {
      pref_min = ip6PrefixLen;
    }
    a1 = *ip6Addr;
    simIP6MaskApply(&a1,pref_min);
    a2 = simCfgData.servPortIpv6Intfs[i].ip6Addr;
    simIP6MaskApply(&a2,pref_min);
    if(memcmp(&a1,&a2, 16) == 0)
      return L7_FAILURE;
  }

  sprintf(ifName,"%s%d",bspapiServicePortNameGet(), bspapiServicePortUnitGet());
  rc =  osapiIfIpv6AddrsGet(ifName, ip6AddrFromStack,&count);
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
* @param    prefixCheckOnly Flag to indicate check the prefix part only
*                           or the complete address for conflict
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with network port ip6 addresses
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simSystemIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr,
                                          L7_uint32 ip6PrefixLen,
                                          L7_BOOL prefixCheckOnly)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i = 0, count = NUM_IPV6_ADDRS_FROM_STACK;
  L7_uint32 pref_min;
  L7_in6_addr_t a1,a2;
  L7_in6_prefix_t ip6AddrFromStack[NUM_IPV6_ADDRS_FROM_STACK];
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE];

  /* Check for conflict with network port config */
  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
      continue;
    }
    if(prefixCheckOnly)
    {
    pref_min = (ip6PrefixLen < simCfgData.systemIpv6Intfs[i].ip6PrefixLen)?
                    ip6PrefixLen:simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
    }
    else
    {
      pref_min = ip6PrefixLen;
    }
    a1 = *ip6Addr;
    simIP6MaskApply(&a1,pref_min);
    a2 = simCfgData.systemIpv6Intfs[i].ip6Addr;
    simIP6MaskApply(&a2,pref_min);
    if(memcmp(&a1,&a2, 16) == 0)
      return L7_FAILURE;
  }

  sprintf(ifName,"%s%d",L7_DTL_PORT_IF,0);
  rc =  osapiIfIpv6AddrsGet(ifName, ip6AddrFromStack,&count);
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


/*********************************************************************
* @purpose  Get the System Mac Address
*
* @param    *sysMacAddr  @b{(output)} pointer to system mac address
*                              Length L7_MAC_ADDR_LEN
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simMacAddrGet(L7_uchar8 *sysMacAddr)
{/* PTin added */ //unsigned char m[8]={00, 06, 0x91, 07, 0xC9, 0xCE}; memcpy(sysMacAddr, m, 6); return;
  if (simGetSystemIPMacType() == L7_SYSMAC_LAA)
  {
    simGetSystemIPLocalAdminMac(sysMacAddr);
  }
  else
  {
    simGetSystemIPBurnedInMac(sysMacAddr);
  }
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
  memcpy(sysBIA, systemBIA, L7_MAC_ADDR_LEN);
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
    sprintf(ifname, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());
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

  sprintf(ifname, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
  if (ipstkIfIsUp(ifname, &linkState) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return linkState;
}

/*********************************************************************
* @purpose  Get the Unit's Service Port Config Mode
*
* @param    none
*
* @returns  mode  Service Port Current Config Mode
*
* @comments   This function is deprecated. I believe it is only used by
*             a dead web page. Use simGetServPortConfigMode().
*
* @end
*********************************************************************/
L7_uint32 simGetServPortCurrentConfigMode(void)
{
  return L7_SYSCONFIG_MODE_NONE;
}

/*********************************************************************
* @purpose  Set the protocol used to acquire an address on the service port.
*
* @param    mode  @b{(input)} Service port protocol
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetServPortConfigMode(L7_uint32 mode)
{
  if (simCfgData.servPortConfigMode == mode)
    return;

  if (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    /* Disabling DHCP on the service port. Release address. */
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_NONE,
                                  L7_MGMT_SERVICEPORT, L7_TRUE);
  }

  /* If the service port debug flag is set, do not reconfigure the */
  /* address otherwise the debug connection will be interrupted.   */
  if (sysapiServicePortDebugFlagGet() == L7_FALSE)
  {
    /* If address is checkpointed, retain it. If service port address is
     * configured, the configured addresses will overwrite the checkpointed
     * address. If enabling DHCP, DHCP will request checkpointed address. */
    if (!simIsServPortAddrCheckpointed())
    {
      if (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_NONE)
      {
        /* Clear configured address */
        /* ROBRICE - could leave configured address in place. If DHCP is
         * enabled, will use configured address and send INFORM to get
         * other parameters. */
        simServPortIPGatewayConfigure(0);
        simServPortIPAddrConfigure(0);
        simServPortIPNetMaskConfigure(0);
      }
      else if ((simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
               (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_BOOTP))
      {
        simSetServPortIPGateway(0);
        simSetServPortAddrWithMask(0, 0);
      }
    }
  }

  /* Update configuration */
  simCfgData.servPortConfigMode = mode;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  if (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_DHCP,
                                  L7_MGMT_SERVICEPORT, L7_FALSE);
  }
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
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE];

  snprintf(ifName, L7_NIM_IFNAME_SIZE, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
  if(mode != simCfgData.servPortIPv6ConfigMode)
  {
    simCfgData.servPortIPv6ConfigMode = mode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    if (simCfgData.servPortIpv6AdminMode == L7_ENABLE)
    {
      if (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_DHCP, ifName, 0);
      }
      else if(simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_NONE)
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_NONE, ifName, 0);
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

  rc = osapiBaudRateChange(baudRate, L7_TRUE);
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
L7_RC_t simTransferInProgressSet(L7_BOOL val)
{
  if (simGetTransferDownloadFileType() == L7_FILE_TYPE_CODE)
  {
    if (val == L7_TRUE)
    {
      if (simImageSemaTake(L7_NO_WAIT)!= L7_SUCCESS)
      {
        return L7_IMAGE_IN_USE;
      }
    }
    else if (val == L7_FALSE)
    {
      simImageSemaGive();
    }
    else
    {
      return L7_FAILURE;
    }
  }
  transferInProgress = val;
  return L7_SUCCESS;
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
  return simCfgData.systemMgmtVlanId;
}

/*********************************************************************
*
* @purpose  Set the management VLAN ID
*
* @param    mgmtVlanId   @b{(input)} proposed management vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE              
*           L7_ALREADY_CONFIGURED   VLAN ID is used internally
*
* @comments  The management VLAN ID is set to 0 in builds that do not
*            include the network port. At runtime, if the value is 0,
*            it can't be dynamically changed to a non-zero value. If the
*            value is non-zero, it can't be dynamically changed to zero.
*
* @end
*
*********************************************************************/
L7_RC_t simMgmtVlanIdSet(L7_uint32 mgmtVlanId)
{
  L7_uchar8 descr[L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR];
  L7_uchar8 mac[6];
  L7_uint32 vlanId;
  L7_uint32 intIfNum = 0;
  L7_RC_t   rc = L7_SUCCESS;
  L7_RC_t rc1 = L7_FAILURE;
  DTL_MAC_TYPE_t mac_type;
  L7_uint32 ageTimeOld = FD_FDB_DEFAULT_AGING_TIMEOUT;
  L7_uint32 fid;

  vlanId = simCfgData.systemMgmtVlanId;
  if ((mgmtVlanId != L7_DOT1Q_NULL_VLAN_ID) &&
      ((mgmtVlanId < L7_DOT1Q_MIN_VLAN_ID) || (mgmtVlanId > L7_DOT1Q_MAX_VLAN_ID)))
  {
    return L7_FAILURE;
  }

  /* Do not bother to change configuration if
     the new value is equal to the old value */
  if (vlanId == mgmtVlanId)
  {
      return L7_SUCCESS;
  }

  if (mgmtVlanId != L7_DOT1Q_NULL_VLAN_ID)
  {
    /* Check if this a Routing VLAN */
  #ifdef L7_ROUTING_PACKAGE
    if (dot1qVlanIntfVlanIdToIntIfNum(mgmtVlanId, &intIfNum) == L7_SUCCESS)
    {
      L7_uint32 mode = 0;
      if (ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          return L7_ERROR;
        }
      }
    }
  #endif /* L7_ROUTING_PACKAGE */
  
    /* Check if VLAN already in use internally, say for a port-based
     * routing interface. */
    if (dot1qInternalVlanUsageDescrGet(mgmtVlanId, descr) == L7_SUCCESS)
    {
      return L7_ALREADY_CONFIGURED;
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
      if ((rc1 = fdbFdbIDGet(FD_SIM_DEFAULT_MGMT_VLAN_ID, &fid)) == L7_SUCCESS)
      {
        ageTimeOld = fdbGetAddressAgingTimeOut(FD_SIM_DEFAULT_MGMT_VLAN_ID);
      }
  
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Error: SIM - could not add management MAC.\n");
      }
      else
      {
        rc = fdbSysMacDelEntry(mac, vlanId, intIfNum, mac_type);
        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                  "Error: SIM - could not delete management MAC.\n");
        }
        /* Set the age time for the new management vlan id. This is purely to update the config structure
         * for the new managemnet vlan id, as the hardware has the same age time for all vlans.
         */
        if (rc1 == L7_SUCCESS)
        {
          (void) fdbSetAddressAgingTimeOut(ageTimeOld, mgmtVlanId);
        }
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    /*  Save the configuration changes */
    simCfgData.systemMgmtVlanId = mgmtVlanId;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
    simNetworkPortStatusUpdate();
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
void simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_t  addrType,
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
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "simAddrChangeEventNotify: Failed to notif registrar_ID %d addrType %d addrFamily %d",
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
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "simAddrChangeEventRegister: Invalid registrat ID %d", registrar_ID);
  }
  else if (addrFamily != L7_AF_INET && addrFamily != L7_AF_INET6)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "simAddrChangeEventRegister: Invalid address family");
  }
#if !defined(L7_IPV6_MGMT_PACKAGE) && !defined(L7_IPV6_PACKAGE)
  else if (addrFamily == L7_AF_INET6)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "simAddrChangeEventRegister: Invalid address family");
  }
#endif
  else if (addrType >= SIM_ADDR_CHANGE_TYPE_TOTAL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "simAddrChangeEventRegister: Invalid address type");
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
* @returns  L7_SUCCESS
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

/*********************************************************************
*
* @purpose  Get the stack up time
*
* @param    none
*
* @returns  stack up time in seconds
*
* @comments  Stack up time is the time since the stack performed a cold
*            restart. Stack up time does not reset on a warm restart.
*
* @end
*
*********************************************************************/
L7_uint32 simSystemUpTimeGet(void)
{
  return (osapiUpTimeRaw() + systemUpTimeDelta);
}

/*********************************************************************
*
* @purpose  Get the stack up time in milliseconds
*
* @param    none
*
* @returns  stack up time (milliseconds)
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 simSystemUpTimeMsecGet(void)
{
  return (osapiTimeMillisecondsGet() + (1000 * systemUpTimeDelta));
}

/*********************************************************************
*
* @purpose  Set the System Up Time
*
* @param    systemUpTime   @b{(input)} The system up time
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simSystemUpTimeSet(L7_uint32 systemUpTime)
{
  systemUpTimeDelta = (systemUpTime - osapiUpTimeRaw());
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Adjust the System Up Time
*
* @param    resetType   @b{(input)} Event causing the systemUpTime adjustment
*
* @returns  L7_SUCCESS
*
* @comments Upon election of a new manager, the system up time is bumped
*           by 5 seconds to ensure the clock doesn't drift backwards.
*           On cold restart, it is set to zero to reflect the time since
*           the last cold restart.
*
* @end
*
*********************************************************************/
L7_RC_t simSystemUpTimeAdjust(L7_RESET_TYPE resetType)
{
  if (resetType == L7_WARMRESET)
  {
    systemUpTimeDelta +=5;
  }
  else
  {
    systemUpTimeDelta = (0 - osapiUpTimeRaw());
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clears the IP stack's ARP cache entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All the IP stack's ARP cache entries are cleared.
*
* @end
*********************************************************************/
L7_RC_t simArpSwitchClear(void)
{
  L7_uchar8 ifname[20];
  L7_uint32 ipAddress=0;
  L7_uint32 ipNetMask=0;


  ipAddress = simGetSystemIPAddr();
  ipNetMask = simGetSystemIPNetMask();
  if ((ipAddress != 0) && (ipNetMask != 0))
  {
    sprintf(ifname, "%s0", L7_DTL_PORT_IF);
    osapiArpFlush(ifname);
  }

  if(cnfgrIsComponentPresent(L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
     ipAddress = simGetServPortIPAddr();
     ipNetMask = simGetServPortIPNetMask();
     if ((ipAddress !=0) && (ipNetMask != 0))
     {
        sprintf(ifname, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
        osapiArpFlush(ifname);
     }
  }
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose   Process the conflicting ARP entry received on mgmt ports
*
* @param     arp_packet @b{(input)} ARP packet
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void checkAddressConflict(L7_ether_arp_t *arp_packet)
{
  L7_IP_ADDR_t        senderIP;
  L7_uchar8           *mac = arp_packet->arp_sha, portMac[L7_ENET_MAC_ADDR_LEN];
  L7_MGMT_PORT_TYPE_t portType;

  memcpy(&senderIP, arp_packet->arp_spa, sizeof(L7_IP_ADDR_t));
  senderIP = osapiNtohl(senderIP);
  if(senderIP == 0)
  {
    /* Could be ARP probes. Don't handle them */
    return;
  }

  memset(portMac, 0, sizeof(portMac));
  if(simOperInfo->systemIpAddress == senderIP)
  {
    portType = L7_MGMT_NETWORKPORT;
    simMacAddrGet(portMac);
  }
  else if(simOperInfo->servPortIpAddress == senderIP)
  {
    portType = L7_MGMT_SERVICEPORT;
    simGetServicePortBurnedInMac(portMac);
  }
  else
  {
    /* Wrong packet reported to be conflicting one */
    return;
  }

  if (memcmp(portMac, mac, L7_ENET_MAC_ADDR_LEN) != 0)
  {
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 macAddrStr[20];

      osapiSnprintf(macAddrStr,20,"%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                    *mac, *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5));
      osapiInetNtoa(senderIP, ipAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_SIM_COMPONENT_ID,
              "IP address conflict on %s for IP address %s."
              " Conflicting host MAC address is %s",
              ((portType == L7_MGMT_SERVICEPORT) ? "Service Port" : "Network Port"),
              ipAddrStr, macAddrStr);

      if(portType == L7_MGMT_SERVICEPORT)
      {
        simOperInfo->servicePortConflictStats.numOfConflictsDetected++;
      }
      else
      {
        simOperInfo->networkPortConflictStats.numOfConflictsDetected++;
      }
      simIPAddrConflictDetectIPMacSet(senderIP, mac);

      /* Send trap via SNMP agent */
      trapMgrSwitchIpAddrConflictTrap(senderIP, mac);

      simIPAddrConflictNotify(portType, senderIP, mac);
  }
}

/*********************************************************************
*
* @purpose   Task to poll the stack for ip address conflicts on
*            the management interface addresses
*
* @param void
*
* @returns void
*
* @notes     Stack (Linux and VxWorks) send the conflicting ARP
*            packets received on mgmt ports (network port or service port)
*            as is to the user to log the error condition.
*
* @end
*
*********************************************************************/
void simAddrConflictTask(void)
{
  L7_uint32 rc = 0, fromLen = 0;
  L7_int32 cFd  = -1, maxFd, optionSetOn = 1, bytesRecvd = 0;
  fd_set    readFds;
  L7_sockaddr_in_t   baddr, fromAddr;
  L7_ether_arp_t arp_packet;

  fromLen = sizeof(fromAddr);
  /* poll for address conflict events (active or passive) on service port */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &cFd) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to create address conflict detect socket - errno %s", strerror(osapiErrnoGet()));
    return;
  }

  if (osapiSocketNonBlockingModeSet(cFd, L7_TRUE) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to set option NONBLOCK on address conflict detect socket");
    osapiSocketClose(cFd);
    return;
  }

  if (osapiSetsockopt(cFd, L7_SOL_SOCKET, L7_SO_REUSEADDR,
                      (L7_char8 *) &optionSetOn, sizeof(optionSetOn)) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to set option REUSEADDR on address conflict detect socket");
    osapiSocketClose(cFd);
    return;
  }

  memset(&baddr, 0, sizeof(baddr));
  baddr.sin_family      = L7_AF_INET;
  baddr.sin_port        = osapiHtons(ADDR_CONFLICT_DETECT_PORT);
  baddr.sin_addr.s_addr = osapiHtonl(ADDR_CONFLICT_LOOPBACK_ADDR);
  if (osapiSocketBind(cFd, (L7_sockaddr_t *)&baddr, sizeof(baddr)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
            "Socket Bind Error on address conflict detect port");
    osapiSocketClose(cFd);
    return;
  }

  do
  {
    FD_ZERO(&readFds);
    FD_SET(cFd, &readFds);
    maxFd = cFd + 1;

    rc = osapiSelect(maxFd, &readFds, L7_NULL, L7_NULL, ADDR_CONFLICT_DETECT_TIMEOUT, 0);
    if(rc <= 0)
    {
      continue;
    }

    if (osapiSocketRecvfrom(cFd, (L7_char8*) &arp_packet, sizeof(L7_ether_arp_t), 0,
                            (L7_sockaddr_t *)&fromAddr, &fromLen, &bytesRecvd)  != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
              "Failed to read conflicting ARP packet from stack - errno %s", strerror(osapiErrnoGet()));
      continue;
    }

    checkAddressConflict(&arp_packet);
  } while (1);
}

/*********************************************************************
*
* @purpose  Trigger the Active Address Conflict Detection
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectRun(void)
{
  simAddrConflictDetectApply();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Clear the Address Conflict Detection Status
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectStatusClear(void)
{
  memset(&(simOperInfo->conflictStatus), 0, sizeof(simAddrConflictStatus_t));
  memset(&(simOperInfo->servicePortConflictStats), 0, sizeof(addrConflictStats_t));
  memset(&(simOperInfo->networkPortConflictStats), 0, sizeof(addrConflictStats_t));

#ifdef L7_ROUTING_PACKAGE
  ipMapAddrConflictStatsReset();
#endif

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Address Conflict Detection Status
*
* @param    conflictDetectStatus   @b{(output)} conflict detection Status
*
* @returns  L7_SUCCESS
*
* @comments status would be returned as L7_TRUE if there was at least
*           one conflict detected since last reset of the status.
*           Else status is returned as L7_FALSE.
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectStatusGet(L7_BOOL *conflictDetectStatus)
{
  *conflictDetectStatus = simOperInfo->conflictStatus.conflictDetectStatus;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the IP Address of the last detected address Conflict
*
* @param    conflictIP   @b{(output)} last detected conflicting IP
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectIPGet(L7_uint32 *conflictIP)
{
  *conflictIP = simOperInfo->conflictStatus.conflictIPAddr;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the IP Address of the last detected address Conflict
*
* @param    conflictIP   @b{(output)} last detected conflicting IP
* @param    conflictMAC  @b{(output)} MAC of last detected
*                                     conflicting host
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictDetectIPMacSet(L7_uint32 conflictIP,
                                        L7_uchar8 *conflictMAC)
{
  simOperInfo->conflictStatus.conflictIPAddr       = conflictIP;
  memcpy(simOperInfo->conflictStatus.conflictMacAddr, conflictMAC, L7_MAC_ADDR_LEN);
  simOperInfo->conflictStatus.conflictReportedTime = simSystemUpTimeGet();
  simOperInfo->conflictStatus.conflictDetectStatus = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the MAC Address of the last detected address conflict
*
* @param    conflictMAC   @b{(output)} MAC of last detected
*                                      conflicting host
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectMACGet(L7_uchar8 *conflictMAC)
{
  memcpy(conflictMAC, simOperInfo->conflictStatus.conflictMacAddr, L7_MAC_ADDR_LEN);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the time in seconds since the last address conflict
*           was detected
*          
* @param    conflictDetectTime   @b{(output)} time since the last
*                                             detected conflict
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictLastDetectTimeGet(L7_uint32 *conflictDetectTime)
{
  *conflictDetectTime = 0;
  if(simOperInfo->conflictStatus.conflictDetectStatus == L7_TRUE)
  {
    *conflictDetectTime = (simSystemUpTimeGet() -
                           simOperInfo->conflictStatus.conflictReportedTime);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Register for the address conflict on mgmt interface(s)
*
* @param    cbFunc   @b{(input)} callback function
*
* @returns  L7_SUCCESS  If function was registered
* @returns  L7_FAILURE  If function was not registered
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simIPAddrConflictRegister(simIPAddrConflictCB_t cbFunc)
{
  L7_RC_t rc = L7_FAILURE;

  if(simOperInfo->simIPAddrConflictCallback == L7_NULL)
  {
    simOperInfo->simIPAddrConflictCallback = cbFunc;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Unregisters for address conflict on mgmt interface(s)
*
* @param    cbFunc   @b{(input)} Callback function
*
* @returns  L7_SUCCESS  If function was unregistered
* @returns  L7_FAILURE  If function was not unregistered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simIPAddrConflictUnregister(simIPAddrConflictCB_t cbFunc)
{
  L7_RC_t rc = L7_FAILURE;

  if(simOperInfo->simIPAddrConflictCallback == cbFunc)
  {
    simOperInfo->simIPAddrConflictCallback = L7_NULL;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Notify registrants of IP address conflict event
*           on network port or service port
*
* @param    portType  @{(input)} type of the mgmt port
* @param    ipAddr    @{(input)} Conflicting IP on this interface
* @param    macAddr   @{(input)} Conflicting host's MAC address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void simIPAddrConflictNotify(L7_MGMT_PORT_TYPE_t portType,
                             L7_uint32 ipAddr,
                             L7_uchar8 *macAddr)
{
  if(simOperInfo->simIPAddrConflictCallback)
  {
    simOperInfo->simIPAddrConflictCallback(portType, ipAddr, macAddr);
  }
}
/*********************************************************************
*
* @purpose  Get the transfer semaphore
*
* @param    wait         wait time
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simImageSemaTake(L7_uint32 wait)
{
  return osapiSemaTake(simImageSemaphore,wait);
}

/*********************************************************************
*
* @purpose  Release the semaphore
*
* @param    None
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t simImageSemaGive()
{
  return osapiSemaGive(simImageSemaphore);
}

/*********************************************************************
* @purpose  Set the CPU Free memory threshold for monitoring
*
* @param    threshold {(input)} Free memory threshold in KB. A value of 0 
*                               indicates that threshold monitoring should be
*                               stopped.
*
* @returns L7_SUCCESS If threshold was successfully set
*          L7_REQUEST_DENIED If the threshold is configured to be more than the
*                            total available memory
*          L7_FAILURE Other errors
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t simCpuFreeMemoryThresholdSet(L7_uint32 threshold)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 numBytesTotal = 0, numBytesAlloc = 0, numBytesFree = 0;

  do
  {
    if (threshold == simCfgData.memoryMonitorData.freeMemoryThreshold)
    {
      break;
    }

    /* Get the free memory */
    if ((rc = osapiGetMemInfo(&numBytesTotal,&numBytesAlloc,&numBytesFree)) != L7_SUCCESS)
    {
      break;  
    }

    if (numBytesTotal < (threshold * 1024))
    {
      /* Free memory threshold cannot be greater than total CPU memory */
      rc = L7_REQUEST_DENIED;
      break;  
    }

    simCfgData.memoryMonitorData.freeMemoryThreshold = threshold;
    simOperInfo->simFreeMemoryTrapSent = L7_FALSE;  

  } while (0);

  if (rc == L7_SUCCESS)
  {
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}


/*********************************************************************
* @purpose  Get the configured CPU Free memory threshold
*
* @param    threshold {(output)} Free memory threshold in KB. A value of 0 
*                                indicates that threshold monitoring should be
*                                stopped.
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t simCpuFreeMemoryThresholdGet(L7_uint32 *threshold)
{
  L7_RC_t rc = L7_SUCCESS;

  *threshold = simCfgData.memoryMonitorData.freeMemoryThreshold;

  return rc;
}

/*********************************************************************
* @purpose  Monitor free memory and generate traps as required.
*
* @param    none
*
* @returns  void
*
* @notes    Called periodically by the osapiMonitorTask. Generates trap
*           if free memory falls below the user configured threshold. Also,
*           a snapshot of the total memory allocated by each component and
*           details of last x allocations are recorded in persistent memory.
*           Another trap is generated when the free memory has recovered
*           SIM_FREE_MEMORY_RECOVERED_THRESHOLD % over the configured threshold.
*
* @end
*********************************************************************/
void simMemoryMonitor(void)
{
  L7_uint32 thresholdBytes, recoveredBytes;
  L7_uint32 numBytesTotal = 0, numBytesAlloc = 0, numBytesFree = 0;

  thresholdBytes = simCfgData.memoryMonitorData.freeMemoryThreshold * 1024;

  /* If Free memory monitoring is not configured, then return */
  if (thresholdBytes == 0)
  {
    return;      
  }

  /* Get the free memory */
  if (osapiGetMemInfo(&numBytesTotal,&numBytesAlloc,&numBytesFree) != L7_SUCCESS)
  {
    return;
  }


  /* Free memory is below threshold, generate a trap if it has not been raised */    
  if ((numBytesFree < (thresholdBytes)) &&
      (simOperInfo->simFreeMemoryTrapSent == L7_FALSE))
  {
    /* Add code to generate trap here */

    trapMgrSwitchCpuFreeMemBelowThresholdTrap((thresholdBytes/1024), 
                                              (numBytesAlloc/1024),
                                              (numBytesFree/1024));
    simOperInfo->simFreeMemoryTrapSent = L7_TRUE;
    
  }

  /* If free memory had fallen below the threshold and has now recovered, then
   * generate another trap.
   */
  if ((simOperInfo->simFreeMemoryTrapSent == L7_TRUE) &&
      (numBytesFree >= thresholdBytes))
  {
    recoveredBytes = numBytesFree - thresholdBytes;
    if (recoveredBytes >= (SIM_FREE_MEMORY_RECOVERED_THRESHOLD * (thresholdBytes)))
    {
      /* Add code to generate trap here */    

      trapMgrSwitchCpuFreeMemAboveThresholdTrap((thresholdBytes/1024), 
                                                (numBytesAlloc/1024),
                                                (numBytesFree/1024));
      simOperInfo->simFreeMemoryTrapSent = L7_FALSE;
    }
  }

  return;
}


/*********************************************************************
* @purpose  Set a CPU Util monitor parameter
*
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(input)} Value of the parameter
*
* @returns L7_SUCCESS If parameter was successfully set
*          L7_FAILURE Other errors
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t simCpuUtilMonitorParamSet(simCpuUtilMonitorParam_t paramType,
                                  L7_uint32 paramVal)
{
  L7_uint32 oldRisingPeriod;
  L7_BOOL   update = L7_FALSE;
  L7_RC_t   rc = L7_SUCCESS;
  
  if (paramType >= SIM_CPU_UTIL_MONITOR_LAST_PARAM)
  {
    return L7_FAILURE;    
  }

  oldRisingPeriod = simCfgData.utilMonitorData.risingPeriod;

  switch (paramType)
  {
    case SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM:
      if (paramVal != simCfgData.utilMonitorData.risingThreshold)
      {
        update = L7_TRUE;    
        simCfgData.utilMonitorData.risingThreshold = paramVal;
      }

      /* If the falling threshold has not been set or is greater than rising,
       * then set it to rising threshold
       */
      if ((simCfgData.utilMonitorData.fallingThreshold > 
           simCfgData.utilMonitorData.risingThreshold) ||
          (simCfgData.utilMonitorData.fallingThreshold == 0))
      {
        simCfgData.utilMonitorData.fallingThreshold = 
                                      simCfgData.utilMonitorData.risingThreshold;    
      }

      break;

    case SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM:
      if (paramVal != simCfgData.utilMonitorData.risingPeriod)
      {
        update = L7_TRUE;    
        simCfgData.utilMonitorData.risingPeriod = paramVal;
      }

      /* If the falling period has not been set then set it to rising period */
      if (simCfgData.utilMonitorData.fallingPeriod == 0) 
      {
        simCfgData.utilMonitorData.fallingPeriod = 
                                      simCfgData.utilMonitorData.risingPeriod;    
      }


      break;

    case SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM:
      if (paramVal != simCfgData.utilMonitorData.fallingThreshold)
      {
        update = L7_TRUE;    
        simCfgData.utilMonitorData.fallingThreshold = paramVal;
      }

      /* If the falling threshold has become greater than rising, then set the
       * rising same as falling. 
       */
      if ((simCfgData.utilMonitorData.fallingThreshold > 
           simCfgData.utilMonitorData.risingThreshold) &&
          (simCfgData.utilMonitorData.risingThreshold != 0))
      {
        simCfgData.utilMonitorData.risingThreshold = 
                                      simCfgData.utilMonitorData.fallingThreshold;    
      }

      break;

    case SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM:
      if (paramVal != simCfgData.utilMonitorData.fallingPeriod)
      {
        update = L7_TRUE;    
        simCfgData.utilMonitorData.fallingPeriod = paramVal;
      }
      break;
   
    default:
      rc = L7_FAILURE;
      break;
  }

  if ((rc == L7_SUCCESS) && (update == L7_TRUE))
  {
    /* Remove the old configuration */
    if (oldRisingPeriod != 0)
    {
      sysapiTotalCpuUtilTablePeriodRemove(oldRisingPeriod);
    }


    if ((simCfgData.utilMonitorData.risingPeriod != 0) &&
        (simCfgData.utilMonitorData.risingThreshold != 0) &&
        (simCfgData.utilMonitorData.fallingThreshold != 0) &&
        (simCfgData.utilMonitorData.fallingPeriod != 0))
    {

      rc = sysapiTotalCpuUtilTablePeriodInsert(simCfgData.utilMonitorData.risingPeriod,
                                               simCfgData.utilMonitorData.risingThreshold,
                                               simCfgData.utilMonitorData.fallingPeriod,
                                               simCfgData.utilMonitorData.fallingThreshold);
    }
  }

  if (rc == L7_SUCCESS)
  {
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}


/*********************************************************************
* @purpose  Get a CPU Util monitor parameter
*
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(output)} Value of the parameter
*
* @returns L7_SUCCESS 
*          L7_FAILURE 
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t simCpuUtilMonitorParamGet(simCpuUtilMonitorParam_t paramType,
                                  L7_uint32 *paramVal)
{
  L7_RC_t rc = L7_SUCCESS;
  
  if (paramType >= SIM_CPU_UTIL_MONITOR_LAST_PARAM)
  {
    return L7_FAILURE;    
  }

  switch (paramType)
  {
    case SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM:
      *paramVal = simCfgData.utilMonitorData.risingThreshold;
      break;

    case SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM:
      *paramVal = simCfgData.utilMonitorData.risingPeriod;
      break;

    case SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM:
      *paramVal = simCfgData.utilMonitorData.fallingThreshold;
      break;

    case SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM:
     *paramVal = simCfgData.utilMonitorData.fallingPeriod;
     break;
   
    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}


/*********************************************************************
* @purpose  Unconfigure Cpu monitoring 
*
* @param    none
*
* @returns  none
*          
*
* @notes none
*
* @end
*********************************************************************/
void simCpuMonitorUnconfigure()
{
  simCpuUtilMonitorParamSet(SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM ,
                            FD_SIM_DEFAULT_CPU_UTIL_THRESHOLD);
  simCpuUtilMonitorParamSet(SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM ,
                            FD_SIM_DEFAULT_CPU_UTIL_THRESHOLD);
  simCpuUtilMonitorParamSet(SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM ,
                            FD_SIM_DEFAULT_CPU_UTIL_PERIOD);
  simCpuUtilMonitorParamSet(SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM ,
                            FD_SIM_DEFAULT_CPU_UTIL_PERIOD);
  simCpuFreeMemoryThresholdSet(FD_SIM_DEFAULT_FREE_MEM_THRESHOLD);

  return;
}

/*********************************************************************
* @purpose  Checks if Startup-config exists or not
*
* @param    none
*
* @returns  L7_TRUE if exists, 
*           L7_FALSE otherwise.
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL simStartupConfigIsExists ()
{
  return simOperInfo->startupConfigIsExists;
}

