/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/broadcom/cli/base/timeranges/cli_show_running_config_timerange.c
 *
 * @purpose show running config commands for time range
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  16 dec 2009
 *
 * @author  Siva Mannem
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "dhcps_exports.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcps_api.h"
#include "defaultconfig.h"
#include "ews.h"
#include "ews_cli.h"
#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"

#include "timerange_exports.h"
#include "usmdb_timerange_api.h"

#define L7_WEEK_END ((1 << L7_DAY_SAT) | (1 << L7_DAY_SUN))

#define L7_WEEK_DAYS ((1 << L7_DAY_MON) | (1 << L7_DAY_TUE) | \
                      (1 << L7_DAY_WED) | (1 << L7_DAY_THU) | \
                      (1 << L7_DAY_FRI)) 

#define L7_DAILY ((L7_WEEK_END) | (L7_WEEK_DAYS))
extern const L7_uchar8 *monthsOfYear[];
/*********************************************************************
* @purpose  To print the running configuration of Time Range
*
* @param    L7_uchar8 daysOfTheWeek     bit mask of days of the week.
* @param    L7_uchar8 *daysOfTheWeekStr String equivalent for the bit
*                                       mask
*
* @returns  void
*
* @notes
*
   @end
*********************************************************************/
static void cliDaysOfWeekGet(L7_uchar8 daysOfTheWeek, L7_uchar8 *daysOfTheWeekStr);

/*********************************************************************
* @purpose  To print the running configuration of Time Range
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningConfigTimeRangeInfo(EwsContext ewsContext, L7_uint32 unit)
{
  static L7_uint32 timeRangeIndex;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 entryNum, rc;
  L7_char8 timeRangeName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 dayOftheWeekStr[L7_CLI_MAX_STRING_LENGTH];
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t entryType;
  
  if (usmDbTimeRangeIndexFirstGet(unit, &timeRangeIndex) == L7_SUCCESS)
  {
    do
    /* this section is executed for both the first and successive Time Ranges */
    {
      memset (timeRangeName, 0, sizeof(timeRangeName));
      rc = usmDbTimeRangeNameGet(unit, timeRangeIndex, timeRangeName);

      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_timerange_1, timeRangeName );
      EWSWRITEBUFFER(ewsContext,stat);

      if (usmDbTimeRangeEntryGetFirst(unit, timeRangeIndex, &entryNum) == L7_SUCCESS)
      {
        do
        /* this section is executed for both the first and successive rules */
        {
          memset (stat, 0, sizeof(stat));
          if (usmDbTimeRangeEntryTypeGet(unit, timeRangeIndex, entryNum, &entryType) == L7_SUCCESS)
          {
            if (entryType == TIMERANGE_ABSOLUTE_ENTRY)
            {
              strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_absolute);
            }
            else if (entryType == TIMERANGE_PERIODIC_ENTRY)
            {
              strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_periodic);
            }

            if (entryType == TIMERANGE_ABSOLUTE_ENTRY)
            {
              if (usmDbTimeRangeIsFieldConfigured(unit,
                                  timeRangeIndex,
                                  entryNum,
                                  TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
              {
                rc = usmDbTimeRangeEntryStartDateAndTimeGet(unit,
                                              timeRangeIndex,
                                              entryNum,
                                              &entryType,
                                              &startDateAndTime);
                if (rc == L7_SUCCESS)
                {
                  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                                              "%s %02d:%02d %02d %s %04d",
                                              pStrInfo_base_start,
                                              startDateAndTime.hour,
                                              startDateAndTime.minute,
                                              startDateAndTime.date.absoluteDate.day,
                                              monthsOfYear[startDateAndTime.date.absoluteDate.month],
                                              startDateAndTime.date.absoluteDate.year);
                                                    
                  strcat(stat, buf);
                }
              }

              if (usmDbTimeRangeIsFieldConfigured(unit,
                                  timeRangeIndex,
                                  entryNum,
                                  TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
              {
                rc = usmDbTimeRangeEntryEndDateAndTimeGet(unit,
                                              timeRangeIndex,
                                              entryNum,
                                              &entryType,
                                              &endDateAndTime);
                if (rc == L7_SUCCESS)
                {
                  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                                              "%s %02d:%02d %02d %s %04d",
                                              pStrInfo_base_end,
                                              endDateAndTime.hour,
                                              endDateAndTime.minute,
                                              endDateAndTime.date.absoluteDate.day,
                                              monthsOfYear[endDateAndTime.date.absoluteDate.month],
                                              endDateAndTime.date.absoluteDate.year);
                                                   
                  strcat(stat, buf);
                }
              }
            }
            else
            {
              rc = usmDbTimeRangeEntryStartDateAndTimeGet(unit,
                                            timeRangeIndex,
                                            entryNum,
                                            &entryType,
                                            &startDateAndTime);
              if (rc == L7_SUCCESS &&
                  entryType == TIMERANGE_PERIODIC_ENTRY)
              {
                memset (dayOftheWeekStr, 0, sizeof(dayOftheWeekStr));
                cliDaysOfWeekGet(startDateAndTime.date.daysOfTheWeek.dayMask, dayOftheWeekStr),
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                                                    "%s%02d:%02d",
                                                    dayOftheWeekStr,
                                                    startDateAndTime.hour,
                                                    startDateAndTime.minute);
                                                         
                strcat(stat, buf);

              }
              rc = usmDbTimeRangeEntryEndDateAndTimeGet(unit,
                                            timeRangeIndex,
                                            entryNum,
                                            &entryType,
                                            &endDateAndTime);
              if (rc == L7_SUCCESS &&
                  entryType == TIMERANGE_PERIODIC_ENTRY)
              {
                if (startDateAndTime.date.daysOfTheWeek.dayMask != 
                    endDateAndTime.date.daysOfTheWeek.dayMask)
                {
                  memset (dayOftheWeekStr, 0, sizeof(dayOftheWeekStr));
                  cliDaysOfWeekGet(endDateAndTime.date.daysOfTheWeek.dayMask, dayOftheWeekStr),
                  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                                                    "%s %s%02d:%02d",
                                                    pStrInfo_base_periodic_to,
                                                    dayOftheWeekStr,
                                                    endDateAndTime.hour,
                                                    endDateAndTime.minute);
                }
                else
                {
                  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                                                    "%s %02d:%02d",
                                                    pStrInfo_base_periodic_to,
                                                    endDateAndTime.hour,
                                                    endDateAndTime.minute);

                }
                                                         
                strcat(stat, buf);

              }
            }
            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
        while (usmDbTimeRangeEntryGetNext(unit, timeRangeIndex, entryNum, &entryNum) == L7_SUCCESS);
      }

      osapiSnprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Exit);
      EWSWRITEBUFFER(ewsContext, buf);

    } while (usmDbTimeRangeIndexNextGet(unit, timeRangeIndex, &timeRangeIndex) == L7_SUCCESS);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of Time Range
*
* @param    L7_uchar8 daysOfTheWeek     bit mask of days of the week.
* @param    L7_uchar8 *daysOfTheWeekStr String equivalent for the bit
*                                       mask
*
* @returns  void
*
* @notes
*
   @end
*********************************************************************/
static void cliDaysOfWeekGet(L7_uchar8 daysOfTheWeek, L7_uchar8 *daysOfTheWeekStr)
{
  if (daysOfTheWeek == L7_DAILY)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_daily);
    return ;
  }
  else if (daysOfTheWeek == L7_WEEK_DAYS)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_weekdays);
    return ;
  }
  else if (daysOfTheWeek == L7_WEEK_END)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_weekends);
    return ;
  }

  if ( daysOfTheWeek & (1 << L7_DAY_SUN ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_sun);
  }

  if ( daysOfTheWeek & (1 << L7_DAY_MON ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_mon);
  }
  if ( daysOfTheWeek & (1 << L7_DAY_TUE ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_tue);
  }
  if ( daysOfTheWeek & (1 << L7_DAY_WED ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_wed);
  }
  if ( daysOfTheWeek & (1 << L7_DAY_THU ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_thu);
  }
  if ( daysOfTheWeek & (1 << L7_DAY_FRI ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_fri);
  }
  if ( daysOfTheWeek & (1 << L7_DAY_SAT ))
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, daysOfTheWeekStr, pStrInfo_base_periodic_sat);
  }
  return;
}

