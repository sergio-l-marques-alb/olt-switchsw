
#ifndef       _K_MIB_FASTPATH_TIMEZONE_API_H_
#define       _K_MIB_FASTPATH_TIMEZONE_API_H_

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the time in hh:mm format.
*
* @param    ct @b{(input)} time in seconds
* @param    ct @b{(output)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t snmpFormattedTimeGet(L7_uint32 *seconds,L7_char8 *time);


/*********************************************************************
* @purpose  Get the system time
*
* @param    ct @b{(output)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/

L7_RC_t snmpSystemTimeGet(L7_uint32 unit,L7_uchar8 *sysTime);

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

L7_RC_t snmpSystemDateGet(L7_uint32 unit,L7_uchar8 *sysDate);


/*************************************************************
* purpose  convert the current time into seconds
*
* @param    ct @b{(input)}mode
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
* @end
*********************************************************************/

void snmpSummerTimeConvert(void);


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

L7_RC_t snmpSystemTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym);
/*********************************************************************
* @purpose  Get the system time source
*
* @param    ct @b{(output)} timeSource
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpSystemTimeSourceGet(L7_uint32 unit,L7_int32 *timeSource);
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

L7_RC_t snmpStModeGet(L7_uint32 unit,L7_int32 *mode);
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

L7_RC_t snmpStModeSet(L7_uint32 unit,L7_int32 mode);
/*************************************************************
*purpose  know whether summer time is in effect or not
*
* @param    ct @b{(output)}state
*
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpSystemSummerTimeStateGet(L7_uint32 unit,L7_int32 *state);
/*********************************************************************
* @purpose  Get the timezone hours offset
*
* @param    ct @b{(output)} hoursOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneHoursOffsetGet(L7_uint32 unit,L7_int32 *hoursOffset);
/*********************************************************************
* @purpose  Get the timezone minutes offset
*
* @param    ct @b{(output)} minutesOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneMinutesOffsetGet(L7_uint32 unit,L7_int32 *minutesOffset);
/*********************************************************************
* @purpose  Get the system time source
*
* @param    ct @b{(output)} hoursOffset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym);

/*********************************************************************
* @purpose  Set the timezone hours offset
*
* @param    ct @b{(output)} Offset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneHoursOffsetSet(L7_uint32 unit,L7_int32 offset);

/*********************************************************************
* @purpose  Set the timezone minutes offset
*
* @param    ct @b{(output)} Offset
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneMinutesOffsetSet(L7_uint32 unit,L7_int32 offset);
/*********************************************************************
* @purpose  Set the timezone acronym
*
* @param    ct @b{(output)} acronym
*
* @returns  L7_SUCCESS

* @end
*********************************************************************/

L7_RC_t snmpTimeZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym);

/*********************************************************************
* @purpose  Validate the configured summer-time date information.
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpSummerTimeValidate(L7_int32 flag);
/*********************************************************************
* @purpose  Get the summer-time recurring zone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym);
/*********************************************************************
* @purpose  Get the  recurring summer-time StartingTime in hh:mm format
*
* @param    ct @b{(input)} startingTime
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringStartingTimeGet(L7_uint32 unit,L7_char8 *time);
/*********************************************************************
* @purpose  Get the  recurring summer-time endTime in hh:mm format
*
* @param    ct @b{(input)} endingTime
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringEndingTimeGet(L7_uint32 unit,L7_char8 *time);

/*********************************************************************
* @purpose  Set the recurring summer-time starting time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringStartingTimeSet(L7_uint32 unit,L7_char8 *time);
/*********************************************************************
* @purpose  Set the recurring summer-time ending time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringEndingTimeSet(L7_uint32 unit,L7_char8 *time);
/*********************************************************************
* @purpose  Set the recurring summer-time timeZone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
*********************************************************************/

L7_RC_t snmpStRecurringZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym);

/*********************************************************************
* @purpose  Get the summer-time non recurring zone Acronym
*
* @param    ct @b{(input)} zoneAcronym
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringZoneAcronymGet(L7_uint32 unit,L7_char8 *zoneAcronym);
/*********************************************************************
* @purpose  Get the  Non recurring summer-time StartingTime in hh:mm format
*
* @param    ct @b{(input)} startingTime
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringStartingTimeGet(L7_uint32 unit,L7_char8 *time);
/*********************************************************************
* @purpose  Get the  Non recurring summer-time endTime in hh:mm format
*
* @param    ct @b{(input)} endingTime
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringEndingTimeGet(L7_uint32 unit,L7_char8 *time);

/*********************************************************************
* @purpose  Set the Non recurring summer-time starting time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringStartingTimeSet(L7_uint32 unit,L7_char8 *time);
/*********************************************************************
* @purpose  Set the Non recurring summer-time ending time
*
* @param    ct @b{(input)} time in hh:mm format
*
* @returns  L7_SUCCESS
            L7_FAILURE

* @end
*********************************************************************/

L7_RC_t snmpStNonRecurringEndingTimeSet(L7_uint32 unit,L7_char8 *time);
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

L7_RC_t snmpStNonRecurringZoneAcronymSet(L7_uint32 unit,L7_char8 *zoneAcronym);

#endif 
