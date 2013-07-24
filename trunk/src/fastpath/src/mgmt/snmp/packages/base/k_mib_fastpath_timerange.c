/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename  k_mib_fastpath_timerange.c
*
* @purpose   Time Range - MIB implementation
*
* @create    19 Dec 2009
*
* @author    Siva Mannem
*
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "usmdb_common.h"
#include "timerange_exports.h"
#include "usmdb_timerange_api.h"
#include "k_mib_fastpath_timerange_api.h"

extern const L7_uint32 daysInMonth[];
fastPathTimeRangeGroup_t *
k_fastPathTimeRangeGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
   static fastPathTimeRangeGroup_t fastPathTimeRangeGroupData;
   if (nominator == I_timeRangeIndexNextFree)
   {
     if (usmdbTimeRangeNextAvailableIndexGet(USMDB_UNIT_CURRENT,
                                    &fastPathTimeRangeGroupData.timeRangeIndexNextFree)== L7_SUCCESS)
     {
       SET_VALID(I_timeRangeIndexNextFree, fastPathTimeRangeGroupData.valid);
       return(&fastPathTimeRangeGroupData);
     }
     else
     {
       return (NULL);
     }
   }
   else
   {
     return (NULL);
   }
}

timeRangeEntry_t *
k_timeRangeEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_UINT32 timeRangeIndex)
{

  static timeRangeEntry_t timeRangeEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    timeRangeEntryData.timeRangeName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(timeRangeEntryData.valid);
  timeRangeEntryData.timeRangeIndex = timeRangeIndex;

  
  SET_VALID(I_timeRangeIndex, timeRangeEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpTimeRangeEntryGet(USMDB_UNIT_CURRENT, &timeRangeEntryData, nominator) != L7_SUCCESS) :
       ((snmpTimeRangeEntryGet(USMDB_UNIT_CURRENT, &timeRangeEntryData, nominator) != L7_SUCCESS) &&
        (snmpTimeRangeEntryGetNext(USMDB_UNIT_CURRENT, &timeRangeEntryData, nominator) != L7_SUCCESS)) )
  {
    ZERO_VALID(timeRangeEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, timeRangeEntryData.valid))
    return(NULL);

  return(&timeRangeEntryData);
}

#ifdef SETS
int
k_timeRangeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    timeRangeEntry_t *data = (timeRangeEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_timeRangeName) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangeName, data->mod);
    }
    else if (object->nominator == I_timeRangeStatus) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangeStatus, data->mod);
    }
    return NO_ERROR;
}

int
k_timeRangeEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    timeRangeEntry_t *data = (timeRangeEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_timeRangeName, data->valid)) {
        /*
         * Examine data->timeRangeName in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangeStatus, data->valid)) {
        /*
         * Examine data->timeRangeStatus in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_timeRangeEntry_set_defaults(doList_t *dp)
{
    timeRangeEntry_t *data = (timeRangeEntry_t *) (dp->data);

    if ((data->timeRangeName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_timeRangeEntry_set(timeRangeEntry_t *data,
                     ContextInfo *contextInfo, int function)
{

  L7_BOOL isCreatedNew = L7_FALSE;
  L7_uint32 temp_timeRangeIndex1 = 0;
  char snmp_buffer[SNMP_BUFFER_LEN];

  /* This variable is added to pass to snmpTimeRangeEntryEntryGet() function 
   * so that our set values in 'data' are not lost.
   */
  timeRangeEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_timeRangeEntry(data);
  tempData->timeRangeIndex = data->timeRangeIndex;          /* QSCAN: DELETE -- Temp fixup */

  if (VALID(I_timeRangeName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->timeRangeName->octet_ptr, data->timeRangeName->length);
    if (usmDbTimeRangeNameStringCheck(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
  else if (VALID(I_timeRangeStatus, data->valid) &&
           data->timeRangeStatus != D_timeRangeStatus_destroy)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  if (snmpTimeRangeEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_timeRangeStatus, data->valid) ||
        (data->timeRangeStatus != D_timeRangeStatus_createAndGo &&
         data->timeRangeStatus != D_timeRangeStatus_createAndWait) ||
        usmDbTimeRangeCreate(USMDB_UNIT_CURRENT, data->timeRangeIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_timeRangeStatus, tempValid);

      if (!VALID(I_timeRangeName, data->valid))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return NO_CREATION_ERROR;
      }
    }
  
    if (VALID(I_timeRangeName, data->valid))
    {
      if (((usmDbTimeRangeNameToIndex(USMDB_UNIT_CURRENT, snmp_buffer, &temp_timeRangeIndex1) == L7_SUCCESS) &&
          (temp_timeRangeIndex1 != data->timeRangeIndex)) ||
          (usmDbTimeRangeNameAdd(USMDB_UNIT_CURRENT, data->timeRangeIndex,
                                 snmp_buffer) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

    }
  }
  else
  {
    if (!VALID(I_timeRangeStatus, data->valid) ||
       (data->timeRangeStatus != D_timeRangeStatus_destroy))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return NO_CREATION_ERROR;
    }
  }

  if (VALID(I_timeRangeStatus, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpTimeRangeStatusSet(USMDB_UNIT_CURRENT, data->timeRangeIndex,
                       data->timeRangeStatus) != L7_SUCCESS)
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_timeRangeEntry_UNDO
/* add #define SR_timeRangeEntry_UNDO in sitedefs.h to
 * include the undo routine for the timeRangeEntry family.
 */
int
timeRangeEntry_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
  timeRangeEntry_t *data = (timeRangeEntry_t *) doCur->data;
  timeRangeEntry_t *undodata = (timeRangeEntry_t *) doCur->undodata;
  timeRangeEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

    /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->timeRangeStatus != D_timeRangeStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->timeRangeStatus == D_timeRangeStatus_destroy)
      return NO_ERROR;

      /* undoin g an add, so delete */
      data->timeRangeStatus = D_timeRangeStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if ((undodata->timeRangeStatus == D_timeRangeStatus_notReady)
         || (undodata->timeRangeStatus == D_timeRangeStatus_notInService))
      {
          undodata->timeRangeStatus = D_timeRangeStatus_createAndWait;
      }
      else
      {
         if (undodata->timeRangeStatus == D_timeRangeStatus_active)
           undodata->timeRangeStatus = D_timeRangeStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_timeRangeEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;

}
#endif /* SR_timeRangeEntry_UNDO */

#endif /* SETS */

timeRangeAbsoluteEntry_t *
k_timeRangeAbsoluteEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 timeRangeIndex,
                             SR_INT32 timeRangeAbsoluteEntryIndex)
{

  static timeRangeAbsoluteEntry_t timeRangeAbsoluteEntryData;

  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    timeRangeAbsoluteEntryData.timeRangeAbsoluteStartDateAndTime = MakeOctetString(NULL, 0);
    timeRangeAbsoluteEntryData.timeRangeAbsoluteEndDateAndTime = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(timeRangeAbsoluteEntryData.valid);
  timeRangeAbsoluteEntryData.timeRangeIndex = timeRangeIndex;
  timeRangeAbsoluteEntryData.timeRangeAbsoluteEntryIndex = timeRangeAbsoluteEntryIndex;

  SET_VALID(I_timeRangeAbsoluteEntryIndex_timeRangeIndex, timeRangeAbsoluteEntryData.valid);
  SET_VALID(I_timeRangeAbsoluteEntryIndex, timeRangeAbsoluteEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpTimeRangeAbsoluteEntryGet(USMDB_UNIT_CURRENT,
                                  &timeRangeAbsoluteEntryData,
                                  nominator) != L7_SUCCESS) :
       ((snmpTimeRangeAbsoluteEntryGet(USMDB_UNIT_CURRENT,
                                   &timeRangeAbsoluteEntryData,
                                   nominator) != L7_SUCCESS) &&
        (snmpTimeRangeAbsoluteEntryGetNext(USMDB_UNIT_CURRENT,
                                       &timeRangeAbsoluteEntryData,
                                       nominator) != L7_SUCCESS)) )
    
  {
    ZERO_VALID(timeRangeAbsoluteEntryData.valid);
    return(NULL);
  }
      
  if (nominator >= 0 && !VALID(nominator, timeRangeAbsoluteEntryData.valid))
    return(NULL);

   return(&timeRangeAbsoluteEntryData);
}

#ifdef SETS
int
k_timeRangeAbsoluteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    timeRangeAbsoluteEntry_t *data = (timeRangeAbsoluteEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_timeRangeAbsoluteStartDateAndTime) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangeAbsoluteStartDateAndTime, data->mod);
    }
    else if (object->nominator == I_timeRangeAbsoluteEndDateAndTime) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangeAbsoluteEndDateAndTime, data->mod);
    }
    else if (object->nominator == I_timeRangeAbsoluteStatus) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangeAbsoluteStatus, data->mod);
    }
    return NO_ERROR;
}

int
k_timeRangeAbsoluteEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    timeRangeAbsoluteEntry_t *data = (timeRangeAbsoluteEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_timeRangeAbsoluteStartDateAndTime, data->valid)) {
        /*
         * Examine data->timeRangeAbsoluteStartDateAndTime in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangeAbsoluteEndDateAndTime, data->valid)) {
        /*
         * Examine data->timeRangeAbsoluteEndDateAndTime in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangeAbsoluteStatus, data->valid)) {
        /*
         * Examine data->timeRangeAbsoluteStatus in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_timeRangeAbsoluteEntry_set_defaults(doList_t *dp)
{
    timeRangeAbsoluteEntry_t *data = (timeRangeAbsoluteEntry_t *) (dp->data);

    if ((data->timeRangeAbsoluteStartDateAndTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->timeRangeAbsoluteEndDateAndTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_timeRangeAbsoluteEntry_set(timeRangeAbsoluteEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;

  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  L7_uint32 startHour, startMin, startDay, startMon;
  L7_uint32 endHour, endMin, endDay, endMon;
  L7_uint32 startYear, endYear;
   /* This variable is added to pass to snmpQosAclMacRuleEntryGet() function 
    * so that our set values in 'data' are not lost.
    */
   timeRangeAbsoluteEntry_t *tempData;
   
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_timeRangeAbsoluteEntry(data);
  tempData->timeRangeIndex = data->timeRangeIndex;
  tempData->timeRangeAbsoluteEntryIndex = data->timeRangeAbsoluteEntryIndex;

  if (snmpTimeRangeAbsoluteEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_timeRangeAbsoluteStatus, data->valid) ||
        (data->timeRangeAbsoluteStatus != D_timeRangeAbsoluteStatus_createAndGo) ||
        snmpTimeRangeEntryAdd(USMDB_UNIT_CURRENT, 
              data->timeRangeIndex, 
              data->timeRangeAbsoluteEntryIndex,
              TIMERANGE_ABSOLUTE_ENTRY) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_timeRangeAbsoluteStatus, tempValid);
      if ((!VALID(I_timeRangeAbsoluteStartDateAndTime, data->valid)) &&
          (!VALID(I_timeRangeAbsoluteEndDateAndTime, data->valid)))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return NO_CREATION_ERROR;
      }
    }
  
    if (VALID(I_timeRangeAbsoluteStartDateAndTime, data->valid))
    {
      if (data->timeRangeAbsoluteStartDateAndTime->length == 6)
      {
        startYear = data->timeRangeAbsoluteStartDateAndTime->octet_ptr[0];
        startYear = (startYear << 8) | \
                    (data->timeRangeAbsoluteStartDateAndTime->octet_ptr[1]);
        startMon  = data->timeRangeAbsoluteStartDateAndTime->octet_ptr[2];
        startDay  = data->timeRangeAbsoluteStartDateAndTime->octet_ptr[3];
        startHour = data->timeRangeAbsoluteStartDateAndTime->octet_ptr[4];
        startMin  = data->timeRangeAbsoluteStartDateAndTime->octet_ptr[5];
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      if( startHour < L7_TIMEZONE_UHOURS_MIN || startHour > L7_TIMEZONE_UHOURS_MAX ||
          startMin < L7_TIMEZONE_MINS_MIN || startMin > L7_TIMEZONE_MINS_MAX)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      if (startMon < 1 || startMon > L7_MONTHS_IN_YEAR)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      if ((startDay > daysInMonth [startMon]) && 
            (!(startDay == L7_LEAP_YEAR_DAY && startMon == L7_MONTH_FEB &&
               L7_LEAP_YEAR_CHECK(startYear))))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      if ((startYear < L7_TIMERANGE_MIN_YEAR) ||
          (startYear > L7_TIMERANGE_MAX_YEAR))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      
      memset(&startDateAndTime, 0, sizeof(startDateAndTime));
      startDateAndTime.date.absoluteDate.year = startYear;
      startDateAndTime.date.absoluteDate.day = startDay;
      startDateAndTime.date.absoluteDate.month = startMon;
      startDateAndTime.hour = startHour;
      startDateAndTime.minute = startMin;
      
      if (usmDbTimeRangeEntryStartDateAndTimeAdd(USMDB_UNIT_CURRENT,
                                     data->timeRangeIndex,
                                     data->timeRangeAbsoluteEntryIndex,
                                     TIMERANGE_ABSOLUTE_ENTRY,
                                     &startDateAndTime) != L7_SUCCESS)
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_timeRangeAbsoluteStartDateAndTime, tempValid);
      }
    }

    if (VALID(I_timeRangeAbsoluteEndDateAndTime, data->valid))
    {
      if (data->timeRangeAbsoluteEndDateAndTime->length == 6)
      {
        endYear = data->timeRangeAbsoluteEndDateAndTime->octet_ptr[0];
        endYear = (endYear << 8) | \
                    (data->timeRangeAbsoluteEndDateAndTime->octet_ptr[1]);
        endMon  = data->timeRangeAbsoluteEndDateAndTime->octet_ptr[2];
        endDay  = data->timeRangeAbsoluteEndDateAndTime->octet_ptr[3];
        endHour = data->timeRangeAbsoluteEndDateAndTime->octet_ptr[4];
        endMin  = data->timeRangeAbsoluteEndDateAndTime->octet_ptr[5];
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      if( endHour < L7_TIMEZONE_UHOURS_MIN || endHour > L7_TIMEZONE_UHOURS_MAX ||
          endMin < L7_TIMEZONE_MINS_MIN || endMin > L7_TIMEZONE_MINS_MAX)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      if (endMon < 1 || endMon > L7_MONTHS_IN_YEAR)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      if ((endDay > daysInMonth [endMon]) && 
            (!(endDay == L7_LEAP_YEAR_DAY && endMon == L7_MONTH_FEB &&
               L7_LEAP_YEAR_CHECK(endYear))))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      if ((endYear < L7_TIMERANGE_MIN_YEAR) ||
          (endYear > L7_TIMERANGE_MAX_YEAR))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      memset(&endDateAndTime, 0, sizeof(endDateAndTime));
      endDateAndTime.date.absoluteDate.year = endYear;
      endDateAndTime.date.absoluteDate.day = endDay;
      endDateAndTime.date.absoluteDate.month = endMon;
      endDateAndTime.hour = endHour;
      endDateAndTime.minute = endMin;

      if (usmDbTimeRangeEntryEndDateAndTimeAdd(USMDB_UNIT_CURRENT,
                                     data->timeRangeIndex,
                                     data->timeRangeAbsoluteEntryIndex,
                                     TIMERANGE_ABSOLUTE_ENTRY,
                                     &endDateAndTime) != L7_SUCCESS)
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_timeRangeAbsoluteEndDateAndTime, tempValid);
      }
    }
  } 
  else
  {
    if (!VALID(I_timeRangeAbsoluteStatus, data->valid) ||
       (data->timeRangeAbsoluteStatus != D_timeRangeAbsoluteStatus_destroy))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }


  if (VALID(I_timeRangeAbsoluteStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      snmpTimeRangeEntryStatusSet(USMDB_UNIT_CURRENT,
                                 data->timeRangeIndex,
                                 data->timeRangeAbsoluteEntryIndex,
                                 data->timeRangeAbsoluteStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_timeRangeAbsoluteEntry_UNDO
/* add #define SR_timeRangeAbsoluteEntry_UNDO in sitedefs.h to
 * include the undo routine for the timeRangeAbsoluteEntry family.
 */
int
timeRangeAbsoluteEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  timeRangeAbsoluteEntry_t *data = (timeRangeAbsoluteEntry_t *) doCur->data;
  timeRangeAbsoluteEntry_t *undodata = (timeRangeAbsoluteEntry_t *) doCur->undodata;
  timeRangeAbsoluteEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL  && (data->timeRangeAbsoluteStatus != D_timeRangeAbsoluteStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->timeRangeAbsoluteStatus == D_timeRangeAbsoluteStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->timeRangeAbsoluteStatus = D_timeRangeAbsoluteStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if ((undodata->timeRangeAbsoluteStatus == D_timeRangeAbsoluteStatus_notReady)
         || (undodata->timeRangeAbsoluteStatus == D_timeRangeAbsoluteStatus_notInService))
      {
          undodata->timeRangeAbsoluteStatus = D_timeRangeAbsoluteStatus_createAndWait;
      }
      else
      {
         if (undodata->timeRangeAbsoluteStatus == D_timeRangeAbsoluteStatus_active)
            undodata->timeRangeAbsoluteStatus = D_timeRangeAbsoluteStatus_createAndGo;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) &&(k_timeRangeAbsoluteEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_timeRangeAbsoluteEntry_UNDO */

#endif /* SETS */

timeRangePeriodicEntry_t *
k_timeRangePeriodicEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 timeRangeIndex,
                             SR_INT32 timeRangePeriodicEntryIndex)
{

  static timeRangePeriodicEntry_t timeRangePeriodicEntryData;

  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    timeRangePeriodicEntryData.timeRangePeriodicStartDay = MakeOctetString(NULL, 0);
    timeRangePeriodicEntryData.timeRangePeriodicEndDay = MakeOctetString(NULL, 0);
    timeRangePeriodicEntryData.timeRangePeriodicEndTime = MakeOctetString(NULL, 0);
    timeRangePeriodicEntryData.timeRangePeriodicStartTime = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(timeRangePeriodicEntryData.valid);
  timeRangePeriodicEntryData.timeRangeIndex = timeRangeIndex;
  timeRangePeriodicEntryData.timeRangePeriodicEntryIndex = timeRangePeriodicEntryIndex;

  SET_VALID(I_timeRangePeriodicEntryIndex_timeRangeIndex, timeRangePeriodicEntryData.valid);
  SET_VALID(I_timeRangePeriodicEntryIndex, timeRangePeriodicEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpTimeRangePeriodicEntryGet(USMDB_UNIT_CURRENT,
                                  &timeRangePeriodicEntryData,
                                  nominator) != L7_SUCCESS) :
       ((snmpTimeRangePeriodicEntryGet(USMDB_UNIT_CURRENT,
                                   &timeRangePeriodicEntryData,
                                   nominator) != L7_SUCCESS) &&
        (snmpTimeRangePeriodicEntryGetNext(USMDB_UNIT_CURRENT,
                                       &timeRangePeriodicEntryData,
                                       nominator) != L7_SUCCESS)) )
    
  {
    ZERO_VALID(timeRangePeriodicEntryData.valid);
    return(NULL);
  }
      
  if (nominator >= 0 && !VALID(nominator, timeRangePeriodicEntryData.valid))
    return(NULL);

   return(&timeRangePeriodicEntryData);
}

#ifdef SETS
int
k_timeRangePeriodicEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    timeRangePeriodicEntry_t *data = (timeRangePeriodicEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_timeRangePeriodicStartDay) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangePeriodicStartDay, data->mod);
    }
    else if (object->nominator == I_timeRangePeriodicStartTime) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangePeriodicStartTime, data->mod);
    }
    else if (object->nominator == I_timeRangePeriodicEndDay) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangePeriodicEndDay, data->mod);
    }
    else if (object->nominator == I_timeRangePeriodicEndTime) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangePeriodicEndTime, data->mod);
    }
    else if (object->nominator == I_timeRangePeriodicStatus) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_timeRangePeriodicStatus, data->mod);
    }
    return NO_ERROR;
}

int
k_timeRangePeriodicEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    timeRangePeriodicEntry_t *data = (timeRangePeriodicEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_timeRangePeriodicStartDay, data->valid)) {
        /*
         * Examine data->timeRangePeriodicStartDay in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangePeriodicStartTime, data->valid)) {
        /*
         * Examine data->timeRangePeriodicStartTime in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangePeriodicEndDay, data->valid)) {
        /*
         * Examine data->timeRangePeriodicEndDay in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangePeriodicEndTime, data->valid)) {
        /*
         * Examine data->timeRangePeriodicEndTime in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    if (VALID(I_timeRangePeriodicStatus, data->valid)) {
        /*
         * Examine data->timeRangePeriodicStatus in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_timeRangePeriodicEntry_set_defaults(doList_t *dp)
{
    timeRangePeriodicEntry_t *data = (timeRangePeriodicEntry_t *) (dp->data);

    if ((data->timeRangePeriodicStartDay = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->timeRangePeriodicStartTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->timeRangePeriodicEndDay = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->timeRangePeriodicEndTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_timeRangePeriodicEntry_set(timeRangePeriodicEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;

  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  L7_uchar8 i;
  L7_uint32 startHour, startMin;
  L7_uint32 endHour, endMin;

   /* This variable is added to pass to snmpQosAclMacRuleEntryGet() function 
    * so that our set values in 'data' are not lost.
    */
   timeRangePeriodicEntry_t *tempData;
   
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_timeRangePeriodicEntry(data);
  tempData->timeRangeIndex = data->timeRangeIndex;          /* QSCAN: DELETE -- Temp fixup */
  tempData->timeRangePeriodicEntryIndex = data->timeRangePeriodicEntryIndex;  /* QSCAN: DELETE -- Temp fixup */

  if (snmpTimeRangePeriodicEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_timeRangePeriodicStatus, data->valid) ||
        (data->timeRangePeriodicStatus != D_timeRangePeriodicStatus_createAndGo) ||
        snmpTimeRangeEntryAdd(USMDB_UNIT_CURRENT, 
              data->timeRangeIndex, 
              data->timeRangePeriodicEntryIndex,
              TIMERANGE_PERIODIC_ENTRY) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_timeRangePeriodicStatus, tempValid);

      if (!VALID(I_timeRangePeriodicStartTime, data->valid) ||
          !VALID(I_timeRangePeriodicStartDay, data->valid) ||
          !VALID(I_timeRangePeriodicEndTime, data->valid) ||
          !VALID(I_timeRangePeriodicEndDay, data->valid))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
    }
  
    if (VALID(I_timeRangePeriodicStartTime, data->valid) &&
      VALID(I_timeRangePeriodicStartDay, data->valid))
    {
      if (data->timeRangePeriodicStartTime->length == 2)
      {
        startHour =  data->timeRangePeriodicStartTime->octet_ptr[0];
        startMin  =  data->timeRangePeriodicStartTime->octet_ptr[1];
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      if( startHour < L7_TIMEZONE_UHOURS_MIN || startHour > L7_TIMEZONE_UHOURS_MAX ||
          startMin < L7_TIMEZONE_MINS_MIN || startMin > L7_TIMEZONE_MINS_MAX)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      memset(&startDateAndTime, 0, sizeof(startDateAndTime));
      startDateAndTime.hour = startHour;
      startDateAndTime.minute = startMin;  
      startDateAndTime.date.daysOfTheWeek.dayMask = 0;
      for (i = 1; i <= 7; i++)
      {  
        if (data->timeRangePeriodicStartDay->octet_ptr[0] & (0x80 >> i))
        {
          startDateAndTime.date.daysOfTheWeek.dayMask |= (1 << i);
        }
      }

      if (usmDbTimeRangeEntryStartDateAndTimeAdd(USMDB_UNIT_CURRENT,
                                     data->timeRangeIndex,
                                     data->timeRangePeriodicEntryIndex,
                                     TIMERANGE_PERIODIC_ENTRY,
                                     &startDateAndTime) != L7_SUCCESS)
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_timeRangePeriodicStartTime, tempValid);
        SET_VALID(I_timeRangePeriodicStartDay, tempValid);
      }
    }

    if (VALID(I_timeRangePeriodicEndTime, data->valid) &&
      VALID(I_timeRangePeriodicEndDay, data->valid))
    {
      if (data->timeRangePeriodicEndTime->length == 2)
      {
        endHour =  data->timeRangePeriodicEndTime->octet_ptr[0];
        endMin  =  data->timeRangePeriodicEndTime->octet_ptr[1];
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      if( endHour < L7_TIMEZONE_UHOURS_MIN || endHour > L7_TIMEZONE_UHOURS_MAX ||
          endMin < L7_TIMEZONE_MINS_MIN || endMin > L7_TIMEZONE_MINS_MAX)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }

      memset(&endDateAndTime, 0, sizeof(endDateAndTime));
      endDateAndTime.hour = endHour;
      endDateAndTime.minute = endMin;  
      endDateAndTime.date.daysOfTheWeek.dayMask = 0;
      for (i = 1; i <= 7; i++)
      {  
        if (data->timeRangePeriodicEndDay->octet_ptr[0] & (0x80 >> i))
        {
          endDateAndTime.date.daysOfTheWeek.dayMask |= (1 << i);
        }
      }
      if (usmDbTimeRangeEntryEndDateAndTimeAdd(USMDB_UNIT_CURRENT,
                                     data->timeRangeIndex,
                                     data->timeRangePeriodicEntryIndex,
                                     TIMERANGE_PERIODIC_ENTRY,
                                     &endDateAndTime) != L7_SUCCESS)
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_timeRangePeriodicEndTime, tempValid);
        SET_VALID(I_timeRangePeriodicEndDay, tempValid);
      }
    }
  }
  else
  {
    if (!VALID(I_timeRangePeriodicStatus, data->valid) ||
       (data->timeRangePeriodicStatus != D_timeRangePeriodicStatus_destroy))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_timeRangePeriodicStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      snmpTimeRangeEntryStatusSet(USMDB_UNIT_CURRENT,
                                 data->timeRangeIndex,
                                 data->timeRangePeriodicEntryIndex,
                                 data->timeRangePeriodicStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_timeRangePeriodicEntry_UNDO
/* add #define SR_timeRangePeriodicEntry_UNDO in sitedefs.h to
 * include the undo routine for the timeRangePeriodicEntry family.
 */
int
timeRangePeriodicEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  timeRangePeriodicEntry_t *data = (timeRangePeriodicEntry_t *) doCur->data;
  timeRangePeriodicEntry_t *undodata = (timeRangePeriodicEntry_t *) doCur->undodata;
  timeRangePeriodicEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL  && (data->timeRangePeriodicStatus != D_timeRangePeriodicStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->timeRangePeriodicStatus == D_timeRangePeriodicStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->timeRangePeriodicStatus = D_timeRangePeriodicStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if ((undodata->timeRangePeriodicStatus == D_timeRangePeriodicStatus_notReady)
         || (undodata->timeRangePeriodicStatus == D_timeRangePeriodicStatus_notInService))
      {
          undodata->timeRangePeriodicStatus = D_timeRangePeriodicStatus_createAndWait;
      }
      else
      {
         if (undodata->timeRangePeriodicStatus == D_timeRangePeriodicStatus_active)
            undodata->timeRangePeriodicStatus = D_timeRangePeriodicStatus_createAndGo;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) &&(k_timeRangePeriodicEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_timeRangePeriodicEntry_UNDO */

#endif /* SETS */


