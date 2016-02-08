/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/common/cli_show_time.c
 *
 * @purpose CLI commands for timezone related commands
 *
 * @component none
 *
 *
 * @create  05-Aug-08
 *
 * @author  drajendra
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "osapi_support.h"
#include "datatypes.h"                 /* for converting from IP to integer */
#include "filter_api.h"
#include "cli_web_user_mgmt.h"
#include "cli_web_util.h"
#include "clicommands_card.h"
#include "log_api.h"
#include "usmdb_log_api.h"
#include "sntp_exports.h"
#include "timezone_exports.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_sntp_api.h"
#include "usmdb_timezone_api.h"
#include "ews.h"

#include "cli_web_include.h"
#include "cli_web_mgr_api.h"
#include "unitmgr_api.h"
#include "session.h"

#include "clicommands_time.h"
#include "strlib_base_cli.h"
#include "strlib_common_common.h"

const L7_uchar8 *monthsOfYear[] = {
          "",
   "Jan",
   "Feb",
   "Mar",
   "Apr",
   "May",
   "Jun",
   "Jul",
   "Aug",
   "Sep",
   "Oct",
   "Nov",
   "Dec"
};

const L7_uchar8 *aggMonthsOfYear[] = {
          "",
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
};

const L7_uchar8 *daysOfWeek[] = {
          "",
    "Sunday",
    "Monday",
   "Tuesday",
 "Wednesday",
  "Thursday",
    "Friday",
  "Saturday"
};

const L7_uchar8 *weekName[] = {
         "",
    "first",
   "second",
    "third",
   "fourth",
     "last"
};

const L7_uchar8 *dayOfWeek[] = {
          "",
    "sun",
    "mon",
   "tue",
 "wed",
  "thu",
    "fri",
  "sat"
};

const L7_uint32 daysInMonth[] = {0, 31,28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*********************************************************************
*
* @purpose  display the time and date from the system clock
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show clock
*
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClock(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit = L7_CLI_ZERO;
  L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 currentTime;
  L7_SNTP_CLIENT_MODE_t clientMode;
  struct tm *LocalTime;
  L7_uint32 stMonth,stDay,stWeek,stHour,stMin,endMonth,endDay,endWeek,endHour,endMin,stYear,endYear;

  L7_int32 hours  = 0;
  L7_uint32 mode=0,offset=0;
  L7_uint32 minutes = 0;
  L7_char8 zoneName [L7_TIMEZONE_ACRONYM_LEN+1];
  /* validate arguments */
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  memset(zoneName,0x00,L7_TIMEZONE_ACRONYM_LEN+1);

  if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_RECURRING)
  {
    usmDbStRecurringGet (unit, L7_START_MONTH, (void *) &stMonth);
    usmDbStRecurringGet (unit, L7_START_DAY, (void *) &stDay) ;
    usmDbStRecurringGet (unit, L7_START_WEEK, (void *) &stWeek);
    usmDbStRecurringGet (unit, L7_START_HOUR, (void *) &stHour);
    usmDbStRecurringGet (unit, L7_START_MINUTE, (void *) &stMin);
    usmDbStRecurringGet (unit, L7_END_MONTH, (void *) &endMonth);
    usmDbStRecurringGet (unit, L7_END_DAY, (void *) &endDay);
    usmDbStRecurringGet (unit, L7_END_WEEK, (void *) &endWeek);
    usmDbStRecurringGet (unit, L7_END_HOUR, (void *) &endHour);
    usmDbStRecurringGet (unit, L7_END_MINUTE, (void *) &endMin);
    usmDbStRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset);
    usmDbStRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName);
  }
  else if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_NON_RECURRING)
  {
    usmDbStNonRecurringGet (unit, L7_START_MONTH, (void *) &stMonth);
    usmDbStNonRecurringGet (unit, L7_START_DAY, (void *) &stDay);
    usmDbStNonRecurringGet (unit, L7_START_YEAR, (void *) &stYear);
    usmDbStNonRecurringGet (unit, L7_START_HOUR, (void *) &stHour);
    usmDbStNonRecurringGet (unit, L7_START_MINUTE, (void *) &stMin);
    usmDbStNonRecurringGet (unit, L7_END_MONTH, (void *) &endMonth);
    usmDbStNonRecurringGet (unit, L7_END_DAY, (void *) &endDay);
    usmDbStNonRecurringGet (unit, L7_END_YEAR, (void *) &endYear);
    usmDbStNonRecurringGet (unit, L7_END_HOUR, (void *) &endHour);
    usmDbStNonRecurringGet (unit, L7_END_MINUTE, (void *) &endMin);
    usmDbStNonRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset);
    usmDbStNonRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName);
  }

  /* Display current time information */

  LocalTime = localtime((time_t *)&currentTime);

  currentTime = usmDbAdjustedTimeGet();
  usmDbTimeZoneOffsetGet(unit, &minutes);
  usmDbSystemTimeZoneAcronymGet(unit, zoneName);

  LocalTime = localtime((time_t *)&currentTime);
  memset(buffer, 0x00, L7_CLI_MAX_STRING_LENGTH);

  if (usmDbSummerTimeIsInEffect()) {
    minutes = minutes + offset;
  }

  /* carry over minutes to hours */
  if (hours < 0) {
    hours = hours - (minutes / L7_MINS_PER_HOUR);
  }
  else {
    hours = hours + (minutes / L7_MINS_PER_HOUR);
  }
  minutes = abs(minutes) % L7_MINS_PER_HOUR;

  sprintf(buffer, "\r\n%2.2d:%2.2d:%2.2d %s(UTC%+d:%02d) %s %d %d \r\n",
          LocalTime->tm_hour, LocalTime->tm_min, LocalTime->tm_sec, zoneName,
          hours, minutes, monthsOfYear[LocalTime->tm_mon+1], LocalTime->tm_mday,
          (LocalTime->tm_year + 1900));

  ewsTelnetWrite(ewsContext, buffer);

  usmDbSntpClientModeGet(unit, &clientMode);
  if (clientMode == L7_SNTP_CLIENT_DISABLED)
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_no_time_src);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_sntp_time_src);
  }
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose  display the time and date from the system clock
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show clock detail
*
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClockDetail(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit = L7_CLI_ZERO;
  L7_uint32 numArg = L7_CLI_ZERO;
  L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 currentTime;
  L7_SNTP_CLIENT_MODE_t clientMode;
  struct tm *LocalTime;
  L7_uint32 mode ;
  L7_uint32 stMonth=0,stDay=0,stWeek=0,stHour=0,stMin=0,endMonth=0,endDay=0,endWeek=0,endHour=0,endMin=0,stYear=0,endYear=0;


  L7_int32  hours = 0, tzHours = 0;
  L7_uint32  minutes = 0, tzMinutes = 0;
  L7_char8  zoneName [L7_TIMEZONE_ACRONYM_LEN+1],tzName[L7_TIMEZONE_ACRONYM_LEN+1];
  L7_char8  currZoneName [L7_TIMEZONE_ACRONYM_LEN+1];

  L7_char8 str[2*L7_CLI_MAX_STRING_LENGTH],tempStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 offset=0;


  memset (str, 0x00, 2*L7_CLI_MAX_STRING_LENGTH);
  memset (currZoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  memset (tzName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  zoneName [0] = '\0';
  tzName [0] = '\0';
  currZoneName [0] = '\0';
  /* validate arguments */
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();


  /* Error Checking for command Type */

  if (numArg != 0)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_base_clisyntax_showclockerror);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_RECURRING)
  {
    usmDbStRecurringGet (unit, L7_START_MONTH, (void *) &stMonth);
    usmDbStRecurringGet (unit, L7_START_DAY, (void *) &stDay) ;
    usmDbStRecurringGet (unit, L7_START_WEEK, (void *) &stWeek);
    usmDbStRecurringGet (unit, L7_START_HOUR, (void *) &stHour);
    usmDbStRecurringGet (unit, L7_START_MINUTE, (void *) &stMin);
    usmDbStRecurringGet (unit, L7_END_MONTH, (void *) &endMonth);
    usmDbStRecurringGet (unit, L7_END_DAY, (void *) &endDay);
    usmDbStRecurringGet (unit, L7_END_WEEK, (void *) &endWeek);
    usmDbStRecurringGet (unit, L7_END_HOUR, (void *) &endHour);
    usmDbStRecurringGet (unit, L7_END_MINUTE, (void *) &endMin);
    usmDbStRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset);
    usmDbStRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName);
  }
  else if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_NON_RECURRING)
  {
    usmDbStNonRecurringGet (unit, L7_START_MONTH, (void *) &stMonth);
    usmDbStNonRecurringGet (unit, L7_START_DAY, (void *) &stDay);
    usmDbStNonRecurringGet (unit, L7_START_YEAR, (void *) &stYear);
    usmDbStNonRecurringGet (unit, L7_START_HOUR, (void *) &stHour);
    usmDbStNonRecurringGet (unit, L7_START_MINUTE, (void *) &stMin);
    usmDbStNonRecurringGet (unit, L7_END_MONTH, (void *) &endMonth);
    usmDbStNonRecurringGet (unit, L7_END_DAY, (void *) &endDay);
    usmDbStNonRecurringGet (unit, L7_END_YEAR, (void *) &endYear);
    usmDbStNonRecurringGet (unit, L7_END_HOUR, (void *) &endHour);
    usmDbStNonRecurringGet (unit, L7_END_MINUTE, (void *) &endMin);
    usmDbStNonRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset);
    usmDbStNonRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName);
  }

  /* Display current time information */

  LocalTime = localtime((time_t *)&currentTime);

  currentTime = usmDbAdjustedTimeGet();
  usmDbTimeZoneOffsetGet(unit, &tzMinutes);
  usmDbSystemTimeZoneAcronymGet(unit, zoneName);

  LocalTime = localtime((time_t *)&currentTime);
  memset(buffer, 0x00, L7_CLI_MAX_STRING_LENGTH);

  /* don't modify tzMinutes so it may be displayed later */
  minutes = tzMinutes;

  tzHours = tzMinutes / L7_MINS_PER_HOUR;
  tzMinutes = tzMinutes % L7_MINS_PER_HOUR;

  if (usmDbSummerTimeIsInEffect()) {
    minutes = minutes + offset;
  }

  /* carry over minutes to hours */
  hours = hours + (minutes / L7_MINS_PER_HOUR);
  minutes = abs(minutes % L7_MINS_PER_HOUR);

  sprintf(buffer, "\r\n%2.2d:%2.2d:%2.2d %s(UTC%+d:%02d) %s %d %d \r\n",
          LocalTime->tm_hour, LocalTime->tm_min, LocalTime->tm_sec, zoneName,
          hours, minutes, monthsOfYear[LocalTime->tm_mon+1], LocalTime->tm_mday,
          (LocalTime->tm_year + 1900));

  ewsTelnetWrite(ewsContext, buffer);

  usmDbSntpClientModeGet(unit, &clientMode);
  if (clientMode == L7_SNTP_CLIENT_DISABLED)
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_no_time_src);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_sntp_time_src);
  }

  /* Summertime Details */

  if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_RECURRING)
  {
    if (!(strlen (zoneName)))
       osapiStrncpySafe (str, pStrInfo_base_no_acronym,sizeof(str));
    else
    {
      memset (tempStr, 0x00, sizeof (tempStr));
      sprintf (tempStr, "\r\n Acronym is %s", zoneName);
      strcpy (str, tempStr);
    }
    minutes=offset;
    strcat (str, "\r\n Recurring every year");

    memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n Begins at %s %s of %s at %2.2d:%2.2d", weekName[stWeek],

        daysOfWeek[stDay], monthsOfYear[stMonth],
        stHour, stMin);

    strcat (str, tempStr);
    memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n Ends at %s %s of %s at %2.2d:%2.2d ", weekName[endWeek],
        daysOfWeek[endDay], monthsOfYear[endMonth],
        endHour, endMin);

    strcat (str, tempStr);
    memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n offset is %u minutes", minutes);
    strcat (str, tempStr);

  }
  else if(usmDbStModeGet(USMDB_UNIT_CURRENT,&mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_NON_RECURRING)
  {
    minutes=offset;
    if (!(strlen (zoneName)))
    osapiStrncpySafe (str, pStrInfo_base_no_acronym,sizeof(str));
    else
    {
      memset (tempStr, 0x00, sizeof (tempStr));
      sprintf (tempStr, "\r\n Acronym is %s", zoneName);
      strcpy (str, tempStr);
    }

    osapiStrncat (str, pStrInfo_base_nr_summ,sizeof(str)-strlen(str)-1);
    L7_assert(strlen(str) >= sizeof(str));

     memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n Begins at %d %s of %d at %2.2d:%2.2d",stDay, aggMonthsOfYear[stMonth],
        stYear, stHour, stMin);
    strcat (str, tempStr);

    memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n Ends at %d %s of %d at %2.2d:%2.2d", endDay, aggMonthsOfYear[endMonth],
        endYear, endHour, endMin);
    strcat (str, tempStr);

    memset (tempStr, 0x00, sizeof (tempStr));
    sprintf (tempStr, "\r\n offset is %u minutes", minutes);
    strcat (str, tempStr);
  }
  else /* EU, USA */
  {
    memset (str, 0x00, sizeof (str));
    osapiStrncpySafe(str,pStrInfo_base_summ_disabled,sizeof(str));  
  }

  usmDbSystemTimeZoneAcronymGet(USMDB_UNIT_CURRENT,currZoneName);
  if (!(strlen (currZoneName)))
  {
      strcpy (currZoneName, " ");
  }

  /*timeZone info */

  ewsTelnetWrite(ewsContext, pStrInfo_base_timezone_2);
  if (!(strlen (tzName)))
    ewsTelnetWrite(ewsContext, pStrInfo_base_no_acronym);
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_acronym_is);
    ewsTelnetWrite(ewsContext,tzName);
  }

  sprintf(buffer, "\r\n Offset is UTC%+d:%02d", tzHours, tzMinutes);

  ewsTelnetWrite(ewsContext, buffer);

  /* summer-time info.*/

  ewsTelnetWrite(ewsContext, pStrInfo_base_summ_time);
  ewsTelnetWrite(ewsContext, str);

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  To print the running configuration of clock timezone info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 @end
*********************************************************************/
L7_RC_t cliRunningConfigClockTimeZone (EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8  buf [L7_CLI_MAX_STRING_LENGTH];
  L7_char8  temp [L7_CLI_MAX_STRING_LENGTH];
  L7_int32 hours, minutes;

  L7_char8 zoneName [L7_TIMEZONE_ACRONYM_LEN+1];
  L7_RC_t rc1,rc2;


  memset (buf, 0x00, sizeof (buf));
  memset (temp, 0x00, sizeof (temp));
  memset (zoneName, 0x00, sizeof (zoneName));


  rc1 = usmDbTimeZoneOffsetGet (unit, &minutes);
  hours = minutes / L7_MINS_PER_HOUR;
  minutes = abs(minutes) % L7_MINS_PER_HOUR;
  rc2 = usmDbTimeZoneAcronymGet (unit, zoneName);

  if (rc1 == L7_SUCCESS && rc2 == L7_SUCCESS)
  {
    if ((hours != FD_TIMEZONE_HRS || minutes != FD_TIMEZONE_MINS) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      sprintf (buf, "\r\nclock timezone %d ", hours);

      if ((hours != FD_TIMEZONE_HRS || minutes != FD_TIMEZONE_MINS) ||(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        sprintf (temp, "minutes %d ", minutes);
        strcat (buf, temp);
      }

      if ((strcmp (zoneName, FD_TIMEZONE_ACRONYM) != L7_NULL)
            || (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT))
      {
        memset (temp, 0x00, sizeof (temp));
        sprintf (temp, "zone %s", zoneName);
        strcat (buf, temp);
      }

      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of summer-time info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 @end
*********************************************************************/
L7_RC_t cliRunningConfigSummerTime(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8  temp[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 stMonth, stDay, stYear, stHour, stMin, endMonth, endDay, endYear, endHour, endMin, offset = 0;
  L7_uint32 stWeek, endWeek, allValidValues = L7_FALSE;
  L7_char8 zoneName[L7_TIMEZONE_ACRONYM_LEN+1];

  memset (temp, 0x00, sizeof (temp));
  memset (buf, 0x00, sizeof (buf));


  if (usmDbStRecurringGet (unit, L7_START_MONTH, (void *) &stMonth) == L7_SUCCESS)
    if (usmDbStRecurringGet (unit, L7_START_DAY, (void *) &stDay)  == L7_SUCCESS)
      if (usmDbStRecurringGet (unit, L7_START_WEEK, (void *) &stWeek) == L7_SUCCESS)
        if (usmDbStRecurringGet (unit, L7_START_HOUR, (void *) &stHour) == L7_SUCCESS)
          if (usmDbStRecurringGet (unit, L7_START_MINUTE, (void *) &stMin) == L7_SUCCESS)
            if (usmDbStRecurringGet (unit, L7_END_MONTH, (void *) &endMonth) == L7_SUCCESS)
              if (usmDbStRecurringGet (unit, L7_END_DAY, (void *) &endDay) == L7_SUCCESS)
                if (usmDbStRecurringGet (unit, L7_END_WEEK, (void *) &endWeek) == L7_SUCCESS)
                  if (usmDbStRecurringGet (unit, L7_END_HOUR, (void *) &endHour) == L7_SUCCESS)
                    if (usmDbStRecurringGet (unit, L7_END_MINUTE, (void *) &endMin) == L7_SUCCESS)
                      if (usmDbStRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset) == L7_SUCCESS)
                        if (usmDbStRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName) == L7_SUCCESS)
                            allValidValues = L7_TRUE;


  if (allValidValues == L7_TRUE)
  {
    if (stWeek != 0 && stDay != 0 && stMonth != 0 && endDay != 0 && endWeek != 0 && endMonth != 0)
    {

      allValidValues = L7_FALSE;
      /* check for USA summertime */
      if (stMonth == L7_SUMMER_TIME_USA_START_MONTH && stDay == L7_SUMMER_TIME_DAY && stWeek == L7_SUMMER_TIME_USA_START_WEEK &&
          stHour == L7_SUMMERTIME_USA_HOUR && stMin == 0 && endMonth == L7_SUMMER_TIME_USA_END_MONTH &&
          endDay == L7_SUMMER_TIME_DAY && endWeek == L7_SUMMER_TIME_USA_END_WEEK && endHour == L7_SUMMERTIME_USA_HOUR && endMin == 0)
      {
        sprintf(buf, "\r\nclock summer-time recurring %s ", "Usa");
      }

      /* check for EU summertime */
      else if (stMonth == L7_SUMMER_TIME_EU_START_MONTH && stDay == L7_SUMMER_TIME_DAY && stWeek == L7_SUMMER_TIME_EU_START_WEEK &&
               stHour == L7_SUMMERTIME_EU_START_HOUR && stMin == 0 && endMonth == L7_SUMMER_TIME_EU_END_MONTH &&
               endDay == L7_SUMMER_TIME_DAY && endWeek == L7_SUMMER_TIME_EU_END_WEEK && endHour == L7_SUMMERTIME_EU_END_HOUR && endMin == 0)
      {

        sprintf(buf, "\r\nclock summer-time recurring %s ", "EU");
      }
      else
      {          sprintf(buf, "\r\nclock summer-time recurring %d %s %s %02d:%02d %d %s %s %02d:%02d ",
          stWeek, dayOfWeek[stDay], monthsOfYear[stMonth], stHour, stMin,
          endWeek, dayOfWeek[endDay], monthsOfYear[endMonth], endHour, endMin);
      }

      if (offset != FD_TIMEZONE_MINS)
      {
        memset (temp, 0x00, sizeof (temp));
        sprintf (temp, "offset %d ", offset);
        strcat (buf, temp);
      }

      if ((strcmp (zoneName, FD_TIMEZONE_ACRONYM) != L7_NULL)
          || (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT))
      {
        memset (temp, 0x00, sizeof (temp));
        sprintf (temp, "zone %s", zoneName);
        strcat (buf, temp);
      }

      EWSWRITEBUFFER(ewsContext, buf);
    }
   /* if(stWeek==0 || stDay==0 || stMonth==0)
      printf("start values zero\n");
    if(endWeek==0 || endDay==0 || endMonth==0)
      printf("end values zero\n");  */
  }

  if (usmDbStNonRecurringGet (unit, L7_START_MONTH, (void *) &stMonth) == L7_SUCCESS)
    if (usmDbStNonRecurringGet (unit, L7_START_DAY, (void *) &stDay)  == L7_SUCCESS)
      if (usmDbStNonRecurringGet (unit, L7_START_YEAR, (void *) &stYear) == L7_SUCCESS)
        if (usmDbStNonRecurringGet (unit, L7_START_HOUR, (void *) &stHour) == L7_SUCCESS)
          if (usmDbStNonRecurringGet (unit, L7_START_MINUTE, (void *) &stMin) == L7_SUCCESS)
            if (usmDbStNonRecurringGet (unit, L7_END_MONTH, (void *) &endMonth) == L7_SUCCESS)
              if (usmDbStNonRecurringGet (unit, L7_END_DAY, (void *) &endDay) == L7_SUCCESS)
                if (usmDbStNonRecurringGet (unit, L7_END_YEAR, (void *) &endYear) == L7_SUCCESS)
                  if (usmDbStNonRecurringGet (unit, L7_END_HOUR, (void *) &endHour) == L7_SUCCESS)
                    if (usmDbStNonRecurringGet (unit, L7_END_MINUTE, (void *) &endMin) == L7_SUCCESS)
                      if (usmDbStNonRecurringGet (unit, L7_OFFSET_MINUTES, (void *) &offset) == L7_SUCCESS)
                        if (usmDbStNonRecurringGet (unit, L7_ZONE_ACRONYM, (void *) zoneName) == L7_SUCCESS)
                          allValidValues = L7_TRUE;

  if (allValidValues == L7_TRUE)
  {
    if (stDay != 0 && stMonth != 0 && stYear != 0 && endDay != 0 && endMonth != 0 && endYear != 0)
    {

      sprintf(buf, "\r\nclock summer-time date %d %s %d %02d:%02d %d %s %d %02d:%02d ",
          stDay, monthsOfYear[stMonth], stYear, stHour, stMin,
          endDay, monthsOfYear[endMonth], endYear, endHour, endMin);

      if (offset != FD_TIMEZONE_MINS)
      {
        memset (temp, 0x00, sizeof (temp));
        sprintf (temp, "offset %d ", offset);
        strcat (buf, temp);
      }

      if ((strcmp (zoneName, FD_TIMEZONE_ACRONYM) != L7_NULL)
          || (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT))
      {
        memset (temp, 0x00, sizeof (temp));
        sprintf (temp, "zone %s", zoneName);
        strcat (buf, temp);
      }

      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Build the tree nodes for system clock
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeShowClock(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4,depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_clock, pStrInfo_base_cli_show_clock_help,commandShowClock ,L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_detail_2, pStrInfo_base_cli_show_clock_detail_help, commandShowClockDetail,L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL,L7_NO_OPTIONAL_PARAMS);
}

