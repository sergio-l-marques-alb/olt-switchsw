/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
 * @filename src/mgmt/broadcom/cli/base/timeranges/cli_show_timerange.c 
*
* @purpose Time range show commands for CLI
*
* @component user interface
*
* @comments none
*
* @create  Dec 16, 2009
*
* @author  Siva Mannem
* @end
*
**********************************************************************/
#include <stdio.h>
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include "clicommands_card.h"

#include "timerange_exports.h"
#include "usmdb_timerange_api.h"
extern const L7_uchar8 *monthsOfYear[];

extern const L7_uchar8 *aggMonthsOfYear[];

extern const L7_uchar8 *daysOfWeek[];

extern const L7_uchar8 *weekName[];

extern const L7_uchar8 *dayOfWeek[];

/*********************************************************************
*
* @purpose  Display days of the week given the bit mask
*
* @param    EwsContext ewsContext
* @param    L7_uchar8  daysOfTheWeek
*
* @end
*
*********************************************************************/
static void cliDaysOfWeekPrint(EwsContext ewsContext, L7_uchar8 daysOfTheWeek)
{
  if ( daysOfTheWeek & (1 << L7_DAY_SUN ))
  {
    ewsTelnetPrintf(ewsContext,"SUN ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_MON ))
  {
    ewsTelnetPrintf(ewsContext,"MON ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_TUE ))
  {
    ewsTelnetPrintf(ewsContext,"TUE ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_WED ))
  {
    ewsTelnetPrintf(ewsContext,"WED ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_THU ))
  {
    ewsTelnetPrintf(ewsContext,"THU ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_FRI ))
  {
    ewsTelnetPrintf(ewsContext,"FRI ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_SAT ))
  {
    ewsTelnetPrintf(ewsContext,"SAT ");
  }

}


/*********************************************************************
*
* @purpose  Display Time Range information
*
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
* @cmdsyntax  command show time-range [<name>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowTimeRange(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  static L7_uint32 timeRangeIndex;
  L7_uint32 argName = 1;
  L7_uint32 unit;
  L7_uint32 absEntryNum, entryNum;
  L7_char8  timeRangeName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numwrites, numArgs;
  L7_uint32 currentTimeRanges, maxTimeRanges;
  L7_BOOL   timeRangesConfigured;
  L7_char8  buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;
  L7_uint32                        entryCount;
  L7_BOOL                          absEntryPresent = L7_FALSE;
  L7_BOOL   headerPrinted = L7_FALSE;
  timeRangeStatus_t                status;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  /* Summary */
  if (numArgs == 0)     /* parameter check */
  {
    cliCmdScrollSet(L7_FALSE);
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      numwrites = 0;
      /* if our question has been answered */
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      timeRangesConfigured = L7_FALSE;
      if (usmDbTimeRangeIndexFirstGet(unit, &timeRangeIndex) == L7_SUCCESS)
      {
        if (usmDbTimeRangeNameGet(unit, timeRangeIndex, timeRangeName) == L7_SUCCESS)
        {
          timeRangesConfigured = L7_TRUE;
        }
      }

      if (timeRangesConfigured != L7_TRUE)
      {  /* no Time Ranges configured */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoTRsAreCfgured_1);
      }

      numwrites = 0;
      if (usmDbTimeRangeMaxNumGet(unit, &maxTimeRanges) == L7_SUCCESS)
      {
        if (usmDbTimeRangeCurrNumGet(unit, &currentTimeRanges) == L7_SUCCESS)
        {
          sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf, pStrErr_base_CfgTRMaxAndCurrentTR, currentTimeRanges, maxTimeRanges);
          numwrites += 2;
          ewsTelnetWrite(ewsContext, buf);
        }
        cliSyntaxTop(ewsContext);
      }
    }

    rc = L7_SUCCESS;
    while((rc == L7_SUCCESS) && numwrites <= (CLI_MAX_SCROLL_LINES-6))
    {
      if (headerPrinted == L7_FALSE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_TRNameStatusPeriodic);
        ewsTelnetWrite(ewsContext, pStrInfo_base_EntryCntAbsEntry);
        ewsTelnetWrite(ewsContext, pStrInfo_base_TRdash);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
        headerPrinted = L7_TRUE;
        numwrites += 3;
      }

      rc = usmDbTimeRangeNameGet(unit, timeRangeIndex, timeRangeName);
      ewsTelnetPrintf(ewsContext, "%-32s", timeRangeName);
      rc = usmDbTimeRangeStatusGet(unit, timeRangeIndex, &status);
      ewsTelnetPrintf (ewsContext, "%-9s",
                       status == TIMERANGE_STATUS_ACTIVE ? \
                       pStrInfo_base_TRActive : pStrInfo_base_TRInActive );

      rc = usmDbTimeRangeAbsoluteEntryNumGet(unit, timeRangeIndex, &absEntryNum);

      if (rc == L7_SUCCESS)
      {
        absEntryPresent = L7_TRUE;
      }
      else
      {
        absEntryPresent = L7_FALSE;
      }

      rc = usmDbTimeRangeNumEntriesGet(unit, timeRangeIndex, &entryCount);
      if (rc == L7_SUCCESS)
      {
        if (absEntryPresent == L7_TRUE && entryCount != 0)
        {
          entryCount = entryCount - 1;
        }
        ewsTelnetPrintf (ewsContext, "    %-8d", entryCount);
      }

      if (absEntryPresent == L7_TRUE)
      {
        ewsTelnetPrintf (ewsContext, "%-15s", pStrInfo_base_exist);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-15s", pStrInfo_base_doesnotexist);
      }
      rc = usmDbTimeRangeIndexNextGet(unit, timeRangeIndex, &timeRangeIndex);
      numwrites ++;
      ewsTelnetWrite(ewsContext,
                   pStrInfo_common_CrLf);
    }
    
    if (rc == L7_SUCCESS)
    {  
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliAlternateCommandSet(pStrErr_base_ShowTimeRange_1);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
    else
    { 
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /* Specific time-range */
  else if (numArgs == 1)
  {

    if (strlen(argv[index+argName]) > L7_TIMERANGE_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidTimeRangeName);
    }

    osapiStrncpySafe(timeRangeName,argv[index+argName], sizeof(timeRangeName));

    if (usmDbTimeRangeNameStringCheck(unit, timeRangeName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidTimeRangeName);
    }

    if (usmDbTimeRangeNameToIndex(unit, timeRangeName, &timeRangeIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TimeRangeDoesntExist);
    }

    cliFormat(ewsContext, pStrInfo_base_TimeRangeName);
    ewsTelnetPrintf (ewsContext, "%s", timeRangeName);
    if (usmDbTimeRangeStatusGet(unit, timeRangeIndex, &status) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                pStrErr_base_TimeRangeStatusGet, timeRangeName);
    }
    cliFormat(ewsContext, pStrInfo_base_TimeRangeStatus);
    ewsTelnetPrintf (ewsContext, "%s",
                       status == TIMERANGE_STATUS_ACTIVE ? \
                       pStrInfo_base_TRActive : pStrInfo_base_TRInActive );

    rc = usmDbTimeRangeAbsoluteEntryNumGet(unit, timeRangeIndex, &absEntryNum);
    if (rc == L7_SUCCESS)
    {
      absEntryPresent = L7_TRUE; 
      cliSyntaxBottom(ewsContext);
      ewsTelnetPrintf (ewsContext, "\r\n%s %d", pStrInfo_base_EntryNum, absEntryNum);

      if (usmDbTimeRangeIsFieldConfigured(unit,
                          timeRangeIndex,
                          absEntryNum,
                          TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
      {
        rc = usmDbTimeRangeEntryStartDateAndTimeGet(unit,
                                      timeRangeIndex,
                                      absEntryNum,
                                      &entryType,
                                      &startDateAndTime);
        if (rc == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_base_AbsoluteStartTime);
          ewsTelnetPrintf (ewsContext, "%02d:%02d %02d %s %04d",
                                       startDateAndTime.hour,
                                       startDateAndTime.minute,
                                       startDateAndTime.date.absoluteDate.day,
                                       monthsOfYear[startDateAndTime.date.absoluteDate.month],
                                       startDateAndTime.date.absoluteDate.year);
        }
      }

      if (usmDbTimeRangeIsFieldConfigured(unit,
                          timeRangeIndex,
                          absEntryNum,
                          TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
      {
        rc = usmDbTimeRangeEntryEndDateAndTimeGet(unit,
                                      timeRangeIndex,
                                      absEntryNum,
                                      &entryType,
                                      &endDateAndTime);
        if (rc == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_base_AbsoluteEndTime);
          ewsTelnetPrintf (ewsContext, "%02d:%02d %02d %s %04d",
                                       endDateAndTime.hour,
                                       endDateAndTime.minute,
                                       endDateAndTime.date.absoluteDate.day,
                                       monthsOfYear[endDateAndTime.date.absoluteDate.month],
                                       endDateAndTime.date.absoluteDate.year);
        }

      }
    }
 
    rc = usmDbTimeRangeNumEntriesGet(unit, timeRangeIndex, &entryCount);
    if (rc == L7_SUCCESS)
    {
      if (absEntryPresent == L7_TRUE && entryCount != 0)
      {
        entryCount = entryCount - 1;
        if (entryCount != 0)
        {
          cliSyntaxBottom(ewsContext);
          ewsTelnetPrintf (ewsContext, "\r\n%s %d", pStrInfo_base_PeriodicEntries, entryCount);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                         ewsContext,
                         pStrInfo_base_NoPerEntriesHaveBeenCfguredForTimeRange);
        }
      }
    }
       /* since this is the first time, get a new entryNum */
    rc = usmDbTimeRangeEntryGetFirst(unit, timeRangeIndex, &entryNum);

    if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoEntriesHaveBeenCfguredForTimeRange);
    }
    else if (rc == L7_SUCCESS)
    {
      do /* this section is executed for both the first and successive times */
      {
        if (entryNum == absEntryNum)
        {
          continue;
        }
        cliSyntaxBottom(ewsContext);
        ewsTelnetPrintf (ewsContext, "\r\n%s %d", pStrInfo_base_EntryNum, entryNum);

        rc = usmDbTimeRangeEntryStartDateAndTimeGet(unit,
                                      timeRangeIndex,
                                      entryNum,
                                      &entryType,
                                      &startDateAndTime);
        if (rc == L7_SUCCESS &&
            entryType == TIMERANGE_PERIODIC_ENTRY)
        {
          cliFormat(ewsContext, pStrInfo_base_PeriodicStartTime);
          cliDaysOfWeekPrint(ewsContext, startDateAndTime.date.daysOfTheWeek.dayMask);
          ewsTelnetPrintf (ewsContext, "%02d:%02d",
                                       startDateAndTime.hour,
                                       startDateAndTime.minute);
        }
        rc = usmDbTimeRangeEntryEndDateAndTimeGet(unit,
                                      timeRangeIndex,
                                      entryNum,
                                      &entryType,
                                      &endDateAndTime);
        if (rc == L7_SUCCESS &&
            entryType == TIMERANGE_PERIODIC_ENTRY)
        {
          cliFormat(ewsContext, pStrInfo_base_PeriodicEndTime);
          cliDaysOfWeekPrint(ewsContext, endDateAndTime.date.daysOfTheWeek.dayMask);
          ewsTelnetPrintf (ewsContext, "%02d:%02d",
                                       endDateAndTime.hour,
                                       endDateAndTime.minute);
        }

      } while(usmDbTimeRangeEntryGetNext(unit, timeRangeIndex, entryNum, &entryNum) == L7_SUCCESS);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowTimeRange);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

