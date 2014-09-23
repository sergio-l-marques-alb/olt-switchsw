
/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
* @filename k_mib_fastpath_timezone_api.c
*
* @purpose SNMP specific value conversion for timezone  MIB
*
* @component Timezone
*
* @comments
*
* @author Himabindu
*
* @end
*
**********************************************************************/

#include <stdlib.h>
#include "k_private_base.h"
#include "k_mib_fastpath_timezone_api.h"
#include "usmdb_timezone_api.h"
#include "dtlapi.h"
#include "sntp_exports.h"
#include "timezone_exports.h"
#include "usmdb_common.h"
#include "usmdb_sntp_api.h"

const L7_uint32 numberDaysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*********************************************************************
* @purpose  Get the time in hh:mm:ss format.
*
* @param    ct @b{(input)} time in seconds
* @param    ct @b{(output)} time in hh:mm:ss format
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/
L7_RC_t snmpFormattedTimeGet(L7_uint32 *seconds,L7_char8 *time)
{
  struct tm LocalTime;
  localtime_r((time_t *)seconds, &LocalTime);
  sprintf(time, "%2.2d:%2.2d:%2.2d", LocalTime.tm_hour, LocalTime.tm_min, LocalTime.tm_sec);
    
 return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the system time
*
* @param    ct @b{(output)} time in hh:mm:ss format
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t snmpSystemTimeGet(L7_uint32 unit,L7_uchar8 *sysTime)
{
 L7_uint32 seconds;
  seconds=usmDbUTCTimeNow();
  snmpFormattedTimeGet(&seconds,sysTime);
  
 return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the system date
*
* @param    ct @b{(output)} date in mon dd yyyy  format
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t snmpSystemDateGet(L7_uint32 unit,L7_uchar8 *sysDate)
{
 L7_uint32 seconds;
 struct tm *LocalTime;
 L7_char8 mon_name[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0",
                              "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0",
                              "Nov\0", "Dec\0" };

  seconds=usmDbUTCTimeNow();
  LocalTime = localtime((time_t *)&seconds);
  sprintf (sysDate, " %s %d %d ",mon_name[LocalTime->tm_mon], LocalTime->tm_mday,
           (LocalTime->tm_year +  1900));

  strcat(sysDate,"\0");

 return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the system time zone acronym
*
* @param    ct @b{(output)} zoneAcronym
*
* @returns  L7_SUCCESS
           
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t snmpSystemTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
   
  if(usmDbSystemTimeZoneAcronymGet(unit,zoneAcronym)==L7_SUCCESS)
  {
  strcat(zoneAcronym,"\0");
  return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the system time source
*
* @param    ct @b{(output)} timeSource
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpSystemTimeSourceGet(L7_uint32 unit,L7_int32 *timeSource)
{

 L7_SNTP_CLIENT_MODE_t clientMode;
 usmDbSntpClientModeGet(unit, &clientMode);
  if (clientMode == L7_SNTP_CLIENT_DISABLED)
     *timeSource=L7_TIME_SOURCE_NONE;
  else
     *timeSource=L7_TIME_SOURCE_SNTP;
 return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Get the summer-time mode
*
* @param    ct @b{(input)}mode
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
* @end
*********************************************************************/

L7_RC_t snmpStModeGet(L7_uint32 unit,L7_int32 *mode)
{
   
  return usmDbStModeGet(unit,mode);

}

/*************************************************************
*purpose  Set the summer-time mode
*
* @param    ct @b{(input)}mode 
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
* @end
*********************************************************************/

L7_RC_t snmpStModeSet(L7_uint32 unit,L7_int32 mode)
{
  L7_RC_t rc;
  
  L7_uint32 stWeek,stDay,stMonth,stHour,endWeek,endDay,endMonth,endHour,offset=0;  
  L7_char8 zone[L7_TIMEZONE_ACRONYM_LEN]="";

  rc = usmDbStModeSet(unit,mode);

  switch(mode)
  {
  case D_agentSummerTimeMode_recurringUsa:

        stWeek=L7_SUMMER_TIME_USA_START_WEEK;
        stDay=L7_SUMMER_TIME_DAY;
        stMonth=L7_SUMMER_TIME_USA_START_MONTH;
        stHour=L7_SUMMERTIME_USA_HOUR;
        endWeek=L7_SUMMER_TIME_USA_END_WEEK;
        endDay=L7_SUMMER_TIME_DAY;
        endMonth=L7_SUMMER_TIME_USA_END_MONTH;
        endHour= L7_SUMMERTIME_USA_HOUR;

        usmDbStRecurringSet(unit,L7_START_WEEK,&stWeek);
        usmDbStRecurringSet(unit,L7_START_DAY,&stDay);
        usmDbStRecurringSet(unit,L7_START_MONTH,&stMonth);
        usmDbStRecurringSet(unit,L7_START_HOUR,&stHour);
        usmDbStRecurringSet(unit,L7_END_WEEK,&endWeek);
        usmDbStRecurringSet(unit,L7_END_DAY,&endDay);
        usmDbStRecurringSet(unit,L7_END_MONTH,&endMonth);
        usmDbStRecurringSet(unit,L7_END_HOUR,&endHour);        
        usmDbStRecurringSet(unit,L7_OFFSET_MINUTES,&offset);
        usmDbStRecurringSet(unit,L7_ZONE_ACRONYM,zone);        

        break;
       
        
  case D_agentSummerTimeMode_recurringEu:

        stWeek=L7_SUMMER_TIME_EU_START_WEEK;
        stDay=L7_SUMMER_TIME_DAY;
        stMonth=L7_SUMMER_TIME_EU_START_MONTH;
        stHour=L7_SUMMERTIME_EU_START_HOUR;
        endWeek=L7_SUMMER_TIME_EU_END_WEEK;
        endDay=L7_SUMMER_TIME_DAY;
        endMonth=L7_SUMMER_TIME_EU_END_MONTH;
        endHour= L7_SUMMERTIME_EU_END_HOUR;

        usmDbStRecurringSet(unit,L7_START_WEEK,&stWeek);
        usmDbStRecurringSet(unit,L7_START_DAY,&stDay);
        usmDbStRecurringSet(unit,L7_START_MONTH,&stMonth);
        usmDbStRecurringSet(unit,L7_START_HOUR,&stHour);
        usmDbStRecurringSet(unit,L7_END_WEEK,&endWeek);
        usmDbStRecurringSet(unit,L7_END_DAY,&endDay);
        usmDbStRecurringSet(unit,L7_END_MONTH,&endMonth);
        usmDbStRecurringSet(unit,L7_END_HOUR,&endHour);
        usmDbStRecurringSet(unit,L7_OFFSET_MINUTES,&offset);
        usmDbStRecurringSet(unit,L7_ZONE_ACRONYM,zone);
        break;
  }


  return rc;

}
/*************************************************************
*purpose  know whether summer time is in effect or not
*
* @param    ct @b{(output)}state
*
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpSystemSummerTimeStateGet(L7_uint32 unit,L7_int32 *state)
{
   
  if(usmDbSummerTimeIsInEffect()==L7_TRUE)
    *state=L7_ENABLE;
  else
    *state=L7_DISABLE;
  
  return L7_SUCCESS;
} 

/*********************************************************************
* @purpose  Get the timezone hours offset
*
* @param    ct @b{(output)} hoursOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneHoursOffsetGet(L7_uint32 unit,L7_int32 *hoursOffset)
{
  L7_int32 minutes;
  if (usmDbTimeZoneOffsetGet(unit,&minutes) == L7_SUCCESS)
  {
    *hoursOffset = minutes / L7_MINS_PER_HOUR;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the timezone minutes offset
*
* @param    ct @b{(output)} minutesOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneMinutesOffsetGet(L7_uint32 unit,L7_int32 *minutesOffset)
{
  L7_int32 minutes;
  if (usmDbTimeZoneOffsetGet(unit,&minutes) == L7_SUCCESS)
  {
    *minutesOffset = minutes % L7_MINS_PER_HOUR;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the system time source
*
* @param    ct @b{(output)} hoursOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
 
  return  usmDbTimeZoneAcronymGet(unit,zoneAcronym);
 
}

/*********************************************************************
* @purpose  Set the timezone hours offset
*
* @param    ct @b{(output)} Offset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneHoursOffsetSet(L7_uint32 unit, L7_int32 offset)
{
  L7_int32 minutes;

  if (usmDbTimeZoneOffsetGet(unit,&minutes) == L7_SUCCESS)
  {
    minutes = abs(minutes) % L7_MINS_PER_HOUR;

    return usmDbTimeZoneOffsetSet(unit,offset * L7_MINS_PER_HOUR + (offset < 0 ? (0-minutes) : minutes) );
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the timezone minutes offset
*
* @param    ct @b{(output)} Offset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneMinutesOffsetSet(L7_uint32 unit, L7_int32 offset)
{
  L7_int32 minutes;
  L7_int32 hours;

  if (usmDbTimeZoneOffsetGet(unit,&minutes) == L7_SUCCESS)
  {
    hours = minutes / L7_MINS_PER_HOUR;

    return usmDbTimeZoneOffsetSet(unit,hours * L7_MINS_PER_HOUR + ((hours < 0) ? (0-offset) : offset));
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the timezone acronym
*
* @param    ct @b{(output)} acronym
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
  if(strlen(zoneAcronym)>L7_TIMEZONE_ACRONYM_LEN)
    return L7_FAILURE; 
  return usmDbTimeZoneAcronymSet(unit,zoneAcronym);

}

    
/*********************************************************************
* @purpose  Get the summer-time recurring zone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
 
 if(usmDbStRecurringGet(unit,L7_ZONE_ACRONYM,zoneAcronym)==L7_SUCCESS)
   {
    strcat(zoneAcronym,"\0");
    return L7_SUCCESS;
   
   }
 return L7_FAILURE;
 
}

/*********************************************************************
* @purpose  Get the  recurring summer-time StartingTime in hh:mm format
*
* @param    ct @b{(input)} startingTime
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringStartingTimeGet(L7_uint32 unit,L7_char8 *time)
{
  L7_uint32 hours,minutes;
  
  if((usmDbStRecurringGet(unit,L7_START_HOUR,&hours)==L7_SUCCESS)&&
     (usmDbStRecurringGet(unit,L7_START_MINUTE,&minutes)==L7_SUCCESS))
     {
       minutes=(minutes * L7_MINS_PER_HOUR) + (hours * L7_MINS_PER_HOUR * L7_MINS_PER_HOUR);
       snmpFormattedTimeGet(&minutes,time);
       return L7_SUCCESS;
     }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the  recurring summer-time endTime in hh:mm format
*
* @param    ct @b{(input)} endingTime
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringEndingTimeGet(L7_uint32 unit,L7_char8 *time)
{
  L7_uint32 hours,minutes;
  
  if((usmDbStRecurringGet(unit,L7_END_HOUR,&hours)==L7_SUCCESS)&&
     (usmDbStRecurringGet(unit,L7_END_MINUTE,&minutes)==L7_SUCCESS))
     {
       minutes=(minutes * L7_MINS_PER_HOUR) + (hours * L7_MINS_PER_HOUR * L7_MINS_PER_HOUR);
       snmpFormattedTimeGet(&minutes,time);
       return L7_SUCCESS;
     }

  return L7_FAILURE;
}
 

  
/*********************************************************************
* @purpose  Set the recurring summer-time starting time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringStartingTimeSet(L7_uint32 unit,L7_char8 *time)
{
   L7_uint32 hours,minutes;
   
   if((usmDbTimeConvert(time,&hours,&minutes)==L7_SUCCESS)&&
      (usmDbStRecurringSet(unit,L7_START_HOUR,&hours)==L7_SUCCESS)&&
      (usmDbStRecurringSet(unit,L7_START_MINUTE,&minutes)==L7_SUCCESS))
       return L7_SUCCESS;

    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Set the recurring summer-time ending time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringEndingTimeSet(L7_uint32 unit,L7_char8 *time)
{
  
    L7_uint32 hours,minutes;
    
    if((usmDbTimeConvert(time,&hours,&minutes)==L7_SUCCESS)&&
       (usmDbStRecurringSet(unit,L7_END_HOUR,&hours)==L7_SUCCESS)&&
       (usmDbStRecurringSet(unit,L7_END_MINUTE,&minutes)==L7_SUCCESS))
       return L7_SUCCESS;

    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Set the recurring summer-time timeZone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
    if(strlen(zoneAcronym)>L7_TIMEZONE_ACRONYM_LEN)
    return L7_FAILURE;
   return usmDbStRecurringSet(unit,L7_ZONE_ACRONYM,zoneAcronym);
  
}

  
/*********************************************************************
* @purpose  Get the summer-time non recurring zone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
 
 return usmDbStNonRecurringGet(unit,L7_ZONE_ACRONYM,zoneAcronym);
}

/*********************************************************************
* @purpose  Get the  Non recurring summer-time StartingTime in hh:mm format
*
* @param    ct @b{(input)} startingTime
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringStartingTimeGet(L7_uint32 unit,L7_char8 *time)
{
  
  L7_int32 minutes,hours;
  
  if((usmDbStNonRecurringGet(unit,L7_START_HOUR,&hours)==L7_SUCCESS)&&
     (usmDbStNonRecurringGet(unit,L7_START_MINUTE,&minutes)==L7_SUCCESS))
     {
      minutes=(minutes * L7_MINS_PER_HOUR) + (hours * L7_MINS_PER_HOUR * L7_MINS_PER_HOUR);
      snmpFormattedTimeGet(&minutes,time);
      return  L7_SUCCESS;
     } 

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the  Non recurring summer-time endTime in hh:mm format
*
* @param    ct @b{(input)} endingTime
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringEndingTimeGet(L7_uint32 unit,L7_char8 *time)
{
 
  L7_int32 minutes,hours;
  
  if((usmDbStNonRecurringGet(unit,L7_END_HOUR,&hours)==L7_SUCCESS)&&
     (usmDbStNonRecurringGet(unit,L7_END_MINUTE,&minutes)==L7_SUCCESS))
     {
      minutes=(minutes * L7_MINS_PER_HOUR) + (hours * L7_MINS_PER_HOUR * L7_MINS_PER_HOUR);
      snmpFormattedTimeGet(&minutes,time);
      return L7_SUCCESS;
    }

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Set the Non recurring summer-time starting time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringStartingTimeSet(L7_uint32 unit,L7_char8 *time)
{
   L7_uint32 hours,minutes;

   if((usmDbTimeConvert(time,&hours,&minutes)==L7_SUCCESS)&&
      (usmDbStNonRecurringSet(unit,L7_START_HOUR,&hours)==L7_SUCCESS)&&
      (usmDbStNonRecurringSet(unit,L7_START_MINUTE,&minutes)==L7_SUCCESS))
      return  L7_SUCCESS;
   return L7_FAILURE;

}
/*********************************************************************
* @purpose  Set the Non recurring summer-time ending time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringEndingTimeSet(L7_uint32 unit,L7_char8 *time)
{
   L7_uint32 hours,minutes;

   if((usmDbTimeConvert(time,&hours,&minutes)==L7_SUCCESS)&&
      (usmDbStNonRecurringSet(unit,L7_END_HOUR,&hours)==L7_SUCCESS)&&
       (usmDbStNonRecurringSet(unit,L7_END_MINUTE,&minutes)==L7_SUCCESS))
        return L7_SUCCESS;

   return L7_FAILURE;

}

/*********************************************************************
* @purpose  Set the Non recurring summer-time timeZone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym)
{
   if(strlen(zoneAcronym)>L7_TIMEZONE_ACRONYM_LEN)
    return L7_FAILURE;
  return usmDbStNonRecurringSet(unit,L7_ZONE_ACRONYM,zoneAcronym);
  
}
