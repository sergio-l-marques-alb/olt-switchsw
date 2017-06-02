/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2010
 *
 **********************************************************************
 *
 * @filename src/mgmt/broadcom/cli/base/timeranges/cli_config_timerange.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  7 Dec 2009
 *
 * @author  Siva Mannem
 *
 * @end
 *
 **********************************************************************/
#include <errno.h>
#include "l7_common.h"
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cli_web_exports.h"
#include "cli_mode.h"
#include "clicommands_card.h"
#include "usmdb_util_api.h"
#include "ews.h"
#include "timerange_exports.h"
#include "usmdb_timerange_api.h"

extern const L7_uint32 daysInMonth[];

const L7_char8 *cliTimeRangePrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                              L7_char8 * tmpUpPrompt, L7_uint32 argc,
                              const L7_char8 ** argv, EwsContext ewsContext);
/*********************************************************************
*
* @purpose Function to set or exit from the time-range config mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliTimeRangeMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliTimeRangePrompt(index+1, pStrInfo_base_TimeRangeCfgModePrompt,pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from time-range config mode.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] time-range <name>
*
* @cmdhelp
*
* @cmddescript  Enters the time-range config mode.
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliTimeRangePrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, index, maxTimeRanges, rc;
  L7_char8 strName[L7_TIMERANGE_NAME_LEN_MAX+1];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
    if (prompt != NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return prompt;
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((argc == depth + 1) &&
      (mainMenu[cliCurrentHandleGet()] !=cliGetMode(L7_TIMERANGE_CONFIG_MODE)))
  {

    if (strlen(argv[depth]) > L7_TIMERANGE_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_base_InvalidTimeRangeName);
    }

    osapiStrncpySafe(strName,argv[depth], sizeof(strName));

    if (usmDbTimeRangeNameStringCheck(unit, strName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_InvalidTimeRangeName);
    }

    /* Check to see if Time Range exists */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Does not exist - create and add */
        if (usmDbTimeRangeNameToIndex(unit, strName, &index) != L7_SUCCESS)
        {
          rc = usmdbTimeRangeNextAvailableIndexGet(unit, &index);
          if (rc != L7_SUCCESS)
          {

            if (rc == L7_TABLE_IS_FULL)
            {
              usmDbTimeRangeMaxNumGet(unit, &maxTimeRanges);           /* This function always returns L7_SUCCESS */
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_MaxOfTimeRangesIsAlreadyCfgured, maxTimeRanges);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UnableToCreateTimeRange);
            }
          }

          rc = usmDbTimeRangeCreate(unit, index);

          switch (rc)
          {
            case L7_SUCCESS:
              break;

            case L7_TABLE_IS_FULL:
              usmDbTimeRangeMaxNumGet(unit, &maxTimeRanges);           /* This function always returns L7_SUCCESS */
              return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_MaxOfTimeRangesIsAlreadyCfgured, maxTimeRanges);

            default:
              return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_base_UnableToCreateTimeRange);
          }

          if (usmDbTimeRangeNameAdd(unit, index, strName) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_base_UnableToCreateTimeRange);
          }
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      osapiStrncpySafe(EWSTIMERANGENAME(ewsContext), strName, sizeof((EWSTIMERANGENAME(ewsContext))));

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_TIMERANGE_CONFIG_MODE));
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbTimeRangeNameToIndex(unit, strName, &index) == L7_SUCCESS)
        {
          rc = usmDbTimeRangeDelete(unit, index);

          switch (rc)
          {
            case L7_SUCCESS:
              break;
            
            default:
              return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_base_TimeRangeDelFail);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_base_TimeRangeDoesntExist);
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}

/*********************************************************************
 *
 * @purpose   To configure a periodic timerange entry
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @cmdsyntax : periodic 

 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandPeriodicTimeRangeEntry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 stHour = 0, stMin = 0;
  L7_uint32 endHour = 0, endMin = 0;
  L7_uchar8 stDayMask = 0, endDayMask = 0;
  L7_uchar8 day;
  L7_uint32 i, endTimeIndex, toIndex = 0, numArg, unit = 0;
  L7_char8  timeRangeName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 timeRangeIndex = 0, timeRangeEntryNum = 0;
  L7_RC_t   rc;
  L7_BOOL   endDayFlag = L7_FALSE;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliExamine(ewsContext, argc, argv, index);
  numArg = cliNumFunctionArgsGet ();

  for (i = 0; i <= numArg; i++)
  {
    if (osapiStrncmp((L7_char8 *)argv[i], pStrInfo_base_periodic_to,
            osapiStrnlen((L7_char8 *)argv[i], L7_CLI_MAX_STRING_LENGTH)) == 0)
    {
      toIndex = i;
      break;
    }
  }
  if (toIndex == 3)
  {
    if (osapiStrncmp((L7_char8 *)argv[1], pStrInfo_base_periodic_weekdays,
            osapiStrnlen((L7_char8 *)argv[1], L7_CLI_MAX_STRING_LENGTH)) == 0)
    {
      stDayMask = ((1 << L7_DAY_MON) | (1 << L7_DAY_TUE) | (1 << L7_DAY_WED) | \
                   (1 << L7_DAY_THU) | (1 << L7_DAY_FRI));
      endDayMask = stDayMask;
    }
    else if (osapiStrncmp((L7_char8 *)argv[1], pStrInfo_base_periodic_weekends,
            osapiStrnlen((L7_char8 *)argv[1], L7_CLI_MAX_STRING_LENGTH)) == 0)
    {
      stDayMask = ((1 << L7_DAY_SAT) | (1 << L7_DAY_SUN));
      endDayMask = stDayMask;
    }
    else if (osapiStrncmp((L7_char8 *)argv[1], pStrInfo_base_periodic_daily,
            osapiStrnlen((L7_char8 *)argv[1], L7_CLI_MAX_STRING_LENGTH)) == 0)
    {
      stDayMask = ((1 << L7_DAY_MON) | (1 << L7_DAY_TUE) | (1 << L7_DAY_WED) | \
                   (1 << L7_DAY_THU) | (1 << L7_DAY_FRI) | (1 << L7_DAY_SAT) | \
                   (1 << L7_DAY_SUN));
      endDayMask = stDayMask;
    }
    else
    {
      cliConvertToLowerCase((L7_char8 *)argv[1]);
      if (cliFullDayOfWeekGet((L7_char8 *)argv[1], &day) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_startweekday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      stDayMask = (1 << day);
      cliConvertToLowerCase((L7_char8 *)argv[4]);
      if (cliFullDayOfWeekGet((L7_char8 *)argv[4], &day) == L7_SUCCESS)
      {
        endDayMask = (1 << day);
        endDayFlag = L7_TRUE;
      }
    }
  }
  else 
  {
    for (i = 1; i < toIndex - 1; i++ )
    {
      cliConvertToLowerCase((L7_char8 *)argv[i]);
      if (cliFullDayOfWeekGet((L7_char8 *)argv[i], &day) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_startweekday);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }
      stDayMask |= (1 << day);
    }
    endDayMask = stDayMask;
  }
  rc = usmDbTimeConvert ((L7_char8 *)argv[toIndex - 1], &stHour, &stMin);
  if ((rc != L7_SUCCESS) || stHour < L7_TIMEZONE_UHOURS_MIN || 
                            stHour > L7_TIMEZONE_UHOURS_MAX ||
                            stMin < L7_TIMEZONE_MINS_MIN ||
                            stMin > L7_TIMEZONE_MINS_MAX)
  {
    ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_starttime);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }
  if (endDayFlag == L7_TRUE)
  {
    endTimeIndex = toIndex + 2;
  }
  else
  {
    endTimeIndex = toIndex + 1;
    endDayMask = stDayMask;
  }
  rc = usmDbTimeConvert ((L7_char8 *)argv[endTimeIndex], &endHour, &endMin);
  if ((rc != L7_SUCCESS) || endHour < L7_TIMEZONE_UHOURS_MIN || 
                            endHour > L7_TIMEZONE_UHOURS_MAX ||
                            endMin < L7_TIMEZONE_MINS_MIN ||
                            endMin > L7_TIMEZONE_MINS_MAX)
  {
    ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_endtime);
    cliSyntaxBottom (ewsContext);
    return cliPrompt (ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    osapiStrncpySafe(timeRangeName, EWSTIMERANGENAME(ewsContext), sizeof(timeRangeName));

    if (usmDbTimeRangeNameToIndex(unit, timeRangeName, &timeRangeIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
    }
    
    memset(&startDateAndTime, 0, sizeof(startDateAndTime));
    startDateAndTime.hour = stHour;
    startDateAndTime.minute = stMin;
    startDateAndTime.date.daysOfTheWeek.dayMask = stDayMask;

    memset(&endDateAndTime, 0, sizeof(endDateAndTime));
    endDateAndTime.hour = endHour;
    endDateAndTime.minute = endMin;
    endDateAndTime.date.daysOfTheWeek.dayMask = endDayMask;

    if (ewsContext->commType != CLI_NO_CMD)
    {
      rc = usmDbTimeRangeEntryIndexNextFreeGet(timeRangeIndex, &timeRangeEntryNum);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                L7_NULLPTR,  ewsContext, pStrErr_base_EntryError);
        }
        else if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
        }
        else 
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, pStrErr_common_Error,
                ewsContext, pStrErr_base_ProcessingTimeRange);
        }
      }
      rc = usmDbTimeRangeEntryAdd(unit, timeRangeIndex,
                                  timeRangeEntryNum, TIMERANGE_PERIODIC_ENTRY);
      switch (rc)
      {
        case L7_SUCCESS:
          break;

        case L7_ERROR:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TimeEntryAddError);

        case L7_TABLE_IS_FULL:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_EntryError);

       default:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ProcessingTimeRange);
      }

      rc = usmDbTimeRangeEntryStartDateAndTimeAdd(unit,
                                                  timeRangeIndex,
                                                  timeRangeEntryNum,
                                                  TIMERANGE_PERIODIC_ENTRY,
                                                  &startDateAndTime);
      if (rc != L7_SUCCESS)
      {
        /*No need to check error case. This is garbage cleaner and doesn't
        throw any error message*/
        usmDbTimeRangeEntryRemove(unit, timeRangeIndex, timeRangeEntryNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AddingPrdStartTime);
      }
      rc = usmDbTimeRangeEntryEndDateAndTimeAdd(unit,
                                                timeRangeIndex,
                                                timeRangeEntryNum,
                                                TIMERANGE_PERIODIC_ENTRY,
                                                &endDateAndTime);
      if (rc != L7_SUCCESS)
      {
       /*No need to check error case. This is garbage cleaner and doesn't
        throw any error message*/
        usmDbTimeRangeEntryRemove(unit, timeRangeIndex, timeRangeEntryNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AddingPrdEndTime);
      }
    }
    else
    {
      if (usmDbTimeRangePeriodicEntryDelete(unit,
                                            timeRangeIndex,
                                            &startDateAndTime,
                                            &endDateAndTime) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PeriodicTimeEntryDoesNotExist);
      }
    }
  }  
   /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose   To configure a absolute timerange entry
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @cmdsyntax : absolute start hh:mm day month year end hh:mm day month year

 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandAbsoluteTimeRangeEntry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 stMonth = 0, stDay = 0, stYear = 0, stHour = 0, stMin = 0;
  L7_uint32 endMonth = 0, endDay = 0, endYear = 0, endHour = 0, endMin = 0;
  L7_uint32 numArg, unit = 0;
  L7_BOOL   stTimeSpecified = L7_FALSE;
  L7_BOOL   endTimeSpecified = L7_FALSE;
  L7_char8  timeRangeName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 timeRangeIndex = 0, timeRangeEntryNum = 0;
  L7_RC_t   rc;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  
  cliExamine(ewsContext, argc, argv, index);
  numArg = cliNumFunctionArgsGet ();

  if (ewsContext->commType != CLI_NO_CMD)
  {
    if (numArg != 5 && numArg != 10)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_base_clisyntax_TimeRangeAbsolute);
      cliSyntaxBottom (ewsContext);
      return cliPrompt (ewsContext);
    }

    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
    }
    
    if (osapiStrncmp((L7_char8 *)argv[index + 1], pStrInfo_base_start,
        osapiStrnlen((L7_char8 *)argv[index + 1], L7_CLI_MAX_STRING_LENGTH)) == 0)
    {
      stTimeSpecified = L7_TRUE;
      rc = usmDbTimeConvert (argv[index + 2], &stHour, &stMin);
      if ((rc != L7_SUCCESS) || stHour < L7_TIMEZONE_UHOURS_MIN || 
                                stHour > L7_TIMEZONE_UHOURS_MAX ||
                                stMin < L7_TIMEZONE_MINS_MIN ||
                                stMin > L7_TIMEZONE_MINS_MAX)
      {
        ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_starttime);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }

      rc = cliConvertTo32BitUnsignedInteger (argv[index + 5], &stYear);
      if ((rc != L7_SUCCESS) || 
          (stYear < L7_TIMERANGE_MIN_YEAR) ||
          (stYear > L7_TIMERANGE_MAX_YEAR))
      {
        ewsTelnetWrite( ewsContext, pStrErr_base_TimeRange_startyear);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }

      if (cliConvertTo32BitUnsignedInteger (argv[index + 3],&stDay) == L7_SUCCESS)
      {
        cliConvertToLowerCase((L7_char8 *)argv[index + 4]);
        if (cliMonthNumberGet (argv[index + 4], &stMonth) != L7_SUCCESS)
        {
          ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_startmonth);
          cliSyntaxBottom (ewsContext);
          return cliPrompt (ewsContext);
        }
        else if ((stDay > daysInMonth [stMonth]) && 
          (!(stDay == L7_LEAP_YEAR_DAY && stMonth == L7_MONTH_FEB &&
             L7_LEAP_YEAR_CHECK(stYear))))
        {
          ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_startday);
          cliSyntaxBottom (ewsContext);
          return cliPrompt (ewsContext);
        }
      }
    }     
    
    if ((osapiStrncmp((L7_char8 *)argv[index + 1], pStrInfo_base_end,
          osapiStrnlen((L7_char8 *)argv[index + 1], L7_CLI_MAX_STRING_LENGTH)) == 0) ||
        ((numArg > 5 ) && osapiStrncmp((L7_char8 *)argv[index + 6], pStrInfo_base_end,
          osapiStrnlen((L7_char8 *)argv[index + 6], L7_CLI_MAX_STRING_LENGTH)) == 0))
    {
      endTimeSpecified = L7_TRUE;
      index = index + (stTimeSpecified * 5);
      rc = usmDbTimeConvert (argv[index + 2], &endHour, &endMin);
      if ( rc != L7_SUCCESS || endHour < L7_TIMEZONE_UHOURS_MIN || 
                               endHour > L7_TIMEZONE_UHOURS_MAX ||
                               endMin < L7_TIMEZONE_MINS_MIN ||
                               endMin > L7_TIMEZONE_MINS_MAX)
      {
        ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_endtime);
        cliSyntaxBottom (ewsContext);
        return cliPrompt (ewsContext);
      }

      rc = cliConvertTo32BitUnsignedInteger (argv[index + 5], &endYear);
      if ((rc != L7_SUCCESS) || 
          (endYear < L7_TIMERANGE_MIN_YEAR) ||
          (endYear > L7_TIMERANGE_MAX_YEAR))
      {
        ewsTelnetWrite( ewsContext, pStrErr_base_TimeRange_endyear);
        cliSyntaxBottom (ewsContext);
        return cliPrompt(ewsContext);
      }

      if (cliConvertTo32BitUnsignedInteger (argv[index + 3],&endDay) == L7_SUCCESS)
      {
        cliConvertToLowerCase((L7_char8 *)argv[index + 4]);
        if (cliMonthNumberGet (argv[index + 4], &endMonth) != L7_SUCCESS)
        {
          ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_endmonth);
          cliSyntaxBottom (ewsContext);
          return cliPrompt (ewsContext);
        }
        else if ((endDay > daysInMonth [endMonth]) && 
          (!(endDay == L7_LEAP_YEAR_DAY && endMonth == L7_MONTH_FEB &&
             L7_LEAP_YEAR_CHECK(endYear))))
        {
          ewsTelnetWrite (ewsContext, pStrErr_base_TimeRange_endday);
          cliSyntaxBottom (ewsContext);
          return cliPrompt (ewsContext);
        }
      }
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      osapiStrncpySafe(timeRangeName, EWSTIMERANGENAME(ewsContext), sizeof(timeRangeName));

      if (usmDbTimeRangeNameToIndex(unit, timeRangeName, &timeRangeIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
      }
      rc = usmDbTimeRangeEntryIndexNextFreeGet(timeRangeIndex, &timeRangeEntryNum);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                L7_NULLPTR,  ewsContext, pStrErr_base_EntryError);
        }
        else if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
        }
      }
      rc = usmDbTimeRangeEntryAdd(unit, timeRangeIndex,
                                  timeRangeEntryNum, TIMERANGE_ABSOLUTE_ENTRY);
      switch (rc)
      {
        case L7_SUCCESS:
          break;

        case L7_ERROR:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                  L7_NULLPTR,  ewsContext, pStrErr_base_TimeEntryAddError);
 
        case L7_TABLE_IS_FULL:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                  L7_NULLPTR,  ewsContext, pStrErr_base_EntryError);

        case L7_ALREADY_CONFIGURED:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                  pStrErr_common_Error,  ewsContext,
                  pStrErr_base_AbsoluteTimeEntryAlreadyExists);

       default:
          return cliSyntaxReturnPromptAddBlanks (2, 1, 0, 0,
                 pStrErr_common_Error,  ewsContext,
                 pStrErr_base_ProcessingTimeRange);
      }

      if (L7_TRUE == stTimeSpecified)
      {
        memset(&startDateAndTime, 0, sizeof(startDateAndTime));
        startDateAndTime.hour = stHour;
        startDateAndTime.minute = stMin;
        startDateAndTime.date.absoluteDate.day = stDay;
        startDateAndTime.date.absoluteDate.month = stMonth;
        startDateAndTime.date.absoluteDate.year = stYear;
        rc = usmDbTimeRangeEntryStartDateAndTimeAdd(unit,
                                                    timeRangeIndex,
                                                    timeRangeEntryNum,
                                                    TIMERANGE_ABSOLUTE_ENTRY,
                                                    &startDateAndTime);
        if (rc != L7_SUCCESS)
        {
          /*No need to check error case. This is garbage cleaner and doesn't
          throw any error message*/
          usmDbTimeRangeEntryRemove(unit, timeRangeIndex, timeRangeEntryNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AddingAbsStartTime);
        }
     }
      if (L7_TRUE == endTimeSpecified)
      {
        memset(&endDateAndTime, 0, sizeof(endDateAndTime));
        endDateAndTime.hour = endHour;
        endDateAndTime.minute = endMin;
        endDateAndTime.date.absoluteDate.day = endDay;
        endDateAndTime.date.absoluteDate.month = endMonth;
        endDateAndTime.date.absoluteDate.year = endYear;
        rc = usmDbTimeRangeEntryEndDateAndTimeAdd(unit,
                                                  timeRangeIndex,
                                                  timeRangeEntryNum,
                                                  TIMERANGE_ABSOLUTE_ENTRY,
                                                  &endDateAndTime);
        if (rc != L7_SUCCESS)
        {
         /*No need to check error case. This is garbage cleaner and doesn't
          throw any error message*/
          usmDbTimeRangeEntryRemove(unit, timeRangeIndex, timeRangeEntryNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AddingAbsEndTime);
        }
      }
    }  
     /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }
  else
  {
     /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      osapiStrncpySafe(timeRangeName, EWSTIMERANGENAME(ewsContext), sizeof(timeRangeName));

      if (usmDbTimeRangeNameToIndex(unit, timeRangeName, &timeRangeIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
      }
      if (usmDbTimeRangeAbsoluteEntryNumGet(unit, timeRangeIndex, &timeRangeEntryNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AbsoluteTimeEntryDoesNotExist);
      }
      if (usmDbTimeRangeEntryRemove(unit, timeRangeIndex, timeRangeEntryNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_AbsoluteTimeEntryDelFail);
      }
    }
       /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
   
  } 
}

