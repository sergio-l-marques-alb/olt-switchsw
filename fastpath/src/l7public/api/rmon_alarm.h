/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_alarm.h
*
* @purpose  this is the header file for RMON Alarm group
*           
* @component SNMP-RMON
*
*
* @create  08/13/2001
*
* @author  K Pragash 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


 
#ifndef RMON_ALARM_H
#define RMON_ALARM_H

#include <l7_common.h>
#include <stdio.h>

#include <string.h>
#include <osapi.h>
#include <snmp_api.h>
#include "rmon_exports.h"

/* Header file for hardware specific system defines */

#include "l7_product.h"


/******************** GET FUNCTIONS ****************************/

/*********************************************************************
*
* @purpose    Checks index for the valid Alarm Entry
*
* @param      Index         Index of AlarmEntry to check
*             searchType    L7_uint32 (Get or GetNext or Set)
*             integerValue  L7_int32  (depends on get or set request)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonAlarmEntryGet(L7_uint32 searchType, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      Index         Index of AlarmEntry to check
*             
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonAlarmEntryNextGet(L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of Interval for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)    pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmIntervalGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Variable for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             buf           pointer to OID
*             buf_len       length of buf
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmVariableGet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of SampleType for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmSampleTypeGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of AlarmValue for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmValueGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of StartUpAlarm for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmStartUpAlarmGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of RisingThreshold for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmRisingThresholdGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of FallingThreshold for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmFallingThresholdGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of RisingEventIndex for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmRisingEventIndexGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of FallingEventIndex for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmFallingEventIndexGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Owner for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmOwnerGet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of Status for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmStatusGet(L7_uint32 Index, L7_uint32 *val);

/******************** SET FUNCTIONS ****************************/

/*********************************************************************
*
* @purpose    Sets value of Interval for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmIntervalSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Variable for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             buf           pointer to OID
*             buf_len       length of buf
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmVariableSet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of SampleType for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmSampleTypeSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of StartUpAlarm for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmStartUpAlarmSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Rising Threshold for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmRisingThresholdSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Falling Threshold for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmFallingThresholdSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Rising Event Index for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmRisingEventIndexSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Falling Event Index for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmFallingEventIndexSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Owner for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmOwnerSet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Status for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmStatusSet(L7_uint32 Index, L7_uint32 val);


#endif  /* RMON_ALARM_H */
