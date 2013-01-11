
#include <string.h>
#include <ctype.h>
#include "usmdb_util_api.h"
#include "usmdb_timezone_api.h"
#include "simapi.h"
#include "osapi.h"
#include "stdio.h"
#include "stdlib.h"

/*********************************************************************
*
* @purpose Convert hh:mm string to int values
*
* @param    buf     @b{(input)} character buffer with time string.
* @param    pHour   @b{(output)} hours.
* @param    pMin    @b{(output)} minutes.
*
* @notes     time may be between 00:00 and 23:59
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end

*********************************************************************/
L7_RC_t usmDbTimeConvert(const char *buf, L7_uint32 *pHour, L7_uint32 *pMin)
{
  if (sscanf(buf, "%d:%d", pHour, pMin) == 2 &&
      *pHour <= L7_TIMEZONE_UHOURS_MAX && *pHour >= L7_TIMEZONE_UHOURS_MIN &&
      *pMin <= L7_TIMEZONE_MINS_MAX && *pMin >= L7_TIMEZONE_MINS_MIN)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Convert minute offset into hour and minutes
*
* @param    minutes @b{(input)} +/- minutes.
* @param    pHour   @b{(output)} +/- hours.
* @param    pMin    @b{(output)} minutes.
*
* @notes     Converts minute offset into hour and minute offset properly
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end

*********************************************************************/
L7_RC_t usmDbMinutesOffsetConvert(L7_int32 minutes, L7_int32 *pHour, L7_int32 *pMin)
{
  *pHour = 0 - (abs(minutes) / L7_MINS_PER_HOUR);
  *pMin = abs(minutes) % L7_MINS_PER_HOUR;

  if (minutes < 0)
  {
    *pHour = 0 - *pHour;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Convert month name to number value
*
* @param    name     @b{(input)} character buffer with week name string.
* @param    number   @b{(output)} week sequence number
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end

*********************************************************************/
L7_RC_t usmDbWeekNumberGet(L7_uchar8 *name, L7_uint32 *number)
{
  L7_uint32     i;
  const L7_uchar8 *weekName[] = {
         "",
    "first",
         "",
         "",
         "",
     "last"
  };

  for(i = 1; i<=5 ; i++)
  {
    if(osapiStrCaseCmp(name, (L7_char8 *)weekName[i], sizeof(weekName[i])) == 0)
    {
      *number = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
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
L7_RC_t usmDbTimeZoneOffsetSet(L7_uint32 unit,L7_int32 minutes)
{
  return simTimeZoneOffsetSet(minutes);
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
L7_RC_t usmDbTimeZoneAcronymSet(L7_uint32 unit,L7_char8 *acronym)
{
  return simTimeZoneAcronymSet(acronym);
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
L7_RC_t usmDbTimeZoneOffsetGet(L7_uint32 unit,L7_int32 *minutes)
{
  return simTimeZoneOffsetGet(minutes);
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
L7_RC_t usmDbTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *acronym)
{
  return simTimeZoneAcronymGet(acronym);
}

/*********************************************************************
*
* @purpose  Get non-recurring summertime parameters. 
*
* @param    unit        @b{(input)}UnitIndex unit number
* @param    parameter   @b{(input)}parameter flag to be set by caller 
*                                  to get corresponding summer-time field.
* @param    ptr        @b{(output)}data, summer-time field to be returned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStNonRecurringGet(L7_uint32 unit, L7_int32 parameter, void *data)
{
  return simStNonRecurringGet(parameter,data);
}

/*********************************************************************
*
* @purpose  Get recurring summertime parameters. 
*
* @param    unit        @b{(input)}UnitIndex unit number
* @param    parameter   @b{(input)}parameter flag to be set by caller 
*                                  to get corresponding summer-time field.
* @param    ptr        @b{(output)}data, summer-time field to be returned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStRecurringGet(L7_uint32 unit,L7_int32 parameter,void *data)
{
  return simStRecurringGet( parameter,data);
}
    
/*********************************************************************
* @purpose  Set recurring summertime parameters. 
*
* @param    unit        @b{(input)}UnitIndex unit number
* @param    parameter   @b{(input)}parameter flag to be set by caller 
*                                  to get corresponding summer-time field.
* @param    ptr        @b{(output)}data, summer-time field to be returned
*
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t usmDbStRecurringSet(L7_uint32 unit,L7_int32 parameter, void *ptr)
{
 return simStRecurringSet(parameter,ptr);
}

/*********************************************************************
* @purpose  Set non-recurring summertime parameters. 
*
* @param    unit        @b{(input)}UnitIndex unit number
* @param    parameter   @b{(input)}parameter flag to be set by caller 
*                                  to get corresponding summer-time field.
* @param    ptr        @b{(output)}data, summer-time field to be returned
*
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t usmDbStNonRecurringSet(L7_uint32 unit,L7_int32 parameter,void *ptr)
{
 return simStNonRecurringSet(parameter,ptr);
}

/*************************************************************
* @purpose  Get the summer-time mode
*
* @param    unit        @b{(input)}UnitIndex unit number
* @param    mode        @b{(output)}summer-time mode to be returned
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
* @end
*********************************************************************/

L7_RC_t usmDbStModeGet(L7_uint32 unit,L7_int32 *mode)
{
  return simStModeGet(mode);
}

/*************************************************************
* @purpose  Set the summer-time mode
*
* @param    unit       @b{(input)}UnitIndex unit number
* @param    mode       @b{(input)}summer-time mode 
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
* @end
*********************************************************************/
L7_RC_t usmDbStModeSet(L7_uint32 unit, L7_int32 mode)
{
  return simStModeSet(mode);
}

/*********************************************************************
*
* @purpose  Gets the timezone acronym string
*
* @param    unit       @b{(input)}UnitIndex unit number
* @param    zoneName   @b{(input)}timezone name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSystemTimeZoneAcronymGet(L7_uint32 unit, L7_char8 *zoneName)
{
  L7_int32 mode = 0;
  if((simSummerTimeIsInEffect()==L7_TRUE) && (usmDbStModeGet(unit,&mode)==L7_SUCCESS))
  {
    if (mode == L7_SUMMER_TIME_RECURRING)
    {
      return simStRecurringGet(L7_ZONE_ACRONYM,zoneName);
    }
    else if (mode == L7_SUMMER_TIME_NON_RECURRING)
    {
      return simStNonRecurringGet(L7_ZONE_ACRONYM,zoneName);
    }
  }
  return simTimeZoneAcronymGet(zoneName);
}

/**********************************************************************
* @purpose  Get the current synchronized UTC time since the Unix Epoch.
*
* @returns  seconds since Unix epoch.
*
* @notes    This is a utility routine for SNTP internal timestamping.
*           Note that NTP epoch is different from Unix epoch. This
*           routine attempts not to return negative time if the clock
*           has not been synchronized at least once.
*
* @end
*********************************************************************/
L7_int32 usmDbUTCTimeNow()
{
 return osapiUTCTimeNow();
}
/*********************************************************************
* @purpose  Get the current UTC time since the Unix Epoch.
*
* @param    ct @b{(output)} UTC time
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t usmDbUTCTimeGet(L7_clocktime * ct)
{
 return osapiUTCTimeGet(ct);
}

/**********************************************************************
* @purpose  know whether the summer-time is in effect or not.
*
* @returns  L7_TRUE
            L7_FALSE

* @end
*********************************************************************/
L7_BOOL usmDbSummerTimeIsInEffect()
{
 return simSummerTimeIsInEffect();
}


/**********************************************************************
* @purpose  Adjusts current time for timezone and summer time
*
* @returns  Adjusted time
*           
*
* @end
*********************************************************************/
L7_uint32 usmDbAdjustedTimeGet()
{
  return simAdjustedTimeGet();
}
