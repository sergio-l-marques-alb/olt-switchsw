#ifndef USMDB_TIMEZONE_API_H
#define USMDB_TIMEZONE_API_H

#include "l7_common.h"
#include "osapi.h"
#include "sntp_exports.h"
#include "timezone_exports.h"

/*********************************************************************
*
* @purpose Convert hh:mm string to int values
*
* @param   buf
* @param   *hour
* @param   *minute
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end

*********************************************************************/
 L7_RC_t usmDbTimeConvert(const char *buf, L7_uint32 *pHour, L7_uint32 *pMin);

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit unsigned integer
*          the value is >= 0 and <= 4294967295.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-numeric character
*              is included in buf.
*
* @end
*
*********************************************************************/
 L7_RC_t convertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal);


/*********************************************************************
*
* @purpose Convert month name to number value
*
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end

*********************************************************************/
 L7_RC_t usmDbWeekNumberGet(L7_uchar8 *name, L7_uint32 *number);

/*********************************************************************
*
* @purpose  Gets the timezone acronym string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbSystemTimeZoneAcronymGet (L7_uint32 unit, L7_char8 *zoneName);

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
L7_RC_t usmDbTimeZoneOffsetSet(L7_uint32 unit,L7_int32 minutes);

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
L7_RC_t usmDbTimeZoneAcronymSet(L7_uint32 unit,L7_char8 *acronym);


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
L7_RC_t usmDbTimeZoneOffsetGet(L7_uint32 unit,L7_int32 *minutes);

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
L7_RC_t usmDbTimeZoneAcronymGet(L7_uint32 unit,L7_char8 *acronym);

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
 L7_RC_t usmDbStNonRecurringGet(L7_uint32 unit,L7_int32 parameter,void *data);

/*********************************************************************
*
* @purpose  Get recurring summertime parameters. 
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
 L7_RC_t usmDbStRecurringGet(L7_uint32 unit,L7_int32 parameter,void *data);

/*********************************************************************
* @purpose  Set recurring summertime parameters. 
*
* @param    L7_uint32     UnitIndex unit number
* @param    L7_uint32     parameter flag to be set by caller to get corresponding
*                         summer-time parameter.
* @param    void *        data, summer-time field to be returned
*
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @end
*********************************************************************/
 L7_RC_t usmDbStRecurringSet(L7_uint32 unit,L7_int32 parameter, void *ptr);

/*********************************************************************
* @purpose  Set non-recurring summertime parameters. 
*
* @param    L7_uint32     UnitIndex unit number
* @param    L7_uint32     parameter flag to be set by caller to get corresponding
*                         summer-time parameter.
* @param    void *        data, summer-time field to be returned
*
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @end
*********************************************************************/
 L7_RC_t usmDbStNonRecurringSet(L7_uint32 unit,L7_int32 parameter,void *ptr);

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
L7_RC_t usmDbUTCTimeGet(L7_clocktime * ct);

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
L7_int32 usmDbUTCTimeNow();


/**********************************************************************
* @purpose  know whether the summer-time is in effect or not.
*
* @returns  L7_TRUE
            L7_FALSE

* @end
*********************************************************************/

L7_BOOL usmDbSummerTimeIsInEffect();


/**********************************************************************
* @purpose  Adjusts current time for timezone and summer time
*
* @returns  Adjusted time
*           
*
* @end
*********************************************************************/
L7_uint32 usmDbAdjustedTimeGet();

/*************************************************************
*purpose  Get the summer-time mode
*
* @param    ct @b{(input)}mode
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
* @end
*********************************************************************/

L7_RC_t usmDbStModeGet(L7_uint32 unit,L7_int32 *mode);

/*********************************************************************
*
* @purpose  Get the summer-time mode including EU and USA
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
L7_RC_t usmDbStFlagGet(L7_uint32 unit,L7_int32  *mode);



/*************************************************************
*purpose  Set the summer-time mode
*
* @param    ct @b{(input)}mode
*
* @returns  L7_SUCCESS
            L7_FALIURE

* @end
* @end
*********************************************************************/

L7_RC_t usmDbStModeSet(L7_uint32 unit,L7_int32 mode);

#endif /* USMDB_TIMEZONE_API_H */

