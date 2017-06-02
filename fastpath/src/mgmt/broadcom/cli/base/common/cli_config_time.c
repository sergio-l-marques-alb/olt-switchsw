/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/common/cli_config_time.c
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
#include "timezone_exports.h"
#include "usmdb_timezone_api.h"
#include "ews.h"
/* #include "l7_platform.h"*/

#include "cli_web_include.h"
#include "cli_web_mgr_api.h"
#include "unitmgr_api.h"
#include "session.h"

#include "clicommands_time.h"
#include "strlib_base_cli.h"
#include "strlib_common_common.h"

extern const L7_uint32 daysInMonth[];
extern const L7_uchar8 *monthsOfYear[];
extern const L7_uchar8 *dayOfWeek[];
extern const L7_uchar8 *weekName[];

/*******************************************************************n
 *
 * @purpose   sets the system time zone offset from UTC
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  clock timezone <hours-offset> [minutes <minutes-offset>] [zone <acronym>]
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandClockTimeZone(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 **argv, L7_uint32 index)

{
  L7_uint32   unit = cliGetUnitId();
  L7_uint32   numArg;
  L7_int32    hours = 0;
  L7_uint32   minutes = 0;
  L7_char8    zoneName [L7_TIMEZONE_ACRONYM_LEN+1];

  memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  zoneName [0] = '\0';
  if (ewsContext->commType != CLI_NO_CMD)
  {
    cliExamine(ewsContext, argc, argv, index);

    /* get switch ID based on presence/absence of STACKING package */
    if (unit == 0)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_base_clierror_unit_id);
      return cliPrompt(ewsContext);
    }

    numArg = cliNumFunctionArgsGet();

    if (numArg != L7_CLI_ONE &&  numArg != L7_CLI_THREE
        &&  numArg != L7_CLI_FIVE)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clisyntax_clocktimezone);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if ((cliConvertTo32BitSignedInteger (argv[index+1], &hours)!=L7_SUCCESS )||
        hours < L7_TIMEZONE_HOURS_MIN || hours > L7_TIMEZONE_HOURS_MAX)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_timezone_invalid_hrs);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if (numArg == L7_CLI_FIVE )
    {
      if (strlen((L7_char8 *)argv[index+5]) > L7_TIMEZONE_ACRONYM_LEN)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }

      osapiStrncpySafe(zoneName, argv[index+5], sizeof(zoneName));

      if((cliConvertTo32BitUnsignedInteger (argv[index+3], &minutes)!=L7_SUCCESS)||
          minutes < L7_TIMEZONE_MINS_MIN || minutes > L7_TIMEZONE_MINS_MAX)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_timezone_invalid_mins);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    else if(numArg == L7_CLI_THREE )
    {
      cliConvertToLowerCase ((L7_char8 *)argv[index+2]);
      if (strcmp ((L7_char8 *)argv[index+2], pStrInfo_base_cli_timezone_acronym) == 0)
      {
        if (strlen ((L7_char8 *)argv[index+3]) > L7_TIMEZONE_ACRONYM_LEN)
        {
          ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
          cliSyntaxBottom (ewsContext);
          return cliPrompt(ewsContext);
        }

        osapiStrncpySafe(zoneName, argv[index+3], sizeof(zoneName));
      }

      else
      {
         if ((cliConvertTo32BitUnsignedInteger(argv[index+3],&minutes)!=L7_SUCCESS) ||
            minutes < L7_TIMEZONE_MINS_MIN || minutes > L7_TIMEZONE_MINS_MAX)
        {
          ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_timezone_invalid_mins);
          cliSyntaxBottom (ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }
  if (usmDbTimeZoneOffsetSet(unit, (hours * L7_MINS_PER_HOUR) + (hours < 0 ? 0-minutes : minutes)) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_clierror_failed_to_set_timezone);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (usmDbTimeZoneAcronymSet (unit, zoneName) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_failed_to_set_timezone);
    cliSyntaxBottom (ewsContext);
    return cliPrompt(ewsContext);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom (ewsContext);
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose   To configure non-recurring paramters for summer time application
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @cmdsyntax in stacking env:  clock summer-time date date month year hh:mm
 date month year hh:mm [offset offset] [zone acronym]

 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandClockSummerTimeDate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 stMonth = 0, stDay = 0, stYear = 0, stHour = 0, stMin = 0;
  L7_uint32 endMonth = 0, endDay = 0, endYear = 0, endHour = 0, endMin = 0;
  L7_uint32 offset = 0;
  L7_uint32 numArg, unit = 0;
  L7_char8 zoneName [L7_TIMEZONE_ACRONYM_LEN +1];
  L7_uint32 mode=L7_SUMMER_TIME_NON_RECURRING;

  memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  zoneName [0]='\0';

  if( ewsContext->commType != CLI_NO_CMD)
  {
    cliExamine(ewsContext, argc, argv, index);

    numArg = cliNumFunctionArgsGet ();

    if(numArg != 8 && numArg != 10 && numArg != 12)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clisyntax_summertimedate);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if (cliIsStackingSupported() == L7_TRUE)
      unit = EWSUNIT(ewsContext);
    else
      unit = cliGetUnitId();

    memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN + 1);


    if(numArg > 8)
      cliConvertToLowerCase((L7_char8 *)argv[index+9]);

    /* day was entered as first parameter */
    if (cliConvertTo32BitUnsignedInteger (argv[index+1], &stDay) == L7_SUCCESS)
    {
      if (cliMonthNumberGet (argv[index + 2], &stMonth) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startmonth);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }

      else if (cliMonthNumberGet (argv[index + 6], &endMonth) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endmonth);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }

      else if (stDay > daysInMonth [stMonth])
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      else if ((cliConvertTo32BitUnsignedInteger (argv[index+5], &endDay) != L7_SUCCESS) ||
          (endDay > daysInMonth[endMonth]))
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }

    }
    /* month entered as first parameter */
    else
    {
      if (cliMonthNumberGet (argv[index+1], &stMonth) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startmonth);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      else if (cliMonthNumberGet (argv[index + 5], &endMonth) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endmonth);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      else if ((cliConvertTo32BitUnsignedInteger (argv[index+2], &stDay) != L7_SUCCESS) ||
          (stDay > daysInMonth[stMonth]))
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      else if ((cliConvertTo32BitUnsignedInteger (argv[index+6], &endDay) != L7_SUCCESS) ||
          (endDay > daysInMonth[endMonth]))
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
    }
    if (cliConvertTo32BitUnsignedInteger (argv[index+3], &stYear) != L7_SUCCESS ||
        (stYear < L7_START_SUMMER_TIME_YEAR) || (stYear > L7_END_SUMMER_TIME_YEAR))
    {
      ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_summertime_startyear);
      cliSyntaxBottom (ewsContext);
      return cliPrompt(ewsContext);
    }
    else if (cliConvertTo32BitUnsignedInteger (argv[index+7], &endYear) != L7_SUCCESS ||
        (endYear < L7_START_SUMMER_TIME_YEAR) || (endYear > L7_END_SUMMER_TIME_YEAR) || (endYear <stYear))
    {
      ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_summertime_endyear);
      cliSyntaxBottom (ewsContext);
      return cliPrompt(ewsContext);
    }
    else if ((usmDbTimeConvert (argv[index + 4], &stHour, &stMin) != L7_SUCCESS)
              || stHour < L7_TIMEZONE_UHOURS_MIN || stHour > L7_TIMEZONE_UHOURS_MAX
              || stMin < L7_TIMEZONE_MINS_MIN || stMin > L7_TIMEZONE_MINS_MAX)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_starttime);
      cliSyntaxBottom (ewsContext);
      return cliPrompt(ewsContext);
    }
    else if ((usmDbTimeConvert (argv[index + 8], &endHour, &endMin) != L7_SUCCESS)
              || endHour < L7_TIMEZONE_UHOURS_MIN || endHour > L7_TIMEZONE_UHOURS_MAX
              || endMin < L7_TIMEZONE_MINS_MIN || endMin > L7_TIMEZONE_MINS_MAX)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endtime);
      return cliPrompt (ewsContext);
    }
    else if((numArg >8) && (strcmp((L7_char8 *)argv[index+9],pStrInfo_base_cli_summertime_offset)==0) &&
        ((cliConvertTo32BitUnsignedInteger(argv[index+10],&offset) != L7_SUCCESS) || offset > L7_SUMMER_TIME_OFFSET_MAX
         || offset < L7_SUMMER_TIME_OFFSET_MIN))
    {
      ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_summertime_mins_offset);
      cliSyntaxBottom (ewsContext);
      return cliPrompt(ewsContext);
    }
    if ( (numArg==12 && (strcmp((L7_char8 *)argv[index+11], pStrInfo_base_cli_timezone_acronym)==0)) ||
        (numArg==10 && (strcmp((L7_char8 *)argv[index+9], pStrInfo_base_cli_timezone_acronym)==0)) )
    {
      if(((numArg == 10) && strlen((L7_char8 *)argv[index+10]) > L7_TIMEZONE_ACRONYM_LEN) ||
          ((numArg == 12) && strlen((L7_char8 *)argv[index+12]) > L7_TIMEZONE_ACRONYM_LEN))
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }

      if (numArg == 10)
        osapiStrncpySafe(zoneName, argv[index+10], sizeof(zoneName));
      else
        osapiStrncpySafe(zoneName, argv[index+12], sizeof(zoneName));

    }

    if (cliSummerTimeValidate (stMonth, stDay, stYear, 0,
                              endMonth, endDay, endYear, 0, L7_FALSE) != L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_cliinvalid_summertime);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if((usmDbStModeSet (unit,mode)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_START_MONTH, (void *) &stMonth)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_START_DAY, (void *) &stDay)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_START_YEAR, (void *) &stYear)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_START_HOUR, (void *) &stHour)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_START_MINUTE, (void *) &stMin)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_END_MONTH, (void *) &endMonth)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_END_DAY, (void *) &endDay)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_END_YEAR, (void *) &endYear)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_END_HOUR, (void *) &endHour)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_END_MINUTE, (void *) &endMin)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_OFFSET_MINUTES, (void *) &offset)!=L7_SUCCESS) ||
        (usmDbStNonRecurringSet (unit, L7_ZONE_ACRONYM, (void *) zoneName)!=L7_SUCCESS) )
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_set);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

  } /* normal command */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_NO_SUMMER_TIME;
    if(usmDbStModeSet (unit,mode)!=L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_disable);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    ewsTelnetWrite (ewsContext, pStrInfo_base_summTimeDisabled);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom (ewsContext);
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose   To configure recurring paramters for summer time
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @cmdsyntax in stacking env:  clock summer-time recurring {<week> <day> <month> <hh:mm>
 <week> <day> <month> <hh:mm>}[offset <offset>] [zone <acronym>]
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandClockSummerTimeRecurring (EwsContext ewsContext,
    L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_int32 unit,stMode=0;
  L7_uint32 stMonth = 0, stDay = 0, stWeek = 0, stHour = 0, stMin =0;
  L7_uint32 endMonth = 0, endDay = 0, endWeek = 0, endHour = 0, endMin = 0;
  L7_uint32 minOffset = 0;
  L7_uint32 numArg;
  L7_char8 zoneName [L7_TIMEZONE_ACRONYM_LEN+1];

  memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  zoneName [0]='\0';

  cliExamine  (ewsContext, argc, argv, index);

  numArg = cliNumFunctionArgsGet ();

  stMode = L7_SUMMER_TIME_RECURRING;

  if(numArg != 7 && numArg != 9 && numArg != 11)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clisyntax_setsummertimerec);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }

  if(numArg > 7)
    cliConvertToLowerCase((L7_char8 *)argv[index+8]);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported () == L7_TRUE)
    unit = EWSUNIT (ewsContext);
  else
    unit = cliGetUnitId ();
    if (cliConvertTo32BitUnsignedInteger (argv[index], &stWeek) != L7_SUCCESS)
  {

  if (usmDbWeekNumberGet ((L7_char8 *)argv[index], &stWeek) != L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startweek);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
  }
  else
  {
    if (stWeek < L7_WEEK_CNT_MIN || stWeek > L7_WEEK_CNT_MAX)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startweek);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
  }
  if (cliConvertTo32BitUnsignedInteger (argv[index + 4], &endWeek) != L7_SUCCESS)
  {
    if(usmDbWeekNumberGet ((L7_char8 *)argv[index + 4], &endWeek) != L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endweek);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
  }
  else if (endWeek < L7_WEEK_CNT_MIN || endWeek > L7_WEEK_CNT_MAX)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startweek);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  if (cliDayOfWeekGet (argv[index+1], &stDay) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startweekday);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  else if (cliDayOfWeekGet(argv[index+5], &endDay) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endweekday);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  else if (cliMonthNumberGet (argv[index+2], &stMonth) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_startmonth);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  else if (cliMonthNumberGet (argv[index+6], &endMonth) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endmonth);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  else if ((usmDbTimeConvert (argv[index + 3], &stHour, &stMin) != L7_SUCCESS)
      || stHour < L7_TIMEZONE_UHOURS_MIN || stHour > L7_TIMEZONE_UHOURS_MAX
      || stMin < L7_TIMEZONE_MINS_MIN || stMin > L7_TIMEZONE_MINS_MAX)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_starttime);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  else if ((usmDbTimeConvert (argv[index + 7], &endHour, &endMin) != L7_SUCCESS)
      || endHour < L7_TIMEZONE_UHOURS_MIN || endHour > L7_TIMEZONE_UHOURS_MAX
      || endMin < L7_TIMEZONE_MINS_MIN || endMin > L7_TIMEZONE_MINS_MAX)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_endtime);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }

  else if ((numArg > 7) && (strcmp ((L7_char8 *)argv[index+8], pStrInfo_base_cli_summertime_offset) == 0) )
  {
    if ((cliConvertTo32BitUnsignedInteger (argv[index+9], &minOffset) != L7_SUCCESS) || minOffset < L7_SUMMER_TIME_OFFSET_MIN
        || minOffset > L7_SUMMER_TIME_OFFSET_MAX)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_mins_offset);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
  }

  if ( (numArg==11 && (strcmp((L7_char8 *)argv[index+10], pStrInfo_base_cli_timezone_acronym)==0)) ||
      (numArg==9 && (strcmp((L7_char8 *)argv[index+8], pStrInfo_base_cli_timezone_acronym)==0)) )
  {
    if(((numArg == 9) && strlen((L7_char8 *)argv[index+9]) > L7_TIMEZONE_ACRONYM_LEN) ||
        ((numArg == 11) && strlen((L7_char8 *)argv[index+11]) > L7_TIMEZONE_ACRONYM_LEN))
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if (numArg == 9)
    {
        osapiStrncpySafe(zoneName, argv[index+9], sizeof(zoneName));

    }
    else
    {
        osapiStrncpySafe(zoneName, argv[index+11], sizeof(zoneName));
    }
  }

  if (cliSummerTimeValidate (stMonth, stDay, 0, stWeek, endMonth,
        endDay, 0, endWeek, L7_TRUE) != L7_SUCCESS)
  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_cliinvalid_recur_summertime);
    cliSyntaxBottom (ewsContext);
    return cliPrompt(ewsContext);
  }


  if((usmDbStModeSet (unit,stMode)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_MONTH, (void *) &stMonth)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_DAY, (void *) &stDay)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_WEEK, (void *) &stWeek)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_HOUR, (void *) &stHour)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_MINUTE, (void *) &stMin)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_MONTH, (void *) &endMonth)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_DAY, (void *) &endDay)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_WEEK, (void *) &endWeek)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_HOUR, (void *) &endHour)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_MINUTE, (void *) &endMin)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_OFFSET_MINUTES, (void *) &minOffset)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_ZONE_ACRONYM, (void *) zoneName)!=L7_SUCCESS))

  {
    ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_recur_summertime_set);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom (ewsContext);
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose   To disable the summer-time setting
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @cmdsyntax in stacking env:  no clock summer-time

 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 * commandNoSummerTime (EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_int32 unit;
  L7_int32 stMode;

  /**** Error Checking for command Type*****/


  if(ewsContext->commType == CLI_NO_CMD)
  {

    cliExamine (ewsContext, argc, argv, index);

    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported () == L7_TRUE)
      unit = EWSUNIT (ewsContext);
    else
      unit = cliGetUnitId ();

    stMode = L7_NO_SUMMER_TIME;
    if (usmDbStModeSet (unit, stMode) != L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_disable);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
  }

  cliSyntaxBottom (ewsContext);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   To configure recurring paramters for summer time application
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @cmdsyntax in stacking env:  clock summer-time recurring {usa | eu | {week day month hh:mm week day month hh:mm}}
                                            [offset offset] [zone acronym]

*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 * commandClockSummerTimeRecurringSpec (EwsContext ewsContext,
                          L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_int32 unit,stMode=0;
  L7_uint32 stMonth = 0, stDay = 0, stWeek=0, stHour = 0, stMin = 0;
  L7_uint32 endMonth = 0, endDay = 0, endWeek = 0, endHour = 0, endMin = 0;
  L7_uint32 minOffset = 0;
  L7_uint32 numArg;
  L7_char8 zoneName [L7_TIMEZONE_ACRONYM_LEN+1];
  memset (zoneName, 0x00, L7_TIMEZONE_ACRONYM_LEN+1);
  zoneName [0]='\0';

  cliExamine(ewsContext, argc, argv, index);

  numArg = cliNumFunctionArgsGet ();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported () == L7_TRUE)
    unit = EWSUNIT (ewsContext);
  else
    unit = cliGetUnitId ();

  minOffset = L7_DEFAULT_SUMMERTIME_OFFSET;

  /* only zone and minOffset entered */
  if (numArg == 4)
  {
    if (convertTo32BitUnsignedInteger (argv[index+2], &minOffset) != L7_SUCCESS)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_summertime_mins_offset);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
    if (strlen ((L7_char8 *)argv[index+4]) > L7_TIMEZONE_ACRONYM_LEN)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }
        osapiStrncpySafe(zoneName, argv[index+4], sizeof(zoneName));
  }

  else if (numArg == 2)
  {
    cliConvertToLowerCase ((L7_char8 *)argv[index+1]);
    if (strcmp((L7_char8 *)argv[index+1], pStrInfo_base_cli_summertime_offset) == 0)
    {
      if(cliConvertTo32BitUnsignedInteger(argv[index+2],&minOffset) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_base_clierror_summertime_mins_offset);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (strlen ((L7_char8 *)argv[index+2]) > L7_TIMEZONE_ACRONYM_LEN)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_timezone_invalid_acronym);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }
        osapiStrncpySafe(zoneName, argv[index+2], sizeof(zoneName));

    }
  }


  cliConvertToLowerCase((L7_char8 *)argv[index]);
  if(strcmp((L7_char8 *)argv[index],pStrInfo_base_usa)==0)
  {
    stMode=L7_SUMMER_TIME_RECURRING_USA;
    stMonth= L7_SUMMER_TIME_USA_START_MONTH;
    stDay=L7_SUMMER_TIME_DAY;
    stWeek=L7_SUMMER_TIME_USA_START_WEEK;
    stHour=L7_SUMMERTIME_USA_HOUR;
    stMin=0;
    endMonth= L7_SUMMER_TIME_USA_END_MONTH;
    endDay=L7_SUMMER_TIME_DAY;
    endWeek=L7_SUMMER_TIME_USA_END_WEEK; 
    endHour=L7_SUMMERTIME_USA_HOUR;
    endMin=0;
  }
  else if(strcmp((L7_char8 *)argv[index],pStrInfo_base_eu)==0)
  {
    stMode =L7_SUMMER_TIME_RECURRING_EU;
    stMonth= L7_SUMMER_TIME_EU_START_MONTH;
    stDay=L7_SUMMER_TIME_DAY;
    stWeek=L7_SUMMER_TIME_EU_START_WEEK;
    stHour=L7_SUMMERTIME_EU_START_HOUR;
    stMin=0;
    endMonth= L7_SUMMER_TIME_EU_END_MONTH;
    endDay=L7_SUMMER_TIME_DAY;
    endWeek=L7_SUMMER_TIME_EU_END_WEEK;
    endHour=L7_SUMMERTIME_EU_END_HOUR;
    endMin=0;
  }

  if((usmDbStModeSet (unit, stMode)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_MONTH, (void *) &stMonth)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_DAY, (void *) &stDay)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_WEEK, (void *) &stWeek)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_HOUR, (void *) &stHour)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_START_MINUTE, (void *) &stMin)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_MONTH, (void *) &endMonth)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_DAY, (void *) &endDay)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_WEEK, (void *) &endWeek)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_HOUR, (void *) &endHour)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_END_MINUTE, (void *) &endMin)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_OFFSET_MINUTES, (void *) &minOffset)!=L7_SUCCESS)||
      (usmDbStRecurringSet (unit, L7_ZONE_ACRONYM, (void *) zoneName)!=L7_SUCCESS))
    {

      ewsTelnetWrite (ewsContext, pStrInfo_base_clierror_recur_summertime_set);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom (ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for clock timezone command
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
void  buildTreeGlobalClockTimeZone(EwsCliCommandP depth2)
{
  EwsCliCommandP  depth3, depth4, depth5, depth6, depth7,depth8,depth9;

  depth3 = ewsCliAddNode (depth2, pStrInfo_base_timezone, pStrInfo_base_clitimezone_help, 
                          commandClockTimeZone, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode (depth3, pStrInfo_base_timezone_range, pStrInfo_base_clitimezone_hours_help,NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_base_minutes, pStrInfo_base_clitimezonemins_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_base_minutes_range, pStrInfo_base_clitimezone_minsval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode (depth6, pStrInfo_common_Cr,   pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode (depth6, pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode (depth7, pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode (depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_base_zone, pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode (depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  Build the tree nodes for clock summer-time date command
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
void buildTreeGlobalClockSummerTimeDate(EwsCliCommandP  depth3)
{
  EwsCliCommandP depth4,depth5, depth6, depth7, depth8, depth9, depth10,depth11,depth12,depth13,depth14,depth15,depth16,depth17;

  /* DDEVI_TBR*/
  /* depth4=ewsCliAddNode(depth14,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS); */
  depth4=ewsCliAddNode(depth3, pStrInfo_base_date, pStrInfo_base_clisummertime_nonrecur_help, commandClockSummerTimeDate, L7_NO_OPTIONAL_PARAMS);

  /* for summertime set date month year format */

  depth5=ewsCliAddNode(depth4, pStrInfo_base_date_range, pStrInfo_base_clisummertime_nr_sd_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6=ewsCliAddNode(depth5, pStrInfo_base_mon, pStrInfo_base_clisummertime_nr_sm_help, NULL,L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6, pStrInfo_base_year_range, pStrInfo_base_clisummertime_nr_sy_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7, pStrInfo_base_time_fmt, pStrInfo_base_clisummertime_st_help,NULL,L7_NO_OPTIONAL_PARAMS);

  depth9=ewsCliAddNode(depth8,pStrInfo_base_date_range,pStrInfo_base_clisummertime_nr_ed_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth10=ewsCliAddNode(depth9,pStrInfo_base_mon,pStrInfo_base_clisummertime_nr_em_help, NULL,L7_NO_OPTIONAL_PARAMS);
  depth11=ewsCliAddNode(depth10,pStrInfo_base_year_range, pStrInfo_base_clisummertime_nr_ey_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth12=ewsCliAddNode(depth11,pStrInfo_base_time_fmt, pStrInfo_base_clisummertime_et_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth13=ewsCliAddNode(depth12, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth13=ewsCliAddNode(depth12, pStrInfo_base_offset, pStrInfo_base_clisummertime_mins_help,NULL, L7_NO_OPTIONAL_PARAMS);
  depth14=ewsCliAddNode(depth13, pStrInfo_base_offset_range, pStrInfo_base_clisummertime_minsval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth15=ewsCliAddNode(depth14,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth15=ewsCliAddNode(depth14, pStrInfo_base_zone, pStrInfo_base_clitimezone_acronym_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth16=ewsCliAddNode(depth15, pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth17=ewsCliAddNode(depth16,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth13=ewsCliAddNode(depth12,pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth14=ewsCliAddNode(depth13, pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth15=ewsCliAddNode(depth14,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  Build the tree nodes for clock summer-time recurring command
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
void buildTreeGlobalClockSummerTimeRecurring(EwsCliCommandP  depth4)
{
  EwsCliCommandP depth5, depth6, depth7, depth8, depth9, depth10,depth11,depth12,depth13,depth14,depth15,depth16,depth17;

  depth5=ewsCliAddNode(depth4, pStrInfo_base_week,pStrInfo_base_clisummertime_sw_help,commandClockSummerTimeRecurring,L7_NO_OPTIONAL_PARAMS);
  depth6=ewsCliAddNode(depth5, pStrInfo_base_day,pStrInfo_base_clisummertime_sd_help, NULL,L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6, pStrInfo_base_mon, pStrInfo_base_clisummertime_sm_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7, pStrInfo_base_time_fmt, pStrInfo_base_clisummertime_st_help,NULL,L7_NO_OPTIONAL_PARAMS);

  depth9=ewsCliAddNode(depth8,pStrInfo_base_week,pStrInfo_base_clisummertime_ew_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth10=ewsCliAddNode(depth9,pStrInfo_base_day,pStrInfo_base_clisummertime_sd_help, NULL,L7_NO_OPTIONAL_PARAMS);
  depth11=ewsCliAddNode(depth10,pStrInfo_base_mon, pStrInfo_base_clisummertime_sm_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth12=ewsCliAddNode(depth11,pStrInfo_base_time_fmt, pStrInfo_base_clisummertime_et_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth13=ewsCliAddNode(depth12, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth13=ewsCliAddNode(depth12, pStrInfo_base_offset, pStrInfo_base_clisummertime_mins_help,NULL, L7_NO_OPTIONAL_PARAMS);
  depth14=ewsCliAddNode(depth13, pStrInfo_base_offset_range , pStrInfo_base_clisummertime_minsval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth15=ewsCliAddNode(depth14,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth15=ewsCliAddNode(depth14, pStrInfo_base_zone, pStrInfo_base_clitimezone_acronym_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth16=ewsCliAddNode(depth15, pStrInfo_base_zoneacronym,pStrInfo_base_clitimezone_acronymval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth17=ewsCliAddNode(depth16,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth13=ewsCliAddNode(depth12,pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth14=ewsCliAddNode(depth13, pStrInfo_base_zoneacronym,pStrInfo_base_clitimezone_acronymval_help,NULL,L7_NO_OPTIONAL_PARAMS);
  depth15=ewsCliAddNode(depth14,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
/*********************************************************************
*
* @purpose  Build the tree nodes for clock summer-time usa | eu command
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
void buildTreeGlobalClockSummerTimeRecurringSpec(EwsCliCommandP  depth4)
{
  EwsCliCommandP depth5, depth6, depth7, depth8, depth9, depth10;

  /* for USA DST */
  depth5=ewsCliAddNode(depth4, pStrInfo_base_usa,pStrInfo_base_clisummertime_usa_help,
                       commandClockSummerTimeRecurringSpec,L7_NO_OPTIONAL_PARAMS);
  depth6=ewsCliAddNode(depth5,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6=ewsCliAddNode(depth5, pStrInfo_base_offset,pStrInfo_base_clisummertime_mins_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6, pStrInfo_base_offset_range , pStrInfo_base_clisummertime_minsval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth8=ewsCliAddNode(depth7,pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9=ewsCliAddNode(depth8,pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10=ewsCliAddNode(depth9,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6=ewsCliAddNode(depth5,pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6,pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

   /* for the EU DST */
  depth5=ewsCliAddNode(depth4, pStrInfo_base_eu,pStrInfo_base_clisummertime_eu_help,
                        commandClockSummerTimeRecurringSpec,L7_NO_OPTIONAL_PARAMS);
  depth6=ewsCliAddNode(depth5,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6=ewsCliAddNode(depth5, pStrInfo_base_offset,pStrInfo_base_clisummertime_mins_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6, pStrInfo_base_offset_range , pStrInfo_base_clisummertime_minsval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth8=ewsCliAddNode(depth7,pStrInfo_base_zone , pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9=ewsCliAddNode(depth8,pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10=ewsCliAddNode(depth9,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6=ewsCliAddNode(depth5, pStrInfo_base_zone, pStrInfo_base_clitimezone_acronym_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7=ewsCliAddNode(depth6, pStrInfo_base_zoneacronym, pStrInfo_base_clitimezone_acronymval_help, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8=ewsCliAddNode(depth7,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
